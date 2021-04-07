#pragma once

#include "../../common.h"
#include "../TES/BSTArray.h"

extern BOOL bFogToggle;

struct z_stream_s
{
	LPCVOID next_in;
	uint32_t avail_in;
	uint32_t total_in;
	LPVOID next_out;
	uint32_t avail_out;
	uint32_t total_out;
	LPCSTR msg;
	struct internal_state *state;
};
static_assert_offset(z_stream_s, state, 0x28);

INT_PTR CALLBACK DialogFuncOverride(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND WINAPI hk_CreateDialogParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
INT_PTR WINAPI hk_DialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
BOOL WINAPI hk_EndDialog(HWND hDlg, INT_PTR nResult);
LRESULT WINAPI hk_SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

INT32 FIXAPI hk_inflateInit(z_stream_s *Stream, LPCSTR Version, INT32 Mode);
INT32 FIXAPI hk_inflate(z_stream_s *Stream, INT32 Flush);

uint32_t FIXAPI sub_1405B31C0(BSTArray<LPVOID>& Array, LPCVOID &Target);
uint32_t FIXAPI sub_1405B31C0_SSE41(BSTArray<LPVOID>& Array, LPCVOID &Target);

BOOL FIXAPI OpenPluginSaveDialog(HWND ParentWindow, LPCSTR BasePath, BOOL IsESM, LPSTR Buffer, uint32_t BufferSize, LPCSTR Directory);
VOID FIXAPI UpdateObjectWindowTreeView(LPVOID Thisptr, HWND ControlHandle, INT64 Unknown);
VOID FIXAPI UpdateCellViewCellList(LPVOID Thisptr, HWND ControlHandle, INT64 Unknown);
VOID FIXAPI UpdateCellViewObjectList(LPVOID Thisptr, HWND *ControlHandle);
VOID FIXAPI BeginUIDefer(VOID);
VOID FIXAPI EndUIDefer(VOID);
VOID FIXAPI InsertComboBoxItem(HWND ComboBoxHandle, LPCSTR DisplayText, LPVOID Value, BOOL AllowResize);
VOID FIXAPI InsertListViewItem(HWND ListViewHandle, LPVOID Parameter, BOOL UseImage, INT32 ItemIndex);

VOID FIXAPI QuitHandler(VOID);

VOID FIXAPI hk_call_140906407(INT64 a1, INT64 a2, INT64 a3);
BOOL FIXAPI hk_call_12E852C(HWND RichEditControl, LPCSTR Text);
VOID FIXAPI PatchTemplatedFormIterator(VOID);
VOID FIXAPI PatchFogToggle(VOID);

// pra bugs rax is zero (just a message)
VOID FIXAPI sub_24F236D(VOID);

template<typename T, BOOL Stable = FALSE>
VOID FIXAPI ArrayQuickSortRecursive(BSTArray<T>& Array, INT32(*SortFunction)(LPCVOID, LPCVOID))
{
	auto compare = [SortFunction](const T& A, const T& B)
	{
		return SortFunction(A, B) == -1;
	};

	if constexpr (Stable)
		std::stable_sort(&Array[0], &Array[Array.QSize()], compare);
	else
		std::sort(&Array[0], &Array[Array.QSize()], compare);
}