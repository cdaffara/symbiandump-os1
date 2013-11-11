// commands.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32debug.h>
#include <e32rom.h>
#include <f32dbg.h>
#include <f32image.h>
#include <hal.h>
#include <fshell/common.mmh>
#include <fshell/ltkutils.h>
#include <fshell/descriptorutils.h>
#include <fshell/heaputils.h>
using LtkUtils::RAllocatorHelper;
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
#include <fshell/memoryaccess.h>
#endif
#include "fshell.h"
#include "commands.h"
#include "command_factory.h"
#include "string_utils.h"


//
// Constants.
//

_LIT(KOptVerbose, "verbose");
_LIT(KOptHuman, "human");

_LIT(KNewLine, "\r\n");
_LIT(KTab, "\t");
_LIT(KRepeatCount, "REPEAT_COUNT");
const TInt KBlockSize = 512;

_LIT(KMatchAll, "*");


//
// CCmdHelp.
//

CCommandBase* CCmdHelp::NewLC()
	{
	CCmdHelp* self = new(ELeave) CCmdHelp();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdHelp::~CCmdHelp()
	{
	delete iFormatter;
	}

CCmdHelp::CCmdHelp() : CCommandBase(ECompleteOnRunL)
	{
	}

const TDesC& CCmdHelp::Name() const
	{
	_LIT(KCmdHelpName, "help");
	return KCmdHelpName;
	}

void CCmdHelp::DoRunL()
	{
	if (iOptions.IsPresent(&iCount))
		{
		const TInt count = gShell->CommandFactory().CountUniqueCommandsL();
		Complete(_L("%d\r\n"), count);
		}
	else
		{
		RArray<TPtrC> commands;
		CleanupClosePushL(commands);
		gShell->CommandFactory().ListCommandsL(commands);
		if (!Stdout().AttachedToConsole())
			{
			// Print them one per line (like how ls does it when not attached to a console)
			for (TInt i = 0; i < commands.Count(); i++)
				{
				// The async writing is a legacy from when help was a local command. We don't need to observe it now.
				Printf(_L("%S\r\n"), &commands[i]);
				}
			Complete();
			}
		else
			{
			iFormatter = CTextFormatter::NewL(Stdout());
			iFormatter->ColumnizeL(0, 2, commands.Array());
			Stdout().Write(iFormatter->Descriptor(), iStatus);
			SetActive();
			}
		CleanupStack::PopAndDestroy(&commands);
		}
	}

void CCmdHelp::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptCount, "count");
	aOptions.AppendBoolL(iCount, KOptCount);
	}


//
// CCmdExit.
//

CCommandBase* CCmdExit::NewLC()
	{
	CCmdExit* self = new(ELeave) CCmdExit();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdExit::~CCmdExit()
	{
	}

CCmdExit::CCmdExit()
	{
	}

const TDesC& CCmdExit::Name() const
	{
	_LIT(KCmdExitName, "exit");
	return KCmdExitName;
	}

void CCmdExit::DoRunL()
	{
	// Note, this command should never execute as 'exit' has handled explicitly by CParser.
	// It exists so that 'exit' appears in fshell's help list and also to support 'exit --help'.
	LeaveIfErr(KErrGeneral, _L("Invalid context for 'exit' command (must be run as a bare command, not in a pipe-line)"));
	}


//
// CCmdLs.
//

CCommandBase* CCmdLs::NewLC()
	{
	CCmdLs* self = new(ELeave) CCmdLs();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCmdLs::~CCmdLs()
	{
	delete iFormatter;
	}

CCmdLs::CCmdLs()
	{
	}

void CCmdLs::ConstructL()
	{
	BaseConstructL();
	}

void CCmdLs::PrintDirContentL(RDir& aDir)
	{
	TInt err = KErrNone;
	TEntry entry;
	while (err == KErrNone)
		{
		err = aDir.Read(entry);
		if (err == KErrNone)
			{
			FormatEntryL(entry);
			Write(iFormatter->Descriptor());
			iFormatter->Zero();

			if (iOptRecurse && entry.IsDir())
				{
				RecursiveScanDirL(entry.iName);
				}
			}
		}
	if (err == KErrEof)
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	}

void CCmdLs::PrintDirContentL(const CDir& aDir)
	{
	IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x100);

	const TInt numEntries = aDir.Count();
	for (TInt i = 0; i < numEntries; ++i)
		{
		const TEntry& entry = aDir[i];
		buf->AppendL(entry.iName);
		buf->AppendL(_L("\t"));
		}

	iFormatter->ColumnizeL(0, 2, buf->Descriptor());
	Write(iFormatter->Descriptor());
	CleanupStack::PopAndDestroy(buf);
	}

void CCmdLs::FormatEntryL(const TEntry& aEntry)
	{
	if (iOptLong)
		{
		iFormatter->AppendFormatL(_L("%c%c%c%c%c  "), aEntry.IsDir() ? 'd' : '-', aEntry.IsReadOnly() ? 'r' : '-', aEntry.IsHidden() ? 'h' : '-', aEntry.IsSystem() ? 's' : '-', aEntry.IsArchive() ? 'a' : '-');
		if (iOptHuman)
			{
			iFormatter->AppendHumanReadableSizeL(aEntry.iSize);
			iFormatter->AppendL(_L("  "));
			}
		else
			{
			iFormatter->AppendFormatL(_L("%+ 10d  "), aEntry.iSize);
			}
		aEntry.iModified.FormatL(iTempBuf, _L("%1%/1%2%/2%3 %H%:1%T%:2%S "));
		TPtrC pathRelativeToBaseDir = iFileName.Mid(iBaseDir.Length());
		iFormatter->AppendFormatL(_L("%S%S%S\r\n"), &iTempBuf, &pathRelativeToBaseDir, &aEntry.iName);
		}
	else
		{
		TPtrC pathRelativeToBaseDir = iFileName.Mid(iBaseDir.Length());
		iFormatter->AppendFormatL(_L("%S%S\r\n"), &pathRelativeToBaseDir, &aEntry.iName);
		}
	}

const TDesC& CCmdLs::Name() const
	{
	_LIT(KCmdLsName, "ls");
	return KCmdLsName;
	}

void CCmdLs::DoRunL()
	{
	if (iFileName.Length() == 0)
		{
		iFileName = Env().Pwd();
		}

	if (!iFileName.IsWild() && !iFileName.IsDriveRoot())
		{
		LeaveIfFileNotFound(iFileName);
		if (iFileName.IsDirL(FsL()))
			{
			iFileName.SetIsDirectoryL();
			}
		}

	if (iOptRecurse || iOptLong || !Stdout().AttachedToConsole())
		{
		iOptOnePerLine = ETrue;
		}

	iFormatter = CTextFormatter::NewL(Stdout());
	iBaseDir.Set(TParsePtrC(iFileName).DriveAndPath());

	if (iOptOnePerLine)
		{
		DoScanDirL();
		}
	else
		{
		CDir* files;
		LeaveIfErr(FsL().GetDir(iFileName, iOptAll ? KEntryAttMaskSupported : KEntryAttNormal | KEntryAttDir, ESortByName, files), _L("Couldn't read whole directory into memory - try using -1"));
		CleanupStack::PushL(files);
		PrintDirContentL(*files);
		CleanupStack::PopAndDestroy(files);
		}
	}

void CCmdLs::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdLsOptAll, "all");
	_LIT(KCmdLsOptLong, "long");
	_LIT(KCmdLsOptHuman, "human");
	_LIT(KCmdLsOptOnePerLine, "one");
	_LIT(KCmdLsOpRecurse, "recurse");

	aOptions.AppendBoolL(iOptAll, KCmdLsOptAll);
	aOptions.AppendBoolL(iOptLong, KCmdLsOptLong);
	aOptions.AppendBoolL(iOptHuman, KCmdLsOptHuman);
	aOptions.AppendBoolL(iOptOnePerLine, KCmdLsOptOnePerLine);
	aOptions.AppendBoolL(iOptRecurse, KCmdLsOpRecurse);
	}

void CCmdLs::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdLsArg, "dir_name");
	aArguments.AppendFileNameL(iFileName, KCmdLsArg);
	}

void CCmdLs::RecursiveScanDirL(const TDesC& aName)
	{
	TInt currentDirectoryLen = iFileName.Length();
	iFileName.AppendComponentL(aName, TFileName2::EDirectory);
	DoScanDirL();
	iFileName.SetLength(currentDirectoryLen); // Since this is called recursively, restore iTempName to what it was previously
	}

void CCmdLs::DoScanDirL()
	{
	RDir dir;
	User::LeaveIfError(dir.Open(FsL(), iFileName, iOptAll ? KEntryAttMaskSupported : KEntryAttNormal | KEntryAttDir));
	CleanupClosePushL(dir);
	iFileName.SetLength(TParsePtrC(iFileName).DriveAndPath().Length()); // Remove any possible wildcard or name from the end of iFileName before recursing
	PrintDirContentL(dir);
	CleanupStack::PopAndDestroy(&dir);
	}

//
// CCmdCd.
//

CCommandBase* CCmdCd::NewLC()
	{
	CCmdCd* self = new(ELeave) CCmdCd();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdCd::~CCmdCd()
	{
	}

CCmdCd::CCmdCd()
	: iDir(TFileName2::EDirectory)
	{
	}

const TDesC& CCmdCd::Name() const
	{
	_LIT(KCmdCdName, "cd");
	return KCmdCdName;
	}

void CCmdCd::DoRunL()
	{
	if (iDir.IsDriveRoot()) // For some reason best known to the designers of F32, it's not possible to retrieve a TEntry object for the root of a drive (fails with KErrBadName).
		{
		TDriveList driveList;
		LeaveIfErr(FsL().DriveList(driveList), _L("Couldn't get drive list"));
		TInt driveNum = TChar(iDir[0]).GetUpperCase() - 'A';
		if (driveNum >= KMaxDrives)
			{
			Complete(KErrArgument, _L("Invalid drive letter"));
			return;
			}
		if (!driveList[driveNum])
			{
			Complete(KErrNotReady, _L("Drive not present"));
			return;
			}
		}
	else
		{
		iDir.Normalize(FsL());
		LeaveIfFileNotFound(iDir);
		if (!iDir.IsDirL(FsL()))
			{
			Complete(KErrBadName, _L("%S is not a directory"), &iDir);
			return;
			}
		}

	Env().SetPwdL(iDir);
	}

void CCmdCd::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdCdArg, "directory");
	aArguments.AppendFileNameL(iDir, KCmdCdArg);
	}


//
// CCmdClear.
//

CCommandBase* CCmdClear::NewLC()
	{
	CCmdClear* self = new(ELeave) CCmdClear();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdClear::~CCmdClear()
	{
	}

CCmdClear::CCmdClear()
	{
	}

const TDesC& CCmdClear::Name() const
	{
	_LIT(KCmdClearName, "clear");
	return KCmdClearName;
	}

void CCmdClear::DoRunL()
	{
	if (iFormFeed)
		{
		TInt consoleHeight = Stdout().GetScreenSizeL().iHeight;
		Stdout().SetCursorPosAbsL(TPoint(0, consoleHeight-1));
		RBuf buf;
		buf.CreateL(consoleHeight * 2);
		while (consoleHeight--) buf.Append(KNewLine);
		Stdout().Write(buf);
		buf.Close();
		Stdout().SetCursorPosAbsL(TPoint(0,0));
		}
	else
		{
		Stdout().ClearScreenL();
		Stdin().SetMode(RIoReadWriteHandle::EText);
		Stdout().SetMode(RIoReadWriteHandle::EText);
		Stderr().SetMode(RIoReadWriteHandle::EText);
		}
	}

void CCmdClear::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptFormFeed, "formfeed");
	aOptions.AppendBoolL(iFormFeed, KOptFormFeed);
	}

//
// CForegroundAdjuster
//

class CForegroundAdjuster : public CActive
	{
public:
	static CForegroundAdjuster* NewL(CCommandBase& aCommand, TInt aJobId, TBool aToForeground);
	~CForegroundAdjuster();
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
private:
	CForegroundAdjuster(CCommandBase& aCommand, TInt aJobId, TBool aToForeground);
private:
	CCommandBase& iCommand;
	TInt iJobId;
	TBool iToForeground;
	};

CForegroundAdjuster* CForegroundAdjuster::NewL(CCommandBase& aCommand, TInt aJobId, TBool aToForeground)
	{
	return new(ELeave) CForegroundAdjuster(aCommand, aJobId, aToForeground);
	}

void CForegroundAdjuster::RunL()
	{
	gShell->ClaimJobsLockLC();

	if (iToForeground)
		{
		gShell->BringJobToForeground(iJobId);
		}
	else
		{
		gShell->SendJobToBackground(iJobId);
		}

	CleanupStack::PopAndDestroy(); // jobs lock.
	iCommand.Complete();
	}

void CForegroundAdjuster::DoCancel()
	{
	}

TInt CForegroundAdjuster::RunError(TInt aError)
	{
	iCommand.Complete(aError);
	return KErrNone;
	}

CForegroundAdjuster::CForegroundAdjuster(CCommandBase& aCommand, TInt aJobId, TBool aToForeground)
	: CActive(CActive::EPriorityStandard), iCommand(aCommand), iJobId(aJobId), iToForeground(aToForeground)
	{
	CActiveScheduler::Add(this);
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

CForegroundAdjuster::~CForegroundAdjuster()
	{
	}


//
// CCmdFg.
//

void ParseJobSpecL(const TDesC& aString, TInt& aJobId)
	{
	TLex lex(aString);
	lex.SkipSpace();
	if (lex.Peek() == '%')
		{
		lex.Get();
		}
	User::LeaveIfError(lex.Val(aJobId));
	}

CCommandBase* CCmdFg::NewLC()
	{
	CCmdFg* self = new(ELeave) CCmdFg();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdFg::~CCmdFg()
	{
	delete iJobSpec;
	delete iForegroundAdjuster;
	}

CCmdFg::CCmdFg() : CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdFg::Name() const
	{
	_LIT(KCmdFgName, "fg");
	return KCmdFgName;
	}

void CCmdFg::DoRunL()
	{
	TInt jobId = -1;
	if (iJobSpec)
		{
		ParseJobSpecL(*iJobSpec, jobId);
		}
	else
		{
		gShell->ClaimJobsLockLC();
		const RPointerArray<CJob>& jobs = gShell->Jobs();
		const TInt numJobs = jobs.Count();
		if (numJobs >= 2)
			{
			jobId = jobs[numJobs - 2]->Id(); // Note, the most recent job (jobs[numJobs - 1]) will be this command.
			}
		CleanupStack::PopAndDestroy(); // jobs lock.
		}
	if (jobId >= 0)
		{
		iForegroundAdjuster = CForegroundAdjuster::NewL(*this, jobId, ETrue);
		}
	else
		{
		Complete();
		}
	}

void CCmdFg::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdFgArg, "job_spec");
	aArguments.AppendStringL(iJobSpec, KCmdFgArg);
	}


//
// CCmdBg.
//

CCommandBase* CCmdBg::NewLC()
	{
	CCmdBg* self = new(ELeave) CCmdBg();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdBg::~CCmdBg()
	{
	delete iJobSpec;
	delete iForegroundAdjuster;
	}

CCmdBg::CCmdBg() : CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdBg::Name() const
	{
	_LIT(KCmdBgName, "bg");
	return KCmdBgName;
	}

void CCmdBg::DoRunL()
	{
	TInt jobId = -1;
	if (iJobSpec)
		{
		ParseJobSpecL(*iJobSpec, jobId);
		}
	else
		{
		gShell->ClaimJobsLockLC();
		const RPointerArray<CJob>& jobs = gShell->Jobs();
		const TInt numJobs = jobs.Count();
		if (numJobs >= 2)
			{
			jobId = jobs[numJobs - 2]->Id(); // Note, the most recent job (jobs[numJobs - 1]) will be this command.
			}
		CleanupStack::PopAndDestroy(); // jobs lock.
		}
	if (jobId >= 0)
		{
		iForegroundAdjuster = CForegroundAdjuster::NewL(*this, jobId, EFalse);
		}
	}

void CCmdBg::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdBgArg, "job_spec");
	aArguments.AppendStringL(iJobSpec, KCmdBgArg);
	}


//
// CCmdJobs.
//

CCommandBase* CCmdJobs::NewLC()
	{
	CCmdJobs* self = new(ELeave) CCmdJobs();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdJobs::~CCmdJobs()
	{
	delete iFormatter;
	}

CCmdJobs::CCmdJobs() : CCommandBase(ECompleteOnRunL)
	{
	}

const TDesC& CCmdJobs::Name() const
	{
	_LIT(KCmgJobsName, "jobs");
	return KCmgJobsName;
	}

void CCmdJobs::DoRunL()
	{
	iFormatter = CTextFormatter::NewL(Stdout());
	gShell->ClaimJobsLockLC();
	const RPointerArray<CJob>& jobs = gShell->Jobs();
	const TInt numJobs = (jobs.Count() - 1);
	if (numJobs > 0)
		{
		IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x100);
		for (TInt i = 0; i < numJobs; ++i)
			{
			const CJob& job = *(jobs[i]);
			buf->AppendFormatL(_L("[%d]\t%S\t%S\r\n"), job.Id(), ShStringify::JobStatus(job.Status()), job.Name());
			}
		iFormatter->TabulateL(0, 2, buf->Descriptor());
		CleanupStack::PopAndDestroy(2); // buf, jobs lock.
		Stdout().Write(iFormatter->Descriptor(), iStatus);
		SetActive();
		}
	else
		{
		CleanupStack::PopAndDestroy(); // jobs lock.
		Complete();
		}
	}


//
// CCmdRm.
//

CCommandBase* CCmdRm::NewLC()
	{
	CCmdRm* self = new(ELeave) CCmdRm();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRm::~CCmdRm()
	{
	delete iFileMan;
	iFileNames.Close();
	iNonExpandedFilenames.ResetAndDestroy();
	}

CCmdRm::CCmdRm()
	{
	}

const TDesC& CCmdRm::Name() const
	{
	_LIT(KCmdRmName, "rm");
	return KCmdRmName;
	}

void CCmdRm::DoRunL()
	{
	iFileMan = CFileMan::NewL(FsL());

	const TInt numFiles = iFileNames.Count();
	for (TInt i = 0; i < numFiles; ++i)
		{
		TFileName2& fileName = iFileNames[i];
		LeaveIfFileNotFound(fileName);
		if (iRecurse)
			{
			if (fileName.IsDirL(FsL()))
				{
				fileName.SetTypeL(TFileName2::EDirectory);
				}
			else
				{
				PrintError(KErrArgument, _L("Invalid use of \"-r\" option - \"%S\" is not a directory."), &fileName);
				User::Leave(KErrArgument);
				}
			}
		else if (fileName.IsDirL(FsL()))
			{
			PrintError(KErrArgument, _L("Couldn't remove \"%S\" because it is a directory - use \"rm -r\" or \"rmdir\" instead."), &fileName);
			User::Leave(KErrArgument);
			}

		TInt err = DoDelete(fileName);
		if (err == KErrAccessDenied && iForce)
			{
			// Try removing read-only attribute from file and try again
			err = Fs().SetAtt(fileName, 0, KEntryAttReadOnly);
			if (!err)
				{
				iForce = EFalse; // Otherwise we'll end up here again (could happen if iFileName is a directory and cannot be deleted because it contains read-only files
				err = DoDelete(fileName);
				}
			}
		LeaveIfErr(err, _L("Couldn't delete '%S'"), &fileName);
		}

	for (TInt i = 0; i < iNonExpandedFilenames.Count(); i++)
		{
		TInt err = DoDelete(*iNonExpandedFilenames[i]);
		LeaveIfErr(err, _L("Couldn't delete '%S'"), iNonExpandedFilenames[i]);
		}
	}

TInt CCmdRm::DoDelete(const TDesC& aFileName)
	{
	TInt err;
	if (iRecurse)
		{
		err = iFileMan->RmDir(aFileName);
		}
	else
		{
		err = iFileMan->Delete(aFileName);
		}
	return err;
	}

void CCmdRm::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdRmOptRecurse, "recurse");
	_LIT(KCmdRmOptForce, "force");
	_LIT(KCmdRmOptNoexpand, "noexpand");
	aOptions.AppendBoolL(iRecurse, KCmdRmOptRecurse);
	aOptions.AppendBoolL(iForce, KCmdRmOptForce);
	aOptions.AppendStringL(iNonExpandedFilenames, KCmdRmOptNoexpand);
	}

void CCmdRm::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdRmArg, "file_name");
	aArguments.AppendFileNameL(iFileNames, KCmdRmArg);
	}


//
// CCmdCp.
//

