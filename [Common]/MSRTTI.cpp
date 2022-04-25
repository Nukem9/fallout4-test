#include "MSRTTI.h"
#include "StdAfx.h"

LPVOID FIXAPI __rtti_MemAlloc(UINT64 Size) {
	return QMemAlloc(Size, 8);
}

VOID FIXAPI __rtti_MemFree(LPVOID Memory) {
	QMemFree(Memory);
}

extern "C" {
	typedef LPVOID (*malloc_func_t)(size_t);
	typedef VOID (*free_func_t)(LPVOID);
	LPSTR __unDNameEx(LPSTR outputString, LPCSTR name, INT maxStringLength, malloc_func_t pAlloc, free_func_t pFree, LPSTR (__fastcall *pGetParameter)(INT), UINT disableFlags);
}

namespace MSRTTI {
	using namespace detail;

	std::vector<Info> Tables;

	VOID FIXAPI Initialize(VOID) {
		for (UINT_PTR i = g_RdataBase; i < (g_RdataEnd - sizeof(UINT_PTR) - sizeof(UINT_PTR)); i++) {
			// Skip all non-2-aligned addresses. Not sure if this is OK or it skips tables.
			if (i % 2 != 0)
				continue;

			//
			// This might be a valid RTTI entry, so check if:
			// - The COL points to somewhere in .rdata
			// - The COL has a valid signature
			// - The first virtual function points to .text
			//
			UINT_PTR addr = *(UINT_PTR*)i;
			UINT_PTR vfuncAddr = *(UINT_PTR*)(i + sizeof(UINT_PTR));

			if (!IsWithinRDATA(addr) || !IsWithinCODE(vfuncAddr))
				continue;

			auto locator = reinterpret_cast<CompleteObjectLocator*>(addr);

			if (!IsValidCOL(locator))
				continue;

			Info info;
			info.VTableAddress	= i + sizeof(UINT_PTR);
			info.VTableOffset	= locator->Offset;
			info.VFunctionCount	= 0;
			info.RawName		= locator->TypeDescriptor.Get()->name;
			info.Locator		= locator;

			// Demangle
			info.Name = __unDNameEx(NULL, info.RawName + 1, 0, __rtti_MemAlloc, __rtti_MemFree, NULL, 0x2800);

			// Determine number of virtual functions
			for (UINT_PTR j = info.VTableAddress; j < (g_RdataEnd - sizeof(UINT_PTR)); j += sizeof(UINT_PTR)) {
				if (!IsWithinCODE(*(UINT_PTR*)j))
					break;

				info.VFunctionCount++;
			}

			// Done
			Tables.push_back(info);
		}
	}

	VOID FIXAPI Dump(FILE* File) {
		for (const Info& info : Tables)
			fprintf(File, "`%s`: VTable [0x%p, 0x%p offset, %lld functions] `%s`\n", info.Name, info.VTableAddress - g_ModuleBase, info.VTableOffset, info.VFunctionCount, info.RawName);
	}

	const Info* FIXAPI Find(LPCSTR Name, BOOL Exact) {
		auto results = FindAll(Name, Exact);
		AssertMsgDebug(results.size() == 1, "Had no results or had more than 1 result");

		return results.at(0);
	}

	std::vector<const Info*> FIXAPI FindAll(LPCSTR Name, BOOL Exact) {
		// Multiple classes can have identical names but different vtable displacements,
		// so return all that match
		std::vector<const Info*> results;

		for (const Info& info : Tables) {
			if (Exact) {
				if (!strcmp(info.Name, Name))
					results.push_back(&info);
			}
			else {
				if (strcasestr(info.Name, Name))
					results.push_back(&info);
			}
		}

		return results;
	}

	namespace detail {
		BOOL FIXAPI IsWithinRDATA(UINT_PTR Address) {
			return (Address >= g_RdataBase && Address <= g_RdataEnd) ||
				(Address >= g_DataBase && Address <= g_DataEnd);
		}

		BOOL FIXAPI IsWithinCODE(UINT_PTR Address) {
			return Address >= g_CodeBase && Address <= g_CodeEnd;
		}

		BOOL FIXAPI IsValidCOL(CompleteObjectLocator* Locator) {
			return Locator->Signature == CompleteObjectLocator::COL_Signature64 && IsWithinRDATA(Locator->TypeDescriptor.Address());
		}

		LPCSTR FIXAPI strcasestr(LPCSTR String, LPCSTR Substring) {
			LPCSTR a, b;

			for (; *String; *String++) {
				a = String;
				b = Substring;

				while (toupper(*a++) == toupper(*b++))
					if (!*b)
						return String;
			}

			return NULL;
		}
	}
}