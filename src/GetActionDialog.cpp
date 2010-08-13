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

// GetActionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WMouseXP.h"
#include "GetActionDialog.h"
#include "CustomCommandDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetActionDialog dialog


CGetActionDialog::CGetActionDialog(int& aKey, int& aMod, CString& aScript,CWnd* pParent /*=NULL*/)
: CDialog(CGetActionDialog::IDD, pParent), iScript(aScript), iKey(aKey), iMod(aMod)
{
	//{{AFX_DATA_INIT(CGetActionDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGetActionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetActionDialog)
	DDX_Control(pDX, IDC_CURACTION, m_CurActionStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetActionDialog, CDialog)
	//{{AFX_MSG_MAP(CGetActionDialog)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetActionDialog message handlers

void CGetActionDialog::UpdateCurAction()
{
	CString str;
	str = "Set Action: ";
	if(iScript.GetLength()>0)
		iActionStr = iScript;
	else
	{		
		CString tmp = CCustomCommandDialog::GetCmdString(iKey,iMod);		

		
		iActionStr = tmp;
		
	}
	str += iActionStr;
	m_CurActionStr.SetWindowText(str);
};

BOOL CGetActionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateCurAction();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGetActionDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGetActionDialog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CGetActionDialog::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default	
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

LRESULT CGetActionDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CGetActionDialog::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYDOWN ||  pMsg->message == WM_SYSKEYUP )
	{
		if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
		{
			iKey = pMsg->wParam;	
			iScript.Empty();

			switch(iKey)
			{ 
			case VK_SHIFT:
				iMod |= KModMaskShift;
			break;
			case VK_CONTROL:
				iMod |= KModMaskCtrl;
			break;
			case VK_MENU:
				iMod |= KModMaskAlt;
			break;
			}

			UpdateCurAction();
		}
		else
		if(pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP)
		{
			switch(pMsg->wParam)
			{
			case VK_SHIFT:
				iMod &= ~KModMaskShift;
			break;
			case VK_CONTROL:
				iMod &= ~KModMaskCtrl;
			break;
			case VK_MENU:
				iMod &= ~KModMaskAlt;
			break;
			}

			UpdateCurAction();		
		}

		return 0;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CGetActionDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(message == WM_SYSCOMMAND)
		return FALSE;
	
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}
