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

#ifndef CTLBSHYBRIDUEASSISTEDMOLRGPSOK_H
#define CTLBSHYBRIDUEASSISTEDMOLRGPSOK_H

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/posserverwatch.h>

// Literals used
_LIT(KLbsHybridUEAssistedMOLRGPSOk,"LbsHybridUEAssistedMOLRGPSOk");


class CT_LbsHybridUEAssistedMOLRGPSOk: public CT_LbsHybridMOLRStep, /*public MLbsPrivacyObserver*/  public MPosServerObserver
{
public:
    static CT_LbsHybridUEAssistedMOLRGPSOk* New(CT_LbsHybridMOLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsHybridUEAssistedMOLRGPSOk();

protected:
/*	// from MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);
*/
	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
	
protected:
    CT_LbsHybridUEAssistedMOLRGPSOk(CT_LbsHybridMOLRServer& aParent);
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

};

#endif //	CTLBSHYBRIDUEASSISTEDMOLRGPSOK_H
