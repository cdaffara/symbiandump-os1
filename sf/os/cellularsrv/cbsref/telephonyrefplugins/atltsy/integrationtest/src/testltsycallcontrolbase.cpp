// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 

#include "te_integration_stltsysuitestepbase.h"
#include "te_integration_stltsysuitedefs.h"
#include "testltsycallcontrolbase.h"

//const define
_LIT16(KPhoneTSY, "phonetsy");
_LIT16(KLtsyTelNumOne, "58848158");
_LIT16(KLtsyTelNumTwo, "58848157");
_LIT16(KLtsyBadNumber, "11");
_LIT16(KLtsySelfNumber, "15901279603");


CTestLtsyCallControlBase::CTestLtsyCallControlBase(CTestLtsyModel& aModel) : iLtsyModel(aModel)
	{
	
	}

CTestLtsyCallControlBase::~CTestLtsyCallControlBase()
	{
	CloseLine();
	
	CloseCallOne();
	
	CloseCallTwo();	
	}

TVerdict CTestLtsyCallControlBase::doTestStepPreambleL()
	{
	
	OpenPhoneL();
	
	OpenLineL();
	
	OpenCustomAPIL();
	
	return EPass;
	}

TVerdict CTestLtsyCallControlBase::doTestStepPostambleL()
	{
	
	CloseLine();
	
	CloseCallOne();
	
	CloseCallTwo();
	
	CloseCustomAPI();
	
	return EPass;
	}

TVerdict CTestLtsyCallControlBase::doTestStepL()
	{
	//Open Call
	OpenCallOneL();
	
	//Dial Call
	DialCallOne();
	
	User::After(1000*1000);
	
	//Hold call
	HoldCallOne();
	
	User::After(1000*1000);
	
	//Resume call
	ResumeCallOne();
	
	User::After(1000*1000);
	
	//Hold call
	HoldCallOne();
	
	User::After(1000*1000);
	
	//Resume call
	ResumeCallOne();
	
	User::After(1000*1000);
	
	//Hang up call
	HangupCallOneL();
	
	User::After(1000*1000);
	
	//Close call
	CloseCallOne();
	
	return EPass;
	}

void CTestLtsyCallControlBase::OpenPhoneL()
	{
	INFO_PRINTF1(_L("starting opne phone"));
	
	if (!iLtsyModel.SharedEnv().PhoneOpened())
		{
		iLtsyModel.SharedEnv().OpenPhoneL(KPhoneTSY);
		}
	}

void CTestLtsyCallControlBase::OpenLineL()
	{
	INFO_PRINTF1(_L("starting opne line"));	
	
	if (iLtsyModel.SharedEnv().PhoneOpened())
		{
		TInt numberLines = 0;
		User::LeaveIfError(iLtsyModel.SharedEnv().Phone().EnumerateLines(numberLines));
		RPhone::TLineInfo lineInfo;
		TBool foundLine = EFalse;
		for (TInt a = 0; a < numberLines; a++)
			{
			User::LeaveIfError(iLtsyModel.SharedEnv().Phone().GetLineInfo(a, lineInfo));
			if (lineInfo.iLineCapsFlags & RLine::KCapsVoice)
				{
				foundLine = ETrue;
				break;
				}
			}
		if (!foundLine)
			{
			INFO_PRINTF1(_L("The phone has opened fail"));	
			User::Leave(KErrNotFound);
			}
		
		User::LeaveIfError(iLtsyModel.CallSharedData().VoiceLine().Open(iLtsyModel.SharedEnv().Phone(), lineInfo.iName));
		INFO_PRINTF1(_L("The line open successful"));	
		}
	else
		{
		INFO_PRINTF1(_L("The phone has opened fail"));	
		}
	}

void CTestLtsyCallControlBase::CloseLine()
	{
	iLtsyModel.CallSharedData().VoiceLine().Close();
	}

void CTestLtsyCallControlBase::OpenCustomAPIL()
	{
	INFO_PRINTF1(_L("starting opne custom API"));
	
	
	if (iLtsyModel.SharedEnv().PhoneOpened())
		{
		User::LeaveIfError(iLtsyModel.CallSharedData().MmCustomAPI().Open(iLtsyModel.SharedEnv().Phone()));
		INFO_PRINTF1(_L("The custom API open successfully"));
		}
	else
		{
		INFO_PRINTF1(_L("The phone has opened fail"));
		}
	}

