
#pragma once
#define _CRT_SECURE_NO_DEPRECATE 1
#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN 1
#define UNICODE  1
#define _UNICODE 1
#include <Windows.h>
#define for if(0);else for

void LoadSettings(HMODULE hModule);

void CacheInit();
void CacheTerm();
BOOL IsOSXPorLater();

#define HOOK_DEFINE(dll, rettype, name, argtype, vars) \
	rettype WINAPI ORIG_##name argtype; \
	rettype WINAPI IMPL_##name argtype;
#include "hooklist.h"
#undef HOOK_DEFINE
