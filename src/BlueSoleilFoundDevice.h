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


// BlueSoleilFoundDevice.h: interface for the CBlueSoleilFoundDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLUESOLEILFOUNDDEVICE_H__F281AF20_614B_4032_822E_43912758B408__INCLUDED_)
#define AFX_BLUESOLEILFOUNDDEVICE_H__F281AF20_614B_4032_822E_43912758B408__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bt_ui.h"

class CBlueSoleilFoundDevice  
{
public:
	CBlueSoleilFoundDevice(const BLUETOOTH_DEVICE_INFO* devinfo);
	virtual ~CBlueSoleilFoundDevice();

	//BYTE iAddr[DEVICE_ADDRESS_LENGTH];	
	//CHAR iName[MAX_DEVICE_NAME_LENGTH];

	SPPEX_SERVICE_INFO iSPPInfo;
	BLUETOOTH_DEVICE_INFO iBTDevInfo;

	
	//UINT8 iWMouseXPscn;
};

#endif // !defined(AFX_BLUESOLEILFOUNDDEVICE_H__F281AF20_614B_4032_822E_43912758B408__INCLUDED_)
