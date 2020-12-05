# Microsoft Developer Studio Project File - Name="MentoHUST" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MentoHUST - Win32 Debug_GB
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MentoHUST.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MentoHUST.mak" CFG="MentoHUST - Win32 Debug_GB"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MentoHUST - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MentoHUST - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MentoHUST - Win32 Release_GB" (based on "Win32 (x86) Application")
!MESSAGE "MentoHUST - Win32 Debug_GB" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MentoHUST - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "./" /I "./Source" /I "./Source/Other" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"../Release/MentoHUST.exe" /libpath:"./Source/Other"

!ELSEIF  "$(CFG)" == "MentoHUST - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I "./Source" /I "./Source/Other" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../Debug/MentoHUST.exe" /pdbtype:sept /libpath:"./Source/Other"

!ELSEIF  "$(CFG)" == "MentoHUST - Win32 Release_GB"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_GB"
# PROP BASE Intermediate_Dir "Release_GB"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_GB"
# PROP Intermediate_Dir "Release_GB"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "./" /I "./Source" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MD /W3 /GX /O1 /I "./" /I "./Source" /I "./Source/Other" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"../Release/MentoHUST.exe"
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"../Release/MentoHUST.exe" /libpath:"./Source/Other"

!ELSEIF  "$(CFG)" == "MentoHUST - Win32 Debug_GB"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MentoHUST___Win32_Debug_GB"
# PROP BASE Intermediate_Dir "MentoHUST___Win32_Debug_GB"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_GB"
# PROP Intermediate_Dir "Debug_GB"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I "./Source" /I "./Source/Other" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I "./Source" /I "./Source/Other" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../Debug/MentoHUST.exe" /pdbtype:sept /libpath:"./Source/Other"
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"../Debug/MentoHUST.exe" /pdbtype:sept /libpath:"./Source/Other"

!ENDIF 

# Begin Target

# Name "MentoHUST - Win32 Release"
# Name "MentoHUST - Win32 Debug"
# Name "MentoHUST - Win32 Release_GB"
# Name "MentoHUST - Win32 Debug_GB"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\AccountPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Source\ConfigDlg.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Source\Other\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Other\md5.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Source\MentoHUST.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MentoHUSTDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ParameterPage.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Source\Process.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\StringList.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\AccountPage.h
# End Source File
# Begin Source File

SOURCE=.\Source\ConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\Other\Hyperlink.h
# End Source File
# Begin Source File

SOURCE=.\Source\MentoHUST.h
# End Source File
# Begin Source File

SOURCE=.\Source\MentoHUSTDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\ParameterPage.h
# End Source File
# Begin Source File

SOURCE=.\Source\Process.h
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Source\StringList.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Certting.ico
# End Source File
# Begin Source File

SOURCE=.\res\exe.manifest
# End Source File
# Begin Source File

SOURCE=.\res\Failed.ico
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MentoHUST.ico
# End Source File
# Begin Source File

SOURCE=.\MentoHUST.rc
# End Source File
# Begin Source File

SOURCE=.\res\MentoHUST.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# End Target
# End Project
