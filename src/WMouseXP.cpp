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


// WMouseXP.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WMouseXP.h"
#include "WMouseXPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWMouseXPApp

BEGIN_MESSAGE_MAP(CWMouseXPApp, CWinApp)
	//{{AFX_MSG_MAP(CWMouseXPApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	//ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWMouseXPApp construction

CWMouseXPApp::CWMouseXPApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWMouseXPApp object

CWMouseXPApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWMouseXPApp initialization

HANDLE g_hMutexAppRunning = NULL;

BOOL AppInstanceExists()
{
   BOOL bAppRunning = FALSE;
   // Create a global mutex. Use a unique name, for example 
// incorporating your company and application name.
   g_hMutexAppRunning = CreateMutex( NULL, FALSE, 
                                     "Global\\My Company MpApp.EXE");
   // Check if the mutex object already exists, indicating an
   // existing application instance
   if (( g_hMutexAppRunning != NULL ) && 
         ( GetLastError() == ERROR_ALREADY_EXISTS))
   {
      // Close the mutex for this application instance. This assumes
      // the application will inform the user that it is 
      // about to terminate 
      CloseHandle( g_hMutexAppRunning );
      g_hMutexAppRunning = NULL;
   }
   // Return False if a new mutex was created, 
// as this means it's the first app instance
   return ( g_hMutexAppRunning == NULL );
}



BOOL CWMouseXPApp::InitInstance()
{


	char* szTitle = "WMouseXP - PC Side";

	 // Check if another application instance is already running
   if ( AppInstanceExists() == TRUE )
   {
      HWND hWndOtherInstance;
      hWndOtherInstance = FindWindow(NULL,szTitle );
      if ( hWndOtherInstance != (HWND)NULL )
      {
         // Application is running in current user's session
         if (IsIconic(hWndOtherInstance)) 
            ShowWindow(hWndOtherInstance, SW_RESTORE);
         SetForegroundWindow(hWndOtherInstance);
      }
      else
      {
         MessageBox(NULL, TEXT(
         "An instance of this app is running in another user's session"),
         szTitle, MB_OK);
      }
      return FALSE;
   }




	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	InitCommonControls();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CWMouseXPDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.


	return FALSE;
}
