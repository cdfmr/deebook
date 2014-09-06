# Microsoft Developer Studio Project File - Name="DeeBook" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DeeBook - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DeeBook.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DeeBook.mak" CFG="DeeBook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DeeBook - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DeeBook - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DeeBook - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../ForceLibrary" /I "../CxImage/CxImage" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 psapi.lib ..\ForceLibrary\ForceLibrary.lib ..\CxImage\lib\CxImage.lib ..\CxImage\lib\Jpeg.lib ..\CxImage\lib\png.lib ..\CxImage\lib\zlib.lib /nologo /subsystem:windows /machine:I386 /out:"../Release/DeeBook.exe"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=COPY /Y ..\ForceLibrary\ForceLibrary.dll ..\Release\ForceLibrary.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DeeBook - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../ForceLibrary" /I "../CxImage/CxImage" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 psapi.lib ..\ForceLibrary\ForceLibrary.lib ..\CxImage\lib\CxImage.lib ..\CxImage\lib\Jpeg.lib ..\CxImage\lib\png.lib ..\CxImage\lib\zlib.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"../Debug/DeeBook.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=COPY /Y ..\ForceLibrary\ForceLibrary.dll ..\Debug\ForceLibrary.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DeeBook - Win32 Release"
# Name "DeeBook - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CHMDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DeeBook.cpp
# End Source File
# Begin Source File

SOURCE=.\DeeBook.rc
# End Source File
# Begin Source File

SOURCE=.\DeeBookDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IEDumper.cpp
# End Source File
# Begin Source File

SOURCE=.\Quantize.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SysUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\WndUtils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CHMDlg.h
# End Source File
# Begin Source File

SOURCE=.\DeeBook.h
# End Source File
# Begin Source File

SOURCE=.\DeeBookDlg.h
# End Source File
# Begin Source File

SOURCE=.\IEDumper.h
# End Source File
# Begin Source File

SOURCE=.\Quantize.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SysUtils.h
# End Source File
# Begin Source File

SOURCE=.\WindowFinder.h
# End Source File
# Begin Source File

SOURCE=.\WndUtils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DeeBook.ico
# End Source File
# Begin Source File

SOURCE=.\res\DeeBook.rc2
# End Source File
# Begin Source File

SOURCE=.\res\WinFinder.ico
# End Source File
# Begin Source File

SOURCE=.\res\WinFinderDrag.cur
# End Source File
# Begin Source File

SOURCE=.\res\WinFinderDrag.ico
# End Source File
# End Group
# Begin Group "HTMLReader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\HTMLReader\LiteHTMLAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\HTMLReader\LiteHTMLAttributes.h
# End Source File
# Begin Source File

SOURCE=..\HTMLReader\LiteHTMLCommon.h
# End Source File
# Begin Source File

SOURCE=..\HTMLReader\LiteHTMLEntityResolver.cpp
# End Source File
# Begin Source File

SOURCE=..\HTMLReader\LiteHTMLEntityResolver.h
# End Source File
# Begin Source File

SOURCE=..\HTMLReader\LiteHTMLReader.cpp
# End Source File
# Begin Source File

SOURCE=..\HTMLReader\LiteHTMLReader.h
# End Source File
# Begin Source File

SOURCE=..\HTMLReader\LiteHTMLTag.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
