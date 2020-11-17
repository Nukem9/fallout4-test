#pragma once

#include "UIBaseWindow.h"

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			class CUICheckbox : public CUIBaseControl
			{
			private:
				BOOL m_Checked;
				UINT m_MenuId;
			public:
				void CreateWnd(const CUIBaseWindow &parent, const CUIBaseControl& control, const UINT menu_id);
				void CreateWnd(const CUIBaseWindow &parent, const std::string &caption, const LONG l, const LONG t, const LONG w, const LONG h, const UINT menu_id);
				void SetChecked(const BOOL value);
				BOOL GetChecked(void) const;
			public:
				PROPERTY(GetChecked, SetChecked) BOOL Checked;
			};
		}
	}
}