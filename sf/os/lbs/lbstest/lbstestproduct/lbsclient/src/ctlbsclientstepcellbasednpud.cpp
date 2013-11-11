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
// @file ctlbsclientstep_cellbasednpud.cpp
// This is the class implementation for the Notify Position Update Tests
// 
//
 
#include "ctlbsclientstepcellbasednpud.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>
#include <lbs/EPos_CPosModules.h>
#include <lbs/EPos_CPosModuleUpdate.h>
#include "netpsy.hrh"

// constant definitions

_LIT(KLbsClientStepCellBasedNotifyPosUpdate, "LbsClientStepCellBasedNotifyPosUpdate");

// by default, a test case will abort after this time if all callbacks not completed:
// Note that this must be less than the TEF timeout in the script to be of any value
TTimeIntervalMicroSeconds32 KDefaultAbortTimerPeriod = 90*1000000;	// 1.5 mins 

// by default, a test case running with real gps hardware will abort after this time 
// Note that this must be less than the TEF timeout in the script to be of any value
TTimeIntervalMicroSeconds32 KDefaultAbortTimerPeriod_real = 100*1000000;	// 5 mins


/**
 * Destructor
 */
CT_LbsClientStep_CellBasedNotifyPosUpdate::~CT_LbsClientStep_CellBasedNotifyPosUpdate()
	{
	iDoPosUpdate->Cancel();
	delete iDoPosUpdate;
	}


/**
 * Constructor
 */
CT_LbsClientStep_CellBasedNotifyPosUpdate::CT_LbsClientStep_CellBasedNotifyPosUpdate(CT_LbsClientServer& aParent) : iParent(aParent)
	{	
	SetTestStepName(KLbsClientStep_CellBasedNotifyPosUpdate);
	}


/**
Static Constructor
*/
CT_LbsClientStep_CellBasedNotifyPosUpdate* CT_LbsClientStep_CellBasedNotifyPosUpdate::New(CT_LbsClientServer& aParent)
	{
	// Can't leave, so return NULL if insufficient memory
	CT_LbsClientStep_CellBasedNotifyPosUpdate* testStep = new CT_LbsClientStep_CellBasedNotifyPosUpdate(aParent);
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
	
void CT_LbsClientStep_CellBasedNotifyPosUpdate::ConstructL()
	{
	// Create the base class object.
    CT_LbsNetSimStep::ConstructL();
	
	// Re-enable the network PSY
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    
    moduleUpdate->SetUpdateAvailability(ETrue);
    moduleUpdate->SetUpdateVisibility(ETrue);
    TUid uid = TUid::Uid(KPosNETPSYImplUid);
    db->UpdateModuleL(uid, *moduleUpdate);
    
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    
    // Active object wrapper for the notify position update.
	iDoPosUpdate = CT_LbsDoPosUpdate::NewL(this, TPositionModuleInfo::ETechnologyNetwork);
	
	}
	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL()"));

	if (TestStepResult()==EPass)
		{
		iNetworkUnavailable = FALSE;
		iIsCancelTest = FALSE;
   		iCancel = FALSE;
		iCancelOnPosUpdate = FALSE;
		iCancelBeforePosUpdate = FALSE;
		iMultiReq = EFalse;
		iNumNPUDs = iParent.iSharedData->iVerifyPosInfoArr.Count(); // how many NPUDs to do
		
		// Expected callbacks flags
		iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect | 	
							KLbsCallback_NetworkBasedLocation |
							KLbsCallback_Got_NotifyPositionUpdate;	// notifypositionupdate completed.
	
		
		TTimeIntervalMicroSeconds32 abortInterval;
		if(iParent.iSharedData->iTestModuleInUse)
			{
			abortInterval = KDefaultAbortTimerPeriod;
			}
		else
			{	
			abortInterval = KDefaultAbortTimerPeriod_real;
			}
		
		// Carryout unique test actions.
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
				switch (testCaseId)
				{												
				case 1:	// LBS-NotifyPosUpdateCellBased-0001 Request a cell-based location
					{
					// nothing to do 
					}
					break;
				
				case 2:	// LBS-NotifyPosUpdateCellBased-0002	Request a cell-based location and cancel before it completes
					{
					iCancel = ETrue;
					iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect |
										KLbsCallback_Got_NotifyPositionUpdate;
					}
					break;
				
				case 3:	// LBS-NotifyPosUpdateCellBased-0003	Request a cell-based location, when network unavailable
					{
					iNetworkUnavailable = ETrue;

					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 	
									KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr | 
									KLbsCallback_Got_NotifyPositionUpdate;
					
					}
					break;
					
				case 4:	// LBS-NotifyPosUpdateCellBased-0004 multiple sequential npuds
					{
					iMultiReq = ETrue;
					}
					break;
					
				case 5:	// LBS-NotifyPosUpdateCellBased-0005	Request a cell-based location and attempt to cancel when the update is received
					{
					iCallbackFlags = 0;
					iCancelOnPosUpdate = ETrue;
					iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation |
										KLbsCallback_Got_NotifyPositionUpdate;
					}
					break;
					
				case 6:	// LBS-NotifyPosUpdateCellBased-0006	Request a cell-based location and cancel before a position update is generated
					{
					iCancelBeforePosUpdate = ETrue;
					iFlagsToHaltOn =	KLbsCallback_NetSim_Got_Connect |
										KLbsCallback_Got_NotifyPositionUpdate;
					}
					
					break;			
	
				default:
					User::Panic(KLbsClientStepCellBasedNotifyPosUpdate, KErrUnknown);			
				}
			}
		else
			{
			User::Panic(KLbsClientStepCellBasedNotifyPosUpdate, KErrUnknown);
			}	
			
		iIsCancelTest = (iCancel || iCancelBeforePosUpdate || iCancelOnPosUpdate);	
		
		// Steps common to all tests:
		
		// Connect to net sim
		iNetSim.ConnectL(this);

	// Configure NetSim:	
		// Set reference location:
		RPointerArray<TAny>& srcPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		TPositionInfo* srcPosInfo = reinterpret_cast<TPositionInfo*>(srcPosInfoArr[0]);
		TPosition srcPos;
		srcPosInfo->GetPosition(srcPos);
		if (!iNetSim.SetReferenceLocation(srcPos))
			{
			INFO_PRINTF1(_L("CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL() Failed: Can't set NetSim's reference location."));
			SetTestStepResult(EFail);
			}	
		
		// Set plugin to use for getting assistance data:
		TUid pluginUid;
		if(iParent.iSharedData->iTestModuleInUse)
			{
			pluginUid = TUid::Uid(KSimpleAssistanceDataProviderPluginUidValue);
			}
		else
			{
			pluginUid = TUid::Uid(KSuplAssistanceDataProviderPluginUidValue);
			}
		if (!iNetSim.SetAssistanceDataProvider(pluginUid))
			{
			INFO_PRINTF1(_L("Can't set NetSim's assistance data plugin uid."));
			SetTestStepResult(EFail);
			}
			
		// Kick off the keep alive timer - do last to ensure NetSim is ready otherwise the
		// timer callback may start a MO-LR if the NetSim connected too early.
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod);

		iKeepAliveTimer->SetTimer(interval);

		// don't wait for TEF to timeout and abort
		iAbortTimer->SetTimer(abortInterval);

		// Kick off test.
		CActiveScheduler::Start();
			
		if(EDone != iState)
			{
			INFO_PRINTF2(_L("Failed test, finished in state %d"), iState);
			INFO_PRINTF3(_L("Callback flags currently set = 0x%x. Expecting 0x%x"), iCallbackFlags, iFlagsToHaltOn);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("Got all expected callbacks (0x%x)"), iCallbackFlags);			
			}
						
		iNetSim.Close();
			
		}
		
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL()"));

	return TestStepResult();
	}


