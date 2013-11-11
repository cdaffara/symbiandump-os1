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

#include "mmtsy.h"
#include "testdef.h"
#include "ETELMM.H"
#include <et_clsvr.h>
#include "Dmmlog.h"

//#define ASYNC_TIMEOUT 300000	// 0.3s

//
//	CCallDMmTsy
//
CCallDMmTsy* CCallDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac)
	{
	CCallDMmTsy* call=new(ELeave) CCallDMmTsy(aPhone,aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallDMmTsy::CCallDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac)
	:CCallDummyBase(aFac), iPhone(aPhone)
	{}

void CCallDMmTsy::ConstructL()
	{
	CCallDummyBase::ConstructL();
	LOGTEXT(_L8("CCallDMmTsy created"));
	}

CCallDMmTsy::~CCallDMmTsy()
	{
	LOGTEXT(_L8("CCallDMmTsy destroyed"));
	}

CTelObject* CCallDMmTsy::OpenNewObjectByNameL(const TDesC& aName)
	{
	// only fax objects can be opened from call objects
	if (aName.Compare(_L("FAX"))==KErrNone)
		return REINTERPRET_CAST(CTelObject*, CFaxDMmTsy::NewL(FacPtr()));
	else
		return NULL;
	}

CTelObject* CCallDMmTsy::OpenNewObjectL(TDes& /*aNewName*/)
	{
	// opening fax without a name is not supported
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CCallDMmTsy::ReqModeL(const TInt aIpc)
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has

	// The following are example request types for this dummy TSY
	// All TSYs do not have to have these request types but they have been given
	// "sensible" values in this test code

	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// No Flow Control NOR Multiple Completion
//
	case EMobileCallGetMobileCallCaps:
	case EMobileCallGetMobileCallStatus:
	case EMobileCallGetMobileCallInfo:
	case EMobileCallGetMobileDataCallCaps:
	case EMobileCallGetMobileDataCallRLPRange:
	case EMobileCallSetDynamicHscsdParams:
	case EMobileCallGetCurrentHscsdInfo:
	case EMobileCallSwitchAlternatingCall:
	case EMobileCallDeflect:
	case EMobileCallTransfer:
	case EMobileCallGoOneToOne:
	case EMobileCallActivateCCBS:
	case EMobileCallRejectCCBS:
	case EMobileCallSetPrivacy:
	case EMobileCallSetTrafficChannel:
	case EMobileCallNotifyTrafficChannelConfirmation:
	case EMobileCallGetUUSCaps:
	case EMobileCallActivateUUS:
	case EMobileCallSendUUI:
	case EMobileCallReceiveUUI:
	case EMobileCallHangupWithUUI:
	case EMobileCallAnswerWithUUI:
	case EMobileCallAnswerMultimediaCallAsVoice:
	case EMobileCallAnswerISV:
		break;
//
// Flow Controlled Services
//
	case EMobileCallHold:
	case EMobileCallResume:
	case EMobileCallSwap:
	case EMobileCallDialEmergencyCall:
	case EMobileCallDialNoFdnCheck:
	case EMobileCallDialISV:
	case EMobileCallDialCallControl:
		ret=KReqModeFlowControlObeyed;
		break;

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileCallNotifyMobileCallStatusChange:
	case EMobileCallNotifyMobileCallCapsChange:
	case EMobileCallNotifyCallEvent:
	case EMobileCallNotifyRemotePartyInfoChange:
	case EMobileCallNotifyMobileDataCallCapsChange:
	case EMobileCallNotifyHscsdInfoChange:
	case EMobileCallNotifyVoiceFallback:
	case EMobileCallNotifyAlternatingCallSwitch:
	case EMobileCallNotifyPrivacyConfirmation:
	case EMobileCallNotifyUUSCapsChange:
	case EMobileCallNotifyAudioToneEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Not Supported
//
	case EMobileCallNotifyMobileCallStatusChangeCancel:
	case EMobileCallHoldCancel:
	case EMobileCallResumeCancel:
	case EMobileCallSwapCancel:
	case EMobileCallNotifyMobileCallCapsChangeCancel:
	case EMobileCallNotifyCallEventCancel:
	case EMobileCallDialEmergencyCallCancel:
	case EMobileCallGetMobileCallInfoCancel:
	case EMobileCallNotifyRemotePartyInfoChangeCancel:
	case EMobileCallNotifyMobileDataCallCapsChangeCancel:
	case EMobileCallGetMobileDataCallRLPRangeCancel:
	case EMobileCallSetDynamicHscsdParamsCancel:
	case EMobileCallSwitchAlternatingCallCancel:
	case EMobileCallActivateCCBSCancel:
	case EMobileCallNotifyPrivacyConfirmationCancel:
	case EMobileCallNotifyTrafficChannelConfirmationCancel:
	case EMobileCallNotifyVoiceFallbackCancel:
	case EMobileCallNotifyUUSCapsChangeCancel:
	case EMobileCallActivateUUSCancel:
	case EMobileCallSendUUICancel:
	case EMobileCallReceiveUUICancel:
	case EMobileCallHangupWithUUICancel:
	case EMobileCallAnswerWithUUICancel:
	case EMobileCallDialNoFdnCheckCancel:
	case EMobileCallDialCallControlCancel:
	case EMobileCallAnswerMultimediaCallAsVoiceCancel:
	case EMobileCallNotifyAudioToneEventCancel:
	case EMobileCallDialISVCancel:
	case EMobileCallAnswerISVCancel:
		User::Leave(KErrNotSupported);
		break;

	default:
		ret=CCallBase::ReqModeL(aIpc);
		break;
		}
	return ret;
	}


