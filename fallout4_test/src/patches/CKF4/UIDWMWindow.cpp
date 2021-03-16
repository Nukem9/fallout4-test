#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "UIDWMWindow.h"
#include "UIGraphics.h"

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			LRESULT CUIDWMWindow::ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				LRESULT lRet = S_OK;
				m_Enabled = FALSE;

				if (!IsWindow(hWnd))
					return S_FALSE;

				DwmDefWindowProc(hWnd, uMsg, wParam, lParam, &lRet);

				m_hWnd = hWnd;
				m_Enabled = TRUE;

				return S_OK;
			}

			VOID CUIDWMWindow::CloseProcessMessage(VOID)
			{
				m_Enabled = FALSE;
				m_hWnd = NULL;
			}

			BOOL CUIDWMWindow::ExtendFrameIntoClientArea(MARGINS* lpMargins) const
			{
				if (!m_Enabled)
					return FALSE;

				return DwmExtendFrameIntoClientArea(m_hWnd, lpMargins) == S_OK;
			}
		}
	}
}