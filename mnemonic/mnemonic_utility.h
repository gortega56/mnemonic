#pragma once
#ifndef MNEMONIC_UTILITY
#define MNEMONIC_UTILITY

#ifndef MNEMONIC_EXPORTS_H
#include "mnemonic_exports.h"
#endif

#ifndef MNEMONIC_TYPES_H
#include "mnemonic_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

MNEMONIC_API uint mnemonic_platform_page_size();

MNEMONIC_API void* mnemonic_align_up(void* ptr, uint align);

MNEMONIC_API void* mnemonic_align_down(void* ptr, uint align);

MNEMONIC_API void* mnemonic_pad_align(void* ptr, uint align);

MNEMONIC_API void* mnemonic_raw(void* ptr);

#ifdef __cplusplus
}
#endif

#endif