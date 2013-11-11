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
#include <f32file.h>

#include "cmdfinalisedrives.h"
#include "ssmcommandparameters.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

/**
Used to create an instance of CCmdFinaliseDrives class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdFinaliseDrives.
*/
CCmdFinaliseDrives* CCmdFinaliseDrives::NewL(RReadStream& aReadStream)
	{
	CCmdFinaliseDrives* self = new (ELeave) CCmdFinaliseDrives();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdFinaliseDrives class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@return	A pointer to an object of type CCmdFinaliseDrives.
*/
CCmdFinaliseDrives* CCmdFinaliseDrives::NewL(TCmdErrorSeverity aSeverity)
	{
	CCmdFinaliseDrives* self = new (ELeave) CCmdFinaliseDrives(aSeverity);
	return self;
	}

/**
Used to create an instance of CCmdFinaliseDrives class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdFinaliseDrives.
*/
CCmdFinaliseDrives* CCmdFinaliseDrives::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdFinaliseDrives* self = new (ELeave) CCmdFinaliseDrives();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdFinaliseDrives class from CCmdFinaliseDrives object
Must be used only by CLE
@param aCmdFinaliseDrives CCmdFinaliseDrives reference 
@return A pointer to an object of type CCmdFinaliseDrives.
*/
CCmdFinaliseDrives* CCmdFinaliseDrives::NewLC(const CCmdFinaliseDrives& aCmdFinaliseDrives)
    {
    CCmdFinaliseDrives* self = new (ELeave) CCmdFinaliseDrives(aCmdFinaliseDrives.Severity());
    CleanupStack::PushL(self);
    self->ConstructL(aCmdFinaliseDrives);
    return self;
    }

void CCmdFinaliseDrives::ConstructL(const CCmdFinaliseDrives& aCmdFinaliseDrives)
    {
    iConditionalResourceId = aCmdFinaliseDrives.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdFinaliseDrives.GetCommandResourceFileName());
        }
#endif
    }

/**
Destructor
*/	
CCmdFinaliseDrives::~CCmdFinaliseDrives()
	{
	Cancel();
	iFs.Close();
	}

/**
Initialises the BIC's active object and initiates the command

@param aStatus 	the TRequestStatus of the active object calling this BIC
*/	
void CCmdFinaliseDrives::Execute( TRequestStatus& aStatus )
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	CompleteRequest(iStatus, iFs.Connect());
	SetActive();
	}

/**
Releases resources associated with this BIC
*/	
void CCmdFinaliseDrives::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/	
void CCmdFinaliseDrives::ExecuteCancel()
	{
	//cancel the outstanding request
	Cancel();
	//and complete the request with KErrCancel
	CompleteExecuteRequest(KErrCancel);
	}

/**
Returns the Type of the BIC
 
@return		The Type of the BIC
*/	
TSsmCommandType CCmdFinaliseDrives::Type() const
	{
	return (ESsmCmdFinaliseDrives);
	}

/**
Returns the Version of the BIC
 
@return		The maximum supported version of the BIC
*/	
TInt CCmdFinaliseDrives::MaxSupportedVersion()
	{
	return (static_cast<TInt>(ECmdFinaliseDrivesInitialVersion));
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdFinaliseDrives::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	}

/**
Externalises the configuration of the BIC
 
@param aWriteStream A write stream to write BIC data to
*/
void CCmdFinaliseDrives::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	}

/**
Completes the tasks of the BIC
*/
void CCmdFinaliseDrives::RunL()
	{
	TInt error = iStatus.Int();
	// These ASSERT_DEBUG statements are useful when the command is executed with 
	// ECmdIgnoreFailure Severity. Any error is supressed with a panic in DEBUG builds
	__ASSERT_DEBUG((KErrNone == error), PanicNow(KPanicCmdFinaliseDrives, ECmdExecutionFailed));
	
	if (KErrNone == error)
		{
		error = iFs.FinaliseDrives();
		__ASSERT_DEBUG((KErrNone == error), PanicNow(KPanicCmdFinaliseDrives, ECmdExecutionFailed));
		}
	
	// If it fails in UREL builds, the error is just returned to CLE
	CompleteExecuteRequest(error);
	}

/**
Called to handle any cleanup if RunL leaves
 
@param aError 	The error to finish with
@return			KErrNone
*/
TInt CCmdFinaliseDrives::RunError(TInt aError)
	{
	CompleteExecuteRequest(aError);
	return KErrNone;
	}

/**
Called during cancellation of the active BIC
*/
void CCmdFinaliseDrives::DoCancel()
	{
	}

/**
Default constructor.
*/
CCmdFinaliseDrives::CCmdFinaliseDrives()
	{
	}

/**
Overloaded constructor

@param aSeverity 	The severity of the command
*/
CCmdFinaliseDrives::CCmdFinaliseDrives(TCmdErrorSeverity aSeverity)
	: CSsmCommandBase(aSeverity)
	{
	}

/**
Constructs the object through read stream.
@param aReadStream A read stream containing BIC data
*/
void CCmdFinaliseDrives::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	}

/**
Constructs an object from resource file.

@param aCommandParameters Object data from a resource file
*/
void CCmdFinaliseDrives::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdFinaliseDrives::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	}
