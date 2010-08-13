/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.

*******************************************************************************************/

//modified for WMouseXP by Kasidit Yusuf

#include "stdafx.h"
//#include <stdio.h>
#include "HelpFunc.h"
#include "bt_def.h"
//#include "main.h"


char g_szIniPath[MAX_PATH] = "BlueSoleilClient.ini";
///////////////////////////////////////////UI HELP/////////////////////////////////////////////////////
DWORD Help_InputAddr(char temp_addr[], UCHAR bdAddr[6])
{
	char cmp_temp[2]={0};
	UCHAR buffer[6] = {0};
	
	for(int i = 1; i < 13; i++)
	{
		if((temp_addr[i-1]>='0' && temp_addr[i-1]<='9') || 
			(temp_addr[i-1]>='A' && temp_addr[i-1] <= 'F') ||
			(temp_addr[i-1]>='a' && temp_addr[i-1] <= 'f'))
		{
			if(i%2 == 1)
			{
				if(temp_addr[i-1]>='0' && temp_addr[i-1]<='9')
					cmp_temp[0] = temp_addr[i-1] - '0';
				else if(temp_addr[i-1]>='A' && temp_addr[i-1]<='F')
					cmp_temp[0] = temp_addr[i-1] - 'A' + 10;
				else if(temp_addr[i-1]>='a' && temp_addr[i-1]<='f')
					cmp_temp[0] = temp_addr[i-1] - 'a' + 10;
			}
			else if(i%2 == 0)
			{
				if(temp_addr[i-1]>='0' && temp_addr[i-1]<='9')
					cmp_temp[1] = temp_addr[i-1] - '0';
				else if(temp_addr[i-1]>='A' && temp_addr[i-1]<='F')
					cmp_temp[1] = temp_addr[i-1] - 'A' + 10;
				else if(temp_addr[i-1]>='a' && temp_addr[i-1]<='f')
					cmp_temp[1] = temp_addr[i-1] - 'a' + 10;
				buffer[6- i/2] = cmp_temp[0]*16 + cmp_temp[1];
			}
		}
		else
		{
			printf("Invalid Address, Please input again!\n");
			return BTSTATUS_FAIL;
		}
	}
	memcpy(bdAddr, buffer, 6);

	return BTSTATUS_SUCCESS;
}

DWORD Help_InputDeviceClass(char temp[], UCHAR device_class[])
{
	char* tokenPtr;
	int i=0;

	tokenPtr = strtok(temp, " ");
	while(tokenPtr!=NULL && i < 3)
	{
		if(((*tokenPtr)-'0') > 10)
		{
			memcpy(device_class, 0, DEVICE_CLASS_LENGTH);
			return BTSTATUS_FAIL;
		}
		device_class[i] = ((*tokenPtr)-'0')*16 + ((*(tokenPtr+1))-'0');
		i++;
		tokenPtr = strtok(NULL, " ");
	}
	return BTSTATUS_SUCCESS;
}

BOOL ConfigConnectDunParam(BYTE* pParam, DWORD* pParamLen)
{
	PDUN_CLIENT_PARAM pDunParam;
	char szTmp[MAX_PATH]={0};
	BOOL bBtConnectOnly =  FALSE;

	pDunParam = (PDUN_CLIENT_PARAM)pParam;
	memset(pDunParam, 0, sizeof(DUN_CLIENT_PARAM));
	pDunParam->dwSize = sizeof(DUN_CLIENT_PARAM);

	//Get bAutoDial
	pDunParam->bAutoDial = GetPrivateProfileInt("CONNECT_DUN_PARAMS", "Auto Dial", 0, g_szIniPath);
	//Get szUserName
	GetPrivateProfileString("CONNECT_DUN_PARAMS", "User Name", "", pDunParam->szUserName, MAX_PATH, g_szIniPath);
	//Get password
	GetPrivateProfileString("CONNECT_DUN_PARAMS", "Password", "", pDunParam->password, MAX_PATH, g_szIniPath);
	//Get dialNumber
	GetPrivateProfileString("CONNECT_DUN_PARAMS", "Dial Number", "", pDunParam->dialNumber, MAX_PATH, g_szIniPath);

	//No dial number set, make Bluetooth connection only
	if(strlen(pDunParam->dialNumber) ==0 )
		bBtConnectOnly = TRUE;

	if(bBtConnectOnly && pParamLen)
		*pParamLen = 0;
	else
		*pParamLen = sizeof(DUN_CLIENT_PARAM);
	
	return TRUE;
		
}

