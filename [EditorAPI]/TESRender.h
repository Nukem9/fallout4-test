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
#pragma pack(push, 1)

#include "..\[Common]\CommInc.h"
#include "BSEntryString.h"
#include "NiTypes.h"
#include "BSTArray.h"
#include "BSTList.h"
#include "TESObjectCELL.h"

class NiNode {
public:
	virtual ~NiNode(VOID);
};

class BSFadeNode : public NiNode {
private:
	volatile LONG64 CountRef;
public:
	INLINE VOID IncrementReference(VOID) { InterlockedIncrement64(&CountRef); }
	INLINE VOID DecrementReference(VOID) { InterlockedDecrement64(&CountRef); }
};

class BSCullingProcess {
public:
	virtual ~BSCullingProcess(VOID);
};

// RVA 0x3C8A8D8
class NiCamera {
public:
	virtual ~NiCamera(VOID);
};

// 
class Renderer {
public:
	virtual VOID Render(NiCamera* Camera = NULL, NiNode* NodeToRender = NULL, BSCullingProcess* CullingProcess = NULL);
};

// RAV 0x3850DA8 (Should be size 0x210)
class NiWindow : public Renderer {
public:
	typedef BSSimpleList<NiPointer<NiCamera>> RenderCameraListT;
public:
	// members
	/*08*/ NiNode* sceneRootNode;					// smart pointer
	/*10*/ DWORD64	renderedFrameCount;				// incremented in Render()
	/*18*/ RenderCameraListT attachedCameras;
public:
	// methods
	virtual VOID SetSceneNode(NiNode* Node) = 0;
	virtual NiCamera* AddCamera(NiCamera** OutCamera) = 0;
	virtual BOOL HandleResize(VOID) = 0;
	virtual BOOL UpdateCameraFrustum(NiCamera* Camera, INT32 Width, INT32 Height, float ClipDistance) = 0;
};

// RVA 0x3843288
class BGSRenderWindowCamera {
public:
	virtual ~BGSRenderWindowCamera(VOID);
private:
	NiNode** UnkNode;
public:
	INLINE NiPoint4 GetPosition(VOID) const {
		return *((NiPoint4*)((DWORD_PTR)(UnkNode) + 0x60));
	}
	LPCSTR GetViewMode(VOID) const;
	LPCSTR GetCameraMode(BOOL Unk = FALSE) const;
public:
	READ_PROPERTY(GetPosition) NiPoint4 Position;
	READ_PROPERTY(GetViewMode) LPCSTR ViewMode;
};

class Helpers {
public:
	static TESObjectREFR* GetRef(DWORD dwUniqueId);
};

// 14
class BGSPickerNode {
protected:
	BGSPickerNode* next;
	BGSPickerNode* prev;
	DWORD index;
public:
	BGSPickerNode(VOID) = default;
public:
	INLINE BGSPickerNode* GetNext(VOID) const { return next; }
	INLINE BGSPickerNode* GetPrev(VOID) const { return prev; }
	INLINE DWORD GetIndex(VOID) const { return index; }
public:
	INLINE TESObjectREFR* GetREFR(VOID) const { return Helpers::GetRef(Index); }
public:
	READ_PROPERTY(GetNext) BGSPickerNode* Next;
	READ_PROPERTY(GetPrev) BGSPickerNode* Prev;
	READ_PROPERTY(GetIndex) DWORD Index;
	READ_PROPERTY(GetREFR) TESObjectREFR* REFR;
};
static_assert(sizeof(BGSPickerNode) == 0x14);

// 14
class BGSPickerList : public BGSPickerNode {
public:
	INLINE BOOL IsEmpty(VOID) const { return next == prev;  }
	INLINE BGSPickerNode* GetFirst(VOID) const { return next; }
	INLINE BGSPickerNode* GetEnd(VOID) const { return prev; }
public:
	READ_PROPERTY(GetFirst) BGSPickerNode* First;
	READ_PROPERTY(GetEnd) BGSPickerNode* End;
};
static_assert(sizeof(BGSPickerList) == 0x14);

typedef class BGSPicker {
private:
	DWORD _SelectCount;
	NiPoint4 _CentralSelectionPosition;
	CHAR pad14[0x2C];
	BGSPickerList* _ItemList;
public:
	BGSPicker(VOID) = default;
public:
	INLINE DWORD GetSelectCount(VOID) const { return _SelectCount; }
	INLINE NiPoint4 GetCentralSelectionPosition(VOID) const { return _CentralSelectionPosition; }
	INLINE const BGSPickerList* GetItems(VOID) const { return _ItemList; }
public:
	READ_PROPERTY(GetSelectCount) DWORD SelectCount;
	READ_PROPERTY(GetCentralSelectionPosition) NiPoint4 CentralSelectionPosition;
	READ_PROPERTY(GetItems) const BGSPickerList* Items;
} BGSPicker;
static_assert(sizeof(BGSPicker) == 0x48);

