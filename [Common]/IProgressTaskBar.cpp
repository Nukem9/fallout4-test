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

// Let's make the program a little more modern, transfer the progress and TaskBar...
// https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-itaskbarlist3

#include "StdAfx.h"

//
// ONLY CK
//
#include "..\[Patches]\CKF4\MainWindow.h"

IProgressTaskBar* g_IProgressTaskBar = NULL;
IProgressTaskBarContext* g_IProgressTaskBarContext = NULL;

IProgressTaskBarContext::IProgressTaskBarContext(VOID) : m_ptbl(NULL) {
	ITaskbarList3* ptbl = NULL;
	if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ptbl)))) {
		if (SUCCEEDED(ptbl->HrInit()))
			m_ptbl = ptbl;
		else {
			ptbl->Release();
			_MESSAGE("ITaskbarList::HrInit() has failed.");
		}
	}
	else
		_MESSAGE("ITaskbarList could not be created.");
}

IProgressTaskBarContext::~IProgressTaskBarContext(VOID) {
	if (m_ptbl) {
		m_ptbl->Release();
		m_ptbl = NULL;
	}
}

BOOL IProgressTaskBarContext::SetProgressState(HWND _linkWnd, DWORD _state) {
	return SUCCEEDED(m_ptbl->SetProgressState(_linkWnd, (TBPFLAG)_state));
}

IProgressTaskBarContext* IProgressTaskBarContext::GetInstance(VOID) {
	return g_IProgressTaskBarContext;
}

IProgressTaskBarContext* IProgressTaskBarContext::CreateInstance(VOID) {
	if (g_IProgressTaskBarContext)
		return g_IProgressTaskBarContext;

	g_IProgressTaskBarContext = new IProgressTaskBarContext();
	return g_IProgressTaskBarContext;
}

VOID IProgressTaskBarContext::DeleteInstance(VOID) {
	if (g_IProgressTaskBarContext) {
		delete g_IProgressTaskBarContext;
		g_IProgressTaskBarContext = NULL;
	}
}

IProgressTaskBar::IProgressTaskBar(HWND _LinkWnd, ULONGLONG _total) : 
	m_LinkWnd(NULL), m_Marquee(FALSE), m_ullCompleted(0), m_ullTotal(std::max<ULONGLONG>(1, _total)) {

	if (!g_IProgressTaskBarContext)
		_MESSAGE("IProgressTaskBarContext not be created.");
	else if (!IsWindow(_LinkWnd))
		_MESSAGE("IProgressTaskBar could not be created, because _LinkWnd isn't window.");
	else {
		if (g_IProgressTaskBarContext->SetProgressState(_LinkWnd, TBPF_NOPROGRESS))
			m_LinkWnd = _LinkWnd;
		else
			_MESSAGE("ITaskbarList::SetProgressState() has failed.");
	}
}

IProgressTaskBar::~IProgressTaskBar(VOID) {
	if (g_IProgressTaskBarContext && m_LinkWnd)
		g_IProgressTaskBarContext->SetProgressState(m_LinkWnd, TBPF_NOPROGRESS);
}


VOID IProgressTaskBar::SetMarquee(BOOL _value) {
	if (g_IProgressTaskBarContext && m_LinkWnd && (_value != m_Marquee)) {
		if (_value)
			m_Marquee = g_IProgressTaskBarContext->SetProgressState(m_LinkWnd, TBPF_INDETERMINATE);
		else {
			if (m_Marquee = !g_IProgressTaskBarContext->SetProgressState(m_LinkWnd, TBPF_NORMAL); !m_Marquee)
				(*g_IProgressTaskBarContext)->SetProgressValue(m_LinkWnd, m_ullCompleted, m_ullTotal);
		}
	}
}

VOID IProgressTaskBar::SetPosition(ULONGLONG _complete) {
	if (!m_Marquee) {
		m_ullCompleted = std::max<ULONGLONG>(0, std::min<ULONGLONG>(_complete, m_ullTotal));
		(*g_IProgressTaskBarContext)->SetProgressValue(m_LinkWnd, m_ullCompleted, m_ullTotal);
	}
}

IProgressTaskBar* IProgressTaskBar::GetInstance(VOID) {
	return g_IProgressTaskBar;
}

IProgressTaskBar* IProgressTaskBar::CreateInstance(VOID) {
	if (g_IProgressTaskBar)
		return g_IProgressTaskBar;

	g_IProgressTaskBar = new IProgressTaskBar(MainWindow::GetWindow(), 95);
	return g_IProgressTaskBar;
}

VOID IProgressTaskBar::DeleteInstance(VOID) {
	if (g_IProgressTaskBar) {
		delete g_IProgressTaskBar;
		g_IProgressTaskBar = NULL;
	}
}