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
/**
@file
@internalComponent
@released
*/

#ifndef _X3PREQUEST_H 
#define _X3PREQUEST_H

#include <lbs/lbsx3p.h>
#include "lbscommoninternaldatatypes.h"
#include "lbsnetinternalapi.h"


// Panic Category
_LIT(KX3pRequestFault,"Lbs-Nrh X3pRequest");


/**
 *
 * Mixin class to report expiry of request timer.
 *
 */
class MX3pRequestObserver
    {
public:
    virtual void OnRequestTimeout(const TLbsNetSessionIdInt& aTimedOutRequestId) = 0;
    };
    
    
class CX3pRequest : public CBase,
					public MLbsCallbackTimerObserver

	{
enum TLbsNrhX3pRequestPanic
	{
	EPanicTimerError = 0x01
	};	

public:
	static CX3pRequest* NewL(MX3pRequestObserver* aObserver, 
							 const TLbsNetSessionIdInt& aSessionId,
							 const TDesC& aDestinationID,
							 TUint aTransmitPriority, 
							 const TLbsTransmitPositionOptions& aTransmitOptions);

	const TLbsNetSessionIdInt& SessionId();
	const TDesC& DestinationId();
	TUint TransmitPriority();
	void SetRequestQuality(const TLbsNetPosRequestQualityInt& aRequestQuality);
	TBool FixIsAccurate(const TPositionInfo& aPosInfo, TBool aHybridMode);
	
	void StartTimer();
	void ReStartTimerOnce();
	~CX3pRequest();

protected:
	CX3pRequest(MX3pRequestObserver* aObserver,
				const TLbsNetSessionIdInt& aSessionId,
				TUint aTransmitPriority);

	void ConstructL(const TDesC& aDestinationId,
					const TLbsTransmitPositionOptions& aTransmitOptions);

protected:
	void RunL();

protected: // From MLbsCallbackTimerObserver
	/** Called when the timer expires */
	void OnTimerEventL(TInt aTimerId);
	/** Called if OnTimerEventL leaves */
	TInt OnTimerError(TInt aTimerId, TInt aError);


private:
	MX3pRequestObserver* iObserver;
	TLbsNetSessionIdInt iSessionId;
	TBuf<64> iDestinationId;
	TUint iTransmitPriority; 
	TLbsTransmitPositionOptions iTransmitOptions;
	TLbsNetPosRequestQualityInt iRequestQuality;
	TBool iQualityParametersReceived;
	CLbsCallbackTimer* iTimer;
	TBool iTimerReStarted;
    };
#endif // _X3PREQUEST_H 
