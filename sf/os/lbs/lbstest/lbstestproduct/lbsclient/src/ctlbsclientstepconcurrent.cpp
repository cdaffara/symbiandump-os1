// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsclientstepconcurrent.cpp
// 
//

// LBS includes.
#include <lbspositioninfo.h>
#include <lbsareainfo.h>
#include <e32base.h>
#include <lbs.h>

#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>

#include <etel3rdparty.h>
#include <simtsy.h>


// LBS test includes.
#include "ctlbsclientstepconcurrent.h"

const TInt KSimTsyTransitionDelay = 4000000; // Currently it seems to take the SimTsy 3 seconds to change cell

//Literals Used
_LIT(KTestCaseId, "tc_id");
_LIT(KPosInfosFile, "pos_infos_file");
_LIT(KLbsModuleType, "module_type");
_LIT(KLbsModuleType_Real, "real");

/**
 * Destructor
 */
CT_LbsClientStep_Concurrent::~CT_LbsClientStep_Concurrent()
	{
	// Delete this active scheduler
	if (iScheduler)
		delete iScheduler;

	// Clear arrays.
	for (TInt i = 0; i < iVerifyPosInfoArr.Count(); ++i)
		delete iVerifyPosInfoArr[i];
	iVerifyPosInfoArr.Close();

	for (TInt i = 0; i < iCurrentPosInfoArr.Count(); ++i)
		delete iCurrentPosInfoArr[i];
	iCurrentPosInfoArr.Close();
	}

/**
 * Constructor
 */
CT_LbsClientStep_Concurrent::CT_LbsClientStep_Concurrent()
	{
	iTestModuleInUse = ETrue;
	SetTestStepName(KLbsClientStepConcurrent);
	}

