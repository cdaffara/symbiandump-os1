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

#ifndef CTLBSAUTONOMOUSMOLRFUTILE_H
#define CTLBSAUTONOMOUSMOLRFUTILE_H

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/test/posserverwatch.h>


// Literals used
_LIT(KLbsAutonomousMOLRFutile, "LbsAutonomousMolrFutile");

class CT_LbsAutonomousMOLRFutile :	public	CT_LbsHybridMOLRStep, public MPosServerObserver
	{
public:
	~CT_LbsAutonomousMOLRFutile();

	static CT_LbsAutonomousMOLRFutile* New(CT_LbsHybridMOLRServer& aParent);
	virtual TVerdict doTestStepL();

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
protected:
	CT_LbsAutonomousMOLRFutile(CT_LbsHybridMOLRServer& aParent);
	void ConstructL();

private:
	TLbsNetSessionId iSessionId;
	};

#endif // CTLBSAUTONOMOUSMOLRFUTILE_H

