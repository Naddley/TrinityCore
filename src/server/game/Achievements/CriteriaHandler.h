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

#ifndef CriteriaHandler_h__
#define CriteriaHandler_h__

#include "Common.h"
#include "DBCEnums.h"
#include "Duration.h"
#include "ObjectGuid.h"
#include <span>
#include <unordered_map>
#include <vector>
#include <ctime>

class Player;
class WorldObject;
class WorldPacket;
struct AchievementEntry;
struct CriteriaEntry;
struct CriteriaTreeEntry;
struct ModifierTreeEntry;
struct QuestObjective;
struct ScenarioStepEntry;

struct ModifierTreeNode
{
    ModifierTreeEntry const* Entry;
    std::vector<ModifierTreeNode const*> Children;
};

enum CriteriaFlagsCu
{
    CRITERIA_FLAG_CU_PLAYER             = 0x1,
    CRITERIA_FLAG_CU_ACCOUNT            = 0x2,
    CRITERIA_FLAG_CU_GUILD              = 0x4,
    CRITERIA_FLAG_CU_SCENARIO           = 0x8,
    CRITERIA_FLAG_CU_QUEST_OBJECTIVE    = 0x10
};

struct Criteria
{
    uint32 ID = 0;
    CriteriaEntry const* Entry = nullptr;
    ModifierTreeNode const* Modifier = nullptr;
    uint32 FlagsCu = 0;
};

typedef std::vector<Criteria const*> CriteriaList;
typedef std::unordered_map<uint32, CriteriaList> CriteriaListByAsset;

struct CriteriaTree
{
    uint32 ID = 0;
    CriteriaTreeEntry const* Entry = nullptr;
    AchievementEntry const* Achievement = nullptr;
    ScenarioStepEntry const* ScenarioStep = nullptr;
    ::QuestObjective const* QuestObjective = nullptr;
    ::Criteria const* Criteria = nullptr;
    std::vector<CriteriaTree const*> Children;
};

typedef std::vector<CriteriaTree const*> CriteriaTreeList;

struct CriteriaProgress
{
    uint64 Counter = 0;
    std::time_t Date = std::time_t(0);                      // latest update time.
    ObjectGuid PlayerGUID;                                  // GUID of the player that last updated the criteria
    bool Changed = false;
};

enum CriteriaDataType
{                                                 // value1         value2        comment
    CRITERIA_DATA_TYPE_NONE                 = 0,  // 0              0
    CRITERIA_DATA_TYPE_T_CREATURE           = 1,  // CreatureId     0
    CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE  = 2,  // ClassId        RaceId
    CRITERIA_DATA_TYPE_T_PLAYER_LESS_HEALTH = 3,  // HealthPercent 0
    CRITERIA_DATA_TYPE_S_AURA               = 5,  // SpellId        EffectIndex
    CRITERIA_DATA_TYPE_T_AURA               = 7,  // SpellId        EffectIndex
    CRITERIA_DATA_TYPE_VALUE                = 8,  // Minvalue                     value provided with  update must be not less that limit
    CRITERIA_DATA_TYPE_T_LEVEL              = 9,  // Minlevel                     minlevel of target
    CRITERIA_DATA_TYPE_T_GENDER             = 10, // Gender                       0=male; 1=female
    CRITERIA_DATA_TYPE_SCRIPT               = 11, // Scripted requirement
    CRITERIA_DATA_TYPE_MAP_DIFFICULTY       = 12, // used on 3.3.5a branch
    CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT     = 13, // Count                        "with less than %u people in the zone"
    CRITERIA_DATA_TYPE_T_TEAM               = 14, // Team                         HORDE(67), ALLIANCE(469)
    CRITERIA_DATA_TYPE_S_DRUNK              = 15, // DrunkenState   0             (enum DrunkenState) of player
    CRITERIA_DATA_TYPE_HOLIDAY              = 16, // HolidayId      0             event in holiday time
    CRITERIA_DATA_TYPE_BG_LOSS_TEAM_SCORE   = 17, // MinScore       MaxScore      player's team win bg and opposition team have team score in range
    CRITERIA_DATA_TYPE_INSTANCE_SCRIPT      = 18, // 0              0             maker instance script call for check current criteria requirements fit
    CRITERIA_DATA_TYPE_S_EQUIPPED_ITEM      = 19, // ItemLevel      Quality       for equipped item in slot to check item level and quality
    CRITERIA_DATA_TYPE_MAP_ID               = 20, // MapId          0             player must be on map with id in map_id
    CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE  = 21, // Class          Race
    CRITERIA_DATA_TYPE_NTH_BIRTHDAY         = 22, // used on 3.3.5a branch
    CRITERIA_DATA_TYPE_S_KNOWN_TITLE        = 23, // TitleId                      known (pvp) title, values from dbc
    CRITERIA_DATA_TYPE_GAME_EVENT           = 24, // GameEventId    0
    CRITERIA_DATA_TYPE_S_ITEM_QUALITY       = 25, // ItemQuality    0

