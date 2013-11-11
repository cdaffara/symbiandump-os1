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
// @file ctlbsclientsteppartialupdate.cpp
// This is the class implementation for the UpdateOptions Tests
// 
//
 
#include "ctlbsclientsteppartialupdate.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>

#define EXPECTED_GPS_ACCURACY_HORIZONTAL 20
#define EXPECTED_GPS_ACCURACY_VERTICAL 20

#define KLbsKeepAlivePeriod 		10000

#define REFPOS_LAT				2
#define REFPOS_LONG				3
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	100
#define REFPOS_VERT_ACCURACY	100

#define REFPOS_HORZ_ACCURACY_BETTER	5
#define REFPOS_VERT_ACCURACY_BETTER	5


const TInt KTestAssistanceDataProviderPluginUidValue = 0x10281D77;
const TInt KRealAssistanceDataProviderPluginUidValue = 0x1028225B;


/**
 * Destructor
 */
CT_LbsClientStep_PartialUpdate::~CT_LbsClientStep_PartialUpdate()
	{
	delete iDoPosUpdatePtr;
	}


/**
 * Constructor
 */
CT_LbsClientStep_PartialUpdate::CT_LbsClientStep_PartialUpdate(CT_LbsClientServer& aParent) : CT_LbsNetSimStep(), iParent(aParent)
	{
	SetTestStepName(KLbsClientStep_PartialUpdate);
	}


/**
Static Constructor
*/
CT_LbsClientStep_PartialUpdate* CT_LbsClientStep_PartialUpdate::New(CT_LbsClientServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsClientStep_PartialUpdate* testStep = new CT_LbsClientStep_PartialUpdate(aParent);
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
	
void CT_LbsClientStep_PartialUpdate::ConstructL()
	{
	// Create the base class objects.
    CT_LbsNetSimStep::ConstructL();
	
	// and the active object wrapper for the notify position update.
	iDoPosUpdatePtr = CT_LbsDoPosUpdate::NewL(this, TPositionModuleInfo::ETechnologyTerminal);

	}

	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_PartialUpdate::doTestStepL()
	{
	// Generic test step used to test the LBS SetUpdateOptions API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_PartialUpdate::doTestStepL()"));

	if (TestStepResult()==EPass) 
		{
		TInt err = KErrNone;
		iPosUpdateStatus = EPositionUpdateStart;
		
		//Configure partial Updates
		TPositionUpdateOptions optsA;
		optsA.SetAcceptPartialUpdates(ETrue);
			
		if(!optsA.AcceptPartialUpdates())
			{
			INFO_PRINTF1(_L("Partial Updates not set."));
			SetTestStepResult(EFail);
			}	
		
		err = iDoPosUpdatePtr->SetOptions(optsA);
		User::LeaveIfError(err);
		
		CLbsAdmin* lbsAdminApi = CLbsAdmin::NewL();
		CleanupStack::PushL(lbsAdminApi);

		// Carryout unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
				switch (iTestCaseId)
				{
				// Test case LBS-Partial-Update-Options-0001
				case 1:
					{
					CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsAutonomous;
					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);

					//Configure Partial Update to EFalse
					TPositionUpdateOptions updOpts;
					updOpts.SetUpdateTimeOut(10*1000000);
					updOpts.SetAcceptPartialUpdates(EFalse);
					if(EFalse != updOpts.AcceptPartialUpdates())
						{
						INFO_PRINTF1(_L("Partial Updates not set."));
						SetTestStepResult(EFail);
						}	
					
					err = iDoPosUpdatePtr->SetOptions(updOpts);
					User::LeaveIfError(err);
					
					break;
					}
	
				// Test case LBS-Partial-Update-Options-0002
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
					{
					CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsAutonomous;
					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);

					break;
					}
					
				case 21:
				case 22:
				case 24:
				case 25:
				case 26:
					{
					CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsPreferTerminalBased;
					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);

					break;
					}
					
				case 23:
					{
					CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsPreferTerminalBased;
					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);

					//Configure Partial Update to EFalse
					TPositionUpdateOptions updOpts;
					updOpts.SetAcceptPartialUpdates(EFalse);
					if(EFalse != updOpts.AcceptPartialUpdates())
						{
						INFO_PRINTF1(_L("Partial Updates not set."));
						SetTestStepResult(EFail);
						}	
					
					err = iDoPosUpdatePtr->SetOptions(updOpts);
					User::LeaveIfError(err);
					
					break;
					}

				case 27:
				case 28:
					{
					//Tracking
					CLbsAdmin::TGpsMode gpsMode;
					if(iTestCaseId == 27)
						{
						gpsMode = CLbsAdmin::EGpsAutonomous;
						}
					else
						{
						gpsMode = CLbsAdmin::EGpsPreferTerminalBased;
						}

					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);

					TPositionUpdateOptions updOpts;
					updOpts.SetUpdateInterval(10*1000000); //Set Update Interval to 10 secs
					updOpts.SetAcceptPartialUpdates(ETrue);
					
					err = iDoPosUpdatePtr->SetOptions(updOpts);
					User::LeaveIfError(err);

					break;
					}

				default:
					User::Panic(KLbsClientStep_PartialUpdate, KErrUnknown);					
						
					}
				}
		
		User::LeaveIfError(OpenNetSim());

		// Kick off the test abort and keep alive timers.
		TTimeIntervalMicroSeconds32 keepAliveInterval(KLbsKeepAlivePeriod);
		iKeepAliveTimer->SetTimer(keepAliveInterval);

		// Kick off test.
		CActiveScheduler::Start();

		// Verify location data.
		VerifyPosInfos();

		//Reset Test module timeout
		TModuleDataIn modDataIn; 
		modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
		modDataIn.iTimeOut = 0;

		T_LbsUtils utils;
		utils.NotifyModuleOfConfigChangeL(modDataIn);

		// Clean up.
		CloseNetSim();
		
		CleanupStack::PopAndDestroy(lbsAdminApi);
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_PartialUpdate::doTestStepL()"));

	return TestStepResult();
	}