void CT_LbsClientStep_CellBasedNotifyPosUpdate::Connected()
	{
	// Call base implementation.
	CT_LbsNetSimStep::Connected();
			
	if(iCancelBeforePosUpdate)
		{
		TPositionInfo* posInfo = new(ELeave) TPositionInfo();
		iDoPosUpdate->StartL(*posInfo);
	
		iNumNPUDs--;
	
		// Wait for all expected callback flags to be set:
		iState = CT_LbsNetSimStep::EWaiting;	
		
		// Cancel the request for the cancel test.
		iDoPosUpdate->CancelRequest();
		delete posInfo;
		}
	else
		{
		T_LbsUtils utils;
		// Create a posinfo and store in our shared array for later verification.
		RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		TPositionInfo* posInfo = new(ELeave) TPositionInfo();
		utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
		posInfoArr.Append(posInfo);
		
		iDoPosUpdate->StartL(*posInfo);
	
		iNumNPUDs--;
	
		// Wait for all expected callback flags to be set:
		iState = CT_LbsNetSimStep::EWaiting;	
		
		// Cancel the request for the cancel test.
		if(iCancel)				
			{
			User::After((TTimeIntervalMicroSeconds32)100000);   // arbitrary delay as workaround for DEF107349
			iDoPosUpdate->CancelRequest();
			}			
		}
	}
	
void CT_LbsClientStep_CellBasedNotifyPosUpdate::Disconnected()
	{
	// Call base implementation.
	CT_LbsNetSimStep::Disconnected();
	}

void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	// need to tell NetSim to produce an error here
	if(iNetworkUnavailable)
		{
		// Tell netsim to make network unavailable until further notice:
		if(!iNetSim.SetResponseError(RLbsNetSimTest::KNetSimNetworkNotAvailable, ETrue))	//sticky
			{
			INFO_PRINTF1(_L("CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL() Failed: Can't set NetSim's response error."));
			SetTestStepResult(EFail);
			}
		}
	
    if (!iIsCancelTest)
    	{
    	// Call base implementation.
		CT_LbsNetSimStep::NotifyRegisterLcsMoLr(aData);
    	}
	}

