/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include "persistencestep.h"
#include "te_lbslocationmonitorsuitedefs.h"

#include "rlbslocmonitordb.h"

#include <Lbs.h> 


CPersistenceStep::~CPersistenceStep()
/**
 * Destructor
 */
	{

	}

CPersistenceStep::CPersistenceStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPersistenceStep);
	}

TVerdict CPersistenceStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsLocationMonitorSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CPersistenceStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test checks that we can retrieve GSM and WCDMA cell info stored in a previous step
 */
	{
	__UHEAP_MARK;
	  if (TestStepResult()==EPass)
		{
		RLbsLocMonitorDb locMonitorDb = RLbsLocMonitorDb();
		locMonitorDb.OpenL();
		CleanupClosePushL(locMonitorDb);
		CTe_LbsLocationMonitorSuiteStepBase::InitWaitersL();

		// GSM position that was stored by a previous test (timestampstep.cpp)
		TPosition dummy1;
		dummy1.SetCoordinate(10.33, 54.22, 10.01);
		dummy1.SetHorizontalAccuracy(100);
		dummy1.SetVerticalAccuracy(1000);
				
		// Check GSM position stored by previous test suite can be retrieved
		INFO_PRINTF1(_L("Doing a GetPosition. Checking that a GSM record is returned"));
		TLbsLocMonitorAreaInfoGci out;
		out.iMnc = 1;
		out.iMcc = 1;
		out.iLac = 2;
		out.iCid = 2;
        out.iValidity = ETrue;
        out.iIs3gNetworkMode = EFalse;
		
		RPointerArray<TLbsLocMonitorAreaInfoBase> outArray;
		outArray.Append(&out);
		TPosition outPosition;
		TPositionAreaExtendedInfo matchLevel;
		locMonitorDb.GetPosition(outPosition,outArray,matchLevel,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		TEST(matchLevel.LocationAreaCodeMatch());
		TEST(matchLevel.CellIdMatch());
		ComparePositions(dummy1, outPosition);
		
		// Check WCDMA position stored by previous test suite can be retrieved
		INFO_PRINTF1(_L("Doing a GetPosition. Checking that a WCDMA record is returned"));

		TPosition dummy2;		
		dummy2.SetCoordinate(30.22, 104.11, 30.03);
		dummy2.SetHorizontalAccuracy(100);
		dummy2.SetVerticalAccuracy(1000);

		// Reset the search parameters...
        out.iIs3gNetworkMode = ETrue;
		out.iLac = 1;

		outArray.Append(&out);
		TPosition outPosition2;
		locMonitorDb.GetPosition(outPosition2, outArray, matchLevel,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		TEST(matchLevel.LocationAreaCodeMatch());
		TEST(matchLevel.CellIdMatch());
		ComparePositions(dummy2, outPosition2);

		// Reset DB to initial state again
		locMonitorDb.ClearDatabase();
		
		CleanupStack::PopAndDestroy(&locMonitorDb);
		DeleteWaiters();
		}
	  __UHEAP_MARKEND;
	  return TestStepResult();
	}



TVerdict CPersistenceStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{

	return TestStepResult();
	}
