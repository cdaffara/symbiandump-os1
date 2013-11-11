// pipe_line.cpp
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

#include <fshell/ioutils.h>
#include "error.h"
#include "pipe_line.h"
#include "console.h"
#include "command_factory.h"
#include "command_wrappers.h"


//
// RPipeSection.
//

RPipeSection::TRedirection::TRedirection()
	: iType(ENotRedirected), iFileName(NULL), iHandle(EUnknown)
	{
	}

void RPipeSection::TRedirection::SetFileNameL(const TDesC& aCwd, const TDesC& aName)
	{
	if ((aName == _L("NUL")) || (aName == _L("/dev/null")))
		{
		iType = ENull;
		}
	else
		{
		if (iFileName == NULL)
			{
			iFileName = new(ELeave) TFileName2();
			}
		*iFileName = aName;

		if (iFileName->Length() > 0)
			{
			const TUint16 firstChar = (*iFileName)[0];
			const TUint16 lastChar = (*iFileName)[iFileName->Length() - 1];
			const TUint16 singleQuote = '\'';
			const TUint16 doubleQuote = '"';
			if (((firstChar == singleQuote) && (lastChar == singleQuote)) || ((firstChar == doubleQuote) && (lastChar == doubleQuote)))
				{
				// The string is quoted - remove the quotes.
				*iFileName = iFileName->Mid(1, iFileName->Length() - 2);
				}
			}
		iFileName->MakeAbsoluteL(aCwd);
		}
	}

RPipeSection::RPipeSection()
	: iCommandArguments(5), iCommandArgumentsBuf(NULL)
	{
	}

void RPipeSection::Close()
	{
	iCommandArguments.Close();
	delete iStdinRedirection.iFileName;
	delete iStdoutRedirection.iFileName;
	delete iStderrRedirection.iFileName;
	delete iCommandArgumentsBuf;
	iCommandArgumentsBuf = NULL;
	}

HBufC* RPipeSection::GetCommandArguments() const
	{
	if (iCommandArgumentsBuf) return iCommandArgumentsBuf;

	TInt length = 0;
	const TInt numArgs = iCommandArguments.Count();
	for (TInt i = 0; i < numArgs; ++i)
		{
		length += iCommandArguments[i].Length() + 1;
		}
	iCommandArgumentsBuf = HBufC::New(length);
	if (iCommandArgumentsBuf)
		{
		TPtr ptr(iCommandArgumentsBuf->Des());
		for (TInt i = 0; i < numArgs; ++i)
			{
			ptr.Append(iCommandArguments[i]);
			if (i < (numArgs - 1))
				{
				ptr.Append(_L(" "));
				}
			}
		}
	return iCommandArgumentsBuf;
	}



//
// CPipeLine.
//

CPipeLine* CPipeLine::NewL(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, const RArray<RPipeSection>& aPipeSections, TBool aBackground, MPipeLineObserver* aObserver, TError& aErrorContext)
	{
	CPipeLine* self = NewLC(aIoSession, aStdin, aStdout, aStderr, aEnv, aFactory, aPipeSections, aBackground, aObserver, aErrorContext);
	CleanupStack::Pop(self);
	return self;
	}

CPipeLine* CPipeLine::NewLC(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, const RArray<RPipeSection>& aPipeSections, TBool aBackground, MPipeLineObserver* aObserver, TError& aErrorContext)
	{
	CPipeLine* self = new(ELeave) CPipeLine(aIoSession, aStdin, aStdout, aStderr, aEnv, aFactory, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aPipeSections, aBackground, aErrorContext);
	return self;
	}

CPipeLine::~CPipeLine()
	{
	const TInt count = iCommands.Count();
	for (TInt i = 0; i < count; ++i)
		{
		iCommands[i].Close();
		}
	iCommands.Close();
	delete iCompletionCallBack;
	}

void CPipeLine::Kill()
	{
	const TInt numCommands = iCommands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		MCommand* command = iCommands[i].iCommand;
		if (command)
			{
			command->CmndKill();
			}
		}
	}

TInt CPipeLine::Suspend()
	{
	TInt ret = KErrNone;
	const TInt numCommands = iCommands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		MCommand* command = iCommands[i].iCommand;
		if (command)
			{
			TInt err = command->CmndSuspend();
			if (err != KErrNone)
				{
				ret = err;
				}
			}
		}
	return ret;
	}

TInt CPipeLine::Resume()
	{
	TInt ret = KErrNone;
	const TInt numCommands = iCommands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		MCommand* command = iCommands[i].iCommand;
		if (command)
			{
			TInt err = command->CmndResume();
			if (err != KErrNone)
				{
				ret = err;
				}
			}
		}
	return ret;
	}