    MAX_CRITERIA_DATA_TYPE
};

struct CriteriaData
{
    CriteriaDataType DataType;
    union
    {
        // CRITERIA_DATA_TYPE_NONE              = 0 (no data)
        // CRITERIA_DATA_TYPE_T_CREATURE        = 1
        struct
        {
            uint32 Id;
        } Creature;
        // CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE = 2
        // CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE = 21
        struct
        {
            uint32 Class;
            uint32 Race;
        } ClassRace;
        // CRITERIA_DATA_TYPE_T_PLAYER_LESS_HEALTH = 3
        struct
        {
            uint32 Percent;
        } Health;
        // CRITERIA_DATA_TYPE_S_AURA            = 5
        // CRITERIA_DATA_TYPE_T_AURA            = 7
        struct
        {
            uint32 SpellId;
            uint32 EffectIndex;
        } Aura;
        // CRITERIA_DATA_TYPE_VALUE             = 8
        struct
        {
            uint32 Value;
            uint32 ComparisonType;
        } Value;
        // CRITERIA_DATA_TYPE_T_LEVEL           = 9
        struct
        {
            uint32 Min;
        } Level;
        // CRITERIA_DATA_TYPE_T_GENDER          = 10
        struct
        {
            uint32 Gender;
        } Gender;
        // CRITERIA_DATA_TYPE_SCRIPT            = 11 (no data)
        // CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT  = 13
        struct
        {
            uint32 MaxCount;
        } MapPlayers;
        // CRITERIA_DATA_TYPE_T_TEAM            = 14
        struct
        {
            uint32 Team;
        } Team;
        // CRITERIA_DATA_TYPE_S_DRUNK           = 15
        struct
        {
            uint32 State;
        } Drunk;
        // CRITERIA_DATA_TYPE_HOLIDAY           = 16
        struct
        {
            uint32 Id;
        } Holiday;
        // CRITERIA_DATA_TYPE_BG_LOSS_TEAM_SCORE= 17
        struct
        {
            uint32 Min;
            uint32 Max;
        } BattlegroundScore;
        // CRITERIA_DATA_TYPE_INSTANCE_SCRIPT   = 18 (no data)
        // CRITERIA_DATA_TYPE_S_EQUIPPED_ITEM   = 19
        struct
        {
            uint32 ItemLevel;
            uint32 Quality;
        } EquippedItem;
        // CRITERIA_DATA_TYPE_MAP_ID            = 20
        struct
        {
            uint32 Id;
        } Map;
        // CRITERIA_DATA_TYPE_KNOWN_TITLE       = 23
        struct
        {
            uint32 Id;
        } KnownTitle;
        // CRITERIA_DATA_TYPE_GAME_EVENT        = 25
        struct
        {
            uint32 Id;
        } GameEvent;
        // CRITERIA_DATA_TYPE_S_ITEM_QUALITY    = 24
        struct
        {
            uint32 Quality;
        } ItemQuality;
        // raw
        struct
        {
            uint32 Value1;
            uint32 Value2;
        } Raw;
    };
    uint32 ScriptId;

    CriteriaData() : DataType(CRITERIA_DATA_TYPE_NONE)
    {
        Raw.Value1 = 0;
        Raw.Value2 = 0;
        ScriptId = 0;
    }

    CriteriaData(uint32 _dataType, uint32 _value1, uint32 _value2, uint32 _scriptId) : DataType(CriteriaDataType(_dataType))
    {
        Raw.Value1 = _value1;
        Raw.Value2 = _value2;
        ScriptId = _scriptId;
    }

    bool IsValid(Criteria const* criteria);
    bool Meets(uint32 criteriaId, Player const* source, WorldObject const* target, uint32 miscValue1 = 0, uint32 miscValue2 = 0) const;
};