BOOL ConfigConnectOppParam(BYTE* pParam, DWORD* pParamLen)
{
	POPP_CLIENT_PARAM pOppParam;
	pOppParam = (POPP_CLIENT_PARAM)pParam;
	memset(pOppParam, 0, sizeof(OPP_CLIENT_PARAM));
	pOppParam->dwSize = sizeof(OPP_CLIENT_PARAM);

	int nTmp;
	nTmp = GetPrivateProfileInt("CONNECT_OPP_PARAMS", "PUSH",0, g_szIniPath);
	if(nTmp)
		pOppParam->wCmdFlags |= OPP_COMMAND_PUSH;
	
	nTmp = GetPrivateProfileInt("CONNECT_OPP_PARAMS", "PULL",0, g_szIniPath);
	if(nTmp)
		pOppParam->wCmdFlags |= OPP_COMMAND_PULL;

	GetPrivateProfileString("CONNECT_OPP_PARAMS", "Push Object", "",pOppParam->szObjectPath , MAX_PATH, g_szIniPath);

	if(pParamLen)
		*pParamLen = sizeof(OPP_CLIENT_PARAM);

	return TRUE;
}


BOOL ConfigConnectSyncParam(BYTE* pParam, DWORD* pParamLen)
{
	PSYNC_CLIENT_PARAM pSyncParam;
	pSyncParam = (PSYNC_CLIENT_PARAM)pParam;
	memset(pSyncParam, 0, sizeof(SYNC_CLIENT_PARAM));
	pSyncParam->dwSize = sizeof(SYNC_CLIENT_PARAM);

	pSyncParam->bShowSyncDlg = GetPrivateProfileInt("CONNECT_SYNC_PARAMS", "Show Dialog",0, g_szIniPath);
	pSyncParam->ucSyncType = GetPrivateProfileInt("CONNECT_SYNC_PARAMS", "Sync Type ",1, g_szIniPath);

	if(pParamLen)
		*pParamLen = sizeof(SYNC_CLIENT_PARAM);
	
	return TRUE;
}


BOOL ConfigConnectSppParam(BYTE* pParam, DWORD* pParamLen)
{
	PSPP_CLIENT_PARAM pSppParam = (PSPP_CLIENT_PARAM)pParam;
	memset(pSppParam, 0, sizeof(SPP_CLIENT_PARAM));
	pSppParam->dwSize = sizeof(SPP_CLIENT_PARAM);

	if(pParamLen)
		*pParamLen = sizeof(SPP_CLIENT_PARAM);
	
	return TRUE;
	
}

BOOL ConfigConnectParams(WORD wSvcCls, BYTE* pParam, DWORD* pParamLen)
{
	BOOL bRet;
	switch(wSvcCls)
	{
	case CLS_DIALUP_NET:
		bRet = ConfigConnectDunParam(pParam, pParamLen);
		break;
	case CLS_SERIAL_PORT:
		bRet = ConfigConnectSppParam(pParam, pParamLen);
		break;
	case CLS_IRMC_SYNC:
		bRet = ConfigConnectSyncParam(pParam, pParamLen);
		break;
	case CLS_OBEX_OBJ_PUSH:
		bRet = ConfigConnectOppParam(pParam, pParamLen);
		break;
	default:
		if(pParamLen)
			*pParamLen = 0;
		bRet = TRUE;
		break;
	}

	return bRet;
}

