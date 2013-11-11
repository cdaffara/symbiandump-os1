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
#include <ssm/ssmloadamastart.h>
#include "cmdamastarter.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

_LIT( KAmaStartDLL, "loadamastart.dll" );
typedef CSsmLoadAmaStart* (*TFuncLoadAmaCreateL)(void);

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdAmaStarter class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aExecutionBehaviour Execution behaviour of this command
@param aDscId DSC id to start the AMA
@param aPriority Priority of the command
@return	A pointer to an object of type CCmdAmaStarter.
*/
CCmdAmaStarter* CCmdAmaStarter::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TUid aDscId, const TUint16 aPriority)
	{
	SSMLOGLEAVEIFTRUE(aDscId == KNullUid, KErrArgument);		//aDscId shouldn't be NULL
	CCmdAmaStarter* self = new (ELeave) CCmdAmaStarter(aSeverity, aExecutionBehaviour, aDscId, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdAmaStarter class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aExecutionBehaviour Execution behaviour of this command
@param aDscId DSC id to start the AMA
@return	A pointer to an object of type CCmdAmaStarter.
*/
CCmdAmaStarter* CCmdAmaStarter::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TUid aDscId)
	{
	SSMLOGLEAVEIFTRUE(aDscId == KNullUid, KErrArgument);		//aDscId shouldn't be NULL
	CCmdAmaStarter* self = new (ELeave) CCmdAmaStarter(aSeverity, aExecutionBehaviour, aDscId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdAmaStarter class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdAmaStarter.
*/
CCmdAmaStarter* CCmdAmaStarter::NewL(RReadStream& aReadStream)
	{
	CCmdAmaStarter* self = new (ELeave) CCmdAmaStarter();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdAmaStarter class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdAmaStarter.
*/
CCmdAmaStarter* CCmdAmaStarter::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdAmaStarter* self = new (ELeave) CCmdAmaStarter();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdAmaStarter class from CCmdAmaStarter object
Must be used only by CLE
@param aCmdAmaStarter CCmdAmaStarter reference
@return A pointer to an object of type CCmdAmaStarter.
*/
CCmdAmaStarter* CCmdAmaStarter::NewLC(const CCmdAmaStarter& aCmdAmaStarter)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdAmaStarter* self = new (ELeave) CCmdAmaStarter(aCmdAmaStarter.Severity(),aCmdAmaStarter.ExecutionBehaviour(),
                                                       aCmdAmaStarter.DscId(),aCmdAmaStarter.Priority());
#else
    CCmdAmaStarter* self = new (ELeave) CCmdAmaStarter(aCmdAmaStarter.Severity(),aCmdAmaStarter.ExecutionBehaviour(),
                                                           aCmdAmaStarter.DscId());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdAmaStarter);
    return self;
    }

void CCmdAmaStarter::ConstructL(const CCmdAmaStarter& aCmdAmaStarter)
    {
    iConditionalResourceId = aCmdAmaStarter.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
	    {
	    SetCommandResourceFileNameL(aCmdAmaStarter.GetCommandResourceFileName());
	    }
#endif    
    }
/**
Returns the version of the BIC
@return The version of BIC
*/
TInt CCmdAmaStarter::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdAmaStarterVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdAmaStarterInitialVersion));
#endif
	}

/**
Destructor
*/
CCmdAmaStarter::~CCmdAmaStarter()
	{
	Cancel();
	delete iAmaStart;
	iAmaStartLib.Close();
	}

/**
Execute this command
@param aStatus 	The TRequestStatus which is completed when value is set either 
				with KErrNone or with error value.
*/
void CCmdAmaStarter::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;
	DEBUGPRINT3(_L("Starting AMA with DSC Id = 0x%x and execution behaviour = %d"), iDscId.iUid, iExecutionBehaviour);
	if (iExecutionBehaviour != ESsmWaitForSignal)
		{
		// complete the command request and initiate execution of next command
		CompleteExecuteRequest(KErrNone);
		}

	TRAPD(err, LoadAmaStartLibraryL());
	if(KErrNone != err)
		{
		// complete the command request with error
		CompleteDeferredExecuteRequest(err);
		return;
		}
	SetActive();
	iAmaStart->Start(iDscId, iStatus);
	}