/**
Static Constructor
*/
CT_LbsClientStep_Concurrent* CT_LbsClientStep_Concurrent::New()
	{
	CT_LbsClientStep_Concurrent* testStep = new CT_LbsClientStep_Concurrent();
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
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

void CT_LbsClientStep_Concurrent::ConstructL()
	{
	
	// Create this objects active scheduler in 
	// case some tests needs it.
	iScheduler = new (ELeave) CActiveScheduler();

	CActiveScheduler::Install(iScheduler);
	}

/**
 * @return - TVerdict code
 */
TVerdict CT_LbsClientStep_Concurrent::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("   CT_LbsClientStep_Concurrent::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		TPositionAreaExtendedInfo matchLevel, expectedMatchLevel;
		// Connect to self locate server.
		TInt err = iServer.Connect();
		User::LeaveIfError(err);
		CleanupClosePushL(iServer);
		TPositionModuleId modId;
		T_LbsUtils utils;
		modId = utils.GetAGpsModuleIdL(iServer);
		User::LeaveIfError(iPositioner.Open(iServer, modId));
		CleanupClosePushL(iPositioner);
		
		
		// Create the verifiable position info
		// CreateVerifyPositionInfoL();

		// Carry out actions required by each specific test (id).
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				case 0:
					{
					// This test is for setting up the database. It is sometimes
					// run sequentially before starting other concurrent tests.
					
					// First clear the DB.
					TRequestStatus emptyStatus;
					iServer.EmptyLastKnownPositionStore(emptyStatus);
					User::WaitForRequest(emptyStatus);
					
					TPositionInfo* posInfo = new (ELeave) TPositionInfo();
					//Move to next cell in sim tsy config file to ensure
					// the Location Monitor DB has at least information about
					// one area/cell Id.
					RProperty::Set(KUidPSSimTsyCategory,
							KPSSimTsyTimersReduceTime, KReduceSimTsyTimers);
					User::After(KSimTsyTransitionDelay);

					// Issue NPUD waiting for result to make sure a position
					// has gone into the Location Monitor database before
					// the real test starts
					TRequestStatus status;
					iPositioner.NotifyPositionUpdate(*posInfo, status);
					User::WaitForRequest(status);
					delete posInfo;
					break;
					}
				case 1:
					{
					//
					// This test requests an NPUD and a GLKPA from the
					// same subsession
					//
					TPositionInfo* posInfo1 = new (ELeave) TPositionInfo();
					TPositionInfo* posInfo2 = new (ELeave) TPositionInfo();
					iCurrentPosInfoArr.Append(posInfo1);
					iCurrentPosInfoArr.Append(posInfo2);
					
					//1. Move to next cell in sim tsy config file
					//RProperty::Set(KUidPSSimTsyCategory,
					//		KPSSimTsyTimersReduceTime, KReduceSimTsyTimers);
					//User::After(KSimTsyTransitionDelay);

					//2. NPUD waiting for result to make sure a position
					// has gone into the Location Monitor database before
					// the real test starts
					TRequestStatus status1;
					//iPositioner.NotifyPositionUpdate(*posInfo1, status1);
					//User::WaitForRequest(status1);
					
					//3. NPUD that will be simultaneous to a GLKPA (step 4)
					iPositioner.NotifyPositionUpdate(*posInfo1, status1);

					//4. GLKPA that is simultaneous to the NPUD at step 3
					TRequestStatus status2;
					iPositioner.GetLastKnownPositionArea(*posInfo2 , matchLevel, status2);					

					User::WaitForRequest(status1);
					User::WaitForRequest(status2);
					
					// NOTE: in concurrent mode, this validation only makes sense when
					// the integration module has been asked to reply always with the same
					// position. This is because at this point there's no way to know if the
					// posInfo2 will contain the position that went into the Location Monitor's
					// Database after step 3 above or the value returned by the integration
					// module after a concurrent step got its response
					// to the NPUD in step 3 above. If the position returned by the integration
					// module is always the same, then the validation should pass.
					TESTL(KErrNone == status2.Int());
					Validate(*posInfo1, *posInfo2, matchLevel, ETrue, ETrue, ETrue, ETrue);
					break;
					}
					
				case 2:
					{
					//
					// This test requests an GLKP and a GLKPA from the
					// same subsession
					//
					TPositionInfo* posInfo3 = new (ELeave) TPositionInfo();
					TPositionInfo* posInfo4 = new (ELeave) TPositionInfo();
					iCurrentPosInfoArr.Append(posInfo3);
					iCurrentPosInfoArr.Append(posInfo4);

					
					
					//1. Move to next cell in sim tsy config file
					//RProperty::Set(KUidPSSimTsyCategory,
					//		KPSSimTsyTimersReduceTime, KReduceSimTsyTimers);
					//User::After(KSimTsyTransitionDelay);

					//2. NPUD waiting for result to make sure a position
					// has gone into the Location Monitor database before
					// the real test starts
					TRequestStatus status3;
					//iPositioner.NotifyPositionUpdate(*posInfo3, status3);
					//User::WaitForRequest(status3);
					
					//3. GLKP that will be simultaneous to a GLKPA (step 4)
					iPositioner.GetLastKnownPosition(*posInfo3, status3);

					//4. GLKPA that is simultaneous to the NPUD at step 3
					TRequestStatus status4;
					iPositioner.GetLastKnownPositionArea(*posInfo4 , matchLevel, status4);					

					User::WaitForRequest(status3);
					User::WaitForRequest(status4);
					
					// NOTE: in concurrent mode, this validation only makes sense when
					// the integration module has been asked to reply always with the same
					// position. This is because at this point there's no way to know if the
					// posInfo2 will contain the position that went into the Location Monitor's
					// Database after step 3 above or the value returned by the integration
					// module after a concurrent step got its response
					// to the NPUD in step 3 above. If the position returned by the integration
					// module is always the same, then the validation should pass.
					TESTL(KErrNone == status4.Int());
					Validate(*posInfo3, *posInfo4, matchLevel, ETrue, ETrue, ETrue, ETrue);
					break;
					}
					
				case 3:
					{
					//
					// This test only requests GLKPA (meant to be used 
					// concurrently with other test cases in this file)
					//
					TPositionInfo* posInfo5 = new (ELeave) TPositionInfo();
					TPositionInfo* posInfo6 = new (ELeave) TPositionInfo();
					iCurrentPosInfoArr.Append(posInfo5);
					iCurrentPosInfoArr.Append(posInfo6);

					TRequestStatus status5;

					iPositioner.GetLastKnownPositionArea(*posInfo6 , matchLevel, status5);					

					User::WaitForRequest(status5);
					
					TESTL(KErrNone == status5.Int());
					
					break;
					}
					
				case 31: 
					{
					//
					// This test requests GLKPA then cancels it (meant to be used 
					// concurrently with other test cases in this file)
					//
					TRequestStatus status6;

					TPositionInfo* posInfo7 = new (ELeave) TPositionInfo();
					TPositionInfo* posInfo8 = new (ELeave) TPositionInfo();
					iCurrentPosInfoArr.Append(posInfo7);
					iCurrentPosInfoArr.Append(posInfo8);
					
					//3. GLKPA 
					iPositioner.GetLastKnownPositionArea(*posInfo8 , matchLevel, status6);
					
					//4. Cancel the above request
					err = iPositioner.CancelRequest(RPositioner::EReqGetLastKnownPositionArea);

					User::WaitForRequest(status6);
					
					// The cancel may either return KErrNone, in which case the GLKPA
					// must complete with KErrCancel or the cancel may complete with
					// KErrNotFound, in which case the GLKPA should complete normally
					// with KErrNone
					INFO_PRINTF2(_L("GLKPA completed with completion code %d"), status6.Int());
					if(err != KErrNone && err != KErrNotFound)
						{
						ERR_PRINTF2(_L("Incorrect Cancel error: %d"), err);
						SetTestStepResult(EFail);
						}
					else if ((err == KErrNone) && (status6.Int() != KErrCancel))
						{
						ERR_PRINTF2(_L("Incorrect GLKPA completion code: %d"), status6.Int());
						SetTestStepResult(EFail);	
						}
					else if ((err == KErrNotFound) && (status6.Int() != KErrNone))
						{
						ERR_PRINTF2(_L("Incorrect GLKPA completion code: %d"), status6.Int());
						SetTestStepResult(EFail);	
						}
					
					break;
					}

				case 4:
					{
					TRequestStatus emptyStatus;
					iServer.EmptyLastKnownPositionStore(emptyStatus);
					User::WaitForRequest(emptyStatus);
					if(emptyStatus.Int() != KErrNone)
						{
						ERR_PRINTF2(_L("Incorrect err %d returned"), emptyStatus.Int());
						SetTestStepResult(EFail);
						}
					break;
					}
					
				case 41: 
					{
					TRequestStatus emptyStatus;
					iServer.EmptyLastKnownPositionStore(emptyStatus);
					//5. Cancel Clear Database
					iServer.CancelRequest(RPositionServer::EReqEmptyLastKnownPositionStore);
					User::WaitForRequest(emptyStatus);
					if(emptyStatus.Int() != KErrNone && emptyStatus.Int() != KErrCancel)
						{
						ERR_PRINTF2(_L("Incorrect err %d returned"), emptyStatus.Int());
						SetTestStepResult(EFail);
						}
					break;
					}
				default:
					{
					INFO_PRINTF2(_L("<FONT><B>   Case number %d did not match any expected value</B></FONT>"),iTestCaseId);
					User::Panic(KLbsClientStepConcurrent, KErrUnknown);
					}
    		    }	
			}
						
			// Unless the testcase has been testcase 0000, whose only purpose is
			// setting up the Location Monitor DB, delete the database after each
			// test. Waiting two seconds before deleting the database gives
			// concurrent testcases terminating later a chance to use the database.
			// 
			//if (iTestCaseId != 0)
			//	{
			//	User::After(2000000); // 2 seconds
			//	TRequestStatus emptyStatus;
			//	iServer.EmptyLastKnownPositionStore(emptyStatus);
			//	User::WaitForRequest(emptyStatus);
			//	}

			
			// All done, clean up.
			CleanupStack::PopAndDestroy(&iPositioner);
			CleanupStack::PopAndDestroy(&iServer);	
		}
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_Concurrent::doTestStepL()"));
	return TestStepResult();
	}


