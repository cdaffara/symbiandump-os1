/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    Implementation for the CDosEventNotifier class.
*
*/


#include <DosSvrServices.h>

//
// ---------------------------------------------------------
// CDosEventNotifier Constructor
// ---------------------------------------------------------
//  

CDosEventNotifier::CDosEventNotifier(CDosEventListenerBase* aListener,TPriority aPriority)
: CActive(aPriority)
{
	CActiveScheduler::Add(this);
	iListener = aListener;
}

//
// ---------------------------------------------------------
// CDosEventNotifier Destructor
// ---------------------------------------------------------
//  

CDosEventNotifier::~CDosEventNotifier()
{
	Cancel();
	iReceiver.Close();
}

//
// ---------------------------------------------------------
// CDosEventNotifier::NewL
// ---------------------------------------------------------
//  

CDosEventNotifier* CDosEventNotifier::NewL(RDosServer& aServer,CDosEventListenerBase* aListener)
{
	CDosEventNotifier* result = new(ELeave) CDosEventNotifier(aListener);

	CleanupStack::PushL(result);
	result->ConstructL(aServer);
	CleanupStack::Pop();

	return result;
}

//
// ---------------------------------------------------------
// CDosEventNotifier::ConstructL
// ---------------------------------------------------------
//  

void CDosEventNotifier::ConstructL(RDosServer& aServer)
{
	User::LeaveIfError(iReceiver.Open(aServer));
}

//
// ---------------------------------------------------------
// CDosEventNotifier::RunL
// ---------------------------------------------------------
//  

void CDosEventNotifier::RunL()
{
	if(iStatus.Int()==KErrNone)
	{
		TRegisterEvent currentEvent = {iReceiver.Event(),iReceiver.ParameterSize(),iReceiver.Queue()};

		iListener->HandleEventL(currentEvent,/*par*/ iReceiver.Parameter());
	
		iReceiver.WaitEvent(iStatus);
		SetActive();
	}
	else
		User::Leave(iStatus.Int());
}

//
// ---------------------------------------------------------
// CDosEventNotifier::RunError
// ---------------------------------------------------------
//  

TInt CDosEventNotifier::RunError(TInt aError)
{
	TBool stopListening = EFalse;

	iListener->OnError(aError,stopListening);
	
	if(aError!=KErrServerTerminated && !stopListening)
	{
	   iReceiver.WaitEvent(iStatus);
	   SetActive();
	}

	return KErrNone;
}

//
// ---------------------------------------------------------
// CDosEventNotifier::DoCancel
// ---------------------------------------------------------
//  

void CDosEventNotifier::DoCancel()
{
	iReceiver.CancelWaitEvent();
	iReceiver.UnRegister(); // No need to check if it's registered or not.
}

//
// ---------------------------------------------------------
// CDosEventNotifier::StartListeningL
// ---------------------------------------------------------
//  

void CDosEventNotifier::StartListeningL(TUint aEvent,TInt aParamSize,TQueueType aQueue)
{
	User::LeaveIfError(iReceiver.Register(aEvent,aParamSize,aQueue));
	iReceiver.WaitEvent(iStatus);
	
	SetActive();
}


