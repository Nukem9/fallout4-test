#include <libdeflate/libdeflate.h>
#include <xbyak/xbyak.h>
#include <CommCtrl.h>
#include <smmintrin.h>
#include "Editor.h"
#include "EditorUI.h"
#include "LogWindow.h"

#pragma comment(lib, "libdeflate.lib")

BOOL bFogToggle = TRUE;

struct DialogOverrideData
{
	DLGPROC DialogFunc;	// Original function pointer
	BOOL IsDialog;		// True if it requires EndDialog()
};

std::recursive_mutex g_DialogMutex;
std::unordered_map<HWND, DialogOverrideData> g_DialogOverrides;
thread_local DialogOverrideData DlgData;

BOOL FIXAPI OpenPluginSaveDialog(HWND ParentWindow, LPCSTR BasePath, BOOL IsESM, LPSTR Buffer, uint32_t BufferSize, LPCSTR Directory)
{
	if (!BasePath)
		BasePath = "\\Data";

	LPCSTR filter = "TES Plugin Files (*.esp)\0*.esp\0TES Master Files (*.esm)\0*.esm\0\0";
	LPCSTR title = "Select Target Plugin";
	LPCSTR extension = "esp";

	if (IsESM)
	{
		filter = "TES Master Files (*.esm)\0*.esm\0\0";
		title = "Select Target Master";
		extension = "esm";
	}

	return ((BOOL(__fastcall*)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPVOID, BOOL, BOOL, LPSTR, uint32_t, LPCSTR, LPVOID))
		OFFSET(0x6461B0, 0))(ParentWindow, BasePath, filter, title, extension, NULL, FALSE, TRUE, Buffer, BufferSize, Directory, NULL);
}

INT_PTR CALLBACK DialogFuncOverride(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DLGPROC proc = NULL;

	g_DialogMutex.lock();
	{
		if (auto itr = g_DialogOverrides.find(hwndDlg); itr != g_DialogOverrides.end())
			proc = itr->second.DialogFunc;

		// if (is new entry)
		if (!proc)
		{
			g_DialogOverrides[hwndDlg] = DlgData;
			proc = DlgData.DialogFunc;

			DlgData.DialogFunc = NULL;
			DlgData.IsDialog = FALSE;
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
	DlgData.IsDialog = FALSE;

	// Override certain default dialogs to use this DLL's resources
	switch ((uintptr_t)lpTemplateName)
	{
	case 122:
	case 162:
	case 165:
	case 166:
	case 175:
	case 220:
	case 243:
	case 252:
	case 279:
	case 316:
	case 350:
		hInstance = (HINSTANCE)&__ImageBase;
		break;
	}

	return CreateDialogParamA(hInstance, lpTemplateName, hWndParent, DialogFuncOverride, dwInitParam);
}

INT_PTR WINAPI hk_DialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	// EndDialog MUST be used
	DlgData.DialogFunc = lpDialogFunc;
	DlgData.IsDialog = TRUE;

	// Override certain default dialogs to use this DLL's resources
	switch ((uintptr_t)lpTemplateName)
	{
	case 122:
	case 162:
	case 165:
	case 166:
	case 175:
	case 220:
	case 243:
	case 252:
	case 279:
	case 316:
	case 350:
		hInstance = (HINSTANCE)&__ImageBase;
		break;
	}

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

INT32 FIXAPI hk_inflateInit(z_stream_s *Stream, LPCSTR Version, INT32 Mode)
{
	// Force inflateEnd to error out and skip frees
	Stream->state = NULL;

	return 0;
}

INT32 FIXAPI hk_inflate(z_stream_s *Stream, INT32 Flush)
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

uint32_t FIXAPI sub_1405B31C0(BSTArray<LPVOID>& Array, LPCVOID &Target)
{
	for (uint32_t i = 0; i < Array.QSize(); i++)
	{
		if (Array[i] == Target)
			return i;
	}

	return 0xFFFFFFFF;
}

uint32_t FIXAPI sub_1405B31C0_SSE41(BSTArray<LPVOID>& Array, LPCVOID &Target)
{
	uint32_t index = 0;
	PINT64 data = (PINT64)Array.QBuffer();

	const uint32_t comparesPerIter = 4;
	const uint32_t vectorizedIterations = (Array.QSize() - index) / comparesPerIter;

	//
	// Compare 4 pointers per iteration - use SIMD instructions to generate a bit mask. Set
	// bit 0 if 'array[i + 0]'=='target', set bit 1 if 'array[i + 1]'=='target', set bit X...
	//
	// AVX: mask = _mm256_movemask_pd(_mm256_castsi256_pd(_mm256_cmpeq_epi64(targets, _mm256_loadu_si256((__m256i *)&data[i]))));
	//
	const __m128i targets = _mm_set1_epi64x((INT64)Target);

	for (uint32_t iter = 0; iter < vectorizedIterations; iter++)
	{
		__m128i test1 = _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i *)&data[index + 0]));
		__m128i test2 = _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i *)&data[index + 2]));

		INT32 mask = _mm_movemask_pd(_mm_castsi128_pd(_mm_or_si128(test1, test2)));

		// if (target pointer found) { break into the remainder loop to get the index }
		if (mask != 0)
			break;

		index += comparesPerIter;
	}

	// Scan the rest 1-by-1
	for (; index < Array.QSize(); index++)
	{
		if (data[index] == (INT64)Target)
			return index;
	}

	return 0xFFFFFFFF;
}

