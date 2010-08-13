/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.

*******************************************************************************************/


//modified for WMouseXP by Kasidit Yusuf

#include "stdafx.h"

#include "log.h"

#include "ComPort.h"
#include <stdio.h>
#include <bt_ui.h>
#include "BlueSoleilHandler.h"
#include "WMouseXP.h"
#include "WMouseXPDlg.h"

#define MAX_IO_THREAD 4


typedef struct _COM_IO{
	DWORD dwConnHandle;
	int nComPort;
	HANDLE hThread;
	BOOL bStopFlag;
}COM_IO;

COM_IO g_comIoList[MAX_IO_THREAD]={0};

HANDLE OpenPort(UCHAR nPort)
{
	HANDLE handle;
	char  buf[64];
	DWORD nread = 64;

	sprintf(buf,"\\\\.\\COM%d",nPort);
	// open the com port.
	handle = CreateFile(buf, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (handle == INVALID_HANDLE_VALUE){
		printf("Failed to open COM port %d, last error=%d\n",nPort,GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	DCB dcb;
	COMMTIMEOUTS commTimeouts;
	
	GetCommState((HANDLE)handle,&dcb);
	dcb.BaudRate = CBR_115200;
	dcb.Parity = NOPARITY;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	SetCommState((HANDLE)handle,&dcb);

	commTimeouts.ReadIntervalTimeout = MAXDWORD;
	commTimeouts.ReadTotalTimeoutConstant = 0;
	commTimeouts.ReadTotalTimeoutMultiplier = 0;
	commTimeouts.WriteTotalTimeoutConstant = 100;
	commTimeouts.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts((HANDLE)handle,&commTimeouts);
	PurgeComm((HANDLE)handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
	SetupComm((HANDLE)handle, 1024, 1024);
	EscapeCommFunction((HANDLE)handle,SETRTS);
	EscapeCommFunction((HANDLE)handle,SETDTR);

	return handle;
}
BOOL CheckActive(HANDLE hPort)
{
	//we're not connecting to a AT command handler, we're connecting to WMouseXP!
	
	return TRUE;



	// Write AT command to Modem.
	OVERLAPPED writeo, reado;	
	DWORD dwErrors;
	DWORD byte_sent;
	BOOL   ret = TRUE;
	BOOL  bWaitingOnWrite = TRUE;
	COMSTAT comStat;  
	DWORD nwritten,nread;
	CHAR buf[256]={0};
	nwritten = 0;

	memset(&writeo,0,sizeof(OVERLAPPED));
	writeo.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	memset(&reado,0,sizeof(OVERLAPPED));
	reado.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if ( !WriteFile(hPort, "AT\r", 3, &byte_sent, &writeo)){
		dwErrors=GetLastError();
		if ( dwErrors == ERROR_IO_PENDING) { 
			while(!GetOverlappedResult((HANDLE)hPort, &writeo, &byte_sent, TRUE)){
				if ( GetLastError() == ERROR_IO_INCOMPLETE){
              		nwritten += byte_sent;
					continue;
				}
				else{
					ClearCommError((HANDLE)hPort, &dwErrors, &comStat); 
					break;
				}
           	}
			nwritten += byte_sent;			
		}
		else{ 
			ClearCommError((HANDLE)hPort, &dwErrors, &comStat);
		} 
	}
	else{
		nwritten += byte_sent;
	}

	if ( nwritten < 3 ){
		CloseHandle(hPort);
		ret=FALSE;
		goto check_exit;
	}

	printf("Sent data to com port:AT\\r\n");

	// Read Response from Modem.
	DWORD nreturned;
	Sleep(500);
	nread=9;
	if(!ReadFile((HANDLE)hPort, buf, nread, &nreturned, &reado) ){
		dwErrors=GetLastError();
		if ( dwErrors== ERROR_IO_PENDING) { 
			DWORD dwRes;
			dwRes = WaitForSingleObject(reado.hEvent, 2000);
			 switch(dwRes)
			 {
				 case WAIT_OBJECT_0: 
					 GetOverlappedResult((HANDLE)hPort, &reado, &nreturned, FALSE);
					 break;
				 case WAIT_TIMEOUT:
			 		break;
				 default:					 
					break;
			 }
		}else{ 
			ClearCommError( (HANDLE)hPort, &dwErrors, &comStat); 
		}
	}
	else{
		DWORD error = GetLastError();
	}

	if((nreturned) && 
		((!strncmp((LPCTSTR)buf,"\r\nOK\r\n",6))) ||
		((!strncmp((LPCTSTR)buf,"AT\r\r\nOK\r\n",9))))
	{
		ret = TRUE;
	}
	else
		ret = FALSE;

	PurgeComm(hPort,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	
	printf("Received data: %s\n",buf);

check_exit:
	if(writeo.hEvent){
		CloseHandle(writeo.hEvent);
	}	
	if(reado.hEvent){
		CloseHandle(reado.hEvent);
	}
	return ret;
}

OVERLAPPED /*writeo,*/ reado;

int HandleComPortData(HANDLE hComPort)
{
	DWORD nreturned,nread;
	//OVERLAPPED /*writeo,*/ reado;	
	DWORD dwErrors;
	DWORD byte_sent;
	BOOL  bWaitingOnWrite = TRUE;
	COMSTAT comStat;  
	DWORD nwritten;
	UCHAR buf[256]={0};

	

	if(hComPort==INVALID_HANDLE_VALUE){
		return -1;
	}

	//memset(&writeo,0,sizeof(OVERLAPPED));
	//writeo.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	memset(&reado,0,sizeof(OVERLAPPED));
	reado.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	nread=2;

	
	

	if(!ReadFile(hComPort, buf, nread, &nreturned, &reado) ){
		
	

		if (GetLastError() == ERROR_IO_PENDING) { 

	

			DWORD dwRes;
			dwRes = WaitForSingleObject(reado.hEvent, INFINITE);
			switch(dwRes)
			{
			case WAIT_OBJECT_0: 
				GetOverlappedResult(hComPort, &reado, &nreturned, FALSE);
				break;
			default:
				goto HandleComPortData_Exit;
			}
	
		}
		else{ 
	
			ClearCommError( hComPort, &dwErrors, &comStat); 
	
		}
	}

	

	//printf("SPPEX Server Received data: %s\n",buf);

	
	if(CWMouseXPDlg::curInstance)
		CWMouseXPDlg::curInstance->OnData(buf,nreturned);


	/*
	
	if(nreturned&&strncmp((LPCTSTR)buf,"AT\r",3)==0){
		nwritten = 0;
		if ( !WriteFile(hComPort,"\r\nOK\r\n", 6, &byte_sent, &writeo)){
			if ( GetLastError() == ERROR_IO_PENDING) { 
				while(!GetOverlappedResult(hComPort, &writeo, &byte_sent, TRUE)){
					if ( GetLastError() == ERROR_IO_INCOMPLETE){
						nwritten += byte_sent;
						continue;
					}else{
						ClearCommError(hComPort, &dwErrors, &comStat); 
						break;
					}
				}
				nwritten += byte_sent;			
			}
			else{ 
				ClearCommError(hComPort, &dwErrors, &comStat); 
			} 
		}
		if(nwritten == 6)
			printf("SPPEX Server write data: \\r\\nOK\\r\\n\n");
	}*/



HandleComPortData_Exit:
	
//	CloseHandle(writeo.hEvent);

	CloseHandle(reado.hEvent);


	return 0;
}



DWORD WINAPI SPPClientIOThread(LPVOID param)
{

	int nIndex = (int)param;

	

	HANDLE hComPort = OpenPort(g_comIoList[nIndex].nComPort);
	if(hComPort == INVALID_HANDLE_VALUE){
		
		CloseHandle(g_comIoList[nIndex].hThread);
		
		memset(&g_comIoList[nIndex],0, sizeof(COM_IO));
		return 0;
	}


	
	while(g_comIoList[nIndex].bStopFlag != TRUE) {
		//CheckActive(hComPort);
		//For WMouseXP we need to read data as "bt client mode"		

		HandleComPortData(hComPort);

		Sleep(1);
	}
	
	
	CloseHandle(hComPort);

	return 0;
}

DWORD WINAPI SPPServerIOThread(LPVOID param)
{

	int nIndex = (int)param;

	HANDLE hComPort = OpenPort(g_comIoList[nIndex].nComPort);
	if(hComPort == INVALID_HANDLE_VALUE){
		CloseHandle(g_comIoList[nIndex].hThread);
		memset(&g_comIoList[nIndex],0, sizeof(COM_IO));
		return 0;
	}

	while(g_comIoList[nIndex].bStopFlag != TRUE) {
		HandleComPortData(hComPort);
		Sleep(1000);
	}
	
	CloseHandle(hComPort);

	return 0;
}


BOOL StartIoOnComport(DWORD dwConnHandle,int nComport, BOOL bClientRole)
{
	int i;
	DWORD dwID;
	BOOL bRet = FALSE;

mydebug::log("StartIoOnComport 0");

	for(i = 0; i<MAX_IO_THREAD; i++){
		if(g_comIoList[i].hThread == NULL){
			
			
mydebug::log("StartIoOnComport 1");

			g_comIoList[i].nComPort = nComport;
			g_comIoList[i].dwConnHandle = dwConnHandle;
			if(bClientRole)
				g_comIoList[i].hThread = CreateThread(NULL,0,SPPClientIOThread,(LPVOID)i,0,&dwID);		
			else
				g_comIoList[i].hThread = CreateThread(NULL,0,SPPServerIOThread,(LPVOID)i,0,&dwID);

mydebug::log("StartIoOnComport 2");

			Sleep(100);
			
			if(g_comIoList[i].hThread == NULL){
				memset(&g_comIoList[i], 0, sizeof(COM_IO));
				bRet = FALSE;
			}else
				bRet = TRUE;

mydebug::log("StartIoOnComport 3");

		   break;
		}
	}

	return bRet;
}

BOOL StopIoOnComport(DWORD dwConnHandle)
{
	int i;
	BOOL bRet = FALSE;

	SetEvent(reado.hEvent);
	
	for(i = 0; i<MAX_IO_THREAD; i++){
		if((dwConnHandle == 0  || dwConnHandle == g_comIoList[i].dwConnHandle) && g_comIoList[i].hThread != NULL){
			g_comIoList[i].bStopFlag = TRUE;
			if(WaitForSingleObject(g_comIoList[i].hThread, 3000) != WAIT_OBJECT_0)
				TerminateThread(g_comIoList[i].hThread, -1);
			CloseHandle(g_comIoList[i].hThread);
			memset(&g_comIoList[i], 0, sizeof(COM_IO));	
			bRet = TRUE;
			if(dwConnHandle != 0)
				break;
		}
	}
	
	return bRet;
	
}
