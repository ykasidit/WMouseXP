/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.
  
*******************************************************************************************/

//modified for WMouseXP by Kasidit Yusuf

#include "stdafx.h"

#include "HelpFunc.h"
#include "SdkFunc.h"



#include "BlueSoleilHandler.h"
#include "ComPort.h"

//#include <stdio.h>
//#include "menu.h"



#include "ConnectCheck.h"
char g_szIniPathDup[MAX_PATH] = "BlueSoleilClient.ini";//complier re-definition problem - tired of fixing it so use this lazy fix

GENERAL_CONNECT_INFO g_connList[MAX_CONNECT_COUNT] = {0};
GENERAL_SERVICE_INFO g_svcList[MAX_SERVICE_COUNT]={0};
SPPEX_INFO g_sppexSvr[MAX_SPPEX]={0};
DWORD g_dwLastError;



BOOL SDK_Init()
{
	//AfxMessageBox("0");
    DWORD dwBtVersion;
	
	if(!BT_InitializeLibrary()){
		printf("BT_InitializeLibrary() failed!\n");
		return FALSE;
	}
	//AfxMessageBox("1");
	dwBtVersion = BT_GetVersion();
	printf("BlueSoleil API version: %d.%d\n",LOBYTE(LOWORD(dwBtVersion)),HIBYTE(LOWORD(dwBtVersion)));
	
	//BT_GetBtSpecVersion(&dwBtVersion);
	printf("Compliant BT Spec version: %d.%d\n",LOBYTE(LOWORD(dwBtVersion)),HIBYTE(LOWORD(dwBtVersion)));
	
	//AfxMessageBox("2");
//	ChangeMenu2(MENU_STANDBY);
//	ShowMenu();

	return TRUE;
	
}



void SDK_Done()
{
	BT_UninitializeLibrary();	
//	ChangeMenu2(MENU_IDLE);
}

/*void SDK_BtPair(BYTE* g_targetBdAddr)
{
	BLUETOOTH_DEVICE_INFO devInfo={0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	memcpy(devInfo.address,g_targetBdAddr,6);
	
	char lpPinCode[MAX_PIN_CODE_LENGTH]={0};
	WORD wPinCodeLen = 0;
	
	GetPrivateProfileString("BT_PAIR_DEVICE", "Pin Code", NULL, lpPinCode, MAX_PIN_CODE_LENGTH, g_szIniPathDup);
	wPinCodeLen = strlen(lpPinCode);
	
	int bKeepOldKeyOnFail;
	bKeepOldKeyOnFail=GetPrivateProfileInt("BT_PAIR_DEVICE", "KeepOldKeyOnFail", 0, g_szIniPathDup);
	
	int bShowPincode;
	bShowPincode=GetPrivateProfileInt("BT_PAIR_DEVICE", "ShowPincode", 0, g_szIniPathDup);
	
	DWORD dwResult;
	dwResult=BT_PairDevice(&devInfo, wPinCodeLen, ((wPinCodeLen == 0) ? NULL : (UCHAR*)lpPinCode),bKeepOldKeyOnFail,bShowPincode);
	PrintError("BT_PairDevice",dwResult);
}

void SDK_BtUnpair()
{
	DWORD dwResult;
	dwResult = BT_UnpairDevice(g_targetBdAddr);
	PrintError("BT_UnpairDevice",dwResult);
}*/

void SDK_BtInquiry()
{
	UCHAR	ucInqMode = /*INQUIRY_GENERAL_REFRESH*/INQUIRY_GENERAL_MODE,ucInqLen = 10;

	BLUETOOTH_DEVICE_INFO	lpDevsList[MAX_SERVICE_COUNT] = {0};
	DWORD	DevsListLen = sizeof(BLUETOOTH_DEVICE_INFO) * MAX_SERVICE_COUNT;
	
	/*char temp[MAX_PATH]={0};
	if(GetPrivateProfileString("BT_INQUIRY_DEVICE", "Inquiry Mode", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
		AfxMessageBox("Get Inquiry Mode Failed!\n");
		return;
	}
	ucInqMode = atoi(temp);
	//Get Inquiry Length		0x00-0xFF
	if(GetPrivateProfileString("BT_INQUIRY_DEVICE", "Inquiry Length", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
		AfxMessageBox("Get Inquiry Mode Failed!\n");
		return;
	}
	ucInqLen = atoi(temp);
	if(ucInqLen<=0){
		AfxMessageBox("Get Inquiry Mode Failed!\n");
		return;
	}
	
    BOOL bAsynchronous = GetPrivateProfileInt("BT_INQUIRY_DEVICE", "bAsynchronous",1, g_szIniPathDup);
	
	DWORD dwResult;
	if(bAsynchronous)*/

	BOOL bAsynchronous = FALSE;
	DWORD dwResult;
		//DevsListLen = 0;
	
	dwResult = BT_InquireDevices(ucInqMode, ucInqLen,  &DevsListLen, lpDevsList);
	PrintError("BT_InquireDevices",dwResult);
	
	if(!bAsynchronous && dwResult == BTSTATUS_SUCCESS){
		printf("Devices List\n");
		DWORD i, offset;
		offset = sizeof(BLUETOOTH_DEVICE_INFO);
		BLUETOOTH_DEVICE_INFO *pDevice;
		for(i=0; i<((DevsListLen)/sizeof(BLUETOOTH_DEVICE_INFO)); i++){
			pDevice=(BLUETOOTH_DEVICE_INFO*)((UCHAR*)lpDevsList+i*offset);
			
			CString s;

			s.Format("Device No.%d        Device Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
				i,
				pDevice->address[5],
				pDevice->address[4],
				pDevice->address[3],
				pDevice->address[2],
				pDevice->address[1],
				pDevice->address[0]);

			//AfxMessageBox(s);
			/*
			DWORD dwMask = 0;
			BLUETOOTH_DEVICE_INFO_EX devInfo = {0};
			memcpy(&devInfo.address, pDevice->address, DEVICE_ADDRESS_LENGTH);
			devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_EX);
			devInfo.szName[0]='\0';
			dwMask = MASK_DEVICE_NAME;
			
			//DWORD dwResult;
			
			//dwResult = BT_SetRemoteDeviceInfo(dwMask, &devInfo);
			//PrintError("BT_SetRemoteDeviceInfo",dwResult);

			//dwResult = BT_GetRemoteDeviceInfo(dwMask, &devInfo);
			//PrintError("BT_GetRemoteDeviceInfo",dwResult);
			
			//printf("        Device Name: %s\n", devInfo.szName);
			//AfxMessageBox(devInfo.szName);

			//PrintDeviceClassInfo(pDevice->classOfDevice);
			if(pDevice->bPaired)
				printf("        Device Is Paired: Yes\n");
			else
				printf("        Device Is Paired: No\n");
				*/

			if(CBlueSoleilHandler::curInstance)
				CBlueSoleilHandler::curInstance->DoAddDevice(*pDevice);
		}
	}
}

