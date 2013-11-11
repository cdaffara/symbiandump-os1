// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "te_mmsetpriorityclient.h"
#include "testdef.h"

CTestMMSetPrClientPreallocHeap::CTestMMSetPrClientPreallocHeap()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetPrClientPreallocHeap"));
	}
	
enum TVerdict CTestMMSetPrClientPreallocHeap::doTestStepL()
//
// To check even under low memory condition of default Etel process heap ,the APIs using 
// reserved heap doesn't fail
//
	{
	INFO_PRINTF1(_L("To check even under low memory condition of default Etel process heap ,the APIs using reserved heap doesn't fail"));
#ifdef _DEBUG
	TInt ret=iTelServer.SetPriorityClient();
	TEST(ret==KErrNone);
	OOMTest_DialEmergencyCall();
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG	
	return TestStepResult();
	}
	
CTestMMSetPrClientAndSetEmgClient::CTestMMSetPrClientAndSetEmgClient()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetPrClientAndSetEmgClient"));
	}

enum TVerdict CTestMMSetPrClientAndSetEmgClient::doTestStepL()
//
// To test a combination of SetPriorityClient API and SetEmergencyClient API.
//
	{
	INFO_PRINTF1(_L("To test a combination of SetPriorityClient API and SetEmergencyClient API."));
#ifdef _DEBUG	
	TInt ret=iTelServer.SetPriorityClient();
	TEST(ret==KErrNone);
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
    TEST(setemergency==KErrAlreadyExists);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest); "), setemergency);
 	OOMTest_DialEmergencyCall();
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG 	
   	return TestStepResult();
	}

CTestMMSetPrClientV2AndSetEmgClientVoiceReq::CTestMMSetPrClientV2AndSetEmgClientVoiceReq()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetPrClientV2AndSetEmgClientVoiceReq"));
	}

enum TVerdict CTestMMSetPrClientV2AndSetEmgClientVoiceReq::doTestStepL()
//
// To check even under low memory condition of default Etel process heap ,the APIs using 
// reserved heap doesn't fail
//	
	{
	INFO_PRINTF1(_L("To check even under low memory condition of default Etel process heap ,the APIs using reserved heap doesn't fail"));
#ifdef _DEBUG	
	TInt ret=iTelServer.SetPriorityClientV2();
	TEST(ret==KErrNone);
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
    TEST(setemergency==KErrNone);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest); "), setemergency);
 	OOMTest_DialEmergencyCall();
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG 	
  	return TestStepResult();
	}

CTestMMSetPrClientV2AndSetEmgClientLcsReq::CTestMMSetPrClientV2AndSetEmgClientLcsReq()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetPrClientV2AndSetEmgClientLcsReq"));
	}
		
enum TVerdict CTestMMSetPrClientV2AndSetEmgClientLcsReq::doTestStepL()
//
// To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest) API.
//
	{
	INFO_PRINTF1(_L("To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest) API."));
#ifdef _DEBUG	
	TInt ret=iTelServer.SetPriorityClientV2();
	TEST(ret==KErrNone);
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
    TEST(setemergency==KErrNone);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyLCSRequest); "), setemergency);
    OOMTest_LBS();
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG     	
    return TestStepResult();
	}

CTestMMSetPrClientV2AndSetEmgClientVoiceOrLbsReq::CTestMMSetPrClientV2AndSetEmgClientVoiceOrLbsReq()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetPrClientV2AndSetEmgClientVoiceOrLbsReq"));
	}
		
enum TVerdict CTestMMSetPrClientV2AndSetEmgClientVoiceOrLbsReq::doTestStepL()
//
// To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest or Voice request) API.
//
	{
	INFO_PRINTF1(_L("To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest or Voice request) API"));
#ifdef _DEBUG
	TInt ret=iTelServer.SetPriorityClientV2();
	TEST(ret==KErrNone);
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest | RPhone::EEmergencyLCSRequest);
    TEST(setemergency==KErrNone);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest| RPhone::EEmergencyLCSRequest); "), setemergency);
    OOMTest_DialEmergencyCall();
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG     
   	return TestStepResult();
	}
