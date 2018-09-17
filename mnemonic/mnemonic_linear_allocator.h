#pragma once
#ifndef MNEMONIC_LINEAR_ALLOCATOR_H
#define MNEMONIC_LINEAR_ALLOCATOR_H

#ifndef MNEMONIC_EXPORTS_H
#include "mnemonic_exports.h"
#endif

#ifndef MNEMONIC_TYPES_H
#include "mnemonic_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


MNEMONIC_API linear_allocator* mnemonic_linear_create(uint size);

MNEMONIC_API void mnemonic_linear_destroy(linear_allocator* allocator);

MNEMONIC_API linear_allocator* mnemonic_linear_init(void* begin, uint size, byte flags);

MNEMONIC_API void* mnemonic_linear_alloc(linear_allocator* allocator, uint size, uint align, uint offset);

MNEMONIC_API void mnemonic_linear_free(linear_allocator* allocator, void* ptr);





MNEMONIC_API pool_allocator* mnemonic_pool_create(uint size, uint stride, uint align, uint offset);

MNEMONIC_API void mnemonic_pool_destroy(pool_allocator* allocator);

MNEMONIC_API pool_allocator* mnemonic_pool_init(void* begin, uint size, uint stride, uint align, uint offset, byte flags);

MNEMONIC_API void* mnemonic_pool_alloc(pool_allocator* allocator);

MNEMONIC_API void mnemonic_pool_free(pool_allocator* allocator, void* ptr);

#ifdef __cplusplus
}
#endif

#endif