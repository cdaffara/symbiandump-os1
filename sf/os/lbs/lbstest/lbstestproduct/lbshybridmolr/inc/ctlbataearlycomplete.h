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
// This contains the header file for Hybrid MoLr Tests
// 
//

#ifndef __CT_LBS_EARLY_COMPLETE_H__
#define __CT_LBS_EARLY_COMPLETE_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/test/posserverwatch.h>

#include "lbs/test/ctlbsagpshandler.h"

// Literals used
_LIT(KLbsATAEarlyComplete, "LbsATAEarlyComplete");

class CT_LbsATAEarlyComplete :	public	CT_LbsHybridMOLRStep, public MPosServerObserver, MT_ResponseObserver 
	{
public:
	~CT_LbsATAEarlyComplete();

	static CT_LbsATAEarlyComplete* New(CT_LbsHybridMOLRServer& aParent);
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);

	// from MT_ResponseObserver
	virtual void ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse);

protected:
	CT_LbsATAEarlyComplete(CT_LbsHybridMOLRServer& aParent);
	void ConstructL();
	
	// Communication channel between A-Gps module and Test Harness
	CT_LbsAGpsHandler* iAGpsHandler;
	
private:
	enum TState
		{
		EInitializing,
		EGotFirstPosition,
		EGotCompleteRequestPosition,
		EDone
		};
	
	TState iState;

	TLbsNetSessionId iSessionId;
	};

#endif // __CT_LBS_EARLY_COMPLETE_H__

