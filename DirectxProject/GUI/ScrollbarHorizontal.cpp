#include "CGUI.h"

#define SCROLLBAR_MINTHUMBSIZE 8
#define SCROLLBAR_ARROWCLICK_START 0.05
#define SCROLLBAR_ARROWCLICK_REPEAT 0.33

#define CMPSTATE( a, b, c, d ) a == b ? c : d 

CScrollBarHorizontal::CScrollBarHorizontal ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_SCROLLBARHORIZONTAL );

	m_bShowThumb = true;
	m_bDrag = false;

	m_nStep = 1;
	m_nPosition = 0;
	m_nPageSize = 1;
	m_nStart = 0;
	m_nEnd = 1;
	nThumbOffset = 0;
	m_Arrow = CLEAR;
}

void CScrollBarHorizontal::Draw ( void )
{
	if ( !m_bVisible )
		return;

	CVector mPos = m_pDialog->GetMouse ()->GetPos ();

	// Check if the arrow button has been held for a while.
	// If so, update the thumb position to simulate repeated
	// scroll.
	if ( m_Arrow == CLICKED_UP &&
		 m_rUpButton.ContainsPoint ( mPos ) &&
		 !m_timer.Running () )
	{
		Scroll ( -m_nStep );
		m_timer.Start ( SCROLLBAR_ARROWCLICK_START );
	}
	else if ( m_Arrow == CLICKED_DOWN &&
			  m_rDownButton.ContainsPoint ( mPos ) &&
			  !m_timer.Running () )
	{
		Scroll ( m_nStep );
		m_timer.Start ( SCROLLBAR_ARROWCLICK_START );
	}

	// Check for click on track
	if ( m_bPressed &&
		 !m_timer.Running () &&
		 m_rBoundingBox.ContainsPoint ( mPos ) )
	{
		if ( m_Arrow == HELD_UP &&
			 m_rThumb.m_pos.m_nX > mPos.m_nX &&
			 m_rBoundingBox.m_pos.m_nX <= mPos.m_nX )
		{
			Scroll ( -( m_nPageSize - 1 ) );
			m_timer.Start ( SCROLLBAR_ARROWCLICK_START );
		}
		else if ( m_Arrow == HELD_DOWN &&
				  m_rThumb.m_pos.m_nX + m_rThumb.m_size.cx <= mPos.m_nX &&
				  ( m_rBoundingBox.m_pos.m_nX + m_rBoundingBox.m_size.cy ) + m_rBoundingBox.m_size.cx > mPos.m_nX)
		{
			Scroll ( m_nPageSize - 1 );
			m_timer.Start ( SCROLLBAR_ARROWCLICK_START );
		}
	}

	CWidget::Draw ();

	SControlColor::SControlState eState = m_bShowThumb && m_bEnabled ?
		SControlColor::STATE_NORMAL :
		SControlColor::STATE_DISABLED;

	D3DCOLOR d3dColorThumb = m_sControlColor.d3dColorBox [ CMPSTATE ( m_Arrow, CLICKED_THUMB, SControlColor::STATE_PRESSED, eState ) ];
	D3DCOLOR d3dColorUp = m_sControlColor.d3dColorBox [ CMPSTATE ( m_Arrow, CLICKED_UP, SControlColor::STATE_PRESSED, eState ) ];
	D3DCOLOR d3dColorDown = m_sControlColor.d3dColorBox [ CMPSTATE ( m_Arrow, CLICKED_DOWN, SControlColor::STATE_PRESSED, eState ) ];

	if ( m_bShowThumb &&
		 !m_bPressed )
	{
		if ( m_rThumb.ContainsPoint ( mPos ) )
			d3dColorThumb = m_sControlColor.d3dColorBox [ m_eState ];
		else if ( m_rUpButton.ContainsPoint ( mPos ) )
			d3dColorUp = m_sControlColor.d3dColorBox [ m_eState ];
		else if ( m_rDownButton.ContainsPoint ( mPos ) )
			d3dColorDown = m_sControlColor.d3dColorBox [ m_eState ];
	}

	// Background track layer
	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline, m_bAntAlias );

	// Right Arrow
	m_pDialog->DrawBox ( m_rUpButton, d3dColorUp, m_sControlColor.d3dColorOutline, m_bAntAlias );

	SControlRect rShape = m_rUpButton;
	rShape.m_pos.m_nX += m_rUpButton.m_size.cx / 2 + 4;
	rShape.m_pos.m_nY += m_rUpButton.m_size.cx / 2;
	rShape.m_size.cx = m_rUpButton.m_size.cx / 2 - 3;
	m_pDialog->DrawTriangle ( rShape, 90.f, m_sControlColor.d3dColorShape, 0 );

	// Left Arrow
	m_pDialog->DrawBox ( m_rDownButton, d3dColorDown, m_sControlColor.d3dColorOutline, m_bAntAlias );

	rShape.m_pos.m_nX += m_rDownButton.m_pos.m_nX + m_rDownButton.m_size.cx / 2 - 4;
	rShape.m_pos.m_nY += m_rDownButton.m_pos.m_nY + m_rDownButton.m_size.cx / 2;
	m_pDialog->DrawTriangle ( SControlRect ( rShape.m_pos, rShape.m_size ), 270.f, m_sControlColor.d3dColorShape, 0 );

	// Thumb button
	m_pDialog->DrawBox ( m_rThumb, d3dColorThumb, m_sControlColor.d3dColorOutline, m_bAntAlias );
}


