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


// SelectPhoneDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WMouseXP.h"
#include "SelectPhoneDialog.h"
#include "WMouseXPMobileDevice.h"
#include "WidcommFoundDevice.h"
#include "BlueSoleilFoundDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectPhoneDialog dialog


CSelectPhoneDialog::CSelectPhoneDialog(TBTStackEnum vectorStackType, TidyVector* WMouseXPMobileDeviceVector,int& SelectedDev,CWnd* pParent, PBTDEVINFOLIST aToshList)
	: CDialog(CSelectPhoneDialog::IDD, pParent),iSelectedDev(SelectedDev)
{
	
	iWMouseXPMobileDeviceVector = WMouseXPMobileDeviceVector;
	
	iVectorStackType = vectorStackType;

	iToshList = aToshList;

	
	//iDiscoveredIndexVector = DiscoveredIndexVector;
	//{{AFX_DATA_INIT(CSelectPhoneDialog)
	//}}AFX_DATA_INIT


}


void CSelectPhoneDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectPhoneDialog)
	DDX_Control(pDX, IDC_LIST1, m_lb);
	//}}AFX_DATA_MAP
}


void CSelectPhoneDialog::OnOK()
{
	this->iSelectedDev = m_lb.GetCurSel();

	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CSelectPhoneDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectPhoneDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectPhoneDialog message handlers

BOOL CSelectPhoneDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(iToshList == NULL) //not toshiba
	{
		for(int i=0;i<iWMouseXPMobileDeviceVector->GetSize();i++)
		{
			if(iVectorStackType == EMSStack)
			{
				CString devname(((WMouseXPMobileDevice*)iWMouseXPMobileDeviceVector->GetAt(i))->iDevName);
				
				if(devname.GetLength()==0)
				{
					//BYTE* devaddr = (BYTE*) ((WMouseXPMobileDevice*)iWMouseXPMobileDeviceVector->GetAt(i))->iAddr;

					//devname.Format("%02X:%02X:%02X:%02X:%02X:%02X",devaddr[0],devaddr[1],devaddr[2],devaddr[3],devaddr[4],devaddr[5]);
					//devname = "GetName not ready - try again";

					devname = ((WMouseXPMobileDevice*)iWMouseXPMobileDeviceVector->GetAt(i))->iAddr;
				}

				m_lb.AddString(devname);

			}
			else
			if(iVectorStackType == EWidcommStack)
			{
				CString devname(((CWidcommFoundDevice*)iWMouseXPMobileDeviceVector->GetAt(i))->iName);

				if(devname.GetLength()==0)
				{
					BYTE* devaddr = ((CWidcommFoundDevice*)iWMouseXPMobileDeviceVector->GetAt(i))->iAddr;

					devname.Format("%02X:%02X:%02X:%02X:%02X:%02X",devaddr[0],devaddr[1],devaddr[2],devaddr[3],devaddr[4],devaddr[5]);
					//devname = "GetName not ready - try again";
				}

				
					m_lb.AddString(devname);
			}
			else
			if(iVectorStackType == EBlueSoleilStack)
			{
			
					CString devname(((CBlueSoleilFoundDevice*)iWMouseXPMobileDeviceVector->GetAt(i))->iBTDevInfo.szName);
					
					if(devname.GetLength()==0)
					{
						BYTE* devaddr = ((CBlueSoleilFoundDevice*)iWMouseXPMobileDeviceVector->GetAt(i))->iBTDevInfo.address;

						////BlueSoleil uses reversed byte order.... 
						devname.Format("%02X:%02X:%02X:%02X:%02X:%02X",devaddr[5],devaddr[4],devaddr[3],devaddr[2],devaddr[1],devaddr[0]);
						//devname = "GetName not ready - try again";
					}

					m_lb.AddString(devname);		
			}

		}
	}
	else
	{

			for (int i=0; i<iToshList->dwDevListNum; i++) 
			{
				DWORD dwMask = (BTDEVINFOSTS_BDADDR_DEV|BTDEVINFOSTS_BDADDR_DB);
				if (iToshList->DevInfo[i].dwStatus & dwMask) 
				{
					//CopyMemory(BdAddr, ptr->DevInfo[i].BdAddr, sizeof(BDADDR));
					CString name;
					BDADDR	BdAddr;
										
					name = iToshList->DevInfo[i].FriendlyName;				

					if(name.GetLength() == 0)
					{
						CopyMemory(BdAddr, iToshList->DevInfo[i].BdAddr, sizeof(BDADDR));
						name.Format("%02X:%02X:%02X:%02X:%02X:%02X", BdAddr[0], BdAddr[1], BdAddr[2], BdAddr[3], BdAddr[4], BdAddr[5]);
					}
					
					
					m_lb.AddString(name);	
				}
			}
	
	
	}

	m_lb.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
