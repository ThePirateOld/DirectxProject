﻿#include "CGUI.h"

#define MinState( a, b, c, d ) a < b ? c : d 

//-----------------------------------------------------------------------------
// CUniBuffer class for the edit control
//-----------------------------------------------------------------------------

// Static member initialization
HINSTANCE               CUniBuffer::s_hDll = NULL;
HRESULT ( WINAPI*CUniBuffer::_ScriptApplyDigitSubstitution )( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*,
															  SCRIPT_STATE* ) = Dummy_ScriptApplyDigitSubstitution;
HRESULT ( WINAPI*CUniBuffer::_ScriptStringAnalyse )( HDC, const void*, int, int, int, DWORD, int, SCRIPT_CONTROL*,
													 SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*,
													 SCRIPT_STRING_ANALYSIS* ) = Dummy_ScriptStringAnalyse;
HRESULT ( WINAPI*CUniBuffer::_ScriptStringCPtoX )( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) = Dummy_ScriptStringCPtoX;
HRESULT ( WINAPI*CUniBuffer::_ScriptStringXtoCP )( SCRIPT_STRING_ANALYSIS, int, int*, int* ) = Dummy_ScriptStringXtoCP;
HRESULT ( WINAPI*CUniBuffer::_ScriptStringFree )( SCRIPT_STRING_ANALYSIS* ) = Dummy_ScriptStringFree;
const SCRIPT_LOGATTR*   ( WINAPI*CUniBuffer::_ScriptString_pLogAttr )( SCRIPT_STRING_ANALYSIS ) =
Dummy_ScriptString_pLogAttr;
const int*              ( WINAPI*CUniBuffer::_ScriptString_pcOutChars )( SCRIPT_STRING_ANALYSIS ) =
Dummy_ScriptString_pcOutChars;


//--------------------------------------------------------------------------------------
void CUniBuffer::Initialize ()
{
	if ( s_hDll ) // Only need to do once
		return;

	s_hDll = LoadLibrary ( UNISCRIBE_DLLNAME );
	if ( s_hDll )
	{
		FARPROC Temp;
		GETPROCADDRESS ( s_hDll, ScriptApplyDigitSubstitution, Temp );
		GETPROCADDRESS ( s_hDll, ScriptStringAnalyse, Temp );
		GETPROCADDRESS ( s_hDll, ScriptStringCPtoX, Temp );
		GETPROCADDRESS ( s_hDll, ScriptStringXtoCP, Temp );
		GETPROCADDRESS ( s_hDll, ScriptStringFree, Temp );
		GETPROCADDRESS ( s_hDll, ScriptString_pLogAttr, Temp );
		GETPROCADDRESS ( s_hDll, ScriptString_pcOutChars, Temp );
	}
}


//--------------------------------------------------------------------------------------
void CUniBuffer::Uninitialize ()
{
	if ( s_hDll )
	{
		PLACEHOLDERPROC ( ScriptApplyDigitSubstitution );
		PLACEHOLDERPROC ( ScriptStringAnalyse );
		PLACEHOLDERPROC ( ScriptStringCPtoX );
		PLACEHOLDERPROC ( ScriptStringXtoCP );
		PLACEHOLDERPROC ( ScriptStringFree );
		PLACEHOLDERPROC ( ScriptString_pLogAttr );
		PLACEHOLDERPROC ( ScriptString_pcOutChars );

		FreeLibrary ( s_hDll );
		s_hDll = NULL;
	}
}

//--------------------------------------------------------------------------------------
bool CUniBuffer::SetBufferSize ( int nNewSize )
{
	// If the current m_size is already the maximum allowed,
	// we can't possibly allocate more.
	if ( m_nBufferSize == 0xFFFF )
		return false;

	int nAllocateSize = ( nNewSize == -1 || nNewSize < m_nBufferSize * 2 ) ? ( m_nBufferSize ? m_nBufferSize *
																			   2 : 256 ) : nNewSize * 2;

	// Cap the buffer m_size at the maximum allowed.
	if ( nAllocateSize > 0xFFFF )
	{
		nAllocateSize = 0xFFFF;
	}

	WCHAR* pTempBuffer = new WCHAR [ nAllocateSize ];
	if ( !pTempBuffer )
		return false;

	ZeroMemory ( pTempBuffer, sizeof ( WCHAR ) * nAllocateSize );

	if ( m_pwszBuffer )
	{
		CopyMemory ( pTempBuffer, m_pwszBuffer, m_nBufferSize * sizeof ( WCHAR ) );
		delete [] m_pwszBuffer;
	}

	m_pwszBuffer = pTempBuffer;
	m_nBufferSize = nAllocateSize;
	return true;
}


