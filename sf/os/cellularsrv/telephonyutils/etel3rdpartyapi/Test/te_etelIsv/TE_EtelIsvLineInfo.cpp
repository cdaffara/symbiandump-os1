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
//

#include "TE_EtelIsvbase.h"
#include "TE_EtelIsvCallFunc.h"
#include "TE_EtelIsvLineInfo.h"


/**
@file

@SYMTestCaseID Etel3rdParty 22.1, TestGetLineStatus
@SYMTestCaseDesc Test GetLineStatus 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.2.2
*/ 
 

CTestGetLineStatus::CTestGetLineStatus(TInt& aCallCount)
/** Each test step initialises it's own name
*/
	: iCallCount(aCallCount)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetLineStatus"));
	}
	
enum TVerdict CTestGetLineStatus::doTestStepL()
	{
/**
This step tests the CTelephony::GetLineStatus() API
*/

	INFO_PRINTF1(_L("START TEST 22.1"));	
 
	INFO_PRINTF1(_L("Test retrieving of the mobile line status"));
	_LIT(KGoodNumber,"1789");
	
	SetTestNumber(15); 

	//Set the call parameters for MO call
	CTelephony::TCallParamsV1 callParams;
	callParams.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg(callParams);	
	
	TTestCallParams testCallParams;
	testCallParams.iLine = CTelephony::EVoiceLine;
	testCallParams.iCallParamsV1Pckg = &callParamsPckg;
	testCallParams.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams.iExpStat = KErrNone;
	testCallParams.iExpAPIErr = KErrNone;
	
	CTelephony::TCallStatusV1 statusV1;
	CTelephony::TCallStatusV1Pckg statusV1Pckg(statusV1);
	
	// set up local data references
	TDes8* statusV1Data;
	if (iBufferMode)
		{
		statusV1Data = &iStatusV1Buff;
		}
	else
		{
		statusV1Data = &statusV1Pckg;
		}

	CTelephony::TCallStatusV1& statusV1Results = reinterpret_cast<CTelephony::TCallStatusV1&> 
    												( const_cast<TUint8&> ( *statusV1Data->Ptr() ));
	
	
	//	
	//TEST 1: GetLineStatus with on Data & Fax line.
	//
	testCallParams.iLine = CTelephony::EDataLine;
	TInt err = iTelephony->GetLineStatus(testCallParams.iLine, *statusV1Data);
	TEST1(err==KErrNone, ETrue);
	TEST1(statusV1Results.iStatus==CTelephony::EStatusIdle, ETrue);
	
	testCallParams.iLine = CTelephony::EFaxLine;
	err = iTelephony->GetLineStatus(testCallParams.iLine, *statusV1Data);
	//Fax line not in SIM TSY
	TEST1(err==KErrNotSupported, ETrue);
	
	//	
	//TEST 2: GetLineStatus with no active call.
	//
	testCallParams.iLine = CTelephony::EVoiceLine;
	//test GetLineStatus with no active call	
	err = iTelephony->GetLineStatus(testCallParams.iLine, *statusV1Data);
	TEST1(err==testCallParams.iExpAPIErr, ETrue);
	TEST1(statusV1Results.iStatus==CTelephony::EStatusIdle, ETrue);
	
	//	
	//TEST 3:Make a call & Hangup a call.  Set a linetstatus notifer going and each time it is invoked call
	//		 GetLineStatus to compare the states which should match.
	//
	
	//test line status for MO calls
	testCallParams.iExpAPIErr = KErrNone;
	CNotifyLineStatusMOAct* testGetLineStatusMOAct = CNotifyLineStatusMOAct::NewLC(this, iTelephony);	
	testGetLineStatusMOAct->StartTestL(testCallParams, CNotifyLineStatusBaseAct::EGetLineStatus);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(); //testGetLineStatusMOAct

	//
	//TEST 4:Answer a call & Hangup a call.  Set a linestatus notifer going and each time it is invoked call
	//		 GetLineStatus to compare the states which should match.
	//

	//test line status for MT calls
	CNotifyLineStatusMTAct* testGetLineStatusMTAct = CNotifyLineStatusMTAct::NewLC(this, iTelephony, iCallCount);	
	testGetLineStatusMTAct->StartTestL(testCallParams, CNotifyLineStatusBaseAct::EGetLineStatus);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(); //testGetLineStatusMTAct	

//
//TEST 5:Make a call - check the line state.  
//	     Put the call on hold - check the line state. 
//		 Resume the call - check the line state.
//       Hangup the call.- check the line state.
//

	//test state change for calls on hold
	
	// Create active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct = CTestDialNewCallAct::NewLC(this, iTelephony);	
	
	// Create a call - should succeed
	testCallParams.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct->TestDialNewCall(testCallParams);
	CActiveScheduler::Start();
		
	//get line status from GetLineStatus API
	err = iTelephony->GetLineStatus(testCallParams.iLine, *statusV1Data);
	TEST1(err == KErrNone, ETrue);
	TEST1(statusV1Results.iStatus==CTelephony::EStatusConnected, ETrue);	
	
	//put call on hold
	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);
	testCallParams.iExpStat=KErrNone;
	TestHoldAct->TestHold(testCallParams);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(TestHoldAct);//TestHoldAct
	
	//get line status from GetLineStatus API
	err = iTelephony->GetLineStatus(testCallParams.iLine, *statusV1Data);
	TEST1(err == KErrNone, ETrue);
	TEST1(statusV1Results.iStatus==CTelephony::EStatusHold, ETrue);	
	
	//resume the call
	CTestResumeAct* testResumeAct = CTestResumeAct::NewLC(this, iTelephony);
	testCallParams.iExpStat=KErrNone;
	testResumeAct->TestResume(testCallParams);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(testResumeAct);//testResumeAct
	
	//get line status from GetLineStatus API
	err = iTelephony->GetLineStatus(testCallParams.iLine, *statusV1Data);
	TEST1(err == KErrNone, ETrue);
	TEST1(statusV1Results.iStatus==CTelephony::EStatusConnected, ETrue);	
	
	// Create an activeHangupAct object
	CTestHangupAct* testHangupAct = CTestHangupAct::NewLC(this, iTelephony);	
	
	//Hangup call1
	testHangupAct->TestHangup(testCallParams);
	CActiveScheduler::Start();
	
	err = iTelephony->GetLineStatus(testCallParams.iLine, *statusV1Data);
	TEST1(err == KErrNone, ETrue);
	TEST1(statusV1Results.iStatus==CTelephony::EStatusIdle, ETrue);	

	CleanupStack::PopAndDestroy(testHangupAct);//testHangupAct
	CleanupStack::PopAndDestroy(testDialNewCallAct);//testDialNewCallAct

	return TestStepResult();
	}
	
