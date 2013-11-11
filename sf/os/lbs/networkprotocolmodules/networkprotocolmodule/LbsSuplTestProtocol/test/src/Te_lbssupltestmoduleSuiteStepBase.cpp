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
 @file Te_lbssupltestmoduleSuiteStepBase.cpp
*/

#include "Te_lbssupltestmoduleSuiteStepBase.h"
#include "Te_lbssupltestmoduleSuiteDefs.h"

const TTimeIntervalMicroSeconds32 KWait4seconds(1000000); // 4 seconds

// Device driver constants

TVerdict CTe_lbssupltestmoduleSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPreambleL() of the class CTe_lbssupltestmoduleSuiteStepBase!"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_lbssupltestmoduleSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPostambleL() of the class CTe_lbssupltestmoduleSuiteStepBase!"));
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_lbssupltestmoduleSuiteStepBase::~CTe_lbssupltestmoduleSuiteStepBase()
	{
	}

CTe_lbssupltestmoduleSuiteStepBase::CTe_lbssupltestmoduleSuiteStepBase(CTe_lbssupltestmoduleSuite* aServer):
	iServer(aServer)
	{
	}


CAssistanceDataRetriever* CAssistanceDataRetriever::NewL(MTestStepObserver& aTestObserver, const TInt& aMaxNumberRequests)
	{
	CAssistanceDataRetriever* self= new(ELeave) CAssistanceDataRetriever(aTestObserver,aMaxNumberRequests);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CAssistanceDataRetriever::DoCancel()
{
	
}

void CAssistanceDataRetriever::ConstructL()
	{
	iSet.OpenL();

	//Load the Supl Module
	//
	TRAPD(err, iSuplModule = CAssistanceDataSourceModuleBase::NewL(*this));
    if (err != KErrNone)
    	{
  		// here
  		iTestObserver.notifyResult(err);
    	}

	// Instantiate a SUPL session
	if (iSuplModule != NULL)
		{
		iSuplSession = iSuplModule->NewDataSourceL();
		}
	else
		{
		// assistDataSource doesn't exist
 		iTestObserver.notifyResult(KErrNotFound);
 		User::Leave(KErrNotFound);
		}
		
	if (iSuplSession == NULL)
		{
		//suplSession doesn't exist
		iTestObserver.notifyResult(KErrNotFound);
		User::Leave(KErrNotFound);
		}
		
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	
	//Default position for which Assistance Data is obtained is London's
	//
	iPosition.SetCoordinate(57,0);
	}

CAssistanceDataRetriever::CAssistanceDataRetriever(MTestStepObserver& aTestObserver,const TInt& aMaxNumberRequests) : CActive(EPriorityStandard),
	iTestObserver(aTestObserver),
	iMaxNumberRequests(aMaxNumberRequests),
	iDataSetsReceived(0)
	{
	// Attach to active scheduler
	//
	CActiveScheduler::Add(this);
	};


CAssistanceDataRetriever::~CAssistanceDataRetriever()
{
	if (iSuplSession != NULL)
		{
		delete iSuplSession;
		iSuplSession = NULL;
		}
	if(iTimer)
		{
		delete iTimer;
		}		
	iSet.Close();
}

void CAssistanceDataRetriever::RunL()
	{
	// RunL just requests assistance data
	// from the SUPL session a number of times
	// (iMaxNumberRequests)
	if (iStatus == KErrNone)
		{
		iDataSetsReceived++;
		
		if (iDataSetsReceived < iMaxNumberRequests)
			{
			GetTheAssistanceData(iAssistanceDataFilter);
			}
		else
			{
			// Got all the sets of assistance data
			// we wanted, delete SUPL session
			delete iSuplSession;
			iSuplSession = NULL;
    		// Stop Active Scheduler
    		CActiveScheduler::Stop();		
			}
		}
	else
		{
		// If there's been an error stop.
		delete iSuplSession;
		iSuplSession = NULL;
    	// Stop Active Scheduler
    	CActiveScheduler::Stop();	
		}		
	}
	
void CAssistanceDataRetriever::DataSourceNotification(const TInt& aError)
	{

	TRequestStatus* localStatus = &iStatus;

    if(aError != KErrNone)
    	{
    	iTestObserver.notifyResult(aError);
    	User::RequestComplete(localStatus, aError);
    	}
	else
		{
		if(iDataSetsReceived < 2)
			{
			// The first 2 times Assistance Data is notified,
			// complete self immediately. This has the effect
			// of requesting again Assitance Data straigth away.
			//  
			User::RequestComplete(localStatus, KErrNone);	
			}
		else
			{
			// After the second request is notified change the timing of the 
			// subsequent requests.
			// Wait 4 seconds before self-completing to allow
			// SUPL state machine to reach state EConnectedAwaitingRequest
			// before a new set of assistance data is requested.
			iTimer->Start(KWait4seconds, KWait4seconds, TCallBack(StopWaitingForTimer, this));			
			}
		}
	}
	
TInt CAssistanceDataRetriever::StopWaitingForTimer(TAny* aPtr)
	{
	CAssistanceDataRetriever* retriever = static_cast<CAssistanceDataRetriever*>(aPtr);
	if (retriever)
		{
		retriever->DoStopWaitingForTimer();
		}
	return KErrNone;
	}


void CAssistanceDataRetriever::DoStopWaitingForTimer()
	{
	iTimer->Cancel();
	//Complete self
	TRequestStatus* localStatus = &iStatus;
	User::RequestComplete(localStatus, KErrNone);	
	}

void CAssistanceDataRetriever::GetTheAssistanceData(const TLbsAssistanceDataGroup & aAssistanceDataFilter)
{

	iAssistanceDataFilter = aAssistanceDataFilter;
	
    // Ask the session to provide assistance data
	iStatus = KRequestPending;
	SetActive();
	iSuplSession->GetAssistanceData(iSet, iAssistanceDataFilter, iPosition);
}

void CAssistanceDataRetriever::	SetPosition(const TCoordinate& aPosition)
{
	iPosition = aPosition;
}

