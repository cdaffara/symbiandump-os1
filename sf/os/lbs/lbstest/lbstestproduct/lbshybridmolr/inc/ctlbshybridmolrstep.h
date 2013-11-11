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
// @file ctlbshybridmolrstep.h
// This contains the header file for LBS HYBRID MOLR Test Step Base
// 
//

#ifndef __CT_LBS_HYBRID_MOLR_STEP_H__
#define __CT_LBS_HYBRID_MOLR_STEP_H__

#include <test/testexecutestepbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/test/ctlbsagpsmoduleeventlistener.h>

#include "ctlbshybridmolrserver.h"

/** Must be the Id of the protocol module used for the test */
const TUid KRequestUid = {0x10282293};

enum TExpectedApiBehaviour
	{
	EApiVariant1,
	EApiVariant2
	};


class CT_LbsHybridMOLRStep : public CTestStep, MT_AgpsModuleEventHandler
{

public:
	virtual ~CT_LbsHybridMOLRStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsHybridMOLRStep(CT_LbsHybridMOLRServer& aParent);
	void ConstructL();
	
	CT_LbsHybridMOLRServer& iParent;

private: // from MT_AgpsModuleEventHandler
	void OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions);
	
private:
	RPointerArray<TLbsGpsOptions> iExpectedModuleGpsOptions;
	TInt iExpectedModuleGpsOptionsIndex;
	
	CT_AgpsModuleEventListener* iAgpsModuleEventListener;
	
protected:
	TExpectedApiBehaviour iExpectedApiBehaviour;
};

struct SData			
	{
	TLbsNetSessionId sessionId;
	TVerdict response;
	};


#endif // __CT_LBS_HYBRID_MOLR_STEP_H__
