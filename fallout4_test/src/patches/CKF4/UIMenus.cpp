#include "../../common.h"
#include "UIMenus.h"
#include "LogWindow.h"
#include "EditorUI.h"

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			void CUIMenuItem::Click(void) const
			{
				PostMessageA(EditorUI::GetWindow(), WM_COMMAND, GetID(), 0);
			}

			void CUIMenuItem::SetText(const std::string& text)
			{
				MENUITEMINFOA m_mif = {0};
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_STRING;
				m_mif.fType = MFT_STRING;
				m_mif.cch = text.length();
				m_mif.dwTypeData = const_cast<char*>(&text[0]);

				SetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);
			}

			std::string CUIMenuItem::GetText(void) const
			{
				std::string str;
			
				MENUITEMINFOA m_mif = { 0 };
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_STRING;
				m_mif.fType = MFT_STRING;
				m_mif.dwTypeData = NULL;

				if (GetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif))
				{
					m_mif.cch++;
					str.resize(m_mif.cch);
					m_mif.dwTypeData = &str[0];
					GetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);
				}

				return str;
			}

			void CUIMenuItem::SetID(const UINT menu_id)
			{
				MENUITEMINFOA m_mif = { 0 };
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_ID;
				m_mif.wID = menu_id;

				SetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);
			}

			UINT CUIMenuItem::GetID(void) const
			{
				MENUITEMINFOA m_mif = { 0 };
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_ID;
		
				GetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);

				return m_mif.wID;
			}

			void CUIMenuItem::SetChecked(const BOOL value)
			{
				if (GetChecked() == value)
					return;

				MENUITEMINFOA m_mif = { 0 };
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_STATE;

				if (value)
					m_mif.fState = (GetEnabled() ? MFS_ENABLED : MFS_DISABLED) | MFS_CHECKED;
				else
					m_mif.fState = (GetEnabled() ? MFS_ENABLED : MFS_DISABLED) | MFS_UNCHECKED;

				SetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);
			}

			BOOL CUIMenuItem::GetChecked(void) const
			{
				MENUITEMINFOA m_mif = { 0 };
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_STATE;
				GetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);

				return (m_mif.fState & MFS_CHECKED) == MFS_CHECKED;
			}

			void CUIMenuItem::SetEnabled(const BOOL value)
			{
				if (GetEnabled() == value)
					return;

				MENUITEMINFOA m_mif = { 0 };
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_STATE;

				if (value)
					m_mif.fState = (GetChecked() ? MFS_CHECKED : MFS_UNCHECKED) | MFS_ENABLED;
				else
					m_mif.fState = (GetChecked() ? MFS_CHECKED : MFS_UNCHECKED) | MFS_DISABLED;

				SetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);
			}

			BOOL CUIMenuItem::GetEnabled(void) const
			{
				MENUITEMINFOA m_mif = { 0 };
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_STATE;
				GetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);

				return (m_mif.fState & MFS_ENABLED) == MFS_ENABLED;
			}

			BOOL CUIMenuItem::IsSeparate(void) const
			{
				MENUITEMINFOA m_mif = { 0 };
				m_mif.cbSize = sizeof(MENUITEMINFOA);
				m_mif.fMask = MIIM_FTYPE;
				GetMenuItemInfoA(m_Menu->Handle, m_Pos, m_ByPos, &m_mif);

				return (m_mif.fType & MFT_SEPARATOR) == MFT_SEPARATOR;
			}

			void CUIMenuItem::Remove(CUIMenuItem* MenuItem)
			{
				Assert(MenuItem && IsMenu(MenuItem->Menu()->Handle));
				DeleteMenu(MenuItem->Menu()->Handle, MenuItem->ID, MenuItem->ByPosition() ? MF_BYPOSITION : MF_BYCOMMAND);
			}

			void CUIMenuItem::Remove(CUIMenuItem& MenuItem)
			{
				Remove(&MenuItem);
			}

			// CUIMenu

			void CUIMenu::SetHandle(const HMENU value)
			{
				Assert(IsMenu(value));
				m_Handle = value;
			}

			HMENU CUIMenu::GetHandle(void) const
			{
				return m_Handle;
			}

			CUIMenu CUIMenu::CreateSubMenu(void)
			{
				return CreateMenu();
			}

			UINT CUIMenu::Count(void) const
			{
				return GetMenuItemCount(m_Handle);
			}

			void CUIMenu::Remove(const UINT MenuID)
			{
				Assert(IsMenu(m_Handle));
				DeleteMenu(m_Handle, MenuID, MF_BYCOMMAND);
			}

			void CUIMenu::RemoveByPos(const UINT Position)
			{
				Assert(IsMenu(m_Handle));
				DeleteMenu(m_Handle, Position, MF_BYPOSITION);
			}

			CUIMenuItem CUIMenu::GetItem(const UINT MenuID)
			{
				return CUIMenuItem(*this, MenuID, FALSE);
			}

			CUIMenuItem CUIMenu::GetItemByPos(const UINT Position)
			{
				return CUIMenuItem(*this, Position, TRUE);
			}

			BOOL CUIMenu::Insert(const std::string& Text, const UINT Position, const UINT MenuID, const BOOL Enabled, const BOOL Checked)
			{
				MENUITEMINFOA minfo = { 0 };
				minfo.cbSize = sizeof(MENUITEMINFOA);
				minfo.fMask = MIIM_STRING | MIIM_ID | MIIM_STATE;
				minfo.wID = (UINT)MenuID;
				minfo.cch = Text.length();
				minfo.dwTypeData = const_cast<char*>(&Text[0]);
				minfo.fState = (Enabled ? MFS_ENABLED : MFS_DISABLED) | (Checked ? MFS_CHECKED : MFS_UNCHECKED);
				return InsertMenuItemA(m_Handle, Position, TRUE, &minfo);
			}

			BOOL CUIMenu::Append(const std::string& Text, const UINT MenuID, const BOOL Enabled, const BOOL Checked)
			{
				return Insert(Text, -1, MenuID, Enabled, Checked);
			}

			BOOL CUIMenu::Insert(const std::string& Text, const UINT Position, const CUIMenu& Menu, const BOOL Enabled)
			{
				MENUITEMINFOA minfo = { 0 };
				minfo.cbSize = sizeof(MENUITEMINFOA);
				minfo.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_STATE;
				minfo.hSubMenu = Menu.Handle;
				minfo.cch = Text.length();
				minfo.dwTypeData = const_cast<char*>(&Text[0]);
				minfo.fState = Enabled ? MFS_ENABLED : MFS_DISABLED;
				return InsertMenuItemA(m_Handle, Position, TRUE, &minfo);
			}

			BOOL CUIMenu::Append(const std::string& Text, const CUIMenu& Menu, const BOOL Enabled)
			{
				return Insert(Text, -1, Menu, Enabled);
			}

			BOOL CUIMenu::InsertSeparator(const UINT Position)
			{
				MENUITEMINFOA minfo = { 0 };
				minfo.cbSize = sizeof(MENUITEMINFOA);
				minfo.fMask = MIIM_FTYPE | MIIM_STATE;
				minfo.fType = MFT_SEPARATOR;
				return InsertMenuItemA(m_Handle, Position, TRUE, &minfo);
			}

			BOOL CUIMenu::AppendSeparator(void)
			{
				return InsertSeparator(-1);
			}

			CUIMenu CUIMenu::GetSubMenuItem(const UINT Position)
			{
				return GetSubMenu(m_Handle, Position);
			}

			CUIMenuItem CUIMenu::First(void)
			{
				return CUIMenuItem(*this, 0, TRUE);
			}

			CUIMenuItem CUIMenu::Last(void)
			{
				return CUIMenuItem(*this, Count() - 1, TRUE);
			}
		}
	}
}