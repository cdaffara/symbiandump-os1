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
#include "EPos_CPosNmeaController.h"
#include "EPos_CPosFileHandler.h"
#include "EPos_SimPsyConstants.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosNmeaController::CPosNmeaController(
    TBool aTimeRelativePlayback) :
    CPosControllerBase(aTimeRelativePlayback),
    iHasAlreadyReadFirstSentence(EFalse), iState(EIdle)
    {
    CActiveScheduler::Add(this);   
    iTimerTrigger.CreateLocal();
    }

// EPOC default constructor can leave.
void CPosNmeaController::ConstructL(const TDesC& aNMEAFile)
    {
    iFileHandler = CPosFileHandler::NewL(aNMEAFile);
    CPosSimulationPositioner::ClearAndSetDefaultPositionData(iPosition);
    }

// Two-phased constructor.
CPosNmeaController* CPosNmeaController::NewL(
    const TDesC& aNMEAFile,
    TBool aTimeRelativePlayback)
    {
    CPosNmeaController* self = new (ELeave) CPosNmeaController(aTimeRelativePlayback);
    CleanupStack::PushL(self);
    self->ConstructL(aNMEAFile);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosNmeaController::~CPosNmeaController()
    {
    iTimerTrigger.Cancel();
    iTimerTrigger.Close();
    Cancel();
    delete iFileHandler;
    }

// ----------------------------------------------------------------------------
// CPosNmeaController::NotifyPositionUpdate
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosNmeaController::NotifyPositionUpdate(
    TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus,
    CPosSimulationPositioner& aSimPos)
    {
    NotifyPositionUpdateBase(aPosInfo, aStatus, aSimPos);

    if(iState == EIdle && !IsActive())
    	{
    	iState = EWaiting;
    	if(iTimeRelativePlayback)
			{
			iTimerTrigger.After(iStatus, iTimeBetweenReads);
			}
		else
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		SetActive();
    	}
    }

// ----------------------------------------------------------------------------
// CPosNmeaController::CompleteRequest
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosNmeaController::CompleteRequest(TInt aErrorCode)
    {
    CompleteRequestBase(aErrorCode);
	iParser.Reset();
	if(aErrorCode == KErrNone && iTimeRelativePlayback)
		{
		StartTrpDelayTimer();
		}
	else
		{
		iState = EIdle;
		}
    }

// ----------------------------------------------------------------------------
// CPosNmeaController::ReadSentence
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosNmeaController::ReadSentence()
    {
    iState = EReadingSentence;
    TRequestStatus* status = &iStatus;
    
    iStatus = iFileHandler->Read(iInputBuffer);
    User::After(1);
    User::RequestComplete(status, iStatus.Int());
    SetActive();
 
    }

// ----------------------------------------------------------------------------
// CPosNmeaController::ParseSentence
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosNmeaController::ParseSentence()
    {
    iState = EParsingSentence;
    TInt err = iParser.ParseSentence(iInputBuffer, iPosition);
    if(err != KPosErrorAlreadyParsed)
    	{
    	for(TInt i=0; i< iSimulationPositioners.Count(); i++)
			{
			//reading same information which were read into iParser
			//so we can properly process all possible types of position:
			//HPositionInfo, TPositionInfo, HPositionGenericInfo, TPositionCourseInfo,
			//TPositionSatelliteInfo, etc.
			iParser.ParseSentence(iInputBuffer, iSimulationPositioners[i].iPosition, ETrue);
			}
    	}
    
    if (err == KPosErrorAlreadyParsed)
        {
        // Make sure this sentence is kept until next request.
        iHasAlreadyReadFirstSentence = ETrue;
        CompleteOrRetry();
        }
    else if (err == KErrNone)
        {
        ReadSentence();
        }
    else
        {
        CompleteRequest(err);
        }
    }

// ----------------------------------------------------------------------------
// CPosNmeaController::IsPartialPosition
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CPosNmeaController::IsPartialPosition(
    TPositionInfoBase& aPosInfo)
    {
    TPositionInfo& posInfo = static_cast<TPositionInfo&> (aPosInfo);
    TPosition pos;
    posInfo.GetPosition(pos);

    // Check if time is set
    // Check if longitude is a number
    // Check if latitude is a number
    if (pos.Time().Int64() == 0 ||
        Math::IsNaN(TRealX(pos.Longitude())) ||
        Math::IsNaN(TRealX(pos.Latitude())))
        {
        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPosNmeaController::CompleteOrRetry
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosNmeaController::CompleteOrRetry()
    {
    TInt progress = iParser.Progress();
    TBool partialPosition = IsPartialPosition(iPosition);

    if (progress == KPosAllParsed && !partialPosition)
        {
        // Full position fix
        CompleteRequest(KErrNone);
        return;
        }
    
    if (partialPosition)
        {
        // Partial update position fix
        CompleteRequest(KPositionPartialUpdate);
        }
    
    //check for any outstanding requests
    for(TInt i = 0; i < iSimulationPositioners.Count(); i++)
    	{
    		CPosSimulationPositioner::ClearAndSetDefaultPositionData(iSimulationPositioners[i].iPosition);
    	}
    if(iSimulationPositioners.Count()>0)
        {
        // Otherwise reset the parser and try again to get a full fix
        iParser.Reset();
        CPosSimulationPositioner::ClearAndSetDefaultPositionData(iPosition);

        if (iHasAlreadyReadFirstSentence)
            {
            ParseSentence();
            }
        else
            {
            ReadSentence();
            }
        }
    else if (iTimeRelativePlayback)
    	{
    	StartTrpDelayTimer();
    	}
    }

void CPosNmeaController::StartTrpDelayTimer()
	{
	iState = EWaiting;
	__ASSERT_ALWAYS(iTimeRelativePlayback, User::Panic(_L("CPosNmeaController"), KErrTotalLossOfPrecision ));
	iTimerTrigger.After(iStatus, iTimeBetweenReads);
	SetActive();
	}
// ----------------------------------------------------------------------------
// CPosNmeaController::RunL
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosNmeaController::RunL()
    {
    switch (iState)
    	{
    	case EIdle:
    	case EWaiting:
    		if(iHasAlreadyReadFirstSentence)
				{
				iHasAlreadyReadFirstSentence = EFalse;
				ParseSentence();
				}
			else
				{
				ReadSentence();
				}
    		break;
    	case EReadingSentence:
    		if (iStatus == KErrNone)
    			{
    			ParseSentence();
    			}
    		else if (iStatus == KErrEof)
    			{
    			CompleteOrRetry();
    			}
    		else
    			{
    			CompleteRequest(iStatus.Int());
    			}
    		break;
    	case EParsingSentence:
    	default:
    		User::Panic(_L("CPosNmeaController state"), KErrGeneral);
    	}

    }

// ----------------------------------------------------------------------------
// CPosNmeaController::DoCancel
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosNmeaController::DoCancel()
    {
    }

//  End of File
