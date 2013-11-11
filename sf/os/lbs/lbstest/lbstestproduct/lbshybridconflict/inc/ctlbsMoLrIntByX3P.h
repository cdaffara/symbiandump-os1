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
// This contains the header file for Hybrid MoLr Interrupted by X3P Tests
// 
//

#ifndef __CT_LBS_MOLRINTBYX3P_H__
#define __CT_LBS_MOLRINTBYX3P_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridconflictstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/test/posserverwatch.h>


// Literals used
_LIT(KLbsMoLrIntByX3P, "LbsMoLrIntByX3P");

class CT_LbsMoLrIntByX3P :	public	CT_LbsHybridConflictStep, public MPosServerObserver
	{
public:
	~CT_LbsMoLrIntByX3P();

	static CT_LbsMoLrIntByX3P* New(CT_LbsHybridConflictServer& aParent);
	virtual TVerdict doTestStepL();

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
protected:
	CT_LbsMoLrIntByX3P(CT_LbsHybridConflictServer& aParent);
	void ConstructL();

private:
	enum TState
		{
		EInitializing,
		ERefLocReceived,
		ENetworkLocReceived
		};
	
	TState iState;

	TLbsNetSessionId iSessionId;
	};

#endif // __CT_LBS_MOLRINTBYX3P_H__