void CTestLtsyCallControlBase::CloseCustomAPI()
	{
	INFO_PRINTF1(_L("starting close custom API"));
	iLtsyModel.CallSharedData().MmCustomAPI().Close();
	}

void CTestLtsyCallControlBase::CustomAPITerminateAllCallsL()
	{
	INFO_PRINTF1(_L("RCustomAPI start terminate all calls"));
	
	TBuf<128> allcall(_L("ALL_CALLS"));
	TRequestStatus iStatus;
	iLtsyModel.CallSharedData().MmCustomAPI().TerminateCall(iStatus, allcall);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("RCustomAPI terminate all calls result = %d"), iStatus.Int());
	
	if (iStatus.Int() != KErrNone)
		{
		User::LeaveIfError(iStatus.Int());
		}	
	}

void CTestLtsyCallControlBase::PhoneTerminateAllCallsL()
	{
	INFO_PRINTF1(_L("phone start terminate all calls"));
	
	TRequestStatus iStatus;
	iLtsyModel.SharedEnv().Phone().TerminateAllCalls(iStatus);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("phone terminate all calls result = %d"), iStatus.Int());
	
	if (iStatus.Int() != KErrNone)
		{
		User::LeaveIfError(iStatus.Int());
		}		
	}

//It must open RCustomAPI before create CTelephony
void CTestLtsyCallControlBase::CreateThirtyPartyTelephonyL()
	{
	iLtsyModel.CallSharedData().ThirdPartyTelephony() = CTelephony::NewL();
	}

void CTestLtsyCallControlBase::DialCallOne()
	{
	INFO_PRINTF1(_L("starting dial call one"));
	INFO_PRINTF2(_L("starting dial number %S..."), &KLtsyTelNumOne);
	INFO_PRINTF1(_L("If ring please pick up your telephone "));
	
	TRequestStatus tDialStatus;
	TRequestStatus tNotifyStatus;
	
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallOne().Dial(tDialStatus, KLtsyTelNumOne);
	User::WaitForRequest(tDialStatus);
	
	INFO_PRINTF3(_L("Dial number %S\t Result = %d"), &KLtsyTelNumOne, tDialStatus.Int());
	
	if (tDialStatus.Int() == KErrNone)
		{
		User::WaitForRequest(tNotifyStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Dial number first call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Dial call notify call status fail"));
			return;
			}
		
		iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
		User::WaitForRequest(tNotifyStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Dial number second call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Dial call notify call status fail"));
			return;
			}
		
		iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
		User::WaitForRequest(tNotifyStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Dial number three call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Dial call notify call status fail"));
			return;
			}		
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallOne().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		}
	}

void CTestLtsyCallControlBase::CallOneDialBadNumberL()
	{
	INFO_PRINTF1(_L("starting dial call one"));
	INFO_PRINTF2(_L("starting dial bad number %S..."), &KLtsyBadNumber);
	
	TRequestStatus tDialStatus;
	iLtsyModel.CallSharedData().VoiceCallOne().Dial(tDialStatus, KLtsyBadNumber);
	User::WaitForRequest(tDialStatus);	
	
	User::LeaveIfError(tDialStatus.Int());
	}

void CTestLtsyCallControlBase::CallOneDialSelfNumberL()
	{
	INFO_PRINTF1(_L("starting dial call one"));
	INFO_PRINTF2(_L("starting dial bad number %S..."), &KLtsySelfNumber);
	
	TRequestStatus tDialStatus;
	iLtsyModel.CallSharedData().VoiceCallOne().Dial(tDialStatus, KLtsySelfNumber);
	User::WaitForRequest(tDialStatus);	
	
	User::LeaveIfError(tDialStatus.Int());	
	}

