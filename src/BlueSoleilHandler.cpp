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


// WidcommHandler.cpp: implementation of the CBlueSoleilHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include <bt_ui.h>

#include "WMouseXP.h"
#include "WMouseXPdlg.h"

#include "BlueSoleilHandler.h"
#include "selectphonedialog.h"

#include "BlueSoleilFoundDevice.h"

#include "sdkfunc.h"//BlueSoleil helper functions

#include "strsafe.h"

#include "comport.h" //BlueSoleil helper comport handler

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//static GUID ObexObjectPush_guid = CBtIf::guid_SERVCLASS_OBEX_OBJECT_PUSH;

CBlueSoleilHandler* CBlueSoleilHandler::curInstance = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool IsSameBDADDR(BYTE* b1,BYTE* b2)
{
	for(int i=0;i<DEVICE_ADDRESS_LENGTH;i++)
	{
		if(b1[i]!=b2[i])
			return false;
	}

	return true;
}

void CopyBDADDR(BYTE* src, BYTE* dest)
{
	for(int i=0;i<DEVICE_ADDRESS_LENGTH;i++)
	{
		dest[i]= src[i];
	}
}



CBlueSoleilHandler::CBlueSoleilHandler()
{
	this->iButton =NULL;
	this->iLabel = NULL;
	this->iFileData = NULL;
	this->iFileLen = 0;
	iDiscoverCommandSent = false;

	iCaller = NULL;

	
	GetModuleFileName(AfxGetInstanceHandle(), iTmpObexFilePath, MAX_PATH);
	
	 /* Search backward. */
	CHAR ch('\\');
	CHAR* pdest = strrchr( iTmpObexFilePath, ch );

	if( pdest != NULL )
	{
		if( MAX_PATH > pdest - iTmpObexFilePath)
			*(pdest+1) = NULL;
	}	
	
	
	StringCbCatA(iTmpObexFilePath,MAX_PATH,"WMouseXP.jar");

	
	//GetFullPathNameW(_L("WMouseXP.jar"),KPATHMAXLEN,iTmpObexFilePath,NULL);
	
	//GetTempPathW(KPATHMAXLEN,iTmpObexFilePath);

	/*(*__argv);
	
	*/

	//wcscat(iTmpObexFilePath,_L("WMouseXP.jar"));
	
	

	iUsingPrevDev = false;

	m_TimerID = 0;

	iState = EHandlerStateIdle;

	iInquiryThreadID = 0;

	iConnectionHandle = 0;
	memset(iConnectedDevice,0,6);

}

CBlueSoleilHandler* CBlueSoleilHandler::GetInstance()
{
	curInstance=NULL;

	if(BlueSoleilSupported() && SDK_Init())
	{
		////////////////////events we're interested in
		SDK_BtRegisterCallBack(EVENT_ERROR);
		SDK_BtRegisterCallBack(EVENT_CONNECTION_STATUS);		
		SDK_BtRegisterCallBack(EVENT_SPPEX_CONNECTION_STATUS);
		///////////////////////////////////////////////

		curInstance = new CBlueSoleilHandler();		
	}

	return curInstance;
}

bool CBlueSoleilHandler::BlueSoleilSupported()
{
	HMODULE h = LoadLibrary("btfunc.dll");
		if (h == NULL) {				
		return false;
	}
	FreeLibrary(h);	
	return true;
}

CBlueSoleilHandler::~CBlueSoleilHandler()
{
	curInstance = NULL;

	CloseAndRevertEngineState();

	iFoundDevices.RemoveAll();

	////////////////////unregister callbacks events we're interested in
	SDK_BtUnRegisterCallBack(EVENT_ERROR);
	SDK_BtUnRegisterCallBack(EVENT_CONNECTION_STATUS);		
	SDK_BtUnRegisterCallBack(EVENT_SPPEX_CONNECTION_STATUS);
	////////////////////////////////////////////////////////////////////

	SDK_Done();


}

