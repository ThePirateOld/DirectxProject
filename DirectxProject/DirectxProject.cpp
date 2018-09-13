#include "plugin.h"

#include "d3dx9.h"

#include "rwd3d9.h"
#include <stdio.h>

using namespace plugin;

#include "GUI\CGUI.h"

// Windows
CWindow * fServBrowser;
CWindow * fChat;
CWindow * fOption;
CWindow * fUserRegister;
CWindow * fUserLogin;
CWindow * fInfo;
CWindow * fEnterNick;
// Chat elements
CLogBox * cc_tChat;
CEditBox * cc_tEnter;
CButton * cc_bEnter;
// Server Browser elemenets
CListView *sbServList, *sbPlayerList;
CButton *sbTab[4];
CEditBox *sbEnterIP, *sbEnterPort;
CButton *sbConnect, *sbRefresh, *sbAddToFav;
CDropDown *sbFltPing;
CCheckBox *sbFltNotFull, *sbFltNoPassword, *sbFltNotEmpty;
CEditBox *sbFltLocation, *sbFltMode;
// Register elements
CEditBox *urLogin, *urPass, *urConfirm, *urEmail, *urNick;
CLabel *urLoginText, *urPassText, *urConfirmText, *urEmailText, *urNickText, *urInfoText;
CButton *urSendReg, *urCancelReg;
// Login elements
CLabel *upLoginInfo, *upStrLogin, *upStrPass;
CEditBox *upLogin, *upPassword;
CButton *upSendLogin, *upShowRegister;
CCheckBox *upRemeberMe;
// Enter nick
CEditBox * enNick;
CButton * enOK;
CDialog *pGui = NULL;

class DXFont {
public:
	static ID3DXFont *m_pD3DXFont;

	static void InitFont () {
		IDirect3DDevice9 *device = reinterpret_cast<IDirect3DDevice9 *>(RwD3D9GetCurrentD3DDevice ());
		if (FAILED (D3DXCreateFontA (device, 48, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "arial", &m_pD3DXFont)))
		{
			Error ("Failed to create D3DX font!");
		}
		pGui = new CDialog (device);
		pGui->LoadFont (_UI ("arial"), 9, false);


		CWindow *pWindow = pGui->CreateWidget<CWindow> (Pos (200, 200), 700, 100, _UI ("text"));
		CWindow *pWindow2 = pGui->CreateWidget<CWindow> (Pos (200, 200), 700, 500, _UI ("text 2"));
		pWindow2->SetAlwaysOnTop (true);

		CEditBox *pEditBox = pGui->CreateWidget<CEditBox> (Pos (0, 220), 700, 20, _UI ("text 2"));
		
		CButton *pButton = pGui->CreateWidget<CButton> (Pos (200, 250), 100, 10, _UI ("text"));
		CButton *pButton2 = pGui->CreateWidget<CButton> (Pos (200, 350), 100, 10, _UI ("text 2"));
		CListView *pListView = pGui->CreateWidget<CListView> (Pos (210, 0), 600, 500, _UI ("text 2"));
		CRadioButton *pRadio = pGui->CreateWidget<CRadioButton> (Pos (210, 120), 600, 500, _UI ("text 2"));


		pListView->AddColumn (_UI ("Column 1"), 350);
		pListView->AddColumn (_UI ("Column 2"), 350);
		//pListView->AddColumn ( _UI ( "Column 3" ), 50 );
		//pListView->AddColumn ( _UI ( "Column 4" ), 50 );

		pListView->AddColumnItem (0, _UI ("1 3"));
		pListView->AddColumnItem (0, _UI ("1 3"));
		pListView->AddColumnItem (0, _UI ("13"));
		pListView->AddColumnItem (1, _UI ("1 4"));
		pListView->AddColumnItem (1, _UI ("1 4"));
		pListView->AddColumnItem (0, _UI ("1 3"));
		pListView->AddColumnItem (1, _UI ("1 4"));



		/*for ( size_t i = 0; i < 400; i++ )
		{
		int n = 0;
		if ( i >= 100 )
		n = i / 100;
		TCHAR szChar [ 128 ];
		SIMPLEGUI_SPRINTF ( szChar, _UI ( "Item %i" ), i );
		pListView->AddColumnItem ( n, szChar );
		}*/

		pListView->SetSortable (true);


		CTabPanel *pTabPanel = pGui->CreateWidget<CTabPanel> (Pos (123, 0), 1100, 100, _UI ("text 2"));
		pTabPanel->AddTab (_UI ("Tab 1"), 200);
		pTabPanel->AddTab (_UI ("Tab 2"), 200);
		pTabPanel->AddTab (_UI ("Tab 3"), 200);
		pTabPanel->AddTab (_UI ("Tab 3"), 200);

		pTabPanel->SetRelativeX (CWidget::eRelative::RELATIVE_SIZE);
		pTabPanel->SetRelativeY (CWidget::eRelative::RELATIVE_POS);

		//pListView->SetRelativeX ( CWidget::eRelative::RELATIVE_SIZE );
		//pListView->SetRelativeY ( CWidget::eRelative::RELATIVE_SIZE );


		pTabPanel->AddControl (0, pButton);
		pTabPanel->AddControl (1, pButton2);
		//pTabPanel->AddControl ( 1, pRadio );
		pTabPanel->AddControl (2, pListView);
		//pGui->AddWidget ( pTabPanel );
		//pWindow->AddControl ( pListView );
		pWindow2->AddControl (pTabPanel);

		/*pGui->AddWidget ( pButton );
		pGui->AddWidget ( pButton2 );*/
		pGui->AddWidget (pEditBox);
		pGui->AddWidget (pWindow);
		pGui->AddWidget (pWindow2);
		pGui->AddWidget (pListView);

		pGui->SetVisible (true);
		//LeaveCriticalSection ( &cs_gui );
	}

	static void DestroyFont () {
		if (m_pD3DXFont) {
			m_pD3DXFont->Release ();
			m_pD3DXFont = NULL;
		}
		if (pGui)
			pGui->OnLostDevice ();
	}

	static void OnResetDevice ()
	{
		//EnterCriticalSection ( &cs_gui );
		if (pGui)
			pGui->OnResetDevice ();
		//LeaveCriticalSection ( &cs_gui );
	}

	static void Draw () {
		if (m_pD3DXFont) {
			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.bottom = RsGlobal.maximumHeight;
			rect.right = RsGlobal.maximumWidth;
			std::string gameName = std::string ("D3DXFont example for GTA ") + GTAGAME_NAME;
			m_pD3DXFont->DrawTextA (NULL, gameName.c_str (), -1, &rect, DT_CENTER | DT_VCENTER, D3DCOLOR_RGBA (255, 255, 0, 255));
		}
		if (pGui)
			pGui->Draw ();
	}

	DXFont () {
		m_pD3DXFont = NULL;
		Events::drawingEvent.Add (Draw);
		Events::initRwEvent.Add (InitFont);
		Events::shutdownRwEvent.Add (DestroyFont);
		Events::d3dLostEvent.Add (DestroyFont);
		Events::d3dResetEvent.Add (OnResetDevice);
	}
} dxFont;

ID3DXFont *DXFont::m_pD3DXFont;