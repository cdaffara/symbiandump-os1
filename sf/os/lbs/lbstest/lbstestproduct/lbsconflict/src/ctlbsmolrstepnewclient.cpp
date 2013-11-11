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
// @file ctlbsmolrstepnewclient.cpp
// This is the class implementation for the MoLr New Client Tests
// EPOC includes.
// 
//

// LBS includes. 
#include <lbs.h>
#include <lbs/lbsnetcommon.h>
#include <lbssatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <e32math.h>

// LBS test includes.
#include "ctlbsmolrstepnewclient.h"
#include <lbs/test/tlbsutils.h>


/**
Static Constructor
*/
CT_LbsMolrStep_NewClient* CT_LbsMolrStep_NewClient::New(CT_LbsConflictServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMolrStep_NewClient* testStep = new CT_LbsMolrStep_NewClient(aParent);
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
CT_LbsMolrStep_NewClient::CT_LbsMolrStep_NewClient(CT_LbsConflictServer& aParent) : CT_LbsConflictStep(aParent)
	{
	SetTestStepName(KLbsMolrStep_NewClient);
	}


void CT_LbsMolrStep_NewClient::ConstructL()
	{
	// Create the base class objects.
	CT_LbsConflictStep::ConstructL();
	
	// Self locate async wrapper.
	iDoPosUpdate = CT_LbsDoPosUpdate::NewL(this);

	}

/**
 * Destructor
 */
CT_LbsMolrStep_NewClient::~CT_LbsMolrStep_NewClient()
	{
	iDoPosUpdate->Cancel();
	delete iDoPosUpdate;
	}



/** A standard TEF test case doTestStepL, this SHOULD only support a single test case.
	Typically the function will look much like this.
 */
TVerdict CT_LbsMolrStep_NewClient::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMolrStep_NewClient::doTestStepL()"));

	CLbsAdmin* lbsAdminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(lbsAdminApi);

	if (TestStepResult() == EPass)
		{		
		// Carry out unique test actions.
		iPosUpdateFlag = ETrue;
		
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				case 01:
					{
					CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsAutonomous;
					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);
					}
					break;
				
				case 02:
					{
					CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsPreferTerminalBased;
					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);

					//We dont check for EClient_Got_PosUpdate_Complete event as we not sure when it will come
					//But we do make sure that EClient_Got_PosUpdate_Complete is called appropriate number of times
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);		// <-- From Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);		// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);			// 
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net
					}
					break;
				
				case 03:
				case 04:
					{
					CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsPreferTerminalBased;
					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);
					
					//We dont check for EClient_Got_PosUpdate_Complete event as we not sure when it will come
					//But we do make sure that EClient_Got_PosUpdate_Complete is called appropriate number of times
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);		// <-- From Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);		// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);			// 
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net

					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);		// <-- From Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);		// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);			// 
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net
					}
					break;

				case 05:
					{
					CLbsAdmin::TGpsMode gpsMode = CLbsAdmin::EGpsPreferTerminalBased;
					lbsAdminApi->Set(KLbsSettingHomeGpsMode,gpsMode);
					
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
					//SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);		// <-- From Net
					}
					break;

				case 06:
					{
					T_LbsUtils utils;
					TModuleDataIn modDataIn; // Used to send test information to the test module.
					modDataIn.iRequestType = TModuleDataIn::EModuleRequestTestMode;
					modDataIn.iAssDataEventType = TModuleDataIn::EAssDataEventExpectSingle;
					modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_SomeDataNotAvailable;
					
					utils.NotifyModuleOfConfigChangeL(modDataIn);

					//We dont check for EClient_Got_PosUpdate_Complete event as we not sure when it will come
					//But we do make sure that EClient_Got_PosUpdate_Complete is called appropriate number of times
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);		// <-- From Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);		// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);		// <-- From Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);		// --> To Net
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);			// 
					SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net
					}
					break;
				
				default:
					{
					User::Panic(KLbsMolrStep_NewClient, KErrUnknown);
					}					
				}
			}

		// Open and setup net sim.
		OpenNetSim(this);

		// Kick off the test abort and keep alive timers.
		TTimeIntervalMicroSeconds32 abortInterval(KLbsAbortPeriod);
		TTimeIntervalMicroSeconds32 keepAliveInterval(KLbsKeepAlivePeriod);

		iAbortTimer->SetTimer(abortInterval);
		iKeepAliveTimer->SetTimer(keepAliveInterval);

		// Kick off test.
		CActiveScheduler::Start();

		// Verify location data.
		VerifyPosInfos();

		// Clean up.
		CloseNetSim();
		}

	CleanupStack::PopAndDestroy(lbsAdminApi);

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsMolrStep_NewClient::doTestStepL()"));

	return TestStepResult();
	}


