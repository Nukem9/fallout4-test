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

#include "../common.h"
#include "TESForm.h"

#pragma pack(push, 1)

namespace api {
	class TESCell;
	class TESObjectREFR;
	class BGLayer : public TESForm {
	private:
		enum { eTypeID = ftLayer };
	private:
		BGLayer* _parent;
		BSTArray<TESObjectREFR*> _items;
		BSTArray<BGLayer*> _childs;
	public:
		inline DWORD GetChildsCount(VOID) const {
			return (DWORD)_childs.QSize();
		}
		inline const BGLayer** GetChildsArrayConst(VOID) const {
			return (const BGLayer**)_childs.QBuffer();
		}
		inline BGLayer** GetChildsArray(VOID) {
			return (BGLayer**)_childs.QBuffer();
		}
		inline BGLayer* GetChildById(DWORD dwId) {
			return _childs[dwId];
		}
		inline DWORD GetItemsCount(VOID) const {
			return (DWORD)_items.QSize();
		}
		inline const TESObjectREFR** GetItemsArrayConst(VOID) const {
			return (const TESObjectREFR**)_items.QBuffer();
		}
		inline TESObjectREFR** GetItemsArray(VOID) {
			return (TESObjectREFR**)_items.QBuffer();
		}
		inline TESObjectREFR* GetItemById(DWORD dwId) {
			return _items[dwId];
		}
		inline const BGLayer* GetParentConst(VOID) const {
			return _parent;
		}
		inline BGLayer* GetParent(VOID) {
			return _parent;
		}
		inline BOOL IsParent(VOID) const {
			return _parent != NULL;
		}
	public:
		DWORD GetItemsCountInCell(const TESCell* cell) const;
	public:
		inline TESObjectREFR* operator[](DWORD dwId) {
			return _items[dwId];
		}
		inline const TESObjectREFR* operator[](DWORD dwId) const {
			return _items[dwId];
		}
	public:
		READ_PROPERTY(GetChildsCount) DWORD ChildsCount;
		READ_PROPERTY(GetChildsArrayConst) const BGLayer** ChildsArrayConst;
		READ_PROPERTY(GetChildsArray) BGLayer** ChildsArray;
		READ_PROPERTY(GetItemsCount) DWORD ItemsCount;
		READ_PROPERTY(GetItemsArrayConst) const TESObjectREFR** ItemsArrayConst;
		READ_PROPERTY(GetItemsArray) TESObjectREFR** ItemsArray;
		READ_PROPERTY(GetParentConst) const BGLayer* ParentConst;
		READ_PROPERTY(GetParent) BGLayer* Parent;
	};
}

#pragma pack(pop)