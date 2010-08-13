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


/////////////////////////////////////////////////////////////////////
// MyDialog.cpp CDialog derivative to avoid exits when user presses enter or Esc.
// Autor: Kasidit Yusuf
// Copyright (c) 2005 Kasidit Yusuf. All rights reserved.
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\mydialog.h"

CMyDialog::CMyDialog(void):CDialog()
	{
		
	}

CMyDialog::~CMyDialog(void)
	{
	}
CMyDialog::CMyDialog(UINT n,CWnd* p):CDialog(n, p){}
CMyDialog::CMyDialog(LPCTSTR tn,CWnd* p):CDialog(tn, p){}
void CMyDialog::OnOK(){}
void CMyDialog::OnCancel(){}