bool CBlueSoleilHandler::StartRemoteControlSession(HWND hwnd, CButton* buttontodisable, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, MRemoteControlCaller* caller)
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
	
	SDK_CancelInquiry();
	Sleep(100);
	//Close();

	CWMouseXPDlg* that= (CWMouseXPDlg*) caller;

	iUsingPrevDev = that->m_PrevDevCheckBox.IsWindowEnabled() && that->m_PrevDevCheckBox.GetCheck() == BST_CHECKED;

	if(!iUsingPrevDev)
	{
		iState = EHandlerStateDoingInquiry;
		//StartBTInteractionTimer();

		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartInquiry,this,0,&iInquiryThreadID);	

		return true;
	}
	else
	{
		for(int i=0;i<DEVICE_ADDRESS_LENGTH;i++)
		{
				this->iDiscoveringDevice[i]= that->iPrevDev[i];
		}

		CString s(that->iPrevDev);
		
		

		

		if(s.GetLength()!=19)
		{
			iLabel->SetWindowText("Invalid previous device address. Searching instead");
			
			iState = EHandlerStateDoingInquiry;
			
			CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartInquiry,this,0,&iInquiryThreadID);	

			return true;
	
		}

		s.Delete(0);//(
		s.Delete(s.GetLength()-1);//)
		
		int found;

		while((found = s.Find(':'))>=0)
			s.Delete(found);

			

		
		for(i=0;i<6;i++)
		{
			CString hexpart = s.Mid(i*2,2);
			
			//BlueSoleil uses opposite direction of byte ordering...
			iDiscoveringDevice[5-i] = _tcstoul(hexpart, 0, 16);
			
		}
	
		const CHAR* devname = "Previous Device";

		BLUETOOTH_DEVICE_INFO prevdevinfo;
		memset(&prevdevinfo,0,sizeof(BLUETOOTH_DEVICE_INFO));
		prevdevinfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
		CopyBDADDR(iDiscoveringDevice,prevdevinfo.address );
		StringCbCopy(prevdevinfo.szName,MAX_DEVICE_NAME_LENGTH,devname);


		DoAddDevice(prevdevinfo);		
		iCurDevice = 0; //start going through all the devices		
		
		//the iFoundDevices's size is not 0 so this is the flag to not do inquiry in this case
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartInquiry,this,0,&iInquiryThreadID);		

		return true;
	}
	 


}

bool CBlueSoleilHandler::StartSendFileSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, BYTE* fdata, int fdatalen)
{
	if(iButton)
		return false;

	iRequestedCommand = ESendFileSession;
	
	//TODO 101007: implemt below comments... 
	iHwnd = hwnd;
	iButtonToDisable = connectbutton;
	iIcon = icon;
	iSearchAnim = searchinganimrsc;
	iSendAnim = sendinganimrsc;
	iButton = button;
	iAnimRscType = rsctype;
	iLabel = label;
	this->iFileData = fdata;
	this->iFileLen = fdatalen;


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

	SDK_CancelInquiry();
	Sleep(100);
//	Close();


////////////////////////////////////////////////////CREATE THREAD

		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartInquiry,this,0,&iInquiryThreadID);	

	
/////////////////////////////////////////////////////////////////////

		return true;
	
	
	 
	//do inquiry
	//show dialog to choose dev
	//do obexop to selected dev

	
}

int CBlueSoleilHandler::StartInquiry(LPVOID that)
{
	
	if(CBlueSoleilHandler::curInstance->iFoundDevices.GetSize()==0) //if 0 means need to do inquiry, if 1 means connect to previous device
	{
		if(CBlueSoleilHandler::curInstance)
			CBlueSoleilHandler::curInstance->iState = EHandlerStateDoingInquiry;
		else
			return 2;
	

		CBlueSoleilHandler::curInstance->StartBTInteractionTimer();
		SDK_BtInquiry();
		CBlueSoleilHandler::curInstance->StopBTInteractionTimer();
	}

		CBlueSoleilHandler::curInstance->OnInquiryComplete();		

		return 1;

}
/*
void CBlueSoleilHandler::OnDeviceResponded (BYTE* bda, DEV_CLASS devClass, BD_NAME bdName, BOOL bConnected)
{
    
    // Add the device address and name (if any)
    // to the Server List.  It is OK to pass
    // duplicates.  This method will ignore them.
    //

    DoAddDevice(bda, bdName);	
	
}*/

static char m_serviceName[253 + 1] = "WMouseXPService";


