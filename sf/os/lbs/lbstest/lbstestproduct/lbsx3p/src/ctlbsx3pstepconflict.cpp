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
// @file ctlbsx3pstepconflict.cpp
// This is the class implementation for the Module Information Tests
// EPOC includes.
// 
//

// LBS includes. 
#include <lbs.h>
#include <lbs/lbsnetcommon.h>
//#include <kernel.h>
// LBS test includes.
//#include <lbs/test/lbsnetsimtest.h>
#include "ctlbsx3pstepconflict.h"
#include <lbs/test/tlbsutils.h>

const TInt KLbsKeepAlivePeriod1=10*1000000; 

_LIT(KDestIDL, "07725547323");
_LIT(KDestIDH, "07703125837");
_LIT(KDestIDM, "07703103066");
const TUint priorityL = X3P_PRIORITY_TIMER;
const TUint priorityM = X3P_PRIORITY_MENU;
const TUint priorityH = X3P_PRIORITY_PUSH;
//TPositionInfo postitionInfo;
static TUint iterator=0;

/**
 * Destructor
 */
CT_LbsX3PStep_Conflict::~CT_LbsX3PStep_Conflict()
	{
	iDoPosUpdate->Cancel();
	delete iDoPosUpdate;
	if(iDoTransmitPosAO!=NULL)
		delete iDoTransmitPosAO;
	if(iDoTransmitPosAO2 != NULL)
		delete iDoTransmitPosAO2;
	
	iTransmitPositioner2.Close();
	iTransmitPositioner.Close();
	iServer.Close();					
	
	iTSArray.Close();
	}


/**
 * Constructor
 */
CT_LbsX3PStep_Conflict::CT_LbsX3PStep_Conflict(CT_LbsX3PServer& aParent) : CT_LbsX3PStep(aParent)
	{
	SetTestStepName(KLbsX3PStep_Conflict);
	}


