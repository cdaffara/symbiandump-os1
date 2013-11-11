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
// This file contains all the interfaces classes that can be implemented by
// the Licensee LTSY relating to CallControl related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHCALLCONTROLINTERFACE_H_
#define MLTSYDISPATCHCALLCONTROLINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>
#include <ctsy/rmmcustomapi.h>

class MLtsyDispatchCallControlAnswer : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlAnswerApiId = KDispatchCallControlFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EEtelCallAnswer
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlAnswerComp()
	 *
	 * Implementation of this interface should answer the specified incoming call.
	 *
	 * @param aCallId The Call ID of the call to answer.
	 *
	 * @param aIsIsvCall ETrue if the request to answer the call comes from a
	 * 3rd party application, EFalse otherwise. This parameter exists in case the
	 * LTSY wishes to perform special handling of ISV calls.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RCall::AnswerIncomingCall()
	 * @see RCall::AnswerIncomingCallISV()
	 * @see CTelephony::AnswerIncomingCall()
	 */
	virtual TInt HandleAnswerReqL(TInt aCallId, TBool aIsIsvCall) = 0;

	}; // class MLtsyDispatchCallControlAnswer



class MLtsyDispatchCallControlHold : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlHoldApiId = KDispatchCallControlFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileCallHold
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlHoldComp()
	 *
	 * Implementation of this interface should hold the specified call.
	 *
	 * @param aCallId The Call ID of the call to hold.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::Hold()
	 */
	virtual TInt HandleHoldReqL(TInt aCallId) = 0;

	}; // class MLtsyDispatchCallControlHold



class MLtsyDispatchCallControlDialEmergency : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlDialEmergencyApiId = KDispatchCallControlFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileCallDialEmergencyCall
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlDialEmergencyComp()
	 *
	 * Implementation of this interface should dial an emergency call to the specified
	 * number.
	 *
	 * @param aEmergencyNumber The emergency phone number to dial.  The descriptor
	 * should be of type RMobileENStore::TEmergencyNumber.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::DialEmergencyCall()
	 */
	virtual TInt HandleDialEmergencyReqL(const TDes& aEmergencyNumber) = 0;

	}; // class MLtsyDispatchCallControlDialEmergency


class MLtsyDispatchCallControlStopDtmfTone : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlStopDtmfToneApiId = KDispatchCallControlFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneStopDTMFTone
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlStopDtmfToneComp()
	 *
	 * Implementation of this interface should stop sending the DTMF tone previously
	 * started by MLtsyDispatchCallControlStartDtmfTone::HandleStartDtmfToneReqL().
	 *
	 * @param aCallId Call ID of the connected call the stop request will be sent
	 * through.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::StopDTMFTone()
	 */
	virtual TInt HandleStopDtmfToneReqL(TInt aCallId) = 0;

	}; // class MLtsyDispatchCallControlStopDtmfTone



class MLtsyDispatchCallControlSetActiveAlsLine : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlSetActiveAlsLineApiId = KDispatchCallControlFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetALSLine
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlSetActiveAlsLineComp()
	 *
	 * Implementation of this interface should set the active ALS line to that
	 * specified.
	 *
	 * @param aAlsLine The new active ALS line.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobilePhone::SetALSLine()
	 */
	virtual TInt HandleSetActiveAlsLineReqL(RMobilePhone::TMobilePhoneALSLine aAlsLine) = 0;

	}; // class MLtsyDispatchCallControlSetActiveAlsLine



class MLtsyDispatchCallControlSendDtmfTonesCancel : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlSendDtmfTonesCancelApiId = KDispatchCallControlFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSendDTMFTonesCancel
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesCancelComp()
	 *
	 * Implementation of this interface should attempt to cancel the sending of
	 * a string of DTMF tones which were previously sent by
	 * MLtsyDispatchCallControlSendDtmfTones::HandleSendDtmfTonesReqL()
	 *
	 * @param aCallId The Call ID of the active call that the cancel request relates
	 * to.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobilePhone::SendDTMFTones()
	 */
	virtual TInt HandleSendDtmfTonesCancelReqL(TInt aCallId) = 0;

	}; // class MLtsyDispatchCallControlSendDtmfTonesCancel



