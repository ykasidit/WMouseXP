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


// WidcommFoundDevice.h: interface for the CWidcommFoundDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIDCOMMFOUNDDEVICE_H__24572E76_361A_48DA_B11D_571BCE081807__INCLUDED_)
#define AFX_WIDCOMMFOUNDDEVICE_H__24572E76_361A_48DA_B11D_571BCE081807__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "btifdefinitions.h"

class CWidcommFoundDevice  
{
public:
	CWidcommFoundDevice(BD_ADDR addr, BD_NAME name);
	virtual ~CWidcommFoundDevice();	
	

	BD_ADDR iAddr;
	BD_NAME iName;
	UINT8 iWMouseXPscn;

};

#endif // !defined(AFX_WIDCOMMFOUNDDEVICE_H__24572E76_361A_48DA_B11D_571BCE081807__INCLUDED_)
