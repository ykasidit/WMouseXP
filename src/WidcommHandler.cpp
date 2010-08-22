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


// WidcommHandler.cpp: implementation of the CWidcommHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WMouseXP.h"
#include "WMouseXPdlg.h"

#include "WidcommHandler.h"
#include "widcommfounddevice.h"
#include "selectphonedialog.h"

#include "log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


static GUID ObexObjectPush_guid = CBtIf::guid_SERVCLASS_OBEX_OBJECT_PUSH;

CWidcommHandler* CWidcommHandler::curInstance = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool IsSameBDADDR(BD_ADDR b1,BD_ADDR b2)
{
	for(int i=0;i<BD_ADDR_LEN;i++)
	{
		if(b1[i]!=b2[i])
			return false;
	}

	return true;
}

void CopyBDADDR(BD_ADDR src, BD_ADDR dest)
{
	for(int i=0;i<BD_ADDR_LEN;i++)
	{
		dest[i]= src[i];
	}
}



CWidcommHandler::CWidcommHandler()
{
	this->iButton =NULL;
	this->iLabel = NULL;
	this->iFileData = NULL;
	this->iFileLen = 0;
	iDiscoverCommandSent = false;

	iCaller = NULL;

	
	GetModuleFileNameW(AfxGetInstanceHandle(), iTmpObexFilePath, MAX_PATH);
	
	 /* Search backward. */
	wchar_t ch('\\');
	wchar_t* pdest = wcsrchr( iTmpObexFilePath, ch );

	if( pdest != NULL )
	{
		if( MAX_PATH > pdest - iTmpObexFilePath)
			*(pdest+1) = NULL;
	}	
	
	
	wcscat(iTmpObexFilePath, KWmxpJarPathRelativeToReleaseExe_wc );

	
	//GetFullPathNameW(_L("WMouseXP.jar"),KPATHMAXLEN,iTmpObexFilePath,NULL);
	
	//GetTempPathW(KPATHMAXLEN,iTmpObexFilePath);

	/*(*__argv);
	
	*/

	//wcscat(iTmpObexFilePath,_L("WMouseXP.jar"));

	

	iUsingPrevDev = false;

	m_TimerID = 0;

	iState = EHandlerStateIdle;



}

CWidcommHandler* CWidcommHandler::GetInstance()
{
	curInstance=NULL;

	if(WidcommSupported())
	{
		curInstance = new CWidcommHandler();

			char sdkver[MAX_PATH];
			int sdkverlen=0;

			if(GetDKVersionInfo(sdkver,sdkverlen))
			{	
			sdkver[63] = NULL;
			mydebug::log(sdkver);
			}

			if(GetBTWVersionInfo(sdkver,sdkverlen))
			{	
			sdkver[63] = NULL;
			mydebug::log(sdkver);
			}
	}
	
	return curInstance;
}

bool CWidcommHandler::WidcommSupported()
{
	HMODULE h = LoadLibrary("wbtapi.dll");
		if (h == NULL) {				
		return false;
	}
	FreeLibrary(h);	
	return true;
}

CWidcommHandler::~CWidcommHandler()
{
	CloseAndRevertEngineState();

	iFoundDevices.RemoveAll();
}

bool CWidcommHandler::StartRemoteControlSession(HWND hwnd, CButton* buttontodisable, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, MRemoteControlCaller* caller)
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
	
	StopInquiry();	
	Sleep(100);
	//Close();

	CWMouseXPDlg* that= (CWMouseXPDlg*) caller;

	iUsingPrevDev = that->m_PrevDevCheckBox.IsWindowEnabled() && that->m_PrevDevCheckBox.GetCheck() == BST_CHECKED;

	if(!iUsingPrevDev)
	{
		iState = EHandlerStateDoingInquiry;
		StartBTInteractionTimer();
		return StartInquiry();
	}
	else
	{
		for(int i=0;i<BD_ADDR_LEN;i++)
		{
				this->iDiscoveringDevice[i]= that->iPrevDev[i];
		}

		CString s(that->iPrevDev);
		
		

		

		if(s.GetLength()!=19)
		{
			iLabel->SetWindowText("Invalid previous device address. Searching instead");
			
			iState = EHandlerStateDoingInquiry;
			StartBTInteractionTimer();
			return StartInquiry();
	
		}

		s.Delete(0);//(
		s.Delete(s.GetLength()-1);//)
		
		int found;

		while((found = s.Find(':'))>=0)
			s.Delete(found);

			

		
		for(i=0;i<6;i++)
		{
			CString hexpart = s.Mid(i*2,2);
			
			iDiscoveringDevice[i] = _tcstoul(hexpart, 0, 16);
		}
	
		DoAddDevice(iDiscoveringDevice, (unsigned char*)("Previous Device"));		
		iCurDevice = 0; //start going through all the devices
		return DoDiscoverWMouseXPThroughDeviceList();	
	}
	 


}

