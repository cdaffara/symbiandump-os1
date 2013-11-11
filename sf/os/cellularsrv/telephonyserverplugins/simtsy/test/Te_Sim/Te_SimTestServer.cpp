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
//

/**
 @file
*/

#include "Te_SimTestServer.h"

#include "Te_SimAPNControlList.h"
#include "Te_SimAPNControlListStatus.h"
#include "Te_SimBatteryChargerTest.h"
#include "Te_SimBasicVoiceTest.h"
#include "Te_SimCallOOMTest.h"
#include "Te_SimCallOpenTest.h"
#include "Te_SimDtmfTest.h"
#include "Te_SimEmergencyCallTest.h"
#include "Te_SimGetSubscriberIdTest.h"
#include "Te_SimIncomingDataTest.h"
#include "Te_SimIndicatorTest.h"
#include "Te_SimMiscTest.h"
#include "Te_SimNetworkStatusTest.h"
#include "Te_SimNitzTest.h"
#include "Te_SimCellInfoTest.h"
#include "Te_SimNoConfigFileTest.h"
#include "Te_SimOutGoingDataTest.h"
#include "Te_SimPacketAdditionalParamsTest.h"
#include "Te_SimPacketContextTest.h"
#include "Te_SimPacketCorruptedConfigFileTest.h"
#include "Te_SimPacketFinalContextTest.h"
#include "Te_SimPacketGPRSNotifyConfigCancelTest.h"
#include "Te_SimPacketGPRSNotifyConfigChangeTest.h"
#include "Te_SimPacketGPRSContextTest.h"
#include "Te_SimPacketGPRSQOSTest.h"
#include "Te_SimPacketRel99Rel5QOSTest.h"
#include "Te_SimPacketRel99Rel5QOSPubSubNotifTest.h"
#include "Te_SimPacketGPRSRel99NotifyProfileChangeTest.h"
#include "Te_SimPacketNotify2ThreadTest.h"
#include "Te_SimPacketPenultimateContextTest.h"
#include "Te_SimPacketQoS.h"
#include "Te_SimPacketRel5ContextTest.h"
#include "Te_SimPacketRel5QOSTest.h"
#include "Te_SimPacketRel99ContextTest.h"
#include "Te_SimPacketRel99QOSTest.h"
#include "Te_SimPacketTest.h"
#include "Te_SimPhBkErrorOOBTest.h"
#include "Te_SimPhBkLockInfoTest.h"
#include "Te_SimPhBkPhoneSecurityTest.h"
#include "Te_SimPhBkSecurityCapsTest.h"
#include "Te_SimPhBkSmartCardTest.h"
#include "Te_SimPhBkSmartCardUSimTest.h"
#include "Te_SimPhBkStoreICCLockedTest.h"
#include "Te_SimPhBkStoreTest.h"
#include "Te_SimPhBkStoreUICCLockedTest.h"
#include "Te_SimPhBkUSimAppsTest.h"
#include "Te_SimPhBkUSimErrorOOBTest.h"
#include "Te_SimPhBkUSimStoreTest.h"
#include "Te_SimONStoreTest.h"
#include "Te_SimSatTest.h"
#include "Te_SimSignalStrengthTest.h"
#include "Te_SimSmsListTest.h"
#include "Te_SimSmsReceiveModesTest.h"
#include "Te_SimSmsRxTest.h"
#include "Te_SimSmsStoreTest.h"
#include "Te_SimSmsTxTest.h"
#include "Te_SimTestServer.h"
#include "Te_SimTestStepBase.h"
#include "Te_SimVoiceTest.h"
#include "Te_SimPacketNetworkNotifyConfigChangeTest.h"
#include "Te_SimUSimRel6Test.h"

