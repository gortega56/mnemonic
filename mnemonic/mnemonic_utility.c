#include "mnemonic_utility.h"
#include <Windows.h>
#include <stdint.h>
#include <assert.h>

// https://stackoverflow.com/a/4840428

uint mnemonic_platform_page_size()
{
    LPSYSTEM_INFO systemInfo = 0;
    GetSystemInfo(systemInfo);
    return (uint)systemInfo->dwPageSize;
}

void* mnemonic_align_up(void* ptr, uint align)
{
    assert(align > 0 && (align & (align - 1)) == 0);
    assert(ptr != NULL);

    // Promote to pointer size
    uintptr_t a = align;
    uintptr_t raw = (uintptr_t)ptr;

    return (void*)((raw + (a - 1)) & (UINTPTR_MAX - a + 1));
}

void* mnemonic_align_down(void* ptr, uint align)
{
    assert(align > 0 && (align & (align - 1)) == 0);
    assert(ptr != NULL);

    // Promote to pointer size
    uintptr_t a = align;
    uintptr_t raw = (uintptr_t)ptr;

    return (void*)(raw & (UINTPTR_MAX - align + 1));
}

void* mnemonic_pad_align(void* ptr, uint align)
{
    // http://www.gameenginebook.com/
    // This aligns up by max align - 1 or pads by max 4

    // Create mask to round down.
    byte mask = (align - 1);
    byte* raw = (byte*)ptr;

    byte misalignment = (byte)raw & mask;
    byte adjustment = (align - misalignment);
    byte* aligned = raw + adjustment;

    // TODO: Overwrites previous allocations if adjustment = 0
    assert(adjustment < 256);
    //aligned[-1] = adjustment;

    return aligned;
}

void* mnemonic_raw(void* aligned)
{
    byte* pByte = (byte*)aligned;
	return pByte - pByte[-1];
}

