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
// @file ctlbsx3psteptransmit.cpp
// This is the class implementation for the Module Information Tests
// EPOC includes.
// 
//

// LBS includes.
#include <lbs.h>
#include <lbs/lbsx3p.h>
#include <lbs/test/lbsnetsimtest.h>

#include <lbs/lbsadmin.h>
#include <lbs/lbslocerrors.h>

#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbsx3psteptransmit.h"

// Required for LBS-X3P-Transmit-0007
#include  "ctlbsstepsetautonomousmode.h"


const TInt KTestAssistanceDataProviderPluginUidValue = 0x10281D77;
const TInt KRealAssistanceDataProviderPluginUidValue = 0x10285AC2;

const TUint KLbsGpsLocManagerUidValue		= 0x10281D44;
const TUid  KLbsGpsLocManagerUid			= {KLbsGpsLocManagerUidValue};

#define REFPOS_LAT				52.2
#define REFPOS_LONG				0.2
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	100
#define REFPOS_VERT_ACCURACY	100

#define NETWORK_MAX_FIX_TIME	5000000
#define NETWORK_HORZ_ACCURACY	10
#define NETWORK_VERT_ACCURACY	10


//profile_id in ini file
_LIT(KLbsProfileId, "profile_id");

//max_time in ini file
_LIT(KLbsMaxTime, "max_time");

/**
 * Destructor
 */
CT_LbsX3PStep_Transmit::~CT_LbsX3PStep_Transmit()
	{
	iServer.Close();
	iPositionTransmitter.Close();
	}


/**
 * Constructor
 */
CT_LbsX3PStep_Transmit::CT_LbsX3PStep_Transmit(CT_LbsX3PServer& aParent) : CT_LbsX3PStep(aParent)
	{
	SetTestStepName(KLbsX3PStep_Transmit);
	}


