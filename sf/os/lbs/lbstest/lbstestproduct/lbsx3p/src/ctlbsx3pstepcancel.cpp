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
// @file ctlbsx3pstepcancel.cpp
// This is the class implementation for the Module Information Tests
// EPOC includes.
// 
//

// LBS includes. 
#include <lbs.h>
#include <lbs/lbsx3p.h>

// LBS test includes.
#include <lbs/test/lbsnetsimtest.h>
#include "ctlbsx3pstepcancel.h"
#include <lbs/test/tlbsutils.h>

//const TTimeIntervalMicroSeconds32 KExtendedTimerInterval = 60*1000000;	// 1 minute
//const TInt KLbsTestModuleTimeOut = 50*1000000;
//const TTimeIntervalMicroSeconds KLbsX3PTimeOut = 20*1000000;

//const TInt KSimpleAssistanceDataProviderPluginUidValue = 0x10281D77;
//const TInt KSuplAssistanceDataProviderPluginUidValue = 0x1028225B;
//_LIT(KLbsRefPos, "RefPos");


//TBool bConnected = EFalse; //To make sure that connected wouldnt get called twice - TBD
/**
 * Destructor
 */
CT_LbsX3PStep_Cancel::~CT_LbsX3PStep_Cancel()
	{	
	if(iDoTransmitPosAO != NULL)
			delete iDoTransmitPosAO;
	
	if(iDoTransmitPosAO2 != NULL)
			delete iDoTransmitPosAO2;
	}

/**
 * Constructor
 */
CT_LbsX3PStep_Cancel::CT_LbsX3PStep_Cancel(CT_LbsX3PServer& aParent) : CT_LbsX3PStep(aParent)
	{
	SetTestStepName(KLbsX3PStep_Cancel);
	//iCallbackFlags = 0;
	}


