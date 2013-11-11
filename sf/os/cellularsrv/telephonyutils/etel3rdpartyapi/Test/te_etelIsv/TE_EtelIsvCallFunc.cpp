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
// Tests for ETEL 3rd Party Call APIs.
// 
//

/**
 @file
*/

#include "TE_EtelIsvbase.h"
#include "TE_EtelIsvCallFunc.h"


CTestDialNewCall::CTestDialNewCall()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestDialNewCall"));
}

/**
*
@SYMTestCaseID Etel3rdParty 9.1
@SYMTestCaseDesc Test DialNewCall 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.1
*/ 

enum TVerdict CTestDialNewCall::doTestStepL()
{
	INFO_PRINTF1(_L("CTestDialNewCall::doTestStepL Start"));
	
	_LIT(KGoodNumber,"1789");
	
	SetTestNumber(15); 
	
	//Set the call parameters for call 1
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams1.iExpStat = KErrNone;
	
	//Set the call parameters for call 2
	CTelephony::TCallParamsV1 callParams2;
	callParams2.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg2(callParams2);	
	
	TTestCallParams testCallParams2;
	testCallParams2.iLine = CTelephony::EVoiceLine;
	testCallParams2.iCallParamsV1Pckg = &callParamsPckg2;
	testCallParams2.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams2.iExpStat = KErrNone;
	
	//Set the call parameters for call 3 - should not successfully make a call
	CTelephony::TCallParamsV1 callParams3;
	callParams3.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg3(callParams3);	
	
	TTestCallParams testCallParams3;
	testCallParams3.iLine = CTelephony::EVoiceLine;
	testCallParams3.iCallParamsV1Pckg = &callParamsPckg3;
	testCallParams3.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams3.iExpStat = KErrAccessDenied;
	
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);
	
	// Create active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
//
//TEST 1: Test making call on fax & data line.  Should fail
//
	
	INFO_PRINTF1(_L("Test making call on fax & data line.  Should fail"));
	testCallParams1.iLine = CTelephony::EDataLine;
	testCallParams1.iExpStat = KErrAccessDenied;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	testCallParams1.iLine = CTelephony::EFaxLine;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();	
	
//
//TEST 2: Test OOM on Dial
//
#if defined(_DEBUG)
	INFO_PRINTF1(_L("Test OOM on for Dial"));
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iExpStat = KErrNoMemory;
	testCallParams1.iCallId = CTelephony::EISVMaxNumOfCalls;	
	__UHEAP_FAILNEXT(1);	
	// Create a call - should fail	
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	__UHEAP_RESET;
	
	__UHEAP_FAILNEXT(2);	 
	// Create a call - should fail
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	__UHEAP_RESET;
#endif
//
//TEST 3: Create & Hangup a single call
//

	INFO_PRINTF1(_L("Create & Hangup a single call"));
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iExpStat = KErrNone;		 
	// Create a call - should succeed
	testCallParams1.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct1->TestDialNewCall(testCallParams1, ETrue);
	CActiveScheduler::Start();
			
	TInt err = iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
	TEST1(err == KErrNone, ETrue);	
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	TEST1(testCallParams1.iCallId == CTelephony::EISVCall1, ETrue);
	TEST1(callParams1.iIdRestrict == CTelephony::ESendMyId, ETrue);
	
	// Create an activeHangupAct object
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(this, iTelephony);

	testCallParams1.iExpStat = KErrNone;
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
	err = iTelephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
	TEST1(err == KErrNone, ETrue);
	TEST1(status.iStatus == CTelephony::EStatusIdle, ETrue);
	
//
//TEST 4: Create 2 calls. try and create a third which should fail
//
	
	INFO_PRINTF1(_L(" Create 2 calls. try and create a third which should fail."));
	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);	
	
	INFO_PRINTF1(_L(" Create first call."));
	testCallParams1.iExpStat = KErrNone;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	err = iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
	TEST1(err == KErrNone, ETrue);	
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	TEST1(testCallParams1.iCallId == CTelephony::EISVCall1, ETrue);
	
	// Create 2nd call - should fail as other call is active
	INFO_PRINTF1(_L(" Create first 2nd call - should fail as 1st call active"));
	testCallParams2.iExpStat = KErrGeneral;
	testDialNewCallAct1->TestDialNewCall(testCallParams2);
	CActiveScheduler::Start();
	
	//put 1st call on hold
	INFO_PRINTF1(_L(" Put 1st call on hold."));
	testCallParams1.iExpStat=KErrNone;
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();	
	
	err = iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
	TEST1(err == KErrNone, ETrue);	
	TEST1(status.iStatus == CTelephony::EStatusHold, ETrue);
	
	// Create 2nd call - should suceed
	INFO_PRINTF1(_L(" Create 2nd call."));
	testCallParams2.iExpStat=KErrNone;
	testDialNewCallAct1->TestDialNewCall(testCallParams2);
	CActiveScheduler::Start();
	
	// Create a 3rd call - should fail
	INFO_PRINTF1(_L(" Create 3rd call - should fail"));
	testCallParams3.iExpStat = KErrAccessDenied;
	testDialNewCallAct1->TestDialNewCall(testCallParams3);
	CActiveScheduler::Start();			
	
	//Hangup calls
	INFO_PRINTF1(_L("Hangup 1st call"));
	testCallParams1.iExpStat = KErrNone;
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
	err = iTelephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
	TEST1(err == KErrNone, ETrue);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	INFO_PRINTF1(_L("Hangup 2nd call"));
	testCallParams1.iExpStat = KErrNone;
	testHangupAct1->TestHangup(testCallParams2);
	CActiveScheduler::Start();
	
	err = iTelephony->GetLineStatus(testCallParams2.iLine,  statusPckg);
	TEST1(err == KErrNone, ETrue);
	TEST1(status.iStatus == CTelephony::EStatusIdle, ETrue);	

//
//TEST 5: test cancel
//
	INFO_PRINTF1(_L(" Test dial cancel."));
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EDialNewCallCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CTestCanceller* canceller = testDialNewCallAct1->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(testDialNewCallAct1->iStatus.Int() == KErrCancel, ETrue);
	
//
//TEST 6: Test re-dialling after a cancel
//
 	INFO_PRINTF1(_L(" Test dial after a cancel."));
 	err = iTelephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusIdle, ETrue);
	
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();	
	
	err = iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	
	//Hangup calls
	INFO_PRINTF1(_L("Hangup call"));
	testCallParams1.iExpStat = KErrNone;
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();	
	
	CleanupStack::PopAndDestroy(TestHoldAct);
	CleanupStack::PopAndDestroy(testHangupAct1);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	
	return TestStepResult();	
}

/**
Dial New Call Active Object
**/
CTestDialNewCallAct* CTestDialNewCallAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
//Factory constructor
	{
	CTestDialNewCallAct* obj = new(ELeave) CTestDialNewCallAct(aTestStep, aTelephony, NULL, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestDialNewCallAct* CTestDialNewCallAct::NewL(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent)
//Factory constructor
	{
	CTestDialNewCallAct* obj = new(ELeave) CTestDialNewCallAct(aTestStep, aTelephony, aParent, ETrue);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop(obj);
	return obj;
	}
	
CTestDialNewCallAct::CTestDialNewCallAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent, TBool aUseMacros)
    : CEtelIsvActBase(aTestStep, aTelephony)
    , iParent(aParent)
    , iUseMacros(aUseMacros) 
	{  
	}

void CTestDialNewCallAct::ConstructL()
	{    
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestDialNewCallAct::~CTestDialNewCallAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestDialNewCallAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EDialNewCallCancel);
	}

void CTestDialNewCallAct::TestDialNewCall(TTestCallParams& aTestCallParams, TBool aCheckConcurrency)
	{
 	iTestCallParams = &aTestCallParams;

	// Update the expected return value.
	iExpStat=aTestCallParams.iExpStat;
	
	TDes8* data;
    
	if (iTestStep->iBufferMode)
 		{
 		// initialize the buffer first
		iCallParamsBuff = *(reinterpret_cast<CTelephony::TCallParamsV1*> ( const_cast<TUint8*> (aTestCallParams.iCallParamsV1Pckg->Ptr() ) ));
		data = &iCallParamsBuff;   
		}
	else
		{
		data = aTestCallParams.iCallParamsV1Pckg;
		}

        
	// Make a new call
	iTelephony->DialNewCall(iStatus, *data ,iTestCallParams->iTelAddress.iTelNumber, iTestCallParams->iCallId, iTestCallParams->iLine);
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->DialNewCall(tmpStatus, *data ,iTestCallParams->iTelAddress.iTelNumber, iTestCallParams->iCallId, iTestCallParams->iLine);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive(); 
	}
	
void CTestDialNewCallAct::RunL()
	{
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	if(iUseMacros)
		{
		iTestStep-> INFO_PRINTF1(_L(" CTestDialNewCallAct::RunL "));
		iTestStep->INFO_PRINTF2(_L("Request Status returned is: %d"), iStatus.Int()   );

		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		}

	if(iStatus.Int() == KErrNone)
		{
		if(iUseMacros)
			{
			iTestStep->INFO_PRINTF1(_L(" KErrNone returned") );
			iTestStep->INFO_PRINTF2(_L(" Call ID is %d"), iTestCallParams->iCallId );	

			CTelephony::TCallStatusV1 status;
			CTelephony::TCallStatusV1Pckg statusPckg(status);

			iTelephony->GetCallStatus(iTestCallParams->iCallId,  statusPckg);
			iTestStep->INFO_PRINTF2(_L(" Call status is %d"), status.iStatus );
			}
		}
	else
		{
		if(iUseMacros)
			{
			iTestStep->INFO_PRINTF2(_L("Request Status expected is: %d"), iExpStat.Int()   ); 
			iTestStep->INFO_PRINTF2(_L("Error for dialnewcall: %d"), iStatus.Int() );
			}
		}
	if(!iParent)
		CActiveScheduler::Stop();
	else
		iParent->ActionParent(iStatus.Int(), MActionParent::EDialNewCall);
	}
	
 
