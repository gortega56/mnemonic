#include "mnemonic_malloc.h"
#include <Windows.h>

static void* mnemonic_platform_malloc(uint size);

static void* mnemonic_platform_reserve(uint size);

static void* mnemonic_platform_commit(void* ptr, uint size);

static int mnemonic_platform_free(void* ptr);

void* mnemonic_platform_reserve(uint size)
{
    return (void*)VirtualAlloc(0, size, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

void* mnemonic_platform_commit(void* ptr, uint size)
{
    return (void*)VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

int mnemonic_platform_free(void* ptr)
{
    return VirtualFree(ptr, 0, MEM_RELEASE);
}

void* mnemonic_platform_malloc(uint size)
{
    return (void*)VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

void* mnemonic_malloc(const uint size)
{
    return mnemonic_platform_malloc(size);
}

int mnemonic_free(void* ptr)
{
    return mnemonic_platform_free(ptr);
}