/*void CBlueSoleilHandler::OnDiscoveryComplete()
{

	if(!iDiscoverCommandSent)
		return;
	
	iDiscoverCommandSent = false;

	int services_found = 0;
	CSdpDiscoveryRec    SdpRecs[MAX_SERVICES_PER_DEVICE];
	
	UINT16 numrecs;
	
	
	if(DISCOVERY_RESULT_SUCCESS == GetLastDiscoveryResult(iDiscoveringDevice,&numrecs))
		;
	else
	{
		MessageBox(iHwnd,"Discovery Failed","Send Failed",MB_OK | MB_ICONASTERISK);
	}


    services_found = ReadDiscoveryRecords(iDiscoveringDevice,MAX_SERVICES_PER_DEVICE,SdpRecs,&iDiscoveringGUID);


	
	if(iRequestedCommand == ESendFileSession)
	{
	
		if(services_found<=0)
		{
			this->iLabel->SetWindowText("OBEX Object Push not found on device.");
			MessageBox(iHwnd,"OBEX Object Push not found on selected device.\r\n\r\nWMouseXP can't send the mobile-side installer to it. Please see the Help on how to install the mobile side manually.","Send Failed",MB_OK | MB_ICONASTERISK);
			
			RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
			return;		
		}
		else
		{
			for(int i=0;i<services_found;i++)
			{
				UINT8 rfcommscn;
				if(SdpRecs[i].FindRFCommScn(&rfcommscn))
				{
					
				
					//change icon to blue and start the icon
					//extract the jar to a temporary location...
					//do obex op to this device using this sdp record
					iLabel->SetWindowText("Sending File...");
				
					
					///////////////////////////////////////////////////
						//make jar and send to device


						HGLOBAL hResourceLoaded;		// handle to loaded resource 
						HRSRC hRes;						// handle/ptr. to res. info. 
						BYTE* lpResLock;				// pointer to resource data 
						DWORD dwSizeRes;
						

						hRes = FindResource( NULL, MAKEINTRESOURCE(IDR_JAR3), "JAR" );
						
						if(hRes==NULL)
							MessageBox(iHwnd,"Internal resource missing.","Send Failed",MB_OK | MB_ICONASTERISK);
						// loads the specified resource into global memory. 
						hResourceLoaded = LoadResource( NULL, hRes ); 

						// get a pointer to the loaded resource!
						lpResLock = (BYTE*)LockResource( hResourceLoaded ); 

						// determine the size of the resource, so we know how much to write out to file!  
						dwSizeRes = SizeofResource( NULL, hRes );

						

						CFile file;
						CString path(iTmpObexFilePath);
						
						

						if(!file.Open( path, CFile::modeCreate | CFile::modeWrite|CFile::typeBinary))
						{
							//showerr
							MessageBox(iHwnd,"Create temp file failed.","Send Failed",MB_OK | MB_ICONASTERISK);
							RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
							return;	

						}
						else
						{
						
							//write to file
							file.Write(lpResLock,dwSizeRes);
							file.Flush();
							file.Close();
							
							//send							
							if(OPP_CLIENT_SUCCESS == Push(iDiscoveringDevice,iTmpObexFilePath,SdpRecs[i]))
							{
							
							
							}
							else
							{
									if(OPP_CLIENT_SUCCESS == Push(iDiscoveringDevice,iTmpObexFilePath,SdpRecs[i]))
									{
									
									
									}
									else
									{
									
										MessageBox(iHwnd,"Temporary failure, Please try again.","Send Failed",MB_OK | MB_ICONASTERISK);
										RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
										return;	

									}						

							}
							
							
													
						}


					

					
					
					return;
				}
			}
		
			this->iLabel->SetWindowText("No valid OBEX Object Push channels on device.");
			MessageBox(iHwnd,"No valid OBEX Object Push channels on device.\r\n\r\nWMouseXP can't send the mobile-side installer to it. Please see the Help on how to install the mobile side manually.","Send Failed",MB_OK | MB_ICONASTERISK);
			
			RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
			return;	

		}
	}
	else
	{
		if(services_found<=0)
		{			
			iLabel->SetWindowText("WMouseXP not found on this device...");			
		}
		else
		{
			bool found = false;

			for(int i=0;i<services_found;i++)
			{
				UINT8 rfcommscn=0;
				if(SdpRecs[i].FindRFCommScn(&rfcommscn) && rfcommscn!=0)
				{					
					CWidcommFoundDevice* dev = (CWidcommFoundDevice*) iFoundDevices.GetAt(iCurDevice-1);										
					(dev->iWMouseXPscn) = rfcommscn;
					iWMouseXPDevicesIndex.Add(new int(iCurDevice-1));
					found = true;				
					break;
				}
			}
			
			if(!found)					
			{
				iLabel->SetWindowText("WMouseXP not found on this device...");				
			}
			
		}

		if(DoDiscoverWMouseXPThroughDeviceList())
		{
			;//wait until it finishes the list
		}
		else
		{
			//finished discovering all devices
			
			
			//see howmany devices remain in the list			
			if(iWMouseXPDevicesIndex.GetSize()==1)
			{
				//connect rfcomm to its scn
				int index = (*((int*)iWMouseXPDevicesIndex.GetAt(0)));

				CWidcommFoundDevice* dev = (CWidcommFoundDevice*) iFoundDevices.GetAt(index);					
				
				CString str = "Connecting to WMouseXP on ";
				str += dev->iName;									
				iLabel->SetWindowText(str);

				
				if (! m_RfCommIf.AssignScnValue(&iDiscoveringGUID,dev->iWMouseXPscn,m_serviceName))
				{
					iLabel->SetWindowText("Assign scn failed");
					RevertToIdle();				
					return;
				}

				 UINT8 sec_level = BTM_SEC_NONE;

				if (! m_RfCommIf.SetSecurityLevel(m_serviceName, sec_level , false))
				{
					iLabel->SetWindowText("Error setting security level");
					RevertToIdle();				
					return;					
				}


				CopyBDADDR(dev->iAddr,iDiscoveringDevice);

				if(PORT_RETURN_CODE::SUCCESS!= OpenClient(dev->iWMouseXPscn, dev->iAddr))
				{
					iLabel->SetWindowText("Error opening rfcomm connection");
					RevertToIdle();				
					return;					
				}
					

					


			}
			else
			if(iWMouseXPDevicesIndex.GetSize()<=0)
			{
				//show message
				if(!iUsingPrevDev)
				{
					iLabel->SetWindowText("Can't find any device with WMouseXP running");
					MessageBox(iHwnd,"Can't find any device with WMouseXP running:\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\").\r\n- Make sure your phone's WMouseXP (Mobile Side) is started and displaying \"Awaiting PC Side\".\r\n- Try Again.","Connect Failed",MB_OK | MB_ICONASTERISK);
				}
				else
				{
					iLabel->SetWindowText("Can't connect to WMouseXP on the Previous Phone");
					MessageBox(iHwnd,"Can't connect to WMouseXP on the Previous Phone:\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\").\r\n- Make sure your phone's WMouseXP (Mobile Side) is started and displaying \"Awaiting PC Side\".\r\n- If you don't want to use the previous phone, uncheck the \"Previous Phone\" box.\r\n- Try Again.","Connect Failed",MB_OK | MB_ICONASTERISK);
				}
				
				
				RevertToIdle();				
			}
			else// more than one device with WMouseXP found
			{				
				//let user choose the device like the msstack implementation
				int SelectedDev=-1;
				CSelectPhoneDialog dlg(EWidcommStack,&iFoundDevices,SelectedDev,CWMouseXPDlg::curInstance);

				if(IDOK == dlg.DoModal() && SelectedDev>=0)
				{
					int index = (*((int*)iWMouseXPDevicesIndex.GetAt(SelectedDev)));

					CWidcommFoundDevice* dev = (CWidcommFoundDevice*) iFoundDevices.GetAt(index);					
					
					CString str = "Connecting to WMouseXP on ";
					str += dev->iName;									
					iLabel->SetWindowText(str);

					
					if (! m_RfCommIf.AssignScnValue(&iDiscoveringGUID,dev->iWMouseXPscn,m_serviceName))
					{
						iLabel->SetWindowText("Assign scn failed");
						RevertToIdle();				
						return;
					}

					 UINT8 sec_level = BTM_SEC_NONE;

					if (! m_RfCommIf.SetSecurityLevel(m_serviceName, sec_level , false))
					{
						iLabel->SetWindowText("Error setting security level");
						RevertToIdle();				
						return;					
					}


					CopyBDADDR(dev->iAddr,iDiscoveringDevice);

					if(PORT_RETURN_CODE::SUCCESS!= OpenClient(dev->iWMouseXPscn, dev->iAddr))
					{
						iLabel->SetWindowText("Error opening rfcomm connection");
						RevertToIdle();				
						return;					
					}
				}
				else
				{				
					RevertToIdle();				
				}



			}			
		
		}
		
	
	}
		
	
}
*/

