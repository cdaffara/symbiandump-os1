// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TE_ETELISVLINEINFO.CPP
// 
//

#include "TE_EtelIsvbase.h"
#include "TE_EtelIsvCallFunc.h"
#include "TE_EtelIsvStress.h"
#include "TE_EtelIsvPhoneSettings.h"
#include "TE_EtelIsvNetworkInfo.h"

/**
*
@SYMTestCaseID Etel3rdParty 27.1, TestStress
@SYMTestCaseDesc Stress Test 3rd party APIs
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc,
* REQ 2116.4.2, 2116.4.3, 2116.4.3, 2116.4.4, 2116.4.6, 2116.3.2
*/  
 

CTestStress::CTestStress(TInt& aCallCount)
/** Each test step initialises it's own name
*/
	: iCallCount(aCallCount)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestStress"));
	}
	
	
	
enum TVerdict CTestStress::doTestStepL()
	{
/**
This performs some simple stress testing using multiple clients.
*/	
	TTestData testData1;
	TTestData testData2;
	TBuf<100> testbuf1;
	TBuf<100> testbuf2;	
	
	//
	//Test repeated concurrent calls to GetPhoneId() and GetSignalStrength() from separate clients
	//
	SetTestNumber(35);	
	
	testData1.iTestStep = this;
	testData1.iApiToTest = EGetPhoneIdAPI;
	testData1.iClientId = 1;
	testData1.iTestSuccess = EFalse;	
	testData1.iRepCount = 4;	
	
	
	testData2.iTestStep = this;
	testData2.iApiToTest = EGetSignalStrengthAPI;
	testData2.iClientId = 2;
	testData2.iTestSuccess = EFalse;	
	testData2.iRepCount = 4;
	
	testbuf1 = _L("Get Phone Id");
	testbuf2 = _L("Get Signal Strength");	
		
	RunTestL(testData1,testData2,testbuf1, testbuf2);
	
	//
	//Test repeated concurrent calls to DialNewCall() and GetPhoneId() from separate clients
	//
	testData1.iTestStep = this;
	testData1.iApiToTest = EGetNetworkInfoAPI;
	testData1.iClientId = 1;
	testData1.iTestSuccess = EFalse;	
	testData1.iRepCount = 4;	
	
	
	testData2.iTestStep = this;
	testData2.iApiToTest = EDialNewCallAPI;
	testData2.iClientId = 2;
	testData2.iTestSuccess = EFalse;	
	testData2.iRepCount = 4;
	
	testbuf1 = _L("Get Network Info");
	testbuf2 = _L("Dial New Call");	
		
	RunTestL(testData1,testData2,testbuf1, testbuf2);
	
	iMyAsyncOneShot = CMyAsyncOneShot::NewL();
	iMyAsyncOneShot->StartOneShot();
	CActiveScheduler::Start();
	
	return TestStepResult();
	}
	
void CTestStress::RunTestL(TTestData& aTestData1, TTestData& aTestData2,const TDes& aTestName1,const TDes& aTestName2)
/**
Launch threads and wait for them to complete.  Check the success status of the result.
*/
	{
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;

	_LIT(KClient1ThreadName,"ClientThread1");
	_LIT(KClient2ThreadName,"ClientThread2");

	RThread t1;
	RThread t2;

	TInt res1=t1.Create(KClient1ThreadName,StartThreadFunction,
					    KStackSize,KHeapSize,KMaxHeapSize,&aTestData1);

	TInt res2=t2.Create(KClient2ThreadName,StartThreadFunction,
					    KStackSize,KHeapSize,KMaxHeapSize,&aTestData2);

	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	TBool req1Complete=EFalse;
	TBool req2Complete=EFalse;
	
	if (res1==KErrNone && res2==KErrNone)
		{
		t1.Resume();
		t2.Resume();
		t1.Logon(reqStatus1);
		t2.Logon(reqStatus2);

		while ( req1Complete==EFalse || req2Complete==EFalse)
			{
			User::WaitForAnyRequest();
			if (reqStatus1 != KRequestPending && req1Complete==EFalse)
				{
				TInt res1=reqStatus1.Int();
				TEST1(res1 == KErrNone,ETrue); 
				TEST1(aTestData1.iTestSuccess != EFalse,ETrue); 
				INFO_PRINTF4(_L("%S ClientId=%d TestSuccess = %d"), &aTestName1, aTestData1.iClientId,  aTestData1.iTestSuccess);
				req1Complete=ETrue;
				}
			if (reqStatus2 != KRequestPending && req2Complete==EFalse)
				{
				TInt res2=reqStatus2.Int();
				TEST1(res2 == KErrNone,ETrue);
				TEST1(aTestData2.iTestSuccess != EFalse,ETrue); 
				INFO_PRINTF4(_L("%S  ClientId=%d TestSuccess = %d"), &aTestName2, aTestData2.iClientId, aTestData2.iTestSuccess);
				req2Complete=ETrue;
				}
			}
		
		}
	
	t1.Close();
	t2.Close();
	}
	
