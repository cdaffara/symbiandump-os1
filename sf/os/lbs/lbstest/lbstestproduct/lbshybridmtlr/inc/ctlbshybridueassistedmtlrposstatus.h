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
// This contains the header file for MTLR UE Assisted GPS ok
// 
//

#ifndef __CT_LBS_HYBRID_UEASSISTED_MTLR_POS_STATUS_H__
#define __CT_LBS_HYBRID_UEASSISTED_MTLR_POS_STATUS_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include "lbs/lbspositioningstatus.h"


// Literals used
_LIT(KLbsHybridUEAssistedMTLRPosStatus,"LbsHybridUEAssistedMTLRPosStatus");

class CT_LbsHybridUEAssistedMTLRPosStatus: public CT_LbsHybridMTLRStep, public MLbsPrivacyObserver, public MLbsPositioningStatusObserver
{
public:
    static CT_LbsHybridUEAssistedMTLRPosStatus* New(CT_LbsHybridMTLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsHybridUEAssistedMTLRPosStatus();

protected:
	// from MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);

	//MLbsPositioningStatusObserver
    void OnPositioningStatusUpdate(const CLbsPositioningStatus::TLbsPositioningStatus& aPositioningStatus);
	
protected:
    CT_LbsHybridUEAssistedMTLRPosStatus(CT_LbsHybridMTLRServer& aParent);
    void ConstructL();

private:
	enum TState
		{
		EInitializing,
		EPrivacyCheckOk,
		ERefLocReceived,
		EGpsLocReceived,
		ERequestComplete
		};
	
	TState iState;
	TLbsNetSessionId iSessionId;
	CLbsPrivacyController* iController;
	CNetProtocolProxy* iProxy;

	CLbsPositioningStatus* iLbsPostioningStatus;
    TInt       iPosStatusCount;
    CLbsPositioningStatus::TLbsPositioningStatus iPositioningStatus;
};

#endif //__CT_LBS_HYBRID_UEASSISTED_MTLR_POS_STATUS_H__
