/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "KillRewarder.h"
#include "Creature.h"
#include "DB2Stores.h"
#include "FlatSet.h"
#include "Formulas.h"
#include "Group.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Pet.h"
#include "Player.h"
#include "Scenario.h"
#include "SpellAuraEffects.h"
#include <boost/container/small_vector.hpp>

 // == KillRewarder ====================================================
 // KillRewarder encapsulates logic of rewarding player upon kill with:
 // * XP;
 // * honor;
 // * reputation;
 // * kill credit (for quest objectives).
 // Rewarding is initiated in two cases: when player kills unit in Unit::Kill()
 // and on battlegrounds in Battleground::RewardXPAtKill().
 //
 // Rewarding algorithm is:
 // 1. Initialize internal variables to default values.
 // 2. In case when player is in group, initialize variables necessary for group calculations:
 // 2.1. _count - number of alive group members within reward distance;
 // 2.2. _sumLevel - sum of levels of alive group members within reward distance;
 // 2.3. _maxLevel - maximum level of alive group member within reward distance;
 // 2.4. _maxNotGrayMember - maximum level of alive group member within reward distance,
 //      for whom victim is not gray;
 // 2.5. _isFullXP - flag identifying that for all group members victim is not gray,
 //      so 100% XP will be rewarded (50% otherwise).
 // 3. Reward killer (and group, if necessary).
 // 3.1. If killer is in group, reward group.
 // 3.1.1. Initialize initial XP amount based on maximum level of group member,
 //        for whom victim is not gray.
 // 3.1.2. Alter group rate if group is in raid (not for battlegrounds).
 // 3.1.3. Reward each group member (even dead) within reward distance (see 4. for more details).
 // 3.2. Reward single killer (not group case).
 // 3.2.1. Initialize initial XP amount based on killer's level.
 // 3.2.2. Reward killer (see 4. for more details).
 // 4. Reward player.
 // 4.1. Give honor (player must be alive and not on BG).
 // 4.2. Give XP.
 // 4.2.1. If player is in group, adjust XP:
 //        * set to 0 if player's level is more than maximum level of not gray member;
 //        * cut XP in half if _isFullXP is false.
 // 4.2.2. Apply auras modifying rewarded XP.
 // 4.2.3. Give XP to player.
 // 4.2.4. If player has pet, reward pet with XP (100% for single player, 50% for group case).
 // 4.3. Give reputation (player must not be on BG).
 // 4.4. Give kill credit (player must not be in group, or he must be alive or without corpse).
 // 5. Credit instance encounter.
 // 6. Update guild achievements.
 // 7. Scenario credit

KillRewarder::KillRewarder(Trinity::IteratorPair<Player**> killers, Unit* victim, bool isBattleGround) :
    // 1. Initialize internal variables to default values.
    _killers(killers), _victim(victim),
    _groupRate(1.0f), _maxNotGrayMember(nullptr), _count(0), _sumLevel(0), _xp(0),
    _isFullXP(false), _maxLevel(0), _isBattleGround(isBattleGround), _isPvP(false)
{
    // mark the credit as pvp if victim is player
    if (victim->GetTypeId() == TYPEID_PLAYER)
        _isPvP = true;
    // or if its owned by player and its not a vehicle
    else if (victim->GetCharmerOrOwnerGUID().IsPlayer())
        _isPvP = !victim->IsVehicle();
}

