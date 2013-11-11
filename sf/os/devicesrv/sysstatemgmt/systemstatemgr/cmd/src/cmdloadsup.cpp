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

#include "cmdloadsup.h"
#include "ssmdebug.h"
#include "ssmcommandparameters.h"

/**
Used to create an instance of CCmdLoadSup class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdLoadSup.
*/
CCmdLoadSup* CCmdLoadSup::NewL(RReadStream& aReadStream)
	{
	CCmdLoadSup* self = new (ELeave) CCmdLoadSup();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdLoadSup class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity 			The severity of the command
@param aExecutionBehaviour 	The execution behaviour of the command
@param aRetries 			The number of allowed retries
@param aInfo	 			SUP-specific info
@param aPriority 			The priority of the command in the list
@return	A pointer to an object of type CCmdLoadSup.
*/
CCmdLoadSup* CCmdLoadSup::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo, const TUint16 aPriority)
	{
	CCmdLoadSup* self = new (ELeave) CCmdLoadSup(aSeverity, aExecutionBehaviour, aRetries, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aInfo);
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdLoadSup class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity 			The severity of the command
@param aExecutionBehaviour 	The execution behaviour of the command
@param aRetries 			The number of allowed retries
@param aInfo	 			SUP-specific info
@return	A pointer to an object of type CCmdLoadSup.
*/
CCmdLoadSup* CCmdLoadSup::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo)
	{
	CCmdLoadSup* self = new (ELeave) CCmdLoadSup(aSeverity, aExecutionBehaviour, aRetries);
	CleanupStack::PushL(self);
	self->ConstructL(aInfo);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdLoadSup class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdLoadSup.
*/
CCmdLoadSup* CCmdLoadSup::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdLoadSup* self = new (ELeave) CCmdLoadSup();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdLoadSup class from CCmdLoadSup object
Must be used only by CLE
@param aCmdLoadSup CCmdLoadSup reference 
@return A pointer to an object of type CCmdLoadSup.
*/
CCmdLoadSup* CCmdLoadSup::NewLC(const CCmdLoadSup& aCmdLoadSup)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdLoadSup* self = new (ELeave) CCmdLoadSup(aCmdLoadSup.Severity(),aCmdLoadSup.ExecutionBehaviour(),
                                                 aCmdLoadSup.Retries(),aCmdLoadSup.Priority());
#else
    CCmdLoadSup* self = new (ELeave) CCmdLoadSup(aCmdLoadSup.Severity(),aCmdLoadSup.ExecutionBehaviour(),
                                                     aCmdLoadSup.Retries());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdLoadSup);
    return self;
    }

void CCmdLoadSup::ConstructL(const CCmdLoadSup& aCmdLoadSup)
    {
    iConditionalResourceId = aCmdLoadSup.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdLoadSup.GetCommandResourceFileName());
        }
#endif
    iSupInfo = aCmdLoadSup.SsmSupInfo();
    iSupInfoBuf = new (ELeave) TPckgC<TSsmSupInfo>(iSupInfo);
    }

/**
Destructor
*/	
CCmdLoadSup::~CCmdLoadSup()
	{
	Cancel();
	iSusCli.Close();
	delete iSupInfoBuf;
	}

/**
Initialises the BIC's active object and initiates the command

@param aStatus 	the TRequestStatus of the active object calling this BIC
*/	
void CCmdLoadSup::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	if (iExecutionBehaviour != ESsmWaitForSignal)
		{
		// complete the execute request with KErrNone so that CLE can execute next command
		CompleteExecuteRequest(KErrNone);
		}

	// try to connect and request to load a SUP
	TInt err = ConnectAndLoadSup();	
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
void CCmdLoadSup::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/	
void CCmdLoadSup::ExecuteCancel()
	{
	Cancel();
	CompleteDeferredExecuteRequest(KErrCancel);
	}

/**
Returns the Type of the BIC
 
@return	The Type of the BIC
*/	
TSsmCommandType CCmdLoadSup::Type() const
	{
	return (ESsmCmdLoadSup);
	}

/**
Returns the Version of the BIC
 
@return	The maximum supported version of the BIC
*/	
TInt CCmdLoadSup::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdLoadSupVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdLoadSupInitialVersion));
#endif
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdLoadSup::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());
	TSsmSupInfo info;
	info.InternalizeL(aReadStream);
	iRetries = aReadStream.ReadInt16L();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif
	ConstructL(info);
	}

