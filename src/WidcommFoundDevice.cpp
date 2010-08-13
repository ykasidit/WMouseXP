/*
Copyright (C) 2007-2010 Kasidit Yusuf.

This file is part of WMouseXP.

WMouseXP is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

WMouseXP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WMouseXP. If not, see <http://www.gnu.org/licenses/>.
*/


// WidcommFoundDevice.cpp: implementation of the CWidcommFoundDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WMouseXP.h"
#include "WidcommFoundDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWidcommFoundDevice::CWidcommFoundDevice(BD_ADDR addr, BD_NAME name)
{
	for(int i=0;i<BD_ADDR_LEN;i++)
			iAddr[i] = addr[i];

	for(i=0;i<BD_NAME_LEN;i++)
			iName[i] = name[i];

	iWMouseXPscn = 0;
}

CWidcommFoundDevice::~CWidcommFoundDevice()
{

}
