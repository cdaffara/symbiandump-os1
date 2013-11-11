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
// This contains the header file for PTB MTLR Cancel
// 
//

#ifndef __CT_LBS_PTB_CANCEL_H__
#define __CT_LBS_PTB_CANCEL_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbsptbmtlrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>



// Literals used
_LIT(KLbsPTBMTLRCancel,"LbsPTBMTLRCancel");

class CT_LbsPTBMTLRCancel: public CT_LbsPTBMTLRStep, public MLbsPrivacyObserver
{
public:
    static CT_LbsPTBMTLRCancel* New(CT_LbsPTBMTLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsPTBMTLRCancel();

protected:
	// from MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);
	
protected:
	CT_LbsPTBMTLRCancel(CT_LbsPTBMTLRServer& aParent);
    void ConstructL();
private:
	void InitiateCancelMTLR(TUint aRequestId);
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

#endif //__CT_LBS_PTB_CANCEL_H__
