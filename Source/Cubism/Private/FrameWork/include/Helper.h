#pragma once

// ---------- //
// ALLOCATION //
// ---------- //

/// Allocates heap memory without user align constraint.
///
/// @param  size   Number of bytes to allocate.
///
/// @return  Valid address to allocated memory on success; '0' otherwise.
void* Allocate(const unsigned int size);

/// Frees non-user alignment constraint heap memory.
///
/// @param  memory  Address of memory block to free.
void Deallocate(void* memory);


/// Allocates aligned heap memory.
///
/// @param  size       Number of bytes to allocate.
/// @param  alignment  Alignment for memory block.
///
/// @return  Valid address to allocated memory on success; '0' otherwise.
void* AllocateAligned(const unsigned int size, const unsigned int alignment);

/// Frees aligned heap memory.
///
/// @param  memory  Address of memory block to free.
void DeallocateAligned(void* alignedMemory);