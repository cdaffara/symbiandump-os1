// job.cpp
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

#include "job.h"
#include "command_factory.h"


//
// CJob.
//

CJob* CJob::NewL(TInt aId, const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, MJobObserver* aObserver)
	{
	CJob* self = new(ELeave) CJob(aId, aObserver, aStderr, aEnv);
	CleanupStack::PushL(self);
	self->ConstructL(aCommandLine, aIoSession, aStdin, aStdout, aStderr, aEnv, aFactory);
	CleanupStack::Pop(self);
	return self;
	}

CJob::~CJob()
	{
	delete iParser;
	delete iCommandLine;
	delete iCompletionCallBack;
	}

void CJob::Start(TBool& aIsForeground)
	{
	iParser->Start(aIsForeground);
	}

CJob::CJob(TInt aId, MJobObserver* aObserver, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv)
	: iId(aId), iStatus(EPending), iObserver(aObserver), iCompletionError(aStderr, aEnv)
	{
	}

void CJob::Kill()
	{
	iParser->Kill();
	}

void CJob::Suspend()
	{
	TInt err = iParser->Suspend();
	if (err == KErrNone)
		{
		iStatus = EStopped;
		}
	}

void CJob::Resume()
	{
	TInt err = iParser->Resume();
	if (err == KErrNone)
		{
		iStatus = ERunning;
		}
	}

TInt CJob::BringToForeground()
	{
	return iParser->BringToForeground();
	}

void CJob::SendToBackground()
	{
	iParser->SendToBackground();
	}

TInt CJob::Id() const
	{
	return iId;
	}

const TDesC* CJob::Name() const
	{
	return iCommandLine;
	}

CJob::TStatus CJob::Status() const
	{
	return iStatus;
	}

TInt CJob::Reattach(RIoEndPoint& aStdinEndPoint, RIoEndPoint& aStdoutEndPoint, RIoEndPoint& aStderrEndPoint)
	{
	return iParser->Reattach(aStdinEndPoint, aStdoutEndPoint, aStderrEndPoint);
	}

TBool CJob::IsDisownable() const
	{
	return iParser->IsDisownable();
	}

void CJob::Disown()
	{
	iParser->Disown();
	}

void CJob::ConstructL(const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory)
	{
	if (iObserver)
		{
		iCompletionCallBack = new(ELeave) CAsyncCallBack(TCallBack(CompletionCallBack, this), CActive::EPriorityStandard);
		}
	iCommandLine = aCommandLine.AllocL();
	iParser = CParser::NewL(CParser::ENormal, *iCommandLine, aIoSession, aStdin, aStdout, aStderr, aEnv, aFactory, this);
	iStatus = ERunning;
	}

TInt CJob::CompletionCallBack(TAny* aSelf)
	{
	CJob* self = static_cast<CJob*>(aSelf);
	self->iObserver->HandleJobComplete(*self, self->iCompletionError);
	return KErrNone;
	}

void CJob::HandleParserComplete(CParser& aParser, const TError& aError)
	{
	ASSERT(iParser == &aParser);
	iCompletionError = aError;
	if (iObserver)
		{
		iCompletionCallBack->CallBack();
		}
	}
