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

#include "TESFormComponents.h"

#pragma pack(push, 1)

namespace api {

	// Form Type
	enum {
		ftReserved000 = 0,			// NONE
		ftReserved001,				// TES4
		ftReserved002,				// GRUP
		ftReserved003,				// GMST
		ftKeyword,
		ftLocationRefType,
		ftAction,
		ftReserved007,				// TRNS
		ftReserved008,				// CMPO
		ftTextureSet,
		ftMenuIcon,
		ftGlobal,
		ftReserved012,				// DMGT
		ftClass,
		ftFaction,
		ftHeadPart,
		ftEyes,
		ftRace,
		ftSoundMarker,
		ftAcousticSpace,
		ftReserved020,				// SKIL
		ftEffectSetting,
		ftScript,
		ftLandTexture,
		ftEnchantment,
		ftSpell,
		ftScroll,
		ftActivator,
		ftTalkingActivator,
		ftArmor,
		ftBook,
		ftContainer,
		ftDoor,
		ftIngredient,
		ftLight,
		ftMiscItem,
		ftStatic,
		ftStaticCollection,
		ftMovableStatic,
		ftGrass,
		ftTree,
		ftFlora,
		ftFurniture,
		ftWeapon,
		ftAmmo,
		ftActor,
		ftLevelCharacter,
		ftKey,
		ftPotion,
		ftIdleMarker,
		ftNote,
		ftProjectile,
		ftHazard052,
		ftReserved053,				// BNDS
		ftSoulGem,
		ftTerminal,
		ftLeveledItem,
		ftWeather,
		ftClimate,
		ftShaderParticleGeometryData,
		ftReferenceEffect,
		ftRegion,
		ftNavMeshInfoMap,
		ftCell,
		ftReference,
		ftCharacter,
		ftMissile,
		ftArrow,
		ftGrenade,
		ftBeam,
		ftFlame,
		ftCone,
		ftBarrier,
		ftHazard073,				// Hazard ?
		ftWorldSpace,
		ftLandspace,
		ftNavMesh,
		ftReserved077,				// TLOD
		ftTopic,
		ftTopicInfo,
		ftQuest,
		ftIdle,
		ftPackage,
		ftCombatStyle,
		ftLoadScreen,
		ftLeveledSpell,
		ftUnknown086,				// ANIO
		ftWater,
		ftEffectShader,
		ftReserved089,				// TOFT
		ftExplosion,
		ftDebris,
		ftImageSpace,
		ftImageSpaceModifier,
		ftListForm,
		ftPerk,
		ftBodyPartData,
		ftAddonNode,
		ftActorValueInfo,
		ftCameraShot,
		ftCameraPath,
		ftVoice,
		ftMaterial,
		ftImpactData,
		ftImpactDataSet,
		ftReserved105,				// ARMA
		ftEncounterZone,
		ftLocation,
		ftMessage,
		ftRagdoll,
		ftReserved110,				// DOBJ
		ftReserved111,				// DFOB
		ftLightingTemplate,
		ftMusic,
		ftFootstep,
		ftFootstepSet,
		ftStoryManagerBranchNode,
		ftStoryManagerQuestNode,
		ftStoryManagerEventNode,
		ftDialogue,
		ftMusicTrackFormWrapper,
		ftReserved121,				// DLVW
		ftWordOfPower,
		ftShout,
		ftEquipSlot,
		ftRelationship,
		ftScene,
		ftAssociationType,
		ftOutfit,
		ftArt,
		ftMaterialObject,
		ftMovementType,
		ftSoundDescriptorForm,
		ftDualCastData,
		ftSoundCategory,
		ftSoundOutput,
		ftCollisionLayer,
		ftColorForm,
		ftReverbParameters,
		ftReserved139,				// PKIN
		ftReserved140,				// RFGP
		ftReserved141,				// AMDL
		ftLayer,
		ftReserved143,				// COBJ
		ftReserved144,				// OMOD
		ftMaterialSwap,
		ftReserved146,				// ZOOM
		ftReserved147,				// INNR
		ftReserved148,				// KSSM
		ftReserved149,				// AECH
		ftReserved150,				// SCCO
		ftReserved151,				// AORU
		ftReserved152,				// SCSN
		ftReserved153,				// STAG
		ftReserved154,				// NOCM
		ftReserved155,				// LENS
		ftReserved156,				// LSPR
		ftReserved157,				// GDRY
		ftReserved158,				// OVIS
		ftMax
	};