void CT_LbsClientStep_PartialUpdate::Connected()
	{
	// Call base implementation.
	//CT_LbsNetSimStep::Connected();
	
	// Create a posinfo and store in our shared array for later verification.
	T_LbsUtils utils;
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

	iPosInfoPtr1 = new(ELeave) TPositionInfo();
	posInfoArr.Append(iPosInfoPtr1);
	iPosInfoPtr2 = new(ELeave) TPositionInfo();
	posInfoArr.Append(iPosInfoPtr2);

	//For test case 22, Set Reference Position better than required position
	if(iTestCaseId == 22)
		{
		// Set the reference position, which also be used for any required verification.
		iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
		iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY_BETTER, REFPOS_VERT_ACCURACY_BETTER);
		iRefPos.SetCurrentTime();

		if (!iNetSim.SetReferenceLocation(iRefPos))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
			SetTestStepResult(EFail);
			}	
		}

	// Kick off pos update.
	iDoPosUpdatePtr->StartL(*iPosInfoPtr1);
	}


void CT_LbsClientStep_PartialUpdate::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("Got - Client Notify Update Complete - Callback Event."));
	
	T_LbsUtils utils;
	TInt err = aStatus.Int();
	
	switch(iTestCaseId)
		{
		case 1:
			{
			if ((iExpectedApiBehaviour == EApiVariant1) && (KPositionQualityLoss != err))
				{
				INFO_PRINTF1(_L("Failed. Didn't get expected qualityLoss error on variant1"));
				SetTestStepResult(EFail);
				}
			else if((iExpectedApiBehaviour == EApiVariant2) && (KErrNone != err))
				{
				INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error on variant2"));
				SetTestStepResult(EFail);
				}
			iState = EWaiting;
			break;
			}

		case 2:
			if ((iExpectedApiBehaviour == EApiVariant1) && (KPositionPartialUpdate != err)) 
				{
				INFO_PRINTF1(_L("Failed. Didn't get expected KPositionPartialUpdate error on variant1"));
				SetTestStepResult(EFail);				
				}
			else if ((iExpectedApiBehaviour == EApiVariant2) && (KErrNone != err))
				{
				INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error on variant2"));
				__ASSERT_DEBUG(iExpectedApiBehaviour == EApiVariant2, User::Panic(_L("Unrecognised 'Expected Api Behaviour'"), 1));
				SetTestStepResult(EFail);			
				}
			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				// Kick off pos update.
				iDoPosUpdatePtr->StartL(*iPosInfoPtr2);
				iPosUpdateStatus = EPositionUpdateComplete;				
				}
			else
				iState = EWaiting;
			
			break;		
			
		case 3:
			{
			if (KPositionPartialUpdate != err)
				{
				INFO_PRINTF1(_L("Failed. Didn't get expected KPositionPartialUpdate error"));
				SetTestStepResult(EFail);
				}	

			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				// Kick off pos update.
				iDoPosUpdatePtr->StartL(*iPosInfoPtr2);
				iPosUpdateStatus = EPositionUpdateComplete;				
				}
			else
				iState = EWaiting;
			
			break;
			}

		case 4:
			{
			if (KErrNone != err)
				{
				INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error"));
				SetTestStepResult(EFail);
				}	

			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				// Kick off pos update.
				iDoPosUpdatePtr->StartL(*iPosInfoPtr2);
				iPosUpdateStatus = EPositionUpdateComplete;				
				}
			else
				iState = EWaiting;

			break;
			}

		case 5:
		case 6:
		
		case 21:
		case 22:
			{
			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				if(iTestCaseId == 5 || iTestCaseId == 21)
					{
					if (KPositionPartialUpdate != err)
						{
						INFO_PRINTF1(_L("Failed. Didn't get expected KPositionPartialUpdate error"));
						SetTestStepResult(EFail);
						}	
					}
				else
					{
					if (KErrNone != err)
						{
						INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error"));
						SetTestStepResult(EFail);
						}	
					}
				}
			else
				{
				if (KPositionEarlyComplete != err)
					{
					INFO_PRINTF1(_L("Failed. Didn't get expected KPositionEarlyComplete error"));
					SetTestStepResult(EFail);
					}	
				}

			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				TModuleDataIn modDataIn; // Used to send test information to the test module.
				modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
				modDataIn.iTimeOut = 5*1000000;
				
				// Send timeout value to test module - will block.
				utils.NotifyModuleOfConfigChangeL(modDataIn);

				// Kick off pos update.
				iDoPosUpdatePtr->StartL(*iPosInfoPtr2);

				if(iExpectedApiBehaviour == EApiVariant1)
					{				
					err = iDoPosUpdatePtr->CompleteRequest(EPositionerNotifyPositionUpdate);
					if (err != KErrNone)
						{
						INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error"));
						SetTestStepResult(EFail);
						}	
					}
				iPosUpdateStatus = EPositionUpdateComplete;				

				}
			else
				{
				//if(iTestCaseId == 5 || iTestCaseId == 6)
					{
					iState = EWaiting;	
					}
				}

			break;
			}
			
		case 7:
			{
			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				if (KPositionPartialUpdate != err)
					{
					INFO_PRINTF1(_L("Failed. Didn't get expected KPositionPartialUpdate error"));
					SetTestStepResult(EFail);
					}	
					
				// Kick off pos update.
				iDoPosUpdatePtr->StartL(*iPosInfoPtr2);
				iPosUpdateStatus = EPositionUpdateComplete;				
				}
			else
				{
				if (KErrNone != err)
					{
					INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error"));
					SetTestStepResult(EFail);
					}	
					
				if(iExpectedApiBehaviour == EApiVariant1)
					{				
					err = iDoPosUpdatePtr->CompleteRequest(EPositionerNotifyPositionUpdate);
					if (err != KErrNotFound)
						{
						INFO_PRINTF1(_L("Failed. Didn't get expected KErrNotFound error"));
						SetTestStepResult(EFail);
						}	
					}
				iState = EWaiting;	
				}

			break;
			}			
		
		case 23:
			{
			if (KErrNone != err)
				{
				INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error"));
				SetTestStepResult(EFail);
				}	

			break;
			}

		case 24:
		case 27:
			{
			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				if (KPositionPartialUpdate != err)
					{
					INFO_PRINTF1(_L("Failed. Didn't get expected KPositionPartialUpdate error"));
					SetTestStepResult(EFail);
					}

				if(iPosUpdateStatus == EPositionUpdateStart)
					{
					iDoPosUpdatePtr->StartL(*iPosInfoPtr2);
					iPosUpdateStatus = EPositionUpdateContinue;
					}
				else
					{
					TModuleDataIn modDataIn; // Used to send test information to the test module.
					modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
					modDataIn.iTimeOut = 5*1000000;
					
					// Send timeout value to test module - will block.
					utils.NotifyModuleOfConfigChangeL(modDataIn);

					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfoPtr3;
					posInfoPtr3 = new(ELeave) TPositionInfo();
					posInfoArr.Append(posInfoPtr3);
					
					// Kick off pos update.
					iDoPosUpdatePtr->StartL(*posInfoPtr3);

					if(iExpectedApiBehaviour == EApiVariant1)
						{						
						err = iDoPosUpdatePtr->CompleteRequest(EPositionerNotifyPositionUpdate);
						if (err != KErrNone)
							{
							INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error"));
							SetTestStepResult(EFail);
							}	
						}
	
					iPosUpdateStatus = EPositionUpdateComplete;				
					}
				}
			else
				{
				if (KPositionEarlyComplete != err)
					{
					INFO_PRINTF1(_L("Failed. Didn't get expected KPositionEarlyComplete error"));
					SetTestStepResult(EFail);
					}	
					
				//if (iTestCaseId == 27)
				//	{
					iState = EWaiting;
				//	}	
				}

			break;
			}

		case 25:
		case 26:
			{
			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				if (KPositionPartialUpdate != err)
					{
					INFO_PRINTF1(_L("Failed. Didn't get expected KPositionPartialUpdate error"));
					SetTestStepResult(EFail);
					}

				if(iPosUpdateStatus == EPositionUpdateStart)
					{
					iDoPosUpdatePtr->StartL(*iPosInfoPtr2);
					iPosUpdateStatus = EPositionUpdateContinue;
					}
				else
					{
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfoPtr3;
					posInfoPtr3 = new(ELeave) TPositionInfo();
					posInfoArr.Append(posInfoPtr3);
					
					// Kick off pos update.
					iDoPosUpdatePtr->StartL(*posInfoPtr3);

					iPosUpdateStatus = EPositionUpdateComplete;				
					}
				}
			else
				{
				if (KPositionPartialUpdate != err && KPositionQualityLoss != err)
					{
					_LIT(KUnexpectedErrorWhilePartialUpdate, "Unexpected error returned: %d");
					INFO_PRINTF2(KUnexpectedErrorWhilePartialUpdate, err);
					SetTestStepResult(EFail);
					}
						
				if(iExpectedApiBehaviour == EApiVariant1)
					{					
					err = iDoPosUpdatePtr->CompleteRequest(EPositionerNotifyPositionUpdate);
					if (err != KErrNotFound)
						{
						INFO_PRINTF1(_L("Failed. Didn't get expected KErrNotFound error"));
						SetTestStepResult(EFail);
						}	
					}
				iState =EWaiting;
				
				TPositionInfo* curPosInfo = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[0]);
				TPositionInfo* curPosInfo2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[1]);
				TPositionInfo* curPosInfo3 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[2]);
				}

			break;
			}

		case 28:
			{
			if(iPosUpdateStatus != EPositionUpdateComplete)
				{
				if (KPositionPartialUpdate != err)
					{
					INFO_PRINTF1(_L("Failed. Didn't get expected KPositionPartialUpdate error"));
					SetTestStepResult(EFail);
					}

				if(iPosUpdateStatus == EPositionUpdateStart)
					{
					iDoPosUpdatePtr->StartL(*iPosInfoPtr2);
					iPosUpdateStatus = EPositionUpdateContinue;
					}
				else
					{
					TModuleDataIn modDataIn; // Used to send test information to the test module.
					modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
					modDataIn.iTimeOut = 5*1000000;
					
					// Send timeout value to test module - will block.
					utils.NotifyModuleOfConfigChangeL(modDataIn);

					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfoPtr3;
					posInfoPtr3 = new(ELeave) TPositionInfo();
					posInfoArr.Append(posInfoPtr3);
					
					// Kick off pos update.
					iDoPosUpdatePtr->StartL(*posInfoPtr3);

					if(iExpectedApiBehaviour == EApiVariant1)
						{						
						err = iDoPosUpdatePtr->CompleteRequest(EPositionerNotifyPositionUpdate);
						if (err != KErrNone)
							{
							INFO_PRINTF1(_L("Failed. Didn't get expected KErrNone error"));
							SetTestStepResult(EFail);
							}	
						}
					iPosUpdateStatus = EPositionUpdateComplete;				
					}
				}
			else
				{
				if (KPositionEarlyComplete != err)
					{
					INFO_PRINTF1(_L("Failed. Didn't get expected KPositionEarlyComplete error"));
					SetTestStepResult(EFail);
					}
				
				iState = EWaiting;	
				}

			break;
			}
			
		default:
			break;
		}
	}	

