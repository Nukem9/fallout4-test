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

#include "../../../common.h"
#include "NiRTTI.h"

std::map<std::string, const NiRTTI *> NiRTTIMap;

NiRTTI *NiRTTI::__ctor__(void *Instance, const char *Name, const NiRTTI *BaseType)
{
	return new (Instance) NiRTTI(Name, BaseType);
}

NiRTTI::NiRTTI(const char *Name, const NiRTTI *BaseType) : m_pcName(Name), m_pkBaseRTTI(BaseType)
{
	NiRTTIMap[Name] = this;

	//
	// Avoid a huge array of if/else by using a compile-time switch.
	// https://stackoverflow.com/a/43253227 (FNV32/64)
	//
	auto hashName = [](const char *Input) constexpr
	{
		size_t hash = sizeof(size_t) == 8 ? 0xcbf29ce484222325 : 0x811c9dc5;
		const size_t prime = sizeof(size_t) == 8 ? 0x00000100000001b3 : 0x01000193;

		while (*Input)
		{
			hash ^= static_cast<size_t>(*Input);
			hash *= prime;
			++Input;
		}

		return hash;
	};

	switch (hashName(Name))
	{
#define DefineNiRTTI(x) __pragma(warning(suppress:4307)) case hashName(#x): ms_##x = this; break;
#include "NiRTTI.inl"
#undef DefineNiRTTI
	}
}

bool NiRTTI::CopyName(char* NameBuffer, uint32_t MaxSize) const
{
	const char* pcName = GetName();

	if (!pcName || !NameBuffer)
	{
		strcpy_s(NameBuffer, MaxSize, "\0");
		return false;
	}

	strcpy_s(NameBuffer, MaxSize, pcName);
	return true;
}

bool NiRTTI::Inherits(const NiRTTI *Other) const
{
	const NiRTTI *rtti = this;

	while (rtti != Other && rtti)
		rtti = rtti->GetBaseRTTI();

	return (rtti == Other);
}

const char *NiRTTI::GetName() const
{
	return m_pcName;
}

const NiRTTI *NiRTTI::GetBaseRTTI() const
{
	return m_pkBaseRTTI;
}

const std::map<std::string, const NiRTTI *>& NiRTTI::GetAllTypes()
{
	return NiRTTIMap;
}

void NiRTTI::DumpRTTIListing(FILE *File, bool IDAScript)
{
	if (IDAScript)
	{
		fprintf(File, "#include <idc.idc>\n");
		fprintf(File, "static main()\n");
		fprintf(File, "{\n");

		for (const auto& kv : NiRTTIMap)
		{
			void *addr = (void *)((uintptr_t)kv.second - g_ModuleBase + 0x140000000);
			fprintf(File, "\tcreate_qword(0x%p); set_name(0x%p, \"%s::ms_RTTI\");\n", addr, addr, kv.first.c_str());
		}

		fprintf(File, "}\n");
	}
	else
	{
		for (const auto& kv : NiRTTIMap)
			fprintf(File, "DefineNiRTTI(%s)\n", kv.first.c_str());
	}
}

#define DefineNiRTTI(x) NiRTTI *NiRTTI::ms_##x;
#include "NiRTTI.inl"
#undef DefineNiRTTI