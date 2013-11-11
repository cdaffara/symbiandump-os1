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
// This contains the header file for Emergency Network Induced and MTLR requests
// 
//

#ifndef __CT_LBS_OOM_H__
#define __CT_LBS_OOM_H__
// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include "LbsOomTest.h"

// Literals used
_LIT(KLbsOom,"LbsOom");

class CT_LbsOom: public CT_LbsHybridMTLRStep, public MLbsPrivacyObserver
{
public:
    static CT_LbsOom* New(CT_LbsHybridMTLRServer& aParent);

	TVerdict doTestStepPreambleL();
    TVerdict doTestStepL();
    ~CT_LbsOom();

protected:
	// from MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);
	
protected:
	CT_LbsOom(CT_LbsHybridMTLRServer& aParent);
    void ConstructL();
    
    void PerformMeasurementControl();

    
	void ProcessNotifyAndAccept();
		
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
	//Out-Of-Memory test
	ROomRequest 		iOomRequest;
};

#endif //__CT_LBS_OOM_H__