//--------------------------------------------------------------------------------------
// Uniscribe -- Analyse() analyses the string in the buffer
//--------------------------------------------------------------------------------------
HRESULT CUniBuffer::Analyse ()
{
	if ( m_Analysis )
		_ScriptStringFree ( &m_Analysis );

	SCRIPT_CONTROL ScriptControl; // For uniscribe
	SCRIPT_STATE ScriptState;   // For uniscribe
	ZeroMemory ( &ScriptControl, sizeof ( ScriptControl ) );
	ZeroMemory ( &ScriptState, sizeof ( ScriptState ) );
	_ScriptApplyDigitSubstitution ( NULL, &ScriptControl, &ScriptState );

	if ( !m_pFontNode )
		return E_FAIL;

	size_t size = wcslen (m_pwszBuffer);
	HDC hDC = m_pFontNode->GetDC ();
	HRESULT hr = _ScriptStringAnalyse (hDC,
		m_pwszBuffer,
		size + 1,  // NULL is also analyzed.
		size * 3 / 2 + 16,
		-1,
		SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
		0,
		&ScriptControl,
		&ScriptState,
		NULL,
		NULL,
		NULL,
		&m_Analysis);
	if ( SUCCEEDED ( hr ) )
	{
		m_bAnalyseRequired = false;  // Analysis is up-to-date
	}
	return hr;
}


//--------------------------------------------------------------------------------------
CUniBuffer::CUniBuffer ( int nInitialSize )
{
	CUniBuffer::Initialize ();  // ensure static vars are properly init'ed first

	m_nBufferSize = 0;
	m_pwszBuffer = NULL;
	m_bAnalyseRequired = true;
	m_Analysis = NULL;
	m_pFontNode = NULL;

	if ( nInitialSize > 0 )
	{
		SetBufferSize ( nInitialSize );
	}
}


//--------------------------------------------------------------------------------------
CUniBuffer::~CUniBuffer ()
{
	delete [] m_pwszBuffer;
	if ( m_Analysis )
	{
		_ScriptStringFree ( &m_Analysis );
	}
}


//--------------------------------------------------------------------------------------
WCHAR& CUniBuffer::operator[]( int n )  // No param checking
{
	// This version of operator[] is called only
	// if we are asking for write access, so
	// re-analysis is required.
	m_bAnalyseRequired = true;
	return m_pwszBuffer [ n ];
}


//--------------------------------------------------------------------------------------
void CUniBuffer::Clear ()
{
	*m_pwszBuffer = L'\0';
	m_bAnalyseRequired = true;
}


//--------------------------------------------------------------------------------------
// Inserts the char at specified index.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
bool CUniBuffer::InsertChar ( int nIndex, WCHAR wChar )
{
	assert ( nIndex >= 0 );

	if ( nIndex < 0 || nIndex > lstrlenW ( m_pwszBuffer ) )
		return false;  // invalid index

					   // Check for maximum length allowed
	if ( GetTextSize () + 1 >= DXUT_MAX_EDITBOXLENGTH )
		return false;

	if ( wcslen ( m_pwszBuffer ) + 1 >= m_nBufferSize )
	{
		if ( !SetBufferSize ( -1 ) )
			return false;  // out of memory
	}

	assert ( m_nBufferSize >= 2 );

	// Shift the characters after the index, start by copying the null terminator
	WCHAR* dest = m_pwszBuffer + wcslen ( m_pwszBuffer ) + 1;
	WCHAR* stop = m_pwszBuffer + nIndex;
	WCHAR* src = dest - 1;

	while ( dest > stop )
	{
		*dest-- = *src--;
	}

	// Set new character
	m_pwszBuffer [ nIndex ] = wChar;
	m_bAnalyseRequired = true;

	return true;
}


//--------------------------------------------------------------------------------------
// Removes the char at specified index.
// If nIndex == -1, remove the last char.
//--------------------------------------------------------------------------------------
bool CUniBuffer::RemoveChar (int nIndex)
{
	size_t size = wcslen (m_pwszBuffer);

	if (!size ||
		nIndex < 0 ||
		nIndex >= size)
		return false;  // Invalid index

	MoveMemory (m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof (WCHAR) * (size - nIndex));
	m_bAnalyseRequired = true;
	return true;
}