void CBlueSoleilHandler::OnInquiryComplete ()
{

//	StopBTInteractionTimer();

	Sleep (100);
	

	if(iFoundDevices.GetSize() <=0 )
	{
		//CString str(((CWidcommFoundDevice*) iFoundDevices.GetAt(0))->iName);
		
		//TODO: get HWND so message box can be seen
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
		return;
	}

	

	if(iRequestedCommand == ESendFileSession)
	{	
		int SelectedDev=-1;
		iIcon->Stop();
		
		//let user choose from listbox
		CSelectPhoneDialog dlg(EBlueSoleilStack,&iFoundDevices,SelectedDev,CWMouseXPDlg::curInstance);

		if(IDOK == dlg.DoModal() && SelectedDev>=0)
		{
			//do obexop to the selected device
			CString dsp("Discovering services on ");

			CBlueSoleilFoundDevice* dev = (CBlueSoleilFoundDevice*) iFoundDevices.GetAt(SelectedDev);
			
			dsp += dev->iBTDevInfo.szName;

			iLabel->SetWindowText(dsp);

			CopyBDADDR(dev->iBTDevInfo.address,iDiscoveringDevice);

			iDiscoverCommandSent = true;

			//OBEX OBJECT PUSH GUID
			iDiscoveringGUID.Data1 =	0x00001105;
			iDiscoveringGUID.Data2 =	0x0000;
			iDiscoveringGUID.Data3 =	0x1000;
			iDiscoveringGUID.Data4[0] = 0x80;
			iDiscoveringGUID.Data4[1] = 0x00;
			iDiscoveringGUID.Data4[2] = 0x00;
			iDiscoveringGUID.Data4[3] = 0x80;
			iDiscoveringGUID.Data4[4] = 0x5F;
			iDiscoveringGUID.Data4[5] = 0x9B;
			iDiscoveringGUID.Data4[6] = 0x34;
			iDiscoveringGUID.Data4[7] = 0xFB;
			



			int obexchannel = SDK_BtSearchSPPExServices(iDiscoveringDevice, (BYTE*) &iDiscoveringGUID,(dev->iSPPInfo) );

			if( obexchannel > 0)
			{
				/*CString sc;
				sc.Format("channel %d",obexchannel);
				AfxMessageBox(sc);
				*/










	
					//change icon to blue and start the icon
					//extract the jar to a temporary location...
					//do obex op to this device using this sdp record

				iIcon->UnLoad();
				iIcon->Load(MAKEINTRESOURCE(iSendAnim),iAnimRscType);
				iIcon->Draw();

					iLabel->SetWindowText("Sending File...");
				
					
					///////////////////////////////////////////////////
						//make jar and send to device


						HGLOBAL hResourceLoaded;		// handle to loaded resource 
						HRSRC hRes;						// handle/ptr. to res. info. 
						BYTE* lpResLock;				// pointer to resource data 
						DWORD dwSizeRes;
						

						hRes = FindResource( NULL, MAKEINTRESOURCE(IDR_JAR3), "JAR" );
						
						if(hRes==NULL)
							MessageBox(iHwnd,"Internal resource missing.","Send Failed",MB_OK | MB_ICONASTERISK);
						// loads the specified resource into global memory. 
						hResourceLoaded = LoadResource( NULL, hRes ); 

						// get a pointer to the loaded resource!
						lpResLock = (BYTE*)LockResource( hResourceLoaded ); 

						// determine the size of the resource, so we know how much to write out to file!  
						dwSizeRes = SizeofResource( NULL, hRes );

						

						CFile file;
						CString path(iTmpObexFilePath);
						
						

						if(!file.Open( path, CFile::modeCreate | CFile::modeWrite|CFile::typeBinary))
						{
							//showerr
							MessageBox(iHwnd,"Create temp file failed.","Send Failed",MB_OK | MB_ICONASTERISK);
							RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
							return;	

						}
						else // jar constructed...
						{
						
							//write to file
							file.Write(lpResLock,dwSizeRes);
							file.Flush();
							file.Close();	
																	
							
							//send file via OBEX object push to this device...
							
							/*			DWORD BT_ConnectService(
						 [in]  PBLUETOOTH_DEVICE_INFO pDevInfo, 
						 [in]  PGENERAL_SERVICE_INFO pServiceInfo,
						 [in, out]  BYTE* lpParam,
						 [out]  DWORD* lpConnectionHandle
						);*/

							BLUETOOTH_DEVICE_INFO btdev;
							memset(&btdev,0,sizeof(BLUETOOTH_DEVICE_INFO));				
							btdev.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
							CopyBDADDR(iDiscoveringDevice,btdev.address);

							GENERAL_SERVICE_INFO sinfo;
							memset(&sinfo,0,sizeof(GENERAL_SERVICE_INFO));				
							sinfo.dwSize = sizeof(GENERAL_SERVICE_INFO);
							sinfo.wServiceClassUuid16 = CLS_OBEX_OBJ_PUSH;

							OPP_CLIENT_PARAM oppparam;
							memset(&oppparam,0,sizeof(OPP_CLIENT_PARAM));				
							oppparam.dwSize = sizeof(OPP_CLIENT_PARAM);
							oppparam.wCmdFlags = OPP_COMMAND_PUSH;	
							StringCbCopy(oppparam.szObjectPath,MAX_PATH,iTmpObexFilePath);
							
							DWORD obexhandle = 0;

							if(BTSTATUS_SUCCESS == BT_ConnectService(&btdev, &sinfo, (BYTE*) &oppparam, &obexhandle) )
							{
								//AfxMessageBox("File  success");
							}
							else
							{
								AfxMessageBox("Send Failed...\r\n\r\nPlease see \"Help\" for other installation methods:\r\n- Open \"wap.clearevo.com\" in your mobile's WAP browser and download WMouseXP directly.\r\n- Go to the installation path of this program and send the \"WMouseXP.jar\" via Bluetooth or phone's PC-Suite manually.");
							}

							
													
						}

				
			}
			else  //obexop not found
			{
				this->iLabel->SetWindowText("OBEX Object Push not found on device.");
				MessageBox(iHwnd,"OBEX Object Push not found on selected device: WMouseXP can't send the mobile-side installer to it.\r\n\r\nPlease see \"Help\" for other installation methods:\r\n- Open \"wap.clearevo.com\" in your mobile's WAP browser and download WMouseXP directly.\r\n- Go to the installation path of this program and send the \"WMouseXP.jar\" via Bluetooth or phone's PC-Suite manually.","Send Failed",MB_OK | MB_ICONASTERISK);
				
				RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
				return;								
			}
				

			
			RevertToIdle();

			
			/*
			iIcon->UnLoad();
			iIcon->Load(MAKEINTRESOURCE(iSendAnim),iAnimRscType);
			iIcon->Draw();

			//Sleep(3000);	// L2CAP needs at least 1 sec to clear previous discovery connection

			if(	StartDiscovery(dev->iAddr, &ObexObjectPush_guid) )
				;
				///Sleep(2000);
			else
			{
				
				MessageBox(iHwnd,"General discovery fault: Try again soon.","Discovey Fault",MB_OK | MB_ICONASTERISK);
				RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
				return;	
			}

			*/


		}
		else //dialog cancelled
		{
			RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
			return;	
		}
	}
	else //if it's a remote-control session
	{
		//start discovering for WMouseXP service on all devices.			

		
			DoDiscoverWMouseXPThroughDeviceList();	

	}


}




