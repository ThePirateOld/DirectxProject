#pragma once

class CTexture;
class CTimer;
class CColor;
class CMouse;
class CKeyboard;
class CWidget;
class CWindow;
class CButton;
class CCheckBox;
class CProgressBarHorizontal;
class CLogBox;
class CListBox;
class CListView;
class CDropDown;
class CScrollBarVertical;
class CRadioButton;
class CBox;
class CPictureBox;
class CTrackBarHorizontal;
class CScrollBarHorizontal;
class CTrackBarVertical;
class CEntryList;
class CScrollablePane;
class CTabPanel;
class CScrollbar;

#define UNISCRIBE_DLLNAME _UI("usp10.dll")
#define GETPROCADDRESS( Module, APIName, Temp ) \
    Temp = GetProcAddress( Module, #APIName ); \
    if( Temp ) \
        *(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC( APIName ) \
    _##APIName = Dummy_##APIName

#define IMM32_DLLNAME _UI("imm32.dll")
#define VER_DLLNAME _UI("version.dll")

#define DXUT_MAX_EDITBOXLENGTH 0xFFFF


#include <usp10.h>
#include <dimm.h>
#include <stdarg.h>
#include <stdio.h>
#include "renderers\CGraphics.h"

#include "TextUtils.h"
#include "LogFile.h"

#include "CVar.h"
#include "CTimer.h"

#include "CPos.h"
#include "CElement.h"
#include "Scrollbar.h"
#include "CMouse.h"
#include "CKeyboard.h"
#include "Entry.h"
#include "CFigure.h"
#include "Entry.h"
#include "ScrollbarHorizontal.h"
#include "CTrackBar.h"
#include "CText.h"
#include "CWindow.h"
#include "CScrollBarVertical.h"
#include "CButton.h"
#include "CRadioButton.h"
#include "CCheckBox.h"
#include "CEditBox.h"
#include "CDropDown.h"
#include "CTextBox.h"
#include "CListBox.h"
#include "CListView.h"
#include "CProgressBar.h"
#include "ProgressBarVertical.h"
#include "ScrollablePane.h"
#include "TrackBar.h"
#include "CTabPanel.h"


class CDialog
{
public:
	CDialog ( IDirect3DDevice9 *pDevice );
	~CDialog ();

	template<class T>
	T* CreateWidget(Pos pos, int nWidth, int nHeight, TCHAR *pszText)
	{
		CWidget* pWidget = new T(this);
		pWidget->SetPos(pos);
		pWidget->SetSize(nWidth, nHeight);
		pWidget->SetText(pszText);

		return static_cast<T*>(pWidget);
	}

	CProgressBarHorizontal *AddProgressBarHorizontal ( CWindow *pWindow, int X, int Y, int Width, int iHeight, float fMax, float fValue, tAction Callback = NULL );
	CProgressBarVertical *AddProgressBarVertical ( CWindow *pWindow, int X, int Y, int Width, int iHeight, float fMax, float fValue, tAction Callback = NULL );

	CWindow* AddWindow ( int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, bool bAlwaysOnTop = false, tAction Callback = NULL );
	CButton* AddButton ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CCheckBox* AddCheckBox ( CWindow *pWindow, int X, int Y, int Width, bool bChecked, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CLogBox* AddTextBox ( CWindow *pWindow, int X, int Y, int Width, int Height, tAction Callback = NULL );
	CListBox* AddListBox ( CWindow *pWindow, int X, int Y, int Width, int Height, tAction Callback = NULL );
	CListView* AddListView ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CLabel* AddLabel ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CEditBox* AddEditBox ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, bool bSelected = false, tAction Callback = NULL );
	CDropDown* AddDropDown ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CRadioButton *AddRadioButton ( CWindow *pWindow, int iGroup, int X, int Y, int Width, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CPictureBox *AddImage ( CWindow *pWindow, const TCHAR *szPath, int X, int Y, int Width, int Height, tAction Callback = NULL );
	CTabPanel *AddTabPanel ( CWindow *pWindow, int X, int Y, int Width, int Height, tAction Callback = NULL );

	CTrackBarHorizontal *AddTrackBar ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nValue, tAction Callback = NULL );
	CTrackBarVertical *AddTrackBarVertical ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nValue, tAction Callback = NULL );

	CScrollBarVertical *AddScrollBar ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nPagSize, int nValue, tAction Callback = NULL );
	CScrollBarHorizontal *AddScrollBarHorizontal ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nPagSize, int nValue, tAction Callback = NULL );

	void LoadTexture ( const SIMPLEGUI_CHAR *szPath, CD3DTexture **ppTexture = NULL );
	void LoadTexture ( LPCVOID pSrc, UINT uSrcSize, CD3DTexture **ppTexture = NULL );

	void RemoveTexture ( CD3DTexture *pTexture );

	void LoadFont ( const SIMPLEGUI_CHAR *szFontName, DWORD dwHeight = 15, bool bBold = false, CD3DFont **ppFont = NULL );
	void RemoveFont ( CD3DFont *pFont );

	void DrawFont ( SControlRect &rect, DWORD dwColor, const SIMPLEGUI_CHAR* szText, DWORD dwFlags, CD3DFont *pFont = NULL );
	void DrawBox ( SControlRect &rect, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias = true );
	void DrawTriangle ( SControlRect &rect, float fAngle, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias = true );
	void DrawCircle ( SControlRect &rect, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias = true );

	void SetFocussedWidget (CWidget *pWindow );
	void ClearFocussedWidget ( void );
	CWidget *GetFocussedWidget ( void );
	void BringWidgetToTop (CWidget *pWindow );

	CWidget *GetWindowByText ( const SIMPLEGUI_CHAR *pszText );
	CWidget *GetWidgetAtPos ( Pos pos );

	void AddWidget (CWidget *pWindow );
	void RemoveWidget (CWidget *pWindow );
	void RemoveAllWidgets ( void );

	bool IsWidgetInList ( CWidget *pWindow );

	void Draw ( void );

	void OnLostDevice ( void );
	void OnResetDevice ( void );

	void MsgProc ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	IDirect3DDevice9 *GetDevice ( void );

	CMouse *GetMouse ( void );
	CD3DRender *GetRenderer ( void );
	CD3DFont *GetFont ( int ID = 0 );
	CD3DTexture *GetTexture ( int ID = 0 );

	void SetVisible ( bool bVisible );
	bool IsVisible ( void );

private:

	CD3DStateBlock *m_pState;

	D3DVIEWPORT9 m_oldvp;

	bool m_bVisible;

	CMouse *m_pMouse;

	CD3DRender *m_pRender;
	IDirect3DDevice9 *m_pDevice;

	std::vector<CD3DFont*>m_vFont;
	std::vector<CD3DTexture*>m_vTexture;
	std::vector<CWidget*> m_vWidgets;

	CWidget *m_pFocussedWidget;
	CWidget *m_pMouseOverWidget;

	CRITICAL_SECTION cs;   
}; 