CCommandBase* CCmdCp::NewLC()
	{
	CCmdCp* self = new(ELeave) CCmdCp();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdCp::~CCmdCp()
	{
	delete iFileMan;
	}

CCmdCp::CCmdCp()
	{
	}

const TDesC& CCmdCp::Name() const
	{
	_LIT(KCmdCpName, "cp");
	return KCmdCpName;
	}

void CCmdCp::ForciblyCopyFileL(const TDesC& aSourceFileName, const TDesC& aDestFileName)
	{
	RFs& fs = FsL();
	TInt err = fs.MkDirAll(aDestFileName);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	TInt numRetries = 3;
retry:
	TEntry startEntry;
	User::LeaveIfError(fs.Entry(aSourceFileName, startEntry));
	RFile destFile;
	if (iOverwrite)
		{
		User::LeaveIfError(destFile.Replace(fs, aDestFileName, EFileShareExclusive | EFileStream | EFileWrite));
		}
	else
		{
		User::LeaveIfError(destFile.Create(fs, aDestFileName, EFileShareExclusive | EFileStream | EFileWrite));
		}
	CleanupClosePushL(destFile);

	HBufC8* blockBuf = HBufC8::NewLC(KBlockSize);
	TPtr8 blockPtr(blockBuf->Des());
	TInt pos = 0;
	while (pos < startEntry.iSize)
		{
		User::LeaveIfError(fs.ReadFileSection(aSourceFileName, pos, blockPtr, KBlockSize));
		if (blockPtr.Length() == 0)
			{
			Printf(_L("\r\nReadFileSection returned zero length descriptor, aborting\r\n"));
			pos = startEntry.iSize;
			}
		else
			{
			pos += blockPtr.Length();
			User::LeaveIfError(destFile.Write(*blockBuf));
			}
		}
	Printf(KNewLine);

	CleanupStack::PopAndDestroy(2, &destFile); // blockBuf, destFile.

	TEntry endEntry;
	User::LeaveIfError(fs.Entry(aSourceFileName, endEntry));
	if (startEntry.iModified != endEntry.iModified)
		{
		--numRetries;
		if (numRetries > 0)
			{
			Printf(_L("Modification while copying - retrying...\r\n"));
			fs.Delete(aDestFileName);	// Ignore error.
			goto retry;
			}
		else
			{
			Printf(_L("Modification while copying - failed 3 times, aborting...\r\n"));
			}
		}
	}

void CCmdCp::DoRunL()
	{
	if (iRecurse && iForce)
		{
		PrintWarning(_L("--recurse and --force options can't be used together"));
		User::Leave(KErrArgument);
		}

	if (iForce)
		{
		ForciblyCopyFileL(iFrom, iTo);
		}
	else
		{
		iFileMan = CFileMan::NewL(FsL());
		TUint flags = 0;
		if (iRecurse) flags |= CFileMan::ERecurse;
		if (iOverwrite) flags |= CFileMan::EOverWrite;

		if (iRecurse && iFrom.Exists(FsL()) && iFrom.IsDirL(FsL()))
			{
			// CFileMan behaves markedly differently to standard posix cp, in the case of copying a directory.
			// POSIX cp does different things depending on whether the destination exists or not:
			// 1) "cp -r dirname existingdir" will produce the tree "./existingdir/dirname/<files>"
			// 2) "cp -r dirname nonexistingdir" will produce the tree "./nonexistingdir/<files>"
			//
			// CFileMan differs in case (1) and puts the contents of dirname straight into the destination dir.
			// Therefore to behave sensibly like posix cp in case (1) we add the source name to the destination
			if (iTo.Exists(FsL()))
				{
				iTo.AppendComponentL(iFrom.Name(), TFileName2::EDirectory);
				}
			}
		User::LeaveIfError(iFileMan->Copy(iFrom, iTo, flags));
		}
	}

void CCmdCp::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdCpOptRecurse, "recurse");
	_LIT(KCmdCpOptOverwrite, "overwrite");
	_LIT(KCmdCpOptForce, "force");
	aOptions.AppendBoolL(iRecurse, KCmdCpOptRecurse);
	aOptions.AppendBoolL(iOverwrite, KCmdCpOptOverwrite);
	aOptions.AppendBoolL(iForce, KCmdCpOptForce);
	}

void CCmdCp::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdCpArg1, "source");
	_LIT(KCmdCpArg2, "destination");
	aArguments.AppendFileNameL(iFrom, KCmdCpArg1);
	aArguments.AppendFileNameL(iTo, KCmdCpArg2);
	}


//
// CCmdMv.
//

CCommandBase* CCmdMv::NewLC()
	{
	CCmdMv* self = new(ELeave) CCmdMv();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdMv::~CCmdMv()
	{
	delete iFileMan;
	}

CCmdMv::CCmdMv()
	{
	}

const TDesC& CCmdMv::Name() const
	{
	_LIT(KCmdMvName, "mv");
	return KCmdMvName;
	}

void CCmdMv::DoRunL()
	{
	if (iTo.Exists(FsL()) && iTo.IsDirL(FsL()))
		{
		iTo.SetTypeL(TFileName2::EDirectory);
		}
	iFileMan = CFileMan::NewL(FsL());
	User::LeaveIfError(iFileMan->Move(iFrom, iTo, 0));
	}

void CCmdMv::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdMvArg1, "source_file");
	_LIT(KCmdMvArg2, "destination_file");
	aArguments.AppendFileNameL(iFrom, KCmdMvArg1);
	aArguments.AppendFileNameL(iTo, KCmdMvArg2);
	}


//
// CCmdMkDir.
//

CCommandBase* CCmdMkDir::NewLC()
	{
	CCmdMkDir* self = new(ELeave) CCmdMkDir();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdMkDir::~CCmdMkDir()
	{
	}

CCmdMkDir::CCmdMkDir()
	: iDir(TFileName2::EDirectory)
	{
	}

const TDesC& CCmdMkDir::Name() const
	{
	_LIT(KCmdMkDirName, "mkdir");
	return KCmdMkDirName;
	}

void CCmdMkDir::DoRunL()
	{
	TInt err = FsL().MkDirAll(iDir);
	if (iAllowExists && (err == KErrAlreadyExists))
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	}

void CCmdMkDir::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdMkDirOptAllowExists, "allow_exists");
	aOptions.AppendBoolL(iAllowExists, KCmdMkDirOptAllowExists);
	}

void CCmdMkDir::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdMkDirArg, "dir_name");
	aArguments.AppendFileNameL(iDir, KCmdMkDirArg);
	}


//
// CCmdRmDir.
//

CCommandBase* CCmdRmDir::NewLC()
	{
	CCmdRmDir* self = new(ELeave) CCmdRmDir();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRmDir::~CCmdRmDir()
	{
	delete iFileMan;
	iDirs.Close();
	}

CCmdRmDir::CCmdRmDir()
	{
	}

const TDesC& CCmdRmDir::Name() const
	{
	_LIT(KCmdRmDirName, "rmdir");
	return KCmdRmDirName;
	}

void CCmdRmDir::DoRunL()
	{
	const TInt numDirs = iDirs.Count();
	for (TInt i = 0; i < numDirs; ++i)
		{
		TFileName2& dir = iDirs[i];
		if (dir.IsDirL(FsL()))
			{
			dir.SetTypeL(TFileName2::EDirectory);
			}
		else
			{
			PrintError(KErrArgument, _L("\"%S\" is not a directory."), &dir);
			User::Leave(KErrArgument);
			}

		LeaveIfFileNotFound(dir);
	
		if (iRecurse)
			{
			if (iFileMan == NULL)
				{
				iFileMan = CFileMan::NewL(FsL());
				}
			User::LeaveIfError(iFileMan->RmDir(dir));
			}
		else
			{
			User::LeaveIfError(FsL().RmDir(dir));
			}
		}
	}

void CCmdRmDir::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdRmDirOptRecurse, "recurse");
	aOptions.AppendBoolL(iRecurse, KCmdRmDirOptRecurse);
	}

void CCmdRmDir::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdRmDirArg, "dir_name");
	aArguments.AppendFileNameL(iDirs, KCmdRmDirArg);
	}


//
// CCmdMatch.
//

CCommandBase* CCmdMatch::NewLC()
	{
	CCmdMatch* self = new(ELeave) CCmdMatch();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdMatch::~CCmdMatch()
	{
	delete iToMatch;
	}

CCmdMatch::CCmdMatch()
	{
	}

const TDesC& CCmdMatch::Name() const
	{
	_LIT(KCmdMatchName, "match");
	return KCmdMatchName;
	}

void CCmdMatch::DoRunL()
	{
	Stdin().SetReadMode(RIoReadHandle::ELine);
	TBuf<0x100> line;
	TInt count = 0;
	while (Stdin().Read(line) == KErrNone)
		{
		TBool matches;
		if (iIgnoreCase)
			{
			matches = (line.MatchF(*iToMatch) != KErrNotFound);
			}
		else
			{
			matches = (line.Match(*iToMatch) != KErrNotFound);
			}
		if (iInvertMatch)
			{
			matches = !matches;
			}
		if (matches)
			{
			if (iCount)
				{
				count++;
				}
			else
				{
				Write(line);
				}
			}
		}

	if (iCount)
		{
		Printf(_L("Count = %d"), count);
		}
	}

void CCmdMatch::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdMatchOptIgnoreCase, "ignore-case");
	_LIT(KCmdMatchOptInvertMatch, "invert-match");
	_LIT(KCmdMatchOptCount, "count");
	aOptions.AppendBoolL(iIgnoreCase, KCmdMatchOptIgnoreCase);
	aOptions.AppendBoolL(iInvertMatch, KCmdMatchOptInvertMatch);
	aOptions.AppendBoolL(iCount, KCmdMatchOptCount);
	}

void CCmdMatch::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdMatchArg1, "pattern");
	aArguments.AppendStringL(iToMatch, KCmdMatchArg1);
	}


//
// CCmdEcho.
//

CCommandBase* CCmdEcho::NewLC()
	{
	CCmdEcho* self = new(ELeave) CCmdEcho();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdEcho::~CCmdEcho()
	{
	delete iToEcho;
	iAttributes.Close();
	}

CCmdEcho::CCmdEcho()
	: iForegroundColor(ConsoleAttributes::EUnchanged), iBackgroundColor(ConsoleAttributes::EUnchanged)
	{
	}

void CCmdEcho::DoWriteL(const TDesC& aDes)
	{
	if (iToStderr)
		{
		Stderr().Write(aDes);
		}
	else
		{
		Stdout().Write(aDes);
		}
	}

const TDesC& CCmdEcho::Name() const
	{
	_LIT(KCmdEchoName, "echo");
	return KCmdEchoName;
	}

void CCmdEcho::DoRunL()
	{
	RIoConsoleWriteHandle& out = iToStderr ? Stderr() : Stdout();

	if (iBinaryMode)
		{
		LeaveIfErr(out.SetMode(RIoReadWriteHandle::EBinary), _L("Unable to set write handle to binary mode"));
		}
	
	CTextFormatter* formatter = CTextFormatter::NewLC(out);

	TUint attributes = 0;
	const TInt numAttributes = iAttributes.Count();
	for (TInt i = 0; i < numAttributes; ++i)
		{
		switch ((TAttr)iAttributes[i])
			{
			case EBold:
				attributes |= ConsoleAttributes::EBold;
				break;
			case EUnderscore:
				attributes |= ConsoleAttributes::EUnderscore;
				break;
			case EBlink:
				attributes |= ConsoleAttributes::EBlink;
				break;
			case EInverse:
				attributes |= ConsoleAttributes::EInverse;
				break;
			case EConceal:
				attributes |= ConsoleAttributes::EConceal;
				break;
			}
		}

	formatter->SetAttributesL(attributes, iForegroundColor, iBackgroundColor);

	if (iToEcho)
		{
		if (iWrap) 
			{
			formatter->WrapL(iIndent, *iToEcho);
			}
		else
			{
			formatter->AppendL(*iToEcho);
			}
		// Add a newline if it doesn't already end in one (and --no-newline isn't specified)
		if (!iNoNewline && iToEcho->Right(KNewLine().Length()) != KNewLine)
			{
			formatter->AppendL(KNewLine);
			}
		}
	else
		{
		TBuf<0x100> buf;
		TInt err = KErrNone;
		do
			{
			err = Stdin().Read(buf);
			if (err == KErrNone)
				{
				if (iWrap) 
					{
					formatter->WrapL(iIndent, buf);
					}
				else
					{
					formatter->AppendL(buf);
					}
				}
			}
			while (err == KErrNone);
		}

	formatter->Write();
	CleanupStack::PopAndDestroy(formatter);
	}

void CCmdEcho::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdEchoOptToStderr, "stderr");
	_LIT(KCmdEchoOptWrap, "wrap");
	_LIT(KCmdEchoOptIndent, "indent");
	_LIT(KCmdEchoOptAttributes, "attributes");
	_LIT(KCmdEchoOptForground, "foreground-color");
	_LIT(KCmdEchoOptBackground, "background-color");
	_LIT(KCmdEchoOptBinaryMode, "binary");
	_LIT(KCmdEchoOptNoNewline, "no-newline");

	aOptions.AppendBoolL(iToStderr, KCmdEchoOptToStderr);
	aOptions.AppendBoolL(iWrap, KCmdEchoOptWrap);
	aOptions.AppendUintL(iIndent, KCmdEchoOptIndent);
	aOptions.AppendEnumL(iAttributes, KCmdEchoOptAttributes);
	aOptions.AppendEnumL((TInt&)iForegroundColor, KCmdEchoOptForground);
	aOptions.AppendEnumL((TInt&)iBackgroundColor, KCmdEchoOptBackground);
	aOptions.AppendBoolL(iBinaryMode, KCmdEchoOptBinaryMode);
	aOptions.AppendBoolL(iNoNewline, KCmdEchoOptNoNewline);
	}

void CCmdEcho::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdEchoArg1, "string");
	aArguments.AppendStringL(iToEcho, KCmdEchoArg1);
	}


//
// CCmdMore.
//

CCommandBase* CCmdMore::NewLC()
	{
	CCmdMore* self = new(ELeave) CCmdMore();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdMore::~CCmdMore()
	{
	}

CCmdMore::CCmdMore()
	{
	}

const TDesC& CCmdMore::Name() const
	{
	_LIT(KCmdMoreName, "more");
	return KCmdMoreName;
	}
	
void CCmdMore::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KMoreArg1, "file_name");
	aArguments.AppendFileNameL(iFile, KMoreArg1);
	}

void CCmdMore::DoRunL()
	{
	RIoReadHandle input;
	input.CreateL(IoSession());
	CleanupClosePushL(input);

	RIoFile file;
	CleanupClosePushL(file);

	if (iArguments.IsPresent(0))
		{
		file.CreateL(IoSession(), iFile, RIoFile::ERead);
		IoSession().SetObjectNameL(file.SubSessionHandle(), iFile);
		_LIT(KReaderSuffix, "_more_reader");
		iFile.SetLength(Min(iFile.Length(), iFile.MaxLength() - KReaderSuffix().Length()));
		iFile.Append(KReaderSuffix);
		IoSession().SetObjectNameL(input.SubSessionHandle(), iFile);
		file.AttachL(input, RIoEndPoint::EForeground);
		}
	else
		{
		input.DuplicateL(Stdin());
		input.SetToForegroundL();
		}

	PageL(input);
	
	CleanupStack::PopAndDestroy(2, &input);
	}


//
// CCmdTrace.
//

CCommandBase* CCmdTrace::NewLC()
	{
	CCmdTrace* self = new(ELeave) CCmdTrace();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTrace::~CCmdTrace()
	{
	iEnable.ResetAndDestroy();
	iDisable.ResetAndDestroy();
	}

CCmdTrace::CCmdTrace()
	{
	}

const TDesC& CCmdTrace::Name() const
	{
	_LIT(KCmdTraceName, "trace");
	return KCmdTraceName;
	}

const LtkUtils::SLitC8 KTraceBitNames[] =
	{
	DESC8("khardware"),
	DESC8("kboot"),
	DESC8("kserver"),
	DESC8("kmmu"),
	DESC8("ksemaphore"),
	DESC8("ksched"),
	DESC8("kproc"),
	DESC8("kexec"),
	DESC8("kdebugger"),
	DESC8("kthread"),
	DESC8("kdll"),
	DESC8("kipc"),
	DESC8("kpbus1"),
	DESC8("kpbus"),
	DESC8("kpbusdrv"),
	DESC8("kpower"),
	DESC8("ktiming"),
	DESC8("kevent"),
	DESC8("kobject"),
	DESC8("kdfc"),
	DESC8("kextension"),
	DESC8("ksched2"),
	DESC8("klocdrv"),
	DESC8("kfail"),
	DESC8("kthread2"),
	DESC8("kdevice"),
	DESC8("kmemtrace"),
	DESC8("kdma"),
	DESC8("kmmu2"),
	DESC8("knkern"),
	DESC8("kscratch"),
	DESC8("kpanic"),
	// Word 1
	DESC8("kusb"),
	DESC8("kusbpsl"),
	DESC8("knetwork1"),
	DESC8("knetwork2"),
	DESC8("ksound1"),
	DESC8("kusbhost"),
	DESC8("kusbotg"),
	DESC8("kusbjournal"),
	DESC8("kusbho"),
	DESC8("kresmanager"),
	DESC8("kiic"),
	DESC8("khcr"),
	DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""),
	DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""),
	DESC8("kpipe"),
	DESC8("kpci"),
	DESC8("kdatapagewarn"),
	DESC8("klocdpaging"),
	DESC8("kpaging"),
	DESC8("krealtime"),
	// Word 2
	DESC8("kallthreadssystem"),
	DESC8("ktestfast"),
	DESC8("ktestlatency"),
	DESC8("kdebugmonitordisable"),
	DESC8("kcrashmonitordisable"),
	DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""),
	DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""),
	DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""), DESC8(""),
	DESC8(""), DESC8(""), DESC8(""),
	// Word 3
	DESC8("kuserheaptrace"),
	};
const TInt KTraceBitCount = sizeof(KTraceBitNames) / sizeof(LtkUtils::SLitC);

#ifndef KCOMPFS // Not defined on 9.1
#define	KCOMPFS		0x0400
#endif

void CCmdTrace::DoRunL()
	{
	TInt debugVal = 0;
	if (iOptF32)
		debugVal|=KFSERV;
	if (iOptLoader)
		debugVal|=KFLDR;
	if (iOptFat)
		debugVal|=KFSYS;
	if (iOptLffs)
		debugVal|=KLFFS;
#ifdef EKA2
	if (iOptIso9660)
		debugVal|=KISO9660;
	if (iOptNtfs)
		debugVal|=KNTFS;
	if (iOptMultiThread)
		debugVal|=KTHRD;
	if (iOptRofs)
		debugVal|=KROFS;
	if (iOptCompfs)
		debugVal|=KCOMPFS;
#endif
	TInt err = FsL().SetDebugRegister(debugVal);
	if (err && debugVal != 0)
		{
		// Don't bother printing a warning if none of the FS-related options were actually specified
		PrintWarning(_L("Unable to configure file server traces: %d"), err);
		}
	
	if (iArguments.IsPresent(0))
		{
#ifdef EKA2
		User::SetDebugMask(iMask, iIndex);
#else
		User::SetDebugMask(iMask);
#endif
		}
	else if (debugVal == 0)
		{
		if (iEnable.Count() || iDisable.Count())
			{
			for (TInt i = 0; i < iEnable.Count(); i++)
				{
				TInt err = SetFlag(*iEnable[i], ETrue);
				if (err) PrintWarning(_L("Flag '%S' not understood"), iEnable[i]);
				}
			for (TInt i = 0; i < iDisable.Count(); i++)
				{
				TInt err = SetFlag(*iDisable[i], EFalse);
				if (err) PrintWarning(_L("Flag '%S' not understood"), iEnable[i]);
				}
			}
		else
			{
			PrintConfig();
			}
		}
	}

void CCmdTrace::PrintConfig()
	{
#ifdef EKA2
	for (TInt i = 0; i < 8; ++i)
		{
		TBool seen = EFalse;
		TUint32 mask = UserSvr::DebugMask(i);
		Printf(_L("DebugMask(%d) = 0x%08X"), i, mask);
		for (TInt bit = 0; bit < 32; bit++)
			{
			if (mask & (1 << bit))
				{
				if (seen)
					{
					Write(_L("|"));
					}
				else
					{
					Write(_L(" ("));
					}
				seen = ETrue;
				TInt idx = i*32 + bit;
				if (idx < KTraceBitCount && KTraceBitNames[idx]().Length())
					{
					Printf(KTraceBitNames[idx]());
					}
				else
					{
					Write(_L("?"));
					}
				}
			}
		if (seen) Write(_L(")"));
		Write(KNewLine);
		}
#else
	Printf(_L("DebugMask = 0x%08X\r\n"), UserSvr::DebugMask());
#endif
	}

TInt CCmdTrace::SetFlag(const TDesC& aFlagName, TBool aSet)
	{
	// Find aFlagName in KTraceBitNames
	TBuf8<64> flagname;
	for (TInt i = 0; i < KTraceBitCount; i++)
		{
		flagname.Copy(aFlagName);
		flagname.LowerCase();
		if (flagname.Compare(KTraceBitNames[i]) == 0)
			{
			TInt word = i / 32;
			TInt idx = i % 32;
			TUint32 mask = UserSvr::DebugMask(word);
			if (aSet)
				{
				mask |= 1 << idx;
				}
			else
				{
				mask &= ~(1 << idx);
				}
			User::SetDebugMask(mask, word);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

void CCmdTrace::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdTraceOptS, "f32");
	_LIT(KCmdTraceOptM, "multi-thread");
	_LIT(KCmdTraceOptL, "loader");
	_LIT(KCmdTraceOptF, "fat");
	_LIT(KCmdTraceOptT, "lffs");
	_LIT(KCmdTraceOptI, "iso9660");
	_LIT(KCmdTraceOptN, "ntfs");
	_LIT(KCmdTraceOptO, "rofs");
	_LIT(KCmdTraceOptC, "compfs");

	aOptions.AppendBoolL(iOptF32, KCmdTraceOptS);
	aOptions.AppendBoolL(iOptMultiThread, KCmdTraceOptM);
	aOptions.AppendBoolL(iOptLoader, KCmdTraceOptL);
	aOptions.AppendBoolL(iOptFat, KCmdTraceOptF);
	aOptions.AppendBoolL(iOptLffs, KCmdTraceOptT);
#ifdef EKA2
	aOptions.AppendBoolL(iOptIso9660, KCmdTraceOptI);
	aOptions.AppendBoolL(iOptNtfs, KCmdTraceOptN);
	aOptions.AppendBoolL(iOptRofs, KCmdTraceOptO);
	aOptions.AppendBoolL(iOptCompfs, KCmdTraceOptC);
#endif

	_LIT(KEnable, "enable");
	aOptions.AppendStringL(iEnable, KEnable);
	_LIT(KDisable, "disable");
	aOptions.AppendStringL(iDisable, KDisable);
	}

void CCmdTrace::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdTraceArg1, "debug_mask");
	aArguments.AppendUintL(iMask, KCmdTraceArg1);
#ifdef EKA2
	_LIT(KCmdTraceArg2, "index");
	aArguments.AppendUintL(iIndex, KCmdTraceArg2);
#endif
	}


//
// CCmdMemInfo.
//

