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


#include "stdafx.h"
#include "log.h"
#include "strsafe.h"


#define WRITESTRTOFILE  \
			file.SeekToEnd();\
			file.Write(s,s.GetLength());\
			file.Write("\r\n",2);\
			file.Flush();\
			file.Close();\

CString mydebug::iPath = "";

void mydebug::InitPath()
	{	
	
	

	}


void mydebug::log(LPCTSTR str)
	{	
		if(iPath.GetLength()==0)
		{

	char TmpObexFilePath[MAX_PATH];
		
	GetModuleFileName(AfxGetInstanceHandle(), TmpObexFilePath, MAX_PATH);
	
	 /* Search backward. */
	CHAR ch('\\');
	CHAR* pdest = strrchr( TmpObexFilePath, ch );	

	if( pdest != NULL )
	{
		if( MAX_PATH > pdest - TmpObexFilePath)
			*(pdest+1) = NULL;
	}		
	
	
	StringCbCatA(TmpObexFilePath,MAX_PATH,"log.txt");
	
	iPath = TmpObexFilePath;

	/*CString test_msg = iPath;
	test_msg.Insert(0,"This version of WMouseXP is for testing purposes.\r\nPlease kindly zip & email the log file from below to support@clearevo.com after any problem happens:\r\n\r\n");
	AfxMessageBox(test_msg);
		};*/
		}
			

		CFile file;	

		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];

	  time ( &rawtime );
	  timeinfo = localtime ( &rawtime );

	  strftime (buffer,80,"%H:%M:%S ",timeinfo);

	  CString s = buffer;
	  s+=str;



		if(file.Open(iPath,CFile::modeReadWrite))
		{
			if(file.GetLength() > 200*1024)
			{
				file.SetLength(0);
				char* str = "--OLD LOG TRUNCATED--\r\n";
				file.Write(str,strlen(str));
			}

			WRITESTRTOFILE
		}
		else
		{
			if(file.Open(iPath,CFile::modeReadWrite|CFile::modeCreate))
			{
			WRITESTRTOFILE
			}
		
		}
	}
