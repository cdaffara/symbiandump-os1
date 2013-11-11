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

#include "dbstep.h"
#include "te_lbslocationmonitorsuitedefs.h"

#include "rlbslocmonitordb.h" 

#include "ctasyncwaiter.h"

const TInt KLowestWcdmaCell = 0x10000;


CDBStep::~CDBStep()
/**
 * Destructor
 */
	{

	}

CDBStep::CDBStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KDBStep);
	}

TVerdict CDBStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsLocationMonitorSuiteStepBase::doTestStepPreambleL();

	iDummyPosition.SetCoordinate(10.33, 54.22, 10.01);
	iDummyPosition.SetHorizontalAccuracy(100);
	iDummyPosition.SetVerticalAccuracy(1000);

	iDummyPosition2.SetCoordinate(11.33, 55.22, 11.01);
	iDummyPosition2.SetHorizontalAccuracy(101);
	iDummyPosition2.SetVerticalAccuracy(1001);


	return TestStepResult();
	}


TVerdict CDBStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test check that KErrArgument is returned when calling the Get function for the 
 * KLbsSpecialFeatureMaximumRequestDelay entry with an out of range delay value.
 */
	{
	__UHEAP_MARK;
	if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("Init locMonitorDb"));
		RLbsLocMonitorDb locMonitorDb = RLbsLocMonitorDb();
		locMonitorDb.OpenL();
		INFO_PRINTF1(_L("Init Waiters"));
		CTe_LbsLocationMonitorSuiteStepBase::InitWaitersL();
		CleanupClosePushL(locMonitorDb);
		
		// First Reset DB to initial state
		INFO_PRINTF1(_L("First Reset DB to initial state"));
		locMonitorDb.ClearDatabase();
		
		INFO_PRINTF1(_L("Adding 1000 records"));
		TTime addStart;
		addStart.UniversalTime();
		// Create 900 GSM cells amd 100 WCDMA cells with the next loop
		for(TInt i = 1; i <= 900; i++)
			{
			//TRequestStatus status;
			TLbsLocMonitorAreaInfoGci in;
			in.iMcc = i;
			in.iMnc = i+1000;
			in.iLac = i+2000;
			in.iCid = i+3000;
			in.iValidity = ETrue;
			in.iIs3gNetworkMode = EFalse;
						
			RPointerArray<TLbsLocMonitorAreaInfoBase> inArray;
			inArray.Append(&in);
			locMonitorDb.SavePosition(iDummyPosition,inArray,ETrue,iWaiter->iStatus);
			iWaiter->StartAndWait();
			inArray.Reset();


			// Generate 100 WCDMA database records as well.
			if (i <100)
				{
				// we deliberately only change the cell id, 3g flag value and the position
				in.iCid = i + KLowestWcdmaCell;
				in.iIs3gNetworkMode = ETrue;

				inArray.Append(&in);
				locMonitorDb.SavePosition(iDummyPosition2,inArray,ETrue,iWaiter->iStatus);
				iWaiter->StartAndWait();
				inArray.Reset();
				}
			}
		
		TTime addEnd;
		addEnd.UniversalTime();
		TTimeIntervalMicroSeconds interval = addEnd.MicroSecondsFrom(addStart);
		
		//INFO_PRINTF2(_L("Start Time %d"), addStart.Int64());
		//INFO_PRINTF2(_L("End Time %d "), addEnd.Int64());
		INFO_PRINTF3(_L("%d Adding 1000 records took %d Microseconds"), 0, interval.Int64());
		
		//
		// Read 5 values from previously created db 
		// As measuring time tsken, info_printfs commented out, but uncomment if test starts to fail! 
		//
		INFO_PRINTF1(_L("Reading 5 records"));
		TTime readStart;
		readStart.UniversalTime();
		
		// First value is a GSM cell that we have an exact match for in the database
		//INFO_PRINTF1(_L("First value, GSM cell we have in the database"));
		TLbsLocMonitorAreaInfoGci out;
		out.iMcc = 51;
		out.iMnc = 1051;
		out.iLac = 2051;
		out.iCid = 3051;
		out.iValidity = ETrue;
		out.iIs3gNetworkMode = EFalse;
		TPosition outPosition;
		RPointerArray<TLbsLocMonitorAreaInfoBase> outArray;
		outArray.Append(&out);
		TPositionAreaExtendedInfo matchLevel1; 
		locMonitorDb.GetPosition(outPosition,outArray,matchLevel1,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel1.CellIdMatch())
			{
			ERR_PRINTF1(_L("Cell Match not found"));
			SetTestStepResult(EFail);
			}
		ComparePositions(iDummyPosition,outPosition);
		if (TestStepResult() == EFail)
			{
			ERR_PRINTF1(_L("Failed on comparison with first value"));
			}
		
        // Here we retrieve a gsm position on LAC, not a similar wcdma one.
		//INFO_PRINTF1(_L("Second record - GSM cell similar to WCDMA that is matched on LAC"));
		out.iMcc = 82;
		out.iMnc = 1082;
		out.iLac = 2082;
		out.iCid = 5454;
		out.iValidity = ETrue;
		out.iIs3gNetworkMode = EFalse;
		outArray.Append(&out);
		TPositionAreaExtendedInfo matchLevel2; 
		locMonitorDb.GetPosition(outPosition,outArray,matchLevel2,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel2.LocationAreaCodeMatch())
			{
			ERR_PRINTF1(_L("LAC Match not found"));
			SetTestStepResult(EFail);
			}
		ComparePositions(iDummyPosition,outPosition);
		if (TestStepResult() == EFail)
			{
			ERR_PRINTF1(_L("Failed on comparison with second value"));
			}


		// Use a WCDMA cell, but expect to retrieve a GSM position matching on Mobile operator code
		//INFO_PRINTF1(_L("Get forth value matching on Mobile operator code (GSM cell position)"));	
		out.iMcc = 423;
		out.iMnc = 1423;
		out.iLac = 4442342;
		out.iCid = 92922;
		out.iValidity = ETrue;
		out.iIs3gNetworkMode = ETrue;

		outArray.Append(&out);
		TPositionAreaExtendedInfo matchLevel3; 
		locMonitorDb.GetPosition(outPosition,outArray,matchLevel3,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel3.MobileNetworkCodeMatch())
			{
			ERR_PRINTF1(_L("MNC match not found"));
			SetTestStepResult(EFail);
			}
		ComparePositions(iDummyPosition,outPosition);
		if (TestStepResult() == EFail)
			{
			ERR_PRINTF1(_L("Failed on comparison with fourth value"));
			}

		// Use a WCDMA cell, but expect to retrieve a GSM position matching only on country code
		//INFO_PRINTF1(_L("Get forth value matching only on country code (GSM cell position)"));	
		out.iMcc = 423;
		out.iMnc = 43535445;
		out.iLac = 4442342;
		out.iCid = 92922;
		out.iValidity = ETrue;
		out.iIs3gNetworkMode = ETrue;

		outArray.Append(&out);
		TPositionAreaExtendedInfo matchLevel4; 
		locMonitorDb.GetPosition(outPosition,outArray, matchLevel4, iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel4.MobileCountryCodeMatch())
			{
			ERR_PRINTF1(_L("MCC match not found"));
			SetTestStepResult(EFail);
			}
		ComparePositions(iDummyPosition,outPosition);
		if (TestStepResult() == EFail)
			{
			ERR_PRINTF1(_L("Failed on comparison with fourth value"));
			}

		// Use a GSM cell and expect to retrieve a GSM position which doesn't match on anything
		//INFO_PRINTF1(_L("Fifth value: get value which doesn't match on anything (GSM cell position)"));	
		out.iMcc = 990;
		out.iMnc = 4353545;
		out.iLac = 4534;
		out.iCid = 5454;
		out.iValidity = ETrue;
		out.iIs3gNetworkMode = EFalse;
		outArray.Append(&out);
		TPositionAreaExtendedInfo matchLevel5;  
		locMonitorDb.GetPosition(outPosition,outArray,matchLevel5,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(matchLevel5.MobileCountryCodeMatch())
			{
			ERR_PRINTF1(_L("Spurious match found"));
			SetTestStepResult(EFail);
			}
		ComparePositions(iDummyPosition,outPosition);
		if (TestStepResult() == EFail)
			{
			ERR_PRINTF1(_L("Failed on comparison with fifth value"));
			}

		TTime readEnd;
		readEnd.UniversalTime();
		TTimeIntervalMicroSeconds interval2 = readEnd.MicroSecondsFrom(readStart);
		

		//INFO_PRINTF2(_L("Start Time %d"), readStart.Int64());
		//INFO_PRINTF2(_L("End Time %d "), readEnd.Int64());
		INFO_PRINTF3(_L("%d Reading 5 records took %d Microseconds"), 0, interval2.Int64());
		if(interval2.Int64() > 2500000)
			{
			ERR_PRINTF2(_L("Retrieving 5 records took: %d microseconds, Expected less than 2500000"), interval2.Int64());
			}

		// Here we retrieve a wcdma position on LAC value, not a similar gsm one. So we set
		// up a wcdma cell that we don't have in the database
		//INFO_PRINTF1(_L("Get third record matched on LAC, WCDMA cell this time"));
		out.iMcc = 10;
		out.iMnc = 1010;
		out.iLac = 2010;
		out.iCid = KLowestWcdmaCell + 500;
		out.iValidity = ETrue;
		out.iIs3gNetworkMode = ETrue;		
		outArray.Append(&out);
		TPositionAreaExtendedInfo matchLevel6;  
		locMonitorDb.GetPosition(outPosition, outArray, matchLevel6, iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(!matchLevel6.LocationAreaCodeMatch())
			{
			ERR_PRINTF1(_L("LAC Match not found"));
			SetTestStepResult(EFail);
			}
		ComparePositions(iDummyPosition2,outPosition);
		if (TestStepResult() == EFail)
			{
			ERR_PRINTF1(_L("Failed on comparison with third value"));
			}

		INFO_PRINTF1(_L("Performing a read with a second client"));
		RLbsLocMonitorDb locMonitorDb2 = RLbsLocMonitorDb();
		locMonitorDb2.OpenL();
		CleanupClosePushL(locMonitorDb2);
		out.iMcc = 34000023;
		out.iMnc = 4353545;
		out.iLac = 4534;
		out.iCid = 5454;
		out.iValidity = ETrue;
		out.iIs3gNetworkMode = EFalse;
		outArray.Append(&out);
		TPositionAreaExtendedInfo matchLevel7;  
		locMonitorDb2.GetPosition(outPosition, outArray, matchLevel7,iWaiter->iStatus);
		iWaiter->StartAndWait();
		outArray.Reset();
		if(matchLevel7.MobileCountryCodeMatch())
			{
			ERR_PRINTF1(_L("Spurious match found"));
			SetTestStepResult(EFail);
			}
		ComparePositions(iDummyPosition,outPosition);
		CleanupStack::PopAndDestroy(&locMonitorDb2);
		locMonitorDb.ClearDatabase();
		CleanupStack::PopAndDestroy(&locMonitorDb);
		DeleteWaiters();
		}
	__UHEAP_MARKEND;
    return TestStepResult();
	}


TVerdict CDBStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{

	return TestStepResult();
	}