class BGSRenderWindowMouseInputManager {
public:
	BOOL IsMouseInClientArea(VOID) const;
public:
	static BGSRenderWindowMouseInputManager* CreateInstance(VOID);
	static BGSRenderWindowMouseInputManager* GetInstance(VOID);
};

class BGSRenderWindowReferenceEditModule {
private:
	CHAR _pad8[0x68];		// 0x08
	NiPoint3 _EarlyOffset;	// 0x70
public:
	virtual ~BGSRenderWindowReferenceEditModule(VOID);
public:
	VOID hk_MoveObjects(NiPoint3& Offset);
	inline static VOID(*MoveObjects)(BGSRenderWindowReferenceEditModule*, NiPoint3*);
public:
	READ_PROPERTY2(NiPoint3, EarlyOffset, _EarlyOffset);
};

class BGSRenderWindow {
public:
	~BGSRenderWindow(VOID);
public:
	// Since the days of CSE, they have not changed the timer ID
	enum { kTimer_ViewportUpdate = 1 };
	enum {
		kRenderWindowState_SnapToGrid			= 0x1,
		kRenderWindowState_SnapToAngle			= 0x2,
		kRenderWindowState_SnapToConnectPoints	= 0x8,
	};
private:
	inline static VOID(*InitializeRender)(BGSRenderWindow*, HWND);
	VOID hk_InitializeRender(HWND WindowHandle);
	static TESObjectREFR* FIXAPI hk_GetREFRByMouseMove(LPVOID UnkNode);
	static HCURSOR FIXAPI hk_SetCursor(HCURSOR handle);
public:
	// DON'T USE!!! This function is performed once and only inside the DirectX11 patch.
	static VOID InitializeHook(VOID);
	// The main render class is in conjunction with the window, which makes it a little more complicated, 
	// you already need to know the window handle.
	// In this regard, this function should be called only after initialization of all windows and called inside them for rendering.
	// In all other cases, exclude the use of this function.
	static BGSRenderWindow* GetInstance(VOID);
	static BGSRenderWindow* GetInstanceWithoutHWND(VOID);

	static TESObjectREFR* GetCaptureREFROnScene(VOID);
	static VOID GetCellGeomDescription(TESObjectCELL* Cell, LPSTR Buffer);
public:
	VOID UpdateCaptionRenderWindow(VOID);
	VOID MouseMoveHandler(WORD x, WORD y) const;
	VOID SetCell(TESObjectCELL* cell, BOOL Unk = TRUE) const;
	VOID Refresh3D(VOID);
	DWORD* GetStateFlags(VOID) const;
public:
	//////////////////////////////
	/// SNAP

	INLINE BOOL IsSnapToGrid(VOID) const { return (*GetStateFlags() & kRenderWindowState_SnapToGrid); }
	INLINE BOOL IsSnapToAngle(VOID) const { return (*GetStateFlags() & kRenderWindowState_SnapToAngle); }
	INLINE BOOL IsSnapToConnectPoints(VOID) const { return (*GetStateFlags() & kRenderWindowState_SnapToConnectPoints); }
	VOID SetSnapToGrid(BOOL val);
	VOID SetSnapToAngle(BOOL val);
	VOID SetSnapToConnectPoints(BOOL val);
	DWORD GetMovementSnapToGrid(VOID) const;
	VOID SetMovementSnapToGrid(DWORD val);
	PROPERTY(GetMovementSnapToGrid, SetMovementSnapToGrid) DWORD MovementSnapToGrid;
	FLOAT GetMovementSnapToAngle(VOID) const;
	VOID SetMovementSnapToAngle(FLOAT val);
	PROPERTY(GetMovementSnapToAngle, SetMovementSnapToAngle) FLOAT MovementSnapToAngle;
	TESObjectREFR* GetSnapReference(VOID) const;
	READ_PROPERTY(GetSnapReference) TESObjectREFR* SnapReference;
	VOID ShowSnapReference(VOID) const;
	VOID ClearSnapReference(VOID) const;
	//////////////////////////////
	/// CAMERA MOVEMENT

