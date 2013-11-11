// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// wins\estart\estartmain.cpp
// 
//

#include <e32std.h>
#include <f32fsys.h>
#include <e32uid.h>
#include "estart.h"

#include <e32wins.h>
#include <emulator.h>
#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#include <windows.h>
#pragma warning( default : 4201 ) // nonstandard extension used : nameless struct/union



_LIT(KLitAddFsFail,"Add fs fail");
_LIT(KLitConnectFsFail2,"Connect fs 2 fail");
_LIT(KLitNoWS,"No WSERV");
_LIT(KStartupModeFail,"Get startup mode failed");
_LIT(KLocalFsName,"elocal");

class TWinsFSStartup : public TFSStartup
	{
public:
	virtual TInt StartSystem();
	virtual TInt DefaultLocalDrive(TInt aLocalDrive);
	virtual TPtrC LocalDriveMappingFileName();
#if !defined(AUTODETECT_DISABLE)		
	virtual TInt GetNextStandardFSInfoEntry(const SFileSystemInfo** anEntry,TInt aPos);
#endif
public:	
	void LocalFSInitialisation();
protected:
	TInt GetStartupMode();
	};

#if !defined(AUTODETECT_DISABLE)
TInt DetectEmulRAM(RLocalDrive ld, TInt cr, TLocalDriveCapsV2& caps)
	{
	(void)ld;
	(void)cr;
	if (caps.iType==EMediaRam)
		return KErrNone;
	return KErrGeneral;
	}

TInt DetectEmul_CF_FAT(RLocalDrive ld, TInt cr, TLocalDriveCapsV2& caps)
	{
	(void)ld;
	(void)cr;
	if (caps.iType!=EMediaHardDisk || !(caps.iDriveAtt & KDriveAttRemovable))
		return KErrGeneral;
	if (cr==KErrNotReady)
		return KErrNone;	// removable and not ready - assume fat
	if (cr==KErrNone && PartitionIsFAT(caps.iPartitionType))
		return KErrNone;
	return KErrGeneral;
	}

TInt DetectEmul_CF_FAT32(RLocalDrive ld, TInt cr, TLocalDriveCapsV2& caps)
	{
	(void)ld;
	(void)cr;
	if (caps.iType!=EMediaHardDisk || !(caps.iDriveAtt & KDriveAttRemovable))
		return KErrGeneral;
	if (cr==KErrNotReady)
		return KErrNone;	// removable and not ready - assume fat
	if (cr==KErrNone && (PartitionIsFAT32(caps.iPartitionType) || PartitionIsFAT(caps.iPartitionType)) )
		return KErrNone;
	return KErrGeneral;
	}
#endif
	
	
_LIT(KLocalDriveMappingFileSysBin,"Z:\\SYS\\DATA\\ESTART.TXT");
TPtrC TWinsFSStartup::LocalDriveMappingFileName()
	{	
	__ASSERT_ALWAYS(InitCompositeFileSystem()==KErrNone,User::Panic(_L("InitCompositeFileSystem failed."),0));
	return(KLocalDriveMappingFileSysBin());
	}
	
LOCAL_C void AutoRun()
	{
	// Ensure that any background initialisation completes before auto-running
	RProcess me;
	me.SetPriority(EPriorityLow);
	const char* autorun = EmulatorAutoRun();
	if (autorun)
		{
		TInt r = KErrNone;
		const char* cmdline = EmulatorCommandLine();
		TFileName exe;
		exe.Copy(TPtrC8((const TText8*)autorun));
		TPtrC8 c((const TText8*)cmdline);
		HBufC* command = HBufC::New(c.Length());
		if (!command)
			r = KErrNoMemory;
		else
			{
			TPtr ptr(command->Des());
			ptr.Copy(c);
			RProcess p;
			r = p.Create(exe, *command);
			if (r == KErrNone)
				{
				TRequestStatus s;
				p.Logon(s);
				me.SetPriority(EPriorityHigh);	// ensure we detect termination early
				p.Resume();
				p.Close();
				User::WaitForRequest(s);
				r = s.Int();
				}
			}
		Emulator::Escape();	// make sure we don't get suspended while exiting
		ExitProcess(r);
		}
	}
	
const TInt WinsDefaultLocalDrives[KMaxLocalDrives]=
			{
			EDriveY,               //0
			EDriveX,               //1 MMC - Slot 0, partition 0
			KDriveInvalid,         //2 MMC - Slot 0, partition 1
			EDriveT,               //3 MMC - Slot 1, partition 0
			KDriveInvalid,         //4 MMC - Slot 1, partition 1
			EDriveU,               //5 NAND - USER DATA
			EDriveV,               //6 NAND - CODE
			KDriveInvalid,         //7
			EDriveW,	           //8
			KDriveInvalid,         //9
			KDriveInvalid,         //10
			KDriveInvalid,         //11
			KDriveInvalid,         //12
			KDriveInvalid,         //13
			KDriveInvalid,         //14
			KDriveInvalid          //15
			};
			
TInt TWinsFSStartup::DefaultLocalDrive(TInt aLocalDrive)
	{
	
	return(WinsDefaultLocalDrives[aLocalDrive]);
	}

