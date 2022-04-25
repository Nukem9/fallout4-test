#pragma once

/*
An apple fell on my head :D
I came up with the idea of using simple languages to generate dialogs.
This should solve the problem of bad dialogs in the future and support 10pt and 12pt text in dialogs.
*/

#include "..\..\[Common]\StdAfx.h"

namespace Core
{
	namespace Classes
	{
		namespace UI
		{	
			using jDialog = perchik71::jDialogs::jDialogA;

			enum CDialogFontType
			{
				jdtUnknown,
				jdt8pt,
				jdt10pt,
				jdt12pt,
			};

			typedef class CDialogManager
			{
			private:
				using tbb_map = tbb::concurrent_map<ULONG, jDialog*>;
			private:
				BOOL m_bInit;
				tbb_map m_items_8pt;
				tbb_map m_items_10pt;
				tbb_map m_items_12pt;
			private:
				tbb_map* GetItems(const CDialogFontType type);
			public:
				BOOL IsDialog(const LONG uid, const CDialogFontType type);
				BOOL AddDialog(const std::string& json_file, const LONG uid, const CDialogFontType type);
				BOOL AddDialogByCode(const std::string& json_code, const LONG uid, const CDialogFontType type);
				jDialog* GetDialog(const LONG uid, const CDialogFontType type);
			public:
				BOOL Empty(VOID) const;
			protected:
				VOID Init(VOID);
				VOID Release(VOID);
			public:
				CDialogManager(VOID);
				virtual ~CDialogManager(VOID);
			} *PDialogManager, *LPDialogManager;
		}
	}
}

extern Core::Classes::UI::LPDialogManager g_DialogManager;