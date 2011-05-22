# Microsoft Developer Studio Project File - Name="ecuExplorer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ecuExplorer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ecuExplorer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ecuExplorer.mak" CFG="ecuExplorer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ecuExplorer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ecuExplorer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ecuExplorer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\general library" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Ws2_32.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "ecuExplorer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\general library" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "ENABLE_EDIT_ROM" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ecuExplorer - Win32 Release"
# Name "ecuExplorer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "gui.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AW_CMultiViewSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogCOMM.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogEcuFlash.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogEcuQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogEditor_dataitem.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogEditor_label.cpp
# End Source File
# Begin Source File

SOURCE=.\ecuExplorer.cpp
# End Source File
# Begin Source File

SOURCE=.\frameCommSerial.cpp
# End Source File
# Begin Source File

SOURCE=.\frameDataItems.cpp
# End Source File
# Begin Source File

SOURCE=.\frameDTCReader.cpp
# End Source File
# Begin Source File

SOURCE=.\frameErrorLog.cpp
# End Source File
# Begin Source File

SOURCE=.\frameGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\frameList.cpp
# End Source File
# Begin Source File

SOURCE=.\frameLogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\frameMain.cpp
# End Source File
# Begin Source File

SOURCE=.\frameRealtime.cpp
# End Source File
# Begin Source File

SOURCE=.\frameResourceTree.cpp
# End Source File
# Begin Source File

SOURCE=.\frameRomTables.cpp
# End Source File
# Begin Source File

SOURCE=.\frameTableEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\frameTroubleCodes.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GridDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\handlerError.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceList.cpp
# End Source File
# Begin Source File

SOURCE=.\resources.rc
# End Source File
# Begin Source File

SOURCE=.\STabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\TitleTip.cpp
# End Source File
# End Group
# Begin Group "diagnostics.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\commSerial.cpp
# End Source File
# Begin Source File

SOURCE=.\protocolSSM.cpp
# End Source File
# End Group
# Begin Group "ecuflash.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\densocomm.cpp
# End Source File
# Begin Source File

SOURCE=.\densoecu.cpp
# End Source File
# Begin Source File

SOURCE=.\ecutools.cpp
# End Source File
# Begin Source File

SOURCE=.\error.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\serial.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "gui.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AW_CMultiViewSplitter.h
# End Source File
# Begin Source File

SOURCE=.\CellRange.h
# End Source File
# Begin Source File

SOURCE=.\definitionError.h
# End Source File
# Begin Source File

SOURCE=.\definitionLocal.h
# End Source File
# Begin Source File

SOURCE=.\definitionSSM.h
# End Source File
# Begin Source File

SOURCE=.\dialogCOMM.h
# End Source File
# Begin Source File

SOURCE=.\dialogEcuFlash.h
# End Source File
# Begin Source File

SOURCE=.\dialogEcuQuery.h
# End Source File
# Begin Source File

SOURCE=.\dialogEditor_dataitem.h
# End Source File
# Begin Source File

SOURCE=.\dialogEditor_label.h
# End Source File
# Begin Source File

SOURCE=.\ecuExplorer.h
# End Source File
# Begin Source File

SOURCE=.\frameCommSerial.h
# End Source File
# Begin Source File

SOURCE=.\frameDataItems.h
# End Source File
# Begin Source File

SOURCE=.\frameDTCReader.h
# End Source File
# Begin Source File

SOURCE=.\frameErrorLog.h
# End Source File
# Begin Source File

SOURCE=.\frameGrid.h
# End Source File
# Begin Source File

SOURCE=.\frameList.h
# End Source File
# Begin Source File

SOURCE=.\frameLogFile.h
# End Source File
# Begin Source File

SOURCE=.\frameMain.h
# End Source File
# Begin Source File

SOURCE=.\frameRealtime.h
# End Source File
# Begin Source File

SOURCE=.\frameResourceTree.h
# End Source File
# Begin Source File

SOURCE=.\frameRomTables.h
# End Source File
# Begin Source File

SOURCE=.\frameTableEditor.h
# End Source File
# Begin Source File

SOURCE=.\frameTroubleCodes.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GridDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\handlerError.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceList.h
# End Source File
# Begin Source File

SOURCE=.\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\STabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TitleTip.h
# End Source File
# End Group
# Begin Group "diagnostics.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\commSerial.h
# End Source File
# Begin Source File

SOURCE=.\protocolSSM.h
# End Source File
# End Group
# Begin Group "ecuflash.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\densocomm.h
# End Source File
# Begin Source File

SOURCE=.\densoecu.h
# End Source File
# Begin Source File

SOURCE=.\ecutools.h
# End Source File
# Begin Source File

SOURCE=.\error.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\serial.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Black_Chip.ico
# End Source File
# Begin Source File

SOURCE=.\ecuExplorer.ico
# End Source File
# Begin Source File

SOURCE=.\idr_data.ico
# End Source File
# Begin Source File

SOURCE=.\Red_Chip.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\BuildHistory.txt
# End Source File
# Begin Source File

SOURCE=.\KERNEL.HEX
# End Source File
# Begin Source File

SOURCE=.\scrap.txt
# End Source File
# End Target
# End Project
