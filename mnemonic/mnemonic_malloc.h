#pragma once
#ifndef MNEMONIC_MALLOC_H
#define MNEMONIC_MALLOC_H

#ifndef MNEMONIC_EXPORTS_H
#include "mnemonic_exports.h"
#endif

#ifndef MNEMONIC_TYPES_H
#include "mnemonic_types.h"
#endif

MNEMONIC_API void* mnemonic_malloc(const uint size);

#endif