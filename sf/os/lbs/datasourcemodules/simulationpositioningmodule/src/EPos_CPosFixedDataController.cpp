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



// INCLUDE FILES
#include "EPos_CPosFixedDataController.h"

const TReal64 KDefaultLatitude = 0.0;
const TReal64 KDefaultLongitude = 0.0;
const TReal32 KDefaultAltitude = 0.0;
const TReal32 KDefaultHorzAccuracy = 0.0;
const TReal32 KDefaultVertAccuracy = 0.0;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosFixedDataController::CPosFixedDataController():CPosControllerBase(EFalse)
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CPosFixedDataController::ConstructL()
    {
    // Left empty intentionally
    }

// Two-phased constructor.
CPosFixedDataController* CPosFixedDataController::NewL()
    {
    CPosFixedDataController* self = new (ELeave) CPosFixedDataController();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosFixedDataController::~CPosFixedDataController()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// CPosFixedDataController::NotifyPositionUpdate
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosFixedDataController::NotifyPositionUpdate(
    TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus,
    CPosSimulationPositioner& aSimPos
    )
    {
    aStatus = KRequestPending;
    NotifyPositionUpdateBase(aPosInfo, aStatus, aSimPos);
    
	// Here we set the default values
    TPosition pos;
    
    pos.SetCoordinate(KDefaultLatitude,KDefaultLongitude,KDefaultAltitude);
    pos.SetAccuracy(KDefaultHorzAccuracy,KDefaultVertAccuracy);
    pos.SetDatum(KPositionDatumWgs84);
    pos.SetCurrentTime();

	// Now we set the     
    iPosition.SetPosition(pos);
    // Complete the request which will result in the RunL being called.        
    if(!IsActive())
    	{
    	TRequestStatus* status = &iStatus;
    	iStatus = KErrNone;
		User::RequestComplete(status, KErrNone);
		SetActive();
    	}
    }

// ----------------------------------------------------------------------------
// CPosFixedDataController::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosFixedDataController::CancelNotifyPositionUpdate()
    {
    if (IsActive())
        {
        CompleteRequest(KErrCancel);
        Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CPosFixedDataController::CompleteRequest
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosFixedDataController::CompleteRequest(TInt aErrorCode)
    {
    CompleteRequestBase(aErrorCode);
    }

// ----------------------------------------------------------------------------
// CPosFixedDataController::RunL
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosFixedDataController::RunL()
    {
    CompleteRequest(iStatus.Int());
    }

// ----------------------------------------------------------------------------
// CPosFixedDataController::DoCancel
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosFixedDataController::DoCancel()
    {
    }

//  End of File