TInt CTestStress::StartThreadFunction(TAny* aThreadData)
/**
Thread has started. Check which API thread function is to be tested and set it going.
*/
	{
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;
		
	CActiveScheduler* activeScheduler=new CActiveScheduler();
	if(activeScheduler==NULL)
		return KErrNoMemory;
	CActiveScheduler::Install(activeScheduler);
	
	TTestData* testData = static_cast<TTestData*>(aThreadData);
	
	TInt ret = KErrNone;
	if(testData->iApiToTest == EGetPhoneIdAPI)
		{
		TRAP(ret,GetPhoneIdThreadFunctionL(testData));
		}	
	else if(testData->iApiToTest == EGetSignalStrengthAPI)
		{
		TRAP(ret,GetSignalStrengthThreadFunctionL(testData));	
		}	
	else if(testData->iApiToTest == EDialNewCallAPI	)
		{
		TRAP(ret,MakeCallHangupThreadFunctionL(testData));	
		}
	else if(testData->iApiToTest == EGetNetworkInfoAPI	)
		{
		TRAP(ret,GetCurrentNetworkInfoThreadFunctionL(testData));	
		}
	else
		{
		ret = KErrNotFound;
		}		
		
	delete activeScheduler;
	delete cleanup;
	
	return ret;
	}
	
TInt CTestStress::GetPhoneIdThreadFunctionL(TTestData* aTestData)
/**
Thread function which repeatedly calls GetPhoneId()
*/
	{	
	//Open a new client
	CTelephony* telephony = CTelephony::NewLC();	
	
	//expected data
	_LIT16(manufacturerid, "PSION");
	_LIT16(modelid, "666");
	_LIT16(serialid, "1234567890");
	CTelephony::TPhoneIdV1 ExpectedPhoneId;
	ExpectedPhoneId.iManufacturer=manufacturerid;
	ExpectedPhoneId.iModel=modelid;
	ExpectedPhoneId.iSerialNumber=serialid;
	TRequestStatus expStat=KErrNone;
	CTelephony::TPhoneIdV1Pckg ExpectedPhoneIdPckg(ExpectedPhoneId);
	
	CTestGetPhoneIdAct* GetPhoneIdAct = CTestGetPhoneIdAct::NewLC(aTestData->iTestStep, telephony, EFalse);
	
	for(int i = 0; i < aTestData->iRepCount; i++)
		{
		//start GetPhoneId
		GetPhoneIdAct->GetPhoneId(ExpectedPhoneIdPckg, expStat);
		CActiveScheduler::Start();
		
		//have to perform the etst oursleves as cannot use test execute macros in threads!!	
		aTestData->iTestSuccess = ETrue;
		if(GetPhoneIdAct->iStatus != KErrNone)
			{
			aTestData->iTestSuccess = EFalse;
			}
		if(GetPhoneIdAct->iPhoneId.iManufacturer != ExpectedPhoneId.iManufacturer)
			{
			aTestData->iTestSuccess = EFalse;
			}
		if(GetPhoneIdAct->iPhoneId.iModel != ExpectedPhoneId.iModel)
			{
			aTestData->iTestSuccess = EFalse;
			}
		if(GetPhoneIdAct->iPhoneId.iSerialNumber != ExpectedPhoneId.iSerialNumber)
			{
			aTestData->iTestSuccess = EFalse;
			}
		if(aTestData->iTestSuccess == EFalse)
			{
			break;	//test failed.
			}
		}
	
	CleanupStack::PopAndDestroy(GetPhoneIdAct); //GetPhoneIdAct
	CleanupStack::PopAndDestroy(telephony); //telephony
	return KErrNone;
	}
	
