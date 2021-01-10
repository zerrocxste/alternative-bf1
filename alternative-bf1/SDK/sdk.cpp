#include "../includes.h"
#include "sdk.hpp"

bool ClientSoldierEntity::GetBonePos(int BoneId, Vector& vOut)
{
	BoneCollisionComponent* pBoneCollisionComponent = this->bonecollisioncomponent;

	if (!IsValidPtr(pBoneCollisionComponent))
		return false;

	QuatTransform* pQuat = pBoneCollisionComponent->m_ragdollTransforms.m_ActiveWorldTransforms;

	if (!IsValidPtr(pQuat))
		return false;

	vOut.x = pQuat[BoneId].m_TransAndScale.x;
	vOut.y = pQuat[BoneId].m_TransAndScale.y;
	vOut.z = pQuat[BoneId].m_TransAndScale.z;

	return true;
}

Vector Mat3Vector(Matrix4x4 mat, Vector vec)
{
	return Vector
	(
		mat._11 * vec.x + mat._12 * vec.x + mat._13 * vec.x,
		mat._21 * vec.y + mat._22 * vec.y + mat._23 * vec.y,
		mat._31 * vec.z + mat._32 * vec.z + mat._33 * vec.z
	);
}

namespace memory_utils
{
	#ifdef _WIN64
		#define DWORD_OF_BITNESS DWORD64
		#define PTRMAXVAL ((PVOID)0x000F000000000000)
	#elif _WIN32
		#define DWORD_OF_BITNESS DWORD
		#define PTRMAXVAL ((PVOID)0xFFF00000)
	#endif

	bool is_valid_ptr(PVOID ptr)
	{
		return (ptr >= (PVOID)0x10000) && (ptr < PTRMAXVAL) && ptr != nullptr && !IsBadReadPtr(ptr, sizeof(ptr));
	}

	DWORD_OF_BITNESS base;
	DWORD_OF_BITNESS get_base()
	{
		if (!base)
		{
			base = (DWORD_OF_BITNESS)GetModuleHandle(0);
			return base;
		}
		else
		{
			return base;
		}
	}

	template<class T>
	void write(std::vector<DWORD_OF_BITNESS>address, T value)
	{
		size_t lengh_array = address.size() - 1;
		DWORD_OF_BITNESS relative_address;
		relative_address = address[0];
		for (int i = 1; i < lengh_array + 1; i++)
		{
			if (is_valid_ptr((LPVOID)relative_address) == false)
				return;

			if (i < lengh_array)
				relative_address = *(DWORD_OF_BITNESS*)(relative_address + address[i]);
			else
			{
				T* writable_address = (T*)(relative_address + address[lengh_array]);
				*writable_address = value;
			}
		}
	}

	template<class T>
	T read(std::vector<DWORD_OF_BITNESS>address)
	{
		size_t lengh_array = address.size() - 1;
		DWORD_OF_BITNESS relative_address;
		relative_address = address[0];
		for (int i = 1; i < lengh_array + 1; i++)
		{
			if (is_valid_ptr((LPVOID)relative_address) == false)
				return 0;

			if (i < lengh_array)
				relative_address = *(DWORD_OF_BITNESS*)(relative_address + address[i]);
			else
			{
				T readable_address = *(T*)(relative_address + address[lengh_array]);
				return readable_address;
			}
		}
	}

	DWORD_OF_BITNESS get_module_size(DWORD_OF_BITNESS address)
	{
		return PIMAGE_NT_HEADERS(address + (DWORD_OF_BITNESS)PIMAGE_DOS_HEADER(address)->e_lfanew)->OptionalHeader.SizeOfImage;
	}

	DWORD_OF_BITNESS find_pattern(HMODULE module, const char* pattern, const char* mask)
	{
		DWORD_OF_BITNESS base = (DWORD_OF_BITNESS)module;
		DWORD_OF_BITNESS size = get_module_size(base);

		DWORD_OF_BITNESS patternLength = (DWORD_OF_BITNESS)strlen(mask);

		for (DWORD_OF_BITNESS i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (DWORD_OF_BITNESS j = 0; j < patternLength; j++)
			{
				found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
			}

			if (found)
			{
				return base + i;
			}
		}

		return NULL;
	}

	void patch_instruction(DWORD_OF_BITNESS instruction_address, const char* instruction_bytes, int sizeof_instruction_byte)
	{
		DWORD dwOldProtection;

		VirtualProtect((LPVOID)instruction_address, sizeof_instruction_byte, PAGE_EXECUTE_READWRITE, &dwOldProtection);

		memcpy((LPVOID)instruction_address, instruction_bytes, sizeof_instruction_byte);

		VirtualProtect((LPVOID)instruction_address, sizeof_instruction_byte, dwOldProtection, NULL);

		FlushInstructionCache(GetCurrentProcess(), (LPVOID)instruction_address, sizeof_instruction_byte);
	}
}

float get_recoil()
{
	return memory_utils::read<float>( { memory_utils::get_base(), 0x0351C150, 0x20, 0x3A8, 0x18, 0x138, 0x100, 0x0, 0x74 } );
}