	// Form State
	enum {
		fsMaster = 0x1,										// form is from an esm file
		fsModified = 0x2,									// form is overriden by active mod or save file
		fsLinked = 0x8,										// set after formids have been resolved into TESForm*
		fsGroundPiece = 0x10,								// ??
		fsDeleted = 0x20,									// set on deletion, not saved in CS or savegame
		fsBorderRegion = 0x40,								// ??
		fsTurnOffFire = 0x80,								// ??
		fsLODRespectsEnableState = 0x100,
		fsHiddenFromLocalMap = 0x200,
		fsHidden = 0x400,
		fsInitialDisabled = 0x800,
		fsIgnored = 0x1000,									// ??
		fsTemporary = 0x4000,								// not saved in CS, probably game as well
		fsVisibleWhenDistant = 0x8000,
		fsIsFullLOD = 0x10000,
		fsOffLimits = 0x20000,
		fsCompressed = 0x40000,
		fsCantWait = 0x80000,
		fsIgnoresFriendlyHits = 0x100000,
		fsReflectedByAutoWater = 0x10000000,
		fsNoRespawns = 0x40000000,
		fsNoHavok = 0x60000000,
	};

	class TESForm : public TESPersistent {
	protected:
		BSModInfo* _modinfo;															// 08-10
		DWORD _FormFlags;																// 10-14
		DWORD _FormID;																	// 14-18
		BSEntryString* _EntryEditID;													// 18-20
		CHAR _unkn2[0x6];																// 20-26
		CHAR _type;																		// 26-27
		CHAR _unkn3[0x1];																// 27-28
	public:
		virtual ~TESForm(VOID);															// 000
		virtual VOID VT_Func07(VOID);													// 038 ---
		virtual VOID VT_Func08(VOID);													// 040 not implemented
		virtual VOID VT_Func09(VOID);													// 048 ---
		virtual VOID VT_Func10(VOID);													// 050 not implemented ??? ret 1
		virtual VOID VT_Func11(VOID);													// 058 not implemented
		virtual VOID VT_Func12(VOID);													// 060 not implemented
		virtual VOID VT_Func13(VOID);													// 068 not implemented
		virtual VOID VT_Func14(VOID);													// 070 ??? Update text EditID from IDC_ID 
		virtual BOOL CheckIDControlByDialog(HWND hDialog) const;						// 078 
		virtual VOID VT_Func16(VOID);													// 080 ??? IDC_ID 
		virtual VOID VT_Func17(VOID);													// 088 ??? IDC_ID 
		virtual VOID VT_Func18(VOID);													// 090 not implemented
		virtual VOID VT_Func19(VOID);													// 098 not implemented
		virtual VOID VT_Func20(VOID);													// 0A0 not implemented
		virtual VOID VT_Func21(VOID);													// 0A8 ---
		virtual VOID VT_Func22(VOID);													// 0B0 ---
		virtual VOID VT_Func23(VOID);													// 0B8 ---
		virtual VOID VT_Func24(VOID);													// 0C0 not implemented ??? ret 1
		virtual VOID VT_Func25(VOID);													// 0C8 not implemented ??? ret 1
		virtual VOID VT_Func26(VOID);													// 0D0 ---
		virtual VOID VT_Func27(VOID);													// 0D8 ---
		virtual VOID VT_Func28(VOID);													// 0E0 call 0xB8 (skip)
		virtual VOID VT_Func29(VOID);													// 0E8 ??? Write to file
		virtual VOID VT_Func30(VOID);													// 0F0 ---
		virtual VOID CheckEqualType(const TESForm* cType) const;						// 0F8 
		virtual VOID VT_Func32(VOID);													// 100 ---
		virtual VOID VT_Func33(VOID);													// 108 not implemented
		virtual LPVOID GetLastModifiedMod(VOID) const;									// 110
		virtual LPVOID GetLastModifiedMod_2(VOID) const;								// 118
		virtual CHAR GetType(VOID) const;												// 120
		virtual VOID DebugInfo(LPSTR buffer, DWORD dwSize) const;						// 128
		virtual BOOL IsPlayerKnows(VOID) const;											// 130
		virtual VOID VT_Func39(VOID);													// 138 rcx+12 (0x0000XXXX) where XXXX value (??? 0x10000) LOD ???
		virtual VOID VT_Func40(VOID);													// 140 hmmm... no 0x8
		virtual VOID VT_Func41(VOID);													// 148 not implemented ??? ret 0
		virtual VOID VT_Func42(VOID);													// 150 not implemented ??? ret 0
		virtual VOID VT_Func43(VOID);													// 158 rcx+10 test value 0x20000
		virtual BOOL IsREFR(VOID) const;												// 160 ??? Nukem9 = value 0x80000
		virtual VOID VT_Func45(VOID);													// 168 not implemented
		virtual VOID VT_Func46(VOID);													// 170 not implemented
		virtual VOID IsNoDelete(VOID) const;											// 178 
		virtual VOID VT_Func48(VOID);													// 180 ??? test flag 0x100
		virtual VOID VT_Func49(VOID);													// 188 ??? set flag 0x200 or 0xFFFFFDFF
		virtual VOID VT_Func50(VOID);													// 190 not implemented ??? ret 0
		virtual VOID VT_Func51(VOID);													// 198 ---
		virtual BOOL MarkChanged(BOOL change) const;									// 1A0 
		virtual VOID VT_Func53(VOID);													// 1A8 test flag
		virtual VOID VT_Func54(VOID);													// 1B0 not implemented
		virtual VOID VT_Func55(VOID);													// 1B8 not implemented
		virtual VOID VT_Func56(VOID);													// 1C0 not implemented
		virtual VOID VT_Func57(VOID);													// 1C8 not implemented
		virtual VOID VT_Func58(VOID);													// 1D0 ret 0 (depended by class)
		virtual VOID VT_Func59(VOID);													// 1D8 ret 0 (depended by class)
		virtual VOID VT_Func60(VOID);													// 1E0 ret 0 (depended by class)
		virtual VOID VT_Func61(VOID);													// 1E8 ret 0 (depended by class)
		virtual TESForm* ThisIsREFR(VOID) const;										// 1F0 what ??? 
		virtual TESForm* ThisIsREFR_2(VOID) const;										// 1F8 what ??? 
		virtual VOID VT_Func64(VOID);													// 200 not implemented ??? ret 0 (depended by class)
		virtual VOID VT_Func65(VOID);													// 208 ---
		virtual VOID VT_Func66(VOID);													// 210 --- (depended by class)
		virtual VOID VT_Func67(VOID);													// 218 --- (depended by class)
		virtual VOID VT_Func68(VOID);													// 220 --- (depended by class)
		virtual VOID VT_Func69(VOID);													// 228 --- (depended by class)
		virtual DWORD GetEditIDLength(VOID) const;										// 230
		virtual LPCSTR GetEditID(VOID) const;											// 238
	public:
		inline BOOL	IsReference(VOID) const { return (_type == ftReference) || (_type == ftCharacter); }
		inline BOOL IsGroundPiece(VOID) const { return (_FormFlags & fsGroundPiece); }
		inline BOOL IsLODRespectsEnableState(VOID) const { return (_FormFlags & fsLODRespectsEnableState); }
		inline BOOL IsHiddenFromLocalMap(VOID) const { return (_FormFlags & fsHiddenFromLocalMap); }
		inline BOOL	IsActive(VOID) const { return (_FormFlags & fsModified); }
		inline BOOL	IsDeleted(VOID) const { return (_FormFlags & fsDeleted); }
		inline BOOL	IsTemporary(VOID) const { return (_FormFlags & fsTemporary); }
		inline BOOL IsFullLOD(VOID) const { return (_FormFlags & fsIsFullLOD); }
		inline BOOL IsReflectedByAutoWater(VOID) const { return (_FormFlags & fsReflectedByAutoWater); }
		inline BOOL IsNoRespawns(VOID) const { return (_FormFlags & fsNoRespawns); }
		inline BOOL IsNoHavok(VOID) const { return (_FormFlags & fsNoHavok); }
		inline BOOL	IsVisibleWhenDistant(VOID) const { return (_FormFlags & fsVisibleWhenDistant); }
		inline BOOL	IsInitiallyDisabled(VOID) const { return (_FormFlags & fsInitialDisabled); }
	public:
		std::string GetID(VOID) const;
		inline DWORD GetFormFlags(VOID) const { return _FormFlags; }
		inline DWORD GetFormID(VOID) const { return _FormID; }
	public:
		READ_PROPERTY(IsActive) BOOL Active;
		READ_PROPERTY(GetFormID) DWORD FormID;
		READ_PROPERTY(GetFormFlags) DWORD FormFlag;
		READ_PROPERTY(GetID) std::string EditID;
		READ_PROPERTY(GetType) CHAR TypeID;
	};

	TESForm* GetFormByNumericID(const DWORD SearchID);
}

#pragma pack(pop)