//--------------------------------------------------------------------------------------
void CScrollBarHorizontal::OnClickLeave ( void )
{
	m_bPressed = m_bDrag = false;
	m_Arrow = CLEAR;
}

//--------------------------------------------------------------------------------------
bool CScrollBarHorizontal::OnClickEvent ( void )
{
	return ( m_bPressed || m_bDrag || m_Arrow != CLEAR );
}

bool CScrollBarHorizontal::OnMouseButtonDown ( sMouseEvents e )
{
	if ( e.eButton == sMouseEvents::LeftButton )
	{
		// Check for click on up button
		if ( m_rUpButton.ContainsPoint ( e.pos ) )
		{
			if ( m_nPosition > m_nStart )
				Scroll ( -m_nStep );

			UpdateThumbRect ();
			m_timer.Start ( SCROLLBAR_ARROWCLICK_REPEAT );

			m_Arrow = CLICKED_UP;
			m_bPressed = true;

			_SetFocus ();

			return true;
		}

		// Check for click on down button
		if ( m_rDownButton.ContainsPoint ( e.pos ) )
		{
			if ( m_nPosition + m_nPageSize <= m_nEnd )
				Scroll ( m_nStep );

			UpdateThumbRect ();
			m_timer.Start ( SCROLLBAR_ARROWCLICK_REPEAT );

			m_Arrow = CLICKED_DOWN;
			m_bPressed = true;

			_SetFocus ();

			return true;
		}

		// Check for click on thumb
		if ( m_rThumb.ContainsPoint ( e.pos ) )
		{
			nThumbOffset = e.pos.m_nX - m_rThumb.m_pos.m_nX;
			m_Arrow = CLICKED_THUMB;
			m_bDrag = true;
			m_bPressed = true;

			_SetFocus ();

			return true;
		}

		// Check for click on track
		if ( m_rBoundingBox.ContainsPoint ( e.pos ) )
		{
			if ( m_rThumb.m_pos.m_nX > e.pos.m_nX &&
				 m_rBoundingBox.m_pos.m_nX <= e.pos.m_nX)
			{
				Scroll ( -( m_nPageSize - 1 ) );

				_SetFocus ();

				m_timer.Start ( 0.5 );
				m_bPressed = true;
				m_Arrow = HELD_UP;

				return true;
			}
			else if ( m_rThumb.m_pos.m_nX + m_rThumb.m_size.cx <= e.pos.m_nX &&
					  ( m_rBoundingBox.m_pos.m_nX + m_rBoundingBox.m_size.cy ) + m_rBoundingBox.m_size.cx > e.pos.m_nX )
			{
				Scroll ( m_nPageSize - 1 );

				_SetFocus ();

				m_timer.Start ( 0.5 );
				m_bPressed = true;
				m_Arrow = HELD_DOWN;

				return true;
			}
		}
	}

	return false;
}

