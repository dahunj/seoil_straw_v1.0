# Microsoft Developer Studio Project File - Name="SeoilVision" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SeoilVision - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SeoilVision.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SeoilVision.mak" CFG="SeoilVision - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SeoilVision - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SeoilVision - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SeoilVision - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "C:\Program Files\OpenCV\cxcore\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "SeoilVision - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\Program Files\OpenCV\cxcore\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SeoilVision - Win32 Release"
# Name "SeoilVision - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\classes\CannyDetect.cpp
# End Source File
# Begin Source File

SOURCE=.\com_serial.cpp
# End Source File
# Begin Source File

SOURCE=.\classes\ControlCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSol.cpp
# End Source File
# Begin Source File

SOURCE=.\DateTime.cpp
# End Source File
# Begin Source File

SOURCE=.\classes\GetPicture.cpp
# End Source File
# Begin Source File

SOURCE=.\INSParam.cpp
# End Source File
# Begin Source File

SOURCE=.\classes\inspectimage.cpp
# End Source File
# Begin Source File

SOURCE=.\classes\InspectSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InspResult.cpp
# End Source File
# Begin Source File

SOURCE=.\Labeling.cpp
# End Source File
# Begin Source File

SOURCE=.\LightControl.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\classes\PreInspection.cpp
# End Source File
# Begin Source File

SOURCE=.\classes\PreInspection1.cpp
# End Source File
# Begin Source File

SOURCE=.\classes\PreInspection1.h
# End Source File
# Begin Source File

SOURCE=.\SeoilMonitoring.cpp
# End Source File
# Begin Source File

SOURCE=.\SeoilVision.cpp
# End Source File
# Begin Source File

SOURCE=.\SeoilVision.rc
# End Source File
# Begin Source File

SOURCE=.\SeoilVisionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetCam.cpp
# End Source File
# Begin Source File

SOURCE=.\SetColorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\classes\StrawProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\light\TLightForm.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\classes\CannyDetect.h
# End Source File
# Begin Source File

SOURCE=.\com_serial.h
# End Source File
# Begin Source File

SOURCE=.\classes\ControlCamera.h
# End Source File
# Begin Source File

SOURCE=.\ControlSol.h
# End Source File
# Begin Source File

SOURCE="..\..\Program Files\OpenCV\cv\include\cv.h"
# End Source File
# Begin Source File

SOURCE="..\..\Program Files\OpenCV\cxcore\include\cxcore.h"
# End Source File
# Begin Source File

SOURCE=.\DateTime.h
# End Source File
# Begin Source File

SOURCE=.\classes\GetPicture.h
# End Source File
# Begin Source File

SOURCE="..\..\Program Files\OpenCV\otherlibs\highgui\highgui.h"
# End Source File
# Begin Source File

SOURCE=.\INSParam.h
# End Source File
# Begin Source File

SOURCE=.\classes\inspectimage.h
# End Source File
# Begin Source File

SOURCE=.\classes\InspectSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\InspResult.h
# End Source File
# Begin Source File

SOURCE=.\Labeling.h
# End Source File
# Begin Source File

SOURCE=.\LightControl.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\classes\PreInspection.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SeoilMonitoring.h
# End Source File
# Begin Source File

SOURCE=.\SeoilVision.h
# End Source File
# Begin Source File

SOURCE=.\SeoilVisionDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetCam.h
# End Source File
# Begin Source File

SOURCE=.\SetColorDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetTrigger.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\classes\StrawProcess.h
# End Source File
# Begin Source File

SOURCE=.\light\TLightForm.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\CViewerTest\res\CViewerTest.ico
# End Source File
# Begin Source File

SOURCE=..\CViewerTest\res\CViewerTestDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\SeoilVision.ico
# End Source File
# Begin Source File

SOURCE=.\res\SeoilVision.ico
# End Source File
# Begin Source File

SOURCE=.\res\SeoilVision.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SeoilVision2.ico
# End Source File
# Begin Source File

SOURCE=.\res\SeoilVision2.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Lib\cv.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\cvaux.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\cvcam.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\cvd_i7.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\cvhaartraining.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\cxcore.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\cxcored_i7.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\cxts.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\highgui.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\ml.lib
# End Source File
# Begin Source File

SOURCE=".\Lib\PCI-Dask.lib"
# End Source File
# Begin Source File

SOURCE=.\Lib\newflycap\PGRFlyCapture.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\newflycap\pgrflycapturegui.lib
# End Source File
# End Target
# End Project