CSimTestServer* CSimTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSimTestServer * testServer = new (ELeave) CSimTestServer();
	CleanupStack::PushL(testServer);
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	testServer->ConstructL(serverName.Name());
	CleanupStack::Pop(testServer);
	return testServer;
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
   	TInt result = StartC32();
	if (result != KErrNone  &&  result != KErrAlreadyExists)
		{
		User::Leave(result);
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	
	// Create the CTestServer derived server
	CSimTestServer* testServer = NULL;
	TRAPD(err,testServer = CSimTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete testServer;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }

CTestStep* CSimTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

   if (aStepName == _L("AdditionalParamsContextTest"))
		{
		testStep = new CSimPacketAdditionalParamsTest;
		}
	else if (aStepName == _L("AdditionalParamsContextTest2"))
		{
		testStep = new CSimPacketAdditionalParamsTest2;
		}
	else if (aStepName == _L("AdditionalParamsContextTest3"))
		{
		testStep = new CSimPacketAdditionalParamsTest3;
		}
	else if(aStepName == _L("BatteryChargerTest"))
		{
		testStep = new CSimBatteryChargerTest;
		}
	else if(aStepName == _L("BasicVoiceTest"))
		{
		testStep = new CSimBasicVoiceTest;
		}
	else if(aStepName == _L("BinaryAdditionalParamsContextTest1"))
		{
		testStep = new CSimPacketBinaryAdditionalParamsTest1;
		}
	else if(aStepName == _L("BinaryAdditionalParamsContextTest2"))
		{
		testStep = new CSimPacketBinaryAdditionalParamsTest2;
		}
	else if(aStepName == _L("CallOOMTest"))
		{
		testStep = new CSimCallOOMTest;
		}
	else if(aStepName == _L("CallOpenTest"))
		{
		testStep = new CSimCallOpenTest;
		}
	else if(aStepName == _L("ContextTest"))
		{
		testStep = new CSimPacketContextTest;
		}
	else if(aStepName == _L("CorruptedConfigFileTest"))
		{
		testStep = new CSimPacketCorruptedConfigFileTest;
		}
	else if (aStepName == _L("DefaultAdditionalParamsContextTest"))
		{
		testStep = new CSimPacketDefaultAdditionalParamsTest;
		}
	else if (aStepName == _L("DefaultBinaryAdditionalParamsContextTest"))
		{
		testStep = new CSimPacketDefaultBinaryAdditionalParamsTest;
		}		
	else if(aStepName == _L("DtmfTest"))
		{
		testStep = new CSimDtmfTest;
		}
	else if(aStepName == _L("EmergencyCallTest"))
		{
		testStep = new CSimEmergencyCallTest;
		}
	else if(aStepName == _L("ErrorOOBTest"))
		{
		testStep = new CSimPhBkErrorOOBTest;
		}
	else if(aStepName == _L("FinalContextTest"))
		{
		testStep = new CSimPacketFinalContextTest;
		}
	else if(aStepName == _L("GetSubscriberIdTest"))
		{
		testStep = new CSimGetSubscriberIdTest;
		}
	else if(aStepName == _L("GPRSNotifyConfigChangeTest"))
		{
		testStep = new CSimPacketGPRSNotifyConfigChangeTest;
		}
	else if(aStepName == _L("GPRSNotifyConfigCancelTest"))
		{
		testStep = new CSimPacketGPRSNotifyConfigCancelTest;
		}
	else if(aStepName == _L("GPRSContextTest"))
		{
		testStep = new CSimPacketGPRSContextTest;
		}
	else if(aStepName == _L("GPRSQOSTest"))
		{
		testStep = new CSimPacketGPRSQOSTest;
		}
	else if(aStepName == _L("GPRSRel99NotifyProfileChangeTest"))
		{
		testStep = new CSimPacketGPRSRel99NotifyProfileChangeTest;
		}
	else if(aStepName == _L("IncomingDataTest"))
		{
		testStep = new CSimIncomingDataTest;
		}
	else if(aStepName == _L("IndicatorTest"))
		{
		testStep = new CSimIndicatorTest;
		}
	else if(aStepName == _L("LockInfoTest"))
		{
		testStep = new CSimPhBkLockInfoTest;
		}
	else if(aStepName == _L("MiscTest"))
		{
		testStep = new CSimMiscTest;
		}
	else if(aStepName == _L("NetworkStatusTest"))
		{
		testStep = new CSimNetworkStatusTest;
		}
	else if(aStepName == _L("NitzTest"))
		{
		testStep = new CSimNitzTest;
		}
	else if(aStepName == _L("CellInfoTest"))
		{
		testStep = new CSimCellInfoTest;
		}
	else if(aStepName == _L("NoConfigFileTest"))
		{
		testStep = new CSimNoConfigFileTest;
		}
	else if(aStepName == _L("Notify2ThreadTest"))
		{
		testStep = new CSimPacketNotify2ThreadTest;
		}
	else if(aStepName == _L("OutGoingDataTest"))
		{
		testStep = new CSimOutGoingDataTest;
		}
	else if(aStepName == _L("PacketTest"))
		{
		testStep = new CSimPacketsTest;
		}
	else if(aStepName == _L("PenultimateContextTest"))
		{
		testStep = new CSimPacketPenultimateContextTest;
		}
	else if(aStepName == _L("PhBkStoreTest"))
		{
		testStep = new CSimPhBkStoreTest;
		}
	else if(aStepName == _L("PhBkUSimStoreTest"))
		{
		testStep = new CSimPhBkUSimStoreTest;
		}
	else if(aStepName == _L("PhoneSecurityTest"))
		{
		testStep = new CSimPhBkPhoneSecurityTest;
		}
	else if(aStepName == _L("ONStoreTest"))
		{
		testStep = new CSimONStoreTest;
		}
	else if(aStepName == _L("Rel5ContextTest"))
		{
		testStep = new CSimPacketRel5ContextTest;
		}
	else if(aStepName == _L("Rel5QOSTest"))
		{
		testStep = new CSimPacketRel5QOSTest;
		}
	else if(aStepName == _L("Rel99ContextTest"))
		{
		testStep = new CSimPacketRel99ContextTest;
		}
	else if(aStepName == _L("Rel99QOSTest"))
		{
		testStep = new CSimPacketRel99QOSTest;
		}
	else if(aStepName == _L("Rel99Rel5QOSTest"))
		{
		testStep = new CSimPacketRel99Rel5QOSTest;
		}
	else if(aStepName == _L("Rel99Rel5QOSPubSubNotifTest"))
		{
		testStep = new CSimPacketRel99Rel5QOSPubSubNotifTest;
		}
	else if(aStepName == _L("SatTest"))
		{
		testStep = new CSimSatTest;
		}
	else if(aStepName == _L("SatTestRetMMPCmd"))
		{
		testStep = new CSimSatRetMMPCmdTest;
		}
	else if(aStepName == _L("SatTestSubMMPCmd"))
		{
		testStep = new CSimSatSubMMPCmdTest;
		}
	else if(aStepName == _L("SatTestDispMMPCmd"))
		{
		testStep = new CSimSatDispMMPCmdTest;
		}
	else if(aStepName == _L("SatTestSetFrmsPCmd"))
		{
			testStep = new CSimSatSetFrmsPCmdTest;
		}
	else if(aStepName == _L("SatTestGetFrmsPCmd"))
		{
			testStep = new CSimSatGetFrmsPCmdTest;
		}
	else if(aStepName == _L("SatTestMmsNotificationDownload"))
		{
			testStep = new CSimSatMmsNotificationDownload;
		}
	else if(aStepName == _L("SatTestProvideLocalInfo"))
		{
			testStep = new CSimSatProvideLocalInfo;
		}
	else if(aStepName == _L("SatTestUssdDataDownload"))
		{
			testStep = new CSimSatUssdDataDownload;
		}
	else if(aStepName == _L("SatTestLaunchBrowser"))
		{
			testStep = new CSimSatLnchBrwsr;
		}
	else if(aStepName == _L("SatTestOpenChnl"))
		{
			testStep = new CSimSatOpenChnl;
		}
	else if(aStepName == _L("SatTestPlayTone"))
		{
			testStep = new CSimSatPlayTone;
		}
	else if(aStepName == _L("SatTestSetupCall"))
		{
			testStep = new CSimSatSetupCall;
		}
	else if(aStepName == _L("SatTestRefresh"))
		{
			testStep = new CSimSatRefresh;
		}
	else if(aStepName == _L("SatTestEventDnld"))
		{
			testStep = new CSimSatEventDnld;
		}
	else if(aStepName == _L("SatTestSendUssd"))
		{
			testStep = new CSimSatSendUssd;
		}
	else if(aStepName == _L("SatTestGenPCmd"))
		{
			testStep = new CSimSatGenPCmd;
		}
	else if(aStepName == _L("SatTestElemFiles"))
		{
			testStep = new CSimSatElemFile;
		}
	else if(aStepName == _L("SecurityCapsTest"))
		{
		testStep = new CSimPhBkSecurityCapsTest;
		}
	else if(aStepName == _L("SetQoSDelayOnFirstSet"))
		{
		testStep = new CSetQoSDelayOnFirstSet;
		}
	else if(aStepName == _L("SetQoSDelayOnThirdSet"))
		{
		testStep = new CSetQoSDelayOnThirdSet;
		}
	else if(aStepName == _L("SetQoSDelayUnset"))
		{
		testStep = new CSetQoSDelayUnset;
		}
	else if(aStepName == _L("SetQoSNegativeDelay"))
		{
		testStep = new CSetQoSNegativeDelay;
		}
	else if(aStepName == _L("SignalStrengthTest"))
		{
		testStep = new CSimSignalStrengthTest;
		}
	else if(aStepName == _L("SmartCardTest"))
		{
		testStep = new CSimPhBkSmartCardTest;
		}
	else if(aStepName == _L("SmartCardUSimTest"))
		{
		testStep = new CSimPhBkSmartCardUSimTest;
		}
	else if(aStepName == _L("SetVersion"))
		{
		testStep = new CSetVersion;
		}
	else if(aStepName == _L("CheckVersionSupport"))
		{
		testStep = new CCheckVersionSupport;
		}
	else if(aStepName == _L("SmsListTest"))
		{
		testStep = new CSimSmsListTest;
		}
	else if(aStepName == _L("SmsRecieveModesTest"))
		{
		testStep = new CSimSmsReceiveModesTest;
		}
	else if(aStepName == _L("SmsRxTest"))
		{
		testStep = new CSimSmsRxTest;
		}
	else if(aStepName == _L("SmsStoreTest"))
		{
		testStep = new CSimSmsStoreTest;
		}
	else if(aStepName == _L("SmsTxTest"))
		{
		testStep = new CSimSmsTxTest;
		}
	else if(aStepName == _L("StoreICCLockedTest"))
		{
		testStep = new CSimPhBkStoreICCLockedTest;
		}
	else if(aStepName == _L("StoreUICCLockedTest"))
		{
		testStep = new CSimPhBkStoreUICCLockedTest;
		}
	else if(aStepName == _L("USimAppsTest"))
		{
		testStep = new CSimPhBkUSimAppsTest;
		}
	else if(aStepName == _L("USimErrorOOBTest"))
		{
		testStep = new CSimPhBkUSimErrorOOBTest;
		}
	else if(aStepName == _L("VoiceTest"))
		{
		testStep = new CSimVoiceTest;
		}
	else if(aStepName == _L("APNControlList"))
		{
		testStep = new CSimAPNControlList;
		}		
	else if(aStepName == _L("APNControlListStatus"))
		{
		testStep = new CSimAPNControlListStatus;
		}			
	else if(aStepName == _L("NetworkNotifyConfigChangeTest"))
		{
		testStep = new CSimPacketNetworkNotifyConfigChangeTest;
		}
	else if(aStepName == _L("VideomailTest1"))
		{
		testStep = new CSimVideomailTest1;
		}
	else if(aStepName == _L("VideomailTest2"))
		{
		testStep = new CSimVideomailTest2;
		}
	else if(aStepName == _L("VideomailTest3"))
		{
		testStep = new CSimVideomailTest3;
		}
	else if(aStepName == _L("VideomailTest4"))
		{
		testStep = new CSimVideomailTest4;
		}		
	else if(aStepName == _L("VideomailTest5"))
		{
		testStep = new CSimVideomailTest5;
		}	
	else if(aStepName == _L("WlanTest1"))
		{
		testStep = new CSimWlanTest1;
		}
	else if(aStepName == _L("WlanTest2"))
		{
		testStep = new CSimWlanTest2;
		}
	else if(aStepName == _L("WlanTest3"))
		{
		testStep = new CSimWlanTest3;
		}
	else if(aStepName == _L("WlanTest4"))
		{
		testStep = new CSimWlanTest4;
		}
	else if(aStepName == _L("WlanTest5"))
		{
		testStep = new CSimWlanTest5;
		}
	else if(aStepName == _L("WlanTest6"))
		{
		testStep = new CSimWlanTest6;
		}
	else if(aStepName == _L("WlanTest7"))
		{
		testStep = new CSimWlanTest7;
		}
	else if(aStepName == _L("WlanTest8"))
		{
		testStep = new CSimWlanTest8;
		}
	else if(aStepName == _L("WlanTest9"))
		{
		testStep = new CSimWlanTest9;
		}
	else if(aStepName == _L("WlanTest10"))
		{
		testStep = new CSimWlanTest10;
		}
	else if(aStepName == _L("WlanTest11"))
		{
		testStep = new CSimWlanTest11;
		}
	else if(aStepName == _L("WlanTest12"))
		{
		testStep = new CSimWlanTest12;
		}
	else if(aStepName == _L("WlanTest13"))
		{
		testStep = new CSimWlanTest13;
		}
	else if(aStepName == _L("WlanTest14"))
		{
		testStep = new CSimWlanTest14;
		}
	else if(aStepName == _L("WlanTest15"))
		{
		testStep = new CSimWlanTest15;
		}
	else if(aStepName == _L("WlanTest16"))
		{
		testStep = new CSimWlanTest16;
		}
	else if(aStepName == _L("WlanTest17"))
		{
		testStep = new CSimWlanTest17;
		}
	else if(aStepName == _L("WlanTest18"))
		{
		testStep = new CSimWlanTest18;
		}
	else if(aStepName == _L("WlanTest19"))
		{
		testStep = new CSimWlanTest19;
		}
	else if(aStepName == _L("WlanTest20"))
		{
		testStep = new CSimWlanTest20;
		}
	else if(aStepName == _L("WlanTest21"))
		{
		testStep = new CSimWlanTest21;
		}
	else if(aStepName == _L("WlanTest22"))
		{
		testStep = new CSimWlanTest22;
		}
	else if(aStepName == _L("3GSecurityContextTest1"))
		{
		testStep = new CSim3GSecurityContextTest1;
		}
	else if(aStepName == _L("3GSecurityContextTest2"))
		{
		testStep = new CSim3GSecurityContextTest2;
		}
	else if(aStepName == _L("3GSecurityContextTest3"))
		{
		testStep = new CSim3GSecurityContextTest3;
		}
	else if(aStepName == _L("3GSecurityContextTest4"))
		{
		testStep = new CSim3GSecurityContextTest4;
		}
	else if(aStepName == _L("3GSecurityContextTest5"))
		{
		testStep = new CSim3GSecurityContextTest5;
		}
	else if(aStepName == _L("UsimR6GBATest1"))
		{
		testStep = new CSimUsimR6GBATest1;
		}	
	else if(aStepName == _L("UsimR6GBATest2"))
		{
		testStep = new CSimUsimR6GBATest2;
		}	
	else if(aStepName == _L("UsimR6GBATest3"))
		{
		testStep = new CSimUsimR6GBATest3;
		}	
	else if(aStepName == _L("UsimR6GBATest4"))
		{
		testStep = new CSimUsimR6GBATest4;
		}	
	else if(aStepName == _L("UsimR6GBATest5"))
		{
		testStep = new CSimUsimR6GBATest5;
		}	
	else if(aStepName == _L("UsimR6GBATest6"))
		{
		testStep = new CSimUsimR6GBATest6;
		}	
	else if(aStepName == _L("UsimR6GBATest7"))
		{
		testStep = new CSimUsimR6GBATest7;
		}	
	else if(aStepName == _L("UsimR6GBATest8"))
		{
		testStep = new CSimUsimR6GBATest8;
		}	
	else if(aStepName == _L("UsimR6GBATest9"))
		{
		testStep = new CSimUsimR6GBATest9;
		}	
	else if(aStepName == _L("UsimR6GBATest10"))
		{
		testStep = new CSimUsimR6GBATest10;
		}	
	else if(aStepName == _L("UsimR6GBATest11"))
		{
		testStep = new CSimUsimR6GBATest11;
		}	
	else if(aStepName == _L("UsimR6GBATest12"))
		{
		testStep = new CSimUsimR6GBATest12;
		}	
	else if(aStepName == _L("UsimR6GBATest13"))
		{
		testStep = new CSimUsimR6GBATest13;
		}	
	else if(aStepName == _L("UsimR6GBATest14"))
		{
		testStep = new CSimUsimR6GBATest14;
		}	
	else if(aStepName == _L("UsimR6GBATest15"))
		{
		testStep = new CSimUsimR6GBATest15;
		}
	else if(aStepName == _L("UsimR6GBATest16"))
		{
		testStep = new CSimUsimR6GBATest16;
		}	
	else if(aStepName == _L("UsimR6GBATest17"))
		{
		testStep = new CSimUsimR6GBATest17;
		}	
	else if(aStepName == _L("UsimR6GBATest18"))
		{
		testStep = new CSimUsimR6GBATest18;
		}	
	else if(aStepName == _L("UsimR6GBATest19"))
		{
		testStep = new CSimUsimR6GBATest19;
		}	
	else if(aStepName == _L("UsimR6GBATest20"))
		{
		testStep = new CSimUsimR6GBATest20;
		}
	else if(aStepName == _L("UsimR6GBATest21"))
		{
		testStep = new CSimUsimR6GBATest21;
		}	
	else if(aStepName == _L("UsimR6GBATest22"))
		{
		testStep = new CSimUsimR6GBATest22;
		}	
	else if(aStepName == _L("UsimR6GBATest23"))
		{
		testStep = new CSimUsimR6GBATest23;
		}
	else if(aStepName == _L("UsimR6MBMSTest1"))
		{
		testStep = new CSimUsimR6MBMSTest1;
		}	
	else if(aStepName == _L("UsimR6MBMSTest2"))
		{
		testStep = new CSimUsimR6MBMSTest2;
		}	
	else if(aStepName == _L("UsimR6MBMSTest3"))
		{
		testStep = new CSimUsimR6MBMSTest3;
		}
	else if(aStepName == _L("UsimR6MBMSTest4"))
		{
		testStep = new CSimUsimR6MBMSTest4;
		}	
	else if(aStepName == _L("UsimR6MBMSTest5"))
		{
		testStep = new CSimUsimR6MBMSTest5;
		}	
	else if(aStepName == _L("UsimR6MBMSTest6"))
		{
		testStep = new CSimUsimR6MBMSTest6;
		}
	else if(aStepName == _L("UsimR6MBMSTest7"))
		{
		testStep = new CSimUsimR6MBMSTest7;
		}	
	else if(aStepName == _L("UsimR6MBMSTest8"))
		{
		testStep = new CSimUsimR6MBMSTest8;
		}	
	else if(aStepName == _L("UsimR6MBMSTest9"))
		{
		testStep = new CSimUsimR6MBMSTest9;
		}
	else if(aStepName == _L("UsimR6MBMSTest10"))
		{
		testStep = new CSimUsimR6MBMSTest10;
		}	
	else if(aStepName == _L("UsimR6MBMSTest11"))
		{
		testStep = new CSimUsimR6MBMSTest11;
		}	
	else if(aStepName == _L("UsimR6MBMSTest12"))
		{
		testStep = new CSimUsimR6MBMSTest12;
		}
	else if(aStepName == _L("UsimR6MBMSTest13"))
		{
		testStep = new CSimUsimR6MBMSTest13;
		}	
	else if(aStepName == _L("UsimR6MBMSTest14"))
		{
		testStep = new CSimUsimR6MBMSTest14;
		}	
	else if(aStepName == _L("UsimR6MBMSTest15"))
		{
		testStep = new CSimUsimR6MBMSTest15;
		}
	else if(aStepName == _L("UsimR6MBMSTest16"))
		{
		testStep = new CSimUsimR6MBMSTest16;
		}	
	else if(aStepName == _L("UsimR6MBMSTest17"))
		{
		testStep = new CSimUsimR6MBMSTest17;
		}
	else if(aStepName == _L("UsimR6MBMSTest18"))
		{
		testStep = new CSimUsimR6MBMSTest18;
		}	
	else if(aStepName == _L("UsimR6MBMSTest19"))
		{
		testStep = new CSimUsimR6MBMSTest19;
		}	
	else if(aStepName == _L("UsimR6MBMSTest20"))
		{
		testStep = new CSimUsimR6MBMSTest20;
		}
	else if(aStepName == _L("UsimR6MBMSTest21"))
		{
		testStep = new CSimUsimR6MBMSTest21;
		}
	else if(aStepName == _L("UsimR6MBMSTest22"))
		{
		testStep = new CSimUsimR6MBMSTest22;
		}
	else if(aStepName == _L("UsimR6MBMSTest23"))
		{
		testStep = new CSimUsimR6MBMSTest23;
		}
	else if(aStepName == _L("UsimR6MBMSTest24"))
		{
		testStep = new CSimUsimR6MBMSTest24;
		}
	else if(aStepName == _L("UsimR6MBMSTest25"))
		{
		testStep = new CSimUsimR6MBMSTest25;
		}
	else if(aStepName == _L("UsimR6MBMSTest26"))
		{
		testStep = new CSimUsimR6MBMSTest26;
		}
	else if(aStepName == _L("UsimR6MBMSTest27"))
		{
		testStep = new CSimUsimR6MBMSTest27;
		}
	else if(aStepName == _L("UsimR6MBMSTest28"))
		{
		testStep = new CSimUsimR6MBMSTest28;
		}
	else if(aStepName == _L("UsimR6MBMSTest29"))
		{
		testStep = new CSimUsimR6MBMSTest29;
		}
   	return testStep;
	}