struct CriteriaDataSet
{
    CriteriaDataSet() : _criteriaId(0) { }
    void Add(CriteriaData const& data) { _storage.push_back(data); }
    bool Meets(Player const* source, WorldObject const* target, uint32 miscValue1 = 0, uint32 miscValue2 = 0) const;
    void SetCriteriaId(uint32 id) { _criteriaId = id; }
private:
    uint32 _criteriaId;
    std::vector<CriteriaData> _storage;
};

typedef std::unordered_map<uint32, CriteriaProgress> CriteriaProgressMap;

enum ProgressType
{
    PROGRESS_SET,
    PROGRESS_ACCUMULATE,
    PROGRESS_HIGHEST
};

class TC_GAME_API CriteriaHandler
{
public:
    CriteriaHandler();
    virtual ~CriteriaHandler();

    CriteriaHandler(CriteriaHandler const& right) = delete;
    CriteriaHandler(CriteriaHandler&& right) = delete;
    CriteriaHandler& operator=(CriteriaHandler const& right) = delete;
    CriteriaHandler& operator=(CriteriaHandler&& right) = delete;

    virtual void Reset();

    void UpdateCriteria(CriteriaType type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, uint64 miscValue3 = 0, WorldObject const* ref = nullptr, Player* referencePlayer = nullptr);

    virtual void SendAllData(Player const* receiver) const = 0;

    void UpdateTimedCriteria(Milliseconds timeDiff);
    void StartCriteria(CriteriaStartEvent startEvent, uint32 entry, Milliseconds timeLost = Milliseconds::zero());
    virtual void FailCriteria(CriteriaFailEvent failEvent, uint32 asset);

protected:
    virtual void SendCriteriaUpdate(Criteria const* criteria, CriteriaProgress const* progress, Seconds timeElapsed, bool timedCompleted) const = 0;

    void UpdateCriteria(Criteria const* criteria, uint64 miscValue1 = 0, uint64 miscValue2 = 0, uint64 miscValue3 = 0, WorldObject const* ref = nullptr, Player* referencePlayer = nullptr);
    CriteriaProgress* GetCriteriaProgress(Criteria const* entry);
    void SetCriteriaProgress(Criteria const* criteria, uint64 changeValue, Player* referencePlayer, ProgressType progressType = PROGRESS_SET);
    void RemoveCriteriaProgress(Criteria const* criteria);
    virtual void SendCriteriaProgressRemoved(uint32 criteriaId) = 0;

    bool IsCompletedCriteriaTree(CriteriaTree const* tree);
    virtual bool CanUpdateCriteriaTree(Criteria const* criteria, CriteriaTree const* tree, Player* referencePlayer) const;
    virtual bool CanCompleteCriteriaTree(CriteriaTree const* tree);
    virtual void CompletedCriteriaTree(CriteriaTree const* tree, Player* referencePlayer) = 0;
    virtual void AfterCriteriaTreeUpdate(CriteriaTree const* /*tree*/, Player* /*referencePlayer*/) { }

    bool IsCompletedCriteria(Criteria const* criteria, uint64 requiredAmount);
    bool CanUpdateCriteria(Criteria const* criteria, CriteriaTreeList const* trees, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, WorldObject const* ref, Player* referencePlayer);

    virtual void SendPacket(WorldPacket const* data) const = 0;

    bool ConditionsSatisfied(Criteria const* criteria, Player* referencePlayer) const;
    bool RequirementsSatisfied(Criteria const* criteria, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, WorldObject const* ref, Player* referencePlayer) const;
    virtual bool RequiredAchievementSatisfied(uint32 /*achievementId*/) const { return false; }
    bool ModifierTreeSatisfied(ModifierTreeNode const* parent, uint64 miscValue1, uint64 miscValue2, WorldObject const* ref, Player* referencePlayer) const;
    bool ModifierSatisfied(ModifierTreeEntry const* modifier, uint64 miscValue1, uint64 miscValue2, WorldObject const* ref, Player* referencePlayer) const;

    virtual std::string GetOwnerInfo() const = 0;
    virtual CriteriaList const& GetCriteriaByType(CriteriaType type, uint32 asset) const = 0;

    CriteriaProgressMap _criteriaProgress;
    std::unordered_map<uint32 /*criteriaID*/, Milliseconds /*time left*/> _startedCriteria;
};

