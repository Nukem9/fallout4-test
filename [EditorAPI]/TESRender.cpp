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

#include "..\[Common]\StdAfx.h"
#include "..\[Common]\D3D11Proxy.h"
#include "TESRender.h"
#include "Settings.h"
#include "Core.h"

#include "..\[Patches]\CKF4\Editor.h"
#include "..\[Patches]\CKF4\EditorUI.h"
#include "..\[Patches]\CKF4\RenderWindow.h"
#include "..\[Patches]\CKF4\EditorUIProgressDialog.h"
#include "..\[Patches]\CKF4\MainWindow.h"
#include "..\[Patches]\CKF4\LayersWindow.h"

HWND g_RenderWindowHandle = NULL;
TESObjectREFR* g_CaptureREFR = NULL;
BGSRenderWindowMouseInputManager* g_RenderWindowMouseInputManager;
NiPoint3 g_MoveOffsetSafe;

__hotkey_t HK_RefreshT;

extern ID3D11Device* g_Device;
extern IDXGISwapChain* g_SwapChain;
extern ID3D11DeviceContext* g_DeviceContext;

TESObjectREFR* Helpers::GetRef(DWORD dwUniqueId) {
	if (!dwUniqueId)
		return NULL;
	
	auto RefArrayUnk = (DWORD_PTR)OFFSET(0x46CD050, 0);

	auto RefPtr = *(DWORD_PTR*)((RefArrayUnk + (((DWORD_PTR)dwUniqueId & 0x1FFFFF) << 4)) + 0x8);
	if (!RefPtr)
		return NULL;

	auto Ret = (TESObjectREFR*)(RefPtr - 48);
	return ((TESForm::FormType)*((BYTE*)((DWORD_PTR)Ret + 0x26))) == TESForm::ftReference ? Ret : NULL;
}

LPCSTR BGSRenderWindowCamera::GetViewMode(VOID) const {
	return thisCall<LPCSTR>(0x470860, this);
}

LPCSTR BGSRenderWindowCamera::GetCameraMode(BOOL Unk) const {
	return thisCall<LPCSTR>(0x470880, this, Unk);
}

BOOL BGSRenderWindowMouseInputManager::IsMouseInClientArea(VOID) const {
	auto rndClass = TESRENDER;
	if (!rndClass)
		return FALSE;

	if (GetForegroundWindow() != rndClass->RenderWindowHandle)
		return FALSE;

	RECT rcClient;
	GetClientRect(rndClass->RenderWindowHandle, &rcClient);

	POINT pMouse;
	GetCursorPos(&pMouse);
	ScreenToClient(rndClass->RenderWindowHandle, &pMouse);

	if (pMouse.x < 0 || pMouse.y < 0 || rcClient.right < pMouse.x || rcClient.bottom < pMouse.y)
		return FALSE;
	
	return TRUE;
}

BGSRenderWindowMouseInputManager* BGSRenderWindowMouseInputManager::CreateInstance(VOID) {
	if (g_RenderWindowMouseInputManager)
		return g_RenderWindowMouseInputManager;

	g_RenderWindowMouseInputManager = new BGSRenderWindowMouseInputManager();
	return g_RenderWindowMouseInputManager;
}

BGSRenderWindowMouseInputManager* BGSRenderWindowMouseInputManager::GetInstance(VOID) {
	return g_RenderWindowMouseInputManager;
}

VOID BGSRenderWindowReferenceEditModule::hk_MoveObjects(NiPoint3& Offset) {
	NiPoint3* current_addpos_to_form = (NiPoint3*)((char*)(this) + 0x70);
	//*current_addpos_to_form += Offset;
	/*
	NiPoint3 offset;
	offset.x = *((float*)(OFFSET(0x9AF1988, 0)));
	offset.y = *((float*)(OFFSET(0x9AF198C, 0)));
	offset.z = *((float*)(OFFSET(0x9AF1990, 0)));
		*/
	/*auto Render = BGSRenderWindow::GetInstanceWithoutHWND();
	auto Picker = Render->GetPicker();


	LPVOID Unk1 = NULL;
	DWORD64 UniqueOff = 0;
	fastCall<VOID>(0x43A3D0, Picker, &Unk1);

	if (fastCall<BOOL>(0x43A400, Picker, &UniqueOff, &Unk1)) {
		do {
			auto ObjRef = Helpers::GetRef(UniqueOff);
			if (ObjRef) {
				auto Node = thisVirtualCall<BSFadeNode*>(0x558, ObjRef);
				if (Node)
					Node->IncrementReference();
				
				ObjRef->Position += *current_addpos_to_form;
				//Node
				
				ObjRef->SetPosition();

				_MESSAGE_FMT("test %ull", Unk2);

				if (Node)
					Node->DecrementReference();
			}
		} while (fastCall<BOOL>(0x43A400, Picker, &UniqueOff, &Unk1));
	}*/
}

