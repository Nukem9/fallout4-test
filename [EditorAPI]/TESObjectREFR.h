//////////////////////////////////////////
/*
* Copyright (c) 2020-2022 Perchik71 <email:perchik71@outlook.com>
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

#include "NiRefObject.h"
#include "NiEvents.h"
#include "BGSPrimitive.h"
#include "TESForm.h"
#include "TESObjectCELL.h"

#pragma pack(push, 1)

class BGSLayer;

// 10
class BSHandleRefObject : public NiRefObject {
public:
	enum { mask_RefCount = 0x3FF };

	INLINE DWORD GetRefCount(VOID) const { return ((DWORD)m_ll64RefCount) & mask_RefCount; }
	INLINE VOID DecRefHandle(VOID) {
		if (((DWORD)(InterlockedDecrement64(&m_ll64RefCount)) & mask_RefCount) == 0)
			DeleteThis();
	}
};

class TESObjectREFR : public TESForm {
private:
	enum { eTypeID = FormType::ftReference };
	enum SpecialFlagsForm {
		// https://github.com/shadeMe/Construction-Set-Extender/blob/master/EditorAPI/TESObjectREFR.h
		fs3DInvisible = 1 << 31,
		fsChildren3DInvisible = 1 << 30,
		fsFrozen = 1 << 29,
	};
private:
	BSTEventSink<LPVOID> _unkEventSink01;	// 0x028
	BSHandleRefObject _handleRefObject;		// 0x030
	BSTEventSink<LPVOID> _unkEventSink02;	// 0x040
	BSTEventSink<LPVOID> _unkEventSink03;	// 0x048
	BSTEventSink<LPVOID> _unkEventSink04;	// 0x050
	CHAR _pad0[0x70];						// 0x040
	TESObjectCELL* _parentCell;				// 0x0C8
	NiPoint3 _rotate;						// 0x0D0, 0x0D4, 0x0D8
	FLOAT _unkDC;							// 0x0DC
	NiPoint3 _position;						// 0x0E0, 0x0E4, 0x0E8
	FLOAT _unkEC;							// 0x0EC
	TESForm* _baseForm;						// 0x0F0
	LPVOID _unkE8;							// 0x0F8
	LPVOID _unk100;							// 0x100
	LPVOID _unk108;							// 0x108
	LPVOID _extraData;						// 0x110
	WORD _scale;							// 0x118
	CHAR _pad118[0x26];						// 0x11A
public:
	virtual ~TESObjectREFR(VOID) = 0;
public:
	INLINE NiPoint3 GetPosition(VOID) const { return _position; }
	INLINE VOID SetPosition(const NiPoint3& val) { _position = val; }
	INLINE NiPoint3 GetRotate(VOID) const { return _rotate; }
	INLINE VOID SetRotate(const NiPoint3& val) { _rotate = val; }
	INLINE WORD GetScale(VOID) const { return _scale; }
	INLINE VOID SetScale(WORD val) { _scale = val; }
	INLINE FLOAT GetScaleFloat(VOID) const { return ((FLOAT)_scale / 100); }
	INLINE VOID SetScaleFloat(FLOAT val) { _scale = WORD(val * 100); }
	INLINE BOOL IsInvisible(VOID) const { return (_FormFlags & SpecialFlagsForm::fs3DInvisible); }
	INLINE BOOL IsChildrenInvisible(VOID) const { return (_FormFlags & SpecialFlagsForm::fsChildren3DInvisible); }
	INLINE BOOL IsFrozen(VOID) const { return (_FormFlags & SpecialFlagsForm::fsFrozen); }
public:
	BGSPrimitive* GetPrimitive(VOID) const;
public:
	READ_PROPERTY2(TESObjectCELL*, ParentCell, _parentCell);
	READ_PROPERTY2(TESForm*, Parent, _baseForm);
	READ_PROPERTY2(const BSHandleRefObject*, RefHandler, &_handleRefObject);
	PROPERTY(GetPosition, SetPosition) NiPoint3 Position;
	PROPERTY(GetRotate, SetRotate) NiPoint3 Rotate;
	PROPERTY(GetScale, SetScale) WORD Scale;
	PROPERTY(GetScaleFloat, SetScaleFloat) FLOAT ScaleFloat;
public:
	inline static VOID(*SetNewPosition)(TESObjectREFR*, NiPoint3*);
	VOID hk_SetNewPosition(NiPoint3* newPos);
};
static_assert(sizeof(TESObjectREFR) == 0x140);

#pragma pack(pop)