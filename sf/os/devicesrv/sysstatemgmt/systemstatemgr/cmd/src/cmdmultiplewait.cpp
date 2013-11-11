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

#include <s32strm.h>
#include <barsread2.h>

#include "ssmcommandparameters.h"
#include "cmdmultiplewait.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

const TInt KMultipleWaitMilliToMicro = 1000;

/**
Used to create an instance of CCmdMultipleWait class from given parameters.
This method is used by SsmCommandFactory to create a command.

Creating multiple wait command with timeout value and 
Timer will be constructed only when command is executed through CLE

@param aTimeout Timeout specified for MULTIPLE_WAIT command
@return	A pointer to an object of type CCmdMultipleWait.
*/
CCmdMultipleWait* CCmdMultipleWait::NewL(TInt32 aTimeout)
	{
	CCmdMultipleWait* self = new (ELeave) CCmdMultipleWait(aTimeout);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdMultipleWait class from given parameters.
This method is used by SsmCommandFactory to create a command.

Creating multiple wait command with timeout value and Priority
Timer will be constructed only when command is executed through CLE

@param aTimeout Timeout specified for MULTIPLE_WAIT command
@param aPriority The priority of command in the list
@return	A pointer to an object of type CCmdMultipleWait.
*/
CCmdMultipleWait* CCmdMultipleWait::NewL(TInt32 aTimeout, const TUint16 aPriority)
	{
	CCmdMultipleWait* self = new (ELeave) CCmdMultipleWait(aTimeout, aPriority);
	return self;
	}
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE

/**
Used to create an instance of CCmdMultipleWait class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

Creating multiple wait command with ReadStream and DeferredCommands 
Timer will be constructed only when command is executed through CLE

@param aReadStream Read stream containing data through which object can be created
@param aDeferredCommands List of deferred commands
@return	A pointer to an object of type CCmdMultipleWait.
*/
CCmdMultipleWait* CCmdMultipleWait::NewL(RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredCommands)
	{
	CCmdMultipleWait* self = new (ELeave) CCmdMultipleWait();
	CleanupStack::PushL(self);
	self->ConstructL(aDeferredCommands, aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdMultipleWait class from resource.

Creating multiple wait command with CommandParameters and 
Timer will be constructed only when command is executed through CLE

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdMultipleWait.
*/
CCmdMultipleWait* CCmdMultipleWait::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdMultipleWait* self = new (ELeave) CCmdMultipleWait();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdMultipleWait class from CCmdMultipleWait object
Must be used only by CLE
@param aCmdMultipleWait CCmdMultipleWait reference 
@param aDeferredCommands TArray<MSsmCommand*> reference 
@return A pointer to an object of type CCmdMultipleWait.
*/
CCmdMultipleWait* CCmdMultipleWait::NewLC(const CCmdMultipleWait& aCmdMultipleWait, TArray<MSsmCommand*>& aDeferredCommands)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdMultipleWait* self = new (ELeave) CCmdMultipleWait(aCmdMultipleWait.Timeout(),aCmdMultipleWait.Priority());
#else
    CCmdMultipleWait* self = new (ELeave) CCmdMultipleWait(aCmdMultipleWait.Timeout());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdMultipleWait, aDeferredCommands);
    return self;
    }

void CCmdMultipleWait::ConstructL(const CCmdMultipleWait& aCmdMultipleWait, TArray<MSsmCommand*>& aDeferredCommands)
    {
    iConditionalResourceId = aCmdMultipleWait.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdMultipleWait.GetCommandResourceFileName());
        } 
#endif
    const TInt count = aDeferredCommands.Count();
    for (TInt i = 0; i < count; i++)
        {
        iDeferredCommands.AppendL(aDeferredCommands[i]);
        }
    ConstructL();   // Constructs the timer
    }

/**
Returns the version of the BIC
@return		The maximum supported version of the BIC
*/
TInt CCmdMultipleWait::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdMultipleWaitVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdMultipleWaitInitialVersion));
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE
	}

/**
Destructor
*/
CCmdMultipleWait::~CCmdMultipleWait()
	{
	Cancel();
	iDeferredCommands.Close();
	delete iMultipleWaitTimer;
	}

/**
Execute this command
@param aStatus 	The TRequestStatus which is completed when value is set either 
				with KErrNone or with error value.
*/
void CCmdMultipleWait::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	if (0 == iDeferredCommands.Count())
		{
		DEBUGPRINT1A("Deferred command list is empty, Multiple wait completing request with KErrNone");
		CompleteExecuteRequest(KErrNone);
		}
	else 
		{
		iCommandIndex = 0;	// start processing with first command in deferred command list
		MoveToNextState(EStateCheckCompletionStatus);
		}
	}

