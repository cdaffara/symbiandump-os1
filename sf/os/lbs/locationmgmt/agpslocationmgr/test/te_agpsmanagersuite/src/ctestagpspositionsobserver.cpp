// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ctestagpsupdatesobserver.cpp
// GPS Integration Module Observer for AGPS Manager UNIT testing
// 
//


#include "ctestagpspositionsobserver.h"


CTestAgpsUpdatesObserver* CTestAgpsUpdatesObserver::NewL()
	{
	CTestAgpsUpdatesObserver* self = new(ELeave) CTestAgpsUpdatesObserver();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestAgpsUpdatesObserver::ConstructL()
	{
	CTestObserverBase::ConstructL();
	iUpdatesListener = CTestUpdatesListener::NewL(*this);
	}

CTestAgpsUpdatesObserver::CTestAgpsUpdatesObserver()
	{	
	}

CTestAgpsUpdatesObserver::~CTestAgpsUpdatesObserver()
	{
	delete iUpdatesListener;
	}

TBool CTestAgpsUpdatesObserver::IsLocationUpdateAvailable()
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(/*locationupdate*/))
		{
		ret = ETrue;
		}
	return ret;	
	}

TBool CTestAgpsUpdatesObserver::CheckCallbackObserved()
	{
	TBool ret = EFalse;
	if (WasObserverCalled())
		{	
	 	CallbackHandled();
	 	ret = ETrue;
		}
	return ret;	
	}

void CTestAgpsUpdatesObserver::UpdateAvailable()
	{
	// This method doesn't do much at the moment.
	// In the future it may receive and store the 
	// update and associated info (if new tests
	// require that info)	
	SignalObserverCallBack(ECallBackErrorNone);
	}
	


CTestUpdatesListener::CTestUpdatesListener(MTestUpdateListenerObserver& aUpdateObserver) : CActive(EPriorityStandard),
iUpdateObserver(aUpdateObserver)
	{
	CActiveScheduler::Add(this);
	}

// aChannel observer will receive the messages from the AGPS Manager
CTestUpdatesListener* CTestUpdatesListener::NewL(MTestUpdateListenerObserver& aUpdateObserver)
	{
	CTestUpdatesListener* self = new(ELeave) CTestUpdatesListener(aUpdateObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestUpdatesListener::ConstructL()
	{
	iPosUpdates.OpenL(KLbsGpsLocManagerUid);
	// start listening for updates
	iPosUpdates.NotifyPositionUpdate(iStatus);	
	SetActive();
	}
	
CTestUpdatesListener::~CTestUpdatesListener()
	{
	Cancel();
	iPosUpdates.Close();
	}

void CTestUpdatesListener::RunL()
	{
	ASSERT(KErrNone == iStatus.Int());
	
	// Update available...continue to listen for new updates
	iPosUpdates.NotifyPositionUpdate(iStatus);
	SetActive();
	
	// Let the observer now that an update is available
	iUpdateObserver.UpdateAvailable();
	}

TInt CTestUpdatesListener::RunError(TInt aError)
	{
	// Just panic
	ASSERT(EFalse);
	return aError;
	}

void CTestUpdatesListener::DoCancel()
	{
	iPosUpdates.CancelNotifyPositionUpdate();
	}
