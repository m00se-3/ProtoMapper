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

#ifndef PROTOMAPPER_TEXT_ALLOCATOR_HPP
#define PROTOMAPPER_TEXT_ALLOCATOR_HPP

#include <memory_resource>
#include <list>

/*
	The TextAllocator memory resource is used to keep text data within the app stored in
	contiguous blocks. Therefore we are able to access and modify them in a performance
	friendly way.
*/

// An entry in a memory block's free list. Contains the location and size of each free block.
struct FreeBlock
{
	using CharType = char;
	
	CharType* location = nullptr;
	std::size_t size = 0u;

	FreeBlock() = default;
	FreeBlock(CharType* loc, size_t s);
};

/*
	A block of preallocated memory for storing textual data. Each block of memory can be modified
	and read from by the TextAllocator class.

	A linked list of AllocatedBlock objects are used to keep memory blocks contiguous and avoid 
	needless copying.
*/
struct AllocatedBlock
{
	using CharType = char;
	
	CharType* memory = nullptr;
	std::size_t offset = 0u, size = 0u;

	AllocatedBlock();
	AllocatedBlock(const AllocatedBlock&) = delete;
	AllocatedBlock(AllocatedBlock&&) = delete;
	~AllocatedBlock();
};

/*
	The TextAllocator class is responsible for efficiently utilizing blocks of preallocated blocks of
	string data. The TextAllocator contains a linked list of fixed-size memory blocks as well as a
	free list of blocks that can be reused.
*/
class TextAllocator : public std::pmr::memory_resource
{
	using CharType = char;
	
	std::list<AllocatedBlock> _memBlocks;
	std::list<FreeBlock> _freeList;

public:
	TextAllocator() = default;
	~TextAllocator();

protected:
	
	void* do_allocate(std::size_t bytes, std::size_t alignment) override;
	void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;
	bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

};

#endif // !PROTOMAPPER_TEXT_ALLOCATOR_HPP
