//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#pragma once

#pragma warning(disable:4094) // untagged 'struct' declared no symbols
#pragma warning(disable:4172)

#include <atomic>
#include <algorithm>
#include <functional>
#include <string>
#include <future>
#include <locale>
#include <chrono>

#include <strsafe.h>

//#define ProfileCounterInc(x)			if(IsDebuggerPresent()) {};

#define Assert(Cond)					if(!(Cond)) XUtil::XAssert(__FILE__, __LINE__, #Cond);
#define AssertDebug(Cond)				if(!(Cond)) XUtil::XAssert(__FILE__, __LINE__, #Cond);
#define AssertMsgVa(Cond, Msg, ...)		if(!(Cond)) XUtil::XAssert(__FILE__, __LINE__, "%s\n\n" Msg, #Cond, ##__VA_ARGS__);
#define AssertMsg(Cond, Msg)			AssertMsgVa(Cond, Msg)
#define AssertMsgDebug(Cond, Msg)		AssertMsgVa(Cond, Msg)

#define _TIMING_START					XUtil::Timing.Begin()
#define _TIMING_END						XUtil::Timing.End(__FUNCTION__)
#define _TIMING_END_FMT(x, ...)			XUtil::Timing.End(__FUNCTION__, x, ##__VA_ARGS__)

#define RECTWIDTH(r)					(r.right - r.left)
#define RECTHEIGHT(r)					(r.bottom - r.top)

#define PatchIAT(detour, module, procname) Detours::IATHook(g_ModuleBase, (module), (procname), (uintptr_t)(detour));

#define templated(...)					__VA_ARGS__
#define AutoPtr(Type, Name, Offset)		static Type& Name = (*(Type *)((uintptr_t)GetModuleHandle(nullptr) + Offset))
#define AutoFunc(Type, Name, Offset)	static auto Name = ((Type)((uintptr_t)GetModuleHandle(nullptr) + Offset))

#define static_assert_offset(Structure, Member, Offset) struct __declspec(empty_bases) : CheckOffset<offsetof(Structure, Member), Offset> { }
#define assert_vtable_index(Function, Index) AssertMsgVa(VtableIndexUtil::GetIndexOf(Function) == Index, "Virtual table index does not match (%d != %d)", VtableIndexUtil::GetIndexOf(Function), Index)

#define GAME_TLS(Type, Offset) *(Type *)(*(uintptr_t *)(__readgsqword(0x58u) + 8i64 * (*(uint32_t *)(g_ModuleBase + 0x34BBA78))) + (Offset))

#define STATIC_CONSTRUCTOR(Id, Lambda) \
	struct \
	{ \
		static void Id() \
		{ \
			StaticConstructor<&Id>::C; \
			Lambda(); \
		} \
	};

#define DECLARE_CONSTRUCTOR_HOOK(Class) \
	static Class *__ctor__(void *Instance) \
	{ \
		return new (Instance) Class(); \
	} \
	\
	static Class *__dtor__(Class *Thisptr, unsigned __int8) \
	{ \
		Thisptr->~Class(); \
		return Thisptr; \
	}

class VtableIndexUtil {
private:
	typedef int(*VtableIndexFn)();
	static VtableIndexUtil* GlobalInstance;
public:
	static VtableIndexUtil* Instance();

	template<typename T>
	static int GetIndexOf(T ptr) {
		return (Instance()->**((decltype(&ForceVtableReference)*)(&ptr)))();
	}
private:
	virtual int ForceVtableReference();
};

template<void(*ctor)()>
struct StaticConstructor
{
	struct Constructor
	{
		Constructor()
		{
			ctor();
		}
	};

	static Constructor C;
};

template<void(*ctor)()>
typename StaticConstructor<ctor>::Constructor StaticConstructor<ctor>::C;

template <size_t Offset, size_t RequiredOffset>
struct __declspec(empty_bases)CheckOffset
{
	static_assert(Offset <= RequiredOffset, "Offset is larger than expected");
	static_assert(Offset >= RequiredOffset, "Offset is smaller than expected");
};

// thread-safe template versions of thisVirtualCall()

template<typename TR>
__forceinline TR thisVirtualCall(size_t reloff, const void* ths) {
	return (*(TR(__fastcall**)(const void*))(*(__int64*)ths + reloff))(ths);
}

