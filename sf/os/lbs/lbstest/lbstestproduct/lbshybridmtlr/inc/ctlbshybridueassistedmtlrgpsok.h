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

#ifndef __CT_LBS_HYBRID_UEASSISTED_GPS_OK_H__
#define __CT_LBS_HYBRID_UEASSISTED_GPS_OK_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>



// Literals used
_LIT(KLbsHybridUEAssistedMTLRGPSOK,"LbsHybridUEAssistedMTLRGPSOK");

class CT_LbsHybridUEAssistedMTLRGPSOK: public CT_LbsHybridMTLRStep, public MLbsPrivacyObserver
{
public:
    static CT_LbsHybridUEAssistedMTLRGPSOK* New(CT_LbsHybridMTLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsHybridUEAssistedMTLRGPSOK();

protected:
	// from MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);
	
protected:
    CT_LbsHybridUEAssistedMTLRGPSOK(CT_LbsHybridMTLRServer& aParent);
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

#endif //__CT_LBS_HYBRID_UEASSISTED_GPS_OK_H__
