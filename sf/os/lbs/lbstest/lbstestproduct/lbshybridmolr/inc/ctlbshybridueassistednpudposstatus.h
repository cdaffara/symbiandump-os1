// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains the header file for MOLR UE Assisted GPS ok
// 
//

#ifndef CTLBSHYBRIDUEASSISTEDNPUDPOSSTATUS_H
#define CTLBSHYBRIDUEASSISTEDNPUDPOSSTATUS_H

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/posserverwatch.h>
#include "lbs/lbspositioningstatus.h"

// Literals used
_LIT(KLbsHybridUEAssistedNpudPosStatus,"LbsHybridUEAssistedNpudPosStatus");


class CT_LbsHybridUEAssistedNpudPosStatus: public CT_LbsHybridMOLRStep, /*public MLbsPrivacyObserver*/  public MPosServerObserver, public MLbsPositioningStatusObserver
{
public:
    static CT_LbsHybridUEAssistedNpudPosStatus* New(CT_LbsHybridMOLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsHybridUEAssistedNpudPosStatus();

protected:
/*	// from MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);
*/
	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
	//MLbsPositioningStatusObserver
    void OnPositioningStatusUpdate(const CLbsPositioningStatus::TLbsPositioningStatus& aPositioningStatus);
	
protected:
    CT_LbsHybridUEAssistedNpudPosStatus(CT_LbsHybridMOLRServer& aParent);
    void ConstructL();

private:
	enum TState
		{
		EInitializing,
		EPrivacyCheckOk,
		ERefLocReceived,
		EGpsLocReceived
		};
	
	TState iState;
	TLbsNetSessionId iSessionId;
	TInt       iPosStatusCount;
	CLbsPositioningStatus::TLbsPositioningStatus iPositioningStatus;
	CLbsPositioningStatus* iLbsPositioningStatus;
};

#endif //	CTLBSHYBRIDUEASSISTEDNPUDPOSSTATUS_H
