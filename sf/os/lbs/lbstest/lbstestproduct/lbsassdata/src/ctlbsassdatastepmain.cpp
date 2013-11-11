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
// @file ctlbsassdatastepmain.cpp
// This is the class implementation for the Module Information Tests
// EPOC includes.
// 
//

// LBS includes. 
#include <lbs.h>
#include <lbs/lbsnetcommon.h>
#include <lbssatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbsassdatastepmain.h"
#include <lbs/test/tlbsutils.h>


const TInt KAssistanceDataProviderPluginUidValue = 0x10281D77;


/**
Static Constructor
*/
CT_LbsAssDataStep_Main* CT_LbsAssDataStep_Main::New(CT_LbsAssDataServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsAssDataStep_Main* testStep = new CT_LbsAssDataStep_Main(aParent);
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
CT_LbsAssDataStep_Main::CT_LbsAssDataStep_Main(CT_LbsAssDataServer& aParent) : CT_LbsAssDataStep(aParent)
	{
	SetTestStepName(KLbsAssDataStep_Main);
	}

void CT_LbsAssDataStep_Main::ConstructL()
	{
	// Create the base class objects.
	CT_LbsAssDataStep::ConstructL();
	
	// and the active object wrapper for the notify position update.
	iDoPosUpdate = CT_LbsDoPosUpdate::NewL(this);

	// Create and start the module data bus monitor, to listen for incoming data.	
	iModuleDataBusMonitor = CT_LbsAssDataModuleDataBusMonitor::NewL(this);

	// Create the privacy controller for the MT-LR.
	iPrivacyController = CLbsPrivacyController::NewL(*this);
	}

/**
 * Destructor
 */
CT_LbsAssDataStep_Main::~CT_LbsAssDataStep_Main()
	{
	iDoPosUpdate->Cancel();
	delete iDoPosUpdate;
	
	iModuleDataBusMonitor->Cancel();
	delete iModuleDataBusMonitor;

	iPrivacyController->CancelNetworkLocationRequest(0);
	delete iPrivacyController;
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsAssDataStep_Main::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsAssDataStep_Main::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
		// Carry out common test actions (such as connecting to a server).
		T_LbsUtils utils;
		TModuleDataIn modDataIn; // Used to send test information to the test module.

		// Request type is module test mode and most tests will only expect
		// a single assistance data event.
		modDataIn.iRequestType = TModuleDataIn::EModuleRequestTestMode;
		modDataIn.iAssDataEventType = TModuleDataIn::EAssDataEventExpectSingle;
												
		// Expected callbacks flags, most test steps require MO-LR.
		iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect | 		// Net Sim callback events.
							KLbsCallback_MoLr |
							KLbsCallback_Got_NotifyUpdate |			// Test case callback events.
							KLbsCallback_Got_ModuleAssDataAnswer;	// Test module returned an answer.
		
		// Carry out unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				// Test case LBS-Ass-Data-0001
				case 1:
					{
					// Test case LBS-Ass-Data-0001 - A-GPS module creation and destruction.

					// TODO: Require a method of shuting down Lbs.
					}
					break;

				
				// Test case LBS-Ass-Data-0010 - Verify a single batch of GPS assistance data.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required assistance data test mode.
				//					Setup net sim assistance data.
				//					Do MO-LR.
				// Module:			Request assistance data.
				//					Read and verify assistance data and inform test step, ensure correct number of events.
				// Local callback:	Verify assistance data ok response.
				// External step:	Verify the position info data.
				case 10:
					{
					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn;
					}
					break;


				// Test case LBS-Ass-Data-0011 - Verify multiple batches of GPS assistance data.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required multiple assistance data events test mode.
				//					Setup net sim assistance data.
				//					Do MO-LR.
				// Module:			Request assistance data.
				//					Read and verify assistance data, ensure correct number of events.
				// Local callback:	Verify assistance data ok response.
				// External step:	Verify the position info data.
				
				// TODO: May not be possible to pass assist data which causes multi events, also test 42 does some similar.
				case 11:


				// Test case LBS-Ass-Data-0012 - Cancel assistance data request test.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required assistance data test mode.
				//					Setup net sim assistance data.
				//					Do MO-LR
				// Module:			Start assistance data request.
				//					Cancel assistance data request - invoked via register callback.
				// Local callback:	Ensure the KErrCancel is returned for ReleaseLcsMoLr callback.
				//					Verify cancel request was recived by the module.
				//					Verify assistance data cancelled ok response.
				// External step:	Verify the position info data.
				case 12:
					{
					// Expected callbacks flags, for a assitance data cancel during a MO-LR.
					//
					// Note: Currently we get a module answer because the assistance data event
					// function is still called within the module - this may not happen if the
					// timing of the cancel is different on other environments.
					iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect | 		// Net Sim callback events.
										
										KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
										KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr |
										
										KLbsCallback_Got_NotifyUpdate |			// Test case callback events.
										KLbsCallback_Got_ModuleAssDataAnswer;	// Test module returned an answer.					

					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage1;
					}
					break;


				// Test case LBS-Ass-Data-0020 - Verify unsolicited GPS assistance data.
				//
				// External step:	Create position for MT-LR.
				// Local step:		Inform module of required single ass data event, and unsolicited test.
				// 					Setup net sim assistance data.
				//					Do MT-LR
				// Module:			Read and verify ass data, ensure correct number of events.
				// Local callback:	Verify ass data ok response from module.
				case 20:
					{
					// Expected callbacks flags for this test step.
					iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect | 				// Net Sim callback events.
										KLbsCallback_MtLr |
										KLbsCallback_Got_ModuleAssDataAnswer;			// Test module returned an answer.

					// Setup emergency locate service and pause to ensure value is set.
					CLbsAdmin*	admin = CLbsAdmin::NewL();
					User::LeaveIfError(admin->Set(KLbsSettingHomeEmergencyLocate, CLbsAdmin::EExternalLocateOn));
					User::After(10000000);

					delete admin;
					
					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_Unsolicited;
					}
					break;


				// Test case LBS-Ass-Data-0030 - Verify the GPS assistance data time stamp.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required assistance data test mode.
				// 					Setup net sim assistance data.
				//					Do MO-LR.
				// Module:			Read and verify assistance data, ensure correct number of events,
				//					and verify timestamp.
				// Local callback:	Verify assistance data ok response.
				// External step:	Verify the position info data.
				case 30:
					{
					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_TimeStamp;
					}
				break;


				// Test case LBS-Ass-Data-0040 - Empty GPS assistance data cache test.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required assistance data test mode.
				// 					Setup net sim assistance data.
				// 					Do MO-LR.
				// Module:			Attempt to read assistance data before receiving assistance data
				//					event, and verify KErrNotFound is returned, and inform test step.
				// Local callback:	Verify assistance data test ok response.
				// External step:	Verify the position info data.
				case 40:
					{
					// Expected callbacks flags, not asking for assitance data so not expecting any MO-LR callbacks.
					iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect | 		// Net Sim callback events.
																				
										KLbsCallback_Got_NotifyUpdate |			// Test case callback events.
										KLbsCallback_Got_ModuleAssDataAnswer;	// Test module returned an answer.					

					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_DataNotAvailable;
					}
					break;


				// Test case LBS-Ass-Data-0041 - Request GPS assistance data when no network available.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required assistance data test mode.
				// 					Setup net sim assistance data.
				//					Do MO-LR.
				// Module:			Request assistance data, ensure KErrNotReady is return when the 
				//					data event is fired.
				// Local callback:	Verify assistance data ok response.
				// External step:	Verify the position info data.
				case 41:
					{
					// Expected callbacks flags, expect the assistance request to fail so not expecting any MO-LR callbacks.
					iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect | 		// Net Sim callback events.

										KLbsCallback_Got_NotifyUpdate |			// Test case callback events.
										KLbsCallback_Got_ModuleAssDataAnswer;	// Test module returned an answer.					

					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_NoNetwork;
					}
					break;


				// Test case LBS-Ass-Data-0042 - Request multiple items of GPS assistance data when not all of the itmes are available.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required assistance data test mode.
				// 					Setup net sim assistance data, to return 1 item of data.
				//					Do MO-LR.
				// Module:			Request 2 items of assistance data, ensure item 1 is deliveried, item 2 returns an error
				//					of KErrNotFound.
				// Local callback:	Verify assistance data ok response, for both data items.
				// External step:	Verify the position info data.
				case 42:
					{
					// Add the expected MeasurementReportRequestMoreAssistanceData flag.
					iFlagsToHaltOn |= KLbsCallback_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData;


					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_SomeDataNotAvailable;
					}
					break;


				// Test case LBS-Ass-Data-0050 - Request assistance data, upper boundary test.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required assistance data test mode.
				// 					Setup net sim assistance data.
				//					Do MO-LR.
				// Module:			Request non-exsisting type of assistance data, ensure the data event is fired with 
				//					a error of KErrArgument.
				// Local callback:	Verify assistance data ok response.
				// External step:	Verify the position info data.
				case 50:
					{
					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_InvalidDataRequest;
					}
					break;

				// Test case LBS-Ass-Data-0051 - Get assistance data item, upper boundary test.
				//
				// External step:	Create verify position for MO-LR.
				// Local step:		Inform module of required assistance data test mode.
				// 					Setup net sim assistance data.
				//					Do MO-LR.
				// Module:			Request assistance data.
				//					From the event callback attempt to get assistance data using a invalid mask,
				//					ensure the error of KErrNotFound is returned.
				// Local callback:	Verify assistance data ok response.
				// External step:	Verify the position info data.
				case 51:
					{
					// Set the test mode of the test module.
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_InvalidDataGet;
					}
					break;

								
				default:
					{
					User::Panic(KLbsAssDataStep_Main, KErrUnknown);
					}					
				}
			}
		
		
		// Carry out the test.

		// Inform the test module of the required test mode - will block.
		utils.NotifyModuleOfConfigChangeL(modDataIn);
			
		// Connect to net sim.
		iNetSim.ConnectL(this);

	    // Setup net sim's assistance data.
   
		// some tests doesn't need ref loc
		switch (iTestCaseId)
			{
		    // Test case LBS-Ass-Data-0012 - Cancel assistance data request test.
		    case 12:
			// Test case LBS-Ass-Data-0040 - Empty GPS assistance data cache test.
			case 40:
			// Test case LBS-Ass-Data-0041 - Request GPS assistance data when no network available.
			case 41:	
				{
				 // Do nothing.
				}
				break;
		    default:
			    {
			    // Create a refposinfo and store in our shared array for later verification.
			    RPointerArray<TAny>& refposInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
			    TPositionInfo* refposinfo =   reinterpret_cast<TPositionInfo*>(refposInfoArr[1]);
			    TPosition pos;
			  	refposinfo->GetPosition(pos);
					
				if (!iNetSim.SetReferenceLocation(pos))
					{
					INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
					SetTestStepResult(EFail);

					iNetSim.Close();

					return TestStepResult();
					}	
			
				// Set the position quality required by the network.
				TTimeIntervalMicroSeconds maxFixTime(55000000);
				TLbsNetPosRequestQuality netPosQuality;

				netPosQuality.SetMaxFixTime(maxFixTime);
				netPosQuality.SetMinHorizontalAccuracy(6.6);
				netPosQuality.SetMinVerticalAccuracy(7.7);
				
				if (!iNetSim.SetQuality(netPosQuality))
					{
					INFO_PRINTF1(_L("Failed test, can't set NetSim's quality."));
					SetTestStepResult(EFail);

					iNetSim.Close();

					return TestStepResult();
				    }
	         
			   }
			}
		// Set plugin to use.
		TUid pluginUid = TUid::Uid(KAssistanceDataProviderPluginUidValue);
		if (!iNetSim.SetAssistanceDataProvider(pluginUid))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's assistance data plugin uid."));
			SetTestStepResult(EFail);
			
			iNetSim.Close();

			return TestStepResult();
			}

		// Start monitoring the module data bus, to retrieve assistance data verification
		// from the test module.
		iModuleDataBusMonitor->StartMonitorL();
	
	
		// Kick off the keep alive timer.
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod);

		iKeepAliveTimer->SetTimer(interval);

		// Kick off test.
		CActiveScheduler::Start();


		// Clean up.
		iNetSim.ClearAssistanceDataFilters();
		iNetSim.Close();
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsAssDataStep_Main::doTestStepL()"));

	return TestStepResult();
	}

