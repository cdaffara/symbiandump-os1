/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file ctlbsclientsteplastknownposareasources.cpp
* This is the class implementation for the Last Known Position Area Data Sources Tests
*
*/

 
#include "ctlbsclientsteplastknownposareasources.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>

#include <etel3rdparty.h>
#include <simtsy.h>


// constant definitions
_LIT(KLbsClientStepLastKnownPosAreaSources, "LbsClientStepLastKnownPosAreaSources");

/**
 * Destructor
 */
CT_LbsClientStep_LastKnownPosAreaSources::~CT_LbsClientStep_LastKnownPosAreaSources()
	{
	}


/**
 * Constructor
 */
CT_LbsClientStep_LastKnownPosAreaSources::CT_LbsClientStep_LastKnownPosAreaSources(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent)
	{
	SetTestStepName(KLbsClientStep_LastKnownPosAreaSources);
	}


/**
Static Constructor
*/
CT_LbsClientStep_LastKnownPosAreaSources* CT_LbsClientStep_LastKnownPosAreaSources::New(CT_LbsClientServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsClientStep_LastKnownPosAreaSources* testStep = new CT_LbsClientStep_LastKnownPosAreaSources(aParent);
	if (testStep)
		{
		TInt err = KErrNone;
		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
	return testStep;
	}


void CT_LbsClientStep_LastKnownPosAreaSources::ConstructL()
	{
	}

//from MT_NotifyPosUpdateObserver:
/**
 * 	Callback - called when NotifyPositionUpdate request completes
 */

void CT_LbsClientStep_LastKnownPosAreaSources::NotifyPositionUpdateCallback(TRequestStatus& aStatus)
	{
	TInt err = aStatus.Int();
	if (KErrCancel != err)	
		{
		SetTestStepResult(EFail); // the request always gets cancelled (see test 0122)
		}
	CActiveScheduler::Stop();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_LastKnownPosAreaSources::doTestStepL()
	{
	// Test step used to test the LBS Client Notify Position Area update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_LastKnownPosAreaSources::doTestStepL()"));

	if (TestStepResult()==EPass)
		{
		// Connect to self locate server.	
		TInt err = iServer.Connect();
		User::LeaveIfError(err);
		CleanupClosePushL(iServer);
		//RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		T_LbsUtils utils;
		//utils.ResetAndDestroy_PosInfoArr(posInfoArr);
		//TPositionModuleId modId;
		//modId = utils.GetAGpsModuleIdL(iServer);
		
		
		// Carry out actions.
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			switch (testCaseId)
				{
				// Only Clear the database
				case 1001:
					{
					TRequestStatus emptyStatus;
					iServer.EmptyLastKnownPositionStore(emptyStatus);
					User::WaitForRequest(emptyStatus);
					break;
					}
				// Verify Position and match level
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
					{
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					//1. LastKnown Pos Area
					TPositionInfo posInfo;
					TPositionAreaExtendedInfo matchLevel, expectedMatchLevel;
					err = DoLastKnownPosAreaL(posInfo, matchLevel);
					if (KErrNone != err)
						{
						ERR_PRINTF2(_L("Incorrect err %d returned"), err);
						SetTestStepResult(EFail);
						}
					expectedMatchLevel.SetMobileCountryCodeMatch(ETrue);
					expectedMatchLevel.SetMobileNetworkCodeMatch(ETrue);
					expectedMatchLevel.SetLocationAreaCodeMatch(ETrue);
					expectedMatchLevel.SetCellIdMatch(ETrue);
					VerifyMatchLevel(expectedMatchLevel, matchLevel);
					TPosition lhsPos;
					// MOLR TA
					if(testCaseId == 1)
						{
						lhsPos.SetAccuracy(10.0,10.0);
						lhsPos.SetCoordinate(50.2454,0.1668,1.0);	
						}
					// Cell
					else if(testCaseId == 3)
						{
						lhsPos.SetAccuracy(2.0,3.0);
						lhsPos.SetCoordinate(49.2,3.5,50.0);	
						}
					// BT
					else if(testCaseId == 5)
						{
						lhsPos.SetAccuracy(10.0,30.0);
						lhsPos.SetCoordinate(61.448,23.855,0.0);	
						}
					TPositionInfo lhsPosInfo;
					lhsPosInfo.SetPosition(lhsPos);
					if(!utils.Compare_PosInfo(lhsPosInfo, posInfo, T_LbsUtils::ERoughAccuracy))
						{
						ERR_PRINTF1(_L("Unexpected position returned"));
						SetTestStepResult(EFail);
						}
					//2. Clear Database
					TRequestStatus emptyStatus;
					iServer.EmptyLastKnownPositionStore(emptyStatus);
					User::WaitForRequest(emptyStatus);
					CleanupStack::PopAndDestroy(&iPositioner);
					break;
					}
				default:
					User::Panic(KLbsClientStepLastKnownPosAreaSources, KErrUnknown);					
    		    }
			}
		// All done, clean up.
		
		CleanupStack::PopAndDestroy(&iServer);		
		}
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_LastKnownPosAreaSources::doTestStepL()"));
	return TestStepResult();
	}


void CT_LbsClientStep_LastKnownPosAreaSources::Validate(TPositionInfo& aExpectedPosition, TPositionInfo& aActualPosition, 
		TPositionAreaExtendedInfo& aActualArea, TBool aMccExpected, TBool aMncExpected, TBool aLacExpected, TBool aCidExpected)
	{
	TPositionAreaExtendedInfo expectedArea;
	expectedArea.SetMobileCountryCodeMatch(aMccExpected);
	expectedArea.SetMobileNetworkCodeMatch(aMncExpected);
	expectedArea.SetLocationAreaCodeMatch(aLacExpected);
	expectedArea.SetCellIdMatch(aCidExpected);	
	VerifyMatchLevel(expectedArea, aActualArea);
	
	T_LbsUtils utils;
	TBool equality = utils.Compare_PosInfo(aExpectedPosition, aActualPosition);
	if(!equality)
		{
		ERR_PRINTF1(_L("Position does not match"));
		SetTestStepResult(EFail);
		}
	}


TInt CT_LbsClientStep_LastKnownPosAreaSources::VerifyMatchLevel(TPositionAreaExtendedInfo& aExpected, TPositionAreaExtendedInfo& aActual)
	{
	TInt err = KErrNone;
	if (!(aExpected.MobileCountryCodeMatch() == aActual.MobileCountryCodeMatch() &&
			aExpected.MobileNetworkCodeMatch() == aActual.MobileNetworkCodeMatch() &&
			aExpected.LocationAreaCodeMatch() == aActual.LocationAreaCodeMatch() &&
			aExpected.CellIdMatch() == aActual.CellIdMatch()))
		{
		ERR_PRINTF1(_L("Incorrect Match level"));
		ERR_PRINTF5(_L("Expected: MCC:%d, MNC:%d, LAC:%d, CID:%d"), 
				aExpected.MobileCountryCodeMatch(), 
				aExpected.MobileNetworkCodeMatch(),
				aExpected.LocationAreaCodeMatch(),
				aExpected.CellIdMatch());
		ERR_PRINTF5(_L("Actual: MCC:%d, MNC:%d, LAC:%d, CID:%d"), 
				aActual.MobileCountryCodeMatch(), 
				aActual.MobileNetworkCodeMatch(),
				aActual.LocationAreaCodeMatch(),
				aActual.CellIdMatch());
		SetTestStepResult(EFail);
		err = KErrGeneral;
		}
	return err;
	}

TInt CT_LbsClientStep_LastKnownPosAreaSources::DoLastKnownPosAreaL(TPositionInfoBase& aPosInfo, TPositionAreaExtendedInfo& aMatchLevel)
{
	// Use wrapper active object for the async call below.
	CT_LbsAsyncWaiter*	waiter = CT_LbsAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);
	User::LeaveIfError(iPositioner.SetRequestor(CRequestor::ERequestorService,
												CRequestor::EFormatApplication,
												_L("LbsClient LKPA Tests")));
	iPositioner.GetLastKnownPositionArea(aPosInfo, aMatchLevel, waiter->iStatus);
	// Wait for and process the result.
	waiter->StartAndWait();
	TInt err = waiter->Result();
	if (KErrNone == err)
		{
		INFO_PRINTF1(_L("Last Known Pos Area position obtained successfully"));
		}
	else
		{
		INFO_PRINTF2(_L("Position update failed with error %d"), err);
		}
	CleanupStack::PopAndDestroy(waiter);
	return err;
}
