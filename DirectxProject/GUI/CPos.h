#pragma once
#include "CGUI.h"

class CPos
{
public:
	int m_nX, m_nY;

	CPos (POINT point);
	CPos (CPos *pos);
	CPos (int iX, int iY);
	CPos ();

	~CPos ();

	CPos operator * (CPos otherPos);
	CPos operator / (CPos otherPos);

	CPos operator *= (CPos otherPos);
	CPos operator /= (CPos otherPos);

	CPos operator * (int pos);
	CPos operator / (int pos);

	CPos operator *= (int pos);
	CPos operator /= (int pos);

	CPos operator + (CPos otherPos);
	CPos operator - (CPos otherPos);

	CPos operator += (CPos otherPos);
	CPos operator -= (CPos otherPos);

	CPos operator + (int nIncPos);
	CPos operator - (int nDecPos);

	CPos operator += (int pos);
	CPos operator -= (int pos);

	bool operator == (CPos otherPos);
	bool operator != (CPos otherPos);
};