/** NetSim callbacks given for a MoLr, which we invoke as a result of the notify position update.
*/
void CT_LbsAssDataStep_Main::Connected()
	{
	// Call base implementation.
	CT_LbsNetSimStep::Connected();
	
	switch (iTestCaseId)
		{
		// Test case LBS-Ass-Data-0020 - Verify unsolicited GPS assistance data.
		case 20:
			{
			// Start a MT-LR request.
			_LIT8(KTestRequesterId, "LbsAssData test requester id");
			_LIT8(KTestClientName, "LbsAssData test client name");
			_LIT8(KTestClientExternalId, "LbsAssData test client external id");

			TLbsExternalRequestInfo reqInfo; 	
			reqInfo.SetRequesterId(KTestRequesterId);	
			reqInfo.SetClientName(KTestClientName);
			reqInfo.SetClientExternalId(KTestClientExternalId);			
		
			TLbsNetPosRequestPrivacy reqType;
			reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
			reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);

			iNetSim.SetEmergenyStatus(ETrue);
						
			// kick off the privacy request:
			iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);

			iState = EStartMTLRPrivReq;
			}		
			break;

		// Test case LBS-Ass-Data-0041 - Request GPS assistance data when no network available.
		case 41:
		
			if (!iNetSim.SetResponseError(RLbsNetSimTest::KNetSimNetworkNotAvailable, ETrue))
				{
				INFO_PRINTF1(_L("Failed test, can't set NetSim's response code."));
				SetTestStepResult(EFail);
				}
				
		// All other test cases required a MO-LR.
		default:
			{
			// Set net sim response code to KErrNone.
			if (iTestCaseId != 41)
				{
				if (!iNetSim.SetResponseError(KErrNone, ETrue))
					{
					INFO_PRINTF1(_L("Failed test, can't set NetSim's response code."));
					SetTestStepResult(EFail);
					}
				}
			
			// Create a posinfo and store in our shared array for later verification.
			RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
			TPositionInfo* posInfo = new(ELeave) TPositionInfo();

			T_LbsUtils utils;
			utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

			posInfoArr.Append(posInfo);

			// Kick off pos update - this will invoke the assistance data processing
			// within the test module. The result of this will be reported back to
			// the test step via the module data bus monitor.
			iDoPosUpdate->StartL(*posInfo);
					
			// Move to the first pos update state after kick off first pos update
			iState = EFirstPosUpdate;
			}
		}
	}

