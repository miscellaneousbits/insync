# Microsoft Developer Studio Project File - Name="insync" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=insync - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "InSync.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "InSync.mak" CFG="insync - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "insync - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "insync - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "insync - Win32 Release Uncompressed" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/insyncProjects/insync", OXAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "insync - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /pdb:none /map:"InSync.map" /machine:I386
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds="c:\program files\neolite\neolitecli.exe" -e -q -d release\insync.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "insync - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "insync - Win32 Release Uncompressed"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "insync___Win32_Release_Uncompressed"
# PROP BASE Intermediate_Dir "insync___Win32_Release_Uncompressed"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MBCS" /FR /YX"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /pdb:none /map:"InSync.map" /machine:I386
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 /nologo /subsystem:windows /pdb:none /map:"InSync.map" /machine:I386
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "insync - Win32 Release"
# Name "insync - Win32 Debug"
# Name "insync - Win32 Release Uncompressed"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AddCriteria.cpp
# End Source File
# Begin Source File

SOURCE=.\AllJobs.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowserDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\cgfiltyp.cpp
# End Source File
# Begin Source File

SOURCE=.\checkreg.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdLnInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\CNodes.cpp
# End Source File
# Begin Source File

SOURCE=.\copyfile.c
# End Source File
# Begin Source File

SOURCE=.\criteria.cpp
# End Source File
# Begin Source File

SOURCE=.\dd.cpp
# End Source File
# Begin Source File

SOURCE=.\DirListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ExcludedDirectoriesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\fnmatch.c
# End Source File
# Begin Source File

SOURCE=.\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\insync.cpp
# End Source File
# Begin Source File

SOURCE=.\insyncDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\JobDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\JobNode.cpp
# End Source File
# Begin Source File

SOURCE=.\munge.c
# End Source File
# Begin Source File

SOURCE=.\OXShellFolderTree.cpp
# End Source File
# Begin Source File

SOURCE=.\OXShellNamespaceNavigator.cpp
# End Source File
# Begin Source File

SOURCE=.\RegisterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SrcNode.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\syncdir.cpp
# End Source File
# Begin Source File

SOURCE=.\SyncListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TrgNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\WarningDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XTFlatComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ycode.inl
# End Source File
# Begin Source File

SOURCE=.\ydigest.inl
# End Source File
# Begin Source File

SOURCE=.\yrot.inl
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDialog.h
# End Source File
# Begin Source File

SOURCE=.\AddCriteria.h
# End Source File
# Begin Source File

SOURCE=.\BrowserDialog.h
# End Source File
# Begin Source File

SOURCE=.\cgfiltyp.h
# End Source File
# Begin Source File

SOURCE=.\CheckReg.h
# End Source File
# Begin Source File

SOURCE=.\CmdLnInfo.h
# End Source File
# Begin Source File

SOURCE=.\CNodes.h
# End Source File
# Begin Source File

SOURCE=.\CopyFile.h
# End Source File
# Begin Source File

SOURCE=.\criteria.h
# End Source File
# Begin Source File

SOURCE=.\criterias.h
# End Source File
# Begin Source File

SOURCE=.\dd.h
# End Source File
# Begin Source File

SOURCE=.\DirListBox.h
# End Source File
# Begin Source File

SOURCE=.\ExcludedDirectoriesDlg.h
# End Source File
# Begin Source File

SOURCE=.\FileEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\fnmatch.h
# End Source File
# Begin Source File

SOURCE=.\HyperLink.h
# End Source File
# Begin Source File

SOURCE=.\insync.h
# End Source File
# Begin Source File

SOURCE=.\insyncDlg.h
# End Source File
# Begin Source File

SOURCE=.\JobDialog.h
# End Source File
# Begin Source File

SOURCE=.\LinkList.h
# End Source File
# Begin Source File

SOURCE=.\munge.h
# End Source File
# Begin Source File

SOURCE=.\OXDllExt.h
# End Source File
# Begin Source File

SOURCE=.\OXShellFolderTree.h
# End Source File
# Begin Source File

SOURCE=.\OXShellNamespaceNavigator.h
# End Source File
# Begin Source File

SOURCE=.\RegisterDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SyncDir.h
# End Source File
# Begin Source File

SOURCE=.\SyncListBox.h
# End Source File
# Begin Source File

SOURCE=.\usrmsgs.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# Begin Source File

SOURCE=.\WarningDlg.h
# End Source File
# Begin Source File

SOURCE=.\XTFlatComboBox.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\dst_icon.ico
# End Source File
# Begin Source File

SOURCE=.\insync.rc
# End Source File
# Begin Source File

SOURCE=.\insync.rc2
# End Source File
# Begin Source File

SOURCE=.\insyncsig.ico
# End Source File
# Begin Source File

SOURCE=.\mandala00.ico
# End Source File
# Begin Source File

SOURCE=.\mandala01.ico
# End Source File
# Begin Source File

SOURCE=.\mandala02.ico
# End Source File
# Begin Source File

SOURCE=.\mandala03.ico
# End Source File
# Begin Source File

SOURCE=.\mandala04.ico
# End Source File
# Begin Source File

SOURCE=.\mandala05.ico
# End Source File
# Begin Source File

SOURCE=.\mandala06.ico
# End Source File
# Begin Source File