CTestHangup::CTestHangup()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestHangup"));
	}

/**
*
@SYMTestCaseID Etel3rdParty 10.1
@SYMTestCaseDesc Test Hangup 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.3
*/

enum TVerdict CTestHangup::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestHangup::doTestStepL Start"));
	
	_LIT(KGoodNumber,"1789");
	
	SetTestNumber(15); 
	
	//Hangup is been used extensively in other tests.  This test therefore just tests cancelling and trying to 
	//hang-up when a call is not active
	
	//Set the call parameters
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams1.iExpStat = KErrNone;
 
 	// Create an active HangupAct object
	CTestHangupAct* TestHangupAct = CTestHangupAct::NewLC(this, iTelephony);		
	// Create active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
//
//TEST 1: Test Hangup with no active call
//

	//try hanging up when no call active
	testCallParams1.iCallId = CTelephony::EISVCall1;
	testCallParams1.iExpStat = KErrAccessDenied;
	TestHangupAct->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
	INFO_PRINTF1(_L(" Create a call."));
	testCallParams1.iExpStat = KErrNone;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
//
//TEST 2: Test OOM on Hangup
//
#if defined(_DEBUG)
	INFO_PRINTF1(_L("Test OOM on for Hangup"));
	testCallParams1.iExpStat = KErrNoMemory;
	__UHEAP_FAILNEXT(1);	
	TestHangupAct->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	__UHEAP_RESET;
#endif
//
//TEST 3: Test make a call and hangup
//
	
	INFO_PRINTF1(_L("Hangup the call"));
	testCallParams1.iExpStat = KErrNone;
	TestHangupAct->TestHangup(testCallParams1, ETrue);
	CActiveScheduler::Start();
	
//
//TEST 4: Test Cancel
//

	//make a call first
	INFO_PRINTF1(_L(" Create a call."));
	testCallParams1.iExpStat = KErrNone;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();	
	
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EHangupCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	TestHangupAct->TestHangup(testCallParams1);
	CTestCanceller* canceller = TestHangupAct->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();
	TInt result = TestHangupAct->iStatus.Int();
	
	if(result == KErrCancel)
		{
		//call should still be active
		CTelephony::TCallStatusV1 status;
		CTelephony::TCallStatusV1Pckg statusPckg(status);
		TInt err = iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg);
		TEST1(err == KErrNone, ETrue);	
		TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
		//now hangup properly
		TestHangupAct->TestHangup(testCallParams1);
		CActiveScheduler::Start();
		}
	else if (result == KErrNone)
		{
		INFO_PRINTF1(_L("Test Cancel HangUp call: HangUp completed with KErrNone,"));
		INFO_PRINTF1(_L("KErrCancel expected. Test Inconclusive."));
		SetTestStepResult(EInconclusive);
		}
	else
		{
		SetTestStepResult(EFail);	
		}
 
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	CleanupStack::PopAndDestroy(TestHangupAct);
	return TestStepResult();
	}


/**
Hangup Active Object
**/
CTestHangupAct* CTestHangupAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros)
//Factory constructor
	{
	CTestHangupAct* obj = new(ELeave) CTestHangupAct(aTestStep, aTelephony, NULL, aUseMacros);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestHangupAct* CTestHangupAct::NewL(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent)
//Factory constructor
	{
	CTestHangupAct* obj = new(ELeave) CTestHangupAct(aTestStep, aTelephony, aParent, ETrue);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop();//obj
	return obj;
	}
	
CTestHangupAct::CTestHangupAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent, TBool aUseMacros)
	: CEtelIsvActBase(aTestStep, aTelephony)	 
	, iParent(aParent)
    , iUseMacros(aUseMacros)
	{  
	}

void CTestHangupAct::ConstructL()
	{    
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestHangupAct::~CTestHangupAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestHangupAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EHangupCancel);
	}

void CTestHangupAct::TestHangup(TTestCallParams& aTestCallParams, TBool aCheckConcurrency) 
               
	{
 	iTestCallParams = &aTestCallParams;

	// Update the expected return value.
	iExpStat=iTestCallParams->iExpStat;

	// Hangup existing call
	iTelephony->Hangup(iStatus, iTestCallParams->iCallId);
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->Hangup(tmpStatus, iTestCallParams->iCallId);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive(); 
	}
	
void CTestHangupAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	if(iUseMacros)
		{
		iTestStep-> INFO_PRINTF1(_L(" CTestHangupAct::RunL "));
		iTestStep->INFO_PRINTF2(_L("Request Status returned is: %d"), iStatus.Int()   );
		iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
		}
	
	if(iStatus.Int() == KErrNone)
		{
		if(iUseMacros)
			{
			iTestStep->INFO_PRINTF2(_L(" Call ID is %d"), iTestCallParams->iCallId );	 
			}
		}
		else
		{
		if(iUseMacros)
			{
			iTestStep->INFO_PRINTF2(_L("Error for hangup: %d"), iStatus.Int() );
			}
		}
	if(!iParent)
		CActiveScheduler::Stop();
	else
		iParent->ActionParent(iStatus.Int(), MActionParent::EHangup);
	}

CTestAnswerIncomingCall::CTestAnswerIncomingCall(TInt& aCallCount)
/** Each test step initialises it's own name
*/
: iCallCount(aCallCount)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestAnswerIncomingCall"));
	}

/**
*
@SYMTestCaseID Etel3rdParty 11.1
@SYMTestCaseDesc Test AnswerIncomingCall 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.5, INC074753 
*/
enum TVerdict CTestAnswerIncomingCall::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestAnswerIncomingCall::doTestStepL Start"));
	
	SetTestNumber(15); 
	
	_LIT(KGoodNumber,"1789");
	
	//Set the call parameters to receive call 1
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = NULL;
	testCallParams1.iExpStat = KErrNone;
	
	//Set the call parameters for MO call 1
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testMOCallParams1;
	testMOCallParams1.iLine = CTelephony::EVoiceLine;
	testMOCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testMOCallParams1.iTelAddress.iTelNumber = KGoodNumber;
	testMOCallParams1.iExpStat = KErrNone; 
	
	
	//Set the call parameters for MO call 2
	CTelephony::TCallParamsV1 callParams2;
	callParams2.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg2(callParams2);	
	
	TTestCallParams testMOCallParams2;
	testMOCallParams2.iLine = CTelephony::EVoiceLine;
	testMOCallParams2.iCallParamsV1Pckg = &callParamsPckg2;
	testMOCallParams2.iTelAddress.iTelNumber = KGoodNumber;
	testMOCallParams2.iExpStat = KErrNone; 
	
	// Create an active AnswerIncomingCallAct object
	CTestAnswerIncomingCallAct* testAnswerIncomingCallAct1 = CTestAnswerIncomingCallAct::NewLC(this, iTelephony);
	
//
//TEST 1: Test answering call on fax & data line.  Should fail
//
	
	INFO_PRINTF1(_L("Test answering call on fax & data line.  Should fail"));
	testCallParams1.iLine = CTelephony::EDataLine;
	testCallParams1.iExpStat = KErrAccessDenied;
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1);
	CActiveScheduler::Start();
	
	testCallParams1.iLine = CTelephony::EFaxLine;
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1);
	CActiveScheduler::Start();	

//
//TEST 2: Test AnswerIncomingCall with no ringing call
//
	INFO_PRINTF1(_L("Test answering call with no ringing"));
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iExpStat = KErrAccessDenied;
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1);
	CActiveScheduler::Start();
	
	//initiate callback
	testAnswerIncomingCallAct1->CallMe(iCallCount);
	 
//
//TEST 2: Test OOM on AnswerIncomingCall
//
#if defined(_DEBUG)
	INFO_PRINTF1(_L("Test answering call with OOM"));
	testCallParams1.iExpStat = KErrNoMemory;
	__UHEAP_FAILNEXT(1);
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1);
	CActiveScheduler::Start();
	__UHEAP_RESET;
#endif
 	