class MLtsyDispatchCallControlHangUp : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlHangUpApiId = KDispatchCallControlFuncUnitId + 7;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EEtelCallHangUp
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlHangUpComp()
	 *
	 * Implementation of this interface should
	 * make a request to the modem to hang up the call specified
	 * in aCallId.  The completion should be called to indicate the result of
	 * placing the hang up request with the baseband.
	 *
	 *  - If aCallId refers to a connected call, the Common TSY will pass
	 * a hang up cause of KErrGsmReleaseByUser to indicate that the call is
	 * being released by the user.
	 *
	 *  - If aCallId refers to a mobile terminated call which is ringing,
	 * the Common TSY will pass a hang up cause of KErrGsmBusyUserRequest to
	 * indicate that the user is rejecting the incoming call.
	 *
	 * @param aCallId The Call ID of the call to hang up.
	 *
	 * @param aHangupCause The reason for the hang up request.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RCall::HangUp()
	 */
	virtual TInt HandleHangUpReqL(TInt aCallId, TInt aHangupCause) = 0;

	}; // class MLtsyDispatchCallControlHangUp



class MLtsyDispatchCallControlResume : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlResumeApiId = KDispatchCallControlFuncUnitId + 8;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileCallResume
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlResumeComp()
	 *
	 * Implementation of this interface should resume the specified call.
	 * See 3GPP 24.083
	 *
	 * @param aCallId The Call ID of the call to resume.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::Resume()
	 */
	virtual TInt HandleResumeReqL(TInt aCallId) = 0;

	}; // class MLtsyDispatchCallControlResume



class MLtsyDispatchCallControlSetDynamicHscsdParams : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlSetDynamicHscsdParamsApiId = KDispatchCallControlFuncUnitId + 9;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileCallSetDynamicHscsdParams
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlSetDynamicHscsdParamsComp()
	 *
	 * Implementation of this interface should set the dynamic HSCSD parameters
	 * of the specified data call.
	 *
	 * @param aCallId The call ID of the data call.
	 *
	 * @param aHscsdParams The requested dynamic HSCSD parameters.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::SetDynamicHscsdParams()
	 */
	virtual TInt HandleSetDynamicHscsdParamsReqL(TInt aCallId, const RMobileCall::TMobileHscsdCallParamsV1& aHscsdParams) = 0;

	}; // class MLtsyDispatchCallControlSetDynamicHscsdParams



class MLtsyDispatchCallControlDialVoice : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlDialVoiceApiId = KDispatchCallControlFuncUnitId + 10;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EEtelCallDial
	 * request from the CTSY for dialling a voice call.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp()
	 *
	 * Implementation of this interface should
	 * dial the requested voice call and complete the callback to indicate
	 * that an attempt to dial the call has been made.  The callback function is used
	 * to indicate the outcome of this attempt to initiate the dialling process.
	 *
	 * @param aCallLine The line to use to dial the call. This parameter can have
	 * one of two possible values RMobilePhone::EAlternateLinePrimary to use the
	 * primary voice line or RMobilePhone::EAlternateLineAuxiliary to use the
	 * auxiliary voice line.
	 *
	 * @param aDialledParty Details about the dialled party including the phone
	 * number to dial.
	 *
	 * @param aCallParamsV1 The call parameters of the call to dial. If aIsIsvCall
	 * is set to ETrue, only the RMobileCall::TMobileCallParamsV1::iIdRestrict
	 * and RMobileCall::TMobileCallParamsV1::iAutoRedial fields are valid.
	 *
	 * @param aIsIsvCall Indicates whether the call originated from a third party
	 * application which would have used the CTelephony class to dial the call.
	 * This information is provided in case the LTSY wishes to do special handling
	 * for third party calls, for example, not allow third party applications to
	 * place emergency calls. See also
	 * MLtsyDispatchCallControlQueryIsEmergencyNumber::HandleQueryIsEmergencyNumberSyncL()
	 * See also: CTelephony::DialNewCall() and RMobileCall::DialISV()
	 *
	 * @param aCallOrigin The origin of the dial request. e.g. Whether the dial
	 * came from an Etel client or the SIM or another source.
	 * 
	 * @param aPerformFdnCheck whether or not an FDN (Fixed Dialling Number) check should be performed.
	 *
	 * @return KErrNone on success; KErrNotSupported if the LTSY does not
	 * support handling of this request or another error code indicating the
	 * failure otherwise.
	 *
	 * @see RCall::Dial()
	 * @see RMobileCall::DialISV()
	 * @see RMobileCall::DialNoFdnCheck()
	 */
	virtual TInt HandleDialVoiceReqL(RMobilePhone::TMobilePhoneALSLine aCallLine, const RMobilePhone::TMobileAddress& aDialledParty, const RMobileCall::TMobileCallParamsV1& aCallParamsV1, TBool aIsIsvCall, RMobileCall::TCallParamOrigin aCallOrigin,
			TBool aPerformFdnCheck) = 0;

	}; // class MLtsyDispatchCallControlDialVoice



