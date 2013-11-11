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
// CLtsyCallControlHandler

//system include
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>
#include <ctsy/ltsy/ltsylogger.h>
//#include <e32des16.h> 

#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>

//user include
#include "cltsycallcontrolhandler.h"
#include "globalphonemanager.h"
#include "ltsycommondefine.h"
#include "mslogger.h"
#include "ltsymacros.h"
#include "tsyconfg.h"
#include "atwaitforcallhandler.h"
#include "atdialvoice.h"
#include "athangup.h"
#include "atanswer.h"
#include "atchld.h"
#include "unsolicitedcommandcallcontrolhandler.h"
#include "atdtmfvts.h"
#include "ltsycallinformationmanager.h"
#include "athangupcommandrecords.h"


//const define
_LIT16(KLtsyEmergencyCallNumber911, "911");
_LIT16(KLtsyEmergencyCallNumber112, "112");

CLtsyCallControlHandler::CLtsyCallControlHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback, CGlobalPhonemanager* aGloblePhone)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback), iGloblePhone(aGloblePhone)
	{

	}


CLtsyCallControlHandler::~CLtsyCallControlHandler()
	{
	delete iEmergencyCall;
	iEmergencyCall = NULL;
	
	delete iUnsolicitedHandler;
	iUnsolicitedHandler = NULL;
	
	delete iWaitForCall;
	iWaitForCall = NULL;
	}


CLtsyCallControlHandler* CLtsyCallControlHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,
	CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyCallControlHandler* self = new (ELeave) CLtsyCallControlHandler(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyCallControlHandler* CLtsyCallControlHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback, CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyCallControlHandler* self=
			CLtsyCallControlHandler::NewLC(aCtsyDispatcherCallback, aGloblePhone);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyCallControlHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{	
	//Emergency Call
	iEmergencyCall = CATDialVoice::NewL((*iGloblePhone),iCtsyDispatcherCallback);
	iEmergencyCall->SetEmergnecyCallFlag(ETrue);
	
	//Process unsolicited command
	iUnsolicitedHandler = CUnsolicitedCommandCallControlHandler::NewL((*iGloblePhone), 
			                                                          iCtsyDispatcherCallback);
	
	//Monitor incoming call
	iWaitForCall = CATWaitForCallHandler::NewL((*iGloblePhone), iCtsyDispatcherCallback);
	iWaitForCall->StartToWaitForCall();

	} // CLtsyCallControlHandler::ConstructL

TBool CLtsyCallControlHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface API is supported or not.
 *
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	TBool bFlag(EFalse);
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY
		case MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId :
			bFlag =  ETrue;
			break;		
		case MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId:
			bFlag =  ETrue;
			break;
		case MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId :
			bFlag =  ETrue;
			break;
		case MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId :
			bFlag =  ETrue;
			break;		
		case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId :
			bFlag = ETrue;
			break;
		case MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId :
			bFlag = ETrue;
			break;
		default:
			bFlag = EFalse;
			break;
		}
	return bFlag;
	}

void CLtsyCallControlHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 *
 * @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
 * @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{

	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchCallControlNotifyCallStatusChangeIndId |
		KLtsyDispatchCallControlReceiveUuiIndId |
		KLtsyDispatchCallControlNotifyDataCallCapsChangeIndId|
		KLtsyDispatchCallControlNotifyIncomingCallIndId|
		KLtsyDispatchCallControlNotifyHscsdInfoChangeIndId|
		KLtsyDispatchCallControlNotifyCallEventIndId|
		KLtsyDispatchCallControlGetCallInfoIndId|
		KLtsyDispatchCallControlNotifyIccCallForwardingStatusChangeIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	
	}