TInt CTestStress::GetSignalStrengthThreadFunctionL(TTestData* aTestData)
/**
Thread function which repeatedly calls GetSignalStrength()
*/
	{
	//Open a new client
	CTelephony* telephony = CTelephony::NewLC();	
	
	//Set expected result values
	TRequestStatus expStat=KErrNone;
	CTelephony::TSignalStrengthV1 ExpSignalStrength;
	ExpSignalStrength.iBar=3;
	ExpSignalStrength.iSignalStrength=-51;
	CTelephony::TSignalStrengthV1Pckg expSignalStrengthPckg(ExpSignalStrength);
	
	CTestGetSignalStrengthAct* GetSignalStrengthAct = CTestGetSignalStrengthAct::NewLC(aTestData->iTestStep, telephony, EFalse);
	
	//start get signal strength
	for(int i = 0; i < aTestData->iRepCount; i++)
		{
		GetSignalStrengthAct->GetSignalStrength(expSignalStrengthPckg, expStat);	
		CActiveScheduler::Start();
		
		aTestData->iTestSuccess = ETrue;
		
		if(GetSignalStrengthAct->iStatus != KErrNone)
			{
			aTestData->iTestSuccess = EFalse;
			}
		if(GetSignalStrengthAct->iSignalStrength.iBar != ExpSignalStrength.iBar)
			{
			aTestData->iTestSuccess = EFalse;
			}		
		if(GetSignalStrengthAct->iSignalStrength.iSignalStrength != ExpSignalStrength.iSignalStrength)
			{
			aTestData->iTestSuccess = EFalse;
			}
		if(aTestData->iTestSuccess == EFalse)
			{
			break;	//test failed.
			}		
		}
	
	CleanupStack::PopAndDestroy(GetSignalStrengthAct); //GetSignalStrengthAct	
	CleanupStack::PopAndDestroy(telephony); //telephony	
	return KErrNone;
	}
	
TInt CTestStress::MakeCallHangupThreadFunctionL(TTestData* aTestData)
/**
Thread function which repeatedly initiates a call and hangups
*/
	{	
	_LIT(KGoodNumber,"1789");
	
	//Open a new client
	CTelephony* telephony = CTelephony::NewLC();	
	
	//Set the call parameters for call 
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams1.iExpStat = KErrNone;
	
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);
	
	// Create active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(aTestData->iTestStep, telephony, EFalse);
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(aTestData->iTestStep, telephony, EFalse);
	
	for(int i = 0; i < aTestData->iRepCount; i++)
		{
		//dial a new call
		testDialNewCallAct1->TestDialNewCall(testCallParams1);
		CActiveScheduler::Start();
		
		aTestData->iTestSuccess = ETrue;
		if(testDialNewCallAct1->iStatus != KErrNone)
			{
			aTestData->iTestSuccess = EFalse;
			}
		
		TInt err = telephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
		if(err != KErrNone)
			{
			aTestData->iTestSuccess = EFalse;
			}
		if(status.iStatus != CTelephony::EStatusConnected)
			{
			aTestData->iTestSuccess = EFalse;
			}
			
		//hangup the call
		testHangupAct1->TestHangup(testCallParams1);
		CActiveScheduler::Start();
		
		err = telephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
		if(err != KErrNone)
			{
			aTestData->iTestSuccess = EFalse;
			}
		if(status.iStatus != CTelephony::EStatusIdle)
			{
			aTestData->iTestSuccess = EFalse;
			}
		
		if(aTestData->iTestSuccess == EFalse)
			{
			break;	//test failed.
			}	
		}
		
	CleanupStack::PopAndDestroy(testHangupAct1);	//testHangupAct1
	CleanupStack::PopAndDestroy(testDialNewCallAct1); //testDialNewCallAct1
	CleanupStack::PopAndDestroy(telephony); //telephony	
	return KErrNone;
	}
	