void CBlueSoleilHandler::DoDiscoverWMouseXPThroughDeviceList()
{
			
				//set to WMouseXP GUID

				iDiscoveringGUID.Data1 = 0x2bc2b92e;
				iDiscoveringGUID.Data2 = 0x3992;
				iDiscoveringGUID.Data3 = 0x11dc;
				iDiscoveringGUID.Data4[0] = 0x83;
				iDiscoveringGUID.Data4[1] = 0x14;
				iDiscoveringGUID.Data4[2] = 0x08;
				iDiscoveringGUID.Data4[3] = 0x00;
				iDiscoveringGUID.Data4[4] = 0x20;
				iDiscoveringGUID.Data4[5] = 0x0c;
				iDiscoveringGUID.Data4[6] = 0x9a;			
				iDiscoveringGUID.Data4[7] = 0x66;			

				
				iCurDevice = 0;

				for(int i=0; i< iFoundDevices.GetSize(); i++)
				{
											
					CString dsp("Discovering services on ");
					CBlueSoleilFoundDevice* dev = (CBlueSoleilFoundDevice*) iFoundDevices.GetAt(i);			
					dsp += dev->iBTDevInfo.szName;

					iLabel->SetWindowText(dsp);

					CopyBDADDR(dev->iBTDevInfo.address,iDiscoveringDevice);

					/*CString h;
			h.Format("%x %x %x %x %x %x",iDiscoveringDevice[0],iDiscoveringDevice[1],iDiscoveringDevice[2],iDiscoveringDevice[3],iDiscoveringDevice[4],iDiscoveringDevice[5]);
			AfxMessageBox(h);*/

					iDiscoverCommandSent = true;

					int wmxpchannel = SDK_BtSearchSPPExServices(iDiscoveringDevice, (BYTE*) &iDiscoveringGUID,(dev->iSPPInfo) );

					if( wmxpchannel > 0)
					{
						//dev->iWMouseXPscn = obexchannel; already copied to "dev->iSPPInfo"

						this->iWMouseXPDevicesIndex.Add(new int(i));
					}					
				
				}





			//handle the discovery result

			if(iWMouseXPDevicesIndex.GetSize()==1)
			{
				//connect rfcomm to its scn
				int index = (*((int*)iWMouseXPDevicesIndex.GetAt(0)));

				CBlueSoleilFoundDevice* dev = (CBlueSoleilFoundDevice*) iFoundDevices.GetAt(index);					
				
				CString str = "Connecting to WMouseXP on ";
				str += dev->iBTDevInfo.szName;									
				iLabel->SetWindowText(str);

					
				// iConnectedDevice is used to mean the current device... for event matching, etc...
				CopyBDADDR(dev->iBTDevInfo.address,iConnectedDevice);

				//BlueSoleil Connect using com port		


					/*			DWORD BT_ConnectSPPExService (
				 [in]  PBLUETOOTH_DEVICE_INFO pDevInfo,
				 [in][out]  PSPPEX_SERVICE_INFO pServiceInfo,
				 [out]  DWORD* lpConnectionHandle
				);*/


				//if(dev->iSPPInfo.szServiceName[3] == ((CHAR*)&iDiscoveringGUID)[3])
				//	AfxMessageBox("same");
				//else
				//	AfxMessageBox("not same");

				DWORD dwResult;
				dwResult = BT_ConnectSPPExService(&(dev->iBTDevInfo),&(dev->iSPPInfo),&iConnectionHandle);
//				PrintError("BT_SearchSPPExServices",dwResult);
	
				if(dwResult == BTSTATUS_SUCCESS && StartIoOnComport(iConnectionHandle,dev->iSPPInfo.ucComIndex, TRUE))
				{

					BYTE reversed[6]; //BlueSoleil uses reversed order with other stacks
					for(int i=0;i<6;i++)
					{
						//BlueSoleil uses opposite direction of byte ordering...
						reversed[i] = iConnectedDevice[5-i];						
					}

					iCaller->OnConnected(reversed,6);
				
				
				}
				else
				{
					CString str;
					str = "Failed to connect to ";
					str += dev->iBTDevInfo.szName;
					str += "\r\nThis is probably a common temporary failure.\r\n\r\nPlease try again...";
					RevertToIdle();	
					

					iLabel->SetWindowText(str);

					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
					AfxMessageBox(str);
					else
					if(CWMouseXPDlg::curInstance)
						CWMouseXPDlg::curInstance->AddLog(str);
					

				}

		


			}
			else
			if(iWMouseXPDevicesIndex.GetSize()<=0)
			{
				//show message
				if(!iUsingPrevDev)
				{
					iLabel->SetWindowText("Can't find any device with WMouseXP running");

					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						MessageBox(iHwnd,"Can't find any device with WMouseXP running:\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\").\r\n- Make sure your phone's WMouseXP (Mobile Side) is started and displaying \"Awaiting PC Side\".\r\n- Try Again.","Connect Failed",MB_OK | MB_ICONASTERISK);
				}
				else
				{
					iLabel->SetWindowText("Can't connect to WMouseXP on the Previous Phone");

					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						MessageBox(iHwnd,"Can't connect to WMouseXP on the Previous Phone:\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\").\r\n- Make sure your phone's WMouseXP (Mobile Side) is started and displaying \"Awaiting PC Side\".\r\n- If you don't want to use the previous phone, uncheck the \"Previous Phone\" box.\r\n- Try Again.","Connect Failed",MB_OK | MB_ICONASTERISK);
				}
				
				
				RevertToIdle();				
			}
			else// more than one device with WMouseXP found
			{				
				//let user choose the device like the msstack implementation
				int SelectedDev=-1;
				CSelectPhoneDialog dlg(EBlueSoleilStack,&iFoundDevices,SelectedDev,CWMouseXPDlg::curInstance);

				if(IDOK == dlg.DoModal() && SelectedDev>=0)
				{
					//int index = (*((int*)iWMouseXPDevicesIndex.GetAt(SelectedDev))); That's for MS Stack style...
					int index = SelectedDev;

					CBlueSoleilFoundDevice* dev = (CBlueSoleilFoundDevice*) iFoundDevices.GetAt(index);					
					
					CString str = "Connecting to WMouseXP on ";
					str += dev->iBTDevInfo.szName;									
					iLabel->SetWindowText(str);




				// iConnectedDevice is used to mean the current device... for event matching, etc...
				CopyBDADDR(dev->iBTDevInfo.address,iConnectedDevice);


					///////////////////BlueSoleil Connect using com port

					DWORD dwResult;
					dwResult = BT_ConnectSPPExService(&(dev->iBTDevInfo),&(dev->iSPPInfo),&iConnectionHandle);
	//				PrintError("BT_SearchSPPExServices",dwResult);
		
					if(dwResult == BTSTATUS_SUCCESS && StartIoOnComport(iConnectionHandle,dev->iSPPInfo.ucComIndex, TRUE))
					{

						BYTE reversed[6]; //BlueSoleil uses reversed order with other stacks
						for(int i=0;i<6;i++)
						{
							//BlueSoleil uses opposite direction of byte ordering...
							reversed[i] = iConnectedDevice[5-i];						
						}

						iCaller->OnConnected(reversed,6);
					
					
					}
					else
					{
						CString str;
						str = "Failed to connect to ";
						str += dev->iBTDevInfo.szName;
						str += "\r\nThis is probably a common temporary failure.\r\n\r\nPlease try again...";
						RevertToIdle();	
						

						iLabel->SetWindowText(str);
						
						if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
						AfxMessageBox(str);
						else
						if(CWMouseXPDlg::curInstance)						
							CWMouseXPDlg::curInstance->AddLog(str);

						

					}
					/////////////////////////////////////////


				}
				else
				{				
					RevertToIdle();				
				}



			}

				
				

}


