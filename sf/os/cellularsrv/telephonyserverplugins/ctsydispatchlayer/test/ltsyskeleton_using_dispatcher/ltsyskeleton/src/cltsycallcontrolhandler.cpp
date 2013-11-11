// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cltsydispatchcallcontrolhandler.cpp
//

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>



#include "cltsycallcontrolhandler.h"
#include "ltsymacros.h"
#include <ctsy/ltsy/ltsylogger.h>

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

CLtsyCallControlHandler::CLtsyCallControlHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}


CLtsyCallControlHandler::~CLtsyCallControlHandler()
	{
	}


CLtsyCallControlHandler* CLtsyCallControlHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyCallControlHandler* self = new (ELeave) CLtsyCallControlHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyCallControlHandler* CLtsyCallControlHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyCallControlHandler* self=
			CLtsyCallControlHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyCallControlHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;


	} // CLtsyCallControlHandler::ConstructL

TBool CLtsyCallControlHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface API is supported or not.
 *
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_SUPPORT_MOCKLTSYENGINE(KDispatchCallControlFuncUnitId, aDispatchApiId);

	/* uncomment when implementing for real LTSY 
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;

		default:
			return EFalse;
		}
	*/
	}

void CLtsyCallControlHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 *
 * @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
 * @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_IND_SUPPORT_MOCKLTSYENGINE(KDispatchCallControlFuncUnitId, aIdGroup, aIndIdBitMask);

	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	/*
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchPhoneNotifyNetworkModeChangeIndId |
						KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId |
						KLtsyDispatchPhoneNotifySignalStrengthChangeIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	*/
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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId,
		MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId, aCallId, aIsIsvCall);

	return TSYLOGSETEXITERR(ret);
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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId, aCallId);

	return TSYLOGSETEXITERR(ret);
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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId, &aEmergencyNumber);

	return TSYLOGSETEXITERR(ret);
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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId,
			MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId,
			aCallId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleStopDtmfToneReqL


TInt CLtsyCallControlHandler::HandleSetActiveAlsLineReqL(RMobilePhone::TMobilePhoneALSLine aAlsLine)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId,
		MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId, &aAlsLine);

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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId,
				aCallId);

	return TSYLOGSETEXITERR(ret);
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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, aCallId, aHangupCause);

	return TSYLOGSETEXITERR(ret);
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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId, aCallId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleResumeReqL


TInt CLtsyCallControlHandler::HandleSetDynamicHscsdParamsReqL(
	TInt aCallId, const RMobileCall::TMobileHscsdCallParamsV1& aHscsdParams)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId, aCallId, &aHscsdParams);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleSetDynamicHscsdParamsReqL


TInt CLtsyCallControlHandler::HandleDialVoiceReqL(RMobilePhone::TMobilePhoneALSLine aLine,
	const RMobilePhone::TMobileAddress& aDialledParty, const RMobileCall::TMobileCallParamsV1& aCallParamsV1,
	TBool aIsIsvCall, RMobileCall::TCallParamOrigin aCallOrigin, TBool aPerformFdnCheck)
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
 * @param aPerformFdnCheck whether or not a Fixed Dialling Number check should be
 * performed.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId,
			&aLine, &aDialledParty, &aCallParamsV1, aIsIsvCall, &aCallOrigin, aPerformFdnCheck);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleDialReqL


TInt CLtsyCallControlHandler::HandleTransferReqL(TInt aCallId, TInt aSecondCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlTransferComp()
 *
 * @param aCallId Call ID of the held call to transfer.
 *
 * @param aSecondCallId Call ID of the other to transfer the held call to.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId,
			aCallId, aSecondCallId);

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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret,
			KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId,
			aCallId, &aTones);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleSendDtmfTonesReqL


TInt CLtsyCallControlHandler::HandleGetIdentityServiceStatusReqL(RMobilePhone::TMobilePhoneIdService aService)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId,
			MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId,
			&aService);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetIdentityServiceStatusReqL


TInt CLtsyCallControlHandler::HandleSwapReqL(TInt aCallId, TInt aSecondCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlSwapComp()
 *
 * @param aCallId The Call ID of the call to swap.
 *
 * @param aSecondCallId The Call ID of the second call to swap.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId,
			aCallId, aSecondCallId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleSwapReqL

TInt CLtsyCallControlHandler::HandleSwapReqL(TInt aCallId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlSwapComp()
 *
 * @param aCallId The Call ID of the call to swap.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId,
			aCallId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleSwapReqL

TInt CLtsyCallControlHandler::HandleLoanDataPortSyncL(TInt aCallId, RCall::TCommPort& aCommPort)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId,
		aCallId, &aCommPort);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleLoanDataPortSyncL


TInt CLtsyCallControlHandler::HandleRecoverDataPortSyncL(TInt aCallId, RCall::TCommPort& aCommPort)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId,
			MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId,
			aCallId, &aCommPort);

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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId,
		MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId,
		aCallId, &aTone);

	return TSYLOGSETEXITERR(ret);
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchCallControlFuncUnitId, KLtsyDispatchCallControlGetActiveAlsLineApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetActiveAlsLineReqL

TInt CLtsyCallControlHandler::HandleDialDataReqL(const RMobilePhone::TMobileAddress& aDialledParty, const RMobileCall::TMobileDataCallParamsV1& aCallParamsV1, TBool aPerformFdnCheck)
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
 * @param aPerformFdnCheck whether or not a Fixed Dialling Number check should be
 * performed.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId,
			&aDialledParty, &aCallParamsV1, aPerformFdnCheck);

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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId,
			&aNumber, &aIsEmergencyNumber);

	return TSYLOGSETEXITERR(ret);
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchCallControlFuncUnitId,
			MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId);

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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchCallControlFuncUnitId,
			MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetAlsBlockedStatus

TInt CLtsyCallControlHandler::HandleSetAlsBlockedL(RMmCustomAPI::TSetAlsBlock aBlocked)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId,
                                   MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId, aBlocked);

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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchCallControlFuncUnitId,
			MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetLifeTime

TInt CLtsyCallControlHandler::HandleTerminateErrorCallL(TInt aCallId)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId,
                                   MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId, aCallId);

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
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchCallControlFuncUnitId,
                                   MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId);

	return TSYLOGSETEXITERR(ret);
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchCallControlFuncUnitId,
			MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler::HandleGetCallForwardingIndicator

TInt CLtsyCallControlHandler::HandleUpdateLifeTimerReqL(TUint32 aDuration)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId,
			aDuration);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlHandler:::HandleUpdateLifeTimerReqL