void CT_LbsAssDataStep_Main::Disconnected()
	{
	// Call base implementation.
	CT_LbsNetSimStep::Disconnected();
	}


void CT_LbsAssDataStep_Main::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyRegisterLcsMoLr(aData);

	// Verify the aData is blank to indicate this is a MOLR.
	if (aData != KNullDesC)
		{
		INFO_PRINTF1(_L("Failed test, register contains data."));
		SetTestStepResult(EFail);	
		}

	switch (iTestCaseId)
		{
		// Test case LBS-Ass-Data-0012 - Cancel assistance data request test.
		case 12:
			{
			TModuleDataIn modDataIn; // Used to send test information to the test module.

			// Request type is module test mode and most tests will only expect
			// a single assistance data event.
			modDataIn.iRequestType = TModuleDataIn::EModuleRequestTestMode;

			// Inform the module it should perform the second part of the cancel test.
			modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage2;

			// Inform the test module of the required test mode - will block.		
			T_LbsUtils utils;
				
			utils.NotifyModuleOfConfigChangeL(modDataIn);
			}
		}
	}


void CT_LbsAssDataStep_Main::NotifyReleaseLcsMoLr(TInt aReason)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyReleaseLcsMoLr(aReason);

	switch (iTestCaseId)
		{
		// Test case LBS-Ass-Data-0012 - Cancel assistance data request test.
		case 12:
			{
			if (aReason != KErrCancel)
				{
				INFO_PRINTF2(_L("Failed test, incorrect reason code expecting KErrCancel, got %d."), aReason);
				
				SetTestStepResult(EFail);
				}
			}
			break;
			
		default:
			{
			if (aReason != KErrNone)
				{	
				INFO_PRINTF2(_L("Failed test, bad release reason %d."), aReason);
				SetTestStepResult(EFail);
				}
			}
		}
	}


