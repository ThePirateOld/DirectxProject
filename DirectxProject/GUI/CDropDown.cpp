#include "CGUI.h"

#define TEXTBOX_TEXTSPACE 18

//--------------------------------------------------------------------------------------
CDropDown::CDropDown ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_DROPDOWN );

	m_sControlColor.d3dColorFont = D3DCOLOR_RGBA ( 0, 0, 0, 255 );
	m_sControlColor.d3dColorSelectedFont = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

	m_iSelected = m_iIndex = -1;
	m_uSize = 20;

	m_pEntryList = new CEntryList ( pDialog );

	if ( !m_pEntryList )
		MessageBox ( 0, _UI ( "CDropDown::CDropDown: Error for creating CEntryList" ), _UI ( "GUIAPI.asi" ), 0 );

	m_pEntryList->GetScrollbar ()->SetControl ( this );
}

//--------------------------------------------------------------------------------------
CDropDown::~CDropDown()
{
	SAFE_DELETE ( m_pEntryList );
}

//--------------------------------------------------------------------------------------
void CDropDown::Draw ()
{
	if ( !m_bVisible ||
		 !m_pEntryList )
		return;

	CWidget::Draw ();
	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBox [ m_eState ], m_sControlColor.d3dColorOutline, m_bAntAlias );

	SControlRect rText = m_rBoundingBox;
	rText.m_pos.m_nX += 2;
	rText.m_pos.m_nY += m_rBoundingBox.m_size.cy / 2 - 8;

	SControlRect rShape = m_rBoundingBox;
	rShape.m_pos.m_nX += m_rBoundingBox.m_size.cx - 12;
	rShape.m_pos.m_nY += m_rBoundingBox.m_size.cy / 2 - 1;
	rShape.m_size.cx = 7;

	SIMPLEGUI_STRING str ( GetSelectedItem () ? GetSelectedItem ()->m_sText.c_str () : GetText () );

	m_pFont->CutString ( rText.m_size.cx - ( rShape.m_size.cx + 10 ), str );
	m_pDialog->DrawFont ( rText, m_sControlColor.d3dColorSelectedFont, str.c_str (), 0, m_pFont );

	m_pDialog->DrawTriangle ( rShape, 180.f, m_sControlColor.d3dColorShape, 0, m_bAntAlias );
	//SetScissor ( m_pDialog->GetDevice (), m_rBack.GetRect () );

	if ( m_bPressed )
	{
		m_pDialog->DrawBox ( m_rBack, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline, m_bAntAlias );
		m_pEntryList->Render ( m_rBack,
							   m_sControlColor.d3dColorBoxSel,
							   m_sControlColor.d3dColorBox [ SControlColor::STATE_PRESSED ],
							   m_sControlColor.d3dColorSelectedFont,
							   m_sControlColor.d3dColorFont,
							   m_iIndex );
	}
}

void CDropDown::AddItem ( SEntryItem *pEntry )
{
	m_pEntryList->AddEntry ( pEntry );
}

void CDropDown::AddItem ( const SIMPLEGUI_CHAR *szText, const SIMPLEGUI_CHAR *szValue )
{
	m_pEntryList->AddEntry ( new SEntryItem ( szText, szValue ) );
}

void CDropDown::RemoveItem ( SEntryItem *pEntry )
{
	m_pEntryList->RemoveEntry ( pEntry );
}

void CDropDown::RemoveItem ( UINT uIndex )
{
	m_pEntryList->RemoveEntry ( m_pEntryList->GetEntryByIndex ( uIndex ) );
}

SEntryItem *CDropDown::GetSelectedItem ( void )
{
	return m_pEntryList->GetSelectedEntry ();
}

void CDropDown::SetSelectedItem ( SEntryItem *pEntry, bool bSelect )
{
	m_pEntryList->SetSelectedEntry ( pEntry, bSelect );
}

void CDropDown::SetSelectedItemByIndex ( UINT uIndex, bool bSelect )
{
	m_pEntryList->SetSelectedEntryByIndex ( uIndex, bSelect );
}

void CDropDown::OnClickLeave ( void )
{
	CWidget::OnClickLeave ();

	m_pEntryList->GetScrollbar ()->OnClickLeave ();
}

bool CDropDown::OnClickEvent ( void )
{
	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	return ( CWidget::OnClickEvent () ||
			 pScrollbar->OnClickEvent () );
}

void CDropDown::OnFocusIn ( void )
{
	CWidget::OnFocusIn ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnFocusIn ();
}

void CDropDown::OnFocusOut ( void )
{
	CWidget::OnFocusOut ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnFocusOut ();
}

void CDropDown::OnMouseEnter ( void )
{
	CWidget::OnMouseEnter ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnMouseEnter ();
}

void CDropDown::OnMouseLeave ( void )
{
	CWidget::OnMouseLeave ();
	m_iIndex = -1;
	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnMouseLeave ();
}

bool CDropDown::OnMouseOver ( void )
{
	return ( m_iIndex || CWidget::OnMouseOver () );
}

bool CDropDown::OnKeyDown ( WPARAM wParam )
{
	if ( !CanHaveFocus () )
		return false;

	CScrollbar *pVerScrollbar = m_pEntryList->GetScrollbar ()->GetVerScrollbar ();

	switch ( wParam )
	{
		case VK_RETURN:
		{
			if ( m_bPressed )
			{
				CloseBox ();
				return true;
			}
			break;
		}

		case VK_UP:
		{
			if ( m_iSelected > 0 )
			{
				m_iSelected--;
				pVerScrollbar->ShowItem ( m_iSelected );
			}
			else
			{
				m_iSelected = m_pEntryList->GetSize () - 1;
				pVerScrollbar->Scroll ( m_iSelected );
			}

			SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
			m_pEntryList->SetSelectedEntryByIndex ( m_iSelected, true );

			return true;
		}

		case VK_DOWN:
		{
			if ( m_iSelected + 1 < ( int ) m_pEntryList->GetSize () )
			{
				m_iSelected++;
			}
			else
			{
				m_iSelected = 0;
			}

			SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );

			pVerScrollbar->ShowItem ( m_iSelected );
			m_pEntryList->SetSelectedEntryByIndex ( m_iSelected, true );

			return true;
		}
	}
	return false;
}