#if !defined(AUTODETECT_DISABLE)	
const TInt KMaxWinsFSInfoTableEntries=7;	
LOCAL_D const SFileSystemInfo FileSystems[KMaxWinsFSInfoTableEntries] =
	{
		{DetectEmulRAM,      _S("efat32"),      _S("fat"),      0,             FS_FORMAT_CORRUPT},
		{DetectEmul_CF_FAT32,_S("efat32"),      _S("fat"),      0,             FS_FORMAT_CORRUPT},
		{DetectFtl,          _S("efat32"),      _S("fat"),      0,             FS_FORMAT_CORRUPT},
		{DetectRofs,         _S("erofs"),       _S("rofs"),     0,             FS_DISMNT_CORRUPT},
		{DetectEneaLFFS,     _S("elffs"),       _S("lffs"),     0,             FS_FORMAT_CORRUPT},
		{DetectIso9660,      _S("iso9660"),     0,              0,             0},
		{DetectNtfs,         _S("ntfs"),        0,              0,             0},
	};

// additional structure for DetectFtl entry with extension set to "0"
LOCAL_D const SFileSystemInfo FileSystemForDetectFtl =
		{DetectFtl,         _S("efat32"),         _S("fat"),      0,	 			  FS_FORMAT_CORRUPT};


TInt TWinsFSStartup::GetNextStandardFSInfoEntry(const SFileSystemInfo** anEntry,TInt aPos)
	{
    if (aPos<KMaxWinsFSInfoTableEntries)
		{
		*anEntry=&FileSystems[aPos];
		return(KErrNone);
		}
	else
		return(KErrNotFound);	
	}
#endif		
	
void TWinsFSStartup::LocalFSInitialisation()
//
// Add and mount local file system
//
	{
	
	DEBUGPRINT("LocalFSInitialisation");

	// Add local file system
	TInt r=iFs.AddFileSystem(KLocalFsName);
	if (r!=KErrNone)
		User::Panic(KLitAddFsFail,r);

	// Close and reopen to make sure the session path is updated
	iFs.Close();
	r = iFs.Connect();
	if (r!=KErrNone)
		User::Panic(KLitConnectFsFail2,r);

	TInt i;
	_LIT(KLitLocalFsName,"Win32");
	TChar drive;
	TBuf<4> root=_L("?:\\");
	for(i=0;i<KMaxDrives-1;++i)
		{
		r=RFs::DriveToChar(i,drive);
		if(r!=KErrNone)
			continue;
		root[0]=(TText)drive;
		TFileName dummy;
		if (MapEmulatedFileName(dummy,root)==KErrNone)
			{
			r=iFs.MountFileSystem(KLitLocalFsName,i);
			DEBUGPRINT2("MountFileSystem() on drive %d r=%d",i,r);
			}
		}
	}

_LIT(KWindowServerRootName1, "EWSRV.EXE");
_LIT(KWindowServerRootName2, "WSERV.EXE");	
_LIT(KSystemStarterName, "z:\\sys\\bin\\SYSSTART.EXE");
_LIT(KSystemStateManager, "z:\\sys\\bin\\SYSSTATEMGR.EXE");
_LIT(KPropertyError, "RProperty return error");

TInt TWinsFSStartup::StartSystem()
	{
	// Get boot mode with need to access file server
	TInt r = GetStartupModeFromFile();
	if (r != KErrNotSupported)
        {
        if (r != KErrNone)
		    User::Panic(KStartupModeFail,r);
        // Update startup mode property value
        r = RProperty::Set(KUidSystemCategory, KSystemStartupModeKey, iStartupMode);
        if (r != KErrNone)
            User::Panic(KPropertyError, r);
        }

	// Launch system starter and return unless in noGui or textshell mode
	TBool noSysStart = EFalse;
	UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalBoolProperty,  (TAny*)"_NOSYSSTART",  &noSysStart);
	
	if (!EmulatorNoGui() && !EmulatorTextShell() && !EmulatorMiniGui() && !noSysStart)
		{
		RProcess ws;
		r=ws.Create(KSystemStateManager, KNullDesC);
		if (r!=KErrNone)
			{
			r=ws.Create(KSystemStarterName, KNullDesC);
			if (r==KErrNone)
				{
				TRequestStatus status;
				ws.Rendezvous(status);
				ws.Resume();		
				User::WaitForRequest(status);
				// Some system starters may not exit on completion.
				if (ws.ExitType() != EExitKill && ws.ExitType() != EExitPending)
					User::Panic(_L("SysStart died"),status.Int());
				ws.Close();
				return KErrNone;
				}
			}
		else
			{
			TRequestStatus status;
			ws.Rendezvous(status);
			ws.Resume();		
			User::WaitForRequest(status);
			if (ws.ExitType() != EExitKill && ws.ExitType() != EExitPending)
				User::Panic(_L("SysStart died"),status.Int());
			ws.Close();
			return KErrNone;
			}
		}

	// Start system agent 2, continue if executable not found, otherwise panic
	r = StartSysAgt2();
	if (r!=KErrNone && r!=KErrNotFound)
		User::Panic(_L("Error launching System Agent"),r);

	// If noGui specifed, then we're done
	if (EmulatorNoGui())
		return KErrNone;

	// Attempt to start the appropriate window server
	TDriveList list;
	iFs.DriveList(list);
	if (!EmulatorTextShell() && CreateServer(list,KWindowServerRootName2))
		return KErrNone;
	if (!CreateServer(list,KWindowServerRootName1))
		User::Panic(KLitNoWS,KErrNotFound);

	return KErrNone;
	}

TInt TWinsFSStartup::GetStartupMode()
    {
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty, 
						 (TAny*)"StartupMode", &iStartupMode);
	return(KErrNone);
    }
	
GLDEF_C TInt E32Main()
	{
	
	TWinsFSStartup fsStart;
	fsStart.Init();
	
	fsStart.LocalFSInitialisation();
	fsStart.Run();
	
	fsStart.StartSystem();
	
	fsStart.Close();
	
	AutoRun();
	
	return(0);
	}
		
