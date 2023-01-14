//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

#include <minwindef.h>
#include <string>

namespace LogWindow {
	class IChannel {
	protected:
		BOOL _skipdel;
	protected:
		virtual VOID Add(LPCSTR Text) = 0;
		virtual BOOL Check(LPCSTR Text) = 0;
	public:
		VOID WriteVa(LPCSTR Format, va_list Va);
		VOID Write(LPCSTR Format, ...);
	public:
		IChannel(VOID);
	};

	class IPendingMessages {
	private:
		DWORD _limit;
		LPVOID _pend;
	protected:
		VOID QueuePend(LPCSTR Text);
	public:
		DWORD CountPend(VOID) const;
		INLINE LPVOID GetPend(VOID) { return _pend; }
	public:
		IPendingMessages(DWORD limit);
		virtual ~IPendingMessages(VOID);
	};

	class IOutputFileHandler {
	private:
		FILE* _handle;
	protected:
		BOOL _writefile;
	protected:
		VOID WriteFile(LPCSTR Text);
	public:
		INLINE BOOL IsOpen(VOID) const { return _handle != NULL; }
	public:
		IOutputFileHandler(LPCSTR _filename);
		virtual ~IOutputFileHandler(VOID);
	};

	class ChannelBased : public IChannel, public IPendingMessages, public IOutputFileHandler {
	private:
		DWORD _hashlast;
		std::string _storage;
	protected:
		virtual VOID Add(LPCSTR Text);
		virtual BOOL Check(LPCSTR Text);
	public:
		INLINE VOID ResetHash(VOID) { _hashlast = 0xFFFFFFFF; }
		INLINE VOID Clear(VOID) { ResetHash(); _storage.clear(); }
		INLINE LPCSTR GetStorage(VOID) const { return _storage.c_str(); }
	public:
		INLINE VOID SetSkipDelimeter(BOOL val) { _skipdel = val; }
		INLINE BOOL GetSkipDelimeter(VOID) const { return _skipdel; }
		PROPERTY(GetSkipDelimeter, SetSkipDelimeter) BOOL SkipDelimeter;
		INLINE VOID SetOutputFile(BOOL val) { _writefile = val; }
		INLINE BOOL GetOutputFile(VOID) const { return _writefile; }
		PROPERTY(GetOutputFile, SetOutputFile) BOOL OutputFile;
	public:
		ChannelBased(LPCSTR _filename, DWORD limit_pend = 50000);
	};

	class Channel : public ChannelBased {
	public:
		CHAR _name[33];
	public:
		READ_PROPERTY2(LPCSTR, Name, _name);
	public:
		Channel(LPCSTR _namech, LPCSTR _filename);
	};

	//////////////////////////////////////////////////
	
	HWND FIXAPI GetWindow(VOID);
	BOOL FIXAPI Initialize(VOID);
	VOID FIXAPI InitializeChannel(VOID);
	BOOL FIXAPI SaveRichTextToFile(LPCSTR _filename);

	LRESULT CALLBACK WndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	// WRITE TO CHANGE LOG CHANNEL

	VOID FIXAPI LogVa(LPCSTR Format, va_list Va);
	VOID FIXAPI Log(LPCSTR Format, ...);
	VOID FIXAPI LogWarningVa(INT32 Type, LPCSTR Format, va_list& Va);
	VOID FIXAPI LogWarning(INT32 Type, LPCSTR Format, ...);
	VOID FIXAPI LogWarningUnknown1(LPCSTR Format, ...);
	VOID FIXAPI LogWarningUnknown2(INT64 Unused, LPCSTR Format, ...);
	VOID FIXAPI LogAssert(LPCSTR File, INT32 Line, LPCSTR Message, ...);
	VOID FIXAPI LogInsteadOfMsgBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

	VOID FIXAPI LogWcVa(LPCWSTR Format, va_list Va);
	VOID FIXAPI LogWc(LPCWSTR Format, ...);

	// WRITE TO DEFAULT CHANNEL

	VOID FIXAPI LogVaDef(LPCSTR Format, va_list Va);
	VOID FIXAPI LogDef(LPCSTR Format, ...);

	VOID FIXAPI LogWcVaDef(LPCWSTR Format, va_list Va);
	VOID FIXAPI LogWcDef(LPCWSTR Format, ...);
}

// WRITE TO CHANGE LOG CHANNEL

#define _MESSAGE(x)					LogWindow::Log(x)
#define _MESSAGE_FMT(x, ...)		LogWindow::Log(x, ##__VA_ARGS__)
#define _MESSAGEW(x)				LogWindow::LogWc(x)
#define _MESSAGEW_FMT(x, ...)		LogWindow::LogWc(x, ##__VA_ARGS__)

// WRITE TO DEFAULT CHANNEL

#define _F4CKMSG(x)					LogWindow::LogDef(x)
#define _F4CKMSG_FMT(x, ...)		LogWindow::LogDef(x, ##__VA_ARGS__)
#define _F4CKMSGW(x)				LogWindow::LogWcDef(x)
#define _F4CKMSGW_FMT(x, ...)		LogWindow::LogWcDef(x, ##__VA_ARGS__)

#define _MESSAGE_BEGIN_PATCH(x)		_F4CKMSG_FMT("[F4CK] Initialize %s patch...", x);
#define _MESSAGE_END_PATCH			