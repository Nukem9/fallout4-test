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

#include <map>

class NiRTTI
{
private:
	const char *m_pcName;
	const NiRTTI *m_pkBaseRTTI;

public:
	static NiRTTI *__ctor__(void *Instance, const char *Name, const NiRTTI *BaseType);
	NiRTTI(const char *Name, const NiRTTI *BaseType);

	bool CopyName(char* NameBuffer, uint32_t MaxSize) const;
	bool Inherits(const NiRTTI *Other) const;

	const char *GetName() const;
	const NiRTTI *GetBaseRTTI() const;


	static const std::map<std::string, const NiRTTI *>& GetAllTypes();
	static void DumpRTTIListing(FILE *File, bool IDAScript);

#define DefineNiRTTI(x) static NiRTTI *ms_##x;
#include "NiRTTI.inl"
#undef DefineNiRTTI
};
static_assert(sizeof(NiRTTI) == 0x10);
//static_assert_offset(NiRTTI, m_pcName, 0x0);
//static_assert_offset(NiRTTI, m_pkBaseRTTI, 0x10);