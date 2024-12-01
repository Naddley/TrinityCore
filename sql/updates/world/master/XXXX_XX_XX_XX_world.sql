SET @CGUID := 11100000;

-- Creature
DELETE FROM `creature` WHERE `guid` = @CGUID+0;
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnDifficulties`, `PhaseId`, `PhaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `MovementType`, `npcflag`, `unit_flags`, `unit_flags2`, `unit_flags3`, `VerifiedBuild`) VALUES
(@CGUID+0, 221067, 2601, 14753, 15330, '0', 0, 0, 0, 0, -1974.5347900390625, -99.8784713745117187, -1198.851806640625, 4.910727500915527343, 120, 0, 0, 0, NULL, NULL, NULL, NULL, 57689); -- Orta (Area: Old Sacrificial Pit - Difficulty: 0) CreateObject1 (Auras: 450167 - [DNT]Generic Power Bar Color - Rage)

-- Template Addon
DELETE FROM `creature_template_addon` WHERE `entry` = 221067;
INSERT INTO `creature_template_addon` (`entry`, `PathId`, `mount`, `StandState`, `AnimTier`, `VisFlags`, `SheathState`, `PvpFlags`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `visibilityDistanceType`, `auras`) VALUES
(221067, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, '450167'); -- 221067 (Orta) - [DNT]Generic Power Bar Color - Rage

-- Template
UPDATE `creature_template` SET `faction`=16, `speed_walk`=2, `speed_run`=2, `BaseAttackTime`=2000, `VehicleId`=8549 WHERE `entry`=221067; -- Orta
UPDATE `creature_template` SET `ScriptName` = 'worldboss_orta' WHERE `entry`=221067;

-- Creature Text
DELETE FROM `creature_text` WHERE `CreatureID` = 221067;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(221067, 0, 0, 'CRUSH. YOU.', 14, 0, 100, 0, 0, 54929, 0, 0, 'Orta');

-- Areatrigger
DELETE FROM `areatrigger_template` WHERE (`IsCustom`=0 AND `Id` = 35952);
INSERT INTO `areatrigger_template` (`Id`, `IsCustom`, `Flags`, `VerifiedBuild`) VALUES
(35952, 0, 0, 57689);

DELETE FROM `areatrigger_create_properties` WHERE (`IsCustom`=0 AND `Id` = 33488);
INSERT INTO `areatrigger_create_properties` (`Id`, `IsCustom`, `AreaTriggerId`, `IsAreatriggerCustom`, `Flags`, `MoveCurveId`, `ScaleCurveId`, `MorphCurveId`, `FacingCurveId`, `AnimId`, `AnimKitId`, `DecalPropertiesId`, `SpellForVisuals`, `TimeToTarget`, `TimeToTargetScale`, `Shape`, `ShapeData0`, `ShapeData1`, `ShapeData2`, `ShapeData3`, `ShapeData4`, `ShapeData5`, `ShapeData6`, `ShapeData7`, `VerifiedBuild`) VALUES
(33488, 0, 35952, 0, 0, 0, 0, 0, 0, -1, 0, 0, NULL, 0, 20000, 0, 6, 6, 0, 0, 0, 0, 0, 0, 57689); -- Spell: 450862 (Rupturing Runes)

UPDATE `areatrigger_create_properties` SET `ScriptName` = 'at_orta_rapturing_runes' WHERE (`IsCustom`=0 AND `Id` = 33488);

-- SpellScript
DELETE FROM `spell_script_names` WHERE `spell_id` IN (450406, 450454, 450677, 450929, 451130);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(450406, 'spell_orta_colossal_slam'),
(450454, 'spell_orta_tectonic_roar'),
(450677, 'spell_orta_rapturing_runes_channel'),
(450929, 'spell_orta_mountains_grasp'),
(451130, 'spell_orta_discard_weakling_selector');

