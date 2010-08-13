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

// CustomCommandDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WMouseXP.h"
#include "WMouseXPDlg.h"
#include "CustomCommandDialog.h"
#include "getactiondialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomCommandDialog dialog


CCustomCommandDialog::CCustomCommandDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomCommandDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomCommandDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pToolTip = NULL;
}

CCustomCommandDialog::~CCustomCommandDialog()
{
	delete m_pToolTip;
}

void CCustomCommandDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomCommandDialog)
	DDX_Control(pDX, IDC_BUTTON0, m_Num0);
	DDX_Control(pDX, IDC_BUTTONSTAR, m_NumStar);
	DDX_Control(pDX, IDC_BUTTONHASH, m_NumHash);
	DDX_Control(pDX, IDC_BUTTON9, m_Num9);
	DDX_Control(pDX, IDC_BUTTON8, m_Num8);
	DDX_Control(pDX, IDC_BUTTON7, m_Num7);
	DDX_Control(pDX, IDC_BUTTON6, m_Num6);
	DDX_Control(pDX, IDC_BUTTON5, m_Num5);
	DDX_Control(pDX, IDC_BUTTON4, m_Num4);
	DDX_Control(pDX, IDC_BUTTON3, m_Num3);
	DDX_Control(pDX, IDC_BUTTON2, m_Num2);
	DDX_Control(pDX, IDC_BUTTON1, m_Num1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomCommandDialog, CDialog)
	//{{AFX_MSG_MAP(CCustomCommandDialog)
	ON_BN_CLICKED(IDC_BUTTON13, OnButton13)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTONSTAR, OnButtonstar)
	ON_BN_CLICKED(IDC_BUTTON0, OnButton0)
	ON_BN_CLICKED(IDC_BUTTONHASH, OnButtonhash)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomCommandDialog message handlers

void CCustomCommandDialog::ReloadCmdStrings() 
{
	CWMouseXPDlg* wmxp = CWMouseXPDlg::curInstance;

	m_Num1.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum1],wmxp->iNumpadModifiers[ENum1]) ) ;
	m_Num2.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum2],wmxp->iNumpadModifiers[ENum2]) ) ;
	m_Num3.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum3],wmxp->iNumpadModifiers[ENum3]) ) ;
	m_Num4.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum4],wmxp->iNumpadModifiers[ENum4]) ) ;
	m_Num5.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum5],wmxp->iNumpadModifiers[ENum5]) ) ;
	m_Num6.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum6],wmxp->iNumpadModifiers[ENum6]) ) ;
	m_Num7.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum7],wmxp->iNumpadModifiers[ENum7]) ) ;
	m_Num8.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum8],wmxp->iNumpadModifiers[ENum8]) ) ;
	m_Num9.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum9],wmxp->iNumpadModifiers[ENum9]) ) ;
	m_NumStar.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENumStar],wmxp->iNumpadModifiers[ENumStar]) ) ;
	m_Num0.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENum0],wmxp->iNumpadModifiers[ENum0]) ) ;
	m_NumHash.SetWindowText( GetCmdString(wmxp->iNumpadCmdMap[ENumHash],wmxp->iNumpadModifiers[ENumHash]) ) ;
}