void CT_LbsAssDataStep_Main::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	T_LbsUtils utils;	
	TInt err;
	
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementControlLocation(aPosition, aData, aQuality);

	// Verify the reference position - use the entry in the verify pos info array.
	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1]);
	if (!utils.Compare_PosInfo(* verifyPosInfo, aPosition))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		}
   
    // Verify the assistance data.	
	RLbsAssistanceDataBuilderSet& data = const_cast<RLbsAssistanceDataBuilderSet&>(aData);
	RUEPositioningGpsReferenceTimeBuilder* refTimeBuilder = NULL;
	
	data.GetDataBuilder(refTimeBuilder);

	// Create a reader from the builder's data to allow us to verify the actual
	// assistance data.
	RUEPositioningGpsReferenceTimeReader refTimeReader;
		
	TRAP(err, refTimeReader.OpenL());
	if (err == KErrNone)
		{
		refTimeReader.DataBuffer() = refTimeBuilder->DataBuffer();
		
		if (!utils.VerifySimpleAssistanceData(refTimeReader))
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
		
	   

// TODO: Check if we can verify aQuality in any way.
	(void)aQuality;
	}


void CT_LbsAssDataStep_Main::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyReleaseLcsLocationNotification(aResult);


	// We should get this callback if requesting a MT-LR.
	ASSERT(iState == EStartMTLRPrivReq);				
					
	// Should allways except privacy requests.
	if (CLbsNetworkProtocolBase::EPrivacyResponseAccepted == aResult)
		{
		// Now invoke MT-LR location request.
		iNetSim.StartNetworkLocationRequest();
		
		iState = EStartMTLRLocReq;
		}

	// Un-expected response.
	else
		{
		INFO_PRINTF1(_L("Failed test, should have excepted MT-LR privacy request."));
		SetTestStepResult(EFail);
		}
	}


