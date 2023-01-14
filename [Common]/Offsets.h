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

#include <array>

#define OFFSET(RelOffset, Version) (g_ModuleBase + RelOffset)
#define OFFSET_ENTRY_KEY(RelOffset, Version) (((uint64_t)(Version) << 32) | (RelOffset))
#define OFFSET_ENTRY(RelOffset, Version, Signature, SigAdjustment, TranslatedOffset) { RelOffset, Version, Signature, SigAdjustment, (uint32_t)(TranslatedOffset) },

namespace Offsets {
	struct OffsetEntry {
		uint32_t RelOffset;
		uint32_t Version;
		const char *Signature;
		int SigAdjustment;
		uint32_t TranslatedOffset;
	};

	uintptr_t Resolve(uint32_t RelOffset, uint32_t Version);
	bool CanResolve(uint32_t RelOffset, uint32_t Version);

	void BuildTableForCKF4Version(uint32_t Version);
	void BuildTableForGameVersion(uint32_t Version);
	void BuildTable(const OffsetEntry *Table, size_t Count, bool CurrentVersion);
	void ValidateTable(const OffsetEntry *Table, size_t Count);
	void DumpLoadedTable(const char *FilePath);
}

// thread-safe template versions of fastCall()

template<typename TR>
__forceinline TR fastCall(size_t reloff) {
	return ((TR(__fastcall*)())OFFSET(reloff, 0))();
}

template<typename TR, typename T1>
__forceinline TR fastCall(size_t reloff, T1 a1) {
	return ((TR(__fastcall*)(T1))OFFSET(reloff, 0))(a1);
}

template<typename TR, typename T1, typename T2>
__forceinline TR fastCall(size_t reloff, T1 a1, T2 a2) {
	return ((TR(__fastcall*)(T1, T2))OFFSET(reloff, 0))(a1, a2);
}

template<typename TR, typename T1, typename T2, typename T3>
__forceinline TR fastCall(size_t reloff, T1 a1, T2 a2, T3 a3) {
	return ((TR(__fastcall*)(T1, T2, T3))OFFSET(reloff, 0))(a1, a2, a3);
}

template<typename TR, typename T1, typename T2, typename T3, typename T4>
__forceinline TR fastCall(size_t reloff, T1 a1, T2 a2, T3 a3, T4 a4) {
	return ((TR(__fastcall*)(T1, T2, T3, T4))OFFSET(reloff, 0))(a1, a2, a3, a4);
}

template<typename TR, typename T1, typename T2, typename T3, typename T4, typename T5>
__forceinline TR fastCall(size_t reloff, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) {
	return ((TR(__fastcall*)(T1, T2, T3, T4, T5))OFFSET(reloff, 0))(a1, a2, a3, a4, a5);
}

template<typename TR, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
__forceinline TR fastCall(size_t reloff, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) {
	return ((TR(__fastcall*)(T1, T2, T3, T4, T5, T6))OFFSET(reloff, 0))(a1, a2, a3, a4, a5, a6);
}

template<typename TR, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
__forceinline TR fastCall(size_t reloff, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) {
	return ((TR(__fastcall*)(T1, T2, T3, T4, T5, T6, T7))OFFSET(reloff, 0))(a1, a2, a3, a4, a5, a6, a7);
}

template<typename TR, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
__forceinline TR fastCall(size_t reloff, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) {
	return ((TR(__fastcall*)(T1, T2, T3, T4, T5, T6, T7, T8))OFFSET(reloff, 0))(a1, a2, a3, a4, a5, a6, a7, a8);
}

// thread-safe template versions of thisCall()

template<typename TR>
__forceinline TR thisCall(size_t reloff, const void* ths) {
	return ((TR(__fastcall*)(const void*))OFFSET(reloff, 0))(ths);
}

template<typename TR, typename T1>
__forceinline TR thisCall(size_t reloff, const void* ths, T1 a1) {
	return ((TR(__fastcall*)(const void*, T1))OFFSET(reloff, 0))(ths, a1);
}

template<typename TR, typename T1, typename T2>
__forceinline TR thisCall(size_t reloff, const void* ths, T1 a1, T2 a2) {
	return ((TR(__fastcall*)(const void*, T1, T2))OFFSET(reloff, 0))(ths, a1, a2);
}

template<typename TR, typename T1, typename T2, typename T3>
__forceinline TR thisCall(size_t reloff, const void* ths, T1 a1, T2 a2, T3 a3) {
	return ((TR(__fastcall*)(const void*, T1, T2, T3))OFFSET(reloff, 0))(ths, a1, a2, a3);
}

template<typename TR, typename T1, typename T2, typename T3, typename T4>
__forceinline TR thisCall(size_t reloff, const void* ths, T1 a1, T2 a2, T3 a3, T4 a4) {
	return ((TR(__fastcall*)(const void*, T1, T2, T3, T4))OFFSET(reloff, 0))(ths, a1, a2, a3, a4);
}