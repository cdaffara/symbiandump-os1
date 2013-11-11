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
// This contains the header file for LBS HYBRID Conflict Test Step Base
// 
//

#ifndef __CT_LBS_HYBRID_CONFLICT_STEP_H__
#define __CT_LBS_HYBRID_CONFLICT_STEP_H__

#include <test/testexecutestepbase.h>
#include <lbs/lbsnetcommon.h>

#include "ctlbshybridconflictsrv.h"

/** Must be the Id of the protool module used for the test */
const TUid KRequestUid = {0x10282293};

class CT_LbsHybridConflictStep : public CTestStep
{

public:
	virtual ~CT_LbsHybridConflictStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsHybridConflictStep(CT_LbsHybridConflictServer& aParent);
	void ConstructL();
	
	CT_LbsHybridConflictServer& iParent;
};

struct SData			
	{
	TLbsNetSessionId sessionId;
	TVerdict response;
	};


#endif // __CT_LBS_HYBRID_CONFLICT_STEP_H__
