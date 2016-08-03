#ifndef _JE_NEW_H_
#define _JE_NEW_H_

#pragma warning( disable : 4290 )
#include <new>

#include "jemalloc\jemalloc.h"

void* operator new(size_t n) throw (std::bad_alloc);

void* operator new[](size_t n) throw (std::bad_alloc);


void operator delete(void* p) throw();

void operator delete[](void* p) throw();

#endif