TInt CCallDMmTsy::RegisterNotification(const TInt aIpc)
	{
	// RegisterNotification is called when the server recognises that this notification
	// is being posted for the first time on this sub-session object.

	// It enables the TSY to "turn on" any regular notification messages that it may 
	// receive from the phone

	switch (aIpc)
		{
	case EMobileCallNotifyMobileCallStatusChange:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Call Status Change "));
		return KErrNone;
	case EMobileCallNotifyMobileCallCapsChange:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Call Control Caps Change "));
		return KErrNone;
	case EMobileCallNotifyCallEvent:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Call Event "));
		return KErrNone;
	case EMobileCallNotifyRemotePartyInfoChange:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Remote Party Info Change "));
		return KErrNone;
	case EMobileCallNotifyMobileDataCallCapsChange:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Data Call Caps Change "));
		return KErrNone;
	case EMobileCallNotifyHscsdInfoChange:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Notify Hscsd Info Change "));
		return KErrNone;
	case EMobileCallNotifyVoiceFallback:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Notify Voice Fallback "));
		return KErrNone;
	case EMobileCallNotifyAlternatingCallSwitch:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Notify Alternating Call Switch "));
		return KErrNone;
	case EMobileCallNotifyPrivacyConfirmation:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Notify Privacy Confirmation "));
		return KErrNone;
	case EMobileCallNotifyTrafficChannelConfirmation:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Notify Traffic Channel Confirmation "));
		return KErrNone;
	case EMobileCallNotifyUUSCapsChange:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Notify UUS Caps Change "));
		return KErrNone;
	case EMobileCallNotifyAudioToneEvent:
		LOGTEXT(_L8("CCallDMmTsy: RegisterNotification - Notify Audio Tone Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CCallDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CCallDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	// DeregisterNotification is called when the server recognises that this notification
	// will not be posted again because the last client to have a handle on this sub-session
	// object has just closed the handle.

	// It enables the TSY to "turn off" any regular notification messages that it may 
	// receive from the phone

	switch (aIpc)
		{
	case EMobileCallNotifyMobileCallStatusChange:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Call Status Change "));
		return KErrNone;
	case EMobileCallNotifyMobileCallCapsChange:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Call Control Caps Change "));
		return KErrNone;
	case EMobileCallNotifyCallEvent:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Call Event "));
		return KErrNone;
	case EMobileCallNotifyRemotePartyInfoChange:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Remote Party Info Change "));
		return KErrNone;
	case EMobileCallNotifyMobileDataCallCapsChange:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Data Call Caps Change "));
		return KErrNone;
	case EMobileCallNotifyHscsdInfoChange:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Notify Hscsd Info Change "));
		return KErrNone;
	case EMobileCallNotifyVoiceFallback:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Notify Voice Fallback "));
		return KErrNone;
	case EMobileCallNotifyAlternatingCallSwitch:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Notify Alternating Call Switch "));
		return KErrNone;
	case EMobileCallNotifyPrivacyConfirmation:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Notify Privacy Confirmation "));
		return KErrNone;
	case EMobileCallNotifyTrafficChannelConfirmation:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Notify Traffic Channel Confirmation "));
		return KErrNone;
	case EMobileCallNotifyUUSCapsChange:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Notify UUS Caps Change "));
		return KErrNone;
	case EMobileCallNotifyAudioToneEvent:
		LOGTEXT(_L8("CCallDMmTsy: DeregisterNotification - Notify Audio Tone Event "));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CCallDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CCallDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileCallNotifyMobileCallStatusChange:
	case EMobileCallNotifyMobileCallCapsChange:
	case EMobileCallNotifyCallEvent:
	case EMobileCallNotifyMobileDataCallCapsChange:
	case EMobileCallNotifyHscsdInfoChange:
	case EMobileCallNotifyVoiceFallback:
	case EMobileCallNotifyAlternatingCallSwitch:
	case EMobileCallNotifyPrivacyConfirmation:
	case EMobileCallNotifyTrafficChannelConfirmation:
	case EMobileCallNotifyUUSCapsChange:
	case EMobileCallNotifyAudioToneEvent:
		LOGTEXT(_L8("CCallDMmTsy: Registered with 5 slots"));
		numberOfSlots=5;
		break;
	case EMobileCallNotifyRemotePartyInfoChange:
	
		LOGTEXT(_L8("CCallDMmTsy: Registered with 1 slot"));
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CCallDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CCallDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
						const TDataPackage& aPackage)
	{
	// ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
	// for the TSY to process
	// A request handle, request type and request data are passed to the TSY

	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type

	switch (aIpc)
		{
//
// No Flow Control NOR Multiple Completion
//
	case EMobileCallGetMobileCallCaps:
		return GetMobileCallCaps(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallGetMobileCallStatus:
		return GetMobileCallStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*,dataPtr));

	case EMobileCallGetMobileCallInfo:
		return GetMobileCallInfo(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallGetMobileDataCallCaps:
		return GetMobileDataCallCaps(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallGetMobileDataCallRLPRange:
		return GetMobileDataCallRLPRange(aTsyReqHandle,
			REINTERPRET_CAST(TInt *, dataPtr), 
            aPackage.Des2n());

	case EMobileCallSetDynamicHscsdParams:
		return SetDynamicHscsdParams(aTsyReqHandle,
			REINTERPRET_CAST(TInt*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobileCallGetCurrentHscsdInfo:
		return GetCurrentHscsdInfo(aTsyReqHandle,aPackage.Des1n());

	case EMobileCallSwitchAlternatingCall:
		return SwitchAlternatingCall(aTsyReqHandle);

	case EMobileCallDeflect:
		return Deflect(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallDeflect*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobileAddress*, dataPtr2));

	case EMobileCallTransfer:
		return Transfer(aTsyReqHandle);

	case EMobileCallGoOneToOne:
		return GoOneToOne(aTsyReqHandle);

	case EMobileCallActivateCCBS:
		return ActivateCCBS(aTsyReqHandle, 
			REINTERPRET_CAST(TInt*, dataPtr));
	
	case EMobileCallRejectCCBS:
		return RejectCCBS(aTsyReqHandle);

	case EMobileCallSetPrivacy:
		return SetPrivacySetting(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePrivacy*, dataPtr));

	case EMobileCallNotifyPrivacyConfirmation:
		return NotifyPrivacyConfirmation(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePrivacy*, dataPtr));

	case EMobileCallSetTrafficChannel:
		return SetTrafficChannel(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallTch*, dataPtr));

	case EMobileCallNotifyTrafficChannelConfirmation:
		return NotifyTrafficChannelConfirmation(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallTch*,dataPtr));

	case EMobileCallGetUUSCaps:
		return GetUUSCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*,dataPtr));

	case EMobileCallActivateUUS:
		return ActivateUUS(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallSendUUI:
		return SendUUI(aTsyReqHandle,
			REINTERPRET_CAST(TInt*,dataPtr), aPackage.Des2u());

	case EMobileCallReceiveUUI:
		return ReceiveUUI(aTsyReqHandle, aPackage.Des1u());

	case EMobileCallHangupWithUUI:
		return HangupWithUUI(aTsyReqHandle, aPackage.Des1u());

	case EMobileCallAnswerWithUUI:
		return AnswerIncomingCallWithUUI(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2u()); 
	
	case EMobileCallAnswerMultimediaCallAsVoice:
		return AnswerMultimediaCallAsVoice(aTsyReqHandle, 
				aPackage.Des1n(), aPackage.Des2u());

	case EMobileCallAnswerISV:
		return AnswerIncomingCallISV(aTsyReqHandle, aPackage.Des1n());

//
// Flow Controlled Services
//
	case EMobileCallHold:
		return Hold(aTsyReqHandle);

	case EMobileCallResume:
		return Resume(aTsyReqHandle);

	case EMobileCallSwap:
		return Swap(aTsyReqHandle);

	case EMobileCallDialEmergencyCall:
		return DialEmergencyCall(aTsyReqHandle, aPackage.Des1u());

	case EMobileCallDialNoFdnCheck:
		return DialNoFdnCheck(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2u());
	
	case EMobileCallDialISV:
		return DialISV(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2u());
		
	case EMobileCallDialCallControl:
		return DialCallControl(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2u());

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileCallNotifyMobileCallStatusChange:
		return NotifyMobileCallStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*, dataPtr));

	case EMobileCallNotifyMobileCallCapsChange:
		return NotifyMobileCallCapsChange(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallNotifyCallEvent:
		return NotifyCallEvent(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallEvent*, dataPtr));

	case EMobileCallNotifyRemotePartyInfoChange:
		return NotifyRemotePartyInfoChange(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallNotifyMobileDataCallCapsChange:
		return NotifyMobileDataCallCapsChange(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallNotifyHscsdInfoChange:
		return NotifyHscsdInfoChange(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallNotifyVoiceFallback:
		return NotifyVoiceFallback(aTsyReqHandle, aPackage.Des1u());

	case EMobileCallNotifyAlternatingCallSwitch:
		return NotifyAlternatingCallSwitch(aTsyReqHandle);

	case EMobileCallNotifyUUSCapsChange:
		return NotifyUUSCapsChange(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));
			
	case EMobileCallNotifyAudioToneEvent:
		return NotifyAudioToneEvent(aTsyReqHandle, aPackage.Des1n());
//
// Cancels
//
	case EMobileCallNotifyMobileCallStatusChangeCancel:
		return NotifyMobileCallStatusChangeCancel(aTsyReqHandle);
	case EMobileCallHoldCancel:
		return HoldCancel(aTsyReqHandle);
	case EMobileCallGetMobileCallInfoCancel:
		return GetMobileCallInfoCancel(aTsyReqHandle);
	case EMobileCallResumeCancel:
		return ResumeCancel(aTsyReqHandle);
	case EMobileCallSwapCancel:
		return SwapCancel(aTsyReqHandle);
	case EMobileCallNotifyMobileCallCapsChangeCancel:
		return NotifyMobileCallCapsChangeCancel(aTsyReqHandle);
	case EMobileCallNotifyCallEventCancel:
		return NotifyCallEventCancel(aTsyReqHandle);
	case EMobileCallDialEmergencyCallCancel:
		return DialEmergencyCallCancel(aTsyReqHandle);
	case EMobileCallNotifyRemotePartyInfoChangeCancel:
		return NotifyRemotePartyInfoChangeCancel(aTsyReqHandle);
	case EMobileCallNotifyVoiceFallbackCancel:
		return NotifyVoiceFallbackCancel(aTsyReqHandle);
	case EMobileCallNotifyUUSCapsChangeCancel:
		return NotifyUUSCapsChangeCancel(aTsyReqHandle);
	case EMobileCallDialNoFdnCheckCancel:
		return DialNoFdnCheckCancel(aTsyReqHandle);
	case EMobileCallDialCallControlCancel:
		return DialCallControlCancel(aTsyReqHandle);
	case EMobileCallNotifyAudioToneEventCancel:
		return NotifyAudioToneEventCancel(aTsyReqHandle);
	case EMobileCallDialISVCancel:
		return DialISVCancel(aTsyReqHandle);
	case EMobileCallAnswerISVCancel:
		return AnswerIncomingCallISVCancel(aTsyReqHandle);
	default:
		return KErrNotSupported;
		}
	}

TInt CCallDMmTsy::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.

	switch (aIpc)
		{
	case EMobileCallNotifyMobileCallStatusChange:
		return NotifyMobileCallStatusChangeCancel(aTsyReqHandle);
	case EMobileCallHold:
		return HoldCancel(aTsyReqHandle);
	case EMobileCallGetMobileCallInfo:
		return GetMobileCallInfoCancel(aTsyReqHandle);
	case EMobileCallResume:
		return ResumeCancel(aTsyReqHandle);
	case EMobileCallSwap:
		return SwapCancel(aTsyReqHandle);
	case EMobileCallNotifyMobileCallCapsChange:
		return NotifyMobileCallCapsChangeCancel(aTsyReqHandle);
	case EMobileCallNotifyCallEvent:
		return NotifyCallEventCancel(aTsyReqHandle);
	case EMobileCallDialEmergencyCall:
		return DialEmergencyCallCancel(aTsyReqHandle);
	case EMobileCallNotifyRemotePartyInfoChange:
		return NotifyRemotePartyInfoChangeCancel(aTsyReqHandle);
	case EMobileCallNotifyMobileDataCallCapsChange:
		return NotifyMobileDataCallCapsChangeCancel(aTsyReqHandle);
	case EMobileCallGetMobileDataCallRLPRange:
		return GetMobileDataCallRLPRangeCancel(aTsyReqHandle);
	case EMobileCallSetDynamicHscsdParams:
		return SetDynamicHscsdParamsCancel(aTsyReqHandle);
	case EMobileCallNotifyHscsdInfoChange:
		return NotifyHscsdInfoChangeCancel(aTsyReqHandle);
	case EMobileCallNotifyVoiceFallback:
		return NotifyVoiceFallbackCancel(aTsyReqHandle);
	case EMobileCallNotifyAlternatingCallSwitch:
		return NotifyAlternatingCallSwitchCancel(aTsyReqHandle);
	case EMobileCallDeflect:
		return DeflectCancel(aTsyReqHandle);
	case EMobileCallTransfer:
		return TransferCancel(aTsyReqHandle);
	case EMobileCallGoOneToOne:
		return GoOneToOneCancel(aTsyReqHandle);
	case EMobileCallSwitchAlternatingCall:
		return SwitchAlternatingCallCancel(aTsyReqHandle);
	case EMobileCallActivateCCBS:
		return ActivateCCBSCancel(aTsyReqHandle);
	case EMobileCallNotifyPrivacyConfirmation:
		return NotifyPrivacyConfirmationCancel(aTsyReqHandle);
	case EMobileCallNotifyTrafficChannelConfirmation:
		return NotifyTrafficChannelConfirmationCancel(aTsyReqHandle);
	case EMobileCallNotifyUUSCapsChange:
		return NotifyUUSCapsChangeCancel(aTsyReqHandle);
	case EMobileCallActivateUUS:
		return ActivateUUSCancel(aTsyReqHandle);
	case EMobileCallSendUUI:
		return SendUUICancel(aTsyReqHandle);
	case EMobileCallReceiveUUI:
		return ReceiveUUICancel(aTsyReqHandle);
	case EMobileCallHangupWithUUI:
		return HangupWithUUICancel(aTsyReqHandle);
	case EMobileCallAnswerWithUUI:
		return AnswerIncomingCallWithUUICancel(aTsyReqHandle);
	case EMobileCallDialNoFdnCheck:
		return DialNoFdnCheckCancel(aTsyReqHandle);
	case EMobileCallDialCallControl:
		return DialCallControlCancel(aTsyReqHandle);
	case EMobileCallAnswerMultimediaCallAsVoice:
		return AnswerMultimediaCallAsVoiceCancel(aTsyReqHandle);
	case EMobileCallNotifyAudioToneEvent:
		return NotifyAudioToneEventCancel(aTsyReqHandle);
	case EMobileCallDialISV:
		return DialISVCancel(aTsyReqHandle);
    case EMobileCallAnswerISV:
		return AnswerIncomingCallISVCancel(aTsyReqHandle);
	default:
		return CCallBase::CancelService(aIpc,aTsyReqHandle);
		}
	}