void CT_LbsMolrStep_NewClient::VerifyPosInfos()
	{
	T_LbsUtils utils;
    RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	RPointerArray<TAny>& currPosInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	//TPositionInfo* currPosInfo;
	

	//If we get Netsim callbacks, the first position in iCurrentPosInfoArr is reference position from Netsim
	

	if(verifyPosInfoArr.Count() == currPosInfoArr.Count())
		{
		TPositionInfo* verifyPosInfo;
		TPositionInfo* currPosInfo;
		
		for(TInt count = 0; count < verifyPosInfoArr.Count(); count++)
			{
			if(iTestCaseId ==1 || iTestCaseId==5) //Autonomous mode
				{
				verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[count]);
				currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[count]);
				if (!utils.Compare_PosInfo(*verifyPosInfo, *currPosInfo))
					{
					INFO_PRINTF1(_L("Failed test, Positions incorrect."));
					SetTestStepResult(EFail);
					}
				}
			else if(iTestCaseId==3 || iTestCaseId==4)
				{
				currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[count]);
				TPositionInfo *refPosition = new(ELeave) TPositionInfo();
				refPosition->SetPosition(iRefPos);
				if (!utils.Compare_PosInfo(*refPosition, *currPosInfo))
					{
					INFO_PRINTF1(_L("Failed test, Positions incorrect."));
					SetTestStepResult(EFail);
					}
				
				delete refPosition;
				}
			else	//Preferred Terminal mode
				{
				if(count==0)
					{
					currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[count]);
					TPositionInfo *refPosition = new(ELeave) TPositionInfo();
					refPosition->SetPosition(iRefPos);
					if (!utils.Compare_PosInfo(*refPosition, *currPosInfo))
						{
						INFO_PRINTF1(_L("Failed test, Positions incorrect."));
						SetTestStepResult(EFail);
						}
					
					delete refPosition;
					}
				else
					{
					verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
					currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[count]);
					if (!utils.Compare_PosInfo(*verifyPosInfo, *currPosInfo))
						{
						INFO_PRINTF1(_L("Failed test, Positions incorrect."));
						SetTestStepResult(EFail);
						}
					}
				}

			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed test, Positions incorrect."));
		SetTestStepResult(EFail);
		}
	}



/** NetSim callbacks given for a MoLr, which we invoke as a result of the notify position update.
*/
void CT_LbsMolrStep_NewClient::Connected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Connected();
	
	// Kick off first pos update.
	
	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;

	T_LbsUtils utils;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
	iPosInfo1 = new(ELeave) TPositionInfo();
	posInfoArr.Append(iPosInfo1);
	iPosInfo2 = new(ELeave) TPositionInfo();
	posInfoArr.Append(iPosInfo2);

	// Kick off pos update.
	iDoPosUpdate->StartL(*iPosInfo1);
	}


void CT_LbsMolrStep_NewClient::Disconnected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Disconnected();
	}

void CT_LbsMolrStep_NewClient::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	if (aData != KNullDesC)
		{
		INFO_PRINTF1(_L("Got unexpected - NetSim Notify Register Lcs MoLr - Event."));
		SetTestStepResult(EFail);
		}
	else
		INFO_PRINTF1(_L("Got - NetSim Notify Register Lcs MoLr - Callback Event."));

	if(iTestCaseId==5)
		{
		if(!iNetSim.SetResponseError(RLbsNetSimTest::KNetSimNetworkNotAvailable, ETrue))
			{
			INFO_PRINTF1(_L("CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL() Failed: Can't set NetSim's response error."));
			SetTestStepResult(EFail);
			}
		}
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);
	}


