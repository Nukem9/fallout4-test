#include "../../common.h"
#include <libdeflate/libdeflate.h>
#include <xbyak/xbyak.h>
#include <CommCtrl.h>
#include <smmintrin.h>
#include "Editor.h"
#include "LogWindow.h"

#pragma comment(lib, "libdeflate.lib")

struct DialogOverrideData
{
	DLGPROC DialogFunc;	// Original function pointer
	bool IsDialog;		// True if it requires EndDialog()
};

std::recursive_mutex g_DialogMutex;
std::unordered_map<HWND, DialogOverrideData> g_DialogOverrides;
thread_local DialogOverrideData DlgData;

INT_PTR CALLBACK DialogFuncOverride(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DLGPROC proc = nullptr;

	g_DialogMutex.lock();
	{
		if (auto itr = g_DialogOverrides.find(hwndDlg); itr != g_DialogOverrides.end())
			proc = itr->second.DialogFunc;

		// if (is new entry)
		if (!proc)
		{
			g_DialogOverrides[hwndDlg] = DlgData;
			proc = DlgData.DialogFunc;

			DlgData.DialogFunc = nullptr;
			DlgData.IsDialog = false;
		}

		// Purge old entries every now and then
		if (g_DialogOverrides.size() >= 50)
		{
			for (auto itr = g_DialogOverrides.begin(); itr != g_DialogOverrides.end();)
			{
				if (itr->first == hwndDlg || IsWindow(itr->first))
				{
					itr++;
					continue;
				}

				itr = g_DialogOverrides.erase(itr);
			}
		}
	}
	g_DialogMutex.unlock();

	return proc(hwndDlg, uMsg, wParam, lParam);
}

HWND WINAPI hk_CreateDialogParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	// EndDialog MUST NOT be used
	DlgData.DialogFunc = lpDialogFunc;
	DlgData.IsDialog = false;

	/*
	// Override certain default dialogs to use this DLL's resources
	switch ((uintptr_t)lpTemplateName)
	{
	case 0:
		hInstance = (HINSTANCE)&__ImageBase;
		break;
	}
	*/

	return CreateDialogParamA(hInstance, lpTemplateName, hWndParent, DialogFuncOverride, dwInitParam);
}

INT_PTR WINAPI hk_DialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	// EndDialog MUST be used
	DlgData.DialogFunc = lpDialogFunc;
	DlgData.IsDialog = true;

	/*
	// Override certain default dialogs to use this DLL's resources
	switch ((uintptr_t)lpTemplateName)
	{
	case 0:
		hInstance = (HINSTANCE)&__ImageBase;
		break;
	}
	*/

	return DialogBoxParamA(hInstance, lpTemplateName, hWndParent, DialogFuncOverride, dwInitParam);
}

BOOL WINAPI hk_EndDialog(HWND hDlg, INT_PTR nResult)
{
	std::lock_guard lock(g_DialogMutex);

	// Fix for the CK calling EndDialog on a CreateDialogParamA window
	if (auto itr = g_DialogOverrides.find(hDlg); itr != g_DialogOverrides.end() && !itr->second.IsDialog)
	{
		DestroyWindow(hDlg);
		return TRUE;
	}

	return EndDialog(hDlg, nResult);
}

LRESULT WINAPI hk_SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (hWnd && Msg == WM_DESTROY)
	{
		std::lock_guard lock(g_DialogMutex);

		// If this is a dialog, we can't call DestroyWindow on it
		if (auto itr = g_DialogOverrides.find(hWnd); itr != g_DialogOverrides.end())
		{
			if (!itr->second.IsDialog)
				DestroyWindow(hWnd);
		}

		return 0;
	}

	return SendMessageA(hWnd, Msg, wParam, lParam);
}

int hk_inflateInit(z_stream_s *Stream, const char *Version, int Mode)
{
	// Force inflateEnd to error out and skip frees
	Stream->state = nullptr;

	return 0;
}

int hk_inflate(z_stream_s *Stream, int Flush)
{
	size_t outBytes = 0;
	libdeflate_decompressor *decompressor = libdeflate_alloc_decompressor();

	libdeflate_result result = libdeflate_zlib_decompress(decompressor, Stream->next_in, Stream->avail_in, Stream->next_out, Stream->avail_out, &outBytes);
	libdeflate_free_decompressor(decompressor);

	if (result == LIBDEFLATE_SUCCESS)
	{
		Assert(outBytes < std::numeric_limits<uint32_t>::max());

		Stream->total_in = Stream->avail_in;
		Stream->total_out = (uint32_t)outBytes;

		return 1;
	}

	if (result == LIBDEFLATE_INSUFFICIENT_SPACE)
		return -5;

	return -2;
}