CCommandBase* CCmdMemInfo::NewLC()
	{
	CCmdMemInfo* self = new(ELeave) CCmdMemInfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdMemInfo::~CCmdMemInfo()
	{
	}

CCmdMemInfo::CCmdMemInfo()
	{
	}

const TDesC& CCmdMemInfo::Name() const
	{
	_LIT(KCmdMemInfoName, "meminfo");
	return KCmdMemInfoName;
	}

void CCmdMemInfo::AppendLineL(CTextFormatter& aFormatter, const TDesC& aCaption, HALData::TAttribute aHalAttribute, TBool aNewLine)
	{
	TInt value;
	TInt err = HAL::Get(aHalAttribute, value);
	if (err)
		{
		PrintWarning(_L("Unable to read %S%d"), &aCaption, err);
		}
	else
		{
		if ((aHalAttribute != HALData::EMemoryRAMFree) || ((aHalAttribute == HALData::EMemoryRAMFree) && (value != iLastTotalRamUsage)))
			{
			aFormatter.AppendL(aCaption);
			if (iHumanReadable)
				{
				aFormatter.AppendHumanReadableSizeL(value, EColumnAlignedRight);
				}
			else
				{
				aFormatter.AppendFormatL(_L("%d"), value);
				}
			if (aNewLine)
				{
				aFormatter.AppendL(KNewLine);
				}
			}
		if (aHalAttribute == HALData::EMemoryRAMFree)
			{
			iLastTotalRamUsage = value;
			}
		}
	}

void CCmdMemInfo::DoRunL()
	{
	if (iOnlyFreeRam && iOnlyTotalRam)
		{
		LeaveIfErr(KErrArgument, _L("Mutually exclusive options"));
		}
	CTextFormatter* formatter = CTextFormatter::NewLC(Stdout());
	if (iOnlyFreeRam)
		{
		AppendLineL(*formatter, KNullDesC, HALData::EMemoryRAMFree, EFalse);
		}
	else if (iOnlyTotalRam)
		{
		AppendLineL(*formatter, KNullDesC, HALData::EMemoryRAM, EFalse);
		}
	else
		{
		AppendLineL(*formatter, _L("Total ROM: "), HALData::EMemoryROM, ETrue);
		AppendLineL(*formatter, _L("Total RAM: "), HALData::EMemoryRAM, ETrue);
		AppendLineL(*formatter, _L("Page size: "), HALData::EMemoryPageSize, ETrue);
		AppendLineL(*formatter, _L("Free RAM:  "), HALData::EMemoryRAMFree, EFalse);
		}

	Write(formatter->Descriptor());
	if (iUpdateRate)
		{
		if (Stdout().AttachedToConsole())
			{
			RIoConsoleWriteHandle stdout = Stdout();
			stdout.SetCursorHeight(0);
			}
		FOREVER
			{
			User::After(iUpdateRate * 1000);
			formatter->Zero();
			AppendLineL(*formatter, _L("\rFree RAM:  "), HALData::EMemoryRAMFree, EFalse);
			Write(formatter->Descriptor());
			}
		}
	CleanupStack::PopAndDestroy(formatter);
	}

void CCmdMemInfo::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdMemInfoOptUpdateRate, "rate");
	_LIT(KCmdMemInfoOptHuman, "human");
	_LIT(KCmdMemInfoOptOnlyFreeRam, "free");
	_LIT(KCmdMemInfoOptOnlyTotalRam, "total");

	aOptions.AppendBoolL(iHumanReadable, KCmdMemInfoOptHuman);
	aOptions.AppendUintL(iUpdateRate, KCmdMemInfoOptUpdateRate);
	aOptions.AppendBoolL(iOnlyFreeRam, KCmdMemInfoOptOnlyFreeRam);
	aOptions.AppendBoolL(iOnlyTotalRam, KCmdMemInfoOptOnlyTotalRam);
	}


//
// CCmdDump.
//

void Dump(const TDesC8& aData, CCommandBase& aCommand, TInt& aPos)
	{
	TBuf<80> out;
	TBuf<16> ascii;
	TInt dataIndex = 0;
	do
		{
		out.Zero();
		ascii.Zero();
		out.AppendNumFixedWidthUC(aPos, EHex, 8);
		out.Append(_L(": "));
		for (TInt i = 0; i < 16; ++i)
			{
			if (dataIndex < aData.Length())
				{
				TUint8 byte = aData[dataIndex++];
				out.AppendNumFixedWidthUC(byte, EHex, 2);
				out.Append(_L(" "));
				if ((byte < 0x20) || (byte >= 0x7f) || byte == '%')
					{
					byte = '.';
					}
				ascii.Append(TChar(byte));
				++aPos;
				}
			else
				{
				out.Append(_L("   "));
				aPos = 0;
				}
			}
		out.Append(ascii);
		out.Append(KNewLine);
		aCommand.Write(out);
		}
		while (dataIndex < aData.Length());
	}

CCommandBase* CCmdDump::NewLC()
	{
	CCmdDump* self = new(ELeave) CCmdDump();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdDump::~CCmdDump()
	{
	iFileNames.Close();
	}

CCmdDump::CCmdDump()
	{
	}

const TDesC& CCmdDump::Name() const
	{
	_LIT(KCmdDumpName, "dump");
	return KCmdDumpName;
	}

void CCmdDump::DoRunL()
	{
	if (iFileNames.Count())
		{
		for (TInt i = 0; i < iFileNames.Count(); i++)
			{
			TInt pos = 0;
			const TFileName& fn = iFileNames[i];
			LeaveIfFileNotFound(fn);
			RFile file;
			User::LeaveIfError(file.Open(FsL(), fn, EFileRead));
			TBuf8<KBlockSize> buf;
			TInt err = KErrNone;
			if (iFileNames.Count() > 1) Printf(_L("%S:\r\n"), &fn); // For compatability don't print the file name if there's only one file
			FOREVER
				{
				err = file.Read(buf);
				if ((err == KErrNone) && (buf.Length() > 0))
					{
					Dump(buf, *this, pos);
					buf.Zero();
					}
				else
					{
					break;
					}
				}
			file.Close();
			}
		}
	else
		{
		TInt pos = 0;
		RIoConsoleReadHandle& stdin = Stdin();
		if (iBinaryMode)
			{
			stdin.SetModeL(RIoReadWriteHandle::EBinary);
			}
		stdin.SetReadModeL(RIoReadHandle::EOneOrMore);
		TBuf<KBlockSize> buf;
		TInt err = KErrNone;
		while (err == KErrNone)
			{
			err = stdin.Read(buf);
			if (err == KErrNone)
				{
				Dump(TPtrC8((TUint8*)buf.Ptr(), buf.Length() * 2), *this, pos);
				}
			}
		}
	}

void CCmdDump::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdDumpOptBinaryMode, "binary");
	aOptions.AppendBoolL(iBinaryMode, KCmdDumpOptBinaryMode);
	}

void CCmdDump::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdDumpArg1, "file_name");
	aArguments.AppendFileNameL(iFileNames, KCmdDumpArg1);
	}


//
// CCmdSleep.
//

CCommandBase* CCmdSleep::NewLC()
	{
	CCmdSleep* self = new(ELeave) CCmdSleep();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSleep::~CCmdSleep()
	{
	}

CCmdSleep::CCmdSleep() : CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdSleep::Name() const
	{
	_LIT(KCmdSleepName, "sleep");
	return KCmdSleepName;
	}

void CCmdSleep::DoRunL()
	{
	if (iDuration > (KMaxTUint / 1000000))
		{
		LeaveIfErr(KErrArgument, _L("Duration too large"));
		}

	if (iDuration > 0)
		{
		User::After(iDuration * 1000000);
		Complete();
		}
	}

void CCmdSleep::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdSleepArg1, "duration");
	aArguments.AppendUintL(iDuration, KCmdSleepArg1);
	}


//
// CCmdEnv.
//

CCommandBase* CCmdEnv::NewLC()
	{
	CCmdEnv* self = new(ELeave) CCmdEnv();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdEnv::~CCmdEnv()
	{
	}

CCmdEnv::CCmdEnv()
	{
	}

const TDesC& CCmdEnv::Name() const
	{
	_LIT(KCmdEnvName, "env");
	return KCmdEnvName;
	}

void CCmdEnv::DoRunL()
	{
	const IoUtils::CEnvironment& env = Env();

	RPointerArray<HBufC> keys;
	LtkUtils::CleanupResetAndDestroyPushL(keys);
	env.GetKeysL(keys);

	const TInt numVars = keys.Count();
	for (TInt i = 0; i < numVars; ++i)
		{
		const TDesC& key = *keys[i];
		if (env.IsInt(key))
			{
			Printf(_L("%S=%d\r\n"), &key, env.GetAsIntL(key));
			}
		else if (env.IsDes(key))
			{
			const TDesC& des = env.GetAsDesL(key);
			Printf(_L("%S=%S\r\n"), &key, &des);
			}
		}
	CleanupStack::PopAndDestroy(&keys);
	}


//
// CCmdExport.
//

CCommandBase* CCmdExport::NewLC()
	{
	CCmdExport* self = new(ELeave) CCmdExport();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdExport::~CCmdExport()
	{
	delete iKey;
	delete iVal;
	}

CCmdExport::CCmdExport()
	{
	}

const TDesC& CCmdExport::Name() const
	{
	_LIT(KCmdExportName, "export");
	return KCmdExportName;
	}

void CCmdExport::DoRunL()
	{
	if (iStdin)
		{
		if (iVal)
			{
			Complete(KErrArgument, _L("You can only specify one of --stdin or <value>"));
			return;
			}

		Stdin().SetReadModeL(RIoReadHandle::EFull);
		TInt err = Stdin().Read(iBuf);
		if (err == KErrEof)
			{
			Env().SetL(*iKey, KNullDesC);
			}
		else
			{
			LeaveIfErr(err, _L("Couldn't read from stdin"));
			Env().SetL(*iKey, iBuf);
			}
		}
	else if (iVal)
		{
		Env().SetL(*iKey, *iVal);
		}
	else
		{
		Env().RemoveL(*iKey);
		}
	}

void CCmdExport::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdExportOptStdin, "stdin");
	aOptions.AppendBoolL(iStdin, KCmdExportOptStdin);
	}

void CCmdExport::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdExportArg1, "variable");
	_LIT(KCmdExportArg2, "value");
	aArguments.AppendStringL(iKey, KCmdExportArg1);
	aArguments.AppendStringL(iVal, KCmdExportArg2);
	}


//
// CCmdSort.
//

CCommandBase* CCmdSort::NewLC()
	{
	CCmdSort* self = new(ELeave) CCmdSort();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSort::~CCmdSort()
	{
	}

CCmdSort::CCmdSort()
	{
	}

const TDesC& CCmdSort::Name() const
	{
	_LIT(KCmdSortName, "sort");
	return KCmdSortName;
	}

TInt ComparePtrs(const TPtrC& aPtr1, const TPtrC& aPtr2)
	{
	return aPtr1.Compare(aPtr2);
	}

void CCmdSort::DoRunL()
	{
	// Read all input into a buffer.
	CTextBuffer* buffer = CTextBuffer::NewLC(0x100);
	Stdin().SetReadMode(RIoReadHandle::EOneOrMore);
	TBuf<0x100> buf;
	while (Stdin().Read(buf) == KErrNone)
		{
		buffer->AppendL(buf);
		}

	// Populate an ordered list of lines.
	RArray<TPtrC> sortedLines;
	CleanupClosePushL(sortedLines);
	TPtrC ptr(buffer->Descriptor());
	FOREVER
		{
		TInt pos = ptr.Find(KNewLine);
		if (pos >= 0)
			{
			pos += KNewLine().Length();
			TPtrC line(ptr.Left(pos));
			User::LeaveIfError(sortedLines.InsertInOrderAllowRepeats(line, TLinearOrder<TPtrC>(ComparePtrs)));
			if (pos < (ptr.Length() - 1))
				{
				ptr.Set(ptr.Mid(pos));
				}
			else
				{
				break;
				}
			}
		else
			{
			User::LeaveIfError(sortedLines.InsertInOrderAllowRepeats(ptr, TLinearOrder<TPtrC>(ComparePtrs)));
			break;
			}
		}

	// Output the ordered list of lines.
	const TInt numLines = sortedLines.Count();
	if (iReverse)
		{
		for (TInt i = (numLines - 1); i >= 0; --i)
			{
			Write(sortedLines[i]);
			}
		}
	else
		{
		for (TInt i = 0; i < numLines; ++i)
			{
			Write(sortedLines[i]);
			}
		}

	CleanupStack::PopAndDestroy(2, buffer);
	}

void CCmdSort::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdSortOptReverse, "reverse");
	aOptions.AppendBoolL(iReverse, KCmdSortOptReverse);
	}


//
// CCmdExists.
//

CCommandBase* CCmdExists::NewLC()
	{
	CCmdExists* self = new(ELeave) CCmdExists();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdExists::~CCmdExists()
	{
	}

CCmdExists::CCmdExists()
	{
	}

const TDesC& CCmdExists::Name() const
	{
	_LIT(KCmdExistsName, "exists");
	return KCmdExistsName;
	}

void CCmdExists::DoRunL()
	{
	TUint att;
	Complete(!(FsL().Att(iFileName, att) == KErrNone));
	}

void CCmdExists::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdExistsArg, "file_name");
	aArguments.AppendFileNameL(iFileName, KCmdExistsArg);
	}


//
// CCmdInfoPrint.
//

CCommandBase* CCmdInfoPrint::NewLC()
	{
	CCmdInfoPrint* self = new(ELeave) CCmdInfoPrint();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdInfoPrint::~CCmdInfoPrint()
	{
	delete iToPrint;
	}

CCmdInfoPrint::CCmdInfoPrint()
	{
	}

const TDesC& CCmdInfoPrint::Name() const
	{
	_LIT(KCmdInfoPrintName, "infoprint");
	return KCmdInfoPrintName;
	}

void CCmdInfoPrint::DoRunL()
	{
	if (!iToPrint)
		{
		TBuf<0x100> toPrint;
		Stdin().SetReadModeL(RIoReadHandle::EFull);
		Stdin().Read(toPrint);
		// remove a newline from the end, if one exists
		if ((toPrint.Length() > 0) && (toPrint[toPrint.Length()-1] == '\n'))
			{
			toPrint.SetLength(toPrint.Length()-1);
			if ((toPrint.Length() > 0) && (toPrint[toPrint.Length()-1] == '\r'))
				{
				toPrint.SetLength(toPrint.Length()-1);
				}
			}
		User::InfoPrint(toPrint);
		}
	else
		{
		User::InfoPrint(*iToPrint);
		}
	}

void CCmdInfoPrint::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdInfoPrintArg, "message");
	aArguments.AppendStringL(iToPrint, KCmdInfoPrintArg);
	}


//
// CCmdRDebug.
//

CCommandBase* CCmdRDebug::NewLC()
	{
	CCmdRDebug* self = new(ELeave) CCmdRDebug();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRDebug::~CCmdRDebug()
	{
	delete iToPrint;
	}

CCmdRDebug::CCmdRDebug()
	{
	}

const TDesC& CCmdRDebug::Name() const
	{
	_LIT(KCmdRDebugName, "rdebug");
	return KCmdRDebugName;
	}

void CCmdRDebug::DoRunL()
	{
	if (iToPrint)
		{
		RDebug::Print(*iToPrint);
		}
	else
		{
		RIoConsoleReadHandle& stdin = Stdin();
		stdin.SetReadModeL(RIoReadHandle::ELine);
		TBuf<512> buf;
		TInt err = KErrNone;
		while (err == KErrNone)
			{
			err = stdin.Read(buf);
			if (err == KErrNone)
				{
				RDebug::Print(buf);
				}
			}
		}
	}

void CCmdRDebug::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdRDebugArg, "message");
	aArguments.AppendStringL(iToPrint, KCmdRDebugArg);
	}


//
// CCmdDate.
//

// Appearently kernel reckons to 365.2485 days per year between 0AD (nominal gregorian) and 2000AD, which comes to 730497 days in total.
// timeanddate.com agrees once you factor in Julian->Gregorian and assuming 0AD wasn't a leap year (it refuses to allow you to directly enter 0AD nominal Gregorian)
const TInt64 KY2kInMicroSeconds = 730497 * 24 * 60 * (TInt64)(60 * 1000 * 1000);

CCommandBase* CCmdDate::NewLC()
	{
	CCmdDate* self = new(ELeave) CCmdDate();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdDate::~CCmdDate()
	{
	delete iDateToSet;
	}

CCmdDate::CCmdDate()
	{
	}

void CCmdDate::Display(const TTime& aTime)
	{
	if (iRaw)
		{
		TInt64 time = aTime.Int64();
		if (iUseKernelFormat) time += KY2kInMicroSeconds;
		Printf(_L("%Ld\r\n"), time);
		}
	else if (iUseTimestampFormat)
		{
		TDateTime dateTime(aTime.DateTime());
		Printf(_L("%04d%02d%02d-%02d%02d.%02d"), dateTime.Year(), dateTime.Month() + 1, dateTime.Day() + 1, dateTime.Hour(), dateTime.Minute(), dateTime.Second());
		}
	else
		{
		TDateTime dateTime(aTime.DateTime());
		Printf(_L("%+02d/%+02d/%+04d %+02d:%+02d:%+02d.%+06d\r\n"), dateTime.Day() + 1, dateTime.Month() + 1, dateTime.Year(), dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond());
		}
	}

const TDesC& CCmdDate::Name() const
	{
	_LIT(KCmdDateName, "date");
	return KCmdDateName;
	}

void CCmdDate::DoRunL()
	{
	if (!iDateToSet && !iOptions.IsPresent(&iRawTimeToSet))
		{
		TTime time;
		if (iUniversalTime)
			{
#ifdef FSHELL_CORE_SUPPORT_SECURE_TIME
			if (iSecure)
				{
				LeaveIfErr(time.UniversalTimeSecure(), _L("Unable to get secure universal time"));
				}
			else
#endif
				{
				time.UniversalTime();
				}
			}
		else
			{
#ifdef FSHELL_CORE_SUPPORT_SECURE_TIME
			if (iSecure)
				{
				LeaveIfErr(time.HomeTimeSecure(), _L("Unable to get secure home time"));
				}
			else
#endif
				{
				time.HomeTime();
				}
			}
		Display(time);
		}
	else
		{
		_LIT(KSettingError, "Cannot set the time");
		if (iUseKernelFormat) iRawTimeToSet -= KY2kInMicroSeconds;
		TTime time(iRawTimeToSet);
		if (iDateToSet)
			{
			_LIT(KParsingTime, "Cannot parse time string");
			LeaveIfErr(time.Parse(*iDateToSet), KParsingTime);
			}
		if (iJustDisplay)
			{
			Display(time);
			}
		else
			{
			if (iUniversalTime)
				{
#ifdef FSHELL_CORE_SUPPORT_SECURE_TIME
				if (iSecure)
					{
					LeaveIfErr(User::SetUTCTimeSecure(time), KSettingError);
					}
				else
#endif
					{
					LeaveIfErr(User::SetUTCTime(time), KSettingError);
					}
				}
			else
				{
#ifdef FSHELL_CORE_SUPPORT_SECURE_TIME
				if (iSecure)
					{
					LeaveIfErr(User::SetHomeTimeSecure(time), KSettingError);
					}
				else
#endif
					{
					LeaveIfErr(User::SetHomeTime(time), KSettingError);
					}
				}
			if (iOptions.IsPresent(&iUtcOffset))
				{
				User::SetUTCOffset(TTimeIntervalSeconds(iUtcOffset));
				}
			}
		}
	}

void CCmdDate::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdDateOptUniversal, "universal");
	_LIT(KCmdDateOptSet, "set");
	_LIT(KCmdDateOptUtcOffset, "utc-offset");
#ifdef FSHELL_CORE_SUPPORT_SECURE_TIME
	_LIT(KCmdDateOptSecure, "secure");
#endif
	_LIT(KCmdDateOptRaw, "raw");
	_LIT(KCmdDateOptJustDisplay, "just-display");
	_LIT(KCmdDateOptSetRaw, "raw-set");
	_LIT(KCmdDateOptTimestamp, "timestamp");
	_LIT(KCmdDateOptKern, "kern");

	aOptions.AppendBoolL(iUniversalTime, KCmdDateOptUniversal);
	aOptions.AppendStringL(iDateToSet, KCmdDateOptSet);
	aOptions.AppendIntL(iUtcOffset, KCmdDateOptUtcOffset);
#ifdef FSHELL_CORE_SUPPORT_SECURE_TIME
	aOptions.AppendBoolL(iSecure, KCmdDateOptSecure);
#endif
	aOptions.AppendBoolL(iRaw, KCmdDateOptRaw);
	aOptions.AppendBoolL(iJustDisplay, KCmdDateOptJustDisplay);
	aOptions.AppendIntL(iRawTimeToSet, KCmdDateOptSetRaw);
	aOptions.AppendBoolL(iUseTimestampFormat, KCmdDateOptTimestamp);
	aOptions.AppendBoolL(iUseKernelFormat, KCmdDateOptKern);
	}


//
// CCmdFsck.
//

CCommandBase* CCmdFsck::NewLC()
	{
	CCmdFsck* self = new(ELeave) CCmdFsck();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdFsck::~CCmdFsck()
	{
	delete iDriveLetter;
	}

CCmdFsck::CCmdFsck()
	{
	}

const TDesC& CCmdFsck::Name() const
	{
	_LIT(KCmdFsckName, "fsck");
	return KCmdFsckName;
	}

void CCmdFsck::DoRunL()
	{
	TInt ret = FsL().CheckDisk(*iDriveLetter);

	switch (ret)
		{
		case KErrNotSupported:
			Printf(_L("Not supported\r\n"));
			break;
		case 0:
			Printf(_L("No errors\r\n"));
			break;
		case 1:
			Printf(_L("Error: File cluster chain contains a bad value (<2 or >maxCluster)\r\n"));
			break;
		case 2:
			Printf(_L("Error: Two files are linked to the same cluster\r\n"));
			break;
		case 3:
			Printf(_L("Error: Unallocated cluster contains a value != 0\r\n"));
			break;
		case 4:
			Printf(_L("Error: Size of file != number of clusters in chain\r\n"));
			break;
		default:
			Printf(_L("Error: %d\r\n"), ret);
		}

	User::LeaveIfError(ret);
	}

void CCmdFsck::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdFsckArg, "drive_letter");
	aArguments.AppendStringL(iDriveLetter, KCmdFsckArg);
	}


//
// CCmdDriver.
//

