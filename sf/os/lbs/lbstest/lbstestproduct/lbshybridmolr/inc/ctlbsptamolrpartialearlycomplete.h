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

#ifndef CTLBSPTAMOLRPARTIALEARLYCOMPLETE_H
#define CTLBSPTAMOLRPARTIALEARLYCOMPLETE_H

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/posserverwatch.h>

// Literals used
_LIT(KLbsPTAMOLRPartialEarlyComplete,"LbsPTAMOLRPartialEarlyComplete");

class CT_LbsPTAMOLRPartialEarlyComplete: public CT_LbsHybridMOLRStep, public MPosServerObserver
{
public:
    static CT_LbsPTAMOLRPartialEarlyComplete* New(CT_LbsHybridMOLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsPTAMOLRPartialEarlyComplete();

protected:

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
	
protected:
    CT_LbsPTAMOLRPartialEarlyComplete(CT_LbsHybridMOLRServer& aParent);
    void ConstructL();

private:
	enum TState
		{
		EInitializing,
		EGpsPartialInitReceived,
		EGpsPartialEarlyReceived
		};
	
	TState iState;
	TLbsNetSessionId iSessionId;
};

#endif //	CTLBSPTAMOLRPARTIALEARLYCOMPLETE_H