class MLtsyDispatchCallControlTransfer : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlTransferApiId = KDispatchCallControlFuncUnitId + 11;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileCallTransfer
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlTransferComp()
	 *
	 * This request should be completed when the request to transfer the call has
	 * been placed.
	 *
	 * Implementation of this interface should transfer the specified held call.
	 * Transferring a call can be requested when there is one held call and another
	 * call either connected or in the ringing state. When the two remote parties
	 * are transferred to each other, the party requesting this operation (us)
	 * drops out of the call.
	 *
	 * @param aHeldCallId Call ID of the call held call to transfer.
	 *
	 * @param aOtherCallId Call ID of the other call to transfer the held call to.
	 * This call is either an incoming call in the ringing state or a connected call.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::Transfer()
	 */
	virtual TInt HandleTransferReqL(TInt aHeldCallId, TInt aOtherCallId) = 0;

	}; // class MLtsyDispatchCallControlTransfer



class MLtsyDispatchCallControlSendDtmfTones : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlSendDtmfTonesApiId = KDispatchCallControlFuncUnitId + 12;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSendDTMFTones
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesComp()
	 *
	 * Implementation of this interface should send the specified DTMF string through
	 * the currently active call.
	 *
	 * @param aCallId The Call ID of the call through which the DTMF string will be
	 * sent.
	 *
	 * @param aTones Tones to send through the active call.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobilePhone::SendDTMFTones()
	 */
	virtual TInt HandleSendDtmfTonesReqL(TInt aCallId, const TDesC& aTones) = 0;

	}; // class MLtsyDispatchCallControlSendDtmfTones



class MLtsyDispatchCallControlGetIdentityServiceStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlGetIdentityServiceStatusApiId = KDispatchCallControlFuncUnitId + 13;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetIdentityServiceStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlGetIdentityServiceStatusComp()
	 *
	 * Implementation of this interface should request for the status of the
	 * specified identity service such as the Caller ID Presentation Service.
	 *
	 * @param aService The service whose status needs to be retrieved.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobilePhone::GetIdentityServiceStatus()
	 */
	virtual TInt HandleGetIdentityServiceStatusReqL(RMobilePhone::TMobilePhoneIdService aService) = 0;

	}; // class MLtsyDispatchCallControlGetIdentityServiceStatus



