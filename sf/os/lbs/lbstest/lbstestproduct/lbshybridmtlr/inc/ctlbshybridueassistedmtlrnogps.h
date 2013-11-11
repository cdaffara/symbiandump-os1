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

#ifndef __CT_LBS_HYBRID_UE_ASSISTED_MTLR_N0_GPS_H__
#define __CT_LBS_HYBRID_UE_ASSISTED_MTLR_N0_GPS_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsnetprotocolbase.h>

// LBS test includes.
#include "ctlbshybridmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>

/*
The ini file for NW or GPS ok should differ - We should probably hav a key word in the ini file to say 
that the GPS has found the result and it sends a cancel request to the network.
*/

// Literals used
_LIT(KLbsHybridUEAssitedMTLRNoGPS, "LbsHybridUEAssistedMTLRNoGPS");

class CT_LbsHybridUEAssistedMTLRNoGPS :	public	CT_LbsHybridMTLRStep, public MLbsPrivacyObserver
	{
public:
	~CT_LbsHybridUEAssistedMTLRNoGPS();

	static CT_LbsHybridUEAssistedMTLRNoGPS* New(CT_LbsHybridMTLRServer& aParent);
	virtual TVerdict doTestStepL();

// From MLbsPrivacyObserver
void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
void ProcessRequestComplete(TUint aRequestId, TInt aReason);

protected:
	CT_LbsHybridUEAssistedMTLRNoGPS(CT_LbsHybridMTLRServer& aParent);
	void ConstructL();		

private:
	enum TState
		{
		EInitializing,
		EPrivacyCheckOk,
		ERefLocReceived,
		ERequestComplete
		};
		
	TState iState;
	TLbsNetSessionId iSessionId;
    CLbsPrivacyController* iController;	
    CNetProtocolProxy* iProxy;
	};

#endif //__CT_LBS_HYBRID_UE_ASSISTED_MTLR_N0_GPS_H__

