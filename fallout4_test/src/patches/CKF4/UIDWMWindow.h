#include "..\ComClasses.h"

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			class CUIDWMWindow
			{
			private:
				HWND m_hWnd;
				BOOL m_Enabled;
			public:
				LRESULT ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
				VOID CloseProcessMessage(VOID);
				BOOL ExtendFrameIntoClientArea(MARGINS* lpMargins) const;
			public:
				CUIDWMWindow(VOID) : m_hWnd(NULL) {}
			};
		}
	}
}