CTestGetCallInfo::CTestGetCallInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetCallInfo"));
	}

/**
*
@SYMTestCaseID Etel3rdParty 23.1, TestGetCallInfo
@SYMTestCaseDesc Test GetCallInfo 3rd party API
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.3.4, REQ 2116.3.9
*/ 

enum TVerdict CTestGetCallInfo::doTestStepL()
	{
/**
This step tests the CTelephony::GetCallInfo() API
*/
	INFO_PRINTF1(_L("Test retrieving of the mobile call status"));
	_LIT(KGoodNumber,"1789");
	
	SetTestNumber(15); 
	
	//Set the call parameters for MO call
	CTelephony::TCallParamsV1 callParams;
	callParams.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg(callParams);	
	
	TTestCallParams testCallParams;
	testCallParams.iLine = CTelephony::EVoiceLine;
	testCallParams.iCallParamsV1Pckg = &callParamsPckg;
	testCallParams.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams.iExpStat = KErrNone;
	
	//test GetCallInfo with no active call
	
	CTelephony::TCallInfoV1 callInfoV1;
	CTelephony::TCallInfoV1 expCallInfoV1;
	CTelephony::TCallInfoV1Pckg callInfoV1Pckg(callInfoV1);
	CTelephony::TRemotePartyInfoV1 remoteInfoV1;
	CTelephony::TRemotePartyInfoV1 expRemoteInfoV1;
	CTelephony::TRemotePartyInfoV1Pckg remoteInfoV1Pckg(remoteInfoV1);
	
	CTelephony::TCallSelectionV1 callSelection;
	CTelephony::TCallSelectionV1Pckg callSelectionV1Pckg(callSelection);
	callSelection.iSelect = CTelephony::EActiveCall;
	callSelection.iLine = CTelephony::EVoiceLine;
	
	// set up local data references
	TDes8* callInfoV1Data;
	TDes8* remoteInfoV1Data;
	TDes8* callSelectionV1Data;
	
	if (iBufferMode)
		{
		// initialize required buffers
		iCallSelectionV1Buff = callSelection;
	    
		//initialize data pointers
		callInfoV1Data = &iCallInfoV1Buff;
		remoteInfoV1Data = &iRemoteInfoV1Buff;
		callSelectionV1Data = &iCallSelectionV1Buff;
		}
	else
		{
		callInfoV1Data = &callInfoV1Pckg;
		remoteInfoV1Data = &remoteInfoV1Pckg;
		callSelectionV1Data = &callSelectionV1Pckg;
		}
	
	CTelephony::TCallInfoV1& callInfoV1Results = reinterpret_cast<CTelephony::TCallInfoV1&> 
													( const_cast<TUint8&> ( *callInfoV1Data->Ptr() ) );
	CTelephony::TRemotePartyInfoV1& remoteInfoV1Results = reinterpret_cast<CTelephony::TRemotePartyInfoV1&> 
													( const_cast<TUint8&> ( *remoteInfoV1Data->Ptr() ) );
	
	TInt err;
	#if defined(_DEBUG)
	//
	//TEST 1: OOM
	//

		__UHEAP_FAILNEXT(3);
		err = iTelephony->GetCallInfo(callSelectionV1Pckg, callInfoV1Pckg, remoteInfoV1Pckg);
		TEST1(err==KErrNoMemory, ETrue);
		__UHEAP_RESET;
	#endif

	//
	//TEST 2: GetCallInfo with no active call
	//	
	err = iTelephony->GetCallInfo(*callSelectionV1Data, *callInfoV1Data, *remoteInfoV1Data);
	TEST1(err==KErrNotFound, ETrue);
	
	//
	//TEST 2: GetCallInfo for a single MO call that is put on held and resumed
	//	      This tests EActiveCall and EHeldCall
	//
	
	// Create active DialNewCallAct object and Hangup object
	CTestDialNewCallAct* testDialNewCallAct = CTestDialNewCallAct::NewLC(this, iTelephony);	
	
	// Create a call - should succeed
	testCallParams.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct->TestDialNewCall(testCallParams);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(testDialNewCallAct);
	
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);		
	iTelephony->GetCallStatus(testCallParams.iCallId,  statusPckg);	
	TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
	
	expCallInfoV1.iStatus = CTelephony::EStatusConnected;
	expCallInfoV1.iCallId = CTelephony::EISVCall1;
	expCallInfoV1.iDuration = 0;
	expCallInfoV1.iExitCode = 0;
	expCallInfoV1.iSecurity = CTelephony::ECipheringOff;
	expCallInfoV1.iDialledParty.iTypeOfNumber = CTelephony::EUnknownNumber;
	expCallInfoV1.iDialledParty.iNumberPlan = CTelephony::EUnknownNumberingPlan;
	
	expRemoteInfoV1.iRemoteIdStatus = CTelephony::ERemoteIdentityUnknown;
	expRemoteInfoV1.iDirection = CTelephony::EDirectionUnknown;
	expRemoteInfoV1.iRemoteNumber.iTypeOfNumber = CTelephony::EUnknownNumber;
	expRemoteInfoV1.iRemoteNumber.iNumberPlan = CTelephony::EUnknownNumberingPlan;
	
	//check status of active call
	callSelection.iSelect = CTelephony::EActiveCall;
	if (iBufferMode)
		{
		iCallSelectionV1Buff = callSelection;
		}
	    
	err = iTelephony->GetCallInfo(*callSelectionV1Data, *callInfoV1Data, *remoteInfoV1Data);
	TEST1(err==KErrNone, ETrue);
	if(err == KErrNone)
		{
		CNotifyLineStatusBaseAct::PrintGetCallInfo(this, CTelephony::EActiveCall, callInfoV1Results, remoteInfoV1Results);
		TestResults(callInfoV1Results, expCallInfoV1, remoteInfoV1Results, expRemoteInfoV1);
		}
		
	//check status of held call - should fail
	callSelection.iSelect = CTelephony::EHeldCall;
	if (iBufferMode)
		{
		iCallSelectionV1Buff = callSelection;
		}
	err = iTelephony->GetCallInfo(*callSelectionV1Data, *callInfoV1Data, *remoteInfoV1Data);
	TEST1(err==KErrNotFound, ETrue);
	
	//check status of in-progress call - should fail
	callSelection.iSelect = CTelephony::EInProgressCall;
	if (iBufferMode)
		{
		iCallSelectionV1Buff = callSelection;
		}	
	err = iTelephony->GetCallInfo(*callSelectionV1Data, *callInfoV1Data, *remoteInfoV1Data);
	TEST1(err==KErrNotFound, ETrue);
	
	//put call on hold
	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this, iTelephony);
	testCallParams.iExpStat=KErrNone;
	TestHoldAct->TestHold(testCallParams);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(TestHoldAct);//TestHoldAct
	
	//check status of held call 
	callSelection.iSelect = CTelephony::EHeldCall;
	if (iBufferMode)
		{
		iCallSelectionV1Buff = callSelection;
		}
	expCallInfoV1.iStatus = CTelephony::EStatusHold;
	err = iTelephony->GetCallInfo(*callSelectionV1Data, *callInfoV1Data, *remoteInfoV1Data);
	TEST1(err==KErrNone, ETrue);
	if(err == KErrNone)
		{
		CNotifyLineStatusBaseAct::PrintGetCallInfo(this, CTelephony::EHeldCall, callInfoV1Results, remoteInfoV1Results);
		TestResults(callInfoV1Results, expCallInfoV1, remoteInfoV1Results, expRemoteInfoV1);
		}

	//Hangup call1
	CTestHangupAct* testHangupAct = CTestHangupAct::NewLC(this, iTelephony);	
	testCallParams.iExpStat = KErrNone;
	testHangupAct->TestHangup(testCallParams);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(testHangupAct);
	
	//
	//TEST 3: Create a single MO call and each time a line state change notification is received call
	//	      GetCallInfo. This tests EInProgressCall
	//
	//test call info for MO calls
	CNotifyLineStatusMOAct* testGetCallInfoMOAct = CNotifyLineStatusMOAct::NewLC(this, iTelephony);	
	testGetCallInfoMOAct->StartTestL(testCallParams, CNotifyLineStatusBaseAct::EGetCallInfo);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(); //testGetCallInfoMOAct

	return TestStepResult();
	}
	
