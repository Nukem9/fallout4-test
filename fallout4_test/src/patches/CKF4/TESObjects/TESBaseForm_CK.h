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

#include "../../../common.h"
#include "../../TES/BSTArray.h"

#define TESFORM_ACTIVE_FLAGS 2

class TESBaseForm_CK {
public:
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
private:
	CHAR _unkn1[0x8];	// 08
	DWORD _FormFlags;	// 10
	DWORD _FormID;		// 14
	CHAR _unkn2[0xE];	// 18
	CHAR _type;			// 26
	CHAR _unkn3[0x1];	// 27
protected:
	inline CHAR __GetTypeID(VOID) const { return _type; }
public:
	inline BOOL GetActive(VOID) const { return (_FormFlags & TESFORM_ACTIVE_FLAGS) != 0; }
	inline DWORD GetFormID(VOID) const { return _FormID; }
public:
	READ_PROPERTY(GetActive) BOOL Active;
	READ_PROPERTY(GetFormID) DWORD FormID;
public:
	virtual ~TESBaseForm_CK(VOID);
};
