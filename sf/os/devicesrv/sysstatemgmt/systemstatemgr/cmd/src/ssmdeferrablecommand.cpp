// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "ssmdeferrablecommand.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

CSsmDeferrableCommand::CSsmDeferrableCommand()
	: iCompletionStatus(KSsmCommandPending), iExecutionBehaviour(ESsmFireAndForget)
	{
	}

CSsmDeferrableCommand::CSsmDeferrableCommand(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour)
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE 
	: CSsmCommandBase(aSeverity, KDefaultCommandPriority),
#else
	: CSsmCommandBase(aSeverity),
#endif
	iCompletionStatus(KSsmCommandPending), iExecutionBehaviour(aExecutionBehaviour)
	{
	}

CSsmDeferrableCommand::CSsmDeferrableCommand(TCmdErrorSeverity aSeverity)
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	: CSsmCommandBase(aSeverity, KDefaultCommandPriority),
#else
	: CSsmCommandBase(aSeverity),
#endif
	iCompletionStatus(KSsmCommandPending), iExecutionBehaviour(ESsmFireAndForget)
	{
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
CSsmDeferrableCommand::CSsmDeferrableCommand(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUint16 aPriority)
	: CSsmCommandBase(aSeverity, aPriority), iCompletionStatus(KSsmCommandPending), iExecutionBehaviour(aExecutionBehaviour)
	{
	}

CSsmDeferrableCommand::CSsmDeferrableCommand(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	: CSsmCommandBase(aSeverity, aPriority), iCompletionStatus(KSsmCommandPending), iExecutionBehaviour(ESsmFireAndForget)
	{
	}

#endif
CSsmDeferrableCommand::~CSsmDeferrableCommand()
	{
	}

TBool CSsmDeferrableCommand::IsDeferred() const
	{
	return iExecutionBehaviour != ESsmWaitForSignal;
	}

TSsmExecutionBehaviour CSsmDeferrableCommand::ExecutionBehaviour() const
	{
	return iExecutionBehaviour;
	}

TInt CSsmDeferrableCommand::CompletionStatus() const
	{
	return iCompletionStatus;
	}

void CSsmDeferrableCommand::RegisterCompletionObserver(TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iCompletionObserver == NULL, PanicNow(KPanicDeferrableCommand, EInvalidDeferrebleCmdRegistration)); // guard against registering more than once
	aStatus = KRequestPending;
	iCompletionObserver = &aStatus; 

	if (iCompletionStatus != KSsmCommandPending)
		{
		CompleteRequest(*iCompletionObserver, KErrNone);
		iCompletionObserver = NULL;
		}
	}

void CSsmDeferrableCommand::RegisterCompletionObserverCancel()
	{
	if (iCompletionObserver)
		{
		// assume the request is still pending
		CompleteRequest(*iCompletionObserver, KErrCancel);
		iCompletionObserver = NULL;
		}
	}

TBool CSsmDeferrableCommand::EvaluateCondition(MSsmConditionalCallback* aCallback, CResourceFile* aResourceFile)
	{
	TBool ret = CSsmCommandBase::EvaluateCondition(aCallback, aResourceFile);
	if (ret == EFalse)
		{
		// Need to set iCompletionStatus to KErrNone as this command will never run
		CompleteDeferredExecuteRequest(KErrNone);
		}
	return ret;
	}

void CSsmDeferrableCommand::CompleteDeferredExecuteRequest(TInt aReason)
	{
	if (iCompletionObserver)
		{
		// notify observer of command completion
		CompleteRequest(*iCompletionObserver, KErrNone); // observer is expected to call CompletionCode() to get the error code 
		iCompletionObserver = NULL;
		}

	if (Severity()==ECmdIgnoreFailure && aReason!=KErrCancel)
		{
		iCompletionStatus = KErrNone;
		}
	else
		{
		iCompletionStatus = aReason;
		}
	DEBUGVERBOSE2A("iCompletionStatus set to %d", iCompletionStatus);
				
	if (iExecutionBehaviour != ESsmDeferredWaitForSignal)
		{
		CompleteExecuteRequest(aReason);
		}
	}

void CSsmDeferrableCommand::ValidateL()
	{
	if (iExecutionBehaviour != ESsmFireAndForget && 
		iExecutionBehaviour != ESsmWaitForSignal && 
		iExecutionBehaviour != ESsmDeferredWaitForSignal )
		{
		User::Leave(KErrArgument);
		}
	}