void PrintError(CHAR* lpszAPI,DWORD dwError)
{
	printf(lpszAPI);
	printf(" returned %d: ",dwError);
	switch(dwError){
	case BTSTATUS_SUCCESS:
		printf("Succeeded.\n");
		break;
	case BTSTATUS_FAIL:
		printf("General failed!\n");
		break;
	case BTSTATUS_SYSTEM_ERROR:
		printf("System error occured!\n");
		break;
	case BTSTATUS_BT_NOT_READY:
		printf("Bluetooth is not ready.\n");
		break;
	case BTSTATUS_ALREADY_PAIRED:
		printf("BlueSoleil is already paired with the device.\n");
		break;
	case BTSTATUS_AUTHENTICATE_FAILED:
		printf("Authentication failed!\n");
		break;
	case BTSTATUS_BT_BUSY:
		printf("Bluetooth is busy with browsing services or connecting to a device.\n");
		break;
	case BTSTATUS_CONNECTION_EXIST:
		printf("The connection to the service is already established.\n");
		break;
	case BTSTATUS_CONNECTION_NOT_EXIST:
		printf("The connection does not exist or is released.\n");
		break;
	case BTSTATUS_PARAMETER_ERROR:
		printf("Parameter error!\n");
		break;
	case BTSTATUS_SERVICE_NOT_EXIST:
		printf("Service does not exist.\n");
        break; 
	default:
		printf("Unknown error.\n");
	}
}

typedef struct _CBK_ERROR{
	DWORD dwError;
	CHAR *lpszDesc;
}CBK_ERROR;

CBK_ERROR g_cbkError[]=
{
	{0x01,	"Unknown HCI Command."},
	{0x02,	"No Connection."},
	{0x03,	"Hardware Failure."},
	{0x04,	"Page Timeout."},
	{0x05,	"Authentication Failure."},
	{0x06,	"Key Missing."},
	{0x07,	"Memory Full."},
	{0x08,	"Connection Timeout."},
	{0x09,	"Max Number Of Connections."},
	{0x0A,	"Max Number Of SCO Connections To A Device."},
	{0x0B,	"ACL connection Already exists."},
	{0x0C,	"Command Disallowed."},
	{0x0D,	"Host Rejected Due to Limited resources."},
	{0x0E,	"Host Rejected Due to Security reasons."},
	{0x0F,	"Host Rejected Due to Remote device Is only A personal device."},
	{0x10,	"Host Timeout."},
	{0x11,	"Unsupported Feature or Parameter Value."},
	{0x12,	"Invalid HCI Command Parameters."},
	{0x13,	"Other End Terminated Connection: User Ended Connection."},
	{0x14,	"Other End Terminated Connection : Low Resources."},
	{0x15,	"Other End Terminated Connection: About to Power Off."},
	{0x16,	"Connection Terminated by Local Host."},
	{0x17,	"Repeated Attempts."},
	{0x18,	"Pairing Not Allowed."},
	{0x19,	"Unknown LMP PDU."},
	{0x1A,	"Unsupported Remote Feature."},
	{0x1B,	"SCO Offset Rejected."},
	{0x1C,	"SCO Interval Rejected."},
	{0x1D,	"SCO Air Mode Rejected."},
	{0x1E,	"Invalid LMP Parameters."},
	{0x1F,	"Unspecified Error."},
	{0x20,	"Unsupported LMP Parameter Value."},
	{0x21,	"Role Change Not Allowed."},
	{0x22,	"LMP Response Timeout."},
	{0x23,	"LMP Error Transaction Collision."},
	{0x24,	"LMP PDU Not Allowed."},
	{0x25,	"Encryption Mode Not Acceptable."},
	{0x26,	"Unit Key Used"},
	{0x27,	"QoS Is Not Supported"},
	{0x28,	"Instant Passed"},
	{0x29,	"Pairing with Unit Key Not Supported"},
};
	
#define MAX_CBK_ERROR sizeof(g_cbkError)/sizeof(CBK_ERROR)

void PrintCbkError(DWORD dwError)
{
	for(int i=0; i<MAX_CBK_ERROR; i++){
		if(g_cbkError[i].dwError == dwError)
			break;
	}
	printf("Error Code: 0x%x\t%s\n", dwError, i<MAX_CBK_ERROR ? g_cbkError[i].lpszDesc : "Unknown");			
}

