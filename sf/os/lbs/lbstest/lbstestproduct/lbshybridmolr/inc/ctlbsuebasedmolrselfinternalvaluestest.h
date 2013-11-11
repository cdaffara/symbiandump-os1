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

#ifndef __CT_LBS_UEBASED_MOLR_SELF_INTERNAL_VALUE_H__
#define __CT_LBS_UEBASED_MOLR_SELF_INTERNAL_VALUE_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/test/posserverwatch.h>

// Literals used
_LIT(KLbsUEBasedMOLRSelfInternalValueTest, "LbsUEBasedMOLRSelfInternalValueTest");

const TInt KMinHorizontalAccBis = 40.0;
const TInt KMinVerticalAccBis = 900.0;
const TInt KDefaultMaxFixTimeBis = 130*1000*1000;

class CT_LbsUEBasedMOLRSelfInternalValueTest :	public	CT_LbsHybridMOLRStep, public MPosServerObserver
	{
public:
	~CT_LbsUEBasedMOLRSelfInternalValueTest();

	static CT_LbsUEBasedMOLRSelfInternalValueTest* New(CT_LbsHybridMOLRServer& aParent);
	virtual TVerdict doTestStepL();

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
protected:
	CT_LbsUEBasedMOLRSelfInternalValueTest(CT_LbsHybridMOLRServer& aParent);
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
	};

#endif //__CT_LBS_UEBASED_MOLR_H__

