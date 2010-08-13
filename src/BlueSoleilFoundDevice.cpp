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

// BlueSoleilFoundDevice.cpp: implementation of the CBlueSoleilFoundDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "WMouseXP.h"
#include "BlueSoleilFoundDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlueSoleilFoundDevice::CBlueSoleilFoundDevice(const BLUETOOTH_DEVICE_INFO* btdev)
{
	
	memset(&iBTDevInfo,0,sizeof(BLUETOOTH_DEVICE_INFO));
	memset(&iSPPInfo,0,sizeof(SPPEX_SERVICE_INFO));
	
	memcpy(&iBTDevInfo, btdev, sizeof(BLUETOOTH_DEVICE_INFO));


	/*for(int i=0;i<DEVICE_ADDRESS_LENGTH;i++)
			iAddr[i] = addr[i];

	for(i=0;i<MAX_DEVICE_NAME_LENGTH;i++)
			iName[i] = name[i];

	iWMouseXPscn = 0;*/
}

CBlueSoleilFoundDevice::~CBlueSoleilFoundDevice()
{

}
