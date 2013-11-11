/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TE_LBSLOCATIONMONITOR_STEP_BASE__
#define __TE_LBSLOCATIONMONITOR_STEP_BASE__
#include <test/TestExecuteStepBase.h>
#include <lbs.h>
#include "ctasyncwaiter.h"


// Please add your include here if you have 

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsLocationMonitorSuiteStepBase : public CTestStep
	{

public:
	virtual ~CTe_LbsLocationMonitorSuiteStepBase();
	CTe_LbsLocationMonitorSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
	void ComparePositions(TPosition& aLeft, TPosition& aRight);
	void InitWaitersL();
	void DeleteWaiters();

protected:
	CActiveScheduler* iScheduler;
	CT_AsyncWaiter* iTimedWaiter;
	CT_AsyncWaiter* iWaiter;
	};

#endif
