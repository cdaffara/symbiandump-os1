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
// This contains the header file for Network Induced MTLR requests
// 
//

#ifndef __CT_LBS_NETWORK_INDUCED_MTLR_H__
#define __CT_LBS_NETWORK_INDUCED_MTLR_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>


// Literals used
_LIT(KLbsNetworkInducedMTLR,"LbsNetworkInducedMTLR");

class CT_LbsNetworkInducedMTLR: public CT_LbsHybridMTLRStep, public MLbsPrivacyObserver
{
public:
    static CT_LbsNetworkInducedMTLR* New(CT_LbsHybridMTLRServer& aParent);

	TVerdict doTestStepPreambleL();
    TVerdict doTestStepL();


    ~CT_LbsNetworkInducedMTLR();

protected:
	// from MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);
	
protected:
    CT_LbsNetworkInducedMTLR(CT_LbsHybridMTLRServer& aParent);
    void ConstructL();
    
    void PerformMeasurementControl();

    void ProcessNotifyAndReject();
	void ProcessNotifyAndAccept();
	void ProcessVerifyAndAccept();
	void ProcessVerifyAndReject();
	void ProcessVerifyAndTimeout();
	void ProcessReject();
	
    void PerformSessionComplete();

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
	TPositionInfo iRefPosInfo;
	CLbsPrivacyController* iController;
	CNetProtocolProxy* iProxy;
	
	// Test paramaters, read from ini file.
	TBool												iProcessPrivReq;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService	iService;
	TBool												iEmergency;
	CLbsAdmin::TExternalLocateService					iExternalLocateValue;
	CLbsPrivacyController::TRequestVerificationResult	iExpectedResponse;
};

#endif //__CT_LBS_NETWORK_INDUCED_MTLR_H__
