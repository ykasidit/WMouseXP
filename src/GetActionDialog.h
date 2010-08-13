#if !defined(AFX_GETACTIONDIALOG_H__B7235BCA_AB64_457F_A15D_45B5DF8BC24F__INCLUDED_)
#define AFX_GETACTIONDIALOG_H__B7235BCA_AB64_457F_A15D_45B5DF8BC24F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetActionDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetActionDialog dialog
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


class CGetActionDialog : public CDialog
{
// Construction
public:
	CGetActionDialog(int& aKey, int& aMod, CString& aScript, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetActionDialog)
	enum { IDD = IDD_GETACTIONDIALOG };
	CStatic	m_CurActionStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetActionDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetActionDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void UpdateCurAction();
	CString iActionStr;

	int& iKey;
	int& iMod;
	CString& iScript;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETACTIONDIALOG_H__B7235BCA_AB64_457F_A15D_45B5DF8BC24F__INCLUDED_)
