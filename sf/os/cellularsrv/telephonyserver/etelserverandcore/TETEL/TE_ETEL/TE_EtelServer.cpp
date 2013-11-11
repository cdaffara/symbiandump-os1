// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ETel Unit Test server test code.
// 
//

/**
 @file
 @internalComponent
*/

#include "e32property.h"
#include "simtsy.h"

#include "TE_EtelServer.h"
#include "TE_EtelBase.h"
#include "TE_Etelbadclient.h"
#include "TE_Etelcallowner.h"
#include "TE_Etelcancelservice.h"
#include "TE_Etelcoredstd.h"
#include "TE_Etelcoredstdnc.h"
#include "TE_Eteldataport.h"
#include "TE_Etelduplicatephonename.h"
#include "TE_Etelflowcontrol.h"
#include "TE_Etelloaddrv.h"
#include "TE_Etelloadmodule.h"
#include "TE_Etellowmem.h"
#include "TE_Etelmembadclient.h"
#include "TE_Etelmemberfn.h"
#include "TE_Etelmemcallowner.h"
#include "TE_Etelmemdataport.h"
#include "TE_Etelmemrelinquish.h"
#include "TE_Etelmemzerolength.h"
#include "TE_Etelmulticompletion.h"
#include "TE_Etelmultithreadserveropen.h"
#include "TE_Etelopendevice.h"
#include "TE_Etelopenmemfail.h"
#include "TE_Etelphoneinfo.h"
#include "TE_Etelplatsecdelegation.h"
#include "TE_Etelrelinquish.h"
#include "TE_Etelreqmode.h"
#include "te_etelerr.h"
#include "TE_Etelsetdes.h"
#include "te_etelsetpriorityclient.h"
#include "te_etelsetemergencyclient.h"
#include "te_etelpanicafterrequest.h"



/**
 *  Called inside the MainL() function to create and start the CTestServer
 *  derived server.
 *
 *  @return Instance of the test server
 */
CETelTestServer* CETelTestServer::NewL()
	{
	CETelTestServer*  server = new(ELeave) CETelTestServer();
	CleanupStack::PushL(server);
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	} // CETelTestServer::NewL


/**
 *  Uses the new Rendezvous() call to ETel with the client.
 */
LOCAL_C void MainL()
	{
	//
	// Ensure C32 is running...
	//
    TInt  result;
    
    result = StartC32();
	if (result != KErrNone  &&  result != KErrAlreadyExists)
		{
		User::Leave(result);
		}

	//
	// Start an active scheduler...
	//
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	//
	// Ensure the SIMTSY config number is reset prior to starting...
	//
	result = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0);
	if (result != KErrNone  &&  result != KErrNotFound)
		{
		User::Leave(result);
		}

	//
	// Create the CTestServer derived server...
	//
	CETelTestServer* server = NULL;

	TRAPD(err, server = CETelTestServer::NewL());
	if (err == KErrNone)
		{
		//
		// ETel with the client and enter the active scheduler
		//
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}

	//
	// Clean up...
	//
	CleanupStack::Pop(scheduler);
	delete server;
	delete scheduler;
	} // MainL


/**
 *  @return Standard Epoc error code on exit.
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup*  cleanup = CTrapCleanup::New();

	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, MainL());

	delete cleanup;

	return err;
	} // E32Main


/**
 *  Implementation of CTestServer pure virtual.
 *
 *  @return A CTestStep derived instance.
 */
