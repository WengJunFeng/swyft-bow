#include <WinSock2.h>
#include <iostream>
#include "Hook.h"
#include "StringUtils.h"

typedef int (WINAPI * pWSASend)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);

typedef struct
{
	char* string;
	char* end;
} WSBUFFER, *PWSBUFFER;

pWSASend oWSASend = NULL;
DWORD SendFrameAddress = 0;
DWORD SendFrameReturn = 0;
PWSBUFFER WebsocketBuffer;

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

__declspec(naked) void SendFrameHook()
{
	__asm
	{
		mov eax, [edi + 0x04]
		sub eax, ecx
		mov [WebsocketBuffer], edi
	}

	//TODO: PROCESSING HERE

	__asm
	{
		jmp dword ptr ds : [SendFrameReturn]
	}
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
		//Flash hook (soon to be deprecated)
		PBYTE apiAddress = (PBYTE)GetProcAddress(GetModuleHandle(L"ws2_32.dll"), "WSASend");
		oWSASend = (pWSASend)DetourFunc(apiAddress, (PBYTE)&WSASend_hook, 5);
		
		//WebSockets SendFrame hook
		//SendFrameAddress AoB: 8B 47 04 2B C1 3B 4F 04 50 0F 44 CA 51 8D 8B ? ? 00 00 E8 ? ? ? ? 83 F8 02 0F 84 ? ? ? ? 83 F8 01 8B 45 C8 75 04
		SendFrameAddress = (DWORD)GetModuleHandle(L"chrome.dll") + 0x011C9202;
		SendFrameReturn = SendFrameAddress + 5;
		DetourFunc((PBYTE)SendFrameAddress, (PBYTE)&SendFrameHook, 5);
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