void CTestGetCallInfo::TestResults(CTelephony::TCallInfoV1& aActualCallInfo, CTelephony::TCallInfoV1& aExpCallInfo, CTelephony::TRemotePartyInfoV1& aActualRemotePartyInfo,  CTelephony::TRemotePartyInfoV1& aExpRemotePartyInfo)
	{
	TEST1(aActualCallInfo.iStatus == aExpCallInfo.iStatus, ETrue);
	TEST1(aActualCallInfo.iDialledParty.iTypeOfNumber == aExpCallInfo.iDialledParty.iTypeOfNumber, ETrue);
	TEST1(aActualCallInfo.iDialledParty.iNumberPlan == aExpCallInfo.iDialledParty.iNumberPlan, ETrue);
	TEST1(aActualCallInfo.iDialledParty.iTelNumber == aExpCallInfo.iDialledParty.iTelNumber, ETrue);
	TEST1(aActualCallInfo.iDuration == aExpCallInfo.iDuration, ETrue);
	TEST1(aActualCallInfo.iExitCode == aExpCallInfo.iExitCode, ETrue);
	TEST1(aActualCallInfo.iSecurity == aExpCallInfo.iSecurity, ETrue);
	TEST1(aActualCallInfo.iCallId == aExpCallInfo.iCallId, ETrue);
	
	TEST1(aActualRemotePartyInfo.iRemoteIdStatus == aExpRemotePartyInfo.iRemoteIdStatus, ETrue);
	TEST1(aActualRemotePartyInfo.iCallingName == aExpRemotePartyInfo.iCallingName, ETrue);
	TEST1(aActualRemotePartyInfo.iDirection == aExpRemotePartyInfo.iDirection, ETrue);
	TEST1(aActualRemotePartyInfo.iRemoteNumber.iTypeOfNumber == aExpRemotePartyInfo.iRemoteNumber.iTypeOfNumber, ETrue);
	TEST1(aActualRemotePartyInfo.iRemoteNumber.iNumberPlan == aExpRemotePartyInfo.iRemoteNumber.iNumberPlan, ETrue);
	TEST1(aActualRemotePartyInfo.iRemoteNumber.iTelNumber == aExpRemotePartyInfo.iRemoteNumber.iTelNumber, ETrue);	
	}
	
 