TInt CLtsyCallControlHandler::HandleAnswerReqL(TInt aCallId, TBool aIsIsvCall)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlAnswerComp()
 *
 * @param aCallId The Call ID of the call to answer.
 *
 * @param aIsIsvCall ETrue if the request to answer the call comes from a
 * 3rd party application, EFalse otherwise. This parameter exists in case the
 * LTSY wishes to perform special handling of ISV calls.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleAnswerReqL()"));
	
	TInt nEmergencyCallId = iGloblePhone->GetCallInfoManager().FindEmergencyCallId();
	if (nEmergencyCallId != KErrNotFound)
		{
		const TLtsyCallInformation& tCallInfo(iGloblePhone->GetCallInfoManager().GetCallInformationByCallId(nEmergencyCallId));
		if (TLtsyCallInformation::EHeldCall  !=  tCallInfo.GetCallState())
			{
			//Reset Ring state
			iGloblePhone->GetCallInfoManager().SetIncomingCallFlag(EFalse);			
			return KErrNotSupported;
			}
		}	
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Reset Ring state
		iGloblePhone->GetCallInfoManager().SetIncomingCallFlag(EFalse);
		
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		//Create answer call
		CATAnswer* cAnswerCall = CATAnswer::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cAnswerCall);
		cAnswerCall->SetAnswerCallId(aCallId);
		cAnswerCall->SetIsIsvCall(aIsIsvCall);
		//Start answer
		cAnswerCall->StartRequest();		
		}
	
	return nRet;
	} // CLtsyCallControlHandler::HandleAnswerReqL

TInt CLtsyCallControlHandler::HandleHoldReqL(TInt aCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlHoldComp()
 *
 * @param aCallId The Call ID of the call to hold.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleHoldReqL"));
	LOGTEXT2(_L8("[Ltsy CallControl] aCallId = %d"), aCallId);
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Create hold call
		CATChld* cHoldCall = CATChld::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cHoldCall);
		//Check call id is a hold call or not
		if (cHoldCall->IsHoldCall(aCallId))
			{
			LOGTEXT2(_L8("[Ltsy CallControl] aCallId = %d is a hold call so return KErrNone"), aCallId);
			iGloblePhone->iEventSignalActive = EFalse;
			return KErrNotSupported;
			}
		
		cHoldCall->SetCallId(aCallId);
		cHoldCall->SetIpcType(CATChld::ELtsyReqCallHold);
		cHoldCall->SetChldType(CATChld::EHeldAllActiveCallAndAcceptHeldWaiting);
		cHoldCall->StartRequest();			
		}
	
	return nRet;
	} // CLtsyCallControlHandler::HandleHoldReqL


TInt CLtsyCallControlHandler::HandleDialEmergencyReqL(const TDes& aEmergencyNumber)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlDialEmergencyComp()
 *
 * @param aEmergencyNumber The emergency phone number to dial.  The descriptor
 * should be of type RMobileENStore::TEmergencyNumber.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleDialEmergencyReqL()"));
	
	//Checking is have emergency call
	if (iGloblePhone->GetCallInfoManager().FindEmergencyCallId() != KErrNotFound)
		{
		return KErrNotSupported;
		}
	
	TBuf8<RMobilePhone::KMaxMobileTelNumberSize> tTelNum;
	tTelNum.Copy(aEmergencyNumber.Left(tTelNum.MaxLength()));
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Init call
		iEmergencyCall->InitVariable();
		
		//Set telnum
		iEmergencyCall->SetTelephoneNumber(tTelNum);
		iEmergencyCall->StartRequest();
		}
	else // KErrInUse
		{
		if (iGloblePhone->GetCallInfoManager().FindDialingOrAlertingCallId() != KErrNotFound)
			{
			//Delete last call
			iGloblePhone->DeleteLastActiveRequest();
			
			//Setting Port active
			iGloblePhone->iEventSignalActive = ETrue;
			
			//Init call
			iEmergencyCall->InitVariable();
			//Set telnum
			iEmergencyCall->SetTelephoneNumber(tTelNum);
			iEmergencyCall->StartRequest();
			nRet = KErrNone;
			}	
		}
	
	return nRet;
	} // CLtsyCallControlHandler::HandleDialEmergencyReqL

