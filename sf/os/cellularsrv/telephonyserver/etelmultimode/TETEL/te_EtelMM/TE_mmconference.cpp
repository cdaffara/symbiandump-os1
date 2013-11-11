// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "TE_mmconference.h"
#include "testdef.h"

CTestConferenceCall::CTestConferenceCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestConferenceCall"));
	}

enum TVerdict CTestConferenceCall::doTestStepL()
	{
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Phone Conference Call Functionality"));
//	LOGTEXT(_L8("Test Mobile Phone Conference Call Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileConferenceCall cc;
	cc.Open(mmPhone);
	
	TRequestStatus reqStatus;

	TUint32 caps, asyncCaps;

	cc.GetCaps(caps);
	TEST(caps == DMMTSY_CONFERENCE_CAPS);

	INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::GetCaps (sync) passed"), iTestCount++);

	cc.NotifyCapsChange(reqStatus,asyncCaps);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	TEST(asyncCaps == DMMTSY_CONFERENCE_CAPS);

	cc.NotifyCapsChange(reqStatus,caps);
	cc.CancelAsyncRequest(EMobileConferenceCallNotifyCapsChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::NotifyCapsChange (async & cancel (Cancelled Request) passed"), iTestCount++);
	else
		{
		TEST(asyncCaps == DMMTSY_CONFERENCE_CAPS);
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::NotifyCapsChange (async & cancel (Request Not Cancelled) passed"), iTestCount++);
		}

	cc.CreateConference(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	cc.CreateConference(reqStatus);
	cc.CancelAsyncRequest(EMobileConferenceCallCreateConference);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::CreateConference (async & cancel (Cancelled Request) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::CreateConference (async & cancel (Request Not Cancelled) passed"), iTestCount++);
	

	TName callName = DMMTSY_CONFERENCE_CALL_NAME;
	cc.AddCall(reqStatus,callName);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	cc.AddCall(reqStatus,callName);
	cc.CancelAsyncRequest(EMobileConferenceCallAddCall);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::AddCall (async & cancel (Cancelled Request) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::AllCall (async & cancel (Request Not Cancelled) passed"), iTestCount++);


	cc.Swap(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	cc.Swap(reqStatus);
	cc.CancelAsyncRequest(EMobileConferenceCallSwap);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::Swap (async & cancel (Cancelled Request) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::Swap (async & cancel (Request Not Cancelled) passed"), iTestCount++);


	cc.HangUp(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	cc.HangUp(reqStatus);
	cc.CancelAsyncRequest(EMobileConferenceCallHangUp);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::HangUp (async & cancel (Cancelled Request) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::HangUp (async & cancel (Request Not Cancelled) passed"), iTestCount++);


	TInt calls;
	TEST(cc.EnumerateCalls(calls) == KErrNone);
	TEST(calls == DMMTSY_CONFERENCE_CALL_COUNT);
	INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::EnumerateCalls (sync) passed"), iTestCount++);

	RMobileCall::TMobileCallInfoV1 call;
	RMobileCall::TMobileCallInfoV1Pckg callPckg(call);

	TEST(cc.GetMobileCallInfo(DMMTSY_CONFERENCE_CALL_INDEX,callPckg) == KErrNone);
	TEST(call.iCallName.Compare(DMMTSY_CONFERENCE_CALL_NAME) == 0);
	TEST(call.iLineName.Compare(DMMTSY_CONFERENCE_CALL_LINE) == 0);

	INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::GetMobileCallInfo (sync) passed"), iTestCount++);

	RMobileConferenceCall::TMobileConferenceStatus callStatus, asyncCallStatus;

	TEST(cc.GetConferenceStatus(callStatus) == KErrNone);
	TEST(callStatus == DMMTSY_CONFERENCE_CALL_STATUS);
	INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::GetConferenceStatus (sync) passed"), iTestCount++);

	cc.NotifyConferenceStatusChange(reqStatus,asyncCallStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	TEST(asyncCallStatus == DMMTSY_CONFERENCE_CALL_STATUS);

	cc.NotifyConferenceStatusChange(reqStatus,asyncCallStatus);
	cc.CancelAsyncRequest(EMobileConferenceCallNotifyConferenceStatusChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::NotifyConferenceStatusChange (async & cancel (Cancelled Request) passed"), iTestCount++);
	else
		{
		TEST(asyncCallStatus == DMMTSY_CONFERENCE_CALL_STATUS);
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::NotifyConferenceStatusChange (async & cancel (Request Not Cancelled) passed"), iTestCount++);
		}

	RMobileConferenceCall::TMobileConferenceEvent event;
	TName eventName; 

	cc.NotifyConferenceEvent(reqStatus,event,eventName);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	TEST(event == DMMTSY_CONFERENCE_CALL_EVENT);
	TEST(eventName.Compare(DMMTSY_CONFERENCE_CALL_NAME) == 0);

	cc.NotifyConferenceEvent(reqStatus,event,eventName);
	cc.CancelAsyncRequest(EMobileConferenceCallNotifyConferenceEvent);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::NotifyConferenceEvent (async & cancel (Cancelled Request) passed"), iTestCount++);
	else
		{
		TEST(event == DMMTSY_CONFERENCE_CALL_EVENT);
		TEST(eventName.Compare(DMMTSY_CONFERENCE_CALL_NAME) == 0);
		INFO_PRINTF2(_L("Test %d - RMobileConferenceCall::NotifyConferenceEvent (async & cancel (Request Not Cancelled) passed"), iTestCount++);
		}

	cc.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}
