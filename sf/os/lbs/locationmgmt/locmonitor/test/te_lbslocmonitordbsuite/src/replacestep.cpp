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

#include "replacestep.h"
#include "te_lbslocationmonitorsuitedefs.h"

#include "rlbslocmonitordb.h"

#include <Lbs.h> 


CReplaceStep::~CReplaceStep()
/**
 * Destructor
 */
	{

	}

CReplaceStep::CReplaceStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KReplaceStep);
	}

TVerdict CReplaceStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsLocationMonitorSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CReplaceStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test:
 *		i)  checks that we can add new positions for the same network parameters (cell id etc). 
 *		ii) the newest position is retrieved.
 */
	{
	__UHEAP_MARK;
	if (TestStepResult()==EPass)
		{
		RLbsLocMonitorDb locMonitorDb = RLbsLocMonitorDb();
		locMonitorDb.OpenL();
		CleanupClosePushL(locMonitorDb);
		CTe_LbsLocationMonitorSuiteStepBase::InitWaitersL();
		
		// First Reset DB to initial state
		locMonitorDb.ClearDatabase();
		
		// We'll keep re-using these...
		TLbsLocMonitorAreaInfoGci in;
		TLbsLocMonitorAreaInfoGci out;
		RPointerArray<TLbsLocMonitorAreaInfoBase> inArray;
		RPointerArray<TLbsLocMonitorAreaInfoBase> outArray;

		// Add first GSM cell position

		TPosition dummyPos1;
		dummyPos1.SetCoordinate(10.10, 1.01, 11.11);
		dummyPos1.SetHorizontalAccuracy(100);
		dummyPos1.SetVerticalAccuracy(1000);

		INFO_PRINTF1(_L("Saving first position"));
		TRequestStatus status;
		in.iMcc = 120;
		in.iMnc = 101;
		in.iLac = 2000;
		in.iCid = 3000;
		in.iValidity = ETrue;
        in.iIs3gNetworkMode = EFalse;

		inArray.Append(&in);
		locMonitorDb.SavePosition(dummyPos1,inArray,ETrue,iWaiter->iStatus);
		iWaiter->StartAndWait();
		
		// Check first GSM cell position was correctly stored
		INFO_PRINTF1(_L("Retrieving first position"));
		out.iMcc = 120;
		out.iMnc = 101;
		out.iLac = 2000;
		out.iCid = 3000;
        out.iValidity = ETrue;
        out.iIs3gNetworkMode = EFalse;
        
		outArray.Append(&out);
		TPosition outPosition1;
		TPositionAreaExtendedInfo matchLevel; 
		locMonitorDb.GetPosition(outPosition1,outArray,matchLevel,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel.CellIdMatch())
			{
			ERR_PRINTF2(_L("Read Match Level: %d, Expected 4"), matchLevel.Area());
			SetTestStepResult(EFail);
			}
		ComparePositions(dummyPos1,outPosition1);
	
		// Add second GSM cell position
		TPosition dummyPos2;
		dummyPos2.SetCoordinate(20.20, 2.02, 22.22);
		dummyPos2.SetHorizontalAccuracy(200);
		dummyPos2.SetVerticalAccuracy(2000);		
		INFO_PRINTF1(_L("Saving second position"));
		locMonitorDb.SavePosition(dummyPos2,inArray,ETrue,iWaiter->iStatus);
		iWaiter->StartAndWait();
		inArray.Reset();
				
		// Check that second GSM cell position retrieved.
		INFO_PRINTF1(_L("Retrieving second position"));
		TPosition outPosition2;
		outArray.Append(&out);
		locMonitorDb.GetPosition(outPosition2,outArray,matchLevel,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel.CellIdMatch())
			{
			ERR_PRINTF2(_L("Read Match Level: %d, Expected 4"), matchLevel.Area());
			SetTestStepResult(EFail);
			}
		ComparePositions(dummyPos2,outPosition2);

		// Add a WCDMA position, check that it doesn't replace the GSM position.
		TPosition dummyPos3;
		dummyPos3.SetCoordinate(30.30, 3.03, 33.33);
		dummyPos3.SetHorizontalAccuracy(300);
		dummyPos3.SetVerticalAccuracy(3000);

		INFO_PRINTF1(_L("Saving first WCDMA position"));
        in.iIs3gNetworkMode = ETrue;   // we can re-use in but with different mode!
		inArray.Append(&in);
		locMonitorDb.SavePosition(dummyPos3,inArray,ETrue,iWaiter->iStatus);
		iWaiter->StartAndWait();

		INFO_PRINTF1(_L("Retrieving second GSM position, despite similar WCDMA network parameters"));
		outArray.Append(&out);
		TPosition outPosition3;
		locMonitorDb.GetPosition(outPosition3,outArray,matchLevel,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel.CellIdMatch())
			{
			ERR_PRINTF2(_L("Read Match Level: %d, Expected 4"), matchLevel.Area());
			SetTestStepResult(EFail);
			}
		ComparePositions(dummyPos2,outPosition3);

		// Retrieving the first WCDMA position
		out.iIs3gNetworkMode = ETrue;

		outArray.Append(&out);
		TPosition outPosition4;
		locMonitorDb.GetPosition(outPosition4,outArray,matchLevel,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel.CellIdMatch())
			{
			ERR_PRINTF2(_L("Read Match Level: %d, Expected 4"), matchLevel.Area());
			SetTestStepResult(EFail);
			}
		ComparePositions(dummyPos3,outPosition4);

		// Add a second WCDMA position, check that it replaces the first
		TPosition dummyPos4;
		dummyPos4.SetCoordinate(40.40, 4.04, 44.44);
		dummyPos4.SetHorizontalAccuracy(400);
		dummyPos4.SetVerticalAccuracy(4000);

		INFO_PRINTF1(_L("Saving second WCDMA position"));

		locMonitorDb.SavePosition(dummyPos4,inArray,ETrue,iWaiter->iStatus);
		iWaiter->StartAndWait();
		inArray.Reset();

		INFO_PRINTF1(_L("Retrieving second WCDMA position"));
		outArray.Append(&out);
		TPosition outPosition5;
		locMonitorDb.GetPosition(outPosition5,outArray,matchLevel,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel.CellIdMatch())
			{
			ERR_PRINTF2(_L("Read Match Level: %d, Expected 4"), matchLevel.Area());
			SetTestStepResult(EFail);
			}
		ComparePositions(dummyPos4,outPosition5);

		
		// Reset DB to initial state again
		locMonitorDb.ClearDatabase();
		
		CleanupStack::PopAndDestroy(&locMonitorDb);
		DeleteWaiters();
		}
	  __UHEAP_MARKEND;
	  return TestStepResult();
	}



TVerdict CReplaceStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{

	return TestStepResult();
	}
