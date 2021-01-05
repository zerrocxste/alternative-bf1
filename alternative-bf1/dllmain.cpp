#include "includes.h"

namespace console
{
	FILE* output_stream = nullptr;

	void attach(const char *name)
	{
		if (AllocConsole())
		{
			freopen_s(&output_stream, "conout$", "w", stdout);
		}
		SetConsoleTitle(name);
	}

	void detach()
	{
		if (output_stream)
		{
			fclose(output_stream);
		}
		FreeConsole();
	}
}

void SetupHackThread(void)
{
	//allocate console
	console::attach("alternative bf1");

	auto* cLocalPlayer = GetLocalPlayer();

	std::cout << "hello " << cLocalPlayer->szName << "!" << std::endl;

	vars::load_default_settings();

	//setup / setup hook
	globals::hGame = FindWindow(NULL, "Battlefield™ 1");
	RAISE_ERROR(globals::hGame, "Error find window", "window handle: ")

	//hook func    
	if (MH_Initialize() != MH_OK)
	{
		MessageBoxA(NULL, "Error initialize minhook", "alternative hack", MB_OK | MB_ICONERROR);
	}

	m_pHook->SetupSetCursorPosHook();
	RAISE_ERROR(m_pHook->pSetCursorPosAddress, "Error hook SetCursorPos", "SetCursorPos: ")

	m_pHook->SetupDX11Hook();
	RAISE_ERROR(m_pHook->pPresentAddress, "Error hook DX11", "present: ")

	m_pHook->SetupWndProcHook();
	RAISE_ERROR(m_pHook->pWndProc, "Error hook wndproc", "wndproc: ")

	//main loop
	while (true)
	{
		if (globals::unload_dll) break;
		Sleep(228);
	}

	//unhook
	vars::bMenuOpen = false;
	Sleep(30);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Sleep(100);

	MH_DisableHook(m_pHook->pPresentAddress);
	MH_RemoveHook(m_pHook->pPresentAddress);
	Sleep(100);

	MH_DisableHook((PVOID)SetCursorPos);
	MH_RemoveHook((PVOID)SetCursorPos);
	Sleep(100);

	MH_Uninitialize();
	Sleep(100);

	SetWindowLongPtr(globals::hGame, GWLP_WNDPROC, (LONG_PTR)m_pHook->pWndProc);
	Sleep(100);

	//free library
	std::cout << "free library...\n\n";
	FreeLibraryAndExitThread(globals::hmModule, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	globals::hmModule = hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:    
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SetupHackThread, NULL, NULL, NULL);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