/**
Static Constructor
*/
CT_LbsX3PStep_Transmit* CT_LbsX3PStep_Transmit::New(CT_LbsX3PServer& aParent)
	{
	CT_LbsX3PStep_Transmit* testStep = new CT_LbsX3PStep_Transmit(aParent);
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

void CT_LbsX3PStep_Transmit::ConstructL()
	{
	CT_LbsX3PStep::ConstructL();
	
	//We need to wait while the log events are added
	SetExtendedTimerInterval(1000000);

	// Carry out common test actions (such as connecting to a server).

	// Connect to the Transmit Position server.
	User::LeaveIfError(iServer.Connect());

	// Open the positioner.
	User::LeaveIfError(iPositionTransmitter.Open(iServer));

	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsX3PStep_Transmit::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep_Transmit::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		//Code common to all switch statements

		// Create the active object to carry out the transmit location.
		iDoTransmitPos = CT_LbsX3PDoTransmitPos::NewL(this, iPositionTransmitter);
		CleanupStack::PushL(iDoTransmitPos);

		// Location to use.
		//RPointerArray<TAny>& srcPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		//TPositionInfo* srcPosInfo = reinterpret_cast<TPositionInfo*>(srcPosInfoArr[0]);
		//TPosition srcPos;
		//srcPosInfo->GetPosition(srcPos);



	    iRefPosFlag = GetIntFromConfig(ConfigSection(), KLbsRefPos, iRefPosFlag);

		iFlagsToHaltOn =	KLbsCallback_MoLr |
							KLbsCallback_Got_NotifyDoTransmitPos |
							(iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
							KLbsCallback_NetSim_Got_Connect; // Test case callback events.

		TModuleDataIn modDataIn; // Used to send test information to the test module.
		//Reset test module timeout
		//if(iParent.iSharedData->iTestModuleInUse)
		//	{
		//	modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
		//	modDataIn.iTimeOut = 0;
		//
		//	iUtils.NotifyModuleOfConfigChangeL(modDataIn);
		//	}

		CLbsAdmin* adminApi = CLbsAdmin::NewL();
		CleanupStack::PushL(adminApi);

		// Kick off the keep alive timer.
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod);
		iKeepAliveTimer->SetTimer(interval);

		// Carry out unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				// Test case LBS-X3P-Options-0001
				case 01:
				// Test case LBS-X3P-Options-0001	Request Transmit to third party with timeout
				//
				// External step:	Inform module of required single position to be transmitted.
				// Local step:		Do X3P.
				// Local callbacks:	Verify callback parameters are as expected.
				//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
				//					TransmitPosition callback, aStatus should be KErrTimedOut

					{

					// We now expect extra callback of NotifyMeasurementReportControlFailure instead of
					// KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation due to timeout.
					iFlagsToHaltOn = 	KLbsCallback_MoLr |
										KLbsCallback_NetSim_Got_Connect |
										KLbsCallback_Got_NotifyDoTransmitPos |
										(iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
										KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure;
										// We expet failure callback due to tmieout!

					iFlagsToHaltOn &=~ 	KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation;
					iFlagsToHaltOn &=~ 	KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult;

					//1.

					//2
					//Transmit Location
					TLbsTransmitPositionOptions transmitOptions;
					transmitOptions.SetTimeOut(5*1000000);	//Set X3P Timeout to 5 sec
					iPositionTransmitter.SetTransmitOptions(transmitOptions);

					User::After(1000);

					TLbsTransmitPositionOptions retrievedTransmitOptions;
					iPositionTransmitter.GetTransmitOptions(retrievedTransmitOptions);

					if(retrievedTransmitOptions.TimeOut() != transmitOptions.TimeOut())
						{
						INFO_PRINTF1(_L("<FONT><B>Set and Get results didnt match</B></FONT>"));
						SetTestStepResult(EFail);
						}

					// The module request type - time out value.
					modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
					modDataIn.iTimeOut = 10*1000000;

					// Send timeout value to test module - will block.
					iUtils.NotifyModuleOfConfigChangeL(modDataIn);

					}
					break;

				case 03:
					{
					//Timeout for Transmit Location
					TLbsTransmitPositionOptions transmitOptions;

					TTimeIntervalMicroSeconds timeout = transmitOptions.TimeOut();
					if(timeout.Int64() != 0)
						{
						INFO_PRINTF1(_L("<FONT><B>Default timeout should be zero</B></FONT>"));
						SetTestStepResult(EFail);
						}

					//iFlagsToHaltOn = 	KLbsCallback_NetSim_Got_Connect ;

					CleanupStack::PopAndDestroy(adminApi);
					iDoTransmitPos->Cancel();
					CleanupStack::PopAndDestroy(iDoTransmitPos);
					return TestStepResult();
					}

				case 04:
					{
					//We might have to put this test in LBS-X3P-TransmitOptions
					//Timeout for Transmit Location
					TLbsTransmitPositionOptions transmitOptions(-10);

					//The code should panic before and not come to this point
					INFO_PRINTF1(_L("<FONT><B>Negative timout should not be allowed</B></FONT>"));
					SetTestStepResult(EFail);

					//return TestStepResult();
					}

				case 05:
					{
					if(iParent.iSharedData->iTestModuleInUse)
						{
						//Configure gps-test-module to respond after the specified timeout.
						modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
						modDataIn.iTimeOut = 140*1000000;

						// Send timeout value to test module - will block.
						iUtils.NotifyModuleOfConfigChangeL(modDataIn);
						}

					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect |
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									 KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr|
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
									 KLbsCallback_Got_NotifyDoTransmitPos;

					}
					break;

				// Test case LBS-X3P-Transmit-0001
				// This test uses the default functionality in each of the callbacks.
				case 11:
					{
					// Kick off basic test.
					// Test case LBS-X3P-Transmit-0001	Transmit current position with default options (no SetTransmitOptions() call
					//									and quality profile left as default) to a valid destination.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					}
					break;

				// Test case LBS-X3P-Transmit-0003
				// This test switches to specific functionality in each of the callbacks.
				case 13:
					{
					// Test case LBS-X3P-Transmit-0003	Request transmit current position from multiple sessions.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do multiple X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					// 					NotifyMeasurementReportLocation callback, each aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					//	1. Create single client (RlbsTransnitPositionServer object)
					//	2. Open multiple sub-sessions (RlbsTransnitPosition objects)

					CT_LbsX3PDoTransmitPos* pDoTransPos;
					//RLbsTransmitPosition transPos1;
					//RLbsTransmitPosition transPos2;
					//RLbsTransmitPosition transPos3;
					iTransmitPos1.Open(iServer);
					iTransmitPos2.Open(iServer);
					iTransmitPos3.Open(iServer);

						//	3. Request X3P on each sub-session (can have same priorities)
					// Create the active object array to carry out the transmit location.

					pDoTransPos = CT_LbsX3PDoTransmitPos::NewL(this, iTransmitPos1);
					iDoTransmitPosArr.AppendL(pDoTransPos);

					pDoTransPos = CT_LbsX3PDoTransmitPos::NewL(this, iTransmitPos2);
					iDoTransmitPosArr.AppendL(pDoTransPos);

					pDoTransPos = CT_LbsX3PDoTransmitPos::NewL(this, iTransmitPos3);
					iDoTransmitPosArr.AppendL(pDoTransPos);

					}
					break;

				// Test case LBS-X3P-Transmit-0004
				case 14:

					// Test case LBS-X3P-Transmit-0004	Transmit current position with specified quality profile Id to a valid destination.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:      Set admin KLbsSettingQualityProfileTransmitLocate and profile Id 4((which is exist in lbsprofile.ini)
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					// Test case LBS-X3P-Transmit-Profile-0033
				case 133:
				// Test case LBS-X3P-Transmit-Profile-0033	Request Transmit to third party with timeout
				//
				// External step:	Inform module of required single position to be transmitted.
				// Local step:      Set admin KLbsSettingQualityProfileTransmitLocate and profile Id 0((which is exist in lbsprofile.ini)
				// Local step:		Do X3P.
				// Local callbacks:	Verify callback parameters are as expected.
				//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
				//					TransmitPosition callback, aStatus should be KErrTimedOut

					{

					if(iTestCaseId==133)
						{
						// We now expect extra callback of NotifyMeasurementReportControlFailure instead of
						// KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation due to timeout.
						iFlagsToHaltOn = 	KLbsCallback_MoLr |
											KLbsCallback_NetSim_Got_Connect |
											KLbsCallback_Got_NotifyDoTransmitPos |
											(iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
											KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure;
											// We expet failure callback due to tmieout!

						iFlagsToHaltOn &=~ 	KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation;
						iFlagsToHaltOn &=~ 	KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult;


						// The module request type - time out value.
					    modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
					    modDataIn.iTimeOut = 10*1000000;

					    // Send timeout value to test module - will block.
					    iUtils.NotifyModuleOfConfigChangeL(modDataIn);
						}

					TInt profileid_ini;
					TBool profileid_exists;
			        profileid_exists = GetIntFromConfig(ConfigSection(), KLbsProfileId, profileid_ini);
		            if(!profileid_exists)
						{
						INFO_PRINTF1(_L("Could not get profileid from section"));
						SetTestStepResult(EFail);
						}

					TLbsQualityProfileId profileId;
					// Set Profile id
					User::LeaveIfError(adminApi->Set(KLbsSettingQualityProfileTransmitLocate, TLbsQualityProfileId(profileid_ini)));
					//wait some time to propagate the setting
					User::After(5*1000000);

					// Verify that the setting is what we expect
					User::LeaveIfError(adminApi->Get(KLbsSettingQualityProfileTransmitLocate, profileId));
					if(profileId != TLbsQualityProfileId(profileid_ini))
						{
						INFO_PRINTF1(_L("<FONT><B>Set Profile id failed</B></FONT>"));
						SetTestStepResult(EFail);
						}
					 }
					break;

				// Test case LBS-X3P-Transmit-0006
				case 16:
					{

					// Test case LBS-X3P-Transmit-0006	Request X3P when gps unavailable
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should not be KErrNone

					iFlagsToHaltOn = 	KLbsCallback_NetSim_Got_Connect |
										KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
										KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure |
										KLbsCallback_Got_NotifyDoTransmitPos |
										(iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
										KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr;

					//	1. Configure test a-gps module to respond with error (gps unavailable)
					// The module request type - error value.
					modDataIn.iRequestType = TModuleDataIn::EModuleRequestError;
					modDataIn.iError = KErrNotFound;

					// Send error value to test module - will block.
					iUtils.NotifyModuleOfConfigChangeL(modDataIn);

					}
					break;

				// Test case LBS-X3P-Transmit-0007
				case 17:
					{

					// Test case LBS-X3P-Transmit-0007	Transmit to 3rd party with 'autonomous-only' mode set
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					}
					break;

				// Test case LBS-X3P-Transmit-0008
				case 18:
					{

					// Test case LBS-X3P-Transmit-0008	Request X3P when X3P turned off in admin settings.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					Callback should not be invoked
					//					TransmitPosition callback, aStatus should be KErrAccessDenied

					//	1. ETransmitLocateOff for KSettingHomeTransmitLocate

					// X3P is turned off so no netsim callbacks should be recieved.
					iFlagsToHaltOn = 	KLbsCallback_NetSim_Got_Connect |
										(iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
										KLbsCallback_Got_NotifyDoTransmitPos;

					adminApi->Set(KLbsSettingHomeTransmitLocate, CLbsAdmin::ETransmitLocateOff);
					User::After(10*1000000);
					}
					break;

				// Test case LBS-X3P-Transmit-0009
				case 19:
					{

					// Test case LBS-X3P-Transmit-0009	Request X3P when self-locate turned off in admin settings.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					//	1. ESelfLocateOff for KSettingHomeTransmitLocate
					adminApi->Set(KLbsSettingHomeTransmitLocate, CLbsAdmin::ESelfLocateOff);
					User::After(5*1000000);
					}
					break;

				// Test case LBS-X3P-Transmit-0010
				case 110:
					{

					// Test case LBS-X3P-Transmit-0010	Request X3P when ExternalLocateOff set for KSettingHomeTransmitLocate in admin settings.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					//	1. EExternalLocateOff for KSettingHomeTransmitLocate
					adminApi->Set(KLbsSettingHomeTransmitLocate, CLbsAdmin::EExternalLocateOff);
					User::After(5*1000000);
					}
					break;

				// Test case LBS-X3P-Transmit-0011
				case 111:
					{

					// Test case LBS-X3P-Transmit-0011	Request X3P when X3P turned off in admin settings. Roaming.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					iFlagsToHaltOn = 	KLbsCallback_NetSim_Got_Connect |
										(iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
										KLbsCallback_Got_NotifyDoTransmitPos;

					#pragma message("TO DO: LBS-X3P-Transmit-0011 CANNOT TEST THIS IN ROAMING MODE YET. INCOMPLETE")

					//	1. EExternalLocateOff for KSettingRoamingTransmitLocate
					adminApi->Set(KLbsSettingRoamingTransmitLocate, CLbsAdmin::ETransmitLocateOff);
					User::After(5*1000000);
					}
					break;

				// Test case LBS-X3P-Transmit-0012
				case 112:
					{

					// Test case LBS-X3P-Transmit-0012	Request X3P when ESelfLocateOff set for KLbsSettingRoamingTransmitLocate in admin settings.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					//	1. ESelfLocateOff for KLbsSettingRoamingTransmitLocate
					adminApi->Set(KLbsSettingRoamingTransmitLocate, CLbsAdmin::ESelfLocateOff);
					User::After(5*1000000);
					}
					break;

				// Test case LBS-X3P-Transmit-0013
				case 113:
					{

					// Test case LBS-X3P-Transmit-0013	Request X3P when EExternalLocateOff set for KLbsSettingRoamingTransmitLocate in admin settings.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					//	1. EExternalLocateOff for KLbsSettingRoamingTransmitLocate
					adminApi->Set(KLbsSettingRoamingTransmitLocate, CLbsAdmin::EExternalLocateOff);
					User::After(5*1000000);
					}
					break;

				// Test case LBS-X3P-Transmit-0014
				case 114:
					{
					if(iParent.iSharedData->iTestModuleInUse)
						{
						//Configure gps-test-module to respond after the specified timeout.
						modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
						modDataIn.iTimeOut = 10*1000000;

						// Send timeout value to test module - will block.
						iUtils.NotifyModuleOfConfigChangeL(modDataIn);
						}

					//Set timeout to 0 - This should disable Timeout
					TLbsTransmitPositionOptions transmitOptions;
					transmitOptions.SetTimeOut(0);
					iPositionTransmitter.SetTransmitOptions(transmitOptions);

					}
					break;

				// Test case LBS-X3P-Transmit-0015
				case 115:
					{
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect |
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
									 KLbsCallback_Got_NotifyDoTransmitPos;

					}
					break;

				// Test case LBS-X3P-Transmit-0016
				case 116:
					{
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect |
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
									 KLbsCallback_Got_NotifyDoTransmitPos;

					}
					break;

				// Test case LBS-X3P-Transmit-0017
				case 117:
					{
					if(iParent.iSharedData->iTestModuleInUse)
						{
						//Configure gps-test-module to request for more assitance data.
						T_LbsUtils utils;
						TModuleDataIn modDataIn; // Used to send test information to the test module.
						modDataIn.iRequestType = TModuleDataIn::EModuleRequestTestMode;
						modDataIn.iAssDataEventType = TModuleDataIn::EAssDataEventExpectSingle;
						modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_SomeDataNotAvailable;

						utils.NotifyModuleOfConfigChangeL(modDataIn);
						}

					iFlagsToHaltOn |= 	KLbsCallback_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData;

					}
					break;

				case 118:
					break;


				// Test case LBS-X3P-Transmit-0030
				case 130:
					{
					// Test case LBS-X3P-Transmit-0030	Kick off X3P test with bad phone number/email.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P but pass in an invalid number/email.
					// Local callbacks:	Verify callback parameters are as expected.
					//					TransmitPosition callback, aStatus should be KErrNotFound
					iFlagsToHaltOn = 	KLbsCallback_NetSim_Got_Connect |
										(iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0) |
										KLbsCallback_Got_NotifyDoTransmitPos |
										KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr;

					}
					break;

				// Test case LBS-X3P-Timeout-0001
				case 134:
					{
					// Kick off basic test.
					// Test case LBS-X3P-Timeout-0001	Transmit current position with default options (no SetTransmitOptions() call
					//									and quality profile left as default) to a valid destination.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aStatus should be KPositionQualityLoss
					//					as the location provided didn't meet the quality requirements
 					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect |
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr |
									 KLbsCallback_Got_NotifyDoTransmitPos |
									 KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation |
									 KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult;
					}
					break;


				default:
					{
					User::Panic(KLbsX3PStep_Transmit, KErrUnknown);
					}
				}
			}

		//Connect to NetSim
		iNetSim.ConnectL(this);

		// Set the reference position, which also be used for any required verification.
		iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
		iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
		iRefPos.SetCurrentTime();

		if (!iNetSim.SetReferenceLocation(iRefPos))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return TestStepResult();
			}

		// Test case LBS-X3P-Transmit-0004 or Test case LBS-X3P-Transmit-0005
		if(iTestCaseId==14 || iTestCaseId==15)
			{
			// Set all parameters invalid (0 for max fix time; NaN for
			// accuracy) then network should use quality profile with the
			// ID set
			TTimeIntervalMicroSeconds maxFixTime(0);
			iNetPosQuality.SetMaxFixTime(maxFixTime);
    		TRealX nan;
    		nan.SetNaN();
			iNetPosQuality.SetMinHorizontalAccuracy(nan);
	    	iNetPosQuality.SetMinVerticalAccuracy(nan);
			}
		// Test case LBS-X3P-Timeout-0001
		else if (iTestCaseId==134)
			{
			TTimeIntervalMicroSeconds maxFixTime(5000000);
	    	iNetPosQuality.SetMaxFixTime(maxFixTime);
			iNetPosQuality.SetMinHorizontalAccuracy(1);
	    	iNetPosQuality.SetMinVerticalAccuracy(1);
			}
		else
			{
			//Set the position quality required by the network.
			TTimeIntervalMicroSeconds maxFixTime(120 * 1000000);//default was 60
																	//max acceptable age of a position
	    	iNetPosQuality.SetMaxFixTime(maxFixTime);
			iNetPosQuality.SetMinHorizontalAccuracy(NETWORK_HORZ_ACCURACY);
	    	iNetPosQuality.SetMinVerticalAccuracy(NETWORK_VERT_ACCURACY);
			}

		if (!iNetSim.SetQuality(iNetPosQuality))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's quality."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return TestStepResult();
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
			return TestStepResult();
			}

		if(iTestCaseId !=133)
			{
			//Set X3P Timeout
			TLbsTransmitPositionOptions transmitOptions;			//This is how long we wait for an X3P
			TTimeIntervalMicroSeconds KLbsTransmitPositionTimeOut;
			if(iTestCaseId == 01)
				KLbsTransmitPositionTimeOut = 5*1000000;  //5 sec
			else if(iTestCaseId==05)
				KLbsTransmitPositionTimeOut = 180*1000000;  //180 sec - Let maxFixtime expire first
			else if (iTestCaseId==134)
				KLbsTransmitPositionTimeOut = 10*1000000;  //10 sec - Let maxFixtime expire first
			else
				KLbsTransmitPositionTimeOut = 20*1000000;  //20 sec

			transmitOptions.SetTimeOut(KLbsTransmitPositionTimeOut);
			iPositionTransmitter.SetTransmitOptions(transmitOptions);
			}

		
		//Kickoff test
		CActiveScheduler::Start();
		
		switch(iTestCaseId)
			{
			case 01:
			case 05:
			case 133:
				{
				// The module request type - time out value.
				modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
				modDataIn.iTimeOut = 0;

				// Reset test module timeout
				iUtils.NotifyModuleOfConfigChangeL(modDataIn);

				}
				break;

			case 13:
				{
				iDoTransmitPosArr.ResetAndDestroy();

				//Close temporary transmitPosition objects
				iTransmitPos1.Close();
				iTransmitPos2.Close();
				iTransmitPos3.Close();
				}
				break;


			case 16:
				{
				//reset the error code back
				// The module request type - error value.
				modDataIn.iRequestType = TModuleDataIn::EModuleRequestError;
				modDataIn.iError = KErrNone;

				// Send error value to test module - will block.
				iUtils.NotifyModuleOfConfigChangeL(modDataIn);
				}
				break;

			default:
				break;
			}

		iNetSim.ClearAssistanceDataFilters();
		iNetSim.Close();

		CleanupStack::PopAndDestroy(adminApi);
		iDoTransmitPos->Cancel();
		CleanupStack::PopAndDestroy(iDoTransmitPos);

		}


	INFO_PRINTF1(_L("&lt;&lt;CT_LbsX3PStep_Transmit::doTestStepL()"));

	return TestStepResult();
	}



/** NetSim callbacks given for a X3P, which we invoke as a result of the notify position update.
*/
void CT_LbsX3PStep_Transmit::Connected()
	{

	CT_LbsNetSimStep::Connected();

	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* posInfo = NULL;


	iUtils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

	//If X3PRefLocation flag is on, we have to test reference location functionality
	//TBool bRefPos;
    //bRefPos = GetIntFromConfig(ConfigSection(), KLbsRefPos, bRefPos);

	switch(iTestCaseId)
		{
		case 3:
		case 4:
			break;

		case 13:
			{
			for(TInt counter = 0 ; counter < KMaxCount ; counter++)
				{
				posInfo = new TPositionInfo();
				if (posInfo != NULL)
					{
					posInfoArr.Append(posInfo);
					CT_LbsX3PDoTransmitPos* localDoTransmitPos;
					localDoTransmitPos = (reinterpret_cast<CT_LbsX3PDoTransmitPos*>(iDoTransmitPosArr[counter]));
					if(iRefPosFlag)
						localDoTransmitPos->TransmitPositionReportingRefLoc(KDefaultDestination, KTransmitPriority, *posInfo);
					else
						localDoTransmitPos->TransmitPosition(KDefaultDestination, KTransmitPriority, *posInfo);

					}
				}
			}
			break;

		case 130:
			{
			// Kick off transmit position.
	 	    posInfo = new(ELeave) TPositionInfo();
			CleanupStack::PushL(posInfo);
			posInfoArr.AppendL(posInfo);
			CleanupStack::Pop(posInfo);
			if(iRefPosFlag)
				iDoTransmitPos->TransmitPositionReportingRefLoc(KInvalidDestination, KTransmitPriority, *posInfo);
			else
				iDoTransmitPos->TransmitPosition(KInvalidDestination, KTransmitPriority, *posInfo);

			iState = EWaiting;
			}
			break;
		default:
			{
			posInfo = new TPositionInfo();
			CleanupStack::PushL(posInfo);
			posInfoArr.AppendL(posInfo);
			CleanupStack::Pop(posInfo);

			if(iTestCaseId ==133)
				{
				// Position requested at
				iTimeRequested.UniversalTime();
				}

			if(iRefPosFlag)
				{
				iDoTransmitPos->TransmitPositionReportingRefLoc(KDefaultDestination, KTransmitPriority, *posInfo);
				}
			else
				{
				iDoTransmitPos->TransmitPosition(KDefaultDestination, KTransmitPriority, *posInfo);
				}


			iState = EWaiting;
			}
			break;

		}
	}

void CT_LbsX3PStep_Transmit::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	//First NetSim callback (FROM: Gateway)

	CT_LbsNetSimStep::NotifyRegisterLcsMoLr(aData);

	// find out what aData is, it may be an idea to verify it's correct
	// Verify correct destination telephone number:
	//		if its an X3P aData will contain "something",
	//		if its an MOLR aData should contain "nothing".

	switch(iTestCaseId)
		{
		// X3P should be turned off for these test cases
		case 111:
		case 18:
			{
			INFO_PRINTF1(_L("<FONT><B>NotifyRegisterLcsMoLr should not be invoked. Test Failed. </B></FONT>"));
			SetTestStepResult(EFail);
			}
			break;

		case 130:
			{
			if(aData.Compare(KInvalidDestination) != 0)
				{
				INFO_PRINTF1(_L("<FOND><B>For case 130 the phone number should have been invalid.</B></FONT>"));
				SetTestStepResult(EFail);
				}
			}
			break;

		case 115:
			{
			if(!iNetSim.SetResponseError(RLbsNetSimTest::KNetSimNetworkNotAvailable, ETrue))
				{
				INFO_PRINTF1(_L("CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL() Failed: Can't set NetSim's response error."));
				SetTestStepResult(EFail);
				}
			}
			break;

		default:
			{
			if(aData.Compare(KDefaultDestination) != 0)
				{
				INFO_PRINTF1(_L("<FOND><B>For an X3P, aData should contain something.</B></FONT>"));
				SetTestStepResult(EFail);
				}
			}
		}
	}



void CT_LbsX3PStep_Transmit::NotifyMeasurementControlLocation(	const TPositionInfo& aPosition,
										const RLbsAssistanceDataBuilderSet& aData,
										const TLbsNetPosRequestQuality& aQuality)
	{
	//Second callback

	// Call base functionality
	CT_LbsNetSimStep::NotifyMeasurementControlLocation(aPosition, aData, aQuality);
	/*
	aPosition is the reference position set at the top in netsim.setrefposition().
	compare aposition and this to verify its correct.
	*/


	//RPointerArray<TAny>& srcPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	//TPositionInfo* srcPosInfo = reinterpret_cast<TPositionInfo*>(srcPosInfoArr[0]);

	//T_LbsUtils::TComparisonAccuracyType whatAccuracyToUse;

	//if(iParent.iSharedData->iTestModuleInUse)
	//	{
	//	whatAccuracyToUse = T_LbsUtils::EExactAccuracy;
	//	}
	//else
	//	{
	//	whatAccuracyToUse = T_LbsUtils::ERoughAccuracy;
	//	}
	//TPositionInfo  refPosInfo;
	//refPosInfo.SetPosition(iRefPos);
	//if(!iUtils.Compare_PosInfo(refPosInfo, aPosition, whatAccuracyToUse))

	TPositionInfo refPosInfo;
	T_LbsUtils::TComparisonAccuracyType accType;

	refPosInfo.SetPosition(iRefPos);

	if(iParent.iSharedData->iTestModuleInUse)
		accType = T_LbsUtils::EExactAccuracy;
	else
		accType = T_LbsUtils::ERoughAccuracy;

	if(!iUtils.Compare_PosInfo(refPosInfo, aPosition, accType))
		{
		INFO_PRINTF1(_L("<FONT><B>Failed test, NotifyMeasurementControlLocation: srcPosInfo differs from aPositionInfo</B></FONT>"));
		SetTestStepResult(EFail);
		}

	if(iParent.iSharedData->iTestModuleInUse)
		{

		// Verify the assistance data for test mode.
		RLbsAssistanceDataBuilderSet& data = const_cast<RLbsAssistanceDataBuilderSet&>(aData);
		RUEPositioningGpsReferenceTimeBuilder* refTimeBuilder = NULL;

		data.GetDataBuilder(refTimeBuilder);

		// Create a reader from the builder's data to allow us to verify the actual
		// assistance data.
		TInt err;

		RUEPositioningGpsReferenceTimeReader refTimeReader;

		TRAP(err, refTimeReader.OpenL());
		if (err == KErrNone)
			{
			refTimeReader.DataBuffer() = refTimeBuilder->DataBuffer();

			if (!iUtils.VerifySimpleAssistanceData(refTimeReader))
				{
				INFO_PRINTF1(_L("Failed test, assistance data incorrect."));
				SetTestStepResult(EFail);
				}
			refTimeReader.Close();
			}

		else
			{
			INFO_PRINTF2(_L("Failed test, assistance data reader err %d."), err);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		//Real data, dont know what to expect so cant check it for cnonsistency.
		}


	// Need to check quality. Should be same for real and test.
	// Can't check for NaN by a direct check
	TBool maxFixTimeOK = aQuality.MaxFixTime() == iNetPosQuality.MaxFixTime();
	TBool minHAccuracyOK = (aQuality.MinHorizontalAccuracy() == iNetPosQuality.MinHorizontalAccuracy()) ||
				(Math::IsNaN(aQuality.MinHorizontalAccuracy()) && Math::IsNaN(iNetPosQuality.MinHorizontalAccuracy()));
	TBool minVAccuracyOK = (aQuality.MinVerticalAccuracy() == iNetPosQuality.MinVerticalAccuracy()) ||
				(Math::IsNaN(aQuality.MinVerticalAccuracy()) && Math::IsNaN(iNetPosQuality.MinVerticalAccuracy()));

	if(	!maxFixTimeOK || !minHAccuracyOK || !minVAccuracyOK)
		{
		INFO_PRINTF1(_L("NotifyMeasurementControlLocation: Quality should have matched."));
		SetTestStepResult(EFail);
		}

	if(iTestCaseId == 116)
		{
		if(!iNetSim.SetResponseError(RLbsNetSimTest::KNetSimNetworkNotAvailable, ETrue))
			{
			INFO_PRINTF1(_L("CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL() Failed: Can't set NetSim's response error."));
			SetTestStepResult(EFail);
			}
		}
	}


// NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr
void CT_LbsX3PStep_Transmit::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{

	//Third NetSim callback (FROM: Gateway)
	CT_LbsNetSimStep::NotifyMeasurementReportLocation(aPosition);

	T_LbsUtils::TComparisonAccuracyType whatAccuracyToUse;

	if(iParent.iSharedData->iTestModuleInUse)
		{
		whatAccuracyToUse = T_LbsUtils::EExactAccuracy;
		}
		else
		{
		whatAccuracyToUse = T_LbsUtils::ERoughAccuracy;
		}

	// get the ref position from the iVerifyPosInfoArr (the last time) then verify with aPosition
	// the array would have been added to at the call to TransmitPosition above in net connect callback

	// Common stuff
	// Create a posinfo and store in our shared array for later verification.

	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iVerifyPosInfoArr;


	switch (iTestCaseId)
		{
		case 13:
			{
			for(TInt counter = 0 ; counter < KMaxCount ; counter++)
				{
				if(!iUtils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(posInfoArr[counter])), aPosition, whatAccuracyToUse))
					{
					INFO_PRINTF1(_L("<FONT><B>aPosition didn't match any of the contents of posInfoArr.</B></FONT>"));
					SetTestStepResult(EFail);
					}
				}
			}
		break;

		// X3P should be turned off for these test cases
		case 111:
		case 18:
			{
			INFO_PRINTF1(_L("<FONT><B>NotifyMeasurementReportLocation should not be invoked. Test Failed. </B></FONT>"));
			SetTestStepResult(EFail);
			}
		break;

		default:
			{
			if(!iUtils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(posInfoArr[0])), aPosition, whatAccuracyToUse))
				{
				INFO_PRINTF1(_L("<FONT><B>aPosition didn't match contents of posInfoArr.</B></FONT>"));
				SetTestStepResult(EFail);
				}
			}
		break;
		}

	}