void CT_LbsClientStep_Concurrent::CreateVerifyPositionInfoL()
{
		INFO_PRINTF1(_L("   CreateVerifyPositionInfoL"));

		TPtrC configFileName;
		GetStringFromConfig(ConfigSection(), KPosInfosFile, configFileName);

		T_LbsUtils utils;

		utils.GetConfigured_PosInfosL(configFileName, ConfigSection(), iVerifyPosInfoArr); // Note previous entries will be cleared.

		// Notify the test module that it's required to update it's test data.
		TPtrC modType;
		GetStringFromConfig(ConfigSection(), KLbsModuleType, modType);
		if (modType != KLbsModuleType_Real)
			{

			TModuleDataIn modDataIn;

			modDataIn.iRequestType = TModuleDataIn::EModuleRequestConfig;
			modDataIn.iConfigFileName = configFileName;
			modDataIn.iConfigSection = ConfigSection();

			// Now publish the ini file and section name to the test module - will block.
			utils.NotifyModuleOfConfigChangeL(modDataIn);
			}
		else
			{
			iTestModuleInUse = EFalse;	// Indicate to test harness the real A-GPS integration module is being used.
			}
}

void CT_LbsClientStep_Concurrent::VerifyPosInfo()
{
	INFO_PRINTF1(_L("   VerifyPosInfo"));

	T_LbsUtils utils;
	TBool res;
	// Verify arrays are equal.
	if(iTestModuleInUse)
		res = utils.Compare_PosInfoArr(iVerifyPosInfoArr, iCurrentPosInfoArr, T_LbsUtils::EExactAccuracy);
	else
		res = utils.Compare_PosInfoArr(iVerifyPosInfoArr, iCurrentPosInfoArr, T_LbsUtils::ERoughAccuracy);

	// Fail test if arrays do not match.
	if (!res)
    	{
    	INFO_PRINTF1(_L("   Positions didnt match"));
		SetTestStepResult(EFail);
	    }
}

void CT_LbsClientStep_Concurrent::Validate(TPositionInfo& aExpectedPosition, TPositionInfo& aActualPosition, 
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


TInt CT_LbsClientStep_Concurrent::VerifyMatchLevel(TPositionAreaExtendedInfo& aExpected, TPositionAreaExtendedInfo& aActual)
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


