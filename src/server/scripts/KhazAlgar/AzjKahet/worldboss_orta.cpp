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

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "Containers.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum WorldbossOrta
{
    // Spells
    SPELL_ORTA_POWER_ENERGIZE_RAGE              = 450168,
    SPELL_ORTA_TECTONIC_ROAR                    = 450454,
    SPELL_ORTA_COLOSSAL_SLAM_SELECTOR           = 450406,
    SPELL_ORTA_COLOSSAL_SLAM_DAMAGE             = 450407,
    SPELL_ORTA_RUPTURING_RUNES_CHANNEL          = 450677,
    SPELL_ORTA_RUPTURING_RUNES_MISSILE          = 450861,
    SPELL_ORTA_RUPTURING_RUNES_DAMAGE           = 450863,
    // SPELL_ORTA_MOUNTAINS_GRASP_SELECTOR      = 451696,
    // SPELL_ORTA_MOUNTAINS_GRASP_GRAP          = 450924,
    SPELL_ORTA_MOUNTAINS_GRASP_ENTER_VEHICLE    = 136531,
    SPELL_ORTA_MOUNTAINS_GRASP_CAST             = 450929,
    SPELL_ORTA_DISCARD_WEAKLING_CAST            = 451702,
    SPELL_ORTA_DISCARD_WEAKLING_SELECTOR        = 451130,
    SPELL_ORTA_DISCARD_WEAKLING_KNOCKBACK       = 451131,


    // Text
    SAY_ORTA_CRUSH_YOU                          = 0,

    // Events
    EVENT_ORTA_COLOSSAL_SLAM                    = 1,
    EVENT_ORTA_TECTONIC_ROAR,
    EVENT_ORTA_RUPTURING_RUNES,
    EVENT_ORTA_MOUNTAINS_GRASP,
    EVENT_DISCARD_WEAKLING
};

constexpr Position OrtaPlatformCenter = { -1974.530f, -99.878f, -1198.8499f, 5.167920f };

// 221067 - Orta
struct worldboss_orta : public WorldBossAI
{
    worldboss_orta(Creature* creature) : WorldBossAI(creature) { }

    void JustAppeared() override
    {
        me->SetPower(me->GetPowerType(), 30);
    }

    void JustEngagedWith(Unit* who) override
    {
        WorldBossAI::JustEngagedWith(who);
       
        DoCast(SPELL_ORTA_POWER_ENERGIZE_RAGE);
        events.ScheduleEvent(EVENT_ORTA_COLOSSAL_SLAM, 5s);
        events.ScheduleEvent(EVENT_ORTA_TECTONIC_ROAR, 500ms);
        events.ScheduleEvent(EVENT_ORTA_RUPTURING_RUNES, 13s + 300ms);
        events.ScheduleEvent(EVENT_ORTA_MOUNTAINS_GRASP, 27s + 300ms);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        me->DespawnOrUnsummon(0s, 30s);
    }

    void Reset() override
    {
        WorldBossAI::Reset();
        me->SetPower(me->GetPowerType(), 30);
    }

    void UpdateAI(uint32 diff) override
    {
        Vehicle* veh = me->GetVehicleKit();

        if ((veh && veh->IsVehicleInUse()) || !UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ORTA_COLOSSAL_SLAM:
                    DoCast(SPELL_ORTA_COLOSSAL_SLAM_SELECTOR);
                    events.ScheduleEvent(EVENT_ORTA_COLOSSAL_SLAM, 31s + 700ms);
                    break;
                case EVENT_ORTA_TECTONIC_ROAR:
                    if (me->GetPower(me->GetPowerType()) >= 100)
                        DoCastVictim(SPELL_ORTA_TECTONIC_ROAR);
                    events.Repeat(500ms);
                    break;
                case EVENT_ORTA_RUPTURING_RUNES:
                    DoCast(SPELL_ORTA_RUPTURING_RUNES_CHANNEL);
                    events.ScheduleEvent(EVENT_ORTA_RUPTURING_RUNES, 31s + 800ms);
                    break;
                case EVENT_ORTA_MOUNTAINS_GRASP:
                    me->SetReactState(REACT_PASSIVE);
                    DoCastVictim(SPELL_ORTA_MOUNTAINS_GRASP_CAST);
                    events.ScheduleEvent(EVENT_DISCARD_WEAKLING, 5s);
                    break;
                case EVENT_DISCARD_WEAKLING:
                    DoCast(SPELL_ORTA_DISCARD_WEAKLING_SELECTOR);
                    events.ScheduleEvent(EVENT_ORTA_MOUNTAINS_GRASP, 31s + 800ms);
                    break;
                default:
                    break;
            }
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }
    }
};

// 450406 - Colossal Slam
class spell_orta_colossal_slam : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ORTA_COLOSSAL_SLAM_DAMAGE });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_ORTA_COLOSSAL_SLAM_DAMAGE, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_orta_colossal_slam::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 450454 - Tectonic Roar
class spell_orta_tectonic_roar : public SpellScript
{
    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        caster->SetPower(caster->GetPowerType(), 0);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_orta_tectonic_roar::HandleAfterCast);
    }
};

