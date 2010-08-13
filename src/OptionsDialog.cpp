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


// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WMouseXP.h"
#include "OptionsDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog


COptionsDialog::COptionsDialog(int &s, int &a, CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDialog::IDD, pParent),iSpeed(s),iAcceleration(a)
{
	//{{AFX_DATA_INIT(COptionsDialog)
	//}}AFX_DATA_INIT
}


void COptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDialog)
	DDX_Control(pDX, IDC_SLIDER2, m_AccelerationSlider);
	DDX_Control(pDX, IDC_SLIDER1, m_SpeedSlider);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	//{{AFX_MSG_MAP(COptionsDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON1, OnDefault)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnCustomdrawSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnReleasedcaptureSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnReleasedcaptureSlider1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog message handlers

void COptionsDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	

	


	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL COptionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_SpeedSlider.SetRange(0, 5);
	m_AccelerationSlider.SetRange(0, 5);
	
	m_SpeedSlider.SetPos(iSpeed);
	m_AccelerationSlider.SetPos(iAcceleration);

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDialog::OnDefault() 
{
	// TODO: Add your control notification handler code here
	m_SpeedSlider.SetPos(KDefaultSpeed);
	m_AccelerationSlider.SetPos(KDefaultAcceleration);

	iSpeed = KDefaultSpeed;
	iAcceleration = KDefaultAcceleration;
}

void COptionsDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	
	

	CDialog::OnClose();
}

void COptionsDialog::OnCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void COptionsDialog::OnCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void COptionsDialog::OnReleasedcaptureSlider2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here	
	iAcceleration = m_AccelerationSlider.GetPos();	

	*pResult = 0;
}

void COptionsDialog::OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	iSpeed = m_SpeedSlider.GetPos();	
	
	
	*pResult = 0;
}
