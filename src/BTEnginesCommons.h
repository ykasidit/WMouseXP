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


#ifndef __WMOUSEXP_BT_ENGINE_COMMONS_H__
#define __WMOUSEXP_BT_ENGINE_COMMONS_H__

const int KPATHMAXLEN = 512;

#include "stdafx.h"

#if !defined(KNotInstalledOnPhoneStr)

#define KNotInstalledOnPhoneStr _T("Send && Install on phone")

#endif

class MRemoteControlCaller
{
	public:

	virtual void OnData(void *p_data, UINT len)=0;
	//virtual void OnConnectEvent(UINT32 event_code,unsigned char* devaddr)=0;
	virtual void OnConnected(BYTE* addr, int len)=0;
	virtual void OnDisconnected()=0;
	
	virtual void OnRFCOMMWriteResult(int len, int err, char* desc)=0;

};

class MBTEngineHandler
{
public:
	virtual bool RFCOMMWrite(BYTE* data, USHORT len, USHORT& lenwritten)=0;
};

#define KWmxpJarPathRelativeToReleaseExe "..\\j2me_client\\WMouseXP\\dist\\WMouseXP.jar"
#define KWmxpJarPathRelativeToReleaseExe_wc _L("..\\j2me_client\\WMouseXP\\dist\\WMouseXP.jar")

#endif