void CT_LbsClientStep_PartialUpdate::VerifyPosInfos()
	{
	T_LbsUtils utils;
	T_LbsUtils::TComparisonAccuracyType accType;
	
	if(iParent.iSharedData->iTestModuleInUse) 
		{
		accType = T_LbsUtils::EExactAccuracy;
		}
	else
		{
		accType = T_LbsUtils::ERoughAccuracy;
		}
	
	switch(iTestCaseId)
		{
		case 5:
		case 6:
			{
			//Verify Position returned - It should be the position returned earlier
			TPositionInfo* curPosInfoPtr1 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[0]);
			TPositionInfo* curPosInfoPtr2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[1]);
			TPositionInfo* verPosInfoPtr = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[0]);

			if (!utils.Compare_PosInfo(*curPosInfoPtr1, *verPosInfoPtr, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			if (!utils.Compare_PosInfo(*curPosInfoPtr2, *verPosInfoPtr, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }
			
			break;
			}
			
		case 7:
			{
			//Verify Position returned - It should be the position returned earlier
			TPositionInfo* curPosInfoPtr1 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[0]);
			TPositionInfo* curPosInfoPtr2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[1]);

			TPositionInfo* verPosInfoPtr1 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[0]);
			TPositionInfo* verPosInfoPtr2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[1]);

			if (!utils.Compare_PosInfo(*curPosInfoPtr1, *verPosInfoPtr1, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			if (!utils.Compare_PosInfo(*curPosInfoPtr2, *verPosInfoPtr2, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			break;
			}
			
		case 21:
		case 22:
			{
			//Verify Position returned - It should be the position returned earlier
			TPositionInfo* curPosInfoPtr1 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[0]);
			TPositionInfo* curPosInfoPtr2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[1]);

			TPositionInfo refPosInfo;
			refPosInfo.SetPosition(iRefPos);

			if (!utils.Compare_PosInfo(*curPosInfoPtr1, refPosInfo, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			if (!utils.Compare_PosInfo(*curPosInfoPtr2, refPosInfo, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			break;
			}

		case 23:
			{
			//Verify Position returned - It should be the position returned earlier
			TPositionInfo* curPosInfoPtr = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[0]);

			TPositionInfo refPosInfo;
			refPosInfo.SetPosition(iRefPos);

			if (!utils.Compare_PosInfo(*curPosInfoPtr, refPosInfo, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			break;
			}
		
		case 24:
		case 25:
		case 26:
		case 28:
			{
			//Verify Position returned - It should be the position returned earlier
			TPositionInfo* curPosInfoPtr1 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[0]);
			TPositionInfo* curPosInfoPtr2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[1]);
			TPositionInfo* curPosInfoPtr3 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[2]);

			TPositionInfo* verPosInfoPtr1 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[0]);
			TPositionInfo* verPosInfoPtr2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[1]);

			TPositionInfo refPosInfo;
			refPosInfo.SetPosition(iRefPos);

			if (!utils.Compare_PosInfo(*curPosInfoPtr1, refPosInfo, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			if (!utils.Compare_PosInfo(*curPosInfoPtr2, *verPosInfoPtr1, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			//if(iTestCaseId == 26 || iTestCaseId == 25)
			//	{
			//	if (!utils.Compare_PosInfo(*curPosInfoPtr3, *verPosInfoPtr2, accType))
			//		{
			//		INFO_PRINTF1(_L("Failed test, position incorrect."));
			//		SetTestStepResult(EFail);
			//	    }
			//	}
			//else
			//	{
				if (!utils.Compare_PosInfo(*curPosInfoPtr3, *verPosInfoPtr1, accType))
					{
					INFO_PRINTF1(_L("Failed test, position incorrect."));
					SetTestStepResult(EFail);
				    }
			//	}
			
			break;
			}

		case 27:
			{
			//Verify Position returned - It should be the position returned earlier
			TPositionInfo* curPosInfoPtr1 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[0]);
			TPositionInfo* curPosInfoPtr2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[1]);
			TPositionInfo* curPosInfoPtr3 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[2]);

			TPositionInfo* verPosInfoPtr1 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[0]);
			TPositionInfo* verPosInfoPtr2 = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[1]);

			if (!utils.Compare_PosInfo(*curPosInfoPtr1, *verPosInfoPtr1, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			if (!utils.Compare_PosInfo(*curPosInfoPtr2, *verPosInfoPtr2, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }

			if (!utils.Compare_PosInfo(*curPosInfoPtr3, *verPosInfoPtr2, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    }
			
			break;
			}
		    
		default:
			break;
		}
	}


void CT_LbsClientStep_PartialUpdate::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	}

void CT_LbsClientStep_PartialUpdate::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;

	}
	
