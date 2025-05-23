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

#ifndef TRINITYCORE_ACHIEVEMENT_PACKETS_H
#define TRINITYCORE_ACHIEVEMENT_PACKETS_H

#include "ObjectGuid.h"
#include "Optional.h"
#include "Packet.h"
#include "PacketUtilities.h"
#include "WowTime.h"

namespace WorldPackets
{
    namespace Achievement
    {
        struct EarnedAchievement
        {
            uint32 Id = 0;
            WowTime Date;
            ObjectGuid Owner;
            uint32 VirtualRealmAddress = 0;
            uint32 NativeRealmAddress = 0;
        };

        struct CriteriaProgress
        {
            uint32 Id = 0;
            uint64 Quantity = 0;
            ObjectGuid Player;
            uint32 StateFlags = 0;
            uint32 Flags = 0;
            WowTime Date;
            Duration<Seconds> TimeFromStart;
            Duration<Seconds> TimeFromCreate;
            Optional<uint64> DynamicID;
        };

        struct AllAchievements
        {
            std::vector<EarnedAchievement> Earned;
            std::vector<CriteriaProgress> Progress;
        };

        class AllAchievementData final : public ServerPacket
        {
        public:
            explicit AllAchievementData() : ServerPacket(SMSG_ALL_ACHIEVEMENT_DATA) { }

            WorldPacket const* Write() override;

            AllAchievements Data;
        };

        class AllAccountCriteria final : public ServerPacket
        {
        public:
            explicit AllAccountCriteria() : ServerPacket(SMSG_ALL_ACCOUNT_CRITERIA) { }

            WorldPacket const* Write() override;

            std::vector<CriteriaProgress> Progress;
        };

        class RespondInspectAchievements final : public ServerPacket
        {
        public:
            explicit RespondInspectAchievements() : ServerPacket(SMSG_RESPOND_INSPECT_ACHIEVEMENTS) { }

            WorldPacket const* Write() override;

            ObjectGuid Player;
            AllAchievements Data;
        };

        class CriteriaUpdate final : public ServerPacket
        {
        public:
            explicit CriteriaUpdate() : ServerPacket(SMSG_CRITERIA_UPDATE, 4 + 8 + 16 + 4 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            uint32 CriteriaID = 0;
            uint64 Quantity = 0;
            ObjectGuid PlayerGUID;
            uint32 StateFlags = 0;
            uint32 Flags = 0;
            WowTime CurrentTime;
            Duration<Seconds> ElapsedTime;
            Timestamp<> CreationTime;
            Optional<uint64> DynamicID;
        };

        class AccountCriteriaUpdate final : public ServerPacket
        {
        public:
            explicit AccountCriteriaUpdate() : ServerPacket(SMSG_ACCOUNT_CRITERIA_UPDATE) { }

            WorldPacket const* Write() override;

            CriteriaProgress Progress;
        };

        class CriteriaDeleted final : public ServerPacket
        {
        public:
            explicit CriteriaDeleted() : ServerPacket(SMSG_CRITERIA_DELETED, 4) { }

            WorldPacket const* Write() override;

            uint32 CriteriaID = 0;
        };

        class AchievementDeleted final : public ServerPacket
        {
        public:
            explicit AchievementDeleted() : ServerPacket(SMSG_ACHIEVEMENT_DELETED, 8) { }

            WorldPacket const* Write() override;

            uint32 AchievementID = 0;
            uint32 Immunities = 0; // this is just garbage, not used by client
        };

        class AchievementEarned final : public ServerPacket
        {
        public:
            explicit AchievementEarned() : ServerPacket(SMSG_ACHIEVEMENT_EARNED, 16 + 4 + 4 + 4 + 4 + 1 + 16) { }

            WorldPacket const* Write() override;