void CTestLtsyCallControlBase::DialCallTwo()
	{
	INFO_PRINTF1(_L("starting dial call two"));
	INFO_PRINTF2(_L("starting dial number %S..."), &KLtsyTelNumTwo);
	
	TRequestStatus tDialStatus;
	TRequestStatus tNotifyStatus;
	
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallTwo().Dial(tDialStatus, KLtsyTelNumTwo);
	User::WaitForRequest(tDialStatus);
	
	INFO_PRINTF3(_L("Dial number %S\t Result = %d"), &KLtsyTelNumTwo, tDialStatus.Int());
	
	if (tDialStatus.Int() == KErrNone)
		{
		User::WaitForRequest(tNotifyStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Dial number first call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Dial call notify call status fail"));
			return;
			}
		
		iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
		User::WaitForRequest(tNotifyStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Dial number second call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Dial call notify call status fail"));
			return;
			}
		
		iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
		User::WaitForRequest(tNotifyStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Dial number three call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Dial call notify call status fail"));
			return;
			}		
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallTwo().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		}
	}

void CTestLtsyCallControlBase::HangupCallOneL()
	{
	INFO_PRINTF1(_L("starting hang up call one"));
	
	TRequestStatus tHangupStatus;
	TRequestStatus tNotifyStatus;	
	
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallOne().HangUp(tHangupStatus);
	User::WaitForRequest(tHangupStatus);
	
	INFO_PRINTF2(_L("Hang up call one result = %d"), tHangupStatus.Int());
	
	if (tHangupStatus.Int() == KErrNone)
		{
		User::WaitForRequest(tNotifyStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Hang up notify call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Hang up notify call status fail"));
			User::LeaveIfError(tNotifyStatus.Int());
			}
		
		RMobileCall::TMobileCallStatus tCallStatus;
		iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Hang up notify call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Hang up notify call status fail"));
			User::LeaveIfError(tNotifyStatus.Int());
			}
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallOne().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		User::LeaveIfError(tHangupStatus.Int());
		}
	}

void CTestLtsyCallControlBase::HangupCallTwoL()
	{
	INFO_PRINTF1(_L("starting hang up call two"));
	
	TRequestStatus tHangupStatus;
	TRequestStatus tNotifyStatus;	
	
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallTwo().HangUp(tHangupStatus);
	User::WaitForRequest(tHangupStatus);
	
	INFO_PRINTF2(_L("Hang up call one result = %d"), tHangupStatus.Int());
	
	if (tHangupStatus.Int() == KErrNone)
		{
		User::WaitForRequest(tNotifyStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Hang up notify call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Hang up notify call status fail"));
			User::LeaveIfError(tNotifyStatus.Int());
			}
		
		RMobileCall::TMobileCallStatus tCallStatus;
		iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(tNotifyStatus, tCallStatus);
		if (tNotifyStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("Hang up notify call status = %d"), tCallStatus);
			}
		else
			{
			INFO_PRINTF1(_L("Hang up notify call status fail"));
			User::LeaveIfError(tNotifyStatus.Int());
			}
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallTwo().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		User::LeaveIfError(tHangupStatus.Int());
		}	
	}

void CTestLtsyCallControlBase::CloseCallOne()
	{
	INFO_PRINTF1(_L("starting close call one"));
	
	iLtsyModel.CallSharedData().VoiceCallOne().Close();
	}

void CTestLtsyCallControlBase::CloseCallTwo()
	{
	INFO_PRINTF1(_L("starting close call two"));
	
	iLtsyModel.CallSharedData().VoiceCallTwo().Close();	
	}

void CTestLtsyCallControlBase::OpenCallOneL()
	{
	INFO_PRINTF1(_L("starting open call one"));
	
	User::LeaveIfError(iLtsyModel.CallSharedData().VoiceCallOne().OpenNewCall(iLtsyModel.CallSharedData().VoiceLine()));
	}

void CTestLtsyCallControlBase::OpenCallTwoL()
	{
	INFO_PRINTF1(_L("starting open call two"));
	
	User::LeaveIfError(iLtsyModel.CallSharedData().VoiceCallTwo().OpenNewCall(iLtsyModel.CallSharedData().VoiceLine()));	
	}