CTestStep* CETelTestServer::CreateTestStep(const TDesC& aStepName)
	{
    //
	// Create the required test step...
	//
	CTestStep*  testStep = NULL;

	if (aStepName == _L("TestEtelLoadDrv"))
		{
		testStep = new CTestEtelLoadDrv();
		}

	if (aStepName == _L("TestMultiThreadServerOpen"))
		{
		testStep = new CTestMultiThreadServerOpen();
		}

	if (aStepName == _L("TestMultiCompletion"))
		{
		testStep = new CTestMultiCompletion();
		}

	if (aStepName == _L("TestMultiCompletionDif"))
		{
		testStep = new CTestMultiCompletionDif();
		}

	if (aStepName == _L("TestReqMode"))
		{
		testStep = new CTestReqMode();
		}

	if (aStepName == _L("TestRelinquish"))
		{
		testStep = new CTestRelinquish();
		}

	if (aStepName == _L("TestRelinquishOwner"))
		{
		testStep = new CTestRelinquishOwner();
		}

	if (aStepName == _L("TestRelinquishCase1"))
		{
		testStep = new CTestRelinquishCase1();
		}

	if (aStepName == _L("TestMemberFnSetGet"))
		{
		testStep = new CTestMemberFnSetGet();
		}

	if (aStepName == _L("TestPhoneMemberFn"))
		{
		testStep = new CTestPhoneMemberFn();
		}

	if (aStepName == _L("TestLineMemberFn"))
		{
		testStep = new CTestLineMemberFn();
		}

	if (aStepName == _L("TestCallMemberFn"))
		{
		testStep = new CTestCallMemberFn();
		}

	if (aStepName == _L("TestFaxMemberFn"))
		{
		testStep = new CTestFaxMemberFn();
		}

	if (aStepName == _L("TestCancelService1"))
		{
		testStep = new CTestCancelService1();
		}

	if (aStepName == _L("TestCancelService2"))
		{
		testStep = new CTestCancelService2();
		}

	if (aStepName == _L("TestCancelService3"))
		{
		testStep = new CTestCancelService3();
		}

	if (aStepName == _L("TestCancelService4"))
		{
		testStep = new CTestCancelService4();
		}

	if (aStepName == _L("TestCancelService6"))
		{
		testStep = new CTestCancelService6();
		}

	if (aStepName == _L("TestCancelService7"))
		{
		testStep = new CTestCancelService7();
		}

	if (aStepName == _L("TestDataPort1"))
		{
		testStep = new CTestDataPort1();
		}

	if (aStepName == _L("TestDataPort2"))
		{
		testStep = new CTestDataPort2();
		}

	if (aStepName == _L("TestDataPort4"))
		{
		testStep = new CTestDataPort4();
		}

	if (aStepName == _L("TestNcTermFaxSess"))
		{
		testStep = new CTestNcTermFaxSess();
		}

	if (aStepName == _L("TestTermFaxSess1"))
		{
		testStep = new CTestTermFaxSess1();
		}

	if (aStepName == _L("TestTermFaxSess2"))
		{
		testStep = new CTestTermFaxSess2();
		}

	if (aStepName == _L("TestNcCancelLineCall"))
		{
		testStep = new CTestNcCancelLineCall();
		}

	if (aStepName == _L("TestCancelNoReq"))
		{
		testStep = new CTestCancelNoReq();
		}

	if (aStepName == _L("TestNcCancelPhone"))
		{
		testStep = new CTestNcCancelPhone();
		}

	if (aStepName == _L("TestLoadTwice"))
		{
		testStep = new CTestLoadTwice();
		}

	if (aStepName == _L("TestNcCancelWrFax"))
		{
		testStep = new CTestNcCancelWrFax();
		}

	if (aStepName == _L("TestGetNcFns"))
		{
		testStep = new CTestGetNcFns();
		}

	if (aStepName == _L("TestCallOwner1"))
		{
		testStep = new CTestCallOwner1();
		}

	if (aStepName == _L("TestCallOwner2"))
		{
		testStep = new CTestCallOwner2();
		}

	if (aStepName == _L("TestCallOwnerCliPanicA"))
		{
		testStep = new CTestCallOwnerCliPanicA();
		}

	if (aStepName == _L("TestOpenPhoneFrServ"))
		{
		testStep = new CTestOpenPhoneFrServ();
		}

	if (aStepName == _L("TestOpenLineFrServ"))
		{
		testStep = new CTestOpenLineFrServ();
		}

	if (aStepName == _L("TestOpenCallFrServ"))
		{
		testStep = new CTestOpenCallFrServ();
		}

	if (aStepName == _L("TestOpenCallFrPhone"))
		{
		testStep = new CTestOpenCallFrPhone();
		}

	if (aStepName == _L("TestOpenLineFrPhone"))
		{
		testStep = new CTestOpenLineFrPhone();
		}

	if (aStepName == _L("TestOpenCallFrLine"))
		{
		testStep = new CTestOpenCallFrLine();
		}

	if (aStepName == _L("TestOpenFaxFrCall"))
		{
		testStep = new CTestOpenFaxFrCall();
		}

	if (aStepName == _L("TestPhoneInfo"))
		{
		testStep = new CTestPhoneInfo();
		}

	if (aStepName == _L("TestErrGetTsyOk"))
		{
		testStep = new CTestErrGetTsyOk();
		}

	if (aStepName == _L("TestErrGetTsyIllegalBuffer"))
		{
		testStep = new CTestErrGetTsyIllegalBuffer();
		}

	if (aStepName == _L("TestErrGetTsySmallBuffer"))
		{
		testStep = new CTestErrGetTsySmallBuffer();
		}

	if (aStepName == _L("TestErrGetTsyVersionNumberOk"))
		{
		testStep = new CTestErrGetTsyVersionNumberOk();
		}

	if (aStepName == _L("TestErrGetTsyVersionNumberIllegalBuffer"))
		{
		testStep = new CTestErrGetTsyVersionNumberIllegalBuffer();
		}

	if (aStepName == _L("TestErrGetTsyVersionNumberSmallBuffer"))
		{
		testStep = new CTestErrGetTsyVersionNumberSmallBuffer();
		}

	if (aStepName == _L("TestErrGetPhoneInfoOk"))
		{
		testStep = new CTestErrGetPhoneInfoOk();
		}

	if (aStepName == _L("TestErrGetPhoneInfoIllegalBuffer"))
		{
		testStep = new CTestErrGetPhoneInfoIllegalBuffer();
		}

	if (aStepName == _L("TestErrGetPhoneInfoSmallBuffer"))
		{
		testStep = new CTestErrGetPhoneInfoSmallBuffer();
		}

	if (aStepName == _L("TestErrIsSupportedByModuleOk"))
		{
		testStep = new CTestErrIsSupportedByModuleOk();
		}

	if (aStepName == _L("TestErrIsSupportedByModuleRetIllegalBuffer"))
		{
		testStep = new CTestErrIsSupportedByModuleRetIllegalBuffer();
		}

	if (aStepName == _L("TestErrIsSupportedByModuleRetSmallBuffer"))
		{
		testStep = new CTestErrIsSupportedByModuleRetSmallBuffer();
		}

	if (aStepName == _L("TestErrIsSupportedByModuleIllegalBuffer"))
		{
		testStep = new CTestErrIsSupportedByModuleIllegalBuffer();
		}

	if (aStepName == _L("TestErrIsSupportedByModuleSmallBuffer"))
		{
		testStep = new CTestErrIsSupportedByModuleSmallBuffer();
		}

	if (aStepName == _L("TestErrSetExtendedErrorGranularityOk"))
		{
		testStep = new CTestErrSetExtendedErrorGranularityOk();
		}

	if (aStepName == _L("TestErrSetExtendedErrorGranularityPanic"))
		{
		testStep = new CTestErrSetExtendedErrorGranularityPanic();
		}

	if (aStepName == _L("TestErrNewTelObjectOk"))
		{
		testStep = new CTestErrNewTelObjectOk();
		}

	if (aStepName == _L("TestErrNewTelObjectIllegalBuffer"))
		{
		testStep = new CTestErrNewTelObjectIllegalBuffer();
		}

	if (aStepName == _L("TestErrNewTelObjectSmallBuffer"))
		{
		testStep = new CTestErrNewTelObjectSmallBuffer();
		}

	if (aStepName == _L("TestErrNewTelObjectHandleIllegalBuffer"))
		{
		testStep = new CTestErrNewTelObjectHandleIllegalBuffer();
		}

	if (aStepName == _L("TestSetDes"))
		{
		testStep = new CTestSetDes();
		}

	if (aStepName == _L("TestLoadModule1"))
		{
		testStep = new CTestLoadModule1();
		}

	if (aStepName == _L("TestLoadModule2"))
		{
		testStep = new CTestLoadModule2();
		}

	if (aStepName == _L("TestLoadModule3"))
		{
		testStep = new CTestLoadModule3();
		}

	if (aStepName == _L("TestFlowControl1"))
		{
		testStep = new CTestFlowControl1();
		}

	if (aStepName == _L("TestFlowControl2"))
		{
		testStep = new CTestFlowControl2();
		}

	if (aStepName == _L("TestFlowControl3"))
		{
		testStep = new CTestFlowControl3();
		}

	if (aStepName == _L("TestFlowControl4"))
		{
		testStep = new CTestFlowControl4();
		}

	if (aStepName == _L("TestFlowControl5"))
		{
		testStep = new CTestFlowControl5();
		}

	if (aStepName == _L("TestFlowControl6"))
		{
		testStep = new CTestFlowControl6();
		}

	if (aStepName == _L("TestFlowControl7"))
		{
		testStep = new CTestFlowControl7();
		}

	if (aStepName == _L("TestOpenCallFrPhone"))
		{
		testStep = new CTestOpenCallFrPhone();
		}

	if (aStepName == _L("TestOpenCallFrServ"))
		{
		testStep = new CTestOpenCallFrServ();
		}

	if (aStepName == _L("TestOpenCallFrLine"))
		{
		testStep = new CTestOpenCallFrLine();
		}

	if (aStepName == _L("TestOpenPhoneFc"))
		{
		testStep = new CTestOpenPhoneFc();
		}

	if (aStepName == _L("TestOpenAllSubSessInFc"))
		{
		testStep = new CTestOpenAllSubSessInFc();
		}

	if (aStepName == _L("TestOpenAllSubSess"))
		{
		testStep = new CTestOpenAllSubSess();
		}

	if (aStepName == _L("TestServerOpenClose"))
		{
		testStep = new CTestServerOpenClose();
		}

	if (aStepName == _L("TestUnload"))
		{
		testStep = new CTestUnload();
		}

	if (aStepName == _L("TestOpenMoreSubSess"))
		{
		testStep = new CTestOpenMoreSubSess();
		}

	if (aStepName == _L("TestNotify"))
		{
		testStep = new CTestNotify();
		}

	if (aStepName == _L("TestNotify2Client"))
		{
		testStep = new CTestNotify2Client();
		}

	if (aStepName == _L("TestBadClient1"))
		{
		testStep = new CTestBadClient1();
		}

	if (aStepName == _L("TestBadClient1a"))
		{
		testStep = new CTestBadClient1a();
		}

	if (aStepName == _L("TestBadClient1b"))
		{
		testStep = new CTestBadClient1b();
		}

	if (aStepName == _L("TestBadClient2"))
		{
		testStep = new CTestBadClient2();
		}

	if (aStepName == _L("TestBadClient2a"))
		{
		testStep = new CTestBadClient2a();
		}

	if (aStepName == _L("TestBadClient3"))
		{
		testStep = new CTestBadClient3();
		}

	if (aStepName == _L("TestBadClient3a"))
		{
		testStep = new CTestBadClient3a();
		}

	if (aStepName == _L("TestBadClient4"))
		{
		testStep = new CTestBadClient4();
		}

	if (aStepName == _L("TestBadClient5"))
		{
		testStep = new CTestBadClient5();
		}

	if (aStepName == _L("TestBadClient6"))
		{
		testStep = new CTestBadClient6();
		}

	if (aStepName == _L("TestBadClient8"))
		{
		testStep = new CTestBadClient8();
		}

	if (aStepName == _L("TestDuplicatePhoneName"))
		{
		testStep = new CTestDuplicatePhoneName();
		}

	if (aStepName == _L("TestOpenCallNoName"))
		{
		testStep = new CTestOpenCallNoName();
		}

	if (aStepName == _L("TestVersion"))
		{
		testStep = new CTestVersion();
		}

	if (aStepName == _L("TestLoadTsySamePhoneName"))
		{
		testStep = new CTestLoadTsySamePhoneName();
		}

	if (aStepName == _L("TestOpenPhoneSamePhoneName"))
		{
		testStep = new CTestOpenPhoneSamePhoneName();
		}

	if (aStepName == _L("TestOpenPhoneModeIdentPhoneName"))
		{
		testStep = new CTestOpenPhoneModeIdentPhoneName();
		}

	if (aStepName == _L("TestStaticCallPoolTsy"))
		{
		testStep = new CTestStaticCallPoolTsy();
		}

	if (aStepName == _L("TestPlatSecDelegation"))
		{
		testStep = new CTestPlatSecDelegation();
		}

	//
	// Out of Memory tests...
	//
	if (aStepName == _L("TestOpenMemFail"))
		{
		testStep = new CTestOpenMemFail();
		}

	if (aStepName == _L("TestLowMem"))
		{
		testStep = new CTestLowMem();
		}

	if (aStepName == _L("TestLowMemPhoneHCheck"))
		{
		testStep = new CTestLowMemPhoneHCheck();
		}

	if (aStepName == _L("TestLowMemFnFail"))
		{
		testStep = new CTestLowMemFnFail();
		}

	if (aStepName == _L("TestMemRelinquish"))
		{
		testStep = new CTestMemRelinquish();
		}

	if (aStepName == _L("TestMemBadClient7"))
		{
		testStep = new CTestMemBadClient7();
		}

	if (aStepName == _L("TestMemBadClient7a"))
		{
		testStep = new CTestMemBadClient7a();
		}

	if (aStepName == _L("TestMemBadClient7b"))
		{
		testStep = new CTestMemBadClient7b();
		}

	if (aStepName == _L("TestMemDataPort"))
		{
		testStep = new CTestMemDataPort();
		}

	if (aStepName == _L("TestMemCallOwner"))
		{
		testStep = new CTestMemCallOwner();
		}

	if (aStepName == _L("TestDataPortCase5"))
		{
		testStep = new CTestDataPortCase5();
		}

	if (aStepName == _L("TestMemZeroLength"))
		{
		testStep = new CTestMemZeroLength();
		}

	if (aStepName == _L("TestEtelUnLoadDrv"))
		{
		testStep = new CTestEtelUnLoadDrv();
		}
		
	if (aStepName == _L("TestSetEmergencyClientCapabilityPosCheck"))
		{
		testStep = new CTestSetEmergencyClientCapabilityPosCheck();
		}
		
	if (aStepName == _L("TestSetEmergencyClientCapabilityNegCheck"))
		{
		testStep = new CTestSetEmergencyClientCapabilityNegCheck();
		}	
			
	if (aStepName == _L("TestSetEmgClientLCSReqMulCallFrSameSes"))
		{
		testStep = new CTestSetEmgClientLCSReqMulCallFrSameSes();
		}
		
	if (aStepName == _L("TestSetEmgClientLCSReqMulCallFrDiffSubSes"))
		{
		testStep = new CTestSetEmgClientLCSReqMulCallFrDiffSubSes();
		}
		
	if (aStepName == _L("TestSetEmgClientLCSReqMulCallFrDiffClt"))
		{
		testStep = new CTestSetEmgClientLCSReqMulCallFrDiffClt();
		}
			
	if (aStepName == _L("TestSetEmgClientVoiceReqMulCallFrSameSes"))
		{
		testStep = new CTestSetEmgClientVoiceReqMulCallFrSameSes();
		}
		
	if (aStepName == _L("TestSetEmgClientVoiceReqMulCallFrDiffSubSes"))
		{
		testStep = new CTestSetEmgClientVoiceReqMulCallFrDiffSubSes();
		}
		
	if (aStepName == _L("TestSetEmgClientVoiceReqMulCallFrDiffClt"))
		{
		testStep = new CTestSetEmgClientVoiceReqMulCallFrDiffClt();
		}
			
	if (aStepName == _L("TestSetPrClientCapNsidPosCheck"))
		{
		testStep = new CTestSetPrClientCapNsidPosCheck();
		}
		
	if (aStepName == _L("TestSetPrClientCapNsidNegCheck"))
		{
		testStep = new CTestSetPrClientCapNsidNegCheck();
		}	
		
	if (aStepName == _L("TestSetPrClientMulCallFrSameSes"))
		{
		testStep = new CTestSetPrClientMulCallFrSameSes();
		}
		
	if (aStepName == _L("TestSetPrClientMulCallFrDiffSubSes"))
		{
		testStep = new CTestSetPrClientMulCallFrDiffSubSes();
		}
		
	if (aStepName == _L("TestSetPrClientMulCallFrDiffClt"))
		{
		testStep = new CTestSetPrClientMulCallFrDiffClt();
		}
		
	if (aStepName == _L("TestSetPrClientV2CapNsidPosCheck"))
		{
		testStep = new CTestSetPrClientV2CapNsidPosCheck();
		}
		
	if (aStepName == _L("TestSetPrClientV2CapNsidNegCheck"))
		{
		testStep = new CTestSetPrClientV2CapNsidNegCheck();
		}
			
	if (aStepName == _L("TestSetPrClientV2MulCallFrSameSes"))
		{
		testStep = new CTestSetPrClientV2MulCallFrSameSes();
		}
		
	if (aStepName == _L("TestSetPrClientV2MulCallFrDiffSubSes"))
		{
		testStep = new CTestSetPrClientV2MulCallFrDiffSubSes();
		}
		
	if (aStepName == _L("TestSetPrClientV2MulCallFrDiffClt"))
		{
		testStep = new CTestSetPrClientV2MulCallFrDiffClt();
		}	
	if (aStepName == _L("TestPanicAfterRequest"))
		{
		testStep = new CTestPanicAfterRequest();
		}

	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}
		
	return testStep;
	} // CETelTestServer::CreateTestStep