void CT_LbsMolrStep_NewClient::NotifyReleaseLcsMoLr(TInt aReason)
	{
	(void)aReason;

	INFO_PRINTF1(_L("Got - NetSim Notify Release Lcs MoLr - Callback Event."));
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);

	switch(iTestCaseId)
		{
		case 02:
			{
			// Kick off pos update.
			iDoPosUpdate->StartL(*iPosInfo2);
			}
			break;
			
		case 03:
			{
			if(iPosUpdateFlag)
				{
				//Close the connection first
				iDoPosUpdate->ClosePositioner();
				
				//Open new connection
				iDoPosUpdate->OpenPositioner();
				
				// Kick off pos update.
				iDoPosUpdate->StartL(*iPosInfo2);
				
				iPosUpdateFlag=EFalse;
				}
			else
				iState = EWaiting;
				
			}
			break;
		
		case 04:
			{
			if(iPosUpdateFlag)
				{
				//Close the connection first
				iDoPosUpdate->ClosePositioner();
				iDoPosUpdate->CloseServer();
				
				//Open new connection
				iDoPosUpdate->ConnectServer();
				iDoPosUpdate->OpenPositioner();
			
				// Kick off pos update.
				iDoPosUpdate->StartL(*iPosInfo2);
				iPosUpdateFlag=EFalse;
				}
			else
				iState = EWaiting;
			}
			break;
			
		default:
			break;
			
		}
	}


void CT_LbsMolrStep_NewClient::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;

	INFO_PRINTF1(_L("Got - NetSim Notify Measurement Control Location - Callback Event."));
	
	//if(iTestCaseId==5)
	//	{
	//	if(!iNetSim.SetResponseError(RLbsNetSimTest::KNetSimNetworkNotAvailable, ETrue))
	//		{
	//		INFO_PRINTF1(_L("CT_LbsClientStep_CellBasedNotifyPosUpdate::doTestStepL() Failed: Can't set NetSim's response error."));
	//		SetTestStepResult(EFail);
	//		}
	//	}
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);
	}


void CT_LbsMolrStep_NewClient::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	(void)aResult;

	INFO_PRINTF1(_L("Got - NetSim Notify Release Lcs Location Notification - Callback Event."));
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
	}


void CT_LbsMolrStep_NewClient::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	(void)aReason;
	(void)aPosition;

	INFO_PRINTF1(_L("Got - NetSim Notify Facility Lcs MoLr Result - Callback Event."));
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
	}


void CT_LbsMolrStep_NewClient::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{
	(void)aPosition;

	INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report - Callback Event."));
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);
	}


void CT_LbsMolrStep_NewClient::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report Request More Assistance Data - Callback Event."));
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
	}


void CT_LbsMolrStep_NewClient::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	(void)aReason;

	INFO_PRINTF2(_L("Got - Net Sim Notify Measurement Report Control Failure - Callback Event. Reason = %d"), aReason);
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
	}


/**	Notify position update callback.
	The notify position update has completed.
*/
void CT_LbsMolrStep_NewClient::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("Got - Client Notify Update Complete - Callback Event."));
	

	if(iTestCaseId==3 || iTestCaseId==4)
		{
		if(KErrNone != aStatus.Int())
			{
			INFO_PRINTF1(_L("<FONT><B>Request should have returned KErrNone</B></FONT>"));
			SetTestStepResult(EFail);
			}
		return;
		}

	if(iPosUpdateFlag)
		{
		switch(iTestCaseId)
			{
			case 01:
			case 06:
				{
				if(KErrNone != aStatus.Int())
					{
					INFO_PRINTF1(_L("aStatus is not KErrNone"));
					SetTestStepResult(EFail);
					}
					
				iDoPosUpdate->StartL(*iPosInfo2);
				}
				break;

			case 05:
				{
				if(KErrTimedOut != aStatus.Int() && KErrNone != aStatus.Int())
					{
					INFO_PRINTF1(_L("<FONT><B>Request should have returned KErrTimedOut</B></FONT>"));
					SetTestStepResult(EFail);	
					}
				else				
					iDoPosUpdate->StartL(*iPosInfo2);
				}
				break;
			
			default:
				{
				if(KErrNone != aStatus.Int())
					{
					INFO_PRINTF1(_L("aStatus is not KErrNone"));
					SetTestStepResult(EFail);
					}
				}
				break;
			}

		iPosUpdateFlag = EFalse;
		}
	else
		iState = EWaiting;
	}	
