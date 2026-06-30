#include <Windows.h>

#include <stdio.h>
#pragma warning(push)
#pragma warning(disable: 4152)
#pragma warning(pop)


BOOL FileExistW(wchar_t* Filename)
{
	DWORD FileAttributes = GetFileAttributesW(Filename);
	return (FileAttributes != INVALID_FILE_ATTRIBUTES);
}




//Usage: > RemoteDllInjector.exe <ProcessID> <PathToDLL>
int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	UNREFERENCED_PARAMETER(envp);
	wchar_t* UsageString = L"Usage: > RemoteDllInjector.exe <ProcessID> <PathToDLL>\n";
	DWORD ProcessID = 0;
	wchar_t* Dllpath = NULL;
	HANDLE RemoteProcessH = 0;
	HANDLE Kernel32ModuleHandle = 0;
	void* LoadLibraryAddr = NULL;

	if (argc != 3){
		wprintf(L"%s", UsageString);
		return(0);

	}

	if ((ProcessID = _wtoi(argv[1])) == 0)
	{
		wprintf(L"Cannot convert ProcessID!\n");
		wprintf(L"%s", UsageString);
		return(0);
	}

	Dllpath = argv[2];

	if (FileExistW(Dllpath) == FALSE)
	{
		wprintf(L"Cannot locate DLL!\n");
		wprintf(L"%s", UsageString);
		return(0);

	}


	RemoteProcessH = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
	if (RemoteProcessH)
	{
		
	}
	else
	{
		HANDLE HandleForProcessToken = NULL;
		LUID luid = { 0 };
		TOKEN_PRIVILEGES TokenPrivileges = { 0 };
		HandleForProcessToken = GetCurrentProcessToken();
		LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &luid);
		TokenPrivileges.PrivilegeCount = 1;
		TokenPrivileges.Privileges[0].Luid = luid;
		TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &HandleForProcessToken))
		{
			wprintf(L"Failed to open process token\n");
		}
		else
		{
			if (AdjustTokenPrivileges(HandleForProcessToken,
				FALSE,
				&TokenPrivileges,
				0,
				(PTOKEN_PRIVILEGES)NULL,
				(PDWORD)NULL) == 0)
			{
				wprintf(L"Failed to adjust process token\n");
				return(0);
			}
			wprintf(L"executed\n");

		}

	}

	RemoteProcessH = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

	if (RemoteProcessH == NULL)
	{
		wprintf(L"Cannot open process\n");
		return(0);
	}

	LPVOID remoteMem = VirtualAllocEx(RemoteProcessH, NULL, (wcslen(Dllpath) + 1) * sizeof(wchar_t),MEM_COMMIT, PAGE_READWRITE);
	if (remoteMem == NULL)
	{
		wprintf(L"Failed to allocate memory\n");
		CloseHandle(RemoteProcessH);
		return(0);
	}


	if (WriteProcessMemory(RemoteProcessH, remoteMem, Dllpath, (wcslen(Dllpath) + 1) * sizeof(wchar_t), NULL) == FALSE)
	{
		wprintf(L"Failed to write dll into process memory.\n");
		VirtualFreeEx(RemoteProcessH, remoteMem, 0, MEM_RELEASE);
		CloseHandle(RemoteProcessH);
		return(0);
	}

	Kernel32ModuleHandle = GetModuleHandle(L"kernel32.dll");
	LoadLibraryAddr = GetProcAddress(Kernel32ModuleHandle, "LoadLibraryW");
	if (LoadLibraryAddr == NULL)
	{
		wprintf(L"Failed to load kernel32.dll\n");
		VirtualFreeEx(RemoteProcessH, remoteMem, 0, MEM_RELEASE);
		CloseHandle(RemoteProcessH);
		return(0);
	}

	HANDLE hThread = CreateRemoteThread(
		RemoteProcessH,
		NULL, 0,
		(LPTHREAD_START_ROUTINE)LoadLibraryAddr,remoteMem, 0, NULL);
	if (hThread == NULL)
	{
		wprintf(L"Failed to CreateRemoteThread\n");
		VirtualFreeEx(RemoteProcessH, remoteMem, 0, MEM_RELEASE);
		CloseHandle(RemoteProcessH);
		return(0);
	}


	wprintf(L"Successfully inject dll into PID %ld\n", ProcessID);


	return(0);
}