
#include "override.h"

typedef struct {
	void*	addr;
	BYTE	hook [1 + sizeof(void*)];
	BYTE	orig [1 + sizeof(void*)];
	CRITICAL_SECTION lock;
} HookData;

#pragma warning(push)
#pragma warning(disable:4312)
static FARPROC  xatoi(LPCSTR s)
{
	DWORD p = 0;

	while (*s) {
		p <<= 4;

		if ('0' <= *s && *s <= '9') {
			p |= *s++ - '0';
		} else if ('A' <= *s && *s <= 'F') {
			p |= *s++ - 'A' + 10;
		} else if ('a' <= *s && *s <= 'f') {
			p |= *s++ - 'a' + 10;
		} else {
			break;
		}
	}

	return (FARPROC) p;
}
#pragma warning(pop)

static BOOL  _hook_init(LPCSTR dllname, LPCSTR funcname, HookData* p, FARPROC hook)
{
	p->addr = NULL;
	HMODULE module = LoadLibraryA(dllname);

	if (!module) {
		return FALSE;
	}

	FARPROC func;

	if (funcname[0] == '0' && funcname[1] == 'x') {
		func = xatoi(funcname + 2);
	} else {
		func = GetProcAddress(module, funcname);

		if (!func) {
			return FALSE;
		}
	}

	DWORD old;

	if (!VirtualProtect(func, sizeof(p->orig), PAGE_EXECUTE_READWRITE, &old)) {
		return FALSE;
	}

	p->addr = (void*) func;
	InitializeCriticalSection(&p->lock);
	memcpy(p->orig, p->addr, sizeof(p->orig));
	char* offset = (char*)((char*)hook - (char*)p->addr - sizeof(p->hook));
	p->hook[0] = 0xE9;
	memcpy(p->hook + 1, &offset, sizeof(offset));
	memcpy(p->addr, p->hook, sizeof(p->hook));
	return TRUE;
}


static void  _hook_term(HookData* p)
{
	if (p->addr) {
		EnterCriticalSection(&p->lock);
		memcpy(p->addr, p->orig, sizeof(p->orig));
		LeaveCriticalSection(&p->lock);
		DeleteCriticalSection(&p->lock);
		p->addr = NULL;
	}
}
#define HOOK_DEFINE(dll, rettype, name, argtype, vars) \
	static HookData  DATA_##name = { NULL, {0}, {0}, {0} }; \
	rettype WINAPI ORIG_##name argtype \
	{ \
		EnterCriticalSection(&DATA_##name.lock); \
		memcpy(DATA_##name.addr, DATA_##name.orig, sizeof(DATA_##name.orig)); \
		FlushInstructionCache((HANDLE)-1, DATA_##name.addr, sizeof(DATA_##name.orig)); \
		rettype v = ( (rettype (WINAPI*) argtype) DATA_##name.addr) vars; \
		memcpy(DATA_##name.addr, DATA_##name.hook, sizeof(DATA_##name.hook)); \
		FlushInstructionCache((HANDLE)-1, DATA_##name.addr, sizeof(DATA_##name.hook)); \
		LeaveCriticalSection(&DATA_##name.lock); \
		return v; \
	}
#include "hooklist.h"
#undef HOOK_DEFINE

#define HOOK_DEFINE(dll, rettype, name, argtype, vars) \
	_hook_init(#dll, #name, &DATA_##name, (FARPROC)IMPL_##name);
static void hook_init()
{
#include "hooklist.h"
}
#undef HOOK_DEFINE

#define HOOK_DEFINE(dll, rettype, name, argtype, vars) \
	_hook_term(&DATA_##name);
static void hook_term()
{
#include "hooklist.h"
}
#undef HOOK_DEFINE

BOOL WINAPI  DllMain(HINSTANCE instance, DWORD reason, LPVOID lp)
{
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls((HMODULE)instance);
		LoadSettings((HMODULE)instance);
		CacheInit();
		hook_init();
		WCHAR szFilename[MAX_PATH];
		GetModuleFileNameW(instance, szFilename, MAX_PATH);
		LoadLibraryW(szFilename);
	} else if (reason == DLL_PROCESS_DETACH) {
		hook_term();
		CacheTerm();
	}

	return TRUE;
}

#pragma comment(linker, "/export:GetMsgProc@12=GetMsgProc")
extern "C"
int CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(NULL, code, wParam, lParam);
}