TInt CPipeLine::BringToForeground()
	{
	TInt ret = KErrNone;
	const TInt numCommands = iCommands.Count();
	if (numCommands > 0)
		{
		MCommand* command = iCommands[0].iCommand;
		if (command)
			{
			ret = command->CmndStdin().SetToForeground();
			}
		else
			{
			ret = KErrDied;
			}
		if (ret == KErrNone)
			{
			for (TInt i = 0; i < numCommands; ++i)
				{
				MCommand* command = iCommands[i].iCommand;
				if (command)
					{
					command->CmndForeground();
					}
				}
			}
		}
	else
		{
		ret = KErrNotFound;
		}
	return ret;
	}

void CPipeLine::SendToBackground()
	{
	const TInt numCommands = iCommands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		MCommand* command = iCommands[i].iCommand;
		if (command)
			{
			command->CmndBackground();
			}
		}
	}

TInt CPipeLine::Reattach(RIoEndPoint& aStdinEndPoint, RIoEndPoint& aStdoutEndPoint, RIoEndPoint& aStderrEndPoint)
	{
	// Go through the array of pipe-commands attaching them to the new end points where appropriate.
	// Note, some commands may have already completed, in which case they can be ignored.
	// Also, we have to be careful to only reattach I/O handles that weren't explicitly redirected
	// by the user, or by virtue of their position in the pipe-line.

	TInt err = KErrNone;
	const TInt numCommands = iCommands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		RPipedCommand& pipedCommand = iCommands[i];
		if (pipedCommand.iCommand)
			{
			if ((i == 0) && !pipedCommand.iStdinRedirected)
				{
				// The first command in the pipe-line whose stdin was not redirected.
				err = pipedCommand.iCommand->CmndReattachStdin(aStdinEndPoint);
				}
			if ((err == KErrNone) && ((i < (numCommands - 1)) || (numCommands == 1)) && !pipedCommand.iStderrRedirected)
				{
				// A middle command in the pipe-line whose stderr has not been redirected.
				err = pipedCommand.iCommand->CmndReattachStderr(aStderrEndPoint);
				}
			if ((err == KErrNone) && (i == (numCommands - 1)) && !pipedCommand.iStdoutRedirected)
				{
				// The last command in the pipe-line, whose stdout has not been redirected.
				err = pipedCommand.iCommand->CmndReattachStdout(aStdoutEndPoint);
				}
			}
		if (err)
			{
			break;
			}
		}
	return err;
	}

TBool CPipeLine::IsDisownable() const
	{
	const TInt numCommands = iCommands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		const RPipedCommand& pipedCommand = iCommands[i];
		if (pipedCommand.iCommand && !pipedCommand.iCommand->CmndIsDisownable())
			{
			return EFalse;
			}
		}
	return ETrue;
	}

void CPipeLine::Disown()
	{
	const TInt numCommands = iCommands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		RPipedCommand& pipedCommand = iCommands[i];
		if (pipedCommand.iCommand)
			{
			pipedCommand.iCommand->CmndDisown();
			}
		}
	}

CPipeLine::CPipeLine(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, MPipeLineObserver* aObserver)
	: iIoSession(aIoSession), iStdin(aStdin), iStdout(aStdout), iStderr(aStderr), iEnv(aEnv), iFactory(aFactory), iObserver(aObserver), iCompletionError(aStderr, aEnv)
	{
	}

void SetIoObjectName(RIoSession& aIoSession, TInt aObjHandle, TRefByValue<const TDesC> aFmt, ...)
	{
	TOverflowTruncate overflow;
	VA_LIST list;
	VA_START(list, aFmt);
	TFullName name;
	name.AppendFormatList(aFmt, list, &overflow);
	aIoSession.SetObjectName(aObjHandle, name);
	}