void CTestLtsyCallControlBase::HoldCallOne()
	{
	INFO_PRINTF1(_L("starting hold call one"));
	
	TRequestStatus iStatus;
	TRequestStatus iCallChangeStatus;
	TRequestStatus iCallEvent;
	
	RMobileCall::TMobileCallStatus tCallStatus;
	RMobileCall::TMobileCallEvent tCallEvent;
	
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);	
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyCallEvent(iCallEvent, tCallEvent);	
	
	iLtsyModel.CallSharedData().VoiceCallOne().Hold(iStatus);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(_L("Hold call one result = %d"), iStatus.Int());
	
	if (iStatus.Int() == KErrNone)
		{
		User::WaitForRequest(iCallChangeStatus);
		INFO_PRINTF2(_L("Hold call notify call status = %d"), tCallStatus);
		
		User::WaitForRequest(iCallEvent);
		INFO_PRINTF2(_L("Hold call notify call Event = %d"), tCallEvent);
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallOne().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		iLtsyModel.CallSharedData().VoiceCallOne().CancelAsyncRequest(EMobileCallNotifyCallEvent);
		}
	}

void CTestLtsyCallControlBase::HoldCallTwo()
	{
	INFO_PRINTF1(_L("starting hold call two"));
	
	TRequestStatus iStatus;
	TRequestStatus iCallChangeStatus;
	TRequestStatus iCallEvent;
	
	RMobileCall::TMobileCallStatus tCallStatus;
	RMobileCall::TMobileCallEvent tCallEvent;
	
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);	
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyCallEvent(iCallEvent, tCallEvent);	
	
	iLtsyModel.CallSharedData().VoiceCallTwo().Hold(iStatus);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(_L("Hold call two result = %d"), iStatus.Int());
	
	if (iStatus.Int() == KErrNone)
		{
		User::WaitForRequest(iCallChangeStatus);
		INFO_PRINTF2(_L("Hold call notify call status = %d"), tCallStatus);
		
		User::WaitForRequest(iCallEvent);
		INFO_PRINTF2(_L("Hold call notify call Event = %d"), tCallEvent);
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallTwo().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		iLtsyModel.CallSharedData().VoiceCallOne().CancelAsyncRequest(EMobileCallNotifyCallEvent);
		}	
	}
	
void CTestLtsyCallControlBase::ResumeCallOne()
	{
	INFO_PRINTF1(_L("starting resume call one"));
	
	TRequestStatus iStatus;
	
	TRequestStatus iCallChangeStatus;
	RMobileCall::TMobileCallStatus tCallStatus;
	
	TRequestStatus iCallEvent;
	RMobileCall::TMobileCallEvent tCallEvent;
	
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyCallEvent(iCallEvent, tCallEvent);
	
	iLtsyModel.CallSharedData().VoiceCallOne().Resume(iStatus);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("Resume call one result = %d"), iStatus.Int());
	
	if (iStatus.Int() == KErrNone)
		{
		User::WaitForRequest(iCallChangeStatus);
		INFO_PRINTF2(_L("Resume call one status = %d"), tCallStatus);
		
		User::WaitForRequest(iCallEvent);
		INFO_PRINTF2(_L("Resume call one call event = %d"), tCallEvent);
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallOne().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		iLtsyModel.CallSharedData().VoiceCallOne().CancelAsyncRequest(EMobileCallNotifyCallEvent);
		}
	}

void CTestLtsyCallControlBase::ResumeCallTwo()
	{
	INFO_PRINTF1(_L("starting resume call two"));
	
	TRequestStatus iStatus;
	
	TRequestStatus iCallChangeStatus;
	RMobileCall::TMobileCallStatus tCallStatus;
	
	TRequestStatus iCallEvent;
	RMobileCall::TMobileCallEvent tCallEvent;
	
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyCallEvent(iCallEvent, tCallEvent);
	
	iLtsyModel.CallSharedData().VoiceCallTwo().Resume(iStatus);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("Resume call two result = %d"), iStatus.Int());
	
	if (iStatus.Int() == KErrNone)
		{
		User::WaitForRequest(iCallChangeStatus);
		INFO_PRINTF2(_L("Resume call two status = %d"), tCallStatus);
		
		User::WaitForRequest(iCallEvent);
		INFO_PRINTF2(_L("Resume call two call event = %d"), tCallEvent);
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallTwo().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		iLtsyModel.CallSharedData().VoiceCallTwo().CancelAsyncRequest(EMobileCallNotifyCallEvent);
		}	
	}

