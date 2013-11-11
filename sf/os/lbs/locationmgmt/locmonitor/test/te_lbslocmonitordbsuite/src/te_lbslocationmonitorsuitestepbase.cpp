/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

 
#include "te_lbslocationmonitorsuitestepbase.h"


TVerdict CTe_LbsLocationMonitorSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
    iScheduler = new(ELeave) CActiveScheduler();
    CActiveScheduler::Install(iScheduler);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsLocationMonitorSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	return TestStepResult();
	}

CTe_LbsLocationMonitorSuiteStepBase::~CTe_LbsLocationMonitorSuiteStepBase()
	{
	if(iTimedWaiter)
		{
		delete iTimedWaiter;
		}
	if(iWaiter)
		{
		delete iWaiter;
		}
	delete iScheduler;
	}

CTe_LbsLocationMonitorSuiteStepBase::CTe_LbsLocationMonitorSuiteStepBase():
	iTimedWaiter(NULL),
	iWaiter(NULL)
	{

	}


void CTe_LbsLocationMonitorSuiteStepBase::ComparePositions(TPosition& aLeft, TPosition& aRight)
	{
	TEST(aLeft.HorizontalAccuracy() == aRight.HorizontalAccuracy());
	TEST(aLeft.VerticalAccuracy() == aRight.VerticalAccuracy());
	TEST(aLeft.Latitude() == aRight.Latitude());
	TEST(aLeft.Longitude() == aRight.Longitude());
	TEST(aLeft.Altitude() == aRight.Altitude());
		
	}


void CTe_LbsLocationMonitorSuiteStepBase::InitWaitersL()
	{
	iWaiter = CT_AsyncWaiter::NewL();
	iTimedWaiter = CT_AsyncWaiter::NewL();
	}


void CTe_LbsLocationMonitorSuiteStepBase::DeleteWaiters()
	{
	if(iTimedWaiter)
		{
		delete iTimedWaiter;
		iTimedWaiter = NULL;
		}
	if(iWaiter)
		{
		delete iWaiter;
		iWaiter = NULL;
		}
	}