//
//TEST 3: Test answering call & hanging up on voice line 
//	
	INFO_PRINTF1(_L("Test answering call with hangup"));

	CTelephony::TCallInfoV1 callInfoV1;
	CTelephony::TCallInfoV1Pckg callInfoV1Pckg(callInfoV1);
	CTelephony::TRemotePartyInfoV1 remoteInfoV1;
	CTelephony::TRemotePartyInfoV1Pckg remoteInfoV1Pckg(remoteInfoV1);
	
	CTelephony::TCallSelectionV1 callSelection;
	CTelephony::TCallSelectionV1Pckg callSelectionV1Pckg(callSelection);
	callSelection.iSelect = CTelephony::EInProgressCall;
	callSelection.iLine = CTelephony::EVoiceLine;
	
	TInt err = iTelephony->GetCallInfo(callSelectionV1Pckg, callInfoV1Pckg, remoteInfoV1Pckg);

	testCallParams1.iExpStat = KErrNone;
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1, ETrue);
	CActiveScheduler::Start();
	TEST1(testCallParams1.iCallId == CTelephony::EISVCall1, ETrue);
		
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);		
	iTelephony->GetLineStatus(testCallParams1.iLine, statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	iTelephony->GetCallStatus(testCallParams1.iCallId, statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	//and hangup
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(this, iTelephony);		
	
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
//
//TEST 4: Test dialling a call & answering a 2nd call
//	
	INFO_PRINTF1(_L("Test dial a call & answering call."));
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	INFO_PRINTF1(_L("Create a single call"));
	// Create a call - should succeed
	testMOCallParams1.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct1->TestDialNewCall(testMOCallParams1);
	CActiveScheduler::Start();
	
	iTelephony->GetCallStatus(testMOCallParams1.iCallId, statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	//initiate callback
	testAnswerIncomingCallAct1->CallMe(iCallCount);
	
	iTelephony->GetLineStatus(testMOCallParams1.iLine,  statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusRinging, ETrue);
	
	//put 1st call on hold
	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);	
	INFO_PRINTF1(_L(" Put 1st call on hold."));
	testCallParams1.iExpStat=KErrNone;
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(TestHoldAct);
	
	iTelephony->GetCallStatus(testMOCallParams1.iCallId, statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusHold, ETrue);
	
	//answer a call
	testCallParams1.iExpStat = KErrNone;
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1);
	CActiveScheduler::Start();
	
	iTelephony->GetCallStatus(testCallParams1.iCallId, statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	//try and make a 3rd call - should fail
	testMOCallParams2.iExpStat = KErrAccessDenied;
	testDialNewCallAct1->TestDialNewCall(testMOCallParams2);
	CActiveScheduler::Start();
		
	//Hangup calls
	testCallParams1.iExpStat = KErrNone;
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
	testHangupAct1->TestHangup(testMOCallParams1);
	CActiveScheduler::Start();	
	
//
//TEST 5: Test Cancel
//
	INFO_PRINTF1(_L("Test answering call cancel"));
	
	//initiate callback
	testAnswerIncomingCallAct1->CallMe(iCallCount);	
	
	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EAnswerIncomingCallCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1);
	CTestCanceller* canceller = testAnswerIncomingCallAct1->RetrieveCanceller();
	canceller->Call();
	CActiveScheduler::Start();	
	TEST1(testAnswerIncomingCallAct1->iStatus.Int() == KErrNone, ETrue);
	
	iTelephony->GetCallStatus(testCallParams1.iCallId, statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	//NOTE: Attempting to Cancel an AnswerIncomingCall fails.  The specs say you can disconnect from
	//an answering state but this is not implemented in the SIM TSY which means the cancel request fails.
	
	//Hangup calls
	testCallParams1.iExpStat = KErrNone;
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
	
//
//TEST 6: Test Answer call after a cancel
//
	INFO_PRINTF1(_L("Test answering call after a cancel"));
	
	//initiate callback
	testAnswerIncomingCallAct1->CallMe(iCallCount);	
	
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1);
	CActiveScheduler::Start();	
	
	iTelephony->GetCallStatus(testCallParams1.iCallId, statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	//Hangup calls
	testCallParams1.iExpStat = KErrNone;
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();

	
//
//TEST 7: Test Answer call when no notification of incoming call received
//
	INFO_PRINTF1(_L("Test answering call when no notification of incoming call received."));
 
 	SetTestNumber(38);
 
	//Set the call parameters for call
	CTelephony::TCallParamsV1 callParams3;
	//callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg3(callParams3);	
	
	TTestCallParams testCallParams3;
	testCallParams3.iLine = CTelephony::EVoiceLine;
	testCallParams3.iCallParamsV1Pckg = &callParamsPckg3;
	testCallParams3.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams3.iExpStat = KErrTimedOut; 

	CTelephony::TCallStatusV1 status2;
	CTelephony::TCallStatusV1Pckg statusPckg2(status2);		
	iTelephony->GetLineStatus(testCallParams3.iLine, statusPckg2);
	TEST1(status2.iStatus == CTelephony::EStatusIdle, ETrue);
 	
 	// Initiate callback
	testAnswerIncomingCallAct1->CallMe(iCallCount);
	
	iTelephony->GetLineStatus(testCallParams3.iLine,  statusPckg2);
	TEST1(status2.iStatus == CTelephony::EStatusRinging, ETrue);
 	
 	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams3);
 	CActiveScheduler::Start();

//
//TEST 8: Test Answer call when notify incoming call completes 
//        when answer incoming call is waiting for it. 
//
 	
 	// NOTE - This test tries to force the incoming call notification to complete
 	// when AnswerIncomingCall is doing its one second wait. The 
 	// NotifyIncomingCallPauseDuration in the config file may have to be adjusted to make 
 	// sure this happens on your computer.  	
 	INFO_PRINTF1(_L("Test answering call when answer incoming call needs to wait for the call name"));
	INFO_PRINTF1(_L("NOTE: This test forces simtsy to delay sending the incoming call notification"));
	INFO_PRINTF1(_L("(which provides the call name) when an incoming call arrives."));
	INFO_PRINTF1(_L("CAnswerIncomingCallAct will wait up to one second it. This test will fail if"));
	INFO_PRINTF1(_L("the computer running this test is running slowly causing the notification to arrive"));
	INFO_PRINTF1(_L("too late."));
 	
 	SetTestNumber(39);
 	
 	testCallParams3.iExpStat = KErrNone;
 	
 	 // Initiate callback
	testAnswerIncomingCallAct1->CallMe(iCallCount);
	
	iTelephony->GetLineStatus(testCallParams3.iLine,  statusPckg2);
	TEST1(status2.iStatus == CTelephony::EStatusRinging, ETrue);
 	
 	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams3);
 	CActiveScheduler::Start();	
 	 
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	CleanupStack::PopAndDestroy(testHangupAct1); 
	CleanupStack::PopAndDestroy(testAnswerIncomingCallAct1); //testAnswerIncomingCallAct1
	return TestStepResult();	
	}

 
CTestAnswerIncomingCallAct* CTestAnswerIncomingCallAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestAnswerIncomingCallAct* obj = new(ELeave) CTestAnswerIncomingCallAct(aTestStep, aTelephony, NULL);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestAnswerIncomingCallAct* CTestAnswerIncomingCallAct::NewL(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent)
//Factory constructor
	{
	CTestAnswerIncomingCallAct* obj = new(ELeave) CTestAnswerIncomingCallAct(aTestStep, aTelephony, aParent);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop(obj);
	return obj;
	}
		
CTestAnswerIncomingCallAct::CTestAnswerIncomingCallAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent)
	: CEtelIsvActBase(aTestStep, aTelephony)
	, iParent(aParent)	 
	{  
	}

void CTestAnswerIncomingCallAct::ConstructL()
	{    
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestAnswerIncomingCallAct::~CTestAnswerIncomingCallAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestAnswerIncomingCallAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EAnswerIncomingCallCancel);
	}

void CTestAnswerIncomingCallAct::TestAnswerIncomingCall(TTestCallParams& aTestCallParams, TBool aCheckConcurrency) 
	{
	// Update the expected return value.
	iTestCallParams = &aTestCallParams;

	// Update the expected return value.
	iExpStat=aTestCallParams.iExpStat;

	// Answer incoming call
	iTelephony->AnswerIncomingCall(iStatus, iTestCallParams->iCallId, iTestCallParams->iLine);
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->AnswerIncomingCall(tmpStatus, iTestCallParams->iCallId, iTestCallParams->iLine);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive(); 
	}
	
void CTestAnswerIncomingCallAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	iTestStep->INFO_PRINTF1(_L(" CTestAnswerIncomingCallAct::RunL "));
	iTestStep->INFO_PRINTF2(_L("Request Status returned is: %d"), iStatus.Int()   );
	iTestStep->INFO_PRINTF2(_L("Expected Status: %d"), iExpStat.Int());
		
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);	
	
	if(iStatus.Int() == KErrNone)
		{
		iTestStep->INFO_PRINTF1(_L(" KErrNone returned") );
		iTestStep->INFO_PRINTF2(_L(" Call ID = %d"), iTestCallParams->iCallId);	 
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("Error for AnswerIncomingCall: %d"), iStatus.Int() );
		}

	if(!iParent)
		CActiveScheduler::Stop();
	else
		iParent->ActionParent(iStatus.Int(), MActionParent::EAnswerIncomingCall);
	}

CTestHold::CTestHold()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestHold"));
}

/**
*
@SYMTestCaseID Etel3rdParty 12.1
@SYMTestCaseDesc Test Hold 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.7
*/
  
enum TVerdict CTestHold::doTestStepL()
{
	INFO_PRINTF1(_L("CTestHold::doTestStepL Start"));
	
	_LIT(KCallNumber1,"1789");
	
	// Set mapping to config.txt 
	SetTestNumber(15); 
	
	//Set the call parameters for a basic call. 
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KCallNumber1;
	testCallParams1.iExpStat = KErrNone; 
	
	//
	//
	// TEST 1: OOM error case
	// Make a call. Put the call on hold. 
	//
	//
	
	INFO_PRINTF1(_L("HOLD TEST 1 Make call.Set OOM condition. Attempt to put call on hold."));
	
