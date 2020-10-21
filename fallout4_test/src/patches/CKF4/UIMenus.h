#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			class CUIMenu;
			class CUIMenuItem
			{
			private:
				CUIMenu* m_Menu;
				BOOL m_ByPos;
				UINT m_Pos;
			public:
				void SetText(const std::string &text);
				std::string GetText(void) const;
				void SetID(const UINT menu_id);
				UINT GetID(void) const;
				void SetChecked(const BOOL value);
				BOOL GetChecked(void) const;
				void SetEnabled(const BOOL value);
				BOOL GetEnabled(void) const;
				BOOL IsSeparate(void) const;
				inline CUIMenu* Menu(void) const { return m_Menu; };
				inline BOOL ByPosition(void) const { return m_ByPos; };
			public:
				static void Remove(CUIMenuItem* MenuItem);
				static void Remove(CUIMenuItem& MenuItem);
			public:
				CUIMenuItem(void) :
					m_Menu(NULL), m_Pos(0), m_ByPos(TRUE)
				{}
				CUIMenuItem(CUIMenu& Menu, const UINT Position, const BOOL ByPosition = TRUE) :
					m_Menu(&Menu), m_Pos(Position), m_ByPos(ByPosition)
				{}
				CUIMenuItem(const CUIMenuItem &base) : 
					m_Menu(base.m_Menu), m_Pos(base.m_Pos), m_ByPos(base.m_ByPos)
				{}
			public:
				__declspec(property(get = GetChecked, put = SetChecked)) const BOOL Checked;
				__declspec(property(get = GetEnabled, put = SetEnabled)) const BOOL Enabled;
				__declspec(property(get = GetID, put = SetID)) const UINT ID;
				__declspec(property(get = GetText, put = SetText)) std::string Text;
			};

			class CUIMenu
			{
			private:
				HMENU m_Handle;
			public:
				void SetHandle(const HMENU value);
				HMENU GetHandle(void) const;
			public:
				UINT Count(void) const;
				BOOL Insert(const std::string &Text, const UINT Position, const UINT MenuID, const BOOL Enabled = TRUE, const BOOL Checked = FALSE);
				BOOL Append(const std::string &Text, const UINT MenuID, const BOOL Enabled = TRUE, const BOOL Checked = FALSE);
				BOOL Insert(const std::string& Text, const UINT Position, const CUIMenu &Menu, const BOOL Enabled = TRUE);
				BOOL Append(const std::string& Text, const CUIMenu &Menu, const BOOL Enabled = TRUE);
				BOOL InsertSeparator(const UINT Position);
				BOOL AppendSeparator(void);
				void Remove(const UINT MenuID);
				void RemoveByPos(const UINT Position);
				inline BOOL IsEmpty(void) const { return (BOOL)Count(); }
				CUIMenuItem GetItem(const UINT MenuID);
				CUIMenuItem GetItemByPos(const UINT Position);
				CUIMenuItem First(void);
				CUIMenuItem Last(void);
				CUIMenu GetSubMenuItem(const UINT Position);
			public:
				static CUIMenu CreateSubMenu(void);
			public:
				CUIMenu(void) : m_Handle(NULL) {}
				CUIMenu(const HMENU menu) : m_Handle(menu) {}
				CUIMenu(const CUIMenu& base) : m_Handle(base.m_Handle) {}
			public:
				__declspec(property(get = GetHandle, put = SetHandle)) const HMENU Handle;
			};
		}
	}
}