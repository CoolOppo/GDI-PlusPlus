
#define _CRT_SECURE_NO_DEPRECATE 1
#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN 1
#define UNICODE  1
#define _UNICODE 1
#include <Windows.h>
#include <ShellApi.h>
#include <ComDef.h>
#include <ShlObj.h>
#include <wchar.h>
#include <stdarg.h>

#define for if(0);else for

#pragma comment(linker, "/subsystem:windows")
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")


static void errmsg(LPCWSTR format, ...)
{
	WCHAR  buffer [1024];
	va_list val;
	va_start(val, format);
	_vsnwprintf(buffer, 1024, format, val);
	va_end(val);
	MessageBox(NULL, buffer, L"gdi++ ERROR", MB_OK | MB_ICONSTOP);
}

static bool inject_dll(HANDLE process, LPVOID entrypoint)
{
	WCHAR dllpath [MAX_PATH];
	UINT len = GetModuleFileName(NULL, dllpath, MAX_PATH);
	wcscpy(dllpath + len - 4, L".dll");
	BYTE opcode1 [8] = {
		0xB8, 0, 0, 0, 0,
		0xFF, 0xE0,
		0xCC,
	};
	BYTE  opcode2 [32 + 16 + sizeof(dllpath)] = {
		0x8B, 0x48, 0x24,
		0x8B, 0x50, 0x28,
		0x89, 0x11,
		0x8B, 0x50, 0x2C,
		0x89, 0x51, 0x04,
		0x51,
		0x8B, 0xD0,
		0x83, 0xC2, 0x30,
		0x52,
		0xFF, 0x50, 0x20,
		0x59,
		0xFF, 0xE1,
		0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	};
	HMODULE kernel32 = GetModuleHandle(L"Kernel32.dll");
	LPVOID  loadlibrary = GetProcAddress(kernel32, "LoadLibraryW");
	DWORD   n;
	memcpy(opcode2 + 32, &loadlibrary, sizeof(LPVOID));
	memcpy(opcode2 + 36, &entrypoint,  sizeof(LPVOID));

	if (!ReadProcessMemory(process, entrypoint, opcode2 + 40, sizeof(opcode1), &n)) {
		return false;
	}

	wcscpy((LPWSTR)(opcode2 + 48), dllpath);
	LPVOID mem = VirtualAllocEx(process, NULL, sizeof(opcode2), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!mem) {
		return false;
	}

	if (!WriteProcessMemory(process, mem, opcode2, sizeof(opcode2), &n)) {
		return false;
	}
	memcpy(opcode1 + 1, &mem, sizeof(LPVOID));

	if (!VirtualProtectEx(process, entrypoint, sizeof(opcode1), PAGE_EXECUTE_READWRITE, &n)) {
		return false;
	}

	if (!WriteProcessMemory(process, entrypoint, opcode1, sizeof(opcode1), &n)) {
		return false;
	}

	return true;
}


static LPVOID get_entrypoint(LPCWSTR path)
{
	LPVOID entrypoint = NULL;
	DWORD len;
	IMAGE_DOS_HEADER dos;
	IMAGE_NT_HEADERS pe;
	HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (file == INVALID_HANDLE_VALUE) {
		errmsg(L"This error message was lost in translation.\n%s", path);
	} else if (!ReadFile(file, &dos, sizeof(dos), &len, NULL) ||
						 dos.e_magic != IMAGE_DOS_SIGNATURE ||
						 !SetFilePointer(file, dos.e_lfanew, NULL, FILE_BEGIN) ||
						 !ReadFile(file, &pe, sizeof(pe), &len, NULL) ||
						 pe.Signature != IMAGE_NT_SIGNATURE) {
		errmsg(L"This error message was lost in translation.\n%s", path);
	} else {
#pragma warning(push)
#pragma warning(disable:4312)
		entrypoint = (LPVOID)(pe.OptionalHeader.ImageBase + pe.OptionalHeader.AddressOfEntryPoint);
#pragma warning(pop)
	}

	if (file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
	}

	return entrypoint;
}