/**
Static Constructor
*/
CT_LbsX3PStep_Conflict* CT_LbsX3PStep_Conflict::New(CT_LbsX3PServer& aParent)
	{
	CT_LbsX3PStep_Conflict* testStep = new CT_LbsX3PStep_Conflict(aParent);
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

void CT_LbsX3PStep_Conflict::ConstructL()
	{
		CT_LbsX3PStep::ConstructL();

		//The active object wrapper for the notify position update.
		iDoPosUpdate = CT_LbsDoPosUpdate::NewL(this);
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsX3PStep_Conflict::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep_Conflict::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
		//TInt err = KErrNone;
		//RLbsTransmitPositionServer server;

		//TInt testCaseId;
		TRequestStatus aStatus;
		TLbsTransmitPositionOptions tOptions;

		// Carry out common test actions (such as connecting to a server).
		iStateX3P = EX3POk;
		User::LeaveIfError(iServer.Connect());
		//CleanupClosePushL(server);

		INFO_PRINTF1(_L("server opened"));
	
		User::LeaveIfError(iTransmitPositioner.Open(iServer));
		//CleanupClosePushL(iTransmitPositioner);

		//second transmit positioner is used for test case 7-10
		User::LeaveIfError(iTransmitPositioner2.Open(iServer));
		//CleanupClosePushL(iTransmitPositioner2);

		iDoTransmitPosAO = CT_LbsX3PDoTransmitPos::NewL(this, iTransmitPositioner);
		iDoTransmitPosAO2 = CT_LbsX3PDoTransmitPos::NewL(this, iTransmitPositioner2);
		
		//define local variables to point current and verify arrays.
		RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		RPointerArray<TAny>& curPosInfoArr = iParent.iSharedData->iCurrentPosInfoArr;

		T_LbsUtils utils;
		RPointerArray<TTime> doTransmitPosTime;
		RPointerArray<TTime> notifyMRLocationTime;
		RPointerArray<TTime> doPosUpdateTime;
		RPointerArray<TTime> notifyMCLocationTime;
		RPointerArray<TTime> notifyReleaseLcsMoLrTime;
			
		TModuleDataIn modDataInTimeout; // Used to send test information to the test module.
		const TInt KLbsTestModuleTimeOut = 1000000;
		// The module request type - time out value.
		modDataInTimeout.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
		// Micro seconds time value to delay the return position update from the module.
		modDataInTimeout.iTimeOut = KLbsTestModuleTimeOut;

		TModuleDataIn modDataInAssDataOn;
		modDataInAssDataOn.iRequestType = TModuleDataIn::EModuleRequestTestMode;
		modDataInAssDataOn.iAssDataEventType = TModuleDataIn::EAssDataEventExpectSingle;
		modDataInAssDataOn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn;
		
		iNetSim.ConnectL(this); // Once we have connected, we will get NetSim callbacks.
		iNetSim.SetStepMode(ETrue); //To run NetSim in step mode

		// Create reference location:
		// Location to use.
		TPositionInfo* srcPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
		TPosition srcPos;
		srcPosInfo->GetPosition(srcPos);
	
		if (!iNetSim.SetReferenceLocation(srcPos))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return TestStepResult();
			}	
	
		// Set the position quality required by the network.
		TTimeIntervalMicroSeconds maxFixTime(20*1000000);
		TLbsNetPosRequestQuality netPosQuality;

		netPosQuality.SetMaxFixTime(maxFixTime);
		netPosQuality.SetMinHorizontalAccuracy(srcPos.HorizontalAccuracy());
		netPosQuality.SetMinVerticalAccuracy(srcPos.VerticalAccuracy());
		
		if (!iNetSim.SetQuality(netPosQuality))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's quality."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return TestStepResult();
			}

		// Set plugin to use.
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
			INFO_PRINTF1(_L("Failed test, can't set NetSim's assistance data plugin uid."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return TestStepResult();
			}

		//Timeout for Transmit Location
		TLbsTransmitPositionOptions transmitOptions;
		transmitOptions.SetTimeOut(20*1000000);  

		iTransmitPositioner.SetTransmitOptions(transmitOptions);

		//Reset iterator value
		iterator=0;
		
		// Kick off the keep alive timer.
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod1);
		iKeepAliveTimer->SetTimer(interval);

		// Carry out unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				// Test case LBS-X3P-Conflict-0001
				case 1:
					break;
					
				// Test case LBS-X3P-Conflict-0002
				case 2:
				case 3:
				case 4:
					{
					//Set assistence data mode flag to on so that MoLr gets all notifications.
					utils.NotifyModuleOfConfigChangeL(modDataInAssDataOn);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_Got_DoPosUpdate, doPosUpdateTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
					else
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);

					if(doTransmitPosTime.Count() !=1 ||doPosUpdateTime.Count() != 0  || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MoLr wasn't cancelled after getting X3P(Push)</B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;

				case 5:
				case 12:
					{
					//Set assistence data mode flag to on so that MoLr gets all notifications.
					utils.NotifyModuleOfConfigChangeL(modDataInAssDataOn);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_Got_DoPosUpdate, doPosUpdateTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						}

					if(doTransmitPosTime.Count() !=1 ||doPosUpdateTime.Count() != 1  ||doPosUpdateTime[0]->Int64() > doTransmitPosTime[0]->Int64() || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>X3P didn't complete before MoLr</B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;
					
					
				case 21:
				case 22:
				case 54: 	//Check that X3P(Push) completes after X3P(Menu) as it was too late to cancel X3P(Menu) by X3P(Push)
				case 62:	//Check
				case 102:
				case 94:  
					{
					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						}

					if(doTransmitPosTime.Count() !=2 || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>X3P(Push) wasn't completed before X3P(Timer)</B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;

					
				case 31:
				case 32:
				case 34:

				case 72:
				case 74:

				case 112:
				case 114:
					{
					//Set minimum response time for a-gps
					utils.NotifyModuleOfConfigChangeL(modDataInTimeout);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation, notifyMCLocationTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						}

					//Check that X3P(push) complets and MTLR doesn't start
					//There should be one transmit position flag 
					//There should be just one NotifyMeasurementControlLocation flag to make sure that MTLR is not even started
					if(doTransmitPosTime.Count() !=1 ||notifyMCLocationTime.Count() !=1 || ! bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;
					
				case 35:
				case 75:
				case 115:
					{
					//Set minimum response time for a-gps
					utils.NotifyModuleOfConfigChangeL(modDataInTimeout);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation, notifyMRLocationTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						}

					//Check that X3P(Push) completes followed by MTLR
					//There should be one transmit position flag
					//there should be two notifyMeasurementReportLocation flags
					//second notifymesurementreportlocation flag should come after transmitPostion flag
					if(doTransmitPosTime.Count() !=1 ||notifyMRLocationTime.Count() !=2 ||doTransmitPosTime[0]->Int64() > notifyMRLocationTime[1]->Int64() || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR didnt follow X3P(Push) </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;


				case 42:
				case 43:

				case 82:		//0082
				case 83: 	//0083

				case 122:	//0122
				case 123:	//0123

				case 1001:
					{
					//Set minimum response time for a-gps
					utils.NotifyModuleOfConfigChangeL(modDataInTimeout);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation, notifyMRLocationTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr, notifyReleaseLcsMoLrTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						}

					//Check that E-MTLR interrupts X3P(Push) and complets before X3P(Push)
					//There should be one transmit position flag
					//There should be two notifiyMeasurementReportLocation flags
					//The time stamp of second mesurementreportlocation flag is before transmit position flag.
					//There should be two releaseLcsMoLr flags
					if(doTransmitPosTime.Count() !=1 ||notifyMRLocationTime.Count() !=2 || notifyReleaseLcsMoLrTime.Count() !=2 ||
						doTransmitPosTime[0]->Int64() < notifyMRLocationTime[1]->Int64() || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;
					
				case 44:
				case 84:	//0084
				case 124:	//0124
					{
					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation, notifyMRLocationTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr, notifyReleaseLcsMoLrTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						}

					//Check that E-MTLR interrupts X3P(Push) and complets before X3P(Push)
					//There should be one transmit position flag
					//There should be three notifiyMeasurementReportLocation flags
					//The time stamp of third mesurementreportlocation flag is before transmit position flag.
					//There should be two releaseLcsMoLr flags
					if(doTransmitPosTime.Count() !=1 ||notifyMRLocationTime.Count() !=3 || notifyReleaseLcsMoLrTime.Count() !=2 ||
						doTransmitPosTime[0]->Int64() < notifyMRLocationTime[1]->Int64() || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;
					
				case 45:
				case 85:		//0085
				case 125:	//0125
					{
					//Set minimum response time for a-gps
					utils.NotifyModuleOfConfigChangeL(modDataInTimeout);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation, notifyMRLocationTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr, notifyReleaseLcsMoLrTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						}

					//Check that E-MTLR interrupts X3P(Push) and complets before X3P(Push)
					//There should be one transmit position flag
					//There should be two notifiyMeasurementReportLocation flags
					//The time stamp of second mesurementreportlocation flag is after transmit position flag.
					//There should be one releaseLcsMoLr flags
					if(doTransmitPosTime.Count() !=1 ||notifyMRLocationTime.Count() !=2 || notifyReleaseLcsMoLrTime.Count() !=1 ||
						doTransmitPosTime[0]->Int64() > notifyMRLocationTime[1]->Int64() || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;

					
				case 51:
					//TBD
					break;
					
				case 53:
					{
					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation, notifyMCLocationTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr, notifyReleaseLcsMoLrTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
					else
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);

					//Check that X3P(Push) cancels X3P(Menu) 
					//There should be one transmit position flag
					//There should be two notifiyMeasurementControlLocation flags
					//There should be two releaseLcsMoLr flags
					if(doTransmitPosTime.Count() !=1 ||notifyMCLocationTime.Count() !=2 || notifyReleaseLcsMoLrTime.Count() !=2 || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>X3P(Push) didn't cancel X3P(Menu) </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;
					
					
				case 61:
					break;
					
					
				case 71:
					break;
					
					
				case 81:	//0081
					break;
					
					
				case 93:	//0093
					{
					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation, notifyMCLocationTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr, notifyReleaseLcsMoLrTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						}

					//X3P(Push) should interupt X3P(Timer) and X3P(Timer) should complete after X3P(Push)
					//There should be two transmit position flags
					//There should be three release flags
					if(doTransmitPosTime.Count() !=2 ||notifyMCLocationTime.Count() !=3 || notifyReleaseLcsMoLrTime.Count() !=3 || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;
					
					
				case 111:
					break;
					
					
				case 121:	//0121
					break;
					

				case 133:
					{
					//Set minimum response time for a-gps
					utils.NotifyModuleOfConfigChangeL(modDataInTimeout);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation, notifyMCLocationTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
					else
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);

					//X3P(Push) should cancel MTLR when X3P(Push) called at 
					//There should be one transmit position flags
					//There should be one MeasurementControlLocation flag
					if(doTransmitPosTime.Count() !=1 ||notifyMCLocationTime.Count() !=1 || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;

				case 134:
					{
					//Set minimum response time for a-gps
					utils.NotifyModuleOfConfigChangeL(modDataInTimeout);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation, notifyMRLocationTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						}

					//X3P(Push) should cancel MTLR when X3P(Push) called at 
					//There should be one transmit position flags
					//There should be one MeasurementReportLocation flag
					if(doTransmitPosTime.Count() !=1 ||notifyMRLocationTime.Count() !=1 || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;

				case 135:	//0135

				case 143:
				case 144:
					{
					//Set minimum response time for a-gps
					utils.NotifyModuleOfConfigChangeL(modDataInTimeout);

					// Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation, notifyMRLocationTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						}

					//X3P(Push) should complete after MTLR as its too late to cancel MTLR. 
					//There should be one transmit position flags
					//There should be two MeasurementReportLocation flag
					//The time stamp of Transmit position should be after second MeasurementReportLocation flag
					if(doTransmitPosTime.Count() !=1 ||notifyMRLocationTime.Count() !=2 || 
						doTransmitPosTime[0]->Int64() < notifyMRLocationTime[1]->Int64() || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;


				case 1002:
					{
					//Set minimum response time for a-gps
					utils.NotifyModuleOfConfigChangeL(modDataInTimeout);

					//Kick off test.
					CActiveScheduler::Start();

					AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
					AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation, notifyMRLocationTime);

					TBool bResult;
					if(iParent.iSharedData->iTestModuleInUse)
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[2])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::EExactAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[2])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::EExactAccuracy);
						}
					else
						{
						bResult = utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[2])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[1])), T_LbsUtils::ERoughAccuracy);
						bResult &= utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[2])), *(reinterpret_cast<TPositionInfo*>(curPosInfoArr[0])), T_LbsUtils::ERoughAccuracy);
						}

					//X3P(Push) should cancel MTLR when X3P(Push) called at 
					//There should be one transmit position flags
					//There should be one MeasurementReportLocation flag
					if(doTransmitPosTime.Count() !=2 ||notifyMRLocationTime.Count() !=3 ||
						doTransmitPosTime[0]->Int64() < notifyMRLocationTime[1]->Int64() || !bResult)
						{
						INFO_PRINTF1(_L("<FONT><B>MTLR wasn't get cancelled </B></FONT>"));
						SetTestStepResult(EFail);	
						}
					}
					break;

				case 1003:
					//TBD
					ASSERT(FALSE);
					break;

				case 1004:
					//TBD
					ASSERT(FALSE);
					break;


				default:
					{
					User::Panic(KLbsX3PStep_Conflict, KErrUnknown);
					}					
				}
			}
		// Clean up.
		iNetSim.Close();
		//CleanupStack::PopAndDestroy(&iTransmitPositioner2);					
		//CleanupStack::PopAndDestroy(&iTransmitPositioner);					
		//CleanupStack::PopAndDestroy(&server);	
		}


	INFO_PRINTF1(_L("&lt;&lt;CT_LbsX3PStep_Conflict::doTestStepL()"));

	return TestStepResult();
	}

