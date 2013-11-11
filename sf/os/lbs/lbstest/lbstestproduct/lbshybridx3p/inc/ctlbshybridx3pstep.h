/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef __CT_LBS_HYBRID_X3P_STEP_H__
#define __CT_LBS_HYBRID_X3P_STEP_H__


// EPOC includes.
#include <test/testexecutestepbase.h>
#include <lbs/test/ctlbsagpsmoduleeventlistener.h>

#include "ctlbshybridx3pserver.h"

const TUid KX3PRequestUid = {0x1028228F};
/** Delta timout value used in all tests */
const TInt KDelta = 2.0 * 1000 * 1000;	//2 seconds

class CT_LbsHybridX3PStep :	public CTestStep, MT_AgpsModuleEventHandler
{

public:
	virtual ~CT_LbsHybridX3PStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsHybridX3PStep(CT_LbsHybridX3PServer& aParent);

	void ConstructL();

	TPosition iRefPos;
	
	CT_LbsHybridX3PServer& iParent;

private: // from MT_AgpsModuleEventHandler
	void OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions);
	
private:
	RPointerArray<TLbsGpsOptions> iExpectedModuleGpsOptions;
	TInt iExpectedModuleGpsOptionsIndex;
	
	CT_AgpsModuleEventListener* iAgpsModuleEventListener;
	
};

#endif // __CT_LBS_HYBRID_X3P_STEP_H__
