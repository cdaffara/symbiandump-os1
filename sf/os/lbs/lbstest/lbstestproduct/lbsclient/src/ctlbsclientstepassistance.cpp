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
// @file CT_LbsClientStepAssistance.cpp
// This is the class implementation for the LBS Client Step Assistance
// 
//

//LBS includes.
#include <lbs.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>

// LBS test includes.
#include "ctlbsclientstepassistance.h"
#include <lbs/test/tlbsutils.h>


/**
Static Constructor
*/
CT_LbsClientStep_Assistance* CT_LbsClientStep_Assistance::New(CT_LbsClientServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsClientStep_Assistance* testStep = new CT_LbsClientStep_Assistance(aParent);
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
CT_LbsClientStep_Assistance::CT_LbsClientStep_Assistance(CT_LbsClientServer& aParent) : CT_LbsNetSimStep(), iParent(aParent),iLbsAdmin(NULL)
	{
	SetTestStepName(KLbsClientStep_Assistance);
	}


void CT_LbsClientStep_Assistance::ConstructL()
	{
	// Create the base class objects.
    CT_LbsNetSimStep::ConstructL();
	
	// and the active object wrapper for the notify position update.
	iDoPosUpdate = CT_LbsDoPosUpdate::NewL(this);
	
	// create the admin
	iLbsAdmin = CLbsAdmin::NewL();
	}

/**
 * Destructor
 */
CT_LbsClientStep_Assistance::~CT_LbsClientStep_Assistance()
	{
	delete 	iLbsAdmin;

	iDoPosUpdate->Cancel();
	delete iDoPosUpdate;

	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_Assistance::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API with assistant data.
	INFO_PRINTF1(_L("&gt;&gt; CT_LbsClientStep_Assistance::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
												
		// Expected callbacks flags, most test steps require MO-LR.
		iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect | 		// Net Sim callback events.
							KLbsCallback_MoLr |
							KLbsCallback_Got_NotifyUpdate;	// notifypositionupdate completed.
				
		// Carry out unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
			
				case 16:
			       {   					
				
				   // Set 'assisted-if-possible' (EGpsPreferTerminalBased) mode
				
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
										
					if(mode != CLbsAdmin::EGpsPreferTerminalBased)
						{
						SetTestStepResult(EFail);
						}
				
				    }
					break;
				
				
				case 18: 
                   {
                    	
                                	
                   // Set 'assisted-if-possible' (EGpsPreferTerminalBased) mode
				
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
					
					if(mode != CLbsAdmin::EGpsPreferTerminalBased)
						{
						SetTestStepResult(EFail);
						}
				   }
					break;
			
				//  Test case LBS-NotifyPosUpdate-0020
				case 20:
					{

                    // Set 'assisted-if-possible' (EGpsPreferTerminalBased) mode
				
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
					// TO DO - ideally we should wait for the admin callback for this change here instead of pausing:
				 	User::After(5000000);
					if(mode != CLbsAdmin::EGpsPreferTerminalBased)
						{
						SetTestStepResult(EFail);
						}
					}
					break;
				
				default:
					{
					User::Panic(KLbsClientStep_Assistance, KErrUnknown);
					}					
				}
			}
					
	
	// Inform the test module of the required test mode - will block.
	
	// TODO remove modDataIn stuff all of it
//	utils.NotifyModuleOfConfigChangeL(modDataIn);
			
	// Connect to net sim.

	iNetSim.ConnectL(/*(MLbsNetSimTestObserver*)*/this);

	// Setup net sim's assistance data.

	// Create reference location. The test step will verify this is returned correctly
	// in the netsim callbacks only, it will not be used by the test assistance data provider.
 
	// Location to use.
	RPointerArray<TAny>& srcPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* srcPosInfo = reinterpret_cast<TPositionInfo*>(srcPosInfoArr[0]);
	TPosition srcPos;
	srcPosInfo->GetPosition(srcPos);
	
		if (!iNetSim.SetReferenceLocation(srcPos))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
			SetTestStepResult(EFail);

			iNetSim.Close();

			return TestStepResult();
			}	
	// Set plugin to use.			
	TUid pluginUid = TUid::Uid(KSuplAssistanceDataProviderPluginUidValue);
		if (!iNetSim.SetAssistanceDataProvider(pluginUid))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's assistance data plugin uid."));
			SetTestStepResult(EFail);
			
			iNetSim.Close();

			return TestStepResult();
			}
	
	
		// Kick off the keep alive timer - do last to ensure NetSim is ready otherwise the
		// timer callback may start a MO-LR if the NetSim connected to early.
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod);

		iKeepAliveTimer->SetTimer(interval);


		// Kick off test.
		CActiveScheduler::Start();


		// Clean up.

		//iNetSim.ClearAssistanceDataFilters();
		iNetSim.Close();
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_Assistance::doTestStepL()"));

	return TestStepResult();
	}
	
	/** NetSim callbacks given for a MoLr, which we invoke as a result of the notify position update.
*/

