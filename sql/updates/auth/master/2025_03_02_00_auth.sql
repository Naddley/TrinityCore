DELETE FROM `build_info` WHERE `build` IN (59347,59425,59466);
INSERT INTO `build_info` (`build`,`majorVersion`,`minorVersion`,`bugfixVersion`,`hotfixVersion`) VALUES
(59347,11,1,0,NULL),
(59425,11,1,0,NULL),
(59466,11,1,0,NULL);

DELETE FROM `build_auth_key` WHERE `build` IN (59347,59425,59466);
INSERT INTO `build_auth_key` (`build`,`platform`,`arch`,`type`,`key`) VALUES
(59347,'Mac','A64','WoW',0xC7B81BB6F9207DA0CCEBFAC3186FC073),
(59347,'Mac','A64','WoWC',0xBAB986A50C7FDB89467AA5613A6BA345),
(59347,'Mac','x64','WoW',0x1A11343133BF1ABDBF0FF6B287B37C23),
(59347,'Mac','x64','WoWC',0x3A624C37EE150CC9B71CA3AD1F4532CF),
(59347,'Win','A64','WoW',0x80CAC49F6EA99AC68CA9B834466CA5DC),
(59347,'Win','x64','WoW',0x63228339538F9552D16855976C64848B),
(59347,'Win','x64','WoWC',0xBB45FAC562EB7CF3FC4C137C1AE93AF6),
(59425,'Mac','A64','WoW',0x14A01BEF9E9DB7C5A40438CE87E99B85),
(59425,'Mac','A64','WoWC',0x38B98DA1F8765C927C724CFE4E1D92C2),
(59425,'Mac','x64','WoW',0x9D230A7C010BA2E5B385CB4E24A59529),
(59425,'Mac','x64','WoWC',0xB0BF826891290E30173DCA5CF2B39F50),
(59425,'Win','A64','WoW',0xF40FED2615A2454E31B044B1AD564B7D),
(59425,'Win','x64','WoW',0x5FE3E3980A15A1D4ED64240BC0C6932D),
(59425,'Win','x64','WoWC',0xECA5DA344AF973B0D25AC1103FB694B8),
(59466,'Mac','A64','WoW',0xF6CB8EB7AB66F8909ADCA85052BFAE40),
(59466,'Mac','A64','WoWC',0x213104EC1BEF7656FC3930DCE4176184),
(59466,'Mac','x64','WoW',0x99075D27FF98A87368A08E87A3F105C3),
(59466,'Mac','x64','WoWC',0x516115232E1A8B3F1E3CA709F20EA84D),
(59466,'Win','A64','WoW',0x04F0986C0EAD648F8C7B27736A30088A),
(59466,'Win','x64','WoW',0x118FFD674F5B51C277F5C8505E9479D3),
(59466,'Win','x64','WoWC',0xC299CB416E332E4BBE5AA1AAE905A437);

UPDATE `realmlist` SET `gamebuild`=59466 WHERE `gamebuild` IN (59302,59347,59425);

ALTER TABLE `realmlist` CHANGE `gamebuild` `gamebuild` int unsigned NOT NULL DEFAULT '59466';