void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyReleaseLcsMoLr(TInt aReason)
	{
	if (!iIsCancelTest)
    	{
		// Call base implementation.
		CT_LbsNetSimStep::NotifyReleaseLcsMoLr(aReason);	
    	}	
	}

// Measurement Control has been received from Network:
void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	if (!iIsCancelTest)
    	{
		// Call base implementation.
		CT_LbsNetSimStep::NotifyMeasurementControlLocation(aPosition, aData, aQuality);
    	}
	}

void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	if (!iIsCancelTest)
    	{
		// Call base implementation.
		CT_LbsNetSimStep::NotifyReleaseLcsLocationNotification(aResult);
    	}	
	}

void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	if (!iIsCancelTest)
    	{
		// Call base implementation.
		CT_LbsNetSimStep::NotifyFacilityLcsMoLrResult(aReason, aPosition);
    	}
	}

// Our position has been reported to network
void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	if (iCancelOnPosUpdate)
		{
		// We need to wait until the NPUD is completed. 
		User::After(1000000);
		
		TInt cancelError = iDoPosUpdate->CancelRequestWithResult();
					
		if (cancelError != KErrNotFound)
			{
			INFO_PRINTF2(_L("FAILED: NotifyMeasurementReportLocation cancelling late produces %d rather than KErrNotFound"), cancelError);
			SetTestStepResult(EFail);
			}			
		}
	
	if (!iIsCancelTest || iCancelOnPosUpdate)
    	{
		// Call base implementation.
		CT_LbsNetSimStep::NotifyMeasurementReportLocation(aPosition);
    	}
		
	}

void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	if (!iIsCancelTest)
    	{
		// Call base implementation.
		CT_LbsNetSimStep::NotifyMeasurementReportRequestMoreAssistanceData(aFilter);
    	}
	}

void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	if (!iIsCancelTest && !iMultiReq)
    	{
		// Call base implementation.
		CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);
		}
	else if (iMultiReq && ! ((aReason == KErrPositionHighPriorityReceive) || (aReason == KErrServerBusy)))
		{
		INFO_PRINTF2(_L("FAILED: NotifyMeasurementReportControlFailure with reason = %d"), aReason);
		SetTestStepResult(EFail);
		}
	}

void CT_LbsClientStep_CellBasedNotifyPosUpdate::NotifyError(MLbsNetSimTestObserver::EFunction aFunction, int aError)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyError(aFunction, aError);
	}

void CT_LbsClientStep_CellBasedNotifyPosUpdate::ProcessMeasurementControlLocationError(TInt aError)
	{
	// Call base implementation.
	CT_LbsNetSimStep::ProcessMeasurementControlLocationError(aError);
	}

/**	Notify position update callback.
*/
void CT_LbsClientStep_CellBasedNotifyPosUpdate::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	TInt err = aStatus.Int();
	
	INFO_PRINTF2(_L("CT_LbsClientStep_CellBasedNotifyPosUpdate::MT_LbsDoPosUpdateCallback: Got - Notify Update - Callback Event. Error = %d"), err);
	
	if(!iNumNPUDs)	// only set the flag when we've got them all
		{
		SetCallbackFlag(KLbsCallback_Got_NotifyPositionUpdate);
		}
		
	if(iNetworkUnavailable)
		{
		if(KErrTimedOut != err && KErrDisconnected != err)
			{
			INFO_PRINTF2(_L("FAILED: MT_LbsDoPosUpdateCallback with error %d, expecting KErrTimedOut"), err);
			SetTestStepResult(EFail);
			}
		}
	else
		
		{
		if(iCancel)
			{
			if(KErrCancel != err && KErrNone != err)
				{
				INFO_PRINTF2(_L("FAILED: MT_LbsDoPosUpdateCallback with error = %d"), err);
				SetTestStepResult(EFail);
				}
			}
		else 
			{
			if(iCancelBeforePosUpdate)
				{
				if(KErrCancel != err)
					{
					INFO_PRINTF2(_L("FAILED: MT_LbsDoPosUpdateCallback with error = %d"), err);
					SetTestStepResult(EFail);
					}
				}
			else
				{
				if(KErrNone != err)
					{
					INFO_PRINTF2(_L("FAILED: MT_LbsDoPosUpdateCallback with error = %d"), err);
					SetTestStepResult(EFail);
					}
				}
			}		
		}
	
	if(iNumNPUDs--)
		{	
		// Create another posinfo and store in our shared array for later verification.
		RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		TPositionInfo* posInfo = new(ELeave) TPositionInfo();
		posInfoArr.Append(posInfo);
			
		iDoPosUpdate->StartL(*posInfo);
		}	
	}

