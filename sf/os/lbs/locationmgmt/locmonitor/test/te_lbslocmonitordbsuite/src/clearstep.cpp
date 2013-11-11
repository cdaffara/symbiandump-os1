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

#include "clearstep.h"
#include "te_lbslocationmonitorsuitedefs.h"

#include "rlbslocmonitordb.h"
#include <f32file.h> 

#include <Lbs.h> 


CClearStep::~CClearStep()
/**
 * Destructor
 */
	{

	}

CClearStep::CClearStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KClearStep);
	}

TVerdict CClearStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsLocationMonitorSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CClearStep::doTestStepL()
/**
 * @return - TVerdict code
 * This test check that KErrArgument is returned when calling the Get function for the 
 * KLbsSpecialFeatureMaximumRequestDelay entry with an out of range delay value.
 */
	{
	__UHEAP_MARK;
	  if (TestStepResult()==EPass)
		{
		#ifdef __WINSCW__
		_LIT(KFileName, "C:\\lastknownlocarea.db");
		#else
		_LIT(KFileName, "E:\\lastknownlocarea.db");
		#endif
		
		RLbsLocMonitorDb locMonitorDb = RLbsLocMonitorDb();
		locMonitorDb.OpenL();
		CleanupClosePushL(locMonitorDb);
		CTe_LbsLocationMonitorSuiteStepBase::InitWaitersL();
		locMonitorDb.ClearDatabase();
		TEntry entry;
		RFs fs = RFs();
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		User::LeaveIfError(fs.Entry(KFileName,entry));
		TInt initialSize = entry.iSize;		
		TPosition dummyPosition;
		for(TInt i = 1; i <= 100; i++)
			{
			TLbsLocMonitorAreaInfoGci in;
			in.iMcc = i;
			in.iMnc = i+1000;
			in.iLac = i+2000;
			in.iCid = i+3000;
	        in.iValidity = ETrue;
	        in.iIs3gNetworkMode = EFalse;
			
			RPointerArray<TLbsLocMonitorAreaInfoBase> inArray;
			inArray.Append(&in);
			locMonitorDb.SavePosition(dummyPosition,inArray,ETrue,iWaiter->iStatus);
			iWaiter->StartAndWait();
			inArray.Reset();
			}
		User::LeaveIfError(fs.Entry(KFileName,entry));
		TInt intermediateSize = entry.iSize;
		INFO_PRINTF1(_L("Checking DB has grown after SavePositions"));
		TEST(intermediateSize > initialSize);
		locMonitorDb.ClearDatabase();
		User::LeaveIfError(fs.Entry(KFileName,entry));
		TInt endSize = entry.iSize;
		INFO_PRINTF1(_L("Checking ClearDatabase() has returned DB to original (empty) size"));
		TEST(endSize == initialSize);
		CleanupStack::PopAndDestroy(&fs);
		CleanupStack::PopAndDestroy(&locMonitorDb);
		DeleteWaiters();
		}
	  __UHEAP_MARKEND;
	  return TestStepResult();
	}



TVerdict CClearStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 */
	{

	return TestStepResult();
	}