void CT_LbsAssDataStep_Main::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyFacilityLcsMoLrResult(aReason, aPosition);
	
	if (aReason != KErrNone)
		{	
		INFO_PRINTF2(_L("Failed test, bad release reason %d."), aReason);
		SetTestStepResult(EFail);
		}


	// Verify the real position returned from the network, this will be the same position
	// we sent to the network as the result of the MO-LR, thus use the entry given by
	// the test module.
	T_LbsUtils utils;
	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* verifyRealPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	if (!utils.Compare_PosInfo(*verifyRealPosInfo, aPosition))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
	    } 
	}

void CT_LbsAssDataStep_Main::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementReportLocation(aPosition);
	
	// MT-LR complete.
	if (EStartMTLRLocReq == iState)
		{
		iState = EWaiting;
		}
	
	// Verify the real position given to the network, this will be the same position
	// returned as the result of the MO-LR, thus use the entry given by
	// the test module.
	
	T_LbsUtils utils;
    RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	
	TPositionInfo* verifyRealPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	if (!utils.Compare_PosInfo(*verifyRealPosInfo, aPosition))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		} 
    }

void CT_LbsAssDataStep_Main::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementReportRequestMoreAssistanceData(aFilter);

	switch (iTestCaseId)
		{
		// Test case LBS-Ass-Data-0042 - Request multiple items of GPS assistance data when not all of
		// the items are available.
		case 42:
			{
			if (aFilter != EAssistanceDataReferenceLocation)
				{
				INFO_PRINTF2(_L("Failed test, incorrect assistance data group filter expecting Ref Location, got %d."), aFilter);
				SetTestStepResult(EFail);
				}
			}
			break;
	
		default:
			{
			INFO_PRINTF2(_L("Failed test, got un-expected request for more assistance data with filter value = %d."), aFilter);
			SetTestStepResult(EFail);
			}
		}
	}