CString& CCustomCommandDialog::GetCmdString(int vkeycode, int mod)
{
	CString ret;
	ret.Empty();

	switch(vkeycode)
	{

	case 0:
	;//leave empty
	break;
	
	case KVirtualKeyLaunchMyComputer:
		ret = "MyComputer";
	break;		
	case VK_LBUTTON:
	ret = "LButton";
	break;
	case VK_RBUTTON:       
	ret = "RButton";
	break;
	case VK_CANCEL:
	ret = "Cancel";
	break;
	case VK_MBUTTON:
	ret = "MBUTTON";
	break;
case VK_XBUTTON1:
	ret = "XBUTTON1";
	break;
case VK_XBUTTON2:
	ret = "XBUTTON2";
	break;
case VK_BACK:
	ret = "Backspace";
	break;
case VK_TAB:
	ret = "TAB";
	break;
case VK_CLEAR:
	ret = "CLEAR";
	break;
case VK_RETURN:
	ret = "Enter/Return";
	break;
case VK_SHIFT:
	ret = "SHIFT";
	break;
case VK_CONTROL:
	ret = "CONTROL";
	break;
case VK_MENU:
	ret = "ALT";
	break;
case VK_PAUSE:
	ret = "PAUSE";
	break;
case VK_CAPITAL:
	ret = "CAPITAL";
	break;

case VK_KANA:
	ret = "KANA or HANGEUL";
	break;
case VK_JUNJA:
	ret = "JUNJA";
	break;
case VK_FINAL:
	ret = "FINAL";
	break;
case VK_HANJA:
	ret = "KANJI or HANJA";
	break;

case VK_ESCAPE:
	ret = "Esc";
	break;

case VK_CONVERT:
	ret = "CONVERT";
	break;
case VK_NONCONVERT:
	ret = "NONCONVERT";
	break;
case VK_ACCEPT:
	ret = "ACCEPT";
	break;
case VK_MODECHANGE:
	ret = "MODECHANGE";
	break;
case VK_SPACE:
	ret = "SPACE";
	break;
case VK_PRIOR:
	ret = "Pg.Up";
	break;
case VK_NEXT:
	ret = "Pg.Down";
	break;
case VK_END:
	ret = "END";
	break;
case VK_HOME:
	ret = "HOME";
	break;
case VK_LEFT:
	ret = "LEFT";
	break;
case VK_UP:
	ret = "UP";
	break;
case VK_RIGHT:
	ret = "RIGHT";
	break;
case VK_DOWN:
	ret = "DOWN";
	break;
case VK_SELECT:
	ret = "SELECT";
	break;
case VK_PRINT:
	ret = "PRINT";
	break;
case VK_EXECUTE:
	ret = "EXECUTE";
	break;
case VK_SNAPSHOT:
	ret = "SNAPSHOT";
	break;
case VK_INSERT:
	ret = "INSERT";
	break;
case VK_DELETE:
	ret = "DELETE";
	break;
case VK_HELP:
	ret = "HELP";
	break;
case VK_LWIN:
	ret = "LeftWin";
	break;
case VK_RWIN:
	ret = "RightWin";
	break;
case VK_APPS:
	ret = "Apps";
	break;
case VK_SLEEP:
	ret = "Sleep";
	break;
case VK_NUMPAD0:
	ret = "NUMPAD0";
	break;
case VK_NUMPAD1:
	ret = "NUMPAD1";
	break;
case VK_NUMPAD2:
	ret = "NUMPAD2";
	break;
case VK_NUMPAD3:
	ret = "NUMPAD3";
	break;
case VK_NUMPAD4:
	ret = "NUMPAD4";
	break;
case VK_NUMPAD5:
	ret = "NUMPAD5";
	break;
case VK_NUMPAD6:
	ret = "NUMPAD6";
	break;
case VK_NUMPAD7:
	ret = "NUMPAD7";
	break;
case VK_NUMPAD8:
	ret = "NUMPAD8";
	break;
case VK_NUMPAD9:
	ret = "NUMPAD9";
	break;
case VK_MULTIPLY:
	ret = "MULTIPLY";
	break;
case VK_ADD:
	ret = "ADD";
	break;
case VK_SEPARATOR:
	ret = "SEPARATOR";
	break;
case VK_SUBTRACT:
	ret = "SUBTRACT";
	break;
case VK_DECIMAL:
	ret = "DECIMAL";
	break;
case VK_DIVIDE:
	ret = "DIVIDE";
	break;
case VK_F1:
	ret = "F1";
	break;
case VK_F2:
	ret = "F2";
	break;
case VK_F3:
	ret = "F3";
	break;
case VK_F4:
	ret = "F4";
	break;
case VK_F5:
	ret = "F5";
	break;
case VK_F6:
	ret = "F6";
	break;
case VK_F7:
	ret = "F7";
	break;
case VK_F8:
	ret = "F8";
	break;
case VK_F9:
	ret = "F9";
	break;
case VK_F10:
	ret = "F10";
	break;
case VK_F11:
	ret = "F11";
	break;
case VK_F12:
	ret = "F12";
	break;
case VK_F13:
	ret = "F13";
	break;
case VK_F14:
	ret = "F14";
	break;
case VK_F15:
	ret = "F15";
	break;
case VK_F16:
	ret = "F16";
	break;
case VK_F17:
	ret = "F17";
	break;
case VK_F18:
	ret = "F18";
	break;
case VK_F19:
	ret = "F19";
	break;
case VK_F20:
	ret = "F20";
	break;
case VK_F21:
	ret = "F21";
	break;
case VK_F22:
	ret = "F22";
	break;
case VK_F23:
	ret = "F23";
	break;
case VK_F24:
	ret = "F24";
	break;

case VK_NUMLOCK:
	ret = "NUMLOCK";
	break;
case VK_SCROLL:
	ret = "SCROLL";
	break;

case VK_OEM_NEC_EQUAL:
	ret = "NEC EQUAL or Fujitsu JISHO";	
	break;
case VK_OEM_FJ_MASSHOU:
	ret = "MASSHOU";
	break;
case VK_OEM_FJ_TOUROKU:
	ret = "TOUROKU";
	break;
case VK_OEM_FJ_LOYA:
	ret = "LOYA";
	break;
case VK_OEM_FJ_ROYA:
	ret = "ROYA";
	break;
case VK_LSHIFT:
	ret = "LShift";
	break;
case VK_RSHIFT:
	ret = "RShift";
	break;
case VK_LCONTROL:
	ret = "LControl";
	break;
case VK_RCONTROL:
	ret = "RControl";
	break;
case VK_LMENU:
	ret = "LMenu";
	break;
case VK_RMENU:
	ret = "RMenu";
	break;

case VK_BROWSER_BACK:
	ret = "BrowserBack";
	break;
case VK_BROWSER_FORWARD:
	ret = "BrowserForward";
	break;
case VK_BROWSER_REFRESH:
	ret = "BrowserRefresh";
	break;
case VK_BROWSER_STOP:
	ret = "BrowserStop";
	break;
case VK_BROWSER_SEARCH:
	ret = "BrowserSearch";
	break;
case VK_BROWSER_FAVORITES:
	ret = "BrowserFavorites";
	break;
case VK_BROWSER_HOME:
	ret = "BrowserHome";
	break;

case VK_VOLUME_MUTE:
	ret = "Mute";
	break;
case VK_VOLUME_DOWN:
	ret = "VolumeDown";
	break;
case VK_VOLUME_UP:
	ret = "VolumeUp";
	break;
case VK_MEDIA_NEXT_TRACK:
	ret = "NextTrack";
	break;
case VK_MEDIA_PREV_TRACK:
	ret = "PrevTrack";
	break;
case VK_MEDIA_STOP:
	ret = "Stop";
	break;
case VK_MEDIA_PLAY_PAUSE:
	ret = "Play/Pause";
	break;
case VK_LAUNCH_MAIL:
	ret = "Mail";
	break;
case VK_LAUNCH_MEDIA_SELECT:
	ret = "MediaPlayer";
	break;
/* My Computer above - case VK_LAUNCH_APP1:
	ret = "Apps";
	break;*/
case VK_LAUNCH_APP2:
	ret = "Calculator";
	break;


case VK_OEM_1:
	ret = "OEM_1 ;: for US";
	break;// ';:' for US
case VK_OEM_PLUS:
	ret = "OEM_PLUS +";
	break;// '+' any country
case VK_OEM_COMMA:
	ret = "OEM_COMMA ,";
	break;// ',' any country
case VK_OEM_MINUS:
	ret = "OEM_MINUS -";
	break;// '-' any country
case VK_OEM_PERIOD:
	ret = "OEM_PERIOD .";
	break;// '.' any country
case VK_OEM_2:
	ret = "OEM_2 /?";
	break;// '/?' for US
case VK_OEM_3:
	ret = "OEM_3 ~";
	break;// '`~' for US

case VK_OEM_4:
	ret = "OEM_4 [{";
	break;//  '[{' for US
case VK_OEM_5:
	ret = "OEM_5 \\|";
	break;//  '\|' for US
case VK_OEM_6:
	ret = "OEM_6 ]}";
	break;//  ']}' for US
case VK_OEM_7:
	ret = "OEM_7 '\"";
	break;//  ''"' for US
case VK_OEM_8:
	ret = "OEM_8";
	break;

case VK_OEM_AX:
	ret = "AX key";
	break;//  'AX' key on Japanese AX kbd
case VK_OEM_102:
	ret = "<> or || on RT 102-key kbd";
	break;//  "<>" or "\|" on RT 102-key kbd.
case VK_ICO_HELP:
	ret = "Help key on ICO";
	break;//  Help key on ICO
case VK_ICO_00:
	ret = "00 key on ICO";
	break;//  00 key on ICO


case VK_ICO_CLEAR:
	ret = "ICO_CLEAR";
	break;



case VK_PACKET:
	ret = "PACKET";
	break;


case VK_OEM_RESET:
	ret = "OEM_RESET";
	break;
case VK_OEM_JUMP:
	ret = "OEM_JUMP";
	break;
case VK_OEM_PA1:
	ret = "OEM_PA1";
	break;
case VK_OEM_PA2:
	ret = "OEM_PA2";
	break;
case VK_OEM_PA3:
	ret = "OEM_PA3";
	break;
case VK_OEM_WSCTRL:
	ret = "OEM_WSCTRL";
	break;
case VK_OEM_CUSEL:
	ret = "OEM_CUSEL";
	break;
case VK_OEM_ATTN:
	ret = "OEM_ATTN";
	break;
case VK_OEM_FINISH:
	ret = "OEM_FINISH";
	break;
case VK_OEM_COPY:
	ret = "OEM_COPY";
	break;
case VK_OEM_AUTO:
	ret = "OEM_AUTO";
	break;
case VK_OEM_ENLW:
	ret = "OEM_ENLW";
	break;
case VK_OEM_BACKTAB:
	ret = "OEM_BACKTAB";
	break;

case VK_ATTN:
	ret = "ATTN";
	break;
case VK_CRSEL:
	ret = "CRSEL";
	break;
case VK_EXSEL:
	ret = "EXSEL";
	break;
case VK_EREOF:
	ret = "EREOF";
	break;
case VK_PLAY:
	ret = "PLAY";
	break;
case VK_ZOOM:
	ret = "ZOOM";
	break;
case VK_NONAME:
	ret = "NONAME";
	break;
case VK_PA1:
	ret = "PA1";
	break;
case VK_OEM_CLEAR:
	ret = "OEM_CLEAR";
	break;

	default:
		
/*
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */
		if(vkeycode >= 0x30 && vkeycode <= 0x39)
		{
		ret.Format("%c",('0'+(vkeycode-0x30)));
		}
		else 
		if(vkeycode >= 0x41 && vkeycode <= 0x5A)
		{
		ret.Format("%c",('A'+(vkeycode-0x41)));
		}
		else
			ret.Format("Unknown (%d)",vkeycode);
	break;
	}

		
		if(mod & KModMaskShift)
		{
			ret.Insert(0,"Shift+");
		}
		
		if(mod & KModMaskAlt)
		{
			ret.Insert(0,"Alt+");
		}

		if(mod & KModMaskCtrl)
		{
			ret.Insert(0,"Ctrl+");
		}
		

	/*int altmask = 0x20000000;//00100000 00000000 00000000 00000000;//bit 29
		if((mod&altmask) > 0)
		{
			ret.Insert(0,"Alt+");
			
		}*/


	return ret;
}

