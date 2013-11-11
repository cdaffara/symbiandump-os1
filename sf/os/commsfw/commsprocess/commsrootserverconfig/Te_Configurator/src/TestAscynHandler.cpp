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
// Example CTestStep derived implementation
// 
//

/**
 @file TestAscynHandler.cpp
*/

#include "TestAscynHandler.h"

//Cancelling the Loading  CPM before and after loading
CCanceledLoadCpm* CCanceledLoadCpm::NewL()
	{
	CCanceledLoadCpm* self = new(ELeave) CCanceledLoadCpm;
	CleanupStack::PushL(self);
    self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

	
CCanceledLoadCpm::CCanceledLoadCpm()
: CActive(CActive::EPriorityStandard)
	{
	}

void CCanceledLoadCpm::ConstructL()
	{
	iState = EIdle;
    iConfigurator.Connect();
    CActiveScheduler::Add(this);
    }

CCanceledLoadCpm::~CCanceledLoadCpm()  
	{
	Cancel();
	iTimer.Close();
	iConfigurator.Close();
	}

void CCanceledLoadCpm::RunL()     
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

void CCanceledLoadCpm::DoCancel()   
	{
	switch (iState)
		{
		case EListeningforDeath:
			iPropertyDeath.Cancel();
			break;

		case EWaitingforTimer:
			iTimer.Cancel();
			break;

		case EIdle:		
		default:
			break;
		}
	}
	

// requests
void CCanceledLoadCpm::TryLoad(const TCFModuleName& aName)
	{
	iState = ELoading;
	iName = aName;
	iConfigurator.LoadCpm(aName, iStatus);
	SetActive();
	}

void CCanceledLoadCpm::TryUnLoad(const TCFModuleName& aName, TRSUnLoadType aType)
	{
  	iState = EUnLoading;
	iConfigurator.UnloadCpm(aName, aType, iStatus);
	SetActive();
	}

void CCanceledLoadCpm::TryWaitForDeath(void)
	{
	iState = EListeningforDeath;
	TInt result = iPropertyDeath.Attach( KUidSystemCategory,KUidC32RootModuleDeathKey.iUid );
	if(result != KErrNone)
		{
		_LIT( KAsync, "Async" );
		User::Panic( KAsync, result );
		}
	iPropertyDeath.Subscribe( iStatus );
	SetActive();
	}

void CCanceledLoadCpm::TryCancelDeath()
	{

	// we are already active cos we are waiting on our subscription
	// therefore we will simply cancel the property subscription
	iState=EIdle;
	DoCancel();	
	}

void CCanceledLoadCpm::WaitForTimer(TInt aTimeout_ms)
	{
	iState = EWaitingforTimer;

	DoCancel(); // in case timer already running
	iTimer.After(iStatus, aTimeout_ms * 1000);
	SetActive();
	}


