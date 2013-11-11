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

#include <bluetooth/eirpublisher.h>
#include <e32uid.h>
#include <e32base.h>
#include <bluetooth/eirclient.h>
#include <bluetooth/eirmanshared.h>
#include "eirmanclientlogger.h"


void Panic(TBTEirPublisherPanic aPanic)
	{
	User::Panic(KBTEirPublisherPanic, aPanic);
	}

EXPORT_C CEirPublisher* CEirPublisher::NewL(TEirTag aEirTag, MEirPublisherNotifier& aNotifier)
	{
	LOG1(_L8("CEirPublisher::NewL creating publisher with tag %d"), aEirTag);
	CEirPublisher* self = new(ELeave) CEirPublisher(aEirTag, aNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CEirPublisher::CEirPublisher(TEirTag aEirTag, MEirPublisherNotifier& aNotifier)
	: CActive(CActive::EPriorityStandard)
	, iEirTag(aEirTag)
	, iNotifier(aNotifier)
	, iPublishOutstanding(EFalse)
	, iRequiredBytes(0)
	{
	CActiveScheduler::Add(this);
	}

CEirPublisher::~CEirPublisher()
	{
	LOG_FUNC
	iEirMan.Close(); // this should handle everything!
	Cancel();
	delete iNotifierAo;
	}

void CEirPublisher::ConstructL()
	{
	LOG_FUNC
	iNotifierAo = new(ELeave) CEirPublisherNotifier(*this);
	iEirMan.Connect(iStatus);
	iState = EConnecting;
	SetActive();
	}

/**
Notify server the required bytes from this client for the Extended Inquiry Response packet.
@param aRequiredBytes for the required bytes from this client
*/
EXPORT_C void CEirPublisher::PublishData(TInt aRequiredBytes)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iState != EInvalid, Panic(EUnexpectedPublisherState));
	if(iState == ENoConnection)
		{
		// If there is no connection to server currently, create one and indicate a request is outstanding.
		iEirMan.Connect(iStatus);
		iState = EConnecting;
		iPublishOutstanding = ETrue;
		iRequiredBytes = aRequiredBytes;
		SetActive();
		}
	else if(iState != EIdle)
		{
		iPublishOutstanding = ETrue;
		iRequiredBytes = aRequiredBytes;
		}
	else
		{
		iState = ENewData;
		iEirMan.NewData(aRequiredBytes, iStatus);
		SetActive();
		}
	}

/**
Send data and its type to server to update the Extended Inquiry Response packet.
@param aData for the data to be added into Extended Inquiry Response
@param aDataMode for the type of data from this client (partial or complete)
*/
EXPORT_C void CEirPublisher::SetData(const TDesC8& aData, TEirDataMode aDataMode)
	{
	LOG_FUNC
	// Make sure that client has an invitation to the party
	__ASSERT_ALWAYS(iState == EDataCallback, PANIC(KEirManCliPncCat, EEirManPanicSetDataUninvited));
	iState = ESettingData;
	iEirMan.SetData(aData, aDataMode, iStatus);
	SetActive();
	}

void CEirPublisher::RunL()
	{
	LOG_FUNC
	LEAVEIFERRORL(iStatus.Int());
	
	switch(iState)
		{
	case EConnecting:
		iEirMan.RegisterTag(iEirTag, iStatus);
		iState = ERegisterTag;
		SetActive();
		break;
	case ERegisterTag:
		iNotifierAo->Start();
		// Fall through...
	case ENewData:
		// Fall through...
	case ESettingData:
		if(iPublishOutstanding)
			{
			iPublishOutstanding = EFalse;
			iState = ENewData;
			iEirMan.NewData(iRequiredBytes, iStatus);
			SetActive();
			}
		else if(iCallbackOutstanding)
			{
			iCallbackOutstanding = EFalse;
			iState = EDataCallback;
			LOG1(_L8("CEirPublisher::RunL: About to call back client with bytes available: %d"), iAvailableBytes);
			iNotifier.MepnSpaceAvailable(iAvailableBytes);
			}
		else
			{
			iState = EIdle;
			}
		break;		
	default:
		// Unexpected
		__ASSERT_DEBUG(EFalse, Panic(EUnexpectedPublisherState));
		break;
		}
	}

void CEirPublisher::DoCancel()
	{
	LOG_FUNC
	iNotifierAo->Cancel();
	// Because of asynchronous shutdown - we close the session and tidy up
	// the active objects locally.
	__ASSERT_DEBUG(!iEirMan.Handle(), Panic(EInvalidEirManHandle)); 
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	iState = EInvalid;
	}

TInt CEirPublisher::RunError(TInt aError)
	{
	LOG_FUNC
	LOG1(_L8("CEirPublisher::RunError: %d"), aError);
	// report error to client
	iNotifier.MepnSetDataError(aError);
	if (iState == EConnecting)
		{
		// We have a problem on Connecting to server, set the state to No Connection
		iState = ENoConnection;
		}
	return KErrNone;
	}

void CEirPublisher::SpaceAvailable(TUint aSpace)
	{
	LOG_FUNC
	
	if (aSpace > KEirLengthTagLength)
		{
		iAvailableBytes = aSpace - KEirLengthTagLength;
		}
	else
		{
		iAvailableBytes = 0;
		}
	if(iState == EIdle)
		{
		iState = EDataCallback;
		LOG1(_L8("CEirPublisher::RunL: About to call back client with bytes available: %d"), iAvailableBytes);
		iNotifier.MepnSpaceAvailable(iAvailableBytes);
		}
	else
		{
		iCallbackOutstanding = ETrue;
		}
	}

void CEirPublisher::NotifierError(TInt aError)
	{
	static_cast<void>(RunError(aError));
	}

REirMan& CEirPublisher::EirMan()
	{
	LOG_FUNC
	return iEirMan;
	}



CEirPublisherNotifier::CEirPublisherNotifier(CEirPublisher& aPublisher)
	: CActive(EPriorityStandard)
	, iPublisher(aPublisher)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

CEirPublisherNotifier::~CEirPublisherNotifier()
	{
	LOG_FUNC
	Cancel();
	}

void CEirPublisherNotifier::Start()
	{
	LOG_FUNC
	iPublisher.EirMan().SpaceAvailableNotification(iSpaceAvailable, iStatus);
	SetActive();
	}

void CEirPublisherNotifier::RunL()
	{
	LOG_FUNC
	LEAVEIFERRORL(iStatus.Int());
	TUint spaceAvailable = iSpaceAvailable;
	iPublisher.EirMan().SpaceAvailableNotification(iSpaceAvailable, iStatus);
	SetActive();
	iPublisher.SpaceAvailable(spaceAvailable);
	}

void CEirPublisherNotifier::DoCancel()
	{
	LOG_FUNC
	// Because of asynchronous shutdown - we close the session and tidy up
	// the active objects locally.
	__ASSERT_DEBUG(!(iPublisher.EirMan().Handle()), Panic(EInvalidEirManHandle)); 
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	}

TInt CEirPublisherNotifier::RunError(TInt aError)
	{
	LOG_FUNC
	iPublisher.NotifierError(aError);
	return KErrNone;
	}
