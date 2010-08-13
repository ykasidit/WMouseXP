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

#if !defined(AFX_CUSTOMCOMMANDDIALOG_H__A2D47A81_9D9E_4D79_9B42_0125E0C7DE00__INCLUDED_)
#define AFX_CUSTOMCOMMANDDIALOG_H__A2D47A81_9D9E_4D79_9B42_0125E0C7DE00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomCommandDialog.h : header file
//
#include "wmousexpdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomCommandDialog dialog

class CCustomCommandDialog : public CDialog
{
// Construction
public:
	CCustomCommandDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomCommandDialog)
	enum { IDD = IDD_CUSTOMCOMMANDS_DIALOG };
	CButton	m_Num0;
	CButton	m_NumStar;
	CButton	m_NumHash;
	CButton	m_Num9;
	CButton	m_Num8;
	CButton	m_Num7;
	CButton	m_Num6;
	CButton	m_Num5;
	CButton	m_Num4;
	CButton	m_Num3;
	CButton	m_Num2;
	CButton	m_Num1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomCommandDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


	

protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomCommandDialog)
	afx_msg void OnButton13();
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnButton9();
	afx_msg void OnButtonstar();
	afx_msg void OnButton0();
	afx_msg void OnButtonhash();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	
public:
	void ReloadCmdStrings();
	static CString& GetCmdString(int vkeycode,int mod);
	void HandleRemapButton(TNumpads numpad);
	CToolTipCtrl* m_pToolTip;

	~CCustomCommandDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMCOMMANDDIALOG_H__A2D47A81_9D9E_4D79_9B42_0125E0C7DE00__INCLUDED_)