static size_t  wcscpy_arg(LPWSTR buffer, size_t size, LPCWSTR str)
{
	LPCWSTR c = str;

	for (; *c > L' ' ; c++);

	return _snwprintf(buffer, size, (!*c) ? L"%s " : L"\"%s\" ", str);
}
static LPWSTR  get_commandline(LPWSTR o_app, LPWSTR o_workdir)
{
	o_app[0] = o_workdir[0] = L'\0';

	if (GetVersion() & 0x80000000) {
		errmsg(L"Win9xでは動作しません.");
		return NULL;
	}

	int     argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);

	if (!argv) {
		return NULL;
	}

	if (argc <= 1) {
		MessageBox(NULL,
							 L"gdi++.exe <file> argument ...\n"
							 L"\n"
							 L"EXE, またはショートカットなどを ドロップしてください."
							 , L"gdi++", MB_OK | MB_ICONINFORMATION);
		return NULL;
	}

	LPWSTR o_cmdline = NULL;
	LPWSTR file = argv[1];
	WCHAR  fullpath [MAX_PATH];
	WCHAR  linkpath [MAX_PATH];
	WCHAR  linkarg [MAX_PATH];
	linkarg[0] = L'\0';
	size_t len = (int) wcslen(file);

	if (len >= 4 && _wcsicmp(file + len - 4, L".lnk") == 0) {
		bool result = true;
		IShellLink* shlink = NULL;
		IPersistFile* persist = NULL;

		if (S_OK != CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&shlink) ||
				S_OK != shlink->QueryInterface(IID_IPersistFile, (void**)&persist) ||
				S_OK != persist->Load(file, STGM_READ) ||
				S_OK != shlink->Resolve(NULL, SLR_UPDATE) ||
				S_OK != shlink->GetPath(linkpath, MAX_PATH, NULL, 0) ||
				S_OK != shlink->GetArguments(linkarg, MAX_PATH)) {
			errmsg(L"This error message was lost in translation.\n%s", argv[1]);
			result = false;
		} else {
			shlink->GetWorkingDirectory(o_workdir, MAX_PATH);
			file = linkpath;
		}

		if (persist) {
			persist->Release();
		}

		if (shlink) {
			shlink->Release();
		}

		if (!result) {
			return NULL;
		}
	}

	if (FindExecutable(file, NULL, o_app) < (HINSTANCE)32) {
		errmsg(L"実行ファイルが見つからない\n%s", file);
		return NULL;
	}

	DWORD filetype = GetLastError();

	if (filetype == ERROR_ALREADY_EXISTS) {
		filetype = 0;
	}

	if (filetype == 0) {
		WCHAR tmp [MAX_PATH];

		if (GetFullPathName(file, MAX_PATH, tmp, NULL) &&
				GetShortPathName(tmp, fullpath, MAX_PATH)) {
			file = fullpath;
		}
	}
	len = wcslen(o_app) + 32;

	if (filetype == 0) {
		len += wcslen(file) + 1;
	}

	len += wcslen(linkarg) + 1;

	for (int i = 2 ; i < argc ; i++) {
		len += wcslen(argv[i]) + 3;
	}
	o_cmdline = new WCHAR [len];
	size_t n = wcscpy_arg(o_cmdline, len, o_app);

	if (filetype == 0) {
		n += wcscpy_arg(o_cmdline + n, len - n, file);
	}

	if (linkarg[0]) {
		n += wcscpy_arg(o_cmdline + n, len - n, linkarg);
	}

	for (int i = 2 ; i < argc ; i++) {
		n += wcscpy_arg(o_cmdline + n, len - n, argv[i]);
	}
	if (!o_workdir[0]) {
		LPWSTR c1 = wcsrchr(file, L'\\');
		LPWSTR c2 = wcsrchr(file, L'/');

		if (c1 < c2) {
			c1 = c2;
		}

		if (c1) {
			*c1 = L'\0';
			GetFullPathName(file, MAX_PATH, o_workdir, NULL);
		}
	}

	if (!(GetFileAttributes(o_workdir) & FILE_ATTRIBUTE_DIRECTORY)) {
		o_workdir[0] = L'\0';
	}
	LocalFree(argv);
	return o_cmdline;
}


int WINAPI wWinMain(HINSTANCE ins, HINSTANCE prev, LPWSTR cmd, int show)
{
	CoInitialize(NULL);
	WCHAR app [MAX_PATH];
	WCHAR workdir [MAX_PATH];
	LPWSTR cmdline = get_commandline(app, workdir);
	LPVOID entrypoint = NULL;

	if (cmdline) {
		entrypoint = get_entrypoint(app);
	}

	if (entrypoint) {
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);

		if (!CreateProcess(app, cmdline, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, NULL, workdir[0] ? workdir : NULL, &si, &pi)) {
			errmsg(L"This error message was lost in translation.\n%s", cmdline);
		} else if (!inject_dll(pi.hProcess, entrypoint)) {
			TerminateProcess(pi.hProcess, 0);
			errmsg(L"This error message was lost in translation.\n%s", cmdline);
		} else {
			ResumeThread(pi.hThread);
		}

		if (pi.hThread) {
			CloseHandle(pi.hThread);
		}

		if (pi.hProcess) {
			CloseHandle(pi.hProcess);
		}
	}

	if (cmdline) {
		delete [] cmdline;
	}

	CoUninitialize();
	return 0;
}