void CT_LbsAssDataStep_Main::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);

	switch (iTestCaseId)
		{
		// Test case LBS-Ass-Data-0012 - Cancel assistance data request test.
		case 12:
			{
			if (aReason != KErrCancel)
				{
				INFO_PRINTF2(_L("Failed test, incorrect reason code expecting KErrCancel, got %d."), aReason);
				SetTestStepResult(EFail);
				}
			}
			break;
			
		default:
			{
			INFO_PRINTF2(_L("Failed test, got un-expected control failure with reason code = %d."), aReason);
			SetTestStepResult(EFail);
			}
		}
	}

/**	Notify position update callback.

	The notify position update as completed. We can mark as done in the callback flags.
*/
void CT_LbsAssDataStep_Main::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("Got - Notify Update - Callback Event."));
	
	// We should wait for the agps update (first one will be reference position) before setting this flag
	if(iState != EFirstPosUpdate)
		{
		SetCallbackFlag(KLbsCallback_Got_NotifyUpdate);
		}
	
	if (KErrNone != aStatus.Int())
		{
		INFO_PRINTF2(_L("Failed test, pos info request err = %d."), aStatus.Int());
		SetTestStepResult(EFail);
		}
    
    // iState is in EFirstPosUpdate after quick off first iDoPosUpdate
	if (iState == EFirstPosUpdate)
		{
	    /* iCurrentPosInfoArr index 0 contains ref pos,we need to store this in 
		   iCurrentPosInfoArr index 1 for later use,before index 0 updated with realpos*/
   		RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		TPositionInfo* posInfo = reinterpret_cast<TPositionInfo*>(posInfoArr[0]);
		
    	//Some tests don't need to store ref pos 
		switch (iTestCaseId)
			{
		    // Test case LBS-Ass-Data-0012 - Cancel assistance data request test.
		    case 12:
			// Test case LBS-Ass-Data-0040 - Empty GPS assistance data cache test.
			case 40:
			// Test case LBS-Ass-Data-0041 - Request GPS assistance data when no network available.
			case 41:	
				{
				 // Do nothing.
				}
			    break;
			
		    default:
				{
				TPositionInfo* refPosInfo = new(ELeave) TPositionInfo();
		        
		        // Copy ref loc from posInfo to refPosInfo   
		        TInt err = CopyPositionTypes(*refPosInfo, *posInfo);
			    if (err)
				{
				delete refPosInfo;
				INFO_PRINTF2(_L("Failed test, err %d copying position infos."), err);
				SetTestStepResult(EFail);		
				}
			    // Append ref loc to iCurrentPosInfoArr[1] for later use
				posInfoArr.Append(refPosInfo);

				}
			}
         			
        // Kick off pos update - this will invoke the real position
		iDoPosUpdate->StartL(*posInfo);
		
		// Move to the waiting state after kick off second pos update
		iState = CT_LbsNetSimStep::EWaiting;
    	}
	}	
		