-- Serverside Spell
DELETE FROM `serverside_spell` WHERE `Id` = 136531;
INSERT INTO `serverside_spell` (`Id`, `DifficultyID`, `CategoryId`, `Dispel`, `Mechanic`, `Attributes`, `AttributesEx`, `AttributesEx2`, `AttributesEx3`, `AttributesEx4`, `AttributesEx5`, `AttributesEx6`, `AttributesEx7`, `AttributesEx8`, `AttributesEx9`, `AttributesEx10`, `AttributesEx11`, `AttributesEx12`, `AttributesEx13`, `AttributesEx14`, `Stances`, `StancesNot`, `Targets`, `TargetCreatureType`, `RequiresSpellFocus`, `FacingCasterFlags`, `CasterAuraState`, `TargetAuraState`, `ExcludeCasterAuraState`, `ExcludeTargetAuraState`, `CasterAuraSpell`, `TargetAuraSpell`, `ExcludeCasterAuraSpell`, `ExcludeTargetAuraSpell`, `CasterAuraType`, `TargetAuraType`, `ExcludeCasterAuraType`, `ExcludeTargetAuraType`, `CastingTimeIndex`, `RecoveryTime`, `CategoryRecoveryTime`, `StartRecoveryCategory`, `StartRecoveryTime`, `InterruptFlags`, `AuraInterruptFlags1`, `AuraInterruptFlags2`, `ChannelInterruptFlags1`, `ChannelInterruptFlags2`, `ProcFlags`, `ProcFlags2`, `ProcChance`, `ProcCharges`, `ProcCooldown`, `ProcBasePPM`, `MaxLevel`, `BaseLevel`, `SpellLevel`, `DurationIndex`, `RangeIndex`, `Speed`, `LaunchDelay`, `StackAmount`, `EquippedItemClass`, `EquippedItemSubClassMask`, `EquippedItemInventoryTypeMask`, `ContentTuningId`, `SpellName`, `ConeAngle`, `ConeWidth`, `MaxTargetLevel`, `MaxAffectedTargets`, `SpellFamilyName`, `SpellFamilyFlags1`, `SpellFamilyFlags2`, `SpellFamilyFlags3`, `SpellFamilyFlags4`, `DmgClass`, `PreventionType`, `AreaGroupId`, `SchoolMask`, `ChargeCategoryId`) VALUES
(136531, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 152, 0, 0, 0, -1, 0, 0, 0, 'Orta Mountains Grasp Enter Vehicle', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

DELETE FROM `serverside_spell_effect` WHERE `SpellID`=136531;
INSERT INTO `serverside_spell_effect` (`SpellID`, `EffectIndex`, `DifficultyID`, `Effect`, `EffectAura`, `EffectAmplitude`, `EffectAttributes`, `EffectAuraPeriod`, `EffectBonusCoefficient`, `EffectChainAmplitude`, `EffectChainTargets`, `EffectItemType`, `EffectMechanic`, `EffectPointsPerResource`, `EffectPosFacing`, `EffectRealPointsPerLevel`, `EffectTriggerSpell`, `BonusCoefficientFromAP`, `PvpMultiplier`, `Coefficient`, `Variance`, `ResourceCoefficient`, `GroupSizeBasePointsCoefficient`, `EffectBasePoints`, `EffectMiscValue1`, `EffectMiscValue2`, `EffectRadiusIndex1`, `EffectRadiusIndex2`, `EffectSpellClassMask1`, `EffectSpellClassMask2`, `EffectSpellClassMask3`, `EffectSpellClassMask4`, `ImplicitTarget1`, `ImplicitTarget2`) VALUES
(136531, 0, 0, 6, 236, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0);

-- Difficulty
UPDATE `creature_template_difficulty` SET `LevelScalingDeltaMin`=3, `LevelScalingDeltaMax`=3, `ContentTuningID`=2863, `StaticFlags1`=0x10000000, `VerifiedBuild`=57689 WHERE (`Entry`=221067 AND `DifficultyID`=0); -- 221067 (Orta) - CanSwim

-- Model
DELETE FROM `creature_model_info` WHERE `DisplayID` IN (117384, 117391, 117383, 117394, 117392, 117393, 117390);
INSERT INTO `creature_model_info` (`DisplayID`, `BoundingRadius`, `CombatReach`, `DisplayID_Other_Gender`, `VerifiedBuild`) VALUES
(117384, 3.302712678909301757, 2.599999904632568359, 0, 57689),
(117391, 3.302712678909301757, 2.599999904632568359, 0, 57689),
(117383, 3.302712678909301757, 2.599999904632568359, 0, 57689),
(117394, 3.302712678909301757, 2.599999904632568359, 0, 57689),
(117392, 3.302712678909301757, 2.599999904632568359, 0, 57689),
(117393, 3.302712678909301757, 2.599999904632568359, 0, 57689),
(117390, 3.302712678909301757, 2.599999904632568359, 0, 57689);

UPDATE `creature_model_info` SET `VerifiedBuild`=57689 WHERE `DisplayID` IN (114989, 77759, 118229, 116084, 112196, 119705, 5233, 120007, 120000, 120226, 118182, 120010, 119704, 116448, 70623, 116544, 54439, 45042, 116524, 38551, 109247, 115740, 114531, 114532, 28647, 121467, 90693, 114986, 42372, 121468, 65631, 66845, 88201, 120684, 90691, 90694, 4619, 38615, 24992, 66843, 66844, 121288, 19073, 119366, 119367, 119368, 119365, 27710, 87778, 120686, 89910, 24993, 42373, 40692, 40690, 40689, 57899, 56658, 93574, 120990, 53768, 118709, 116449, 120153, 116457, 116446, 116447, 118563, 116456, 116458, 120227, 120402, 103813, 120403, 23767, 11686, 120368, 117389, 119835, 114988, 114422, 114421, 117385, 120401, 114423, 115755);
UPDATE `creature_model_info` SET `CombatReach`=6.75, `VerifiedBuild`=57689 WHERE `DisplayID` IN (114772, 114768);
UPDATE `creature_model_info` SET `CombatReach`=9, `VerifiedBuild`=57689 WHERE `DisplayID`=119303;