template<typename TR, typename T1>
__forceinline TR thisVirtualCall(size_t reloff, const void* ths, T1 a1) {
	return (*(TR(__fastcall**)(const void*, T1))(*(__int64*)ths + reloff))(ths, a1);
}

template<typename TR, typename T1, typename T2>
__forceinline TR thisVirtualCall(size_t reloff, const void* ths, T1 a1, T2 a2) {
	return (*(TR(__fastcall**)(const void*, T1, T2))(*(__int64*)ths + reloff))(ths, a1, a2);
}

template<typename TR, typename T1, typename T2, typename T3>
__forceinline TR thisVirtualCall(size_t reloff, const void* ths, T1 a1, T2 a2, T3 a3) {
	return (*(TR(__fastcall**)(const void*, T1, T2, T3))(*(__int64*)ths + reloff))(ths, a1, a2, a3);
}

template<typename TR, typename T1, typename T2, typename T3, typename T4>
__forceinline TR thisVirtualCall(size_t reloff, const void* ths, T1 a1, T2 a2, T3 a3, T4 a4) {
	return (*(TR(__fastcall**)(const void*, T1, T2, T3, T4))(*(__int64*)ths + reloff))(ths, a1, a2, a3, a4);
}

namespace XUtil {

	class ITiming {
	public:
		typedef std::chrono::high_resolution_clock clock_t;
		typedef std::chrono::duration<double> msecond_t;
	private:
		clock_t::time_point time;
	private:
		msecond_t getTime(void);
	public:
		ITiming(void) = default;
	public:
		void Begin(void);
		void End(const char* funcname);
		void End(const char* funcname, const char* format, ...);
	};

	extern ITiming Timing;

	std::string GetAppPath(void);
	std::string GetDataPath(void);

	std::wstring GetAppPathUnicode(void);
	std::wstring GetDataPathUnicode(void);

	void SetThreadName(uint32_t ThreadID, const char* ThreadName);
	void Trim(char* Buffer, char C);
	void XAssert(const char* File, int Line, const char* Format, ...);
	uint64_t MurmurHash64A(const void* Key, size_t Len, uint64_t Seed = 0);

	uintptr_t FindPattern(uintptr_t StartAddress, uintptr_t MaxSize, const char* Mask);
	std::vector<uintptr_t> FindPatterns(uintptr_t StartAddress, uintptr_t MaxSize, const char* Mask);
	bool GetPESectionRange(uintptr_t ModuleBase, const char* Section, uintptr_t* Start, uintptr_t* End);
	void PatchMemory(uintptr_t Address, uint8_t* Data, size_t Size);
	void PatchMemory(uintptr_t Address, std::initializer_list<uint8_t> Data);
	void PatchMemoryNop(uintptr_t Address, size_t Size);
	void PatchMemoryWP(uintptr_t Address, uint8_t* Data, size_t Size);
	void PatchMemoryWP(uintptr_t Address, std::initializer_list<uint8_t> Data);
	void PatchMemoryNopWP(uintptr_t Address, size_t Size);
	void UnlockWP(uintptr_t Address, size_t Size);
	void LockWP(uintptr_t Address, size_t Size);
	void DetourJump(uintptr_t Target, uintptr_t Destination);
	void DetourCall(uintptr_t Target, uintptr_t Destination);

	template<typename T>
	__forceinline void DetourJump(uintptr_t Target, T Destination) {
		static_assert(std::is_member_function_pointer_v<T> || (std::is_pointer_v<T> && std::is_function_v<typename std::remove_pointer<T>::type>));
		DetourJump(Target, *(uintptr_t*)&Destination);
	}

	template<typename T>
	__forceinline void DetourCall(uintptr_t Target, T Destination) {
		static_assert(std::is_member_function_pointer_v<T> || (std::is_pointer_v<T> && std::is_function_v<typename std::remove_pointer<T>::type>));
		DetourCall(Target, *(uintptr_t*)&Destination);
	}

	template <typename T>
	__forceinline void __stdcall DetourClassCall(uintptr_t target, T destination){
		Detours::X64::DetourFunctionClass(target, destination, Detours::X64Option::USE_REL32_CALL);
	}

	template <typename T>
	__forceinline void __stdcall DetourClassJump(uintptr_t target, T destination) {
		Detours::X64::DetourFunctionClass(target, destination, Detours::X64Option::USE_REL32_JUMP);
	}