class MLtsyDispatchCallControlSwap : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlSwapApiId = KDispatchCallControlFuncUnitId + 14;
	static const TInt KLtsyDispatchCallControlSingleSwapApiId = KDispatchCallControlFuncUnitId + 29;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileCallSwap
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlSwapComp()
	 *
	 * This request should be completed when the request to swap two calls has
	 * been placed. One is held and the other connected. The held call becomes connected
	 * (RMobileCall::EStatusConnected) and the connected call becomes held 
	 * (RMobileCall::EStatusHold).
	 *
	 * Implementation of this interface should request that the Licensee TSY
	 * swap the specified calls. Swapping is allowed for one or two active calls.
	 *
	 * @param aCallId The Call ID of the call to swap.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::Swap()
	 */
	virtual TInt HandleSwapReqL(TInt aCallId, TInt aSecondCallId) = 0;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileCallSwap
	 * request, for a single call, from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlSwapComp()
	 *
	 * This request should be completed when the request to swap the single call has
	 * been placed.
	 *
	 * Implementation of this interface should request that the Licensee TSY
	 * swap the specified call. The state of the call will be swapped from held to connected 
	 * (or visa versa)
	 *
	 * @param aCallId The Call ID of the single call to swap.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::Swap()
	 */
	virtual TInt HandleSwapReqL(TInt aCallId) = 0;

	}; // class MLtsyDispatchCallControlSwap


	
class MLtsyDispatchCallControlLoanDataPort : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlLoanDataPortApiId = KDispatchCallControlFuncUnitId + 15;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EEtelCallLoanDataPort
	 * request from the CTSY.
	 *
	 * Implementation of this interface should read the port name from CommDat
	 * for use in a data call. This API may be adaptation specific.
	 *
	 * It is a sync call where the output parameters should be completed before returning
	 * from this function.
	 *
	 * @param aCallId Call ID of the data call requiring the data port.
	 *
	 * @param aCommPort Output parameter. The communications port information
	 * retrieved by a client using the RCall::LoanDataPort() API.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleLoanDataPortSyncL(TInt aCallId, RCall::TCommPort& aCommPort) = 0;

	}; // class MLtsyDispatchCallControlLoanDataPort



class MLtsyDispatchCallControlRecoverDataPort : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlRecoverDataPortApiId = KDispatchCallControlFuncUnitId + 16;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EEtelCallRecoverDataPort
	 * request from the CTSY.
	 *
	 * Implementation of this interface should recover the data port loaned by a
	 * previous call to
	 * MLtsyDispatchCallControlLoanDataPort::HandleLoanDataPortSyncL()
	 * and matches a client side call to RCall::RecoverDataPort().
	 *
	 * It is a sync call where the data arguments should be completed before returning.
	 *
	 * @param aCallId Call ID of the data call requiring the data port.
	 *
	 * @param aCommPort The data port to recover.  This contains the details previously
	 * returned by a call to RCall::LoanDataPort()
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleRecoverDataPortSyncL(TInt aCallId, RCall::TCommPort& aCommPort) = 0;

	}; // class MLtsyDispatchCallControlRecoverDataPort



class MLtsyDispatchCallControlStartDtmfTone : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlStartDtmfToneApiId = KDispatchCallControlFuncUnitId + 17;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneStartDTMFTone
	 * request from the CTSY.
	 *
	 * Implementation of this interface should send the specified DTMF tone
	 * through the currently active call.
	 *
	 * @param aCallId Call ID of the connected call through which the DTMF tone
	 * will be sent.
	 *
	 * @param aTone The tone character to send through the call.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobilePhone::StartDTMFTone()
	 */
	virtual TInt HandleStartDtmfToneReqL(TInt aCallId, const TChar& aTone) = 0;

	}; // class MLtsyDispatchCallControlStartDtmfTone




class MLtsyDispatchCallControlGetActiveAlsLine : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchCallControlGetActiveAlsLineApiId = KDispatchCallControlFuncUnitId + 18;

public:

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetActiveAlsLine
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlGetActiveAlsLineComp()
	 *
	 * Implementation of this interface should retrieve the currently selected ALS line.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobilePhone::GetALSLine()
	 */
	virtual TInt HandleGetActiveAlsLineReqL() = 0;

	}; // class MLtsyDispatchCallControlGetActiveAlsLine


