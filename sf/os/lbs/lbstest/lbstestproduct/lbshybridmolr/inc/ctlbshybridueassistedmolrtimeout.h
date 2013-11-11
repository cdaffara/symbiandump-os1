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
// This contains the header file for Hybrid Extended Client Timeout Tests
// 
//

#ifndef __CT_LBS_HYBRID_UEASSISTED_MOLR_TIMEOUT_H__
#define __CT_LBS_HYBRID_UEASSISTED_MOLR_TIMEOUT_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/test/posserverwatch.h>

// Literals used
_LIT(KLbsHybridUEAssistedMOLRTimeOut, "LbsHybridUEAssistedMOLRTimeOut");

class CT_LbsHybridUEAssistedMOLRTimeOut :	public	CT_LbsHybridMOLRStep, public MPosServerObserver
	{
public:
	~CT_LbsHybridUEAssistedMOLRTimeOut();

	static CT_LbsHybridUEAssistedMOLRTimeOut* New(CT_LbsHybridMOLRServer& aParent);
	virtual TVerdict doTestStepL();

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
protected:
	CT_LbsHybridUEAssistedMOLRTimeOut(CT_LbsHybridMOLRServer& aParent);
	void ConstructL();

private:
	enum TState
		{
		EInitializing,
		ERefLocReceived,
		EFinalLocReceived
		};
	
	TState iState;

	TLbsNetSessionId iSessionId;
	};

#endif //__CT_LBS_HYBRID_UEASSISTED_MOLR_TIMEOUT_H__

