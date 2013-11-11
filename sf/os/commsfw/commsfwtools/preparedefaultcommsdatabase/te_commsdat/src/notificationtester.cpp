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
//

/**
 @file notificationtester.cpp
*/
#include "notificationtester.h"
 
CNotificationTester* CNotificationTester::NewL(const TDesC &aThreadName, CommsDat::CMDBSession* aSession, CommsDat::CMDBElement* aElement)
	{
	CNotificationTester* self = new(ELeave) CNotificationTester(aSession, aElement);
	CleanupStack::PushL(self);
	self->ConstructL(aThreadName);
	CleanupStack::Pop(self);
	
	return self;
	}
	
CNotificationTester::CNotificationTester(CommsDat::CMDBSession* aSession, CommsDat::CMDBElement* aElement)
:CActive(0)
	{
	iSession = aSession;
	iElement = aElement;
	}

void CNotificationTester::ConstructL(const TDesC &aThreadName)
	{
	iThreadRunning = EFalse;

	iThread.Create(aThreadName, &ThreadFunction, KDefaultStackSize, NULL, this);
	TRequestStatus waitSchedulerInstallation;  		// Create a rendezvous to ensure that thread
	iThread.Rendezvous(waitSchedulerInstallation);  // completes installation of scheduler before ConstructL finishes
	iThread.Resume(); 
	
	User::WaitForRequest(waitSchedulerInstallation);
	}

CNotificationTester::~CNotificationTester()
	{
	Cancel();
	iThread.Close();
	}
	
void CNotificationTester::SetTester()
	{
	if(iThread.ExitReason() >0 )
		{
		iReason = iThread.ExitReason();
		return;
		}

	iElement->RequestNotification(*iSession, iStatus);
	SetActive();
	}
	
	
void CNotificationTester::LogOn(TRequestStatus &aStatus)
	{
	iThread.Logon(aStatus);
	}

void CNotificationTester::RunL()
	{
	CActiveScheduler::Stop();
	}

void CNotificationTester::DoCancel()
	{
	if((iElement != NULL) && (iSession != NULL))
		iElement->CancelNotification(*iSession, iStatus);
	}
	
void CNotificationTester::CancelNotification(TRequestStatus& /*aStatus*/)	
	{
	// Note: this method is not called in the same thread that originally created and
	// registered the CNotificationTester.  Consequently, issuing Cancel() here will perform
	// the DoCancel(), which will complete the CNotificationTester in the creating thread,
	// but then erroneously perform a User::WaitForRequest() in the *current* thread, swallowing
	// the next unsuspecting completion signal in the current thread.
	DoCancel();
	}

TInt CNotificationTester::ThreadFunction(TAny *aPtr)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	TRAPD(errCode, DoThreadFunctionL(aPtr));
	
	delete cleanup;
	return errCode;
	}
	
TInt CNotificationTester::DoThreadFunctionL(TAny *aPtr)
	{
	
	CActiveScheduler* scheduler=new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
    ((CNotificationTester *)aPtr)->iThreadRunning = ETrue;	
	CActiveScheduler::Add((CActive *)aPtr);
	
	CNotificationTester *tester = (CNotificationTester*)(aPtr);
	tester->SetTester();
	
	RThread::Rendezvous(KErrNone); // Signal the main thread that notification has been requested

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(scheduler);	
	
	return KErrNone;
	}