void CT_LbsX3PStep_Conflict::MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo)
	{
	(void)aStatus;
	(void)aRefPosInfo;
	}



/**	
	Transmit Position callback.
**/

void CT_LbsX3PStep_Conflict::MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos* aDoTransmitPosAO)
	{
	INFO_PRINTF1(_L("Got - Transmit Position - Callback Event."));

	//We can check callback completion order here in the case of two X3Ps

	switch(iTestCaseId)
		{
		case 22:
		case 54:
		case 62:
		case 94:
		case 102:
			{
			if(iterator==0 && aDoTransmitPosAO != iDoTransmitPosAO)
				iStateX3P=EX3PErrorReported;
			else if(iterator==1 && aDoTransmitPosAO != iDoTransmitPosAO2)
				iStateX3P=EX3PErrorReported;
			else if(aStatus.Int() != KErrNone)
				iStateX3P=EX3PErrorReported;
			}
			break;

		case 53:
			{
			if(iterator==0 && aStatus.Int() != KErrCancel)
				iStateX3P=EX3PErrorReported;
			else if(iterator==1 && aDoTransmitPosAO != iDoTransmitPosAO2)
				iStateX3P=EX3PErrorReported;
			else if(aStatus.Int() != KErrNone)
				iStateX3P=EX3PErrorReported;
			}
			break;

		case 93:  //Might have to modify as Timer and Menu priorities are same in the case of MOAP
			{
			if(iterator==0 && aDoTransmitPosAO != iDoTransmitPosAO && aStatus.Int() != KErrCancel)
				iStateX3P=EX3PErrorReported;
			else if(iterator==1 && aDoTransmitPosAO != iDoTransmitPosAO2)
				iStateX3P=EX3PErrorReported;
			else if(iterator==2 && aDoTransmitPosAO != iDoTransmitPosAO)
				iStateX3P=EX3PErrorReported;
			else if(aStatus.Int() != KErrNone)
				iStateX3P=EX3PErrorReported;
			}
			break;

		//For the following cases, the error code should never be cancel or anything else...
		case 32:
		case 34:
		case 35:
		case 45:

		case 72:
		case 74:
		case 75:
		case 85:

		case 112:
		case 114:
		case 115:
		case 125:

		case 133:
		case 134:
		case 135:
		case 143:
		case 144:
			{
			if(aStatus.Int() != KErrNone)
				iStateX3P=EX3PErrorReported;
			}
			break;
			
		case 42:
		case 43:
		case 44:

		case 82:
		case 83:
		case 84:

		case 122:
		case 123:
		case 124:
			{
			if(iterator==0 && aStatus.Int() != KErrCancel)
				iStateX3P=EX3PErrorReported;
			else if(aStatus.Int() != KErrNone)
				iStateX3P=EX3PErrorReported;
			}
			break;
			
		default:
			break;
		}

	if(iStateX3P == EX3PErrorReported)
		{
		INFO_PRINTF1(_L("Error in X3P Completion"));
		SetTestStepResult(EFail);
		}
	
	SetCallbackFlag(KLbsCallback_Got_NotifyDoTransmitPos);
	}	