//
//AO classes used by test steps above
//


CNotifyLineStatusBaseAct::CNotifyLineStatusBaseAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
 	: CEtelIsvActBase(aTestStep, aTelephony)
   	, iCallStatusV1Pckg(iCallStatusV1)
   	, iFinish(EFalse)
    {
    }
    
 
    
void CNotifyLineStatusBaseAct::PrintGetCallInfo(CEtelIsvTestStep* aTestStep, CTelephony::TCallSelect aCallSelect, CTelephony::TCallInfoV1& aCallInfoV1, CTelephony::TRemotePartyInfoV1& aRemoteInfoV1)
/**
Print out results for GetCallInfo
*/
	{
	TBuf<150> strBuf;
	
	if(aCallSelect == CTelephony::EActiveCall)
		strBuf = _L("EActiveCall");
	else if(aCallSelect == CTelephony::EHeldCall)
		strBuf = _L("EHeldCall");
	else // CTelephony::EInProgressCall
		strBuf = _L("EInProgressCall");
	aTestStep->INFO_PRINTF2(_L("For %S GetCallInfo paramters are..."), &strBuf );
	
	//TCallInfoV1
	strBuf=_L("GetCallInfo TCallInfoV1::iStatus=");
	PrintLineStatus(aTestStep, strBuf, aCallInfoV1.iStatus); 
	aTestStep->INFO_PRINTF2(_L("GetCallInfo TCallInfoV1::iDuration=%d "), aCallInfoV1.iDuration.Int() );
	aTestStep->INFO_PRINTF2(_L("GetCallInfo TCallInfoV1::iExitCode=%d "), aCallInfoV1.iExitCode );
	
	switch(aCallInfoV1.iSecurity)
		{
	case CTelephony::ECipheringOff:
		strBuf = _L("ECipheringOff");
		break;
	case CTelephony::ECipheringGSM:
		strBuf = _L("ECipheringGSM");
		break;
	case CTelephony::ECipheringWCDMA:
		strBuf = _L("ECipheringWCDMA");
		break;
	default:
		strBuf = _L("UNKNOWN");
		break;
	
		};
	aTestStep->INFO_PRINTF2(_L("GetCallInfo TCallInfoV1::iSecurity=%S "), &strBuf );
	aTestStep->INFO_PRINTF2(_L("GetCallInfo TCallInfoV1::iCallId=%d "), aCallInfoV1.iCallId );
	
	switch(aRemoteInfoV1.iRemoteIdStatus)
		{
	case CTelephony::ERemoteIdentityUnknown:
		strBuf = _L("ERemoteIdentityUnknown");
		break;
	case CTelephony::ERemoteIdentityAvailable:
		strBuf = _L("ERemoteIdentityAvailable");
		break;
	case CTelephony::ERemoteIdentitySuppressed:
		strBuf = _L("ERemoteIdentitySuppressed");
		break;
	default:
		strBuf = _L("UNKNOWN");
		break;
		};
	aTestStep->INFO_PRINTF2(_L("GetCallInfo TRemotePartyInfoV1::iRemoteIdStatus=%S "), &strBuf );
	aTestStep->INFO_PRINTF2(_L("GetCallInfo TRemotePartyInfoV1::iCallingName=%S "), &aRemoteInfoV1.iCallingName );
	
	switch( aRemoteInfoV1.iDirection)
		{
	case CTelephony::EDirectionUnknown:
		strBuf = _L("EDirectionUnknown");
		break;
	case CTelephony::EMobileOriginated:
		strBuf = _L("EMobileOriginated");
		break;
	case CTelephony::EMobileTerminated:
		strBuf = _L("EMobileTerminated");
		break;
	default:
		strBuf = _L("UNKNOWN");
		break;
		};
	aTestStep->INFO_PRINTF2(_L("GetCallInfo TRemotePartyInfoV1::iDirection=%S "), &strBuf );

	}
	
void CNotifyLineStatusBaseAct::PrintLineStatus(CEtelIsvTestStep* aTestStep, TDes& aLine, const CTelephony::TCallStatus aStatus) 
/**
Print out line status
*/
	{	
	switch(aStatus)
		{
	case CTelephony::EStatusUnknown:
		aLine.Append(_L("EStatusUnknown"));
		break;
	case CTelephony::EStatusIdle:
		aLine.Append(_L("EStatusIdle"));
		break;
	case CTelephony::EStatusDialling:
		aLine.Append(_L("EStatusDialling"));
		break;
	case CTelephony::EStatusRinging:
		aLine.Append(_L("EStaEStatusRinging"));
		break;
	case CTelephony::EStatusAnswering:
		aLine.Append(_L("EStatusAnswering"));
		break;
	case CTelephony::EStatusConnecting:
		aLine.Append(_L("EStatusConnecting"));
		break;
	case CTelephony::EStatusConnected:
		aLine.Append(_L("EStatusConnected"));
		break;
	case CTelephony::EStatusReconnectPending:
		aLine.Append(_L("EStatusReconnectPending"));
		break;
	case CTelephony::EStatusDisconnecting:
		aLine.Append(_L("EStatusDisconnecting"));
		break;
	case CTelephony::EStatusHold:
		aLine.Append(_L("EStatusHold"));
		break;
	case CTelephony::EStatusTransferring:
		aLine.Append(_L("EStatusTransfering"));
		break;
	case CTelephony::EStatusTransferAlerting:
		aLine.Append(_L("EStatusTransferAlerting"));
		break;
	default:
		aLine.Append(_L("Unknown State"));
		}
	aTestStep->INFO_PRINTF2(_L("%S"), &aLine);
	}
	
