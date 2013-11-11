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
//

#ifndef __CT_LBS_HYBRID_UE_ASSISTED_MTLR_TIMEOUT_H__
#define __CT_LBS_HYBRID_UE_ASSISTED_MTLR_TIMEOUT_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsnetprotocolbase.h>

// LBS test includes.
#include "ctlbshybridmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>

// Literals used
_LIT(KLbsHybridUEAssitedMTLRTimeout, "LbsHybridUEAssistedMTLRTimeout");

class CT_LbsHybridUEAssistedMTLRTimeout :	public	CT_LbsHybridMTLRStep, public MLbsPrivacyObserver
	{
public:
	~CT_LbsHybridUEAssistedMTLRTimeout();

	static CT_LbsHybridUEAssistedMTLRTimeout* New(CT_LbsHybridMTLRServer& aParent);
	virtual TVerdict doTestStepL();

// From MLbsPrivacyObserver
void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
void ProcessRequestComplete(TUint aRequestId, TInt aReason);

protected:
	CT_LbsHybridUEAssistedMTLRTimeout(CT_LbsHybridMTLRServer& aParent);
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
	};

#endif //__CT_LBS_HYBRID_UE_ASSISTED_MTLR_TIMEOUT_H__

