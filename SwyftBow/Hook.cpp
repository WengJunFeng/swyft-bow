#include <WinSock2.h>
#include <iostream>
#include "Hook.h"
#include "StringUtils.h"

typedef int (WINAPI * pWSASend)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
pWSASend oWSASend = NULL;

void *DetourFunc(BYTE *src, const BYTE *dst, const int len)
{
	BYTE *jmp = (BYTE*)malloc(len + 5);
	DWORD dwback;

	DWORD garbage;
	VirtualProtect(jmp, len + 5, PAGE_EXECUTE_READWRITE, &garbage);
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwback);

	memcpy(jmp, src, len);    
	jmp += len;

	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;

	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;

	VirtualProtect(src, len, dwback, &dwback);

	return (jmp - len);
}

int WINAPI WSASend_hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int startLen = lpBuffers->len;
	std::string buffer(lpBuffers->buf);
	int ret = 0;

	if (StringUtils::ProcessMessage(&buffer))
	{
		lpBuffers->buf = _strdup(buffer.c_str());
		lpBuffers->len = strlen(lpBuffers->buf);
	}

	ret = oWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
	if (lpNumberOfBytesSent) *lpNumberOfBytesSent = startLen;
	return ret;
}

bool Hook::InitHook()
{
	char szProcessName[MAX_PATH];
	GetModuleFileNameA(NULL, szProcessName, MAX_PATH);
	
	if (strstr(szProcessName, "chrome.exe") > 0)
	{
		PBYTE apiAddress = (PBYTE)GetProcAddress(GetModuleHandle(L"ws2_32.dll"), "WSASend");
		oWSASend = (pWSASend)DetourFunc(apiAddress, (PBYTE)&WSASend_hook, 5);
		return true;
	}
	else if (strstr(szProcessName, "firefox.exe") > 0)
	{
		//TODO: Add a firefox hook
	}
	else if (strstr(szProcessName, "iexplorer.exe") > 0)
	{
		//TODO: Add an Internet Explorer hook
	}
	else
		MessageBoxA(0, "Process name not recognised or unsupported browser.", "Swagbow Error", MB_OK | MB_ICONERROR);

	return false;
}