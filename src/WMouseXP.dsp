# Microsoft Developer Studio Project File - Name="WMouseXP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WMouseXP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WMouseXP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WMouseXP.mak" CFG="WMouseXP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WMouseXP - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WMouseXP - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WMouseXP - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_BTWLIB" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib version.lib WtsApi32.lib /nologo /subsystem:windows /incremental:yes /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /ignore:4089
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "WMouseXP - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MSVCRT.lib /nologo /subsystem:windows /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /debug /nodefaultlib

!ENDIF 

# Begin Target

# Name "WMouseXP - Win32 Release"
# Name "WMouseXP - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BlueSoleilFoundDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\BlueSoleilHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ByteArray.cpp
# End Source File
# Begin Source File

SOURCE=.\ComPort.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\Conversions.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomCommandDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DevClass.cpp
# End Source File
# Begin Source File

SOURCE=.\GetActionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\MyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\OBEX.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PictureEx.cpp
# End Source File
# Begin Source File

SOURCE=.\SdkFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectPhoneDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TidyVector.cpp
# End Source File
# Begin Source File

SOURCE=.\ToshHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\WidcommFoundDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\WidcommHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\WMouseXP.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\WMouseXP.hpj

!IF  "$(CFG)" == "WMouseXP - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help file...
OutDir=.\Release
InputPath=.\hlp\WMouseXP.hpj
InputName=WMouseXP

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "WMouseXP - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help file...
OutDir=.\Debug
InputPath=.\hlp\WMouseXP.hpj
InputName=WMouseXP

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WMouseXP.rc
# End Source File
# Begin Source File

SOURCE=.\WMouseXPDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WMouseXPMobileDevice.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BlueSoleilFoundDevice.h
# End Source File
# Begin Source File

SOURCE=.\BlueSoleilHandler.h
# End Source File
# Begin Source File

SOURCE=.\BTEnginesCommons.h
# End Source File
# Begin Source File

SOURCE=.\BTStackEnum.h
# End Source File
# Begin Source File

SOURCE=.\ByteArray.h
# End Source File
# Begin Source File

SOURCE=.\ComPort.h
# End Source File
# Begin Source File

SOURCE=.\ConnectCheck.h
# End Source File
# Begin Source File

SOURCE=.\Conversions.h
# End Source File
# Begin Source File

SOURCE=.\CustomCommandDialog.h
# End Source File
# Begin Source File

SOURCE=.\DevClass.h
# End Source File
# Begin Source File

SOURCE=.\GetActionDialog.h
# End Source File
# Begin Source File

SOURCE=.\HelpFunc.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\MyDialog.h
# End Source File
# Begin Source File

SOURCE=.\OBEX.h
# End Source File
# Begin Source File

SOURCE=.\OBEXStructs.h
# End Source File
# Begin Source File

SOURCE=.\OptionsConsts.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\PictureEx.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "WMouseXP - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=WMouseXP
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "WMouseXP - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=WMouseXP
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SdkFunc.h
# End Source File
# Begin Source File

SOURCE=.\SelectPhoneDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TidyVector.h
# End Source File
# Begin Source File

SOURCE=.\ToshHandler.h
# End Source File
# Begin Source File

SOURCE=.\WidcommFoundDevice.h
# End Source File
# Begin Source File

SOURCE=.\WidcommHandler.h
# End Source File
# Begin Source File

SOURCE=.\WMouseXP.h
# End Source File
# Begin Source File

SOURCE=.\WMouseXPDlg.h
# End Source File
# Begin Source File

SOURCE=.\WMouseXPMobileDevice.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bluelactive.gif
# End Source File
# Begin Source File

SOURCE=.\res\connected.gif
# End Source File
# Begin Source File

SOURCE=.\res\default1.bin
# End Source File
# Begin Source File

SOURCE=.\res\gif1.bin
# End Source File
# Begin Source File

SOURCE=".\res\gplv3-127x51.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\jar1.bin
# End Source File
# Begin Source File

SOURCE=.\res\jar3.bin
# End Source File
# Begin Source File

SOURCE=.\res\searching.gif
# End Source File
# Begin Source File

SOURCE=.\res\WMouseXP.ico
# End Source File
# Begin Source File

SOURCE=.\res\WMouseXP.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\eWebClient.dll
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\WMouseXP.jar
# End Source File
# Begin Source File

SOURCE=..\cximage600_full\png\Release\png.lib
# End Source File
# Begin Source File

SOURCE=..\cximage600_full\jpeg\Release\Jpeg.lib
# End Source File
# Begin Source File

SOURCE=..\cximage600_full\jasper\Release\jasper.lib
# End Source File
# Begin Source File

SOURCE=..\cximage600_full\mng\Release\mng.lib
# End Source File
# Begin Source File

SOURCE=..\cximage600_full\tiff\Release\Tiff.lib
# End Source File
# Begin Source File

SOURCE=..\cximage600_full\zlib\Release\zlib.lib
# End Source File
# Begin Source File

SOURCE=..\cximage600_full\raw\Release\libdcr.lib
# End Source File
# Begin Source File

SOURCE=..\cximage600_full\CxImage\Release\cximage.lib
# End Source File
# End Target
# End Project
