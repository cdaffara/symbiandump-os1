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

#include "negativestep.h"
#include "te_lbslocationmonitorsuitedefs.h"

#include "rlbslocmonitordb.h"

#include <Lbs.h> 


CNegativeStep::~CNegativeStep()
/**
 * Destructor
 */
	{

	}

CNegativeStep::CNegativeStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KNegativeStep);
	}

TVerdict CNegativeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsLocationMonitorSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CNegativeStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test check that KErrArgument is returned when calling the Get function for the 
 * KLbsSpecialFeatureMaximumRequestDelay entry with an out of range delay value.
 */
	{
	__UHEAP_MARK;
	  if (TestStepResult()==EPass)
		{
		TPosition dummyIn;
		dummyIn.SetCoordinate(21.12, 54.45, 10.01);
		dummyIn.SetHorizontalAccuracy(100);
		dummyIn.SetVerticalAccuracy(1000);
		RLbsLocMonitorDb locMonitorDb = RLbsLocMonitorDb();
		// Insert() should return KErrNotFound if db has not been Open()ed
		TRequestStatus unused;
		TLbsLocMonitorAreaInfoGci dummyArea;
		RPointerArray<TLbsLocMonitorAreaInfoBase> dummyArray;
		dummyArray.Append(&dummyArea);
		TInt result = locMonitorDb.SavePosition(dummyIn,dummyArray,ETrue,unused);
		dummyArray.Reset();
		TEST(result == KErrNotFound);
		// GetPosition() should return KErrNotFound if db has not been Open()ed
		TLbsLocMonitorAreaInfoGci dummy;
		TRequestStatus dummyStatus;
		dummyArray.Append(&dummy);
		TPositionAreaExtendedInfo matchDummy;
		result = locMonitorDb.GetPosition(dummyIn,dummyArray,matchDummy,dummyStatus);
		dummyArray.Reset();
		TEST(result == KErrNotFound);
		// ClearDatabase() should return KErrNotFound if db has not been Open()ed
		result = locMonitorDb.ClearDatabase();
		TEST(result == KErrNotFound);
				
		locMonitorDb.OpenL();
		CleanupClosePushL(locMonitorDb);
		CTe_LbsLocationMonitorSuiteStepBase::InitWaitersL();
		locMonitorDb.ClearDatabase();
		
		INFO_PRINTF1(_L("Negative Test on SavePosition()"));
		RPointerArray<TLbsLocMonitorAreaInfoBase> inArray;
		for(TInt w = -1;w < 2;w++)
			{
			for(TInt x = -1;x < 2;x++)
				{
				for(TInt y = -1;y < 2;y++)
					{
					for(TInt z = -1;z < 2;z++)
						{
						for (TInt is3gIndex = 0; is3gIndex < 2; is3gIndex++)
							{
							TRequestStatus status;
							TLbsLocMonitorAreaInfoGci in;
							in.iMcc = w;
							in.iMnc = x;
							in.iLac = y;
							in.iCid = z;
							in.iValidity = ETrue;
							if (is3gIndex == 0)
								{
								in.iIs3gNetworkMode = EFalse;
								}
							else
								{
								in.iIs3gNetworkMode = ETrue;
								}							
							inArray.Append(&in);
							result = locMonitorDb.SavePosition(dummyIn,inArray,ETrue,iWaiter->iStatus);
							if(w >= 0 && x >= 0 && y >= 0 && z >= 0)
								{
								iWaiter->StartAndWait();
								TEST(result >= KErrNone);
								}
							else
								{
								TEST(result == KErrArgument);
								}
							inArray.Reset();
							}
						}
					}
				}
			}
		
		INFO_PRINTF1(_L("Negative Test on GetPosition()"));
		TPosition dummyOut;
		RPointerArray<TLbsLocMonitorAreaInfoBase> outArray;
		for(TInt w = -1;w < 2;w++)
			{
			for(TInt x = -1;x < 2;x++)
				{
				for(TInt y = -1;y < 2;y++)
					{
					for(TInt z = -1;z < 2;z++)
						{
						for (TInt is3gIndex = 0; is3gIndex < 2; is3gIndex++)
							{
							TLbsLocMonitorAreaInfoGci out;
							out.iMnc = w;
							out.iMcc = x;
							out.iLac = y;
							out.iCid = z;
							out.iValidity = ETrue;
							if (is3gIndex == 0)
								{
								out.iIs3gNetworkMode = EFalse;
								}
							else
								{
								out.iIs3gNetworkMode = ETrue;
								}														
							RPointerArray<TLbsLocMonitorAreaInfoBase> outArray;
							outArray.Append(&out);
							TPosition outPosition;
							TPositionAreaExtendedInfo matchLevel;
							result = locMonitorDb.GetPosition(outPosition,outArray,matchLevel,iWaiter->iStatus);
							if(w >= 0 && x >= 0 && y >= 0 && z >= 0)
								{
								iWaiter->StartAndWait();
								TEST(result >= KErrNone);
								ComparePositions(dummyIn,outPosition);
								}
							else
								{
								TEST(result == KErrArgument);
								}
							outArray.Reset();
							}
						}
					}
				}
			}
		locMonitorDb.ClearDatabase();
		CleanupStack::PopAndDestroy(&locMonitorDb);
		DeleteWaiters();
		}
	  __UHEAP_MARKEND;
	  return TestStepResult();
	}



TVerdict CNegativeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{

	return TestStepResult();
	}
