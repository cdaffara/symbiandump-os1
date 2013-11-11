// fshell.cpp
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

#include <e32cons.h>
#include <f32file.h>
#include <fshell/common.mmh>
#include <fshell/ltkutils.h>
#include "string_utils.h"
#include "line_completer.h"
#include "command_factory.h"
#include "fshell.h"
#include "license.h"
#include "script_command.h"

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
#include <fshell/memoryaccess.h>
#endif

//
// Constants.
//

const TInt KNoForegroundJob = -1;


//
// Globals.
//

CShell* gShell;
TInt gExitValue(KErrNone);


//
// TError.
//

TError::TError(RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv)
	: iStderr(&aStderr), iEnv(&aEnv), iScriptLineNumber(-1)
	{
	if (iEnv->IsDefined(KScriptLine))
		{
		iScriptFileName = iEnv->GetAsDes(KScriptPath);
		iScriptFileName.Append(iEnv->GetAsDes(KScriptName));
		iScriptLineNumber = iEnv->GetAsInt(KScriptLine);
		}
	}

void TError::Set(const TError& aError)
	{
	if (!iSet)
		{
		iError = aError.iError;
		iReason = aError.iReason;
		iContext = aError.iContext;
		iScriptFileName = aError.iScriptFileName;
		iScriptLineNumber = aError.iScriptLineNumber;
		iSet = ETrue;
		}
	}

void TError::Set(TInt aError, TReason aReason)
	{
	if (!iSet)
		{
		iError = aError;
		iReason = aReason;
		iContext.Zero();
		iSet = ETrue;
		}
	LogIfRequired(aError, aReason, KNullDesC);
	}

void TError::Set(TInt aError, TReason aReason, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	if (!iSet)
		{
		iError = aError;
		iReason = aReason;
		Format(aFmt, list);
		iSet = ETrue;
		}
	LogListIfRequired(aError, aReason, aFmt, list);
	VA_END(list);
	}

void TError::Report() const
	{
	ASSERT(iError < 0);

	switch (iReason)
		{
		case ECommandError:
			{
			// Do nothing - assume an error message has been printed by CCommandBase.
			break;
			}
		case EFailedToConstructCommand:
			{
			FormatError(_L("Unable to create command \"%S\""), &iContext);
			break;
			}
		case EFailedToRunCommand:
			{
			FormatError(_L("Unable to run command \"%S\""), &iContext);
			break;
			}
		case EFailedToCreatePipeLine:
		case EFailedToSetChildErrorVar:
		case EFailedToSetScriptLineVar:
		case EUnknown:
		default:
			{
			PrintError();
			break;
			}
		}
	}

void TError::Format(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	FormatList(aFmt, list);
	VA_END(list);
	}

void TError::FormatList(TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	iContext.Zero();
	TOverflowTruncate overflow;
	iContext.AppendFormatList(aFmt, aList, &overflow);
	}

TInt TError::Error() const
	{
	return iError;
	}

TError::TReason TError::Reason() const
	{
	return iReason;
	}

const TDesC* TError::Context() const
	{
	return &iContext;
	}

const TDesC& TError::ScriptFileName() const
	{
	return iScriptFileName;
	}

TInt TError::ScriptLineNumber() const
	{
	return iScriptLineNumber;
	}

void TError::LogIfRequired(TInt aError, TReason aReason, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	LogListIfRequired(aError, aReason, aFmt, list);
	VA_END(list);
	}

void TError::LogListIfRequired(TInt aError, TReason aReason, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	if (Verbose())
		{
		IoUtils::TOverflowTruncate overflow;
		iScratch.Zero();
		iScratch.AppendFormat(_L("Error: %S (%d), Reason: %S (%d), Context: \""), &overflow, Stringify::Error(aError), aError, StringifyReason(aReason), aReason);
		iScratch.AppendFormatList(aFmt, aList, &overflow);
		iScratch.AppendFormat(_L("\"\r\n"), &overflow);
		iStderr->Write(iScratch);
		}
	}

#define CASE_RETURN_LIT(XXX) case XXX: { _LIT(_KLit, #XXX); return &_KLit; }
#define DEFAULT_RETURN_LIT(XXX) default: { _LIT(_KLit, XXX); return &_KLit; }

