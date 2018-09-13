#include "CFigure.h"

CPictureBox::CPictureBox ( CDialog *pDialog )
{
	SetControl ( pDialog, EControlType::TYPE_IMAGE );
	m_fRotImage = 0.f;
}

CPictureBox::~CPictureBox ( void )
{
	m_pDialog->RemoveTexture ( m_pTexture );
}

void CPictureBox::Draw ( void )
{
	if ( !m_bVisible )
		return;

	CWidget::Draw ();

	m_pTexture->Draw ( m_rBoundingBox.m_pos.m_nX, m_rBoundingBox.m_pos.m_nY, m_rBoundingBox.m_size.cx * 1.5, m_rBoundingBox.m_size.cy * 1.9, 0.f, m_sControlColor.d3dColorBox [ m_eState ] );
}

bool CPictureBox::OnKeyDown ( WPARAM wParam )
{
	if ( !CanHaveFocus () )
		return false;

	if ( wParam == VK_SPACE )
	{
		m_bPressed = true;
		return  true;
	}

	return false;
}

bool CPictureBox::OnKeyUp ( WPARAM wParam )
{
	if ( wParam == VK_SPACE )
	{
		if ( m_bPressed )
		{
			m_bPressed = false;
			SendEvent ( EVENT_CONTROL_CLICKED, true );
			return true;
		}
	}

	return false;
}

bool CPictureBox::OnMouseButtonDown ( sMouseEvents e )
{
	if ( !CanHaveFocus () )
		return false;

	if ( e.eButton == sMouseEvents::LeftButton )
	{
		if ( m_rBoundingBox.ContainsPoint ( e.pos ) )
		{
			// Pressed while inside the control
			m_bPressed = true;

			_SetFocus ();

			return true;
		}
	}

	return false;
}

bool CPictureBox::OnMouseButtonUp ( sMouseEvents e )
{
	if ( m_bPressed )
	{
		m_bPressed = false;

		_ClearFocus ();

		// Button click
		if ( m_rBoundingBox.ContainsPoint ( e.pos ) )
			SendEvent ( EVENT_CONTROL_CLICKED, true );

		return true;
	}

	return false;
}

void CPictureBox::SetTexture ( const SIMPLEGUI_CHAR *szPath )
{
	if ( !m_pDialog || m_pTexture )
		return;

	m_pDialog->LoadTexture ( szPath, &m_pTexture );
}

void CPictureBox::SetTexture ( LPCVOID pSrc, UINT uSrcSize )
{
	if ( !m_pDialog || m_pTexture )
		return;

	m_pDialog->LoadTexture ( pSrc, uSrcSize, &m_pTexture );
}

void CPictureBox::RotateImage ( float fRot )
{
	m_fRotImage = fRot;
}