void CT_LbsClientStep_PartialUpdate::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{

	T_LbsUtils utils;
	T_LbsUtils::TComparisonAccuracyType accType;
	if(iParent.iSharedData->iTestModuleInUse) 
		{
		accType = T_LbsUtils::EExactAccuracy;
		}
	else
		{
		accType = T_LbsUtils::ERoughAccuracy;
		}
	
	switch(iTestCaseId)
		{
		case 21:
		case 22:
			{
			TPositionInfo refPosInfo;
			refPosInfo.SetPosition(iRefPos);
			if (!utils.Compare_PosInfo(refPosInfo, aPosition, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    } 
			    
			break;
			}

		case 24:
		case 25:
		case 28:
			{
			TPositionInfo* verPosInfoPtr = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[0]);

			if (!utils.Compare_PosInfo(*verPosInfoPtr, aPosition, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    } 
			    
			break;
			}

		/*case 25:
			{
			TPositionInfo* verPosInfoPtr = reinterpret_cast<TPositionInfo*>(iParent.iSharedData->iVerifyPosInfoArr[1]);

			if (!utils.Compare_PosInfo(*verPosInfoPtr, aPosition, accType))
				{
				INFO_PRINTF1(_L("Failed test, position incorrect."));
				SetTestStepResult(EFail);
			    } 
			    
			break;
			}*/
			
		case 26:
			{
			//INFO_PRINTF1(_L("Position should not be sent to Network"));
			//SetTestStepResult(EFail);
		    
		    break;
		    }

		default:
			break;
			
		}
	}

void CT_LbsClientStep_PartialUpdate::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	(void)aReason;
	(void)aPosition;
	}