VOID FIXAPI UpdateObjectWindowTreeView(LPVOID Thisptr, HWND ControlHandle, INT64 Unknown)
{
	SendMessageA(ControlHandle, WM_SETREDRAW, FALSE, 0);
	((VOID(__fastcall*)(LPVOID, HWND, INT64))OFFSET(0x0413EB0, 0))(Thisptr, ControlHandle, Unknown);
	SendMessageA(ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(ControlHandle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

VOID FIXAPI UpdateCellViewCellList(LPVOID Thisptr, HWND ControlHandle, INT64 Unknown)
{
	SendMessageA(ControlHandle, WM_SETREDRAW, FALSE, 0);
	((VOID(__fastcall*)(LPVOID, HWND, INT64))OFFSET(0x06434C0, 0))(Thisptr, ControlHandle, Unknown);
	SendMessageA(ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(ControlHandle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

VOID FIXAPI UpdateCellViewObjectList(LPVOID Thisptr, HWND *ControlHandle)
{
	SendMessageA(*ControlHandle, WM_SETREDRAW, FALSE, 0);
	((VOID(__fastcall*)(LPVOID, HWND*))OFFSET(0x05A4320, 0))(Thisptr, ControlHandle);
	SendMessageA(*ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(*ControlHandle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

BOOL g_UseDeferredDialogInsert;
HWND g_DeferredListView;
HWND g_DeferredComboBox;
uintptr_t g_DeferredStringLength;
BOOL g_AllowResize;
std::vector<std::pair<LPCSTR, LPVOID>> g_DeferredMenuItems;

VOID FIXAPI ResetUIDefer(VOID)
{
	g_UseDeferredDialogInsert = FALSE;
	g_DeferredListView = NULL;
	g_DeferredComboBox = NULL;
	g_DeferredStringLength = 0;
	g_AllowResize = FALSE;
	g_DeferredMenuItems.clear();
}

VOID FIXAPI BeginUIDefer(VOID)
{
	ResetUIDefer();
	g_UseDeferredDialogInsert = TRUE;
}

VOID FIXAPI SuspendComboBoxUpdates(HWND ComboHandle, BOOL Suspend)
{
	COMBOBOXINFO info = {};
	info.cbSize = sizeof(COMBOBOXINFO);

	if (!GetComboBoxInfo(ComboHandle, &info))
		return;

	if (!Suspend)
	{
		SendMessageA(info.hwndList, WM_SETREDRAW, TRUE, 0);
		SendMessageA(ComboHandle, CB_SETMINVISIBLE, 30, 0);
		SendMessageA(ComboHandle, WM_SETREDRAW, TRUE, 0);
	}
	else
	{
		SendMessageA(ComboHandle, WM_SETREDRAW, FALSE, 0);// Prevent repainting until finished
		SendMessageA(ComboHandle, CB_SETMINVISIBLE, 1, 0);// Possible optimization for older libraries (source: MSDN forums)
		SendMessageA(info.hwndList, WM_SETREDRAW, FALSE, 0);
	}
}

VOID FIXAPI EndUIDefer(VOID)
{
	if (!g_UseDeferredDialogInsert)
		return;

	if (g_DeferredListView)
	{
		SendMessage(g_DeferredListView, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(g_DeferredListView, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
	}

	if (!g_DeferredMenuItems.empty())
	{
		const HWND control = g_DeferredComboBox;

		// Sort alphabetically if requested to try and speed up inserts
		INT32 finalWidth = 0;
		LONG_PTR style = GetWindowLongPtr(control, GWL_STYLE);

		if ((style & CBS_SORT) == CBS_SORT)
		{
			std::sort(g_DeferredMenuItems.begin(), g_DeferredMenuItems.end(),
				[](const std::pair<LPCSTR, LPVOID>& a, const std::pair<LPCSTR, LPVOID>& b) -> BOOL
			{
				return _stricmp(a.first, b.first) > 0;
			});
		}

		SendMessage(control, CB_INITSTORAGE, g_DeferredMenuItems.size(), g_DeferredStringLength * sizeof(CHAR));

		if (HDC hdc = GetDC(control); hdc)
		{
			SuspendComboBoxUpdates(control, TRUE);

			// Pre-calculate font widths for resizing, starting with TrueType
			INT32 fontWidths[UCHAR_MAX + 1];
			ABC trueTypeFontWidths[UCHAR_MAX + 1];

			if (!GetCharABCWidthsA(hdc, 0, ARRAYSIZE(trueTypeFontWidths) - 1, trueTypeFontWidths))
			{
				BOOL result = GetCharWidthA(hdc, 0, ARRAYSIZE(fontWidths) - 1, fontWidths);
				AssertMsg(result, "Failed to determine any font widths");
			}
			else
			{
				for (INT32 i = 0; i < ARRAYSIZE(fontWidths); i++)
					fontWidths[i] = trueTypeFontWidths[i].abcB;
			}

			// Insert everything all at once
			for (auto [display, value] : g_DeferredMenuItems)
			{
				LRESULT index = SendMessageA(control, CB_ADDSTRING, 0, (LPARAM)display);
				INT32 lineSize = 0;

				if (index != CB_ERR && index != CB_ERRSPACE)
					SendMessageA(control, CB_SETITEMDATA, index, (LPARAM)value);

				for (LPCSTR c = display; *c != '\0'; c++)
					lineSize += fontWidths[*c];

				finalWidth = std::max<INT32>(finalWidth, lineSize);

				free((LPVOID)display);
			}

			SuspendComboBoxUpdates(control, FALSE);
			ReleaseDC(control, hdc);
		}

		// Resize to fit
		if (g_AllowResize)
		{
			LRESULT currentWidth = SendMessageA(control, CB_GETDROPPEDWIDTH, 0, 0);

			if (finalWidth > currentWidth)
				SendMessageA(control, CB_SETDROPPEDWIDTH, finalWidth, 0);
		}
	}

	ResetUIDefer();
}

VOID FIXAPI InsertComboBoxItem(HWND ComboBoxHandle, LPCSTR DisplayText, LPVOID Value, BOOL AllowResize)
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
				if (SIZE size; GetTextExtentPoint32A(hdc, DisplayText, (INT32)strlen(DisplayText), &size))
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

VOID FIXAPI InsertListViewItem(HWND ListViewHandle, LPVOID Parameter, BOOL UseImage, INT32 ItemIndex)
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
			SendMessageA(ListViewHandle, WM_SETREDRAW, FALSE, 0);
		}
	}

	ListView_InsertItem(ListViewHandle, &item);
}

VOID FIXAPI QuitHandler(VOID)
{
	TerminateProcess(GetCurrentProcess(), 0);
}

VOID FIXAPI hk_call_140906407(INT64 a1, INT64 a2, INT64 a3)
{
	if (a2)
		((VOID(__fastcall*)(INT64, INT64, INT64))OFFSET(0x2A6B230, 0))(a1, a2, a3);
}

BOOL FIXAPI hk_call_12E852C(HWND RichEditControl, LPCSTR Text)
{
	SendMessageA(RichEditControl, EM_LIMITTEXT, 500000, 0);
	return SetWindowTextA(RichEditControl, Text);
}

BOOL FIXAPI hk_IsFogEnabled(VOID)
{
	return bFogToggle;
}

VOID FIXAPI PatchFogToggle(VOID)
{
	class FogToggle : public Xbyak::CodeGenerator
	{
	public:
		FogToggle() : Xbyak::CodeGenerator(4096)
		{
			sub(rsp, 0x28);
			mov(rdx, rcx);
			mov(rcx, (uintptr_t)&bFogToggle);
			cmp(byte[rcx], 0);
			je("L1");
			mov(rcx, ptr[rdx + 0x80]);
			test(rcx, rcx);
			je("L1");
			mov(rax, ptr[rcx]);
			movaps(xmm2, xmm6);
			call(ptr[rax + 0x10]);
			L("L1");
			add(rsp, 0x28);
			ret(); 
		}

		static VOID Generate(uintptr_t Target)
		{
			auto hook = new FogToggle();
			XUtil::PatchMemory(Target, { 0x48, 0x89, 0xD9 });
			XUtil::DetourCall(Target + 3, (uintptr_t)hook->getCode());
			XUtil::PatchMemory(Target + 8, { 0xEB, 0x0E });
		}
	};

	FogToggle::Generate(OFFSET(0xF8AF33, 0));

	XUtil::DetourCall(OFFSET(0xF8CAF3, 0), &hk_IsFogEnabled);
	XUtil::DetourJump(OFFSET(0xF90CE0, 0), &hk_IsFogEnabled);
}

FLOAT FIXAPI Fixed_IncorrectSmoothnessValueToMaterialNif(FLOAT a1, FLOAT a2)
{
	if (a2 > 1.0)
		return 1.0;
	else
		return 0.0;
}

VOID FIXAPI Fixed_DeleteTintingRace(INT64 count, INT64 item_id, HWND listview)
{
	if (!count)
		return;
	
	for (auto i = 0; i < count; i++)
		ListView_DeleteItem(listview, item_id);
}

VOID FIXAPI PatchTemplatedFormIterator(VOID)
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

		static VOID Generate(uintptr_t Target)
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