//--------------------------------------------------------------------------------------
// Inserts the first nCount characters of the string pStr at specified index.
// If nCount == -1, the entire string is inserted.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
bool CUniBuffer::InsertString (int nIndex, const WCHAR* pStr, int nCount)
{
	if (nIndex < 0)
		return false;

	size_t size = wcslen (m_pwszBuffer);
	if (nIndex > size)
		return false;  // invalid index

	if (-1 == nCount)
	{
		nCount = wcslen (pStr);
	}

	// Check for maximum length allowed
	if (GetTextSize () + nCount >= DXUT_MAX_EDITBOXLENGTH)
		return false;

	if (size + nCount >= m_nBufferSize)
	{
		if (!SetBufferSize (size + nCount + 1))
			return false;  // out of memory
	}

	MoveMemory (m_pwszBuffer + nIndex + nCount, m_pwszBuffer + nIndex, sizeof (WCHAR) * (size - nIndex + 1));
	CopyMemory (m_pwszBuffer + nIndex, pStr, nCount * sizeof (WCHAR));
	m_bAnalyseRequired = true;

	return true;
}


//--------------------------------------------------------------------------------------
bool CUniBuffer::SetText ( WCHAR* wszText )
{
	assert ( wszText != NULL );

	int nRequired = int ( wcslen ( wszText ) + 1 );

	// Check for maximum length allowed
	if ( nRequired >= DXUT_MAX_EDITBOXLENGTH )
		return false;

	while ( GetBufferSize () < nRequired )
	{
		if ( !SetBufferSize ( -1 ) )
			break;
	}

	// Check again in case out of memory occurred inside while loop.
	if ( GetBufferSize () >= nRequired )
	{
		wcscpy_s ( m_pwszBuffer, GetBufferSize (), wszText );
		m_bAnalyseRequired = true;
		return true;
	}
	else
		return false;
}


//--------------------------------------------------------------------------------------
HRESULT CUniBuffer::CPtoX ( int nCP, BOOL bTrail, int* pX )
{
	assert ( pX );
	*pX = 0;  // Default

	HRESULT hr = S_OK;
	if ( m_bAnalyseRequired )
	{
		hr = Analyse ();
	}

	if ( SUCCEEDED ( hr ) )
	{
		hr = _ScriptStringCPtoX ( m_Analysis, nCP, bTrail, pX );
	}
	return hr;
}


//--------------------------------------------------------------------------------------
HRESULT CUniBuffer::XtoCP ( int nX, int* pCP, int* pnTrail )
{
	assert ( pCP && pnTrail );

	*pCP = 0; 
	*pnTrail = FALSE;  // Default

	HRESULT hr = S_OK;
	if ( m_bAnalyseRequired )
	{
		hr = Analyse ();
	}

	if ( SUCCEEDED ( hr ) )
	{
		hr = _ScriptStringXtoCP ( m_Analysis, nX, pCP, pnTrail );
	}

	size_t size = wcslen (m_pwszBuffer);

	// If the coordinate falls outside the text region, we
	// can get character positions that don't exist.  We must
	// filter them here and convert them to those that do exist.
	if ( *pCP == -1 && *pnTrail == TRUE )
	{
		*pCP = 0; *pnTrail = FALSE;
	}
	else if ( *pCP > size && *pnTrail == FALSE )
	{
		*pCP = size; 
		*pnTrail = TRUE;
	}

	return hr;
}


//--------------------------------------------------------------------------------------
void CUniBuffer::GetPriorItemPos ( int nCP, int* pPrior )
{
	*pPrior = nCP;  // Default is the char itself

	if ( m_bAnalyseRequired )
	{
		if ( FAILED ( Analyse () ) )
			return;
	}

	const SCRIPT_LOGATTR* pLogAttr = _ScriptString_pLogAttr ( m_Analysis );
	if ( !pLogAttr )
		return;

	if ( !_ScriptString_pcOutChars ( m_Analysis ) )
		return;

	int nInitial = *_ScriptString_pcOutChars ( m_Analysis );
	if ( nCP - 1 < nInitial )
	{
		nInitial = nCP - 1;
	}
	
	for ( int i = nInitial; i > 0; --i )
	{
		if ( pLogAttr [ i ].fWordStop ||       // Either the fWordStop flag is set
			 ( !pLogAttr [ i ].fWhiteSpace &&  // Or the previous char is whitespace but this isn't.
			 pLogAttr [ i - 1 ].fWhiteSpace ) )
		{
			*pPrior = i;
			return;
		}
	}

	// We have reached index 0.  0 is always a break point, so simply return it.
	*pPrior = 0;
}


