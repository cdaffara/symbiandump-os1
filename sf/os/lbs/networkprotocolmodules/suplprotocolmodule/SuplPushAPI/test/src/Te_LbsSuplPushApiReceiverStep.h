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
// These are the unit tests for the MLbsSuplPushRecObserver, CLbsSuplPushRec, CLbsSuplPushRecImpl classes
// 
//

#if (!defined __TE_LBSSUPLPUSHAPIRECEIVER_STEP_BASE__)
#define __TE_LBSSUPLPUSHAPIRECEIVER_STEP_BASE__

#include <test/testexecutestepbase.h>

#include <lbs/lbssuplpushreceiver.h>

/** The name of the test step */
_LIT(KLbsSuplPushApiReceiverStep, "LbsSuplPushApiReceiverStep");

/**
This test step runs OOM tests while constructing the CLbsSuplPushRec. It also calls some "exotic" 
functions to improve code coverage statistics.
*/
class CTe_LbsSuplPushApiReceiverStep : public CTestStep
	{

public:
	CTe_LbsSuplPushApiReceiverStep();
	virtual ~CTe_LbsSuplPushApiReceiverStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
private:
	/** An active scheduler created and installed for this step */
	CActiveScheduler* iScheduler;
	};

#endif //__TE_LBSSUPLPUSHAPIRECEIVER_STEP_BASE__
