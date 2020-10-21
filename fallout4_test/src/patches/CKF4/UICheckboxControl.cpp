#include "..\..\Common.h"
#include "UICheckboxControl.h"

#pragma warning(disable: 4312)

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			void CUICheckbox::CreateWnd(const CUIBaseWindow &parent, const std::string &caption, const LONG l, const LONG t, const LONG w, const LONG h, const UINT menu_id)
			{
				Assert(!m_hWnd);
				Assert(menu_id);
				Assert(parent.Is());

				m_hWnd = CreateWindowExA(NULL, "button", caption.c_str(), 
					WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 
					l, t, w, h, parent.Handle, 
					(HMENU)menu_id,
					GetModuleHandle(NULL), NULL);

				Font = parent.Font;
				Font.Apply(m_hWnd);

				Assert(m_hWnd);

				m_Checked = FALSE;
				CheckDlgButton(m_hWnd, menu_id, BST_UNCHECKED);
				m_MenuId = menu_id;
			}

			void CUICheckbox::SetChecked(const BOOL value)
			{
				if (m_Checked == value)
					return;
				
				m_Checked = value;

				if (m_Checked)
					CheckDlgButton(Parent(), m_MenuId, BST_CHECKED);
				else
					CheckDlgButton(Parent(), m_MenuId, BST_UNCHECKED);
			}

			BOOL CUICheckbox::GetChecked(void) const
			{
				return m_Checked;
			}
		}
	}
}