void CNotifyLineStatusBaseAct::CallGetCallInfo(const CTelephony::TCallSelect aSelect, CTelephony::TCallStatus aExpStatus)
/**
Call the GetCallInfo function and print out results
*/
	{
	CTelephony::TCallInfoV1 callInfoV1;
	CTelephony::TCallInfoV1Pckg callInfoV1Pckg(callInfoV1);
	CTelephony::TRemotePartyInfoV1 remoteInfoV1;
	CTelephony::TRemotePartyInfoV1Pckg remoteInfoV1Pckg(remoteInfoV1);
	
	CTelephony::TCallSelectionV1 callSelection;
	CTelephony::TCallSelectionV1Pckg callSelectionV1Pckg(callSelection);
	callSelection.iSelect = aSelect;
	callSelection.iLine = CTelephony::EVoiceLine;
	
	TInt err = iTelephony->GetCallInfo(callSelectionV1Pckg, callInfoV1Pckg, remoteInfoV1Pckg);
	iTestStep->TEST1(err == KErrNone, ETrue);
	iTestStep->TEST1(callInfoV1.iStatus == aExpStatus,ETrue);
	PrintGetCallInfo(iTestStep, aSelect, callInfoV1, remoteInfoV1);
	}

/**
Active Object to request a notifications of line status change and create an MO call.
This AO is used to test two APIs GetLineStatus() and GetCallInfo().
**/
CNotifyLineStatusMOAct* CNotifyLineStatusMOAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CNotifyLineStatusMOAct* obj = new(ELeave) CNotifyLineStatusMOAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CNotifyLineStatusMOAct::CNotifyLineStatusMOAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony)
	: CNotifyLineStatusBaseAct(aTestStep, aTelephony)
   	{  
	}

void CNotifyLineStatusMOAct::ConstructL()
	{    
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CNotifyLineStatusMOAct::~CNotifyLineStatusMOAct()
	{  
	delete iDialNewCallAct;
	delete iHangupAct;
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CNotifyLineStatusMOAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
	}

void CNotifyLineStatusMOAct::StartTestL(TTestCallParams& aTestCallParams, TTestToRun aTestToRun)
/**
Start the test
*/ 
	{
	iTestStep-> INFO_PRINTF1(_L("CNotifyLineStatusMOAct::StartTest() Start"));
 	iTestCallParams = &aTestCallParams;
 	// Update the expected return value.
	iExpStat=aTestCallParams.iExpStat;
	iTestToRun = aTestToRun;
 	
 	//start the notify request for Line Status Change
	iEvent = CTelephony::EVoiceLineStatusChange;
 		
	//set notification request going
 	iTelephony->NotifyChange(iStatus, iEvent, iCallStatusV1Pckg);	
	SetActive(); 
	
	//now initiate a call
	if(iDialNewCallAct)
		{
		delete iDialNewCallAct;
		iDialNewCallAct = NULL;
		}
	//create dial AO
	iDialNewCallAct = CTestDialNewCallAct::NewL(iTestStep, iTelephony, this);	
	//create hangup AO
	if(iHangupAct)
		{
		delete iHangupAct;
		iHangupAct = NULL;
		}
	iHangupAct = CTestHangupAct::NewL(iTestStep, iTelephony, this);	
	
	//dial
	iDialNewCallAct->TestDialNewCall(*iTestCallParams);	
	}
	
void  CNotifyLineStatusMOAct::ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent)
	{
/**
Gets called from a child AO when its RunL has been called.  
*/
	//got callback from child AO. Proceed the test.
	if(aCallStatus.Int() == KErrNone)
		{
		if(aEvent == EDialNewCall)	//new call has been successfully made.  So hangup
			{
			iHangupAct->TestHangup(*iTestCallParams);
			return;
			}
		}		
	//finish the test by cancelling the registered notification
	DoCancel();	
	iFinish = ETrue;
	CActiveScheduler::Stop();
	}
	
void CNotifyLineStatusMOAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	iTestStep-> INFO_PRINTF1(_L(" CNotifyLineStatusMOAct::RunL "));
	iTestStep->INFO_PRINTF2(_L("CNotifyLineStatusMOAct: Request Status returned is: %d"), iStatus.Int()   );
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
	
	if(iStatus.Int() == KErrNone)
		{
		TBuf<100> line=_L("CNotifyLineStatusMOAct: Line status from notification = ");
		//line status from notification
		PrintLineStatus(iTestStep, line, iCallStatusV1.iStatus);
		
		if(iTestToRun == EGetLineStatus)	//test GetLinStatus
			{			
			//line status from GetLineStatus API
			CTelephony::TCallStatusV1 statusV1;
			CTelephony::TCallStatusV1Pckg statusV1Pckg(statusV1);
			TInt err = iTelephony->GetLineStatus(iTestCallParams->iLine, statusV1Pckg);
			line=_L("CNotifyLineStatusMOAct: Line status from GetLineStatus = ");
			iTestStep->TEST1(err == iTestCallParams->iExpAPIErr, ETrue);
			PrintLineStatus(iTestStep, line, statusV1.iStatus);			
			//These should match
			iTestStep->TEST1(statusV1.iStatus == iCallStatusV1.iStatus, ETrue);							
			}
		else	//test GetCallInfo
			{
			//call GetCallInfo API for each call select notification that is call in progress.
			if(iCallStatusV1.iStatus != CTelephony::EStatusConnected && iCallStatusV1.iStatus != CTelephony::EStatusIdle)
				CallGetCallInfo(CTelephony::EInProgressCall, iCallStatusV1.iStatus);
			}		
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("CNotifyLineStatusMOAct: Error for notify ELineStatusChange: %d"), iStatus.Int() );
		}
	//if true finish the test.
	if(iFinish)
		CActiveScheduler::Stop();
	else
		{
		iTelephony->NotifyChange(iStatus, iEvent, iCallStatusV1Pckg);	
		SetActive();
		}
	}	
	
