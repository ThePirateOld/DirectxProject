#pragma once

#include "CGUI.h"
class CTabPanel : public CWidget
{
public:
	CTabPanel ( CDialog *pDialog );
	~CTabPanel ( void );

	void AddTab ( const SIMPLEGUI_CHAR *szTabName, int nWidth );

	void RemoveTab ( const SIMPLEGUI_CHAR *szTabName );
	void RemoveTab ( UINT nTabID );
	void RemoveAllTabs ( void );

	void AddControl ( UINT nTabID, CWidget *pControl );
	void RemoveControl ( UINT nTabID, CWidget *pControl );
	
	void RemoveAllControlsFromTab ( UINT nTabID );
	void RemoveAllControls ( void );

	bool IsControlInList ( CWidget *pControl );
	std::vector<CWidget*>::iterator GetControlIteratorInList ( CWidget *pControl );


	int GetControlTab ( CWidget*pWidget );
	void ResizeWidget ( CWidget *pWidget, bool bCheckInList = false );

	void SetFocussedControl ( CWidget *pControl );
	void BringControlToTop ( UINT nTabID, CWidget *pControl );

	CWidget *GetFocussedControl ( void );

	int GetAllColumnsWidth ( void );
	UINT GetNumOfTabsVisible ( void );

	void ClearControlFocus ( void );

	void MoveControl ( CWidget *pControl, UINT nTabPosition );

	void SetSelectedTab ( UINT nTabID );
	int GetSelectedTab ( void );

	int GetTabIdAtArea ( Pos pos );

	CWidget *GetControlAtArea ( UINT nTabID, Pos pos );

	int GetTabSizeY ( void );
	SIZE GetSize ( void );

	void Draw ( void );

	bool OnMouseButtonDown ( sMouseEvents e );
	bool OnMouseButtonUp ( sMouseEvents e );

	bool OnMouseMove ( CVector pos );
	bool OnMouseWheel ( int zDelta );

	bool ControlMessages ( sControlEvents e );

	void UpdateRects ( void );
	bool ContainsPoint ( CVector pos );

	void OnClickLeave ( void );
	bool OnClickEvent ( void );

	void OnFocusIn ( void );
	void OnFocusOut ( void );

	void OnMouseEnter ( void );
	void OnMouseLeave ( void );

private:
	bool m_bOldState;
	void UpdateScrollbars ( void );

	struct STabList
	{
		CWidget *pFocussedControl;
		CWidget *pMouseOverControl;

		std::vector<CWidget*> vControls;
		SIMPLEGUI_STRING sTabName;

		int nTrackX;
		int nTrackY;

		SIZE nMaxControlSize;

		int nWidth;
	};

	struct STabButton
	{
		SControlRect m_rButton;

		bool bClickedButton;
		bool bOverButton;
		bool bVisible;

		bool InArea (Pos pos )
		{
			return ( bVisible && m_rButton.ContainsPoint ( pos ) );
		}

		bool OnMouseOver (	Pos pos )
		{
			return ( bOverButton && InArea ( pos ) );
		}

		bool OnClickEvent ( Pos pos )
		{
			return ( bClickedButton && InArea ( pos ) );
		}
	};

	STabButton m_sLeftButton;
	STabButton m_sRightButton;

 	int m_nOldAreaX;

	UINT m_nNextTab;
	UINT m_nSelectedTab;

	int m_nOverTabId;

	std::vector<STabList> m_TabList;
	SIZE m_maxControlSize;
	
	CD3DFont *m_pTitleFont;

	SControlRect m_rTabArea;
	SControlRect m_rPanelArea;

	CScrollablePane *m_pScrollbar;
};

