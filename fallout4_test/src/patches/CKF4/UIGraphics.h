#pragma once

#include <set>
#include "..\ComClasses.h"

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			class CRECT
			{
			private:
				LONG m_left;
				LONG m_top;
				LONG m_right;
				LONG m_bottom;
			public:
				CRECT() : m_left(0), m_top(0), m_right(0), m_bottom(0) {}
				CRECT(const LONG l, const LONG t, const LONG r, const LONG b) : m_left(l), m_top(t), m_right(r), m_bottom(b) {}
				CRECT(const RECT& r) : m_left(r.left), m_top(r.top), m_right(r.right), m_bottom(r.bottom) {}
				CRECT(const CRECT& r) : m_left(r.m_left), m_top(r.m_top), m_right(r.m_right), m_bottom(r.m_bottom) {}
			public:
				inline LONG GetWidth(VOID) const { return m_right - m_left; }
				inline LONG GetHeight(VOID) const { return m_bottom - m_top; }
				inline LONG GetLeft(VOID) const { return m_left; }
				inline LONG GetTop(VOID) const { return m_top; }
				inline LONG GetRight(VOID) const { return m_right; }
				inline LONG GetBottom(VOID) const { return m_bottom; }
				inline VOID SetWidth(const LONG value) { m_right = value + m_left; }
				inline VOID SetHeight(const LONG value) { m_bottom = value + m_top; }
				inline VOID SetLeft(const LONG value) { m_right = GetWidth() + value; m_left = value; }
				inline VOID SetTop(const LONG value) { m_bottom = GetHeight() + value; m_top = value; }
				inline VOID SetRight(const LONG value) { m_right = value; }
				inline VOID SetBottom(const LONG value) { m_bottom = value; }
			public:
				inline CRECT& Inflate(const LONG x, const LONG y) { m_left -= x; m_top -= y; m_right += x; m_bottom += y; return *this; }
				inline CRECT& Offset(const LONG x, const LONG y) { m_left += x; m_top += y; m_right += x; m_bottom += y; return *this; }
				inline BOOL IsEmpty(VOID) const { return GetWidth() == 0 || GetHeight() == 0; }
				inline CRECT Dublicate(VOID) const { return *this; }
			public:
				PROPERTY(GetWidth, SetWidth) LONG Width;
				PROPERTY(GetHeight, SetHeight) LONG Height;
				PROPERTY(GetLeft, SetLeft) LONG Left;
				PROPERTY(GetTop, SetTop) LONG Top;
				PROPERTY(GetRight, SetRight) LONG Right;
				PROPERTY(GetBottom, SetBottom) LONG Bottom;
			};

			typedef CRECT* LPCCRECT, PCCRECT;

			class CUICanvas;
			class CUIObjectGUI;

			enum CUIPenStyle
			{
				psClear,
				psSolid,
				psDash,
				psDot,
				psDashDot,
				psDashDotDot,
				psInsideFrame
			};

			typedef VOID (*CUIObjectGUIChangeEvent)(CUIObjectGUI* sender, CUICanvas* canvas);

			class CUIObjectGUI
			{
			protected:
				HANDLE m_fHandle;
			protected:
				VOID Release(VOID);
				VOID DoChange(VOID);
			public:
				inline HANDLE GetHandle(VOID) const { return m_fHandle; }
				HANDLE Select(const CUICanvas& canvas) const;
				HANDLE Select(VOID) const;
			public:
				CUIObjectGUI(VOID) : OnChange(NULL), m_fHandle(NULL), Canvas(NULL) {}
				CUIObjectGUI(const CUIObjectGUI &obj) : OnChange(obj.OnChange), m_fHandle(obj.m_fHandle), Canvas(obj.Canvas) {}
				virtual ~CUIObjectGUI(VOID);
			public:
				CUICanvas* Canvas;
				CUIObjectGUIChangeEvent OnChange;
			public:
				READ_PROPERTY(GetHandle) HANDLE Handle;
			};

			class CUIBitmap : public CUIObjectGUI
			{
			protected:
				VOID Create(INT width, INT height, INT bpp);
				VOID Create(LPCSTR fname);
				VOID Create(HINSTANCE hinst, LPCSTR name);
			public:
				VOID FreeImage(VOID);
				BOOL Empty(VOID) const { return !m_fHandle; };
				VOID SaveToFile(LPCSTR fname) const;
			public:
				CUIBitmap(VOID) : CUIObjectGUI() {}
				CUIBitmap(HBITMAP bitmap) : CUIObjectGUI() { m_fHandle = bitmap; }
				CUIBitmap(INT width, INT height, INT bpp) : CUIObjectGUI() { Create(width, height, bpp); }
				CUIBitmap(LPCSTR fname) : CUIObjectGUI() { Create(fname); }
				CUIBitmap(HINSTANCE hinst, LPCSTR name) : CUIObjectGUI() { Create(hinst, name); }
				CUIBitmap(const CUIBitmap& bitmap);
			};

			class CUIPen : public CUIObjectGUI
			{
			private:
				INT m_fSize;
				COLORREF m_fColor;
				CUIPenStyle m_fStyle;
			protected:
				VOID Create(CUIPenStyle style, INT width, COLORREF color);
			public:
				inline INT GetWidth(VOID) const { return m_fSize; }
				inline COLORREF GetColor(VOID) const { return m_fColor; }
				inline CUIPenStyle GetStyle(VOID) const { return m_fStyle; }
				VOID SetWidth(const INT width);
				VOID SetColor(const COLORREF color);
				VOID SetStyle(const CUIPenStyle style);
			public:
				CUIPen(CUIPenStyle style, INT width, COLORREF color) : CUIObjectGUI() { Create(style, width, color); }
				CUIPen(const CUIPen& pen) : CUIObjectGUI(pen) { Create(pen.m_fStyle, pen.m_fSize, pen.m_fColor); }
			public:
				PROPERTY(GetWidth, SetWidth) const INT Width;
				PROPERTY(GetColor, SetColor) const COLORREF Color;
				PROPERTY(GetStyle, SetStyle) const CUIPenStyle Style;
			};

			CUIPen WINAPI CreateSolidPen(INT width, COLORREF color);
			CUIPen WINAPI CreateDashPen(INT width, COLORREF color);
			CUIPen WINAPI CreateDotPen(INT width, COLORREF color);

			enum CUIBrushStyle
			{
				bsClear,
				bsSolid,
				bsBitmap
			};

			class CUIBrush : public CUIObjectGUI
			{
			private:
				COLORREF m_fColor;
				CUIBitmap m_fBitmap;
				CUIBrushStyle m_fStyle;
			protected:
				VOID Create(const COLORREF color);
				VOID Create(const CUIBitmap& bitmap);
			public:
				inline COLORREF GetColor(VOID) const { return m_fColor; }
				inline CUIBrushStyle GetStyle(VOID) const { return m_fStyle; }
				inline CUIBitmap& GetBitmap(VOID) { return m_fBitmap; }
				VOID SetColor(const COLORREF color);
				VOID SetStyle(const CUIBrushStyle style);
				VOID SetBitmap(const CUIBitmap& bitmap);
			public:
				CUIBrush(const COLORREF color) : CUIObjectGUI() { Create(color); }
				CUIBrush(const CUIBitmap& bitmap) : CUIObjectGUI() { Create(bitmap); }
				CUIBrush(const CUIBrush& brush);
			public:
				PROPERTY(GetColor, SetColor) const COLORREF Color;
				PROPERTY(GetStyle, SetStyle) const CUIBrushStyle Style;
				PROPERTY(GetBitmap, SetBitmap) const CUIBitmap& Bitmap;
			};

			CUIBrush WINAPI CreateSolidBrush(const COLORREF color);
			CUIBrush WINAPI CreatePatternBrush(const CUIBitmap& bitmap);

			enum CUIGradientDirect {
				gdHorz,
				gdVert
			};

			CUIBrush WINAPI CreateGradientBrush(const COLORREF start_color, const COLORREF end_color, const INT size, const CUIGradientDirect direct);

			enum CUIFontStyle {
				fsBold,
				fsItalic,
				fsUnderline,
				fsStrikeOut
			};
			typedef std::set<CUIFontStyle> CUIFontStyles;
			enum CUIFontQuality {
				fqDefault,
				fqClearType,
				fqClearTypeNatural,
				fqProof,
				fqAntialiased,
				fqNoAntialiased,
			};
			enum CUIFontPitch {
				fpDefault,
				fpFixed,
				fpVariable,
				fpMono
			};

			class CUIFont : public CUIObjectGUI
			{
			private:
				BOOL m_lock;
				std::string m_Name;
				CUIFontStyles m_FontStyles;
				CUIFontQuality m_Quality;
				CUIFontPitch m_Pitch;
				LONG m_Height;
			private:
				VOID Change(VOID);
			public:
				std::string GetName(VOID) const { return m_Name; }
				VOID SetName(const std::string& name);
				LONG GetSize(VOID) const;
				VOID SetSize(const LONG value);
				inline LONG GetHeight(VOID) const { return m_Height; }
				VOID SetHeight(const LONG value);
				inline CUIFontStyles GetStyles(VOID) const { return m_FontStyles; }
				VOID SetStyles(const CUIFontStyles& styles);
				inline CUIFontQuality GetQuality(VOID) const { return m_Quality; }
				VOID SetQuality(const CUIFontQuality quality);
				inline CUIFontPitch GetPitch(VOID) const { return m_Pitch; }
				VOID SetPitch(const CUIFontPitch pitch);
			private:
				VOID Recreate(VOID);
				VOID Recreate(const HDC hDC);
			public:
				VOID Apply(HWND window) const;
				VOID Assign(const CUIFont& font);
			public:
				READ_PROPERTY(GetHandle) HFONT Handle;
				PROPERTY(GetName, SetName) const std::string Name;
				PROPERTY(GetSize, SetSize) const LONG Size;
				PROPERTY(GetHeight, SetHeight) const LONG Height;
				PROPERTY(GetStyles, SetStyles) const CUIFontStyles Styles;
				PROPERTY(GetQuality, SetQuality) const CUIFontQuality Quality;
				PROPERTY(GetPitch, SetPitch) const CUIFontPitch Pitch;
			public:
				CUIFont(const std::string& name, const LONG size, const CUIFontStyles& styles = {}, const CUIFontQuality quality = fqClearTypeNatural,
					const CUIFontPitch pitch = fpVariable);
				CUIFont(const HDC hDC);
				CUIFont(const CUIFont& parent) : CUIObjectGUI(parent), m_FontStyles(parent.m_FontStyles), m_Quality(parent.m_Quality), m_Pitch(parent.m_Pitch),
					m_Name(parent.m_Name), m_Height(parent.m_Height), m_lock(FALSE) {
					Recreate();
				}
			};

			class CUICanvas
			{
			private:
				HDC m_hDC;
				CUIPen m_fPen;
				CUIBrush m_fBrush;
				CUIFont m_fFont;
				HPEN m_hOldPen;
				HBRUSH m_hOldBrush;
				HFONT m_hOldFont;
			private:
				static VOID DoChange(CUIObjectGUI* sender, CUICanvas* canvas);
			public:
				VOID MoveTo(INT x, INT y) const;
				VOID MoveTo(const POINT& p) const;
				VOID LineTo(INT x, INT y) const;
				VOID LineTo(const POINT& p) const;
				VOID Fill(const COLORREF color);
				VOID Fill(const RECT& area, const COLORREF color);
				VOID Fill(const CRECT& area, const COLORREF color);
				VOID Fill(const LPCRECT area, const INT nCount, const COLORREF color);
				VOID Fill(const LPCCRECT area, const INT nCount, const COLORREF color);
				VOID GradientFill(const RECT& area, const COLORREF start_color, const COLORREF end_color, const CUIGradientDirect direct);
				VOID GradientFill(const CRECT& area, const COLORREF start_color, const COLORREF end_color, const CUIGradientDirect direct);
				VOID Frame(const COLORREF color);
				VOID Frame(const RECT& area, const COLORREF color);
				VOID Frame(const CRECT& area, const COLORREF color);
				VOID GradientFrame(const RECT& area, const COLORREF start_color, const COLORREF end_color, const CUIGradientDirect direct);
				VOID GradientFrame(const CRECT& area, const COLORREF start_color, const COLORREF end_color, const CUIGradientDirect direct);
				VOID Polygon(const LPPOINT ps, INT count) const;
				VOID Polygon(const std::vector<POINT>& p) const;
				VOID TextSize(LPCSTR text, INT& width, INT& height) const;
				inline INT TextWidth(LPCSTR text) const { INT w, h; TextSize(text, w, h); return w; }
				inline INT TextHeight(LPCSTR text) const { INT w, h; TextSize(text, w, h); return h; }
				VOID TextRect(RECT& area, LPCSTR text, UINT flags) const;
				VOID TextRect(CRECT& area, LPCSTR text, UINT flags) const;
				VOID TextInput(INT x, INT y, LPCSTR text);
			public:
				VOID Update(VOID) const;
				CRECT GetClipRect(VOID) const;
				inline HDC GetHandle(VOID) const { return m_hDC; }
				inline CUIPen& GetPen(VOID) { return m_fPen; }
				inline CUIBrush& GetBrush(VOID) { return m_fBrush; }
				inline CUIFont& GetFont(VOID) { return m_fFont; }
			public:
				CUICanvas(HDC hDC);
				CUICanvas(const CUICanvas& canvas);
				virtual ~CUICanvas(VOID);
			public:
				READ_PROPERTY(GetHandle) HDC Handle;
				READ_PROPERTY(GetPen) CUIPen& Pen;
				READ_PROPERTY(GetBrush) CUIBrush& Brush;
				READ_PROPERTY(GetFont) CUIFont& Font;
			};

			class CUIMonitor
			{
			private:
				HMONITOR m_fHandle;
			public:
				inline HMONITOR GetHandle(VOID) const { return m_fHandle; }
			public:
				BOOL IsPrimary(VOID) const;
				CRECT GetRect(VOID) const;
				CRECT GetWorkAreaRect(VOID) const;
			public:
				CUIMonitor(HMONITOR handle) : m_fHandle(handle) {}
				CUIMonitor(const CUIMonitor& monitor) : m_fHandle(monitor.m_fHandle) {}
			public:
				READ_PROPERTY(GetHandle) HMONITOR Handle;
				READ_PROPERTY(GetRect) CRECT Rect;
				READ_PROPERTY(GetWorkAreaRect) CRECT WorkAreaRect;
			};

			class CUIScreen
			{
			private:
				INT m_nVirtualWidth, m_nVirtualHeight;
			public:
				inline INT GetWidth(VOID) const { return m_nVirtualWidth; }
				inline INT GetHeight(VOID) const { return m_nVirtualHeight; }
				CUIMonitor MonitorFromPoint(const POINT& p) const;
				CUIMonitor MonitorFromRect(const RECT& rect) const;
				CUIMonitor MonitorFromRect(const CRECT& rect) const;
				CUIMonitor MonitorFromWindow(HWND handle) const;
			public:
				CUIScreen(VOID);
			public:
				READ_PROPERTY(GetWidth) INT Width;
				READ_PROPERTY(GetHeight) INT Height;
			};

			extern CUIScreen Screen;
		}
	}
}