/**
Releases resources associated with this BIC
*/
void CCmdMultipleWait::Release()
	{
	delete this;
	}

/**
Cancels the BIC
*/
void CCmdMultipleWait::ExecuteCancel()
	{
	//cancel the outstanding request
	Cancel();
	//and complete the request with KErrCancel
	CompleteExecuteRequest(KErrCancel);
	}

/**
Returns the type of the BIC
@return Type of BIC
*/
TSsmCommandType CCmdMultipleWait::Type() const
	{
	return (ESsmCmdMultipleWait);
	}

/**
Externalises the configuration of the BIC
@param aWriteStream A write stream to write BIC data to
*/
void CCmdMultipleWait::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt32L(iTimeout);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE
	}

/**
Handles command's request completion event.
@panic EInvalidRunLAction if the RunL action is invalid
*/
void CCmdMultipleWait::RunL()
	{
	switch (iInternalState)
		{
		case EStateCheckCompletionStatus:
			{
			CheckCompletion();
			break;
			}
		case EStateTakeAction:
			{
			// pending command is completed so take the proper action
			TakeProperAction();
			// and process remaining deferred commands
			iCommandIndex++;
			MoveToNextState(EStateCheckCompletionStatus);
			break;
			}
		case EStateTimerExpired:
			{
			// multiple wait will not consider the severity in this case,
			// it will just complete the request with KErrTimedOut.
			if (iExecuteRequest && (iExecuteRequest->Int() == KRequestPending))
				{
				CompleteRequest(*iExecuteRequest, KErrTimedOut);
				}
			iExecuteRequest = NULL;

			break;
			}
		default:
			{
			PanicNow(KPanicCmdMultipleWait, EInvalidRunLAction);
			break;
			}
		}
	}

/**
Cancellation of an outstanding request.
*/
void CCmdMultipleWait::DoCancel()
	{
	Cleanup();
	}

/**
Complete the command request with aError. It gets called when RunL leaves.

@param aError Error code with which RunL has left.
@return KErrNone as error is handled in this function.
*/
TInt CCmdMultipleWait::RunError(TInt aError)
	{
	DEBUGPRINT2A("CCmdMultipleWait::RunError called with reason %d", aError);
	CompleteExecuteRequest(aError);
	return KErrNone;
	}

/**
Helper function to check the completion code of a deferred command and move to the next state.
*/
void CCmdMultipleWait::CheckCompletion()
	{
	TInt count = iDeferredCommands.Count();
	for ( ; iCommandIndex < count; iCommandIndex++)
		{
		DEBUGPRINT2A("Processing command %d", iCommandIndex);
		MSsmCommand* cmd = iDeferredCommands[iCommandIndex];
		// Severity of multiple wait should be same as the current command,
		iSeverity = cmd->Severity();
		if (cmd->CompletionStatus() == KSsmCommandPending)
			{
			// Register for the command completion and wait for command to complete
			DEBUGPRINT2A("Command %d is pending, Register for completion", iCommandIndex);
			SetActive();
			iInternalState = EStateTakeAction;
			cmd->RegisterCompletionObserver(iStatus);
			//start timer if not yet started
			if (iMultipleWaitTimer && !iMultipleWaitTimer->IsActive())
				{
				if (iTimeout > 0)
					{
					DEBUGPRINT2A("Timer started for %d milliseconds", iTimeout);
					iMultipleWaitTimer->StartTimer(iTimeout * KMultipleWaitMilliToMicro);
					}
				}
			break;
			}
		else 
			{
			TakeProperAction();
			}
		}
	// processed all deferred command so complete the request
	if (iCommandIndex == count)
		{
		DEBUGPRINT1A("All commands processed!!!");
		CompleteExecuteRequest(iCommandError);
		}
	}

