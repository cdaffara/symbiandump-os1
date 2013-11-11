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
// These are the state transition unit tests for the CLbsSuplPushImpl and related classes
// 
//

#if (!defined __TE_LBSSUPLPUSHSTATE_STEP__)
#define __TE_LBSSUPLPUSHSTATE_STEP__

#include <test/testexecutestepbase.h>

#include "Te_LbsSuplPushBaseStep.h"
#include "Te_LbsSuplPushCreatedTrans.h"

/** The name of the test step */
_LIT(KLbsSuplPushStateStep, "LbsSuplPushStateStep");


/**
The state transition unit tests for the CLbsSuplPushImpl and related classes.
*/
class CTe_LbsSuplPushStateStep : public CTe_LbsSuplPushBaseStep
	{

public:
	CTe_LbsSuplPushStateStep();
	
	virtual CLbsSuplPushImpl* MakeCreatedSuplPushImplLC(MLbsSuplPushObserver& aObserver);
	virtual CLbsSuplPushImpl* MakeWaitingAckSuplPushImplLC(MLbsSuplPushObserver& aObserver);
	virtual CLbsSuplPushImpl* MakeWaitingBusySuplPushImplLC(MLbsSuplPushObserver& aObserver);
	
private:
	void ReallyDoTestStepL();
	void DoNullTransL();
	void DoCreatedTransL();
	void DoWaitingAckTransL();
	void DoWaitingBusyTransL();
	
	};

#endif //__TE_LBSSUPLPUSHSTATE_STEP__