CCommandBase* CCmdDriver::NewLC()
	{
	CCmdDriver* self = new(ELeave) CCmdDriver();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdDriver::~CCmdDriver()
	{
	delete iDriverName;
	}

CCmdDriver::CCmdDriver()
	{
	}

const TDesC& CCmdDriver::Name() const
	{
	_LIT(KCmdDriverName, "driver");
	return KCmdDriverName;
	}

void CCmdDriver::DoRunL()
	{
	TInt err = KErrNone;
	switch (iOperation)
		{
		case ELoad:
			{
			if (iType == ELogical)
				{
				err = User::LoadLogicalDevice(*iDriverName);
				}
			else
				{
				err = User::LoadPhysicalDevice(*iDriverName);
				}
			break;
			}
		case EFree:
			{
			if (iType == ELogical)
				{
				err = User::FreeLogicalDevice(*iDriverName);
				}
			else
				{
				err = User::FreePhysicalDevice(*iDriverName);
				}
			break;
			}
		case EFind:
			{
			if (iType == ELogical)
				{
				TFindLogicalDevice findLogicalDevice;
				TFullName name;
				while (findLogicalDevice.Next(name) == KErrNone)
					{
					Printf(_L("%S\r\n"), &name);
					}
				}
			else
				{
				TFindPhysicalDevice findPhysicalDevice;
				TFullName name;
				while (findPhysicalDevice.Next(name) == KErrNone)
					{
					Printf(_L("%S\r\n"), &name);
					}
				}
			break;
			}
		default:
			{
			ASSERT(EFalse);
			break;
			}
		}

	if (err)
		{
		TPtrC operation(iArguments.AsString(&iOperation));
		TPtrC type(iArguments.AsString(&iType));
		PrintError(err, _L("Unable to %S %S device"), &operation, &type);
		User::Leave(err);
		}
	}

void CCmdDriver::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdDriverArg1, "operation");
	_LIT(KCmdDriverArg2, "type");
	_LIT(KCmdDriverArg3, "name");

	aArguments.AppendEnumL((TInt&)iOperation, KCmdDriverArg1);
	aArguments.AppendEnumL((TInt&)iType, KCmdDriverArg2);
	aArguments.AppendStringL(iDriverName, KCmdDriverArg3);
	}


//
// CCmdChunkInfo.
//

CCommandBase* CCmdChunkInfo::NewLC()
	{
	CCmdChunkInfo* self = new(ELeave) CCmdChunkInfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdChunkInfo::~CCmdChunkInfo()
	{
	delete iOwningProcess;
	delete iBuf;
	delete iFormatter;
	}

CCmdChunkInfo::CCmdChunkInfo()
	{
	}

void CCmdChunkInfo::ListChunksL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TInt bufSize = 1024;
	TInt err = KErrNone;
	HBufC8* addressesBuf;
	do
		{
		addressesBuf = HBufC8::NewLC(bufSize);
		TPtr8 addressesPtr(addressesBuf->Des());
		if (iControllingProcess != 0)
			{
			err = iMemAccess.GetChunkAddresses(iControllingProcess, addressesPtr);
			}
		else
			{
			err = iMemAccess.GetObjectAddresses(EChunk, iOwningProcess ? *iOwningProcess : KNullDesC(), addressesPtr);
			}
		if (err == KErrOverflow)
			{
			CleanupStack::PopAndDestroy(addressesBuf);
			bufSize *= 2;
			}
		}
		while (err == KErrOverflow);

	if (err)
		{
		PrintError(err, _L("Unable to read chunk addresses."));
		User::Leave(err);
		}

	const TInt numAddresses = addressesBuf->Length() / sizeof(TUint32*);
	if (numAddresses == 0)
		{
		Write(_L("No chunks found.\r\n"));
		}
	else
		{
		TUint32* p = (TUint32*)addressesBuf->Ptr();
		TInt offset = 0;
		while (offset < numAddresses)
			{
			TChunkKernelInfo chunkInfo;
			TPckg<TChunkKernelInfo> chunkInfoPckg(chunkInfo);
			TUint32* ptr = p + offset++;
			err = iMemAccess.GetObjectInfo(EChunk, (TUint8*)(*ptr), chunkInfoPckg);
			if (err == KErrNone)
				{
				iFullName.Copy(chunkInfo.iFullName);
				if (iIncludeSize)
					{
					iBuf->AppendFormatL(_L("0x%08x\t"), *ptr);
					if (iHumanReadable)
						{
						iBuf->AppendHumanReadableSizeL(chunkInfo.iSize, EUnaligned);
						iBuf->AppendFormatL(KTab);
						}
					else
						{
						iBuf->AppendFormatL(_L("%d\t"), chunkInfo.iSize);
						}
					iBuf->AppendFormatL(_L("\'%S\'\r\n"), &iFullName);
					}
				else
					{
					iBuf->AppendFormatL(_L("0x%08x\t\'%S\'\r\n"), *ptr, &iFullName);
					}
				}
			else if (err != KErrNotFound)
				{
				// Only abort on something other than KErrNotFound. KErrNotFound could legitimately
				// happen if the chunk in question has been deleted since we called RMemoryAccess::GetObjectAddresses.
				PrintError(err, _L("Unable to read chunk at 0x%08x, aborting."), ptr);
				User::Leave(err);
				}
			}
		}

	CleanupStack::PopAndDestroy(addressesBuf);
#endif
	}

void CCmdChunkInfo::PrintChunkInfoL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TChunkKernelInfo chunkInfo;
	TPckg<TChunkKernelInfo> chunkInfoPckg(chunkInfo);
	TInt err = iMemAccess.GetObjectInfo(EChunk, (TUint8*)iAddress, chunkInfoPckg);
	if (err)
		{
		PrintError(err, _L("Unable to get info for chunk 0x%08x"), iAddress);
		User::Leave(err);
		}
	else
		{
		iFullName.Copy(chunkInfo.iFullName);
		iBuf->AppendFormatL(_L("Name:\t\'%S\'\r\n"), &iFullName);
		iBuf->AppendFormatL(_L("Base:\t0x%08x\r\n"), chunkInfo.iBase);
		PrintSizeL(_L("Max size:\t"), chunkInfo.iMaxSize);
		PrintSizeL(_L("Current size:\t"), chunkInfo.iSize);
		iBuf->AppendFormatL(_L("Attributes:\t0x%08x\r\n"), chunkInfo.iAttributes);
		iBuf->AppendFormatL(_L("Type:\t0x%08x\r\n"), chunkInfo.iChunkType);
		iBuf->AppendFormatL(_L("Restrictions:\t0x%08x\r\n"), chunkInfo.iRestrictions);

		RProcess process;
		TInt err = process.Open(chunkInfo.iControllingOwnerProcessId);
		if (err == KErrNone)
			{
			iName = process.Name();
			process.Close();
			iBuf->AppendFormatL(_L("Controlling process:\t\'%S\' (%u)\r\n"), &iName, chunkInfo.iControllingOwnerProcessId);
			LtkUtils::RProxyAllocatorHelper allocHelper;
			CleanupClosePushL(allocHelper);
			err = allocHelper.OpenChunkHeap(iMemAccess, (TAny*)iAddress);
			if (err == KErrNone)
				{
				iBuf->AppendFormatL(_L("Allocator type:\t%S\r\n"), &allocHelper.Description());
				PrintSizeL(_L("Heap size:\t"), allocHelper.CommittedSize());
				PrintSizeL(_L("Heap max size:\t"), allocHelper.MaxCommittedSize());
				iBuf->AppendFormatL(_L("Alloc count:\t%d\r\n"), allocHelper.AllocationCount());
				PrintSizeL(_L("Total alloc size:\t"), allocHelper.AllocatedSize());
				TReal overheadRatio = ((allocHelper.CommittedSize() - allocHelper.AllocatedSize() - allocHelper.CommittedFreeSpace()) * 100) / (TReal)allocHelper.CommittedSize();
				iBuf->AppendFormatL(_L("Metadata overhead:\t%00.2f%%\r\n"), overheadRatio);
				TInt val = allocHelper.SizeForCellType(RAllocatorHelper::EDlaAllocation);
				if (val >= 0)
					{
					PrintSizeL(_L("DL alloc size:\t"), val);
					iBuf->AppendFormatL(_L("DL alloc count:\t%d\r\n"), allocHelper.CountForCellType(RAllocatorHelper::EDlaAllocation));
					PrintSizeL(_L("DL free size:\t"), allocHelper.SizeForCellType(RAllocatorHelper::EDlaFreeCell));
					iBuf->AppendFormatL(_L("DL free cell count:\t%d\r\n"), allocHelper.CountForCellType(RAllocatorHelper::EDlaFreeCell));
					PrintSizeL(_L("Paged alloc size:\t"), allocHelper.SizeForCellType(RAllocatorHelper::EPageAllocation));
					iBuf->AppendFormatL(_L("Paged alloc count:\t%d\r\n"), allocHelper.CountForCellType(RAllocatorHelper::EPageAllocation));
					PrintSizeL(_L("Slab alloc size:\t"), allocHelper.SizeForCellType(RAllocatorHelper::ESlabAllocation));
					iBuf->AppendFormatL(_L("Slab alloc count:\t%d\r\n"), allocHelper.CountForCellType(RAllocatorHelper::ESlabAllocation));
					PrintSizeL(_L("Slab free size:\t"), allocHelper.SizeForCellType(RAllocatorHelper::ESlabFreeCell));
					iBuf->AppendFormatL(_L("Slab free cell count:\t%d\r\n"), allocHelper.CountForCellType(RAllocatorHelper::ESlabFreeCell));
					PrintSizeL(_L("Unused slabs size:\t"), allocHelper.SizeForCellType(RAllocatorHelper::ESlabFreeSlab));
					iBuf->AppendFormatL(_L("Unused slabs count:\t%d\r\n"), allocHelper.CountForCellType(RAllocatorHelper::ESlabFreeSlab));
					}
				else if (val != KErrNotSupported)
					{
					iBuf->AppendFormatL(_L("\t[Error %d returned from heap stats]\r\n"), val);
					}
				}
			else
				{
				iBuf->AppendL(_L("\t[RHeap allocator not found]\r\n"));
				}
			CleanupStack::PopAndDestroy(&allocHelper);
			}
		if (err)
			{
			PrintWarning(_L("Unable to read RHeap info: %d"), err);
			}
		}
#endif
	}

void CCmdChunkInfo::PrintSizeL(const TDesC& aCaption, TInt aSize)
	{
	if (iHumanReadable)
		{
		iBuf->AppendL(aCaption);
		iBuf->AppendHumanReadableSizeL(aSize, EUnaligned);
		iBuf->AppendL(KNewLine);
		}
	else
		{
		_LIT(KFormat, "%S%d\r\n");
		iBuf->AppendFormatL(KFormat, &aCaption, aSize);
		}
	}

const TDesC& CCmdChunkInfo::Name() const
	{
	_LIT(KCmdChunkInfoName, "chunkinfo");
	return KCmdChunkInfoName;
	}

void CCmdChunkInfo::DoPrintL()
	{
	iBuf->Zero();
	iFormatter->Zero();

	if (iAddress == 0)
		{
		ListChunksL();
		}
	else
		{
		PrintChunkInfoL();
		}

	iFormatter->TabulateL(0, 2, iBuf->Descriptor(), ETruncateLongestColumn);
	Write(iFormatter->Descriptor());
	}

void CCmdChunkInfo::DoRunL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	iFormatter = CTextFormatter::NewL(Stdout());
	iBuf = IoUtils::CTextBuffer::NewL(0x100);
	LoadMemoryAccessL();

	RIoConsoleWriteHandle stdout = Stdout();
	TBool attachedToConsole(stdout.AttachedToConsole());
	if (attachedToConsole && iUpdateRate && iAddress)
		{
		stdout.SetCursorHeight(0);
		stdout.ClearScreen();
		}

	DoPrintL();

	if (iUpdateRate && iAddress)
		{
		FOREVER
			{
			User::After(iUpdateRate * 1000);
			if (attachedToConsole)
				{
				stdout.ClearScreen();
				}
			DoPrintL();
			}
		}
#else
	PrintError(KErrNotSupported, _L("Unable to fetch chunk information because fshell was not built with FSHELL_MEMORY_ACCESS_SUPPORT defined. That probably means this plaform doesn't support the MemoryAccess device driver."));
	User::Leave(KErrNotSupported);
#endif
	}

void CCmdChunkInfo::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdChunkInfoOptRate, "rate");
	_LIT(KCmdChunkInfoOptHuman, "human");
	_LIT(KCmdChunkInfoOptOwningProcess, "owning-process");
	_LIT(KCmdChunkInfoOptControllingProcess, "controlling-process");
	_LIT(KCmdChunkInfoOptSize, "size");

	aOptions.AppendUintL(iUpdateRate, KCmdChunkInfoOptRate);
	aOptions.AppendBoolL(iHumanReadable, KCmdChunkInfoOptHuman);
	aOptions.AppendStringL(iOwningProcess, KCmdChunkInfoOptOwningProcess);
	aOptions.AppendUintL(iControllingProcess, KCmdChunkInfoOptControllingProcess);
	aOptions.AppendBoolL(iIncludeSize, KCmdChunkInfoOptSize);

	}

void CCmdChunkInfo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdChunkInfoArg, "chunk_address");
	aArguments.AppendUintL(iAddress, KCmdChunkInfoArg);
	}


//
// CCmdSvrInfo.
//

CCommandBase* CCmdSvrInfo::NewLC()
	{
	CCmdSvrInfo* self = new(ELeave) CCmdSvrInfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSvrInfo::~CCmdSvrInfo()
	{
	delete iBuf;
	delete iFormatter;
	}

CCmdSvrInfo::CCmdSvrInfo()
	{
	}

void CCmdSvrInfo::ListServersL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TInt bufSize = 1024;
	TInt err = KErrNone;
	HBufC8* addressesBuf;
	do
		{
		addressesBuf = HBufC8::NewLC(bufSize);
		TPtr8 addressesPtr(addressesBuf->Des());
		err = iMemAccess.GetObjectAddresses(EServer, KNullDesC(), addressesPtr);
		if (err == KErrOverflow)
			{
			CleanupStack::PopAndDestroy(addressesBuf);
			bufSize *= 2;
			}
		}
		while (err == KErrOverflow);

	if (err)
		{
		PrintError(err, _L("Unable to read server addresses."));
		User::Leave(err);
		}

	const TInt numAddresses = addressesBuf->Length() / sizeof(TUint32*);
	if (numAddresses == 0)
		{
		Write(_L("No servers found.\r\n"));
		}
	else
		{
		TUint32* p = (TUint32*)addressesBuf->Ptr();
		TInt offset = 0;
		while (offset < numAddresses)
			{
			TPckg<TServerKernelInfo> serverInfoPckg(iServerInfo);
			TUint32* ptr = p + offset++;
			err = iMemAccess.GetObjectInfo(EServer, (TUint8*)(*ptr), serverInfoPckg);
			if (err == KErrNone)
				{
				iFullName.Copy(iServerInfo.iFullName);
				iBuf->AppendFormatL(_L("0x%08x\t\'%S\'\r\n"), *ptr, &iFullName);
				}
			else if (err != KErrNotFound)
				{
				// Only warn on something other than KErrNotFound. KErrNotFound could legitimately
				// happen if the chunk in question has been deleted since we called RMemoryAccess::GetObjectAddresses.
				PrintWarning(_L("Unable to read server at 0x%08x (%d)"), ptr, err);
				}
			}
		}

	CleanupStack::PopAndDestroy(addressesBuf);

#endif
	}

void CCmdSvrInfo::ListSessionsL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TPckg<TServerKernelInfo> serverInfoPckg(iServerInfo);
	TInt err = iMemAccess.GetObjectInfo(EServer, (TUint8*)iAddress, serverInfoPckg);
	if (err)
		{
		PrintError(err, _L("Unable to get info for server 0x%08x"), iAddress);
		User::Leave(err);
		}
	
	TPckg<TThreadKernelInfo> threadInfoPckg(iThreadInfo);
	err = iMemAccess.GetObjectInfo(EThread, iServerInfo.iAddressOfOwningThread, threadInfoPckg);
	if (err)
		{
		PrintError(err, _L("Unable to get info for thread 0x%08x"), iServerInfo.iAddressOfOwningThread);
		User::Leave(err);
		}

	iFullName.Copy(iThreadInfo.iFullName);
	iBuf->AppendFormatL(_L("Owning thread id:\t%u\r\n"), iThreadInfo.iThreadId);
	iBuf->AppendFormatL(_L("Owning thread name:\t\'%S\'\r\n"), &iFullName);
	TUint type = iServerInfo.iSessionType;
	iBuf->AppendFormatL(_L("Session type:\t%u\r\n"), type);

	for (TUint i = iServerInfo.iSessionQ.iStartFrom; i < iServerInfo.iSessionQ.iCount; ++i)
		{
		TPckg<TSessionKernelInfo> sessionInfoPckg(iSessionInfo);
		err = iMemAccess.GetObjectInfo(ESession, iServerInfo.iSessionQ.iBuffer[i], sessionInfoPckg);
		if (err)
			{
			PrintWarning(_L("Unable to get info for session 0x%08x (%d)"), iServerInfo.iSessionQ.iBuffer[i], err);
			}
		else
			{
			iFullName.Copy(iSessionInfo.iFullName);
			iBuf->AppendFormatL(_L("Session:\t\'%S\'\r\n"), &iFullName);

			TPckg<TProcessKernelInfo> processInfoPckg(iProcessInfo);
			err = iMemAccess.GetObjectInfo(EProcess, iSessionInfo.iAddressOfKernelOwner, processInfoPckg);
			if (err == KErrNone)
				{
				iFullName.Copy(iProcessInfo.iFullName);
				iBuf->AppendFormatL(_L("Session owner:\t\'%S\'\r\n"), &iFullName);
				}
			else
				{
				err = iMemAccess.GetObjectInfo(EThread, iSessionInfo.iAddressOfKernelOwner, threadInfoPckg);
				if (err == KErrNone)
					{
					iFullName.Copy(iThreadInfo.iFullName);
					iBuf->AppendFormatL(_L("Session owner:\t\'%S\'\r\n"), &iFullName);
					}
				else
					{
					PrintWarning(_L("Unable to get info for session owner 0x%08x (%d)"), iSessionInfo.iAddressOfKernelOwner, err);
					}
				}
			}
		}
#endif
	}

const TDesC& CCmdSvrInfo::Name() const
	{
	_LIT(KCmdSvrInfoName, "svrinfo");
	return KCmdSvrInfoName;
	}

void CCmdSvrInfo::DoRunL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	iFormatter = CTextFormatter::NewL(Stdout());
	iBuf = IoUtils::CTextBuffer::NewL(0x100);
	LoadMemoryAccessL();

	if (iAddress == 0)
		{
		ListServersL();
		}
	else
		{
		ListSessionsL();
		}

	iFormatter->TabulateL(0, 2, iBuf->Descriptor(), ETruncateLongestColumn);
	Write(iFormatter->Descriptor());
#else
	PrintError(KErrNotSupported, _L("Unable to fetch server information because fshell was not built with FSHELL_MEMORY_ACCESS_SUPPORT defined. That probably means this plaform doesn't support the MemoryAccess device driver."));
	User::Leave(KErrNotSupported);
#endif
	}

void CCmdSvrInfo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdSvrInfoArg, "server_address");
	aArguments.AppendUintL(iAddress, KCmdSvrInfoArg);
	}


//
// CCmdTickle.
//
	
CCommandBase* CCmdTickle::NewLC()
	{
	CCmdTickle* self = new (ELeave) CCmdTickle();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTickle::~CCmdTickle()
	{
	}

CCmdTickle::CCmdTickle()
	{
	}

void CCmdTickle::DoRunL()
	{
	User::ResetInactivityTime();
	const TInt KGiveSystemTimeToFocus = 500000; // microseconds
	User::After(KGiveSystemTimeToFocus);
	}
	
const TDesC& CCmdTickle::Name() const
	{
	_LIT(KName, "tickle");
	return KName;
	}


//
// CCmdTicks.
//
	
CCommandBase* CCmdTicks::NewLC()
	{
	CCmdTicks* self = new (ELeave) CCmdTicks();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTicks::~CCmdTicks()
	{
	}

CCmdTicks::CCmdTicks()
	{
	}

TInt NanoTickPeriod()
	{
	TInt nanoTickPeriod;
	if (HAL::Get(HAL::ENanoTickPeriod, nanoTickPeriod) != KErrNone)
		{
		nanoTickPeriod = 1000;
		}
	return nanoTickPeriod;
	}

TInt FastCounterFrequency()
	{
	TInt fastCounterFrequency;
	if (HAL::Get(HAL::EFastCounterFrequency, fastCounterFrequency) != KErrNone)
		{
		fastCounterFrequency = 1000;
		}
	return fastCounterFrequency;
	}

TBool FastCounterCountsUp()
	{
	TBool countsUp;
	if (HAL::Get(HAL::EFastCounterCountsUp, countsUp) != KErrNone)
		{
		countsUp = EFalse;
		}
	return countsUp;
	}

void CCmdTicks::DoRunL()
	{
	TUint nticks = User::NTickCount();
	TUint fast = User::FastCounter();

	Printf(_L("NKern ticks: %u\r\n"), nticks);
	Printf(_L("Fast ticks: %u\r\n"), fast);

	if (iVerbose)
		{
		Printf(_L("\r\nNKern tick period: %d\r\n"), NanoTickPeriod());
		Printf(_L("Fast counter frequency: %d\r\n"), FastCounterFrequency());
		Printf(_L("Fast counter counts up: %d\r\n"), FastCounterCountsUp());
		}
	}
	
const TDesC& CCmdTicks::Name() const
	{
	_LIT(KName, "ticks");
	return KName;
	}

void CCmdTicks::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	}


//
// CCmdUpTime.
//
	
CCommandBase* CCmdUpTime::NewLC()
	{
	CCmdUpTime* self = new (ELeave) CCmdUpTime();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdUpTime::~CCmdUpTime()
	{
	}

CCmdUpTime::CCmdUpTime()
	{
	}

void AppendTimeUnit(TInt aValue, const TDesC& aUnit, TDes& aBuf)
	{
	if (aValue == 1)
		{
		aBuf.AppendFormat(_L("1 %S "), &aUnit);
		}
	else
		{
		aBuf.AppendFormat(_L("%d %Ss "), aValue, &aUnit);
		}
	}

void FormatTime(const TUint64& aInterval, TDes& aBuf)
	{
	const TInt ms = 1000;
	const TInt s = ms * ms;
	const TInt64 m = s * 60;
	const TInt64 h = m * 60;
	const TInt64 d = h * 24;

	TInt64 interval = aInterval;
	const TInt numDays = interval / d;
	if (numDays > 0)
		{
		AppendTimeUnit(numDays, _L("day"), aBuf);
		interval = interval % d;
		}

	const TInt numHours = interval / h;
	if (numHours > 0)
		{
		AppendTimeUnit(numHours, _L("hour"), aBuf);
		interval = interval % h;
		}

	const TInt numMinutes = interval / m;
	if (numMinutes > 0)
		{
		AppendTimeUnit(numMinutes, _L("minute"), aBuf);
		interval = interval % m;
		}

	TBuf<16> format(_L("%.2f "));
	TReal realInterval = interval;
	if (realInterval >= s)
		{
		realInterval /= s;
		format.Append(_L("s"));
		}
	else if (realInterval >= ms)
		{
		realInterval /= ms;
		format.Append(_L("ms"));
		}
	else
		{
		format.Append(_L("us"));
		}
	aBuf.AppendFormat(format, realInterval); 
	}

void CCmdUpTime::DoRunL()
	{
	TUint64 n = User::NTickCount();
	n *= NanoTickPeriod();

	if (iHuman)
		{
		TBuf<256> timeBuf;
		FormatTime(n, timeBuf);
		Printf(_L("%S"), &timeBuf);
		}
	else
		{
		Printf(_L("%Lu"), n);
		}
	}
	
const TDesC& CCmdUpTime::Name() const
	{
	_LIT(KName, "uptime");
	return KName;
	}

void CCmdUpTime::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iHuman, KOptHuman);
	}


