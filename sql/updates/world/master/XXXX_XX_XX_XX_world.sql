SET @CGUID := 150000000;

-- Creature
DELETE FROM `creature` WHERE `guid` = @CGUID+0;
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnDifficulties`, `PhaseId`, `PhaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `MovementType`, `npcflag`, `unit_flags`, `unit_flags2`, `unit_flags3`, `VerifiedBuild`) VALUES
(@CGUID+0, 50061, 646, 5042, 5291, '0', 0, 0, 0, 0, 751.7205810546875, 157.25390625, 458.444580078125, 2.476229190826416015, 120, 0, 0, 0, NULL, NULL, NULL, NULL, 68453); -- Xariona (Area: Jagged Wastes - Difficulty: 0) CreateObject1 (Auras: 87239 - Zero Power, 73059 - Rogue Class Crit/Dodge Debuff)

-- Template Addon
DELETE FROM `creature_template_addon` WHERE `entry` IN (50061 /*50061 (Xariona) - Zero Power, Rogue Class Crit/Dodge Debuff*/);
INSERT INTO `creature_template_addon` (`entry`, `PathId`, `mount`, `StandState`, `AnimTier`, `VisFlags`, `SheathState`, `PvpFlags`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `visibilityDistanceType`, `auras`) VALUES
(50061, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''); -- 50061 (Xariona) - Zero Power, Rogue Class Crit/Dodge Debuff

-- Template
UPDATE `creature_template` SET `unit_flags`= 0x40|0x8000, `unit_flags2`= 0x800|0x400000, `VehicleId`= 1543, `flags_extra`= 0x40000000, `ScriptName`= 'npc_deepholm_worldboss_xariona' WHERE `entry`= 50061; -- Xariona
UPDATE `creature_template` SET `flags_extra`= 128, `AIName`= 'NullCreatureAI' WHERE `entry`= 50431;

-- Difficulty
UPDATE `creature_template_difficulty` SET `ContentTuningID`=54, `StaticFlags1`=0x30000000, `VerifiedBuild`=68453 WHERE (`Entry`=50061 AND `DifficultyID`=0); -- 50061 (Xariona) - CanSwim, Floating

-- ScriptName
DELETE FROM `spell_script_names` WHERE `ScriptName` IN ('spell_deepholm_twilight_buffet_targeting');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(95385, 'spell_deepholm_twilight_buffet_targeting');

-- Path 
SET @MOVERGUID := @CGUID+0;
SET @ENTRY := 50061;
SET @PATHOFFSET := 0;
SET @PATH := @ENTRY * 100 + @PATHOFFSET;
DELETE FROM `waypoint_path` WHERE `PathId`= @PATH;
INSERT INTO `waypoint_path` (`PathId`, `MoveType`, `Flags`, `Velocity`, `Comment`) VALUES
(@PATH, 0, 0x2, 14.6640, 'Xariona - Cyclic Path');

DELETE FROM `waypoint_path_node` WHERE `PathId`= @PATH;
INSERT INTO `waypoint_path_node` (`PathId`, `NodeId`, `PositionX`, `PositionY`, `PositionZ`, `Orientation`, `Delay`) VALUES
(@PATH, 0, 1187.3923, 77.9028, 458.44458, NULL, 0),
(@PATH, 1, 1003.1581, 57.72592, 458.44458, NULL, 0),
(@PATH, 2, 825.22644, 109.58838, 458.44458, NULL, 0),
(@PATH, 3, 680.686, 225.59448, 458.44458, NULL, 0),
(@PATH, 4, 591.5416, 388.08334, 458.44458, NULL, 0),
(@PATH, 5, 571.36475, 572.31757, 458.44458, NULL, 0),
(@PATH, 6, 623.2272, 750.24915, 458.44458, NULL, 0),
(@PATH, 7, 739.2332, 894.78955, 458.44458, NULL, 0),
(@PATH, 8, 901.7221, 983.93396, 458.44458, NULL, 0),
(@PATH, 9, 1085.9564, 1004.11084, 458.44458, NULL, 0),
(@PATH, 10, 1263.888, 952.2485, 458.44458, NULL, 0),
(@PATH, 11, 1408.4285, 836.2424, 458.44458, NULL, 0),
(@PATH, 12, 1497.5729, 673.7535, 458.44458, NULL, 0),
(@PATH, 13, 1517.7498, 489.51923, 458.44458, NULL, 0),
(@PATH, 14, 1465.8873, 311.58765, 458.44458, NULL, 0),
(@PATH, 15, 1349.8813, 167.04727, 458.44458, NULL, 0);

UPDATE `creature` SET `position_x`=1187.3923, `position_y`=77.9028, `position_z`=458.44458, `wander_distance`=0, `MovementType`=2 WHERE `guid`=@MOVERGUID;
UPDATE `creature_template_addon` SET `PathId` = @PATH WHERE `entry` = 50061;