class TC_GAME_API CriteriaMgr
{
    CriteriaMgr();
    ~CriteriaMgr();

public:
    CriteriaMgr(CriteriaMgr const& right) = delete;
    CriteriaMgr(CriteriaMgr&& right) = delete;
    CriteriaMgr& operator=(CriteriaMgr const& right) = delete;
    CriteriaMgr& operator=(CriteriaMgr&& right) = delete;

    static char const* GetCriteriaTypeString(CriteriaType type);
    static char const* GetCriteriaTypeString(uint32 type);

    static CriteriaMgr* Instance();

    CriteriaList const& GetPlayerCriteriaByType(CriteriaType type, uint32 asset) const;

    CriteriaList const& GetGuildCriteriaByType(CriteriaType type) const
    {
        return _guildCriteriasByType[size_t(type)];
    }

    CriteriaList const& GetScenarioCriteriaByTypeAndScenario(CriteriaType type, uint32 scenarioId) const;

    CriteriaList const& GetQuestObjectiveCriteriaByType(CriteriaType type) const
    {
        return _questObjectiveCriteriasByType[size_t(type)];
    }

    CriteriaTreeList const* GetCriteriaTreesByCriteria(uint32 criteriaId) const;

    std::unordered_map<int32, CriteriaList> const& GetCriteriaByStartEvent(CriteriaStartEvent startEvent) const;
    CriteriaList const* GetCriteriaByStartEvent(CriteriaStartEvent startEvent, int32 asset) const;

    std::unordered_map<int32, CriteriaList> const& GetCriteriaByFailEvent(CriteriaFailEvent failEvent) const;
    CriteriaList const* GetCriteriaByFailEvent(CriteriaFailEvent failEvent, int32 asset) const;

    CriteriaDataSet const* GetCriteriaDataSet(Criteria const* criteria) const;

    static bool IsGroupCriteriaType(CriteriaType type)
    {
        switch (type)
        {
            case CriteriaType::KillCreature:
            case CriteriaType::WinBattleground:
            case CriteriaType::BeSpellTarget:       // NYI
            case CriteriaType::WinAnyRankedArena:
            case CriteriaType::GainAura:            // NYI
            case CriteriaType::WinAnyBattleground:  // NYI
                return true;
            default:
                break;
        }

        return false;
    }

    template<typename Func>
    static void WalkCriteriaTree(CriteriaTree const* tree, Func const& func)
    {
        for (CriteriaTree const* node : tree->Children)
            WalkCriteriaTree(node, func);

        func(tree);
    }

    void LoadCriteriaModifiersTree();
    void LoadCriteriaList();
    void LoadCriteriaData();
    CriteriaTree const* GetCriteriaTree(uint32 criteriaTreeId) const;
    Criteria const* GetCriteria(uint32 criteriaId) const;
    ModifierTreeNode const* GetModifierTree(uint32 modifierTreeId) const;

    static std::span<CriteriaType const> GetRetroactivelyUpdateableCriteriaTypes();

private:
    std::unordered_map<uint32, CriteriaDataSet> _criteriaDataMap;

    std::unordered_map<uint32, CriteriaTree*> _criteriaTrees;
    std::unordered_map<uint32, Criteria*> _criteria;
    std::unordered_map<uint32, ModifierTreeNode*> _criteriaModifiers;

    std::unordered_map<uint32, CriteriaTreeList> _criteriaTreeByCriteria;

    // store criterias by type to speed up lookup
    static CriteriaList const EmptyCriteriaList;
    CriteriaList _criteriasByType[size_t(CriteriaType::Count)];
    CriteriaListByAsset _criteriasByAsset[size_t(CriteriaType::Count)];
    CriteriaList _guildCriteriasByType[size_t(CriteriaType::Count)];
    CriteriaListByAsset _scenarioCriteriasByTypeAndScenarioId[size_t(CriteriaType::Count)];
    CriteriaList _questObjectiveCriteriasByType[size_t(CriteriaType::Count)];

    std::unordered_map<int32, CriteriaList> _criteriasByStartEvent[size_t(CriteriaStartEvent::Count)];
    std::unordered_map<int32, CriteriaList> _criteriasByFailEvent[size_t(CriteriaFailEvent::Count)];
};

#define sCriteriaMgr CriteriaMgr::Instance()

#endif // CriteriaHandler_h__