	// Create an active DialNewCallAct object
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
	// Create an active TestHoldAct object
	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);	
	  
	// Create a call
	testCallParams1.iExpStat=KErrNone;	
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	//while call is active test OOM before first hold call
	#if defined(_DEBUG)
	INFO_PRINTF1(_L("Test OOM on for Hold"));
	testCallParams1.iExpStat = KErrNoMemory;
	__UHEAP_FAILNEXT(1);	
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start(); 
	__UHEAP_RESET;
	#endif
	
	INFO_PRINTF1(_L("END TEST 1"));

	//
	//
	// TEST 2: Normal case
	// Make a call. Put the call on hold. 
	//
	//

	INFO_PRINTF1(_L("HOLD TEST 2 Call active. Hold call. "));
	
	// Put the call on hold.
	// by calling the ETEL 3rd party API
	  
	testCallParams1.iExpStat=KErrNone;
	TestHoldAct->TestHold(testCallParams1, ETrue);
	CActiveScheduler::Start();
		 
 
	// Check that the call is now at held status.
	CTelephony::TCallStatusV1 CallStatusV1;
	CTelephony::TCallStatusV1Pckg CallStatusPckg(CallStatusV1);
	TInt err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);	
	 
	INFO_PRINTF1(_L("END TEST 2"));
	
	//
	//
	// TEST 3: Error case
	// Call is already on hold. Try to perform a hold on the call.
	//
	//
	
	INFO_PRINTF1(_L("HOLD TEST 3. Call already held. Call hold API.")); 
	
	// Call 3rd Party hold API
	testCallParams1.iExpStat=KErrAccessDenied;
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();
			 
	// Make sure call is still in held status
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);	
	
	INFO_PRINTF1(_L("END TEST 3"));
	
	//
	//
	// TEST 4: Error case
	// Hang up a call. Try to put the call on hold.
	//
	//
	
	INFO_PRINTF1(_L("HOLD TEST 4 Hangup call. Try to put on hold."));
	
	// Call hangup 3rd party API
	testCallParams1.iExpStat=KErrNone;	
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(this, iTelephony);	
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();	
 
	// Make sure the line is idle
	err = iTelephony->GetLineStatus(testCallParams1.iLine, CallStatusPckg);
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusIdle,1);	
 
 	// Attempt to hold call. Should fail. 
	testCallParams1.iExpStat=KErrAccessDenied;	 
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	// Make sure call is still in idle status
	err = iTelephony->GetLineStatus(testCallParams1.iLine, CallStatusPckg);
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusIdle,1);	
	
	INFO_PRINTF1(_L("END TEST 4"));	
	
	//
	//
	// TEST 5: Test cancel
	// Dial call. Make a request to hold call. Attempt to cancel 
	// the request.
	//
	//
	
	INFO_PRINTF1(_L("TEST5 Hold call. Cancel hold request"));
	
	// Make a new call
	testCallParams1.iExpStat=KErrNone;		
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();

	// Request the call to be held then
 	// attempt to cancel the hold request
 	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EHoldCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
 	testCallParams1.iExpStat=KErrNone;	
	CTestCanceller* canceller = TestHoldAct->RetrieveCanceller();
	TestHoldAct->TestHold(testCallParams1);
	canceller->Call();
	CActiveScheduler::Start();	
	
	// Status will be KErrCancel if the hold request has been cancelled.
	// It may be KErrNone if the hold request has managed to complete before 
	// execution of the cancel.
	TEST1(((TestHoldAct->iStatus.Int() == KErrCancel) || 
	        (TestHoldAct->iStatus.Int() == KErrNone)), 1);

	INFO_PRINTF1(_L("END TEST 5"));	
	
	
	// Hang up the call for completion.
	testCallParams1.iExpStat=KErrNone;	
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();		
 
	CleanupStack::PopAndDestroy(testHangupAct1);
	CleanupStack::PopAndDestroy(TestHoldAct);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);	
	
	return TestStepResult();
	
}

 
CTestHoldAct* CTestHoldAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestHoldAct* obj = new(ELeave) CTestHoldAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestHoldAct::CTestHoldAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony)	 
	{  
	}

void CTestHoldAct::ConstructL()
	{    
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestHoldAct::~CTestHoldAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestHoldAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EHoldCancel);
	}

void CTestHoldAct::TestHold(TTestCallParams& aTestCallParams, TBool aCheckConcurrency) 
	{
	// Update the expected return value.
	iExpStat=aTestCallParams.iExpStat;
	 
	iTelephony->Hold(iStatus, aTestCallParams.iCallId);
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->Hold(tmpStatus, aTestCallParams.iCallId);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive(); 
 
	}
	
void CTestHoldAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	
	iTestStep-> INFO_PRINTF1(_L(" CTestHoldAct::RunL "));
	iTestStep->INFO_PRINTF2(_L("Request Status expected is: %d"), iExpStat.Int()   );
	iTestStep->INFO_PRINTF2(_L("Request Status returned is: %d"), iStatus.Int()   );
	
 	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), 1);	
	CActiveScheduler::Stop();
	}



CTestResume::CTestResume()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestResume"));
}

/**
@SYMTestCaseID Etel3rdParty 13.1, TestResume
@SYMTestCaseDesc Test Resume 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.8
*/

enum TVerdict CTestResume::doTestStepL()
{ 
	
	 INFO_PRINTF1(_L("CTestResume::doTestStepL Start"));
	
	_LIT(KCallNumber1,"1789");
	
	SetTestNumber(15); 
	
	//Set the call parameters  
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KCallNumber1;
	testCallParams1.iExpStat = KErrNone;
	
	//
	//
	// TEST 1: OOM error scenario
	// Make a call. Put the call on hold. Attempt to resume the call.
	//
	//
	
	INFO_PRINTF1(_L("RESUME TEST 1 Make call. Put call on hold. Test OOM condition."));
	
	// Create the active objects for a basic call
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	CTestHoldAct* testHoldAct1 = CTestHoldAct::NewLC(this, iTelephony);
	CTestResumeAct* testResumeAct1 = CTestResumeAct::NewLC(this, iTelephony);	
	  
	// Make a call
	testCallParams1.iExpStat = KErrNone; 
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	// Put the call on hold.
	// using the ETEL 3rd party API
	testCallParams1.iExpStat = KErrNone;		
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();

	//while call is on hold test OOM before first resume call
	#if defined(_DEBUG)
		INFO_PRINTF1(_L("Test OOM on for Resume"));
		testCallParams1.iExpStat = KErrNoMemory;
		__UHEAP_FAILNEXT(1);	
		testResumeAct1->TestResume(testCallParams1);
		CActiveScheduler::Start(); 
		__UHEAP_RESET;
	#endif

	INFO_PRINTF1(_L("END TEST 1"));	 
	
	//
	//
	// TEST 2: Normal scenario
	// Call is at hold status. Resume the call.
	//
	//
	
	INFO_PRINTF1(_L("RESUME TEST 2 Call held. Resume call."));
	
	// Check that the call is now at held status.
	CTelephony::TCallStatusV1 CallStatusV1;
	CTelephony::TCallStatusV1Pckg CallStatusPckg(CallStatusV1);
	TInt err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold, 1);
	
	// Resume the call using the ETEL 3rdparty API
	testCallParams1.iExpStat = KErrNone;		
	testResumeAct1->TestResume(testCallParams1, ETrue);
	CActiveScheduler::Start();
 
 	// Check that the call is now at active status.
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
		
	INFO_PRINTF1(_L("END TEST 2"));	 
	
	//
	//
	// TEST 3: Error scenario
	// Call is already active. Try to perform a resume on the call.
	//
	//
	
	INFO_PRINTF1(_L("RESUME TEST 3 Attempt to resume an active call"));
	 
	// Call is currently active from previous test
	// Call resume 3rd party API
	testCallParams1.iExpStat=KErrAccessDenied;
	testResumeAct1->TestResume(testCallParams1);
	CActiveScheduler::Start();		
	 
	// Make sure call is still in connected status
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	INFO_PRINTF1(_L("END TEST 3"));	 
	
	//
	//
	// TEST 4: Error scenario
	// Hang up a call. Try to resume the call.
	//
	//
	
	INFO_PRINTF1(_L("RESUME TEST 4 Attempt to resume an idle call"));
	 
	// Hangup the active call
	testCallParams1.iExpStat=KErrNone;
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(this, iTelephony);	
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();	
	
	// Make sure the line is idle
	err = iTelephony->GetLineStatus(testCallParams1.iLine, CallStatusPckg);
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusIdle,1);	
 
 	// Attempt to resume call. Should fail. 
	testCallParams1.iExpStat=KErrAccessDenied;	 
	testResumeAct1->TestResume(testCallParams1);
	CActiveScheduler::Start();
	
	// Make sure call is still in idle status
	err = iTelephony->GetLineStatus(testCallParams1.iLine, CallStatusPckg);
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusIdle,1);	
	
	INFO_PRINTF1(_L("END TEST 4"));	 
	
	//
	//
	// TEST 5: Cancel
	// Resume call. Attempt to cancel the request.
	//
	//
	
	INFO_PRINTF1(_L("RESUME TEST 5 Attempt to cancel a resume request"));
	
	// Create a new call
	testCallParams1.iExpStat=KErrNone; 	 
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	// Hold the call
	testCallParams1.iExpStat=KErrNone; 	
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();

	
	// Request the call to be resumed then
 	// attempt to cancel the resume request
 	TInt cancelerr = iTelephony->CancelAsync(CTelephony::EResumeCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
 	testCallParams1.iExpStat=KErrNone;	
	CTestCanceller* canceller = testResumeAct1->RetrieveCanceller();
	testResumeAct1->TestResume(testCallParams1);
	canceller->Call();
	CActiveScheduler::Start();	
	
	// Status will be KErrCancel if the hold request has been cancelled.
	// It may be KErrNone if the resume request has managed to complete before 
	// execution of the cancel.
	TEST1(((testResumeAct1->iStatus.Int() == KErrCancel) || 
	        (testResumeAct1->iStatus.Int() == KErrNone)), 1);
	        

	INFO_PRINTF1(_L("END TEST 5"));	 
	
	// Hang up the call for completion.
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();	
	
	CleanupStack::PopAndDestroy(testHangupAct1);
	CleanupStack::PopAndDestroy(testResumeAct1);
	CleanupStack::PopAndDestroy(testHoldAct1);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	return TestStepResult();
	
}

 
CTestResumeAct* CTestResumeAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestResumeAct* obj = new(ELeave) CTestResumeAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestResumeAct::CTestResumeAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony) 
	{  
	}

void CTestResumeAct::ConstructL()
	{    
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestResumeAct::~CTestResumeAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestResumeAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EResumeCancel);
	}
 
void  CTestResumeAct::TestResume(TTestCallParams& aTestCallParams, TBool aCheckConcurrency)
	{

	// Update the expected return value.
	iExpStat=aTestCallParams.iExpStat;
	 
	// Call the 3rd party API
	iTelephony->Resume(iStatus, aTestCallParams.iCallId);
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->Resume(tmpStatus, aTestCallParams.iCallId);
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive(); 	
	 	
	}
	
void CTestResumeAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	iTestStep-> INFO_PRINTF1(_L(" CTestResumeAct::RunL "));
	 
	iTestStep->INFO_PRINTF2(_L(" Call ID is = %d"), iCallId );
	iTestStep->INFO_PRINTF2(_L("Request Status expected is: %d"), iExpStat.Int()   );
	iTestStep->INFO_PRINTF2(_L("Request Status returned is: %d"), iStatus.Int()   );
	
 	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), 1);	
	CActiveScheduler::Stop();
	}



CTestSwap::CTestSwap()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSwap"));
}


/**
@SYMTestCaseID Etel3rdParty 14.1, TestSwap
@SYMTestCaseDesc Test Swap 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.20
*/