void CT_LbsX3PStep_Transmit::NotifyReleaseLcsMoLr(TInt aReason)
	{

	// Fifth NetSim callback (FROM: Gateway)
	iReleaseCallBackCount++;

	if(iTestCaseId != 13)
		{
		//INFO_PRINTF1(_L("Got - Notify Release Lcs MoLr Callback Event"));
		CT_LbsNetSimStep::NotifyReleaseLcsMoLr(aReason);
		}

	switch (iTestCaseId)
		{
		case 01:
		case 05:
		case 133:
			{
			if( aReason != KErrTimedOut)
				{
				INFO_PRINTF3(_L("<FONT><B>Case %d iDoTransmitPos should have timed out. Error recieved: %d. Test Failed. </B></FONT>"),iTestCaseId, aReason);
				SetTestStepResult(EFail);
				}
			}
		break;

		case 16:
		{
		if(aReason != KErrNone)
			{
			INFO_PRINTF2(_L("<FONT><B>NotifyReleaseLcsMoLr had an unexpected aReason of: %d</B></FONT>"), aReason);
			SetTestStepResult(EFail);
			}
		}
		break;



		case 13:
			{
			INFO_PRINTF2(_L("Got - Notify Release Lcs MoLr Callback Event %d times"),iReleaseCallBackCount);
			if( iReleaseCallBackCount == (iDoTransmitPosArr.Count() * 2))
				{
				INFO_PRINTF1(_L("		Setting Notify Release Lcs MoLr Callback Event bit"));
				CT_LbsNetSimStep::NotifyReleaseLcsMoLr(aReason);
				}

			if(KErrNone != aReason)
				{
				INFO_PRINTF2(_L("<FONT><B>NotifyReleaseLcsMoLr had an unexpected aReason of: %d</B></FONT>"), aReason);
				SetTestStepResult(EFail);
				}

			}
		default:
		if(KErrNone != aReason)
			{
			INFO_PRINTF2(_L("<FONT><B>NotifyReleaseLcsMoLr had an unexpected aReason of: %d</B></FONT>"), aReason);
			SetTestStepResult(EFail);
			}

		}
	}