/***********************************************************************************/
//
// The following methods are called from ExtFunc and/or CancelService.
// Each of these will process a TSY request or cancel a TSY request
// Here, example values are returned or checked within this dummy TSY in order to ensure
// that the integrity of the data passed to/from client is maintained
//
/***********************************************************************************/


TInt CCallDMmTsy::GetMobileDataCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	LOGTEXT(_L8("CCallDMmTsy::GetMobileDataCallCaps called"));
	InitialiseDataCallCaps(aCaps);
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyMobileDataCallCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	if (!iNotifyMobileDataCallCapsChange++)
		{
		LOGTEXT(_L8("CCallDMmTsy::NotifyMobileDataCallCapsChange called"));
		InitialiseDataCallCaps(aCaps);
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyMobileDataCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyMobileDataCallCapsChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

void CCallDMmTsy::InitialiseDataCallCaps(TDes8* aCallCaps)
	{
	RMobileCall::TMobileCallDataCapsV1Pckg* callCapsV1Pckg = REINTERPRET_CAST(RMobileCall::TMobileCallDataCapsV1Pckg*,aCallCaps);
	RMobileCall::TMobileCallDataCapsV1& callCapsV1 = (*callCapsV1Pckg)();
	
	callCapsV1.iSpeedCaps = DMMTSY_CALL_DATA_CAPS_SPEED;
	callCapsV1.iProtocolCaps = DMMTSY_CALL_DATA_CAPS_CARRIER;
	callCapsV1.iServiceCaps = DMMTSY_CALL_DATA_CAPS_SERVICE;
	callCapsV1.iHscsdSupport = DMMTSY_CALL_DATA_CAPS_HSCSD;
	callCapsV1.iMClass = DMMTSY_CALL_DATA_CAPS_MCLASS;
	callCapsV1.iMaxRxTimeSlots = DMMTSY_CALL_DATA_CAPS_MAXRXTS;
	callCapsV1.iMaxTxTimeSlots = DMMTSY_CALL_DATA_CAPS_MAXTXTS;
	callCapsV1.iTotalRxTxTimeSlots = DMMTSY_CALL_DATA_CAPS_TOTRXTXTS;
	callCapsV1.iCodingCaps = DMMTSY_CALL_DATA_CAPS_CODING;
	callCapsV1.iAsymmetryCaps = DMMTSY_CALL_DATA_CAPS_ASYMETRY;
	callCapsV1.iUserInitUpgrade = DMMTSY_CALL_DATA_CAPS_USERIMI;
	}

TInt CCallDMmTsy::GetMobileDataCallRLPRange(const TTsyReqHandle aTsyReqHandle, TInt* aRLPVersion, TDes8* aRLPRange)
	{
	LOGTEXT(_L8("CCallDMmTsy::GetMobileDataCallRLPRange called"));

	RMobileCall::TMobileDataRLPRangesV1Pckg* rlpRangeV1Pckg = REINTERPRET_CAST(RMobileCall::TMobileDataRLPRangesV1Pckg*, aRLPRange);
	RMobileCall::TMobileDataRLPRangesV1& rlpRangeV1 = (*rlpRangeV1Pckg)();

	if(*aRLPVersion == DMMTSY_CALL_RLP_VERSION1)
		{
		rlpRangeV1.iIWSMax = DMMTSY_CALL_DATA_RLP_WSMAX;
		rlpRangeV1.iIWSMin = DMMTSY_CALL_DATA_RLP_WSMIN;
		rlpRangeV1.iMWSMax = DMMTSY_CALL_DATA_RLP_MWSMAX;
		rlpRangeV1.iMWSMin = DMMTSY_CALL_DATA_RLP_MWSMIN;
		rlpRangeV1.iN2Max = DMMTSY_CALL_DATA_RLP_N2MAX;
		rlpRangeV1.iN2Min = DMMTSY_CALL_DATA_RLP_N2MIN;
		rlpRangeV1.iT1Max = DMMTSY_CALL_DATA_RLP_T1MAX;
		rlpRangeV1.iT1Min = DMMTSY_CALL_DATA_RLP_T1MIN;
		rlpRangeV1.iT4Max = DMMTSY_CALL_DATA_RLP_T4MAX;
		rlpRangeV1.iT4Min = DMMTSY_CALL_DATA_RLP_T4MIN;

		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}
	
TInt CCallDMmTsy::GetMobileDataCallRLPRangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::GetMobileDataCallRLPRangeCancel called"));

	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::SetDynamicHscsdParams(const TTsyReqHandle aTsyReqHandle, TInt* aAiur, TInt* aRxTimeslots)
	{
	LOGTEXT(_L8("CCallDMmTsy::SetHscsdParams called"));

	if ((*aAiur != DMMTSY_CALL_HCSD_AIUR) ||
		(*aRxTimeslots != DMMTSY_CALL_HCSD_TS))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		
	return KErrNone;
	}

TInt CCallDMmTsy::SetDynamicHscsdParamsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::SetHscsdParamsCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::GetCurrentHscsdInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aHSCSDInfo)
	{
	RMobileCall::TMobileCallHscsdInfoV1Pckg* HSCSDInfoPckg = REINTERPRET_CAST(RMobileCall::TMobileCallHscsdInfoV1Pckg*,aHSCSDInfo);
	RMobileCall::TMobileCallHscsdInfoV1& hscsdInfo = (*HSCSDInfoPckg)();
	LOGTEXT(_L8("CCallDMmTsy::GetCurrentHscsdInfo called"));
	InitialiseHscsdInfo(hscsdInfo);
	
	if ((hscsdInfo.ExtensionId() == KEtelExtMultimodeV7) ||
		(hscsdInfo.ExtensionId() == KEtelExtMultimodeV8))
		{
		RMobileCall::TMobileCallHscsdInfoV7Pckg* HSCSDInfoV7Pckg = REINTERPRET_CAST(RMobileCall::TMobileCallHscsdInfoV7Pckg*,aHSCSDInfo);
		RMobileCall::TMobileCallHscsdInfoV7& hscsdInfoV7 = (*HSCSDInfoV7Pckg)();
		
		hscsdInfoV7.iCallParamOrigin           = DMMTSY_CALL_PARAM_ORIGIN;
		hscsdInfoV7.iIconId.iQualifier         = DMMTSY_CALL_ICON_ID_QUALIFIER;
		hscsdInfoV7.iIconId.iIdentifier        = DMMTSY_CALL_ICON_ID_IDENTIFIER;
		hscsdInfoV7.iAlphaId                   = DMMTSY_CALL_ALPHA_ID;
		hscsdInfoV7.iParamsCallControlModified = DMMTSY_CALL_PARAMS_CALL_CONTROL_MODIFIED;
		}
	if (hscsdInfo.ExtensionId() == KEtelExtMultimodeV8)
		{
		RMobileCall::TMobileCallHscsdInfoV8Pckg* HSCSDInfoV8Pckg = REINTERPRET_CAST(RMobileCall::TMobileCallHscsdInfoV8Pckg*,aHSCSDInfo);
		RMobileCall::TMobileCallHscsdInfoV8& hscsdInfoV8 = (*HSCSDInfoV8Pckg)();
		
		hscsdInfoV8.iSubAddress        = DMMTSY_CALL_SUBADDRESS;
		hscsdInfoV8.iBearerCap1        = DMMTSY_CALL_BEARER_CAP1;
		hscsdInfoV8.iBearerCap2        = DMMTSY_CALL_BEARER_CAP2;
		hscsdInfoV8.iBCRepeatIndicator = DMMTSY_CALL_BC_REPEAT_INDICATOR;
		}


	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyHscsdInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aHSCSDInfo)
	{
	if (!iNotifyHscsdInfoChange++)
		{
		RMobileCall::TMobileCallHscsdInfoV1Pckg* HSCSDInfoPckg = REINTERPRET_CAST(RMobileCall::TMobileCallHscsdInfoV1Pckg*,aHSCSDInfo);
		RMobileCall::TMobileCallHscsdInfoV1& HSCSDInfo = (*HSCSDInfoPckg)();

		LOGTEXT(_L8("CCallDMmTsy::NotifyHscsdInfoChange called"));
		InitialiseHscsdInfo(HSCSDInfo);
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyHscsdInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyHscsdInfoChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

void CCallDMmTsy::InitialiseHscsdInfo(RMobileCall::TMobileCallHscsdInfoV1& aHSCSDInfo)
	{
	aHSCSDInfo.iAiur = DMMTSY_CALL_HCSD_AIUR;
	aHSCSDInfo.iRxTimeSlots = DMMTSY_CALL_HCSD_TS;
	aHSCSDInfo.iTxTimeSlots = DMMTSY_CALL_HCSD_TS;
	aHSCSDInfo.iCodings = DMMTSY_CALL_HCSD_CODING;
	}

TInt CCallDMmTsy::NotifyVoiceFallback(const TTsyReqHandle aTsyReqHandle, TDes* aCallName)
	{
	if (!iNotifyVoiceFallback++)
		{
		LOGTEXT(_L8("CCallDMmTsy::NotifyVoiceFallback Called"));
		*aCallName = DMMTSY_VOICE_FALLBACK_CALL_NAME;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyVoiceFallbackCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyVoiceFallbackCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::SwitchAlternatingCall(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::SwitchAlternatingCall called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;	
	}

TInt CCallDMmTsy::SwitchAlternatingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::SwitchAlternatingCallCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyAlternatingCallSwitch(const TTsyReqHandle aTsyReqHandle)
	{
	if (!iNotifyAlternatingCallSwitch++)
		{
		LOGTEXT(_L8("CCallDMmTsy::NotifyAlternatingCallSwitch called"));
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyAlternatingCallSwitchCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyAlternatingCallSwitchCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}



TInt CCallDMmTsy::GetMobileCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	LOGTEXT(_L8("CCallDMmTsy::GetMobileCallCaps called"));
	RMobileCall::TMobileCallCapsV1Pckg *capsPckg = REINTERPRET_CAST(RMobileCall::TMobileCallCapsV1Pckg *,aCaps);
	RMobileCall::TMobileCallCapsV1& caps = (*capsPckg)();
	caps.iCallControlCaps=DMMTSY_CALL_CONTROL_CAPS1;
	caps.iCallEventCaps=DMMTSY_CALL_EVENT_CAPS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyMobileCallCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	if(!iNotifyMobileCallCapsChange++)
		{
		LOGTEXT(_L8("CCallDMmTsy::NotifyMobileCallCapsChange called"));
		RMobileCall::TMobileCallCapsV1Pckg *capsPckg = REINTERPRET_CAST(RMobileCall::TMobileCallCapsV1Pckg *,aCaps);
		RMobileCall::TMobileCallCapsV1& caps = (*capsPckg)();
		caps.iCallControlCaps=DMMTSY_CALL_CONTROL_CAPS2;
		caps.iCallEventCaps=DMMTSY_CALL_EVENT_CAPS2;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyMobileCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyMobileCallCapsChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CCallDMmTsy::GetMobileCallStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus)
	{
	LOGTEXT(_L8("CCallDMmTsy::GetMobileCallStatus called"));
	*aStatus=DMMTSY_CALL_STATUS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyMobileCallStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus)
	{
	if (iNotifyMobileCallStatusChange<DMMTSY_NUMBER_OF_SLOTS_STATUS)
		{
		LOGTEXT(_L8("CCallDMmTsy::NotifyMobileCallStatusChange called"));
		switch(iNotifyMobileCallStatusChange)
			{
		case 0:
			*aStatus=DMMTSY_CALL_STATUS2;
			break;
		case 1:
			*aStatus=DMMTSY_CALL_STATUS3;
			break;
		case 2:
			*aStatus=DMMTSY_CALL_STATUS4;
			break;
		default:
			break;
			}
		iNotifyMobileCallStatusChange++;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyMobileCallStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyMobileCallStatusChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::Hold(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::Hold called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::HoldCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::HoldCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::Resume(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::Resume called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::ResumeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::ResumeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::Swap(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::Swap called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::SwapCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::SwapCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::Deflect(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallDeflect* aDeflectType, RMobilePhone::TMobileAddress* aDestination)
	{
	LOGTEXT(_L8("CCallDMmTsy::Deflect called"));

	if ((*aDeflectType != DMMTSY_CALL_DEFLECT_TYPE) ||
		(aDestination->iNumberPlan != DMMTSY_PHONE_NPI) || 
	    (aDestination->iTypeOfNumber != DMMTSY_PHONE_TON) ||
		(aDestination->iTelNumber.Compare(DMMTSY_PHONE_TEL_NUMBER) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::DeflectCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::DeflectCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::Transfer(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::Transfer called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::TransferCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::TransferCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::GoOneToOne(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::GoOneToOne called"));
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::GoOneToOneCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::GoOneToOneCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyCallEvent(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallEvent* aEvent)
	{
	if(!iNotifyCallEvent++)
		{
		LOGTEXT(_L8("CCallDMmTsy::NotifyCallEvent called"));
		*aEvent=DMMTSY_CALL_EVENT1;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyCallEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyCallEventCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::SetPrivacySetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aPrivacySetting)
	{
	LOGTEXT(_L8("CCallDMmTsy::Set Privacy called"));
	if (*aPrivacySetting != DMMTSY_CALL_SET_PRIVACY)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyPrivacyConfirmation(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aPrivacySetting)
	{
	if(!iNotifyPrivacyConfirmation++)
		{
		LOGTEXT(_L8("CCallDMmTsy::Notify Privacy Confirmation Change"));
		*aPrivacySetting = DMMTSY_CALL_SET_PRIVACY;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyPrivacyConfirmationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::Notify Privacy Confirmation Cancel"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::SetTrafficChannel(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallTch* aTchRequest)
	{
	LOGTEXT(_L8("CCallDMmTsy::Set Traffic Channel called"));
	if (*aTchRequest != DMMTSY_CALL_TCH_REQUEST)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyTrafficChannelConfirmation(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallTch* aTchType)
	{
	if (!iNotifyTrafficChannelConfirmation++)
		{
		LOGTEXT(_L8("CCallDMmTsy::Notify Traffic Channel Confirmation"));
		*aTchType = DMMTSY_CALL_TCH_SETTING;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyTrafficChannelConfirmationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::Notify Traffic Channel Confirmation Cancel"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::GetMobileCallInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo)
	{
	LOGTEXT(_L8("CCallDMmTsy::GetMobileCallInfo called"));
	RMobileCall::TMobileCallInfoV1Pckg* infoV1Pckg = (RMobileCall::TMobileCallInfoV1Pckg*)aInfo;
	RMobileCall::TMobileCallInfoV1& infoV1 = (*infoV1Pckg)();

	infoV1.iCallName=DMMTSY_CALL_NAME1;
	infoV1.iLineName=DMMTSY_LINE_VOICE_NAME;
	infoV1.iStatus=DMMTSY_CALL_STATUS4;
	infoV1.iDuration=DMMTSY_CALL_DURATION1;
	infoV1.iCallId=DMMTSY_CALL_ID1;
	infoV1.iRemoteParty.iRemoteIdStatus=DMMTSY_CALL_REMOTEPARTYSTATUS1;
	infoV1.iRemoteParty.iRemoteNumber.iTelNumber=DMMTSY_CALL_REMOTEPARTYNUMBER1;
	infoV1.iRemoteParty.iDirection=DMMTSY_CALL_REMOTEPARTYDIRECTION1;
	infoV1.iRemoteParty.iCallingName=DMMTSY_CALL_REMOTEPARTYNAME;
	infoV1.iExitCode=DMMTSY_CALL_EXIT_CODE;
	infoV1.iEmergency=DMMTSY_CALL_EMERGENCY_FLAG;
	infoV1.iForwarded=DMMTSY_CALL_FORWARDED_FLAG;
	infoV1.iService = DMMTSY_CALL_SERVICE;
	infoV1.iValid = DMMTSY_CALL_VALID_FLAG;
	infoV1.iPrivacy = DMMTSY_CALL_PRIVACY;
	infoV1.iTch = DMMTSY_CALL_CHANNEL;
	infoV1.iAlternatingCall = DMMTSY_CALL_ALTERNATING;
	infoV1.iStartTime.Set(DMMTSY_PHONE_NWTINFO_YEAR,DMMTSY_PHONE_NWTINFO_MONTH,DMMTSY_PHONE_NWTINFO_DAY,
		DMMTSY_PHONE_NWTINFO_HOUR, DMMTSY_PHONE_NWTINFO_MINUTE, DMMTSY_PHONE_NWTINFO_SECOND,
		DMMTSY_PHONE_NWTINFO_MICRO);
		
	if (infoV1.ExtensionId() == KETelExtMultimodeV3 ||
	   (infoV1.ExtensionId() == KETelExtMultimodeV4))
		{
		RMobileCall::TMobileCallInfoV3Pckg* infoV3Pckg = (RMobileCall::TMobileCallInfoV3Pckg*)aInfo;
		RMobileCall::TMobileCallInfoV3& infoV3 = (*infoV3Pckg)();
		infoV3.iSecurity = DMMTSY_CALL_SECURITY;
		}

	if (infoV1.ExtensionId() == KEtelExtMultimodeV7)
		{
		RMobileCall::TMobileCallInfoV7Pckg* infoV7Pckg = (RMobileCall::TMobileCallInfoV7Pckg*)aInfo;
		RMobileCall::TMobileCallInfoV7& infoV7 = (*infoV7Pckg)();
		infoV7.iSecurity = DMMTSY_CALL_SECURITY;
		infoV7.iCallParamOrigin = DMMTSY_CALL_ORIGIN;
		infoV7.iIconId.iQualifier  = DMMTSY_CALL_ICON_ID_QUALIFIER;
		infoV7.iIconId.iIdentifier  = DMMTSY_CALL_ICON_ID_IDENTIFIER;	
		infoV7.iAlphaId = DMMTSY_CALL_ALPHA_ID;
		infoV7.iParamsCallControlModified = DMMTSY_CALL_PARAMS_CALL_CONTROL_MODIFIED;
		}
	if (infoV1.ExtensionId() == KEtelExtMultimodeV8)
		{
		RMobileCall::TMobileCallInfoV8Pckg* infoV8Pckg = (RMobileCall::TMobileCallInfoV8Pckg*)aInfo;
		RMobileCall::TMobileCallInfoV8& infoV8 = (*infoV8Pckg)();
		infoV8.iSecurity = DMMTSY_CALL_SECURITY;
		infoV8.iCallParamOrigin = DMMTSY_CALL_ORIGIN;
		infoV8.iIconId.iQualifier  = DMMTSY_CALL_ICON_ID_QUALIFIER;
		infoV8.iIconId.iIdentifier  = DMMTSY_CALL_ICON_ID_IDENTIFIER;	
		infoV8.iAlphaId = DMMTSY_CALL_ALPHA_ID;
		infoV8.iParamsCallControlModified = DMMTSY_CALL_PARAMS_CALL_CONTROL_MODIFIED;
		infoV8.iSubAddress = DMMTSY_CALL_SUBADDRESS;
		infoV8.iBearerCap1 = DMMTSY_CALL_BEARER_CAP1;
		infoV8.iBearerCap2 = DMMTSY_CALL_BEARER_CAP2;
		infoV8.iBCRepeatIndicator = DMMTSY_CALL_BC_REPEAT_INDICATOR;
		}

	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::GetMobileCallInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::GetMobileCallInfoCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyRemotePartyInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aRemotePartyInfo)
	{
	if (!iNotifyRemotePartyInfoChange++)
		{
		LOGTEXT(_L8("CCallDMmTsy::NotifyRemotePartyInfoChange called"));
		RMobileCall::TMobileCallRemotePartyInfoV1Pckg* infoV1Pckg = (RMobileCall::TMobileCallRemotePartyInfoV1Pckg*)aRemotePartyInfo;
		RMobileCall::TMobileCallRemotePartyInfoV1& infoV1 = (*infoV1Pckg)();

		infoV1.iRemoteIdStatus=DMMTSY_CALL_REMOTEPARTYSTATUS2;
		infoV1.iRemoteNumber.iTelNumber=DMMTSY_CALL_EMPTY_STRING;
		infoV1.iDirection=DMMTSY_CALL_REMOTEPARTYDIRECTION2;
		infoV1.iCallingName=DMMTSY_CALL_EMPTY_STRING;

		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyRemotePartyInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyRemotePartyInfoChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::DialNoFdnCheck(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDesC* aNumber)
	/**
	* This method dials the number specified by aNumber. The number used for dialling  
	* is not checked against those in the Fixed Dialling Number list even if the FDN service is enabled
	*
	* @param aCallParams Supplies the call parameters for the dial request
	* @param aNumber Supplies the number to dial
	*/

	{
	LOGTEXT(_L8("CCallDMmTsy::DialNoFdnCheck called"));

	TInt error = ValidateCallParams(aCallParams);

	if((error != KErrNone) || (aNumber->Compare(DMMTSY_PHONE_TEL_NUMBER) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else		
		{
		LOGTEXT(_L8("CCallDMmTsy::DialNoFdnCheck CallParams and DialNumber validated"));
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	}

	return KErrNone;
	}

TInt CCallDMmTsy::DialNoFdnCheckCancel(const TTsyReqHandle aTsyReqHandle)
	/**
	* This method cancels the dialling of the number specified by aNumber. The number used for dialling  
	* is not checked against those in the Fixed Dialling Number list even if the FDN service is enabled
	*/
	{
	LOGTEXT(_L8("CCallDMmTsy::DialNoFdnCheckCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::DialCallControl(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDesC* aNumber)
/**
 * This method dials the number specified by aNumber. Dialling via
 * this function allows the TSY to know that the dial request is
 * the result of call control.
 *
 * @param aCallParams Supplies the call parameters for the dial request
 * @param aNumber Supplies the number to dial
 */

	{
	LOGTEXT(_L8("CCallDMmTsy::DialCallControl called"));

	TInt error = ValidateCallParams(aCallParams); // Check V1 params

	// TODO need to validate the other params
	
	if((error != KErrNone) || (aNumber->Compare(DMMTSY_PHONE_TEL_NUMBER) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else		
		{
		LOGTEXT(_L8("CCallDMmTsy::DialCallControl CallParams and DialNumber validated"));
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}

	return KErrNone;
	}

TInt CCallDMmTsy::DialCallControlCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * This method cancels the dialling of the number specified by aNumber. 
 */
	{
	LOGTEXT(_L8("CCallDMmTsy::DialCallControlCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::DialEmergencyCall(const TTsyReqHandle aTsyReqHandle, TDesC* aNumber)
	{
	LOGTEXT(_L8("CCallDMmTsy::DialEmergencyCall called"));
	if(aNumber->Compare(DMMTSY_EN_STORE_AMBULANCE_NUM) != 0)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		ReqCompleted(aTsyReqHandle,KErrNone);

	return KErrNone;
	}

TInt CCallDMmTsy::DialEmergencyCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::DialEmergencyCallCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::DialISV(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDesC* aNumber)
	{
	LOGTEXT(_L8("CCallDMmTsy::DialISV called"));

	TInt error = KErrNone;
 
	if(aCallParams)
		{  
		
		RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg* callparamsPckg=
				reinterpret_cast<RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg*>(const_cast<TDesC8*>(aCallParams));
 		RMobileCall::TEtel3rdPartyMobileCallParamsV1& callparams=(*callparamsPckg)();
	
		// check that the request is a 3rd party request
		if(callparams.ExtensionId() == RMobileCall::KETel3rdPartyCallParamsV1)	
	  		{
			LOGTEXT(_L8("CCallDMmTsy::DIALISV,  request from 3rd party."));
			
			//Validate parameters
			if ((aNumber->Compare(DMMTSY_PHONE_TEL_NUMBER)!=KErrNone) ||
				(callparams.iIdRestrict != DMMTSY_CALL_CLI_RESTRICT_SETTING1) ||
				(callparams.iAutoRedial != DMMTSY_CALL_AUTO_REDIAL_FLAG))
				{
			 	error=KErrCorrupt;	
				}
			}			
		else 
			{
			LOGTEXT(_L8("CCallDMmTsy::DIALISV, request from non-3rd party."));
			error = KErrAccessDenied;
			}
		}
		
	 else
		{	// No call parms data
			error = KErrCorrupt;
		} 
		 

	if (error != KErrNone)
		ReqCompleted(aTsyReqHandle,error);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
 
	}

TInt CCallDMmTsy::DialISVCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::DialISVCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
		

TInt CCallDMmTsy::ActivateCCBS(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
	{
	LOGTEXT(_L8("CCallDMmTsy::ActivateCCBS called"));
	*aIndex=DMMTSY_PHONE_CCC_INDEX1;
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::ActivateCCBSCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::ActivateCCBSCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::RejectCCBS(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::RejectCCBS called"));
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDMmTsy::GetUUSCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CCallDMmTsy::GetUUSCaps called"));
	*aCaps = DMMTSY_CALL_UUS_CAPS;
	
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyUUSCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	if (!iNotifyUUSCapsChange++)
		{
		LOGTEXT(_L8("CCallDMmTsy::NotifyUUSCapsChange called"));
		*aCaps = DMMTSY_CALL_UUS_CAPS;

		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyUUSCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyUUSCapsChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::ValidateCallParams(TDesC8* aCallParams)
	{
	TInt error = KErrNone;

	RCall::TCallParamsPckg* paramsPckgV1 = (RCall::TCallParamsPckg*)aCallParams;
	RCall::TCallParams& paramsV1 = (*paramsPckgV1)();

	iDataCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
	iDataCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
	iDataCallParams.iInterval = paramsV1.iInterval;
	iDataCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;
	iMobileCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
	iMobileCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
	iMobileCallParams.iInterval = paramsV1.iInterval;
	iMobileCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;

	if(paramsV1.ExtensionId()==RMobileCall::KETelMobileCallParamsV1)
		{
		if (iMobileCallParams.iInterval != DMMTSY_CALL_INTERVAL)
			error=KErrCorrupt;

		RMobileCall::TMobileCallParamsV1Pckg* mmParamsPckgV1 = (RMobileCall::TMobileCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileCallParamsV1& mmParamsV1 = (*mmParamsPckgV1)();

		iMobileCallParams.iIdRestrict = mmParamsV1.iIdRestrict;
		iMobileCallParams.iCug = mmParamsV1.iCug;
		iMobileCallParams.iAutoRedial = mmParamsV1.iAutoRedial;

		if ((iMobileCallParams.iIdRestrict != DMMTSY_CALL_CLI_RESTRICT_SETTING1) ||
			(iMobileCallParams.iCug.iExplicitInvoke != DMMTSY_CALL_CUG_INVOKE) ||
			(iMobileCallParams.iCug.iCugIndex != DMMTSY_CALL_CUG_INDEX) ||
			(iMobileCallParams.iCug.iSuppressOA != DMMTSY_CALL_CUG_OA) ||
			(iMobileCallParams.iCug.iSuppressPrefCug != DMMTSY_CALL_CUG_PREF) ||
			(iMobileCallParams.iAutoRedial != DMMTSY_CALL_AUTO_REDIAL_FLAG))
			
			error=KErrCorrupt;
		}
	else if(paramsV1.ExtensionId()==RMobileCall::KETelMobileCallParamsV2)
		{
		if (iMobileCallParams.iInterval != DMMTSY_CALL_INTERVAL)
			error=KErrCorrupt;

		RMobileCall::TMobileCallParamsV2Pckg* mmParamsPckgV2 = (RMobileCall::TMobileCallParamsV2Pckg*)aCallParams;
		RMobileCall::TMobileCallParamsV2& mmParamsV2 = (*mmParamsPckgV2)();

		iMobileCallParams.iIdRestrict = mmParamsV2.iIdRestrict;
		iMobileCallParams.iCug = mmParamsV2.iCug;
		iMobileCallParams.iAutoRedial = mmParamsV2.iAutoRedial;
		iMobileCallParams.iBearerMode = mmParamsV2.iBearerMode;

		if ((iMobileCallParams.iIdRestrict != DMMTSY_CALL_CLI_RESTRICT_SETTING1) ||
			(iMobileCallParams.iCug.iExplicitInvoke != DMMTSY_CALL_CUG_INVOKE) ||
			(iMobileCallParams.iCug.iCugIndex != DMMTSY_CALL_CUG_INDEX) ||
			(iMobileCallParams.iCug.iSuppressOA != DMMTSY_CALL_CUG_OA) ||
			(iMobileCallParams.iCug.iSuppressPrefCug != DMMTSY_CALL_CUG_PREF) ||
			(iMobileCallParams.iAutoRedial != DMMTSY_CALL_AUTO_REDIAL_FLAG) ||
			(iMobileCallParams.iBearerMode != DMMTSY_CALL_NEW_BEARER))
			{
			error=KErrCorrupt;
			}
		}
	else if (paramsV1.ExtensionId()==RMobileCall::KETelMobileCallParamsV7)
		{
		if (iMobileCallParams.iInterval != DMMTSY_CALL_INTERVAL)
			error=KErrCorrupt;

		RMobileCall::TMobileCallParamsV7Pckg* mmParamsPckgV7 = (RMobileCall::TMobileCallParamsV7Pckg*)aCallParams;
		RMobileCall::TMobileCallParamsV7& mmParamsV7 = (*mmParamsPckgV7)();

		iMobileCallParams.iIdRestrict = mmParamsV7.iIdRestrict;
		iMobileCallParams.iCug = mmParamsV7.iCug;
		iMobileCallParams.iAutoRedial = mmParamsV7.iAutoRedial;
		iMobileCallParams.iBearerMode = mmParamsV7.iBearerMode;
		iMobileCallParams.iCallParamOrigin = mmParamsV7.iCallParamOrigin;
		iMobileCallParams.iSubAddress = mmParamsV7.iSubAddress;
		iMobileCallParams.iBearerCap1 = mmParamsV7.iBearerCap1;
		iMobileCallParams.iBearerCap2 = mmParamsV7.iBearerCap2;
		iMobileCallParams.iBCRepeatIndicator = mmParamsV7.iBCRepeatIndicator;
		iMobileCallParams.iIconId.iQualifier = mmParamsV7.iIconId.iQualifier;
		iMobileCallParams.iIconId.iIdentifier = mmParamsV7.iIconId.iIdentifier;
		iMobileCallParams.iAlphaId = mmParamsV7.iAlphaId;

		if ((iMobileCallParams.iIdRestrict != DMMTSY_CALL_CLI_RESTRICT_SETTING1) ||
			(iMobileCallParams.iCug.iExplicitInvoke != DMMTSY_CALL_CUG_INVOKE) ||
			(iMobileCallParams.iCug.iCugIndex != DMMTSY_CALL_CUG_INDEX) ||
			(iMobileCallParams.iCug.iSuppressOA != DMMTSY_CALL_CUG_OA) ||
			(iMobileCallParams.iCug.iSuppressPrefCug != DMMTSY_CALL_CUG_PREF) ||
			(iMobileCallParams.iAutoRedial != DMMTSY_CALL_AUTO_REDIAL_FLAG) ||
			(iMobileCallParams.iBearerMode != DMMTSY_CALL_NEW_BEARER) ||
			(iMobileCallParams.iSubAddress != DMMTSY_CALL_SUBADDRESS) ||
			(iMobileCallParams.iBearerCap1 != DMMTSY_CALL_BEARER_CAP1) ||
			(iMobileCallParams.iBearerCap2 != DMMTSY_CALL_BEARER_CAP2) ||
			(iMobileCallParams.iBCRepeatIndicator != DMMTSY_CALL_BC_REPEAT_INDICATOR) ||
			(iMobileCallParams.iIconId.iQualifier != DMMTSY_CALL_ICON_ID_QUALIFIER) ||
			(iMobileCallParams.iIconId.iIdentifier != DMMTSY_CALL_ICON_ID_IDENTIFIER) ||
			(iMobileCallParams.iAlphaId != DMMTSY_CALL_ALPHA_ID))
			{
			error=KErrCorrupt;
			}
		}
	
	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV1) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV2) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV8))
		{
		RMobileCall::TMobileDataCallParamsV1Pckg* dataParamsPckgV1 = (RMobileCall::TMobileDataCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV1& dataParamsV1 = (*dataParamsPckgV1)();

		iDataCallParams.iService = dataParamsV1.iService;
		iDataCallParams.iSpeed = dataParamsV1.iSpeed;
		iDataCallParams.iProtocol = dataParamsV1.iProtocol;
		iDataCallParams.iQoS = dataParamsV1.iQoS;
		iDataCallParams.iRLPVersion = dataParamsV1.iRLPVersion;
		iDataCallParams.iV42bisReq = dataParamsV1.iV42bisReq;
 		iDataCallParams.iUseEdge = dataParamsV1.iUseEdge;

		if ((iDataCallParams.iService != DMMTSY_DATA_CALL_SERVICE) ||
		    (iDataCallParams.iSpeed != DMMTSY_DATA_CALL_SPEED) ||
		    (iDataCallParams.iProtocol != DMMTSY_DATA_CALL_PROTOCOL) ||
			(iDataCallParams.iQoS != DMMTSY_DATA_CALL_QOS) || 
			(iDataCallParams.iRLPVersion != DMMTSY_DATA_CALL_RLP) ||
			(iDataCallParams.iV42bisReq != DMMTSY_DATA_CALL_V42BIS) ||
 			(iDataCallParams.iUseEdge != DMMTSY_DATA_CALL_EGPRS_REQUIRED))
			
			error=KErrCorrupt;
		}
	
	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV2) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV8))
		{
		RMobileCall::TMobileDataCallParamsV2Pckg* dataParamsPckgV2 = (RMobileCall::TMobileDataCallParamsV2Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV2& dataParamsV2 = (*dataParamsPckgV2)();

		iDataCallParams.iBearerMode = dataParamsV2.iBearerMode;
	
		if (iDataCallParams.iBearerMode != DMMTSY_PHONE_MULTICALLBEARERMODE_NOTSUPPORTED)
		
			error=KErrCorrupt;
		}
	if (paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV8)
		{
		RMobileCall::TMobileDataCallParamsV8Pckg* dataParamsPckgV8 = (RMobileCall::TMobileDataCallParamsV8Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV8& dataParamsV8 = (*dataParamsPckgV8)();

		iDataCallParams.iCallParamOrigin = dataParamsV8.iCallParamOrigin;
		iDataCallParams.iSubAddress = dataParamsV8.iSubAddress;
		iDataCallParams.iBearerCap1 = dataParamsV8.iBearerCap1;
		iDataCallParams.iBearerCap2 = dataParamsV8.iBearerCap2;
		iDataCallParams.iBCRepeatIndicator = dataParamsV8.iBCRepeatIndicator;
		iDataCallParams.iIconId.iIdentifier = dataParamsV8.iIconId.iIdentifier;
		iDataCallParams.iIconId.iQualifier = dataParamsV8.iIconId.iQualifier;
		iDataCallParams.iAlphaId = dataParamsV8.iAlphaId;
		
		if ((iDataCallParams.iCallParamOrigin != DMMTSY_CALL_PARAM_ORIGIN) ||
		    (iDataCallParams.iSubAddress != DMMTSY_CALL_SUBADDRESS) ||
		    (iDataCallParams.iBearerCap1 != DMMTSY_CALL_BEARER_CAP1) ||
			(iDataCallParams.iBearerCap2 != DMMTSY_CALL_BEARER_CAP2) || 
			(iDataCallParams.iBCRepeatIndicator != DMMTSY_CALL_BC_REPEAT_INDICATOR)||
			(iDataCallParams.iIconId.iIdentifier != DMMTSY_CALL_ICON_ID_IDENTIFIER) ||
		    (iDataCallParams.iIconId.iQualifier != DMMTSY_CALL_ICON_ID_QUALIFIER) ||
		    (iDataCallParams.iAlphaId != DMMTSY_CALL_ALPHA_ID))
			
			error=KErrCorrupt;
		}

	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1) || 
	    (paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV2) ||
	    (paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV7) ||
	    (paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileHscsdCallParamsV1Pckg* hscsdParamsPckgV1 = (RMobileCall::TMobileHscsdCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV1& hscsdParamsV1 = (*hscsdParamsPckgV1)();

		iDataCallParams.iWantedAiur = hscsdParamsV1.iWantedAiur;
		iDataCallParams.iWantedRxTimeSlots = hscsdParamsV1.iWantedRxTimeSlots;
		iDataCallParams.iMaxTimeSlots = hscsdParamsV1.iMaxTimeSlots;
		iDataCallParams.iCodings = hscsdParamsV1.iCodings;
		iDataCallParams.iAsymmetry = hscsdParamsV1.iAsymmetry;
		iDataCallParams.iUserInitUpgrade = hscsdParamsV1.iUserInitUpgrade;

		if ((iDataCallParams.iWantedAiur != DMMTSY_CALL_HCSD_AIUR) ||
		    (iDataCallParams.iWantedRxTimeSlots != DMMTSY_CALL_HCSD_TS) ||
		    (iDataCallParams.iMaxTimeSlots != DMMTSY_CALL_HCSD_MAXTS) ||
		    (iDataCallParams.iCodings != DMMTSY_CALL_HCSD_CODING) ||
			(iDataCallParams.iAsymmetry != DMMTSY_CALL_HCSD_ASYMMETRY) ||
			(iDataCallParams.iUserInitUpgrade != DMMTSY_CALL_HCSD_USER_IMI))

			error=KErrCorrupt;
		}
	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV2) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV7) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileHscsdCallParamsV2Pckg* hscsdParamsPckgV2 = (RMobileCall::TMobileHscsdCallParamsV2Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV2& hscsdParamsV2 = (*hscsdParamsPckgV2)();

		iDataCallParams.iBearerMode = hscsdParamsV2.iBearerMode;
		
		if ((iDataCallParams.iBearerMode != DMMTSY_CALL_NEW_BEARER))

			error=KErrCorrupt;
		}
	
	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV7) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileHscsdCallParamsV7Pckg* hscsdParamsPckgV7 = (RMobileCall::TMobileHscsdCallParamsV7Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV7& hscsdParamsV7 = (*hscsdParamsPckgV7)();

		iDataCallParams.iCallParamOrigin = hscsdParamsV7.iCallParamOrigin;
		iDataCallParams.iIconId =  hscsdParamsV7.iIconId;
		iDataCallParams.iAlphaId = hscsdParamsV7.iAlphaId;
			
		if ((iDataCallParams.iCallParamOrigin != DMMTSY_CALL_PARAM_ORIGIN) ||
			(iDataCallParams.iAlphaId != DMMTSY_CALL_ALPHA_ID))

			error=KErrCorrupt;
		}

	if (paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8)
		{
		RMobileCall::TMobileHscsdCallParamsV8Pckg* hscsdParamsPckgV8 = (RMobileCall::TMobileHscsdCallParamsV8Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV8& hscsdParamsV8 = (*hscsdParamsPckgV8)();
		
		iDataCallParams.iBearerMode = hscsdParamsV8.iBearerMode;
		iDataCallParams.iCallParamOrigin = hscsdParamsV8.iCallParamOrigin;
		iDataCallParams.iIconId.iIdentifier = hscsdParamsV8.iIconId.iIdentifier;
		iDataCallParams.iIconId.iQualifier = hscsdParamsV8.iIconId.iQualifier;
		iDataCallParams.iAlphaId = hscsdParamsV8.iAlphaId;
		iDataCallParams.iSubAddress = hscsdParamsV8.iSubAddress;
		iDataCallParams.iBearerCap1 = hscsdParamsV8.iBearerCap1;
		iDataCallParams.iBearerCap2 = hscsdParamsV8.iBearerCap2;
		iDataCallParams.iBCRepeatIndicator = hscsdParamsV8.iBCRepeatIndicator;
					
		if ((iDataCallParams.iBearerMode != DMMTSY_CALL_NEW_BEARER) ||
			(iDataCallParams.iCallParamOrigin != DMMTSY_CALL_PARAM_ORIGIN) ||
			(iDataCallParams.iIconId.iIdentifier != DMMTSY_CALL_ICON_ID_IDENTIFIER) ||
			(iDataCallParams.iIconId.iQualifier != DMMTSY_CALL_ICON_ID_QUALIFIER) ||
			(iDataCallParams.iAlphaId != DMMTSY_CALL_ALPHA_ID)||
			(iDataCallParams.iSubAddress != DMMTSY_CALL_SUBADDRESS) ||
			(iDataCallParams.iBearerCap1 != DMMTSY_CALL_BEARER_CAP1) ||
			(iDataCallParams.iBearerCap2 != DMMTSY_CALL_BEARER_CAP2) || 
			(iDataCallParams.iBCRepeatIndicator != DMMTSY_CALL_BC_REPEAT_INDICATOR))
			{
			error = KErrCorrupt;
			}
		}

	return error;
	}

TInt CCallDMmTsy::ActivateUUS(const TTsyReqHandle aTsyReqHandle, TDes8* aUUIRequest)
	{
	LOGTEXT(_L8("CCallDMmTsy::ActivateUUS called"));

	RMobileCall::TMobileCallUUSRequestV1Pckg* uuiReqPckg = (RMobileCall::TMobileCallUUSRequestV1Pckg*)aUUIRequest;
	RMobileCall::TMobileCallUUSRequestV1& uuiReq = (*uuiReqPckg)();

	if((uuiReq.iServiceReq != DMMTSY_CALL_UUI_SERVICE_REQ) || 
	   (uuiReq.iUUI.Compare(DMMTSY_CALL_UUI_INFO_DIAL) !=0 ))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CCallDMmTsy::ActivateUUSCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::ActivateUUSDuringCallCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::SendUUI(const TTsyReqHandle aTsyReqHandle, TBool* aMore, TDesC16* aUUI)
	{
	LOGTEXT(_L8("CCallDMmTsy::SendUUI called"));
	if( (aUUI->Compare(DMMTSY_CALL_UUI_INFO) != 0) ||
	    (*aMore != DMMTSY_CALL_UUI_MORE_INFO) )
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CCallDMmTsy::SendUUICancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::SendUUICancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::ReceiveUUI(const TTsyReqHandle aTsyReqHandle, TDes16* aUUI)
	{
	LOGTEXT(_L8("CCallDMmTsy::ReceiveUUI called"));

	aUUI->Copy(DMMTSY_CALL_UUI_INFO);
		
	iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CCallDMmTsy::ReceiveUUICancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::ReceiveUUICancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::HangupWithUUI(const TTsyReqHandle aTsyReqHandle, TDesC16* aUUI)
	{
	LOGTEXT(_L8("CCallDMmTsy::HangupWithUUI called"));

	if(aUUI->Compare(DMMTSY_CALL_UUI_INFO_HANGUP) != 0)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle, this);

	return KErrNone;
	}

TInt CCallDMmTsy::HangupWithUUICancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::HangupWithUUICancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDMmTsy::AnswerIncomingCallWithUUI(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDesC16* aUUI)
	{
	LOGTEXT(_L8("CCallDMmTsy::AnswerIncomingCallWithUUI called"));
	
	TInt error = ValidateCallParams(aCallParams);

	if((error != KErrNone) || (aUUI->Compare(DMMTSY_CALL_UUI_INFO_ANSWER) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CCallDMmTsy::AnswerIncomingCallWithUUICancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::AnswerIncomingCallWithUUICancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CCallDMmTsy::AnswerMultimediaCallAsVoice(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDes16* aCallName)
	{
	LOGTEXT(_L8("CCallDMmTsy::AnswerMultimediaCallAsVoice called"));
	
	TInt error = ValidateCallParams(aCallParams);
	
	*aCallName=DMMTSY_LINE_VOICE_NAME;
	if(error != KErrNone)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CCallDMmTsy::AnswerMultimediaCallAsVoiceCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::AnswerMultimediaCallAsVoiceCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CCallDMmTsy::AnswerIncomingCallISV(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams)
	{
	LOGTEXT(_L8("CCallDMmTsy::AnswerIncomingCallISV called"));
 
 	TInt error = KErrNone;
 		
 	if(aCallParams)
		{  
		
		RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg* callparamsPckg=
				reinterpret_cast<RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg*>(const_cast<TDesC8*>(aCallParams));
 		RMobileCall::TEtel3rdPartyMobileCallParamsV1& callparams=(*callparamsPckg)();
	
		// check that the request is a 3rd party request
		if(callparams.ExtensionId() == RMobileCall::KETel3rdPartyCallParamsV1)	
	  		{
			LOGTEXT(_L8("CCallDMmTsy::ANSWERINCOMINGCALLISV,  request from 3rd party."));   
			}
		else 
		   {
		   	LOGTEXT(_L8("CCallDMmTsy::ANSWERINCOMINGCALLISV, request from non-3rd party."));
		   	error = KErrAccessDenied;
		   }
		}
		
	 	else
		{	// No call parms data
			error = KErrCorrupt;
		} 
		 

	if (error != KErrNone)
		ReqCompleted(aTsyReqHandle,error);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
 
	}

TInt CCallDMmTsy::AnswerIncomingCallISVCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::AnswerIncomingCallISVCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CCallDMmTsy::NotifyAudioToneEvent(const TTsyReqHandle aTsyReqHandle, TDes8* aAudioTone)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyAudioToneEventChange called"));
	
	RMobileCall::TAudioToneV3Pckg* audioPckgV3 = (RMobileCall::TAudioToneV3Pckg*)aAudioTone;
	RMobileCall::TAudioToneV3& audioTone = (*audioPckgV3)();
		
	switch(iNotifyAudioToneEvent++)
		{
	case 0:
		audioTone.iEvent = DMMTSY_AUDIO_START_TONE;
		audioTone.iTone = DMMTSY_AUDIO_CONGESTION_TONE;
		audioTone.iPlayedLocally = DMMTSY_AUDIO_PLAYED_LOCALLY;
		ReqCompleted(aTsyReqHandle,KErrNone);
		break;
	case 1:
		audioTone.iEvent = DMMTSY_AUDIO_STOP_TONE;
		audioTone.iTone = DMMTSY_AUDIO_NO_TONE;
		audioTone.iPlayedLocally = DMMTSY_AUDIO_PLAYED_LOCALLY;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		break;
	default:	
		break;
		}	
	return KErrNone;
	}

TInt CCallDMmTsy::NotifyAudioToneEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallDMmTsy::NotifyAudioToneEventChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
