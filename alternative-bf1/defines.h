#pragma once

#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / IM_PI) )
#define DEG2RAD( x ) ( (float)(x) * (float)(IM_PI / 180.f) )

#define RAISE_ERROR(check_var, error_message, success_message) if (!check_var) { MessageBoxA(NULL, error_message, "alternative hack", MB_OK | MB_ICONERROR); FreeLibraryAndExitThread(globals::hmModule, 1); } else { std::cout << success_message << "0x"<< (DWORD)check_var << std::endl; }