void CT_LbsX3PStep_Conflict::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("Got - Notify Update - Callback Event."));

	switch(iTestCaseId)
		{
		case 2:
		case 3:
		case 4:
			if(aStatus.Int() != KErrCancel)
				iStateX3P = EX3PErrorReported;
			break;

		case 5:
		case 12:
			if(aStatus.Int() != KErrNone || !TestCallbackFlags(KLbsCallback_Got_NotifyDoTransmitPos))
				iStateX3P = EX3PErrorReported;

			iState = EWaiting;
			break;

		default:
			iStateX3P = EX3PErrorReported;
		}

	if(iStateX3P==EX3PErrorReported)
		{
		INFO_PRINTF1(_L("Failed in Notify Update"));
		SetTestStepResult(EFail);
		}

	SetCallbackFlag(KLbsCallback_Got_DoPosUpdate);
	}	
	
/** Keep alive timer callback.
	Check the state machine to determine when to halt the test. Once all the callbacks
	have been received the test can end.
	This function should just monitor iState and nothing else releated to the test.
**/

void CT_LbsX3PStep_Conflict::HandleTimerL(TInt aTimerId, const TTime& aTargetTime)
	{
	(void)aTimerId;
	(void)aTargetTime;

	INFO_PRINTF1(_L("Got - Keep Alive Timer - Callback Event."));

	RPointerArray<TTime> doTransmitPosTime;
	RPointerArray<TTime> releaseLcsMoLrTime;
	RPointerArray<TTime> notifyMRLocationTime;

	AnalyseCallbackFlags( KLbsCallback_Got_NotifyDoTransmitPos, doTransmitPosTime);
	AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation, notifyMRLocationTime);
	AnalyseCallbackFlags( KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr, releaseLcsMoLrTime);
	
	//Setting termination condition
	switch(iTestCaseId)
		{
		case 2:
		case 3:
		case 4:
			{
			if(doTransmitPosTime.Count()==1)
				iState = EWaiting;
			}
			break;

		case 5:	//These cases are already handled in MT_LbsDoPosUpdateCallback() function
		case 12:
			break; 

		//If we got two Transmit positions, we can terminate
		case 22:
		case 53:
		case 54:
		case 62:
		case 94:	
		case 102:
			{
			if(doTransmitPosTime.Count()==2)
				iState = EWaiting;
			}
			break;

		case 93:	
			{
			if(doTransmitPosTime.Count()==3)
				iState = EWaiting;
			}
			break;

		//The second MTLR request should be cancelled so if we get one Transmit Position, we can terminate
		case 32:
		case 34:
		case 72:
		case 74:
		case 112:
		case 114:

		case 133:
		case 134:
			{
			if(doTransmitPosTime.Count()==1)
				iState = EWaiting;
			}
			break;

		//When X3P is interrupted  by (E)MTLR, if we get one transmit position and two ReleaseLcsMoLr, we can terminate
		case 42:
		case 43:
		case 44:
			
		case 82:		//0082
		case 83:		//0083
		case 84:		//0084
			
		case 122:	//0122
		case 123:	//0123
		case 124:	//0124
			{
			if(doTransmitPosTime.Count()==2 && releaseLcsMoLrTime.Count()==2)
				iState = EWaiting;
			}
			break;
		
		//When X3P is finished followed by (E)MTLR, if we get one transmit position and one ReleaseLcsMoLr, we can terminate
		case 35:
		case 45:
		case 75:
		case 85:		//0085
		case 115:
		case 125:	//0125
			{
			if(doTransmitPosTime.Count()==1 && releaseLcsMoLrTime.Count()==1)
				iState = EWaiting;
			}
			break;

		//If MTLR is finished followed by X3P, we should get one transmitposition and one Measurementcontrolreport to terminate
		case 135:	//0135
		case 143:
		case 144:
			{
			if(doTransmitPosTime.Count()==1 && notifyMRLocationTime.Count()==1)
				iState = EWaiting;
			}
			break;

		default:
			{
			INFO_PRINTF1(_L("Unknown Error"));
			SetTestStepResult(EFail);
			}
			break;
		}


	// We stop test from here when in the correct state and , there should not be anything outstanding.
	if (iStateX3P == EX3PErrorReported ||iState == EWaiting) 	//(iState != EDone && (iStateX3P == EX3PDone || iStateX3P == EX3PNormal) && (TestCallbackFlags(KLbsCallback_Got_NotifyDoTransmitPos) ||TestCallbackFlags(KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation))))
		{
		iState = EDone;
		iStateX3P = EX3POk;
		CActiveScheduler::Stop();
		}
	else    // Keep going, still waiting for callbacks.	
		{
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod1);
		iKeepAliveTimer->SetTimer(interval);
		}
	}