//--------------------------------------------------------------------------------------
void CUniBuffer::GetNextItemPos ( int nCP, int* pPrior )
{
	*pPrior = nCP;  // Default is the char itself

	HRESULT hr = S_OK;
	if ( m_bAnalyseRequired )
		hr = Analyse ();

	if ( FAILED ( hr ) )
		return;

	const SCRIPT_LOGATTR* pLogAttr = _ScriptString_pLogAttr ( m_Analysis );
	if ( !pLogAttr )
		return;

	if ( !_ScriptString_pcOutChars ( m_Analysis ) )
		return;

	int nInitial = *_ScriptString_pcOutChars ( m_Analysis );
	if ( nCP + 1 < nInitial )
	{
		nInitial = nCP + 1;
	}

	int i = nInitial;
	int limit = *_ScriptString_pcOutChars ( m_Analysis );
	while ( limit > 0 && i < limit - 1 )
	{
		if ( pLogAttr [ i ].fWordStop )      // Either the fWordStop flag is set
		{
			*pPrior = i;
			return;
		}
		else if ( pLogAttr [ i ].fWhiteSpace &&  // Or this whitespace but the next char isn't.
				  !pLogAttr [ i + 1 ].fWhiteSpace )
		{
			*pPrior = i + 1;  // The next char is a word stop
			return;
		}

		++i;
		limit = *_ScriptString_pcOutChars ( m_Analysis );
	}

	// We have reached the end. It's always a word stop, so simply return it.
	*pPrior = *_ScriptString_pcOutChars ( m_Analysis ) - 1;
}

//--------------------------------------------------------------------------------------
CEditBox::CEditBox ( CDialog *pDialog )
{
	SetControl ( pDialog, EControlType::TYPE_EDITBOX );

	m_bCaretOn = m_bInsertMode = true;
	m_nFirstVisible = m_nCaret = m_nSelStart = 0;

	m_timer.Start ( 0.8 );
	m_sControlColor.d3dColorBox [ SControlColor::STATE_PRESSED ] = D3DCOLOR_RGBA ( 200, 200, 200, 255 );
}

//--------------------------------------------------------------------------------------
CEditBox::~CEditBox ( void )
{
	ClearText ();
}

//--------------------------------------------------------------------------------------
const TCHAR* CEditBox::GetText ( void )
{
	
#ifdef UNICODE 
	wstring strResult = m_Buffer.GetBuffer ();
#else
	wstring wstr = m_Buffer.GetBuffer ();
	string strResult = string (wstr.begin (), wstr.end ());
#endif // 

	return strResult.c_str();
}

//--------------------------------------------------------------------------------------
int CEditBox::GetTextLength ( void )
{
	return m_Buffer.GetTextSize ();
}

//--------------------------------------------------------------------------------------
// PlaceCaret: Set the caret to a character position, and adjust the scrolling if
//             necessary.
//--------------------------------------------------------------------------------------
void CEditBox::PlaceCaret ( int nCP )
{
	assert ( nCP >= 0 && nCP <= m_Buffer.GetTextSize () );
	m_nCaret = nCP;

	// Obtain the X offset of the character.
	int nX1st, nX, nX2;
	m_Buffer.CPtoX ( m_nFirstVisible, FALSE, &nX1st );  // 1st visible char
	m_Buffer.CPtoX ( nCP, FALSE, &nX );  // LEAD
	
	// If nCP is the NULL terminator, get the leading edge instead of trailing.
	if ( nCP == m_Buffer.GetTextSize () )
	{
		nX2 = nX;
	}
	else
	{
		m_Buffer.CPtoX ( nCP, TRUE, &nX2 );  // TRAIL
	}

	// If the left edge of the char is smaller than the left edge of the 1st visible char,
	// we need to scroll left until this char is visible.
	if ( nX < nX1st )
	{
		// Simply make the first visible character the char at the new caret position.
		m_nFirstVisible = nCP;
	}
	else
	{	// If the right of the character is bigger than the offset of the control's
		// right edge, we need to scroll right to this character.
		if ( nX2 > nX1st + m_rText.m_size.cx )
		{
			// Compute the X of the new left-most pixel
			int nXNewLeft = nX2 - m_rText.m_size.cx;

			// Compute the char position of this character
			int nCPNew1st, nNewTrail;
			m_Buffer.XtoCP ( nXNewLeft, &nCPNew1st, &nNewTrail );

			// If this coordinate is not on a character border,
			// start from the next character so that the caret
			// position does not fall outside the text rectangle.
			int nXNew1st;
			m_Buffer.CPtoX ( nCPNew1st, FALSE, &nXNew1st );
			if ( nXNew1st < nXNewLeft )
			{
				++nCPNew1st;
			}

			m_nFirstVisible = nCPNew1st;
		}
	}
}