/**
Helper function to take the proper action and move to the next state.
*/
void CCmdMultipleWait::TakeProperAction()
	{
	MSsmCommand* cmd = iDeferredCommands[iCommandIndex];
	iCommandError = cmd->CompletionStatus();
	DEBUGPRINT4A("Pending command %d completed with error %d and Severity is %d", iCommandIndex, iCommandError, cmd->Severity());
	if (iCommandError != KErrNone && iSeverity != ECmdIgnoreFailure)
		{
		// means command has failed which has severity other then ECmdIgnoreFailure
		//cancel all pending commands if any
		Cleanup();
		CompleteExecuteRequest(iCommandError);
		}
	else 
		{
		// ignore the error if command has ECmdIgnoreFailure severity
		iCommandError = KErrNone;
		}
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded CTOR
@param aTimeout Timeout for this command
@param aPriority The priority for the command
*/
CCmdMultipleWait::CCmdMultipleWait( TInt32 aTimeout, const TUint16 aPriority)
	:iTimeout(aTimeout)
	{
	iPriority = aPriority;
	}
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE

/**
Overloaded CTOR
@param aTimeout Timeout for this command
*/
CCmdMultipleWait::CCmdMultipleWait( TInt32 aTimeout)
	: iTimeout(aTimeout)
	{
	}

/**
Default CTOR
*/
CCmdMultipleWait::CCmdMultipleWait()
	{
	}

/**
Construct the command object with deferred commands and readstream.
@param aDeferredCommands List of deferred commands.
@param aReadStream Readstream containing data for this BIC.
*/
void CCmdMultipleWait::ConstructL(TArray<MSsmCommand*>&  aDeferredCommands, RReadStream& aReadStream)
	{
	TInt count = aDeferredCommands.Count();
	
	for (TInt i = 0; i < count; i++)
		{
		iDeferredCommands.AppendL(aDeferredCommands[i]);
		}
	InternalizeL(aReadStream);
	}

/**
Constructs an object from resource file.
@param aCommandParameters Object data from a resource file
*/
void CCmdMultipleWait::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdMultipleWait::MaxSupportedVersion()), KErrNotSupported);
	iTimeout = reader.ReadInt32L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdMultipleWaitInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif
	}

void CCmdMultipleWait::ConstructL()
	{
	iMultipleWaitTimer = CMultipleWaitTimer::NewL(*this);
	}
/**
Helper function to move to the next action.
@param aNextState Next action to be taken.
*/
void CCmdMultipleWait::MoveToNextState(TStates aNextState)
	{
	DEBUGPRINT2A("CCmdMultipleWait Moving to next state : %d", aNextState);
	iInternalState = aNextState;

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

/**
Helper function to handle the timeout.
*/
void CCmdMultipleWait::HandleTimeoutCallback()
	{
	DEBUGPRINT1A("Timer has expired");
	Cleanup();

	iInternalState = EStateTimerExpired;
	if (iMultipleWaitTimer)
		{
		iMultipleWaitTimer->Cancel();
		}
	}

void CCmdMultipleWait::Cleanup()
	{
	// cancel all pending deferred commands, 
	// starting from the current command in the list
	TInt count = iDeferredCommands.Count();
	if (iCommandIndex < count && iCommandIndex >= 0)
		{
		MSsmCommand* cmd = iDeferredCommands[iCommandIndex];
		if (cmd)
			{
			DEBUGPRINT1A("Cancel  completion observer");
			cmd->RegisterCompletionObserverCancel();
			}

		for (; iCommandIndex < count; iCommandIndex++)
			{
			cmd = iDeferredCommands[iCommandIndex];
			if (KSsmCommandPending == cmd->CompletionStatus())
				{
				DEBUGPRINT2A("Cancelling command %d ", iCommandIndex);
				iDeferredCommands[iCommandIndex]->ExecuteCancel();				
				}
			}		
		}
	}

/**
Configures the BIC using data contained in a ReadStream
@param aReadStream 	A read stream containing BIC data
*/
void CCmdMultipleWait::InternalizeL(RReadStream& aReadStream)
	{
	iTimeout = aReadStream.ReadInt32L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif
	}

CCmdMultipleWait::CMultipleWaitTimer* CCmdMultipleWait::CMultipleWaitTimer::NewL(CCmdMultipleWait& aMultipleWaitCmd)
	{
	CMultipleWaitTimer* self = new (ELeave) CMultipleWaitTimer(aMultipleWaitCmd);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCmdMultipleWait::CMultipleWaitTimer::~CMultipleWaitTimer()
	{
	Cancel();
	}

void CCmdMultipleWait::CMultipleWaitTimer::StartTimer(TInt32 aTimeout)
	{
	After(aTimeout);
	}

void CCmdMultipleWait::CMultipleWaitTimer::RunL()
	{
	iMultipleWaitCmd.HandleTimeoutCallback();
	}

void CCmdMultipleWait::CMultipleWaitTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

CCmdMultipleWait::CMultipleWaitTimer::CMultipleWaitTimer(CCmdMultipleWait& aMultipleWaitCmd)
	: CTimer(EPriorityStandard), iMultipleWaitCmd(aMultipleWaitCmd)
	{
	CActiveScheduler::Add(this);
	}

