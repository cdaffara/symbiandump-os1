# Microsoft Developer Studio Project File - Name="utracedecoder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=utracedecoder - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "utracedecoder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "utracedecoder.mak" CFG="utracedecoder - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utracedecoder - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "utracedecoder - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "utracedecoder"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "utracedecoder - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "utracedecoder___Win32_Release"
# PROP BASE Intermediate_Dir "utracedecoder___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "utracedecoder - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "utracedecoder___Win32_Debug"
# PROP BASE Intermediate_Dir "utracedecoder___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "utracedecoder - Win32 Release"
# Name "utracedecoder - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "messagedefparser (src)"

# PROP Default_Filter ""
# Begin Group "identifiers (src)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\messagedefparser\constidentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\contextidentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\enumidentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\identifierbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\integeridentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\messageidentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\messageidtypeidentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\padidentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\signatureidentifier.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\structidentifier.cpp
# End Source File
# End Group
# Begin Group "keywordprocessors (src)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\messagedefparser\aliasprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\constprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\contextprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\enumprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\includeprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\initialstate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\keywordprocessorbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\messageprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\signatureprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\structprocessor.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\messagedefparser\definitionparser.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\definitiontokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\statebase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\messagedefparser\statemachinebase.cpp
# End Source File
# End Group
# Begin Group "logevents (src)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\logevents\chunksevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logevents\codesegsevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logevents\kernelmemoryevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logevents\nodemessageevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logevents\rdebuglogevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logevents\simplelogevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logevents\threadidentificationevent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logevents\traceeventbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logevents\traceeventfactory.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\framesorter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\src\util.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utracedecoderapp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utraceframe.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "messagedefparser (inc)"

# PROP Default_Filter ""
# Begin Group "identifiers (inc)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inc\messagedefparser\aliasidentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\constidentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\contextidentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\enumidentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\identifierbase.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\integeridentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\messageidentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\messageidtypeidentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\padidentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\signatureidentifier.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\structidentifier.h
# End Source File
# End Group
# Begin Group "keywordprocessors (inc)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inc\messagedefparser\aliasprocessor.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\constprocessor.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\contextprocessor.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\enumprocessor.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\includeprocessor.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\initialstate.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\keywordprocessorbase.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\messageprocessor.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\signatureprocessor.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\structprocessor.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\inc\messagedefparser\definitionparser.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\definitiontokenizer.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\statebase.h
# End Source File
# Begin Source File

SOURCE=..\inc\messagedefparser\statemachinebase.h
# End Source File
# End Group
# Begin Group "logevents (inc)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\inc\logevents\chunksevent.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\codesegsevent.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\kernelmemoryevent.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\nodemessageevent.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\rdebuglogevent.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\simplelogevent.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\threadidentificationevent.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\traceevent.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\traceeventbase.h
# End Source File
# Begin Source File

SOURCE=..\inc\logevents\traceeventfactory.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\inc\e32btrace.h
# End Source File
# Begin Source File

SOURCE=..\inc\eventoutputhandler.h
# End Source File
# Begin Source File

SOURCE=..\inc\framesorter.h
# End Source File
# Begin Source File

SOURCE=..\inc\nodemessagetypes.h
# End Source File
# Begin Source File

SOURCE=..\inc\util.h
# End Source File
# Begin Source File

SOURCE=..\inc\utracedecoderapp.h
# End Source File
# Begin Source File

SOURCE=..\inc\utraceframe.h
# End Source File
# End Group
# End Target
# End Project