bool CDropDown::OnMouseButtonDown ( sMouseEvents e )
{
	if ( !CanHaveFocus () )
		return false;

	// Let the scroll bar handle it first.
	if ( m_pEntryList->GetScrollbar ()->OnMouseButtonDown ( e ) )
		return true;

	if ( e.eButton == sMouseEvents::LeftButton )
	{
		if ( CWidget::ContainsPoint ( e.pos ) )
		{
			// Toggle dropdown
			m_bPressed ? 
				CloseBox () : 
				OpenBox ();

			return true;
		}

		// Mouse click not on main control or in dropdown
		if ( m_bPressed &&
			 !m_rBack.ContainsPoint ( e.pos ) )
		{
			CloseBox ();
		}
	}

	return false;
}

bool CDropDown::OnMouseButtonUp ( sMouseEvents e )
{
	// Let the scroll bar handle it first.
	if ( m_pEntryList->GetScrollbar ()->OnMouseButtonUp ( e ) )
		return true;

	if ( m_bPressed  )
	{
		// Perhaps this click is within the dropdown
		if ( m_rBack.ContainsPoint ( e.pos ) && m_bOpened )
		{
			m_iSelected = m_iIndex;
			m_pEntryList->SetSelectedEntryByIndex ( m_iSelected, true );

			CloseBox ();		
		}

		m_bOpened = true;
		return true;
	}

	return false;
}

bool CDropDown::OnMouseMove ( CVector pos )
{
	if ( !CanHaveFocus () )
		return false;

	// Let the scroll bar handle it first.
	if ( m_pEntryList->GetScrollbar ()->OnMouseMove ( pos ) )
		return true;

	CScrollBarVertical *pScrollbarVer = m_pEntryList->GetScrollbar ()->GetVerScrollbar ();

	m_iIndex = -1;
	if ( m_bPressed )
	{
		SControlRect rText = m_rBack;
		rText.m_pos.m_nX += 4;
		rText.m_size.cx -= ( m_pEntryList->GetScrollbar ()->IsHorScrollbarNeeded () ? pScrollbarVer->GetWidth () + 3 : 0 );
		rText.m_size.cy = TEXTBOX_TEXTSPACE - 2;

		for ( int i = pScrollbarVer->GetTrackPos (); i < pScrollbarVer->GetTrackPos () + pScrollbarVer->GetPageSize (); i++ )
		{
			if ( i < ( int ) m_pEntryList->GetSize () )
			{
				if (i != pScrollbarVer->GetTrackPos())
					rText.m_pos.m_nY += m_pEntryList->GetTextSize().cy;

				SEntryItem *pEntry = m_pEntryList->GetEntryByIndex ( i );

				// Check for a valid 'pEntry' pointer and if text is not NULL
				// and determine which item has been selected
				if ( pEntry &&
					 pEntry->m_sText.c_str () != NULL &&
					 rText.ContainsPoint ( pos ) )
				{
					m_iIndex = i;	
					//return true;
				}
			}
		}
	}

	return false;
}

bool CDropDown::OnMouseWheel ( int zDelta )
{
	if ( !CanHaveFocus () )
		return false;

	if ( m_bPressed )
	{
		UINT uLines;
		SystemParametersInfo ( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );

		m_pEntryList->GetScrollbar ()->OnMouseWheel ( -zDelta * uLines );
		return true;
	}
	else
	{
		if ( zDelta > 0 )
		{
			if ( m_iSelected > 0 )
			{
				m_iSelected--;

				if ( !m_bPressed )
					SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
			}
		}
		else
		{
			if ( m_iSelected + 1 < ( int ) m_pEntryList->GetSize () )
			{
				m_iSelected--;

				if ( !m_bPressed )
					SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
			}
		}
	}

	return false;
}

bool CDropDown::CanHaveFocus ( void )
{
	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	return ( CWidget::CanHaveFocus () ||
			 pScrollbar->CanHaveFocus () );
}

void CDropDown::SetSortedList ( bool bSort )
{
	m_pEntryList->SetSortedList ( bSort );
}

void CDropDown::OpenBox ( void )
{
	_SetFocus ();

	m_bOpened = false;
	m_bPressed = true;
}

void CDropDown::CloseBox ( void )
{
	_ClearFocus ();

	m_bOpened = false;
	m_bPressed = false;
	SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
}

//--------------------------------------------------------------------------------------
void CDropDown::UpdateRects ( void )
{
	CWidget::UpdateRects ();

	m_rBack = m_rBoundingBox;
	m_rBack.m_size.cy = m_uSize * m_pEntryList->GetTextSize().cy;
	m_rBack.m_pos.m_nY += m_rBoundingBox.m_size.cy;

	m_pEntryList->UpdateScrollbars ( m_rBack );
}

//--------------------------------------------------------------------------------------
bool CDropDown::ContainsPoint ( CVector pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( ( m_rBack.ContainsPoint ( pos ) && m_bPressed ) ||
			 m_pEntryList->ContainsRects ( m_rBoundingBox, pos ) );
}