/**
Active Object to request a notifications of line status change and answer a MT call.
This AO is used to test two APIs GetLineStatus() and GetCallInfo().
**/
CNotifyLineStatusMTAct* CNotifyLineStatusMTAct::NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& aCallCount)
//Factory constructor
	{
	CNotifyLineStatusMTAct* obj = new(ELeave) CNotifyLineStatusMTAct(aTestStep, aTelephony, aCallCount);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CNotifyLineStatusMTAct::CNotifyLineStatusMTAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& aCallCount)
	: CNotifyLineStatusBaseAct(aTestStep, aTelephony)
	, iCallCount(aCallCount)
   	{  
	}

void CNotifyLineStatusMTAct::ConstructL()
	{    
	iCanceller = new(ELeave) CTestCanceller(this);
	CActiveScheduler::Add(this);
	}

CNotifyLineStatusMTAct::~CNotifyLineStatusMTAct()
	{  
	delete iAnswerIncomingCallAct;
	delete iHangupAct;
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	delete iCanceller;
	}

void CNotifyLineStatusMTAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
	}
	
void CNotifyLineStatusMTAct::StartTestL(TTestCallParams& aTestCallParams, TTestToRun aTestToRun) 
/**
Start the test
*/
	{
	iTestStep->INFO_PRINTF1(_L("CNotifyLineStatusMTAct::StartTest() Start"));
 	iTestCallParams = &aTestCallParams;
 	// Update the expected return value.
	iExpStat=aTestCallParams.iExpStat;
	iTestToRun = aTestToRun;
 	
 	//start the notify request for Line Status Change
	iEvent = CTelephony::EVoiceLineStatusChange;
 		
	//set notification request going
	iTelephony->NotifyChange(iStatus, iEvent, iCallStatusV1Pckg);	
	SetActive(); 
	
	//get SIMTSY to call us
	CallMe(iCallCount);
	
	//now set up AO to wait for a call
	if(iAnswerIncomingCallAct)
		{
		delete iAnswerIncomingCallAct;
		iAnswerIncomingCallAct = NULL;
		}
	//create dial AO
	iAnswerIncomingCallAct = CTestAnswerIncomingCallAct::NewL(iTestStep, iTelephony, this);	
	//create hangup AO
	if(iHangupAct)
		{
		delete iHangupAct;
		iHangupAct = NULL;
		}
	iHangupAct = CTestHangupAct::NewL(iTestStep, iTelephony, this);		
	}
	
void  CNotifyLineStatusMTAct::ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent)
	{
/**
Gets called from a child AO when its RunL has been called. 
*/
	//got callback from child AO. Proceed the test.
	if(aCallStatus.Int() == KErrNone)
		{
		if(aEvent == EAnswerIncomingCall)	//new call has been successfully made.  So hangup
			{
			iHangupAct->TestHangup(*iTestCallParams);
			return;
			}
		}		
	//finish the test by cancelling the registered notification
	DoCancel();	
	iFinish = ETrue;
	CActiveScheduler::Stop();
	}
	
void CNotifyLineStatusMTAct::RunL()
	{ 
	// ActiveScheduler will have set status to KRequestComplete, just before
	// calling this RunL().
	iTestStep-> INFO_PRINTF1(_L("CNotifyLineStatusMTAct::RunL "));
	iTestStep->INFO_PRINTF2(_L("CNotifyLineStatusMTAct: Request Status returned is: %d"), iStatus.Int()   );
	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
	
	if(iStatus.Int() == KErrNone)
		{
		TBuf<100> line=_L("CNotifyLineStatusMTAct: Line status from notification = ");
		//line status from notification
		PrintLineStatus(iTestStep, line, iCallStatusV1.iStatus);
		
		if(iTestToRun == EGetLineStatus)	//test GetLinStatus
			{			
			//line status from GetLineStatus API
			CTelephony::TCallStatusV1 statusV1;
			CTelephony::TCallStatusV1Pckg statusV1Pckg(statusV1);
			TInt err = iTelephony->GetLineStatus(iTestCallParams->iLine, statusV1Pckg);
			line=_L("CNotifyLineStatusMTAct: Line status from GetLineStatus = ");
			iTestStep->TEST1(err == iTestCallParams->iExpAPIErr, ETrue);
			PrintLineStatus(iTestStep, line, statusV1.iStatus);			
			//These should match
			iTestStep->TEST1(statusV1.iStatus == iCallStatusV1.iStatus, ETrue);							
			}	
		}
	else
		{
		iTestStep->INFO_PRINTF2(_L("CNotifyLineStatusMTAct: Error for notify ELineStatusChange: %d"), iStatus.Int() );
		}
	
	if(iTestToRun == EGetLineStatus && iCallStatusV1.iStatus == CTelephony::EStatusRinging)
		{
		//answer the call call
		iAnswerIncomingCallAct->TestAnswerIncomingCall(*iTestCallParams);	
		}
		
	//if true finish the test.
	if(iFinish)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iTelephony->NotifyChange(iStatus, iEvent, iCallStatusV1Pckg);
		SetActive();
		}
	}

