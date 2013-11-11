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
// @file ctlbsclockstepmain.cpp
// This is the class implementation for the Clock Tests
// EPOC includes.
// LBS includes. 
// 
//

// LBS test includes.
#include "ctlbsclockstepmain.h"

const TTimeIntervalMicroSeconds32 KVerifyTimerInterval = 250000;//0.25s
const TTimeIntervalMicroSeconds KDefaultAdjustClockTimeout = 3000000;
const TTimeIntervalMicroSeconds KModuleTimeout = 50 * 1000* 1000;
const TInt64 KMaxDiffForRoughCompare = 5000000;
const TInt64 KMaxDiffForExactCompare = 2000000;
const TInt64 KMaxDiffForTimeoutCompare = 500000;

const TVersion verifyVer(1,0,0);

_LIT(KTimeOffset, "time_offset");
_LIT(KClientTimeout, "client_timeout");

/** When adding new tests try to follow the test execute path currently taken by existing tests.

A typical test case follows this logic flow though the code:

doTestStepL() is called, where the required clock admin is set, which in turn causes the admin notification function
OnSettingUpdateEvent() to be called, from which the test starts (some tests will end here also).

Then from OnSettingUpdateEvent() a async operation is fired, which results in the RunL() being called, from here the test will
come to a end or continue with a further cycle of RunL().

During the test CheckForTestHalt() will be called to determine is the test has finished, which done as a result of
the correct test flags being set.
*/


/**
Static Constructor
*/
CT_LbsClockStep_Main* CT_LbsClockStep_Main::New(CT_LbsClockServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsClockStep_Main* testStep = new CT_LbsClockStep_Main(aParent);
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


/**
 * Constructor
 */
CT_LbsClockStep_Main::CT_LbsClockStep_Main(CT_LbsClockServer& aParent) : CT_LbsClockStep(aParent), CActive(CActive::EPriorityStandard)
	{
	SetTestStepName(KLbsClockStep_Main);
	}


void CT_LbsClockStep_Main::ConstructL()
	{
	// Create the base class objects.
	CT_LbsClockStep::ConstructL();

	// Connect to the LBS self locate server.
	User::LeaveIfError(iServer.Connect());
	CleanupClosePushL(iServer);
	
	// Open the positioner.
	User::LeaveIfError(iPositioner.Open(iServer));
	CleanupClosePushL(iPositioner);

	// set requester will be removed...
	User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
													CRequestor::EFormatApplication,
													_L("Tom Tom")));

	// Create the admin.
	iAdmin = CLbsAdmin::NewL(*this);

	// Create the verify timer.
	iVerifyTimer = CT_LbsTimerUtils::NewL(this, 0);
	
	// Create the plugin data bus monitor.
	iPluginDataBusMonitor = CT_LbsClockPluginDataBusMonitor::NewL(this);

	// Determine the A-GPS module's time to first fix.
	DetermineModuleTimeToFirstFixL();

	CActiveScheduler::Add(this);

	CleanupStack::Pop(&iPositioner);
	CleanupStack::Pop(&iServer);
	}


/**
 * Destructor
 */
CT_LbsClockStep_Main::~CT_LbsClockStep_Main()
	{
	Cancel();
	iVerifyTimer->CancelTimer();
	iPluginDataBusMonitor->CancelMonitor();
	
	delete iPluginDataBusMonitor;
	delete iVerifyTimer;
	delete iAdmin;
	iPositioner.Close();
	iServer.Close();	
	}


/** Used to mark each callback that has fired.
*/
void CT_LbsClockStep_Main::SetCallbackFlag(TLbsClockCallbackFlags aCallbackFlag)
	{
	iCallbackFlags |= aCallbackFlag;
	}


