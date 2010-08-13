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


#if !defined(AFX_OPTIONSDIALOG_H__168586B9_C043_45DA_A893_AE7069C81968__INCLUDED_)
#define AFX_OPTIONSDIALOG_H__168586B9_C043_45DA_A893_AE7069C81968__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog

#include "optionsconsts.h"

class COptionsDialog : public CDialog
{
// Construction
public:
	COptionsDialog(int& speed, int& acc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDialog)
	enum { IDD = IDD_DIALOG1 };
	CSliderCtrl	m_AccelerationSlider;
	CSliderCtrl	m_SpeedSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDialog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnDefault();
	afx_msg void OnClose();
	afx_msg void OnCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	
	int& iSpeed;
	int& iAcceleration;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDIALOG_H__168586B9_C043_45DA_A893_AE7069C81968__INCLUDED_)