void CBlueSoleilHandler::DoAddDevice(BLUETOOTH_DEVICE_INFO& dev)
{
	
	//don't add if alraedy added
	for(int i=0;i<iFoundDevices.GetSize();i++)
	{		
		if(IsSameBDADDR( dev.address ,((CBlueSoleilFoundDevice*) iFoundDevices.GetAt(i))->iBTDevInfo.address ))
		{
			//copy the latest name
			
			if(	((CBlueSoleilFoundDevice*) iFoundDevices.GetAt(i))->iBTDevInfo.szName[0] == 0)
				for(int j=0;j<MAX_DEVICE_NAME_LENGTH;j++)
					{
						((CBlueSoleilFoundDevice*) iFoundDevices.GetAt(i))->iBTDevInfo.szName[j] = dev.szName[j];
					}

			return;
		}
	}
	
	
	iFoundDevices.Add(new CBlueSoleilFoundDevice(&dev));

	CString s;
	s.Format("Found %d devices, please wait...",iFoundDevices.GetSize());

	iLabel->SetWindowText(s);
	
 }

  
void CBlueSoleilHandler::RevertToIdle()
{
	if(!iButton)
		return;

	

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
		
		if(iConnectionHandle>0)
		{
			BT_DisconnectSPPExService(iConnectionHandle);
			StopIoOnComport(iConnectionHandle);
			iConnectionHandle = 0;
			memset(iConnectedDevice,0,6);
		}

		this->iButtonToDisable->EnableWindow(true);
		this->iButtonToDisable->SetWindowText(CString(KNotInstalledOnPhoneStr));
		
		iButton->EnableWindow(true);	
		iButton->SetWindowText("Connect");	
	}


	iIcon->Stop();
	iIcon->ShowWindow(false);
	iIcon->UnLoad();

	iButton = NULL;

	CloseAndRevertEngineState();

	//CWMouseXPDlg::curInstance->StartNokiaPCSuiteServiceLayer(); start and stop too frequently makes inquiry fail

}