// This method called when a client connects to the gateway API

void CT_LbsClientStep_Assistance::Connected()
	{
	// Call base implementation.
	CT_LbsNetSimStep::Connected();
	
	switch (iTestCaseId)
		{
		
		case 16:
			{
			//Configure network simulation to ensure it does not deliver assistance data.
			//need to be changed
			TBool ret = iNetSim.SetResponseError(KErrNotReady, ETrue);
			}
			break;
				
		// All other test cases required a MO-LR.
		default:
			{
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
			
			// Move to the waiting state.
			iState = CT_LbsNetSimStep::EWaiting;
			}
		}
	}


//called when a client disconnects to the gateway API
void CT_LbsClientStep_Assistance::Disconnected()
	{
	// Call base implementation.
	CT_LbsNetSimStep::Disconnected();
	}

// The data to start MoLr . If this blank then we are starting an MoLr, if not then X3P 		
void CT_LbsClientStep_Assistance::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyRegisterLcsMoLr(aData);

	// Verify the aData is blank to indicate this is a MOLR.
	if (aData != KNullDesC)
		{
		INFO_PRINTF1(_L("Failed test, register contains data."));
		SetTestStepResult(EFail);	
		}
	}					
// The reason the MoLr ended					
void CT_LbsClientStep_Assistance::NotifyReleaseLcsMoLr(TInt aReason)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyReleaseLcsMoLr(aReason);

	if (aReason != KErrNone)
	    {	
		INFO_PRINTF2(_L("Failed test, bad release reason %d."), aReason);
		SetTestStepResult(EFail);
	    }
			
	}
// A measurement control   

void CT_LbsClientStep_Assistance::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
  														      const TLbsNetPosRequestQuality& aQuality)
	
	{
	T_LbsUtils utils;	
	
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementControlLocation(aPosition, aData, aQuality);

	// Verify the reference position - use the entry in the verify pos info array.
	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	
	if (!utils.Compare_PosInfo(*verifyPosInfo, aPosition, T_LbsUtils::ERoughAccuracy))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		}
		
		
// TODO: Check if we can verify aQuality in any way.
	(void)aQuality;
	}
	
	
void CT_LbsClientStep_Assistance::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyFacilityLcsMoLrResult(aReason, aPosition);
	
	if (aReason != KErrNone)
		{	
		INFO_PRINTF2(_L("Failed test, bad release reason %d."), aReason);
		SetTestStepResult(EFail);
		}
	
	// Verify the position returned from the network, this will be the same position
	// we sent to the network as the result of the MO-LR, thus use the entry given by
	// the test module.
	T_LbsUtils utils;
	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	
	if (!utils.Compare_PosInfo(*verifyPosInfo, aPosition, T_LbsUtils::ERoughAccuracy))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		}	
	}

void CT_LbsClientStep_Assistance::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementReportLocation(aPosition);
	
	// Verify the position given to the network, this will be the same position
	// returned as the result of the MO-LR, thus use the entry given by
	// the test module.
	T_LbsUtils utils;
	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	
	if (!utils.Compare_PosInfo(*verifyPosInfo, aPosition, T_LbsUtils::ERoughAccuracy))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		}
	}

void CT_LbsClientStep_Assistance::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);

    INFO_PRINTF2(_L("Failed test, got un-expected control failure with reason code = %d."), aReason);
	SetTestStepResult(EFail);
	
	}
	
void CT_LbsClientStep_Assistance::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report Request More Assistance Data - Callback Event."));
	
	// Note that we do NOT set the callback flag here (since it may not be set and the base class does an equivalence test, so we don't test for it)
	}	
	
/**	Notify position update callback.

	The notify position update as completed. We can mark as done in the callback flags.
*/
void CT_LbsClientStep_Assistance::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("Got - Notify Update - Callback Event."));
	
	SetCallbackFlag(KLbsCallback_Got_NotifyUpdate);
	
	if (KErrNone != aStatus.Int())
		{
		INFO_PRINTF2(_L("Failed test, pos info request err = %d."), aStatus.Int());
		SetTestStepResult(EFail);
		}
	
// TODO also we expect this to happen before the last netsim callbacks happen if not
// we can't use current pos in those funcs maybe have to use the verify one not as nice.	


	}	


//END

