// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "collector.h"
#include "internal.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef KJS_DEBUG_MEM
#include <typeinfo>
#endif

namespace KJS {

  class CollectorBlock {
  public:
    CollectorBlock(int s);
    ~CollectorBlock();
    int size;
    int filled;
    ValueImp** mem;
    CollectorBlock *prev, *next;
  };

} // namespace

using namespace KJS;

CollectorBlock::CollectorBlock(int s)
  : size(s),
    filled(0),
    prev(0L),
    next(0L)
{
  mem = new ValueImp*[size];
}

CollectorBlock::~CollectorBlock()
{
  delete [] mem;
  mem = 0L;
}

CollectorBlock* Collector::root = 0L;
CollectorBlock* Collector::currentBlock = 0L;
unsigned long Collector::filled = 0;
unsigned long Collector::softLimit = KJS_MEM_INCREMENT;

bool Collector::memLimitReached = false;

#ifdef KJS_DEBUG_MEM
bool Collector::collecting = false;
#endif

void* Collector::allocate(size_t s)
{
  if (s == 0)
    return 0L;

  // Try and deal with memory requirements in a scalable way. Simple scripts
  // should only require small amounts of memory, but for complex scripts we don't
  // want to end up running the garbage collector hundreds of times a second.
  if (filled >= softLimit) {
    collect();
    if (softLimit/(1+filled) < 2 && softLimit < KJS_MEM_LIMIT) {
      // Even after collection we are still using more than half of the limit,
      // so increase the limit
      softLimit = (unsigned long) (softLimit * 1.4);
    }
  }

  ValueImp *m = static_cast<ValueImp*>(malloc(s));
#ifdef KJS_DEBUG_MEM
  //fprintf( stderr, "allocate: size=%d valueimp=%p\n",s,m);
#endif

  // VI_CREATED and VI_GCALLOWED being unset ensure that value
  // is protected from GC before any constructors are run
  static_cast<ValueImp*>(m)->_flags = 0;

  if (!root) {
    root = new CollectorBlock(BlockSize);
    currentBlock = root;
  }

  CollectorBlock *block = currentBlock;
  if (!block)
    block = root;

  // search for a block with space left
  while (block->next && block->filled == block->size)
    block = block->next;

  if (block->filled >= block->size) {
#ifdef KJS_DEBUG_MEM
    //fprintf( stderr, "allocating new block of size %d\n", block->size);
#endif
    CollectorBlock *tmp = new CollectorBlock(BlockSize);
    block->next = tmp;
    tmp->prev = block;
    block = tmp;
  }
  currentBlock = block;
  // fill free spot in the block
  *(block->mem + block->filled) = m;
  filled++;
  block->filled++;

  if (softLimit >= KJS_MEM_LIMIT) {
    memLimitReached = true;
    fprintf(stderr,"Out of memory");
  }

  return m;
}

/**
 * Mark-sweep garbage collection.
 */
bool Collector::collect()
{
#ifdef KJS_DEBUG_MEM
  fprintf(stderr,"Collector::collect()\n");
#endif
  bool deleted = false;
  // MARK: first unmark everything
  CollectorBlock *block = root;
  while (block) {
    ValueImp **r = block->mem;
    assert(r);
    for (int i = 0; i < block->filled; i++, r++)
      (*r)->_flags &= ~ValueImp::VI_MARKED;
    block = block->next;
  }

  // mark all referenced objects recursively
  // starting out from the set of root objects
  if (InterpreterImp::s_hook) {
    InterpreterImp *scr = InterpreterImp::s_hook;
    do {
      //fprintf( stderr, "Collector marking interpreter %p\n",(void*)scr);
      scr->mark();
      scr = scr->next;
    } while (scr != InterpreterImp::s_hook);
  }

  // mark any other objects that we wouldn't delete anyway
  block = root;
  while (block) {
    ValueImp **r = block->mem;
    assert(r);
    for (int i = 0; i < block->filled; i++, r++)
    {
      ValueImp *imp = (*r);
      // Check for created=true, marked=false and (gcallowed=false or refcount>0)
      if ((imp->_flags & (ValueImp::VI_CREATED|ValueImp::VI_MARKED)) == ValueImp::VI_CREATED &&
          ( (imp->_flags & ValueImp::VI_GCALLOWED) == 0 || imp->refcount ) ) {
        //fprintf( stderr, "Collector marking imp=%p\n",(void*)imp);
        imp->mark();
      }
    }
    block = block->next;
  }

  // SWEEP: delete everything with a zero refcount (garbage)
  // 1st step: destruct all objects
  block = root;
  while (block) {
    ValueImp **r = block->mem;
    for (int i = 0; i < block->filled; i++, r++) {
      ValueImp *imp = (*r);
      // Can delete if marked==false
      if ((imp->_flags & (ValueImp::VI_CREATED|ValueImp::VI_MARKED)) == ValueImp::VI_CREATED ) {
        // emulate destructing part of 'operator delete()'
        //fprintf( stderr, "Collector::deleting ValueImp %p (%s)\n", (void*)imp, typeid(*imp).name());
        imp->~ValueImp();
      }
    }
    block = block->next;
  }

  // 2nd step: free memory
  block = root;
  while (block) {
    ValueImp **r = block->mem;
    int freespot = block->filled;
    bool firstfreeset = false;
    int del = 0;
    for (int i = 0; i < block->filled; i++, r++) {
      ValueImp *imp = (*r);
      if ((imp->_flags & ValueImp::VI_DESTRUCTED) != 0) {
        free(imp);
        del++;
        if (!firstfreeset) {
          firstfreeset = true;
          freespot = r - block->mem;
        }
      } else if (firstfreeset) {
         *(block->mem + freespot) = imp;
         freespot++;
      }
    }
    filled -= del;
    block->filled -= del;
    assert(freespot == block->filled);
    block = block->next;
    if (del)
      deleted = true;
  }

  // delete the empty containers
  block = root;
  currentBlock = 0L;
  CollectorBlock *last = root;
  while (block) {
    CollectorBlock *next = block->next;
    if (block->filled == 0) {
      if (block->prev)
        block->prev->next = next;
      if (block == root)
        root = next;
      if (next)
        next->prev = block->prev;
      assert(block != root);
      delete block;
    } else if (!currentBlock) {
        if (block->filled < block->size)
          currentBlock = block;
        else
          last = block;
    }
    block = next;
  }
  if (!currentBlock)
    currentBlock = last;
#if 0
  // This is useful to track down memory leaks
  static int s_count = 0;
  fprintf(stderr, "Collector done (was run %d)\n",s_count);
  if (s_count++ % 50 == 2)
    finalCheck();
#endif
  return deleted;
}

#ifdef KJS_DEBUG_MEM
void Collector::finalCheck()
{
  CollectorBlock *block = root;
  while (block) {
    ValueImp **r = block->mem;
    for (int i = 0; i < block->size; i++, r++) {
      if (*r ) {
        fprintf( stderr, "Collector::finalCheck() still having ValueImp %p (%s)  [marked:%d gcAllowed:%d created:%d refcount:%d]\n",
                 (void*)(*r), typeid( **r ).name(),
                 (bool)((*r)->_flags & ValueImp::VI_MARKED),
                 (bool)((*r)->_flags & ValueImp::VI_GCALLOWED),
                 (bool)((*r)->_flags & ValueImp::VI_CREATED),
                 (*r)->refcount);
      }
    }
    block = block->next;
  }
}
#endif