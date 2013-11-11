// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CT_LBS_SUPL_MOLR_TB_FUTILE_SP_H__
#define __CT_LBS_SUPL_MOLR_TB_FUTILE_SP_H__

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/posserverwatch.h>

// Literals used
_LIT(KLbsSuplMolrTbFutileSp,"LbsSuplMolrTbFutileSp");


class CT_LbsSuplMolrTbFutileSp: public CT_LbsHybridMOLRStep, public MPosServerObserver
{
public:
    static CT_LbsSuplMolrTbFutileSp* New(CT_LbsHybridMOLRServer& aParent);
    enum TVerdict doTestStepL();
    ~CT_LbsSuplMolrTbFutileSp();

protected:
	void Perform_Molr(TInt aExpectedUpdateErr);
	
	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
	
protected:
	CT_LbsSuplMolrTbFutileSp(CT_LbsHybridMOLRServer& aParent);
    void ConstructL();

private:
	enum TState
		{
		EInitializing,
		ERefLocReceived,
		EGpsLocReceived
		};
	
	TState iState;
	TLbsNetSessionId iSessionId;
	
	RPositionServer iServer;
	RPositioner iPositioner;
	
	CNetProtocolProxy* iProxy;
	CPosServerWatcher* iPosSrvWatcher;
};

#endif // __CT_LBS_SUPL_MOLR_TB_FUTILE_SP_H__