//
// CCmdVar.
//
	
CCommandBase* CCmdVar::NewLC()
	{
	CCmdVar* self = new (ELeave) CCmdVar();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdVar::~CCmdVar()
	{
	delete iVar1;
	delete iArg;
	}

CCmdVar::CCmdVar()
	{
	}

void CCmdVar::DoRunL()
	{
	IoUtils::CEnvironment& env = Env();
	switch (iOperation)
		{
		case EDefined:
			if (iArg)
				{
				Complete(KErrArgument, _L("Argument should not be specified for \"defined\" operation"));
				}
			else
				{
				SetErrorReported(ETrue); // To silence leaves.
				Complete(!env.IsDefined(*iVar1));
				}
			break;
		case ENotDefined:
			if (iArg)
				{
				Complete(KErrArgument, _L("Argument should not be specified for \"not-defined\" operation"));
				}
			else
				{
				SetErrorReported(ETrue); // To silence leaves.
				Complete(env.IsDefined(*iVar1));
				}
			break;
		case EEqual:
			if (iArg == NULL)
				{
				Complete(KErrArgument, _L("Argument must be specified for == operation"));
				}
			else
				{
				SetErrorReported(ETrue); // To silence leaves.
				Complete(!(env.GetAsDesL(*iVar1) == *iArg));
				}
			break;
		case ENotEqual:
			if (iArg == NULL)
				{
				Complete(KErrArgument, _L("Argument must be specified for != operation"));
				}
			else
				{
				SetErrorReported(ETrue); // To silence leaves.
				Complete(env.GetAsDesL(*iVar1) == *iArg);
				}
			break;
		case EAdd:
		case ESubtract:
		case EMultiply:
			{
			if (iArg == NULL) LeaveIfErr(KErrArgument, _L("Argument must be specified for add, subtract and multiply operations"));
			TLex lex(*iArg);
			TInt operand;
			TInt err = lex.Val(operand);
			LeaveIfErr(err, _L("Couldn't parse an integer from argument '%S'"), iArg);
			TInt value = 0;
			TRAP_IGNORE(value = env.GetAsIntL(*iVar1)); // If it doesn't yet exist or isn't an int, we'll treat it as if it was zero
			if (iOperation == ESubtract) value = value - operand;
			else if (iOperation == EMultiply) value = value * operand;
			else value = value + operand;
			env.SetL(*iVar1, value);
			break;
			}
		default:
			User::Leave(KErrNotSupported);
		}
	}
	
const TDesC& CCmdVar::Name() const
	{
	_LIT(KName, "var");
	return KName;
	}

void CCmdVar::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgVariable, "variable");
	_LIT(KArgOperation, "operation");
	_LIT(KArgArgument, "argument");

	aArguments.AppendStringL(iVar1, KArgVariable);
	aArguments.AppendEnumL((TInt&)iOperation, KArgOperation);
	aArguments.AppendStringL(iArg, KArgArgument);
	}


//
// CCmdSource.
//

CCommandBase* CCmdSource::NewLC()
	{
	CCmdSource* self = new (ELeave) CCmdSource();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSource::~CCmdSource()
	{
	delete iArgs;
	delete iScriptData;
	delete iParser;
	}

CCmdSource::CCmdSource() : CCommandBase(EManualComplete)
	{
	}

void CCmdSource::DoRunL()
	{
	TIoHandleSet ioHandles(IoSession(), Stdin(), Stdout(), Stderr());
	TBool helpPrinted;
	iScriptData = CShell::ReadScriptL(iFileName, iArgs, Env(), FsL(), ioHandles, helpPrinted);

	if (helpPrinted)
		{
		Complete();
		}
	else
		{
		TUint mode = CParser::EExportLineNumbers;
		if (iKeepGoing)
			{
			mode |= CParser::EKeepGoing;
			}
		iParser = CParser::NewL(mode, *iScriptData, IoSession(), Stdin(), Stdout(), Stderr(), Env(), gShell->CommandFactory(), this);
		iParser->Start();
		}
	}
	
const TDesC& CCmdSource::Name() const
	{
	_LIT(KName, "source");
	return KName;
	}

void CCmdSource::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFileName, "script_file_name");
	aArguments.AppendFileNameL(iFileName, KArgFileName);

	_LIT(KArgArgs, "script_args");
	aArguments.AppendStringL(iArgs, KArgArgs);
	}

void CCmdSource::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptKeepGoing, "keep-going");
	aOptions.AppendBoolL(iKeepGoing, KOptKeepGoing);
	}

void CCmdSource::HandleParserComplete(CParser&, const TError& aError)
	{
	TInt err = aError.Error();
	if (err < 0)
		{
		aError.Report();
		PrintError(err, _L("Aborted \"%S\" at line %d"), &aError.ScriptFileName(), aError.ScriptLineNumber());
		}
	Complete(err);
	}

//
// CCmdStart.
//
	
CCommandBase* CCmdStart::NewLC()
	{
	CCmdStart* self = new (ELeave) CCmdStart();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdStart::~CCmdStart()
	{
	delete iExe;
	delete iCommandLine;
	}

CCmdStart::CCmdStart()
	{
	}

void CCmdStart::DoRunL()
	{
	if (iRendezvous && iWait)
		{
		LeaveIfErr(KErrArgument, _L("--rendezvous cannot be used with --wait"));
		}
	if (iTimeout && !(iRendezvous || iWait))
		{
		LeaveIfErr(KErrArgument, _L("--timeout must be used with either --rendezvous or --wait"));
		}
	if (iMeasure && !(iRendezvous || iWait))
		{
		LeaveIfErr(KErrArgument, _L("--measure must be used with either --rendezvous or --wait"));
		}

	TRequestStatus waitStatus;
	RProcess process;
	LeaveIfErr(process.Create(*iExe, iCommandLine ? *iCommandLine : KNullDesC(), EOwnerThread), _L("Couldn't create \"%S\" process"), iExe);
	CleanupClosePushL(process);

	if (iRendezvous)
		{
		process.Rendezvous(waitStatus);
		if (waitStatus != KRequestPending)
			{
			User::WaitForRequest(waitStatus);
			process.Kill(0);
			LeaveIfErr(waitStatus.Int(), _L("Failed to log on to process"));
			}
		}
	else if (iWait)
		{
		process.Logon(waitStatus);
		if (waitStatus != KRequestPending)
			{
			User::WaitForRequest(waitStatus);
			process.Kill(0);
			LeaveIfErr(waitStatus.Int(), _L("Failed to log on to process"));
			}
		}

	TRequestStatus timerStat;
	RTimer timer;
	if (iTimeout)
		{
		User::LeaveIfError(timer.CreateLocal());
		CleanupClosePushL(timer);
		timer.After(timerStat, iTimeout * 1000 * 1000);
		}

	TUint32 startTime = 0, endTime = 0;
	if (iMeasure) startTime = User::NTickCount();

	process.Resume();

	if (iTimeout)
		{
		User::WaitForRequest(waitStatus, timerStat);
		if (iMeasure) endTime = User::NTickCount();

		if (waitStatus == KRequestPending)
			{
			// Then the timer expired
			if (iRendezvous)
				{
				process.RendezvousCancel(waitStatus);
				}
			else if (iWait)
				{
				process.LogonCancel(waitStatus);
				}
			User::WaitForRequest(waitStatus);
			LeaveIfErr(KErrTimedOut, _L("Timed out waiting for \"%S\""), iExe);
			}
		else
			{
			timer.Cancel();
			User::WaitForRequest(timerStat);
			}

		CleanupStack::PopAndDestroy(&timer);
		}
	else if (iRendezvous || iWait)
		{
		User::WaitForRequest(waitStatus);
		if (iMeasure) endTime = User::NTickCount();
		}

	CleanupStack::PopAndDestroy(&process);

	if (iMeasure)
		{
		TUint32 elapsed = endTime - startTime;
		elapsed *= NanoTickPeriod();
		TBuf<50> timeBuf;
		FormatTime(elapsed, timeBuf);
		Write(timeBuf);
		Write(_L("\r\n"));
		}

	if (iRendezvous)
		{
		LeaveIfErr(waitStatus.Int(), _L("Error returned from rendezvous"));
		}
	if (iWait)
		{
		LeaveIfErr(waitStatus.Int(), _L("Error returned from logon"));
		}
	}
	
const TDesC& CCmdStart::Name() const
	{
	_LIT(KName, "start");
	return KName;
	}

void CCmdStart::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KExeName, "exe-name");
	_LIT(KArgs, "arguments");

	aArguments.AppendStringL(iExe, KExeName);
	aArguments.AppendStringL(iCommandLine, KArgs);
	}

void CCmdStart::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptRendezvous, "rendezvous");
	_LIT(KOptWait, "wait");
	_LIT(KOptTimeout, "timeout");
	_LIT(KOptMeasure, "measure");

	aOptions.AppendBoolL(iRendezvous, KOptRendezvous);
	aOptions.AppendBoolL(iWait, KOptWait);
	aOptions.AppendIntL(iTimeout, KOptTimeout);
	aOptions.AppendBoolL(iMeasure, KOptMeasure);
	}


//
// CCmdCompare.
//
	
CCommandBase* CCmdCompare::NewLC()
	{
	CCmdCompare* self = new (ELeave) CCmdCompare();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdCompare::~CCmdCompare()
	{
	}

CCmdCompare::CCmdCompare()
	{
	}

void CCmdCompare::DoRunL()
	{
	RFile file1;
	LeaveIfErr(file1.Open(FsL(), iFileName1, EFileRead), _L("Couldn't open \"%S\" for reading"), &iFileName1);
	CleanupClosePushL(file1);
	RFile file2;
	LeaveIfErr(file2.Open(FsL(), iFileName2, EFileRead), _L("Couldn't open \"%S\" for reading"), &iFileName2);
	CleanupClosePushL(file2);

	TBool same(ETrue);
	FOREVER
		{
		TBuf8<KBlockSize> buf1;
		TBuf8<KBlockSize> buf2;
		LeaveIfErr(file1.Read(buf1), _L("Couldn't read \"%S\""), &iFileName1);
		LeaveIfErr(file2.Read(buf2), _L("Couldn't read \"%S\""), &iFileName2);
		if (buf1 != buf2)
			{
			same = EFalse;
			break;
			}
		if (buf1.Length() == 0)
			{
			break;
			}
		}

	CleanupStack::PopAndDestroy(2, &file1);
	if (iVerbose)
		{
		if (same)
			{
			Write(_L("Identical\r\n"));
			}
		else
			{
			Write(_L("Different\r\n"));
			}
		}
	Complete(!same);
	}
	
const TDesC& CCmdCompare::Name() const
	{
	_LIT(KName, "compare");
	return KName;
	}

void CCmdCompare::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KFileName1, "file_name_1");
	_LIT(KFileName2, "file_name_2");
	aArguments.AppendFileNameL(iFileName1, KFileName1);
	aArguments.AppendFileNameL(iFileName2, KFileName2);
	}

void CCmdCompare::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	}


//
// CCmdTime.
//
	
CCommandBase* CCmdTime::NewLC()
	{
	CCmdTime* self = new (ELeave) CCmdTime();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTime::~CCmdTime()
	{
	delete iParser;
	delete iCommandLine;
	}

CCmdTime::CCmdTime() : CCommandBase(EManualComplete), iRepeatCount(1)
	{
	}

void CCmdTime::DoRunL()
	{
	NextIterationL();
	}

void CCmdTime::NextIterationL()
	{
	delete iParser;
	iParser = NULL;
	iParser = CParser::NewL(CParser::ENormal, *iCommandLine, IoSession(), Stdin(), Stdout(), Stderr(), Env(), gShell->CommandFactory(), this);
	iCountBefore = iFastCounter ? User::FastCounter() : User::NTickCount();
	iParser->Start();
	}
	
const TDesC& CCmdTime::Name() const
	{
	_LIT(KName, "time");
	return KName;
	}

void CCmdTime::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgs, "command");
	aArguments.AppendStringL(iCommandLine, KArgs);
	}

void CCmdTime::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptFastCounter, "fast-counter");
	_LIT(KOptRepeatCount, "repeat");
	aOptions.AppendBoolL(iHuman, KOptHuman);
	aOptions.AppendBoolL(iFastCounter, KOptFastCounter);
	aOptions.AppendIntL(iRepeatCount, KOptRepeatCount);
	}

void CCmdTime::HandleParserComplete(CParser&, const TError& aError)
	{
	if (aError.Error() < 0)
		{
		aError.Report();
		}

	TUint32 countAfter = iFastCounter ? User::FastCounter() : User::NTickCount();
	
	TUint64 difference;
	if (iFastCounter)
		{
		difference = FastCounterCountsUp() ? (countAfter - iCountBefore) : (iCountBefore - countAfter);
		difference *= 1000000;
		difference /= FastCounterFrequency();
		}
	else
		{
		difference = countAfter - iCountBefore;
		difference *= NanoTickPeriod();
		}

	if (Stdout().AttachedToConsole())
		{
		TPoint cursorPos;
		Stdout().GetCursorPos(cursorPos);
		if (cursorPos.iX != 0)
			{
			Write(KNewLine);
			}
		}

	if (iHuman)
		{
		TBuf<256> timeBuf;
		FormatTime(difference, timeBuf);
		timeBuf.Append(KNewLine);
		Write(timeBuf);
		}
	else if (iRepeatCount == 1)
		{
		// Don't print individual times if we asked for non-human readable output. (If you want that behaviour, use "repeat n time xyz...")
		Printf(_L("%Lu\r\n"), difference);
		}
	
	iTotalTime += difference;
	iIteration++;
	if (iRepeatCount > 1 && iIteration == iRepeatCount)
		{
		// Need to print the average
		difference = iTotalTime / iRepeatCount;
		if (iHuman)
			{
			TBuf<256> timeBuf;
			timeBuf.Append(_L("Average: "));
			FormatTime(difference, timeBuf);
			timeBuf.Append(KNewLine);
			Write(timeBuf);
			}
		else
			{
			Printf(_L("%Lu\r\n"), difference);
			}
		}
	if (aError.Error() < 0 || iIteration == iRepeatCount)
		{
		Complete(aError.Error());
		}
	else
		{
		// Rerun the fun
		TRAPD(err, NextIterationL());
		if (err) Complete(err);
		}
	}


//
// CCmdRepeat.
//
	
CCommandBase* CCmdRepeat::NewLC()
	{
	CCmdRepeat* self = new (ELeave) CCmdRepeat();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRepeat::~CCmdRepeat()
	{
	delete iParser;
	delete iCommandLine;
	}

CCmdRepeat::CCmdRepeat() : CCommandBase(EManualComplete)
	{
	}

void CCmdRepeat::DoRunL()
	{
	if (iNumRepeats == 0)
		{
		iForever = ETrue;
		}

	CreateParserL();
	}
	
const TDesC& CCmdRepeat::Name() const
	{
	_LIT(KName, "repeat");
	return KName;
	}

void CCmdRepeat::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCount, "count");
	_LIT(KArgs, "command");

	aArguments.AppendUintL(iNumRepeats, KCount);
	aArguments.AppendStringL(iCommandLine, KArgs);
	}

void CCmdRepeat::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptKeepGoing, "keep-going");
	_LIT(KOptWait, "wait");

	aOptions.AppendBoolL(iKeepGoing, KOptKeepGoing);
	aOptions.AppendIntL(iWaitTime, KOptWait);
	}

void CCmdRepeat::HandleParserComplete(CParser&, const TError& aError)
	{
	TRAPD(err, HandleParserCompleteL(aError));
	if (err)
		{
		Complete(err);
		}
	}

void CCmdRepeat::HandleParserCompleteL(const TError& aError)
	{
	if (aError.Error() < 0)
		{
		aError.Report();
		}

	if (((aError.Error() == KErrNone) || iKeepGoing) && ((++iCount < iNumRepeats) || iForever))
		{
		delete iParser;
		iParser = NULL;
		if (iWaitTime) User::After(iWaitTime * 1000);
		CreateParserL();
		}
	else
		{
		Complete(aError.Error());
		}
	}

void CCmdRepeat::CreateParserL()
	{
	TBuf<32> buf;
	buf.AppendNum(iCount);
	Env().SetL(KRepeatCount, buf);
	iParser = CParser::NewL(iKeepGoing ? CParser::EKeepGoing : CParser::ENormal, *iCommandLine, IoSession(), Stdin(), Stdout(), Stderr(), Env(), gShell->CommandFactory(), this);
	iParser->Start();
	}


//
// CCmdDebug.
//
	
CCommandBase* CCmdDebug::NewLC()
	{
	CCmdDebug* self = new (ELeave) CCmdDebug();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdDebug::~CCmdDebug()
	{
	delete iScriptData;
	delete iParser;
	delete iArgs;
	}

CCmdDebug::CCmdDebug() : CCommandBase(EManualComplete)
	{
	}

void CCmdDebug::DoRunL()
	{
	TIoHandleSet ioHandles(IoSession(), Stdin(), Stdout(), Stderr());
	TBool helpPrinted;
	iScriptData = CShell::ReadScriptL(iFileName, iArgs, Env(), FsL(), ioHandles, helpPrinted);
	if (helpPrinted)
		{
		Complete();
		}
	else
		{
		TUint mode = CParser::EDebug | CParser::EExportLineNumbers;
		if (iKeepGoing)
			{
			mode |= CParser::EKeepGoing;
			}
		iParser = CParser::NewL(mode, *iScriptData, IoSession(), Stdin(), Stdout(), Stderr(), Env(), gShell->CommandFactory(), this);
		iParser->Start();
		}
	}
	
const TDesC& CCmdDebug::Name() const
	{
	_LIT(KName, "debug");
	return KName;
	}

void CCmdDebug::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFileName, "script_file_name");
	aArguments.AppendFileNameL(iFileName, KArgFileName);

	_LIT(KArgArgs, "script_args");
	aArguments.AppendStringL(iArgs, KArgArgs);
	}

void CCmdDebug::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptKeepGoing, "keep-going");
	aOptions.AppendBoolL(iKeepGoing, KOptKeepGoing);
	}

void CCmdDebug::HandleParserComplete(CParser&, const TError& aError)
	{
	Printf(_L("Completed with %d (%S)\r\n"), aError.Error(), Stringify::Error(aError.Error()));
	Complete(aError.Error());
	}

void CCmdDebug::AboutToExecuteLine(const TDesC& aOrignalLine, const TDesC& aExpandedLine)
	{
	Write(aOrignalLine);
	Write(KNewLine);
	TRAP_IGNORE(InteractL(aExpandedLine));
	}

void CCmdDebug::LineReturned(TInt aError)
	{
	Printf(_L("Returned %d (%S)\r\n"), aError, Stringify::Error(aError));
	}

void CCmdDebug::InteractL(const TDesC& aExpandedLine)
	{
	FOREVER
		{
		TBuf<1> command;
		ReadL(command);

		_LIT(KStep, "s");
		_LIT(KExpand, "x");

		if (command == KStep)
			{
			return;
			}
		else if (command == KExpand)
			{
			Write(aExpandedLine);
			Write(KNewLine);
			}
		else
			{
			Write(_L("Unknown command\r\n"));
			}
		}
	}


//
// CCmdReadMem.
//

CCommandBase* CCmdReadMem::NewLC()
	{
	CCmdReadMem* self = new(ELeave) CCmdReadMem();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdReadMem::~CCmdReadMem()
	{
	}

CCmdReadMem::CCmdReadMem()
	{
	}

const TDesC& CCmdReadMem::Name() const
	{
	_LIT(KName, "readmem");
	return KName;
	}

void CCmdReadMem::DoRunL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	LoadMemoryAccessL();

	if (iFileName.Length() > 0)
		{
		LeaveIfErr(iFile.Create(FsL(), iFileName, EFileWrite), _L("Unable to create \"%S\""), &iFileName);
		}

	if (!iOptions.IsPresent(&iThreadId))
		{
		iThreadId = RThread().Id();
		}

	TBuf8<KBlockSize> buf;
	TInt bytesRead = 0;
	TInt dumpPos = 0;
	while ((TUint)bytesRead < iSize)
		{
		buf.Zero();
		TThreadMemoryAccessParamsBuf memoryAccessParamsBuf;
		TThreadMemoryAccessParams& memoryAccessParams = memoryAccessParamsBuf();
		memoryAccessParams.iId = iThreadId;
		memoryAccessParams.iAddr = (TUint8*)iAddress + bytesRead;
		TInt bytesToRead = iSize - bytesRead;
		if (bytesToRead > KBlockSize)
			{
			bytesToRead = KBlockSize;
			}
		memoryAccessParams.iSize = bytesToRead;
		User::LeaveIfError(iMemAccess.GetThreadMem(memoryAccessParamsBuf, buf));
		if (iFile.SubSessionHandle())
			{
			User::LeaveIfError(iFile.Write(buf));
			}
		else
			{
			Dump(buf, *this, dumpPos);
			}
		bytesRead += bytesToRead;
		}

#else
	PrintError(KErrNotSupported, _L("Unable to read memory because fshell was not built with FSHELL_MEMORY_ACCESS_SUPPORT defined. That probably means this plaform doesn't support the MemoryAccess device driver."));
	User::Leave(KErrNotSupported);
#endif
	}