const TDesC* TError::StringifyReason(TReason aReason) const
	{
	switch (aReason)
		{
		CASE_RETURN_LIT(EUnknown);
		CASE_RETURN_LIT(EFailedToCreatePipeLine);
		CASE_RETURN_LIT(EFailedToConstructCommand);
		CASE_RETURN_LIT(EFailedToRunCommand);
		CASE_RETURN_LIT(EFailedToSetChildErrorVar);
		CASE_RETURN_LIT(EFailedToSetScriptLineVar);
		CASE_RETURN_LIT(ECommandError);
		DEFAULT_RETURN_LIT("*** REASON UNKNOWN ***");
		}
	}

TBool TError::Verbose() const
	{
	_LIT(KVerbose, "FSHELL_VERBOSE");
	return (iEnv->IsDefined(KVerbose));
	}

void TError::NewLine() const
	{
	if (iStderr->AttachedToConsole())
		{
		RIoConsoleWriteHandle consoleWriteHandle;
		consoleWriteHandle = *iStderr;
		TPoint cursorPos;
		consoleWriteHandle.GetCursorPos(cursorPos);
		if (cursorPos.iX != 0)
			{
			iStderr->Write(_L("\r\n"));
			}
		}
	}

void TError::PrintError() const
	{
	NewLine();
	IoUtils::TOverflowTruncate overflow;
	iScratch.Zero();
	iScratch.AppendFormat(_L("Error: %S (%d)\r\n"), &overflow, Stringify::Error(iError), iError);
	iStderr->Write(iScratch);
	}

void TError::FormatError(TRefByValue<const TDesC> aFmt, ...) const
	{
	NewLine();
	VA_LIST list;
	VA_START(list, aFmt);
	IoUtils::TOverflowTruncate overflow;
	iScratch = _L("Error: ");
	iScratch.AppendFormatList(aFmt, list, &overflow);
	VA_END(list);
	iScratch.AppendFormat(_L(" : %S (%d)\r\n"), Stringify::Error(iError), iError);
	iStderr->Write(iScratch);
	}

//
// CShell.
//


void Log(TRefByValue<const TDesC8> aFmt, ...)
	{
	RFs fs;
	TInt err = fs.Connect();
	if (err == KErrNone)
		{
		RFile file;
		err = file.Open(fs, _L("c:\\fshell.txt"), EFileWrite);
		if (err == KErrNotFound)
			{
			err = file.Replace(fs, _L("c:\\fshell.txt"), EFileWrite);
			}
		if (err == KErrNone)
			{
			TOverflowTruncate8 overflow;
			VA_LIST list;
			VA_START(list, aFmt);
			TBuf8<0x100> buf;
			buf.AppendFormatList(aFmt, list, &overflow);
			VA_END(list);
			buf.Append(_L("\r\n"));
			TInt pos(0);
			file.Seek(ESeekEnd, pos);
			file.Write(buf);
			file.Close();
			}
		fs.Close();
		}
	}


