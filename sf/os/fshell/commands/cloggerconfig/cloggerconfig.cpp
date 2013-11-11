// cloggerconfig.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <fshell/clogger.h>
#include <BADESCA.H>
#include <e32msgqueue.h>

using namespace IoUtils;

class CCmdCloggerConfig : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdCloggerConfig();
private:
	CCmdCloggerConfig();
	void PrintLogL();
	void GetBackupFileNameL(TFileName& aName);
	void SetAllL(TBool aEnabled);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RClogger iClogger;
	RCloggerLogConsumer iLogGetter;
	TUint iGlobalOptions;
	TBool iPersist;
	TBool iReset;
	HBufC* iTagName;
	TUint iEnabledMask;
	TUint iRotateOptions;
	TBool iRotate;
	TBool iFollow;
	TBool iWipe;
	TInt iNumBuffers;
	TInt iBufferSize;
	RBuf iTempBuffer;
	TBool iRdebug;
	TBool iBackup;
	TBool iRestore;
	TBool iDisableAll;
	TBool iEnableAll;
	};


CCommandBase* CCmdCloggerConfig::NewLC()
	{
	CCmdCloggerConfig* self = new(ELeave) CCmdCloggerConfig();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdCloggerConfig::~CCmdCloggerConfig()
	{
	iClogger.Close();
	iLogGetter.Close();
	delete iTagName;
	iTempBuffer.Close();
	}

CCmdCloggerConfig::CCmdCloggerConfig()
	{
	}

const TDesC& CCmdCloggerConfig::Name() const
	{
	_LIT(KName, "cloggerconfig");	
	return KName;
	}

#define WRITEIF(opt, flag) if (opt & (RClogger :: flag)) { Write(_L(#flag)); Write(_L(" ")); }

void CCmdCloggerConfig::DoRunL()
	{
	if (iEnableAll && iDisableAll)
		{
		LeaveIfErr(KErrArgument, _L("Can't enable and disable all tags"));
		}

	User::LeaveIfError(iClogger.Connect());

	TBool listSettings = ETrue;
	if (iReset)
		{
		iClogger.ResetSettings();
		listSettings = EFalse;
		}
	
	if (iOptions.IsPresent(&iGlobalOptions))
		{
		iClogger.SetGlobalOptions(iGlobalOptions);
		listSettings = EFalse;
		}
	if (iOptions.IsPresent(&iRotateOptions))
		{
		TInt n;
		iClogger.GetRotateBehaviour(&n);
		iClogger.SetRotateBehaviour(n, iRotateOptions);
		listSettings = EFalse;
		}
	if (iNumBuffers || iBufferSize)
		{
		if (iNumBuffers == 0)
			{
			// Need to read it in as we have to set both at once.  Bad API...
			iClogger.GetRamBufferSize(&iNumBuffers);
			}
		if (iBufferSize == 0)
			{
			// Same reason as above
			iBufferSize = iClogger.GetRamBufferSize(NULL);
			}
		iClogger.SetRamBufferSize(iBufferSize, iNumBuffers);
		listSettings = EFalse;
		}
	if (iPersist || iBackup || iRestore)
		{
		// We don't do anything here, we take action at the end of this function
		listSettings = EFalse;
		}
	if (iRotate)
		{
		TFileName name;
		TInt err = iClogger.Rotate(name);
		Printf(_L("Log rotated to %S\r\n"), &name);
		if (err) PrintError(err, _L("Rotate returned error"));
		listSettings = EFalse;
		}
	else if (iWipe)
		{
		TInt oldLogs;
		TUint rotopt = iClogger.GetRotateBehaviour(&oldLogs);
		iClogger.SetRotateBehaviour(KMaxTInt, RClogger::EDoNothingSpecial); // Change rotate behaviour temporarily
		
		TFileName name;
		TInt err = iClogger.Rotate(name);
		//Printf(_L("Log rotated to %S\r\n"), &name);
		if (err) PrintError(err, _L("Rotate returned error"));
		else
			{
			err = FsL().Delete(name);

			iClogger.SetRotateBehaviour(oldLogs, rotopt); // Restore old behaviour
			LeaveIfErr(err, _L("Couldn't remove wiped log %S"), &name);
			}

		listSettings = EFalse;
		}

	if (iDisableAll)
		{
		SetAllL(EFalse);
		listSettings = EFalse;
		}

	if (iEnableAll)
		{
		SetAllL(ETrue);
		listSettings = EFalse;
		}

	if (iRdebug)
		{
		TUint opts = iClogger.GetGlobalOptions();
		opts |= RClogger::ERedirectRDebugPrintToClogger;
		iClogger.SetGlobalOptions(opts);
		
		PrintLogL();
		}

	if (iFollow)
		{
		PrintLogL(); // Does not return
		}

	if (iArguments.IsPresent(1))
		{
		// Set
		TInt err = iClogger.SetEnabled(*iTagName, iEnabledMask);
		if (err == KErrNotFound)
			{
			PrintError(err, _L("Tag does not exist, couldn't set it. (NOTE: This behaviour will be changed in the future so you can set tags that haven't been created yet!)"));
			User::Leave(KErrNotFound);
			}
		LeaveIfErr(err, _L("Couldn't enable tag."));
		listSettings = EFalse;
		}
	else if (iArguments.IsPresent(0))
		{
		// Get
		TUint32 e = iClogger.IsEnabled(*iTagName);
		Printf(_L("Tag [%S] enabled: 0x%x\r\n"), iTagName, e);
		listSettings = EFalse;
		}
	
	if (listSettings)
		{
		// List global options
		TUint globalOptions = iClogger.GetGlobalOptions();
		Printf(_L("Global options: 0x%x ( "), globalOptions);
		if (!globalOptions)
			{
			Write(_L("None "));
			}
		else
			{
			WRITEIF(globalOptions, EBufferLog);
			WRITEIF(globalOptions, EMirrorToRDebugPrint);
			WRITEIF(globalOptions, EMirrorToBluetooth);
			WRITEIF(globalOptions, EMirrorToMessageQueue);
			WRITEIF(globalOptions, ERedirectRDebugPrintToClogger);
			WRITEIF(globalOptions, EDisableFileWriter);
			}
		Write(_L(")\r\n"));

		// List rotate options
		TInt numLogs = 0;
		TUint rotateBehaviour = iClogger.GetRotateBehaviour(&numLogs);
		Printf(_L("Number of rotated logs to keep: %i\r\n"), numLogs);
		Printf(_L("Rotate behaviour: 0x%x ( "), rotateBehaviour);
		if (!rotateBehaviour)
			{
			Write(_L("EDoNothingSpecial "));
			}
		else
			{
			WRITEIF(rotateBehaviour, ECopyRotatedToExternalMedia);
			WRITEIF(rotateBehaviour, EAutoRotateAtStartup);
			WRITEIF(rotateBehaviour, ECompressRotatedLogs);
			}
		Write(_L(")\r\n"));

		// List buffer size
		TInt numBuffers = 0;
		TInt bufSize = iClogger.GetRamBufferSize(&numBuffers);
		Printf(_L("Number of RAM buffers: %i\r\n"), numBuffers);
		Printf(_L("Size of RAM buffers: %i\r\n"), bufSize);

		// List tags
		CDesC16Array* tags = NULL;
		RBuf8 enabled;
		iClogger.GetTagStatesL(tags, enabled);
		TInt count = enabled.Size() / 4;
		if (count) Printf(_L("\r\n"));
		for (TInt i = 0; i < count; i++)
			{
			TPtrC tag = tags->operator[](i);
			TUint32 e = ((TUint32*)enabled.Ptr())[i];
			Printf(_L("Tag [%S] enabled: 0x%x\r\n"), &tag, e);
			}
		delete tags;
		enabled.Close();
		}

	if (iPersist)
		{
		iClogger.PersistSettings();
		}

	if (iBackup)
		{
		// Args should be: --exec 'cp -ro C:\Private\10202be9\persists\10272efd.cre E:\Private\10202be9\persists\10272efd.cre'
		TFileName args;
		GetBackupFileNameL(args);

		TInt len = args.Length();
		args.Append(args);
		args.Append('\'');
		args[0] = 'C';
		args.Insert(len, _L(" "));
		args.Insert(0, _L("--exec 'cp -ro "));
		
		RChildProcess shell;
		shell.CreateL(_L("fshell.exe"), args, IoSession(), Stdin(), Stdout(), Stderr());
		CleanupClosePushL(shell);
		TRequestStatus stat;
		shell.Run(stat);
		User::WaitForRequest(stat);
		LeaveIfErr(stat.Int(), _L("Failed to copy cenrep file"));
		CleanupStack::PopAndDestroy(&shell);

		const TPtrC name = args.Right(len+1);
		Printf(_L("Settings backed up to '%S, use --restore option to restore.\r\n"), &name);
		}
	else if (iRestore)
		{
		TFileName args;
		GetBackupFileNameL(args);
		TInt len = args.Length();
		//TEntry e;
		//LeaveIfErr(FsL().Entry(args, e), _L("No backup file found"));
		// Args should be: --exec 'cp E:\Private\10202be9\persists\10272efd.cre C:\Private\10202be9\persists\10272efd.cre'
		args.Append(args);
		args.Append('\'');
		args[len] = 'C';
		args.Insert(len, _L(" "));
		args.Insert(0, _L("--exec 'cp -ro "));
		
		RChildProcess shell;
		shell.CreateL(_L("fshell.exe"), args, IoSession(), Stdin(), Stdout(), Stderr());
		CleanupClosePushL(shell);
		TRequestStatus stat;
		shell.Run(stat);
		User::WaitForRequest(stat);
		LeaveIfErr(stat.Int(), _L("Failed to copy cenrep file"));
		CleanupStack::PopAndDestroy(&shell);

		Printf(_L("Settings restored. You must reboot or restart cloggerserver to get the new settings\r\n"));
		}
	}

void CCmdCloggerConfig::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iTagName, _L("tag_name"));
	aArguments.AppendUintL(iEnabledMask, _L("enabled_mask"));
	}