void SDK_BtBrowseServices(BYTE* g_targetBdAddr)
{
	BLUETOOTH_DEVICE_INFO devInfo={0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	memcpy(devInfo.address,g_targetBdAddr,6);
	
	GENERAL_SERVICE_INFO pClsidList[MAX_SERVICE_COUNT]={0};
	DWORD ClsidListLen = MAX_SERVICE_COUNT*sizeof(GENERAL_SERVICE_INFO);
	BOOL bBrowseAllService = TRUE;
	
	//Clear the g_device_service_info
	memset((void*)g_svcList, 0, MAX_SERVICE_COUNT*sizeof(GENERAL_SERVICE_INFO));
	
	char temp[MAX_PATH]={0};
	//Get Browse All Services
	if(GetPrivateProfileString("BT_BROWSE_SERVICE", "Browse All Services", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
		printf("Get Browse All Services Failed!\n");
		return;
	}
	strcpy(temp, strupr(temp));
	if(strcmp(temp, "TRUE") == 0)
		bBrowseAllService = TRUE;
	else if(strcmp(temp, "FALSE") == 0)
		bBrowseAllService = FALSE;
	else{
		printf("Invalid Browse All Service!\n");
		return;
	}
	
	if(!bBrowseAllService){
		if(GetPrivateProfileString("BT_BROWSE_SERVICE", "Specific Service Class", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
			printf("Get Specific Service Class Failed!\n");
			return;
		}
		if((pClsidList[0].wServiceClassUuid16 = atoi(temp)) == 0){
			printf("Invalid Specific Service Class!\n");
			return;
		}
	}
	DWORD dwReusult;
	dwReusult = BT_BrowseServices(&devInfo, bBrowseAllService, &ClsidListLen, pClsidList);
	PrintError("BT_BrowseServices",dwReusult);
	if(dwReusult  ==BTSTATUS_SUCCESS)	{
		
		

		printf("Device Address: %02X:%02X:%02X:%02X:%02X:%02X \n",
			g_targetBdAddr[5],
			g_targetBdAddr[4],
			g_targetBdAddr[3],
			g_targetBdAddr[2], 
			g_targetBdAddr[1], 
			g_targetBdAddr[0]); 

		



		printf("#	Svc ID	Svc Handle	Svc Name\n");
		for(DWORD i=0; i<ClsidListLen/sizeof(GENERAL_SERVICE_INFO);i++){

			
			printf("#%d	0x%x	%d	%s\n", i, pClsidList[i].wServiceClassUuid16, pClsidList[i].dwServiceHandle,pClsidList[i].szServiceName);
			
			g_svcList[i].wServiceClassUuid16 = pClsidList[i].wServiceClassUuid16;
			g_svcList[i].dwServiceHandle = pClsidList[i].dwServiceHandle;
			g_svcList[i].dwSize = pClsidList[i].dwSize;


		}
	}
	
}

void PostConnect(DWORD dwHanlde, WORD wSvcId, BYTE* pParam)
{
	switch(wSvcId)
	{
	case CLS_SERIAL_PORT:
		{
			//Start IO on the COM port
			int bDClientIO = GetPrivateProfileInt("SPP_COM_PORT", "DO Client IO", 0,g_szIniPathDup);
			if(bDClientIO)
				StartIoOnComport(dwHanlde,((PSPP_CLIENT_PARAM)pParam)->ucComPort, TRUE);
		}
		break;
	case CLS_HEADSET:
	case CLS_HANDSFREE:
		{
		}
		break;
	default:;
	}
}


void SDK_BtConnectService(BYTE* g_targetBdAddr)
{
	SDK_BtBrowseServices(g_targetBdAddr);
	
	printf("Please input the number of the service. (-1 to return)\n");
	
	int nInput;
	scanf("%d",&nInput);
	if(nInput == -1 )
		return;
	
	if(nInput > MAX_SERVICE_COUNT || nInput<0 || g_svcList[nInput].wServiceClassUuid16 ==0){
		printf("Invalid item.\n");
	}
	
	BLUETOOTH_DEVICE_INFO devInfo={0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	memcpy(devInfo.address, g_targetBdAddr, 6);
	
	BYTE abyConnParam[512];
	DWORD dwParamLen = 512;
	
	if(!ConfigConnectParams(g_svcList[nInput].wServiceClassUuid16,abyConnParam,&dwParamLen)){
		printf("Failed to config parameters.\n");
		return;
	}
	
	DWORD dwResult;
	DWORD dwHandle;
	dwResult = BT_ConnectService(&devInfo, &g_svcList[nInput], dwParamLen==0 ? NULL : abyConnParam, &dwHandle);
	PrintError("BT_ConnectService",dwResult);
	
	if(dwResult == BTSTATUS_SUCCESS){
		PostConnect(dwHandle, g_svcList[nInput].wServiceClassUuid16,abyConnParam);
	}
}

void SDK_BtDisConnectService()
{
	SDK_EnumConnections();
	
	printf("Please input the number of the connection. (-1 to return)\nBSOL_CLIENT>");
	
	int nInput;
	scanf("%d",&nInput);
	if(nInput == -1 )
		return;
	
	if(nInput > MAX_CONNECT_COUNT || nInput<0 || g_connList[nInput].dwHandle ==0){
		printf("Invalid input.\n");
		return;
	}
	
	DWORD dwResult = BT_DisconnectService(g_connList[nInput].dwHandle);
	PrintError("BT_DisconnectService",dwResult);
	
	if(dwResult == BTSTATUS_SUCCESS){
		memset(&g_connList[nInput], 0 , sizeof(GENERAL_CONNECT_INFO));
	}
	
}


void SDK_BtGetRemoteInfo(BYTE* g_targetBdAddr)
{
	DWORD dwMask = 0;
	
	BLUETOOTH_DEVICE_INFO_EX devInfo={0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_EX);
	memcpy(devInfo.address, g_targetBdAddr, 6);
	
	char temp[MAX_PATH];
	if(GetPrivateProfileString("GET_REMOTE_DEVICE_INFO", "Mask", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
		printf("Get Mask Failed!\n");
		return;
	}
	if((dwMask = atoi(temp)) == 0){
		printf("Invalid Parameters: 'Mask'\n");
		return;
	}
	//dwMask = 0x0000ffff;
	DWORD dwResult = BT_GetRemoteDeviceInfo(dwMask, &devInfo);
	PrintError("BT_GetRemoteDeviceInfo",dwResult);
	if(dwResult != BTSTATUS_SUCCESS){
		return;
	}
	printf("Device Name:			%s\nDevice class:			%02X %02X %02X\n", 
		devInfo.szName, 
		devInfo.classOfDevice[0],
		devInfo.classOfDevice[1],
		devInfo.classOfDevice[2]);
	if(devInfo.bConnected)
		printf("Device Connection Status:	YES\n");
	else
		printf("Device Connection Status:	NO\n");
	
	if(devInfo.bPaired)
		printf("Device Paired Status:		YES\n");
	else
		printf("Device Paired Status:		NO\n");
	printf("\n");
	printf("Device Clock Offset:		%d\n", devInfo.wClockOffset);
	printf("Device LPM version:		%d\n", devInfo.ucLmpVersion);
	printf("Device Manufacture:		%d\n", devInfo.wManuName);
	printf("Device Sub LMP version:		%d\n", devInfo.wLmpSubversion);
	printf("Device dwDataRecvBytes:		%d\n", devInfo.dwDataRecvBytes);
	printf("Device dwDataSentBytes:		%d\n", devInfo.dwDataSentBytes);
	printf("Device Signal Strength:		%d\n", devInfo.cSignalStrength);
	
}

void SDK_BtSetRemoteInfo(BYTE* g_targetBdAddr)
{
	DWORD dwMask = 0;
	BLUETOOTH_DEVICE_INFO_EX devInfo = {0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_EX);
	memcpy(devInfo.address, g_targetBdAddr, 6);
	
	char temp[MAX_PATH];
	//Get Mask
	if(GetPrivateProfileString("SET_REMOTE_DEVICE_INFO", "Mask", NULL, temp, MAX_PATH, g_szIniPathDup)==0)	{
		printf("Get Mask Failed!\n");
		return;
	}
	if((dwMask = atoi(temp)) == 0){
		printf("Invalid Parameters: 'Mask'\n");
		return;
	}
	//Get Name
	if(GetPrivateProfileString("SET_REMOTE_DEVICE_INFO", "Name", "", devInfo.szName, MAX_DEVICE_NAME_LENGTH, g_szIniPathDup)==0){
	}
	//Get Device Class
	if(GetPrivateProfileString("SET_REMOTE_DEVICE_INFO", "Device Class", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
		printf("Get Device Class Failed!\n");
		return;
	}
	if(Help_InputDeviceClass(temp, devInfo.classOfDevice) == BTSTATUS_FAIL){
		printf("Invalid Device Class!\n");
		return;
	}
	
	//Get Clock Offset
	if(GetPrivateProfileString("SET_REMOTE_DEVICE_INFO", "Clock Offset", NULL, temp, MAX_PATH, g_szIniPathDup)==0)	{
		printf("Get Link Key Failed!\n");
		return;
	}
	devInfo.wClockOffset = atoi(temp);
	
	DWORD dwResult;
	dwResult = BT_SetRemoteDeviceInfo(dwMask, &devInfo);
	PrintError("BT_SetRemoteDeviceInfo",dwResult);
	
}

void SDK_BtGetLocalInfo(BYTE* g_targetBdAddr)
{
	DWORD dwMask = 0;
	BLUETOOTH_DEVICE_INFO_EX devInfo = {0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_EX);
	memcpy(devInfo.address, g_targetBdAddr, 6);
	
	char temp[MAX_PATH];
	//Get Mask
	if(GetPrivateProfileString("GET_LOCAL_DEVICE_INFO", "Mask", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
		printf("Get Mask Failed!\n");
		return;
	}
	if((dwMask = atoi(temp)) == 0){
		printf("Invalid Parameters: 'Mask'\n");
		return;
	}
	memcpy(devInfo.address, g_targetBdAddr, 6);
	
	DWORD dwResult;
	dwResult = BT_GetLocalDeviceInfo(dwMask, &devInfo);
	PrintError("BT_GetLocalDeviceInfo",dwResult);
	
	printf("Device Address:	%02X:%02X:%02X:%02X:%02X:%02X \n",
		devInfo.address[5],
		devInfo.address[4],
		devInfo.address[3],
		devInfo.address[2], 
		devInfo.address[1], 
		devInfo.address[0]);
	printf("Device Name:			%s\nDevice class:			%02X %02X %02X\n", 
		devInfo.szName, 
		devInfo.classOfDevice[0],
		devInfo.classOfDevice[1],
		devInfo.classOfDevice[2]);
	printf("Device Manufacturer Name:	%d\n", devInfo.wManuName);
	printf("Device LMP Version:		%d\n", devInfo.ucLmpVersion);
	printf("Device LMP SubVersion:		%d\n", devInfo.wLmpSubversion);
	
	if(devInfo.bConnected)
		printf("Device Connection Status:	YES\n");
	else
		printf("Device Connection Status:	NO\n");
	
	
}

void SDK_BtSetLocalInfo(BYTE* g_targetBdAddr)
{
	DWORD dwMask = 0;
	BLUETOOTH_DEVICE_INFO devInfo = {0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	memcpy(devInfo.address, g_targetBdAddr, 6);
	
	char temp[MAX_PATH];
	//Get Mask
	if(GetPrivateProfileString("SET_LOCAL_DEVICE_INFO", "Mask", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
		printf("Get Mask Failed!\n");
		return;
	}
	if((dwMask = atoi(temp)) == 0){
		printf("Invalid Parameters: 'Mask'\n");
		return;
	}
	//Get Name
	if(GetPrivateProfileString("SET_LOCAL_DEVICE_INFO", "Name", NULL, devInfo.szName, MAX_DEVICE_NAME_LENGTH, g_szIniPathDup)==0){
		printf("Get Name Failed!\n");
		return;
	}
	//Get Device Class
	if(GetPrivateProfileString("SET_LOCAL_DEVICE_INFO", "Device Class", NULL, temp, MAX_PATH, g_szIniPathDup)==0){
		printf("Get Device Class Failed!\n");
		return;
	}
	if(Help_InputDeviceClass(temp, devInfo.classOfDevice) == BTSTATUS_FAIL){
		printf("Invalid Device Class!\n");
		return;
	}
	
    DWORD dwResult;
	dwResult=BT_SetLocalDeviceInfo(dwMask, &devInfo);
	PrintError("BT_SetLocalDeviceInfo",dwResult);
}

void SDK_BtRegisterCallBack(int nInput)
{
	
	DWORD dwResult;
	
	switch(nInput)
	{
	case 0:
		dwResult=BT_RegisterCallback(EVENT_CONNECTION_STATUS, CBK_Connection);
		PrintError("BT_RegisterCallback EVENT_CONNECTION_STATUS",dwResult);

		dwResult=BT_RegisterCallback(EVENT_DUN_RAS_CALLBACK, CBK_DunRas);
		PrintError("BT_RegisterCallback EVENT_DUN_RAS_CALLBACK",dwResult);
		
		dwResult=BT_RegisterCallback(EVENT_ERROR, CBK_GeneralError);
		PrintError("BT_RegisterCallback EVENT_ERROR",dwResult);
		
		dwResult=BT_RegisterCallback(EVENT_INQUIRY_DEVICE_REPORT, CBK_InquiryDeviceReport);
		PrintError("BT_RegisterCallback EVENT_INQUIRY_DEVICE_REPORT",dwResult);
		
		dwResult=BT_RegisterCallback(EVENT_SPPEX_CONNECTION_STATUS, CBK_SPPExConnection);
		PrintError("BT_RegisterCallback EVENT_SPPEX_CONNECTION_STATUS",dwResult);
		
		dwResult=BT_RegisterCallback(EVENT_BLUETOOTH_STATUS, CBK_BluetoothStatus);
		PrintError("BT_RegisterCallback EVENT_BLUETOOTH_STATUS",dwResult);
		
		break;
	case EVENT_CONNECTION_STATUS:
		dwResult=BT_RegisterCallback(nInput, CBK_Connection);
		PrintError("BT_RegisterCallback EVENT_CONNECTION_STATUS",dwResult);
		break;
	case EVENT_DUN_RAS_CALLBACK:
		dwResult=BT_RegisterCallback(EVENT_DUN_RAS_CALLBACK, CBK_DunRas);
		PrintError("BT_RegisterCallback EVENT_DUN_RAS_CALLBACK",dwResult);
		break;
	case EVENT_ERROR:
		dwResult=BT_RegisterCallback(EVENT_ERROR, CBK_GeneralError);
		PrintError("BT_RegisterCallback EVENT_ERROR",dwResult);
		break;
	case EVENT_INQUIRY_DEVICE_REPORT:
		dwResult=BT_RegisterCallback(EVENT_INQUIRY_DEVICE_REPORT, CBK_InquiryDeviceReport);
		PrintError("BT_RegisterCallback EVENT_INQUIRY_DEVICE_REPORT",dwResult);
		break;
	case EVENT_SPPEX_CONNECTION_STATUS:
		dwResult=BT_RegisterCallback(EVENT_SPPEX_CONNECTION_STATUS, CBK_SPPExConnection);
		PrintError("BT_RegisterCallback EVENT_SPPEX_CONNECTION_STATUS",dwResult);
		break;
	case EVENT_BLUETOOTH_STATUS:
		dwResult=BT_RegisterCallback(EVENT_BLUETOOTH_STATUS, CBK_BluetoothStatus);
		PrintError("BT_RegisterCallback EVENT_BLUETOOTH_STATUS",dwResult);
		break;
	case -1:
		break;
	default:
		break;
	}
}

void SDK_BtUnRegisterCallBack(int nInput)
{
	DWORD dwResult;
	switch(nInput)
	{
	case 0:
		for(nInput=1; nInput<=15; nInput++){
			dwResult=BT_UnregisterCallback(nInput);
			PrintError("BT_UnregisterCallback",dwResult);
		}
		break;
	case -1:
		break;
	default:
		dwResult=BT_UnregisterCallback(nInput);
		PrintError("BT_UnregisterCallback",dwResult);
		break;
	}
}

void CBK_Connection(WORD wServiceClass, BYTE* lpBdAddr, UCHAR ucStatus, DWORD dwConnetionHandle)
{
	/*switch(ucStatus)
	{
	case STATUS_INCOMING_CONNECT:
		printf("Incoming Connection Connect Indication!\n");
		break;
	case STATUS_INCOMING_DISCONNECT:
		printf("Incoming Connection Disconnect Indication!\n");
		break;
	case STATUS_OUTGOING_CONNECT:
		printf("Outgoing Connection Connect Indication!\n");
		break;
	case STATUS_OUTGOING_DISCONNECT:
		printf("Outgoing Connection Disconnect Indication!\n");
		break;
	}
	printf("Remote Address:	%02X:%02X:%02X:%02X:%02X:%02X\n",
		lpBdAddr[5],
		lpBdAddr[4],
		lpBdAddr[3],
		lpBdAddr[2], 
		lpBdAddr[1], 
		lpBdAddr[0]);
	printf("Service Class:	%d\n", wServiceClass);
	printf("Connection Handle:	%d\n", dwConnetionHandle);*/
	
	if(wServiceClass==CLS_SERIAL_PORT){
		
		//let wmxp bslhandler handle it...
		if(CBlueSoleilHandler::curInstance)
			CBlueSoleilHandler::curInstance->OnSPPConnectionStatus( -1, lpBdAddr, ucStatus, dwConnetionHandle);

		/*if(ucStatus == STATUS_INCOMING_CONNECT){
			BOOL bIsOutGoing;
			SPP_CONNECT_INFO sppConnInfo ={0};
			DWORD dwLen = sizeof(SPP_CONNECT_INFO);
			BYTE bdAddr[6]={0};
			WORD wClass;
			
			if(BT_GetConnectInfo(dwConnetionHandle,&bIsOutGoing,&wClass,bdAddr,&dwLen,(BYTE*)&sppConnInfo) == BTSTATUS_SUCCESS){
				printf("Com Port = %d\n",sppConnInfo.ucComPort);
				printf("Remote Address:	%02X:%02X:%02X:%02X:%02X:%02X\n",
					bdAddr[5],
					bdAddr[4],
					bdAddr[3],
					bdAddr[2], 
					bdAddr[1], 
					bdAddr[0]);
				
				//If incoming spp connected, get the COM port and start IO
				if(ucStatus == STATUS_INCOMING_CONNECT){
					//Start IO on the COM port
					int bDoServerIO = GetPrivateProfileInt("SPP_COM_PORT", "DO Server IO", 0,g_szIniPathDup);
					if(bDoServerIO)
						StartIoOnComport(dwConnetionHandle, sppConnInfo.ucComPort, FALSE);
				}
				
			}else
				printf("Fail to get com port\n",sppConnInfo.ucComPort);
		}
		else if(ucStatus == STATUS_INCOMING_DISCONNECT || ucStatus == STATUS_OUTGOING_DISCONNECT){
			//If spp disconnected, stop IO on the COM port
			StopIoOnComport(dwConnetionHandle);	
		}*/

	}
	
}

void CBK_DunRas(UCHAR ucStrLen, CHAR* szErrorString, DWORD dwError)
{
	printf("Error Code = %d : %s\n", dwError,szErrorString);
}

void CBK_SPPExConnection(DWORD dwServerHandle, BYTE* lpBdAddr, UCHAR ucStatus,DWORD dwConnetionHandle)
{
	if(CBlueSoleilHandler::curInstance)
		CBlueSoleilHandler::curInstance->OnSPPConnectionStatus( dwServerHandle, lpBdAddr, ucStatus, dwConnetionHandle);
	
}

void CBK_GeneralError(DWORD dwErrorCode)
{
	PrintCbkError(dwErrorCode);
	g_dwLastError = dwErrorCode;

	if(CBlueSoleilHandler::curInstance)
		CBlueSoleilHandler::curInstance->OnError(dwErrorCode);
}


void SDK_BtStartSPPExService()
{
	int nIndex=0;
	while(nIndex<MAX_SPPEX && g_sppexSvr[nIndex].dwHanlde != NULL)
		nIndex ++;
	if(nIndex == MAX_SPPEX){
		printf("No corresponding handle for CBK_SPPExConnection\n");
		return;
	}
	
	memset(& g_sppexSvr[nIndex].svcInfo, 0, sizeof(SPPEX_SERVICE_INFO));
	
	CHAR temp[256];
	g_sppexSvr[nIndex].svcInfo.dwSize = sizeof(SPPEX_SERVICE_INFO);
	if(GetPrivateProfileString("START_SPP_EX_SERVICE", "GUID", NULL, temp, MAX_PATH, g_szIniPathDup)==0)
	{
		printf("Get GUID Failed!\n");
		return;
	}
	if(strlen(temp)!=strlen("000050020000100080000002ee000001")){
		printf("Invalid GUID");
		return;
	}
	if(GetPrivateProfileString("START_SPP_EX_SERVICE", "Service Name", NULL, g_sppexSvr[nIndex].svcInfo.szServiceName, MAX_PATH, g_szIniPathDup)==0)
	{
		printf("Get service name Failed!\n");
		return;
	}
	
    //{00005002-0000-1000-8000-0002ee000001}
	BYTE Data4[8] = {0};
	
	sscanf(temp,"%08x",&(g_sppexSvr[nIndex].svcInfo.serviceClassUuid128.Data1));//0x00005002;
	sscanf(temp+8,"%04x",&(g_sppexSvr[nIndex].svcInfo.serviceClassUuid128.Data2));//0x0000;
	sscanf(temp+8+4,"%04x",&(g_sppexSvr[nIndex].svcInfo.serviceClassUuid128.Data3));//0x1000;
	for(int i=0;i<8;i++)
		sscanf(temp+8+4+4+i*2,"%02x",Data4+i);
	memcpy(g_sppexSvr[nIndex].svcInfo.serviceClassUuid128.Data4, Data4, 8);
	
	DWORD dwResult;
	dwResult = BT_StartSPPExService(&g_sppexSvr[nIndex].svcInfo, &g_sppexSvr[nIndex].dwHanlde);
	PrintError("BT_StartSPPExService",dwResult);
	if(dwResult == BTSTATUS_FAIL)
	{
		memset(& g_sppexSvr[nIndex], 0, sizeof(SPPEX_INFO));
		printf("Start SPPEx Service Failed!\n");
		return;
	}
	printf("SPPEx Service Info:\n");
	printf("SDAP Record Handle:	%d\n", g_sppexSvr[nIndex].svcInfo.dwSDAPRecordHanlde);
	printf("Service Name:	%s\n", g_sppexSvr[nIndex].svcInfo.szServiceName);
	printf("Service Channel:	%02X\n", g_sppexSvr[nIndex].svcInfo.ucServiceChannel);
	printf("COM Port:	%d\n", g_sppexSvr[nIndex].svcInfo.ucComIndex);
}

void SDK_BtStopSPPExService()
{
	int in = -1;
	DWORD dwResult;
	
	printf("All the SPPEx services on local device:\n");
	for(int i = 0; i < MAX_SPPEX; i++)
	{
		if(g_sppexSvr[i].dwHanlde != NULL)
		{
			printf("List NO: %d\n", i);
			printf("SPPEx server Handle:%d\n", g_sppexSvr[i].dwHanlde);
			printf("***********************************************\n");
			in = i;
		}
	}
	if(in == -1)
	{
		printf("There is no connection on this local device!\n");
		return;
	}
	do{
		printf("Please Input the List NO that you want to disconnect:(-1 to Cancel)");
		scanf("%d", &in);
		if(in < 0 || in > 7)
			continue;
		else
		{
			dwResult = BT_StopSPPExService(g_sppexSvr[in].dwHanlde);
			PrintError("BT_StopSPPExService",dwResult);
			memset(&g_sppexSvr[in],0,sizeof(SPPEX_INFO));
		}
	}while(in != -1);
	
}

int SDK_BtSearchSPPExServices(BYTE* g_targetBdAddr, BYTE* guid /*128-bit guid - 16 bytes long*/, SPPEX_SERVICE_INFO& ret /*OUT: valid only if function returns > 0*/) //returns -1 if not found else returns service port
{
	BLUETOOTH_DEVICE_INFO device;
	device.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	memcpy(device.address, g_targetBdAddr, 6);
	
	SPPEX_SERVICE_INFO	sppex_svc_info[5];
	memset(&sppex_svc_info, 0, 5*sizeof(SPPEX_SERVICE_INFO));	
		

	memcpy((BYTE*)(&(sppex_svc_info[0].serviceClassUuid128)), guid, 16); //copy the 128 bit GUID

	
	//CHAR temp[256];
	sppex_svc_info[0].dwSize = sizeof(SPPEX_SERVICE_INFO);
	
	/*if(GetPrivateProfileString("SEARCH_SPP_EX_SERVICE", "GUID", NULL, temp, MAX_PATH, g_szIniPathDup)==0)
	{
		printf("Get GUID Failed!\n");
		return;
	}
	if(strlen(temp)!=strlen("000050020000100080000002ee000001")){
		printf("Invalid GUID");
		return;
	}
	
    //{00005002-0000-1000-8000-0002ee000001}
	BYTE Data4[8] = {0};
	
	sscanf(temp,"%08x",&(sppex_svc_info[0].serviceClassUuid128.Data1));//0x00005002;
	sscanf(temp+8,"%04x",&(sppex_svc_info[0].serviceClassUuid128.Data2));//0x0000;
	sscanf(temp+8+4,"%04x",&(sppex_svc_info[0].serviceClassUuid128.Data3));//0x1000;
	for(DWORD i=0;i<8;i++)
		sscanf(temp+8+4+4+i*2,"%02x",Data4+i);
	memcpy(sppex_svc_info[0].serviceClassUuid128.Data4, Data4, 8);
	*/

	DWORD dwResult;

	DWORD dwLength = 5*sizeof(SPPEX_SERVICE_INFO);

	dwResult = BT_SearchSPPExServices(&device,&dwLength,sppex_svc_info);
	PrintError("BT_SearchSPPExServices",dwResult);
	if(dwResult != BTSTATUS_SUCCESS)
	{
		return -1;
	}
	
	for(int i=0;i<dwLength/sizeof(SPPEX_SERVICE_INFO);i++){
		//printf("SPPEx Service Info No.%d\n",i);
		//printf("SDAP Record Handle:	%d\n", sppex_svc_info[i].dwSDAPRecordHanlde);
		//printf("Service Name:	%s\n", sppex_svc_info[i].szServiceName);
		//printf("Service Channel:	%02X\n", sppex_svc_info[i].ucServiceChannel);
		//printf("COM Port:	%d\n", sppex_svc_info[i].ucComIndex);

		if(sppex_svc_info[i].ucServiceChannel>0)
		{
			memcpy(&ret,&(sppex_svc_info[i]),sizeof(SPPEX_SERVICE_INFO));
			return sppex_svc_info[i].ucServiceChannel;
		}

	}

	return -1;
}

void SDK_BtConnectSPPExService(BYTE* g_targetBdAddr)
{
	BLUETOOTH_DEVICE_INFO device;
	device.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	memcpy(device.address, g_targetBdAddr, 6);
	
	SPPEX_SERVICE_INFO	sppex_svc_info;
	memset(&sppex_svc_info, 0, sizeof(SPPEX_SERVICE_INFO));
	
	CHAR temp[256];
	sppex_svc_info.dwSize = sizeof(SPPEX_SERVICE_INFO);
	if(GetPrivateProfileString("CONNECT_SPP_EX_SERVICE", "GUID", NULL, temp, MAX_PATH, g_szIniPathDup)==0)
	{
		printf("Get GUID Failed!\n");
		return;
	}
	if(strlen(temp)!=strlen("000050020000100080000002ee000001")){
		printf("Invalid GUID");
		return;
	}
	
    //{00005002-0000-1000-8000-0002ee000001}
	BYTE Data4[8] = {0};
	
	sscanf(temp,"%08x",&(sppex_svc_info.serviceClassUuid128.Data1));//0x00005002;
	sscanf(temp+8,"%04x",&(sppex_svc_info.serviceClassUuid128.Data2));//0x0000;
	sscanf(temp+8+4,"%04x",&(sppex_svc_info.serviceClassUuid128.Data3));//0x1000;
	for(int i=0;i<8;i++)
		sscanf(temp+8+4+4+i*2,"%02x",Data4+i);
	memcpy(sppex_svc_info.serviceClassUuid128.Data4, Data4, 8);
	
	DWORD dwResult, dwHandle;
	dwResult = BT_ConnectSPPExService(&device,&sppex_svc_info,&dwHandle);
	PrintError("BT_SearchSPPExServices",dwResult);
	
	if(dwResult == BTSTATUS_SUCCESS)
	{
		printf("SPPEx Service Info No.%d\n",i);
		printf("SDAP Record Handle:	%d\n", sppex_svc_info.dwSDAPRecordHanlde);
		printf("Service Name:	%s\n", sppex_svc_info.szServiceName);
		printf("Service Channel:	%02X\n", sppex_svc_info.ucServiceChannel);
		printf("COM Port:	%d\n", sppex_svc_info.ucComIndex);
		
		//Start IO on the COM port
		int bDClientIO = GetPrivateProfileInt("SPP_COM_PORT", "DO Client IO", 0,g_szIniPathDup);
		if(bDClientIO)
			StartIoOnComport(dwHandle,sppex_svc_info.ucComIndex, TRUE);
		
	}
	
}

void SDK_BtDisConnectSPPExService()
{
	SDK_EnumConnections();
	
	printf("Please input the number of the connection. (-1 to return)\n");
	
	int nInput;
	scanf("%d",&nInput);
	if(nInput == -1 )
		return;
	
	if(nInput > MAX_CONNECT_COUNT || nInput<0 || g_connList[nInput].dwHandle ==0){
		printf("Invalid input.\n");
		return;
	}
	
	DWORD dwResult = BT_DisconnectSPPExService(g_connList[nInput].dwHandle);
	PrintError("BT_DisconnectService",dwResult);
	
	if(dwResult == BTSTATUS_SUCCESS){
		memset(&g_connList[nInput], 0 , sizeof(GENERAL_CONNECT_INFO));
	}
}

typedef struct _DEV_LIST{
	BLUETOOTH_DEVICE_INFO dev;
	_DEV_LIST *pNext;
}DEV_LIST;
DEV_LIST *g_pDevList=NULL;
void CBK_InquiryDeviceReport(/*in*/PBLUETOOTH_DEVICE_INFO pDevInfo)
{
	BYTE zeroBuff[32]={0};
	
	DEV_LIST* pNode;
	
	printf("Inquiry Ind:\n");
	
    if( memcmp(pDevInfo->address,zeroBuff,DEVICE_ADDRESS_LENGTH) == 0 &&
		memcmp(pDevInfo->classOfDevice,zeroBuff,DEVICE_CLASS_LENGTH) == 0)
	{
        //This is the end of the inquiry
		printf("Inquiry completes. BlueSoleil might now continue to query device names\n\n");
		while(g_pDevList){
			pNode = g_pDevList;
			g_pDevList = g_pDevList->pNext;
			free(pNode);
		}
		g_pDevList = NULL;
		return;
	}
	pNode = g_pDevList;
	while(pNode){
		if(memcmp(pNode->dev.address,pDevInfo->address,DEVICE_ADDRESS_LENGTH)==0){
			printf("duplicated dev report.\n");
			return;
		}
		pNode = pNode->pNext;
	}
	pNode = (DEV_LIST*)malloc(sizeof(DEV_LIST));
	memcpy(&(pNode->dev),pDevInfo,sizeof(BLUETOOTH_DEVICE_INFO));
	pNode->pNext = g_pDevList;
	g_pDevList = pNode;
	
	
	printf("Address:%02X:%02X:%02X:%02X:%02X:%02X\n",
		pDevInfo->address[5],
		pDevInfo->address[4],
		pDevInfo->address[3],
		pDevInfo->address[2],
		pDevInfo->address[1],
		pDevInfo->address[0]);
	
	//If get device name afresh
	if(GetPrivateProfileInt("BT_INQUIRY_DEVICE", "Get Fresh Name On Inquiry", 1, g_szIniPathDup) == 1){
		DWORD dwMask = 0;
		BLUETOOTH_DEVICE_INFO_EX devInfo = {0};
		memcpy(&devInfo.address, pDevInfo->address, DEVICE_ADDRESS_LENGTH);
		devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_EX);
		devInfo.szName[0]='\0';
		dwMask = MASK_DEVICE_NAME;
		
		DWORD dwResult;
		dwResult = BT_SetRemoteDeviceInfo(dwMask, &devInfo);
		PrintError("BT_SetRemoteDeviceInfo",dwResult);
		dwResult = BT_GetRemoteDeviceInfo(dwMask, &devInfo);
		PrintError("BT_GetRemoteDeviceInfo",dwResult);
		
		printf("Name:%s \n", devInfo.szName);	
	}
	else
		printf("Name:%s \n", pDevInfo->szName);	
	
	
    printf("Class:%02x:%02x:%02x ---- \n",
		pDevInfo->classOfDevice[2],
		pDevInfo->classOfDevice[1],
		pDevInfo->classOfDevice[0]);
	
	PrintDeviceClassInfo(pDevInfo->classOfDevice);
	
	printf("Pair Status: %d \n\n",pDevInfo->bPaired);
	
}

void SDK_StartBluetooth()
{
	DWORD dwResult;
	dwResult = BT_StartBluetooth();
	PrintError("SDK_StartBluetooth",dwResult);
}
void SDK_StopBluetooth()
{
	DWORD dwResult;
	dwResult = BT_StopBluetooth(TRUE);
	PrintError("BT_StopBluetooth",dwResult);
}
void SDK_CancelInquiry()
{
	DWORD dwResult;
	dwResult = BT_CancelInquiry();
	PrintError("BT_CancelInquiry",dwResult);
}

void CBK_BluetoothStatus(UCHAR ucStatus)
{
	if(ucStatus == STATUS_BLUETOOTH_STARTED)
		printf("Bluetooth is started\n");
	else if(ucStatus == STATUS_BLUETOOTH_STOPED){
		printf("Bluetooth is stoped\n");
	}
	else
		printf("Invalid parameter in Bluetooth ind\n");
	
}

void SDK_EnumConnections()
{
	
	DWORD dwSize = MAX_CONNECT_COUNT*sizeof(GENERAL_CONNECT_INFO);
	memset(g_connList, 0, dwSize);
	
	DWORD dwResult = BT_EnumConnections(&dwSize,g_connList);
	PrintError("BT_EnumConnections",dwResult);
	
	for(DWORD i = 0; i < (dwSize/sizeof(GENERAL_CONNECT_INFO)); i++){
		printf("#%d handle=%d bIsOut=%d Service ID=0x%x bd=%02X:%02X:%02X:%02X:%02X:%02X\n",
			i,g_connList[i].dwHandle,g_connList[i].bIsOutgoing,g_connList[i].wServiceClass,
			g_connList[i].remoteBdAddr[5],
			g_connList[i].remoteBdAddr[4],
			g_connList[i].remoteBdAddr[3],
			g_connList[i].remoteBdAddr[2],
			g_connList[i].remoteBdAddr[1],
			g_connList[i].remoteBdAddr[0]);
	}
}


void PostRobustConnect(DWORD dwHanlde, WORD wSvcId, BYTE* pParam)
{
	switch(wSvcId)
	{
	case CLS_SERIAL_PORT:
	case CLS_DIALUP_NET:
		{
			if(pParam){
				BOOL bIsOutGoing;
				SPP_CONNECT_INFO sppConnInfo ={0};
				DWORD dwLen = sizeof(SPP_CONNECT_INFO);
				BYTE bdAddr[6]={0};
				WORD wClass;
				
				if(BT_GetConnectInfo(dwHanlde,&bIsOutGoing,&wClass,bdAddr,&dwLen,(BYTE*)&sppConnInfo) == BTSTATUS_SUCCESS){
					printf("Com Port = %d\n",sppConnInfo.ucComPort);
					printf("Remote Address:	%02X:%02X:%02X:%02X:%02X:%02X\n",
						bdAddr[5],
						bdAddr[4],
						bdAddr[3],
						bdAddr[2], 
						bdAddr[1], 
						bdAddr[0]);
				}
			}
		}
		break;
	default:;
	}
}
void TS_ConnectRobustness(BYTE* g_targetBdAddr)
{
	int i;
	WORD wSvcCls = GetPrivateProfileInt("CONNECT_ROBUSTNESS_TEST","Service ID", 0, g_szIniPathDup);
	int nGapConn2Disc = GetPrivateProfileInt("CONNECT_ROBUSTNESS_TEST", "Gap between Connect and Disconnect", 5000, g_szIniPathDup);
	int nGapDisc2Conn = GetPrivateProfileInt("CONNECT_ROBUSTNESS_TEST", "Gap between Disconnect and Connect", 4000, g_szIniPathDup);
	int bStopFlag;
	int bLog = GetPrivateProfileInt("CONNECT_ROBUSTNESS_TEST", "Log Enabled", 0, g_szIniPathDup);
	int bDoPostConnect = GetPrivateProfileInt("CONNECT_ROBUSTNESS_TEST", "Do Post Connect", 0, g_szIniPathDup);
	
	
	BLUETOOTH_DEVICE_INFO devInfo={0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	memcpy(devInfo.address, g_targetBdAddr, 6);
	
	GENERAL_SERVICE_INFO svcInfo = {0};
	svcInfo.dwSize = sizeof(GENERAL_SERVICE_INFO);
	svcInfo.wServiceClassUuid16 = wSvcCls;
	
	BYTE abyConnParam[512];
	DWORD dwParamLen = 512;
	
	if(!ConfigConnectParams(wSvcCls,abyConnParam,&dwParamLen)){
		printf("Failed to config parameters.\n");
		return;
	}
	
	DWORD dwResult;
	DWORD dwHandle;
	for(i=0;;i++){
		
		g_dwLastError = 0;
		printf("#%d Connect attempt started.\n", i);
//		if(bLog)
//			WriteLog("#%d Connect attempt started.", i);
		dwResult = BT_ConnectService(&devInfo, &svcInfo,dwParamLen==0 ? NULL : abyConnParam, &dwHandle);
		PrintError("BT_ConnectService",dwResult);
		
		if(dwResult == BTSTATUS_SUCCESS){
			//if(bLog)
			//	WriteLog("#%d Connect succeeded.",i);
			
			PostRobustConnect(dwHandle, wSvcCls, dwParamLen==0 ? NULL : abyConnParam);
			
			if(nGapConn2Disc>0)
				Sleep(nGapConn2Disc);
			
			
			g_dwLastError = 0;
			printf("#%d Disconnect attempt started\n",i);
//			if(bLog)
//				WriteLog("#%d Disconnect attempt started.",i); 
			dwResult = BT_DisconnectService(dwHandle);
			PrintError("DisconnectService",dwResult);
			if(dwResult == BTSTATUS_SUCCESS){
//				if(bLog)
//					WriteLog("#%d Disconnect succeeded.",i);
			}
			else{
				//if(bLog)
				//	WriteLog("Disconnect failed! return:%d, last error:0x%x\n",dwResult,g_dwLastError); 
			}
			
			if(nGapDisc2Conn>0)
				Sleep(nGapDisc2Conn);
		}
		else{
			//if(bLog)
			//	WriteLog("%d Connect failed. Return:%d, Last Error:0x%x.\n",dwResult,g_dwLastError); 
		}
		
		bStopFlag = GetPrivateProfileInt("CONNECT_ROBUSTNESS_TEST", "Stop Flag", 0, g_szIniPathDup);
		if(bStopFlag)
			break;
		
	}
	
}


void TS_PanIpAllocation(BYTE* g_targetBdAddr)
{
	
	CConnectCheck pingChecker;
	
	int bStopFlag;
	
	DWORD dwConnHandle;
	
	BLUETOOTH_DEVICE_INFO devInfo={0};
	devInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	memcpy(devInfo.address, g_targetBdAddr, 6);
	GENERAL_SERVICE_INFO svcInfo = {0};
	svcInfo.dwSize = sizeof(GENERAL_SERVICE_INFO);
	svcInfo.wServiceClassUuid16 = CLS_PAN_GN;
		
	DWORD dwTotalTime = 0, dwErr;
	int nTotal = 0;
	DWORD dwLastTick;
	
	for(int i = 0;; i++)
	{
//		WriteLog("------------------------------------",i);
//		WriteLog("#%d BT_ConnectService attempt started...",i);
		dwLastTick = GetTickCount();
		dwErr = BT_ConnectService(
			&devInfo,
			&svcInfo,
			NULL,
			&dwConnHandle);		
		if(dwErr != BTSTATUS_SUCCESS){
//			WriteLog("#%d BT_ConnectService attempt failed.",i);
		}
		else
		{
//			WriteLog("#%d BT_ConnectService attempt succeeded.",i);
			Sleep(5000);
			if(0)/*Fail to get IP*/
			{
				//WriteLog("Failed to recv a valid ip address or did not recv a change in IP address notification.");
			}
			else
			{
				// Recvd IP address increase the total time and disconnect PAN connection
				DWORD dwTimeTaken = GetTickCount() - dwLastTick;
				dwTotalTime += dwTimeTaken;
				nTotal ++;
				
				//WriteLog("PAN IP received. time taken=%d milliseconds",dwTimeTaken);
				
				//Ping
				int nDoPing;
				char szTargetIP[64];
				nDoPing = GetPrivateProfileInt("CONNECT_ROBUSTNESS_TEST", "Do Ping", 1, g_szIniPathDup);
				if(nDoPing == 1){
					if(GetPrivateProfileString("CONNECT_ROBUSTNESS_TEST","Do Ping Target","",szTargetIP,63,g_szIniPathDup)>0){
						
						if(pingChecker.Ping(NULL,szTargetIP,0))
						{
						//	WriteLog("Ping %s succeeded.",szTargetIP);
						}
						else
						{
						//	WriteLog("Ping %s failed.",szTargetIP);
						}
					}
				}
				
				// disconnect PAN
				//WriteLog("#%d BT_DisconnectService attempt started.",i);
				DWORD dwErr = BT_DisconnectService(dwConnHandle);
				if(dwErr != BTSTATUS_SUCCESS)
				{
					//WriteLog("#%d BT_DisconnectService attempt failed.",i);
					//WriteLog(" Failed to disconnect from PAN Service...step through put a break point here");
				}
				else
				{
					//WriteLog("#%d BT_DisconnectService attempt succeeded.",i);
					dwConnHandle = 0;
				}
			}
		}
		
		bStopFlag = GetPrivateProfileInt("CONNECT_ROBUSTNESS_TEST", "Stop Flag", 0, g_szIniPathDup);
		if(bStopFlag)
			break;
		
	}
	
	
	//WriteLog("Total %d attempts, %d succeed. ", i+1, nTotal);
	
}