CShell* CShell::NewLC()
	{
	CShell* self = new(ELeave) CShell();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CShell::~CShell()
	{
	iJobsLock.Close();
	iJobs.ResetAndDestroy();
	delete iScriptArgs;
	delete iLineEditor;
	delete iLineCompleter;
	delete iConsole;
	delete iCommandFactory;
	delete iScriptData;
	delete iOneLiner;
	delete iParser;
	}

CCommandFactory& CShell::CommandFactory()
	{
	return *iCommandFactory;
	}

void CShell::ClaimJobsLockLC()
	{
	iJobsLock.Wait();
	CleanupStack::PushL(TCleanupItem(ReleaseJobsLock, const_cast<CShell*>(this)));
	}

void CShell::ReleaseJobsLock(TAny* aSelf)
	{
	static_cast<CShell*>(aSelf)->iJobsLock.Signal();
	}

TInt CShell::BringJobToForeground(TInt aJobId)
	{
	TInt ret = KErrNotFound;
	CJob* job = Job(aJobId);
	if (job != NULL)
		{
		Printf(_L("[%d] %S\r\n"), aJobId, job->Name());
		job->Resume();
		ret = job->BringToForeground();
		if (ret == KErrNone)
			{
			if (iForegroundJobId != KNoForegroundJob)
				{
				ForegroundJob().SendToBackground();
				}
			iForegroundJobId = aJobId;
			}
		}
	if (ret)
		{
		PrintError(ret, _L("Unable to bring job [%d] to foreground"), aJobId);
		}
	return ret;
	}

void CShell::SendJobToBackground(TInt aJobId)
	{
	CJob* job = Job(aJobId);
	if (job != NULL)
		{
		job->SendToBackground();
		job->Resume();
		Printf(_L("[%d] %S\r\n"), aJobId, job->Name());
		if (aJobId == iForegroundJobId)
			{
			iForegroundJobId = KNoForegroundJob;
			}
		}
	else
		{
		PrintError(KErrNotFound, _L("Unable to send job [%d] to background"), aJobId);
		}
	}

const RPointerArray<CJob>& CShell::Jobs() const
	{
	ASSERT(const_cast<RMutex&>(iJobsLock).IsHeld()); // IsHeld should be const but isn't
	return iJobs;
	}

CShell::CShell()
	: CCommandBase(EManualComplete | ENotifyStdinChanges | ESharableIoSession), iForegroundJobId(KNoForegroundJob), iWriterAdaptor(Stdout())
	{
	gShell = this;
	}

void CShell::ConstructL()
	{
	BaseConstructL();
	User::LeaveIfError(iJobsLock.CreateLocal());
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	// We call LoadDriver here so that memoryaccess gets a chance to start the thread creator tracker as early as possible
	// Possibly this isn't the best place to put it...
	RMemoryAccess::LoadDriver();
#endif

#ifdef FSHELL_CAP_ALL // If we don't have all caps we can't hack around with sys\bin directory
	// See if we were built/installed with FSHELL_REPLACE_ECONS and if so, do just that.
	// A bit inefficient we do it every time, but can't see a better way
	// The benefit of doing it like this is that it doesn't require us to remove other occurences of econs.dll (like econseik.dll) from the rombuild
	RLibrary iocons;
	TInt err = iocons.Load(_L("iocons.dll"));
	if (err == KErrNone)
		{
		// We don't include iocons.dll unless FSHELL_REPLACE_ECONS was specified at buildrom time
		CleanupClosePushL(iocons);
		RFs fs;
		CleanupClosePushL(fs);
		User::LeaveIfError(fs.Connect());
		_LIT(KPath, "%c:\\sys\\bin\\econs.dll");
		TBuf<32> econsPath;
		econsPath.Format(KPath, LtkUtils::GetSystemDrive());
		TEntry ioconsEntry;
		err = fs.Entry(iocons.FileName(), ioconsEntry);
		if (err == KErrNone)
			{
			TEntry econsEntry;
			if (fs.Entry(econsPath, econsEntry) == KErrNone && econsEntry.iSize == ioconsEntry.iSize)
				{
				// Then we've already copied iocons.dll to econs.dll, don't do it again
				}
			else
				{
				CFileMan* fm = CFileMan::NewL(fs);
				fm->Copy(iocons.FileName(), econsPath, CFileMan::ERecurse | CFileMan::EOverWrite);
				// Ignore the error - if it fails it shouldn't prevent fshell starting up
				delete fm;
				}
			}
		CleanupStack::PopAndDestroy(2, &iocons); // fs, iocons
		}
#endif
	}

void CShell::PrintPrompt()
	{
	const TDesC& pwd = Env().Pwd();
	// Truncate the prompt if it will take up more than half the screen, or half of KMaxLineLength
	TInt maxLen = KMaxLineLength / 2;
	TSize screenSize;
	TInt err = Stdout().GetScreenSize(screenSize);
	if (err == KErrNone && screenSize.iWidth > 3) maxLen = Min(maxLen, screenSize.iWidth / 2); // Nullcons has a width of 1, apparently
	TBuf<KMaxLineLength> prompt;
	if (pwd.Length() > maxLen)
		{
		TPtrC left(pwd.Left(3)); // For the driveletter:\ bit
		TPtrC right = pwd.Right(maxLen - 3);
		prompt.Format(_L("%S...%S>"), &left, &right);
		}
	else
		{
		prompt.Format(_L("%S>"), &pwd);
		}
	iLineEditor->Start(prompt);
	iLineEditor->ReinstatePromptAndUserInput();
	}

void CShell::PrintErr(TInt aError)
	{
	TPoint cursorPos;
	Stdout().GetCursorPos(cursorPos);
	if (cursorPos.iX != 0)
		{
		Stderr().Write(_L("\r\n"));
		}
	TBuf<128> buf;
	IoUtils::TOverflowTruncate overflow;
	buf.AppendFormat(_L("Error: %S (%d)\r\n"), &overflow, Stringify::Error(aError), aError);
	Stderr().Write(buf);
	}

void CShell::ProcessLineL(const TDesC& aLine)
	{
	TInt jobId = NextJobId();
	CJob* job = CJob::NewL(jobId, aLine, IoSession(), Stdin(), Stdout(), Stderr(), Env(), *iCommandFactory, this);
	CleanupStack::PushL(job);
	ClaimJobsLockLC();
	User::LeaveIfError(iJobs.Append(job));
	CleanupStack::PopAndDestroy(); // Jobs lock
	CleanupStack::Pop(job);
	iNextJobId = jobId;
	TBool isForeground(EFalse);
	job->Start(isForeground);
	if (isForeground)
		{
		iForegroundJobId = jobId;
		}
	else
		{
		PrintPrompt();
		}
	}

const TDesC& CShell::Name() const
	{
	_LIT(KName, "fshell");
	return KName;
	}

void CShell::DoRunL()
	{
#if defined (__WINS__) && !defined (EKA2)
	RThread().SetPriority(::EPriorityAbsoluteHigh);
#else
	RProcess().SetPriority(::EPriorityHigh);
#endif
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareAuto()); // Necessary because this handle is used by CCommandFactory and that can be used from the context of other threads (e.g. the "debug" command).
	iCommandFactory = CCommandFactory::NewL(iFs);

	if (iOneLiner)
		{
		// One line script mode.
		iParser = CParser::NewL(CParser::ENormal, *iOneLiner, IoSession(), Stdin(), Stdout(), Stderr(), Env(), *iCommandFactory, this);
		RProcess::Rendezvous(KErrNone);
		iParser->Start();
		}
	else if (iScriptName.Length() > 0)
		{
		TIoHandleSet ioHandles(IoSession(), Stdin(), Stdout(), Stderr());
		TBool helpPrinted;
		iScriptData = ReadScriptL(iScriptName, iScriptArgs, Env(), FsL(), ioHandles, helpPrinted);
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
			iParser = CParser::NewL(mode, *iScriptData, IoSession(), Stdin(), Stdout(), Stderr(), Env(), *iCommandFactory, this);
			RProcess::Rendezvous(KErrNone);
			iParser->Start();
			}
		}
	else
		{
		// Interactive mode.

#ifdef FSHELL_CORE_SUPPORT_LICENSE
		if (TLicense::CheckL(Stdin(), Stdout()) != TLicense::EValid)
			{
			Complete(KErrPermissionDenied);
			return;
			}
#endif
		
		User::LeaveIfError(Stdin().CaptureKey(CTRL('c'), 0, 0));
		User::LeaveIfError(Stdin().CaptureKey(CTRL('z'), 0, 0));
		iConsole = CConsoleReader::NewL(Stdin(), *this);
		iLineCompleter = CLineCompleter::NewL(iFs, *iCommandFactory, Env());
		_LIT(KFshellHistory, "c:\\system\\console\\shell\\history");
		iLineEditor = CLineEditor::NewL(iFs, iWriterAdaptor, *this, *iLineCompleter, KFshellHistory);
		RProcess::Rendezvous(KErrNone);
		PrintPrompt();
		}
	}