void PatchMemory();

uint32_t sub_1414974E0(BSTArray<void *>& Array, const void *&Target, uint32_t StartIndex, __int64 Unused)
{
	for (uint32_t i = StartIndex; i < Array.QSize(); i++)
	{
		if (Array[i] == Target)
			return i;
	}

	return 0xFFFFFFFF;
}

uint32_t sub_1414974E0_SSE41(BSTArray<void *>& Array, const void *&Target, uint32_t StartIndex, __int64 Unused)
{
	const uint32_t size = Array.QSize();
	const __int64 *data = (const __int64 *)Array.QBuffer();

	const uint32_t comparesPerIter = 4;
	const uint32_t alignedSize = size & ~(comparesPerIter - 1);

	// Compare 4 pointers per iteration (Strips off the last 2 bits from array size)
	const __m128i targets = _mm_set1_epi64x((__int64)Target);

	for (uint32_t i = StartIndex; i < alignedSize; i += comparesPerIter)
	{
		//
		// Set bit 0 if 'a1'=='a2', set bit 1 if 'b1'=='b2', set bit X...
		// AVX: mask = _mm256_movemask_pd(_mm256_castsi256_pd(_mm256_cmpeq_epi64(targets, _mm256_loadu_si256((__m256i *)&data[i]))));
		//
		__m128i test1 = _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i *)&data[i + 0]));
		__m128i test2 = _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i *)&data[i + 2]));

		int mask = _mm_movemask_pd(_mm_castsi128_pd(_mm_or_si128(test1, test2)));

		if (mask != 0)
		{
			for (; i < size; i++)
			{
				if (data[i] == (__int64)Target)
					return i;
			}

			__debugbreak();
			__assume(0);
		}
	}

	for (uint32_t i = alignedSize; i < size; i++)
	{
		if (data[i] == (__int64)Target)
			return i;
	}

	return 0xFFFFFFFF;
}