void CT_LbsX3PStep_Transmit::MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo)
	{
	//(void)aStatus;
	//(void)aRefPosInfo;

	INFO_PRINTF1(_L("Got - Transmit Position Reference Postion - Callback Event"));
	SetCallbackFlag(KLbsCallback_Got_NotifyDoTransmitReferencePos);

	switch(iTestCaseId)
		{
		case 18:
		case 130:
		case 15:
		case 115:
			break;

		case 13:
			{
			if(aStatus != KErrNone)
				{
				INFO_PRINTF1(_L("<FONT><B>TransmitPosition Reference Position doesnt complete with KErrNone </B></FONT>"));
				SetTestStepResult(EFail);
				}
			}
			break;

		case 116:
			{
			if(aStatus != KErrDisconnected && aStatus != KErrNone)
    			{
   				// NOTE: Due to a race condition in test framework that can occur on H4, it is a possibility 
   				// that we will not see the KErrDisconnected, but instead KErrNone. Therefore we cant fail the test for this.
   				INFO_PRINTF2(_L("<FONT><B> should have returned KErrDisconnected or KErrNone, recevied %d instead</B></FONT>"),aStatus.Int());

				SetTestStepResult(EFail);
				}
				
			}
			break;

		default:
			{
			TPositionInfo refPosInfo;
			T_LbsUtils::TComparisonAccuracyType accType;

			refPosInfo.SetPosition(iRefPos);

			if(iParent.iSharedData->iTestModuleInUse)
				accType = T_LbsUtils::EExactAccuracy;
			else
				accType = T_LbsUtils::ERoughAccuracy;

			if(!iUtils.Compare_PosInfo(refPosInfo, aRefPosInfo, accType))
				{
				INFO_PRINTF1(_L("<FONT><B>Failed test, X3PRefPos callback: Reference Position differs from aPositionInfo</B></FONT>"));
				SetTestStepResult(EFail);
				}

			if(aStatus != KErrNone ||  (iCallbackFlags&KLbsCallback_Got_NotifyDoTransmitPos) !=0)
				{
				INFO_PRINTF1(_L("<FONT><B>TransmitPosition completed before Reference Position </B></FONT>"));
				SetTestStepResult(EFail);
				}

			//Verify TPositionClassTypeBase parameters
			if(aRefPosInfo.ModuleId() != KLbsGpsLocManagerUid ||
			   aRefPosInfo.PositionMode() != TPositionModuleInfo::ETechnologyNetwork ||
			   aRefPosInfo.PositionModeReason() != EPositionModeReasonNone ||
			   aRefPosInfo.UpdateType() != EPositionUpdateGeneral )
				{
				INFO_PRINTF1(_L("<FONT><B>Reference Position values mismatch </B></FONT>"));
				SetTestStepResult(EFail);
				}

			}
			break;
		}
	}


