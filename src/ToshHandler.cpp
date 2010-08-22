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


// ToshHandler.cpp: implementation of the CToshHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WMouseXP.h"
#include "ToshHandler.h"

#include "SelectPhoneDialog.h"
#include "WMouseXPDlg.h"

#include "TosBtObexAPI.h"
#include "strsafe.h"

#include "comport.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char* rfcommport = "COM76";
const int rfcommport_int = 76;

#include "log.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void ShowHex(unsigned char* data, int len)
{
	CString str;

	for(int i=0;i<len;i++)
	{
		CString part;
		part.Format("%x ",data[i]);
		str +=part;
	}
	AfxMessageBox(str);
}

void CopyBDADDR(const unsigned char* src, unsigned char* dest)
{
	for(int i=0;i<6;i++)
	{
		dest[i]= src[i];
	}
}


bool CToshHandler::ToshSupported()
{
	HMODULE h = LoadLibrary("TosBtAPI.dll");
		if (h == NULL) {				
		return false;
	}
	FreeLibrary(h);	
	return true;
}


CToshHandler* CToshHandler::curInstance = 0;

CToshHandler* CToshHandler::GetInstance(TToshParams& aParams)
{
	curInstance=NULL;
	long startresult = 0;
	if(ToshSupported() && BtOpenAPI(aParams.iHWND,aParams.iAppName,&aParams.iStatus) && BtExecBtMng(&startresult))
	{		
		curInstance = new CToshHandler(aParams);		
	}

	return curInstance;
}



CToshHandler::CToshHandler(TToshParams& aParams)
{
	iParams = aParams;

	memset(&iDevInfo,0,sizeof(iDevInfo));

	memset(&iPrevDevInfoList,0,sizeof(iPrevDevInfoList));
	

	iOpenedObexApi = false;
	iCID = 0;
	iSDPSSRESULT = NULL;

	iRFCOMMCID = 0;
//	iHFile = 0;


		GetModuleFileName(AfxGetInstanceHandle(), iTmpObexFilePath, MAX_PATH);
	
	 /* Search backward. */
	CHAR ch('\\');
	CHAR* pdest = strrchr( iTmpObexFilePath, ch );

	if( pdest != NULL )
	{
		if( MAX_PATH > pdest - iTmpObexFilePath)
			*(pdest+1) = NULL;
	}	
	
	
	StringCbCatA(iTmpObexFilePath,MAX_PATH,KWmxpJarPathRelativeToReleaseExe);

	//////////prepare tosh bt sdp struct iWmxpToshSDPPattern in ram
	iWmxpToshSDPPattern = NULL;
	iWmxpToshSDPPatternSize = 0;

	BTUUIDINFO aBTUUIDINFO;
	aBTUUIDINFO.wUUIDType = 128;
	
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[0] = 0x2b;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[1] = 0xc2;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[2] = 0xb9;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[3] = 0x2e;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[4] = 0x39;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[5] = 0x92;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[6] = 0x11;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[7] = 0xdc;

	((unsigned char*)&(aBTUUIDINFO.BtUUID))[8] = 0x83;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[9] = 0x14;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[10] = 0x08;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[11] = 0x00;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[12] = 0x20;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[13] = 0x0c;
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[14] = 0x9a;			
	((unsigned char*)&(aBTUUIDINFO.BtUUID))[15] = 0x66;	

	

	
	
	//BTUUIDLIST aBTUUIDLIST; cannot static because the declaration was BTUUIDINFO BtUUIDInfo[]; so we need to alloc for the first one too
	BTUUIDLIST* aPBTUUIDLIST = (PBTUUIDLIST) malloc(sizeof(BTUUIDLIST) + sizeof(BTUUIDINFO));
	aPBTUUIDLIST->dwUUIDInfoNum=1;
	memcpy(&(aPBTUUIDLIST->BtUUIDInfo[0]) ,&aBTUUIDINFO, sizeof(BTUUIDINFO));

	iWmxpToshSDPPattern = NULL;
	iWmxpToshAttrIdList = NULL;
	iMemSDP_SSA_Result = NULL;
	
	long status;
	if(BtMakeServiceSearchPattern2(aPBTUUIDLIST, &iWmxpToshSDPPatternSize, &iWmxpToshSDPPattern, &status))
	{
		;//ok 	

		//ShowHex(iWmxpToshSDPPattern,iWmxpToshSDPPatternSize);
			
		
		BTUNIVATTRIBUTE mask = 0;
		mask |= TOSBTAPI_ATR_PROTOCOLDESCRIPTORLIST;
		mask |= TOSBTAPI_ATR_SERVICERECORDHANDLE;
		if(BtMakeAttributeIDList2(mask, &iWmxpToshAttrIdListSize, &iWmxpToshAttrIdList, &iStatus))
			mydebug::log("th164");
		else
		{
			AfxMessageBox("BtMakeAttributeIDList2 failed");
			delete iWmxpToshAttrIdList;
			iWmxpToshAttrIdList = NULL;		
		}
			
	}
	else
	{
		AfxMessageBox("BtMakeServiceSearchPattern2 failed");
		delete iWmxpToshSDPPattern;
		iWmxpToshSDPPattern = NULL;
	}

	if(iWmxpToshSDPPattern ==NULL || aPBTUUIDLIST==NULL)
		AfxMessageBox("SearchPattern or UUID list generation failed");

	mydebug::log("th165");

	free(aPBTUUIDLIST);

	mydebug::log("th166");
	////////////////////////////

	
}