TInt CLtsyCallControlHandler::HandleStopDtmfToneReqL(TInt aCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlStopDtmfToneComp()
 *
 * @param aCallId Call ID of the connected call the stop request will be sent
 * through.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleStopDtmfToneReqL()"));
	LOGTEXT2( _L8("[Ltsy CallControl] CallId = %d"), aCallId);
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Create DTMF 
		CATDtmfVts* cDtmfVts = CATDtmfVts::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cDtmfVts);
		
		//Set CallId
		nRet = cDtmfVts->SetCallId(aCallId);
		if (nRet != KErrNone)
			{
			return nRet;
			}	
		
		//Set work type
		cDtmfVts->SetDtmfWorkType(CATDtmfVts::EDtmfStopOneTone);
		
		//Start request
		cDtmfVts->StartRequest();		
		}	
	
	return nRet;
	} // CLtsyCallControlHandler::HandleStopDtmfToneReqL


TInt CLtsyCallControlHandler::HandleSetActiveAlsLineReqL(RMobilePhone::TMobilePhoneALSLine /*aAlsLine*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlSetActiveAlsLineComp()
 *
 * @param aAlsLine The new active ALS line.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleSetActiveAlsLineReqL


TInt CLtsyCallControlHandler::HandleSendDtmfTonesCancelReqL(TInt aCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesCancelComp()
 *
 * @param aCallId The Call ID through which the cancel request should be sent.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{

	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleSendDtmfTonesCancelReqL()"));
	LOGTEXT2( _L8("[Ltsy CallControl] CallId = %d"), aCallId);
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Create DTMF 
		CATDtmfVts* cDtmfVts = CATDtmfVts::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cDtmfVts);
		
		//Set CallId
		nRet = cDtmfVts->SetCallId(aCallId);
		if (nRet != KErrNone)
			{
			return nRet;
			}	
		
		//Set work type
		cDtmfVts->SetDtmfWorkType(CATDtmfVts::EDtmfCancelMoreTones);
		
		//Start request
		cDtmfVts->StartRequest();		
		}
	
	return nRet;
	} // CLtsyCallControlHandler::HandleSendDtmfTonesCancelReqL


TInt CLtsyCallControlHandler::HandleHangUpReqL(TInt aCallId, TInt aHangupCause)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlHangUpComp()
 *
 * @param aCallId The Call ID of the call to hang up.
 *
 * @param aHangupCause The reason for the hang up request.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleHangUpReqL"));
	LOGTEXT3(_L8("[Ltsy CallControl]  aCallId = %d aHangupCause = %d"), aCallId, aHangupCause);

	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Reset Ring state
		iGloblePhone->GetCallInfoManager().SetIncomingCallFlag(EFalse);
		
		//Set hang up active
		iGloblePhone->GetCallInfoManager().GetHangupCommandRecords().SetHangupActiveFlag(ETrue);
		
		//Create Hang up  
		CATChld* cHangupCall = CATChld::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cHangupCall);
		cHangupCall->SetCallId(aCallId);
		cHangupCall->SetHangupCase(aHangupCause);
		cHangupCall->SetIpcType(CATChld::ELtsyReqCallHangup);
		cHangupCall->SetChldType(CATChld::ERleaseSpecificCall);
		cHangupCall->StartRequest();			
		}
	else if (nRet == KErrInUse)
		{
		LOGTEXT( _L8("[Ltsy CallControl]  Starting Hang up in process so add hangup information"));
		
		const TLtsyCallInformation& tCallInfo(iGloblePhone->GetCallInfoManager().GetCallInformationByCallId(aCallId));
		
		if (iGloblePhone->GetCallInfoManager().GetHangupCommandRecords().GetHangupActiveFlag())
			{
			//Hang up is processing so add its information to array
			nRet = iGloblePhone->GetCallInfoManager().GetHangupCommandRecords().AddHangupCommandInfo(aCallId, aHangupCause);					
			}
		else if (TLtsyCallInformation::EDialingCall  ==  tCallInfo.GetCallState() ||
				 TLtsyCallInformation::EAlertingCall ==  tCallInfo.GetCallState())
			{
			
			//Delete last call
			iGloblePhone->DeleteLastActiveRequest();
			
			//Setting Port active
			iGloblePhone->iEventSignalActive = ETrue;	
			
			//Set hang up active
			iGloblePhone->GetCallInfoManager().GetHangupCommandRecords().SetHangupActiveFlag(ETrue);
			
			//Create Hang up  
			CATChld* cHangupCall = CATChld::NewL((*iGloblePhone), iCtsyDispatcherCallback);
			iGloblePhone->SetActiveRequest(cHangupCall);
			cHangupCall->SetCallId(aCallId);
			cHangupCall->SetHangupCase(aHangupCause);
			cHangupCall->SetIpcType(CATChld::ELtsyReqCallHangup);
			cHangupCall->SetChldType(CATChld::ERleaseSpecificCall);
			cHangupCall->StartRequest();
			nRet = KErrNone;
			}
		}	
	
	return nRet;
	} // CLtsyCallControlHandler::HandleHangUpReqL


TInt CLtsyCallControlHandler::HandleResumeReqL(TInt aCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlResumeComp()
 *
 * @param aCallId The Call ID of the call to resume.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleResumeReqL"));
	LOGTEXT2(_L8("[Ltsy CallControl] aCallId = %d"), aCallId);	

	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Create DTMF 
		CATChld* cResumeCall = CATChld::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cResumeCall);
		if (cResumeCall->IsActiveCall(aCallId))
			{
			LOGTEXT2(_L8("[Ltsy CallControl] aCallId = %d is a active call so return KErrNone"), aCallId);
			iGloblePhone->iEventSignalActive = EFalse;
			return KErrNotSupported;
			}
		cResumeCall->SetCallId(aCallId);
		cResumeCall->SetIpcType(CATChld::ELtsyReqCallResume);
		cResumeCall->SetChldType(CATChld::EHeldAllActiveCallAndAcceptHeldWaiting);
		cResumeCall->StartRequest();		
		}
	
	return nRet;
	} // CLtsyCallControlHandler::HandleResumeReqL

TInt CLtsyCallControlHandler::HandleSetDynamicHscsdParamsReqL(
	TInt /*aCallId*/, const RMobileCall::TMobileHscsdCallParamsV1& /*aHscsdParams*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlSetDynamicHscsdParamsComp()
 *
 * @param aCallId The call ID of the data call.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleSetDynamicHscsdParamsReqL


TInt CLtsyCallControlHandler::HandleDialVoiceReqL(RMobilePhone::TMobilePhoneALSLine /*aLine*/,
	const RMobilePhone::TMobileAddress& aDialledParty, const RMobileCall::TMobileCallParamsV1& /*aCallParamsV1*/,
	TBool /*aIsIsvCall*/, RMobileCall::TCallParamOrigin /*aCallOrigin*/, TBool /*aPerformFdnCheck*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp()
 *
 * @param aCallLine The line to use to dial the call.
 *
 * @param aDialledParty Details about the dialled party including the phone
 * number to dial.
 *
 * @param aCallParamsV1 The call parameters of the call to dial.
 *
 * @param aIsIsvCall Indicates whether the call originated from a third party
 * application.
 *
 * @param aCallOrigin The origin of the dial request. e.g. Whether the dial
 * came from an Etel client or the SIM or another source.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleDialVoiceReqL()"));
	
	TInt nEmergencyCallId = iGloblePhone->GetCallInfoManager().FindEmergencyCallId();
	if (nEmergencyCallId != KErrNotFound)
		{
		const TLtsyCallInformation& tCallInfo(iGloblePhone->GetCallInfoManager().GetCallInformationByCallId(nEmergencyCallId));
		if (TLtsyCallInformation::EHeldCall  !=  tCallInfo.GetCallState())
			{
			return KErrNotSupported;
			}
		}
	
	//Set telephone number
	TBuf8<KCommsDbSvrMaxFieldLength> dialModifier;
	TInt nRet = iGloblePhone->iConfiguration->ConfigModemStringL(
								TPtrC(KCDTypeNameDialToneWaitModifier),
								dialModifier);
	if (nRet != KErrNone)
		{
		return nRet;
		}
	
	TBuf8<RMobilePhone::KMaxMobileTelNumberSize> tTelNum;
	tTelNum.Copy(aDialledParty.iTelNumber.Left(tTelNum.MaxLength()));
	
	nRet = tTelNum.FindF(dialModifier);
	if (nRet != KErrNotFound)
		{
		tTelNum.Delete(nRet, 1);
		}
	
	nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Create dial voice
		CATDialVoice* cDialVoice = CATDialVoice::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cDialVoice);
		cDialVoice->SetTelephoneNumber(tTelNum);
		cDialVoice->StartRequest();
		}
	
	return nRet;
	} // CLtsyCallControlHandler::HandleDialReqL

TInt CLtsyCallControlHandler::HandleTransferReqL(TInt /*aHeldCallId*/, TInt /*aSecondCallId*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlTransferComp()
 *
 * @param aHeldCallId Call ID of the held call to transfer.
 *
 * @param aSecondCallId Call ID of the other to transfer the held call to.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleTransferReqL


TInt CLtsyCallControlHandler::HandleSendDtmfTonesReqL(TInt aCallId, const TDesC &aTones)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesComp()
 *
 * @param aCallId The Call ID of the call through which the DTMF string will be
 * sent.
 *
 * @param aTones Tones to send through the active call.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT( _L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleSendDtmfTonesReqL()"));
	LOGTEXT3( _L8("[Ltsy CallControl] CallId = %d\tTones = %S"), aCallId, &aTones);
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Create DTMF 
		CATDtmfVts* cDtmfVts = CATDtmfVts::NewL((*iGloblePhone), iCtsyDispatcherCallback);	
		iGloblePhone->SetActiveRequest(cDtmfVts);
		//Set CallId
		TInt nRet = cDtmfVts->SetCallId(aCallId);
		if (nRet != KErrNone)
			{
			return nRet;
			}	
		
		//Set Tones
		nRet = cDtmfVts->SetDtmfString(aTones);
		if (nRet != KErrNone)
			{
			return nRet;
			}	
		
		//set work type
		cDtmfVts->SetDtmfWorkType(CATDtmfVts::EDtmfSendMoreTones);
		cDtmfVts->StartRequest();
		}	
	
	return nRet;
	} // CLtsyCallControlHandler::HandleSendDtmfTonesReqL


TInt CLtsyCallControlHandler::HandleGetIdentityServiceStatusReqL(RMobilePhone::TMobilePhoneIdService /*aService*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlGetIdentityServiceStatusComp()
 *
 * @param aService The service whose status needs to be retrieved.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetIdentityServiceStatusReqL


TInt CLtsyCallControlHandler::HandleSwapReqL(TInt aHeldCallId, TInt aConnectedCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlSwapComp()
 *
 * @param aHeldCallId The Call ID of the held call to swap.
 *
 * @param aConnectedCallId The Call ID of the connected call to swap.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleSwapReqL"));
	LOGTEXT3(_L8("[Ltsy CallControl] aHeldCallId = %d\taConnectedCallId = %d"), aHeldCallId, aConnectedCallId);
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Create DTMF 
		CATChld* cSwapCall = CATChld::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cSwapCall);
		cSwapCall->SetCallId(aHeldCallId);
		cSwapCall->SetConnectedCallId(aConnectedCallId);
		cSwapCall->SetIpcType(CATChld::ELtsyReqCallSwap);
		cSwapCall->SetChldType(CATChld::EHeldAllActiveCallAndAcceptHeldWaiting);
		cSwapCall->StartRequest();			
		}
	
	return nRet;
	} // CLtsyCallControlHandler::HandleSwapReqL

TInt CLtsyCallControlHandler::HandleLoanDataPortSyncL(TInt /*aCallId*/, RCall::TCommPort& /*aCommPort*/)
/**
 * This request is part of
 * MLtsyDispatchCallControlLoanDataPort::HandleLoanDataPortSyncL()
 *
 * @param aCallId Call ID of the data call requiring the data port.
 *
 * @param aCommPort Output parameter. The communications port information
 * retrieved by a client using the RCall::LoanDataPort() API.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleLoanDataPortSyncL


TInt CLtsyCallControlHandler::HandleRecoverDataPortSyncL(TInt /*aCallId*/, RCall::TCommPort& /*aCommPort*/)
/**
 * This request is part of
 * MLtsyDispatchCallControlRecoverDataPort::HandleRecoverDataPortSyncL()
 *
 * @param aCallId Call ID of the data call requiring the data port.
 *
 * @param aCommPort The data port to recover.  This contains the details previously
 * returned by a call to RCall::LoanDataPort()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleRecoverDataPortSyncL


TInt CLtsyCallControlHandler::HandleStartDtmfToneReqL(TInt aCallId, const TChar& aTone)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlStartDtmfTone()
 *
 * @param aCallId Call ID of the connected call through which the DTMF tone
 * will be sent.
 *
 * @param aTone The tone character to send through the call.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleStartDtmfToneReqL()"));
	LOGTEXT2( _L8("[Ltsy CallControl] CallId = %d"), aCallId);
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Create DTMF 
		CATDtmfVts* cDtmfVts = CATDtmfVts::NewL((*iGloblePhone), iCtsyDispatcherCallback);	
		iGloblePhone->SetActiveRequest(cDtmfVts);
		//Set CallId
		TInt nRet = cDtmfVts->SetCallId(aCallId);
		if (nRet != KErrNone)
			{
			return nRet;
			}	
		
		//Set Tones
		TBuf<16> bufTone;
		bufTone.Append(aTone);
		nRet = cDtmfVts->SetDtmfString(bufTone);
		if (nRet != KErrNone)
			{
			return nRet;
			}
		
		//set work type
		cDtmfVts->SetDtmfWorkType(CATDtmfVts::EDtmfSendOneTone);
		cDtmfVts->StartRequest();
		}
	
	return nRet;	
	} // CLtsyCallControlHandler::HandleStartDtmfToneSyncL

TInt CLtsyCallControlHandler::HandleGetActiveAlsLineReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlGetActiveAlsLineComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TO DO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetActiveAlsLineReqL

TInt CLtsyCallControlHandler::HandleDialDataReqL(const RMobilePhone::TMobileAddress& /*aDialledParty*/, const RMobileCall::TMobileDataCallParamsV1& /*aCallParamsV1*/, TBool /*aPerformFdnCheck*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlGetActiveAlsLineComp()
 *
 * @param aLine The line to use to dial the call.
 *
 * @param aDialledParty Details about the dialled party including the phone
 * number to dial.
 *
 * @param aCallParamsV1 The call parameters of the call to dial.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TO DO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleDialDataReqL

TInt CLtsyCallControlHandler::HandleQueryIsEmergencyNumberSyncL(const TDesC& aNumber, TBool& aIsEmergencyNumber)
/**
 * This request is part of
 * MLtsyDispatchCallControlQueryIsEmergencyNumber::HandleQueryIsEmergencyNumberSyncL()
 *
 * @param aNumber Input parameter. The phone number being queried to see
 * if it is an emergency number or not.
 *
 * @param aIsEmergencyNumber Output parameter used to indicate whether the
 * aNumber parameter is an emergency number. ETrue if it is, EFalse otherwise.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleQueryIsEmergencyNumberSyncL()"));
	
	TInt nEmergencyCallId = iGloblePhone->GetCallInfoManager().FindEmergencyCallId();
	if (nEmergencyCallId != KErrNotFound)
		{
		const TLtsyCallInformation& tCallInfo(iGloblePhone->GetCallInfoManager().GetCallInformationByCallId(nEmergencyCallId));
		if (TLtsyCallInformation::EHeldCall  !=  tCallInfo.GetCallState())
			{			
			return KErrNotSupported;
			}
		}	
	
	//Check Emergency call number	
	if (aNumber.CompareF(KLtsyEmergencyCallNumber911) == 0 ||
		aNumber.CompareF(KLtsyEmergencyCallNumber112) == 0 )
		{
		aIsEmergencyNumber = ETrue;
		}
	else
		{
		aIsEmergencyNumber = EFalse;
		}
	
	return KErrNone;
	} // CLtsyCallControlHandler::HandleQueryIsEmergencyNumberSyncL


TInt CLtsyCallControlHandler::HandleGetAlsPpSupportL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlGetAlsPpSupportComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetAlsPp

TInt CLtsyCallControlHandler::HandleGetAlsBlockedStatusL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlGetAlsBlockedStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetAlsBlockedStatus

TInt CLtsyCallControlHandler::HandleSetAlsBlockedL(RMmCustomAPI::TSetAlsBlock /*aBlocked*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlSetAlsBlockedComp()
 *
 *
 * @param aBlocked RMmCustomAPI::EDeactivateBlock when the ALS blocked needs to be deactivate,
 *                 RMmCustomAPI::EActivateBlock when the  ALS blocked needs to be activate.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleSetAlsBlockedL

TInt CLtsyCallControlHandler::HandleGetLifeTimeL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetLifeTime

TInt CLtsyCallControlHandler::HandleTerminateErrorCallL(TInt /*aCallId*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlTerminateErrorCallComp()
 *
 *
 * @param aCallId The Call ID of the call to terminate.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleTerminateErrorCallL

TInt CLtsyCallControlHandler::HandleTerminateAllCallsL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::::CallbackCallControlTerminateAllCallsComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	LOGTEXT(_L8("[Ltsy CallControl]  Starting CLtsyCallControlHandler::HandleTerminateAllCallsL()"));
	
	TInt nRet = iGloblePhone->CheckGlobalPhoneStatus();
	if (nRet == KErrNone)
		{
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		
		//Reset Ring state
		iGloblePhone->GetCallInfoManager().SetIncomingCallFlag(EFalse);		
		
		CATHangUp* cTerminateAllCalls = CATHangUp::NewL((*iGloblePhone), iCtsyDispatcherCallback);
		iGloblePhone->SetActiveRequest(cTerminateAllCalls);
		cTerminateAllCalls->SetHangupOperator(CATHangUp::ECustomAPI);
		cTerminateAllCalls->StartRequest();	
		}
	
	return nRet;
	} // CLtsyCallControlHandler::HandleTerminateAllCallsL

TInt CLtsyCallControlHandler::HandleGetCallForwardingIndicatorL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlGetCallForwardingIndicatorComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetCallForwardingIndicator

TInt CLtsyCallControlHandler::HandleUpdateLifeTimerReqL(TUint32 /*aDuration*/)
/**
 * This request is completed by invoking
 *  CCtsyDispatcherCallback::CallbackCallControlUpdateLifeTimerComp
 *
 * @param aDuration Time spent on call since the last invocation of this method.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TO DO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler:::HandleUpdateLifeTimerReqL


MLtsyUnsolicitedCommandObserver* CLtsyCallControlHandler::GetUnsolicitedCallControlHandler() const
	{
	return static_cast<MLtsyUnsolicitedCommandObserver*>(iUnsolicitedHandler);
	}

//End of file
