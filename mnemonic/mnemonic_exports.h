#pragma once
#ifndef MNEMONIC_EXPORTS_H
#define MNEMONIC_EXPORTS_H

#ifdef _WINDLL
#define MNEMONIC_API __declspec(dllexport)
#else
#define MNEMONIC_API __declspec(dllimport)
#endif

#endif