/**
Static Constructor
*/
CT_LbsX3PStep_Cancel* CT_LbsX3PStep_Cancel::New(CT_LbsX3PServer& aParent)
	{
	CT_LbsX3PStep_Cancel* testStep = new CT_LbsX3PStep_Cancel(aParent);
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

void CT_LbsX3PStep_Cancel::ConstructL()
	{
		//Call the base class c'tor to create timer.
		CT_LbsX3PStep::ConstructL();
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsX3PStep_Cancel::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep_Cancel::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		
		TInt err = KErrNone;
		RLbsTransmitPositionServer server;

		// Carry out common test actions (such as connecting to a server).

		User::LeaveIfError(server.Connect());
		CleanupClosePushL(server);

		INFO_PRINTF1(_L("server opened"));
	
		User::LeaveIfError(iTransmitPositioner.Open(server));
		CleanupClosePushL(iTransmitPositioner);

		// Create the active object to carry out the Transmit Position functionality.					
		iDoTransmitPosAO = CT_LbsX3PDoTransmitPos::NewL(this, iTransmitPositioner);
		
		//T_LbsUtils utils;
		//TModuleDataIn modDataIn; // Used to send test information to the test module.

	    iRefPosFlag = GetIntFromConfig(ConfigSection(), KLbsRefPos, iRefPosFlag);
	    

		// Carry out unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				// Test case LBS-X3P-Cancel-0001
				case 1:
					{
					//Try to cancel Transmition without X3P
					iTransmitPositioner.CancelTransmitPosition();  

					//Below code might not be needed...added as Netsim panics if I dont start scheduler
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect;
					
					}
					break;
					
				// Test case LBS-X3P-Cancel-0002
				case 2:
					{
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect| 
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr |
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0)|
									 KLbsCallback_Got_NotifyDoTransmitPos;
					}
					break;
					
				// Test case LBS-X3P-Cancel-0003
				case 3:
					{
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr |
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0)|
									 KLbsCallback_Got_NotifyDoTransmitPos;
					}
					break;
					
				// Test case LBS-X3P-Cancel-0004
				case 4:
					{
					//SetExtendedTimerInterval(30000000);
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									 KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation |
									 KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr|
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0)|
									 KLbsCallback_Got_NotifyDoTransmitPos;
					}
					break;
					
				// Test case LBS-X3P-Cancel-0005
				case 5:
				case 6:
					{
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									 KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation |
									 KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr|
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0)|
									 KLbsCallback_Got_NotifyDoTransmitPos;
					}
					break;
					
													
				// Test case LBS-X3P-Cancel-0007
				case 7:
					{
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr |
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0);

					//Not sure how we will get status in this case when the request is cancelled from Cancel() - TBD
					}
					break;

				
				// Test case LBS-X3P-Cancel-0009-0010
				case 8:
				case 9:
					{
					User::LeaveIfError(iTransmitPositioner2.Open(server));
					CleanupClosePushL(iTransmitPositioner2);

					// Create the active object to carry out the Transmit Position functionality.					
					iDoTransmitPosAO2 = CT_LbsX3PDoTransmitPos::NewL(this, iTransmitPositioner2);

					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									 KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation |								 
									 KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr|
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0)|
									 KLbsCallback_Got_NotifyDoTransmitPos;
					}
					break;


				case 10:
					{
					//Cancel at NotifyMeasurementReportRequestMoreAssitanceData

					//Configure gps-test-module to request for more assitance data.
					if(iParent.iSharedData->iTestModuleInUse)
						{
						T_LbsUtils utils;
						TModuleDataIn modDataIn; // Used to send test information to the test module.
						modDataIn.iRequestType = TModuleDataIn::EModuleRequestTestMode;
						modDataIn.iAssDataEventType = TModuleDataIn::EAssDataEventExpectSingle;
						modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_SomeDataNotAvailable;
						
						utils.NotifyModuleOfConfigChangeL(modDataIn);
						}
						
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData |
									 KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure |
									 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr|
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0)|
									 KLbsCallback_Got_NotifyDoTransmitPos;
					}
					break;					
					
				case 11:
					{
					iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
									 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
									 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									 KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation |
									 KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult |
									 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr|
									 (iRefPosFlag? KLbsCallback_Got_NotifyDoTransmitReferencePos:0)|
									 KLbsCallback_Got_NotifyDoTransmitPos;
					}
					break;

				case 12:
					{
					
					//Note sure which callbacks would be needed - TBD
					if(iRefPosFlag)
						{
						iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
										 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
										 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
										 KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation |
										 KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult |
										 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr|
										 KLbsCallback_Got_NotifyDoTransmitPos;
						}
					else
						{
						INFO_PRINTF2(_L("This test case is invalid for this interface: Testcase ID-  %d"), iTestCaseId);
						ASSERT(FALSE);	
						}
					}
					break;

				case 13:
					{
					if(iRefPosFlag)
						{
						iFlagsToHaltOn = KLbsCallback_NetSim_Got_Connect | 
										 KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr |
										 KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
										 KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure |
										 KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr|
										 KLbsCallback_Got_NotifyDoTransmitReferencePos|
										 KLbsCallback_Got_NotifyDoTransmitPos;
						}
					else
						{
						INFO_PRINTF2(_L("This test case is invalid for this interface: Testcase ID-  %d"), iTestCaseId);
						ASSERT(FALSE);	
						}
					}
					break;

				default:
					{
					INFO_PRINTF2(_L("Unrecognised test case id %d"), iTestCaseId);
					ASSERT(FALSE);	
					}					
				}
			}
		else
			{
			INFO_PRINTF1(_L("Error in reading config file"));
			ASSERT(FALSE);
			}

		RPointerArray<TAny>& srcPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		TPositionInfo* srcPosInfo = reinterpret_cast<TPositionInfo*>(srcPosInfoArr[0]);
		TPosition srcPos;
		srcPosInfo->GetPosition(srcPos);

		// Conect to the NetSim.
		iNetSim.ConnectL(this); // Once we have connected, we will get NetSim callbacks.
		//iNetSim.SetStepMode(ETrue);

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

		if (!iNetSim.SetReferenceLocation(srcPos))
			{
				INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
				SetTestStepResult(EFail);
				iNetSim.Close();
				return TestStepResult();
			}	

		// Set the MaxFix time required by the network.
		TTimeIntervalMicroSeconds maxFixTime(150*1000000);
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
		
		//Timeout for Transmit Location
		TLbsTransmitPositionOptions transmitOptions;
		transmitOptions.SetTimeOut(120*1000000);
		iTransmitPositioner.SetTransmitOptions(transmitOptions);

		// Kick off the keep alive timer.
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod);
		iKeepAliveTimer->SetTimer(interval);

		if(iTestCaseId == 3)
			{
			const TInt32 time=1000;
			iNetSim.SetResponseTime(time, ETrue);  //TBD
			}
		
		CActiveScheduler::Start();
		
		err = iStatus.Int();
		switch(iTestCaseId)
			{
			case 1:
			case 7:
				break;
				
			case 2:
			case 3:
			case 10:
			case 13:
				{
				if(KErrCancel != err)
					{
					INFO_PRINTF2(_L("<FONT><B>CancelTransmitPosition() should have returned KErrCancel, got %d</B></FONT>"), err);
					SetTestStepResult(EFail);	
					}
				}
				break;
			
			case 4:
				{
				if(KErrCancel == err && !(iCallbackFlags & KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure))
					{
					INFO_PRINTF2(_L("<FONT><B>Should have been completed with KErrCancel,  got %d</B></FONT>"), err);
					SetTestStepResult(EFail);	
					}
				}
				//not using break as want to go ahead with below check.
				
			case 5:
			case 6:				
			case 11:
			case 12: //TBD
     			// In testcases that cancel "late" in the X3P proceudure, cancellation may arrive to NRH before
				// or after the session has finished. Therefore the error code will sometimes be
				// KErrCancel (session cancelled) and other times KErrNone (nothing to cancel due to session complete).
				//
				{
				if(!(KErrCancel == err || KErrNone == err))
					{
					INFO_PRINTF2(_L("<FONT><B>CancelTransmitPosition() should have returned KErrCancel or KErrNone, got %d</B></FONT>"), err);
					SetTestStepResult(EFail);	
					}
				}
			break;
			
			case 8:
			case 9:
				{
				if(KErrNone != err)
					{
					INFO_PRINTF2(_L("<FONT><B>CancelTransmitPosition() should have returned KErrNone, got %d</B></FONT>"), err);
					SetTestStepResult(EFail);	
					}
				}
				break;
			
			default:
				{
				INFO_PRINTF2(_L("Unrecognised test case id %d"), iTestCaseId);
				ASSERT(FALSE);	
				}	
			}

		// Clean up.
		// Clear A-GPS timeout.
		//if(iParent.iSharedData->iTestModuleInUse)
		//	{
		//	modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
		//	modDataIn.iTimeOut = 0;
		//	utils.NotifyModuleOfConfigChangeL(modDataIn);
		//	}

		iNetSim.ClearAssistanceDataFilters();
		iNetSim.Close();  

		if(iTestCaseId == 8 || iTestCaseId == 9)
			{
			CleanupStack::PopAndDestroy(&iTransmitPositioner2);					
			}
		//CleanupStack::PopAndDestroy(iKeepAliveTimer);
		CleanupStack::PopAndDestroy(&iTransmitPositioner);					
		CleanupStack::PopAndDestroy(&server);	
		
		
		if(iTestCaseId==13) //Wait while all activities in the PM stop and not interfere with the remaining tests 
			{
			User::After(5000000);
			}
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsX3PStep_Cancel::doTestStepL()"));

	return TestStepResult();
	}

