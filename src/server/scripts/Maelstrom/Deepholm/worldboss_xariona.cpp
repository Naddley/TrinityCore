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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"

namespace Scripts::Maelstrom::Deepholm::WorldbossXariona
{
namespace Creatures
{
    static constexpr uint32 TwilightFissure = 50431;
}
namespace Spells
{
    static constexpr uint32 ZeroPower = 87239;
    static constexpr uint32 RogueClassCritDodgeDebuff = 73059;
    static constexpr uint32 FuryOfTheTwilightFlight = 93554;
    static constexpr uint32 TwilightBreath = 93544;
    static constexpr uint32 TwilightBuffetTargeting = 95385;
    static constexpr uint32 UnleashedMagic = 93556;
    static constexpr uint32 TwilightFissure = 93546;
    static constexpr uint32 VoidBlast = 93545;
    static constexpr uint32 TwilightZone = 93553;
}
namespace Events
{
    static constexpr uint32 EventFuryOfTheTwilightFlight = 1;
    static constexpr uint32 EventTwilightBreath = 2;
    static constexpr uint32 EventTwilightBuffet = 3;
    static constexpr uint32 EventUnleashedMagic = 4;
    static constexpr uint32 EventTwilightFissure = 5;
    static constexpr uint32 EventTwilightZone = 6;
}

// 50061 - Xariona
struct npc_deepholm_worldboss_xariona : public ScriptedAI
{
    npc_deepholm_worldboss_xariona(Creature* creature) : ScriptedAI(creature), _furyOfTheTwilightFlightCount(0) { }

    void JustAppeared() override
    {
        DoCastSelf(Spells::ZeroPower);
        DoCastSelf(Spells::RogueClassCritDodgeDebuff);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetHomePosition(me->GetPosition());
        
        _events.ScheduleEvent(Events::EventFuryOfTheTwilightFlight, 1s);
        _events.ScheduleEvent(Events::EventTwilightBreath, 14s);
        _events.ScheduleEvent(Events::EventTwilightBuffet, 13s);
        _events.ScheduleEvent(Events::EventTwilightFissure, 29s);
        _events.ScheduleEvent(Events::EventTwilightZone, 31s);
    }

    void JustSummoned(Creature* summon) override
    {
        switch (summon->GetEntry())
        {
            case Creatures::TwilightFissure:
                summon->m_Events.AddEventAtOffset([summon]()
                {
                    summon->CastSpell(summon, Spells::VoidBlast);
                    summon->DespawnOrUnsummon(4s);
                }, 4s); // Tooltip states 5 seconds but sniffs say 4
                break;
            default:
                break;
        }
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        me->DespawnOrUnsummon(0s, 30s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case Events::EventFuryOfTheTwilightFlight:
                    DoCastSelf(Spells::FuryOfTheTwilightFlight);
                    ++_furyOfTheTwilightFlightCount;
                    if (_furyOfTheTwilightFlightCount < 2)
                        _events.Repeat(30s + 500ms);
                    else
                        _events.ScheduleEvent(Events::EventUnleashedMagic, 32s);
                    break;
                case Events::EventTwilightBreath:
                    DoCastVictim(Spells::TwilightBreath);
                    _events.Repeat(18s, 20s);
                    break;
                case Events::EventTwilightBuffet:
                    DoCastAOE(Spells::TwilightBuffetTargeting, CastSpellExtraArgs().AddSpellMod(SPELLVALUE_MAX_TARGETS, 1));
                    _events.Repeat(23s, 25s);
                    break;
                case Events::EventUnleashedMagic:
                    DoCastAOE(Spells::UnleashedMagic);
                    _furyOfTheTwilightFlightCount = 0;
                    _events.DelayEvents(5s);
                    _events.ScheduleEvent(Events::EventFuryOfTheTwilightFlight, 1ms);
                    break;
                case Events::EventTwilightFissure:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 45.f, true))
                        DoCast(target, Spells::TwilightFissure);
                    _events.Repeat(29s);
                    break;
                case Events::EventTwilightZone:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 55.f, true))
                        DoCast(target, Spells::TwilightZone);
                    _events.Repeat(31s);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }
    }

private:
    EventMap _events;
    uint8 _furyOfTheTwilightFlightCount;
};

// 95385 - Twilight Buffet Targeting
class spell_deepholm_twilight_buffet_targeting : public SpellScript
{
    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        // Xariona never hits her current victim with Twilight Buffed
        targets.remove_if([&](WorldObject* target)
        {
            return GetCaster()->GetVictim() == target;
        });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetHitUnit(), GetSpellInfo()->GetEffect(EFFECT_0).CalcValueAsInt(caster), TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_deepholm_twilight_buffet_targeting::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_deepholm_twilight_buffet_targeting::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};
}

void AddSC_worldboss_xariona()
{
    using namespace Scripts::Maelstrom::Deepholm::WorldbossXariona;

    // Creature
    RegisterCreatureAI(npc_deepholm_worldboss_xariona);

    // Spells
    RegisterSpellScript(spell_deepholm_twilight_buffet_targeting);
}