/**
Load loadamastart.dll
*/
void CCmdAmaStarter::LoadAmaStartLibraryL()
	{
	SSMLOGLEAVEIFERROR(iAmaStartLib.Load(KAmaStartDLL));
	TFuncLoadAmaCreateL amaStart = reinterpret_cast<TFuncLoadAmaCreateL>(iAmaStartLib.Lookup(1));
	SSMLOGLEAVEIFTRUE(amaStart == NULL, KErrArgument);
	iAmaStart = amaStart();
	SSMLOGLEAVEIFNULL(iAmaStart);
	}

/**
Releases resources associated with this BIC
*/
void CCmdAmaStarter::Release()
	{
	delete this;
	}

/**
Cancels the BIC
*/
void CCmdAmaStarter::ExecuteCancel()
	{
	//cancel the outstanding request
	Cancel();
	//and complete the request with KErrCancel
	CompleteDeferredExecuteRequest(KErrCancel);
	}

/**
Returns the type of the BIC
@return Type of BIC
*/
TSsmCommandType CCmdAmaStarter::Type() const
	{
	return (ESsmCmdAMAStarter);
	}

/**
Externalises the configuration of the BIC

@param aWriteStream A write stream to write BIC data to
*/
void CCmdAmaStarter::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	aWriteStream.WriteUint8L(iExecutionBehaviour);
	aWriteStream.WriteUint32L(iDscId.iUid);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
	}

/**
Handles command's request completion event.
*/
void CCmdAmaStarter::RunL()
	{
	CompleteDeferredExecuteRequest(iStatus.Int());
	}

/**
cancellation of an outstanding request.
*/
void CCmdAmaStarter::DoCancel()
	{
	iAmaStart->StartCancel();
	}

/**
Complete the command request with aError. It gets called when RunL leaves.

@param aError Error code with which RunL has left.
@return KErrNone as error is handled in this function.
*/
TInt CCmdAmaStarter::RunError(TInt aError)
	{
	DEBUGPRINT2A("CCmdAmaStarter::RunError called with error %d", aError);
	CompleteDeferredExecuteRequest(aError);
	return KErrNone;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor

@param aSeverity 	The severity of the command
@param aExecutionBehaviour Execution behaviour for this command
@param aDscId DSC id to start the AMA
@param aPriority Indicating the priority of the command
*/	
CCmdAmaStarter::CCmdAmaStarter(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId, const TUint16 aPriority)
	:CSsmDeferrableCommand(aSeverity, aExecutionBehaviour, aPriority), iDscId(aDscId)
	{
	}
#endif

/**
Overloaded constructor

@param aSeverity 	The severity of the command
@param aExecutionBehaviour Execution behaviour for this command
@param aDscId DSC id to start the AMA
*/	
CCmdAmaStarter::CCmdAmaStarter(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId)
	:CSsmDeferrableCommand(aSeverity, aExecutionBehaviour), iDscId(aDscId)
	{
	}

/**
 Default constructor.
 */
CCmdAmaStarter::CCmdAmaStarter()
	{
	}

/**
Constructs the object through read stream.
@param aReadStream A read stream containing BIC data
*/
void CCmdAmaStarter::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	ConstructL();	//validate the commands data
	}

/**
Constructs an object from resource file.

@param aCommandParameters Object data from a resource file
*/
void CCmdAmaStarter::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdAmaStarter::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(reader.ReadUint8L());
	iDscId.iUid = reader.ReadUint32L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdAmaStarterInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif
	ConstructL();	//validate the commands data
	}
	
/**
Validate data.
*/
void CCmdAmaStarter::ConstructL()
	{
	ValidateL();
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdAmaStarter::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	iDscId.iUid = aReadStream.ReadUint32L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif	
	}

/**
Validate commands data
*/
void CCmdAmaStarter::ValidateL()
	{
	CSsmDeferrableCommand::ValidateL();
	SSMLOGLEAVEIFTRUE(iDscId == KNullUid, KErrArgument);		//iDscId shouldn't be NULL
	}
