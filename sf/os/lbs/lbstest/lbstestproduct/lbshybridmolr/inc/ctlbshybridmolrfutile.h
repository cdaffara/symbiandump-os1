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
// This contains the header file for Hybrid MOLR GPS Futile test cases (ue-based and ue-assisted)
// 
//

#ifndef CTLBSHYBRUDMOLRFUTILE_H
#define CTLBSHYBRUDMOLRFUTILE_H

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/posserverwatch.h>

// Literals used
_LIT(KLbsHybridMOLRFutile,"LbsHybridMolrFutile");


class CT_LbsHybridMOLRFutile: public CT_LbsHybridMOLRStep, /*public MLbsPrivacyObserver*/  public MPosServerObserver
{
public:
    static CT_LbsHybridMOLRFutile* New(CT_LbsHybridMOLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsHybridMOLRFutile();

protected:
	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
	
protected:
    CT_LbsHybridMOLRFutile(CT_LbsHybridMOLRServer& aParent);
    void ConstructL();

private:
	enum TState
		{
		EInitializing,
		EPrivacyCheckOk,
		ERefLocReceived,
		EGpsLocReceived,
		EFnNwLocReceived
		};
	
	TInt iTestCaseId;	
	TState iState;
	TLbsNetSessionId iSessionId;
};

#endif // CTLBSHYBRUDMOLRFUTILE_H