/**
Each test step initialises it's own name
*/
CTestLineFuncFlightModeOnWithPublishSubscribe::CTestLineFuncFlightModeOnWithPublishSubscribe()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineFuncFlightModeOnWithPublishSubscribe"));
	}

/**
@SYMTestCaseID Etel3rdParty 25,1, TestLineFuncFlightModeOn
@SYMTestCaseDesc Test GetLineStatus, GetCallInfo 3rd party APIs
@SYMFssID SGL.GT0166.054 v1.3 A2_Telephony_Functional_Specification.doc, REQ 2116.2.2, 2116.3.4, 2116.3.9
*/
enum TVerdict CTestLineFuncFlightModeOnWithPublishSubscribe::doTestStepL()
	{
	INFO_PRINTF1(_L("START TEST 25.1"));	
	 
	INFO_PRINTF1(_L(" TEST: Signal Strength information not retrievable when flight mode on with publish and subscribe "));

	//Set test number
	SetTestNumber(15);
	
	
	//Set the call parameters for MO call
	_LIT(KGoodNumber,"1789");
	
	CTelephony::TCallParamsV1 callParams;
	callParams.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg(callParams);	
	
	TTestCallParams testCallParams;
	testCallParams.iLine = CTelephony::EVoiceLine;
	testCallParams.iCallParamsV1Pckg = &callParamsPckg;
	testCallParams.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams.iExpStat = KErrNone;
	testCallParams.iExpAPIErr = KErrNone;
	
	CTelephony::TCallStatusV1 statusV1;
	CTelephony::TCallStatusV1Pckg statusV1Pckg(statusV1);
	 
	// Set up phonePowerProperty variable so that flight mode can be manipulated.
	RProperty phonePowerProperty;
	User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
	CleanupClosePushL(phonePowerProperty);
 	
	
	//
	//
	// Test GetLineStatus with flight mode on.
	//
	//
	
	INFO_PRINTF1(_L("Test retrieval of the mobile line status - flight mode on"));
	
	// Create active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct = CTestDialNewCallAct::NewLC(this, iTelephony);	
	
	// Create a call - should succeed
	testCallParams.iCallId = CTelephony::EISVMaxNumOfCalls;
	testDialNewCallAct->TestDialNewCall(testCallParams);
	CActiveScheduler::Start();


	//Switch flight mode on (ie radio mode off)
	TRequestStatus status;
	phonePowerProperty.Subscribe(status);
	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	// Attempt to get line status from GetLineStatus API
	// This should fail as flight mode is on.
	//testCallParams.iExpStat = KErrAccessDenied;
	TInt err = iTelephony->GetLineStatus(testCallParams.iLine, statusV1Pckg);
	TEST1(err == KErrAccessDenied, ETrue);


	//
	//
	// Test GetCallInfo with flight mode on.
	//
	//

	INFO_PRINTF1(_L("Test retrieval of call info - flight mode on"));

	CTelephony::TCallInfoV1 callInfoV1;
	CTelephony::TCallInfoV1 expCallInfoV1;
	CTelephony::TCallInfoV1Pckg callInfoV1Pckg(callInfoV1);
	CTelephony::TRemotePartyInfoV1 remoteInfoV1;
	CTelephony::TRemotePartyInfoV1 expRemoteInfoV1;
	CTelephony::TRemotePartyInfoV1Pckg remoteInfoV1Pckg(remoteInfoV1);
	
	CTelephony::TCallSelectionV1 callSelection;
	CTelephony::TCallSelectionV1Pckg callSelectionV1Pckg(callSelection);
	callSelection.iSelect = CTelephony::EActiveCall;
	callSelection.iLine = CTelephony::EVoiceLine;
 	expCallInfoV1.iStatus = CTelephony::EStatusConnected;
	expCallInfoV1.iCallId = CTelephony::EISVCall1;
	expCallInfoV1.iDuration = 0;
	expCallInfoV1.iExitCode = 0;
	expCallInfoV1.iSecurity = CTelephony::ECipheringOff;
	expCallInfoV1.iDialledParty.iTypeOfNumber = CTelephony::EUnknownNumber;
	expCallInfoV1.iDialledParty.iNumberPlan = CTelephony::EUnknownNumberingPlan;
	
	expRemoteInfoV1.iRemoteIdStatus = CTelephony::ERemoteIdentityUnknown;
	expRemoteInfoV1.iDirection = CTelephony::EDirectionUnknown;
	expRemoteInfoV1.iRemoteNumber.iTypeOfNumber = CTelephony::EUnknownNumber;
	expRemoteInfoV1.iRemoteNumber.iNumberPlan = CTelephony::EUnknownNumberingPlan;
	
	// attempt to get call info of active call
	callSelection.iSelect = CTelephony::EActiveCall;
	err = iTelephony->GetCallInfo(callSelectionV1Pckg, callInfoV1Pckg, remoteInfoV1Pckg);
	TEST1(err==KErrAccessDenied, ETrue);


	//
	//
	// Test GetCallStatus with flight mode on.
	//
	//

	INFO_PRINTF1(_L("Test retrieval of call status - flight mode on"));

	// Attempt to retrive the call status while flight mode is on.
	err = iTelephony->GetCallStatus(testCallParams.iCallId,  statusV1Pckg);
	TEST1(err==KErrAccessDenied, ETrue);

	INFO_PRINTF1(_L("END TEST 25.1"));


	CleanupStack::PopAndDestroy(testDialNewCallAct);
	CleanupStack::PopAndDestroy(&phonePowerProperty);

	return TestStepResult(); 
	}