void CT_LbsX3PStep_Cancel::MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo)
	{
	//(void)aStatus;
	(void)aRefPosInfo;
	
	INFO_PRINTF2(_L("Got - Transmit Position Reference Postion - Callback Event:: Status=%d"), aStatus.Int());

	//if(iTestCaseId !=4)
		{
		SetCallbackFlag(KLbsCallback_Got_NotifyDoTransmitReferencePos);	
		}
	
	switch(iTestCaseId)
		{
		case 2:
		case 3:
		case 7:
			{
			if(aStatus != KErrCancel)
				{
				INFO_PRINTF1(_L("<FONT><B> MT_LbsX3PDoRefPosCallback should have been completed with KErrCancel</B></FONT>"));
				SetTestStepResult(EFail);	
				}
			}
			break;

		case 4:
			{
			if(aStatus != KErrCancel && aStatus != KErrNone)
				{
				INFO_PRINTF1(_L("<FONT><B> MT_LbsX3PDoRefPosCallback should have been completed with KErrCancel</B></FONT>"));
				SetTestStepResult(EFail);	
				}
			}
			break;
			
		case 13:
			{
			iTransmitPositioner.CancelTransmitPosition();
			}
			break;
			
		default:
			break;
		}
	}


/**	Notify position update callback.
	The notify position update as completed. We can mark as done in the callback flags.
**/

