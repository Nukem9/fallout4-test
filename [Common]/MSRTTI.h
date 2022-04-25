#pragma once

#include "[Common]\CommInc.h"

namespace MSRTTI {
	namespace detail { struct CompleteObjectLocator; }

	struct Info {
		UINT_PTR						VTableAddress;				// Address in .rdata section
		UINT_PTR						VTableOffset;				// Offset of this vtable in complete class (from top)
		UINT64							VFunctionCount;				// Number of contiguous functions
		LPCSTR							Name;						// Demangled
		LPCSTR							RawName;					// Mangled
		detail::CompleteObjectLocator*	Locator;					//
	};

	VOID FIXAPI Initialize(VOID);
	VOID FIXAPI Dump(FILE* File);
	const Info* FIXAPI Find(LPCSTR Name, BOOL Exact = TRUE);
	std::vector<const Info*> FIXAPI FindAll(LPCSTR Name, BOOL Exact = TRUE);

	namespace detail {
		struct TypeDescriptor;
		struct PMD;
		struct CompleteObjectLocator;
		struct ClassHierarchyDescriptor;
		struct BaseClassArray;
		struct BaseClassDescriptor;

		template<typename T>
		struct RVA {
			UINT32 Offset;
			UINT_PTR Address() { return (UINT_PTR)GetModuleHandleA(NULL) + Offset; }
			T Get() { return (T)Address(); }
		};

		// Also known as `class type_info`
		struct TypeDescriptor {
			LPVOID	vftable;	// const type_info::`vftable'
			UINT64	unknown;	// CRT internal
			CHAR	name[1];
		};

		struct PMD {
			INT Mdisp;	// Member displacement (vftable offset in the class itself)
			INT Pdisp;	// Vbtable displacement (vbtable offset, -1: vftable is at displacement PMD.mdisp inside the class)
			INT Vdisp;	// Displacement inside vbtable
		};

		struct CompleteObjectLocator {
			enum {
				COL_Signature32 = 0,
				COL_Signature64 = 1,
			};

			UINT32 Signature;								// 32-bit zero, 64-bit one
			UINT32 Offset;									// Offset of this vtable in the complete class
			UINT32 CDOffset;								// Constructor displacement offset
			RVA<TypeDescriptor*> TypeDescriptor;			// TypeDescriptor of the complete class
			RVA<ClassHierarchyDescriptor*> ClassDescriptor;	// Describes inheritance hierarchy
		};

		struct ClassHierarchyDescriptor {
			enum {
				HCD_NoInheritance = 0,
				HCD_MultipleInheritance = 1,
				HCD_VirtualInheritance = 2,
				HCD_AmbiguousInheritance = 4,
			};

			UINT32 Signature;						// Always zero or one
			UINT32 Attributes;						// Flags
			UINT32 NumBaseClasses;					// Number of classes in BaseClassArray
			RVA<BaseClassArray*> BaseClassArray;	// BaseClassArray
		};

#pragma warning(push)
#pragma warning(disable: 4200)						// nonstandard extension used: zero-sized array in struct/union
		struct BaseClassArray {
			UINT32 ArrayOfBaseClassDescriptors[];	// BaseClassDescriptor*
		};
#pragma warning(pop)

		struct BaseClassDescriptor {
			enum {
				BCD_NotVisible = 1,
				BCD_Ambiguous = 2,
				BCD_Private = 4,
				BCD_PrivOrProtBase = 8,
				BCD_Virtual = 16,
				BCD_Nonpolymorphic = 32,
				BCD_HasHierarchyDescriptor = 64,
			};

			RVA<TypeDescriptor*> TypeDescriptor;	// Type descriptor of the class
			UINT32 NumContainedBases;				// Number of nested classes following in the Base Class Array
			PMD Disp;								// Pointer-to-member displacement info
			UINT32 Attributes;						// Flags (BaseClassDescriptorFlags)
		};

		BOOL FIXAPI IsWithinRDATA(UINT_PTR Address);
		BOOL FIXAPI IsWithinCODE(UINT_PTR Address);
		BOOL FIXAPI IsValidCOL(CompleteObjectLocator* Locator);
		LPCSTR FIXAPI strcasestr(LPCSTR String, LPCSTR Substring);
	}
}