/** The module's output data bus has been updated.
	This function is called when the module sends back answers to both the assistance data checks
	and module test mode updates. */
void CT_LbsAssDataStep_Main::HandleDataBusUpdateL(const TModuleDataOut& aModuleDataOut)
	{
	INFO_PRINTF1(_L("Got - Module Response - Callback Event."));

	SetCallbackFlag(KLbsCallback_Got_ModuleAssDataAnswer);

	switch (iTestCaseId)
		{
		//
		case 1:
			{
			// TODO: Look for startup - shutdown ok responses...
			}
			break;
		
		// Check for cancel request response.
		case 12:
			{
			if(aModuleDataOut.iRequestType == TModuleDataOut::EModuleResponse)
				{
				//modDataOut.iResponse = TModuleDataOut::EModuleAssDataOk; 
				switch (aModuleDataOut.iResponse)
					{
					case TModuleDataOut::EModuleResponseOk:
						INFO_PRINTF1(_L("      Cancel request, module response."));
					    break;

					default:
						INFO_PRINTF1(_L("Failed test, unexpected response from module."));
						SetTestStepResult(EFail);
						break;
					}
				
				}
			
			else
				{
				INFO_PRINTF1(_L("Failed test, unexpected request type from module."));
				SetTestStepResult(EFail);	
				}
			}
			break;
		
		default:
			{
			INFO_PRINTF1(_L("      Assistance Data Verify Answer."));

			switch (aModuleDataOut.iResponse)
				{
				// Fail test if ass data failed verifcation, or received un-expected response code.
				case TModuleDataOut::EModuleAssDataOk:
					{
					// Ensure we only get a single assistance data event for each test, except for LBS-Ass-Data_0042
					if (iTestCaseId != 42)
						{
						iAssEventCount++;
						}
					
					if (iAssEventCount > 2)
						{
						INFO_PRINTF1(_L("Failed test, more than a single assistance data event receivced."));
						SetTestStepResult(EFail);
						}
					}
					break;
				
				// Ignore requests for assistance data.
				case TModuleDataOut::EModuleAssDataRequestedOk:
					// Do nothing.
					break;
					
				default:
					{
 					INFO_PRINTF2(_L("Failed test, module assistance data verification, got: %d."), aModuleDataOut.iResponse);
					SetTestStepResult(EFail);
					}
					break;
				}
			}
			break;	
		}
	}


/** Privacy controller callbacks.
*/
void CT_LbsAssDataStep_Main::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType)
	{
	// We not aiming to test MT-LR here so only carryout minimal verification - verify the request id only.
	(void)aRequestInfo;
	(void)aNotificationType;
	
	iPrivacyController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	
	iMtlrRequestId = aRequestId;
	}


void CT_LbsAssDataStep_Main::ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo)
	{
	// Verify the request id.
	if (aRequestId != iMtlrRequestId)
		{
		INFO_PRINTF3(_L("Failed test, incorrect request ID, got %d, expecting %d."), aRequestId, iMtlrRequestId);
		SetTestStepResult(EFail);
		}
	
	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* posInfo = new(ELeave) TPositionInfo();

	T_LbsUtils utils;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

	// As we don't own the passed posinfo, copy it.
	TInt err = CopyPositionTypes(*posInfo, aPosInfo);
	if (err)
		{
		INFO_PRINTF2(_L("Failed test, err %d copying position infos."), err);
		SetTestStepResult(EFail);		
		}

	posInfoArr.Append(posInfo);
	}