void CT_LbsX3PStep_Conflict::Connected()
	{
	INFO_PRINTF1(_L("Got - NetSim Connect - Callback Event."));

	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	iPosInfo0 = new(ELeave) TPositionInfo();
	iPosInfo1 = new(ELeave) TPositionInfo();
	
	T_LbsUtils utils;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
	posInfoArr.Append(iPosInfo0);
	posInfoArr.Append(iPosInfo1);

	switch(iTestCaseId)
		{

		case 2:
		case 3:
		case 4:
		case 5:

		case 12:
			{
			iDoPosUpdate->StartL(*iPosInfo0);
			}
			break;
			
		case 21:
		case 22:

		case 31:
		case 32:
		case 34:
		case 35:

		//case 0041:
		case 42:
		case 43:
		case 44:

		case 1001:
			{
			iDoTransmitPosAO->TransmitPosition( KDestIDH, priorityH, *iPosInfo0);

			if(iTestCaseId == 21)			
				iDoTransmitPosAO2->TransmitPosition( KDestIDL, priorityL, *iPosInfo1); 
			else if(iTestCaseId == 31 || iTestCaseId == 41)			
				{
				//Do we need to put some sleep - TBD
				//Ask Netsim to send MTLR
				TLbsNetPosRequestPrivacy reqType;
				reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
				reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
				TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

				if(iTestCaseId == 41)			
					iNetSim.SetEmergenyStatus(ETrue);
				
				iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
				}
			}
			break;

		//case 51:
		case 53:
		case 54:

		//case 61:
		case 62:

		//case 0071:
		case 72:
		case 74:
		case 75:

		//case 81:  	//0081 - I dont know why but compiler gives error if I write it case 0081 :(
		case 82:	//0082
		case 83:	//0083
		case 84:	//0084
			{
			iDoTransmitPosAO->TransmitPosition( KDestIDM, priorityM, *iPosInfo0);

			#if 0 //will have to look later
			if(iTestCaseId == 51)
				{
				iDoTransmitPosAO->TransmitPosition( KDestIDH, priorityH, *iPosInfo1);
				}
			else if(iTestCaseId == 61)
				{
				iDoTransmitPosAO->TransmitPosition( KDestIDL, priorityL, *iPosInfo1);
				}
			else if(iTestCaseId == 71 || iTestCaseId == 81)
				{
				//Do we need to put some sleep - TBD
				//Ask Netsim to send MTLR
				TLbsNetPosRequestPrivacy reqType;
				reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
				reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
				TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

				if(iTestCaseId == 81)			
					iNetSim.SetEmergenyStatus(ETrue);
				
				iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
				}
			#endif
			
			}
			break;

		case 93:	//0093
		case 94:	//0094

		case 102:

		//case 111:
		case 112:
		case 114:
		case 115:

		//case 121:	//0121 - Now compiler cries as constant used *more than once* if I write it as 0121 :(
		case 122:	//0122
		case 123:	//0123
		case 124:	//0124
			{
			iDoTransmitPosAO->TransmitPosition( KDestIDL, priorityL, *iPosInfo0);

			#if 0 //will have to look later
			if(iTestCaseId == 111 || iTestCaseId == 121)
				{
				//Do we need to put some sleep - TBD
				//Ask Netsim to send MTLR
				TLbsNetPosRequestPrivacy reqType;
				reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
				reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
				TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

				if(iTestCaseId == 121)			
					iNetSim.SetEmergenyStatus(ETrue);
				
				iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
				}
			#endif
			}
			break;
			
		case 133:
		case 134:
		case 135:	//0135

		case 143:
		case 144:
			{
				//Ask Netsim to send MTLR
				TLbsNetPosRequestPrivacy reqType;
				reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
				reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
				TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

				iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
			}
			break;

		case 1002:
			{
			//TPositionInfo* posInfo2;
			//posInfo2= new(ELeave) TPositionInfo();
			//posInfoArr.Append(posInfo2);

			iDoTransmitPosAO->TransmitPosition( KDestIDL, priorityL, *iPosInfo0);
			iDoTransmitPosAO2->TransmitPosition( KDestIDH, priorityH, *iPosInfo1);
			}
			break;
			
		default:
			iStateX3P = EX3PErrorReported;
			break;
		}

		SetCallbackFlag(KLbsCallback_NetSim_Got_Connect);
	}

void CT_LbsX3PStep_Conflict::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	(void)aResult;
	
	INFO_PRINTF1(_L("Got - NetSim NotifyReleaseLcsLocationNotification - Callback Event."));

	switch(iTestCaseId)
		{
		//case 0031:
		case 32:
		case 34:

		//case 0071:
		case 72:
		case 74:

		//case 0111:
		case 112:
		case 114:

		case 1001: //This case is network induced request (call StartNetworkLocationRequest())
			iStateX3P = EX3PErrorReported;
			break;

		case 35:
		case 75:
		case 115:
			{
			iNetSim.StartNetworkLocationRequest();
			break;
			}

		//case 0041:
		case 42:
		case 43:
		case 44:
		case 45:

		//case 81:	//0081
		case 82:		//0082
		case 83:		//0083
		case 84:		//0084
		case 85:		//0085

		//case 121:	//0121
		case 122:	//0122
		case 123:	//0123
		case 124:	//0124
		case 125:	//0125
			{
			//Do we need to set emergency flag here also for MTLR - TBD
			iNetSim.StartNetworkLocationRequest();
			break;
			}

		case 133:
			{
			iNetSim.StartNetworkLocationRequest();
			iDoTransmitPosAO->TransmitPosition( KDestIDH, priorityH, *iPosInfo1);
			}
			break;			

		case 134:
			{
			iNetSim.StartNetworkLocationRequest();
			iDoTransmitPosAO->TransmitPosition( KDestIDL, priorityL, *iPosInfo1);
			}
			break;			

		default:
			iStateX3P = EX3PErrorReported;
			break;
		}

	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyReleaseLcsLocationNotification);
	iNetSim.Next(KErrNone);
	}

	/**
	The response to an MoLr (FROM: Gateway)
	@param aPosition the position
	*/
