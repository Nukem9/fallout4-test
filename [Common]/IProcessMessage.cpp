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

#include "IProcessMessage.h"

BOOL FIXAPI IProcessMessage::CloseHandlerDefault(VOID) {
	return TRUE;
}

VOID FIXAPI IProcessMessage::ProcessMessage(HWND hWindow, DWORD dwMsgFilterMin, DWORD dwMsgFilterMax, DWORD flag,
	CloseHandlerT CloseHandler) {
	MSG msg;

	auto loop = (flag & Loop) == Loop;

	DWORD msg_flag = ((flag & RemoveMessage) == RemoveMessage) ? PM_REMOVE : PM_NOREMOVE;
	if ((flag & NoYield) == NoYield)
		msg_flag |= PM_NOYIELD;

	if ((flag & ProcessingInput) == ProcessingInput)
		msg_flag |= PM_QS_INPUT;

	if ((flag & ProcessingPaint) == ProcessingPaint)
		msg_flag |= PM_QS_PAINT;

	if ((flag & ProcessingSends) == ProcessingSends)
		msg_flag |= PM_QS_SENDMESSAGE;

	if ((flag & ProcessingPosts) == ProcessingPosts)
		msg_flag |= PM_QS_POSTMESSAGE;

	if (loop) {
		if (IsWindow(hWindow)) {
			while (CloseHandler()) {
				if (PeekMessageA(&msg, hWindow, dwMsgFilterMin, dwMsgFilterMax, msg_flag)) {
					TranslateMessage(&msg);
					DispatchMessageA(&msg);

					if (msg.message == WM_CLOSE)
						break;
				}

				Sleep(1);
			}
		}
		else {
			while (CloseHandler()) {
				if (PeekMessageA(&msg, hWindow, dwMsgFilterMin, dwMsgFilterMax, msg_flag)) {
					TranslateMessage(&msg);
					DispatchMessageA(&msg);

					if (msg.message == WM_QUIT)
						break;
				}

				Sleep(1);
			}
		}
	}
	else {
		while (PeekMessageA(&msg, hWindow, dwMsgFilterMin, dwMsgFilterMax, msg_flag)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);

			Sleep(1);
		}
	}
}

VOID FIXAPI IProcessMessage::ProcessMessageAsync(HWND hWindow, DWORD dwMsgFilterMin, DWORD dwMsgFilterMax, DWORD flag,
	CloseHandlerT CloseHandler) {
	std::thread processing_th([](HWND _hWindow, DWORD _dwMsgFilterMin, DWORD _dwMsgFilterMax, DWORD _flag,
		CloseHandlerT _CloseHandler) {
		ProcessMessage(_hWindow, _dwMsgFilterMin, _dwMsgFilterMax, _flag, _CloseHandler);
	}, hWindow, dwMsgFilterMin, dwMsgFilterMax, flag, CloseHandler);
	processing_th.detach();
}