/** Determine the end of the test.

	Check the state machine to determine when to halt the test. Once all the callbacks
	have been received the test can end.
*/
void CT_LbsClockStep_Main::CheckForTestHalt()
	{
	// Check for test finish.
	
	// We stop test from here when in the correct state and, there should not be anything outstanding.
	if ((iState == EWaiting) && (iCallbackFlags == iFlagsToHaltOn))
		{
		iState = EDone;

		iPluginDataBusMonitor->CancelMonitor();

		CActiveScheduler::Stop();
		}
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClockStep_Main::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L(">> CT_LbsClockStep_Main::doTestStepL()"));
	RDebug::Printf(">> CT_LbsClockStep_Main::doTestStepL()");
	if (TestStepResult() == EPass)
		{		
		// Carry out common test actions (such as connecting to a server).

		// Ensure auto adjust is off.
		User::LeaveIfError(iAdmin->Set(KLbsSettingClockAdjust, CLbsAdmin::EClockAdjustOff));
		
		// Only interested in clock related admin changes.
		iAdmin->SetNotificationMaskL(KLbsSettingClockAdjust | KLbsSettingAllowManualClockAdjust);

		iState = EWaitingAdminNotify;

		// Netsim setup is required if the AGPS module is in the EGpsPreferTerminalBased mode.
		TBool iNetSimUsed = EFalse;
		CLbsAdmin::TGpsMode modAgpsMode;
		User::LeaveIfError(iAdmin->Get(KLbsSettingHomeGpsMode, modAgpsMode));
		if (modAgpsMode == CLbsAdmin::EGpsPreferTerminalBased)
			{
			iNetSimUsed = ETrue;

			OpenNetSim();
			}
		
		// Carry out unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				// test case LBS-SetTime-0002 - man
				case 2:
					{
					iFlagsToHaltOn =	KLbsClockCallback_Got_AdjustClock_Stage1 |
										KLbsClockCallback_Got_AdminNotification;

					User::LeaveIfError(iAdmin->Set(KLbsSettingAllowManualClockAdjust, CLbsAdmin::EClockAdjustOff));
					}
					break;


				// Test case LBS-Clock-0004 - man
				case 12:
				case 13:
					{
					iFlagsToHaltOn =	KLbsClockCallback_Got_AdjustClock_Stage1 |
										KLbsClockCallback_Got_AdjustClock_Stage2 |
										KLbsClockCallback_Got_AdminNotification;
						
					// Set time stamp offset, how much to alter the clock by.
					SetTimeOffsetL();	// This will block if test module is in use.
					
					// Switch on auto adjust.
					if (iTestCaseId == 13)
						{
						SetAutoAdjustOnL();
						}
									
					// Switch on the Clock API Kick off both the admin change
					User::LeaveIfError(iAdmin->Set(KLbsSettingAllowManualClockAdjust, CLbsAdmin::EClockAdjustOn));
					}
					break;


				// will timeout
				case 18:
				case 19:
					{
					iFlagsToHaltOn =	KLbsClockCallback_Got_AdjustClock_Stage1 |
										KLbsClockCallback_Got_AdminNotification;

					// Set time stamp offset, how much to alter the clock by.
					SetTimeOffsetL();	// This will block if test module is in use.

					// Set the module timeout to ensure the clock adjust does timeout.
					if ((iTestCaseId == 18) || (iTestCaseId == 19))
						{
						SetModuleTimeoutL(KModuleTimeout); 	// This will block.
						}
				
					// Switch on the Clock APIKick off both the admin change
					User::LeaveIfError(iAdmin->Set(KLbsSettingAllowManualClockAdjust, CLbsAdmin::EClockAdjustOn));
					}
					break;
				
				// Test case LBS-Clock-0020 - auto
				case 20:
				case 21:
				case 41:
					{
					StartAutoAdjustTesting();
					}
					break;
				
				case 121:
					{
					// Soak up the network position info generated when assistance data is requested
					// by the AGPS module.
					iState = ESoakRefPos;

					iPositioner.NotifyPositionUpdate(iRefPosInfo, iStatus);
					SetActive();
					}
					break;

				default:
					{
					User::Panic(KLbsClockStep_Main, KErrUnknown);
					}					
				}
			}
		
		// Kick off test.
		CActiveScheduler::Start();

		// Close Netsim if used by test.
		if (iNetSimUsed)
			{
			CloseNetSim();
			}
		}

	INFO_PRINTF1(_L("<< CT_LbsClockStep_Main::doTestStepL()"));

	return TestStepResult();
	}