enum TVerdict CTestSwap::doTestStepL()
{
	INFO_PRINTF1(_L("CTestSwap::doTestStepL Start"));
	
	_LIT(KCallNumber1,"1789");
	_LIT(KCallNumber2,"1234");
	
	 // Set the call parameters for call 1
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
 	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KCallNumber1;
	testCallParams1.iExpStat = KErrNone;
	
	// Set the call parameters for call 2
	CTelephony::TCallParamsV1 callParams2;
	callParams2.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg2(callParams2);	
	
	TTestCallParams testCallParams2;
	testCallParams2.iLine = CTelephony::EVoiceLine;
	testCallParams2.iCallParamsV1Pckg = &callParamsPckg2;
	testCallParams2.iTelAddress.iTelNumber = KCallNumber2;
	testCallParams2.iExpStat = KErrNone;
 	
	// Create 2 active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);	
	CTestDialNewCallAct* testDialNewCallAct2 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
	// Create hold, resume and swap active objects.
	CTestHoldAct* testHoldAct1 = CTestHoldAct::NewLC(this, iTelephony);	
	CTestResumeAct* testResumeAct1 = CTestResumeAct::NewLC(this, iTelephony);
	CTestSwapAct* testSwapAct1 = CTestSwapAct::NewLC(this, iTelephony);	
 		
	//
	//
	// Test Scenario 1: Normal scenario
	// Make call 1. Put call 1 on hold. Make a 2nd call. 
	// Swap the calls.
	//
	//
	
	INFO_PRINTF1(_L("SWAP TEST 1 Swap 2 calls"));
	
	// Create the first call
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	// Hold the first call
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	// Create a 2nd call
	testDialNewCallAct2->TestDialNewCall(testCallParams2);
	CActiveScheduler::Start();	
		 
	// Check the calls are now in the expected states
	CTelephony::TCallStatusV1 CallStatusV1;
	CTelephony::TCallStatusV1Pckg CallStatusPckg(CallStatusV1);
	
	// Make sure call 1 is in the held state  
	TInt err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Make sure call 2 is in the connected state  
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	//while calls are active test OOM before first swap call
	#if defined(_DEBUG)
		INFO_PRINTF1(_L("Test OOM on for Swap"));
		testCallParams1.iExpStat = KErrNoMemory;
		__UHEAP_FAILNEXT(1);	
		testSwapAct1->TestSwap(testCallParams1, testCallParams2 );
		CActiveScheduler::Start(); 
		__UHEAP_RESET;
	#endif
	
 	// Swap the calls 
    testCallParams1.iExpStat=KErrNone; 	 
	testSwapAct1->TestSwap(testCallParams1, testCallParams2, ETrue);
	CActiveScheduler::Start(); 
	
	// Check that the call states are now reversed.
	
	// Make sure call 1 is in the connected state 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	// Make sure call 2 is in the held state  
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1); 
	
	INFO_PRINTF1(_L("END TEST 1"));	 
	
	//
	//
	// TEST 2: Normal scenario
	// Swap the calls back.
	//
	//
	
	INFO_PRINTF1(_L("SWAP TEST 2: Swap calls back"));
	
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start(); 
	
	// Check that the call states are now reversed.
	
	// Make sure call 1 is in the held state 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Make sure call 2 is in the connected state  
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1); 
	
	INFO_PRINTF1(_L("END TEST 2"));	 
	
	//
	// 
	// TEST 3 Error scenario
	// Call 1 is on hold. Call 2 is idle. Try to swap the calls.
	//
	//
	
	INFO_PRINTF1(_L("SWAP TEST 3  Call 1 on hold, call 2 idle. Attempt swap"));
	
	// Preparation step for the test.
	// Hang up Call 2
	 
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(this, iTelephony);	
	testHangupAct1->TestHangup(testCallParams2);
	CActiveScheduler::Start();	
	
	 
	// Make sure call 1 is in the held state 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	 
	// Attempt to swap the calls
	testCallParams1.iExpStat=KErrAccessDenied; 	
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start(); 
	
	// Check call statuses are unchanged as expected
	
	// Make sure call 1 is in the held state 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	
	INFO_PRINTF1(_L("END TEST 3"));	 
	
	
	//
	//
	// TEST 4 : Error scenario
	// Call 1 active. Call 2 idle. Swap the calls.
	//
	//
	
	INFO_PRINTF1(_L("SWAP TEST 4: Call 1 active, call 2 idle, swap calls"));
	
	// Preparation for test 
	// Make call 1 active.
	testCallParams1.iExpStat=KErrNone; 	
	testResumeAct1->TestResume(testCallParams1);
	CActiveScheduler::Start();	
	
	// Make sure call 1 is in the active state
	iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected, ETrue);	
	 
	 
	// Call 1 is active. Call 2 is idle. Try to swap the calls.
	testCallParams1.iExpStat=KErrAccessDenied; 	
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start();
	
	
	// Check that call states are unchanged as expected
	
	// Make sure call 1 is in the connected state 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	INFO_PRINTF1(_L("END TEST 4"));	 
	
	
	//
	//
	// TEST 5: Cancel
	// Attempt to cancel a swap request
	//
	//
	
	INFO_PRINTF1(_L("SWAP TEST 5 : Cancel a swap request"));
	
	// Put call 1 on hold. 
	testCallParams1.iExpStat=KErrNone; 	
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	// Make call 2
	testCallParams2.iExpStat=KErrNone; 	
	testDialNewCallAct2->TestDialNewCall(testCallParams2);
	CActiveScheduler::Start();	
	
	// Issue a swap request
	testCallParams1.iExpStat=KErrNone; 	
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start();
	
	
	// Request the call to be swapped then
 	// attempt to cancel the swap request
 	TInt cancelerr = iTelephony->CancelAsync(CTelephony::ESwapCancel);
	TEST1(cancelerr == KErrNotFound, ETrue);
 	testCallParams1.iExpStat=KErrNone;	
	CTestCanceller* canceller = testSwapAct1->RetrieveCanceller();
	testSwapAct1->TestSwap(testCallParams1, testCallParams2 );
	canceller->Call();
	CActiveScheduler::Start();	
	// Status will be KErrCancel if the swap request has been cancelled.
	// It may be KErrNone if the swap request has managed to complete before 
	// execution of the cancel.
	TEST1(((testSwapAct1->iStatus.Int() == KErrCancel) || 
	        (testSwapAct1->iStatus.Int() == KErrNone)), 1);
	        
	
	INFO_PRINTF1(_L("END TEST 5"));	 
		
		
	// Hang up the calls for completion
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();	
	 
	testHangupAct1->TestHangup(testCallParams2);
	CActiveScheduler::Start();			 
	 
	CleanupStack::PopAndDestroy(testHangupAct1);
	CleanupStack::PopAndDestroy(testSwapAct1);
	CleanupStack::PopAndDestroy(testResumeAct1);
	CleanupStack::PopAndDestroy(testHoldAct1);
	CleanupStack::PopAndDestroy(testDialNewCallAct2);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	
	return TestStepResult();
	
}

 
CTestSwapAct* CTestSwapAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestSwapAct* obj = new(ELeave) CTestSwapAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestSwapAct::CTestSwapAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony)	 
	{  
	}

void CTestSwapAct::ConstructL()
	{    
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CTestSwapAct::~CTestSwapAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CTestSwapAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::ESwapCancel);
	}

void CTestSwapAct::TestSwap(TTestCallParams& aTestCallParams1, TTestCallParams& aTestCallParams2, TBool aCheckConcurrency)	  
               
	{
	// Update the expected return value.
	iExpStat=aTestCallParams1.iExpStat;
	 
	iTelephony->Swap(iStatus, aTestCallParams1.iCallId, aTestCallParams2.iCallId );
	if(aCheckConcurrency)
		{
		TRequestStatus tmpStatus;
		iTelephony->Swap(tmpStatus, aTestCallParams1.iCallId, aTestCallParams2.iCallId );
		User::WaitForRequest(tmpStatus);
		iTestStep->TEST1(tmpStatus.Int() == KErrInUse, ETrue);
		}
	SetActive(); 	
	 
	}
	
void CTestSwapAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	
	iTestStep-> INFO_PRINTF1(_L(" CTestSwapAct::RunL "));
	iTestStep->INFO_PRINTF2(_L("Request Status expected is: %d"), iExpStat.Int()   );
	iTestStep->INFO_PRINTF2(_L("Request Status returned is: %d"), iStatus.Int()   );
	
 	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), 1);	
	CActiveScheduler::Stop();
	}

 
 
CTestHoldResume2Calls::CTestHoldResume2Calls()
/** Each test step initialises it's own name
*/
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestHoldResume2Calls"));
}

/**
@SYMTestCaseID Etel3rdParty 15.1, TestHoldResume2Calls
@SYMTestCaseDesc Test Hold, Resume 3rd party APIs
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.7, 2116.3.8
*/

enum TVerdict CTestHoldResume2Calls::doTestStepL()
{
	INFO_PRINTF1(_L("CTestHoldResume2Calls::doTestStepL Start"));
	
	_LIT(KGoodNumber1,"1789");
	_LIT(KGoodNumber2,"1234");
	
	SetTestNumber(15); 
	
	//Set the call parameters for call 1
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KGoodNumber1;
	testCallParams1.iExpStat = KErrNone;
	
	//Set the call parameters for call 2
	CTelephony::TCallParamsV1 callParams2;
	callParams2.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg2(callParams2);	
	
	TTestCallParams testCallParams2;
	testCallParams2.iLine = CTelephony::EVoiceLine;
	testCallParams2.iCallParamsV1Pckg = &callParamsPckg2;
	testCallParams2.iTelAddress.iTelNumber = KGoodNumber2;
	testCallParams2.iExpStat = KErrNone;
	
	 
	
