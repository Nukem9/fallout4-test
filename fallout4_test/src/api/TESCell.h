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

#include "TESForm.h"

#pragma pack(push, 1)

namespace api {
	class TESObjectREFR;
	class TESCell : public TESForm {
	private:
		enum { eTypeID = FormType::ftCell };
	public:
		struct TESCellDataExt {
			INT32 gridX;		// 00
			INT32 gridY;		// 04
			DWORD* maxHeight;	// 08
		};
	private:
		enum {
			cfInterior = 1,
			cfExterior = 2,
		};
	private:
		TESFullName _fullName;
		CHAR _pad1[0x10];
		UINT16 _cell_flags;
		CHAR _pad2[0x6];
		LPVOID _extradata;
		TESCellDataExt* _maxheightdata;
		TESForm* _landspace;
		CHAR _pad3[0x8];
		BSTArray<TESForm*>* _navmeshes;
		BSTArray<TESObjectREFR*> _items;
	public:
		inline const TESObjectREFR** GetItemsConst(VOID) const { return (const TESObjectREFR**)_items.QBuffer(); }
		inline TESObjectREFR** GetItems(VOID) { return (TESObjectREFR**)_items.QBuffer(); }
		inline DWORD GetItemsCount(VOID) const { return (DWORD)_items.QSize(); }
		inline const TESForm* GetLandspaceConst(VOID) const { return _landspace; }
		inline TESForm* GetLandspace(VOID) { return _landspace; }
		inline BOOL IsNavMesh(VOID) const { return _navmeshes != NULL; }
		inline DWORD GetNavMeshesCount(VOID) const { return (DWORD)_navmeshes->QSize(); }
		inline const TESForm** GetNavMeshesConst(VOID) const { return (const TESForm**)_navmeshes->QBuffer(); }
		inline TESForm** GetNavMeshes(VOID) { return (TESForm**)_navmeshes->QBuffer(); }
		inline BOOL IsCellDataExt(VOID) const { return _maxheightdata != NULL; }
		inline INT32 GetGridX(VOID) const { return _maxheightdata ? _maxheightdata->gridX : 0; }
		inline INT32 GetGridY(VOID) const { return _maxheightdata ? _maxheightdata->gridY : 0; }
		inline BOOL IsInterior(VOID) const { return _cell_flags == cfInterior; }
		inline BOOL IsExterior(VOID) const { return _cell_flags == cfExterior; }
		inline std::string GetFullName(VOID) const { return _fullName.c_str(); }
	public:
		READ_PROPERTY(GetNavMeshes) TESForm** NavMeshes;
		READ_PROPERTY(GetNavMeshesConst) const TESForm** NavMeshesConst;
		READ_PROPERTY(GetNavMeshesCount) DWORD NavMeshesCount;
		READ_PROPERTY(GetItems) TESObjectREFR** Items;
		READ_PROPERTY(GetItemsConst) const TESObjectREFR** ItemsConst;
		READ_PROPERTY(GetItemsCount) DWORD ItemsCount;
		READ_PROPERTY(GetLandspaceConst) const TESForm* LandspaceConst;
		READ_PROPERTY(GetLandspace) TESForm* Landspace;
		READ_PROPERTY(GetGridX) INT32 GridX;
		READ_PROPERTY(GetGridY) INT32 GridY;
	public:
		virtual ~TESCell(VOID);
	};
}

#pragma pack(pop)