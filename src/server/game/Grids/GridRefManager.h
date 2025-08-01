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

#ifndef _GRIDREFMANAGER
#define _GRIDREFMANAGER

#include "RefManager.h"

template<class OBJECT>
class GridReference;

template<class OBJECT>
class GridRefManager : public RefManager<GridReference<OBJECT>>
{
};

template <typename ObjectType>
struct GridRefManagerContainer
{
    using Container = GridRefManager<ObjectType>;
    using ValueType = ObjectType*;

    static bool Insert(Container& container, ValueType object)
    {
        object->AddToGrid(container);
        return true;
    }

    static bool Remove(Container& /*container*/, ValueType object)
    {
        object->RemoveFromGrid();
        return true;
    }

    static std::size_t Size(Container const& container)
    {
        return container.size();
    }
};

#endif