            ObjectGuid Earner;
            uint32 EarnerNativeRealm = 0;
            uint32 EarnerVirtualRealm = 0;
            uint32 AchievementID = 0;
            WowTime Time;
            bool Initial = false;
            ObjectGuid Sender;
        };

        class BroadcastAchievement final : public ServerPacket
        {
        public:
            explicit BroadcastAchievement() : ServerPacket(SMSG_BROADCAST_ACHIEVEMENT) { }

            WorldPacket const* Write() override;

            ObjectGuid PlayerGUID;
            std::string Name;
            uint32 AchievementID = 0;
            bool GuildAchievement = false;
        };

        struct GuildCriteriaProgress
        {
            int32 CriteriaID = 0;
            Timestamp<> DateCreated;
            Timestamp<> DateStarted;
            WowTime DateUpdated;
            uint64 Quantity = 0;
            ObjectGuid PlayerGUID;
            int32 Flags = 0;
            int32 StateFlags = 0;
        };

        class GuildCriteriaUpdate final : public ServerPacket
        {
        public:
            explicit GuildCriteriaUpdate() : ServerPacket(SMSG_GUILD_CRITERIA_UPDATE) { }

            WorldPacket const* Write() override;

            std::vector<GuildCriteriaProgress> Progress;
        };

        class GuildCriteriaDeleted final : public ServerPacket
        {
        public:
            explicit GuildCriteriaDeleted() : ServerPacket(SMSG_GUILD_CRITERIA_DELETED, 16 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid GuildGUID;
            int32 CriteriaID = 0;
        };

        class GuildSetFocusedAchievement final : public ClientPacket
        {
        public:
            explicit GuildSetFocusedAchievement(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_SET_FOCUSED_ACHIEVEMENT, std::move(packet)) { }

            void Read() override;

            uint32 AchievementID = 0;
        };

        class GuildAchievementDeleted final : public ServerPacket
        {
        public:
            explicit GuildAchievementDeleted() : ServerPacket(SMSG_GUILD_ACHIEVEMENT_DELETED, 16 + 4 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid GuildGUID;
            uint32 AchievementID = 0;
            WowTime TimeDeleted;
        };

        class GuildAchievementEarned final : public ServerPacket
        {
        public:
            explicit GuildAchievementEarned() : ServerPacket(SMSG_GUILD_ACHIEVEMENT_EARNED, 16 + 4 + 4) { }

            WorldPacket const* Write() override;

            uint32 AchievementID = 0;
            ObjectGuid GuildGUID;
            WowTime TimeEarned;
        };

        class AllGuildAchievements final : public ServerPacket
        {
        public:
            explicit AllGuildAchievements() : ServerPacket(SMSG_ALL_GUILD_ACHIEVEMENTS) { }

            WorldPacket const* Write() override;

            std::vector<EarnedAchievement> Earned;
        };

        class GuildGetAchievementMembers final : public ClientPacket
        {
        public:
            explicit GuildGetAchievementMembers(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_GET_ACHIEVEMENT_MEMBERS, std::move(packet)) { }

            void Read() override;

            ObjectGuid PlayerGUID;
            ObjectGuid GuildGUID;
            int32 AchievementID = 0;
        };

        struct GuildAchievementMember
        {
            GuildAchievementMember() = default;
            GuildAchievementMember(ObjectGuid guid) : MemberGUID(guid) { }

            ObjectGuid MemberGUID;
        };

        class GuildAchievementMembers final : public ServerPacket
        {
        public:
            explicit GuildAchievementMembers() : ServerPacket(SMSG_GUILD_ACHIEVEMENT_MEMBERS) { }

            WorldPacket const* Write() override;

            ObjectGuid GuildGUID;
            int32 AchievementID = 0;
            std::vector<GuildAchievementMember> Member;
        };

        ByteBuffer& operator<<(ByteBuffer& data, CriteriaProgress const& criteria);
    }
}

#endif // TRINITYCORE_ACHIEVEMENT_PACKETS_H