/**	Transmit position callback.

	The notify position update as completed. We can mark as done in the callback flags.

	---Implementing inherited method
*/

void CT_LbsX3PStep_Transmit::MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos* aDoTransmitPosAO)
	{
	(void)aDoTransmitPosAO;

	INFO_PRINTF1(_L("Got - Transmit Position - Callback Event."));
	SetCallbackFlag(KLbsCallback_Got_NotifyDoTransmitPos);

	if(iTestCaseId==133)
	{
	// Position received at
	iTimeReceived.UniversalTime();
	}
	TInt counter = 0;
	switch (iTestCaseId)
		{
		case 01:
		case 05:
		case 133:
			{
			if( iDoTransmitPos ->iStatus != KErrTimedOut)
				{
				INFO_PRINTF2(_L("<FONT><B>Case %d iDoTransmitPos should have timed out. Test Failed. </B></FONT>"),iTestCaseId);
				SetTestStepResult(EFail);
				}

			if(aStatus != KErrTimedOut)
				{
				INFO_PRINTF3(_L("<FONT><B>iTestCaseId: %d, TransmitPositionCallBack aStatus: %d</B></FONT>"),iTestCaseId, aStatus.Int());
				}

			if(iTestCaseId==133)
				{
				TInt  maxtime_ini;
			    TBool maxtime_exists;


			    maxtime_exists = GetIntFromConfig(ConfigSection(), KLbsMaxTime, maxtime_ini);
		        if(!maxtime_exists)
					{
					INFO_PRINTF1(_L("Could not get maxtime from ini file"));
				    SetTestStepResult(EFail);
				    }
		    	// we are stampping the received time after callback ,so we are adding 1s for actual maxtime.
			    TInt64 realtimediff= maxtime_ini + 2000000;
				//Timedifference between position actually received and position actually requested
				TInt64 actualtimediff;
		        actualtimediff = iTimeReceived.Int64() - iTimeRequested.Int64();
		        INFO_PRINTF2(_L("timediff = %ld"), actualtimediff);
		        //compare actualtimediff with realtimediff
				if(actualtimediff > realtimediff)
					{
					INFO_PRINTF2(_L("timediff greater than maxfixtime %ld in profile"), realtimediff);
					SetTestStepResult(EFail);
					}
				}
			}
			break;

		case 13:
			{
			// Each completed status needs to equal KErrNone.
			// Not KErrPending

				iReleaseCallBackCount++;
				if( iReleaseCallBackCount == (iDoTransmitPosArr.Count() * 2))
				{
				INFO_PRINTF1(_L("		Setting Notify Release Lcs MoLr Callback Event bit"));
				CT_LbsNetSimStep::NotifyReleaseLcsMoLr(KErrNone);
				}

				for(counter = 0 ; counter < iDoTransmitPosArr.Count() ; counter++)
					{
					CT_LbsX3PDoTransmitPos* transmitter = reinterpret_cast<CT_LbsX3PDoTransmitPos*>(iDoTransmitPosArr[counter]);
					TInt err = transmitter->iStatus.Int();
					switch (err)
						{
						case KRequestPending:
						INFO_PRINTF2(_L("iDoTransmitPos in position %d of the array is pending."),counter);
						break;

						case KErrNone:
						INFO_PRINTF2(_L("iDoTransmitPos in position %d of the array completed successfully."),counter);
						break;

						case KErrTimedOut:
						INFO_PRINTF2(_L("iDoTransmitPos in position %d of the array Timed Out."),counter);
						break;

						default:
						INFO_PRINTF3(_L("iDoTransmitPos in position %d of the array does not appear to have completed successfully, error: %d ."),counter,err);
						break;
						}



					// After TransmitPosition gets called for the last time...
					if( counter == iDoTransmitPosArr.Count() )
						{
						// Check that all requests completed successfully...
						for(counter = 0 ; counter < iDoTransmitPosArr.Count() ; counter++)
							{
							CT_LbsX3PDoTransmitPos* transmitter = reinterpret_cast<CT_LbsX3PDoTransmitPos*>(iDoTransmitPosArr[counter]);
							TInt err = transmitter->iStatus.Int();
							if(err != KErrNone)
								{
								INFO_PRINTF3(_L("<FONT><B>iDoTransmitPos in position %d of the array never completed successfully, error: %d .</B></FONT>"),counter,err);
								SetTestStepResult(EFail);
								}
							}

						}
					}

			if(aStatus != KErrNone)
				{
				INFO_PRINTF3(_L("iTestCaseId: %d, This instance of TransmitPositionCallBack has aStatus: %d"),iTestCaseId, aStatus.Int());
				}
			}
			break;

		// Test case LBS-X3P-Transmit-0005
		case 15:
			{
			if(aStatus != KErrAccessDenied)
				{
				INFO_PRINTF3(_L("<FONT><B>iTestCaseId: %d, TransmitPositionCallBack aStatus: %d, not KErrAccessDenied</B></FONT>"),iTestCaseId, aStatus.Int());
				SetTestStepResult(EFail);
				}
			}
			break;

		case 16:
			{
			if( iDoTransmitPos ->iStatus == KErrNone)
				{
				INFO_PRINTF1(_L("<FONT><B>iDoTransmitPos completed successfully. Test AGPS should be unavailable. Test Failed. </B></FONT>"));
				SetTestStepResult(EFail);
				}

			if(aStatus != KErrNotFound)
				{
				INFO_PRINTF3(_L("<FONT><B>iTestCaseId: %d, aStatus was not KErrNotfound, TransmitPositionCallBack aStatus was: %d</B></FONT>"),iTestCaseId, aStatus.Int());
				}
			}
			break;

		// X3P should be turned off for these test cases
		case 111:
		case 18:
			{
			if(	iDoTransmitPos->iStatus != KErrAccessDenied ||
				iDoTransmitPos->iStatus == KErrNone)
				{
				INFO_PRINTF1(_L("<FONT><B>Transmit X3P should have failed. Test Failed.</B></FONT>"));
				SetTestStepResult(EFail);
				}


			if(	aStatus != KErrAccessDenied ||
				aStatus == KErrNone)
				{
				INFO_PRINTF3(_L("<FONT><B>iTestCaseId: %d, TransmitPositionCallBack aStatus: %d</B></FONT>"),iTestCaseId, aStatus.Int());
				}

			}
			break;

		case 130:
			{
			if( iDoTransmitPos ->iStatus != KErrArgument)
				{
				INFO_PRINTF1(_L("<FONT><B>iDoTransmitPos did not complete with KErrArgument. Test Failed. </B></FONT>"));
				SetTestStepResult(EFail);
				}


			if(aStatus != KErrArgument)
				{
				INFO_PRINTF3(_L("<FONT><B>iTestCaseId: %d, TransmitPositionCallBack aStatus: %d, not KErrArgument.</B></FONT>"),iTestCaseId, aStatus.Int());
				SetTestStepResult(EFail);
				}
			}
			break;

		case 115:
		case 116:
			{
			if(aStatus == KErrNone)  //We should set proper errocode - TBD
				{
				INFO_PRINTF1(_L("<FONT><B> should have returned KErrTimedOut</B></FONT>"));
				SetTestStepResult(EFail);
				}
			}
			break;

		case 118:
			{
			if(!(iCallbackFlags & KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation))
				{
				INFO_PRINTF1(_L("<FONT><B> TransmitPostion completed before </B></FONT>"));
				SetTestStepResult(EFail);
				}
			}
			break;

		case 134: //LBS-X3P-Timeout-0001
			{
			if (aStatus != KPositionQualityLoss)
				SetTestStepResult(EFail);
			}
			break;

		default:
			{

			if(aStatus != KErrNone)
				{
				INFO_PRINTF3(_L("<FONT><B>iTestCaseId: %d, TransmitPositionCallBack aStatus: %d, not KErrNone.</B></FONT>"),iTestCaseId, aStatus.Int());
				SetTestStepResult(EFail);
				}

			}
			break;
		}
	}