VOID BGSRenderWindow::hk_InitializeRender(HWND WindowHandle) {
	// The window handle is known too late, most of the code has been transferred here. 
	// All modules that have links to the render must include ui.h module.
	
	g_RenderWindowHandle = WindowHandle;
	InitializeRender(this, WindowHandle);
}

TESObjectREFR* FIXAPI BGSRenderWindow::hk_GetREFRByMouseMove(LPVOID UnkNode) {
	g_CaptureREFR = fastCall<TESObjectREFR*>(0xE21DE0, UnkNode);
	return g_CaptureREFR;
}

HCURSOR FIXAPI BGSRenderWindow::hk_SetCursor(HCURSOR handle) {
	if (handle == TESRENDER->GetDefaultCursor())
		g_CaptureREFR = NULL;

	return SetCursor(handle);
}

VOID BGSRenderWindow::InitializeHook(VOID) {
	*(uintptr_t*)&BGSRenderWindow::InitializeRender = Detours::X64::DetourFunctionClass(OFFSET(0x462E60, 0), &BGSRenderWindow::hk_InitializeRender);
	XUtil::DetourCall(OFFSET(0x4CD9FB, 0), &hk_GetREFRByMouseMove);
	XUtil::DetourCall(OFFSET(0x463927, 0), &hk_SetCursor);
}

BGSRenderWindow* BGSRenderWindow::GetInstance(VOID) {
	// The main render class is in conjunction with the window, which makes it a little more complicated, 
	// you already need to know the window handle.

	// In this regard, this function should be called only after initialization of all windows and called inside them for rendering.
	// In all other cases, exclude the use of this function.
	return (BGSRenderWindow*)GetWindowLongPtrA(g_RenderWindowHandle, GWLP_USERDATA);
}

BGSRenderWindow* BGSRenderWindow::GetInstanceWithoutHWND(VOID) {
	return *((BGSRenderWindow**)OFFSET(0x6D54D68, 0));
}

TESObjectREFR* BGSRenderWindow::GetCaptureREFROnScene(VOID) {
	return g_CaptureREFR;
}

VOID BGSRenderWindow::GetCellGeomDescription(TESObjectCELL* Cell, LPSTR Buffer) {
	// fastCall<VOID>(0x7B3570, Cell, buffer);

	// This function has been changed, moreover, it no longer accepts TESObjectCELL, 
	// so we will develop our own function.

	if (Cell) {
		if (Cell->TypeID != TESObjectCELL::ftCell)
			goto no_cell;
		
		DWORD uCountObject = 0;
		DWORD uCountLight = 0;
		DWORD uCountNPCs = 0;
		DWORD uCountCreatures = 0;
		DWORD uCountSelObject = 0;
		DWORD uCountSelLight = 0;
		DWORD uCountSelNPCs = 0;

		auto Counter = [](TESObjectREFR** refrs, DWORD count, DWORD& npcs, DWORD& lights, DWORD& objs) {
			DWORD uId;
			npcs = 0;
			lights = 0;
			objs = 0;

			for (uId = 0; uId < count; uId++) {
				auto form = refrs[uId];
				auto formType = form->Parent->TypeID;

				if (!form->IsDeleted()) {
					switch (formType) {
					case TESForm::ftActor:
						npcs++;
						break;
					case TESForm::ftLight:
						lights++;
						break;
					default:
						objs++;
						break;
					}
				}
			}
		};

		Counter(Cell->GetItems(), Cell->GetItemsCount(), uCountNPCs, uCountLight, uCountObject);

		auto picker = TESPICKER;
		auto items = picker->Items;
		if (items && picker->SelectCount > 0) {
			DWORD i = 0;
			for (auto item = items->First; i < picker->SelectCount; item = item->Next, i++) {
				auto form = item->GetREFR();
				auto formType = form->Parent->TypeID;

				if (!form->IsDeleted()) {
					switch (formType) {
					case TESForm::ftActor:
						uCountSelNPCs++;
						break;
					case TESForm::ftLight:
						uCountSelLight++;
						break;
					default:
						uCountSelObject++;
						break;
					}
				}
			}
		}

		sprintf(Buffer, "\tObjects: %d\n\tLights: %d\n\tNPCs: %d\nSelected:\n\tObjects: %d\n\tLights: %d\n\tNPCs: %d",
			uCountObject, uCountLight, uCountNPCs, uCountSelObject, uCountSelLight, uCountSelNPCs);
	}
	else
no_cell:
		strcpy(Buffer, "No Cell");
}