void CPipeLine::ConstructL(const RArray<RPipeSection>& aPipeSections, TBool aBackground, TError& aErrorContext)
	{
	// Run the pipe-line in the background even if we weren't explicitly asked to if fshell's
	// STDIN read handle isn't in the foreground. This prevents the pipe-line from stealing the
	// foreground from whatever has it (normally another instance of fshell running in interactive
	// mode).
	if (!aBackground && !iStdin.IsForeground())
		{
		aBackground = ETrue;
		}

	if (iObserver)
		{
		iCompletionCallBack = new(ELeave) CAsyncCallBack(TCallBack(CompletionCallBack, this), CActive::EPriorityStandard);
		}
	
	TInt i;
	const TInt numPipeSections = aPipeSections.Count();

	// Construct the command objects and IO handles.
	// Note, all the IO handles are duplicated from the shell's to ensure that the underlying console is correctly set.
	// Later redirections may cause some handles to be re-attached to different end points.
	for (i = 0; i < numPipeSections; ++i)
		{
		const RPipeSection& thisPipeSection = aPipeSections[i];
		User::LeaveIfError(iCommands.Append(RPipedCommand()));
		RPipedCommand& pipedCommand = iCommands[i];
		pipedCommand.iCommandName = thisPipeSection.iCommandName.AllocL();
		HBufC* args = thisPipeSection.GetCommandArguments();
		User::LeaveIfNull(args);
		pipedCommand.iCommand = iFactory.CreateCommandL(thisPipeSection.iCommandName, aErrorContext, *args);
		User::LeaveIfError(pipedCommand.iCommand->CmndStdin().Create(iIoSession));
		User::LeaveIfError(pipedCommand.iCommand->CmndStdin().Duplicate(iStdin));
		SetIoObjectName(iIoSession, pipedCommand.iCommand->CmndStdin().SubSessionHandle(), _L("%S_stdin"), &thisPipeSection.iFullName);
		User::LeaveIfError(pipedCommand.iCommand->CmndStdout().Create(iIoSession));
		User::LeaveIfError(pipedCommand.iCommand->CmndStdout().Duplicate(iStdout));
		SetIoObjectName(iIoSession, pipedCommand.iCommand->CmndStdout().SubSessionHandle(), _L("%S_stdout"), &thisPipeSection.iFullName);
		User::LeaveIfError(pipedCommand.iCommand->CmndStderr().Create(iIoSession));
		User::LeaveIfError(pipedCommand.iCommand->CmndStderr().Duplicate(iStderr));
		SetIoObjectName(iIoSession, pipedCommand.iCommand->CmndStderr().SubSessionHandle(), _L("%S_stderr"), &thisPipeSection.iFullName);
		}

	// Construct pipes.
	RArray<RIoPipe> pipes;
	CleanupClosePushL(pipes);
	for (i = 1; i < numPipeSections; ++i)
		{
		RIoPipe pipe;
		User::LeaveIfError(pipe.Create(iIoSession));
		CleanupClosePushL(pipe);
		SetIoObjectName(iIoSession, pipe.SubSessionHandle(), _L("%S ==> %S_pipe"), &aPipeSections[i - 1].iFullName, &aPipeSections[i].iFullName);
		User::LeaveIfError(pipes.Append(pipe));
		}

	RIoNull null;
	User::LeaveIfError(null.Create(iIoSession));
	CleanupClosePushL(null);
	SetIoObjectName(iIoSession, null.SubSessionHandle(), _L("null"));

	// Connect the pipe-line.
	for (i = 0; i < numPipeSections; ++i)
		{
		const RPipeSection& thisPipeSection = aPipeSections[i];
		RPipedCommand& thisPipedCommand = iCommands[i];

		switch (thisPipeSection.iStdinRedirection.iType)
			{
			case RPipeSection::TRedirection::ENotRedirected:
				{
				if (i == 0)
					{
					// This is the first pipe section. No wiring to do - already wired to the shell's stdin.
					if (!aBackground)
						{
						User::LeaveIfError(thisPipedCommand.iCommand->CmndStdin().SetToForeground());
						}
					}
				else
					{
					if (aPipeSections[i - 1].iStdoutRedirection.iType == RPipeSection::TRedirection::ENotRedirected)
						{
						// Wire intermediate pipe sections input up to the previous one's output via a pipe.
						User::LeaveIfError(pipes[i - 1].Attach(thisPipedCommand.iCommand->CmndStdin(), RIoEndPoint::EForeground));
						}
					else
						{
						// The previous pipe section's output has been redirected, so attach this pipe section's input to null.
						User::LeaveIfError(null.Attach(thisPipedCommand.iCommand->CmndStdin(), RIoEndPoint::EForeground));
						}
					}
				break;
				}
			case RPipeSection::TRedirection::EFile:
				{
				RIoFile file;
				User::LeaveIfError(file.Create(iIoSession, *thisPipeSection.iStdinRedirection.iFileName, RIoFile::ERead));
				CleanupClosePushL(file);
				SetIoObjectName(iIoSession, file.SubSessionHandle(), _L("file_%S"), thisPipeSection.iStdinRedirection.iFileName);
				User::LeaveIfError(file.Attach(thisPipedCommand.iCommand->CmndStdin(), RIoEndPoint::EForeground));
				CleanupStack::PopAndDestroy(&file);
				if (i > 0)
					{
					if (aPipeSections[i - 1].iStdoutRedirection.iType == RPipeSection::TRedirection::ENotRedirected)
						{
						// Re-wire the previous pipe section's output to null.
						User::LeaveIfError(null.Attach(iCommands[i - 1].iCommand->CmndStdout()));
						}
					}
				thisPipedCommand.iStdinRedirected = ETrue;
				break;
				}
			case RPipeSection::TRedirection::ENull:
				{
				User::LeaveIfError(null.Attach(thisPipedCommand.iCommand->CmndStdin(), RIoEndPoint::EForeground));
				if (i > 0)
					{
					if (aPipeSections[i - 1].iStdoutRedirection.iType == RPipeSection::TRedirection::ENotRedirected)
						{
						// Re-wire the previous pipe section's output to null.
						User::LeaveIfError(null.Attach(iCommands[i - 1].iCommand->CmndStdout()));
						}
					}
				thisPipedCommand.iStdinRedirected = ETrue;
				break;
				}
			case RPipeSection::TRedirection::EHandle:
			case RPipeSection::TRedirection::EFileAppend:
			default:
				{
				ASSERT(EFalse);
				break;
				}
			}

		switch (thisPipeSection.iStdoutRedirection.iType)
			{
			case RPipeSection::TRedirection::ENotRedirected:
				{
				if (i < (numPipeSections - 1))
					{
					// Attach this pipe section's output to the next one's input via a pipe.
					User::LeaveIfError(pipes[i].Attach(thisPipedCommand.iCommand->CmndStdout()));
					}
				break;
				}
			case RPipeSection::TRedirection::EFile:
			case RPipeSection::TRedirection::EFileAppend:
				{
				RIoFile file;
				User::LeaveIfError(file.Create(iIoSession, *thisPipeSection.iStdoutRedirection.iFileName, (thisPipeSection.iStdoutRedirection.iType == RPipeSection::TRedirection::EFile) ? RIoFile::EOverwrite : RIoFile::EAppend));
				CleanupClosePushL(file);
				SetIoObjectName(iIoSession, file.SubSessionHandle(), _L("file_%S"), thisPipeSection.iStdoutRedirection.iFileName);
				User::LeaveIfError(file.Attach(thisPipedCommand.iCommand->CmndStdout()));
				CleanupStack::PopAndDestroy(&file);
				thisPipedCommand.iStdoutRedirected = ETrue;
				break;
				}
			case RPipeSection::TRedirection::ENull:
				{
				User::LeaveIfError(null.Attach(thisPipedCommand.iCommand->CmndStdout()));
				thisPipedCommand.iStdoutRedirected = ETrue;
				break;
				}
			case RPipeSection::TRedirection::EHandle:
				{
				// Handle redirection of stdout to stderr after stderr has been wired up.
				thisPipedCommand.iStdoutRedirected = ETrue;
				break;
				}
			default:
				{
				ASSERT(EFalse);
				break;
				}
			}

		switch (thisPipeSection.iStderrRedirection.iType)
			{
			case RPipeSection::TRedirection::ENotRedirected:
				{
				// Wire error output directly to the shell's stderr.
				User::LeaveIfError(thisPipedCommand.iCommand->CmndStderr().Duplicate(iStderr));
				break;
				}
			case RPipeSection::TRedirection::EFile:
			case RPipeSection::TRedirection::EFileAppend:
				{
				RIoFile file;
				User::LeaveIfError(file.Create(iIoSession, *thisPipeSection.iStderrRedirection.iFileName, (thisPipeSection.iStderrRedirection.iType == RPipeSection::TRedirection::EFile) ? RIoFile::EOverwrite : RIoFile::EAppend));
				CleanupClosePushL(file);
				SetIoObjectName(iIoSession, file.SubSessionHandle(), _L("file_%S"), thisPipeSection.iStderrRedirection.iFileName);
				User::LeaveIfError(file.Attach(thisPipedCommand.iCommand->CmndStderr()));
				CleanupStack::PopAndDestroy(&file);
				thisPipedCommand.iStderrRedirected = ETrue;
				break;
				}
			case RPipeSection::TRedirection::ENull:
				{
				User::LeaveIfError(null.Attach(thisPipedCommand.iCommand->CmndStderr()));
				thisPipedCommand.iStderrRedirected = ETrue;
				break;
				}
			case RPipeSection::TRedirection::EHandle:
				{
				ASSERT(thisPipeSection.iStderrRedirection.iHandle == RPipeSection::TRedirection::EStdout);
				User::LeaveIfError(thisPipedCommand.iCommand->CmndStderr().Duplicate(thisPipedCommand.iCommand->CmndStdout()));
				thisPipedCommand.iStderrRedirected = ETrue;
				break;
				}
			default:
				{
				ASSERT(EFalse);
				break;
				}
			}

		if (thisPipeSection.iStdoutRedirection.iType == RPipeSection::TRedirection::EHandle)
			{
			ASSERT(thisPipeSection.iStdoutRedirection.iHandle == RPipeSection::TRedirection::EStderr);
			User::LeaveIfError(thisPipedCommand.iCommand->CmndStdout().Duplicate(thisPipedCommand.iCommand->CmndStderr()));
			}
		}

	// Pipe handles (if any) and null object (if needed) now held open by attached read a write handles.
	CleanupStack::PopAndDestroy(&null);
	if (numPipeSections > 1)
		{
		CleanupStack::PopAndDestroy(numPipeSections - 1); // The pipe handles.
		}
	CleanupStack::PopAndDestroy(&pipes);

	// Run the pipe-line.
	for (i = 0; i < numPipeSections; ++i)
		{
		const RPipeSection& thisPipeSection = aPipeSections[i];
		RPipedCommand& thisPipedCommand = iCommands[i];
		HBufC* args = thisPipeSection.GetCommandArguments();
		TInt err = KErrNoMemory;
		if (args)
			{
			err = thisPipedCommand.iCommand->CmndRun(*args, iEnv, *this, iIoSession);
			if ((err == KErrNone) && thisPipedCommand.iCommand)
				{
				aBackground ? thisPipedCommand.iCommand->CmndBackground() : thisPipedCommand.iCommand->CmndForeground();
				}
			}
		if (err)
			{
			Kill();
			aErrorContext.Set(err, TError::EFailedToRunCommand, thisPipeSection.iFullName);
			User::Leave(err);
			}
		}
	}

