#pragma once

#include "CGUI.h"


class CDropDown : public CWidget
{
public:
	CDropDown ( CDialog *pDialog );
	~CDropDown ( void );

	void Draw ( void );

	void AddItem ( SEntryItem *pEntry );
	void AddItem ( const SIMPLEGUI_CHAR *szText, const SIMPLEGUI_CHAR *szValue = _UI ( "" ) );

	void RemoveItem ( SEntryItem *pEntry );
	void RemoveItem ( UINT uIndex );

	SEntryItem *GetSelectedItem ( void );
	int GetSelectedIndex ( void ) { return m_iSelected; }

	void SetSelectedItem ( SEntryItem *pEntry, bool bSelect );
	void SetSelectedItemByIndex ( UINT uIndex, bool bSelect );

	void OnClickLeave ( void );
	bool OnClickEvent ( void );

	void OnFocusIn ( void );
	void OnFocusOut ( void );

	void OnMouseEnter ( void );
	void OnMouseLeave ( void );
	bool OnMouseOver ( void );
	bool OnKeyDown ( WPARAM wParam );

	bool OnMouseButtonDown ( sMouseEvents e );
	bool OnMouseButtonUp ( sMouseEvents e );

	bool OnMouseMove ( CVector pos );
	bool OnMouseWheel ( int zDelta );

	bool CanHaveFocus ( void );

	void SetSortedList ( bool bSort );

	void SetPageSize ( UINT uSize )
	{
		m_uSize = uSize;
	}

	UINT GetPageSize ( void )
	{
		return m_uSize;
	}

	void OpenBox ( void );
	void CloseBox ( void );

	void UpdateRects ( void );
	bool ContainsPoint ( CVector pos );

private:
	CEntryList *m_pEntryList;

	bool m_bOpened;
	
	UINT m_uSize;
	int m_iIndex;
	int m_iSelected;

	SControlRect m_rBack;
};