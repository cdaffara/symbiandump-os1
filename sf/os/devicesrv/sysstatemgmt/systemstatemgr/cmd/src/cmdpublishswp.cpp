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
#include <e32property.h>

#include "cmdpublishswp.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include "ssmcommandparameters.h"

/**
Used to create an instance of CCmdPublishSwp class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdPublishSwp.
*/
CCmdPublishSwp* CCmdPublishSwp::NewL(RReadStream& aReadStream)
	{
	CCmdPublishSwp* self = new (ELeave) CCmdPublishSwp();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdPublishSwp class from given parameters.
This method is used by SsmCommandFactory to create a command.
 
@param aSeverity The severity of the command
@param aSwpInfo System wide property settings
@param aPriority The priority of the command in the list
@return	A pointer to an object of type CCmdPublishSwp
*/
CCmdPublishSwp* CCmdPublishSwp::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo, const TUint16 aPriority)
	{
	CCmdPublishSwp* self = new (ELeave) CCmdPublishSwp(aSeverity, aExecutionBehaviour, aSwpInfo, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdPublishSwp class from given parameters.
This method is used by SsmCommandFactory to create a command.
 
@param aSeverity The severity of the command
@param aSwpInfo System wide property settings
@return	A pointer to an object of type CCmdPublishSwp
*/
CCmdPublishSwp* CCmdPublishSwp::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)
	{
	CCmdPublishSwp* self = new (ELeave) CCmdPublishSwp(aSeverity, aExecutionBehaviour, aSwpInfo);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;

	}

/**
Used to create an instance of CCmdPublishSwp class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdPublishSwp.
*/	
CCmdPublishSwp* CCmdPublishSwp::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdPublishSwp* self = new (ELeave) CCmdPublishSwp();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}
/**
Used to create an instance of CCmdPublishSwp class from CCmdPublishSwp object
Must be used only by CLE
@param aCmdPublishSwp CCmdPublishSwp reference 
@return A pointer to an object of type CCmdPublishSwp.
*/
CCmdPublishSwp* CCmdPublishSwp::NewLC(const CCmdPublishSwp& aCmdPublishSwp)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdPublishSwp* self = new (ELeave) CCmdPublishSwp(aCmdPublishSwp.Severity(),aCmdPublishSwp.ExecutionBehaviour(),
                                                       aCmdPublishSwp.SsmSwpInfo(),aCmdPublishSwp.Priority());
#else
    CCmdPublishSwp* self = new (ELeave) CCmdPublishSwp(aCmdPublishSwp.Severity(),aCmdPublishSwp.ExecutionBehaviour(),
                                                           aCmdPublishSwp.SsmSwpInfo());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdPublishSwp);
    return self;
    }

void CCmdPublishSwp::ConstructL(const CCmdPublishSwp& aCmdPublishSwp)
    {
    iConditionalResourceId = aCmdPublishSwp.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdPublishSwp.GetCommandResourceFileName());
        } 
#endif   
    }

/**
Destructor
*/	
CCmdPublishSwp::~CCmdPublishSwp()
	{
	Cancel();
	}

/**
Initialises the BIC's active object and initiates the command
 
@param aStatus 	the TRequestStatus of the active object calling this BIC
*/	
void CCmdPublishSwp::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	if (iExecutionBehaviour != ESsmWaitForSignal)
		{
		// complete the command request and initiate execution of next command
		CompleteExecuteRequest(KErrNone);
		}

	CompleteRequest(iStatus, KErrNone);
	SetActive();
	}

/**
Releases resources associated with this BIC
*/	
void CCmdPublishSwp::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/	
void CCmdPublishSwp::ExecuteCancel()
	{
	//cancel the outstanding request
	Cancel();
	//and complete the request with KErrCancel
	CompleteDeferredExecuteRequest(KErrCancel);
	}

/**
Returns the Type of the BIC
 
@return					The Type of the BIC
*/	
TSsmCommandType CCmdPublishSwp::Type() const
	{
	return (ESsmCmdPublishSwp);
	}

/**
Returns the Version of the BIC
 
@return		The maximum supported version of the BIC
*/	
TInt CCmdPublishSwp::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdPublishSwpVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdPublishSwpInitialVersion));
#endif
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdPublishSwp::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	const TUint key = aReadStream.ReadUint32L();
	const TInt value = aReadStream.ReadInt32L();	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif
	TSsmSwp swpInfo(key, value);
	iSwpInfo = swpInfo;
	}

/**
Externalises the configuration of the BIC
 
@param aWriteStream A write stream to write BIC data to
*/
void CCmdPublishSwp::ExternalizeL(RWriteStream& aWriteStream) const
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
void CCmdPublishSwp::RunL()
	{
	TInt err = RProperty::Set(RProcess().SecureId(), iSwpInfo.Key(), iSwpInfo.Value());
	DEBUGPRINT4A("SWP %d set with value %d with error %d", iSwpInfo.Key(), iSwpInfo.Value(), err);
	CompleteDeferredExecuteRequest(err);
	}

/**
Called to handle any cleanup if RunL leaves
 
@param aError 	The error to finish with
@return			KErrNone
*/
TInt CCmdPublishSwp::RunError(TInt aError)
	{
	CompleteDeferredExecuteRequest(aError);
	return KErrNone;
	}

/**
Called during cancellation of the active BIC
*/
void CCmdPublishSwp::DoCancel()
	{
	}

/**
default constructor.
*/
CCmdPublishSwp::CCmdPublishSwp()
	: iSwpInfo(0,0)
	{
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor.
@param aSeverity The severity of the command
@param aSwpInfo System wide property settings
@param aPriority The priority of the command in the list
*/
CCmdPublishSwp::CCmdPublishSwp(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo, const TUint16 aPriority)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour, aPriority), iSwpInfo(aSwpInfo) 
	{
	}
#endif

/**
Overloaded constructor.
@param aSeverity The severity of the command
@param aSwpInfo System wide property settings
*/
CCmdPublishSwp::CCmdPublishSwp(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour), iSwpInfo(aSwpInfo)
	{
	}

/**
Constructs an object from resource file.
@param aCommandParameters Object data from a resource file
*/
void CCmdPublishSwp::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdPublishSwp::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(reader.ReadUint8L());
	iSwpInfo = aCommandParameters.SwpL();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdPublishSwpInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif
	ValidateL();
	}

/**
Constructs the object through read stream.
@param aReadStream A read stream containing BIC data
*/
void CCmdPublishSwp::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	ValidateL();
	}

/**
validates the data
*/
void CCmdPublishSwp::ConstructL()
	{
	ValidateL();
	}