CToshHandler::~CToshHandler()
{	
	BtCloseAPI(&iParams.iStatus);

	if(iDevInfo.pDevInfoList != &iPrevDevInfoList && iDevInfo.pDevInfoList != NULL) //delete if it was from a search result - not local devinfo in prevdev case
		BtMemFree(iDevInfo.pDevInfoList);

	iDevInfo.pDevInfoList = NULL;

	delete iSDPSSRESULT;

	if(iOpenedObexApi)
	{
		long status;
		BtOBEXClose(&status);
	}

	delete iWmxpToshSDPPattern;
}


bool CToshHandler::StartInquiry()
{
	mydebug::log("w914");
	return BtDiscoverRemoteDevice2(&(iDevInfo.pDevInfoList), TOSBTAPI_DD_NO_FRIENDLYNAME, &(iDevInfo.lSts), iParams.iHWND, 
									   UM_TOSHMESSAGES, (LPARAM)&iDevInfo);
}

bool CToshHandler::StartRemoteControlSession(HWND hwnd, CButton* buttontodisable, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, MRemoteControlCaller* caller)
{
	if(iButton)
		return false;
	
	iRequestedCommand = ERemoteControlSession;

	iCaller = caller;
	iHwnd = hwnd;
	iButtonToDisable = buttontodisable;
	iIcon = icon;
	iSearchAnim = searchinganimrsc;
	iSendAnim = sendinganimrsc;
	iButton = button;
	iAnimRscType = rsctype;
	iLabel = label;

	mydebug::log("w91");


	iIcon->Load(MAKEINTRESOURCE(iSearchAnim),iAnimRscType);
	mydebug::log("w92");
	iIcon->ShowWindow(true);
	mydebug::log("w93");
	iIcon->Draw();
	mydebug::log("w94");

	iButtonToDisable->EnableWindow(false);

	button->EnableWindow(false);
	button->SetWindowText("Please Wait...");
	
	mydebug::log("w95");
	label->EnableWindow(true);
	label->SetWindowText("Searching for nearby devices...");

	mydebug::log("w96");
	iFoundDevices.RemoveAll();
	iWMouseXPDevicesIndex.RemoveAll();
	
	mydebug::log("w97");

	CWMouseXPDlg* that= (CWMouseXPDlg*) caller;

	mydebug::log("w98");

	iUsingPrevDev = that->m_PrevDevCheckBox.IsWindowEnabled() && that->m_PrevDevCheckBox.GetCheck() == BST_CHECKED;

	mydebug::log("w99");

	if(!iUsingPrevDev)
	{
		iSelectedDev = -1;
		mydebug::log("w910");
		return StartInquiry();
	}
	else
	{		

		mydebug::log("w911");
		CString s(that->iPrevDev);
		mydebug::log("w912");

		if(s.GetLength()!=19)
		{
			iLabel->SetWindowText("Invalid previous device address. Searching instead");
			mydebug::log("w913");
			iSelectedDev = -1;
			return StartInquiry();
	
		}

mydebug::log("w915");

		s.Delete(0);//(
		s.Delete(s.GetLength()-1);//)

		mydebug::log("w916");
		
		int found;

		while((found = s.Find(':'))>=0)
			s.Delete(found);

			mydebug::log("w917");

		
		for(int i=0;i<6;i++)
		{
			CString hexpart = s.Mid(i*2,2);
			
			iDiscoveringDevice[i] = _tcstoul(hexpart, 0, 16);
		}

		mydebug::log("w918");
	
		//DoAddDevice(iDiscoveringDevice, (unsigned char*)("Previous Device"));		
		
		if(iDevInfo.pDevInfoList != &iPrevDevInfoList) //delete if from search result - not pointing to local struct already
		{
		delete iDevInfo.pDevInfoList;
		iDevInfo.pDevInfoList = NULL;
		}

		iDevInfo.pDevInfoList = &iPrevDevInfoList; //the struct contains 1 devinfo in list already

		iDevInfo.pDevInfoList->dwDevListNum = 1;
		DWORD dwMask = (BTDEVINFOSTS_BDADDR_DEV|BTDEVINFOSTS_BDADDR_DB);		
		iDevInfo.pDevInfoList->DevInfo[0].dwStatus = dwMask;

		mydebug::log("w919");

		//ShowHex(iDiscoveringDevice,6);

		CopyMemory(iDevInfo.pDevInfoList->DevInfo[0].BdAddr,iDiscoveringDevice, sizeof(BDADDR));

		//ShowHex(iDevInfo.pDevInfoList->DevInfo[0].BdAddr,6);

		mydebug::log("w920");

		iSelectedDev = 0;

		mydebug::log("w921");

		HandleMsgDevSearchRes(TOSBTAPI_NM_DISCOVERDEVICE_END,(LONG)&iDevInfo);
		mydebug::log("w922");
		return true;	
	}
	 


}


