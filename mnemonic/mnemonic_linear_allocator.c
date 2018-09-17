#include "mnemonic_linear_allocator.h"
#include "mnemonic_utility.h"
#include "mnemonic_malloc.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

typedef struct linear_heap
{
    struct linear_heap* m_prev;
    byte* m_begin;
    size_t m_size;
} linear_heap;

typedef struct linear_allocator
{
    linear_heap* m_current;
    size_t m_offset;
    byte m_flags;
} linear_allocator;

static uint mnemonic_round_up(uint size);

static byte mnemonic_set_bounds_flag(byte flags);

uint mnemonic_round_up(uint size)
{
    uint pageSize = mnemonic_platform_page_size();
    return ((size + pageSize - 1) / pageSize) * pageSize;
}

byte mnemonic_set_bounds_flag(byte flags)
{
    return flags & 1;
}

linear_allocator* mnemonic_linear_create(uint size)
{
    // nonzero size indicates a bounded allocator
    // zero size indicates an unbounded allocator
    byte unbounded = (size == 0);
    if (unbounded)
    {
        size = mnemonic_platform_page_size();
    }

    size += sizeof(linear_allocator) + sizeof(linear_heap);

    void* ptr = mnemonic_malloc(size);

    return mnemonic_linear_init(ptr, size, unbounded);
}

void mnemonic_linear_destroy(linear_allocator* allocator)
{
    mnemonic_linear_free(allocator, NULL);
    int success = mnemonic_free(allocator);
    assert(success);
}

linear_allocator* mnemonic_linear_init(void* begin, uint size, byte flags)
{
    //------------------------------------------
    //| alloc | header | allocatable memory
    //------------------------------------------

    union
    {
        void* as_void;
        byte* as_byte;
    } ptr;

    ptr.as_void = begin;

    linear_allocator* alloc = (linear_allocator*)ptr.as_void;
    alloc->m_current = (linear_heap*)(ptr.as_byte + sizeof(linear_allocator));
    alloc->m_offset = 0;
    alloc->m_flags = flags;

    linear_heap* block = alloc->m_current;
    block->m_begin = ptr.as_byte + sizeof(linear_allocator) + sizeof(linear_heap);
    block->m_prev = NULL;
    block->m_size = size - (sizeof(linear_allocator) + sizeof(linear_heap));

    return alloc;
}

void* mnemonic_linear_alloc(linear_allocator* allocator, uint size, uint align, uint offset)
{
    union
    {
        void* as_void;
        byte* as_byte;
    } ptr;

    byte* current = allocator->m_current->m_begin + allocator->m_offset;

    ptr.as_void = current;
    ptr.as_byte = (byte*)mnemonic_align_up(ptr.as_byte + offset, align) - offset;

    uint alignment = (uint)(ptr.as_byte - current);
    allocator->m_offset += size + alignment + offset;

    if (ptr.as_byte > allocator->m_current->m_begin + allocator->m_current->m_size)
    {
        // Check flags and grow if necessary
        if (allocator->m_flags & 1)
        {
            uint requestSize = mnemonic_round_up(size);

            linear_heap* next = (linear_heap*)mnemonic_malloc(requestSize);
            next->m_begin = (byte*)next + sizeof(linear_heap);
            next->m_prev = allocator->m_current;
            next->m_size = requestSize - sizeof(linear_heap);

            allocator->m_current = next;
            allocator->m_offset = 0;

            ptr.as_void = mnemonic_linear_alloc(allocator, size, align, offset);
        }
        else
        {
            ptr.as_void = NULL;
        }
    }

    return ptr.as_void;
}

void mnemonic_linear_free(linear_allocator* allocator, void* ptr)
{
    while (allocator->m_current->m_prev != NULL)
    {
        linear_heap* current = allocator->m_current;
        allocator->m_current = current->m_prev;
        int success = mnemonic_free(current);
        assert(success);
    }
}

typedef struct pool_heap
{
    struct pool_heap* m_prev;
    uint m_size;
} pool_heap;

typedef struct pool_node
{
    struct pool_node* m_next;
} pool_node;

