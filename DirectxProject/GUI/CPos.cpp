#include "CGUI.h"

CPos::CPos(POINT point)
{
	m_nX = point.x;
	m_nY = point.y;
}

CPos::CPos( CPos *pPos )
{
	m_nX = pPos->m_nX;
	m_nY = pPos->m_nY;
}

CPos::CPos( int nX, int nY )
{
	m_nX = nX;
	m_nY = nY;
}

CPos::CPos()
{
	m_nX = 0;
	m_nY = 0;
}

CPos::~CPos()
{
}

CPos CPos::operator*(CPos otherPos)
{
	return  CPos(m_nX * otherPos.m_nX, m_nY * otherPos.m_nY);
}

CPos CPos::operator/(CPos otherPos)
{
	return CPos(m_nX / otherPos.m_nX, m_nY / otherPos.m_nY);
}

CPos CPos::operator*(int pos)
{
	return CPos(m_nX * pos, m_nY * pos);
}

CPos CPos::operator/(int pos)
{
	return CPos(m_nX / pos, m_nY / pos);
}

CPos CPos::operator*=(CPos otherPos)
{
	m_nX *= otherPos.m_nX;
	m_nY *= otherPos.m_nY;

	return *this;
}

CPos CPos::operator/=(CPos otherPos)
{
	m_nX /= otherPos.m_nX;
	m_nY /= otherPos.m_nY;

	return *this;
}

CPos CPos::operator*=(int pos)
{
	m_nX *= pos;
	m_nY *= pos;

	return *this;
}

CPos CPos::operator/=(int pos)
{
	m_nX /= pos;
	m_nY /= pos;

	return *this;
}

CPos CPos::operator + ( CPos otherPos )
{
	return CPos(m_nX + otherPos.m_nX, m_nY + otherPos.m_nY);
}

CPos CPos::operator - ( CPos otherPos )
{
	return CPos(m_nX - otherPos.m_nX, m_nY - otherPos.m_nY);
}

CPos CPos::operator + ( int pos)
{
	return CPos (m_nX + pos, m_nY + pos);
}

CPos CPos::operator - ( int pos)
{
	return CPos (m_nX - pos, m_nY - pos);
}

CPos CPos::operator+=(CPos otherPos)
{
	m_nX += otherPos.m_nX;
	m_nY += otherPos.m_nY;

	return *this;
}

CPos CPos::operator-=(CPos otherPos)
{
	m_nX -= otherPos.m_nX;
	m_nY -= otherPos.m_nY; 
	
	return *this;
}

CPos CPos::operator+=(int pos)
{
	m_nX += pos;
	m_nY += pos;

	return *this;
}

CPos CPos::operator-=(int pos)
{
	m_nX -= pos;
	m_nY -= pos;

	return *this;
}

bool CPos::operator == (CPos otherPos)
{
	return (m_nX == otherPos.m_nX &&
		m_nY == otherPos.m_nY);
}

bool CPos::operator!=(CPos otherPos)
{
	return !operator==(otherPos);
}