inline void KillRewarder::_InitGroupData(Player const* killer)
{
    if (Group const* group = killer->GetGroup())
    {
        // 2. In case when player is in group, initialize variables necessary for group calculations:
        for (GroupReference const& itr : group->GetMembers())
        {
            Player* member = itr.GetSource();
            if (killer == member || (member->IsAtGroupRewardDistance(_victim) && member->IsAlive()))
            {
                const uint8 lvl = member->GetLevel();
                // 2.1. _count - number of alive group members within reward distance;
                ++_count;
                // 2.2. _sumLevel - sum of levels of alive group members within reward distance;
                _sumLevel += lvl;
                // 2.3. _maxLevel - maximum level of alive group member within reward distance;
                if (_maxLevel < lvl)
                    _maxLevel = lvl;
                // 2.4. _maxNotGrayMember - maximum level of alive group member within reward distance,
                //      for whom victim is not gray;
                uint32 grayLevel = Trinity::XP::GetGrayLevel(lvl);
                if (_victim->GetLevelForTarget(member) > grayLevel && (!_maxNotGrayMember || _maxNotGrayMember->GetLevel() < lvl))
                    _maxNotGrayMember = member;
            }
        }
        // 2.5. _isFullXP - flag identifying that for all group members victim is not gray,
        //      so 100% XP will be rewarded (50% otherwise).
        _isFullXP = _maxNotGrayMember && (_maxLevel == _maxNotGrayMember->GetLevel());
    }
    else
        _count = 1;
}

inline void KillRewarder::_InitXP(Player* player, Player const* killer)
{
    // Get initial value of XP for kill.
    // XP is given:
    // * on battlegrounds;
    // * otherwise, not in PvP;
    // * not if killer is on vehicle.
    if (_isBattleGround || (!_isPvP && !killer->GetVehicle()))
        _xp = Trinity::XP::Gain(player, _victim, _isBattleGround);
}

inline void KillRewarder::_RewardHonor(Player* player)
{
    // Rewarded player must be alive.
    if (player->IsAlive())
        player->RewardHonor(_victim, _count, -1, true);
}

inline void KillRewarder::_RewardXP(Player* player, float rate)
{
    uint32 xp(_xp);
    if (player->GetGroup())
    {
        // 4.2.1. If player is in group, adjust XP:
        //        * set to 0 if player's level is more than maximum level of not gray member;
        //        * cut XP in half if _isFullXP is false.
        if (_maxNotGrayMember && player->IsAlive() &&
            _maxNotGrayMember->GetLevel() >= player->GetLevel())
            xp = _isFullXP ?
            uint32(xp * rate) :             // Reward FULL XP if all group members are not gray.
            uint32(xp * rate / 2) + 1;      // Reward only HALF of XP if some of group members are gray.
        else
            xp = 0;
    }
    if (xp)
    {
        // 4.2.2. Apply auras modifying rewarded XP (SPELL_AURA_MOD_XP_PCT and SPELL_AURA_MOD_XP_FROM_CREATURE_TYPE).
        xp *= player->GetTotalAuraMultiplier(SPELL_AURA_MOD_XP_PCT);
        xp *= player->GetTotalAuraMultiplierByMiscValue(SPELL_AURA_MOD_XP_FROM_CREATURE_TYPE, int32(_victim->GetCreatureType()));

        // 4.2.3. Give XP to player.
        player->GiveXP(xp, _victim, _groupRate);
        if (Pet* pet = player->GetPet())
            // 4.2.4. If player has pet, reward pet with XP (100% for single player, 50% for group case).
            pet->GivePetXP(player->GetGroup() ? xp / 2 : xp);
    }
}

inline void KillRewarder::_RewardReputation(Player* player, float rate)
{
    // 4.3. Give reputation (player must not be on BG).
    // Even dead players and corpses are rewarded.
    player->RewardReputation(_victim, rate);
}

inline void KillRewarder::_RewardKillCredit(Player* player)
{
    // 4.4. Give kill credit (player must not be in group, or he must be alive or without corpse).
    if (!player->GetGroup() || player->IsAlive() || !player->GetCorpse())
    {
        if (Creature* target = _victim->ToCreature())
        {
            player->KilledMonster(target);
            player->UpdateCriteria(CriteriaType::KillAnyCreature, target->GetCreatureType(), 1, 0, target);
        }
    }
}