	// Create 2 active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);	
	CTestDialNewCallAct* testDialNewCallAct2 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
	// Creat hold, resume and swap active objects.
	CTestHoldAct* testHoldAct1 = CTestHoldAct::NewLC(this, iTelephony);
	CTestResumeAct* testResumeAct1 = CTestResumeAct::NewLC(this, iTelephony);
	CTestSwapAct* testSwapAct1 = CTestSwapAct::NewLC(this, iTelephony);
	
	 
	//
	// TEST SUMMARY //
	// 
	// This set of tests covers any hold and resume scenarios involving 
	// 2 calls. These are the tests not covered by TestHold and TestResume 
	// which cover the single call scenarios.
	//  
	//		// Attempt to make a 2nd call
	// 1. 	Call 1 is active. Make a 2nd call. 
	// 		Expected Result: Call 2 will not be allowed.
	// 2. 	Call 1 is on hold. Make a 2nd call. 
	// 		Expected Result: Call 1 remains on hold. Call 2 active.
	//
	//		// Make 2 calls. Attempt to resume the active call.
	// 3. 	Call 1 is on hold. Call 2 is active. Resume call 2.
	//		Expected Result: Call 1 remains on hold. Call 2 remains active.
	// 4.	Call 1 is active. Call 2 is on hold. Resume call 1.
	//		Expected Result: Call 1 remains active. Call 2 remains on hold.
	//
	//		// Make 2 calls. Attempt to hold the held call.
	// 5.	Call 1 is active. Call 2 is on hold. Hold call 2.
	//		Expected Result: Call 1 remains active. Call 2 remains on hold.
	// 6. 	Call 1 is on hold. Call 2 is active. Hold Call 1.
	//		Expected Result: Call 1 remains on hold. Call 2 remains active.
	//
	// 		// Make 2 calls. Attempt to resume the held call
	// 7. 	Call 1 is on hold. Call 2 is active. Resume call 1.
	//		Expected Result: Call 1 remains on hold. Call 2 remains active.
	// 8.	Call 1 is active. Call 2 is on hold. Resume call 2.
	//		Expected Result: Call 1 remains active. Call 2 remains on hold.
	//
	//		// Make 2 calls. Attempt to hold the active call.
	// 9.	Call 1 is active. Call 2 is on hold. Hold call 1.
	//		Expected Result: Call 1 remains active. Call 2 remains on hold.
	// 10. 	Call 1 is on hold. Call 2 is active. Hold Call 2.
	//		Expected Result: Call 1 remains on hold. Call 2 remains active.
	
	
	
	// Test Scenario 1.
	// 1. 	Call 1 is active. Make a 2nd call. 
	// 		Expected Result: Call 2 will not be allowed. 
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 1 "));
	 
	// Create the first call
	testCallParams1.iExpStat = KErrNone;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	// Create a 2nd call
	testCallParams2.iExpStat = KErrGeneral;
	testDialNewCallAct2->TestDialNewCall(testCallParams2);
	CActiveScheduler::Start();	
		 
	// Check that the calls are now in the expected states
	CTelephony::TCallStatusV1 CallStatusV1;
	CTelephony::TCallStatusV1Pckg CallStatusPckg(CallStatusV1);
	
	TInt err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
  
   
	INFO_PRINTF1(_L("END TEST 1"));	 
	
	// Test Scenario 2
	// 2. 	Call 1 is on hold. Make a 2nd call. 
	// 		Expected Result: Call 1 remains on hold. Call 2 active.
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 2 "));
	
	// Put call 1 on hold. 
	testCallParams1.iExpStat=KErrNone; 	
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	// Create a 2nd call
	testCallParams2.iExpStat = KErrNone;
	testDialNewCallAct2->TestDialNewCall(testCallParams2);
	CActiveScheduler::Start();	
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	INFO_PRINTF1(_L("END TEST 2"));	 
	
	//
	//		Test scenario 3
	// 3. 	Call 1 is on hold. Call 2 is active. Resume call 2.
	//		Expected Result: Call 1 remains on hold. Call 2 remains active.
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 3 ")); 
	
	// Resume Call 2
	testCallParams2.iExpStat=KErrAccessDenied;	 
	testResumeAct1->TestResume(testCallParams2);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	INFO_PRINTF1(_L("END TEST 3"));	 
	
	
	//
	//		Test scenario 4
	// 4.	Call 1 is active. Call 2 is on hold. Resume call 1.
	//		Expected Result: Call 1 remains active. Call 2 remains on hold.
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 4 ")); 
	
	// Issue a swap request
	testCallParams1.iExpStat=KErrNone; 	
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Resume Call 1
	testCallParams1.iExpStat=KErrAccessDenied;	 
	testResumeAct1->TestResume(testCallParams1);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	INFO_PRINTF1(_L("END TEST 4"));	 
	
	//
	//
	// 5.	Call 1 is active. Call 2 is on hold. Hold call 2.
	//		Expected Result: Call 1 remains active. Call 2 remains on hold.
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 5 ")); 
	
	// Hold Call 2
	testCallParams2.iExpStat=KErrAccessDenied;	 
	testHoldAct1->TestHold(testCallParams2);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	INFO_PRINTF1(_L("END TEST 5"));	 
	
	
	//
	//
	// 6. 	Call 1 is on hold. Call 2 is active. Hold Call 1.
	//		Expected Result: Call 1 remains on hold. Call 2 remains active.
	//

	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 6 ")); 
	
	// Issue a swap request
	testCallParams1.iExpStat=KErrNone; 	
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	// Hold Call 1
	testCallParams1.iExpStat=KErrAccessDenied;	 
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	INFO_PRINTF1(_L("END TEST 6"));	 
	
	
	//
	//
	// 7. 	Call 1 is on hold. Call 2 is active. Resume call 1.
	//		Expected Result: Call 1 remains on hold. Call 2 remains active.
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 7 "));
	
	// Resume call 1. 
	testCallParams1.iExpStat=KErrGeneral; 	
	testResumeAct1->TestResume(testCallParams1);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);

	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	INFO_PRINTF1(_L("END TEST 7"));	 
	
	//
	//
	// 8.	Call 1 is active. Call 2 is on hold. Resume call 2.
	//		Expected Result: Call 1 remains active. Call 2 remains on hold.
	//
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 8 "));
	
	
	// Issue a swap request
	testCallParams1.iExpStat=KErrNone; 	
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Resume call 2. 
	testCallParams2.iExpStat=KErrGeneral; 	
	testResumeAct1->TestResume(testCallParams2);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	INFO_PRINTF1(_L("END TEST 8"));	 
	
	
	//
	//		 
	// 9.	Call 1 is active. Call 2 is on hold. Hold call 1.
	//		Expected Result: Call 1 remains active. Call 2 remains on hold.
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 9 "));
	
	// Hold Call 1
	testCallParams1.iExpStat=KErrAlreadyExists;	 
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	INFO_PRINTF1(_L("END TEST 9"));	 
	

	//
	//
	// 10. 	Call 1 is on hold. Call 2 is active. Hold Call 2.
	//		Expected Result: Call 1 remains on hold. Call 2 remains active.
	
	INFO_PRINTF1(_L("Hold-Resume-2Calls TEST 10 "));
	
	// Issue a swap request
	testCallParams1.iExpStat=KErrNone; 	
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);
	
	
	// Hold Call 2
	testCallParams2.iExpStat=KErrAlreadyExists;	 
	testHoldAct1->TestHold(testCallParams2);
	CActiveScheduler::Start();
	
	// Check that the calls are now in the expected states 
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusHold,1);
	
	// Check that the calls are now in the expected states
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, CallStatusPckg );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusV1.iStatus == CTelephony::EStatusConnected,1);

	INFO_PRINTF1(_L("END TEST 10"));	 
	
	// Hang up the calls for completion
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(this, iTelephony);
	testCallParams1.iExpStat=KErrNone;	 	
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();	
	
	testCallParams2.iExpStat=KErrNone;	 	 
	testHangupAct1->TestHangup(testCallParams2);
	CActiveScheduler::Start();			 
	 
	  
	CleanupStack::PopAndDestroy(testHangupAct1);
	CleanupStack::PopAndDestroy(testSwapAct1);
	CleanupStack::PopAndDestroy(testResumeAct1);
	CleanupStack::PopAndDestroy(testHoldAct1);
	CleanupStack::PopAndDestroy(testDialNewCallAct2);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);  
	
	return TestStepResult();	
}


CTestGetCallDynamicCaps::CTestGetCallDynamicCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetCallDynamicCaps"));
	}

/**
*
@SYMTestCaseID Etel3rdParty 24.1, TestGetCallDynamicCaps
@SYMTestCaseDesc Test GetCallDynamicCaps 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.19
*/ 