bool CToshHandler::StartSendFileSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label)
{
	if(iButton)
		return false;

	iRequestedCommand = ESendFileSession;
		
	iHwnd = hwnd;
	iButtonToDisable = connectbutton;
	iIcon = icon;
	iSearchAnim = searchinganimrsc;
	iSendAnim = sendinganimrsc;
	iButton = button;
	iAnimRscType = rsctype;
	iLabel = label;


	iIcon->Load(MAKEINTRESOURCE(iSearchAnim),iAnimRscType);
	iIcon->ShowWindow(true);
	iIcon->Draw();

	iButtonToDisable->EnableWindow(false);

	button->EnableWindow(false);
	button->SetWindowText("Please Wait...");
	
	label->EnableWindow(true);
	label->SetWindowText("Searching for nearby devices...");

	iFoundDevices.RemoveAll();
	iWMouseXPDevicesIndex.RemoveAll();

	BtMemFree(iDevInfo.pDevInfoList);
	iDevInfo.pDevInfoList = NULL;

	
	return StartInquiry();	
}

void CToshHandler::HandleMsgDevSearchRes(UINT wParam, LONG lParam)
{

	PDEVINFO pDev = (PDEVINFO)lParam;
	//if (!pDev)	return;

	mydebug::log("w9211");
		
	if(	((USHORT*)&wParam)[1] ==  TOSBTAPI_EVENT_COMMPORT)			
	{
		mydebug::log("w9212");
			//AfxMessageBox("TOSBTAPI_EVENT_COMMPORT");

			USHORT hiword = ((USHORT*)&lParam)[1];
			USHORT loword = ((USHORT*)&lParam)[0];

			if(hiword == 0) //COMM port creaated - tosh doc page 12
			{
				//AfxMessageBox("commport created");
				//Sleep(5000);

				PBTDEVINFOLIST	ptr;			
					ptr = iDevInfo.pDevInfoList;

/*					BTAUTOCONNECTCOMMINFO aBTAUTOCONNECTCOMMINFO;
					
					aBTAUTOCONNECTCOMMINFO.dwSize = sizeof(BTAUTOCONNECTCOMMINFO);
					StringCbCopyA(aBTAUTOCONNECTCOMMINFO.szPortName,5,"COMwm");
					aBTAUTOCONNECTCOMMINFO.bSCN = iWmxpscn; //this is set before BtCreateCOMM
					aBTAUTOCONNECTCOMMINFO.wPSM = 0;
					aBTAUTOCONNECTCOMMINFO.wCID=0;
					CopyBDADDR(ptr->DevInfo[iSelectedDev].BdAddr,aBTAUTOCONNECTCOMMINFO.BdAddr);
					aBTAUTOCONNECTCOMMINFO.dwOption=0; // Not used.(name ‘0’)
					aBTAUTOCONNECTCOMMINFO.wAutoConnect=2;//auto-connect

					

					BtSetAutoConnectCOMMInfo(&aBTAUTOCONNECTCOMMINFO,&iStatus);*/

					CString porttoopen;
					porttoopen = "\\\\.\\"; //known issue for COM port above COM9 see http://support.microsoft.com/kb/115831
					porttoopen += rfcommport;					

					
					//AfxMessageBox(porttoopen);


/*						 iHFile = CreateFile (porttoopen,      // Open One.txt
                      GENERIC_READ|GENERIC_WRITE,           // Open for reading
                      FILE_SHARE_READ|FILE_SHARE_WRITE,    
                      NULL,                   // No security
                      OPEN_ALWAYS,        
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

					   if (iHFile == INVALID_HANDLE_VALUE)
					   {
						   //TODO: l2cap PSM
						   //TODO: cleanup notifycomm and BtCreateCOMM (BtDestroyCOMM)							
      						AfxMessageBox("Failed to open connection... Please try again.");		
							RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...		
							return;
					   }

    /////////////////prepare COMM port file
    DCB dcb;
	COMMTIMEOUTS commTimeouts;
	
	GetCommState((HANDLE)iHFile,&dcb);
	dcb.BaudRate = CBR_115200;
	dcb.Parity = NOPARITY;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	SetCommState((HANDLE)iHFile,&dcb);

	commTimeouts.ReadIntervalTimeout = MAXDWORD;
	commTimeouts.ReadTotalTimeoutConstant = 0;
	commTimeouts.ReadTotalTimeoutMultiplier = 0;
	commTimeouts.WriteTotalTimeoutConstant = 100;
	commTimeouts.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts((HANDLE)iHFile,&commTimeouts);
	PurgeComm((HANDLE)iHFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
	SetupComm((HANDLE)iHFile, 1024, 1024);
	EscapeCommFunction((HANDLE)iHFile,SETRTS);
	EscapeCommFunction((HANDLE)iHFile,SETDTR);
	/////////////////////////////////////////
*/
					mydebug::log("w9213");

					DWORD	dwOpt = 0;

					iLabel->SetWindowText("Connecting to phone...");
					
					 if(BtConnectCOMM2(rfcommport, ptr->DevInfo[iSelectedDev].BdAddr, iWmxpscn,3, &iRFCOMMCID, dwOpt, &iStatus, iParams.iHWND, UM_TOSHMESSAGES, 0))
						 mydebug::log("BtConnectCOMM2 true");
					 else
					 {

						 mydebug::log("BtConnectCOMM2 false");

						 //TODO: cleanup notifycomm and BtCreateCOMM (BtDestroyCOMM)				
						 if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
      						AfxMessageBox("Failed to start connection... Please try again.");		
						 else
							 CWMouseXPDlg::curInstance->AddLog("Failed to start connection");

							RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...		
							return;
					 
					 }

					 mydebug::log("w9214");

			
			
			}
			else if(hiword == 1) //COMM port deleted
			{
				//AfxMessageBox("commport deleted");
			
			}
			else
			{			
			
			}


	}
	else
	{

	// Response message of asynchronous command (BtDiscoverRemoteDevice2)
	switch (wParam) {
		case TOSBTAPI_NM_DISCOVERDEVICE_START:
			mydebug::log("th522");
			iLabel->SetWindowText("Discovering services...");
			mydebug::log("th524");
			break;
		case TOSBTAPI_NM_DISCOVERDEVICE_FOUND:
		{	// Device exist
			mydebug::log("th528");
			iLabel->SetWindowText("found service...");
			mydebug::log("th529");
			//AfxMessageBox("found");

			/*PBTDEVINFOLIST	ptr;
			BDADDR	BdAddr;
			DWORD	i;
			char	szBuff[64] = {0};

			ptr = pDev->pDevInfoList;
			if (!ptr) {
				break;
			}
			if (ptr->dwDevListNum == 0) {
				break;
			}

			// Reset Combo Box for BD_Addr
			// Combo box (BD_ADDR list) Set and Show to list box
			for (i=0; i<ptr->dwDevListNum; i++) {
				DWORD dwMask = (BTDEVINFOSTS_BDADDR_DEV|BTDEVINFOSTS_BDADDR_DB);
				if (ptr->DevInfo[i].dwStatus & dwMask) {
					CopyMemory(BdAddr, ptr->DevInfo[i].BdAddr, sizeof(BDADDR));
					wsprintf(szBuff, "%02X:%02X:%02X:%02X:%02X:%02X", BdAddr[0], BdAddr[1], BdAddr[2], BdAddr[3], BdAddr[4], BdAddr[5]);
					m_cbBdAddr.AddString((LPCTSTR)szBuff);
				}
			}
			// Set cursel
			m_cbBdAddr.SetCurSel(0);*/
			break;
		}
		case TOSBTAPI_NM_DISCOVERDEVICE_END:
		{	
			mydebug::log("w9215");

			iLabel->SetWindowText("service discovery complete...");
			
			PBTDEVINFOLIST	ptr = NULL;
			BDADDR	BdAddr;
			DWORD	i;
			char	szBuff[64] = {0};

			mydebug::log("w9216");

			if(pDev==NULL)
			{
			if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
				AfxMessageBox("InternalError: invalid device set. Please try again.");

			RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
			}

			ptr = pDev->pDevInfoList;

			
			if(pDev==NULL)
			{
				if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
					AfxMessageBox("InternalError: invalid device list. Please try again.");

			RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
			}


			mydebug::log("w9217");

			if (!ptr) 
			{
				if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
					AfxMessageBox("Find device error (0), make sure your PC's Bluetooth is Enabled and try again.");

				RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
				break;
			}
			if (ptr->dwDevListNum == 0) 
			{
				this->iLabel->SetWindowText("No nearby Bluetooth devices found");		
				if(iRequestedCommand == ESendFileSession)
				{
					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						MessageBox(iHwnd,"No nearby Bluetooth devices found.\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\",\"Find me!\",etc.).\r\n- Try Again.\r\n- See \"Help\" for other installation methods.","No nearby devices found",MB_OK | MB_ICONASTERISK);
				}
				else
				{
					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						MessageBox(iHwnd,"No nearby Bluetooth devices found.\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\",\"Find me!\",etc.).\r\n- Try Again.","No nearby devices found",MB_OK | MB_ICONASTERISK);				
				}
				RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
				break;
			}
			
			mydebug::log("w9218");
			
			if(iRequestedCommand == ESendFileSession)				
			{

				mydebug::log("sendfilesession");

				int SelectedDev=-1;
				iIcon->Stop();
			
				mydebug::log("w622");
				
				//let user choose from listbox
				CSelectPhoneDialog dlg(EToshStack,NULL,SelectedDev,CWMouseXPDlg::curInstance,ptr);

				mydebug::log("w627");

				if(IDOK == dlg.DoModal() && SelectedDev>=0)
				{
					mydebug::log("w628");
					iLabel->SetWindowText("Preparing to send file...");
					mydebug::log("w629");
					if(!iOpenedObexApi)
					{
						mydebug::log("w630");
						long status;
						if(BtOBEXOpen(iParams.iHWND,&status))
						{
							mydebug::log("w631");
							iOpenedObexApi = true;						
						}
						else
						{
							MessageBox(iHwnd,"Failed to start the Toshiba Bluetooth file-send engine\r\n\r\nPlease install the Mobile-side program (WMouseXP.jar - at the installation folder of this program) manually.","Send Failed",MB_OK | MB_ICONASTERISK);
							RevertToIdle(); 
							break;
						}
					}

				
						///////////////////////////////////////////////////
					
						{
						
						
							mydebug::log("w636");
							DWORD dwAllocSize = sizeof( TOSBTOBEXAPI_SENDFILE_FILELIST );
							mydebug::log("w637");
							PTOSBTOBEXAPI_SENDFILE_FILELIST ptFileList = (PTOSBTOBEXAPI_SENDFILE_FILELIST) malloc(dwAllocSize);							
							mydebug::log("w638");
							ZeroMemory( ptFileList, dwAllocSize );
							mydebug::log("w639");
							ptFileList->dwCount = 1;
							ptFileList->tFile[0].pszName = iTmpObexFilePath;
							
							//send to phone
							//BtOBEXSendFile(hWndApp, uMessage, pb, ptFileList, dwProfile, dwMaskUI, plStatus);

							iLabel->SetWindowText("Sending file...");

							mydebug::log("w640");

							BtOBEXSendFile(NULL,0,ptr->DevInfo[SelectedDev].BdAddr,ptFileList,0,0,0);

							mydebug::log("w641");

							free(ptFileList);
							
							mydebug::log("w642");

							RevertToIdle();

							mydebug::log("w643");
						}

				}
				else
				{
				RevertToIdle(); 
				}
			}//end of if sendfilesession
			else
			{
				mydebug::log("th63");
				//start connect to phone session
				//discover through list and connect to it
				if(iWmxpToshSDPPattern)
				{
					mydebug::log("th64");
					//connect sdp to first device in list
					if(iSelectedDev == -1)
					{

						mydebug::log("th65");
						iLabel->SetWindowText("please select phone...");
						mydebug::log("th66");
						iIcon->Stop();
						mydebug::log("th67");
						CSelectPhoneDialog dlg(EToshStack,NULL,iSelectedDev,CWMouseXPDlg::curInstance,ptr);
						mydebug::log("th68");

						if(IDOK == dlg.DoModal() && iSelectedDev>=0)
						{
							mydebug::log("th69");
							iLabel->SetWindowText("Looking for WMouseXP on phone...");
							mydebug::log("th70");
							if(BtConnectSDP(ptr->DevInfo[iSelectedDev].BdAddr,&iCID,&iStatus,iParams.iHWND,UM_TOSHMESSAGES,iSelectedDev))							
							mydebug::log("th71");
							else
							{
								mydebug::log("th72");
								if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
									AfxMessageBox("Failed to connect to device...");

								RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
							}
						}
						else
						{
							mydebug::log("th73");
							RevertToIdle();
						}
					}
					else
					{

										mydebug::log("w92183");
						iLabel->SetWindowText("Looking for WMouseXP on phone...");

						if(BtConnectSDP(ptr->DevInfo[iSelectedDev].BdAddr,&iCID,&iStatus,iParams.iHWND,UM_TOSHMESSAGES,iSelectedDev))							
						{
							mydebug::log("w92184");
						}
						else
						{
							if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
								AfxMessageBox("Failed to connect to device...");

							RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
						}

						mydebug::log("w92185");

					}

				}
				else
				{
					mydebug::log("th771");
					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						AfxMessageBox("Invalid tosh sdp pattern.");

					RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
				}
				
			}			
			break;
		}
		case TOSBTAPI_NM_DISCOVERDEVICE_ERROR:
		{	
			if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
				AfxMessageBox("Find device error (1), make sure your PC's Bluetooth is Enabled and try again.");

			RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
			break;
		}
		
		////////////////SDP

		////////////sdp connect result
		case TOSBTAPI_NM_CONNECTSDP_CONNECTED:
		{
			
			mydebug::log("th795");
			//iSDPSSRESULT
			//Sleep(700);

			if(BtServiceSearchAttribute2(iCID,iWmxpToshSDPPatternSize ,iWmxpToshSDPPattern , iWmxpToshAttrIdListSize, iWmxpToshAttrIdList, &iMemSDP_SSA_Result, &iStatus, iParams.iHWND, UM_TOSHMESSAGES , iCID ))
				mydebug::log("th800");
			else
			{			
				mydebug::log("th803");
				if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
					AfxMessageBox("Failed to start search for WMouseXP on device...");

				RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...			
			}

			break;
		}

		case TOSBTAPI_NM_CONNECTSDP_ERROR:
		{
			mydebug::log("th804");
			if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
				AfxMessageBox("Failed to connect to search for WMouseXP on device...");

			mydebug::log("th805");
			RevertToIdle();
			break;
		}

		case TOSBTAPI_NM_CONNECTSDP_END:
		{
			mydebug::log("th822");
			break;		
		}
		/////////////////////////////


		////////////sdp search result
		case TOSBTAPI_NM_SSA_RESULT:
		{
			mydebug::log("th831");
			/*CString str;
			str.Format("%d status %d",iCID,iStatus);			
			AfxMessageBox(str);
			*/

			iLabel->SetWindowText("analyzing service search result...");

			mydebug::log("th832");
			PBTANALYZEDATTRLIST2 *pMemAnalyzedAttributeList;
			PBTANALYZEDATTRLIST2 pAnalyzeResult = NULL;	
			mydebug::log("th833");
			int wmxpscn = -1;
			
			BTUNIVATTRIBUTE mask = 0;
			mask |= TOSBTAPI_ATR_PROTOCOLDESCRIPTORLIST;
			mask |= TOSBTAPI_ATR_SERVICERECORDHANDLE;
mydebug::log("th834");
			if (BtAnalyzeServiceAttributeLists2(mask, iMemSDP_SSA_Result, &pAnalyzeResult, &iStatus) && pAnalyzeResult)
			{	
				mydebug::log("th835");

				/////disconnect now - no need to wait for reverttoidle - we prepare for rfcomm
				BtDisconnectSDP(iCID,&iStatus);
				mydebug::log("th836");
				iCID = 0;
				/////////////////
			mydebug::log("th837");
				BTUNIVATTRIBUTE		BtUnivAttr;
				for (int i=0; i<pAnalyzeResult->dwNum; i++) 
				{
					mydebug::log("th838");
					BtUnivAttr = pAnalyzeResult->BtAnalyzedAttr[i].BtAnalyzedAttribute;
					mydebug::log("th839");
					if (pAnalyzeResult->BtAnalyzedAttr[i].pMemProtocolDescriptorList) 
					{
						mydebug::log("th840");
					//	AfxMessageBox("0");

						PBTPROTOCOLLIST2 aPBTPROTOCOLLIST2 = pAnalyzeResult->BtAnalyzedAttr[i].pMemProtocolDescriptorList->pMemProtocolList[0];
						mydebug::log("th841");

						for(int j=0; j<aPBTPROTOCOLLIST2->dwProtocolNum; j++)			
						{
							mydebug::log("th842");
					//		AfxMessageBox("1");

							PBTPROTOCOL2 protocolinfo = &(aPBTPROTOCOLLIST2->BtProtocol[j]);							
							mydebug::log("th843");
							if(protocolinfo->wParameterType & 1)
							{
							//	AfxMessageBox("2")
								mydebug::log("th844");
								wmxpscn = protocolinfo->dwParameter;
								mydebug::log("th845");
							}
						}
					}
				}

				mydebug::log("th846");
				if(!BtFreePBTANALYZEDATTRLIST2(&pAnalyzeResult, &iStatus)) //let the tosh free their complex struct
				{
					mydebug::log("th847");
					free(pAnalyzeResult);//free as much as we can
					mydebug::log("th848");
				}

				if(wmxpscn == -1)
				{				
					mydebug::log("th849");
					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						AfxMessageBox("WMouseXP is not running on phone, please install/start WMouseXP on phone and try again.");

					mydebug::log("th850");
					RevertToIdle();	
					mydebug::log("th851");
					return;
				}

				iWmxpscn = wmxpscn;
				mydebug::log("th852");
				//prepare rfcomm connection to that wmxp rfcomm scn channel
				iLabel->SetWindowText("Preparing communication to phone...");
				mydebug::log("th853");
				BtDestroyCOMM(rfcommport,&iStatus);
				mydebug::log("th854");
				Sleep(2000);
				mydebug::log("th855");
				if(BtNotifyEvent(TOSBTAPI_EVENTMASK_COMMPORT, &iStatus, iParams.iHWND,UM_TOSHMESSAGES ) )
				{		
					mydebug::log("th856");
					BYTE	bSCN = 0;
					WORD	wPSM = 3;	// 3=RFCOMM

					
					mydebug::log("th857");
					if(BtCreateCOMM(rfcommport, bSCN, wPSM, &iStatus) )//we're using as a client so use scn 0, psm 0 as suggested by tosh doc on BtCreateCOMM
					{
						mydebug::log("th858");;//ok wait for comport created event

					}
					else
					{
						mydebug::log("th859");
						CString errstr;
						errstr.Format("Initial create connection failed: %x",iStatus);
						if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
							AfxMessageBox(errstr);

						RevertToIdle();	
						return;									
					}

					
				}
				else
				{
					mydebug::log("th860");
					CString errstr;
					errstr.Format("Initial connection start to phone failed: %x",iStatus);
					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						AfxMessageBox(errstr);

					RevertToIdle();	
					return;				
				}
				mydebug::log("th861");


			}			
			else
			{
				mydebug::log("th862");
				if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
					AfxMessageBox("Search for WMouseXP on device failed - failed to analyze, try again...");

				RevertToIdle();	
				return;
			}
			

			break;		
		}

		case TOSBTAPI_NM_SSA_ERROR:
		{			
			mydebug::log("TOSBTAPI_NM_SSA_ERROR");

			if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
				AfxMessageBox("Search for WMouseXP on device failed...");		

			RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...						
			break;
		}
		//////////////////////


		/////////////////////////////RFCOMM

		//////////create comm port
		case TOSBTAPI_NM_CREATECOMM_ERROR:
			{
				mydebug::log("TOSBTAPI_NM_CREATECOMM_ERROR");
				if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
					AfxMessageBox("Create comm-port failed...");		

				RevertToIdle();			
				break;
			}

		case TOSBTAPI_NM_CREATECOMM_CREATED:
			{
				mydebug::log("TOSBTAPI_NM_CREATECOMM_CREATED");
				//AfxMessageBox("TOSBTAPI_NM_CREATECOMM_CREATED");		
				break;
			}
		///////////////////////////


		//////////////////////////
		case TOSBTAPI_NM_CONNECTCOMM_ERROR:
			{
				mydebug::log("TOSBTAPI_NM_CONNECTCOMM_ERROR");
				if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
					AfxMessageBox("Failed to connect to phone... Please try again.");		

				RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...						
				break;
			}

		case TOSBTAPI_NM_CONNECTCOMM_CONNECTED:
			{
				mydebug::log("TOSBTAPI_NM_CONNECTCOMM_CONNECTED");

				iLabel->SetWindowText("Connected... preparing data reader");

				mydebug::log("0000000000000000");

				PBTDEVINFOLIST	ptr;
				ptr = iDevInfo.pDevInfoList;

				mydebug::log("connected");

				if(StartIoOnComport(iRFCOMMCID,rfcommport_int, true))
				{
					
					//ShowHex(ptr->DevInfo[iSelectedDev].BdAddr,6);
					
					mydebug::log("StartIoOnComport true");

					iCaller->OnConnected(ptr->DevInfo[iSelectedDev].BdAddr,6);    		

					mydebug::log("StartIoOnComport OnConnected");

					//if(FALSE == BtNotifyCOMM(gpInfo->szPortName, gpInfo->wEventMask, gpInfo->BD_ADDR, &(gpInfo->wCID), &(gpInfo->lSts)
					
					//0x0002 = monitor disconneciton 

					BtNotifyCOMM(rfcommport, 0x0002, ptr->DevInfo[iSelectedDev].BdAddr, &iRFCOMMCID, &iStatus, iParams.iHWND, UM_TOSHMESSAGES, iRFCOMMCID );					
					mydebug::log("BtNotifyCOMM");

					
				}
				else
				{
					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						AfxMessageBox("Failed to start COMM reader");		

					RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...						
				}

				break;
			};		

		case TOSBTAPI_NM_NOTIFYCOMM_ERROR:
			{
				mydebug::log("TOSBTAPI_NM_NOTIFYCOMM_ERROR");
		//	AfxMessageBox("TOSBTAPI_NM_NOTIFYCOMM_ERROR");
				break;
			};
		case TOSBTAPI_NM_NOTIFYCOMM_CONNECTED:
		{
			mydebug::log("TOSBTAPI_NM_NOTIFYCOMM_CONNECTED");
		//	AfxMessageBox("TOSBTAPI_NM_NOTIFYCOMM_CONNECTED");
			break;
		};
		case TOSBTAPI_NM_NOTIFYCOMM_DISCONNECTED:
		{
		//	AfxMessageBox("TOSBTAPI_NM_NOTIFYCOMM_DISCONNECTED");
			mydebug::log("TOSBTAPI_NM_NOTIFYCOMM_DISCONNECTED");

			RevertToIdle();
			
			mydebug::log("RevertToIdle");

			iCaller->OnDisconnected();
			
			mydebug::log("iCaller->OnDisconnected()");


			break;
		};		
		//////////////////////////


		default:
		{	// Unfixed status		
			//AfxMessageBox("toshbt handler state error");
			//RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
			break;
		}
	}//end switch

	}//end else

}