/**
Externalises the configuration of the BIC
 
@param aWriteStream A write stream to write BIC data to
*/
void CCmdLoadSup::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	aWriteStream.WriteUint8L(iExecutionBehaviour);
	iSupInfo.ExternalizeL(aWriteStream);
	aWriteStream.WriteInt16L(iRetries);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
	}

/**
Completes the tasks of the BIC
*/
void CCmdLoadSup::RunL()
	{
	DEBUGPRINT3A("CCmdLoadSup::RunL: Retry attempts %d Connected %d", iAttempts, iConnected);
	if (iStatus.Int() != KErrNone)
		{
		if (++iAttempts >= iRetries)
			{
			SSMLOGLEAVE(iStatus.Int());				// exhausted retries
			}
		// try to connect and load sup
		TInt err = ConnectAndLoadSup();	
		if (KErrNone != err)
			{
			// we need to retry in case of failures
			CompleteRequest(iStatus, err);
			}
		SetActive();
		}
	else
		{
		CompleteDeferredExecuteRequest(KErrNone);	// success
		}
	}

/**
Called to handle any cleanup if RunL leaves
 
@param aError 	The error to finish with
@return			KErrNone
*/
TInt CCmdLoadSup::RunError(TInt aError)
	{
	CompleteDeferredExecuteRequest(aError);
	return KErrNone;
	}

/**
Called during cancellation of the active BIC
*/
void CCmdLoadSup::DoCancel()
	{
	iSusCli.RequestLoadSupCancel();
	}
/**
Default constructor.
*/
CCmdLoadSup::CCmdLoadSup()
	{
	}


#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor

@param aSeverity 	The severity of the command
@param aExecutionBehaviour Execution behaviour for this command
@param aRetries Number of retries used for this command
@param aPriority The priority of the command in the list
*/	
CCmdLoadSup::CCmdLoadSup(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TUint16 aPriority)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour, aPriority), iRetries(aRetries)
	{
	}
#endif

/**
Overloaded constructor

@param aSeverity 	The severity of the command
@param aExecutionBehaviour Execution behaviour for this command
@param aRetries Number of retries used for this command
*/	
CCmdLoadSup::CCmdLoadSup(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour), iRetries(aRetries)
	{
	}

/**
Constructs the object through read stream.
@param aReadStream A read stream containing BIC data
*/
void CCmdLoadSup::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	}

/**
Constructs an object from resource file.

@param aCommandParameters Object data from a resource file
*/
void CCmdLoadSup::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdLoadSup::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(reader.ReadUint8L());
	const TPtrC filename = reader.ReadTPtrCL();
	const TUid uid = { reader.ReadUint32L() };
	const TInt32 ordinal = reader.ReadInt32L();
	iRetries = reader.ReadInt16L();

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdLoadSupInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif
	TSsmSupInfo info;
	info.SetL(filename, ordinal, uid);
	ConstructL(info);
	}

/**
Constructs an object from TSsmSupInfo.
@param aInfo SUP-specific info
*/
void CCmdLoadSup::ConstructL(const TSsmSupInfo& aInfo)
	{
	iSupInfo.SetL(aInfo.FileName(), aInfo.NewLOrdinal(), aInfo.Identity());
	iSupInfoBuf = new TPckgC<TSsmSupInfo>(iSupInfo);
	SSMLOGLEAVEIFNULL(iSupInfoBuf);
	ValidateL();
	}

/**
Function to validate the data.
@leave KErrArgument If commands data is not valid
*/
void CCmdLoadSup::ValidateL()
	{
	CSsmDeferrableCommand::ValidateL();

	SSMLOGLEAVEIFFALSE(iSupInfo.FileName().Length() >0, KErrArgument);
	SSMLOGLEAVEIFFALSE(iSupInfo.NewLOrdinal() >0, KErrArgument);
	SSMLOGLEAVEIFFALSE(iRetries >=0, KErrArgument);
	}

/**
Function to connect to RSsmSusCli and request to load a SUP if connect is successful.
@return KErrNone if connect is successful and RequestLoadSup() is called,
otherwise return one of the system wide error codes
*/
TInt CCmdLoadSup::ConnectAndLoadSup()
	{
	if (!iConnected)
		{
		TInt err = iSusCli.Connect();
		if (KErrNone != err)
			{
			return err;
			}
		iConnected = ETrue;
		}
	const TDesC8& info = (*iSupInfoBuf);
	iSusCli.RequestLoadSup(info, iStatus);

	return KErrNone;
	}

