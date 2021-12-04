//////////////////////////////////////////
/*
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

#include "NiMain/NiRefObject.h"
#include "NiMain/GameEvents.h"

#include "TESForm.h"
#include "TESCell.h"

#pragma pack(push, 1)

namespace api {

	// 10
	class BSHandleRefObject : public NiRefObject {
	public:
		enum { mask_RefCount = 0x3FF };

		inline DWORD GetRefCount(VOID) const { return ((DWORD)m_ll64RefCount) & mask_RefCount; }
		inline VOID DecRefHandle(VOID) {
			if (((DWORD)(InterlockedDecrement64(&m_ll64RefCount)) & mask_RefCount) == 0)
				DeleteThis();
		}
	};

	class TESObjectREFR : public TESForm {
	private:
		enum { eTypeID = FormType::ftReference };
	private:
		BSTEventSink<LPVOID> _unkEventSink01;	// 0x28
		BSHandleRefObject _handleRefObject;		// 0x30
		BSTEventSink<LPVOID> _unkEventSink02;	// 0x40
		BSTEventSink<LPVOID> _unkEventSink03;	// 0x48
		BSTEventSink<LPVOID> _unkEventSink04;	// 0x50
		CHAR _pad0[0x70];						// 0x40
		TESCell* _parentCell;					// 0xC8
		NiPoint3 _rotate;						// 0xD0, 0xD4, 0xD8
		FLOAT _unkDC;							// 0xDC
		NiPoint3 _position;						// 0xE0, 0xE4, 0xE8
		FLOAT _unkEC;							// 0xEC
		TESForm* _baseForm;						// 0xF0
		LPVOID _unkE8;							// 0xF8
	public:
		virtual ~TESObjectREFR(VOID) = 0;
	public:
		inline const TESCell* GetParentCellConst(VOID) const { return _parentCell; }
		inline const TESForm* GetParentConst(VOID) const { return _baseForm; }
		inline TESCell* GetParentCell(VOID) { return _parentCell; }
		inline TESForm* GetParent(VOID) { return _baseForm; }
		inline BSHandleRefObject* GetRefHandler(VOID) { return &_handleRefObject; }
		inline const BSHandleRefObject* GetRefHandlerConst(VOID) const { return &_handleRefObject; }
		inline NiPoint3* GetPosition(VOID) { return &_position; }
		inline const NiPoint3* GetPositionConst(VOID) const { return &_position; }
		inline NiPoint3* GetRotate(VOID) { return &_rotate; }
		inline const NiPoint3* GetRotateConst(VOID) const { return &_rotate; }
	public:
		READ_PROPERTY(GetParentCell) TESCell* ParentCell;
		READ_PROPERTY(GetParent) TESForm* Parent;
		READ_PROPERTY(GetParentCellConst) const TESCell* ParentCellConst;
		READ_PROPERTY(GetParentConst) const TESForm* ParentConst;
		READ_PROPERTY(GetRefHandler) BSHandleRefObject* RefHandler;
		READ_PROPERTY(GetRefHandlerConst) const BSHandleRefObject* RefHandlerConst;
		READ_PROPERTY(GetPosition) NiPoint3* Position;
		READ_PROPERTY(GetPositionConst) const NiPoint3* PositionConst;
		READ_PROPERTY(GetRotate) NiPoint3* Rotate;
		READ_PROPERTY(GetRotateConst) const NiPoint3* RotateConst;
	};
}

#pragma pack(pop)