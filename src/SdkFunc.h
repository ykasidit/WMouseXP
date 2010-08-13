/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.

*******************************************************************************************/

//modified for WMouseXP by Kasidit Yusuf


#ifndef _BSOL_SDK_FUNC_H_
#define  _BSOL_SDK_FUNC_H_

#include <bt_ui.h>

BOOL SDK_Init();
void SDK_Done();
void SDK_BtPair();
void SDK_BtUnpair();
void SDK_BtBrowseServices(BYTE* g_targetBdAddr);
void SDK_BtInquiry();
void SDK_BtConnectService();
void SDK_BtDisConnectService();
void SDK_BtGetRemoteInfo();
void SDK_BtSetRemoteInfo();
void SDK_BtGetLocalInfo();
void SDK_BtSetLocalInfo();
void SDK_BtRegisterCallBack(int nInput);
void SDK_BtUnRegisterCallBack(int nInput);
void SDK_BtStartSPPExService();
void SDK_BtStopSPPExService();

int SDK_BtSearchSPPExServices(BYTE* g_targetBdAddr, BYTE* guid /*128-bit guid*/, SPPEX_SERVICE_INFO& ret /*OUT: valid only if function returns > 0*/); //returns -1 if not found else returns service port

void SDK_BtConnectSPPExService();
void SDK_BtDisConnectSPPExService();
void SDK_StartBluetooth();
void SDK_StopBluetooth();
void SDK_CancelInquiry();
void SDK_StartService();
void SDK_StopService();
void SDK_EnumConnections();

void SDK_ReadRemoteAddress();

/*******************************CALLBACK FUNCTION DEFINITION**********************/
void CBK_Authentication(BYTE* lpAddr, BOOL bSuccess);
DWORD CBK_PinRequest(BYTE* lpAddr, UCHAR* lpPinLen, BYTE *lpPinCode);
void CBK_Connection(WORD wServiceClass, BYTE* lpBdAddr, UCHAR ucStatus,DWORD dwConnetionHandle);
void CBK_SPPExConnection(DWORD dwServerHandle, BYTE* lpBdAddr, UCHAR ucStatus,DWORD dwConnetionHandle);
void CBK_DunRas(UCHAR ucStrLen, CHAR* szErrorString, DWORD dwError);
void CBK_GeneralError(DWORD dwErrorCode);
void CBK_InquiryDeviceReport(/*in*/PBLUETOOTH_DEVICE_INFO pDevInfo);
void CBK_BluetoothStatus(/*in*/UCHAR ucStatus);
void CBK_PanProfile(DWORD dwHandle, DWORD dwEventIndex, UCHAR* pData);
DWORD CBK_Authorization(/*in*/BYTE* lpBdAddr,/*in*/WORD wServiceClass,/*in*/CHAR* lpServiceName, /*out*/BOOL* bAuthorized);

void TS_PanIpAllocation();
void TS_ConnectRobustness();

#define MAX_SPPEX 2
typedef struct _SPPEX_INFO{
	DWORD dwHanlde;
	SPPEX_SERVICE_INFO svcInfo;
	BOOL bIsConnected;
}SPPEX_INFO;

extern SPPEX_INFO g_sppexSvr[MAX_SPPEX];

#endif
