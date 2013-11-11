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
// @file ctlbshybridcombinedstep_base.h
// This contains the header file for LBS HYBRID Combined Test Step Base
// 
//

#ifndef __CT_LBS_HYBRID_COMBINED_STEP_BASE_H__
#define __CT_LBS_HYBRID_COMBINED_STEP_BASE_H__

#include <test/testexecutestepbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/test/ctlbsagpsmoduleeventlistener.h>
#include <lbs/lbsprivacycontroller.h>

#include <lbs/test/lbsnetprotocolproxy.h>
#include "ctlbshybridcombinedserver.h"

/** Must be the Id of the protocol module used for the test */
const TUid KRequestUid = {0x10282293};


class CT_LbsHybridCombinedStep_Base : public CTestStep, MT_AgpsModuleEventHandler
{

public:
	virtual ~CT_LbsHybridCombinedStep_Base();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsHybridCombinedStep_Base(CT_LbsHybridCombinedServer& aParent);
	void ConstructL();
	
	CT_LbsHybridCombinedServer& iParent;
	
private: // from MT_AgpsModuleEventHandler
	void OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions);
	
protected:
    enum TState
        {
        EInitializing,
        ERefLocReceived,
        EGpsLocReceived,
        EPrivacyCheckOk,
        ERequestComplete
        };

    TState iState;
    TBool iPositionUpdateRef;
    CLbsPrivacyController* iController;
    CNetProtocolProxy* iProxy;
    TLbsNetSessionId iSessionId;
    TLbsNetSessionId iSessionId2;

    TInt iClientPosUpdateCount;
    TInt iNetworkPosUpdateCount;
	
private:
	RPointerArray<TLbsGpsOptions> iExpectedModuleGpsOptions;
	TInt iExpectedModuleGpsOptionsIndex;
	
	CT_AgpsModuleEventListener* iAgpsModuleEventListener;
};


#endif // __CT_LBS_HYBRID_COMBINED_STEP_BASE_H__