void UpdateObjectWindowTreeView(void *Thisptr, HWND ControlHandle, __int64 Unknown)
{
	SendMessage(ControlHandle, WM_SETREDRAW, FALSE, 0);
	((void(__fastcall *)(void *, HWND, __int64))OFFSET(0x0413EB0, 0))(Thisptr, ControlHandle, Unknown);
	SendMessage(ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(ControlHandle, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

void UpdateCellViewCellList(void *Thisptr, HWND ControlHandle, __int64 Unknown)
{
	SendMessage(ControlHandle, WM_SETREDRAW, FALSE, 0);
	((void(__fastcall *)(void *, HWND, __int64))OFFSET(0x06434C0, 0))(Thisptr, ControlHandle, Unknown);
	SendMessage(ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(ControlHandle, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

void UpdateCellViewObjectList(void *Thisptr, HWND *ControlHandle)
{
	SendMessage(*ControlHandle, WM_SETREDRAW, FALSE, 0);
	((void(__fastcall *)(void *, HWND *))OFFSET(0x05A4320, 0))(Thisptr, ControlHandle);
	SendMessage(*ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(*ControlHandle, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

bool g_UseDeferredDialogInsert;
HWND g_DeferredListView;
HWND g_DeferredComboBox;
uintptr_t g_DeferredStringLength;
bool g_AllowResize;
std::vector<std::pair<const char *, void *>> g_DeferredMenuItems;

void ResetUIDefer()
{
	g_UseDeferredDialogInsert = false;
	g_DeferredListView = nullptr;
	g_DeferredComboBox = nullptr;
	g_DeferredStringLength = 0;
	g_AllowResize = false;
	g_DeferredMenuItems.clear();
}

void BeginUIDefer()
{
	ResetUIDefer();
	g_UseDeferredDialogInsert = true;
}

void SuspendComboBoxUpdates(HWND ComboHandle, bool Suspend)
{
	COMBOBOXINFO info = {};
	info.cbSize = sizeof(COMBOBOXINFO);

	if (!GetComboBoxInfo(ComboHandle, &info))
		return;

	if (!Suspend)
	{
		SendMessage(info.hwndList, WM_SETREDRAW, TRUE, 0);
		SendMessage(ComboHandle, CB_SETMINVISIBLE, 30, 0);
		SendMessage(ComboHandle, WM_SETREDRAW, TRUE, 0);
	}
	else
	{
		SendMessage(ComboHandle, WM_SETREDRAW, FALSE, 0);// Prevent repainting until finished
		SendMessage(ComboHandle, CB_SETMINVISIBLE, 1, 0);// Possible optimization for older libraries (source: MSDN forums)
		SendMessage(info.hwndList, WM_SETREDRAW, FALSE, 0);
	}
}

void EndUIDefer()
{
	if (!g_UseDeferredDialogInsert)
		return;

	if (g_DeferredListView)
	{
		SendMessage(g_DeferredListView, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(g_DeferredListView, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
	}

	if (!g_DeferredMenuItems.empty())
	{
		const HWND control = g_DeferredComboBox;

		// Sort alphabetically if requested to try and speed up inserts
		int finalWidth = 0;
		LONG_PTR style = GetWindowLongPtr(control, GWL_STYLE);

		if ((style & CBS_SORT) == CBS_SORT)
		{
			std::sort(g_DeferredMenuItems.begin(), g_DeferredMenuItems.end(),
				[](const std::pair<const char *, void *>& a, const std::pair<const char *, void *>& b) -> bool
			{
				return _stricmp(a.first, b.first) > 0;
			});
		}

		SendMessage(control, CB_INITSTORAGE, g_DeferredMenuItems.size(), g_DeferredStringLength * sizeof(char));

		if (HDC hdc = GetDC(control); hdc)
		{
			SuspendComboBoxUpdates(control, true);

			// Pre-calculate font widths for resizing, starting with TrueType
			int fontWidths[UCHAR_MAX + 1];
			ABC trueTypeFontWidths[UCHAR_MAX + 1];

			if (!GetCharABCWidthsA(hdc, 0, ARRAYSIZE(trueTypeFontWidths) - 1, trueTypeFontWidths))
			{
				BOOL result = GetCharWidthA(hdc, 0, ARRAYSIZE(fontWidths) - 1, fontWidths);
				AssertMsg(result, "Failed to determine any font widths");
			}
			else
			{
				for (int i = 0; i < ARRAYSIZE(fontWidths); i++)
					fontWidths[i] = trueTypeFontWidths[i].abcB;
			}

			// Insert everything all at once
			for (auto [display, value] : g_DeferredMenuItems)
			{
				LRESULT index = SendMessageA(control, CB_ADDSTRING, 0, (LPARAM)display);
				int lineSize = 0;

				if (index != CB_ERR && index != CB_ERRSPACE)
					SendMessageA(control, CB_SETITEMDATA, index, (LPARAM)value);

				for (const char *c = display; *c != '\0'; c++)
					lineSize += fontWidths[*c];

				finalWidth = std::max<int>(finalWidth, lineSize);

				free((void *)display);
			}

			SuspendComboBoxUpdates(control, false);
			ReleaseDC(control, hdc);
		}

		// Resize to fit
		if (g_AllowResize)
		{
			LRESULT currentWidth = SendMessage(control, CB_GETDROPPEDWIDTH, 0, 0);

			if (finalWidth > currentWidth)
				SendMessage(control, CB_SETDROPPEDWIDTH, finalWidth, 0);
		}
	}

	ResetUIDefer();
}

void InsertComboBoxItem(HWND ComboBoxHandle, const char *DisplayText, void *Value, bool AllowResize)
{
	if (!ComboBoxHandle)
		return;

	if (!DisplayText)
		DisplayText = "NONE";

	if (g_UseDeferredDialogInsert)
	{
		AssertMsg(!g_DeferredComboBox || (g_DeferredComboBox == ComboBoxHandle), "Got handles to different combo boxes? Reset probably wasn't called.");

		g_DeferredComboBox = ComboBoxHandle;
		g_DeferredStringLength += strlen(DisplayText) + 1;
		g_AllowResize |= AllowResize;

		// A copy must be created since lifetime isn't guaranteed after this function returns
		g_DeferredMenuItems.emplace_back(_strdup(DisplayText), Value);
	}
	else
	{
		if (AllowResize)
		{
			if (HDC hdc = GetDC(ComboBoxHandle); hdc)
			{
				if (SIZE size; GetTextExtentPoint32A(hdc, DisplayText, (int)strlen(DisplayText), &size))
				{
					LRESULT currentWidth = SendMessageA(ComboBoxHandle, CB_GETDROPPEDWIDTH, 0, 0);

					if (size.cx > currentWidth)
						SendMessageA(ComboBoxHandle, CB_SETDROPPEDWIDTH, size.cx, 0);
				}

				ReleaseDC(ComboBoxHandle, hdc);
			}
		}

		LRESULT index = SendMessageA(ComboBoxHandle, CB_ADDSTRING, 0, (LPARAM)DisplayText);

		if (index != CB_ERR && index != CB_ERRSPACE)
			SendMessageA(ComboBoxHandle, CB_SETITEMDATA, index, (LPARAM)Value);
	}
}

void InsertListViewItem(HWND ListViewHandle, void *Parameter, bool UseImage, int ItemIndex)
{
	if (ItemIndex == -1)
		ItemIndex = INT_MAX;

	LVITEMA item;
	memset(&item, 0, sizeof(item));

	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.iItem = ItemIndex;
	item.lParam = (LPARAM)Parameter;
	item.pszText = LPSTR_TEXTCALLBACK;

	if (UseImage)
	{
		item.mask |= LVIF_IMAGE;
		item.iImage = I_IMAGECALLBACK;
	}

	if (g_UseDeferredDialogInsert)
	{
		AssertMsg(!g_DeferredListView || (g_DeferredListView == ListViewHandle), "Got handles to different list views? Reset probably wasn't called.");

		if (!g_DeferredListView)
		{
			g_DeferredListView = ListViewHandle;
			SendMessage(ListViewHandle, WM_SETREDRAW, FALSE, 0);
		}
	}

	ListView_InsertItem(ListViewHandle, &item);
}

void QuitHandler()
{
	TerminateProcess(GetCurrentProcess(), 0);
}

void hk_call_140906407(__int64 a1, __int64 a2, __int64 a3)
{
	if (a2)
		((void(__fastcall *)(__int64, __int64, __int64))OFFSET(0x2A6B230, 0))(a1, a2, a3);
}

void PatchTemplatedFormIterator()
{
	class FormIteratorHook : public Xbyak::CodeGenerator
	{
	public:
		FormIteratorHook(uintptr_t Callback) : Xbyak::CodeGenerator()
		{
			// Allocate enough space for 8 copied parameters (4 regs)
			mov(ptr[rsp + 0x8], rbx);
			mov(ptr[rsp + 0x10], rbp);
			mov(ptr[rsp + 0x18], rsi);
			push(rdi);
			sub(rsp, 0x50);

			mov(rbp, rcx);
			mov(rsi, rdx);
			mov(rdi, r8);
			mov(rbx, r9);
			mov(rax, (uintptr_t)&BeginUIDefer);// BeginUIDefer (0 params)
			call(rax);
			mov(rcx, rbp);
			mov(rdx, rsi);
			mov(r8, rdi);
			mov(r9, rbx);

			for (uint32_t i = 0; i < 6; i++)
			{
				mov(rax, ptr[rsp + (0xA8 - i * 0x8)]);
				mov(ptr[rsp + (0x48 - i * 0x8)], rax);
			}

			mov(rax, Callback);// Callback (X params)
			call(rax);
			mov(rbx, rax);

			mov(rax, (uintptr_t)&EndUIDefer);// EndUIDefer (0 params)
			call(rax);
			mov(rax, rbx);

			add(rsp, 0x50);
			pop(rdi);
			mov(rsi, ptr[rsp + 0x18]);
			mov(rbp, ptr[rsp + 0x10]);
			mov(rbx, ptr[rsp + 0x8]);
			ret();
		}

		static void Generate(uintptr_t Target)
		{
			// Manually resolve the called function address. NOTE: This is leaking memory on purpose. It's a mess.
			Assert(*(uint8_t *)Target == 0xE9);

			auto destination = Target + *(int32_t *)(Target + 1) + 5;
			auto hook = new FormIteratorHook(destination);

			XUtil::DetourJump(Target, (uintptr_t)hook->getCode());
		}
	};

	// List view
	FormIteratorHook::Generate(OFFSET(0x2450A, 0));
	FormIteratorHook::Generate(OFFSET(0x7C16A, 0));
	FormIteratorHook::Generate(OFFSET(0x37DF3, 0));
	FormIteratorHook::Generate(OFFSET(0x75324, 0));
	FormIteratorHook::Generate(OFFSET(0x65F55, 0));
	FormIteratorHook::Generate(OFFSET(0x2B21, 0));

	// Combo box
	FormIteratorHook::Generate(OFFSET(0x77E49, 0));
	FormIteratorHook::Generate(OFFSET(0x52F4, 0));
	FormIteratorHook::Generate(OFFSET(0x191D2, 0));
	FormIteratorHook::Generate(OFFSET(0x28CBD, 0));
	FormIteratorHook::Generate(OFFSET(0x118FB, 0));
}