void CCmdCloggerConfig::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendUintL(iGlobalOptions, _L("set-global"));
	aOptions.AppendBoolL(iPersist, _L("persist"));
	aOptions.AppendBoolL(iReset, _L("reset"));
	aOptions.AppendUintL(iRotateOptions, _L("set-rotate"));
	aOptions.AppendBoolL(iRotate, _L("rotate"));
	aOptions.AppendBoolL(iFollow, _L("follow"));
	aOptions.AppendIntL(iNumBuffers, _L("num-buffers"));
	aOptions.AppendUintL((TUint&)iBufferSize, _L("buffer-size"));
	aOptions.AppendBoolL(iWipe, _L("wipe"));
	aOptions.AppendBoolL(iRdebug, _L("rdebug"));
	aOptions.AppendBoolL(iBackup, _L("backup"));
	aOptions.AppendBoolL(iRestore, _L("restore"));
	aOptions.AppendBoolL(iDisableAll, _L("disable-all"));
	aOptions.AppendBoolL(iEnableAll, _L("enable-all"));
	}


EXE_BOILER_PLATE(CCmdCloggerConfig)

void CCmdCloggerConfig::PrintLogL()
	{
	// Because of how RCloggerLogConsumer API works we can only 'see' things in the clogger ChunkForBufs. This however
	// won't be where the descriptor being logged will be unless buffered logging is enabled. To fix properly,
	// the rdebug chunk would need to be mapped into the client, and also the Server's iSessionTempBuf and 
	// CDebugRouterClient's iTempBuf would need to be moved to an accessible chunk
	TUint opts = iClogger.GetGlobalOptions();
	opts |= RClogger::EBufferLog;
	iClogger.SetGlobalOptions(opts);

	LeaveIfErr(iLogGetter.Connect(), _L("Couldn't connect to RCloggerLogConsumer"));
	iTempBuffer.CreateL(2048);

	TRequestStatus stat;
	TPtrC8 logLine;
	while (ETrue)
		{
		iLogGetter.GetNextLog(stat, logLine);
		User::WaitForRequest(stat);
		LeaveIfErr(stat.Int(), _L("GetNextLog returned error"));
		
		// CCommandBase::Write doesn't take 8-bit descriptor so write in iTempBuffer-sized chunks
		TPtrC8 lineFrag(logLine);
		const TInt buflen = iTempBuffer.MaxLength();
		while (lineFrag.Length())
			{
			TPtrC8 frag = lineFrag.Left(buflen);
			iTempBuffer.Copy(frag);
			//HexDumpL(frag, iTempBuffer);
			TInt err = Stdout().Write(iTempBuffer);
			LeaveIfErr(err, _L("Write of %d bytes (%d total) failed"), iTempBuffer.Length(), logLine.Length());
			lineFrag.Set(lineFrag.Mid(frag.Length()));
			}
		}
	}

