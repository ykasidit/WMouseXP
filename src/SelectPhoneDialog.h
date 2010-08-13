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


#if !defined(AFX_SELECTPHONEDIALOG_H__99A93595_298E_4F44_BB50_019EF29AC4EC__INCLUDED_)
#define AFX_SELECTPHONEDIALOG_H__99A93595_298E_4F44_BB50_019EF29AC4EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectPhoneDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectPhoneDialog dialog

#include "tidyvector.h"
#include "btstackenum.h"

#include "sdptypes.h" //tosh vector style

class CSelectPhoneDialog : public CDialog
{
// Construction
public:
	CSelectPhoneDialog(TBTStackEnum vectorStackType,TidyVector* WMouseXPMobileDeviceVector,int& SelectedDev, CWnd* iParent, PBTDEVINFOLIST aToshList = NULL);   // standard constructor
	void OnOK();
// Dialog Data
	//{{AFX_DATA(CSelectPhoneDialog)
	enum { IDD = IDD_CHOOSE_DeviceDialog };
	CListBox	m_lb;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectPhoneDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int& iSelectedDev;
	TidyVector* iWMouseXPMobileDeviceVector;
	PBTDEVINFOLIST iToshList;
	TBTStackEnum iVectorStackType;

	// Generated message map functions
	//{{AFX_MSG(CSelectPhoneDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTPHONEDIALOG_H__99A93595_298E_4F44_BB50_019EF29AC4EC__INCLUDED_)
