/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include "Local.h"


// -------- //
// REQUIRES //
// -------- //

#include <stdlib.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

void* Allocate(const unsigned int size)
{
  return malloc(size);
}

void Deallocate(void *memory)
{
  free(memory);
}


void *AllocateAligned(const unsigned int size, const unsigned int alignment)
{
  size_t offset, shift, alignedAddress;
  void *allocation, ** preamble;


  offset = alignment - 1 + sizeof(void*);


  allocation = Allocate(size + (unsigned int)offset);


  alignedAddress = (size_t)allocation + sizeof(void*);


  shift = alignedAddress % alignment;


  if (shift)
  {
    alignedAddress += (alignment - shift);
  }


  preamble = (void**)alignedAddress;
  preamble[-1] = allocation;


  return (void*)alignedAddress;
}

void DeallocateAligned(void* alignedMemory)
{
  void **preamble;


  preamble = (void**)alignedMemory;


  Deallocate(preamble[-1]);
}