VOID BGSRenderWindow::UpdateCaptionRenderWindow(VOID) {
	if (ActiveCell)
		SetWindowTextA(RenderWindowHandle, *ActiveCell->EditID);
	else
		SetWindowTextA(RenderWindowHandle, "Render Window");
}

VOID BGSRenderWindow::MouseMoveHandler(WORD x, WORD y) const {
	thisCall<VOID>(0x463530, this, x, y);
}

VOID BGSRenderWindow::SetCell(TESObjectCELL* cell, BOOL Unk) const {
	if (!cell || cell->IsActive())
		return;
	
	std::mutex lock;
	lock.lock();

	// create progressbar on taskbar
	auto ptb = IProgressTaskBar::CreateInstance();
	if (ptb) ptb->SetMarquee(TRUE);

	auto layerWnd = LayersWindow::GetWindow();
	auto menuView = MainWindow::GetWindowObj().MainMenu.GetSubMenuItem(2);
	auto menuItem = menuView.GetItemByPos(9);
	auto layerExists = IsWindow(layerWnd);

	if (layerExists) {
		//SendMessageA(layerWnd, WM_CLOSE, 0, 0);
		//IProcessMessage::ProcessMessage(NULL, 0, 0, IProcessMessage::RemoveMessage | 
		//	IProcessMessage::ProcessingSends | IProcessMessage::NoYield);
		//IProcessMessage::ProcessMessage(NULL, WM_DESTROY, WM_NCDESTROY, IProcessMessage::RemoveMessage | IProcessMessage::NoYield);
	}

	if (!cell->IsInterior())
		EditorUI::SimpleDlgPleaseWait::Show();

	thisCall<VOID>(0x45FE60, this, cell, Unk);
	
	if (!cell->IsInterior())
		EditorUI::SimpleDlgPleaseWait::Close();

	//if (layerExists)
	//	menuItem.Click();

	// enabled all markers
	menuItem = menuView.GetItemByPos(15);
	if (!menuItem.Checked)
		menuItem.Click();

	// enabled light markers
	menuItem = menuView.GetItemByPos(16);
	if (!menuItem.Checked)
		menuItem.Click();

	// enabled sound markers
	menuItem = menuView.GetItemByPos(17);
	if (!menuItem.Checked)
		menuItem.Click();

	Sleep(1);
	
	SetActiveWindow(RenderWindowHandle);
	SetForegroundWindow(RenderWindowHandle);

	// close Progress on taskbar
	IProgressTaskBar::DeleteInstance();

	lock.unlock();
}