void CTestLtsyCallControlBase::SwapCallOne()
	{
	INFO_PRINTF1(_L("starting swap call one"));
	
	TRequestStatus iStatus;
	TRequestStatus iCallChangeStatus;
	
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallOne().Swap(iStatus);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("call one swap result = %d"), iStatus.Int());
	
	if (iStatus.Int() == KErrNone)
		{
		User::WaitForRequest(iCallChangeStatus);
		INFO_PRINTF2(_L("call one swap status = %d"), tCallStatus);
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallOne().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		}
	}
	
void CTestLtsyCallControlBase::NotifyCallOne()
	{
	INFO_PRINTF1(_L("starting notify call one"));
	
	TRequestStatus iCallChangeStatus;
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
	User::WaitForRequest(iCallChangeStatus);
	INFO_PRINTF3(_L("call one after swap result = %d\tstatus = %d"), iCallChangeStatus.Int(), tCallStatus);	
	}

void CTestLtsyCallControlBase::NotifyCallTwo()
	{
	INFO_PRINTF1(_L("starting notify call two"));
	
	TRequestStatus iCallChangeStatus;
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
	User::WaitForRequest(iCallChangeStatus);
	INFO_PRINTF3(_L("call two after swap result = %d\tstatus = %d"), iCallChangeStatus.Int(), tCallStatus);		
	}

void CTestLtsyCallControlBase::SwapCallTwo()
	{
	INFO_PRINTF1(_L("starting swap call two"));
	
	TRequestStatus iStatus;
	TRequestStatus iCallChangeStatus;
	
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallTwo().Swap(iStatus);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("call two swap result = %d"), iStatus.Int());
	
	if (iStatus.Int() == KErrNone)
		{
		User::WaitForRequest(iCallChangeStatus);
		INFO_PRINTF2(_L("call two swap status = %d"), tCallStatus);
		}
	else
		{
		iLtsyModel.CallSharedData().VoiceCallTwo().CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		}	
	}

void CTestLtsyCallControlBase::NotifyIncomingCallOneL()
	{
	INFO_PRINTF1(_L("starting notify incoming call one"));
	
	TName name;
	TRequestStatus iNotifyIncomingCallStatus;
	iLtsyModel.CallSharedData().VoiceLine().NotifyIncomingCall(iNotifyIncomingCallStatus, name);
	User::WaitForRequest(iNotifyIncomingCallStatus);
	
	INFO_PRINTF2(_L("Notify incoming call result = %d"), iNotifyIncomingCallStatus.Int());
	
	if (iNotifyIncomingCallStatus.Int() == KErrNone)
		{
		INFO_PRINTF3(_L("Notify incoming call %S Result = %d"), &name, iNotifyIncomingCallStatus.Int());
		
		INFO_PRINTF1(_L("Starting Open Call one for incoming call..."));
		
		TInt nRes = iLtsyModel.CallSharedData().VoiceCallOne().OpenExistingCall(iLtsyModel.CallSharedData().VoiceLine(), name);

		//Get Call info
		RMobileCall::TMobileCallInfoV1 tCallInfo;
		RMobileCall::TMobileCallInfoV1Pckg tCallInfoPckg(tCallInfo);
		iLtsyModel.CallSharedData().VoiceCallOne().GetMobileCallInfo(tCallInfoPckg);
		
		INFO_PRINTF2(_L("Remote call service(voice=1,data=3,fax=5) = %d"), tCallInfo.iService);

		INFO_PRINTF2(_L("Remote call telnum = %S"), &(tCallInfo.iRemoteParty.iRemoteNumber.iTelNumber));
		
		}
	else
		{
		User::LeaveIfError(iNotifyIncomingCallStatus.Int());
		}
	}