bool CScrollBarHorizontal::OnMouseButtonUp ( sMouseEvents e )
{
	if ( m_bPressed )
	{
		m_bPressed = false;
		m_bDrag = false;
		UpdateThumbRect ();
		m_Arrow = CLEAR;

		_ClearFocus ();

		return true;
	}

	return false;
}

bool CScrollBarHorizontal::OnMouseMove ( CVector pos )
{
	if ( m_bDrag )
	{
		m_rThumb.m_pos.m_nX = pos.m_nX - nThumbOffset;

		if ( m_rThumb.m_pos.m_nX < ( m_rBoundingBox.m_pos.m_nX + m_rBoundingBox.m_size.cy ) )
		{
			m_rThumb.m_pos.m_nX = m_rBoundingBox.m_pos.m_nX + m_rBoundingBox.m_size.cy;
		}
		else if ( m_rThumb.m_pos.m_nX + m_rThumb.m_size.cx >= m_rBoundingBox.m_pos.m_nX + ( m_rBoundingBox.m_size.cx - m_rBoundingBox.m_size.cy ) )
		{
			m_rThumb.m_pos.m_nX = m_rBoundingBox.m_pos.m_nX + (m_rBoundingBox.m_size.cx - m_rBoundingBox.m_size.cy - m_rThumb.m_size.cx);
		}

		// Compute first item index based on thumb position
		int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize + 1;  // Largest possible index for first item
		int nMaxThumb = ( m_rBoundingBox.m_size.cx - ( m_rBoundingBox.m_size.cy * 2 ) ) - m_rThumb.m_size.cx;  // Largest possible thumb position from the top

		m_nPosition = m_nStart +
			( m_rThumb.m_pos.m_nX - ( m_rBoundingBox.m_pos.m_nX + m_rBoundingBox.m_size.cy ) +
			  nMaxThumb / ( nMaxFirstItem * 2 ) ) * // Shift by half a row to avoid last row covered by only one pixel
			nMaxFirstItem / nMaxThumb;

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------
void CScrollBarHorizontal::UpdateRects ( void )
{
	CWidget::UpdateRects ();

	// Make the buttons square
	m_rUpButton = m_rBoundingBox;
	m_rUpButton.m_size.cx = m_rBoundingBox.m_size.cy;

	m_rDownButton = m_rBoundingBox;
	m_rDownButton.m_pos.m_nX += m_rBoundingBox.m_size.cx - m_rBoundingBox.m_size.cy;
	m_rDownButton.m_size.cx = m_rBoundingBox.m_size.cy;

	CScrollbar::SetBackPos ( m_rBoundingBox.m_pos.m_nX + m_rBoundingBox.m_size.cy );
	CScrollbar::SetBackSize ( m_rBoundingBox.m_size.cx - ( m_rBoundingBox.m_size.cy * 2 ) );

	CScrollbar::UpdateThumbRect ();

	m_rThumb = m_rBoundingBox;
	m_rThumb.m_pos.m_nX = CScrollbar::GetThumbPos();
	m_rThumb.m_size.cx = CScrollbar::GetThumbSize ();
}

//--------------------------------------------------------------------------------------
bool CScrollBarHorizontal::ContainsPoint ( CVector pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( m_rBoundingBox.ContainsPoint ( pos ) ||
			 m_rUpButton.ContainsPoint ( pos ) ||
			 m_rDownButton.ContainsPoint ( pos ) ||
			 m_rThumb.ContainsPoint ( pos ) );
}
