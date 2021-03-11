
//memory utils
//author: zerrocxste

namespace memory_utils
{
#ifdef _WIN64
#define DWORD_OF_BITNESS DWORD64
#define PTRMAXVAL ((PVOID)0x000F000000000000)
#elif _WIN32
#define DWORD_OF_BITNESS DWORD
#define PTRMAXVAL ((PVOID)0xFFF00000)
#endif

	extern bool is_valid_ptr(PVOID ptr);

	extern HMODULE get_base();

	extern DWORD_OF_BITNESS get_base_address();

	extern DWORD_OF_BITNESS get_module_size(DWORD_OF_BITNESS address);

	extern DWORD_OF_BITNESS find_pattern(HMODULE module, const char* pattern, const char* mask);

	extern DWORD_OF_BITNESS find_pattern_in_heap(const char* pattern, const char* mask);

	template<class T>
	T read(std::vector<DWORD_OF_BITNESS>address)
	{
		size_t length_array = address.size() - 1;
		DWORD_OF_BITNESS relative_address = address[0];
		for (int i = 1; i < length_array + 1; i++)
		{
			if (is_valid_ptr((LPVOID)relative_address) == false)
				return T();

			if (i < length_array)
				relative_address = *(DWORD_OF_BITNESS*)(relative_address + address[i]);
			else
			{
				T readable_address = *(T*)(relative_address + address[length_array]);
				return readable_address;
			}
		}
	}

	template<class T>
	void write(std::vector<DWORD_OF_BITNESS>address, T value)
	{
		size_t length_array = address.size() - 1;
		DWORD_OF_BITNESS relative_address = address[0];
		for (int i = 1; i < length_array + 1; i++)
		{
			if (is_valid_ptr((LPVOID)relative_address) == false)
				return;

			if (i < length_array)
				relative_address = *(DWORD_OF_BITNESS*)(relative_address + address[i]);
			else
			{
				T* writable_address = (T*)(relative_address + address[length_array]);
				*writable_address = value;
			}
		}
	}

	extern char* read_string(std::vector<DWORD_OF_BITNESS> address);

	extern void write_string(std::vector<DWORD_OF_BITNESS> address, char* value);

	extern void patch_instruction(DWORD_OF_BITNESS instruction_address, const char* instruction_bytes, int sizeof_instruction_byte);
}