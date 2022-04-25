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

#include "CommInc.h"

class IProgressTaskBarContext {
private:
	ITaskbarList3* m_ptbl;
public:
	IProgressTaskBarContext(VOID);
	virtual ~IProgressTaskBarContext(VOID);
public:
	__forceinline ITaskbarList3* operator->(VOID) { return m_ptbl; }
	__forceinline const ITaskbarList3* operator->(VOID) const { return m_ptbl; }
public:
	BOOL SetProgressState(HWND _linkWnd, DWORD _state);
public:
	static IProgressTaskBarContext* GetInstance(VOID);
	static IProgressTaskBarContext* CreateInstance(VOID);
	static VOID DeleteInstance(VOID);
};


class IProgressTaskBar {
private:
	HWND m_LinkWnd;
	BOOL m_Marquee;
	ULONGLONG m_ullCompleted;
	ULONGLONG m_ullTotal;
public:
	IProgressTaskBar(HWND _LinkWnd, ULONGLONG _total);
	virtual ~IProgressTaskBar(VOID);
public:
	__forceinline BOOL IsMarquee(VOID) const { return m_Marquee; }
public:
	VOID SetMarquee(BOOL _value);
	VOID SetPosition(ULONGLONG _complete);
public:
	static IProgressTaskBar* GetInstance(VOID);
	static IProgressTaskBar* CreateInstance(VOID);
	static VOID DeleteInstance(VOID);
};

