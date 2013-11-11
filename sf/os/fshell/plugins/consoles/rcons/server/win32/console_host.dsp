# Microsoft Developer Studio Project File - Name="console_host" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=console_host - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "console_host.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "console_host.mak" CFG="console_host - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "console_host - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "console_host - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "console_host - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\epoc32\build\rcons\server\win32\Release"
# PROP Intermediate_Dir "\epoc32\build\rcons\server\win32\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib shlwapi.lib /nologo /subsystem:windows /machine:I386 /out:"\epoc32\tools\rcons.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "console_host - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\epoc32\build\rcons\server\win32\Debug"
# PROP Intermediate_Dir "\epoc32\build\rcons\server\win32\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\epoc32\build\rcons\server\win32\Debug\rcons.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "console_host - Win32 Release"
# Name "console_host - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Base.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\console_host.cpp
# End Source File
# Begin Source File

SOURCE=.\console_host.rc
# End Source File
# Begin Source File

SOURCE=.\ConsoleWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\Preferences.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoteConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\Server.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Socket.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketCommandHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TextBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\TextView.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Base.h
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.h
# End Source File
# Begin Source File

SOURCE=.\console_host.h
# End Source File
# Begin Source File

SOURCE=.\ConsoleWindow.h
# End Source File
# Begin Source File

SOURCE=.\DynamicBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Misc.h
# End Source File
# Begin Source File

SOURCE=.\Preferences.h
# End Source File
# Begin Source File

SOURCE=.\RemoteConsole.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\ServerSocket.h
# End Source File
# Begin Source File

SOURCE=.\Socket.h
# End Source File
# Begin Source File

SOURCE=.\SocketCommandHandler.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TextBuffer.h
# End Source File
# Begin Source File

SOURCE=.\TextView.h
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\console_host.ico
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