VOID BGSRenderWindow::Refresh3D(VOID) {
	if (GetCapture() != RenderWindowHandle)
		return;
	
	auto view = MainWindow::GetWindowObj().MainMenu.GetSubMenuItem(2);


	auto scanShift = MapVirtualKeyA(VK_SHIFT, MAPVK_VK_TO_VSC);
	auto scanAlt = MapVirtualKeyA(VK_MENU, MAPVK_VK_TO_VSC);
	auto scanCtrl = MapVirtualKeyA(VK_CONTROL, MAPVK_VK_TO_VSC);

	if (HK_RefreshT.Shift)
		keybd_event(VK_SHIFT, scanShift, 0, 0);
	if (HK_RefreshT.Alt)
		keybd_event(VK_MENU, scanAlt, 0, 0);
	if (HK_RefreshT.Ctrl)
		keybd_event(VK_CONTROL, scanCtrl, 0, 0);

	keybd_event(HK_RefreshT.VirtualKey, MapVirtualKeyA(HK_RefreshT.VirtualKey, MAPVK_VK_TO_VSC), KEYEVENTF_EXTENDEDKEY, 0);
	keybd_event(HK_RefreshT.VirtualKey, MapVirtualKeyA(HK_RefreshT.VirtualKey, MAPVK_VK_TO_VSC), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	
	if (HK_RefreshT.Ctrl)
		keybd_event(VK_CONTROL, scanCtrl, KEYEVENTF_KEYUP, 0);
	if (HK_RefreshT.Alt)
		keybd_event(VK_MENU, scanAlt, KEYEVENTF_KEYUP, 0);
	if (HK_RefreshT.Shift)
		keybd_event(VK_SHIFT, scanShift, KEYEVENTF_KEYUP, 0);
}

DWORD* BGSRenderWindow::GetStateFlags(VOID) const {
	return (DWORD*)OFFSET(0x6D54F40, 0);
}

VOID BGSRenderWindow::SetSnapToGrid(BOOL val) {
	if (val == IsSnapToGrid())
		return;

	if (val)
		SetSnapToConnectPoints(FALSE);

	auto Flags = GetStateFlags();
	if (val)
		*Flags |= kRenderWindowState_SnapToGrid;
	else
		*Flags &= ~kRenderWindowState_SnapToGrid;

	hk_SendMessageA(MainWindow::GetToolbarWindow(), TB_CHECKBUTTON, 0x9CC6, IsSnapToGrid());
}

VOID BGSRenderWindow::SetSnapToAngle(BOOL val) {
	if (val == IsSnapToAngle())
		return;

	auto Flags = GetStateFlags();
	if (val)
		*Flags |= kRenderWindowState_SnapToAngle;
	else
		*Flags &= ~kRenderWindowState_SnapToAngle;

	hk_SendMessageA(MainWindow::GetToolbarWindow(), TB_CHECKBUTTON, 0x9CC7, IsSnapToAngle());
}

VOID BGSRenderWindow::SetSnapToConnectPoints(BOOL val) {
	if (val == IsSnapToConnectPoints())
		return;

	if (val)
		SetSnapToGrid(FALSE);
	
	auto Flags = GetStateFlags();
	if (val)
		*Flags |= kRenderWindowState_SnapToConnectPoints;
	else
		*Flags &= ~kRenderWindowState_SnapToConnectPoints;

	hk_SendMessageA(MainWindow::GetToolbarWindow(), TB_CHECKBUTTON, 0xA023, IsSnapToConnectPoints());
}

DWORD BGSRenderWindow::GetMovementSnapToGrid(VOID) const {
	auto MovementSetting = (Setting*)(OFFSET(0x6D54F50, 0));
	return MovementSetting->GetUnsignedInt();
}

VOID BGSRenderWindow::SetMovementSnapToGrid(DWORD val) {
	auto MovementSetting = (Setting*)(OFFSET(0x6D54F50, 0));
	MovementSetting->SetUnsignedInt(val);
	MovementSetting->UpdateLinks();
}

FLOAT BGSRenderWindow::GetMovementSnapToAngle(VOID) const {
	auto MovementSetting = (Setting*)(OFFSET(0x6D54F68, 0));
	return MovementSetting->GetFloat();
}

VOID BGSRenderWindow::SetMovementSnapToAngle(FLOAT val) {
	auto MovementSetting = (Setting*)(OFFSET(0x6D54F68, 0));
	MovementSetting->SetFloat(val);
	MovementSetting->UpdateLinks();
}

TESObjectREFR* BGSRenderWindow::GetSnapReference(VOID) const {
	return Helpers::GetRef(*(DWORD*)OFFSET(0x6D54F30, 0));
}

VOID BGSRenderWindow::ShowSnapReference(VOID) const {
	auto RefStorage = (DWORD*)OFFSET(0x6D54F30, 0);
	
	DWORD RefID = 0;
	DWORD_PTR OldRefID = *RefStorage;

	auto Ret = fastCall<DWORD&>(0x668F40, &RefID, RenderWindowHandle, &OldRefID, 0);
	*RefStorage = Ret;
}

VOID BGSRenderWindow::ClearSnapReference(VOID) const {
	auto RefStorage = (DWORD*)OFFSET(0x6D54F30, 0);
	*RefStorage = 0;
}

FLOAT BGSRenderWindow::GetMovementSpeed(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x6D54FB0, 0));
	return SettingV->GetFloat();
}