void CCmdCloggerConfig::GetBackupFileNameL(TFileName& aName)
	{
	TInt err = KErrNone;
	TInt theDrive = KErrNotFound;
	// Figure out the first external media device
	for (TInt drive = EDriveA; drive <= EDriveZ; drive++)
		{
		TVolumeInfo info;
		err = FsL().Volume(info, drive);
		if (err == KErrNone)
			{
			//LogNote(ELogDisks, _L8("Found drive %c iDrivaAtt=0x%x iType=0x%x"), 'A' + drive, info.iDrive.iDriveAtt, info.iDrive.iType);
			}
		if (err == KErrNone && ((info.iDrive.iDriveAtt & KDriveAttRemovable) || info.iDrive.iType == EMediaHardDisk)) // Added the check for hard disk so we can use _epoc_drive_d in the emulator
			{
			// Found one
			theDrive = drive;
			break;
			}
		}
	LeaveIfErr(theDrive, _L("Couldn't find a removable drive to backup on to"));

	aName = _L("?:\\private\\10202be9\\persists\\10272efd.cre");
	TChar c;
	Fs().DriveToChar(theDrive, c);
	aName[0] = c;
	}

void CCmdCloggerConfig::SetAllL(TBool aEnabled)
	{
	CDesC16Array* tags = NULL;
	RBuf8 states;
	CleanupClosePushL(states);
	iClogger.GetTagStatesL(tags, states);
	CleanupStack::PushL(tags);
	if (aEnabled)
		{
		states.Fill(TChar(0xff));
		}
	else
		{
		states.FillZ();
		}
	iClogger.SetTagStatesL(tags, states);
	CleanupStack::PopAndDestroy(2, &states);
	}