void CT_LbsX3PStep_Transmit::NotifyMeasurementReportControlFailure(TInt aReason)
	{

	// To stop test timing out on unexpected reciept of this callback we wont set the bit
	//switch (iTestCaseId)
	//	{
	//	case 01:
	//	case 05:
	//	case 15:
	//	case 16:
	//		CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);
	//	break;
	//
	//	default:
	//		{
	//		INFO_PRINTF2(_L("Got - Net Sim Notify Measurement Report Control Failure - Callback Event. Reason = %d"), aReason);
	//		INFO_PRINTF1(_L("		Net Sim Notify Measurement Report Control Failure - Unexpected so Bit not set"));
	//		}
	//	break;
	//	}

	switch (iTestCaseId)
		{
		case 01:
		case 133:
			{
			if( aReason != KErrCancel)
				{
				INFO_PRINTF2(_L("<FONT><B>NotifyMeasurementReportControlFailure should have failed with KErrCancel. </B></FONT>"),iTestCaseId);
				SetTestStepResult(EFail);
				}
			CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);
			}
			break;

		case 05:
			{
			if( aReason != KErrTimedOut)
				{
				INFO_PRINTF2(_L("<FONT><B>NotifyMeasurementReportControlFailure should have failed with KErrTimedOut. </B></FONT>"),iTestCaseId);
				SetTestStepResult(EFail);
				}
			CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);
			}
			break;

		// Test case LBS-X3P-Transmit-0005
		case 15:
			{
			if(aReason != KErrAccessDenied)
				{
				INFO_PRINTF2(_L("<FONT><B>NotifyMeasurementReportControlFailure should have failed with KErrAccessDenied, Error recieved is %d</B></FONT>"),aReason);
				SetTestStepResult(EFail);
				}
			CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);
			}
			break;
		case 16:
			{
			if(aReason != KErrNotFound)
				{
				INFO_PRINTF2(_L("<FONT><B>NotifyMeasurementReportControlFailure should have failed with KErrNotFound, Error recieved is %d</B></FONT>"),aReason);
				SetTestStepResult(EFail);
				}
			CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);
			}
			break;

		default:
			if(iParent.iSharedData->iTestModuleInUse)
				{
				INFO_PRINTF2(_L("Failed test, got un-expected NotifyMeasurementReportControlFailure with Reason: %d."), aReason);
				SetTestStepResult(EFail);
				}

			//if( KErrNone != aReason )
			//	{
			//	INFO_PRINTF2(_L("<FONT><B>NotifyMeasurementReportControlFailure had an unexpected aReason of: %d</B></FONT>"), aReason);
			//	SetTestStepResult(EFail);
			//	}
		}
	}