void CT_LbsAssDataStep_Main::ProcessRequestComplete(TUint aRequestId, TInt aReason)
	{	
	// Verify the request id.
	if (aRequestId != iMtlrRequestId)
		{
		INFO_PRINTF3(_L("Failed test, incorrect request ID, got %d, expecting %d."), aRequestId, iMtlrRequestId);
		SetTestStepResult(EFail);
		}
		
	// Verify the reason code.
	if (aReason)
		{
		INFO_PRINTF2(_L("Failed test, incorrect reason code, got %d, expecting KErrNone."), aReason);
		SetTestStepResult(EFail);
		}
	}

/** Helper functions taken from internal Lbs utilities.
*/

/**
Deep copy of position info data by type

@param aTo the destination position info object reference
@param aFrom the source position info object reference
@internalComponent
@released
*/
TInt CopyPositionTypes(TPositionInfoBase& aTo, const TPositionInfoBase& aFrom)	
	{
	const TUint32 toClassType = aTo.PositionClassType();
	const TUint toClassSize = aTo.PositionClassSize();
	const TUint32 fromClassType = aFrom.PositionClassType();
	const TUint fromClassSize = aFrom.PositionClassSize();
	
	// check for self assignment
	if(&aTo == &aFrom)
		{
		return KErrNone; // copy is implicit and already done :)
		}	
	// check the assignment is even possible the source type must be at
	// at least as BIG as the dest, or  you leave unitialized data in the destination
	if(fromClassSize < toClassSize)
		return KErrArgument;
	// check we are not assigning base types
	if(toClassSize == sizeof(TPositionInfoBase)
			|| fromClassSize == sizeof(TPositionInfoBase)
			|| toClassType == EPositionInfoUnknownClass // this the type of a TPositionInfoBase
			|| fromClassType == EPositionInfoUnknownClass)
		{
		return KErrArgument; // bad types - trying to copy between base types
		}
	// check the aTo type
	TInt typeError = SupportedType(toClassType, toClassSize);
	if(typeError != KErrNone)
		return typeError; // i.e. KErrNotSupported
	// check the aFromType
	typeError = SupportedType(fromClassType, fromClassSize);
	if(typeError != KErrNone)
		return typeError; // i.e. KErrNotSupported
	// Ok now we know we can copy these things from one to the other
	// so here goes - the catch is we cannot overwrite the type and size
	// of the destination, so we need a bit of pointer math.
	// NB this relies on the being a class inheritance such that all of the
	// types are derived from TPositionInfoBase first. Or the math goes
	// haywire. This also implies a whole slew of assumptions about the ram
	// layout of these classes....
	TUint8* baseToAddr = reinterpret_cast<TUint8*>(&aTo)+sizeof(TPositionInfoBase);			
	const TUint8* const baseFromAddr = reinterpret_cast<const TUint8*>(&aFrom)+sizeof(TPositionInfoBase);
	TUint32 dataLength = toClassSize-sizeof(TPositionInfoBase); // we copy only this many bytes form the source
	TUint8* endAddr = Mem::Copy(baseToAddr, baseFromAddr, dataLength);
	// Sanity check the Mem::Copy() - just in case...
	if(endAddr != baseToAddr+dataLength)
		return KErrGeneral; // Or KErrNoMemory?
	else
		return KErrNone;
	}

/**
Check the size for supported position info type

@param aType the position info type
@param aSize the size of specified type
@return Symbian standard error code
@internalComponent
@released
*/
TInt SupportedType(const TUint32& aType, const TInt& aSize)
	{
	if(aType==EPositionInfoClass)
		{
		if(aSize!=sizeof(TPositionInfo))
			return KErrNotSupported; // something weird. Type ok but the size is wrong			
		}
	else if(aType==(EPositionInfoClass|EPositionCourseInfoClass))
		{
		if(aSize!=sizeof(TPositionCourseInfo))
			return KErrNotSupported; // something weird. Type ok but the size is wrong			
		}
	else if(aType==(EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass))
		{
		if(aSize!=sizeof(TPositionSatelliteInfo))
			return KErrNotSupported; // something weird. Type ok but the size is wrong			
		}
	// othwerwise the size and type are what we expected
	return KErrNone;
	}

