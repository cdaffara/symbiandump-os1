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
#include "LbsPrivacyNotifierImpl.h"

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
	: CActive(EPriorityNormal)
	{
	iConnected = EFalse;
	}

/*
2nd phase Constructor
*/
void CLbsPrivacyNotifierImpl::ConstructL(TPrivacyDialogType aDlg)
	{
	User::LeaveIfError(iNotifier.Connect());
	CActiveScheduler::Add(this);
	iConnected=ETrue;
	iDialog = aDlg;
	}

/*
2nd phase Constructor
*/
void CLbsPrivacyNotifierImpl::ConstructL(TPrivacyDialogType aDlg, TTimeIntervalMicroSeconds32 aTimeout)
	{
	// for example: User::LeaveIfError(iNotifier.Connect());
	User::LeaveIfError(iNotifier.Connect());
	CActiveScheduler::Add(this);
	iConnected=ETrue;
	iDialog = aDlg;
	iTimeout = aTimeout;
	}

/*
Destructor
*/
CLbsPrivacyNotifierImpl::~CLbsPrivacyNotifierImpl()
	{
	iNotifier.Close();
	}

/*
Dummy implementation
*/
void CLbsPrivacyNotifierImpl::DoCancel()
	{
	//iNotifier.CancelNotifier(/*uid*/);
	}
	

/*
Connect to RNotifier
*/
TInt CLbsPrivacyNotifierImpl::Connect()
	{
	TInt err=KErrNone;
	TRAP(err, User::LeaveIfError(iNotifier.Connect());)	
	return(err);
	}

/*
Process the user's reply
*/
void CLbsPrivacyNotifierImpl::RunL()
	{
	// Here when user dismisses dialogue or when timeout
	
	if (iObserver)
		{
		iObserver->OnNotificationDialogResponse(0, (iButtonVal ? EResponseRejected : EResponseAccepted));
		}
	
	CActiveScheduler::Stop();
	}

	
/*
Observer
*/
void CLbsPrivacyNotifierImpl::SetObserver(MLbsPrivacyNotifierObserver* aObserver)
	{
	iObserver = aObserver;
	}


void CLbsPrivacyNotifierImpl::DisplayL(TInt& /*aErr*/, const TLbsExternalRequestInfo& /*aRequestInfo*/)
	{
	switch (iDialog)
		{
		case EPrivacyDialogNotification:
			{
    		// present user a dialogue notify that position will be divulged 
    		// implemented,for example, using RNotifier::Notify async request 
			_LIT(KText1, "Test Dialog");
			_LIT(KText2, "This is a Test Message.");
			_LIT(KBut1, "Y");
			_LIT(KBut2, "");
			iNotifier.Notify(KText1, KText2, KBut1, KBut2, iButtonVal, CActive::iStatus);
   			break;
			}
		case EPrivacyDialogVerification:
			{
	    	// present user a dialogue asking Verify - Yes or No? 
	    	// implemented,for example, using RNotifier::Notify async request   	
			_LIT(KText1, "Test Dialog");
			_LIT(KText2, "This is a Test");
			_LIT(KBut1, "Y");
			_LIT(KBut2, "N");
			iNotifier.Notify(KText1, KText2, KBut1, KBut2, iButtonVal, CActive::iStatus);
			break;
			}
		default:
			{
			User::Leave(KErrArgument);
			break;
			}
		}
    	
	SetActive();    
   
    // if this is a timed dialog, start a timer
	/*if (iTimeout > TTimeIntervalMicroSeconds32(0))
		{
		User::LeaveIfError(iTimer.CreateLocal());
	
		iStatus = KRequestPending;
		iTimer.After(iStatus, iTimeout);
	
		SetActive();
		}*/

	}
