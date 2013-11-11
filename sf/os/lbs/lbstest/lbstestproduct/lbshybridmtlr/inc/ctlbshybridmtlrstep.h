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
// @file ctlbshybridmtlrserver.h
// This contains the header file for LBS HYBRID MTLR Test Step Base
// 
//

#ifndef __CT_LBS_HYBRID_MTLR_STEP_H__
#define __CT_LBS_HYBRID_MTLR_STEP_H__
#include <test/testexecutestepbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/test/ctlbsagpsmoduleeventlistener.h>

#include "ctlbshybridmtlrserver.h"

/** Must be the Id of the protool module used for the test */
const TUid KRequestUid = {0x10282293};
/** Delta timout value used in all tests */
const TInt KDelta = 2.0 * 1000 * 1000;	//2 seconds

class CT_LbsHybridMTLRStep : public CTestStep, MT_AgpsModuleEventHandler
{

public:
	virtual ~CT_LbsHybridMTLRStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsHybridMTLRStep(CT_LbsHybridMTLRServer& aParent);
	void ConstructL();
	
private: // from MT_AgpsModuleEventHandler
	void OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions);

	
private:
	RPointerArray<TLbsGpsOptions> iExpectedModuleGpsOptions;
	TInt iExpectedModuleGpsOptionsIndex;
	CT_AgpsModuleEventListener* iAgpsModuleEventListener;
	
protected:
	CT_LbsHybridMTLRServer& iParent;
};

struct SData			
	{
	TLbsNetSessionId sessionId;
	TVerdict response;
	};


#endif // __CT_LBS_HYBRID_MTLR_STEP_H__
