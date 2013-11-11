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
#include <e32power.h>

#include "cmdpoweroff.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include "ssmcommandparameters.h"

/**
Used to create an instance of CCmdPowerOff class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream Read stream containing data through which object can be created
@return	A pointer to an object of type CCmdPowerOff.
*/
CCmdPowerOff* CCmdPowerOff::NewL(RReadStream& aReadStream)
	{
	CCmdPowerOff* self = new (ELeave) CCmdPowerOff();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdPowerOff class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity The severity of the command
@param aState The power state to transition into
@return	A pointer to an object of type CCmdPowerOff.
*/
CCmdPowerOff* CCmdPowerOff::NewL(TCmdErrorSeverity aSeverity, TPowerState aState)
	{
	CCmdPowerOff* self = new (ELeave) CCmdPowerOff(aSeverity, aState);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdPowerOff class from resource.

@param aCommandParameters Object data from a resource file
@return	A pointer to an object of type CCmdPowerOff.
*/
CCmdPowerOff* CCmdPowerOff::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdPowerOff* self = new (ELeave) CCmdPowerOff();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}
/**
Used to create an instance of CCmdPowerOff class from CCmdPowerOff object
Must be used only by CLE
@param aCmdPowerOff CCmdPowerOff reference 
@return A pointer to an object of type CCmdPowerOff.
*/
CCmdPowerOff* CCmdPowerOff::NewLC(const CCmdPowerOff& aCmdPowerOff)
    {
    CCmdPowerOff* self = new (ELeave) CCmdPowerOff(aCmdPowerOff.Severity(),aCmdPowerOff.PowerStateInfo());
    CleanupStack::PushL(self);
    self->ConstructL(aCmdPowerOff);
    return self;
    }

void CCmdPowerOff::ConstructL(const CCmdPowerOff& aCmdPowerOff)
    {
    iConditionalResourceId = aCmdPowerOff.ConditionalInformation();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdPowerOff.GetCommandResourceFileName());
        } 
#endif 
    }

/**
Destructor
*/	
CCmdPowerOff::~CCmdPowerOff()
	{
	Cancel();
	}

/**
Initialises the BIC's active object and initiates the command
 
@param aStatus 	the TRequestStatus of the active object calling this BIC
*/	
void CCmdPowerOff::Execute(TRequestStatus& aStatus)
	{
	DEBUGVERBOSE1A("CCmdPowerOff::Execute");
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	// power down and wait for a wakeup event
	if (iState == EPwStandby)
		{
#ifdef TEST_SSM_GRACEFUL_SHUTDOWN
		TInt testErr = RProperty::Set(iTestCategory, iTestKey, iState);
		DEBUGPRINT3A("Testing TPowerState::EPwStandby(%d), Published with error(%d)", iState, testErr);
		CompleteRequest(iStatus, KErrNone);	// For Testing complete the shutdown.
#else
		Power::RequestWakeupEventNotification(iStatus);
		TInt err = Power::EnableWakeupEvents(iState);
		if (err == KErrNone)
			{
			err = Power::PowerDown();

			if (err != KErrNone)
				{
				Power::CancelWakeupEventNotification();
				}
			}
#endif	//TEST_SSM_GRACEFUL_SHUTDOWN
		}
	else
		{
		CompleteRequest(iStatus, KErrNone);
		}

	SetActive();
	}

/**
Releases resources associated with this BIC
*/	
void CCmdPowerOff::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/	
void CCmdPowerOff::ExecuteCancel()
	{
	Cancel();
	CompleteExecuteRequest(KErrCancel);
	}

/**
Returns the Type of the BIC
@return	The Type of the BIC
*/	
TSsmCommandType CCmdPowerOff::Type() const
	{
	return (ESsmCmdPowerOff);
	}

/**
Returns the Version of the BIC
@return	The maximum supported version of the BIC
*/	
TInt CCmdPowerOff::MaxSupportedVersion()
	{
	return (static_cast<TInt>(ECmdPowerOffInitialVersion));
	}

/**
Configures the BIC using data contained in a ReadStream
@param aReadStream 	A read stream containing BIC data
*/
void CCmdPowerOff::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iState = static_cast<TPowerState>(aReadStream.ReadInt16L());
	}

/**
Externalises the configuration of the BIC

@param aWriteStream A write stream to write BIC data to
*/
void CCmdPowerOff::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	aWriteStream.WriteInt16L(iState);
	}

/**
Completes the tasks of the BIC
*/
void CCmdPowerOff::RunL()
	{
	SSMLOGLEAVEIFERROR(iStatus.Int());

	TInt err = KErrNone;
	if (iState != EPwStandby)
		{
#ifdef TEST_SSM_GRACEFUL_SHUTDOWN
		TInt testErr = RProperty::Set(iTestCategory, iTestKey, iState);
		DEBUGPRINT3A("Testing TPowerState::!EPwStandby(%d), Published with error(%d)", iState, testErr);
#else
		err = Power::EnableWakeupEvents(iState);
		if (err == KErrNone)
			{
			err = Power::PowerDown();	// will not return from this function on success...
			}
#endif	//TEST_SSM_GRACEFUL_SHUTDOWN
		}

	CompleteExecuteRequest(err);
	}

/**
Called to handle any cleanup if RunL leaves
@param aError The error to finish with
@return	KErrNone
*/
TInt CCmdPowerOff::RunError(TInt aError)
	{
	CompleteExecuteRequest(aError);
	return KErrNone;
	}

/**
Called during cancellation of the active BIC
*/
void CCmdPowerOff::DoCancel()
	{
	Power::CancelWakeupEventNotification();
	}

/**
Default constructor.
*/
CCmdPowerOff::CCmdPowerOff()
	{
#ifdef TEST_SSM_GRACEFUL_SHUTDOWN
	iTestKey = 123;
	iTestCategory = TUid::Uid(0x456);
#endif	//TEST_SSM_GRACEFUL_SHUTDOWN	
	}

/**
Overloaded constructor
@param aSeverity The severity of the command
@param aState The power state to transition into
*/
CCmdPowerOff::CCmdPowerOff(TCmdErrorSeverity aSeverity, TPowerState aState)
	: CSsmCommandBase(aSeverity), iState(aState)
	{
#ifdef TEST_SSM_GRACEFUL_SHUTDOWN
	iTestKey = 123;
	iTestCategory = TUid::Uid(0x456);
#endif	//TEST_SSM_GRACEFUL_SHUTDOWN
	}


/**
Constructs the object through read stream.
@param aReadStream A read stream containing BIC data
*/
void CCmdPowerOff::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	ConstructL();
	}

/**
Constructs an object from resource file.
@param aCommandParameters Object data from a resource file
*/
void CCmdPowerOff::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdPowerOff::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iState = static_cast<TPowerState>(reader.ReadInt16L());
	ConstructL();
	}

void CCmdPowerOff::ConstructL()
	{
	ValidateL();
	}

/**
Validates the data
@leave KErrArgument If value of power state is not one of the value in TPowerState
*/
void CCmdPowerOff::ValidateL()
	{
	if (iState!=EPwActive && iState!=EPwStandby && iState!=EPwOff &&
		iState!=EPwRestart && iState!=EPwLimit)
		{
		SSMLOGLEAVE(KErrArgument);
		}
	}

