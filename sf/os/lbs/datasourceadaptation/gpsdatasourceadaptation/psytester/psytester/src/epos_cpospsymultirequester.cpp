// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include "epos_cpospsymultirequester.h"
#include "epos_cpospsyrequester.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYMultiRequester::CPosPSYMultiRequester() : CActive(EPriorityHigh)
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CPosPSYMultiRequester::ConstructL(
    MPosRequestObserver* aParent) 
    {
	iParent = aParent;
    User::LeaveIfError(iTimer.CreateLocal());
    }

// Two-phased constructor.
CPosPSYMultiRequester* CPosPSYMultiRequester::NewL(
	MPosRequestObserver* aParent) 
    {
    CPosPSYMultiRequester* self = new (ELeave) CPosPSYMultiRequester;
    CleanupStack::PushL(self);
    self->ConstructL(aParent);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYMultiRequester::~CPosPSYMultiRequester()
    {
    Cancel();
    iTimer.Close();
    iRequesters.ResetAndDestroy();
    iRequesters.Close();
    iRequestersWhoWillCancel.Close();
    iResults.ResetAndDestroy();
    iResults.Close();
    }

// ---------------------------------------------------------
// CPosPSYMultiRequester::OpenPositionerL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::OpenPositionerL(
    const TUid aImplementationUid, 
	const TInt aRequestCount)
    {
	for (TInt i = 0; i < aRequestCount; i++)
        {
        CPosPSYRequester* requester = CPosPSYRequester::NewL(this);            
        CleanupStack::PushL(requester);
		requester->OpenPositionerL(aImplementationUid);
        User::LeaveIfError(iRequesters.Append(requester));
        CleanupStack::Pop(requester);
        }	
	}

// ---------------------------------------------------------
// CPosPSYMultiRequester::ClosePositioner
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::ClosePositioner()
    {
	Cancel();

	TInt requesters = iRequesters.Count();
	for (TInt i=0; i < requesters; i++)
		{
		TInt nr = iRequesters.Count() - 1;
		CPosPSYRequester* req = iRequesters[nr];
		req->ClosePositioner();
		delete req;
		req = NULL;
		iRequesters.Remove(nr);			
		}

	iRequestersWhoWillCancel.Reset();
	
	iResults.ResetAndDestroy();
	iCancelWasCompleted = 0;
	}

// ---------------------------------------------------------
// CPosPSYMultiRequester::MakeRequestsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::MakeRequestsL()
    {
    iNofRequestsDone = 0;

	// initiate cancelling if this is set.
    if (iRequestersWhoWillCancel.Count() > 0)
        {
        iTimer.After(iStatus, TTimeIntervalMicroSeconds32(1000));
        SetActive();
        }

    for (TInt i = 0; i < iRequesters.Count(); i++)
        {
        TPositionInfo* positionInfo = new (ELeave) TPositionInfo();
        CleanupStack::PushL(positionInfo);
        iRequesters[i]->MakeRequest(*positionInfo);
        User::LeaveIfError(iResults.Append(positionInfo));
        CleanupStack::Pop(positionInfo);
        }
    }

// ---------------------------------------------------------
// CPosPSYMultiRequester::GetResult
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::GetResult(
    TInt& aStatus, 
    TTimeIntervalMicroSeconds& aRequestTime,
    TPositionInfo& aInfo,
    TInt aRequesterIndex)
    {
    if ((aRequesterIndex < iRequesters.Count()) && 
       (aRequesterIndex < iResults.Count()))
        {
        iRequesters[aRequesterIndex]->GetResult(aStatus, aRequestTime);
        aInfo = *iResults[aRequesterIndex];
        }
    }

// ---------------------------------------------------------
// CPosPSYMultiRequester::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::HandlePositionCompleteL(
	TInt aCompleteStatus,
	TTimeIntervalMicroSeconds aRequestTime)
    {
    iNofRequestsDone++;

    if (aCompleteStatus == KErrCancel)
        {
        iCancelWasCompleted++;
        }

    if (iNofRequestsDone == iRequesters.Count())
        {
        Cancel();
        
		TRAPD(err, iParent->HandlePositionCompleteL(aCompleteStatus, aRequestTime));
		if (err != KErrNone)
			{
			HandleRequestError(err);
			}
		}
    }

// ---------------------------------------------------------
// CPosPSYMultiRequester::HandleRequestError
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::HandleRequestError(TInt aErrorCode)
    {
	iParent->HandleRequestError(aErrorCode);
	}

// ---------------------------------------------------------
// CPosPSYMultiRequester::AddTestResultL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::AddTestResultL(
	const TDesC& aMessage, 
    const TInt aResultType)
    {
	iParent->AddTestResultL(aMessage, aResultType);
	}

// ---------------------------------------------------------
// CPosPSYMultiRequester::SetRequesterWillCancelL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::SetRequesterWillCancelL(
    TInt aIndex, 
    TBool aBool)
    {
    TInt length = iRequestersWhoWillCancel.Count();
    for (TInt i = 0; i < length; i++)
        {
		if (iRequestersWhoWillCancel[i] == aIndex) 
			{
			if (!aBool)
				{
				iRequestersWhoWillCancel.Remove(i);
				return;
				}
			}
        }
    if (aBool)
		{
		User::LeaveIfError(iRequestersWhoWillCancel.Append(aIndex));
		}
    }

// ---------------------------------------------------------
// CPosPSYMultiRequester::GetNumberOfCanceledRequests
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYMultiRequester::GetNumberOfCanceledRequests()
    {
	return iCancelWasCompleted;
    }

// ---------------------------------------------------------
// CPosPSYMultiRequester::RunL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::RunL()
    {
    // Cancel certain requests.
    TInt nofRequestersWhoWillCancel = iRequestersWhoWillCancel.Count();
    for (TInt i = 0; i < nofRequestersWhoWillCancel; i++)
        {
        iRequesters[iRequestersWhoWillCancel[i]]->CancelRequest();
        }
    }

// ---------------------------------------------------------
// CPosPSYMultiRequester::DoCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMultiRequester::DoCancel()
    {
    iTimer.Cancel();
    }

// End of File