void CT_LbsClientStep_PartialUpdate::NotifyReleaseLcsMoLr(TInt aReason)
	{
	(void)aReason;
	if(iTestCaseId == 23)
		{
		iState = EWaiting;
		}
	}


TInt CT_LbsClientStep_PartialUpdate::OpenNetSim()
	{
	TInt err = KErrNone;
	
	//Connect to NetSim
	err = iNetSim.ConnectL(this);
	if (err)
		{
		INFO_PRINTF1(_L("Failed. Couldn't connect to NetSim"));
		SetTestStepResult(EFail);
		return KErrGeneral;
		}
	
	// Set the reference position, which also be used for any required verification.
	iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	iRefPos.SetCurrentTime();

	if (!iNetSim.SetReferenceLocation(iRefPos))
		{
		INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
		SetTestStepResult(EFail);
		iNetSim.Close();
		return KErrGeneral;
		}	

	//Set the position quality required by the network.
	TTimeIntervalMicroSeconds maxFixTime(60 * 1000000);

	TLbsNetPosRequestQuality netPosQuality;
	netPosQuality.SetMaxFixTime(maxFixTime);
	netPosQuality.SetMinHorizontalAccuracy(iRefPos.HorizontalAccuracy());
	netPosQuality.SetMinVerticalAccuracy(iRefPos.VerticalAccuracy());
								
	if (!iNetSim.SetQuality(netPosQuality))
		{
		INFO_PRINTF1(_L("Failed test, can't set NetSim's quality."));
		SetTestStepResult(EFail);
		iNetSim.Close();
		return KErrGeneral;
		}
			
	TUid pluginUid;		
	if(iParent.iSharedData->iTestModuleInUse) 
		{
		// Set plugin to use.
		pluginUid = TUid::Uid(KTestAssistanceDataProviderPluginUidValue);	
		}
	else
		{
		// Real AGPS Uid
		pluginUid = TUid::Uid(KRealAssistanceDataProviderPluginUidValue);	
		}

	if (!iNetSim.SetAssistanceDataProvider(pluginUid))
		{
		INFO_PRINTF1(_L("Failed test, can't set NetSim's assistance data plugin uid."));
		SetTestStepResult(EFail);
		iNetSim.Close();
		return KErrGeneral;
		} 
	
	return KErrNone;
	}

void CT_LbsClientStep_PartialUpdate::CloseNetSim()
	{
	iNetSim.Close();
	}