TInt CPipeLine::CompletionCallBack(TAny* aSelf)
	{
	CPipeLine* self = static_cast<CPipeLine*>(aSelf);
	self->iCompletionError.Set(self->iCommands[self->iCommands.Count() - 1].iCompletionError, TError::ECommandError);
	self->iObserver->HandlePipeLineComplete(*self, self->iCompletionError);
	return KErrNone;
	}

void CPipeLine::HandleCommandComplete(MCommand& aCommand, TInt aError)
	{
	TBool allNowComplete(ETrue);
	const TInt numCommands = iCommands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		RPipedCommand& thisPipedCommand = iCommands[i];
		if (thisPipedCommand.iCommand == &aCommand)
			{
			if (aCommand.CmndExitType() == EExitPanic)
				{
				_LIT(KFormat, "*** PANIC ***\r\n\tCommand:  \'%S\'\r\n\tCategory: \'%S\'\r\n\tReason:   %d\r\n");
				TBuf<256> buf;
				TOverflowTruncate overflow;
				TExitCategoryName category(aCommand.CmndExitCategory());
				buf.AppendFormat(KFormat, &overflow, thisPipedCommand.iCommandName, &category, aError);
				iStderr.Write(buf);
				if (aError >= 0)
					{
					// Panicking with KERN-EXEC 0 shouldn't equate to a completionerror of KErrNone!
					aError = KErrDied;
					}
				}
			else if (aCommand.CmndExitType() == EExitTerminate)
				{
				_LIT(KFormat, "Command '%S' terminated with reason %d\r\n");
				TBuf<256> buf;
				TOverflowTruncate overflow;
				buf.AppendFormat(KFormat, &overflow, thisPipedCommand.iCommandName, aError);
				iStderr.Write(buf);
					if (aError >= 0)
					{
					// Terminate 0 shouldn't equate to a completionError of KErrNone
					aError = KErrDied;
					}
				}
			thisPipedCommand.iCommand->CmndRelease();
			thisPipedCommand.iCommand = NULL;
			thisPipedCommand.iCompletionError = aError;
			}
		else if (thisPipedCommand.iCommand)
			{
			allNowComplete = EFalse;
			}
		}

	if (allNowComplete && iObserver)
		{
		iCompletionCallBack->CallBack();
		}
	}


//
// CPipeLine::RPipedCommand.
//

CPipeLine::RPipedCommand::RPipedCommand()
	: iCommand(NULL), iCompletionError(KErrNone), iCommandName(NULL), iStdinRedirected(EFalse), iStdoutRedirected(EFalse), iStderrRedirected(EFalse)
	{
	}

void CPipeLine::RPipedCommand::Close()
	{
	if (iCommand)
		{
		iCommand->CmndKill();
		if (iCommand)
			{
			iCommand->CmndRelease();
			iCommand = NULL;
			}
		}
	if (iCommandName)
		{
		delete iCommandName;
		}
	}

