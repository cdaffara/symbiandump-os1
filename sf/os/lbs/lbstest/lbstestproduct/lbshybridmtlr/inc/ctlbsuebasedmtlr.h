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

#ifndef __CT_LBS_UEBASED_MTLR_H__
#define __CT_LBS_UEBASED_MTLR_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>

// Literals used
_LIT(KLbsUEBasedMTLR, "LbsUEBasedMtlr");

class CT_LbsUEBasedMTLR :	public	CT_LbsHybridMTLRStep, public MLbsPrivacyObserver 
	{
public:
	~CT_LbsUEBasedMTLR();

	static CT_LbsUEBasedMTLR* New(CT_LbsHybridMTLRServer& aParent);
	virtual TVerdict doTestStepL();
	
// From MLbsPrivacyObserver
void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
void ProcessRequestComplete(TUint aRequestId, TInt aReason);
	
protected:
	CT_LbsUEBasedMTLR(CT_LbsHybridMTLRServer& aParent);
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
	TBool iPositionUpdateRef;
	TLbsNetSessionId iSessionId;
    CLbsPrivacyController* iController;
    CNetProtocolProxy* iProxy;
	};

#endif //__CT_LBS_UEBASED_MTLR_H__

