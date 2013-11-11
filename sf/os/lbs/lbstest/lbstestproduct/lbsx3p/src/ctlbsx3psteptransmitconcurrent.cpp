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
// @file ctlbsx3psteptransmitconcurrent.cpp
// This is the class implementation for the Module Information Tests
// EPOC includes.
// 
//

// LBS includes.
#include <lbs.h>
#include <lbs/lbsx3p.h>
#include <lbs/test/lbsnetsimtest.h>

#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbsx3psteptransmitconcurrent.h"

const TInt KTestAssistanceDataProviderPluginUidValue = 0x10281D77;
const TInt KRealAssistanceDataProviderPluginUidValue = 0x10285AC2;
_LIT(KPosInfosFile, "pos_infos_file");

#define REFPOS_LAT				2
#define REFPOS_LONG				3
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	100
#define REFPOS_VERT_ACCURACY	100

/**
 * Destructor
 */
CT_LbsX3PStep_Transmit_Concurrent::~CT_LbsX3PStep_Transmit_Concurrent()
	{	// Close the positioner.
		iPositionTransmitter.Close();

		// Close Transmit Position server.
		iServer.Close();

		// Delete this active scheduler
		delete iScheduler;

		// Clear arrays.
		for (TInt i = 0; i < iVerifyPosInfoArr.Count(); ++i)
			delete iVerifyPosInfoArr[i];
		iVerifyPosInfoArr.Close();

		for (TInt i = 0; i < iCurrentPosInfoArr.Count(); ++i)
			delete iCurrentPosInfoArr[i];
		iCurrentPosInfoArr.Close();

		delete iRefLocWaiter;

	}

/**
 * Constructor
 */
CT_LbsX3PStep_Transmit_Concurrent::CT_LbsX3PStep_Transmit_Concurrent() : CActive(EPriorityStandard)
	{
	iTestModuleInUse = ETrue;
	SetTestStepName(KLbsX3PStep_Transmit_Concurrent);
	}

/**
Static Constructor
*/
CT_LbsX3PStep_Transmit_Concurrent* CT_LbsX3PStep_Transmit_Concurrent::New()
	{
	CT_LbsX3PStep_Transmit_Concurrent* testStep = new CT_LbsX3PStep_Transmit_Concurrent();
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

void CT_LbsX3PStep_Transmit_Concurrent::ConstructL()
	{
	// Create this objects active scheduler
	iScheduler = new (ELeave) CActiveScheduler();

	CleanupStack::PushL(iScheduler);

	// Connect to the Transmit Position server.
	User::LeaveIfError(iServer.Connect());
	CleanupClosePushL(iServer);
	// Open the positioner.
	User::LeaveIfError(iPositionTransmitter.Open(iServer));

	CleanupStack::Pop(&iServer);

	// Should not be an active schedular running
	CActiveScheduler::Install( iScheduler );

	// Add this to the AS list
	CActiveScheduler::Add(this);

	CleanupStack::Pop(iScheduler);

	iRefLocWaiter = CT_LbsX3pRefLocWaiter::NewL(this);

	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsX3PStep_Transmit_Concurrent::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("   CT_LbsX3PStep_Transmit_Concurrent::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		// This is the active object carrying out the TransmitPosition

		// Create the verifiable position info
		CreateVerifyPositionInfoL();

		//Connect to NetSim
		User::LeaveIfError(OpenNetSim());

	    TBool bRefPos;
	    bRefPos = GetIntFromConfig(ConfigSection(), KLbsRefPos, bRefPos);

		// Carry out unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{

			// Set the position quality required by the network.
			TTimeIntervalMicroSeconds maxFixTime(600*1000000); 	//default was 60

			switch (iTestCaseId)
				{

				// Test case LBS-X3P-Transmit-0002
				// This test uses the default functionality in each of the callbacks.
				case 12:
					{

					// Run this in concurrent using TEF concurrent command.

					// Test case LBS-X3P-Transmit-0002	Request transmit current position from multiple clients in concurrent.
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do multiple X3P in concurrent.
					// Local callbacks:	Verify callback parameters are as expected.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					// Configure timeouts
					TLbsTransmitPositionOptions temptransmitOptions;			// This is how long we wait for an X3P
					const TTimeIntervalMicroSeconds KLbsTempTestModuleTimeOut(180*1000000);
					temptransmitOptions.SetTimeOut(KLbsTempTestModuleTimeOut);
					iPositionTransmitter.SetTransmitOptions(temptransmitOptions);

					TPositionInfo* posInfo = new TPositionInfo();
					iCurrentPosInfoArr.Append(posInfo);

					//	3. Request X3P on each sub-session (priorities can be the same on each)
					if(bRefPos)
						{
						iPositionTransmitter.TransmitPosition(KConcurrentDestination, KConcurrentTransmitPriority, iRefLocWaiter->iStatus, iRefLocWaiter->iRefPosInfo, iStatus, *posInfo);
						iRefLocWaiter->Activate();
						}
					else
						iPositionTransmitter.TransmitPosition(KConcurrentDestination, KConcurrentTransmitPriority, iStatus, *posInfo);

					SetActive();
					// Kick off test.
				    CActiveScheduler::Start();

					}
					break;

				default:
					{
					INFO_PRINTF2(_L("<FONT><B>   Case number %d did not match any expected value</B></FONT>"),iTestCaseId);
					User::Panic(KLbsX3PStep_Transmit_Concurrent, KErrUnknown);
					}
				}
			}

		VerifyPosInfo();
		//VerifyLogInfo

		iNetSim.Close();

		}


	INFO_PRINTF1(_L("&lt;&lt;CT_LbsX3PStep_Transmit_Concurrent::doTestStepL()"));

	return TestStepResult();
	}