	namespace Parallel
	{
		// https://stackoverflow.com/questions/40805197/parallel-for-each-more-than-two-times-slower-than-stdfor-each
		// fast than tbb::parallel_for_each

		class join_threads
		{
		public:
			explicit join_threads(std::vector<std::thread>& threads)
				: threads_(threads) {}

			~join_threads()
			{
				for (size_t i = 0; i < threads_.size(); ++i)
				{
					if (threads_[i].joinable())
					{
						threads_[i].join();
					}
				}
			}

		private:
			std::vector<std::thread>& threads_;
		};

		template<typename Iterator, typename Func>
		void for_each(Iterator first, Iterator last, Func func)
		{
			const auto length = std::distance(first, last);
			if (0 == length) return;

			const auto min_per_thread = 25u;
			const unsigned max_threads = (length + min_per_thread - 1) / min_per_thread;
			const auto hardware_threads = std::thread::hardware_concurrency();
			const auto num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2u, max_threads);
			const auto block_size = length / num_threads;

			std::vector<std::future<void>> futures(num_threads - 1);
			std::vector<std::thread> threads(num_threads - 1);
			join_threads joiner(threads);

			auto block_start = first;
			for (unsigned i = 0; i < num_threads - 1; ++i)
			{
				auto block_end = block_start;
				std::advance(block_end, block_size);
				std::packaged_task<void(void)> task([block_start, block_end, func]()
					{
						std::for_each(block_start, block_end, func);
					});
				futures[i] = task.get_future();
				threads[i] = std::thread(std::move(task));
				block_start = block_end;
			}

			std::for_each(block_start, last, func);

			for (size_t i = 0; i < num_threads - 1; ++i)
				futures[i].get();
		}
	}

	namespace Str
	{
		static char* whitespaceDelimiters = " \t\n\r\f\v";

		// https://thispointer.com/implementing-a-case-insensitive-stringfind-in-c/

		/*
		 * Find Case Insensitive Sub String in a given substring
		 */

		size_t findCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0);

		// dirnameOf https://stackoverflow.com/questions/8518743/get-directory-from-file-path-c/34740989
		static inline std::string dirnameOf(const std::string& fname) {
			size_t pos = fname.find_last_of("\\/");
			return (std::string::npos == pos)
				? ""
				: fname.substr(0, pos);
		}

		// dirnameOf https://stackoverflow.com/questions/8518743/get-directory-from-file-path-c/34740989
		static inline std::wstring dirnameOf(const std::wstring& fname) {
			size_t pos = fname.find_last_of(L"\\/");
			return (std::wstring::npos == pos)
				? L""
				: fname.substr(0, pos);
		}

		// convert string to upper case
		static inline std::string& UpperCase(std::string& s) {
			std::for_each(s.begin(), s.end(), [](char& c) {
				c = ::toupper(c);
				});
			return s;
		}

		// convert string to upper case
		static inline std::string& LowerCase(std::string& s) {
			std::for_each(s.begin(), s.end(), [](char& c) {
				c = ::tolower(c);
				});
			return s;
		}

		// convert string to upper case
		static inline std::wstring& UpperCaseW(std::wstring& s) {
			std::for_each(s.begin(), s.end(), [](wchar_t& c) {
				c = ::toupper(c);
				});
			return s;
		}

		// convert string to upper case
		static inline std::wstring& LowerCaseW(std::wstring& s) {
			std::for_each(s.begin(), s.end(), [](wchar_t& c) {
				c = ::tolower(c);
				});
			return s;
		}

		// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
		std::string __stdcall GetLastErrorToStr(DWORD err, const std::string& namefunc);
		std::string __stdcall GetLastErrorToStr(const std::string& namefunc);

		static inline std::string ChangeFileExt(const std::string sfile, const std::string snewext) {
			if (auto it = sfile.find_last_of('.'); it != std::string::npos)
				return sfile.substr(0, it) + snewext;
			else
				return sfile + snewext;
		}

		inline std::string& trim(std::string& str) {

			str.erase(str.find_last_not_of(whitespaceDelimiters) + 1);
			str.erase(0, str.find_first_not_of(whitespaceDelimiters));

			return str;
		}

		inline std::string trim(const char* s) {
			std::string str(s);
			return trim(str);
		}

		void replaceAll(std::string& source, const std::string& from, const std::string& to);
	}
}