void CEditBox::CopyToClipboard ()
{
	// Copy the selection text to the clipboard
	if ( m_nCaret != m_nSelStart && OpenClipboard ( NULL ) )
	{
		EmptyClipboard ();

		HGLOBAL hBlock = GlobalAlloc ( GMEM_MOVEABLE, sizeof ( WCHAR ) * ( m_Buffer.GetTextSize () + 1 ) );
		if ( hBlock )
		{
			WCHAR* pwszText = ( WCHAR* ) GlobalLock ( hBlock );
			if ( pwszText )
			{
				int nFirst = __min ( m_nCaret, m_nSelStart );
				int nLast = __max ( m_nCaret, m_nSelStart );

				if ( nLast - nFirst > 0 )
				{
					CopyMemory ( pwszText, m_Buffer.GetBuffer () + nFirst, ( nLast - nFirst ) * sizeof ( WCHAR ) );
				}

				pwszText [ nLast - nFirst ] = L'\0';  // Terminate it
				GlobalUnlock ( hBlock );
			}
			SetClipboardData ( CF_UNICODETEXT, hBlock );
		}
		CloseClipboard ();

		// We must not free the object until CloseClipboard is called.
		if ( hBlock )
		{
			GlobalFree ( hBlock );
		}
	}
}

void CEditBox::PasteFromClipboard ()
{
	DeleteSelectionText ();

	if ( OpenClipboard ( NULL ) )
	{
		HANDLE handle = GetClipboardData ( CF_UNICODETEXT );
		if ( handle )
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			WCHAR* pwszText = ( WCHAR* ) GlobalLock ( handle );
			if ( pwszText )
			{
				// Copy all characters up to null.
				if ( m_Buffer.InsertString ( m_nCaret, pwszText ) )
				{
					PlaceCaret ( m_nCaret + wcslen ( pwszText ) );
				}

				m_nSelStart = m_nCaret;
				GlobalUnlock ( handle );
			}
		}
		CloseClipboard ();
	}
}

//--------------------------------------------------------------------------------------
void CEditBox::ClearText ()
{
	PlaceCaret ( 0 );
	m_Buffer.Clear ();
	m_nFirstVisible = 0;
	m_nSelStart = 0;
}


//--------------------------------------------------------------------------------------
void CEditBox::SetText ( SIMPLEGUI_STRING sString, bool bSelected )
{
	if ( m_pFont )
		m_pFont->RemoveColorTableFromString ( sString );

#ifdef UNICODE 
wstring wstr = sString;
#else
wstring wstr = wstring (sString.begin (), sString.end ());
#endif // 

	m_Buffer.SetText ((WCHAR*)wstr.c_str ());
	m_nFirstVisible = 0;

	// Move the caret to the end of the text
	PlaceCaret ( m_Buffer.GetTextSize () );
	m_nSelStart = bSelected ? 0 : m_nCaret;
}

