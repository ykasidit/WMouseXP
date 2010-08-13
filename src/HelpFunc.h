/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.

*******************************************************************************************/

//modified for WMouseXP by Kasidit Yusuf

#ifndef _HELP_FUNC_H
#define _HELP_FUNC_H

#include "bt_ui.h"

#define MAX_CONNECT_COUNT	8
#define MAX_SERVICE_COUNT	100

DWORD Help_InputAddr(char temp_addr[], UCHAR bdAddr[6]);
DWORD Help_InputDeviceClass(char temp[], UCHAR device_class[]);
DWORD Help_InputLinkKey(char temp[], UCHAR link_key[]);

void PrintError(CHAR* lpszAPI,DWORD dwError);
void PrintCbkError(DWORD dwError);
void PrintDeviceClassInfo(BYTE devClass[3]);

BOOL ConfigConnectParams(WORD wSvcCls, BYTE* pParam, DWORD* pParamLen);



#endif _HELP_FUNC_H
