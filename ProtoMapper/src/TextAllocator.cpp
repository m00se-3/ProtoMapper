/*
    ProtoMapper - Map creation and pathfinding software for game development.
    Copyright (C) 2023  Samuel Bridgham - moosethree473@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "TextAllocator.hpp"

constexpr const size_t BlockAllocationSize = 512u;

FreeBlock::FreeBlock(CharType* loc, size_t s)
	: location(loc), size(s)
{

}

AllocatedBlock::AllocatedBlock()
	: memory(new CharType[BlockAllocationSize * sizeof(CharType)]), size(BlockAllocationSize)
{
	memset(memory, 0, size);
}

AllocatedBlock::~AllocatedBlock() { delete[] memory; }


void* TextAllocator::do_allocate(std::size_t bytes, std::size_t alignment)
{
    auto it = _freeList.begin();
    CharType* result = nullptr;
    
    while(it != _freeList.end())
    {
        if (it->size > bytes * sizeof(CharType))
        {
            result = it->location;
            break;
        }
        ++it;
    }

    if (it != _freeList.end())
    {
        _freeList.erase(it);
        return result;
    }

    result = _memBlocks.emplace_back().memory;

    return result;
}

void TextAllocator::do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment)
{
    const size_t freeSize = bytes * sizeof(CharType);
    
    memset(ptr, 0, freeSize);

    _freeList.emplace_back(ptr, freeSize);
}

bool TextAllocator::do_is_equal(const std::pmr::memory_resource& other) const noexcept
{
    auto* ptr = dynamic_cast<const TextAllocator*>(&other);
    
    if (ptr != this) return false;

    return true;
}

TextAllocator::~TextAllocator()
{
    _memBlocks.clear();
    _freeList.clear();
}
