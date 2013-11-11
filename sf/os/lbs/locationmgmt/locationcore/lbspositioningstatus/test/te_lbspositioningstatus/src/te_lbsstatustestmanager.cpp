// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_lbsstatusstep.cpp
 @internalComponent
*/

#include "te_lbsstatustestmanager.h"
#include "te_lbspositioningstatusstep.h"
#include "lbsrootcenrepdefs.h"
#include "lbspositioningstatusprops.h"
#include <centralrepository.h>
#include <lbs/lbspositioningstatus.h>
#include <e32property.h>

const TTimeIntervalMicroSeconds32 KTimerInterval = 2*1000*1000; 

CTe_LbsStatusTestManager* CTe_LbsStatusTestManager::NewL(CTe_LbsPositioningStatusStep& aTest)
    {
    CTe_LbsStatusTestManager* self = new(ELeave) CTe_LbsStatusTestManager(aTest);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CTe_LbsStatusTestManager::~CTe_LbsStatusTestManager()
    {
	delete iPositioningStatus;
	iTimer.Cancel();
	iTimer.Close();
    }

void CTe_LbsStatusTestManager::StartTest()
    {
	iUpdateExpected = EFalse;
    CompleteSelf();
    CActiveScheduler::Start();
    }

CTe_LbsStatusTestManager::CTe_LbsStatusTestManager(CTe_LbsPositioningStatusStep& aTest)
    : CActive(EPriorityStandard), iTest(aTest), iStep(EStepInitial)
    {
    CActiveScheduler::Add(this);
    }

void CTe_LbsStatusTestManager::ConstructL()
    {
    iPositioningStatus = CLbsPositioningStatus::NewL(*this);
    CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
    TInt posStatusCat;
    User::LeaveIfError(rep->Get(KMoPositioningStatusAPIKey, posStatusCat));
    iMoPosStatusCategory = TUid::Uid(posStatusCat);
    User::LeaveIfError(rep->Get(KNiPositioningStatusAPIKey, posStatusCat));
    iNiPosStatusCategory = TUid::Uid(posStatusCat);
    CleanupStack::PopAndDestroy(rep);
    iTimer.CreateLocal();
    }

void CTe_LbsStatusTestManager::RunL()
    {
	iTest.TESTL(!iTestFailed);
    switch(iStep)
        {
        case EStepInitial:
            {
            // checks that the current status is not active
            iTest.INFO_PRINTF1(_L("Checking that initial status is not active"));
            iTest.TESTL(CLbsPositioningStatus::ELbsPositioningStatusNotActive == iPositioningStatus->CurrentPositioningStatus());
            // sets the MO-LR property to 1...
            iTest.INFO_PRINTF1(_L("Setting the MO-LR property to 1 (expect update)"));
            iTest.TESTL(KErrNone == RProperty::Set(iMoPosStatusCategory, KLbsMoPositioningStatusKey, 1));
            iStep = EStepFirstRequest;
            iUpdateExpected = ETrue;
            iPosStatusUpdateReceived = EFalse;
            iTimer.After(iStatus, KTimerInterval);
            SetActive();
            break;
            }
        case EStepFirstRequest:
            {
            // ...and expects to be notified with status active
            iTest.INFO_PRINTF1(_L("Checking that status now active"));
            iTest.TESTL(iPosStatusUpdateReceived);
            iTest.TESTL(iReceivedPosStatus == CLbsPositioningStatus::ELbsPositioningStatusActive);
            iTest.TESTL(CLbsPositioningStatus::ELbsPositioningStatusActive == iPositioningStatus->CurrentPositioningStatus());
            // sets the NI property to 1 as well (so now both are 1)...
            iTest.INFO_PRINTF1(_L("Setting the NI property to be 1 (don't expect update)"));
            iTest.TESTL(KErrNone == RProperty::Set(iNiPosStatusCategory, KLbsNiPositioningStatusKey, 1));
            iStep = EStepSecondRequest;
            iUpdateExpected = EFalse;
            iPosStatusUpdateReceived = EFalse;
            iTimer.After(iStatus, KTimerInterval);
            SetActive();            
            break;
            }
        case EStepSecondRequest:
            {
            // ...and expects not to be notified and status should be active
            iTest.INFO_PRINTF1(_L("Checking that the update wasn't received and that the status is active"));
            iTest.TESTL(!iPosStatusUpdateReceived);
            iTest.TESTL(CLbsPositioningStatus::ELbsPositioningStatusActive == iPositioningStatus->CurrentPositioningStatus());            
            // sets both the MO-LR and the NI properties to 0...
            iTest.INFO_PRINTF1(_L("Setting both MO-LR and NI properties to 0 (expect update)"));
            iTest.TESTL(KErrNone == RProperty::Set(iMoPosStatusCategory, KLbsMoPositioningStatusKey, 0));
            iTest.TESTL(KErrNone == RProperty::Set(iNiPosStatusCategory, KLbsNiPositioningStatusKey, 0));
            iStep = EStepThirdRequest;
            iUpdateExpected = ETrue;
            iPosStatusUpdateReceived = EFalse;
            iTimer.After(iStatus, KTimerInterval);
            SetActive();            
            break;
            }
        case EStepThirdRequest:
            {
            // ...and expects to be notified with status not active
            iTest.INFO_PRINTF1(_L("Checking update was received and status is not active"));
            iTest.TESTL(iPosStatusUpdateReceived);
            iTest.TESTL(iReceivedPosStatus == CLbsPositioningStatus::ELbsPositioningStatusNotActive);
            iTest.TESTL(CLbsPositioningStatus::ELbsPositioningStatusNotActive == iPositioningStatus->CurrentPositioningStatus());
            // sets the NI property to 1...
            iTest.INFO_PRINTF1(_L("Setting NI property to 1 (expect update)"));
            iTest.TESTL(KErrNone == RProperty::Set(iNiPosStatusCategory, KLbsNiPositioningStatusKey, 1));
            iStep = EStepFourthRequest;
            iUpdateExpected = ETrue;
            iPosStatusUpdateReceived = EFalse;
            iTimer.After(iStatus, KTimerInterval);
            SetActive();            
            break;
            }
        case EStepFourthRequest:
        	{
            // ...and expects to be notified with status active
            iTest.INFO_PRINTF1(_L("Checking update was received and status is now active"));
            iTest.TESTL(iPosStatusUpdateReceived);
            iTest.TESTL(iReceivedPosStatus == CLbsPositioningStatus::ELbsPositioningStatusActive);
            iTest.TESTL(CLbsPositioningStatus::ELbsPositioningStatusActive == iPositioningStatus->CurrentPositioningStatus());
            // sets the NI property to 2...
            iTest.INFO_PRINTF1(_L("Setting NI property to 2 (don't expect update)"));
            iTest.TESTL(KErrNone == RProperty::Set(iNiPosStatusCategory, KLbsNiPositioningStatusKey, 2));
            iStep = EStepFifthRequest;
            iUpdateExpected = EFalse;
            iPosStatusUpdateReceived = EFalse;
            iTimer.After(iStatus, KTimerInterval);
            SetActive();
            break;
        	}
        case EStepFifthRequest:
        	{
        	// ...and expects no update and status to be active
            iTest.INFO_PRINTF1(_L("Checking no update was received and status is still active"));
            iTest.TESTL(!iPosStatusUpdateReceived);
            iTest.TESTL(CLbsPositioningStatus::ELbsPositioningStatusActive == iPositioningStatus->CurrentPositioningStatus());            
            // sets the NI property to 0...
            iTest.INFO_PRINTF1(_L("Setting the NI property to 0 (expect update)"));
            iTest.TESTL(KErrNone == RProperty::Set(iNiPosStatusCategory, KLbsNiPositioningStatusKey, 0));
            iStep = EStepStop;
            iUpdateExpected = ETrue;
            iPosStatusUpdateReceived = EFalse;
            iTimer.After(iStatus, KTimerInterval);
            SetActive();            
            break;
        	}
        case EStepStop:
            {
            // ...and expects to be notified with status not active
            iTest.INFO_PRINTF1(_L("Checking that an update was received and the status is now not active"));
            iTest.TESTL(iPosStatusUpdateReceived);
            iTest.TESTL(iReceivedPosStatus == CLbsPositioningStatus::ELbsPositioningStatusNotActive);
            iTest.TESTL(CLbsPositioningStatus::ELbsPositioningStatusNotActive == iPositioningStatus->CurrentPositioningStatus());
            CActiveScheduler::Stop();
            break;
            }
        default:
            {
            iTest.INFO_PRINTF1(_L("Unexpected step setting"));
            iTest.TESTL(EFalse);
            }
        }
    }

void CTe_LbsStatusTestManager::OnPositioningStatusUpdate(const CLbsPositioningStatus::TLbsPositioningStatus& aPositioningStatus)
    {
	Cancel();
    iTest.INFO_PRINTF1(_L("Received a status update"));
    if(!iUpdateExpected)
        {
        iTest.INFO_PRINTF1(_L("It was an unexpected update, test failed"));
        iTestFailed = ETrue;
        }
    else
        {
		switch(iStep)
			{
	        case EStepInitial:
			case EStepFirstRequest:
            case EStepThirdRequest:
            case EStepFourthRequest:
            case EStepStop:
				{
				iPosStatusUpdateReceived = ETrue;
				iReceivedPosStatus = aPositioningStatus;
				break;
				}
			case EStepSecondRequest:
			case EStepFifthRequest:
			default:
				{
		        iTest.INFO_PRINTF1(_L("Update received when in the wrong test step"));
		        iTestFailed = ETrue;
				}
			}
        
        }
    iUpdateExpected = EFalse;
    CompleteSelf();
    }

TInt CTe_LbsStatusTestManager::RunError(TInt /*aError*/)
    {
    iTest.INFO_PRINTF1(_L("Test failed, stopping"));
    CActiveScheduler::Stop();
    return KErrNone;
    }

void CTe_LbsStatusTestManager::DoCancel()
	{
	iTimer.Cancel();
	}


void CTe_LbsStatusTestManager::CompleteSelf()
    {
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);    
    }

// end of file