void CT_LbsX3PStep_Cancel::MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos* aDoTransmitPosAO)
	{
	(void)aDoTransmitPosAO;
	INFO_PRINTF1(_L("Got - Transmit Position - Callback Event."));

	//if(iTestCaseId == 18)
	//	{
	//	if(!(iCallbackFlags & KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation))
	//		{
	//		INFO_PRINTF1(_L("<FONT><B> TransmitPostion completed before </B></FONT>"));
	//		SetTestStepResult(EFail);	
	//		}
	//	}

	iStatus = aStatus;
	SetCallbackFlag(KLbsCallback_Got_NotifyDoTransmitPos);
	}	

void CT_LbsX3PStep_Cancel::Connected()
	{
//	if(bConnected)
//		return;
	
//	bConnected = ETrue;	//probably a hack
	
	CT_LbsNetSimStep::Connected();
	
	_LIT(KDestID, "07703125837");

	TUint priority = X3P_PRIORITY_PUSH;
	TPositionInfo* posInfo = NULL;;
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	posInfo = new(ELeave) TPositionInfo();
	
	T_LbsUtils utils;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
	posInfoArr.Append(posInfo);

	switch(iTestCaseId)
		{
		case 1:
			//Do not start Transmit Position
			break;
		case 2:
			{
			// Request X3P
			if(iRefPosFlag)
				iDoTransmitPosAO->TransmitPositionReportingRefLoc( KDestID, priority, *posInfo);
			else
				iDoTransmitPosAO->TransmitPosition( KDestID, priority, *posInfo);
			
			//Is it good idea to wait for some time before starting cancel ....TBD
			iTransmitPositioner.CancelTransmitPosition(); 
			
			break;
			}
		case 3:
		case 4:
		case 5:
		case 6:
		case 10:
		case 11:
			{
			if(iRefPosFlag)
				iDoTransmitPosAO->TransmitPositionReportingRefLoc( KDestID, priority, *posInfo);
			else
				iDoTransmitPosAO->TransmitPosition( KDestID, priority, *posInfo);
			}
			break;

		case 7:
			{
			// Request X3P
			if(iRefPosFlag)
				iDoTransmitPosAO->TransmitPositionReportingRefLoc( KDestID, priority, *posInfo);
			else
				iDoTransmitPosAO->TransmitPosition( KDestID, priority, *posInfo);

			//Cancel the active object instead of calling CancelTransmitPosition()
			iDoTransmitPosAO->Cancel(); 
			}
			break;
			
		case 8:
			{
		
			if(iRefPosFlag)
				iDoTransmitPosAO->TransmitPositionReportingRefLoc( KDestID, priority, *posInfo);
			else
				iDoTransmitPosAO->TransmitPosition( KDestID, priority, *posInfo);
			
			//CancelTransmitPosition from different object
			iTransmitPositioner2.CancelTransmitPosition(); 
						
			}
			break;
			
		case 9:
			{
		
			if(iRefPosFlag)
				iDoTransmitPosAO->TransmitPositionReportingRefLoc( KDestID, priority, *posInfo);
			else
				iDoTransmitPosAO->TransmitPosition( KDestID, priority, *posInfo);
			
			//Cancel() the other TransmitPostion Active Object
			iDoTransmitPosAO2->Cancel(); 
			}
			break;
		
		case 12:
		case 13:
			{
			iDoTransmitPosAO->TransmitPositionReportingRefLoc( KDestID, priority, *posInfo);
			}
			break;
					
		default:
			break;
		}
	}