void KillRewarder::_RewardPlayer(Player* player, bool isDungeon)
{
    // 4. Reward player.
    if (!_isBattleGround)
    {
        // 4.1. Give honor (player must be alive and not on BG).
        _RewardHonor(player);
        // 4.1.1 Send player killcredit for quests with PlayerSlain
        if (_victim->GetTypeId() == TYPEID_PLAYER)
            player->KilledPlayerCredit(_victim->GetGUID());
    }
    // Give XP only in PvE or in battlegrounds.
    // Give reputation and kill credit only in PvE.
    if (!_isPvP || _isBattleGround)
    {
        float const rate = player->GetGroup() ?
            _groupRate * float(player->GetLevel()) / _sumLevel : // Group rate depends on summary level.
            1.0f;                                                // Personal rate is 100%.
        if (_xp)
            // 4.2. Give XP.
            _RewardXP(player, rate);
        if (!_isBattleGround)
        {
            // If killer is in dungeon then all members receive full reputation at kill.
            _RewardReputation(player, isDungeon ? 1.0f : rate);
            _RewardKillCredit(player);
        }
    }
}

void KillRewarder::_RewardGroup(Group const* group, Player const* killer)
{
    if (_maxLevel)
    {
        if (_maxNotGrayMember)
            // 3.1.1. Initialize initial XP amount based on maximum level of group member,
            //        for whom victim is not gray.
            _InitXP(_maxNotGrayMember, killer);
        // To avoid unnecessary calculations and calls,
        // proceed only if XP is not ZERO or player is not on battleground
        // (battleground rewards only XP, that's why).
        if (!_isBattleGround || _xp)
        {
            bool const isDungeon = !_isPvP && sMapStore.LookupEntry(killer->GetMapId())->IsDungeon();
            if (!_isBattleGround)
            {
                // 3.1.2. Alter group rate if group is in raid (not for battlegrounds).
                bool const isRaid = !_isPvP && sMapStore.LookupEntry(killer->GetMapId())->IsRaid() && group->isRaidGroup();
                _groupRate = Trinity::XP::xp_in_group_rate(_count, isRaid);
            }

            // 3.1.3. Reward each group member (even dead or corpse) within reward distance.
            for (GroupReference const& itr : group->GetMembers())
            {
                Player* member = itr.GetSource();
                // Killer may not be at reward distance, check directly
                if (killer == member || member->IsAtGroupRewardDistance(_victim))
                    _RewardPlayer(member, isDungeon);
            }
        }
    }
}

void KillRewarder::Reward()
{
    Trinity::Containers::FlatSet<Group const*, std::less<>, boost::container::small_vector<Group const*, 3>> processedGroups;
    for (Player* killer : _killers)
    {
        _InitGroupData(killer);

        // 3. Reward killer (and group, if necessary).
        if (Group* group = killer->GetGroup())
        {
            if (!processedGroups.insert(group).second)
                continue;

            // 3.1. If killer is in group, reward group.
            _RewardGroup(group, killer);
        }
        else
        {
            // 3.2. Reward single killer (not group case).
            // 3.2.1. Initialize initial XP amount based on killer's level.
            _InitXP(killer, killer);
            // To avoid unnecessary calculations and calls,
            // proceed only if XP is not ZERO or player is not on battleground
            // (battleground rewards only XP, that's why).
            if (!_isBattleGround || _xp)
                // 3.2.2. Reward killer.
                _RewardPlayer(killer, false);
        }
    }

    // 5. Credit instance encounter.
    // 6. Update guild achievements.
    // 7. Credit scenario criterias
    if (Creature* victim = _victim->ToCreature())
    {
        if (_killers.begin() != _killers.end())
        {
            if (ObjectGuid::LowType guildId = victim->GetMap()->GetOwnerGuildId())
                if (Guild* guild = sGuildMgr->GetGuildById(guildId))
                    guild->UpdateCriteria(CriteriaType::KillCreature, victim->GetEntry(), 1, 0, victim, *_killers.begin());

            if (Scenario* scenario = victim->GetScenario())
                scenario->UpdateCriteria(CriteriaType::KillCreature, victim->GetEntry(), 1, 0, victim, *_killers.begin());
        }
    }
}