void CShell::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptExec, "exec");
	aOptions.AppendStringL(iOneLiner, KOptExec);
	_LIT(KOptKeepGoing, "keep-going");
	aOptions.AppendBoolL(iKeepGoing, KOptKeepGoing);
	}

void CShell::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgScriptName, "script_name");
	aArguments.AppendFileNameL(iScriptName, KArgScriptName);

	_LIT(KArgArgs, "script_args");
	aArguments.AppendStringL(iScriptArgs, KArgArgs);
	}
	
void CShell::StdinChange(TUint)
	{
	if (iLineEditor && !iIgnoreNextStdinChange)
		{
		iLineEditor->Redraw();
		}
	iIgnoreNextStdinChange = EFalse;
	}

TInt CShell::NextJobId()
	{
	TInt next = iNextJobId;
	++next;
	if (next == KNoForegroundJob)
		{
		next = 0;
		}
	return next;
	}

CJob& CShell::ForegroundJob()
	{
	ASSERT(iForegroundJobId != KNoForegroundJob);
	CJob* job = Job(iForegroundJobId);
	ASSERT(job != NULL);
	return *job;
	}

CJob* CShell::Job(TInt aId)
	{
	ASSERT(iJobsLock.IsHeld());
	const TInt numJobs = iJobs.Count();
	CJob* job = NULL;
	for (TInt i = 0; i < numJobs; ++i)
		{
		if (iJobs[i]->Id() == aId)
			{
			job = iJobs[i];
			break;
			}
		}
	return job;
	}