bool CWidcommHandler::StartSendFileSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label)
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

	StopInquiry();
	Sleep(100);
	Close();



		iState = EHandlerStateDoingInquiry;
		StartBTInteractionTimer();
		return StartInquiry();
	
	
	 
	//do inquiry
	//show dialog to choose dev
	//do obexop to selected dev

	
}


bool CWidcommHandler::RFCOMMWrite(BYTE* data, USHORT len, USHORT& lenwritten)
{
	if(CRfCommPort::SUCCESS == Write(data,len,&lenwritten))
		return true;

	return false;

}

void CWidcommHandler::OnDeviceResponded (BD_ADDR bda, DEV_CLASS devClass, BD_NAME bdName, BOOL bConnected)
{
    
    // Add the device address and name (if any)
    // to the Server List.  It is OK to pass
    // duplicates.  This method will ignore them.
    //

    DoAddDevice(bda, bdName);	
	
}

static char m_serviceName[BT_MAX_SERVICE_NAME_LEN + 1] = "WMouseXPService";


void CWidcommHandler::OnDiscoveryComplete()
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
		if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))						
			MessageBox(iHwnd,"Discovery Failed","Connect Failed",MB_OK | MB_ICONASTERISK);
	}


    services_found = ReadDiscoveryRecords(iDiscoveringDevice,MAX_SERVICES_PER_DEVICE,SdpRecs,&iDiscoveringGUID);


	
	if(iRequestedCommand == ESendFileSession)
	{
	
		if(services_found<=0)
		{
			this->iLabel->SetWindowText("OBEX Object Push not found on device.");
			MessageBox(iHwnd,"OBEX Object Push not found on selected device.\r\n\r\nWMouseXP can't send the mobile-side installer to it. \r\n\r\nPlease see \"Help\" for other installation methods:\r\n- Open \"wap.clearevo.com\" in your mobile's WAP browser and download WMouseXP directly.\r\n- Go to the installation path of this program and send the \"WMouseXP.jar\" via Bluetooth or phone's PC-Suite manually.","Send Failed",MB_OK | MB_ICONASTERISK);
			
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
					


					
						{
						
						
							
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
			MessageBox(iHwnd,"No valid OBEX Object Push channels on device.\r\n\r\nWMouseXP can't send the mobile-side installer to it. \r\n\r\nPlease see \"Help\" for other installation methods:\r\n- Open \"wap.clearevo.com\" in your mobile's WAP browser and download WMouseXP directly.\r\n- Go to the installation path of this program and send the \"WMouseXP.jar\" via Bluetooth or phone's PC-Suite manually.","Send Failed",MB_OK | MB_ICONASTERISK);
			
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
					/*dont give up - try continue - case from "Sébastien DOBLER" <sebastien.dobler@yahoo.fr>
					iLabel->SetWindowText("Error setting security level");
					RevertToIdle();				
					return; */
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
				CSelectPhoneDialog dlg(EWidcommStack,&iFoundDevices,SelectedDev,CWMouseXPDlg::curInstance);

				if(IDOK == dlg.DoModal() && SelectedDev>=0)
				{
					//int index = (*((int*)iWMouseXPDevicesIndex.GetAt(SelectedDev))); That's for MS Stack style...
					int index = SelectedDev;

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

void CWidcommHandler::OnInquiryComplete (BOOL success, short num_responses)
{

	StopBTInteractionTimer();

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
		CSelectPhoneDialog dlg(EWidcommStack,&iFoundDevices,SelectedDev,CWMouseXPDlg::curInstance);

		if(IDOK == dlg.DoModal() && SelectedDev>=0)
		{
			//do obexop to the selected device
			CString dsp("Discovering services on ");

			CWidcommFoundDevice* dev = (CWidcommFoundDevice*) iFoundDevices.GetAt(SelectedDev);
			
			dsp += dev->iName;

			iLabel->SetWindowText(dsp);

			CopyBDADDR(dev->iAddr,iDiscoveringDevice);

			iDiscoverCommandSent = true;

			iDiscoveringGUID = ObexObjectPush_guid;
			
			
			iIcon->UnLoad();
			iIcon->Load(MAKEINTRESOURCE(iSendAnim),iAnimRscType);
			iIcon->Draw();

			//Sleep(3000);	// L2CAP needs at least 1 sec to clear previous discovery connection

			if(	StartDiscovery(dev->iAddr, &ObexObjectPush_guid) )
				/*Sleep(2000)*/;
			else
			{
				if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))										
					MessageBox(iHwnd,"General discovery fault: Try again soon.","Discovey Fault",MB_OK | MB_ICONASTERISK);

				RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
				return;	
			}




		}
		else
		{
			RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
			return;	
		}
	}
	else
	{
		//start discovering for WMouseXP service on all devices.			

			iCurDevice = 0; //start going through all the devices

			DoDiscoverWMouseXPThroughDeviceList();	
	}


}


bool CWidcommHandler::DoDiscoverWMouseXPThroughDeviceList()
{
			if(iCurDevice < iFoundDevices.GetSize())
			{
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


				CString dsp("Discovering services on ");

				CWidcommFoundDevice* dev = (CWidcommFoundDevice*) iFoundDevices.GetAt(iCurDevice);
				
				dsp += dev->iName;

				iLabel->SetWindowText(dsp);

				CopyBDADDR(dev->iAddr,iDiscoveringDevice);

				iDiscoverCommandSent = true;

				Sleep(3000);	// L2CAP needs at least 1 sec to clear previous discovery connection
				
				if(	StartDiscovery(dev->iAddr, &iDiscoveringGUID) )
					/*Sleep(2000)*/;
				else
				{
					if(CWMouseXPDlg::curInstance && !(CWMouseXPDlg::curInstance->iAutoConnectCommand))							
						MessageBox(iHwnd,"General discovery fault: Try again soon.","Discovey Fault",MB_OK | MB_ICONASTERISK);

					RevertToIdle(); //add hwnd, icon movement, icon animation rsc numbers, etc...
					return false;	
				}

				
				iCurDevice++;

				return true;
				

			}
			else
				return false;
}

void CWidcommHandler::DoAddDevice(BD_ADDR p_bda, BD_NAME bd_name)
{
	
	//don't add if alraedy added
	for(int i=0;i<iFoundDevices.GetSize();i++)
	{		
		if(IsSameBDADDR( p_bda,((CWidcommFoundDevice*) iFoundDevices.GetAt(i))->iAddr ))
		{
			//copy the latest name
			for(int j=0;j<BD_NAME_LEN;j++)
			{
				((CWidcommFoundDevice*) iFoundDevices.GetAt(i))->iName[j] = bd_name[j];
			}
			return;
		}
	}
	
	
	iFoundDevices.Add(new CWidcommFoundDevice(p_bda,bd_name));
	

	CString s;
	s.Format("Found %d devices, please wait...",iFoundDevices.GetSize());

	iLabel->SetWindowText(s);


 /*   CString item_text;
    int     item_count = m_num_devices;

    // If the bda is a duplicate then don't add it.
    //
    for (int x=0; x < m_num_devices; x++)
    {
        CBdInfo* p_CmpInfo = (CBdInfo *) m_pDevInfo[x];

        // Is it a duplicate?
        if ( p_CmpInfo->isBdAddrEqual(p_bda))
        {
            // Yes! If we don't have a name
            // yet and this time we were given
            // one, update the info and reset the
            // item text.
            if (p_CmpInfo->m_Name.GetLength() == 0)
            {
                p_CmpInfo->m_Name = bd_name;
                m_ServerList.SetItemText(x, 0, p_CmpInfo->LabelAsString());
            }

            return;
        }
    }

    CBdInfo* p_Info = new CBdInfo(p_bda, bd_name);

	if (m_num_devices < MAX_SERVER_DEVICES)
	{
		// add this device to the list
		m_pDevInfo[m_num_devices] = p_Info;
		m_num_devices++;

		SetDlgItemText(IDC_STATUS_TEXT, "Inquiry found " + p_Info->DeviceAsString());
		Sleep(2000);
	}
	else
	{
		SetDlgItemText(IDC_STATUS_TEXT, "SERVER LIST FULL:  '" + p_Info->DeviceAsString() + "'  NOT INCLUDED!");
		delete p_Info;	// don't need this anymore
		Sleep(5000);
	}*/
}

void CWidcommHandler::RevertToIdle()
{
	if(!iButton)
		return;

	

	if(iRequestedCommand == ESendFileSession)
	{
	this->iButtonToDisable->EnableWindow(true);
	this->iButtonToDisable->SetWindowText("Connect");
	iLabel->SetWindowText("- Make sure it shows \"Awaiting PC Side\".");
	iButton->EnableWindow(true);	
	iButton->SetWindowText(KNotInstalledOnPhoneStr);
	}
	else
	{		
	this->iButtonToDisable->EnableWindow(true);
	this->iButtonToDisable->SetWindowText(KNotInstalledOnPhoneStr);
	
	iButton->EnableWindow(true);	
	iButton->SetWindowText("Connect");	
	}


	iIcon->Stop();
	iIcon->ShowWindow(false);
	iIcon->UnLoad();

	iButton = NULL;

	//CWMouseXPDlg::curInstance->StartNokiaPCSuiteServiceLayer(); start and stop too frequently makes inquiry fail

	//CloseAndRevertEngineState();

}

void CWidcommHandler::OnPushResponse(OPP_RESULT_CODE result_code,  BD_ADDR bda, WCHAR * string)
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


void CWidcommHandler::OnDataReceived (void *p_data, UINT16 len)
{
	if(iRequestedCommand !=  ERemoteControlSession)
		return;
	
    /*CString msg( (LPCTSTR) p_data, len);
    AddLogText("<< " + msg);*/
	
	iCaller->OnData(p_data,len);

}

//////////////////////////////////////////////////////////////////////
//
//  Called when and event is recieved on the port.
//
void CWidcommHandler::OnEventReceived (UINT32 event_code)
{
	if(iRequestedCommand !=  ERemoteControlSession)
		return;
    // Was a connection established?
    //

	if (PORT_EV_CONNECTED & event_code)
    {
			iCaller->OnConnected(this->iDiscoveringDevice,6);    		
    }
	else    
    if ( PORT_EV_CONNECT_ERR & event_code )
    {
			iCaller->OnDisconnected();  			
			unsigned int err = GetLastError();			
			
        
			CString str;			
			str.Format("PORT_EV_CONNECT_ERR: %d ",err);
			mydebug::log(str);
    }
	else
	{
	    	iLabel->SetWindowText("Unknown Broadcom event.");
			
			CString str;			
			str.Format("Unknown Broadcom event: %d ",event_code);
			mydebug::log(str);

			str.Empty();
			unsigned int err = GetLastError();	
			str.Format("PORT_EV_CONNECT_ERR: %d ",err);
			mydebug::log(str);
			//Close();
			//RevertToIdle();
	}

	

}


void CWidcommHandler::CloseAndRevertEngineState()
{
	iRequestedCommand = EStateIdle;
	
	iButton = NULL;//this is another flag used to check the state of the engine 
	StopInquiry();
	Sleep(100);
	Close();	
}


void CWidcommHandler::TimerProc(
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
		
		curInstance->StopInquiry();
		Sleep(100);
		curInstance->OnInquiryComplete(false,-1);//NOT FAILED - IT'S NORMAL THAT WIDCOMM SOMETIMES DOESN'T REPORT OnInquiryComplete
		
		break;
	
	case EHandlerStateDoingServideDiscovery:
		break;
	
	case EHandlerStateSendingFile:
		break;
	
	case EHandlerStateIdle:
	default:
		break;

	}

	curInstance->iState = EHandlerStateIdle;
}


bool CWidcommHandler::StartBTInteractionTimer()
{	
	if(m_TimerID)
		KillTimer(iHwnd,m_TimerID);

	m_TimerID = 0;

	m_TimerID = SetTimer(iHwnd,57/*any identifier because we have only one timer*/, INACTIVITY_TIMEOUT_SECONDS * 1000, TimerProc);

	if(m_TimerID)
		return true;	

	return false;
}



bool CWidcommHandler::StopBTInteractionTimer()
{
	if(m_TimerID && KillTimer(iHwnd,m_TimerID))
	{
		m_TimerID = 0;
		return true;
	}

	return false;
}
