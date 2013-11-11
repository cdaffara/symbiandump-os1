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
// This contains the header file for error in receiving assistance data
// 
//

#ifndef CT_LBSMOLR_ERROR_ASSISTANCE_H
#define CT_LBSMOLR_ERROR_ASSISTANCE_H

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/posserverwatch.h>

// Literals used
_LIT(KLbsMOLRErrorAssistance,"LbsMOLRErrorAssistance");


class CT_LbsMolrErrorAssistance: public CT_LbsHybridMOLRStep, public MPosServerObserver
{
public:
    static CT_LbsMolrErrorAssistance* New(CT_LbsHybridMOLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsMolrErrorAssistance();

protected:

    // MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
protected:
    CT_LbsMolrErrorAssistance(CT_LbsHybridMOLRServer& aParent);
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

#endif //	CT_LBSMOLR_ERROR_ASSISTANCE_H
