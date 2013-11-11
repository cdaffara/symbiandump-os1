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
#include <bautils.h>

#include "cmdpersisthalattributes.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include "ssmcommandparameters.h"

/**
Used to create an instance of CCmdPersistHalAttributes class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdPersistHalAttributes.
*/
CCmdPersistHalAttributes* CCmdPersistHalAttributes::NewL(RReadStream& aReadStream)
	{
	CCmdPersistHalAttributes* self = new (ELeave) CCmdPersistHalAttributes();
	CleanupStack::PushL(self);
	self->InternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdPersistHalAttributes class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@return	A pointer to an object of type CCmdPersistHalAttributes.
*/
CCmdPersistHalAttributes* CCmdPersistHalAttributes::NewL(TCmdErrorSeverity aSeverity)
	{
	CCmdPersistHalAttributes* self = new (ELeave) CCmdPersistHalAttributes(aSeverity);
	return self;
	}

/**
Used to create an instance of CCmdPersistHalAttributes class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdPersistHalAttributes.
*/
CCmdPersistHalAttributes* CCmdPersistHalAttributes::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdPersistHalAttributes* self = new (ELeave) CCmdPersistHalAttributes();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdPersistHalAttributes class from CCmdPersistHalAttributes object
Must be used only by CLE
@param aCmdPersistHalAttributes CCmdPersistHalAttributes reference 
@return A pointer to an object of type CCmdPersistHalAttributes.
*/
CCmdPersistHalAttributes* CCmdPersistHalAttributes::NewLC(const CCmdPersistHalAttributes& aCmdPersistHalAttributes)
    {
    CCmdPersistHalAttributes* self = new (ELeave) CCmdPersistHalAttributes(aCmdPersistHalAttributes.Severity());
    CleanupStack::PushL(self);
    self->ConstructL(aCmdPersistHalAttributes);
    return self;
    }

void CCmdPersistHalAttributes::ConstructL(const CCmdPersistHalAttributes& aCmdPersistHalAttributes)
    {
    iConditionalResourceId = aCmdPersistHalAttributes.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdPersistHalAttributes.GetCommandResourceFileName());
        } 
#endif   
    }

/**
Destructor
*/	
CCmdPersistHalAttributes::~CCmdPersistHalAttributes()
	{
	Cancel();
	}

/**
Initialises the BIC's active object and initiates the command
 
@param aStatus 	the TRequestStatus of the active object calling this BIC
*/	
void CCmdPersistHalAttributes::Execute( TRequestStatus& aStatus )
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;
	CompleteRequest(iStatus, KErrNone);
	SetActive();
	}

/**
Releases resources associated with this BIC
*/
void CCmdPersistHalAttributes::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/
void CCmdPersistHalAttributes::ExecuteCancel()
	{
	Cancel();
	CompleteExecuteRequest(KErrCancel);
	}

/**
Returns the Type of the BIC
@return	The type of the BIC
*/	
TSsmCommandType CCmdPersistHalAttributes::Type() const
	{
	return (ESsmCmdPersistHalAttributes);
	}

/**
Returns the Version of the BIC
@return	The maximum supported version of the BIC
*/	
TInt CCmdPersistHalAttributes::MaxSupportedVersion()
	{
	return (static_cast<TInt>(ECmdPersistHalAttributesInitialVersion));
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdPersistHalAttributes::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	}

/**
Externalises the configuration of the BIC
 
@param aWriteStream A write stream to write BIC data to
*/
void CCmdPersistHalAttributes::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	}

/**
Completes the tasks of the BIC
*/
void CCmdPersistHalAttributes::RunL()
	{
	// iStatus.Int() is always KErrNone as Execute() sets it to KErrNone
	const TInt error = BaflUtils::PersistHAL();
	
	// These ASSERT_DEBUG statements are useful when the command is executed with 
	// IgnoreFailure Severity. Any error is supressed with a panic in DEBUG builds
	__ASSERT_DEBUG((KErrNone == error), PanicNow(KPanicCmdPersistHalAttributes, ECmdExecutionFailed));
	
	// If it fails in UREL builds, the error is just returned to CLE
	CompleteExecuteRequest(error);
	}

/**
Called to handle any cleanup if RunL leaves
 
@param aError The error to finish with
@return	KErrNone
*/
TInt CCmdPersistHalAttributes::RunError(TInt aError)
	{
	CompleteExecuteRequest(aError);
	return KErrNone;
	}

/**
Called during cancellation of the active BIC
*/
void CCmdPersistHalAttributes::DoCancel()
	{
	}

/**
Default constructor.
*/
CCmdPersistHalAttributes::CCmdPersistHalAttributes()
	{
	}

/**
Overloaded constructor

@param aSeverity The severity of the command
*/
CCmdPersistHalAttributes::CCmdPersistHalAttributes(TCmdErrorSeverity aSeverity)
	: CSsmCommandBase(aSeverity)
	{
	}

/**
Constructs the object through read stream.

@param aReadStream A read stream containing BIC data
*/
void CCmdPersistHalAttributes::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	}

/**
Constructs an object from resource file.

@param aCommandParameters Object data from a resource file
*/
void CCmdPersistHalAttributes::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdPersistHalAttributes::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	}