void CCmdReadMem::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdOptThreadId, "thread_id");
	aOptions.AppendIntL(iThreadId, KCmdOptThreadId);
	}

void CCmdReadMem::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdArgAddress, "address");
	_LIT(KCmdArgSize, "size");
	_LIT(KCmdArgFileName, "file_name");

	aArguments.AppendUintL(iAddress, KCmdArgAddress);
	aArguments.AppendUintL(iSize, KCmdArgSize);
	aArguments.AppendFileNameL(iFileName, KCmdArgFileName);
	}


//
// CCmdE32Header.
//

CCommandBase* CCmdE32Header::NewLC()
	{
	CCmdE32Header* self = new(ELeave) CCmdE32Header();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdE32Header::~CCmdE32Header()
	{
	delete iFormatter;
	}

CCmdE32Header::CCmdE32Header()
	{
	}

const TDesC& CCmdE32Header::Name() const
	{
	_LIT(KName, "e32header");
	return KName;
	}

#define CASE_LIT2(x, y) case x: { _LIT(KName, #y); return &KName; }
const TDesC* CompressionType(TUint aType)
	{
	switch (aType)
		{
		CASE_LIT2(0, KFormatNotCompressed)
		CASE_LIT2(0x101F7AFC, KUidCompressionDeflate)
		CASE_LIT2(0x102822AA, KUidCompressionBytePair)
		default:
			_LIT(KUnknown, "?");
			return &KUnknown;
		}
	}

void CCmdE32Header::DoRunL()
	{
	if (iXip && iNotXip)
		{
		LeaveIfErr(KErrArgument, _L("--xip and --not-xip options are mutually exclusive"));
		}

	RFile file;
	CleanupClosePushL(file);
	LeaveIfErr(file.Open(FsL(), iFileName, EFileRead), _L("Couldn't open \"%S\" for reading"), &iFileName);

	iFormatter = CTextFormatter::NewL(Stdout());
	IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x100);

	if ((Fs().IsFileInRom(iFileName) || iXip) && !iNotXip)
		{
		TAny* ptr = Fs().IsFileInRom(iFileName);
		if (ptr)
			{
			buf->AppendFormatL(_L("ROM file address:\t0x%08x\r\n"), ptr);
			}
		TRomImageHeader imageHeader;
		TPckg<TRomImageHeader> pckg(imageHeader);
		LeaveIfErr(file.Read(pckg, sizeof(TRomImageHeader)), _L("Couldn't read E32 image header"));
		buf->AppendFormatL(_L("UIDs:\t0x%08x 0x%08x 0x%08x\r\n"), imageHeader.iUid1, imageHeader.iUid2, imageHeader.iUid3);
		buf->AppendFormatL(_L("UID checksum:\t0x%08x\r\n"), imageHeader.iUidChecksum);
		buf->AppendFormatL(_L("Entry point offset:\t0x%08x\r\n"), imageHeader.iEntryPoint);
		buf->AppendFormatL(_L("Code address:\t0x%08x\r\n"), imageHeader.iCodeAddress);
		buf->AppendFormatL(_L("Data address:\t0x%08x\r\n"), imageHeader.iDataAddress);
		buf->AppendFormatL(_L("Code size:\t%d\r\n"), imageHeader.iCodeSize);
		buf->AppendFormatL(_L("Text size:\t%d\r\n"), imageHeader.iTextSize);
		buf->AppendFormatL(_L("Data size:\t%d\r\n"), imageHeader.iDataSize);
		buf->AppendFormatL(_L("BSS size:\t%d\r\n"), imageHeader.iBssSize);
		buf->AppendFormatL(_L("Minimum heap size:\t%d\r\n"), imageHeader.iHeapSizeMin);
		buf->AppendFormatL(_L("Maximum heap size:\t%d\r\n"), imageHeader.iHeapSizeMax);
		buf->AppendFormatL(_L("Stack size:\t%d\r\n"), imageHeader.iStackSize);
		buf->AppendFormatL(_L("DLL ref table address:\t0x%08x\r\n"), imageHeader.iDllRefTable);
		buf->AppendFormatL(_L("Export dir count:\t%d\r\n"), imageHeader.iExportDirCount);
		buf->AppendFormatL(_L("Export dir address:\t0x%08x\r\n"), imageHeader.iExportDir);
		buf->AppendFormatL(_L("Secure ID:\t0x%08x\r\n"), imageHeader.iS.iSecureId);
		buf->AppendFormatL(_L("Vendor ID:\t0x%08x\r\n"), imageHeader.iS.iVendorId);
		buf->AppendFormatL(_L("Capabilities:\t0x%08x 0x%08x\r\n"), imageHeader.iS.iCaps[0], imageHeader.iS.iCaps[1]);
		TInt majorVersion = imageHeader.iToolsVersion.iMajor;
		TInt minorVersion = imageHeader.iToolsVersion.iMinor;
		TInt buildVersion = imageHeader.iToolsVersion.iBuild;
		buf->AppendFormatL(_L("Tools version:\t%d.%d (%d)\r\n"), majorVersion, minorVersion, buildVersion);
		buf->AppendFormatL(_L("Flags:\t0x%08x\r\n"), imageHeader.iFlags);
		buf->AppendFormatL(_L("Process priority:\t%d\r\n"), imageHeader.iPriority);
		buf->AppendFormatL(_L("Data BSS linear base:\t0x%08x\r\n"), imageHeader.iDataBssLinearBase);
		buf->AppendFormatL(_L("Next extension:\t0x%08x\r\n"), imageHeader.iNextExtension);
		buf->AppendFormatL(_L("Hardware variant:\t0x%08x\r\n"), imageHeader.iHardwareVariant);
		buf->AppendFormatL(_L("Total data size:\t%d\r\n"), imageHeader.iTotalDataSize);
		buf->AppendFormatL(_L("Module version:\t0x%08x (%d.%d)\r\n"), imageHeader.iModuleVersion, imageHeader.iModuleVersion>>16, imageHeader.iModuleVersion&0xFFFF);
		buf->AppendFormatL(_L("Exception descriptor:\t0x%08x\r\n"), imageHeader.iExceptionDescriptor);
		}
	else
		{
		E32ImageHeaderV* imageHeader = new(ELeave) E32ImageHeaderV;
		CleanupStack::PushL(imageHeader);
		TPckg<E32ImageHeaderV> pckg(*imageHeader);
		LeaveIfErr(file.Read(pckg, sizeof(E32ImageHeaderV)), _L("Couldn't read E32 image header"));
		buf->AppendFormatL(_L("UIDs:\t0x%08x 0x%08x 0x%08x\r\n"), imageHeader->iUid1, imageHeader->iUid2, imageHeader->iUid3);
		buf->AppendFormatL(_L("UID checksum:\t0x%08x\r\n"), imageHeader->iUidChecksum);
		buf->AppendFormatL(_L("Header CRC:\t0x%08x\r\n"), imageHeader->iHeaderCrc);
		buf->AppendFormatL(_L("Module version:\t0x%08x (%d.%d)\r\n"), imageHeader->iModuleVersion, imageHeader->iModuleVersion>>16, imageHeader->iModuleVersion&0xFFFF);
		buf->AppendFormatL(_L("Compression type:\t0x%08x (%S)\r\n"), imageHeader->iCompressionType, CompressionType(imageHeader->iCompressionType));
		TInt majorVersion = imageHeader->iToolsVersion.iMajor;
		TInt minorVersion = imageHeader->iToolsVersion.iMinor;
		TInt buildVersion = imageHeader->iToolsVersion.iBuild;
		buf->AppendFormatL(_L("Tools version:\t%d.%d (%d)\r\n"), majorVersion, minorVersion, buildVersion);
		buf->AppendFormatL(_L("Build time:\t0x%08x 0x%08x\r\n"), imageHeader->iTimeLo, imageHeader->iTimeHi);
		buf->AppendFormatL(_L("Flags:\t0x%08x\r\n"), imageHeader->iFlags);
		buf->AppendFormatL(_L("Code size:\t%d\r\n"), imageHeader->iCodeSize);
		buf->AppendFormatL(_L("Minimum heap size:\t%d\r\n"), imageHeader->iHeapSizeMin);
		buf->AppendFormatL(_L("Maximum heap size:\t%d\r\n"), imageHeader->iHeapSizeMax);
		buf->AppendFormatL(_L("Stack size:\t%d\r\n"), imageHeader->iStackSize);
		buf->AppendFormatL(_L("BSS size:\t%d\r\n"), imageHeader->iBssSize);
		buf->AppendFormatL(_L("Entry point offset:\t0x%08x\r\n"), imageHeader->iEntryPoint);
		buf->AppendFormatL(_L("Code base:\t0x%08x\r\n"), imageHeader->iCodeBase);
		buf->AppendFormatL(_L("Data base:\t0x%08x\r\n"), imageHeader->iDataBase);
		buf->AppendFormatL(_L("DLL ref table count:\t%d\r\n"), imageHeader->iDllRefTableCount);
		buf->AppendFormatL(_L("Export dir offset:\t0x%08x\r\n"), imageHeader->iExportDirOffset);
		buf->AppendFormatL(_L("Export dir count:\t%d\r\n"), imageHeader->iExportDirCount);
		buf->AppendFormatL(_L("Text size:\t%d\r\n"), imageHeader->iTextSize);
		buf->AppendFormatL(_L("Code offset:\t0x%08x\r\n"), imageHeader->iCodeOffset);
		buf->AppendFormatL(_L("Data offset:\t0x%08x\r\n"), imageHeader->iDataOffset);
		buf->AppendFormatL(_L("Import offset:\t0x%08x\r\n"), imageHeader->iImportOffset);
		buf->AppendFormatL(_L("Code reloc offset:\t0x%08x\r\n"), imageHeader->iCodeRelocOffset);
		buf->AppendFormatL(_L("Data reloc offset:\t0x%08x\r\n"), imageHeader->iDataRelocOffset);
		TInt temp; // Why use a temp here?
		temp = imageHeader->iProcessPriority;
		buf->AppendFormatL(_L("Process priority:\t%d\r\n"), temp);
		temp = imageHeader->iCpuIdentifier;
		buf->AppendFormatL(_L("CPU identifier:\t%d\r\n"), temp);
		buf->AppendFormatL(_L("Uncompressed size:\t%d\r\n"), imageHeader->iUncompressedSize);
		buf->AppendFormatL(_L("Secure ID:\t0x%08x\r\n"), imageHeader->iS.iSecureId);
		buf->AppendFormatL(_L("Vendor ID:\t0x%08x\r\n"), imageHeader->iS.iVendorId);
		buf->AppendFormatL(_L("Capabilities:\t0x%08x 0x%08x\r\n"), imageHeader->iS.iCaps[0], imageHeader->iS.iCaps[1]);
		buf->AppendFormatL(_L("Exception descriptor:\t0x%08x\r\n"), imageHeader->iExceptionDescriptor);
		buf->AppendFormatL(_L("Spare2:\t0x%08x\r\n"), imageHeader->iSpare2);
		temp = imageHeader->iExportDescSize;
		buf->AppendFormatL(_L("Export description size:\t%d\r\n"), temp);
		temp = imageHeader->iExportDescType;
		buf->AppendFormatL(_L("Export description type:\t%d\r\n"), temp);
		temp = imageHeader->iExportDesc[0];
		buf->AppendFormatL(_L("Export description:\t%d\r\n"), temp);
		CleanupStack::PopAndDestroy(imageHeader);
		}

	iFormatter->TabulateL(0, 2, buf->Descriptor());
	Write(iFormatter->Descriptor());
	CleanupStack::PopAndDestroy(2, &file);
	}

void CCmdE32Header::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdOptXip, "xip");
	_LIT(KCmdOptNotXip, "not-xip");

	aOptions.AppendBoolL(iXip, KCmdOptXip);
	aOptions.AppendBoolL(iNotXip, KCmdOptNotXip);
	}

void CCmdE32Header::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdArgFileName, "file_name");
	aArguments.AppendFileNameL(iFileName, KCmdArgFileName);
	}


//
// CCmdObjInfo.
//

CCommandBase* CCmdObjInfo::NewLC()
	{
	CCmdObjInfo* self = new(ELeave) CCmdObjInfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdObjInfo::~CCmdObjInfo()
	{
	}

CCmdObjInfo::CCmdObjInfo()
	{
	}

const TDesC& CCmdObjInfo::Name() const
	{
	_LIT(KName, "objinfo");
	return KName;
	}

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT

#define CASE_RETURN_LIT(XXX) case XXX: { _LIT(_KLit, #XXX); return &_KLit; }
#define DEFAULT_RETURN_LIT(XXX) default: { _LIT(_KLit, XXX); return &_KLit; }

EXPORT_C const TDesC* StringifyObjectType(TObjectType aObjectType)
	{
	switch (aObjectType)
		{
		CASE_RETURN_LIT(EThread);
		CASE_RETURN_LIT(EProcess);
		CASE_RETURN_LIT(EChunk);
		CASE_RETURN_LIT(ELibrary);
		CASE_RETURN_LIT(ESemaphore);
		CASE_RETURN_LIT(EMutex);
		CASE_RETURN_LIT(ETimer);
		CASE_RETURN_LIT(EServer);
		CASE_RETURN_LIT(ESession);
		CASE_RETURN_LIT(ELogicalDevice);
		CASE_RETURN_LIT(EPhysicalDevice);
		CASE_RETURN_LIT(ELogicalChannel);
		CASE_RETURN_LIT(EChangeNotifier);
		CASE_RETURN_LIT(EUndertaker);
		CASE_RETURN_LIT(EMsgQueue);
		CASE_RETURN_LIT(EPropertyRef);
		CASE_RETURN_LIT(ECondVar);
		CASE_RETURN_LIT(EIpcMessageD);
		CASE_RETURN_LIT(EIpcMessage);
		CASE_RETURN_LIT(EIpcClient);
		DEFAULT_RETURN_LIT("*** OBJECT TYPE UNKNOWN ***");
		}
	}

void CCmdObjInfo::PrintObjectDetailsL(TUint aObjectAddress)
	{
	TObjectType objectType;
	TInt err = iMemAccess.GetObjectType((TUint8*)aObjectAddress, objectType);
	if (err == KErrNone)
		{
		TObjectKernelInfo objectInfo;
		TPckg<TObjectKernelInfo> objectInfoPckg(objectInfo);
		err = iMemAccess.GetObjectInfo(objectType, (TUint8*)(aObjectAddress), objectInfoPckg);
		if (err == KErrNone)
			{
			TFullName fullName;
			fullName.Copy(objectInfo.iFullName);
			Printf(_L("0x%08x %2d %S (type=%d) %S\r\n"), aObjectAddress, objectInfo.iAccessCount, StringifyObjectType(objectType), objectType, &fullName);
			}
		else
			{
			PrintWarning(_L("Couldn't read details of %S 0x%08x : %S(%d)"), StringifyObjectType(objectType), aObjectAddress, Stringify::Error(err), err);
			}
		}
	else
		{
		PrintWarning(_L("Couldn't find type of object 0x%08x : %S(%d)"), aObjectAddress, Stringify::Error(err), err);
		}

	if (iReferencers)
		{
		PrintObjectReferencersL(aObjectAddress);
		}
	}

void CCmdObjInfo::PrintObjectReferencersL(TUint aObjectAddress)
	{
	TInt err = KErrNone;
	TInt bufSize = 256;
	HBufC8* idBuf;
	do
		{
		idBuf = HBufC8::NewLC(bufSize);
		TPtr8 idPtr(idBuf->Des());
		err = iMemAccess.GetHandleOwners((TUint8*)aObjectAddress, idPtr);
		if (err == KErrOverflow)
			{
			CleanupStack::PopAndDestroy(idBuf);
			bufSize *= 2;
			}
		}
		while (err == KErrOverflow);

	const TInt numIds = idBuf->Length() / sizeof(TUint);
	if (numIds > 0)
		{
		Write(_L("Referenced by:\r\n"));
		}
	for (TInt i = 0; i < numIds; i++)
		{
		TUint id = ((TUint*)idBuf->Ptr())[i];
		TBool isThread = ETrue;
		RHandleBase h;
		TInt err = ((RThread&)h).Open(TThreadId(id));
		if (err)
			{
			// Maybe it's a process id instead.
			isThread = EFalse;
			err = ((RProcess&)h).Open(TProcessId(id));
			}

		if (err)
			{
			PrintWarning(_L("Unable to open thread or process for id %d: %S (%d)"), id, Stringify::Error(err), err);
			}
		else
			{
			TFullName name = h.FullName();
			_LIT(KThread,  "\tthread:  ");
			_LIT(KProcess, "\tprocess: ");
			Printf(_L("%S%u\t%S\r\n"), isThread ? &KThread : &KProcess, id, &name);
			}
		}

	CleanupStack::PopAndDestroy(idBuf);
	}

void CCmdObjInfo::PrintReferencedObjectDetailsL(TOwnerType aOwnerType, TUint aId)
	{
	TInt err = KErrNone;
	TInt bufSize = 256;

	HBufC8* addressesBuf;
	do
		{
		addressesBuf = HBufC8::NewLC(bufSize);
		TPtr8 addressesPtr(addressesBuf->Des());
		if (aOwnerType == EOwnerProcess)
			{
			err = iMemAccess.GetProcessHandles(aId, addressesPtr);
			}
		else
			{
			err = iMemAccess.GetThreadHandles(aId, addressesPtr);
			}
		if (err == KErrOverflow)
			{
			CleanupStack::PopAndDestroy(addressesBuf);
			bufSize *= 2;
			}
		}
		while (err == KErrOverflow);

	const TInt numAddresses = addressesBuf->Length() / sizeof(TUint32*);
	if (numAddresses == 0)
		{
		Write(_L("No objects found.\r\n"));
		}
	else
		{
		TUint32* p = (TUint32*)addressesBuf->Ptr();
		TInt offset = 0;
		while (offset < numAddresses)
			{
			TUint32* ptr = p + offset++;
			PrintObjectDetailsL(*ptr);
			}
		}

	CleanupStack::PopAndDestroy(addressesBuf);
	}

#endif // FSHELL_MEMORY_ACCESS_SUPPORT

void CCmdObjInfo::DoRunL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	LoadMemoryAccessL();

	if (iObjectAddress)
		{
		PrintObjectDetailsL(iObjectAddress);
		}
	else if (iProcessId)
		{
		if (iAll)
			{
			RProcess process;
			LeaveIfErr(process.Open(iProcessId), _L("Couldn't open process with id \"%u\""), iProcessId);
			TFullName processName(process.Name());
			Printf(_L("Objects owned by process \"%S\":\r\n"), &processName);
			PrintReferencedObjectDetailsL(EOwnerProcess, iProcessId);
			Write(_L("\r\n"));

			processName.Append(_L("*"));
			TFindThread threadFinder(processName);
			RThread thread;
			TFullName threadName;
			while (threadFinder.Next(threadName) == KErrNone)
				{
				TInt err = thread.Open(threadFinder);
				if (err)
					{
					continue;
					}
				Printf(_L("Objects owned by thread \"%S\":\r\n"), &threadName);
				PrintReferencedObjectDetailsL(EOwnerThread, thread.Id());
				Write(_L("\r\n"));
				}
			}
		else
			{
			PrintReferencedObjectDetailsL(EOwnerProcess, iProcessId);
			}
		}
	else if (iThreadId)
		{
		PrintReferencedObjectDetailsL(EOwnerThread, iThreadId);
		}

#else
	PrintError(KErrNotSupported, _L("Unable to fetch object information because fshell was not built with FSHELL_MEMORY_ACCESS_SUPPORT defined. That probably means this plaform doesn't support the MemoryAccess device driver."));
	User::Leave(KErrNotSupported);
#endif
	}

void CCmdObjInfo::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdOptReferencers, "referencers");
	_LIT(KCmdOptProcessId, "process-id");
	_LIT(KCmdOptThreadId, "thread-id");
	_LIT(KCmdOptAll, "all");

	aOptions.AppendBoolL(iReferencers, KCmdOptReferencers);
	aOptions.AppendUintL(iProcessId, KCmdOptProcessId);
	aOptions.AppendUintL(iThreadId, KCmdOptThreadId);
	aOptions.AppendBoolL(iAll, KCmdOptAll);
	}

void CCmdObjInfo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdArgObjectAddress, "object_address");
	aArguments.AppendUintL(iObjectAddress, KCmdArgObjectAddress);
	}

//
// CCmdTouch.
//

CCommandBase* CCmdTouch::NewLC()
	{
	CCmdTouch* self = new(ELeave) CCmdTouch();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTouch::~CCmdTouch()
	{
	}

CCmdTouch::CCmdTouch()
	{
	}

const TDesC& CCmdTouch::Name() const
	{
	_LIT(KName, "touch");
	return KName;
	}

void CCmdTouch::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdArgFile, "file");
	aArguments.AppendFileNameL(iFileName, KCmdArgFile);
	}

void CCmdTouch::DoRunL()
	{
	TEntry e;
	TInt err = FsL().Entry(iFileName, e);

	if (err == KErrNotFound || err == KErrPathNotFound)
		{
		RFile file;
		err = file.Create(Fs(), iFileName, EFileWrite);
		LeaveIfErr(err, _L("Couldn't create file %S"), &iFileName);
		file.Close();
		}
	else
		{
		LeaveIfErr(err, _L("Couldn't get file info for %S"), &iFileName);
		TTime now;
		now.UniversalTime();
		err = Fs().SetEntry(iFileName, now, 0, 0);
		LeaveIfErr(err, _L("Couldn't update last-modified date for %S"), &iFileName);
		}
	}


//
// CCmdDialog.
//