void CCustomCommandDialog::OnButton13() 
{
	//default
	CWMouseXPDlg::curInstance->DefaultCmdKeyMap();	
	ReloadCmdStrings();
}



BOOL CCustomCommandDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	   m_pToolTip = new CToolTipCtrl;
       if(!m_pToolTip->Create(this))
       {
           TRACE("Unable To create ToolTip\n");
           return TRUE;
       }

       m_pToolTip->AddTool(&m_Num1,"Button 1 Action - Press to change");
	   m_pToolTip->AddTool(&m_Num2,"Button 2 Action - Press to change");
	   m_pToolTip->AddTool(&m_Num3,"Button 3 Action - Press to change");
	   m_pToolTip->AddTool(&m_Num4,"Button 4 Action - Press to change");
	   m_pToolTip->AddTool(&m_Num5,"Button 5 Action - Press to change");
	   m_pToolTip->AddTool(&m_Num6,"Button 6 Action - Press to change");
	   m_pToolTip->AddTool(&m_Num7,"Button 7 Action - Press to change");
	   m_pToolTip->AddTool(&m_Num8,"Button 8 Action - Press to change");
	   m_pToolTip->AddTool(&m_Num9,"Button 9 Action - Press to change");
	   m_pToolTip->AddTool(&m_NumStar,"Button * Action - Press to change");
	   m_pToolTip->AddTool(&m_Num0,"Button 0 Action - Press to change");
	   m_pToolTip->AddTool(&m_NumHash,"Button # Action - Press to change");       

		m_pToolTip->Activate(TRUE);

		ReloadCmdStrings();
		// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCustomCommandDialog::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (NULL != m_pToolTip)
		 m_pToolTip->RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}