CTestGetLineStatusNoCaps::CTestGetLineStatusNoCaps(TInt& aCallCount)
/** Each test step initialises it's own name
*/
 	: iCallCount(aCallCount)
 	{
 	// store the name of this test case
 	// this is the name that is used by the script file
 	SetTestStepName(_L("TestGetLineStatusNoCaps"));
 	}
 	
enum TVerdict CTestGetLineStatusNoCaps::doTestStepL()
/**
Tests an incoming call can be detected when the client has no capabilities.
 The test preamble constructs CTelephony::NewL() which makes a request
 to RLine::NotifyIncomingCall.
 The test step does the following:
 1) Makes a request to be notified of a change to the line status
 by calling RMobileLine::NotifyMobileLineStatusChange.
 2) Triggers an incoming call
 3) Validates that the client receives the line status change.
 
 The test does not directly check that RLine::NotifyIncomingCall completes
 successfully. This is because CNotifyIncomingCallAc only notifies
 CAnswerIncomingCallAct when an incoming call is received. However
 CAnswerIncomingCallAct calls RMobileCall::AnswerIncomingCallISV() 
 which needs Network Service capabilities. The fact that the test
 completes is sufficient to prove that the RLine::NotifyIncomingCall does not
 require NetworkServices - if it did, the CNotifyIncomingCallAc gets
 locked in an infinite loop, causing the test to hang.
 */
 	{
 	INFO_PRINTF1(_L("START TEST 1"));	
  
   	INFO_PRINTF1(_L("Test retrieving of the mobile line status"));
 	_LIT(KGoodNumber,"1789");
 	
 	SetTestNumber(15); 
 
 	//Set the call parameters for MO call
 	CTelephony::TCallParamsV1 callParams;
 	callParams.iIdRestrict = CTelephony::ESendMyId;
 	CTelephony::TCallParamsV1Pckg callParamsPckg(callParams);	
 	
 	TTestCallParams testCallParams;
 	testCallParams.iLine = CTelephony::EVoiceLine;
 	testCallParams.iCallParamsV1Pckg = &callParamsPckg;
 	testCallParams.iTelAddress.iTelNumber = KGoodNumber;
 	testCallParams.iExpStat = KErrNone;
 	testCallParams.iExpAPIErr = KErrNone;
 
 	//test line status for MT calls
 	CNotifyLineStatusNoCapsMTAct* testGetLineStatusMTAct = CNotifyLineStatusNoCapsMTAct::NewLC(this, iTelephony, iCallCount);	
 	testGetLineStatusMTAct->StartTestL(testCallParams, CNotifyLineStatusBaseAct::EGetLineStatus);
 	 	CActiveScheduler::Start();
 	CleanupStack::PopAndDestroy(); //testGetLineStatusMTAct	
 
 	return TestStepResult();
 	}
 
 
 void CNotifyLineStatusNoCapsMTAct::RunL()
 	{ 
 	// ActiveScheduler will have set status to KRequestComplete, just before
 	// calling this RunL().
 	iTestStep-> INFO_PRINTF1(_L("CNotifyLineStatusMTAct::RunL "));
 	iTestStep->INFO_PRINTF2(_L("CNotifyLineStatusMTAct: Request Status returned is: %d"), iStatus.Int()   );
 	iTestStep->TEST1(iStatus.Int() == iExpStat.Int(), ETrue);
 	
 	if(iStatus.Int() == KErrNone)
 		{
 		TBuf<100> line=_L("CNotifyLineStatusMTAct: Line status from notification = ");
 		//line status from notification
 		PrintLineStatus(iTestStep, line, iCallStatusV1.iStatus);
 		
 		if(iTestToRun == EGetLineStatus)	//test GetLinStatus
 			{			
 			//line status from GetLineStatus API
 			CTelephony::TCallStatusV1 statusV1;
 			CTelephony::TCallStatusV1Pckg statusV1Pckg(statusV1);
 			TInt err = iTelephony->GetLineStatus(iTestCallParams->iLine, statusV1Pckg);
 			line=_L("CNotifyLineStatusMTAct: Line status from GetLineStatus = ");
 			iTestStep->TEST1(err == iTestCallParams->iExpAPIErr, ETrue);
 			PrintLineStatus(iTestStep, line, statusV1.iStatus);			
 			//These should match
 			iTestStep->TEST1(statusV1.iStatus == iCallStatusV1.iStatus, ETrue);							
 			}	
 		}
 	else
 		{
 		iTestStep->INFO_PRINTF2(_L("CNotifyLineStatusMTAct: Error for notify ELineStatusChange: %d"), iStatus.Int() );
 		}
 	
 	CActiveScheduler::Stop();
 	}
 

 CNotifyLineStatusNoCapsMTAct* CNotifyLineStatusNoCapsMTAct::NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony, TInt& aCallCount)
 	{
 	CNotifyLineStatusNoCapsMTAct* obj = new(ELeave) CNotifyLineStatusNoCapsMTAct(aTestStep, aTelephony, aCallCount);
 	CleanupStack::PushL(obj);
 	obj->ConstructL();
 	return obj;    
 	}
 
 CNotifyLineStatusNoCapsMTAct::CNotifyLineStatusNoCapsMTAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& aCallCount):
     CNotifyLineStatusMTAct(aTestStep, aTelephony, aCallCount)
     {
     }

