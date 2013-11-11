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
// Implmentation of all test objects required to verify capability check on 
// ETel3rdParty call functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/
#include "callfunctsteps.h"

_LIT(KGoodNumber,"1789");

//
CHoldTestStep::CHoldTestStep()
	:iCallParamsPckg(iCallParams)
	{
	//Set the call parameters for call 2
	iCallParams.iIdRestrict = CTelephony::ESendMyId;
	}
	
TInt CHoldTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	// Put on hold
	iTelephony->Hold(aStatus, CTelephony::EISVCall1);
	return KErrNone;
	}
	
TInt CHoldTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EHoldCancel);
	return KErrNone;
	}	
	
//
CDialNewCallTestStep::CDialNewCallTestStep()
	:iCallParamsPckg(iCallParams),
	iCallId(CTelephony::EISVCall1)
	{
	iCallParams.iIdRestrict = CTelephony::ESendMyId;
	}

TInt CDialNewCallTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->DialNewCall(aStatus, 
							iCallParamsPckg, 
							KGoodNumber(),
							iCallId, 
							CTelephony::EVoiceLine); 
	return KErrNone;
	}

TInt CDialNewCallTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EDialNewCallCancel);
	return KErrNone;
	}
	
//
CGetCallDynamicCaps::CGetCallDynamicCaps()
	:iCallCapsPckg(iCallCaps)
	{
	}
	
TInt CGetCallDynamicCaps::IssueRequestL(TRequestStatus& aStatus)
	{
	CTelephony::TCallId callId = CTelephony::EISVCall1;
	aStatus = iTelephony->GetCallDynamicCaps(callId, iCallCapsPckg);
	return KErrNone;
	}

TInt CGetCallDynamicCaps::CancelRequestL()
	{
	//Synchronous call
	return KErrNone;
	}


//
CGetCallStatus::CGetCallStatus()
	:iCallStatusPckg(iCallStatus)
	{
	}
	
TInt CGetCallStatus::IssueRequestL(TRequestStatus& aStatus)
	{
	CTelephony::TCallId callId = CTelephony::EISVCall1;
	aStatus = iTelephony->GetCallStatus(callId, iCallStatusPckg);

	return KErrNone;
	}

TInt CGetCallStatus::CancelRequestL()
	{
	//Synchronous call
	return KErrNone;
	}
	
//
CResumeTestStep::CResumeTestStep()
	:iCallParamsPckg(iCallParams)
	{
	//Set the call parameters for call 2
	iCallParams.iIdRestrict = CTelephony::ESendMyId;
	}
	
TInt CResumeTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	//Put on hold first
   	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &iCallParamsPckg;
	testCallParams1.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams1.iExpStat = KErrNone;
	testCallParams1.iCallId = CTelephony::EISVCall1;

	CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(NULL,iTelephony);	
	TestHoldAct->TestHold(testCallParams1);
	CActiveScheduler::Start();	
	CleanupStack::PopAndDestroy(TestHoldAct);
	// Resume
	iTelephony->Resume(aStatus, CTelephony::EISVCall1);
	return KErrNone;
	}
	
TInt CResumeTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EResumeCancel);
	return KErrNone;
	}	
	
//
CSwapTestStep::CSwapTestStep()
	{
	}
	
TInt CSwapTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->Swap(aStatus, CTelephony::EISVCall1, CTelephony::EISVCall2);
	return KErrNone;
	}
	
TInt CSwapTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::ESwapCancel);
	return KErrNone;
	}	
	
//
CHangUpTestStep::CHangUpTestStep()
	{
	}
	
TInt CHangUpTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->Hangup(aStatus, CTelephony::EISVCall1);
	return KErrNone;
	}
	
TInt CHangUpTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EHangupCancel);
	return KErrNone;
	}	
	
//
CAnswerIncomingCallTestStep::CAnswerIncomingCallTestStep()
	{
	iCallId = CTelephony::EISVCall1;
	}
	
TInt CAnswerIncomingCallTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	// get the tsy to call us
	RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyIncomingVoiceCall, 1);
	User::After(1000000L);
	
	// verify the status
	CTelephony::TCallStatusV1 callStatus;
	CTelephony::TCallStatusV1Pckg callStatusPckg(callStatus);
	iTelephony->GetLineStatus(CTelephony::EVoiceLine,  callStatusPckg);
	if (callStatus.iStatus != CTelephony::EStatusRinging) 
		{
		return KErrCancel;	
		}
	
	iTelephony->AnswerIncomingCall(aStatus, iCallId, CTelephony::EVoiceLine);
	return KErrNone;
	}
	
TInt CAnswerIncomingCallTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EAnswerIncomingCallCancel);
	return KErrNone;
	}	
	
//
		