enum TVerdict CTestGetCallDynamicCaps::doTestStepL()
	{
 
	INFO_PRINTF1(_L("Test retrieving of the line's dynamic call capabilities"));
	
	_LIT(KCallNumber1,"1789");
	_LIT(KCallNumber2,"1234");
	
	SetTestNumber(15); 
	
	//Set the call parameters for MO call
	CTelephony::TCallParamsV1 callParams;
	callParams.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg(callParams);	
			
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg;
	testCallParams1.iTelAddress.iTelNumber = KCallNumber1;
	testCallParams1.iExpStat = KErrNone;
	
	TTestCallParams testCallParams2;
	testCallParams2.iLine = CTelephony::EVoiceLine;
	testCallParams2.iCallParamsV1Pckg = &callParamsPckg;
	testCallParams2.iTelAddress.iTelNumber = KCallNumber2;
	testCallParams2.iExpStat = KErrNone;

	// Create an active DialNewCallAct object
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
	// Create an second active DialNewCallAct object
	CTestDialNewCallAct* testDialNewCallAct2 = CTestDialNewCallAct::NewLC(this, iTelephony);
	
	// Create an active TestHoldAct object
	CTestHoldAct* testHoldAct1 = CTestHoldAct::NewLC(this, iTelephony);	
	
	// Create an active TestResumeAct object
	CTestResumeAct* testResumeAct1 = CTestResumeAct::NewLC(this, iTelephony);	
	
	// Create status object for GetCallStatus calls
	CTelephony::TCallStatusV1 CallStatusV1;
	CTelephony::TCallStatusV1Pckg CallStatusPckg(CallStatusV1);
	TPckgBuf<CTelephony::TCallStatusV1> CallStatusBuff(CallStatusV1);
	TDes8* CallStatusData;
	if (iBufferMode)
		{
		CallStatusData = &CallStatusBuff;
		}
	else
		{
		CallStatusData = &CallStatusPckg;
		}
	
	CTelephony::TCallStatusV1& CallStatusResult = 
		reinterpret_cast<CTelephony::TCallStatusV1&> ( const_cast<TUint8&> (*CallStatusData->Ptr() ) );
	
	//
	//
	// TEST 1: Normal case
	// Make a call. Retrieve the call capabilities. 
	//
	//
	
	INFO_PRINTF1(_L(" TEST 1 Dynamic Call Caps. Get caps in connected state "));
	
	// Create a call
	testCallParams1.iExpStat=KErrNone;	
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	// Check the call is now at connected status 

	TInt err = iTelephony->GetCallStatus(testCallParams1.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusConnected,1);	
	
	// Get the call capabilities 
	CTelephony::TCallCapsV1 callCaps;
	CTelephony::TCallCapsV1Pckg callCapsPckg(callCaps);
	TPckgBuf<CTelephony::TCallCapsV1> callCapsBuff(callCaps);
	TDes8* callCapsData;
	if (iBufferMode)
		{
		callCapsData = &callCapsBuff;
		}
	else
		{
		callCapsData = &callCapsPckg;
		}
		
	CTelephony::TCallCapsV1& callCapsResults = 
		reinterpret_cast<CTelephony::TCallCapsV1&> ( const_cast<TUint8&> ( *callCapsData->Ptr() ) );
	
	err = iTelephony->GetCallDynamicCaps(testCallParams1.iCallId, *callCapsData);
	TEST1(err==KErrNone, ETrue);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
		
	// Check that the call capabilties are that the call can be held or swapped
	// but not resumed.
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsHold, 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);
	
	
	INFO_PRINTF1(_L("END TEST 1"));
	
	//
	//
	// TEST 2: Normal case
	// Put the call on hold. Retrieve the call capabilities. 
	//
	//
	
	INFO_PRINTF1(_L(" TEST 2 Dynamic Call Caps. Get caps in held state "));
	
	 
	// Put the call on hold.
	// by calling the ETEL 3rd party API	  
	testCallParams1.iExpStat=KErrNone;
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	// Check that the call is now at held status.
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusHold,1);
	
	// Get the call capabilities 
	err = iTelephony->GetCallDynamicCaps(testCallParams1.iCallId, *callCapsData);
	TEST1(err==KErrNone, ETrue);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
		
	// Check that the call capabilties are that the call can be resumed or swapped
	// but not held.
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsHold), 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsResume, 1);	
	
	INFO_PRINTF1(_L("END TEST 2")); 
	
	//
	//
	// TEST 3: Normal case
	// Resume the call. Retrieve the call capabilities. 
	//
	//
	
	INFO_PRINTF1(_L(" TEST 3 Dynamic Call Caps. Get caps after call resumed "));
	
	// Resume the call
	// by calling the ETEL 3rd party API	  
	testCallParams1.iExpStat=KErrNone;
	testResumeAct1->TestResume(testCallParams1);
	CActiveScheduler::Start();
	
	// Check that the call is now at connected status.
	err = iTelephony->GetCallStatus(testCallParams1.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusConnected,1);
	
	// Get the call capabilities 
	err = iTelephony->GetCallDynamicCaps(testCallParams1.iCallId, *callCapsData);
	TEST1(err==KErrNone, ETrue);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
		
	// Check that the call capabilities are that the call can be held or swapped
	// but not resumed.
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsHold, 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);	
	
	INFO_PRINTF1(_L("END TEST 3"));
	
	
	//
	//
	// TEST 4: Normal case
	// Call 1 on hold. Call 2 active.
	// Retrieve call capabilities. 
	//
	//
	
	INFO_PRINTF1(_L(" TEST 4 Dynamic Call Caps. Check capability in 2 call case "));
	
	
	// Hold Call 1
	testCallParams1.iExpStat=KErrNone;	 
	testHoldAct1->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	// Create a second call.
	testCallParams2.iExpStat=KErrNone;	
	testDialNewCallAct2->TestDialNewCall(testCallParams2);
	CActiveScheduler::Start();
	
	// Check the first call is now at held status
    err = iTelephony->GetCallStatus(testCallParams1.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusHold,1);	
	
	// Check the second call is now at connected status
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusConnected,1);	
	
	// Get the call capabilities of the first call
	err = iTelephony->GetCallDynamicCaps(testCallParams1.iCallId, *callCapsData);
	TEST1(err==KErrNone, 1);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
	
	// Check that the call capabilties are that the call can be swapped 
	// but not held or resumed.
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsHold), 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);
 
	// Get the call capabilities of the second call
	err = iTelephony->GetCallDynamicCaps(testCallParams2.iCallId, *callCapsData);
	TEST1(err==KErrNone, 1);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
		
	// Check that the call capabilties are that the call can be held or swapped
	// but not resumed.
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsHold), 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);
	
	
	INFO_PRINTF1(_L("END TEST 4"));
	
	//
	//
	// TEST 5: Normal case
	// Ensure caps are set correctly after a swap operation.
	// Call 1 active. Call 2 on hold.
	// Retrieve call capabilities. 
	//
	//
	
	INFO_PRINTF1(_L(" TEST 5 Dynamic Call Caps. Check capability in 2 call case "));
	
	// Swap the calls 
	CTestSwapAct* testSwapAct1 = CTestSwapAct::NewLC(this, iTelephony);	
    testCallParams1.iExpStat=KErrNone; 	 
	testSwapAct1->TestSwap(testCallParams1, testCallParams2 );
	CActiveScheduler::Start(); 
	
	// Check the first call is now at connected status
    err = iTelephony->GetCallStatus(testCallParams1.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusConnected,1);	
	
	// Check the second call is now at held status
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusHold,1);	
	
	// Get the call capabilities of the first call
	err = iTelephony->GetCallDynamicCaps(testCallParams1.iCallId, *callCapsData);
	TEST1(err==KErrNone, 1);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
	
	// Check that the call capabilties are that the call can be swapped 
	// but not held or resumed.
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsHold), 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);
 
	// Get the call capabilities of the second call
	err = iTelephony->GetCallDynamicCaps(testCallParams2.iCallId, *callCapsData);
	TEST1(err==KErrNone, 1);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
	
	// Check that the call capabilties are that the call can be held or swapped
	// but not resumed.
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsHold), 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);
	
	
	INFO_PRINTF1(_L("END TEST 5"));
	
	//
	//
	// TEST 6: Normal case
	// Ensure caps are set correctly after a swap back operation.
	// Call 1 on hold. Call 2 active.
	// Retrieve call capabilities. 
	//
	//
	
	INFO_PRINTF1(_L(" TEST 6 Dynamic Call Caps. Check capability in 2 call case "));
	
	// Swap the calls 
    testCallParams1.iExpStat=KErrNone; 	 
	testSwapAct1->TestSwap(testCallParams1, testCallParams2 );
	CActiveScheduler::Start(); 
	
	// Check the first call is now at held status
    err = iTelephony->GetCallStatus(testCallParams1.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusHold,1);	
	
	// Check the second call is now at connected status
	err = iTelephony->GetCallStatus(testCallParams2.iCallId, *CallStatusData );
	TEST1(err == KErrNone,1);
	TEST1(CallStatusResult.iStatus == CTelephony::EStatusConnected,1);	
	
	// Get the call capabilities of the first call
	err = iTelephony->GetCallDynamicCaps(testCallParams1.iCallId, *callCapsData);
	TEST1(err==KErrNone,1);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
	
	// Check that the call capabilties are that the call can be swapped 
	// but not held or resumed.
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsHold), 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);
 
	// Get the call capabilities of the second call
	err = iTelephony->GetCallDynamicCaps(testCallParams2.iCallId, *callCapsData);
	TEST1(err==KErrNone,1);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
	
	// Check that the call capabilties are that the call can be swapped
	// but not held or resumed.
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsHold), 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);
	
	
	INFO_PRINTF1(_L("END TEST 6"));
	
	
	
	//
	//
	// TEST 7: Error case
	// Hang up the call. Retrieve the call capabilities. 
	//
	//
	
	INFO_PRINTF1(_L(" TEST 7 Dynamic Call Caps. Get caps in hungup state "));
	
	
	// Hang up call 1  
	CTestHangupAct* testHangupAct1 = CTestHangupAct::NewLC(this, iTelephony);	
	testHangupAct1->TestHangup(testCallParams1);
	CActiveScheduler::Start();	
	
	// Get the call capabilities of Call 1
	err = iTelephony->GetCallDynamicCaps(testCallParams1.iCallId, *callCapsData);
	TEST1(err==KErrAccessDenied, 1);	 
	
	// Get the call capabilities of Call 2
	err = iTelephony->GetCallDynamicCaps(testCallParams2.iCallId, *callCapsData);
	TEST1(err==KErrNone, 1);
	INFO_PRINTF2(_L("iControlCaps is %x"), callCapsResults.iControlCaps);
	
	// Check that the call capabilties are that the call   
	// can be held or swapped.
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsHold, 1);
	TEST1(callCapsResults.iControlCaps & CTelephony::KCapsSwap, 1);
	TEST1(!(callCapsResults.iControlCaps & CTelephony::KCapsResume), 1);	
	
	INFO_PRINTF1(_L("END TEST 7")); 
	
	// Hang up Call 2 for completion.
	testHangupAct1->TestHangup(testCallParams2);
	CActiveScheduler::Start();	
		 
	CleanupStack::PopAndDestroy(testHangupAct1);
	CleanupStack::PopAndDestroy(testSwapAct1);
	CleanupStack::PopAndDestroy(testResumeAct1);
	CleanupStack::PopAndDestroy(testHoldAct1);
	CleanupStack::PopAndDestroy(testDialNewCallAct2);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	
	
	return TestStepResult();
	}