void CT_LbsClockStep_Main::DoCancel()
	{
	}


void CT_LbsClockStep_Main::OnSettingUpdateEvent(TInt aError, const TLbsAdminSetting& aSetting)
	{

	// Only process the notifications we requested.
	if (iState == EWaitingAdminNotify)
	{
		// Only process the clock setting.
		if ((KLbsSettingClockAdjust == aSetting) || (KLbsSettingAllowManualClockAdjust == aSetting))
			{
			SetCallbackFlag(KLbsClockCallback_Got_AdminNotification);
			
			if (aError)
				{
				// Test failed.
				INFO_PRINTF2(_L("Failed test, admin change failed with error %d."), aError);
				SetTestStepResult(EFail);
				}
				
			else
				{
				// next stage of the test here - first clock sync
				switch (iTestCaseId)
					{
					case 2:
						{					
						SetActive();
						iState = EClockAdjustPending_Stage1;
						}
						break;

					case 12:
					case 13:
					case 20:
					case 21:
					case 41:
					case 121:
						{
						AdjustClock(EFalse, 0);

						iState = EClockAdjustPending_Stage1;					
						}
						break;

					case 18:
					case 19:
						{
						if (iTestCaseId == 19)
							{
							}
							
						else
							{
							// Read the client timeout value.
							SetClientTimeout();

							INFO_PRINTF2(_L("AdjustClock(%d)"), iClientTimeout.Int64());

							}
						SetActive();
						
						iAdjustClockCallTime.UniversalTime();

						iState = EClockAdjustPending_Stage1;
						}
						break;
					
					default:
						{
						
						}
					}
				}
			CheckForTestHalt();
			}
		}
	}