//--------------------------------------------------------------------------------------
HRESULT CEditBox::GetTextCopy ( __out_ecount ( bufferCount ) WCHAR* strDest,
									UINT bufferCount )
{
	assert ( strDest );
	wcscpy_s ( strDest, bufferCount, m_Buffer.GetBuffer () );

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CEditBox::DeleteSelectionText ()
{
	int nFirst = __min ( m_nCaret, m_nSelStart );
	int nLast = __max ( m_nCaret, m_nSelStart );

	// Update caret and selection
	PlaceCaret ( nFirst );
	m_nSelStart = m_nCaret;

	// Remove the characters
	for ( int i = nFirst; i < nLast; ++i )
		m_Buffer.RemoveChar ( nFirst );
}

void CEditBox::ResetCaretBlink ( void )
{
	m_bCaretOn = true;
	m_timer.Start ( 0.6f );
}

void CEditBox::Draw ()
{
	CWidget::Draw ();

	if ( m_bHasFocus )
	{
		m_eState = SControlColor::STATE_PRESSED;
	}

	// Left and right X cordinates of the selection region
	int nSelStartX = 0, nCaretX = 0;

	// Compute the X coordinates of the first visible character.
	int nXFirst;
	m_Buffer.CPtoX ( m_nFirstVisible, FALSE, &nXFirst );

	// Compute the X coordinates of the selection rectangle
	m_Buffer.CPtoX ( m_nCaret, FALSE, &nCaretX );
	if ( m_nCaret != m_nSelStart )
	{
		m_Buffer.CPtoX ( m_nSelStart, FALSE, &nSelStartX );
	}
	else
	{
		nSelStartX = nCaretX;
	}

	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBox [ m_eState ], m_sControlColor.d3dColorOutline );

	m_Buffer.SetFontNode ( m_pFont );
	PlaceCaret ( m_nCaret );  // Call PlaceCaret now that we have the font info (node),

	wstring str = m_Buffer.GetBuffer () + m_nFirstVisible;

	// Render the selection rectangle
	if ( m_nCaret != m_nSelStart && m_bHasFocus )
	{
		int nSelLeftX = nCaretX;
		int	nSelRightX = nSelStartX;

		// Swap if left is bigger than right
		if ( nSelLeftX > nSelRightX )
		{
			int nTemp = nSelLeftX;
			nSelLeftX = nSelRightX;
			nSelRightX = nTemp;
		}

		SControlRect rSelection ( nSelLeftX + m_rText.m_pos.m_nX - nXFirst, m_rText.m_pos.m_nY,
								  nSelRightX - nSelLeftX, m_rText.m_size.cy );

		m_pDialog->DrawBox ( rSelection, D3DCOLOR_RGBA ( 40, 40, 40, 255 ), D3DCOLOR_RGBA ( 40, 40, 40, 255 ), false );

		int nMinSelStart = m_nSelStart - m_nFirstVisible;
		int nMinCaret = m_nCaret - m_nFirstVisible;

		wstring strColBlk = L"{00000000}";

		wstring strColWht = L"{FFFFFFFF}";

		SKeyColor sBeginKey = m_nSelStart < m_nCaret ? SKeyColor ( max ( nMinSelStart, 0 ), strColWht ) :
			SKeyColor ( min ( nMinSelStart, str.size () ), strColBlk );

		str.insert ( sBeginKey.nPos, sBeginKey.strColor );

		SKeyColor sEndKey = m_nSelStart < m_nCaret ? SKeyColor ( min ( nMinCaret + 10, str.size () ), strColBlk ) :
			SKeyColor ( nMinCaret, strColWht );

		str.insert ( sEndKey.nPos, sEndKey.strColor );
	}

#ifdef UNICODE 
	wstring wstr = str;
#else
	string wstr = string (str.begin (), str.end ());
#endif // 

	SIZE size;
	m_pFont->GetTextExtent ( _UI ( "Y" ), &size );
	m_pDialog->DrawFont ( SControlRect ( m_rText.m_pos.m_nX, m_rText.m_pos.m_nY + m_rText.m_size.cy / 2 - ( size.cy / 2 ) ),
						  m_sControlColor.d3dColorSelectedFont, wstr.c_str (), D3DFONT_COLORTABLE, m_pFont );

	if ( !m_timer.Running () )
	{
		m_bCaretOn = !m_bCaretOn;
		m_timer.Start ( 0.6f );
	}

	// Render the caret if this control has the focus
	if ( m_bHasFocus && m_bCaretOn )
	{
		int nX = m_rText.m_pos.m_nX - nXFirst + nCaretX;

		CD3DRender *pRender = m_pDialog->GetRenderer ();

		// If we are in overwrite mode, adjust the caret rectangle
		// to fill the entire character.
		str = m_Buffer.GetBuffer ();
		if ( !m_bInsertMode &&
			 m_nCaret == m_nSelStart &&
			 m_nCaret < wstr.size () )
		{
			TCHAR szStr [ 2 ] = { wstr[ m_nCaret ] , 0 };
			GetTextExtentPoint32 ( m_pFont->GetDC (), szStr, 1, &size );
			//m_pFont->GetTextExtent ( szStr, &size );
			pRender->D3DBox ( nX, m_rText.m_pos.m_nY, size.cx - 1, m_rText.m_size.cy, 0, D3DCOLOR_RGBA ( 20, 20, 20, 200 ), 0, false );
		}
		else
		{
			pRender->D3DLine ( nX , m_rText.m_pos.m_nY, nX , m_rText.m_pos.m_nY + m_rText.m_size.cy, 0xFF000000, true );
		}
	}
}

void CEditBox::OnClickLeave ( void )
{
	CWidget::OnClickLeave ();
	m_bMouseDrag  = false;
}

