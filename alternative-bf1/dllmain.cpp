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

#define RAISE_ERROR(check_var, error_message, success_message) \
if (!check_var) \
{ \
	MessageBoxA(NULL, error_message, "alternative hack", MB_OK | MB_ICONERROR); \
	FreeLibraryAndExitThread(globals::hmModule, 1); \
} \
else \
{ \
	std::cout << success_message << "0x" << std::hex << check_var << std::endl; \
} \

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	auto process_id_that_interests_us_very_much = GetCurrentProcessId();

	HWND* cur_hwnd = (HWND*)lParam;

	if ((!GetWindow(hwnd, GW_OWNER)) && IsWindow(hwnd))
	{
		DWORD process_id = NULL;
		GetWindowThreadProcessId(hwnd, &process_id);

		char* text_window = new char[255];

		GetWindowText(hwnd, text_window, 255);

		if (process_id_that_interests_us_very_much == process_id && strstr(text_window, "Battlefield") && !strstr(text_window, ".exe"))
		{
			std::cout << "Window: " << text_window << std::endl;
			*cur_hwnd = hwnd;
			return 0;
		}
	}

	return 1;
}

void SetupHackThread(void)
{
	//allocate console
	console::attach("alternative bf1");

	auto* cLocalPlayer = GetLocalPlayer();

	if (!IsValidPtr(cLocalPlayer) || cLocalPlayer->szName == NULL)
	{
		std::cout << __FUNCTION__ << " Error\n";
		FreeLibraryAndExitThread(globals::hmModule, 1);
	}

	std::cout << "hello " << cLocalPlayer->szName << "!" << std::endl;

	vars::load_default_settings();

	//setup / setup hook

	EnumWindows(&EnumWindowsProc, (LPARAM)&globals::hGame);

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
	RAISE_ERROR(m_pHook->pResizeBuffersAddress, "Error hook DX11", "resizebuffers: ")

	m_pHook->SetupWndProcHook();
	RAISE_ERROR(m_pHook->pWndProc, "Error hook wndproc", "wndproc: ")

	//main loop
	while (true)
	{
		if (globals::unload_dll) break;
		Sleep(228);
	}

	//disable hacks
	if (vars::aimbot::no_recoil)
		m_pFeatures->NoRecoil(false), Sleep(10);

	if (vars::aimbot::increase_fire_rate)
		m_pFeatures->InrecreaseFireRate(false), Sleep(10);

	if (vars::aimbot::weapon_no_overheating)
		m_pFeatures->NoOverheatingWeapon(false), Sleep(10);

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

	MH_DisableHook(m_pHook->pResizeBuffersAddress);
	MH_RemoveHook(m_pHook->pResizeBuffersAddress);

	MH_DisableHook((PVOID)SetCursorPos);
	MH_RemoveHook((PVOID)SetCursorPos);
	Sleep(100);

	render_view->Release();
	render_view = nullptr;
	Sleep(10);

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