void CT_LbsClockStep_Main::RunL()
	{
	TBool errExpected = ETrue;	// Some tests expect an error.
	TInt err = iStatus.Int();
	TTime currentTime;
	INFO_PRINTF2(_L("CT_LbsClockStep_Main::RunL() iTestCaseId=%D"), iTestCaseId);

	// Cancel the verify timer.
	iVerifyTimer->CancelTimer();

	// may not be able to put here - for pos it will be set twice that's all
	SetCallbackFlag(KLbsClockCallback_Got_AdjustClock_Stage1);
			
			
	switch (iTestCaseId)
		{
		case 2:
			{
			if (err != KErrLocked)
				{
				// Test failed.
				INFO_PRINTF2(_L("Failed test, expected KErrLocked got error %d."), err);
				SetTestStepResult(EFail);
				}
				
			iState = EWaiting;
			}
			break;
			

		case 18:
		case 19:
			{
			// Verify the call timed out after the correct interval.
			switch (iTestCaseId)
				{
				case 18:
					{
					ValidateTimeout(iClientTimeout);
					}
					break;
				case 19:
					{
					ValidateTimeout(iGpsTimeToFirstFix);
					}
					break;
				}
			
			
			// Verify error code.
			if (err != KErrTimedOut)
				{
				// Test failed.
				INFO_PRINTF2(_L("Failed test, expected KErrTimedOut got error %d."), err);
				SetTestStepResult(EFail);
				}
			
			iState = EWaiting;
			}
			break;

		case 13:
		case 20:
		case 21:
		case 41:
		case 121:
			{
			// Special case where the ref pos is soaked up. Only required for test cases 120 + 121.
			if (iState == ESoakRefPos)
				{
				// Verify reference position.
				TPositionInfo verifyRefPosInfo;
				T_LbsUtils utils;
		
				verifyRefPosInfo.SetPosition(iRefPos);
				
				// Only verify ref position when using test module. The reason for this
				// is the real module may or may not allways ref pos for new clients.
				if (!utils.Compare_PosInfo(verifyRefPosInfo, iRefPosInfo) &&
					iParent.iSharedData->iTestModuleInUse)
					{
					INFO_PRINTF1(_L("Failed test, reference position incorrect."));
					SetTestStepResult(EFail);
					}

				// Errors not expected.
				errExpected = EFalse;

				// Start test in normal way.
				StartAutoAdjustTesting();

				break;
				}

			// Print expected time after AdjustClock.
			INFO_PRINTF1(_L("Expected time after AdjustClock: "));	
			INFO_PRINTF5(_L("at %d :%d :%d :%d\n"), iVerifyTime.DateTime().Hour(), iVerifyTime.DateTime().Minute(), iVerifyTime.DateTime().Second(), iVerifyTime.DateTime().MicroSecond());

			// Print the current time after AdjustClock.
			TTime currentTime;
			currentTime.UniversalTime();
				
			INFO_PRINTF1(_L("Actual time after AdjustClock: "));	
			INFO_PRINTF5(_L("at %d :%d :%d :%d\n\n"), currentTime.DateTime().Hour(), currentTime.DateTime().Minute(), currentTime.DateTime().Second(), currentTime.DateTime().MicroSecond());

			// Errors not expected.
			errExpected = EFalse;
			
			// System clock updated, verify time was updated correctly.
			if (iState == EClockAdjustPending_Stage1)
				{
				SetCallbackFlag(KLbsClockCallback_Got_AdjustClock_Stage1);
				CompareTimes(T_LbsUtils::ERoughAccuracy);

				// Before next adjust wait a while. To ensure the target time for the second request is always different from the first request.
				// There is a very small chance that the target times for the update request are the same, this is due to the clock being adjusted to precily
				// the same time as it was before the first request.
				User::After(3000000);

				AdjustClock(EFalse, 0);

				iState = EClockAdjustPending_Stage2;
				}
				
			else if (iState == EClockAdjustPending_Stage2)
				{
				SetCallbackFlag(KLbsClockCallback_Got_AdjustClock_Stage2);
				CompareTimes(T_LbsUtils::EExactAccuracy);

				iState = EWaiting;
				}
			}
			break;
		}
	
	// Check if the test is over.	
	CheckForTestHalt();

	// Report any un-expected errors.
	if (!errExpected && err)
		{
		User::LeaveIfError(err);
		}
	}


TInt CT_LbsClockStep_Main::RunError(TInt aError)
	{
	// Test failed.
	INFO_PRINTF2(_L("Failed test, AdjustClock completed with error %d."), aError);
	SetTestStepResult(EFail);

	return aError;
	}




void CT_LbsClockStep_Main::HandleTimerL(TInt aTimerId, const TTime& aTargetTime)
	{
	(void)aTimerId;
	(void)aTargetTime;
	
	iVerifyTime += KVerifyTimerInterval;

	iVerifyTimer->SetTimer(KVerifyTimerInterval);
	}


void CT_LbsClockStep_Main::HandleDataBusUpdateL(const TClockPluginDataOut& aClockPluginDataOut)
	{
	// Indicate that the plugin was used to carry out the clock adjust.
	if (aClockPluginDataOut.iResponse == TClockPluginDataOut::EClockPluginResponseOk)
		{
		SetCallbackFlag(KLbsClockCallback_Got_ClockPluginInUse);
		}
		
	else
		{
		INFO_PRINTF1(_L("Failed test, plugin returned bad response."));
		SetTestStepResult(EFail);
		}
	}


void CT_LbsClockStep_Main::StartAutoAdjustTesting()
	{
	iFlagsToHaltOn =	KLbsClockCallback_Got_AdjustClock_Stage1 |
						KLbsClockCallback_Got_AdjustClock_Stage2 |
						KLbsClockCallback_Got_AdminNotification;
	
	// Set time stamp offset, how much to alter the clock by.
	SetTimeOffsetL();	// This will block if test module is in use.

	// Waiting for admin update.
	iState = EWaitingAdminNotify;

	// As we invoke the automatic adjust via NotifyPositionUpdate,
	// we need somewhere to store the returned posistion infos.

	// Clean the pos info array.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;

	T_LbsUtils utils;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

	// Using a plugin.
	if (iTestCaseId == 41)
		{
		iFlagsToHaltOn |= KLbsClockCallback_Got_ClockPluginInUse;
		
		iPluginDataBusMonitor->StartMonitorL();
		}
	
	SetAutoAdjustOnL();
	}


