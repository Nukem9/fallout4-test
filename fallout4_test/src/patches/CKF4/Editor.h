#pragma once

#include "../TES/BSTArray.h"

struct z_stream_s
{
	const void *next_in;
	uint32_t avail_in;
	uint32_t total_in;
	void *next_out;
	uint32_t avail_out;
	uint32_t total_out;
	const char *msg;
	struct internal_state *state;
};
static_assert_offset(z_stream_s, state, 0x28);

INT_PTR CALLBACK DialogFuncOverride(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND WINAPI hk_CreateDialogParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
INT_PTR WINAPI hk_DialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
BOOL WINAPI hk_EndDialog(HWND hDlg, INT_PTR nResult);
LRESULT WINAPI hk_SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int hk_inflateInit(z_stream_s *Stream, const char *Version, int Mode);
int hk_inflate(z_stream_s *Stream, int Flush);

uint32_t sub_1414974E0(BSTArray<void *>& Array, const void *&Target, uint32_t StartIndex, __int64 Unused);
uint32_t sub_1414974E0_SSE41(BSTArray<void *>& Array, const void *&Target, uint32_t StartIndex, __int64 Unused);

void UpdateObjectWindowTreeView(void *Thisptr, HWND ControlHandle, __int64 Unknown);
void UpdateCellViewCellList(void *Thisptr, HWND ControlHandle, __int64 Unknown);
void UpdateCellViewObjectList(void *Thisptr, HWND *ControlHandle);
void BeginUIDefer();
void EndUIDefer();
void InsertComboBoxItem(HWND ComboBoxHandle, const char *DisplayText, void *Value, bool AllowResize);
void InsertListViewItem(HWND ListViewHandle, void *Parameter, bool UseImage, int ItemIndex);

void QuitHandler();

void hk_call_140906407(__int64 a1, __int64 a2, __int64 a3);
void PatchTemplatedFormIterator();