void CCustomCommandDialog::HandleRemapButton(TNumpads numpad)
	{
		CWMouseXPDlg* wmxp = CWMouseXPDlg::curInstance;
		
		int mod = 0; //wmxp->iNumpadModifiers[numpad];
		int key = 0; //wmxp->iNumpadCmdMap[numpad];
		CString script = wmxp->iNumpadScripts[numpad];
		
		CGetActionDialog dlg(key,mod, script);
		if(dlg.DoModal()==IDOK && key != 0)
		{
			wmxp->iNumpadCmdMap[numpad] = key;
			wmxp->iNumpadScripts[numpad] = script;
			wmxp->iNumpadModifiers[numpad] = mod;
		}

		ReloadCmdStrings();
	}

void CCustomCommandDialog::OnButton1() 
{
	// TODO: Add your control notification handler code here	
	HandleRemapButton(ENum1);
}

void CCustomCommandDialog::OnButton2() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum2);
}

void CCustomCommandDialog::OnButton3() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum3);
}

void CCustomCommandDialog::OnButton4() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum4);
}

void CCustomCommandDialog::OnButton5() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum5);
}

void CCustomCommandDialog::OnButton6() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum6);
}

void CCustomCommandDialog::OnButton7() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum7);
}

void CCustomCommandDialog::OnButton8() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum8);
}

void CCustomCommandDialog::OnButton9() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum9);
}

void CCustomCommandDialog::OnButtonstar() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENumStar);
}

void CCustomCommandDialog::OnButton0() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENum0);
}

void CCustomCommandDialog::OnButtonhash() 
{
	// TODO: Add your control notification handler code here
	HandleRemapButton(ENumHash);
}

void CCustomCommandDialog::OnOK() 
{
	// TODO: Add extra validation here
	//save to file 
	CWMouseXPDlg* wmxp = CWMouseXPDlg::curInstance;
	wmxp->SaveCustomCommands();
	
	CDialog::OnOK();
}


LRESULT CCustomCommandDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message == WM_SYSCOMMAND)
		return FALSE;
	
	
	return CDialog::DefWindowProc(message, wParam, lParam);	
}
