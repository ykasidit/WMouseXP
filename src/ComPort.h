/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.

*******************************************************************************************/

//modified for WMouseXP by Kasidit Yusuf

#ifndef _BSOL_COM_PORT_H_
#define _BSOL_COM_PORT_H_

BOOL StartIoOnComport(DWORD dwConnHandle,int nComport, BOOL bClientRole);
BOOL StopIoOnComport(DWORD dwConnHandle);

#endif