void CTestLtsyCallControlBase::AnswerIncomingCallOneL()
	{
	INFO_PRINTF1(_L("starting answering incoming call one"));
	
	TRequestStatus iStatus;
	TRequestStatus iCallChangeStatus;
	
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallOne().AnswerIncomingCall(iStatus);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("answering call result = %d"), iStatus.Int());
	
	if (iStatus.Int() == KErrNone)
		{
		User::WaitForRequest(iCallChangeStatus);
		if (iCallChangeStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("first answering call status = %d"), tCallStatus);
			}
		else
			{
			User::LeaveIfError(iCallChangeStatus.Int());
			}
		
		iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
		User::WaitForRequest(iCallChangeStatus);
		if (iCallChangeStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("second answering call status = %d"), tCallStatus);
			}
		else
			{
			User::LeaveIfError(iCallChangeStatus.Int());
			}
		}
	else
		{
		User::LeaveIfError(iStatus.Int());
		}
	}

void CTestLtsyCallControlBase::AnswerIncomingCallTwoL()
	{
	INFO_PRINTF1(_L("starting answering incoming call two"));
	
	TRequestStatus iStatus;
	TRequestStatus iCallChangeStatus;
	
	RMobileCall::TMobileCallStatus tCallStatus;
	iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
	
	iLtsyModel.CallSharedData().VoiceCallTwo().AnswerIncomingCall(iStatus);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("answering call result = %d"), iStatus.Int());
	
	if (iStatus.Int() == KErrNone)
		{
		User::WaitForRequest(iCallChangeStatus);
		if (iCallChangeStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("first answering call status = %d"), tCallStatus);
			}
		else
			{
			User::LeaveIfError(iCallChangeStatus.Int());
			}
		
		iLtsyModel.CallSharedData().VoiceCallTwo().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
		User::WaitForRequest(iCallChangeStatus);
		if (iCallChangeStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("second answering call status = %d"), tCallStatus);
			}
		else
			{
			User::LeaveIfError(iCallChangeStatus.Int());
			}
		
		
		iLtsyModel.CallSharedData().VoiceCallOne().NotifyMobileCallStatusChange(iCallChangeStatus, tCallStatus);
		User::WaitForRequest(iCallChangeStatus);
		if (iCallChangeStatus.Int() == KErrNone)
			{
			INFO_PRINTF2(_L("call one swith call status = %d"), tCallStatus);
			}
		else
			{
			User::LeaveIfError(iCallChangeStatus.Int());
			}
		
		}
	else
		{
		User::LeaveIfError(iStatus.Int());
		}	
	}

void CTestLtsyCallControlBase::NotifyIncomingCallTwoL()
	{
	INFO_PRINTF1(_L("starting notify incoming call two"));
	
	TName name;
	TRequestStatus iNotifyIncomingCallStatus;
	iLtsyModel.CallSharedData().VoiceLine().NotifyIncomingCall(iNotifyIncomingCallStatus, name);
	User::WaitForRequest(iNotifyIncomingCallStatus);
	
	INFO_PRINTF2(_L("Notify incoming call result = %d"), iNotifyIncomingCallStatus.Int());
	
	if (iNotifyIncomingCallStatus.Int() == KErrNone)
		{
		INFO_PRINTF3(_L("Notify incoming call %S Result = %d"), &name, iNotifyIncomingCallStatus.Int());
		
		INFO_PRINTF1(_L("Starting Open Call one for incoming call..."));
		
		TInt nRes = iLtsyModel.CallSharedData().VoiceCallTwo().OpenExistingCall(iLtsyModel.CallSharedData().VoiceLine(), name);

		//Get Call info
		RMobileCall::TMobileCallInfoV1 tCallInfo;
		RMobileCall::TMobileCallInfoV1Pckg tCallInfoPckg(tCallInfo);
		iLtsyModel.CallSharedData().VoiceCallTwo().GetMobileCallInfo(tCallInfoPckg);
		
		INFO_PRINTF2(_L("Remote call service(voice=1,data=3,fax=5) = %d"), tCallInfo.iService);

		INFO_PRINTF2(_L("Remote call telnum = %S"), &(tCallInfo.iRemoteParty.iRemoteNumber.iTelNumber));
		
		}	
	else
		{
		User::LeaveIfError(iNotifyIncomingCallStatus.Int());
		}
	}

//End of file