TInt CShell::DisownJob(TInt aId)
	{
	ASSERT(iJobsLock.IsHeld());
	const TInt numJobs = iJobs.Count();
	for (TInt i = 0; i < numJobs; ++i)
		{
		CJob* job = iJobs[i];
		if (job->Id() == aId)
			{
			if (job->IsDisownable())
				{
				job->Disown();
				delete job;
				iJobs.Remove(i);
				return KErrNone;
				}
			else
				{
				return KErrAccessDenied;
				}
			}
		}
	return KErrNotFound;
	}

HBufC* CShell::ReadScriptL(const TDesC& aScriptName, const TDesC* aArguments, IoUtils::CEnvironment& aEnv, RFs& aFs, TIoHandleSet& aIoHandles, TBool& aHelpPrinted, RPointerArray<HBufC>* aAdditionalPrefixArguments)
	{
	TFileName2 scriptName(aScriptName);

	// Check the scripts dirs in case it wasn't given as an absolute path (although iocli will have made it absolute relative to the pwd)
	_LIT(KScriptDir, "y:\\system\\console\\scripts\\");
	_LIT(KScriptSuffix, ".script");
	TInt scriptNameLen = scriptName.Length();
	if (!scriptName.Exists(aFs) && (scriptName.DriveAndPath().CompareF(aEnv.Pwd()) == 0))
		{
		TFindFile finder(aFs);
		TInt found = finder.FindByDir(scriptName.NameAndExt(), KScriptDir);
		if (found == KErrNotFound && scriptName.Right(KScriptSuffix().Length()).CompareF(KScriptSuffix) != 0)
			{
			// If it doesn't already end in .script, try that
			scriptName.Append(KScriptSuffix);
			found = finder.FindByDir(scriptName.NameAndExt(), KScriptDir);
			}
		if (found == KErrNone)
			{
			scriptName.Copy(finder.File());
			}
		else
			{
			// Put scriptname back the way it was
			scriptName.SetLength(scriptNameLen);
			} 
		}

	RFile scriptFile;
	TInt err;
	TInt retries = 5;
	do
		{
		err = scriptFile.Open(aFs, scriptName, EFileRead | EFileShareReadersOnly);
		if ((err == KErrNone) || (err != KErrInUse))
			{
			break;
			}
		User::After(500000);
		--retries;
		}
		while (retries >= 0);
	StaticLeaveIfErr(err, _L("Couldn't open script file %S"), &scriptName);
	CleanupClosePushL(scriptFile);
	TInt scriptFileSize;
	User::LeaveIfError(scriptFile.Size(scriptFileSize));
	HBufC8* scriptData = HBufC8::NewLC(scriptFileSize);
	TPtr8 scriptDataPtr(scriptData->Des());
	User::LeaveIfError(scriptFile.Read(scriptDataPtr));
	HBufC* decodedScriptData = LtkUtils::DecodeUtf8L(*scriptData);
	CleanupStack::PopAndDestroy(2, &scriptFile);
	CleanupStack::PushL(decodedScriptData);
	aEnv.SetLocalL(KScriptName);
	aEnv.SetLocalL(KScriptPath);
	aEnv.SetLocalL(KScriptLine);
	aEnv.SetLocalL(_L("0"));
	aEnv.SetL(KScriptName, scriptName.NameAndExt());
	aEnv.SetL(KScriptPath, scriptName.DriveAndPath());
	aEnv.SetL(_L("0"), scriptName);
	CScriptCommand* scriptCommand = CScriptCommand::NewLC(scriptName, aIoHandles);
	TRAP(err, scriptCommand->ParseCommandLineArgsL(aArguments ? *aArguments : KNullDesC(), aEnv, aAdditionalPrefixArguments));
	if (err == KErrArgument || scriptCommand->ShouldDisplayHelp())
		{
		// Need to display help
		if (scriptCommand->ShouldDisplayHelp())
			{
			err = KErrNone;
			}
		else if (err == KErrArgument)
			{
			scriptCommand->PrintError(KErrArgument, _L("Couldn't parse command line"));
			}
		scriptCommand->DisplayScriptHelpL();
		aHelpPrinted = ETrue;
		}
	else
		{
		aHelpPrinted = EFalse;
		}
	User::LeaveIfError(err); // Propagate error
	CleanupStack::PopAndDestroy(scriptCommand);
	CleanupStack::Pop(decodedScriptData);
	return decodedScriptData;
	}

