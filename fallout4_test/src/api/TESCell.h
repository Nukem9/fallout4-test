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
#include "NiMain/NiTypes.h"

#pragma pack(push, 1)

namespace api {
	class TESObjectREFR;
	class TESCell : public TESForm {
	private:
		enum { eTypeID = FormType::ftCell };
	public:
		// 08
		struct CellCoordinates { INT32 X, Y; };
		// 8C
		struct LightingData {
			/*00*/ NiRGBA		ambient;
			/*04*/ NiRGBA		directional;
			/*08*/ NiRGBA		fogNearColor;
			/*0C*/ FLOAT		fogNear;
			/*10*/ FLOAT		fogFar;
			/*14*/ DWORD		Unk14;
			/*18*/ DWORD		Unk18;
			/*1C*/ DWORD		Unk1C;
			/*20*/ DWORD		Unk20;
			/*24*/ FLOAT		fogPow;

			// 18
			struct DirectionalAmbientLightingData {
				/*00*/ NiRGBA Xplus;
				/*04*/ NiRGBA Xminus;
				/*08*/ NiRGBA Yplus;
				/*0C*/ NiRGBA Yminus;
				/*10*/ NiRGBA Zplus;
				/*14*/ NiRGBA Zminus;
			};

			/*28*/ DirectionalAmbientLightingData DirectionalAmbientLighting;

			/*40*/ DWORD		Unk40;
			/*44*/ FLOAT		Unk44;
			/*48*/ NiRGBA		fogFarColor;
			/*4C*/ FLOAT		fogMax;
			/*50*/ DWORD		Unk50;
			/*54*/ DWORD		Unk54;
			/*58*/ DWORD		Unk58;
			/*5C*/ FLOAT		fogNearHeightMid;
			/*60*/ FLOAT		fogNearHeightRange;
			/*64*/ NiRGBA		fogHighNearColor;
			/*68*/ NiRGBA		fogHighFarColor;
			/*6C*/ FLOAT		fogHighDensityScale;
			/*70*/ FLOAT		fogScaleNear;
			/*74*/ FLOAT		fogScaleFar;
			/*78*/ FLOAT		fogScaleHighNear;
			/*7C*/ FLOAT		fogScaleHighFar;
			/*80*/ FLOAT		fogFarHeightMid;
			/*84*/ FLOAT		fogFarHeightRange;
		};
		// 08
		union CellData {
			/*00*/ CellCoordinates* Grid;						// if exterior
			/*00*/ LightingData* Lighting;						// if interior
		};
	private:
		enum CellFlags : WORD {
			cfInterior			= 0x1,
			cfExterior			= 0x2,
			cfInvertFastTravel	= 0x4,							// interiors: can travel, exteriors: cannot travel
			cfFragment			= 0x40,							// (exteriors only) Exists Ext01, ... ExtXX
		};
		enum CellProcessLevels : WORD {
			cplNone		= 0x0,									// default value
			cplLoaded	= 0x4,									// loaded cells that are not processed
			cplActive	= 0x8,									// current interior cell, or exterior cells within fixed radius of current exterior cell
		};
	private:
		TESFullName _fullName;
		CHAR _pad1[0x10];
		CellFlags _cell_flags;
		CellProcessLevels _cell_process_level;
		CHAR Unk54[0x1];										// set 0x1 when unloaded cell
		CHAR _pad2[0x3];
		LPVOID _extradata;
		CellData _celldata;
		TESForm* _landspace;
		CHAR _pad3[0x8];
		TESFormArray* _navmeshes;
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
		inline INT32 GetGridX(VOID) const { return (IsExterior()) ? _celldata.Grid->X : 0; }
		inline INT32 GetGridY(VOID) const { return (IsExterior()) ? _celldata.Grid->Y : 0; }
		inline LightingData* GetLighting(VOID) { return (IsInterior()) ? _celldata.Lighting : NULL; }
		inline BOOL IsActive(VOID) const { return _cell_process_level == CellProcessLevels::cplActive; }
		inline BOOL IsLoaded(VOID) const { return _cell_process_level == CellProcessLevels::cplLoaded; }
		inline BOOL IsFastTravelEnabled(VOID) const { 
			return IsInterior() ? (CellFlags::cfInvertFastTravel & _cell_flags) : 
				((CellFlags::cfInvertFastTravel & _cell_flags) != CellFlags::cfInvertFastTravel);
		}
		inline BOOL IsInterior(VOID) const { return _cell_flags & CellFlags::cfInterior; }
		inline BOOL IsExterior(VOID) const { return _cell_flags & CellFlags::cfExterior; }
		inline BOOL IsFragment(VOID) const { return _cell_flags & CellFlags::cfFragment; }
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
		READ_PROPERTY(GetLighting) LightingData* Lighting;
	public:
		virtual ~TESCell(VOID);
	};
}

#pragma pack(pop)