void CT_LbsClockStep_Main::AdjustClock(TBool /*aUseTimeout*/, TTimeIntervalMicroSeconds /*aTimeoutValue*/)
	{
	// Store the current time.
	iVerifyTime.UniversalTime();
	iVerifyTimer->SetTimer(KVerifyTimerInterval);

	// Print the current time before AdjustClock.
	INFO_PRINTF1(_L("Time before AdjustClock: "));	
	INFO_PRINTF5(_L("at %d :%d :%d :%d\n"), iVerifyTime.DateTime().Hour(), iVerifyTime.DateTime().Minute(), iVerifyTime.DateTime().Second(), iVerifyTime.DateTime().MicroSecond());

	// Move clock forward by the required offset.
	TTime currentTime;
	
	currentTime.UniversalTime();
	currentTime += iTimeStampOffset;
	INFO_PRINTF5(_L("Putting UTCTime forward to %d :%d :%d :%d"), currentTime.DateTime().Hour(), currentTime.DateTime().Minute(), currentTime.DateTime().Second(), currentTime.DateTime().MicroSecond());	
	User::SetUTCTime(currentTime);
	INFO_PRINTF1(_L("After SetUTCTime"));	

	// Adjust clock to GPS time invoke automatic clock adjust.
	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* posInfo = new(ELeave) TPositionInfo();
	if (posInfo)
		{
		posInfoArr.Append(posInfo);
	
		iPositioner.NotifyPositionUpdate(*posInfo, iStatus);
		}
		
	else
		{
		// Test failed.
		INFO_PRINTF1(_L("Failed test, memory failure creating posInfo."));
		SetTestStepResult(EFail);		
		}
		
	SetActive();	
	}


TBool CT_LbsClockStep_Main::CompareTimes(T_LbsUtils::TComparisonAccuracyType aCmpAccuracy)
	{
	TTime currentTime;
	TInt64 timeDiff, maxTime;
	
	// Get difference between the current + verify times.
	currentTime.UniversalTime();	
	timeDiff = Abs(currentTime.Int64() - iVerifyTime.Int64());
	
	// Get the limit value to compare against.
	if (aCmpAccuracy == T_LbsUtils::ERoughAccuracy)
		{
		maxTime = KMaxDiffForRoughCompare; // Five secs.
		}
		
	else
		{
		maxTime = KMaxDiffForExactCompare; // Two secs.
		}
	
	if(timeDiff > maxTime)
		{
		// Test failed.
		INFO_PRINTF2(_L("Failed test, time values differ by more than %d micro seconds."), maxTime);
		SetTestStepResult(EFail);

		return EFalse;
		}
		
	return ETrue;
	}


TBool CT_LbsClockStep_Main::ValidateTimeout(const TTimeIntervalMicroSeconds aExpectedTimeout)
	{
	TTime adjustClockRunLTime;
	TTimeIntervalMicroSeconds microSecs;
	TInt64 timeElapsed, timeDiff;
	
	adjustClockRunLTime.UniversalTime();
	INFO_PRINTF5(_L("Universal Time %d :%d :%d :%d\n"), adjustClockRunLTime.DateTime().Hour(), adjustClockRunLTime.DateTime().Minute(), adjustClockRunLTime.DateTime().Second(), adjustClockRunLTime.DateTime().MicroSecond());	
	INFO_PRINTF5(_L("Time when Adjust Clock was called %d :%d :%d :%d\n"), iAdjustClockCallTime.DateTime().Hour(), iAdjustClockCallTime.DateTime().Minute(), iAdjustClockCallTime.DateTime().Second(), iAdjustClockCallTime.DateTime().MicroSecond());
	microSecs = adjustClockRunLTime.MicroSecondsFrom(iAdjustClockCallTime);
	timeElapsed = microSecs.Int64();
	
	INFO_PRINTF2(_L("Microseconds Elapsed since AdjustClock called = %d"), timeElapsed);
	
	// Ensure values are close...
	timeDiff = Abs(timeElapsed - aExpectedTimeout.Int64());
	INFO_PRINTF2(_L("TimeDiff = %d microseconds"), timeDiff);
	if (timeDiff > KMaxDiffForTimeoutCompare)
		{
		// Test failed.
		INFO_PRINTF2(_L("Failed test, timeout values differ by more than %d micro seconds."), KMaxDiffForTimeoutCompare);
		SetTestStepResult(EFail);

		return EFalse;
		}
		
	return ETrue;
	}