CTestCallFuncFlightModeOnWithPublishSubscribe::CTestCallFuncFlightModeOnWithPublishSubscribe(TInt& aCallCount)
/** Each test step initialises it's own name
*/
: iCallCount(aCallCount)
{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallFuncFlightModeOnWithPublishSubscribe"));
}

/**
@SYMTestCaseID Etel3rdParty 16.1, TestCallFuncFlightModeOn
@SYMTestCaseDesc Test DialNewCall, HangupCall, AnswerIncomingCall,
*  Hold, Resume, Swap 3rd party APIs
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, 
* REQ 2116.3.1/3/5/7/8/19/20
*/

enum TVerdict CTestCallFuncFlightModeOnWithPublishSubscribe::doTestStepL()
{
	INFO_PRINTF1(_L("START TEST 16.1"));	
	 
	SetTestNumber(15); 
	
	// Set up phonePowerProperty variable so that flight mode can be manipulated.
	RProperty phonePowerProperty;
	User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
	CleanupClosePushL(phonePowerProperty);

	//
	//
	// DialNewCall - Flight Mode on
	//
	//
	
	INFO_PRINTF1(_L(" TEST: DialNewCall disallowed when flight mode on "));
	 
	//Set the call parameters for call 1
	_LIT(KGoodNumber,"1789");
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	
	
	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams1.iExpStat = KErrNone;
	testCallParams1.iLine = CTelephony::EVoiceLine; 
 
	
	// Create active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct1 = CTestDialNewCallAct::NewLC(this, iTelephony);
	 
	// Switch flight mode on (ie radio mode off)
	TRequestStatus requeststatus;
	phonePowerProperty.Subscribe(requeststatus);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(requeststatus);
	TEST(requeststatus.Int() == KErrNone);	
	 
	// Set expected status to KErrAccessDenied as this should be returned in
	// the case where flight mode is on.
	testCallParams1.iExpStat = KErrAccessDenied;	
	
	// Attempt call.	 
	testCallParams1.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	
	//
	//
	// HangUpCall - Flight Mode on
	//
	//
	
	INFO_PRINTF1(_L(" TEST: Hang up disallowed when flight mode on "));
	
	// Switch flight mode off so that a call can be made.
	phonePowerProperty.Subscribe(requeststatus);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
	User::WaitForRequest(requeststatus);
	TEST(requeststatus.Int() == KErrNone);	
	 
	// Make call.	 
	testCallParams1.iExpStat = KErrNone;	
	testCallParams1.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct1->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	
	// Switch flight mode on. Hangup attempt should fail.
	phonePowerProperty.Subscribe(requeststatus);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(requeststatus);
	TEST(requeststatus.Int() == KErrNone);	
	
	INFO_PRINTF1(_L("Attempt to hangup the call"));
	// Create an active HangupAct object
	CTestHangupAct* testHangupAct = CTestHangupAct::NewLC(this, iTelephony);	
	testCallParams1.iExpStat = KErrAccessDenied;
	testHangupAct->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
	// Switch flight mode off and hang up call in preparation
	// for next test.
	phonePowerProperty.Subscribe(requeststatus);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
	User::WaitForRequest(requeststatus);
	TEST(requeststatus.Int() == KErrNone);	

	testCallParams1.iExpStat = KErrNone;
	testHangupAct->TestHangup(testCallParams1);
	CActiveScheduler::Start();
	
	
	//
	//
	// Answer Incoming Call - Flight Mode on
	//
	//
	
	INFO_PRINTF1(_L(" TEST: Answer incoming call disallowed when flight mode on "));
	
	// Create an active AnswerIncomingCallAct object
	CTestAnswerIncomingCallAct* testAnswerIncomingCallAct1 = CTestAnswerIncomingCallAct::NewLC(this, iTelephony);
	
	//Set the call parameters to receive call 1
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = NULL;
	testCallParams1.iExpStat = KErrNone;
	
 	//initiate callback
	testAnswerIncomingCallAct1->CallMe(iCallCount);
	
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);	
	iTelephony->GetLineStatus(testCallParams1.iLine,  statusPckg);
	TEST1(status.iStatus == CTelephony::EStatusRinging, ETrue);
	
	// Switch flight mode on and  attempt to answer the incoming call.
	phonePowerProperty.Subscribe(requeststatus);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(requeststatus);
	TEST(requeststatus.Int() == KErrNone);	
	
	testCallParams1.iExpStat = KErrAccessDenied;
	testAnswerIncomingCallAct1->TestAnswerIncomingCall(testCallParams1);
	CActiveScheduler::Start();
	 
	//
	//
	// Hold Call - Flight Mode on
	//
	//
	
	INFO_PRINTF1(_L(" TEST: Hold call disallowed when flight mode on "));
	
	// Set up call parms
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams1.iExpStat = KErrNone;
	testCallParams1.iLine = CTelephony::EVoiceLine; 
	
	// Create an active TestHoldAct object
	CTestHoldAct* testHoldAct = CTestHoldAct::NewLC(this, iTelephony);	
	
	testCallParams1.iExpStat=KErrAccessDenied;
	testHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();
	
	//
	//
	// Resume Call - Flight Mode on
	//
	//
	
	INFO_PRINTF1(_L(" TEST: Resume call disallowed when flight mode on "));
	
	CTestResumeAct* testResumeAct1 = CTestResumeAct::NewLC(this, iTelephony);	
	
	// Attempt to resume the call using the ETEL 3rdparty API
	testCallParams1.iExpStat = KErrAccessDenied;		
	testResumeAct1->TestResume(testCallParams1);
	CActiveScheduler::Start();
	
	//
	//
	// Swap Call - Flight Mode on
	//
	//
	
	INFO_PRINTF1(_L(" TEST: Swap call disallowed when flight mode on "));
	
	CTestSwapAct* testSwapAct1 = CTestSwapAct::NewLC(this, iTelephony);	
	
	
	// Set up parameters for a second call
	CTelephony::TCallParamsV1 callParams2;
	callParams2.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg2(callParams2);	
	TTestCallParams testCallParams2;
	testCallParams2.iLine = CTelephony::EVoiceLine;
	testCallParams2.iCallParamsV1Pckg = &callParamsPckg2;
	testCallParams2.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams2.iExpStat = KErrNone;
	
	// Attempt a call to the Swap ETEL 3rdparty API
	testCallParams1.iExpStat = KErrAccessDenied;		
	testSwapAct1->TestSwap(testCallParams1, testCallParams2);
	CActiveScheduler::Start();
	
	//
	//
	// Test GetCallDynamicCaps with flight mode on.
	//
	//
	
	INFO_PRINTF1(_L("Test retrieval of dynamic caps - flight mode on"));
	
	// Get the call capabilities 
	CTelephony::TCallCapsV1 callCaps;
	CTelephony::TCallCapsV1Pckg callCapsPckg(callCaps);
	TInt err = iTelephony->GetCallDynamicCaps(testCallParams1.iCallId, callCapsPckg);
	TEST1(err==KErrAccessDenied, ETrue);
		
	INFO_PRINTF1(_L("END TEST 16.1"));
	
	CleanupStack::PopAndDestroy(testSwapAct1);
	CleanupStack::PopAndDestroy(testResumeAct1);
	CleanupStack::PopAndDestroy(testHoldAct);
	CleanupStack::PopAndDestroy(testAnswerIncomingCallAct1);
	CleanupStack::PopAndDestroy(testHangupAct);
	CleanupStack::PopAndDestroy(testDialNewCallAct1);
	CleanupStack::PopAndDestroy(&phonePowerProperty);	 
	 
	return TestStepResult() ;	  
	 
}


CTestDeprecatedDataCallAPIs::CTestDeprecatedDataCallAPIs()
/** Each test step initialises it's own name
*/
{
    SetTestStepName(_L("TestDeprecatedDataCallAPIs"));
}


/**
    @SYMTestCaseID           TEL/EtelIsv/GT0246/1035
    @SYMREQ                  CNON-67WJ2G
    @SYMTestCaseDesc         Test deprecated data call APIs
    @SYMTestPriority         Critical
    @SYMTestActions          Calls each deprecated API and ensures it returns KErrNotSupported
    @SYMTestExpectedResults  Pass
    @SYMTestType             CT
*/
enum TVerdict CTestDeprecatedDataCallAPIs::doTestStepL()
{
    TInt  result;
    TInt  callStatus;
    TRequestStatus  requestStatus;
    TInt  dataChannel;
    TBuf<CTelephony::KMaxTelNumberSize>  dialNumber;

    result = iTelephony->FindDeviceStatus(callStatus);
    TEST1(result == KErrNotSupported, ETrue);

    result = iTelephony->EstablishDataCall(dataChannel, dialNumber);
    TEST1(result == KErrNotSupported, ETrue);

    iTelephony->EstablishDataCall(requestStatus, dataChannel, dialNumber);
    User::WaitForRequest(requestStatus);
    TEST1(requestStatus.Int() == KErrNotSupported, ETrue);

    iTelephony->EstablishDataCall(requestStatus, dataChannel, dialNumber);
    iTelephony->CancelEstablishDataCall();
    User::WaitForRequest(requestStatus);
    TEST1(requestStatus.Int() == KErrNotSupported, ETrue);

    result = iTelephony->TerminateDataCall();
    TEST1(result == KErrNotSupported, ETrue);

    result = iTelephony->ReceiveDataCall(dataChannel);
    TEST1(result == KErrNotSupported, ETrue);

    iTelephony->ReceiveDataCall(requestStatus, dataChannel);
    User::WaitForRequest(requestStatus);
    TEST1(requestStatus.Int() == KErrNotSupported, ETrue);
	 
    iTelephony->ReceiveDataCall(requestStatus, dataChannel);
    iTelephony->CancelReceiveDataCall();
    User::WaitForRequest(requestStatus);
    TEST1(requestStatus.Int() == KErrNotSupported, ETrue);
	 
    return TestStepResult();
} // CTestDeprecatedDataCallAPIs::doTestStepL

