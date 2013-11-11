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
#include "ssmcommandutilprovider.h"
#include "cmdwaitforapparcinit.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdWaitForApparcInit class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aPriority The priority of the command in the list
@return	A pointer to an object of type CCmdWaitForApparcInit.
*/
CCmdWaitForApparcInit* CCmdWaitForApparcInit::NewL(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	{
	CCmdWaitForApparcInit* self = new (ELeave) CCmdWaitForApparcInit(aSeverity, aPriority);
	return self;
	}
#endif

/**
Used to create an instance of CCmdWaitForApparcInit class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@return	A pointer to an object of type CCmdWaitForApparcInit.
*/
CCmdWaitForApparcInit* CCmdWaitForApparcInit::NewL(TCmdErrorSeverity aSeverity)
	{
	CCmdWaitForApparcInit* self = new (ELeave) CCmdWaitForApparcInit(aSeverity);
	return self;
	}

/**
Used to create an instance of CCmdWaitForApparcInit class from a read stream.
CSsmCommandList::InternalizeL() this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdWaitForApparcInit.
*/
CCmdWaitForApparcInit* CCmdWaitForApparcInit::NewL(RReadStream& aReadStream)
	{
	CCmdWaitForApparcInit* self = new (ELeave) CCmdWaitForApparcInit();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdWaitForApparcInit class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdWaitForApparcInit.
*/
CCmdWaitForApparcInit* CCmdWaitForApparcInit::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdWaitForApparcInit* self = new (ELeave) CCmdWaitForApparcInit();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}
/**
Used to create an instance of CCmdWaitForApparcInit class from CCmdWaitForApparcInit object
Must be used only by CLE
@param aCmdWaitForApparcInit CCmdWaitForApparcInit reference 
@param aUtilProvider CSsmCommandUtilProvider reference 
@return A pointer to an object of type CCmdWaitForApparcInit.
*/
CCmdWaitForApparcInit* CCmdWaitForApparcInit::NewLC(const CCmdWaitForApparcInit& aCmdWaitForApparcInit, CSsmCommandUtilProvider* aUtilProvider)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdWaitForApparcInit* self = new (ELeave) CCmdWaitForApparcInit(aCmdWaitForApparcInit.Severity(),aCmdWaitForApparcInit.Priority());
#else
    CCmdWaitForApparcInit* self = new (ELeave) CCmdWaitForApparcInit(aCmdWaitForApparcInit.Severity());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdWaitForApparcInit, aUtilProvider);
    return self;
    }

void CCmdWaitForApparcInit::ConstructL(const CCmdWaitForApparcInit& aCmdWaitForApparcInit, CSsmCommandUtilProvider* aUtilProvider)
    {
    iConditionalResourceId = aCmdWaitForApparcInit.ConditionalInformation();
    SetUtilProvider(*aUtilProvider);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdWaitForApparcInit.GetCommandResourceFileName());
        }
#endif    
    }

/**
Returns the version of the BIC
@return		The maximum supported version of the BIC
*/
TInt CCmdWaitForApparcInit::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdWaitForApparcInitVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdWaitForApparcInitInitialVersion));
#endif
	}
/**
Destructor
*/
CCmdWaitForApparcInit::~CCmdWaitForApparcInit()
	{
	Cancel();
	}

/**
Execute this command
@param aStatus 	Completed to indicate execution of the next command may begin.		
@panic ECmdNullPtr if the information used to create command is null
*/
void CCmdWaitForApparcInit::Execute(TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iUtilProvider, PanicNow(KPanicCmdWaitForApparcInit, ECmdNullPtr));
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;
	
	iUtilProvider->StartSafe().InitApparcServer(iStatus);
	SetActive();
	}

/**
Releases resources associated with this BIC
*/
void CCmdWaitForApparcInit::Release()
	{
	delete this;
	}

/**
Cancels the BIC
*/
void CCmdWaitForApparcInit::ExecuteCancel()
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
TSsmCommandType CCmdWaitForApparcInit::Type() const
	{
	DEBUGPRINT2A("Command type: %d", ESsmCmdWaitForApparcInit);
	return (ESsmCmdWaitForApparcInit);
	}

/**
Externalises the configuration of the BIC

@param aWriteStream A write stream to write BIC data to
*/
void CCmdWaitForApparcInit::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
	}

/**
Handles command's request completion event.
*/
void CCmdWaitForApparcInit::RunL()
	{
	CompleteExecuteRequest(iStatus.Int());
	}

/**
cancellation of an outstanding request.
*/
void CCmdWaitForApparcInit::DoCancel()
	{
	iUtilProvider->StartSafe().InitApparcServerCancel();
	}

/**
Complete the command request with aError. It gets called when RunL leaves.

@param aError Error code with which RunL has left.
@return KErrNone as error is handled in this function.
*/
TInt CCmdWaitForApparcInit::RunError(TInt aError)
	{
	DEBUGPRINT2A("CCmdWaitForApparcInit::RunError called with reason %d", aError);
	CompleteExecuteRequest(aError);
	return KErrNone;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded constructor.
@param aSeverity Severity for this BIC
@param aPriority The priority of the command in the list
 */
CCmdWaitForApparcInit::CCmdWaitForApparcInit(TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	: CSsmCommandBase(aSeverity, aPriority)
	{
	}
#endif

/**
Overloaded constructor.
@param aSeverity Severity for this BIC
 */
CCmdWaitForApparcInit::CCmdWaitForApparcInit(TCmdErrorSeverity aSeverity)
	: CSsmCommandBase(aSeverity)
	{
	}

/**
Default constructor.
*/
CCmdWaitForApparcInit::CCmdWaitForApparcInit()
	{
	}

/**
Constructs an object from resource file.

@param aCommandParameters Object data from a resource file
*/
void CCmdWaitForApparcInit::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdWaitForApparcInit::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdWaitForApparcInitInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif
	}

/**
Constructs an object from a read stream.

@param aReadStream Read stream containing data through which object can be created
*/
void CCmdWaitForApparcInit::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdWaitForApparcInit::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif
	}
