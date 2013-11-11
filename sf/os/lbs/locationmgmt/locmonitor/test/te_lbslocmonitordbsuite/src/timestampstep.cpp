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

#include "timestampstep.h"
#include "te_lbslocationmonitorsuitedefs.h"

#include "rlbslocmonitordb.h"

#include <Lbs.h> 


CTimeStampStep::~CTimeStampStep()
/**
 * Destructor
 */
	{

	}

CTimeStampStep::CTimeStampStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTimeStampStep);
	}

TVerdict CTimeStampStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsLocationMonitorSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTimeStampStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test check that KErrArgument is returned when calling the Get function for the 
 * KLbsSpecialFeatureMaximumRequestDelay entry with an out of range delay value.
 */
	{
	__UHEAP_MARK;
	if (TestStepResult()==EPass)
		{
		RLbsLocMonitorDb locMonitorDb = RLbsLocMonitorDb();
		locMonitorDb.OpenL();
		CleanupClosePushL(locMonitorDb);
		CTe_LbsLocationMonitorSuiteStepBase::InitWaitersL();
		
		locMonitorDb.ClearDatabase();
		
		TPosition dummy1;
		TPosition dummy2;
		TPosition dummy3;
		
		dummy1.SetCoordinate(10.33, 54.22, 10.01);
		dummy1.SetHorizontalAccuracy(100);
		dummy1.SetVerticalAccuracy(1000);
		
		dummy2.SetCoordinate(20.22, 74.11, 20.02);
		dummy2.SetHorizontalAccuracy(100);
		dummy2.SetVerticalAccuracy(1000);

		dummy3.SetCoordinate(30.22, 104.11, 30.03);
		dummy3.SetHorizontalAccuracy(100);
		dummy3.SetVerticalAccuracy(1000);

		INFO_PRINTF1(_L("Saving 3 GSM records, 1 microsecond apart"));
		TLbsLocMonitorAreaInfoGci in1;
		in1.iMcc = 1;
		in1.iMnc = 1;
		in1.iLac = 1;
		in1.iCid = 2;
        in1.iValidity = ETrue;
        in1.iIs3gNetworkMode = EFalse;

		RPointerArray<TLbsLocMonitorAreaInfoBase> arrayIn1;
		arrayIn1.Append(&in1);
		locMonitorDb.SavePosition(dummy1,arrayIn1,ETrue,iWaiter->iStatus);
		iWaiter->StartAndWait();
		arrayIn1.Reset();
		
		User::After(1);
		TLbsLocMonitorAreaInfoGci in2;
		in2.iMcc = 1;
		in2.iMnc = 1;
		in2.iLac = 1;
		in2.iCid = 3;
        in2.iValidity = ETrue;
        in2.iIs3gNetworkMode = EFalse;

		RPointerArray<TLbsLocMonitorAreaInfoBase> arrayIn2;
		arrayIn2.Append(&in2);
		locMonitorDb.SavePosition(dummy2,arrayIn2,ETrue,iWaiter->iStatus);
		iWaiter->StartAndWait();
		arrayIn2.Reset();
		
		User::After(1);
		TLbsLocMonitorAreaInfoGci in3;
		in3.iMcc = 1;
		in3.iMnc = 1;
		in3.iLac = 2;
		in3.iCid = 2;
        in3.iValidity = ETrue;
        in3.iIs3gNetworkMode = EFalse;

		RPointerArray<TLbsLocMonitorAreaInfoBase> arrayIn3;
		arrayIn3.Append(&in3);
		locMonitorDb.SavePosition(dummy1,arrayIn3,ETrue,iWaiter->iStatus);
		iWaiter->StartAndWait();
		arrayIn3.Reset();
		
		INFO_PRINTF1(_L("Doing a GetLastStoredPosition, Checking that record 3 is returned"));
		TPosition outPosition1;
		locMonitorDb.GetLastStoredPosition(outPosition1,iWaiter->iStatus);
		iWaiter->StartAndWait();
		ComparePositions(dummy1, outPosition1);

		// Now add a 4th position which is WCDMA....
		INFO_PRINTF1(_L("Saving a single WCDMA record, 1 microsecond later"));
		User::After(1);
		TLbsLocMonitorAreaInfoGci in4;
		in4.iMcc = 1;
		in4.iMnc = 1;
		in4.iLac = 1;
		in4.iCid = 2;
        in4.iValidity = ETrue;
        in4.iIs3gNetworkMode = ETrue;

		RPointerArray<TLbsLocMonitorAreaInfoBase> arrayIn4;
		arrayIn4.Append(&in4);
		locMonitorDb.SavePosition(dummy3,arrayIn4, ETrue,iWaiter->iStatus);
		iWaiter->StartAndWait();
		arrayIn4.Reset();

		INFO_PRINTF1(_L("Doing a GetPosition that matches 1 & 2, Checking that record 2 is returned"));
		TLbsLocMonitorAreaInfoGci out;
		out.iMnc = 1;
		out.iMcc = 1;
		out.iLac = 1;
		out.iCid = 1;
        out.iValidity = ETrue;
        out.iIs3gNetworkMode = EFalse;

		RPointerArray<TLbsLocMonitorAreaInfoBase> outArray;
		outArray.Append(&out);
		TPosition outPosition2;
		TPositionAreaExtendedInfo matchLevel;
		locMonitorDb.GetPosition(outPosition2,outArray,matchLevel,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		TEST(matchLevel.LocationAreaCodeMatch());
		ComparePositions(dummy2, outPosition2);
		
		INFO_PRINTF1(_L("Doing a GetLastStoredPosition, Checking that WCDMA record 4  is returned"));
		TPosition outPosition3;
		locMonitorDb.GetLastStoredPosition(outPosition3,iWaiter->iStatus);
		iWaiter->StartAndWait();
		ComparePositions(dummy3, outPosition3);
				
		//locMonitorDb.ClearDatabase();
		CleanupStack::PopAndDestroy(&locMonitorDb);
		DeleteWaiters();
		}
	  __UHEAP_MARKEND;
	  return TestStepResult();
	}



TVerdict CTimeStampStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{

	return TestStepResult();
	}
