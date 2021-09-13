//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

namespace Offsets
{
	struct OffsetEntry
	{
		uint32_t RelOffset;
		uint32_t Version;
		const char *Signature;
		int SigAdjustment;
		uint32_t TranslatedOffset;
	};

	uintptr_t Resolve(uint32_t RelOffset, uint32_t Version);
	bool CanResolve(uint32_t RelOffset, uint32_t Version);

	void BuildTableForCKF4Version(uint32_t Version);
	void BuildTableForCKSSEVersion(uint32_t Version);
	void BuildTableForGameVersion(uint32_t Version);
	void BuildTable(const OffsetEntry *Table, size_t Count, bool CurrentVersion);
	void ValidateTable(const OffsetEntry *Table, size_t Count);
	void DumpLoadedTable(const char *FilePath);
}