CCommandBase* CCmdDialog::NewLC()
	{
	CCmdDialog* self = new(ELeave) CCmdDialog();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdDialog::~CCmdDialog()
	{
	Cancel();
	iNotifier.Close();
	delete iTitle;
	delete iBody;
	delete iButton1;
	delete iButton2;
	}

CCmdDialog::CCmdDialog() : CCommandBase(EManualComplete|ECaptureCtrlC)
	{
	SetExtension(this);
	}

const TDesC& CCmdDialog::Name() const
	{
	_LIT(KName, "dialog");
	return KName;
	}

void CCmdDialog::DoRunL()
	{
	if (iTitle == NULL)
		{
		iTitle = _L("Attention").AllocL();
		}
	if ((iButton1 == NULL) && (iButton2 == NULL))
		{
		iButton1 = _L("OK").AllocL();
		iButton2 = _L("Cancel").AllocL();
		}
	else if (iButton2 == NULL)
		{
		iButton2 = KNullDesC().AllocL();
		}
	else if (iButton1 == NULL)
		{
		LeaveIfErr(KErrArgument, _L("First button not specified"));
		}
	
	if (iBody == NULL)
		{
		iBody = HBufC::NewL(0x200);
		Stdin().SetReadMode(RIoReadHandle::EFull);
		TPtr ptr(iBody->Des());
		Stdin().ReadL(ptr);
		}
	
	switch (iMode)
		{
		case EModeNotifier:
			{
			RNotifier notifier;
			LeaveIfErr(iNotifier.Connect(), _L("Couldn't connect to RNotifier"));
			iNotifier.Notify(*iTitle, *iBody, *iButton1, *iButton2, iReturnValue, iStatus);
			SetActive();
			break;
			}
		case EModeConsole:
			{
			RIoConsole console;
			console.OpenL(IoSession(), Stdin());
			CleanupClosePushL(console);
			
			RIoConsoleWriteHandle consWriter;
			consWriter.CreateL(IoSession());
			CleanupClosePushL(consWriter);
			console.AttachL(consWriter);
			
			RIoConsoleReadHandle consReader;
			consReader.CreateL(IoSession());
			CleanupClosePushL(consReader);
			console.AttachL(consReader, RIoEndPoint::EForeground);
			
			consWriter.Write(*iTitle);
			consWriter.Write(KNewLine);
			consWriter.Write(KNewLine);
			consWriter.Write(*iBody);
			consWriter.Write(KNewLine);
			consWriter.Write(KNewLine);
				{
				TBuf<0x100> buf;
				buf.AppendFormat(_L("%S <enter / select> or %S <any other key> "), iButton1, iButton2);
				consWriter.Write(buf);
				}
			TBuf<1> buf;
			consReader.ReadL(buf);
			ClearLineL(consWriter);
			CleanupStack::PopAndDestroy(3); // consReader, consWriter, console
			switch (buf[0])
				{
				case EKeyEnter:
	#ifdef FSHELL_PLATFORM_S60
				case EKeyDevice3:
	#endif
					Complete(0);
					break;
				default:
					Complete(1);
					break;
				}
			break;
			}
		case EModeNull:
			{
			Complete(0);
			break;
			}
		}
	}

void CCmdDialog::ClearLineL(RIoConsoleWriteHandle& aWriteHandle)
	{
	if (aWriteHandle.AttachedToConsole())
		{
		TPoint cursorPos;
		LeaveIfErr(aWriteHandle.GetCursorPos(cursorPos), _L("Couldn't get cursor position"));
		cursorPos.iX = 0;
		LeaveIfErr(aWriteHandle.SetCursorPosAbs(cursorPos), _L("Couldn't set cursor position to beginning of line"));
		LeaveIfErr(aWriteHandle.ClearToEndOfLine(), _L("Couldn't clear to end of line"));
		}
	else
		{
		aWriteHandle.Write(KNewLine);
		}
	}

void CCmdDialog::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptTitle, "title");
	_LIT(KOptButton1, "first-button");
	_LIT(KOptButton2, "second-button");
	_LIT(KOptMode, "mode");

	aOptions.AppendStringL(iTitle, KOptTitle);
	aOptions.AppendStringL(iButton1, KOptButton1);
	aOptions.AppendStringL(iButton2, KOptButton2);
	aOptions.AppendEnumL((TInt&)iMode, KOptMode);
	}

void CCmdDialog::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgBody, "body");
	aArguments.AppendStringL(iBody, KArgBody);
	}

void CCmdDialog::DoCancel()
	{
	iNotifier.NotifyCancel();
	}

void CCmdDialog::RunL()
	{
	// text notifier gets the buttons the wrong way round, so check if it's running and if so swap the return value around
	_LIT(KTextWindowServerName, "EWSRV.EXE[100000bf]0001"); // This is the same on winscw and target. The gui windowserver has a different UID
	RProcess ewsrv;
	TInt err = ewsrv.Open(KTextWindowServerName);
	if (err == KErrNone)
		{
		if (iReturnValue) iReturnValue = 0;
		else iReturnValue = 1;
		ewsrv.Close();
		}

	Complete(iReturnValue);
	}

void CCmdDialog::CtrlCPressed()
	{
	//Cancel();
	//Complete(KErrNone);
	Printf(_L("Sorry, clients of RNotifier cannot safely be killed, and RNotifier::NotifyCancel() doesn't work. If you really want to risk it hit CTRL-Z and run \"kill -Tm *dialog*\". Otherwise you must dismiss the dialog on the device.\r\n"));
	}

//
// CCmdJit.
//

#ifdef __WINS__

CCommandBase* CCmdJit::NewLC()
	{
	CCmdJit* self = new(ELeave) CCmdJit();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdJit::~CCmdJit()
	{
	}

CCmdJit::CCmdJit()
	{
	}

const TDesC& CCmdJit::Name() const
	{
	_LIT(KName, "jit");
	return KName;
	}

void CCmdJit::DoRunL()
	{
	switch (iOperation)
		{
		case EStatus:
			if (User::JustInTime())
				{
				Write(_L("on\r\n"));
				}
			else
				{
				Write(_L("off\r\n"));
				}
			break;
		case EOn:
			User::SetJustInTime(ETrue);
			break;
		case EOff:
			User::SetJustInTime(EFalse);
			break;
		default:
			LeaveIfErr(KErrArgument, _L("Unrecognised operation"));
		}
	}

void CCmdJit::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgOperation, "operation");
	aArguments.AppendEnumL((TInt&)iOperation, KArgOperation);
	}

#endif // __WINS__


//
// CCmdConsole.
//

CCommandBase* CCmdConsole::NewLC()
	{
	CCmdConsole* self = new(ELeave) CCmdConsole();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdConsole::~CCmdConsole()
	{
	}

CCmdConsole::CCmdConsole()
	{
	}

const TDesC& CCmdConsole::Name() const
	{
	_LIT(KName, "console");
	return KName;
	}
	
_LIT(KPersistentConsole, " (persistent console)");

void CCmdConsole::DoRunL()
	{
	if (iIsRemote && iIsNull)
		{
		LeaveIfErr(KErrArgument, _L("-r and -n options are mutually exclusive"));
		}

	RIoConsole console;
	LeaveIfErr(console.Open(IoSession(), Stdout()), _L("Couldn't open handle to console"));
	CleanupClosePushL(console);
	
	TFileName name;
	if (console.IsTypeL(RIoHandle::EPersistentConsole))
		{
		TInt err = console.ObjectName(name);
		if (err!=KErrOverflow) User::LeaveIfError(err);
		name.SetLength(Min(name.Length(), name.MaxLength()-KPersistentConsole().Length()));
		name.Append(KPersistentConsole);
		}
	else
		{
		LeaveIfErr(console.Implementation(name), _L("Couldn't read console implemenation name"));
		}
	if (iIsRemote)
		{
		_LIT(KRconsPattern, "rcons*");
		_LIT(KVt100Pattern, "vt100*");
		Complete(!((name.MatchF(KRconsPattern) == 0) || (name.MatchF(KVt100Pattern) == 0)));
		}
	else if (iIsNull)
		{
		_LIT(KNullPattern, "nullcons*");
		Complete(!(name.MatchF(KNullPattern) == 0));
		}
	else if (iVerbose)
		{
		TSize size;
		LeaveIfErr(Stdout().GetScreenSize(size), _L("Couldn't get screen size"));
		Printf(_L("Name: %S\r\n"), &name);
		Printf(_L("Size: %dx%d\r\n"), size.iWidth, size.iHeight);
		}
	else
		{
		Write(name);
		}
	CleanupStack::PopAndDestroy(&console);
	}

void CCmdConsole::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptIsRemote, "remote");
	_LIT(KOptIsNull, "null");

	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	aOptions.AppendBoolL(iIsRemote, KOptIsRemote);
	aOptions.AppendBoolL(iIsNull, KOptIsNull);
	}


//
// CCmdPcons.
//

CCommandBase* CCmdPcons::NewLC()
	{
	CCmdPcons* self = new(ELeave) CCmdPcons();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdPcons::~CCmdPcons()
	{
	delete iName;
	delete iCommand;
	delete iCommandArgs;
	}

CCmdPcons::CCmdPcons()
	{
	}

const TDesC& CCmdPcons::Name() const
	{
	_LIT(KName, "pcons");
	return KName;
	}

_LIT(KNone, "-");
_LIT(KDefaultCommand, "fshell");
_LIT(KCurrentConsMarker, "(*)");
_LIT(KNew, "new");
_LIT(KExisting, "existing");

void CCmdPcons::DoRunL()
	{
	RIoPersistentConsole pcons;
	CleanupClosePushL(pcons);
	if ((iOperation != ENew) && (iOperation != EStart) && iCommand)
		{
		TPtrC operation(iArguments.AsString(&iOperation));
		PrintWarning(_L("'command' argument being ignored for operation %S"), &operation);
		}
	
	switch (iOperation)
		{
	case EList:
		{
		RIoConsole thisCons;
		LeaveIfErr(thisCons.Open(IoSession(), Stdout()), _L("Couldn't open handle to current console"));
		CleanupClosePushL(thisCons);

		const TDesC& matchStr = iName ? *iName : KMatchAll();
		TInt foundHandle;
		TName foundName;
		TBool foundCurrent = EFalse;
		
		CTextBuffer* buf = CTextBuffer::NewLC(0x20);
		_LIT(KColumnHeadings, "Name\tCreator\tReader\tWriter\r\n");
		buf->AppendL(KColumnHeadings);
		TInt err = IoSession().FindFirstHandle(RIoHandle::EConsole, matchStr, foundHandle, foundName);
		TInt count = 0;
		while (err==KErrNone)
			{
			RIoPersistentConsole pcons;
			pcons.OpenFoundHandleL(IoSession(), foundHandle);
			CleanupClosePushL(pcons);
			if (pcons.IsTypeL(RIoHandle::EPersistentConsole))
				{
				count++;
				buf->AppendL(foundName);
				if (pcons.EqualsL(thisCons))
					{
					buf->AppendL(KCurrentConsMarker);
					foundCurrent = ETrue;
					}
				buf->AppendL(KTab);
				
					{
					TThreadId creatorId = pcons.GetCreatorL();
					RThread creator;
					if (creator.Open(creatorId)==KErrNone)
						{
						TFullName name;
						LtkUtils::GetFriendlyThreadName(creator, name);
						creator.Close();
						buf->AppendL(name);
						buf->AppendL(KTab);
						}
					else
						{
						buf->AppendFormatL(_L("(%Ld)\t"), creatorId.Id());
						}
					}
				
					{
					TName readerName;
					TName writerName;
					pcons.GetAttachedReaderAndWriterNamesL(readerName, writerName);
					
					buf->AppendL(readerName.Length() ? readerName : KNone());
					buf->AppendL(KTab);
					buf->AppendL(writerName.Length() ? writerName : KNone());
					buf->AppendL(KNewLine);
					}
				}			
			CleanupStack::PopAndDestroy(&pcons);
			err = IoSession().FindNextHandle(foundHandle, foundName);
			}
		if (err!=KErrNotFound) User::LeaveIfError(err);
		if (count)
			{
			CTextFormatter* tf = CTextFormatter::NewLC(Stdout());
			// if this fails, it's probably just because the console is too narrow; just print it
			// untabulated in this case.
			TRAP(err, tf->TabulateL(0, 2, buf->Descriptor()));
			if (err==KErrNone)
				{
				Write(tf->Descriptor());
				}
			else
				{
				Write(buf->Descriptor());
				}
			CleanupStack::PopAndDestroy(tf);
			}
		if (foundCurrent)
			{
			Write(KCurrentConsMarker);
			Write(_L(" indicates current console"));
			}
		CleanupStack::PopAndDestroy(2, &thisCons);
		break;
		}
	case EStart:
		{
		if (!iName)
			{
			PrintError(KErrArgument, _L("A persistent console name to start to must be specified"));
			DisplayHelp();
			break;
			}
		TInt err = pcons.OpenByName(IoSession(), *iName);
		const TDesC* newOrExisting;
		if (err==KErrNone)
			{
			newOrExisting = &KExisting;
			if (iCommand)
				{
				PrintWarning(_L("Ignoring 'command' argument as console already exists."));
				}
			}
		else
			{
			newOrExisting = &KNew;
			CreateL(pcons);
			}
		AttachL(pcons, *newOrExisting, ETrue);
		break;
		}
	case EConnect:
		{
		if (!iName)
			{
			PrintError(KErrArgument, _L("A persistent console name to connect to must be specified"));
			DisplayHelp();
			break;
			}
		LeaveIfErr(pcons.OpenByName(IoSession(), *iName), _L("Couldn't open persistent console with name %S"), iName);
		AttachL(pcons, KExisting, EFalse);
		break;
		}
	case EDisconnect:
		{
		if (iName)
			{
			LeaveIfErr(pcons.OpenByName(IoSession(), *iName), _L("Couldn't open persistent console with name %S"), iName);
			}
		else
			{
			LeaveIfErr(pcons.Open(IoSession(), Stdout()), _L("Couldn't open handle to console"));
			if (!pcons.IsTypeL(RIoHandle::EPersistentConsole))
				{
				LeaveIfErr(KErrNotSupported, _L("Not connected to a persistent console"));
				}
			}
		pcons.DetachReader();
		pcons.DetachWriter();
		break;
		}
	case ENew:
		{
		if (!iName)
			{
			PrintError(KErrArgument, _L("A name for the new persistent console must be specified"));
			DisplayHelp();
			break;
			}
		CreateL(pcons);
		break;
		}
		}
	CleanupStack::PopAndDestroy(&pcons);
	}
	
void CCmdPcons::CreateL(RIoPersistentConsole& aPcons)
	{
	const TDesC& command(iCommand ? *iCommand : KDefaultCommand());
	const TDesC& args(iCommandArgs ? *iCommandArgs : KNullDesC());

	LeaveIfErr(aPcons.Create(IoSession(), *iName, *iName), _L("Could not create persistent console '%S'"), iName);

	RChildProcess proc;
	TRAPL(proc.CreateL(command, args, IoSession(), aPcons, &Env()), _L("Could not create process %S"), &command);
	
	// Wait for process to rendezvous before returning to close aPcons - we have to give the child time to connect to the console.
	TRequestStatus status;
	proc.Process().Rendezvous(status);
	proc.Detach();
	User::WaitForRequest(status);
	proc.Close();
	}
	
TInt CCmdPcons::DoAttach(RIoPersistentConsole& aPcons, RIoConsole& aNew, RIoPersistentConsole::TCloseBehaviour aOnClose)
	{
	TInt err = aPcons.AttachWriter(aNew, aOnClose);
	if (err==KErrNone)
		{
		err = aPcons.AttachReader(aNew, aOnClose);
		if (err!=KErrNone)
			{
			aPcons.DetachWriter();
			}
		}
	return err;
	}
	
void CCmdPcons::AttachL(RIoPersistentConsole& aPcons, const TDesC& aNewOrExisting, TBool aForce)
	{
	RIoConsole thisCons;
	LeaveIfErr(thisCons.Open(IoSession(), Stdout()), _L("Couldn't open handle to current console"));
	CleanupClosePushL(thisCons);
	
	TBool usingExistingConsole = !ConsoleImplementation();
	RIoPersistentConsole::TCloseBehaviour onClose = 
			usingExistingConsole ? 
			RIoPersistentConsole::EDetachOnHandleClose : 
			RIoPersistentConsole::EKeepAttachedOnHandleClose;
	
	if (usingExistingConsole)
		{
		Printf(_L("pcons: connecting to %S console %S.\r\n"), &aNewOrExisting, iName);
		}
	TInt err = DoAttach(aPcons, thisCons, onClose);
	if ((err == KErrInUse) && (aForce))
		{
		if (iVerbose) Printf(_L("Detaching existing connection from %S"), iName);
		aPcons.DetachReader();
		aPcons.DetachWriter();
		err = DoAttach(aPcons, thisCons, onClose);
		}
	LeaveIfErr(err, _L("Could not connect to persistent console %S"), iName);
	
	if (usingExistingConsole)
		{
		// we've not been a custom console implementation
		// so we need to wait until the persistent side is disconnected.
		TRequestStatus readerDetach;
		TRequestStatus writerDetach;
		aPcons.NotifyReaderDetach(readerDetach);
		aPcons.NotifyWriterDetach(writerDetach);
		User::WaitForRequest(readerDetach);
		if (iVerbose) Printf(_L("Persistent console reader detached; waiting for writer to detach\r\n"));
		User::WaitForRequest(writerDetach);		
		if (iVerbose) Printf(_L("Persistent console writer detached\r\n"));
		if (Stdout().GetCursorPosL().iX != 0) Printf(KNewLine);
		Printf(_L("pcons: disconnected from %S\r\n"), iName);
		}

	CleanupStack::PopAndDestroy(&thisCons);		
	}

void CCmdPcons::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KOperationName, "operation");
	_LIT(KNameName, "name");
	_LIT(KArgCommand, "command");
	_LIT(KCommandArgs, "command_args");

	aArguments.AppendEnumL((TInt&)iOperation, KOperationName);
	aArguments.AppendStringL(iName, KNameName);
	aArguments.AppendStringL(iCommand, KArgCommand);
	aArguments.AppendStringL(iCommandArgs, KCommandArgs);
	}

void CCmdPcons::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	}


//
// CCmdIoInfo.
//

CCommandBase* CCmdIoInfo::NewLC()
	{
	CCmdIoInfo* self = new(ELeave) CCmdIoInfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdIoInfo::~CCmdIoInfo()
	{
	delete iMatchString;
	}

CCmdIoInfo::CCmdIoInfo()
	{
	}

const TDesC& CCmdIoInfo::Name() const
	{
	_LIT(KName, "ioinfo");
	return KName;
	}

void CCmdIoInfo::DoRunL()
	{
	TInt foundHandle;				
	TName name;
	TInt err = IoSession().FindFirstHandle(iObjectType, iMatchString ? *iMatchString : KMatchAll(), foundHandle, name);
	while (err==KErrNone)
		{
		Write(name);
		Write(KNewLine);
		err = IoSession().FindNextHandle(foundHandle, name);
		}
	if (err!=KErrNotFound) User::LeaveIfError(err);
	}

void CCmdIoInfo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KObjectType, "object_type");
	aArguments.AppendEnumL((TInt&)iObjectType, KObjectType);
	}

void CCmdIoInfo::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KMatch, "match");
	aOptions.AppendStringL(iMatchString, KMatch);
	}


//
// CCmdReattach.
//

CCommandBase* CCmdReattach::NewLC()
	{
	CCmdReattach* self = new(ELeave) CCmdReattach();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdReattach::~CCmdReattach()
	{
	delete iJobSpec;
	delete iStdinEndPointName;
	delete iStdoutEndPointName;
	delete iStderrEndPointName;
	}

CCmdReattach::CCmdReattach() : CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdReattach::Name() const
	{
	_LIT(KName, "reattach");
	return KName;
	}

void CCmdReattach::OpenEndPointLC(RIoEndPoint& aEndPoint, const TDesC& aName)
	{
	TInt foundHandle;
	TName name;
	LeaveIfErr(IoSession().FindFirstHandle(RIoHandle::EEndPoint, aName, foundHandle, name), _L("Couldn't find and end point named \"%S\""), &aName);
	aEndPoint.OpenFoundHandleL(IoSession(), foundHandle);
	CleanupClosePushL(aEndPoint);
	}

void CCmdReattach::DoRunL()
	{
	TInt jobId = -1;
	ParseJobSpecL(*iJobSpec, jobId);
	RIoEndPoint stdinEndPoint;
	RIoEndPoint stdoutEndPoint;
	RIoEndPoint stderrEndPoint;
	if (iStdinEndPointName)
		{
		OpenEndPointLC(stdinEndPoint, *iStdinEndPointName);
		}
	else
		{
		static_cast<RIoNull&>(stdinEndPoint).CreateL(IoSession());
		CleanupClosePushL(stdinEndPoint);
		}
	TInt numCleanupPushes = 1;
	if (iStdoutEndPointName)
		{
		OpenEndPointLC(stdoutEndPoint, *iStdoutEndPointName);
		++numCleanupPushes;
		}
	else
		{
		stdoutEndPoint = stdinEndPoint;
		}
	if (iStderrEndPointName)
		{
		OpenEndPointLC(stderrEndPoint, *iStderrEndPointName);
		++numCleanupPushes;
		}
	else
		{
		stderrEndPoint = stdoutEndPoint;
		}

	gShell->ClaimJobsLockLC();
	CJob* job = gShell->Job(jobId);
	if (job)
		{
		TInt err = job->Reattach(stdinEndPoint, stdoutEndPoint, stderrEndPoint);
		CleanupStack::PopAndDestroy(); // jobs lock.
		if (err)
			{
			Complete(err, _L("Couldn't reattach job %S"), iJobSpec);
			}
		else
			{
			Complete();
			}
		}
	else
		{
		CleanupStack::PopAndDestroy(); // jobs lock.
		Complete(KErrNotFound, _L("Job %S not found"), iJobSpec);
		}

	CleanupStack::PopAndDestroy(numCleanupPushes);
	}

void CCmdReattach::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgJobSpec, "job");
	_LIT(KArgStdinEndPointName, "stdin");
	_LIT(KArgStdoutEndPointName, "stdout");
	_LIT(KArgStderrEndPointName, "stderr");

	aArguments.AppendStringL(iJobSpec, KArgJobSpec);
	aArguments.AppendStringL(iStdinEndPointName, KArgStdinEndPointName);
	aArguments.AppendStringL(iStdoutEndPointName, KArgStdoutEndPointName);
	aArguments.AppendStringL(iStderrEndPointName, KArgStderrEndPointName);
	}


//
// CCmdDisown.
//