SOURCE=.\mandala07.ico
# End Source File
# Begin Source File

SOURCE=.\mandala08.ico
# End Source File
# Begin Source File

SOURCE=.\mandala09.ico
# End Source File
# Begin Source File

SOURCE=.\mandala10.ico
# End Source File
# Begin Source File

SOURCE=.\mandala11.ico
# End Source File
# Begin Source File

SOURCE=.\mandala12.ico
# End Source File
# Begin Source File

SOURCE=.\mandala13.ico
# End Source File
# Begin Source File

SOURCE=.\mandala14.ico
# End Source File
# Begin Source File

SOURCE=.\mandala15.ico
# End Source File
# Begin Source File

SOURCE=.\mandala16.ico
# End Source File
# Begin Source File

SOURCE=.\mandala17.ico
# End Source File
# Begin Source File

SOURCE=.\src_icon.ico
# End Source File
# Begin Source File

SOURCE=.\srcdst_i.ico
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\insync.clw
# End Source File
# Begin Source File

SOURCE=.\insync.exe.manifest

!IF  "$(CFG)" == "insync - Win32 Release"

# Begin Custom Build
TargetDir=.\Release
InputPath=.\insync.exe.manifest

"$(TargetDir)\insync.exe.manifest" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy insync.exe.manifest $(TargetDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "insync - Win32 Debug"

# Begin Custom Build
TargetDir=.\Debug
InputPath=.\insync.exe.manifest

"$(TargetDir)\insync.exe.manifest" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy insync.exe.manifest $(TargetDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "insync - Win32 Release Uncompressed"

# Begin Custom Build
TargetDir=.\Release
InputPath=.\insync.exe.manifest

"$(TargetDir)\insync.exe.manifest" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy insync.exe.manifest $(TargetDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InSync.map
# End Source File
# Begin Source File

SOURCE=.\InSync.opt
# End Source File
# Begin Source File

SOURCE=.\whatsnew.html

!IF  "$(CFG)" == "insync - Win32 Release"

# Begin Custom Build
TargetDir=.\Release
InputPath=.\whatsnew.html

"$(TargetDir)\whatsnew.html" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy whatsnew.html $(TargetDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "insync - Win32 Debug"

# Begin Custom Build
TargetDir=.\Debug
InputPath=.\whatsnew.html

"$(TargetDir)\whatsnew.html" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy whatsnew.html $(TargetDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "insync - Win32 Release Uncompressed"

# Begin Custom Build
TargetDir=.\Release
InputPath=.\whatsnew.html

"$(TargetDir)\whatsnew.html" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy whatsnew.html $(TargetDir)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\about.bmp
# End Source File
# Begin Source File

SOURCE=.\criteria.bmp
# End Source File
# Begin Source File

SOURCE=.\defcrit.bmp
# End Source File
# Begin Source File

SOURCE=.\insync.bmp
# End Source File
# Begin Source File

SOURCE=.\insync.cnt

!IF  "$(CFG)" == "insync - Win32 Release"

# Begin Custom Build - Performing Custom Build insync.cnt
TargetDir=.\Release
InputPath=.\insync.cnt

"$(TargetDir)\insync.cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy insync.cnt $(TargetDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "insync - Win32 Debug"

# Begin Custom Build - Performing Custom Build insync.cnt
TargetDir=.\Debug
InputPath=.\insync.cnt

"$(TargetDir)\insync.cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy insync.cnt $(TargetDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "insync - Win32 Release Uncompressed"

# Begin Custom Build - Performing Custom Build insync.cnt
TargetDir=.\Release
InputPath=.\insync.cnt

"$(TargetDir)\insync.cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy insync.cnt $(TargetDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\insync.hpj

!IF  "$(CFG)" == "insync - Win32 Release"

USERDEP__INSYN="insync.rtf"	"insync.bmp"	"jobedit.bmp"	"about.bmp"	
# Begin Custom Build - Performing Custom Build insync.hpj
TargetDir=.\Release
InputPath=.\insync.hpj

"$(TargetDir)\insync.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcrtf -x insync.hpj 
	copy insync.hlp $(TargetDir) 
	del insync.hlp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "insync - Win32 Debug"

USERDEP__INSYN="insync.rtf"	"insync.bmp"	"jobedit.bmp"	"about.bmp"	
# Begin Custom Build - Performing Custom Build insync.hpj
TargetDir=.\Debug
InputPath=.\insync.hpj

"$(TargetDir)\insync.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcrtf -x insync.hpj 
	copy insync.hlp $(TargetDir) 
	del insync.hlp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "insync - Win32 Release Uncompressed"

USERDEP__INSYN="insync.rtf"	"insync.bmp"	"jobedit.bmp"	"about.bmp"	
# Begin Custom Build - Performing Custom Build insync.hpj
TargetDir=.\Release
InputPath=.\insync.hpj

"$(TargetDir)\insync.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcrtf -x insync.hpj 
	copy insync.hlp $(TargetDir) 
	del insync.hlp 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\insync.rtf
# End Source File
# Begin Source File

SOURCE=.\jobedit.bmp
# End Source File
# Begin Source File

SOURCE=.\profiles.bmp
# End Source File
# Begin Source File

SOURCE=.\regdialog.bmp
# End Source File
# End Group
# End Target
# End Project
