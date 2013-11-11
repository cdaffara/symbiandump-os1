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

#include "cmdreqswpchange.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include "ssmcommandparameters.h"

/**
Used to create an instance of CCmdReqSwpChange class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdReqSwpChange.
*/
CCmdReqSwpChange* CCmdReqSwpChange::NewL(RReadStream& aReadStream)
	{
	CCmdReqSwpChange* self = new (ELeave) CCmdReqSwpChange();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdReqSwpChange class from given parameters.
This method is used by SsmCommandFactory to create a command.
 
@param aSeverity The severity of the command
@param aExecutionBehaviour The execution behaviour of the command
@param aSwpInfo System wide property settings
@param aPriority The priority of the command in the list
@return	A pointer to an object of type CCmdReqSwpChange.
*/
CCmdReqSwpChange* CCmdReqSwpChange::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo, const TUint16 aPriority)
	{
	CCmdReqSwpChange* self = new (ELeave) CCmdReqSwpChange(aSeverity, aExecutionBehaviour, aSwpInfo, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdReqSwpChange class from given parameters.
This method is used by SsmCommandFactory to create a command.
 
@param aSeverity The severity of the command
@param aExecutionBehaviour The execution behaviour of the command
@param aSwpInfo System wide property settings
@return	A pointer to an object of type CCmdReqSwpChange.
*/
CCmdReqSwpChange* CCmdReqSwpChange::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)
	{
	CCmdReqSwpChange* self = new (ELeave) CCmdReqSwpChange(aSeverity, aExecutionBehaviour, aSwpInfo);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdReqSwpChange class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdReqSwpChange.
*/	
CCmdReqSwpChange* CCmdReqSwpChange::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdReqSwpChange* self = new (ELeave) CCmdReqSwpChange();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdReqSwpChange class from CCmdReqSwpChange object
Must be used only by CLE
@param aCmdReqSwpChange CCmdReqSwpChange reference
@return A pointer to an object of type CCmdReqSwpChange.
*/
CCmdReqSwpChange* CCmdReqSwpChange::NewLC(const CCmdReqSwpChange& aCmdReqSwpChange)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdReqSwpChange* self = new (ELeave) CCmdReqSwpChange(aCmdReqSwpChange.Severity(),aCmdReqSwpChange.ExecutionBehaviour(),
                                                           aCmdReqSwpChange.SsmSwpInfo(),aCmdReqSwpChange.Priority());
#else
    CCmdReqSwpChange* self = new (ELeave) CCmdReqSwpChange(aCmdReqSwpChange.Severity(),aCmdReqSwpChange.ExecutionBehaviour(),
                                                               aCmdReqSwpChange.SsmSwpInfo());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdReqSwpChange);
    return self;
    }

void CCmdReqSwpChange::ConstructL(const CCmdReqSwpChange& aCmdReqSwpChange)
    {
    iConditionalResourceId = aCmdReqSwpChange.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdReqSwpChange.GetCommandResourceFileName());
        } 
#endif 
    }

/**
Destructor
*/	
CCmdReqSwpChange::~CCmdReqSwpChange()
	{
	Cancel();
	iSsmStateManager.Close();
	}

/**
Initialises the BIC's active object and initiates the command

@param aStatus 	the TRequestStatus of the active object calling this BIC
*/	
void CCmdReqSwpChange::Execute( TRequestStatus& aStatus )
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	TInt err = iSsmStateManager.Connect();
    if(err == KErrNone)
		{
        iSsmStateManager.RequestSwpChange(iSwpInfo, iStatus);
		}
    else
		{
        // pass the error connecting to RunL
        iStatus = KRequestPending;
        TRequestStatus* statusPtr = &iStatus;
        User::RequestComplete(statusPtr, err);
		}

	if (iExecutionBehaviour != ESsmWaitForSignal)
		{
		// complete the command request and initiate execution of next command
		CompleteExecuteRequest(KErrNone);
		}
	SetActive();
	}

/**
Releases resources associated with this BIC
*/	
void CCmdReqSwpChange::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/	
void CCmdReqSwpChange::ExecuteCancel()
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
TSsmCommandType CCmdReqSwpChange::Type() const
	{
	return (ESsmCmdReqSwProperty);
	}

/**
Returns the Version of the BIC
@return	The maximum supported version of the BIC
*/	
TInt CCmdReqSwpChange::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdReqSwPropertyVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdReqSwPropertyInitialVersion));
#endif
	}

/**
Configures the BIC using data contained in a ReadStream
@param aReadStream A read stream containing BIC data
*/
void CCmdReqSwpChange::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	const TUint key = aReadStream.ReadUint32L();
	const TInt value = aReadStream.ReadInt32L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif
	TSsmSwp swpinfo(key, value);

	iSwpInfo = swpinfo;
	}

/**
Externalises the configuration of the BIC
@param aWriteStream A write stream to write BIC data to
*/
void CCmdReqSwpChange::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	aWriteStream.WriteUint8L(iExecutionBehaviour);
	aWriteStream.WriteUint32L(iSwpInfo.Key());
	aWriteStream.WriteInt32L(iSwpInfo.Value());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
	}

/**
Completes the tasks of the BIC
*/
void CCmdReqSwpChange::RunL()
	{
	CompleteDeferredExecuteRequest(iStatus.Int());
	}

/**
Called during cancellation of the active BIC
*/
void CCmdReqSwpChange::DoCancel()
	{
	iSsmStateManager.RequestSwpChangeCancel();
	}

/**
default constructor.
*/
CCmdReqSwpChange::CCmdReqSwpChange()
	: iSwpInfo(0,0)
	{
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor.
@param aSeverity The severity of the command
@param aExecutionBehaviour The execution behaviour of the command
@param aSwpInfo System wide property settings
@param aPriority The priority of the command in the list
*/
CCmdReqSwpChange::CCmdReqSwpChange(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp aSwpInfo, const TUint16 aPriority)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour, aPriority), iSwpInfo(aSwpInfo)
	{
	}
#endif

/**
Overloaded constructor.
@param aSeverity The severity of the command
@param aExecutionBehaviour The execution behaviour of the command
@param aSwpInfo System wide property settings
*/
CCmdReqSwpChange::CCmdReqSwpChange(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp aSwpInfo)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour), iSwpInfo(aSwpInfo)
	{
	
	}

/**
Connect to RSsmStateManager and validate the commands data.
*/
void CCmdReqSwpChange::ConstructL()
	{
	ValidateL();
	}

/**
Constructs the object through read stream.
@param aReadStream A read stream containing BIC data
*/
void CCmdReqSwpChange::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	ConstructL();	// connect to RSsmStateManager and validate the data
	}

/**
Constructs an object from resource file.
@param aCommandParameters Object data from a resource file
*/
void CCmdReqSwpChange::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdReqSwpChange::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(reader.ReadUint8L());
	const TUint key = reader.ReadUint32L();
	const TInt value = reader.ReadInt32L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdReqSwPropertyInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif
	TSsmSwp swpInfo(key, value);
	iSwpInfo = swpInfo;

	ConstructL();	// connect to RSsmStateManager and validate the data
	}