TInt CTestStress::GetCurrentNetworkInfoThreadFunctionL(TTestData* aTestData)	
/**
Thread function which repeatedly calls GetCurrentNetworkInfo()
*/
	{
	// Set up the expected value to be the same as that provisioned in 
	// the SIM TSY config.txt file.
	_LIT16(KCountrycode, "ABCD");
	_LIT16(KNetworkid, "Network0");
	
	// Need to test display tag
	_LIT16(KDisplaytag, "Vodafone");
	_LIT16(KShortname, "Voda");
	_LIT16(KLongname, "Vodafone_UK");
	
	//Open a new client
	CTelephony* telephony = CTelephony::NewLC();	
	
	CTelephony::TNetworkInfoV1 ExpectedNetworkInfo;
	ExpectedNetworkInfo.iCountryCode=KCountrycode;
	ExpectedNetworkInfo.iNetworkId=KNetworkid;
	ExpectedNetworkInfo.iDisplayTag=KDisplaytag;
	ExpectedNetworkInfo.iShortName=KShortname;
	ExpectedNetworkInfo.iLongName=KLongname;	
	ExpectedNetworkInfo.iMode=CTelephony::ENetworkModeGsm;
	ExpectedNetworkInfo.iStatus = CTelephony::ENetworkStatusCurrent;
	ExpectedNetworkInfo.iBandInfo=CTelephony::EBandUnknown;
	ExpectedNetworkInfo.iCdmaSID.SetLength(0);
	ExpectedNetworkInfo.iAccess = CTelephony::ENetworkAccessGsm; 
	ExpectedNetworkInfo.iAreaKnown = EFalse;
	ExpectedNetworkInfo.iLocationAreaCode = 0;
	ExpectedNetworkInfo.iCellId = 0;
	
	TRequestStatus ExpStat=KErrNone;

	// Create an active object and push it onto the clean up stack
	CTestGetCurrentNetworkInfoAct* GetCurrentNetworkInfoAct = CTestGetCurrentNetworkInfoAct::NewLC(aTestData->iTestStep, telephony, EFalse);
	
	for(int i = 0; i < aTestData->iRepCount; i++)
		{
		GetCurrentNetworkInfoAct->TestGetCurrentNetworkInfo(ExpectedNetworkInfo, ExpStat);
		CActiveScheduler::Start();
	
		aTestData->iTestSuccess = ETrue;
		if(GetCurrentNetworkInfoAct->iStatus != KErrNone)
			{
			aTestData->iTestSuccess = EFalse;
			}			
		if(GetCurrentNetworkInfoAct->iNetworkInfo.iCountryCode != ExpectedNetworkInfo.iCountryCode )
			{
			aTestData->iTestSuccess = EFalse;
			}
		
		if(GetCurrentNetworkInfoAct->iNetworkInfo.iNetworkId != ExpectedNetworkInfo.iNetworkId )
			{
			aTestData->iTestSuccess = EFalse;
			}
		
		if(GetCurrentNetworkInfoAct->iNetworkInfo.iDisplayTag != ExpectedNetworkInfo.iDisplayTag )
			{
			aTestData->iTestSuccess = EFalse;
			}
		
		if(GetCurrentNetworkInfoAct->iNetworkInfo.iShortName != ExpectedNetworkInfo.iShortName )
			{
			aTestData->iTestSuccess = EFalse;
			}
		
		if(GetCurrentNetworkInfoAct->iNetworkInfo.iLongName!= ExpectedNetworkInfo.iLongName )
			{
			aTestData->iTestSuccess = EFalse;
			}					

		if(aTestData->iTestSuccess == EFalse)
			{
			break;	//test failed.
			}	
		}	
	
	CleanupStack::PopAndDestroy(GetCurrentNetworkInfoAct); //GetCurrentNetworkInfoAct
	CleanupStack::PopAndDestroy(telephony); //telephony	
	return KErrNone;
	}


/**
 *  Simple pre-amble that doesn't open CTelephony.
 */
TVerdict CTestFailedOpen::doTestStepPreambleL()
	{
	//
	// Just mark the heap and create an Active Scheduler...
	//
	__UHEAP_MARK;

	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	return TestStepResult();
	} // CTestFailedOpen::doTestStepPreambleL


/**
 *  Test of opening CTelephony without a phone being available.
 */
enum TVerdict CTestFailedOpen::doTestStepL()
	{
	//
	// Set the SIMTSY config to be invalid and open CTelephony.
	//
	User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory,
									  KPSSimTsyTestNumber, 999));
	
	TRAPD(leaveCode, iTelephony = CTelephony::NewL());
	TEST1(leaveCode == KErrNotFound, ETrue);

	delete iTelephony;
	iTelephony = NULL;
	
	return TestStepResult();
	} // CTestFailedOpen::doTestStepL


/**
 *  Simple post-amble that deletes the Active Scheduler.
 */
TVerdict CTestFailedOpen::doTestStepPostambleL()
	{
	delete iScheduler;

	__UHEAP_MARKEND;

 	return TestStepResult();
	} // CTestFailedOpen::doTestStepPostambleL
