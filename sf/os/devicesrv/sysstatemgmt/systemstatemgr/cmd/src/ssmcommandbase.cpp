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

#include "ssmcommandbase.h"
#include "ssmpanic.h"
#include "ssmdebug.h"

#include <ssm/ssmconditionalcallback.h>

CSsmCommandBase::CSsmCommandBase()
	:CActive(EPriorityStandard), iSeverity(ECmdIgnoreFailure)
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	, iPriority(KDefaultCommandPriority)
#endif
	{
	CActiveScheduler::Add(this);
	}

CSsmCommandBase::CSsmCommandBase(TCmdErrorSeverity aSeverity)
	: CActive(EPriorityStandard), iSeverity(aSeverity)
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	, iPriority (KDefaultCommandPriority)
#endif
	{
	CActiveScheduler::Add(this);
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
CSsmCommandBase::CSsmCommandBase(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	: CActive(EPriorityStandard), iSeverity(aSeverity), iPriority(aPriority)
	{
	CActiveScheduler::Add(this);
	}
#endif

/**
*/
CSsmCommandBase::~CSsmCommandBase()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iCmdResourceFileName.Close();
#endif
	}

void CSsmCommandBase::SetUtilProvider(CSsmCommandUtilProvider& aUtilProvider)
	{
	iUtilProvider = &aUtilProvider;
	}

TBool CSsmCommandBase::IsDeferred() const
	{
	return EFalse;
	}

TSsmExecutionBehaviour CSsmCommandBase::ExecutionBehaviour() const
	{
	return ESsmWaitForSignal;
	}

TCmdErrorSeverity CSsmCommandBase::Severity() const
	{
	return iSeverity;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
TUint16 CSsmCommandBase::Priority() const
	{
	return iPriority;
	}

void CSsmCommandBase::SetCommandResourceFileNameL(const TDesC& aFileName)
	{
	if (iCmdResourceFileName.MaxLength() < aFileName.Length())
		{
		iCmdResourceFileName.ReAllocL(aFileName.Length());		
		}
	iCmdResourceFileName.Copy(aFileName);
	}

const TDesC& CSsmCommandBase::GetCommandResourceFileName() const
	{
	return iCmdResourceFileName;
	}

TInt CSsmCommandBase::GetCommandResourceFileNameLength() const
	{
	return iCmdResourceFileName.Length();
	}
#endif

TInt CSsmCommandBase::CompletionStatus() const
	{
	__ASSERT_ALWAYS(0, PanicNow(KPanicSsmCommand, EIllegalMethodCallInClass));
	return KErrNotSupported;
	}

void CSsmCommandBase::RegisterCompletionObserver(TRequestStatus& /*aStatus*/)
	{
	__ASSERT_ALWAYS(0, PanicNow(KPanicSsmCommand, EIllegalMethodCallInClass));
	}

void CSsmCommandBase::RegisterCompletionObserverCancel()
	{
	__ASSERT_ALWAYS(0, PanicNow(KPanicSsmCommand, EIllegalMethodCallInClass));
	}

void CSsmCommandBase::CompleteExecuteRequest(TInt aReason)
	{
	if (iExecuteRequest && (iExecuteRequest->Int() == KRequestPending))
		{
		DEBUGVERBOSE3A("Severity %d Errorcode %d", Severity(), aReason);
		if (Severity()==ECmdIgnoreFailure && aReason!=KErrCancel)
			{
			aReason = KErrNone;
			}
		CompleteRequest(*iExecuteRequest, aReason);
		}
	iExecuteRequest = NULL;
	}

void CSsmCommandBase::CompleteRequest(TRequestStatus& aStatus, TInt aReason)
	{
	DEBUGPRINT2A("Completing command with errorcode %d", aReason);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, aReason);
	}

TBool CSsmCommandBase::EvaluateCondition(MSsmConditionalCallback* aCallback, CResourceFile* aResourceFile)
	{
	if(aCallback && aResourceFile && iConditionalResourceId)
		{
		// All the information is available to perform a condition evaluation
		TBool response = ETrue;
		TRAPD(err, response = aCallback->ConditionalCommandAllowedL(*aResourceFile, iConditionalResourceId));
		if(err != KErrNone)
			{
			// Failed to get any response information, so don't execute this command
			response = EFalse;
			}
		return response;
		}
	// Otherwise return true as conditional information has not been specified
	// If the conditional information isn't set then a command defaults to being
	// unconditional so therefore should always return ETrue in that situation.
	//
	// If no callback is provided or if no resource file is specified then
	// this command has not been created from a resource file so therefore
	// all commands are treated as unconditional, so again ETrue is returned.
	return ETrue;
	}

void CSsmCommandBase::SetConditionalInformation(TInt aConditionalInformation)
	{
	iConditionalResourceId = aConditionalInformation;
	}

TInt CSsmCommandBase::ConditionalInformation() const
	{
	return iConditionalResourceId;
	}
