//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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
#include "NiRTTI.h"

class NiNode;
class NiStream;
class NiCloningProcess;
class NiObjectGroup;

class NiObject : public NiRefObject
{
public:
	virtual const NiRTTI						*GetRTTI() const;
	virtual const NiNode						*IsNode() const;
	virtual class NiSwitchNode					*IsSwitchNode() const;
	virtual class BSFadeNode					*IsFadeNode() const;
	virtual class BSMultiBoundNode				*IsMultiBoundNode() const;
	virtual class BSGeometry					*IsGeometry() const;
	virtual class NiTriStrips					*IsTriStrips() const;
	virtual class BSTriShape					*IsTriShape() const;
	virtual class BSSegmentedTriShape			*IsSegmentedTriShape() const;
	virtual class BSSubIndexTriShape			*IsSubIndexTriShape() const;
	virtual class BSDynamicTriShape				*IsDynamicTriShape() const;
	virtual class NiGeometry					*IsNiGeometry() const;
	virtual class NiTriBasedGeom				*IsNiTriBasedGeom() const;
	virtual class NiTriShape					*IsNiTriShape() const;
	virtual class NiParticles					*IsParticlesGeom() const;
	virtual class BSLines						*IsLinesGeom() const;
	virtual class bhkNiCollisionObject			*IsBhkNiCollisionObject() const;
	virtual class bhkBlendCollisionObject		*IsBhkBlendCollisionObject() const;
	virtual class bhkAttachmentCollisionObject	*IsBhkAttachmentCollisionObject() const;
	virtual class bhkRigidBody					*IsBhkRigidBody() const;
	virtual class bhkLimitedHingeConstraint		*IsBhkLimitedHingeConstraint() const;

	virtual NiObject *CreateClone(NiCloningProcess &);
	virtual void LoadBinary(NiStream &);
	virtual void LinkObject(NiStream &);
	virtual bool RegisterStreamables(NiStream &);
	virtual void SaveBinary(NiStream &);
	virtual bool IsEqual(NiObject *Other);
	virtual void ProcessClone(NiCloningProcess &);
	virtual void PostLinkObject(NiStream &);
	virtual bool StreamCanSkip();
	virtual const NiRTTI *GetStreamableRTTI();
	virtual unsigned int GetBlockAllocationSize();
	virtual NiObjectGroup *GetGroup();
	virtual void *SetGroup(NiObjectGroup *);
	virtual class NiControllerManager *IsNiControllerManager() const;

	void GetViewerStrings(void(*Callback)(const char *, ...), bool Recursive) const
	{
		if (Recursive)
			__super::GetViewerStrings(Callback, Recursive);

		Callback("-- NiObject --\n");
		Callback("NiRTTI = %s\n", GetRTTI()->GetName());
	}

	bool IsExactKindOf(NiRTTI *RTTI) const
	{
		return GetRTTI() == RTTI;
	}

	bool IsKindOf(NiRTTI *RTTI) const
	{
		for (auto currentRTTI = GetRTTI(); currentRTTI; currentRTTI = currentRTTI->GetBaseRTTI())
		{
			if (currentRTTI == RTTI)
				return true;
		}

		return false;
	}
};
static_assert(sizeof(NiObject) == 0x10);