void CT_LbsX3PStep_Transmit_Concurrent::DoCancel()
	{
		iPositionTransmitter.CancelTransmitPosition();
	}


void CT_LbsX3PStep_Transmit_Concurrent::RunL()
{
	TInt err = iStatus.Int();

	INFO_PRINTF1(_L("   RunL()  Starting..."));
	switch (iTestCaseId)
		{
		case 12:
			{
			if(err != KErrNone)
				{
				INFO_PRINTF3(_L("<FONT><B>iTestCaseId: %d, iStatus: %d, KErrNone was expected.</B></FONT>"),iTestCaseId, iStatus.Int());
				SetTestStepResult(EFail);
				}
			else
				{
				// VerifyPos
				}

			}
			break;

		default:
			{
			INFO_PRINTF2(_L("iTestCaseId: %d, is not Supported"),iTestCaseId);
			}
			break;
		}

	CActiveScheduler::Stop();
	User::LeaveIfError(err);
	INFO_PRINTF1(_L("   RunL()  Stopping..."));
}


TInt CT_LbsX3PStep_Transmit_Concurrent::RunError(TInt aError)
{
	INFO_PRINTF2(_L("   RunL has left with error %d"), aError);
	return aError;
}


TInt CT_LbsX3PStep_Transmit_Concurrent::OpenNetSim()
{
	INFO_PRINTF1(_L("   OpenNetSim"));

	// Connect to net sim.
	TInt err = iNetSim.ConnectL(NULL);
	if (err)
		{
		return err;
		}

	// Set the reference position, which also be used for any required verification.
	iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	iRefPos.SetCurrentTime();

	if (!iNetSim.SetReferenceLocation(iRefPos))
		{
		INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
		iNetSim.Close();

		return KErrGeneral;
		}

	// Set plugin to use.
	TUid pluginUid;

	if(iTestModuleInUse)
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
		iNetSim.Close();

		return KErrGeneral;
		}

	// Set the position quality required by the network.
	TTimeIntervalMicroSeconds maxFixTime(600*1000000); 	//default was 60

	iNetPosQuality.SetMaxFixTime(maxFixTime);
	iNetPosQuality.SetMinHorizontalAccuracy(iRefPos.HorizontalAccuracy());
	iNetPosQuality.SetMinVerticalAccuracy(iRefPos.VerticalAccuracy());

	if (!iNetSim.SetQuality(iNetPosQuality))
		{
		INFO_PRINTF1(_L("Failed test, can't set NetSim's quality."));
		SetTestStepResult(EFail);

		iNetSim.Close();

		return TestStepResult();
		}

	return KErrNone;
}

void CT_LbsX3PStep_Transmit_Concurrent::CreateVerifyPositionInfoL()
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

void CT_LbsX3PStep_Transmit_Concurrent::VerifyPosInfo()
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

void CT_LbsX3PStep_Transmit_Concurrent::MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo)
	{
	INFO_PRINTF1(_L("Got - Transmit Position Reference Pos callback"));

	TPositionInfo refPosInfo;
	T_LbsUtils utils;
	T_LbsUtils::TComparisonAccuracyType accType;

	refPosInfo.SetPosition(iRefPos);

	if(iTestModuleInUse)
		accType = T_LbsUtils::EExactAccuracy;
	else
		accType = T_LbsUtils::ERoughAccuracy;

	if(!utils.Compare_PosInfo(refPosInfo, aRefPosInfo, accType) || aStatus != KErrNone)
		{
		INFO_PRINTF1(_L("<FONT><B>Failed test, X3PRefPos callback: Reference Position differs from aPositionInfo</B></FONT>"));
		SetTestStepResult(EFail);
		}

	}

void CT_LbsX3PStep_Transmit_Concurrent::MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos* aDoTransmitPosAO)
	{
	(void)aDoTransmitPosAO;
	(void)aStatus;
	//This callback will never be called
	INFO_PRINTF1(_L("<FONT><B>Unexpected call....Should never happen</B></FONT>"));
	}