// 450677 - Rapturing Runes
class spell_orta_rapturing_runes_channel : public AuraScript
{
    static constexpr float RuneMinDist = 25.0f;
    static constexpr float RuneMaxDist = 55.0f;
    static constexpr float RuneAngleBetweenRunes = float(M_PI / 5);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        for (int8 i = 0; i < 2; i++)
        {
            float randomDist = frand(RuneMinDist, RuneMaxDist);
            float randomAngle = frand(_currentAngle, _currentAngle + RuneAngleBetweenRunes);
            float x = OrtaPlatformCenter.GetPositionX() + randomDist * std::cos(randomAngle);
            float y = OrtaPlatformCenter.GetPositionY() + randomDist * std::sin(randomAngle);
            float z = OrtaPlatformCenter.GetPositionZ();
            GetTarget()->UpdateGroundPositionZ(x, y, z);

            Position missileDest(x, y, z);
            GetTarget()->CastSpell(missileDest, SPELL_ORTA_RUPTURING_RUNES_MISSILE, CastSpellExtraArgsInit{
                .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR
            });
            _currentAngle += RuneAngleBetweenRunes;
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_orta_rapturing_runes_channel::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }

private:
    float _currentAngle = frand(0.0f, float(2 * M_PI));
};

// 33488 - Areatrigger: Rapturing Runes
struct at_orta_rapturing_runes : AreaTriggerAI
{
    at_orta_rapturing_runes(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (!unit->IsPlayer())
            return;

        Unit* caster = at->GetCaster();
        if (!caster)
            return;

        caster->CastSpell(unit, SPELL_ORTA_RUPTURING_RUNES_DAMAGE, CastSpellExtraArgsInit{
            .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR
        });
        at->Remove();
    }
};

// 450929 - Mountains Grasp
class spell_orta_mountains_grasp : public SpellScript
{
    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (!GetHitUnit()->IsPlayer())
            return;

        GetHitUnit()->CastSpell(GetCaster(), SPELL_ORTA_MOUNTAINS_GRASP_ENTER_VEHICLE, CastSpellExtraArgsInit{
            .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR
        });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_orta_mountains_grasp::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_orta_mountains_grasp_aurascript : public AuraScript
{
    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        GetTarget()->RemoveAurasDueToSpell(SPELL_ORTA_MOUNTAINS_GRASP_ENTER_VEHICLE, caster->GetGUID());
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_orta_mountains_grasp_aurascript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

class DiscardWeaklingEvent : public BasicEvent
{
public:
    DiscardWeaklingEvent(Unit* owner, ObjectGuid targetGUID) : BasicEvent(), _owner(owner), _targetGUID(targetGUID) { }

    bool Execute(uint64, uint32) override
    {
        if (Unit* target = ObjectAccessor::GetUnit(*_owner, _targetGUID))
            _owner->CastSpell(target, SPELL_ORTA_DISCARD_WEAKLING_KNOCKBACK, CastSpellExtraArgsInit{
                .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR
            });
        return true;
    }

private:
    Unit* _owner;
    ObjectGuid _targetGUID;
};

// 451130 - Discard Weakling
class spell_orta_discard_weakling_selector : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ORTA_DISCARD_WEAKLING_CAST });
    }

    // ToDo: Passengers are not selected

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->m_Events.AddEventAtOffset(new DiscardWeaklingEvent(GetCaster(), GetHitUnit()->GetGUID()), 1s + 500ms);
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Creature* creature = GetCaster()->ToCreature();
        if (!creature)
            return;

        GetCaster()->CastSpell(nullptr, SPELL_ORTA_DISCARD_WEAKLING_CAST);
        creature->SetReactState(REACT_AGGRESSIVE);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Vehicle* veh = GetCaster()->GetVehicleKit();
        if (!veh)
            return;

        for (auto const& [_, seat] : veh->Seats)
        {
            Unit* passenger = ObjectAccessor::GetUnit(*GetCaster(), seat.Passenger.Guid);
            if (!passenger)
                continue;

            targets.push_back(passenger);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_orta_discard_weakling_selector::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHit += SpellEffectFn(spell_orta_discard_weakling_selector::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_orta_discard_weakling_selector::FilterTargets, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_worldboss_orta()
{
    // Creature
    RegisterCreatureAI(worldboss_orta);

    // Areatrigger
    RegisterAreaTriggerAI(at_orta_rapturing_runes);

    // Spells
    RegisterSpellScript(spell_orta_colossal_slam);
    RegisterSpellScript(spell_orta_tectonic_roar);
    RegisterSpellScript(spell_orta_rapturing_runes_channel);
    RegisterSpellScript(spell_orta_discard_weakling_selector);
    RegisterSpellAndAuraScriptPair(spell_orta_mountains_grasp, spell_orta_mountains_grasp_aurascript);
}
