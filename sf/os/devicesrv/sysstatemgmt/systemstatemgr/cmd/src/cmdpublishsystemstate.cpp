// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ssm/ssmstate.h>
#include <domaindefs.h>

#include "cmdpublishsystemstate.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include "ssmcommandparameters.h"

/**
Used to create an instance of CCmdPublishSystemState class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdPublishSystemState.
*/
CCmdPublishSystemState* CCmdPublishSystemState::NewL(RReadStream& aReadStream)
	{
	CCmdPublishSystemState* self = new (ELeave) CCmdPublishSystemState();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdPublishSystemState class from given parameters.
This method is used by SsmCommandFactory to create a command.
 
@param aSeverity The severity of the command
@param aExecutionBehaviour The execution behaviour of the command
@param aInfo System state settings
@param aPriority The priority of the command in the list
@return	A pointer to an object of type CCmdPublishSystemState.
*/
CCmdPublishSystemState* CCmdPublishSystemState::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority)
	{
	CCmdPublishSystemState* self = new (ELeave) CCmdPublishSystemState(aSeverity, aExecutionBehaviour, aInfo, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdPublishSystemState class from given parameters.
This method is used by SsmCommandFactory to create a command.
 
@param aSeverity The severity of the command
@param aExecutionBehaviour The execution behaviour of the command
@param aInfo System state settings
@return	A pointer to an object of type CCmdPublishSystemState.
*/
CCmdPublishSystemState* CCmdPublishSystemState::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmPublishSystemStateInfo& aInfo)
	{
	CCmdPublishSystemState* self = new (ELeave) CCmdPublishSystemState(aSeverity, aExecutionBehaviour, aInfo);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdPublishSystemState class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdPublishSystemState.
*/
CCmdPublishSystemState* CCmdPublishSystemState::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdPublishSystemState* self = new (ELeave) CCmdPublishSystemState();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdPublishSystemState class from CCmdPublishSystemState object
Must be used only by CLE
@param aCmdPublishSystemState CCmdPublishSystemState reference 
@return A pointer to an object of type CCmdPublishSystemState.
*/
CCmdPublishSystemState* CCmdPublishSystemState::NewLC(const CCmdPublishSystemState& aCmdPublishSystemState)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdPublishSystemState* self = new (ELeave) CCmdPublishSystemState(aCmdPublishSystemState.Severity(),aCmdPublishSystemState.ExecutionBehaviour(),
                                                                       aCmdPublishSystemState.PublishSystemStateInfo(),aCmdPublishSystemState.Priority());
#else
    CCmdPublishSystemState* self = new (ELeave) CCmdPublishSystemState(aCmdPublishSystemState.Severity(),aCmdPublishSystemState.ExecutionBehaviour(),
                                                                           aCmdPublishSystemState.PublishSystemStateInfo());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdPublishSystemState);
    return self;
    }

void CCmdPublishSystemState::ConstructL(const CCmdPublishSystemState& aCmdPublishSystemState)
    {
    iConditionalResourceId = aCmdPublishSystemState.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdPublishSystemState.GetCommandResourceFileName());
        } 
#endif    
    }
/**
Destructor
*/	
CCmdPublishSystemState::~CCmdPublishSystemState()
	{
	ExecuteCancel();
	iDomainManager.Close();
	}

/**
Execute this command

@param aStatus 	the TRequestStatus of the active object calling this BIC
*/	
void CCmdPublishSystemState::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;
	iConnected = EFalse;

	if (iExecutionBehaviour != ESsmWaitForSignal)
		{
		// complete the execute request with KErrNone so that CLE can execute next command
		CompleteExecuteRequest(KErrNone);
		}
	// try to connect and request the transition
	TInt err = ConnectAndRequestTransition();	
	if (KErrNone != err)
		{
		// we need to retry in case of failures otherwise we have successfully requested for transition
		CompleteRequest(iStatus, err);
		}
	SetActive();
	}

/**
Releases resources associated with this BIC
*/	
void CCmdPublishSystemState::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/	
void CCmdPublishSystemState::ExecuteCancel()
	{
	//cancel the outstanding request
	Cancel();
	//and complete the request with KErrCancel
	CompleteDeferredExecuteRequest(KErrCancel);
	}

/**
Returns the Type of the BIC

@return	The Type of the BIC
*/	
TSsmCommandType CCmdPublishSystemState::Type() const
	{
	return ESsmCmdPublishSystemState;
	}

/**
Returns the Version of the BIC
 
@return	The maximum supported version of the BIC
*/	
TInt CCmdPublishSystemState::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdPublishSystemStateVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdPublishSystemStateInitialVersion));
#endif
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdPublishSystemState::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	TSsmState state;
	state.SetFromInt(aReadStream.ReadUint32L());
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
	const TDmTraverseDirection direction = static_cast<TDmTraverseDirection>(aReadStream.ReadInt32L());
#else
	const TSsmDmTraverseDirection direction = static_cast<TSsmDmTraverseDirection>(aReadStream.ReadInt32L());
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	const TInt16 retries = aReadStream.ReadInt16L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif
	TSsmPublishSystemStateInfo info;
	info.Set(state, direction, retries);
	iStateInfo = info;
	}

