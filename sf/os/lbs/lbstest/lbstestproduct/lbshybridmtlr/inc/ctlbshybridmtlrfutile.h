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
// This contains the header file for Location Data Source Tests
// 
//

#ifndef CTLBSHYBRIDMTLRFUTILE_H
#define CTLBSHYBRIDMTLRFUTILE_H

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/test/ctlbsagpshandler.h>

// LBS test includes.
#include "ctlbshybridmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>

// Literals used
_LIT(KLbsHybridMTLRFutile, "LbsHybridMtlrFutile");

class CT_LbsHybridMTLRFutile :	public CT_LbsHybridMTLRStep, 
								public MLbsPrivacyObserver,
								public MT_ResponseObserver
	{
public:
	~CT_LbsHybridMTLRFutile();

	static CT_LbsHybridMTLRFutile* New(CT_LbsHybridMTLRServer& aParent);
	virtual TVerdict doTestStepL();
	
// From MLbsPrivacyObserver
void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
void ProcessRequestComplete(TUint aRequestId, TInt aReason);

	void ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse);
	
protected:
	CT_LbsHybridMTLRFutile(CT_LbsHybridMTLRServer& aParent);
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
	TInt iTestCaseId;	
	TBool iPositionUpdateRef;
	TLbsNetSessionId iSessionId;
    CLbsPrivacyController* iController;
    CNetProtocolProxy* iProxy;

	// Communication channel between A-Gps module and Test Harness
	CT_LbsAGpsHandler* iAGpsHandler;
	};

#endif //	CTLBSHYBRIDMTLRFUTILE_H