typedef struct pool_allocator
{
    pool_heap* m_current;
    pool_node* m_head;
    uint m_stride;
    byte m_flags;
    
} pool_allocator;

pool_allocator* mnemonic_pool_create(uint size, uint stride, uint align, uint offset)
{
    // nonzero size indicates a bounded allocator
    // zero size indicates an unbounded allocator
    byte unbounded = (size == 0);
    if (unbounded)
    {
        size = mnemonic_platform_page_size();
    }

    size += sizeof(pool_allocator) + sizeof(pool_heap);

    void* ptr = mnemonic_malloc(size);

    return mnemonic_pool_init(ptr, size, stride, align, offset, unbounded);
}

void mnemonic_pool_destroy(pool_allocator* allocator)
{
    while (allocator->m_current->m_prev != NULL)
    {
        pool_heap* heap = allocator->m_current;
        allocator->m_current = heap->m_prev;
        int success = mnemonic_free(heap);
        assert(success);
    }

    int success = mnemonic_free(allocator);
    assert(success);
}

pool_allocator* mnemonic_pool_init(void* begin, uint size, uint stride, uint align, uint offset, byte flags)
{
    union
    {
        void* as_void;
        byte* as_byte;
        pool_node* as_pool_node;
    } ptr;

    ptr.as_void = begin;

    pool_allocator* alloc = (pool_allocator*)ptr.as_void;
    alloc->m_stride = stride;
    alloc->m_flags = flags;

    ptr.as_byte += sizeof(pool_allocator);

    pool_heap* heap = (pool_heap*)ptr.as_void;
    heap->m_prev = NULL;
    heap->m_size = size - (sizeof(pool_allocator) + sizeof(pool_heap));
    alloc->m_current = heap;

    ptr.as_byte += sizeof(pool_heap);
    
    ptr.as_void = mnemonic_align_up(ptr.as_byte + offset, align);
    ptr.as_byte -= offset;

    alloc->m_head = ptr.as_pool_node;

    byte* end = ptr.as_byte + alloc->m_current->m_size;
    uint diff = (uint)(end - ptr.as_byte);
    uint count =  diff / stride;
    
    ptr.as_byte += stride;

    pool_node* iter = alloc->m_head;

    for (uint i = 1; i < count; ++i)
    {
        iter->m_next = ptr.as_pool_node;
        iter = iter->m_next;
        ptr.as_byte += stride;
    }

    return alloc;
}

void* mnemonic_pool_alloc(pool_allocator* allocator)
{    
    pool_node* node = allocator->m_head;

    if (node == NULL)
    {
        if (allocator->m_flags & 1)
        {
            uint size = allocator->m_current->m_size;

            pool_heap* heap = (pool_heap*)mnemonic_malloc(size);
            heap->m_prev = allocator->m_current;
            heap->m_size = size - sizeof(pool_heap);
            allocator->m_current = heap;

            byte* begin = (byte*)heap + sizeof(pool_heap);
            byte* end = begin + heap->m_size;

            uint diff = (uint)(end - begin);
            uint count =  diff / allocator->m_stride;

            allocator->m_head = (pool_node*)begin;

            union
            {
                void* as_void;
                byte* as_byte;
                pool_node* as_pool_node;
            } ptr;

            uintptr_t stride = (uintptr_t)allocator->m_stride;
            ptr.as_void = allocator->m_head;
            ptr.as_byte = ptr.as_byte + stride;

            pool_node* iter = allocator->m_head;

            for (uint i = 1; i < count; ++i)
            {
                iter->m_next = ptr.as_pool_node;
                iter = iter->m_next;
                ptr.as_byte = ptr.as_byte + stride;
            }

            node = allocator->m_head;
        }
        else
        {
            return NULL;
        }
    }

    allocator->m_head = node->m_next;
    return node;
}

void mnemonic_pool_free(pool_allocator* allocator, void* ptr)
{
    pool_node* head = (pool_node*)ptr;
    head->m_next = allocator->m_head;
    allocator->m_head = head;
}