void CT_LbsX3PStep_Cancel::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	CT_LbsNetSimStep::NotifyRegisterLcsMoLr(aData);
	
	if(iTestCaseId==3)
		{
		iTransmitPositioner.CancelTransmitPosition();
		}
	else if(iTestCaseId == 12)
		{
		iDoTransmitPosAO->GetRefLocWaiter()->Cancel();
		}
	
	//iNetSim.Next(KErrNone);
	}

void CT_LbsX3PStep_Cancel::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
													  const RLbsAssistanceDataBuilderSet& aData, 
													  const TLbsNetPosRequestQuality& aQuality)
	{
	CT_LbsNetSimStep::NotifyMeasurementControlLocation(aPosition, aData, aQuality);
	
	//When we cancel the TransmitLocation request, it may happen that the ProcessAssistanceData request
	//from a PM to the NG is sent before the cancel takes effect.
	//If it happens we need to add KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation to iFlagsToHaltOn
	//to avoid the test hanging.
	if(iTestCaseId==2 || iTestCaseId==7)
		{
		iFlagsToHaltOn |= KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation;
		}
	
	if(iTestCaseId==4)
		{
		iTransmitPositioner.CancelTransmitPosition();
		}
	//iNetSim.Next(KErrNone);
	}

void CT_LbsX3PStep_Cancel::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{
	CT_LbsNetSimStep::NotifyMeasurementReportLocation(aPosition);
	
	if(iTestCaseId==5)
		{
		iTransmitPositioner.CancelTransmitPosition();
		}
	
	//iNetSim.Next(KErrNone);
	}

void CT_LbsX3PStep_Cancel::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);
	switch(iTestCaseId)
		{
		case 4:
		//case 5:
		//case 11:
			{
			iFlagsToHaltOn &=  ~KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation;
			iFlagsToHaltOn &=  ~KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult;
			iFlagsToHaltOn |=	KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure;

			if(aReason != KErrCancel)
				{
				INFO_PRINTF1(_L("<FONT><B>Should have returned KErrCancel</B></FONT>"));
				SetTestStepResult(EFail);	
				}
			}
			break;
		case 2:
		case 7:	
			iFlagsToHaltOn |=	KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure;
			//break is ommited intentionally here
		case 10:
		case 13:
			{
			if(aReason != KErrCancel)
				{
				INFO_PRINTF1(_L("<FONT><B>Should have returned KErrCancel</B></FONT>"));
				SetTestStepResult(EFail);	
				}
			}
			break;
		
		default:
			ASSERT(FALSE);
			break;
		}
	//iNetSim.Next(KErrNone);
	}


void CT_LbsX3PStep_Cancel::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
{

	CT_LbsNetSimStep::NotifyFacilityLcsMoLrResult(aReason,aPosition);

	switch(iTestCaseId)
	{
		case 11:
			iTransmitPositioner.CancelTransmitPosition();		
			break;
			
		default:
			break;
	}

    //iNetSim.Next(KErrNone);
}


void CT_LbsX3PStep_Cancel::NotifyReleaseLcsMoLr(TInt aReason)
	{
	CT_LbsNetSimStep::NotifyReleaseLcsMoLr(aReason);
	
	switch(iTestCaseId)
		{
		case 2:
		case 3:
		//case 4:
			{
			if(aReason != KErrCancel)
				{
				INFO_PRINTF1(_L("<FONT><B>Should have returned KErrCancel</B></FONT>"));
				SetTestStepResult(EFail);	
				}
			}
			break;
			
		case 6:
			// Add a very short delay here in order to ensure that the protocol module really has completed
			// all the actions relating to the x3p operation.
			User::After(100000);
			iTransmitPositioner.CancelTransmitPosition();	
			break;
		
		default:
			break;
		}
	//iNetSim.Next(KErrNone);
	}

void CT_LbsX3PStep_Cancel::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{

	switch (iTestCaseId)
		{
		case 10:
			{
			// Call base implementation.
			CT_LbsNetSimStep::NotifyMeasurementReportRequestMoreAssistanceData(aFilter);
			iTransmitPositioner.CancelTransmitPosition();	
			// Add a short delay here in order to ensure that LBS has completed
			// all the actions relating to the x3p cancel.			
			User::After(500000);
			}
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
	//iNetSim.Next(KErrNone);
	}
