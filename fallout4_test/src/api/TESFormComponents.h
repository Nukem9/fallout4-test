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
#include "NiMain/GameTypes.h"
#include "BSTArray.h"
#include "BSTList.h"

#pragma pack(push, 1)

namespace api {
	// Form State
	enum {
		fsModified = 0x2,
		fsGroundPiece = 0x10,
		fsDeleted = 0x20,
		fsLODRespectsEnableState = 0x100,
		fsHiddenFromLocalMap = 0x200,
		fsHidden = 0x400,
		fsInitialDisabled = 0x800,
		fsTemporary = 0x4000, // ???
		fsIsFullLOD = 0x10000,
		fsReflectedByAutoWater = 0x10000000,
		fsNoRespawns = 0x40000000,
		fsNoHavok = 0x60000000,
	};

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

	struct BSModInfo {
		LPVOID entries;
		UINT64 size;
	};

	class TESPersistent {
	public:
		TESPersistent(VOID) = default;
	public:
		virtual ~TESPersistent(VOID);	// 000
		virtual VOID VT_Func01(VOID);	// 008
		virtual VOID VT_Func02(VOID);	// 010
		virtual VOID VT_Func03(VOID);	// 018
		virtual VOID VT_Func04(VOID);	// 020 not implemented
		virtual VOID VT_Func05(VOID);	// 028 not implemented
		virtual VOID VT_Func06(VOID);	// 030 ---
	};

	class TESFullName : public TESPersistent {
	public:
		virtual ~TESFullName(VOID);														// 000
	public:
		virtual VOID VT_Func07(VOID);													// 038
		virtual VOID VT_Func08(VOID);													// 040
		virtual VOID VT_Func09(VOID);													// 048
		virtual VOID VT_Func10(VOID);													// 050
		virtual VOID VT_Func11(VOID);													// 058	not implemented
		virtual VOID VT_Func12(VOID);													// 060	not implemented
		virtual VOID VT_Func13(VOID);													// 068	not implemented
		virtual VOID VT_Func14(VOID);													// 070
		// Checking for the existence of a control
		// control UID 1060(0x424)
		virtual VOID Debug_CheckExistsControl_DisplayName_N1060(HWND hDialog) const;	// 078
		virtual VOID VT_Func16(VOID);													// 080
		// The function is passed a window descriptor where there is an edit IDC_FULL_NAME window. 
		// She takes the text from there and saves it.
		// control UID 1060(0x424)
		virtual VOID UpdateStringFromControl_DisplayName_N1060(HWND hDialog) const;		// 088
		virtual VOID VT_Func18(VOID);													// 090	not implemented
		virtual VOID VT_Func19(VOID);													// 098	not implemented
		virtual VOID VT_Func20(VOID);													// 0A0	not implemented
		virtual VOID VT_Func21(VOID);													// 0A8	
		// return full name
		virtual LPCSTR c_str(VOID) const;												// 0B0
	protected:
		BSFixedString String;
	};
}

#pragma pack(pop)