void CT_LbsX3PStep_Conflict::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{
	INFO_PRINTF1(_L("Got - NetSim NotifyMeasurementReportLocation - Callback Event."));

	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	//TPositionInfo* verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	T_LbsUtils utils;

	switch(iTestCaseId)
		{
		case 2:
		case 3:
			
		case 53:
			{
			if(iterator==0)
				{
				iStateX3P=EX3PErrorReported;
				}
			if(iterator==1)
				{
				if(iParent.iSharedData->iTestModuleInUse)
					{
					//temp code -braj
					//TPositionInfo* pos = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1]);
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else
				iStateX3P=EX3PErrorReported;
			}
			break;

		case 4:
		case 54:
		case 94:	//0094
		case 135:

		case 5:
		case 12:
		case 22:
		case 62:
		case 102:
		case 35:
		case 45:
		case 75:
		case 85:		//0085
		case 115:
		case 125:	//0125

		case 143:
		case 144:
			{
			if(iterator==0)
				{
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}

				if(iTestCaseId==4 || iTestCaseId== 54 || iTestCaseId== 94 ||iTestCaseId== 135)
					iDoTransmitPosAO2->TransmitPosition( KDestIDH, priorityH, *iPosInfo1);
				}
			else if(iterator==1)
				{
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else
				iStateX3P=EX3PErrorReported;
			}
			break;

		//case 31:
		case 32:

		case 71:
		case 72:

		case 111:
		case 112:

		case 34:
		case 74:
		case 114:
			{
			if(iterator==0)
				{
				//compare position with the second entry in verify array.
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}

				if(iTestCaseId == 34 || iTestCaseId == 74 || iTestCaseId == 114)
					{
					//Ask Netsim to send MTLR
					TLbsNetPosRequestPrivacy reqType;
					reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
					reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
					TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

					iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
					}
				}
			else
				iStateX3P = EX3PErrorReported;
			}
			break;

		case 42:
		case 43:
		case 44:
		
		case 82:	//0082
		case 83:	//0083
		case 84:

		case 93:	//0093
			
		case 122:	//0122
		case 123:	//0123
		case 124:

		case 1001:
			{
			if(iterator==0)	
				{
				iStateX3P = EX3PErrorReported;
				}
			else if(iterator==1)
				{ 
				//This should return E-MTLR location
				//compare position with the second entry in verify array.
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else if(iterator==2)
				{
				//This should return X3P location
				//compare position with the second entry in verify array.
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else
				iStateX3P = EX3PErrorReported;
			}
			break;

		case 133:
		case 134:
			{
			if(iterator==0)	
				{
				//compare position with the second entry in verify array.
				//This should return X3P(Push) location as MTLR is cancelled
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else
				iStateX3P = EX3PErrorReported;
			}
			break;

		case 1002:
			{
			if(iterator==0)
				{ 
				//This should return E-MTLR location
				//compare position with the second entry in verify array.
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[2])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[2])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else if(iterator==1)
				{
				//This should return X3P location
				//compare position with the second entry in verify array.
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else if(iterator==2)
				{
				//This should return X3P location
				//compare position with the second entry in verify array.
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else
				iStateX3P = EX3PErrorReported;
			}
			break;
			
			
		default:
			break;
		}

	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation);

	if(iStateX3P==EX3PErrorReported)
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		}
	//Increment iterator in case of MTLR test cases. For only X3P conflicts, it should be incremented in NotifyReleaseLcsMoLr - TBD
	if(iTestCaseId==143 || iTestCaseId==144 || iTestCaseId==1002)
		iterator++;
	iNetSim.Next(KErrNone);
	}

void CT_LbsX3PStep_Conflict::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
													  const RLbsAssistanceDataBuilderSet& aData, 
													  const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	
	INFO_PRINTF1(_L("Got - NetSim NotifyMeasurementControlLocation - Callback Event."));

	switch(iTestCaseId)
		{
		case 3:
		case 93:	//0093
			{
			if(iterator==0)
				{
				INFO_PRINTF1(_L("Iterator = 0; calling TransmitPosition() from NotifyMeasurementControlLocation callback."));
				iDoTransmitPosAO2->TransmitPosition( KDestIDH, priorityH, *iPosInfo1);
				}

			break;
			}

		case 43:
		case 83:	//0083
		case 123:	//0123
			{
			if(iterator==0)
				{
				//Ask Netsim to send MTLR
				TLbsNetPosRequestPrivacy reqType;
				reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
				reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
				TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

				iNetSim.SetEmergenyStatus(ETrue);
				iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
				}
			}
			break;
		
		case 53:
		case 134:
			{
			if(iterator==0)
				{
				INFO_PRINTF1(_L("Iterator = 0; calling TransmitPosition() from NotifyMeasurementControlLocation callback."));
				iDoTransmitPosAO2->TransmitPosition( KDestIDH, priorityH, *iPosInfo1);
				}
			}
			break;

		case 144:
			{
			if(iterator==0)
				{
				INFO_PRINTF1(_L("Iterator = 0; calling TransmitPosition() from NotifyMeasurementControlLocation callback."));
				iDoTransmitPosAO2->TransmitPosition( KDestIDL, priorityL, *iPosInfo1);
				}
			}
			break;
			
		default:
			break;
		}

	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation);
	iNetSim.Next(KErrNone);
	}

void CT_LbsX3PStep_Conflict::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	INFO_PRINTF1(_L("Got - NetSim NotifyRegisterLcsMoLr - Callback Event."));

	switch(iTestCaseId)
		{
		case 2:
			{
			if(iterator==0)
				iDoTransmitPosAO->TransmitPosition( KDestIDH, priorityH, *iPosInfo1);
			else if(iterator==1)
				{
				if(aData.Compare(KDestIDH) !=0)
					iStateX3P = EX3PErrorReported;				
				}
			else
				iStateX3P = EX3PErrorReported;
			}
			break;

		case 3:
		case 4:
		case 5:
			{
			if(iterator == 1)
				{
				if(aData.Compare(KDestIDH) != 0)
					iStateX3P = EX3PErrorReported;				
				}
			else if (iterator == 2)
				{
				iStateX3P = EX3PErrorReported;
				}
			}
			break;
			
		case 12:
		case 22:

		case 62:

		case 102:
			{
			if(iterator==0)
				iDoTransmitPosAO2->TransmitPosition( KDestIDL, priorityL, *iPosInfo1);
			else if(iterator==1)
				{
				if(aData.Compare(KDestIDL) !=0)
					iStateX3P= EX3PErrorReported;
				}
			else
				iStateX3P = EX3PErrorReported;
			}
			break;
		
		case 32:
		case 42:
		case 72:
		case 82:	//0082
		case 112:
		case 122:	//0122
			{
			if(iterator==0)
				{
				//Ask Netsim to send MTLR
				TLbsNetPosRequestPrivacy reqType;
				reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
				reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
				TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

				if(iTestCaseId == 42 || iTestCaseId == 82 || iTestCaseId == 122)
					iNetSim.SetEmergenyStatus(ETrue);
				iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
				}
			else if(iterator==1 && (iTestCaseId == 32 || iTestCaseId == 42))
				{
				if(aData.Compare(KDestIDH) != 0)
					iStateX3P = EX3PErrorReported;				
				}
			else if(iterator==1 && (iTestCaseId == 72 || iTestCaseId == 82))
				{
				if(aData.Compare(KDestIDM) != 0)
					iStateX3P = EX3PErrorReported;				
				}
			else if(iterator==1 && (iTestCaseId == 112 || iTestCaseId == 122))
				{
				if(aData.Compare(KDestIDL) != 0)
					iStateX3P = EX3PErrorReported;				
				}
			else
				iStateX3P = EX3PErrorReported;
				
			}
			break;

		case 1001:
			{
			if(iterator==0)
				{
				//Ask Netsim to send network induced emergency request
				iNetSim.SetEmergenyStatus(ETrue);
				iNetSim.StartNetworkLocationRequest();
				}
			else if(iterator==1)
				{
				if(aData.Compare(KDestIDH) != 0)
					iStateX3P = EX3PErrorReported;				
				}
			else
				iStateX3P = EX3PErrorReported;
				
			}
			break;

		case 1002:
			{
			if(iterator==0)
				{
				//Ask Netsim to send network induced emergency request
				iNetSim.SetEmergenyStatus(ETrue);
				iNetSim.StartNetworkLocationRequest();
				}
			else if(iterator==1)
				{
				if(aData.Compare(KDestIDH) != 0)
					iStateX3P = EX3PErrorReported;				
				}
			else if(iterator==2)
				{
				if(aData.Compare(KDestIDL) != 0)
					iStateX3P = EX3PErrorReported;				
				}
			else
				iStateX3P = EX3PErrorReported;
				
			}
			
			
		default:
			break;

		}

	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr);
	iNetSim.Next(KErrNone);
	}