class MLtsyDispatchCallControlDialData : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchCallControlDialDataApiId = KDispatchCallControlFuncUnitId + 19;
public:

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EEtelCallDial
	 * request from the CTSY when a data call is requested.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlDialDataComp()
	 *
	 * Implementation of this interface should dial the data call to the
	 * specified destination. The callback should be completed when the dial
	 * request has been sent to the baseband and should indicate
	 * that an attempt to dial the call has been made.  The callback function is used
	 * to indicate the outcome of this attempt to initiate the dialling process.
	 *
	 * @param aLine The line to use to dial the call.
	 *
	 * @param aDialledParty Details about the dialled party including the phone
	 * number to dial.
	 *
	 * @param aCallParamsV1 The call parameters of the call to dial.
	 * 
	 * @param aPerformFdnCheck whether or not an FDN (Fixed Dialling Number) check should be performed.
	 *
	 * @return KErrNone on success; KErrNotSupported if the LTSY does not
	 * support handling of this request or another error code indicating the
	 * failure otherwise.
	 *
	 * @see RCall::Dial()
	 */
	virtual TInt HandleDialDataReqL(const RMobilePhone::TMobileAddress& aDialledParty, const RMobileCall::TMobileDataCallParamsV1& aCallParamsV1,
			TBool aPerformFdnCheck) = 0;

	}; // class MLtsyDispatchCallControlDialData


class MLtsyDispatchCallControlQueryIsEmergencyNumber : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchCallControlQueryIsEmergencyNumberApiId = KDispatchCallControlFuncUnitId + 20;
public:

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the
	 * ECustomCheckEmergencyNumberIPC request from the CTSY.
	 *
	 * It is a sync call where the output data arguments should be completed
	 * before returning.
	 *
	 * Implementation of this interface should check whether the specified
	 * number is an emergency number and return the result back to the caller
	 * via the output parameter.
	 *
	 * @param aNumber Input parameter. The phone number being queried to see
	 * if it is an emergency number or not.
	 *
	 * @param aIsEmergencyNumber Output parameter used to indicate whether the
	 * aNumber parameter is an emergency number. ETrue if it is, EFalse otherwise.
	 *
	 * @return KErrNone if the LTSY was successful in determining whether the
	 * queried number is an emergency one; KErrNotSupported if the LTSY does not
	 * support handling of this request or another error code indicating the
	 * failure otherwise.
	 */
	virtual TInt HandleQueryIsEmergencyNumberSyncL(const TDesC& aNumber, TBool& aIsEmergencyNumber) = 0;

	}; // class MLtsyDispatchCallControlQueryIsEmergencyNumber


class MLtsyDispatchCallControlGetAlsPpSupport : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlGetAlsPpSupportApiId = KDispatchCallControlFuncUnitId + 21;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomCheckAlsPpSupportIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlGetAlsPpSupportComp()
	 *
	 * Implementation of this interface should return check if the product profile support ALS.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMmCustomAPI::CheckAlsPpSupport()
	 */
	virtual TInt HandleGetAlsPpSupportL() = 0;

	}; // class MLtsyDispatchCallControlGetAlsPpSupport


class MLtsyDispatchCallControlGetAlsBlockedStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlGetAlsBlockedStatusApiId = KDispatchCallControlFuncUnitId + 22;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetAlsBlockedIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlGetAlsBlockedStatusComp()
	 *
	 * Implementation of this interface should return the blocked status of the ALS service.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMmCustomAPI::GetAlsBlocked()
	 */
	virtual TInt HandleGetAlsBlockedStatusL() = 0;

	}; // class MLtsyDispatchCallControlGetAlsBlockedStatus


class MLtsyDispatchCallControlSetAlsBlocked : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlSetAlsBlockedApiId = KDispatchCallControlFuncUnitId + 23;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSetAlsBlockedIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlSetAlsBlockedComp()
	 *
	 * Implementation of this interface should set the ALS blocked status.
	 *
     * @param aBlocked RMmCustomAPI::EDeactivateBlock when the ALS block needs to be deactivated,
     *                 RMmCustomAPI::EActivateBlock when the ALS block needs to be activated.
     *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMmCustomAPI::SetAlsBlocked()
	 */
	virtual TInt HandleSetAlsBlockedL(RMmCustomAPI::TSetAlsBlock aBlocked) = 0;

	}; // class MLtsyDispatchCallControlSetAlsBlocked