CCommandBase* CCmdDisown::NewLC()
	{
	CCmdDisown* self = new(ELeave) CCmdDisown();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdDisown::~CCmdDisown()
	{
	delete iJobSpec;
	}

CCmdDisown::CCmdDisown() : CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdDisown::Name() const
	{
	_LIT(KName, "disown");
	return KName;
	}

void CCmdDisown::DoRunL()
	{
	TInt jobId = -1;
	ParseJobSpecL(*iJobSpec, jobId);
	gShell->ClaimJobsLockLC();
	TInt err = gShell->DisownJob(jobId);
	CleanupStack::PopAndDestroy(); // jobs lock.
	if (err)
		{
		Complete(err, _L("Couldn't disown job %S"), iJobSpec);
		}
	else
		{
		Complete();
		}
	}

void CCmdDisown::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgJobSpec, "job_spec");
	aArguments.AppendStringL(iJobSpec, KArgJobSpec);
	}


//
// CCmdDebugPort
//

CCommandBase* CCmdDebugPort::NewLC()
	{
	CCmdDebugPort* self = new(ELeave) CCmdDebugPort();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdDebugPort::~CCmdDebugPort()
	{
	}

CCmdDebugPort::CCmdDebugPort()
	{
	}

const TDesC& CCmdDebugPort::Name() const
	{
	_LIT(KName, "debugport");
	return KName;
	}

void CCmdDebugPort::DoRunL()
	{
	if (iArguments.IsPresent(0))
		{
		if (iForce)
			{
			LoadMemoryAccessL();
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
			LeaveIfErr(iMemAccess.SetDebugPort(iPort), _L("Couldn't set debug port"));
#endif
			}
		else
			{
			LeaveIfErr(HAL::Set(HAL::EDebugPort, iPort), _L("Couldn't set debug port (Try --force)."));
			}
		}
	else
		{
		TInt debugport = 0;
		TInt err = HAL::Get(HAL::EDebugPort, debugport);
		LeaveIfErr(err, _L("Couldn't read debug port."));
		Printf(_L("Debug port: %d\r\n"), debugport);
		}
	}

void CCmdDebugPort::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgPort, "port");
	aArguments.AppendIntL(iPort, KArgPort);
	}

void CCmdDebugPort::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptForce, "force");
	aOptions.AppendBoolL(iForce, KOptForce);
	}


//
// CCmdRom
//

CCommandBase* CCmdRom::NewLC()
	{
	CCmdRom* self = new(ELeave) CCmdRom();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRom::~CCmdRom()
	{
	delete iBuffer;
	}

CCmdRom::CCmdRom()
	{
	}

const TDesC& CCmdRom::Name() const
	{
	_LIT(KName, "rom");
	return KName;
	}

void CCmdRom::PrintIntL(TInt32 aInt32, const TDesC& aLabel, const TDesC& aDescription)
	{
	_LIT(KFormat, "%S:\t%d (0x%08x)\t%S\r\n");
	iBuffer->AppendFormatL(KFormat, &aLabel, aInt32, aInt32, &aDescription);
	}

void CCmdRom::PrintIntL(TInt64 aInt64, const TDesC& aLabel, const TDesC& aDescription)
	{
	_LIT(KFormat, "%S:\t%Ld\t%S\r\n");
	iBuffer->AppendFormatL(KFormat, &aLabel, aInt64, &aDescription);
	}

void CCmdRom::PrintUintL(TUint32 aUint32, const TDesC& aLabel, const TDesC& aDescription)
	{
	_LIT(KFormat, "%S:\t%u (0x%08x)\t%S\r\n");
	iBuffer->AppendFormatL(KFormat, &aLabel, aUint32, aUint32, &aDescription);
	}

void CCmdRom::PrintUintL(TUint64 aUint64, const TDesC& aLabel, const TDesC& aDescription)
	{
	_LIT(KFormat, "%S:\t%Lu\t%S\r\n");
	iBuffer->AppendFormatL(KFormat, &aLabel, aUint64, &aDescription);
	}

void CCmdRom::PrintAddressL(TLinAddr aAddress, const TDesC& aLabel, const TDesC& aDescription)
	{
	_LIT(KFormat, "%S:\t0x%08x\t%S\r\n");
	iBuffer->AppendFormatL(KFormat, &aLabel, aAddress, &aDescription, &aDescription);
	}

void CCmdRom::PrintSizeL(TInt32 aSize, const TDesC& aLabel, const TDesC& aDescription)
	{
	if (iHumanReadable)
		{
		_LIT(KFormat1, "%S:\t");
		iBuffer->AppendFormatL(KFormat1, &aLabel);
		iBuffer->AppendHumanReadableSizeL((TInt)aSize, EUnaligned);
		_LIT(KFormat2, "\t%S\r\n");
		iBuffer->AppendFormatL(KFormat2, &aDescription);
		}
	else
		{
		PrintIntL(aSize, aLabel, aDescription);
		}
	}

void CCmdRom::PrintTimeL(TInt64 aTime, const TDesC& aLabel, const TDesC& aDescription)
	{
	if (iHumanReadable)
		{
		TTime time(aTime);
		TDateTime dateTime(time.DateTime());
		_LIT(KFormat, "%S:\t%+02d/%+02d/%+04d %+02d:%+02d:%+02d.%+06d\t%S\r\n");
		iBuffer->AppendFormatL(KFormat, &aLabel, dateTime.Day() + 1, dateTime.Month() + 1, dateTime.Year(), dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond(), &aDescription);
		}
	else
		{
		PrintIntL(aTime, aLabel, aDescription);
		}
	}

void CCmdRom::PrintVersionL(TVersion aVersion, const TDesC& aLabel, const TDesC& aDescription)
	{
	_LIT(KFormat, "%S:\t%d.%d (build %d)\t%S\r\n");
	iBuffer->AppendFormatL(KFormat, &aLabel, aVersion.iMajor, aVersion.iMinor, aVersion.iBuild, &aDescription);
	}

void CCmdRom::DoRunL()
	{
#ifdef __WINS__
	LeaveIfErr(KErrNotSupported, _L("This command is not supported on the emulator"));
#endif

	TRomHeader* header = (TRomHeader*)UserSvr::RomHeaderAddress();

	iBuffer = CTextBuffer::NewL(0x100);

	PrintVersionL(header->iVersion, _L("iVersion"), _L("The ROM version number."));
	PrintTimeL(header->iTime, _L("iTime"), _L("The time and date that the ROM image was built."));
	PrintSizeL(header->iRomSize, _L("iRomSize"), _L("The size of the ROM image. This includes the second section for sectioned ROMs."));
	PrintUintL((TUint32)header->iCheckSum, _L("iCheckSum"), _L("A computed constant that causes the 32-bit checksum of the image to equal the value specified by the \"romchecksum\" keyword."));
	PrintUintL(header->iKernelConfigFlags, _L("iKernelConfigFlags"), _L("Contains the flags that define the security options in force. See TKernelConfigFlags for details."));

	if (iVerbose)
		{
		PrintAddressL(header->iRestartVector, _L("iRestartVector"), _L("The restart vector"));
		PrintUintL(header->iTimeHi, _L("iTimeHi"), _L("The high order 32 bits of the ROM image build time and date."));
		PrintAddressL(header->iRomBase, _L("iRomBase"), _L("The virtual address of the base of the ROM."));
		PrintAddressL(header->iRomRootDirectoryList, _L("iRomRootDirectoryList"), _L("The virtual address of the TRomRootDirectoryList structure."));
		PrintAddressL(header->iKernDataAddress, _L("iKernDataAddress"), _L("The virtual address of kernel data, when in RAM."));
		PrintAddressL(header->iKernelLimit, _L("iKernelLimit"), _L("The virtual address of the top of the kernel region, when in RAM."));
		PrintAddressL(header->iPrimaryFile, _L("iPrimaryFile"), _L("The virtual address of the primarys ROM file image header."));
		PrintAddressL(header->iSecondaryFile, _L("iSecondaryFile"), _L("The virtual address of the secondarys ROM file image header."));
		PrintUintL(header->iHardware, _L("iHardware"), _L("A unique number identifying the hardware for which this ROM image has been built. Note that this is only used for testing purposes."));
		PrintIntL(header->iLanguage, _L("iLanguage"), _L("A bitmask identifying which of the languages are supported by the ROM image, as specified by the \"languages\" keyword. Note that this is only used for testing purposes."));
		PrintAddressL(header->iRomExceptionSearchTable, _L("iRomExceptionSearchTable"), _L("The virtual address of the TRomExceptionSearchTable structure."));
		PrintUintL(header->iRomHeaderSize, _L("iRomHeaderSize"), _L("Current size of ROM header (previously iUnused3). If value is 0x69966996 then header size is 0x100."));
		PrintAddressL(header->iRomSectionHeader, _L("iRomSectionHeader"), _L("The virtual address of the ROM section header, if the ROM is sectioned. See TRomSectionHeader for details."));
		PrintSizeL(header->iTotalSvDataSize, _L("iTotalSvDataSize"), _L("The total supervisor data size, i.e. the amount of memory mapped for the kernel data chunk."));
		PrintAddressL(header->iVariantFile, _L("iVariantFile"), _L("The virtual address of the TRomEntry for the Variant file. See TRomEntry for details."));
		PrintAddressL(header->iExtensionFile, _L("iExtensionFile"), _L("The virtual address of TRomEntry for the first extension file, if it exists. If there is more than one extension, then the next extension field in the extensions TRomImageHeader points to the TRomEntry for that next extension. See TRomEntry and TRomImageHeader for details."));
		PrintAddressL(header->iRelocInfo, _L("iRelocInfo"), _L("The old position of the kernel trace mask, see iTraceMask."));
		PrintUintL(header->iOldTraceMask, _L("iOldTraceMask"), _L("Current size of ROM header (previously iUnused3). If value is 0x69966996 then header size is 0x100."));
		PrintAddressL(header->iUserDataAddress, _L("iUserDataAddress"), _L("The virtual address of the user data area."));
		PrintSizeL(header->iTotalUserDataSize, _L("iTotalUserDataSize"), _L("The total size of the user data."));
		PrintUintL(header->iDebugPort, _L("iDebugPort"), _L("A value that is interpreted by the Bootstrap and kernel as the debug port to be used."));
		PrintUintL(header->iCompressionType, _L("iCompressionType"), _L("The type of compression used for the image. This is a UID, or 0 if there is no compression."));
		PrintSizeL(header->iCompressedSize, _L("iCompressedSize"), _L("The size of the image after compression."));
		PrintSizeL(header->iUncompressedSize, _L("iUncompressedSize"), _L("The size of the image before compression."));
		PrintUintL(header->iDisabledCapabilities[0], _L("iDisabledCapabilities[0]"), _L("Disabled capabilities."));
		PrintUintL(header->iDisabledCapabilities[1], _L("iDisabledCapabilities[1]"), _L("Disabled capabilities."));
		for (TInt i = 0; i < TRomHeader::KNumTraceMask; ++i)
			{
			TBuf<16> buf;
			_LIT(KFormat, "iTraceMask[%d]");
			buf.Format(KFormat, i);
			PrintUintL(header->iTraceMask[i], buf, _L("The initial value for one of the kernel trace masks."));
			}
#ifdef FSHELL_BTRACE_SUPPORT
		for (TInt i = 0; i < 9; ++i)
			{
			TBuf<32> buf;
			_LIT(KFormat, "iInitialBTraceFilter[%d]");
			buf.Format(KFormat, i);
			PrintUintL(header->iInitialBTraceFilter[i], buf, _L("Initial value for a btrace filter."));
			}
		PrintSizeL(header->iInitialBTraceBuffer, _L("iInitialBTraceBuffer"), _L("The initial btrace buffer size."));
		PrintUintL((TUint32)header->iInitialBTraceMode, _L("iInitialBTraceMode"), _L("The initial btrace mode."));
#endif
#ifndef FSHELL_9_1_SUPPORT
		PrintIntL((TInt32)header->iPageableRomStart, _L("iPageableRomStart"), _L("An offset, in bytes from ROM start, for the start of the pageable ROM area."));
		PrintSizeL(header->iPageableRomSize, _L("iPageableRomSize"), _L("The size of the pageable ROM area."));
		PrintIntL((TInt32)header->iRomPageIndex, _L("iRomPageIndex"), _L("An offset in bytes from ROM start, for the page index. (Array of SRomPageInfo objects.)"));
		PrintUintL((TUint32)header->iDemandPagingConfig.iMinPages, _L("iMinPages"), _L("The minimum number of demand pages."));
		PrintUintL((TUint32)header->iDemandPagingConfig.iMaxPages, _L("iMinPages"), _L("The maximum number of demand pages."));
		PrintUintL((TUint32)header->iDemandPagingConfig.iYoungOldRatio, _L("iYoungOldRatio"), _L("The young / old ration of demand pages."));
#endif
		}

	CTextFormatter* formatter = CTextFormatter::NewLC(Stdout());
	formatter->TabulateL(0, 2, iBuffer->Descriptor(), EWrapLastColumn);
	formatter->Write();
	CleanupStack::PopAndDestroy(formatter);
	}

void CCmdRom::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	aOptions.AppendBoolL(iHumanReadable, KOptHuman);
	}

CCommandBase* CCmdWhich::NewLC()
	{
	CCmdWhich* self = new(ELeave) CCmdWhich();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdWhich::~CCmdWhich()
	{
	delete iCommand;
	}

CCmdWhich::CCmdWhich()
	{
	}

const TDesC& CCmdWhich::Name() const
	{
	_LIT(KName, "which");
	return KName;
	}
	
void CCmdWhich::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCommand, "command");
	aArguments.AppendStringL(iCommand, KCommand);
	}
	
void CCmdWhich::DoRunL()
	{
	LtkUtils::RLtkBuf desc;
	CleanupClosePushL(desc);
	TRAPD(err, gShell->CommandFactory().GetCommandInfoL(*iCommand, desc));
	_LIT(KFmt, "%S: %S\r\n");
	_LIT(KFailed, "%S not a built-in or an exe in \\sys\\bin\r\n");
	if (err == KErrNotFound)
		{
		// Must be an external non-fshell exe
		TFindFile find(FsL());
		TInt err = find.FindByDir(*iCommand, _L("Y:\\sys\\bin\\")); // Specify Y: to prevent it from checking the session path first
		if (err == KErrNone)
			{
			Printf(KFmt, iCommand, &find.File());
			}
		else if (err == KErrPermissionDenied)
			{
			// FShell must be built without AllFiles - try just creating it and getting its path
			RProcess proc;
			err = proc.Create(*iCommand, KNullDesC);
			if (err == KErrNone)
				{
				TFileName path = proc.FileName();
				Printf(KFmt, iCommand, &path);
				proc.Kill(KErrCancel);
				proc.Close();
				}
			else
				{
				Printf(KFailed, iCommand);
				}
			}
		else
			{
			Printf(KFailed, iCommand);
			}
		}
	else
		{
		LeaveIfErr(err, _L("Error finding command %S"), iCommand);
		Printf(_L("%S: %S\r\n"), iCommand, &desc);
		}
	CleanupStack::PopAndDestroy(&desc);
	}

CCommandBase* CCmdTee::NewLC()
	{
	CCmdTee* self = new(ELeave) CCmdTee();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTee::~CCmdTee()
	{
	iFileNames.Close();
	for (TInt i = 0; i < iFiles.Count(); i++)
		{
		iFiles[i].Close();
		}
	iFiles.Close();
	}

CCmdTee::CCmdTee()
	{
	}

const TDesC& CCmdTee::Name() const
	{
	_LIT(KName, "tee");
	return KName;
	}

void CCmdTee::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendFileNameL(iFileNames, _L("file"));
	}

void CCmdTee::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iAppend, _L("append"));
	aOptions.AppendBoolL(iRdebug, _L("rdebug"));
	}
	
void CCmdTee::DoRunL()
	{
	TUint filemode = EFileWrite | EFileShareAny | EFileStream; // Allow someone else to read it while we're writing
	for (TInt i = 0; i < iFileNames.Count(); i++)
		{
		RFile f;
		TInt err = KErrNone;
		if (iAppend)
			{
			err = f.Open(FsL(), iFileNames[i], filemode); // First try append
			if (err) err = f.Create(FsL(), iFileNames[i], filemode); // Then just create
			TInt whatDoesThisArgumentForSeekEndDoDoesAnybodyKnow = 0;
			f.Seek(ESeekEnd, whatDoesThisArgumentForSeekEndDoDoesAnybodyKnow);
			}
		else
			{
			err = f.Replace(FsL(), iFileNames[i], filemode);
			}
		LeaveIfErr(err, _L("Couldn't open file %S"), &iFileNames[i]);
		iFiles.AppendL(f);
		}
	
	RBuf readBuf;
	CleanupClosePushL(readBuf);
	readBuf.CreateL(512);
	LtkUtils::RLtkBuf8 writeBuf;
	CleanupClosePushL(writeBuf);
	writeBuf.CreateL(readBuf.MaxSize() * 4);
	Stdin().SetReadModeL(RIoReadHandle::EOneOrMore);
	while (Stdin().Read(readBuf) == KErrNone)
		{
		writeBuf.CopyAsUtf8L(readBuf);
		for (TInt i = 0; i < iFiles.Count(); i++)
			{
			TInt err = iFiles[i].Write(writeBuf);
			if (err) PrintWarning(_L("Failed to write to file %d: %d"), i, err);
			}
		if (iRdebug) LtkUtils::RawPrint(writeBuf);
		Stdout().Write(readBuf);
		}
	
	CleanupStack::PopAndDestroy(2, &readBuf);
	}

CCommandBase* CCmdError::NewLC()
	{
	CCmdError* self = new(ELeave) CCmdError();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdError::~CCmdError()
	{
	delete iErrorText;
	}

CCmdError::CCmdError()
	: iErrorVal(KErrAbort)
	{
	}

const TDesC& CCmdError::Name() const
	{
	_LIT(KName, "error");
	return KName;
	}

void CCmdError::DoRunL()
	{
	if (iErrorVal >= 0) LeaveIfErr(KErrArgument, _L("Non-negative value %d passed to error command"), iErrorVal);

	if (iErrorText)
		{
		LeaveIfErr(iErrorVal, *iErrorText);
		}
	else
		{
		User::Leave(iErrorVal);
		}
	}

void CCmdError::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendIntL(iErrorVal, _L("error-number"));
	aArguments.AppendStringL(iErrorText, _L("text"));
	}

	
//
// CCmdReboot
//

CCommandBase* CCmdReboot::NewLC()
	{
	CCmdReboot* self = new(ELeave) CCmdReboot();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdReboot::~CCmdReboot()
	{
	}

CCmdReboot::CCmdReboot()
	{
	}

const TDesC& CCmdReboot::Name() const
	{
	_LIT(KName, "reboot");
	return KName;
	}

void CCmdReboot::DoRunL()
	{
	if (!iArguments.IsPresent(0))
		{
		iReason = 0x40000000; // On most baseports this represents 'soft reboot'
		}

	LoadMemoryAccessL();
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	iMemAccess.Reboot(iReason);
#endif
	}

void CCmdReboot::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgReason, "reason");
	aArguments.AppendUintL(iReason, KArgReason);
	}

	
//
// CCmdForEach
//

CCommandBase* CCmdForEach::NewLC()
	{
	CCmdForEach* self = new(ELeave) CCmdForEach();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdForEach::~CCmdForEach()
	{
	iDir.Close();
	}

CCmdForEach::CCmdForEach()
	: iDirName(TFileName2::EDirectory)
	{
	}

void CCmdForEach::DoNextL(TBool aFirstTime)
	{
	if (!aFirstTime) Env().RemoveAll(); // Remove all only does locals

	delete iScriptData;
	iScriptData = NULL;
	delete iParser;
	iParser = NULL;

	TEntry entry;
	TInt err = iDir.Read(entry);
	if (err == KErrEof)
		{
		Complete(iLastError);
		}
	else
		{
		LeaveIfErr(err, _L("Couldn't read directory '%S'"), &iDirName);
		// Update the first element of the argument array with the full name of the file we just found.
		TFileName2 fileName(iDirName);
		fileName.AppendComponentL(entry.iName, TFileName2::EFile);

		RPointerArray<HBufC> extraArgs;
		LtkUtils::CleanupResetAndDestroyPushL(extraArgs);
		extraArgs.AppendL(fileName.AllocLC());
		CleanupStack::Pop(); // hbufc

		TIoHandleSet ioHandles(IoSession(), Stdin(), Stdout(), Stderr());
		TBool helpPrinted;
		iScriptData = CShell::ReadScriptL(iFileName, iArgs, Env(), FsL(), ioHandles, helpPrinted, &extraArgs);
		CleanupStack::PopAndDestroy(&extraArgs);
		if (helpPrinted)
			{
			Complete();
			}
		else
			{
			TUint mode = CParser::EExportLineNumbers;
			if (iKeepGoing)
				{
				mode |= CParser::EKeepGoing;
				}
			iParser = CParser::NewL(mode, *iScriptData, IoSession(), Stdin(), Stdout(), Stderr(), Env(), gShell->CommandFactory(), this);
			iParser->Start();
			}
		}
	}

const TDesC& CCmdForEach::Name() const
	{
	_LIT(KName, "foreach");
	return KName;
	}

void CCmdForEach::DoRunL()
	{
	LeaveIfFileNotFound(iDirName);
	if (!iDirName.IsDirL(FsL()))
		{
		LeaveIfErr(KErrArgument, _L("'%S' is not a directory"), &iDirName);
		}
	LeaveIfErr(iDir.Open(FsL(), iDirName, KEntryAttNormal | KEntryAttDir), _L("Couldn't open directory '%S'"), &iDirName);

	DoNextL(ETrue);
	}

void CCmdForEach::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgDirectoryName, "directory_name");
	aArguments.AppendFileNameL(iDirName, KArgDirectoryName);

	CCmdSource::ArgumentsL(aArguments);
	}

void CCmdForEach::HandleParserComplete(CParser&, const TError& aError)
	{
	TInt err = aError.Error();
	if (err < 0)
		{
		aError.Report();
		PrintError(err, _L("Aborted \"%S\" at line %d"), &aError.ScriptFileName(), aError.ScriptLineNumber());
		iLastError = err;
		}

	if ((err < 0) && !iKeepGoing)
		{
		Complete(iLastError);
		}
	else
		{
		TRAPD(err, DoNextL());
		if (err)
			{
			Complete(err);
			}
		}
	}