void CBlueSoleilHandler::OnData(BYTE* data, int len)
{
	if(iConnectionHandle==0 || iRequestedCommand != ERemoteControlSession)
		return;

	/*CString s;

	for(int i=0;i<len;i++)
	{
		CString num;
		num.Format("%x ",data[i]);
		s+=num;
	}

	iLabel->SetWindowText(s);*/
	
	if(len>0)
		iCaller->OnData(data,len);


}

void CBlueSoleilHandler::OnSPPConnectionStatus(DWORD dwServerHandle, BYTE* lpBdAddr, UCHAR ucStatus,DWORD dwConnetionHandle)
{
	

	if(iConnectionHandle==0 || iRequestedCommand != ERemoteControlSession)
		return;

	
	
	if(dwConnetionHandle != iConnectionHandle) //if not related to our connection
		return;
	
	


	if(ucStatus == STATUS_OUTGOING_DISCONNECT && IsSameBDADDR(lpBdAddr, iConnectedDevice) )
	{
		
		//If spp disconnected, stop IO on the COM port
	
		if(iConnectionHandle>0)
		{
			

			BT_DisconnectSPPExService(iConnectionHandle);
			StopIoOnComport(iConnectionHandle);
			iConnectionHandle = 0;
			memset(iConnectedDevice,0,6);
			
			iCaller->OnDisconnected();	//use this instead of only RevertToIdle so main dlg can call stoptrialtimer also!
			RevertToIdle();

		}

		
	}


}