class MLtsyDispatchCallControlGetLifeTime : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlGetLifeTimeApiId = KDispatchCallControlFuncUnitId + 24;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetLifeTimeIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking one of the 
	 * CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp()
	 *
	 * Implementation of this interface should return the lifetime of the phone. The lifetime
     * information includes the manufacturing date of the phone and/or the total amount of air time used
     * from the manufacturing date until the call to this method. Calling this method does not reset any data. 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMmCustomAPI::GetLifeTime()
	 */
	virtual TInt HandleGetLifeTimeL() = 0;

	}; // class MLtsyDispatchCallControlGetLifeTimeStatus


class MLtsyDispatchCallControlTerminateErrorCall : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlTerminateErrorCallApiId = KDispatchCallControlFuncUnitId + 25;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomTerminateCallIPC
	 * request from the CTSY with a valid call id.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlTerminateErrorCallComp()
	 *
	 * Implementation of this interface should terminate the call with an error.
	 *
	 * @param aCallId The Call ID of the call to terminate.
     *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMmCustomAPI::TerminateCall()
	 */
	virtual TInt HandleTerminateErrorCallL(TInt aCallId) = 0;

	}; // class MLtsyDispatchCallControlTerminateErrorCall


class MLtsyDispatchCallControlTerminateAllCalls : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlTerminateAllCallsApiId = KDispatchCallControlFuncUnitId + 26;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomTerminateCallIPC
	 * request from the CTSY with an invalid call id.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlTerminateAllCallsComp()
	 *
	 * Implementation of this interface should terminate all the active, connecting and held calls.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMmCustomAPI::TerminateCall()
	 */
	virtual TInt HandleTerminateAllCallsL() = 0;

	}; // class MLtsyDispatchCallControlTerminateAllCalls


class MLtsyDispatchCallControlGetCallForwardingIndicator : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlGetCallForwardingIndicatorApiId = KDispatchCallControlFuncUnitId + 27;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetIccCallForwardingStatusIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking one of the 
	 * CCtsyDispatcherCallback::CallbackCallControlGetCallForwardingIndicatorComp()
	 *
	 * Implementation of this interface should return the call forwarding indicator from the network.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMmCustomAPI::GetIccCallForwardingIndicatorStatus()
	 */
	virtual TInt HandleGetCallForwardingIndicatorL() = 0;

	}; // class MLtsyDispatchCallControlGetCallForwardingIndicator

class MLtsyDispatchCallControlUpdateLifeTimer : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchCallControlUpdateLifeTimerApiId = KDispatchCallControlFuncUnitId + 28;
public:

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the
	 * ECtsyUpdateLifeTimeReq request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlUpdateLifeTimerComp()
	 *
	 * Implementation of this interface can accumulate the number of seconds that
	 * have elapsed since a call started. By initialising a variable in the LTSY when a 
	 * call is dialed, then adding the value contained in aDuration to this variable 
	 * it is possible to track the duration of a call.
	 * 
	 * HandleUpdateLifeTimerReqL is usually invoked every 10 seconds.
	 *
	 * @param aDuration The number of seconds that have elapsed since the last time
	 * this interface was invoked (or since the start of the call if the first invocation).
	 *
	 * @return KErrNone if the LTSY was successful; KErrNotSupported if the LTSY does not
	 * support handling of this request or another error code indicating the
	 * failure otherwise.
	 */
	virtual TInt HandleUpdateLifeTimerReqL(TUint32 aDuration) = 0;

	}; // class MLtsyDispatchCallControlUpdateLifeTimer

	
// Note: A static constant has been defined in MLtsyDispatchCallControlSwap (abbove) with the highest Id 
// in this file... "KLtsyDispatchCallControlSingleSwapApiId = KDispatchCallControlFuncUnitId + 29"
// If adding a new Id it must be greater than KDispatchCallControlFuncUnitId + 29.

#endif /*MLTSYDISPATCHCALLCONTROLINTERFACE_H_*/