/**
Externalises the configuration of the BIC

@param aWriteStream A write stream to write BIC data to
*/
void CCmdPublishSystemState::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	aWriteStream.WriteUint8L(iExecutionBehaviour);
	aWriteStream.WriteInt32L(iStateInfo.State().Int());
	aWriteStream.WriteInt32L(iStateInfo.Direction());
	aWriteStream.WriteInt16L(iStateInfo.Retries());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
	}

/**
Completes the tasks of the BIC
*/
void CCmdPublishSystemState::RunL()
	{
	DEBUGPRINT5A("CCmdPublishSystemState::RunL: Error %d Retry attempt %d of %d Connected %d", iStatus.Int(), iAttempts+1, iStateInfo.Retries(), iConnected);
	if (iStatus.Int() != KErrNone)
		{
		if (++iAttempts >= iStateInfo.Retries())
			{
			SSMLOGLEAVE(iStatus.Int());	// exhausted retries
			}

		// try to connect and request the transition
		TInt err = ConnectAndRequestTransition();	
		if (KErrNone != err)
			{
			// we need to retry in case of failures otherwise we have successfully requested for transition
			CompleteRequest(iStatus, err);
			}
		SetActive();
		}
	else
		{
		CompleteDeferredExecuteRequest(KErrNone);		// success
		// Close domain manager connection
		iDomainManager.Close();
		iConnected = EFalse;
		}
	}

/**
Called to handle any cleanup if RunL leaves

@param aError 	The error to finish with
@return	KErrNone
*/
TInt CCmdPublishSystemState::RunError(TInt aError)
	{
	CompleteDeferredExecuteRequest(aError);
	return KErrNone;
	}

/**
Called during cancellation of the active BIC
*/
void CCmdPublishSystemState::DoCancel()
	{
	if(iConnected)
		{
		iDomainManager.CancelTransition();
		// Close domain manager connection
		iDomainManager.Close();
		iConnected = EFalse;
		}
	}

CCmdPublishSystemState::CCmdPublishSystemState()
	{
	}

void CCmdPublishSystemState::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	ValidateL();
	}

/**
@leave KErrNotSupported If command type or version of the command does not match.
*/
void CCmdPublishSystemState::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdPublishSystemState::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(reader.ReadUint8L());
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
	const TDmTraverseDirection direction = static_cast<TDmTraverseDirection>(reader.ReadInt8L());
#else
	const TSsmDmTraverseDirection direction = static_cast<TSsmDmTraverseDirection>(reader.ReadInt8L());
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	const TInt retries = reader.ReadInt16L();
	const TInt8 failurePolicy = reader.ReadInt8L();// just to make sure priority is handled properly
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdPublishSystemStateInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif
	const TSsmState state = aCommandParameters.StateL();
	TSsmPublishSystemStateInfo stateInfo;
	stateInfo.Set(state, direction, retries);
	iStateInfo = stateInfo;

	ValidateL();
	}

void CCmdPublishSystemState::ConstructL()
	{
	ValidateL();
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
CCmdPublishSystemState::CCmdPublishSystemState(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour, aPriority), iStateInfo(aInfo)
	{
	}
#endif

CCmdPublishSystemState::CCmdPublishSystemState(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmPublishSystemStateInfo& aInfo)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour), iStateInfo(aInfo)
	{
	}

/**
@leave KErrArgument If arguments are wrong.
*/
void CCmdPublishSystemState::ValidateL()
	{
	CSsmDeferrableCommand::ValidateL();
	SSMLOGLEAVEIFFALSE(iStateInfo.State().Int() <= 0xffffff, KErrArgument);	// only allowed lower 24 bits to use
	SSMLOGLEAVEIFFALSE(iStateInfo.Direction() >= ETraverseParentsFirst, KErrArgument);
	SSMLOGLEAVEIFFALSE(iStateInfo.Direction() <= ETraverseMax , KErrArgument);
	SSMLOGLEAVEIFFALSE(iStateInfo.Retries() >= 0, KErrArgument);
	}

/**
 @return KErrNone if connect is successful and RequestSystemTransition() is called, 
 otherwise return one of the system wide error codes
 */
TInt CCmdPublishSystemState::ConnectAndRequestTransition()
	{
	if (!iConnected)
		{
		TInt err = iDomainManager.Connect(KDmHierarchyIdStartup);
		if (KErrNone != err)
			{
			return err;
			}
		iConnected = ETrue;
		}

	const TDmDomainState state = static_cast<TDmDomainState>(iStateInfo.State().Int());
	DEBUGPRINT3A("Requesting state transition %04x %04x...", iStateInfo.State().MainState(), iStateInfo.State().SubState());
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
	iDomainManager.RequestSystemTransition(state, iStateInfo.Direction(), iStatus);
#else
	const TDmTraverseDirection direction = static_cast<TDmTraverseDirection>(iStateInfo.Direction());
	iDomainManager.RequestSystemTransition(state, direction, iStatus);
#endif

	return KErrNone;
	}