void CShell::SetToForeground()
	{
	Stdin().SetToForeground();
	iIgnoreNextStdinChange = ETrue;
	}

void CShell::CoHandleKey(TUint aKeyCode, TUint aModifiers)
	{
	switch (aKeyCode)
		{
		case CTRL('c'):
			{
			if (iForegroundJobId == KNoForegroundJob)
				{
				PrintPrompt();
				}
			else
				{
				ClaimJobsLockLC();
				ForegroundJob().Kill();
				CleanupStack::PopAndDestroy(); // Jobs lock.
				}
			SetToForeground();
			break;
			}
		case CTRL('z'):
			{
			if (iForegroundJobId != KNoForegroundJob)
				{
				ClaimJobsLockLC();
				CJob& job = ForegroundJob();
				job.Suspend();
				CleanupStack::PopAndDestroy(); // Jobs lock.
				SetToForeground();
				Printf(_L("[%d] %S\t\t%S\r\n"), iForegroundJobId, ShStringify::JobStatus(job.Status()), job.Name());
				PrintPrompt();
				iForegroundJobId = KNoForegroundJob;
				}
			break;
			}
		default:
			{
			// Only handle the key if there's no foreground job. Note, it's possible for the foreground job
			// to still be pending, but have implicilty given keyboard focus back to fshell. For example:
			//
			// sleep& && echo hello
			//
			// The above job will not complete until the background sleep command is killed. During this period
			// fshell has implicitly taken keyboard focus because the hello command has released it. However
			// fshell's line editor hasn't been started and so isn't ready to handle input. Perhaps the line
			// editor should implicitly start in this situation? For the time being playing safe by ignoring
			// the keyboard input if there's a foreground job in progress.
			if (iForegroundJobId == KNoForegroundJob)
				{
				iLineEditor->HandleKey(aKeyCode, aModifiers);
				}
			}
		}
	}

void CShell::CoHandleError(TInt aError)
	{
	// Treat console errors as fatal (they're likely to be due to a remote console being closed).
	if (!IsComplete()) Complete(aError);
	}

void CShell::LeoHandleLine(const TDesC& aLine)
	{
	if (aLine.Length() > 0)
		{
		TRAPD(err, ProcessLineL(aLine));
		if (err)
			{
			PrintErr(err);
			PrintPrompt();
			}
		}
	else
		{
		PrintPrompt();
		}
	}

void CShell::HandleJobComplete(CJob& aJob, const TError& aError)
	{
	if (aError.Error() < 0)
		{
		aError.Report();
		}

	if (aJob.Id() == iForegroundJobId)
		{
		iForegroundJobId = KNoForegroundJob;
		PrintPrompt();
		}

#ifdef _DEBUG
	TInt pos = iJobs.Find(&aJob);
	ASSERT(pos >= 0);
#else
	TInt pos = iJobs.Find(&aJob);
#endif

	delete &aJob;
	iJobs.Remove(pos);
	}

void CShell::HandleParserComplete(CParser&, const TError& aError)
	{
	if ((aError.Error() < 0))
		{
		aError.Report();
		if (!iOneLiner)
			{
			PrintError(aError.Error(), _L("Aborted \"%S\" at line %d"), &aError.ScriptFileName(), aError.ScriptLineNumber());
			}
		SetErrorReported(ETrue);
		}

	gExitValue = aError.Error();
	Complete(aError.Error());
	}


//
// Main.
//

void MainL()
	{
	_LIT(KShellName, "fshell");
#ifdef EKA2
	User::RenameThread(KShellName);
#else
	RThread().Rename(KShellName);
#endif
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	IoUtils::CCommandBase* command = CShell::NewLC();
	command->RunCommandL();
	CleanupStack::PopAndDestroy(2, scheduler);
	if (gExitValue)
		{
		User::Leave(gExitValue);
		}
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup)
		{
		TRAP(err, MainL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return err;
	}