VOID BGSRenderWindow::SetMovementSpeed(FLOAT val) {
	auto SettingV = (Setting*)(OFFSET(0x6D54FB0, 0));
	SettingV->SetFloat(val);
	SettingV->UpdateLinks();
}

FLOAT BGSRenderWindow::GetRotationSpeed(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x6D54F98, 0));
	return SettingV->GetFloat();
}

VOID BGSRenderWindow::SetRotationSpeed(FLOAT val) {
	auto SettingV = (Setting*)(OFFSET(0x6D54F98, 0));
	SettingV->SetFloat(val);
	SettingV->UpdateLinks();
}

FLOAT BGSRenderWindow::GetCameraRotationSpeed(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x6D54FC8, 0));
	return SettingV->GetFloat();
}

VOID BGSRenderWindow::SetCameraRotationSpeed(FLOAT val) {
	auto SettingV = (Setting*)(OFFSET(0x6D54FC8, 0));
	SettingV->SetFloat(val);
	SettingV->UpdateLinks();
}

FLOAT BGSRenderWindow::GetCameraZoomSpeed(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x6D54FE0, 0));
	return SettingV->GetFloat();
}

VOID BGSRenderWindow::SetCameraZoomSpeed(FLOAT val) {
	auto SettingV = (Setting*)(OFFSET(0x6D54FE0, 0));
	SettingV->SetFloat(val);
	SettingV->UpdateLinks();
}

FLOAT BGSRenderWindow::GetCameraOrthoZoomSpeed(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x6D54FF8, 0));
	return SettingV->GetFloat();
}

VOID BGSRenderWindow::SetCameraOrthoZoomSpeed(FLOAT val) {
	auto SettingV = (Setting*)(OFFSET(0x6D54FF8, 0));
	SettingV->SetFloat(val);
	SettingV->UpdateLinks();
}

FLOAT BGSRenderWindow::GetCameraPanSpeed(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x6D55010, 0));
	return SettingV->GetFloat();
}

VOID BGSRenderWindow::SetCameraPanSpeed(FLOAT val) {
	auto SettingV = (Setting*)(OFFSET(0x6D55010, 0));
	SettingV->SetFloat(val);
	SettingV->UpdateLinks();
}

BOOL BGSRenderWindow::GetCameraRotationInvertX(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x452C408, 0));
	return SettingV->GetBool();
}

VOID BGSRenderWindow::SetCameraRotationInvertX(BOOL val) {
	auto SettingV = (Setting*)(OFFSET(0x452C408, 0));
	SettingV->SetBool(val);
	SettingV->UpdateLinks();
}

BOOL BGSRenderWindow::GetCameraRotationInvertY(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x452C420, 0));
	return SettingV->GetBool();
}

VOID BGSRenderWindow::SetCameraRotationInvertY(BOOL val) {
	auto SettingV = (Setting*)(OFFSET(0x452C420, 0));
	SettingV->SetBool(val);
	SettingV->UpdateLinks();
}

BOOL BGSRenderWindow::GetCameraPanInvertX(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x452C438, 0));
	return SettingV->GetBool();
}

VOID BGSRenderWindow::SetCameraPanInvertX(BOOL val) {
	auto SettingV = (Setting*)(OFFSET(0x452C438, 0));
	SettingV->SetBool(val);
	SettingV->UpdateLinks();
}

BOOL BGSRenderWindow::GetCameraPanInvertY(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x452C450, 0));
	return SettingV->GetBool();
}

VOID BGSRenderWindow::SetCameraPanInvertY(BOOL val) {
	auto SettingV = (Setting*)(OFFSET(0x452C450, 0));
	SettingV->SetBool(val);
	SettingV->UpdateLinks();
}

BOOL BGSRenderWindow::GetCameraZoomWithV(VOID) const {
	auto SettingV = (Setting*)(OFFSET(0x452C4B0, 0));
	return SettingV->GetBool();
}

VOID BGSRenderWindow::SetCameraZoomWithV(BOOL val) {
	auto SettingV = (Setting*)(OFFSET(0x452C4B0, 0));
	SettingV->SetBool(val);
	SettingV->UpdateLinks();
}