bool CEditBox::OnMouseButtonDown ( sMouseEvents e )
{
	if ( !CanHaveFocus () )
		return false;

	if ( e.eButton == sMouseEvents::LeftButton )
	{
		if ( m_pDialog->GetMouse ()->GetLeftButton () == 2 )
		{
			m_nSelStart = CTextUtils::getWordStartIdx ( m_Buffer.GetBuffer (),
				( m_nCaret == m_Buffer.GetBufferSize () ) ? m_nCaret : m_nCaret + 1 );

			PlaceCaret ( CTextUtils::getNextWordStartIdx ( m_Buffer.GetBuffer (), m_nCaret ));
		}
		else
		{
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CPtoX ( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
			if ( SUCCEEDED ( m_Buffer.XtoCP ( e.pos.m_nX - m_rText.m_pos.m_nX + nX1st, &nCP, &nTrail ) ) )
			{
				// Cap at the NULL character.
				if ( nTrail && nCP < m_Buffer.GetTextSize () )
				{
					PlaceCaret ( nCP + 1 );
				}
				else
				{
					PlaceCaret ( nCP );
				}

				m_nSelStart = m_nCaret;
			}
		}

		if ( m_rBoundingBox.ContainsPoint ( e.pos ) )
		{
			// Pressed while inside the control
			m_bPressed = m_bMouseDrag = true;

			_SetFocus ();
			return true;
		}
	}

	return false;
}

bool CEditBox::OnMouseButtonUp ( sMouseEvents e )
{
	m_bMouseDrag = false;
	return false;
}

bool CEditBox::OnMouseMove ( Pos pos )
{
	if ( m_bMouseDrag )
	{
		// Determine the character corresponding to the coordinates.
		int nCP, nTrail, nX1st;
		m_Buffer.CPtoX ( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
		if ( SUCCEEDED ( m_Buffer.XtoCP ( pos.m_nX - m_rText.m_pos.m_nX + nX1st, &nCP, &nTrail ) ) )
		{
			// Cap at the NULL character.
			if ( nTrail && nCP < m_Buffer.GetTextSize () )
			{
				PlaceCaret ( nCP + 1 );
			}
			else
			{
				PlaceCaret ( nCP );
			}
			return true;
		}
	}

	return false;
}

bool CEditBox::OnKeyDown ( WPARAM wParam )
{
	if ( !CanHaveFocus () )
		return false;

	bool bHandled = false;

	switch ( wParam )
	{
		case VK_TAB:
			// We don't process Tab in case keyboard input is enabled and the user
			// wishes to Tab to other controls.
			break;

		case VK_HOME:
		{
			PlaceCaret ( 0 );

			// Shift is not down. Update selection
			// start along with the caret.
			if ( GetKeyState ( VK_SHIFT ) >= 0 )
			{
				m_nSelStart = m_nCaret;
			}

			bHandled = true;
			ResetCaretBlink ();
			break;
		}

		case VK_END:
		{
			PlaceCaret ( m_Buffer.GetTextSize () );

			// Shift is not down. Update selection
			// start along with the caret.
			if ( GetKeyState ( VK_SHIFT ) >= 0 )
			{
				m_nSelStart = m_nCaret;
			}

			bHandled = true;
			ResetCaretBlink ();
			break;
		}

		case VK_INSERT:
		{
			if ( GetKeyState ( VK_CONTROL ) < 0 )
			{
				// Control Insert. Copy to clipboard
				CopyToClipboard ();
			}
			else if ( GetKeyState ( VK_SHIFT ) < 0 )
			{
				// Shift Insert. Paste from clipboard
				PasteFromClipboard ();
			}
			else
			{
				// Toggle caret insert mode
				m_bInsertMode = !m_bInsertMode;
			}
			break;
		}

		case VK_DELETE:
		{
			// Check if there is a text selection.
			if ( m_nCaret != m_nSelStart )
			{
				DeleteSelectionText ();
				SendEvent ( EVENT_CONTROL_CHANGE, true );
			}
			else
			{
				// Deleting one character
				if ( m_Buffer.RemoveChar ( m_nCaret ) )
				{
					SendEvent ( EVENT_CONTROL_CHANGE, true );
				}
			}

			bHandled = true;
			ResetCaretBlink ();
			break;
		}

		case VK_LEFT:
		{
			if ( GetKeyState ( VK_CONTROL ) < 0 )
			{
				// Control is down. Move the caret to a new item
				// instead of a character.
				m_Buffer.GetPriorItemPos ( m_nCaret, &m_nCaret );
				PlaceCaret ( m_nCaret );
			}
			else if ( m_nCaret > 0 )
			{
				PlaceCaret ( m_nCaret - 1 );
			}

			// Shift is not down. Update selection
			// start along with the caret.
			if ( GetKeyState ( VK_SHIFT ) >= 0 )
			{
				m_nSelStart = m_nCaret;
			}

			bHandled = true;
			ResetCaretBlink ();
			break;
		}

		case VK_RIGHT:
		{
			if ( GetKeyState ( VK_CONTROL ) < 0 )
			{
				// Control is down. Move the caret to a new item
				// instead of a character.
				m_Buffer.GetNextItemPos ( m_nCaret, &m_nCaret );
				PlaceCaret ( m_nCaret );
			}
			else if ( m_nCaret < m_Buffer.GetTextSize () )
			{
				PlaceCaret ( m_nCaret + 1 );
			}

			// Shift is not down. Update selection
			// start along with the caret.
			if ( GetKeyState ( VK_SHIFT ) >= 0 )
			{
				m_nSelStart = m_nCaret;
			}

			bHandled = true;
			ResetCaretBlink ();
			break;
		}

		case VK_UP:
		case VK_DOWN:
		{
			// Trap up and down arrows so that the dialog
			// does not switch focus to another control.
			bHandled = true;
			break;
		}

		default:
			bHandled = wParam != VK_ESCAPE;  // Let the application handle Esc.
	}

	return bHandled;
}

bool CEditBox::OnKeyCharacter ( WPARAM wParam )
{
	if ( !CanHaveFocus () )
		return false;

	bool bHandled = false;
	switch ( ( WCHAR ) wParam )
	{
		// Backspace
		case VK_BACK:
		{
			// If there's a selection, treat this
			// like a delete key.
			if ( m_nCaret != m_nSelStart )
			{
				DeleteSelectionText ();
				SendEvent ( EVENT_CONTROL_CHANGE, true );
			}
			else if ( m_nCaret > 0 )
			{
				// Move the caret, then delete the char.
				PlaceCaret ( m_nCaret - 1 );
				m_nSelStart = m_nCaret;
				m_Buffer.RemoveChar ( m_nCaret );
				SendEvent ( EVENT_CONTROL_CHANGE, true );
			}

			bHandled = true;
			ResetCaretBlink ();
			break;
		}

		case 24:        // Ctrl-X Cut
		case VK_CANCEL: // Ctrl-C Copy
		{
			CopyToClipboard ();

			// If the key is Ctrl-X, delete the selection too.
			if ( ( WCHAR ) wParam == 24 )
			{
				DeleteSelectionText ();
				SendEvent ( EVENT_CONTROL_CHANGE, true );
			}
			bHandled = true;
			break;
		}

		// Ctrl-V Paste
		case 22:
		{
			PasteFromClipboard ();
			SendEvent ( EVENT_CONTROL_CHANGE, true );

			bHandled = true;
			break;
		}

		// Ctrl-A Select All
		case 1:
		{
			if ( m_nSelStart == m_nCaret )
			{
				m_nSelStart = 0;
				PlaceCaret ( m_Buffer.GetTextSize () );
			}

			bHandled = true;
			break;
		}

		case VK_RETURN:
		{
			// Invoke the callback when the user presses Enter.
			SendEvent ( EVENT_CONTROL_CHANGE, true );
			bHandled = true;
			break;
		}
			// Junk characters we don't want in the string
		case 26:  // Ctrl Z
		case 2:   // Ctrl B
		case 14:  // Ctrl N
		case 19:  // Ctrl S
		case 4:   // Ctrl D
		case 6:   // Ctrl F
		case 7:   // Ctrl G
		case 10:  // Ctrl J
		case 11:  // Ctrl K
		case 12:  // Ctrl L
		case 17:  // Ctrl Q
		case 23:  // Ctrl W
		case 5:   // Ctrl E
		case 18:  // Ctrl R
		case 20:  // Ctrl T
		case 25:  // Ctrl Y
		case 21:  // Ctrl U
		case 9:   // Ctrl I
		case 15:  // Ctrl O
		case 16:  // Ctrl P
		case 27:  // Ctrl [
		case 29:  // Ctrl ]
		case 28:  // Ctrl \ 
			break;

		default:
		{
			// If there's a selection and the user
			// starts to type, the selection should
			// be deleted.
			if ( m_nCaret != m_nSelStart )
			{
				DeleteSelectionText ();
			}

			// If we are in overwrite mode and there is already
			// a char at the caret's position, simply replace it.
			// Otherwise, we insert the char as normal.
			if ( !m_bInsertMode && m_nCaret < m_Buffer.GetTextSize () )
			{
				m_Buffer [ m_nCaret ] = ( WCHAR ) wParam;
				PlaceCaret ( m_nCaret + 1 );
				m_nSelStart = m_nCaret;
			}
			else
			{
				// Insert the char
				if ( m_Buffer.InsertChar ( m_nCaret, ( WCHAR ) wParam ) )
				{
					PlaceCaret ( m_nCaret + 1 );
					m_nSelStart = m_nCaret;
				}
			}

			SendEvent ( EVENT_CONTROL_CHANGE, true );
			bHandled = true;
			ResetCaretBlink ();
		}
	}

	return bHandled;
}

void CEditBox::UpdateRects ( void )
{
	CWidget::UpdateRects ();

	m_rText = m_rBoundingBox;
	m_rText.m_pos.m_nX += 4;
	m_rText.m_pos.m_nY += 2;
	
	m_rText.m_size.cx -= 6;
	m_rText.m_size.cy -= 3;
}

bool CEditBox::ContainsPoint ( Pos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( m_rBoundingBox.ContainsPoint ( pos ) || 
			 m_rText.ContainsPoint ( pos ) );
}
