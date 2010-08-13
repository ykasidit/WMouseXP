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


// WMouseXPMobileDevice.h: interface for the WMouseXPMobileDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WMOUSEXPMOBILEDEVICE_H__005DAA3C_5069_4BD0_B74B_C782BE370493__INCLUDED_)
#define AFX_WMOUSEXPMOBILEDEVICE_H__005DAA3C_5069_4BD0_B74B_C782BE370493__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class WMouseXPMobileDevice  
{
public:
	WMouseXPMobileDevice(char* addr,int port,LPTSTR devname);
	virtual ~WMouseXPMobileDevice();

	char iAddr[40];
	CString iDevName;

	int iPort;
	

};

#endif // !defined(AFX_WMOUSEXPMOBILEDEVICE_H__005DAA3C_5069_4BD0_B74B_C782BE370493__INCLUDED_)
