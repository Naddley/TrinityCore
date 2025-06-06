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

#include "HotfixPackets.h"
#include "PacketOperators.h"
#include "PacketUtilities.h"

namespace WorldPackets::Hotfix
{
ByteBuffer& operator>>(ByteBuffer& data, DB2Manager::HotfixId& hotfixId)
{
    data >> hotfixId.PushID;
    data >> hotfixId.UniqueID;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, DB2Manager::HotfixId const& hotfixId)
{
    data << int32(hotfixId.PushID);
    data << uint32(hotfixId.UniqueID);

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, DB2Manager::HotfixRecord& hotfixRecord)
{
    data >> hotfixRecord.ID;
    data >> hotfixRecord.TableHash;
    data >> hotfixRecord.RecordID;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, DB2Manager::HotfixRecord const& hotfixRecord)
{
    data << hotfixRecord.ID;
    data << uint32(hotfixRecord.TableHash);
    data << int32(hotfixRecord.RecordID);

    return data;
}

void DBQueryBulk::Read()
{
    _worldPacket >> TableHash;
    _worldPacket >> BitsSize<13>(Queries);

    for (DBQueryRecord& record : Queries)
        _worldPacket >> record.RecordID;
}

WorldPacket const* DBReply::Write()
{
    _worldPacket << uint32(TableHash);
    _worldPacket << uint32(RecordID);
    _worldPacket << uint32(Timestamp);
    _worldPacket << Bits<3>(Status);
    _worldPacket << Size<uint32>(Data);
    _worldPacket.append(Data);

    return &_worldPacket;
}

WorldPacket const* AvailableHotfixes::Write()
{
    _worldPacket.reserve(4 + 4 + sizeof(DB2Manager::HotfixId) * Hotfixes.size());

    _worldPacket << int32(VirtualRealmAddress);
    _worldPacket << Size<uint32>(Hotfixes);
    for (DB2Manager::HotfixId const& hotfixId : Hotfixes)
        _worldPacket << hotfixId;

    return &_worldPacket;
}

void HotfixRequest::Read()
{
    _worldPacket >> ClientBuild;
    _worldPacket >> DataBuild;

    uint32 hotfixCount = _worldPacket.read<uint32>();
    if (hotfixCount > sDB2Manager.GetHotfixCount())
        OnInvalidArraySize(hotfixCount, sDB2Manager.GetHotfixCount());

    Hotfixes.resize(hotfixCount);
    for (int32& hotfixId : Hotfixes)
        _worldPacket >> hotfixId;
}

ByteBuffer& operator<<(ByteBuffer& data, HotfixConnect::HotfixData const& hotfixData)
{
    data << hotfixData.Record;
    data << uint32(hotfixData.Size);
    data << Bits<3>(hotfixData.Record.HotfixStatus);
    data.FlushBits();

    return data;
}

WorldPacket const* HotfixConnect::Write()
{
    _worldPacket << Size<uint32>(Hotfixes);
    for (HotfixData const& hotfix : Hotfixes)
        _worldPacket << hotfix;

    _worldPacket << Size<uint32>(HotfixContent);
    _worldPacket.append(HotfixContent);

    return &_worldPacket;
}
}
