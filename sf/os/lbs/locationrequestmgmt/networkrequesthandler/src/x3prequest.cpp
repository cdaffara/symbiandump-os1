/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include <e32std.h>
#include <e32test.h>

// LBS-specific
#include <lbs.h>
#include <lbs/lbsx3p.h>
#include "lbstimer.h"
#include <lbs/lbslocdatasourcegpsbase.h>

#include "x3prequest.h"
#include "nrhpanic.h"


CX3pRequest::CX3pRequest(MX3pRequestObserver* aObserver,
						 const TLbsNetSessionIdInt& aSessionId,
						 TUint aTransmitPriority) :
	iObserver(aObserver),
	iSessionId(aSessionId),
	iTransmitPriority(aTransmitPriority),
	iQualityParametersReceived(EFalse),
	iTimerReStarted(EFalse)
    {
    }

CX3pRequest::~CX3pRequest()
    {
	iTimer->Cancel();
	delete iTimer;
    }

CX3pRequest* CX3pRequest::NewL(MX3pRequestObserver* aObserver,
							   const TLbsNetSessionIdInt& aSessionId, 
							   const TDesC& aDestinationId,
							   TUint aTransmitPriority, 
							   const TLbsTransmitPositionOptions& aTransmitOptions)
    {
	CX3pRequest* self = new (ELeave) CX3pRequest(aObserver, aSessionId, aTransmitPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aDestinationId, aTransmitOptions);
	CleanupStack::Pop(self);
	return(self);		
    }
    
void CX3pRequest::ConstructL(const TDesC& aDestinationId,
							 const TLbsTransmitPositionOptions& aTransmitOptions)
	{
	iTransmitOptions = aTransmitOptions;
	iDestinationId = aDestinationId;
	iTimer = CLbsCallbackTimer::NewL(*this);
	}

void CX3pRequest::StartTimer()
	{
	if(iTransmitOptions.TimeOut() != 0)
		{
		iTimer->Cancel();
		iTimer->EventAfter(iTransmitOptions.TimeOut(), 1);
		}
	}
	
void CX3pRequest::ReStartTimerOnce()
	{
	if (!iTimerReStarted)
		{
		StartTimer();
		iTimerReStarted = ETrue;
		}
	}

void CX3pRequest::SetRequestQuality(const TLbsNetPosRequestQualityInt& aRequestQuality)
	{
	iRequestQuality = aRequestQuality;
	iQualityParametersReceived = ETrue;
	}
	
TBool CX3pRequest::FixIsAccurate(const TPositionInfo& aPosInfo, TBool aTerminalAssistedPos)
	{    
	TBool fixIsAccurate = EFalse;
    
    if(!iQualityParametersReceived) 
    	{
    	// Nothing to compare it against. Assume it's OK
    	// (Don't think this can happen)
    	return(ETrue);
    	}
    	
	// Compare the accuracy to the request values.
	TPosition pos;
	aPosInfo.GetPosition(pos);

	if (pos.HorizontalAccuracy() <= iRequestQuality.MinHorizontalAccuracy() &&
		pos.VerticalAccuracy() <= iRequestQuality.MinVerticalAccuracy())
		{
		fixIsAccurate = ETrue;
		}

	// For network-calculated position, first check whether the data is valid. If any of 
	// longitude, latitude, or horizontal accuracy is missing, or horizontal 
	// accuracy is zero, then it's not valid, so there's no point going any 
	// further.
	if(aTerminalAssistedPos)
		{
		if (Math::IsNaN(pos.Latitude()) || 
			Math::IsNaN(pos.Longitude()) || 
			Math::IsNaN(pos.HorizontalAccuracy()) || 
			(pos.HorizontalAccuracy() == 0))
			{
			fixIsAccurate = EFalse;
			}
		}

	return(fixIsAccurate);
    }

const TLbsNetSessionIdInt& CX3pRequest::SessionId()
    {
    return(iSessionId);
    }
    
const TDesC& CX3pRequest::DestinationId()
    {
    return(iDestinationId);
    }
    
TUint CX3pRequest::TransmitPriority()
    {
    return(iTransmitPriority);
    }

// ----------------------------------------------------------------------------- 
// CX3pRequest::OnTimerEventL
// Description: The Location Update timer has expired.
// Cancel the request, and pass on the response if any has been received,
// otherwise report failure.
// ----------------------------------------------------------------------------- 
//
void CX3pRequest::OnTimerEventL(TInt /*aTimerId*/)
	{	
   	iObserver->OnRequestTimeout(iSessionId);
	}

/** Called if OnTimerEventL leaves */
TInt CX3pRequest::OnTimerError(TInt /*aTimerId*/, 
								TInt aError)
	{
	__ASSERT_DEBUG(EFalse, Panic(ENrhPanicX3pTimerError));
	return(aError);
	}
