#ifndef _ALLOCTOR_H_
#define _ALLOCTOR_H_
#include <sys/types.h>

void* operator new(size_t size) __attribute__((__externally_visible__));
void operator delete(void* p) noexcept(true);
void* operator new[](size_t size) __attribute__((__externally_visible__));
void operator delete[](void* p) noexcept(true);
void* mem_alloc(size_t size);
void mem_free(void* p);
#endif // !_ALLOCTOR_H_