void CBlueSoleilHandler::OnError(DWORD dwErrorCode)
{

	if(iConnectionHandle==0 || iRequestedCommand != ERemoteControlSession)
		return;
	
		if(iConnectionHandle>0)
		{


			BT_DisconnectSPPExService(iConnectionHandle);
			StopIoOnComport(iConnectionHandle);
			iConnectionHandle = 0;
			memset(iConnectedDevice,0,6);
			
			iCaller->OnDisconnected();//use this instead of RevertToIdle so main dlg can call stoptrialtimer also!
			RevertToIdle();

		}

	

}

/*void CBlueSoleilHandler::OnPushResponse(OPP_RESULT_CODE result_code,  BD_ADDR bda, WCHAR * string)
{
	if(COMPLETED == result_code)
	{
		iLabel->SetWindowText("File sent successfully.");
		//MessageBox(iHwnd,"WMouseXP Mobile-Side Installer was successfully sent via Bluetooth.","File Sent",MB_OK | MB_ICONASTERISK);		
		
		
		//CString path(iTmpObexFilePath);
		//DeleteFile(path);



	}
	else
	{
		CString str("File send failed");
		iLabel->SetWindowText(str);

		MessageBox(iHwnd,"File send failed.\r\n\r\nWMouseXP can't send the mobile-side installer to it. Please see the Help on how to install the mobile side manually.","Send Failed",MB_OK | MB_ICONASTERISK);

	}

	this->RevertToIdle();

}


void CBlueSoleilHandler::OnDataReceived (void *p_data, UINT16 len)
{
	if(iRequestedCommand !=  ERemoteControlSession)
		return;
	
	
	iCaller->OnData(p_data,len);

}
*/
//////////////////////////////////////////////////////////////////////
//
//  Called when and event is recieved on the port.
//
/*void CBlueSoleilHandler::OnEventReceived (UINT32 event_code)
{
	if(iRequestedCommand !=  ERemoteControlSession)
		return;
    // Was a connection established?
    //
	iCaller->OnConnectEvent(event_code,iDiscoveringDevice);    

}*/


void CBlueSoleilHandler::CloseAndRevertEngineState()
{
	iRequestedCommand = EStateIdle;

	if(iConnectionHandle>0)
		{
			StopIoOnComport(iConnectionHandle);	
			BT_DisconnectSPPExService(iConnectionHandle);			
			iConnectionHandle = 0;

			memset(iConnectedDevice,0,6);
		}
	
	iButton = NULL;//this is another flag used to check the state of the engine 
	SDK_CancelInquiry();
	Sleep(100);	
	StopIoOnComport(NULL);
}


void CBlueSoleilHandler::TimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
)
{
	
	//check if socket is connected and the iLastBTInteraction is older than INACTIVITY_TIMEOUT_MILLIS then close the socket
	//start new session if still in announce state
	
	if(!curInstance)
		return;

	
	if(!(curInstance->StopBTInteractionTimer()))
		return; //it was already stopped

	switch(curInstance->iState)
	{
	
	case EHandlerStateDoingInquiry:
		
		//AfxMessageBox("cancelling");
		SDK_CancelInquiry();
		Sleep(100);
		//TODO: curInstance->OnInquiryComplete(false,-1);//NOT FAILED - IT'S NORMAL THAT WIDCOMM SOMETIMES DOESN'T REPORT OnInquiryComplete
		
		break;
	
	case EHandlerStateDoingServideDiscovery:
		break;
	
	case EHandlerStateSendingFile:
		break;
	
	case EHandlerStateIdle:
	default:
		break;

	}

	//curInstance->iState = EHandlerStateIdle;
}


bool CBlueSoleilHandler::StartBTInteractionTimer()
{	
	if(m_TimerID)
		KillTimer(iHwnd,m_TimerID);

	m_TimerID = 0;

	m_TimerID = SetTimer(iHwnd,57/*any identifier because we have only one timer*/, INACTIVITY_TIMEOUT_SECONDS * 1000, TimerProc);

	if(m_TimerID)
		return true;	

	return false;
}



bool CBlueSoleilHandler::StopBTInteractionTimer()
{
	if(m_TimerID && KillTimer(iHwnd,m_TimerID))
	{
		m_TimerID = 0;
		return true;
	}

	return false;
}