void CT_LbsX3PStep_Conflict::NotifyReleaseLcsMoLr(TInt aReason)
	{
	INFO_PRINTF1(_L("Got - NetSim NotifyReleaseLcsMoLr - Callback Event."));

	switch(iTestCaseId)
		{
		case 2:
		case 3:
		case 4:
			{
			if(iterator ==0)
				{
				if(aReason != KErrCancel)
					iStateX3P = EX3PErrorReported;
				}
			else
				{
				if(aReason != KErrNone)
					iStateX3P = EX3PErrorReported;
				}
			}
			break;

		case 35:
		case 75:
		case 115:
			{
			if(iterator==0)
				{
				//Ask Netsim to send MTLR
				TLbsNetPosRequestPrivacy reqType;
				reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
				reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
				TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

				iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
				}

			if(aReason!=KErrNone)
				iStateX3P = EX3PErrorReported;
			}
			break;
			
		case 45:
		case 85:		
		case 125:	
			{
			if(iterator==0)
				{
				//Ask Netsim to send MTLR
				TLbsNetPosRequestPrivacy reqType;
				reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); // Request will be accepted
				reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent); // No user involvement.
				TLbsExternalRequestInfo reqInfo; // Hope default values are good enough.

				iNetSim.SetEmergenyStatus(ETrue);
				iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
				}

			if(aReason!=KErrNone)
				iStateX3P = EX3PErrorReported;
			}
			break;

		default:
			if(aReason!=KErrNone)
				iStateX3P = EX3PErrorReported;
			break;
		}

	if(iTestCaseId != 143 && iTestCaseId != 144)
		iterator++;
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr);
	iNetSim.Next(KErrNone);
	}

void CT_LbsX3PStep_Conflict::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	(void)aReason;
	
	INFO_PRINTF1(_L("Got - NetSim NotifyMeasurementReportControlFailure - Callback Event."));

	switch(iTestCaseId)
		{
		case 3:
			break;

		default:
			//Panic
			iStateX3P = EX3PErrorReported;
			break;
		}
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure);
	iNetSim.Next(KErrNone);

	}

void CT_LbsX3PStep_Conflict::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	INFO_PRINTF1(_L("Got - NetSim NotifyFacilityLcsMoLrResult - Callback Event."));

	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	T_LbsUtils utils;
	
	switch(iTestCaseId)
		{
		case 5:
			if(iterator==0)
				{
				//the MoLr request would have been completed by now and position should be received.
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				
				iDoTransmitPosAO->TransmitPosition( KDestIDH, priorityH, *iPosInfo1);
				}
			else if(iterator==1)
				{
				if(iParent.iSharedData->iTestModuleInUse)
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::EExactAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				else
					{
					if (!utils.Compare_PosInfo(*(reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[1])), aPosition, T_LbsUtils::ERoughAccuracy))
						iStateX3P=EX3PErrorReported;
					}
				}
			else
				iStateX3P=EX3PErrorReported;

		default:
			break;
		}
	
	if(aReason != KErrNone || iStateX3P==EX3PErrorReported)
		{
		INFO_PRINTF1(_L("<FONT><B>NotifyFacilityLcsMoLrResult returned error</B></FONT>"));
		SetTestStepResult(EFail);	
		}
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult);
	iNetSim.Next(KErrNone);
	}

