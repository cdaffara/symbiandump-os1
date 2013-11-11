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
#include "EPos_CPosDataSimulationController.h"
#include "EPos_CPosFileHandler.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosDataSimController::CPosDataSimController(
		TBool aTimeRelativePlayback) :
    CPosControllerBase(aTimeRelativePlayback), iSimulationState(EPosCheckIfFailRequest), iSimulationInProcess(EFalse) 
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CPosDataSimController::ConstructL(const TDesC& aSimulationFile)
    {
    User::LeaveIfError(iTimer.CreateLocal());

    iSimDataArray = new (ELeave) CDesC8ArrayFlat(KNoOfSimulatedDataItems);

    CPosFileHandler* fileHandler = CPosFileHandler::NewL(aSimulationFile);
    CleanupStack::PushL(fileHandler);
    fileHandler->ReadL(*iSimDataArray);
    CleanupStack::PopAndDestroy(fileHandler);

    iDataSimulator.ParseAndCreatePosObjectL(iBasePosition, iSimDataArray);
    iSimulatedPosition = iBasePosition;
    }

// Two-phased constructor.
CPosDataSimController* CPosDataSimController::NewL(
    const TDesC& aSimulationFile,
    TBool aTimeRelativePlayback)
    {
    CPosDataSimController* self = new (ELeave) CPosDataSimController(aTimeRelativePlayback);
    CleanupStack::PushL(self);
    self->ConstructL(aSimulationFile);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosDataSimController::~CPosDataSimController()
    {
    Cancel();
    iTimer.Cancel();
    iTimer.Close();
    iRequestStatus = NULL;
    delete iSimDataArray;
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::NotifyPositionUpdate
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosDataSimController::NotifyPositionUpdate(
    TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus,
    CPosSimulationPositioner& aSimPos)
    {
    NotifyPositionUpdateBase(aPosInfo, aStatus, aSimPos);

    iNumRequests++;
    iRequestStartTime.UniversalTime();

    if (iSimulationInProcess)
        {
        SetCompletionCodeAndWait();
        }
    else if (!IsActive())
        {
        //this section is done only once at powerup time
        if(iTimeRelativePlayback)
        	{
        	//in TRP mode we are using this variable to 
        	//determinate begin of the simulation
        	//in TRP mode this variable one setuped is never changed
        	iLastRequestTime.UniversalTime();
        	}
        iTimer.Cancel();
        iTimer.After(iStatus,
            I64INT(iDataSimulator.PowerupTime().Int64()));
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CPosDataSimController::CancelNotifyPositionUpdate
//
// (other items were combmented in a header).
// ----------------------------------------------------------------------------
//
void CPosDataSimController::CancelNotifyPositionUpdate(const CPosSimulationPositioner& aSimPos)
    {
    CPosControllerBase::CancelNotifyPositionUpdate(aSimPos);
    if(!iTimeRelativePlayback)
    	{
    	Cancel();
    	}
    }

// ----------------------------------------------------------------------------
// CPosDataSimController::CompleteRequest
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosDataSimController::CompleteRequest(TInt aErrorCode)
    {
    CompleteRequestBase(aErrorCode);
    iSimulationState = EPosCheckIfFailRequest;    
    }

// ----------------------------------------------------------------------------
// CPosDataSimController::SetCompletionCodeAndWait
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosDataSimController::SetCompletionCodeAndWait()
    {
    if(!IsActive())
    	{
		iCompleteCode = iDataSimulator.FailThisRequest(iNumRequests);
	
		iSimulationState = EPosTransferAndComplete;
		iTimer.Cancel();
		iTimer.After(iStatus,
			I64INT(iDataSimulator.TimeToWait(iRequestStartTime).Int64()));
		SetActive();
    	}
    }

// ----------------------------------------------------------------------------
// CPosDataSimController::RunL
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosDataSimController::RunL()
    {
    if (iSimulationState == EPosCheckIfFailRequest)
        {
        SetCompletionCodeAndWait();
        }
    else
        {
        // Powerup has been done
        if (!iSimulationInProcess)
            {
            iSimulationInProcess = ETrue;
            }
        else if (iCompleteCode == KErrNone)
            {
            if (iTimeRelativePlayback)
            	{
            	iSimulatedPosition = iBasePosition;
            	}
            iDataSimulator.ComputeNewPositionL(iSimulatedPosition,
                iRequestStartTime, iLastRequestTime);
            }
        if(!iTimeRelativePlayback)
        	{
        	iLastRequestTime = iRequestStartTime;
        	}
        iSimulatedPosition.SetCurrentTime();
        iPosition.SetPosition(iSimulatedPosition);
        CompleteRequest(iCompleteCode);
        }
    }

// ----------------------------------------------------------------------------
// CPosDataSimController::DoCancel
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosDataSimController::DoCancel()
    {
    // If request cancelled, then we have consumed Powerup Time
    // and are in simulation mode
    //
    iSimulationInProcess = ETrue;
    iTimer.Cancel();
    }

// ----------------------------------------------------------------------------
// CPosDataSimController::RunError
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CPosDataSimController::RunError(TInt aError)
    {
    CompleteRequest(aError);
    return KErrNone;
    }

//  End of File