void CToshHandler::RevertToIdle()
{
	mydebug::log("wr0");
	if(!iButton)
		return;

	CButton* tmpbutton = iButton; //CloseAndRevertEngineState would set it to null

	mydebug::log("wr1");
	CloseAndRevertEngineState();
	mydebug::log("wr2");

	iButton = tmpbutton; //CloseAndRevertEngineState already set it to null

	if(iRequestedCommand == ESendFileSession)
	{
	this->iButtonToDisable->EnableWindow(true);
	this->iButtonToDisable->SetWindowText("Connect");
	iLabel->SetWindowText("- Make sure it shows \"Awaiting PC Side\".");
	iButton->EnableWindow(true);	
	iButton->SetWindowText(CString(KNotInstalledOnPhoneStr));
	}
	else
	{	
		/*
		if(iConnectionHandle>0)
		{
			BT_DisconnectSPPExService(iConnectionHandle);
			StopIoOnComport(iConnectionHandle);
			iConnectionHandle = 0;
			memset(iConnectedDevice,0,6);
		}*/

		Sleep(200);
		mydebug::log("wr21");

		this->iButtonToDisable->EnableWindow(true);
		mydebug::log("wr22");
		this->iButtonToDisable->SetWindowText(CString(KNotInstalledOnPhoneStr));
		mydebug::log("wr23");
		this->iButton->SetWindowText("Connect");			
		mydebug::log("wr24");
		this->iButton->EnableWindow(true);			
		mydebug::log("wr25");
	}


	
mydebug::log("wr3");

	this->iIcon->Stop();
	this->iIcon->ShowWindow(false);
	this->iIcon->UnLoad();

	mydebug::log("wr4");

	

	iButton = NULL;	

	//CWMouseXPDlg::curInstance->StartNokiaPCSuiteServiceLayer(); start and stop too frequently makes inquiry fail

}

void CToshHandler::CloseAndRevertEngineState()
{

	mydebug::log("wr01");
	//stop notify events
	BtNotifyEvent(0, &iStatus, iParams.iHWND,UM_TOSHMESSAGES );
	mydebug::log("wr02");

	if(iCID>0)
	{
		mydebug::log("wr03");
		BtDisconnectSDP(iCID,&iStatus);
		iCID = 0;
	}

	if(iRFCOMMCID >0)
	{
		mydebug::log("wr04");
		BtDisconnectCOMM(rfcommport,&iStatus);
		BtDestroyCOMM(rfcommport,&iStatus);
		iRFCOMMCID = 0;	
	}

	/*if(iHFile>0)
	{
		CloseHandle(iHFile);
		iHFile = 0;
	}*/
	mydebug::log("wr05");
	StopIoOnComport(NULL);

	mydebug::log("wr06");


	iButton = NULL;

}
