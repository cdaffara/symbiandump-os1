// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The class representing a PSY request to the LBS sub-system. 
// 
//

#if (!defined __TE_LBSPSYREQUEST__)
#define __TE_LBSPSYREQUEST__

#include <e32base.h>
#include <lbs.h>
#include <lbssatellite.h>

#include "te_active.h"
#include "LbsInternalInterface.h"

/**
The class representing a CPositioner request to the LBS sub-system.
*/
class CTe_PsyRequest : public CTe_Active
	{
public:	
	IMPORT_C static CTe_PsyRequest* NewLC(MTe_ActiveObserver& aObserver);
	virtual ~CTe_PsyRequest();
	
	IMPORT_C void NotifyPsyRequest();
	
	IMPORT_C void ExpectDefaultUpdateRequest();
	IMPORT_C void ExpectUpdateRequestWithRequestMethod(TPositionModuleInfo::TTechnologyType aMethod);
	IMPORT_C void ExpectDefaultStatusRequest();
	IMPORT_C void ExpectDefaultCancelRequest();
	IMPORT_C void ExpectPowerAdvice(TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice);
	IMPORT_C void CheckPosUpdateOrReqReceivedL();
	IMPORT_C void CheckNothingReceivedL();
	IMPORT_C void CheckPosUpdRequestNotReceivedL();
	IMPORT_C void CheckPosUpdateReqReceivedWithMethodL();
	IMPORT_C void CheckPowerAdviceReceivedL();
	
	inline TLbsPositionUpdateRequest& UpdateRequest();
	inline TLbsPositionUpdateRequestStatus& StatusRequest();
	inline TLbsPositionUpdateRequestCancel& CancelRequest();
	
protected:
	virtual void RunL();
	virtual void DoCancel();
	
private:
	CTe_PsyRequest(MTe_ActiveObserver& aObserver);
	void ConstructL();
		
private:
	RLbsPositionUpdateRequests   iPosUpdateReqs;
	TBool iWasUpdateOrRequestReceived;
	TBool iWasPosUpdRequestReceived;
	TLbsPositionUpdateRequestBase iRequestReceived;
	TLbsPositionUpdateRequestBase iRequestExpected;
	TPositionQuality iDefaultPosQuality;
	};


inline TLbsPositionUpdateRequest& CTe_PsyRequest::UpdateRequest()
	{
	return static_cast<TLbsPositionUpdateRequest&>(iRequestExpected);
	}

inline TLbsPositionUpdateRequestStatus& CTe_PsyRequest::StatusRequest()
	{
	return static_cast<TLbsPositionUpdateRequestStatus&>(iRequestExpected);
	}

inline TLbsPositionUpdateRequestCancel& CTe_PsyRequest::CancelRequest()
	{
	return static_cast<TLbsPositionUpdateRequestCancel&>(iRequestExpected);
	}

#endif //__TE_LBSPSYREQUEST__
