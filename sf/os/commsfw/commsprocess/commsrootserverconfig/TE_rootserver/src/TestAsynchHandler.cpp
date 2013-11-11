// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestAsynchHandler which is an active object to
// handle the asynch calls from the RootServer during testing



// EPOC includes
#include <e32base.h>

// RootServer includes
#include "rsshared.h"

// Test system includes
#include "TestAsynchHandler.h"


CTestAsynchHandler* CTestAsynchHandler::NewLC(RRootServ* aRootServer)
	{
	CTestAsynchHandler* self = new(ELeave) CTestAsynchHandler;
	CleanupStack::PushL(self);
	self->ConstructL(aRootServer);
	return self;
	}

CTestAsynchHandler::CTestAsynchHandler() : CActive(0)
	{
	}

void CTestAsynchHandler::ConstructL(RRootServ* aRootServer)
	{
	iState = EIdle;
	iRootServer = aRootServer;
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}

CTestAsynchHandler::~CTestAsynchHandler()  
	{
	Cancel();
	iTimer.Close();
	}

// from CActive
void CTestAsynchHandler::RunL()     
	{ 
	switch(iState)
		{
		case ELoading:
		case EUnLoading:
		case EBinding:
		case EUnBinding:
		case EListeningforDeath:
		case EWaitingforTimer:
		case EIdle:
		default:
			CActiveScheduler::Stop();
			break;
		}
	iState = EIdle;
	}

void CTestAsynchHandler::DoCancel() 
	{
	switch (iState)
		{
		case EListeningforDeath:
			iPropertyDeath.Cancel();
			break;

		case EWaitingforTimer:
			iTimer.Cancel();
			break;

		case ELoading:
		case EIdle:
		default:
			break;
		}
	}


// requests
void CTestAsynchHandler::TryLoad(const TRSStartModuleParams& aParams, 
									   const TDesC8& aIniData)
	{
    iState = ELoading;
	iRootServer->LoadCpm(iStatus, aParams, aIniData);
	SetActive();
	}

void CTestAsynchHandler::TryUnLoad(const TCFModuleName& aName, TRSUnLoadType aType)
	{
  	iState = EUnLoading;
	iRootServer->UnloadCpm(iStatus, aName, aType);
	SetActive();
	}

void CTestAsynchHandler::TryBind( TRSBindingInfo& aBindInfo )
	{
	iState = EBinding;
	iRootServer->Bind( iStatus, aBindInfo );
	SetActive();
	}
	
void CTestAsynchHandler::TryUnBind( TRSUnBindingInfo& aUnbindInfo )
	{
	iState = EUnBinding;
	iRootServer->Unbind( iStatus, aUnbindInfo );
	SetActive();
	}

void CTestAsynchHandler::TryWaitForDeath(void)
	{
	iState = EListeningforDeath;
	TInt result = iPropertyDeath.Attach( KUidSystemCategory,KUidC32RootModuleDeathKey.iUid );
	if(result != KErrNone)
		{
		_LIT( KAsynch, "Asynch" );
		User::Panic( KAsynch, result );
		}
	iPropertyDeath.Subscribe( iStatus );
	SetActive();
	}

void CTestAsynchHandler::TryCancelDeath()
	{

	// we are already active cos we are waiting on our subscription
	// therefore we will simply cancel the property subscription
	iState=EIdle;
	DoCancel();	
	}

void CTestAsynchHandler::WaitForTimer(TInt aTimeout_ms)
	{
	iState = EWaitingforTimer;

	DoCancel(); // in case timer already running
	iTimer.After(iStatus, aTimeout_ms * 1000);
	SetActive();
	}


