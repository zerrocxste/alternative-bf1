#pragma warning (disable: 4005)
#pragma warning (disable: 26812)
#pragma warning (disable: 4244)

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <deque>
#include <array>
#include <vector>
#include <time.h>

#include "globals.h"
#include "vars/vars.h"

#include "minhook/minhook.h"
#pragma comment (lib, "minhook/minhook.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11")

#include "memory_utils/memory_utils.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include "math/qangle/qangle.h"
#include "math/vector/vector.h"
#include "math/martix4x4/matrix4x4.h"

#include "SDK/FrostBite.hpp"
#include "SDK/sdk.hpp"

#include "menu/fonts.h"
#include "menu/menu.h"
#include "hook/hook.h"
#include "draw-list/draw-list.h"
#include "features/features.h"