	FLOAT GetMovementSpeed(VOID) const;
	VOID SetMovementSpeed(FLOAT val);
	PROPERTY(GetMovementSpeed, SetMovementSpeed) FLOAT MovementSpeed;
	FLOAT GetRotationSpeed(VOID) const;
	VOID SetRotationSpeed(FLOAT val);
	PROPERTY(GetRotationSpeed, SetRotationSpeed) FLOAT RotationSpeed;
	FLOAT GetCameraRotationSpeed(VOID) const;
	VOID SetCameraRotationSpeed(FLOAT val);
	PROPERTY(GetCameraRotationSpeed, SetCameraRotationSpeed) FLOAT CameraRotationSpeed;
	BOOL GetCameraRotationInvertX(VOID) const;
	VOID SetCameraRotationInvertX(BOOL val);
	PROPERTY(GetCameraRotationInvertX, SetCameraRotationInvertX) BOOL CameraRotationInvertX;
	BOOL GetCameraRotationInvertY(VOID) const;
	VOID SetCameraRotationInvertY(BOOL val);
	PROPERTY(GetCameraRotationInvertY, SetCameraRotationInvertY) BOOL CameraRotationInvertY;
	FLOAT GetCameraZoomSpeed(VOID) const;
	VOID SetCameraZoomSpeed(FLOAT val);
	PROPERTY(GetCameraZoomSpeed, SetCameraZoomSpeed) FLOAT CameraZoomSpeed;
	FLOAT GetCameraOrthoZoomSpeed(VOID) const;
	VOID SetCameraOrthoZoomSpeed(FLOAT val);
	PROPERTY(GetCameraOrthoZoomSpeed, SetCameraOrthoZoomSpeed) FLOAT CameraOrthoZoomSpeed;
	FLOAT GetCameraPanSpeed(VOID) const;
	VOID SetCameraPanSpeed(FLOAT val);
	PROPERTY(GetCameraPanSpeed, SetCameraPanSpeed) FLOAT CameraPanSpeed;
	BOOL GetCameraPanInvertX(VOID) const;
	VOID SetCameraPanInvertX(BOOL val);
	PROPERTY(GetCameraPanInvertX, SetCameraPanInvertX) BOOL CameraPanInvertX;
	BOOL GetCameraPanInvertY(VOID) const;
	VOID SetCameraPanInvertY(BOOL val);
	PROPERTY(GetCameraPanInvertY, SetCameraPanInvertY) BOOL CameraPanInvertY;
	BOOL GetCameraZoomWithV(VOID) const;
	VOID SetCameraZoomWithV(BOOL val);
	PROPERTY(GetCameraZoomWithV, SetCameraZoomWithV) BOOL CameraZoomWithV;
public:
	INLINE BGSPicker* GetPicker(VOID) const { return *((BGSPicker**)((DWORD_PTR)this + 0xD0)); }
	INLINE HWND GetRenderWindowHandle(VOID) const { return *((HWND*)((DWORD_PTR)this + 0x70)); }
	INLINE HCURSOR GetRenderCursor(VOID) const { return *((HCURSOR*)((DWORD_PTR)this + 0x78)); }
	INLINE HCURSOR GetDefaultCursor(VOID) const { return *((HCURSOR*)((DWORD_PTR)this + 0x80)); }
	INLINE BGSRenderWindowCamera* GetCamera(VOID) const { return *((BGSRenderWindowCamera**)((DWORD_PTR)this + 0xC0)); }
	INLINE NiWindow* GetWindow(VOID) const { return *((NiWindow**)((DWORD_PTR)this + 0xC8)); }
	INLINE NiPoint2T<DWORD>* GetWindowSize(VOID) const { return ((NiPoint2T<DWORD>*)((DWORD_PTR)this + 0x104)); }
	INLINE NiPoint2T<DWORD>* GetMousePos(VOID) const { return ((NiPoint2T<DWORD>*)((DWORD_PTR)this + 0x10C)); }
	INLINE NiPoint2T<DWORD>* GetMousePosBefore(VOID) const { return ((NiPoint2T<DWORD>*)((DWORD_PTR)this + 0x114)); }
	// I have been searching for at least some information about the current cell for a long time, 
	// but previously I could only know about the interiors. Now it will be easier, although I will not change the code written earlier, it works.
	INLINE TESObjectCELL* GetActiveCell(VOID) const { return *((TESObjectCELL**)((DWORD_PTR)this + 0xF8)); }
public:
	READ_PROPERTY(GetRenderWindowHandle) HWND RenderWindowHandle;
	READ_PROPERTY(GetWindow) NiWindow* Window;
	READ_PROPERTY(GetWindowSize) NiPoint2T<DWORD>* WindowSize;
	READ_PROPERTY(GetMousePos) NiPoint2T<DWORD>* MousePos;
	READ_PROPERTY(GetMousePosBefore) NiPoint2T<DWORD>* MousePos2;
	READ_PROPERTY(GetCamera) BGSRenderWindowCamera* Camera;
	READ_PROPERTY(GetActiveCell) TESObjectCELL* ActiveCell;
};

// The main class responsible for rendering is available after initialization of RenderWindow
#define TESRENDER (BGSRenderWindow::GetInstanceWithoutHWND())
// --- The data about the selected objects is kept for a fairly short time, a mutex thread lock is required
// Fixed: Used Helpers class 
#define TESPICKER (TESRENDER->GetPicker())

#define TESRENDERMOUSEMNG (BGSRenderWindowMouseInputManager::GetInstance())

#pragma pack(pop)