void CT_LbsClockStep_Main::SetTimeOffsetL()
	{
	// Read how much to alter the clock by.
	TInt value = 0;
	GetIntFromConfig(ConfigSection(), KTimeOffset, value);

	TInt64 offsetNum = (TInt64)1000000 * (TInt64)value;
	TTimeIntervalMicroSeconds offsetValue(offsetNum);

	iTimeStampOffset = offsetValue;
	
	// If the test module is in use, transfer the time offset to it.
	if (iParent.iSharedData->iTestModuleInUse)
		{
		T_LbsUtils utils;
		TModuleDataIn modDataIn;

		modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeStampOffset;
		modDataIn.iTimeStampOffset = iTimeStampOffset;
	
		utils.NotifyModuleOfConfigChangeL(modDataIn);	// This will block.
		}
	}

	
void CT_LbsClockStep_Main::SetClientTimeout()
	{
	TInt value = 0;
	GetIntFromConfig(ConfigSection(), KClientTimeout, value);

	TInt64 timeoutNum = (TInt64)1000000 * (TInt64)value;
	TTimeIntervalMicroSeconds timeoutValue(timeoutNum);

	iClientTimeout = timeoutValue;
	}


void CT_LbsClockStep_Main::SetModuleTimeoutL(TTimeIntervalMicroSeconds aTimeout)
	{
	// If the test module is in use, transfer the timeout to it.
	if (iParent.iSharedData->iTestModuleInUse)
		{
		// Inform the module of the timeout.
		T_LbsUtils utils;
		TModuleDataIn modDataIn;

		modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
		modDataIn.iTimeOut = iGpsTimeToFirstFix.Int64() + aTimeout.Int64();
	
		utils.NotifyModuleOfConfigChangeL(modDataIn);	// This will block.
		}
		
	// Can't set a timeout for the real module.
	else
		{
		User::LeaveIfError(KErrNotSupported);
		}
	}


void CT_LbsClockStep_Main::SetAutoAdjustOnL()
	{
	// Switch on the automatic clock adjust.
	TUint adjustInterval = 1;	//100;//100000; // 10th of a sec.

	User::LeaveIfError(iAdmin->Set(KLbsSettingClockAdjustInterval, adjustInterval));
	User::LeaveIfError(iAdmin->Set(KLbsSettingClockAdjustThreshold, adjustInterval));
	User::LeaveIfError(iAdmin->Set(KLbsSettingClockAdjust, CLbsAdmin::EClockAdjustOn));
	}


void CT_LbsClockStep_Main::DetermineModuleTimeToFirstFixL()
	{
	T_LbsUtils utils;

	// Determine the max fix value of the A-GPS module.
	TPositionModuleId gpsModuleId;
	TPositionModuleInfo gpsModuleInfo;
	TPositionQuality gpsQuality;
	
	// Get module id first.
	gpsModuleId = utils.GetAGpsModuleIdL(iServer);
	
	// Get max fix from actual module info, now we have the correct id.			
	User::LeaveIfError(iServer.GetModuleInfoById(gpsModuleId, gpsModuleInfo));
	
	gpsModuleInfo.GetPositionQuality(gpsQuality);
	iGpsTimeToFirstFix = gpsQuality.TimeToFirstFix();
	}