//	Below function might be needed later......
TInt64 CT_LbsX3PStep_Conflict::CallbackTime(TLbsCallbackFlags aCallbackFlag)
	{
  	for(TInt i=0; i<iTSArray.Count();++i)
		{
		if(iTSArray[i]->flag == aCallbackFlag)
			return iTSArray[i]->sysTime.Int64();
		}
	return -1;
	}

void CT_LbsX3PStep_Conflict::SetCallbackFlag(TLbsCallbackFlags aCallbackFlag)
	{
	//Set Timestamp
	TIME_STAMP *timeStamp = new TIME_STAMP;
	timeStamp->flag = aCallbackFlag;
	timeStamp->sysTime.UniversalTime();
	iTSArray.Append(timeStamp);

	//Call Baseclass function
	CT_LbsX3PStep::SetCallbackFlag(aCallbackFlag);
	}

void CT_LbsX3PStep_Conflict::AnalyseCallbackFlags(TLbsCallbackFlags aCallbackFlag, RPointerArray<TTime> & aCallbackTime)
	{
	for(int i=0; i<iTSArray.Count(); ++i)
		{
		if(iTSArray[i]->flag == aCallbackFlag)
			{
			aCallbackTime.Append(&(iTSArray[i]->sysTime));
			}
		}
	}

//End of used code

#if 0
void CT_LbsX3PStep_Conflict::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	//(void)aData;

	INFO_PRINTF1(_L("Got - NetSim NotifyRegisterLcsMoLr - Callback Event."));
	switch(iStateX3P)
		{
		case EX3PNormal:
			//Normal call...do nothing
		break;
			
		case EX3PPriorityInterrupt:
			{
			//Request X3P with high priority
			//set state to EX3PAny
			if(iState != EStartMolrTransmitInNotifyMeasurementControlLocation &&
				iState != EStartMolrTransmitInNotifyMeasurementReportLocation)
				{
				//Release the signalstatus so that the handler can call transmit position
				//TRequestStatus *stat = &iDummyAO->iStatus;
				//User::RequestComplete(stat, KErrNone);
				iDoTransmitPosAO->TransmitPosition( KDestIDH, priorityH, postitionInfo);
				}
			iStateX3P = EX3PAny;
			}
		break;

		case EX3PPriorityNormal:
			{
			//Release the signalstatus so that the handler can call transmit position
			//TRequestStatus *stat = &iDummyAO->iStatus;
			//User::RequestComplete(stat, KErrNone);
			iDoTransmitPosAO->TransmitPosition( KDestIDL, priorityL, postitionInfo);
			iStateX3P = EX3PPhaseOneOK;
			}
		break;
		
		case EX3PControlCancel:
			{
			//compare aData with high priority api destination data
			//if different, something is wrong - set state to EX3PErrorReported
			//else - set state to EX3PPhaseOneOK
			}
		break;
#if 0
		case ENormalNotifyDone:
			{
			//compare adata with second  priority api destination data
			//if different, something is wrong - set state to EErrorReported
			//else - set state to ECorrectTwo
			}
		break;
#endif		
		case EX3PNotifyReleaseDone:
			{
			//compare adata with low priority api destination data
			//if different, something is wrong - set state to EX3PErrorReported
			//else - set state to EX3PPhaseTwoOK
			if(aData.Compare(KDestIDL))
				iStateX3P= EX3PPhaseTwoOK;
			else
				iStateX3P = EX3PErrorReported;				
			}
		break;

		default:
			//Panic - set state to EX3PErrorReported
			iStateX3P = EX3PErrorReported;
		break;
		
		}
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr);
	}

void CT_LbsX3PStep_Conflict::NotifyReleaseLcsMoLr(TInt aReason)
	{
	(void)aReason;

	INFO_PRINTF1(_L("Got - NetSim NotifyReleaseLcsMoLr - Callback Event."));
	switch(iStateX3P)
		{
		case EX3PNormal:
			//Normal call - dont do anything
		break;
			
		case EX3PPhaseOneOK:
			{
				//change state to EX3PNotifyReleaseDone
				iStateX3P = EX3PNotifyReleaseDone;
			}
		break;
#if 0
		case ENormalCorrect:
			{
				//change state to ENormalNotifyDone
			}
		break;
#endif		
		case EX3PPhaseTwoOK:
			{
			iStateX3P = EX3PDone;
			//SetCallbackFlag(/*special flag for these test cases*/);
			}
		break;

		default:
			//Panic - set state to EX3PErrorReported
			iStateX3P = EX3PErrorReported;
			break;
		}
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr);

	}

void CT_LbsX3PStep_Conflict::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	(void)aReason;

	INFO_PRINTF1(_L("Got - NetSim NotifyMeasurementReportControlFailure - Callback Event."));
	switch(iStateX3P)
		{
		case EX3PNormal:
			//Normal call - dont do anything
		break;
			
		case EX3PAny:
			//Set state to EControlCancel:
			iStateX3P = EX3PControlCancel;
		break;

		default:
			//Panic
			iStateX3P = EX3PErrorReported;
			break;
		}
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure);

	}
#endif
 
#if 0
		switch(aCallbackFlag)
			{
			case KLbsCallback_NetSim_Got_Connect:
				break;

			case KLbsCallback_NetSim_Got_Disconnect:
				break;

			case KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr:
				break;

			case KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr:
				break;

			case KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation:
				break;

			case KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation:
				break;

			case KLbsCallback_NetSim_Got_NotifyReleaseLcsLocationNotification:
				break;

			case KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult:
				break;

			case KLbsCallback_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData:
				break;

			case KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure:
				break;


			case KLbsCallback_Got_NotifyDoTransmitPos:
				break;

			default:
				break;

			}
#endif


