// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class implementation of CLbsPrivacyNotifierImpl
// 
//

/**
 @file
 @internalTechnology
*/

#include "e32debug.h"
#include "lbsprivacynotifierimpl.h"
#include "lbsdevloggermacros.h"

/*
Static constructor
*/
CLbsPrivacyNotifierImpl* CLbsPrivacyNotifierImpl::NewL(TPrivacyDialogType aDlg)
	{
	CLbsPrivacyNotifierImpl* self=new (ELeave) CLbsPrivacyNotifierImpl;
	CleanupStack::PushL(self);
	self->ConstructL(aDlg);
	CleanupStack::Pop();
	return self;
	}
	
/*
Static constructor
*/
CLbsPrivacyNotifierImpl* CLbsPrivacyNotifierImpl::NewL(TPrivacyDialogType aDlg, TTimeIntervalMicroSeconds32 aTimeout)
	{
	CLbsPrivacyNotifierImpl* self=new (ELeave) CLbsPrivacyNotifierImpl;
	CleanupStack::PushL(self);
	self->ConstructL(aDlg, aTimeout);
	CleanupStack::Pop();
	return self;
	}


/*
1st phase Constructor
*/
CLbsPrivacyNotifierImpl::CLbsPrivacyNotifierImpl()
	: CActive(EPriorityNormal),
	iNotifierUid(TUid::Uid(0))
	{
	CActiveScheduler::Add(this);
	}

/*
2nd phase Constructor
*/
void CLbsPrivacyNotifierImpl::ConstructL(TPrivacyDialogType aDlg)
	{
	//
	// Timeout for notifier display. 
	// NB Arbitrarily cchosen and not actually used; left here in case
	// a specific timeout for an unacknowleged notifier is required.
	//
	const TTimeIntervalMicroSeconds32 KNotifierDisplayTimeout = 10000000;

	ConstructL(aDlg, KNotifierDisplayTimeout);
	}

/*
2nd phase Constructor
*/
void CLbsPrivacyNotifierImpl::ConstructL(TPrivacyDialogType aDlg, TTimeIntervalMicroSeconds32 aTimeout)
	{
	User::LeaveIfError(Connect());
	iDialog = aDlg;
	iTimeout = aTimeout;
	}

/*
Destructor
*/
CLbsPrivacyNotifierImpl::~CLbsPrivacyNotifierImpl()
	{
	Cancel();
	iNotifier.Close();
	}

/*
Dummy implementation
*/
void CLbsPrivacyNotifierImpl::DoCancel()
	{
	if (iNotifierUid != TUid::Uid(0))
		{
		iNotifier.CancelNotifier(iNotifierUid);
		iNotifierUid = TUid::Uid(0);
		}
	}
	

/*
Connect to RNotifier
*/
TInt CLbsPrivacyNotifierImpl::Connect()
	{
	TInt err = iNotifier.Connect();
	return(err);
	}

/*
Process the user's reply
*/
void CLbsPrivacyNotifierImpl::RunL()
	{
	// Cancel the existing RNotifier if there is one.
	if (iNotifierUid != TUid::Uid(0))
		{
		iNotifier.CancelNotifier(iNotifierUid);
		iNotifierUid = TUid::Uid(0);
		}

	// Here when user dismisses dialogue or there is an error
	TInt err = iStatus.Int();
	if(err == KErrNone)
		{
		TLbsPrivacyNotifierResponse response;
		iPrivacyResponsePckg().GetResponseType(response);
		if (iObserver)
			{
			LBSLOG(ELogP9, "<-S MLbsPrivacyNotifierObserver::OnNotificationDialogResponse() lbsnetworkrequesthandler.exe\n");
			LBSLOG(ELogP9, "  > TInt aErr  = KErrNone\n");
			LBSLOG2(ELogP9, "  > TLbsPrivacyNotifierResponse aResponse  = %d\n", response);
			iObserver->OnNotificationDialogResponse(0, response);
			}
		}
	else
		{
		LBSLOG(ELogP9, "<-S MLbsPrivacyNotifierObserver::OnNotificationDialogResponse() lbsnetworkrequesthandler.exe\n");
		LBSLOG2(ELogP9, "  > TInt aErr  = %d\n", err);
		LBSLOG2(ELogP9, "  > TLbsPrivacyNotifierResponse aResponse  = %d\n", EResponseUnknown);
		iObserver->OnNotificationDialogResponse(err, EResponseUnknown);
		}
	}


/*
Observer
*/
void CLbsPrivacyNotifierImpl::SetObserver(MLbsPrivacyNotifierObserver* aObserver)
	{
	iObserver = aObserver;
	}


void CLbsPrivacyNotifierImpl::DisplayL(TInt& /*aErr*/, const TLbsExternalRequestInfo& aRequestInfo)
	{
	TLbsPrivacyNotifierParams params;
	params.SetRequesterInfo(aRequestInfo);		
	TLbsPrivacyNotifierParamsPckg pckg(params);
	
	// Cancel the existing RNotifier if there is one.
	if (iNotifierUid != TUid::Uid(0))
		{
		iNotifier.CancelNotifier(iNotifierUid);
		iNotifierUid = TUid::Uid(0);
		}
	
	// Start the new notifier/verifier
	switch (iDialog)
		{
		case EPrivacyDialogNotification:
			{
#if defined(NRH_UNIT_TEST)
			// This is a special unit-test notifier implementation UID.
			// It means that we can have the unit-test notifier and
			// the integration test notifier in the same ROM.
			iNotifierUid = TUid::Uid(0x10283742);
#else
			iNotifierUid = KLbsExtLocationRequestNotifyUid;
#endif

			// Start the notify RNotifier
			TInt err = iNotifier.StartNotifier(iNotifierUid, pckg);
			User::LeaveIfError(err);
			
			// Finish (cancel) the RNotifier
			iNotifier.CancelNotifier(iNotifierUid);
			iNotifierUid = TUid::Uid(0);
   			break;
			}
		case EPrivacyDialogVerification:
			{
#if defined(NRH_UNIT_TEST)
			// This is a special unit-test verifier implementation UID.
			// It means that we can have the unit-test verifier and
			// the integration test verifier in the same ROM.
			iNotifierUid = TUid::Uid(0x10283743);
#else
			iNotifierUid = KLbsExtLocationRequestVerifyUid;
#endif
			// Start the verify RNotifier
			iNotifier.StartNotifierAndGetResponse(iStatus, 
								iNotifierUid, 
								pckg, 
								iPrivacyResponsePckg);
								
			SetActive();			
			break;
			}
		default:
			{
			User::Leave(KErrArgument);
			break;
			}
		}
	}

