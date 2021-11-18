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
#include "TESFile.h"

#pragma pack(push, 1)

namespace api {
	class TESForm : public TESPersistent {
	public:
		// Form Type
		enum FormType : BYTE {
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
		enum FormFlags : DWORD {
			fsMaster					= /*00*/ 0x1,			// form is from an esm file
			fsModified					= /*01*/ 0x2,			// form is overriden by active mod or save file
			fsLinked					= /*03*/ 0x8,			// set after formids have been resolved into TESForm*
			fsDeleted					= /*05*/ 0x20,			// set on deletion, not saved in CK 
			fsInitialDisabled			= /*0B*/ 0x800,			// Disable TESObjectREFR
			fsIgnored					= /*0C*/ 0x1000,		// ??
			fsTemporary					= /*0E*/ 0x4000,		// not saved in CK
		};
	protected:
		// 06
		struct BSTrackingData {
			WORD date;         // 00 low byte is day of month, high byte is number of months with 1 = Jan. 2003 (Decembers are a little weird)
			WORD pad02;
			BYTE lastUser;     // 04 userID that last had this form checked out
			BYTE currentUser;  // 05 userID that has this form checked out
		};
	protected:
		BSModInfo* _modinfo;													// 08-10
		DWORD _FormFlags;														// 10-14
		DWORD _FormID;															// 14-18
		BSEntryString* _EntryEditID;											// 18-20
		BSTrackingData _trackingData;											// 20-26
		FormType _type;															// 26-27
		CHAR pad27[0x1];														// 27-28
	public:
		VOID LoadForm(TESFile* file);											// vtbl->0D0
		VOID SaveForm(TESFile* file);											// vtbl->0D8
		FormType GetType(VOID) const;											// vtbl->120
		VOID DebugInfo(LPSTR buffer, DWORD dwSize) const;						// vtbl->128
		VOID MarkAsDeleted(BOOL state = TRUE);									// vtbl->198 
		VOID MarkAsChanged(BOOL state = TRUE);									// vtbl->1A0 
		DWORD GetEditIDLength(VOID) const;										// vtbl->230
		LPCSTR GetEditID(VOID) const;											// vtbl->238
	public:
		inline BOOL	IsReference(VOID) const { return (_type == FormType::ftReference) || (_type == FormType::ftCharacter); }
		inline BOOL IsFromMaster(VOID) const { return (_FormFlags & FormFlags::fsMaster); }
		inline BOOL IsModified(VOID) const { return (_FormFlags & FormFlags::fsModified); }
		inline BOOL IsLinked(VOID) const { return (_FormFlags & FormFlags::fsLinked); }
		inline BOOL IsDeleted(VOID) const { return (_FormFlags & FormFlags::fsDeleted); }
		inline BOOL IsInitialDisabled(VOID) const { return (_FormFlags & FormFlags::fsInitialDisabled); }
		inline BOOL IsIgnored(VOID) const { return (_FormFlags & FormFlags::fsIgnored); }
		inline BOOL IsTemporary(VOID) const { return (_FormFlags & FormFlags::fsTemporary); }
	public:
		BSString GetID(VOID) const;
		inline DWORD GetFormFlags(VOID) const { return _FormFlags; }
		inline DWORD GetFormID(VOID) const { return _FormID; }
	public:
		READ_PROPERTY(IsModified) BOOL Active;
		READ_PROPERTY(GetFormID) DWORD FormID;
		READ_PROPERTY(GetFormFlags) DWORD FormFlag;
		READ_PROPERTY(GetID) BSString EditID;
		READ_PROPERTY(GetType) FormType TypeID;
	};
	static_assert(sizeof(TESForm) == 0x28, "TESForm class should be the size of 0x28");

	TESForm* GetFormByNumericID(const DWORD SearchID);

	typedef BSTArray<TESForm*>	TESFormArray;
	typedef BSTArray<DWORD>		TESFormIDArray;
}

// 400 dublicate function (parent form, refr form, stack, bool)
// 408 delete function (parent form, refr form)
// 428 inc ref function (parent form)
// 430 dec ref function (parent form)
// 438 dublicate function (parent form, refr form, stack, bool)
// 480 dublicate function (parent form, refr form, stack, bool) (general)

#pragma pack(pop)