void CT_LbsX3PStep_Transmit::ProcessMeasurementControlLocationError(TInt aError)
	{
	CT_LbsNetSimStep::ProcessMeasurementControlLocationError(aError);

	switch (iTestCaseId)
	{
		case 130:
			{
			if(iDoTransmitPos->iStatus != KErrArgument)
				{
				//!@SYMTestExpectedResults	Request fails with KErrNotFound?
				INFO_PRINTF1(_L("<FONT><B>iDoTransmitPos completed successfully. Should have recieved KErrNotFound. Test Failed. </B></FONT>"));
				SetTestStepResult(EFail);
				}


			if(aError != KErrArgument)
				{
				INFO_PRINTF3(_L("<FONT><B>iTestCaseId: %d, ProcessMeasurementControlLocationError aError: %d</B></FONT>"),iTestCaseId, aError);
				}
			}
		break;


		default:
			{
			INFO_PRINTF2(_L("<FONT><B>ProcessMeasurementControlLocationError shouldnt have been called in this test. Error code: %d</B></FONT>"),aError);
			SetTestStepResult(EFail);
			}
		break;
	}

	}



void CT_LbsX3PStep_Transmit::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	// CT_LbsNetSimStep::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	(void)aFilter;

	switch (iTestCaseId)
		{
		case 117:
			// Call base implementation.
			CT_LbsNetSimStep::NotifyMeasurementReportRequestMoreAssistanceData(aFilter);
			break;

		default:
			//Note: In the case of real GPS, this message could come at anytime.
			if(iParent.iSharedData->iTestModuleInUse)
				{
				INFO_PRINTF2(_L("Failed test, got un-expected request for more assistance data with filter value = %d."), aFilter);
				SetTestStepResult(EFail);
				}
			break;
		}
	//INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report Request More Assistance Data - Callback Event."));
	//INFO_PRINTF1(_L("      Not setting Net Sim Notify Measurement Report Request More Assistance Data callback flag"));
	//SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
	}
