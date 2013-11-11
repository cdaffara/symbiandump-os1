// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cctsydispatchercallbackTraces.h"
#endif

#include <etelsat.h>
#include <in_sock.h>
#include "ccallcontroldispatcher.h"
#include "ccallcontrolmultipartydispatcher.h"
#include "ccellbroadcastdispatcher.h"
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "cpacketservicesdispatcher.h"
#include "cphonedispatcher.h"
#include "cphonebookdispatcher.h"
#include "cphonebookendispatcher.h"
#include "cphonebookondispatcher.h"
#include "csatdispatcher.h"
#include "csecuritydispatcher.h"
#include "csimdispatcher.h"
#include "csmsdispatcher.h"
#include "csupplementaryservicesdispatcher.h"
#include "ctsydispatcherpanic.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "tdispatcherholder.h"

CCtsyDispatcherCallback::CCtsyDispatcherCallback()
	{
	}

CCtsyDispatcherCallback::~CCtsyDispatcherCallback()
	{
	}

CCtsyDispatcherCallback* CCtsyDispatcherCallback::NewLC()
	{
	CCtsyDispatcherCallback* self = new (ELeave) CCtsyDispatcherCallback();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCtsyDispatcherCallback* CCtsyDispatcherCallback::NewL()
	{
	CCtsyDispatcherCallback* self=CCtsyDispatcherCallback::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

void CCtsyDispatcherCallback::ConstructL()
	{
	}

void CCtsyDispatcherCallback::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.
 *
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;

	iDispatcherHolder = &aDispatcherHolder;
	} // CCtsyDispatcherCallback::SetDispatcherHolder

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlNotifyCallStatusChangeInd(
	TInt aCauseCode, TInt aCallId, RMobileCall::TMobileCallStatus aMobileCallStatus)
/**
 * Used by an LTSY to indicate to the CTSY that there has been a change in the
 * status of a call. For example, if the CTSY has previously made a dial request
 * to make a voice call, the LTSY can call this API to notify the CTSY
 * of progress of the call through the dialling, connecting and connected states.
 *
 * The aCauseCode parameter should be used to indicate the reason for the change
 * in status of a call.
 *
 * In the case of a call going idle (aMobileCallStatus = RMobileCall::EStatusIdle)
 * specific cause codes need to be returned to the Common TSY to indicate
 * specific causes of status changes:
 *
 *  - To indicate that an active call has been released normally by either party,
 *  set aCauseCode = KErrGsmCCNormalCallClearing.
 *
 *  - To indicate that a mobile originated call that is being dialled has gone
 * idle because the remote party has rejected the call, set
 * aCauseCode = KErrGsmCCCallRejected or another error code (which is != KErrNone)
 * to indicate the reason for rejection.
 *
 * - If a mobile originated call is being dialled and the user cancels dialling,
 * causing the call to go idle, set aCauseCode = KErrGsmCCNormalCallClearing.
 *
 * - When an incoming call goes idle because the client has rejected it
 * (done using RCall::HangUp() on the ringing call), set
 * aCauseCode = KErrGsmCCUserBusy.
 *
 * @param aCauseCode The cause of the change in call status.
 *
 * @param aCallId The Call ID of the call whose status has changed.
 *
 * @param aMobileCallStatus The new status of the call refered to by aCallId.
 *
 * @see RMobileCall::NotifyMobileCallStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aCauseCode=%d, aCallId=%d, aMobileCallStatus=%d"), aCauseCode, aCallId, aMobileCallStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackNotifyCallStatusChange(aCauseCode, aCallId, aMobileCallStatus);

	} // CCtsyDispatcherCallback::CallbackCallControlNotifyCallStatusChangeInd


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlReceiveUuiInd(
	TInt aError, TInt aCallId, const TDesC& aCallUui)
/**
 * Indicates to the CTSY that the LTSY has received a User-to-User
 * Information notification. The User-to-User Supplementary Service
 * is detailed in 3GPP TS 24.087.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCallId The Call ID of the call to which the information is being sent.
 * @param aCallUui The User-to-User Information that has been received.
 * This should be a descriptor of length RMobileCall::KMaxUUISize
 *
 * @see RMobileCall:ReceiveUUI()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackReceiveUui(aError, aCallId, aCallUui);

	} // CCtsyDispatcherCallback::CallbackCallControlReceiveUuiInd


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlNotifyDataCallCapsChangeInd(
	TInt aError, TInt aCallId, const RMobileCall::TMobileCallDataCapsV1& aMobileCallDataCaps)
/**
 * Sends a notification to the Common TSY about a change in the data call
 * capabilities.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @param aCallId The Call ID of the call whose capabilities have changed.
 *
 * @param aMobileCallDataCaps The new capabilities of the data call.
 *
 * @see RMobileCall::NotifyMobileDataCallCapsChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackNotifyDataCallCapsChange(aError, aCallId, aMobileCallDataCaps);

	} // CCtsyDispatcherCallback::CallbackCallControlNotifyDataCallCapsChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlNotifyIncomingCallInd(
	TInt aError, const RMobileCall::TMobileCallInfoV1& aCallInfo)
/**
 * Notify the Common TSY about an incoming call to the phone.
 *
 * The aCallInfo parameter is used to provide the Common TSY with details about
 * the incoming call.
 *
 * The field RMobileCall::TMobileCallInfoV1::iCallId is mandatory and should be
 * populated with a unique Call ID allocated by the LTSY. For more details about
 * how to set a valid Call ID, see the documentation on the aCallId parameter
 * in CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp().
 *
 * The field RMobileCall::TMobileCallInfoV1::iService in the aCallInfo parameter
 * is mandatory
 * must be populated by the LTSY to indicate to the Common TSY the type of the
 * incoming call.
 *
 * 	- If there is an incoming voice call on the primary voice line,
 * this field should be set to RMobilePhone::EVoiceService.
 *
 *  - If the incoming voice call is on the auxiliary
 * voice line, this parameter should be set to RMobilePhone::EAuxVoiceService.
 *
 *  - If the incoming call is a data call, this parameter should be set to
 * RMobilePhone::ECircuitDataService.
 *
 * The remainder of the fields in the aCallInfo parameter can be used to
 * indicate other details about the incoming call. See the documentation for
 * RMobileCall::TMobileCallInfoV1 for more details.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCallInfo Information about the incoming call.
 *
 * @see RLine::NotifyIncomingCall()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallInfo.iService=%d, aCallInfo.iCallId=%d"), aError, aCallInfo.iService, aCallInfo.iCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackNotifyIncomingCall(aError, aCallInfo);

	} // CCtsyDispatcherCallback::CallbackCallControlNotifyIncomingCallInd

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlNotifyHscsdInfoChangeInd(
	TInt aError, TInt aCallId, const RMobileCall::TMobileCallHscsdInfoV8& aCallParams,
	const RMobileCall::TMobileCallDataCapsV1& aCallCaps)
/**
 * Notify the Common TSY about a change in the parameters of other information
 * about the specified HSCSD call.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @param aCallId The Call ID of the data call whose parameters / information has changed.
 *
 * @param aCallParams The new call parameters.  The fields of this
 * RMobileCall::TMobileCallHscsdInfoV8 parameter are returned to the client if
 * the client has requested to be notified of a change in the HSCSD information
 * of the call via RMobileCall::NotifyHscsdInfoChange()
 *
 * @param aCallCaps The new call capabilities.  The fields of this
 * RMobileCall::TMobileCallDataCapsV1 parameter are returned to the client if
 * the client has requested to be notified of a change in the HSCSD capabilities
 * of the call via RMobileCall::NotifyMobileDataCallCapsChange() if any of the
 * following fields have changed their value:
 * RMobileCall::TMobileCallDataCapsV1::iMaxRxTimeSlots ,
 * RMobileCall::TMobileCallDataCapsV1::iMaxTxTimeSlots ,
 * RMobileCall::TMobileCallDataCapsV1::iHscsdSupport ,
 * RMobileCall::TMobileCallDataCapsV1::iSpeedCaps ,
 *
 * @see RMobileCall::NotifyMobileDataCallCapsChange()
 * @see RMobileCall::NotifyHscsdInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackNotifyHscsdInfoChange(aError,
			aCallId, aCallParams, aCallCaps);

	} // CCtsyDispatcherCallback::CallbackCallControlNotifyHscsdInfoChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlNotifyCallEventInd(
	TInt aError, TInt aCallId, RMobileCall::TMobileCallEvent aCallEvent)
/**
 * Notify the Common TSY about a remote call event that has occurred, for example,
 * the remote party has put the call on hold.
 *
 * @param aError The error code associated with this notification.
 * KErrNone if there is no error.
 *
 * @param aCallId The Call ID of the call that this call event relates to.
 *
 * @param aCallEvent The event that has occurred to the call specified in aCallId.
 * Current valid values supported by the Common TSY are RMobileCall::ERemoteHold,
 * RMobileCall::ERemoteResume, RMobileCall::ERemoteTerminated and
 * RMobileCall::ERemoteConferenceCreate
 *
 * @see RMobileCall::NotifyCallEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d, aCallEvent=%d"), aError, aCallId, aCallEvent);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackNotifyCallEvent(aError, aCallId, aCallEvent);

	} // CCtsyDispatcherCallback::CallbackCallControlNotifyCallEventInd


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlNotifyCallInfoChangeInd(
	TInt aError, const RMobileCall::TMobileCallInfoV1& aMobileCallInfo)
/**
 * This function should be used by the LTSY to supply information about a call
 * to the Common TSY.  For example, details about the remote party and whether
 * the call has been forwarded. See RMobileCall::TMobileCallInfoV1 for more
 * details on what information about the call can be returned to the CTSY.
 *
 * In aMobileCallInfo, RMobileCall::TMobileCallInfoV1::iCallId must be populated
 * with the Call ID of the call the aMobileCallInfo parameter is referring to.
 *
 * @param aError The error code associated with this notification.
 * KErrNone if there is no error.
 *
 * @param aMobileCallInfo The call information relating to the call identified
 * by aCallId.
 *
 * @see RMobileCall::GetMobileCallInfo()
 * @see RMobileCall::NotifyRemotePartyInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, callId=%d, callMode=%d"), aError, aMobileCallInfo.iCallId, aMobileCallInfo.iService);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackNotifyCallInfoChange(aError, aMobileCallInfo);

	} // CCtsyDispatcherCallback::CallbackCallControlNotifyCallInfoChangeInd


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlRemoteAlertingToneChangeInd(
	TInt aError, RMmCustomAPI::TRemoteAlertingToneStatus aRemoteAlertingTone)
/**
 * This function should be used by the LTSY to indicate to the CTSY that the remote alerting tone had
 * been set or changed.
 *
 * @param aError The error code associated with this notification.
 * KErrNone if there is no error.
 *
 * @param aRemoteAlertingTone The new remote alerting status (either RMmCustomAPI::EUiRbtTone
 *                            when it is a ringback tone or RMmCustomAPI::EUiStopTone when it's a stop tone)
 *
 * @see RMmCustomAPI::GetRemoteAlertingToneStatus()
 * @see RMmCustomAPI::NotifyRemoteAlertingToneStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRemoteAlertingTone=%d"), aError, aRemoteAlertingTone);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackRemoteAlertingToneChange(aError, aRemoteAlertingTone);

	} // CCtsyDispatcherCallback::CallbackCallControlRemoteAlertingToneChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlAnswerComp(TInt aError, TInt aCallId)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlAnswer::HandleAnswerReqL()
 *
 * Used to indicate to the Common TSY the outcome of the request to answer an
 * incoming call.
 *
 * @param aError The error code to be returned to the CTSY by the LTSY
 * indicating the outcome of the attempt to answer the requested call.  This should
 * be KErrNone if the dialling process could be started successfully.  Otherwise
 * another error code to indicate the failure should be returned.
 *
 * @param aCallId The Call ID which could not be answered.
 *
 * @see RCall::AnswerIncomingCall()
 * @see RCall::AnswerIncomingCallISV()
 * @see CTelephony::AnswerIncomingCall()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackAnswer(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlAnswerComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlHoldComp(
	TInt aError, TInt aCallId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlHold::HandleHoldReqL()
 *
 * The result of the hold request on the call specified in aCallId is passed back
 * to the Common TSY.
 *
 * @param aError The error code to be returned to the CTSY by the LTSY
 * indicating the outcome of the attempt to hold the requested call.  This should
 * be KErrNone if the hold process could be started successfully.  Otherwise
 * another error code to indicate the failure should be returned.
 *
 * @param aCallId The Call ID of the call which was requested to be held.
 *
 * @see RMobileCall::Hold()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackHold(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlHoldComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlDialEmergencyComp(
	TInt aError, TInt aCallId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlDialEmergency::HandleDialEmergencyReqL()
 *
 * This callback is used to inform the CTSY of the outcome of the request to
 * initiate the dialling process to dial an emergency call.
 *
 * This callback is similar to CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp()
 * but refers specifically to the dialling of an emergency call.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCallId The call ID of the call that the dial attempt refers to which
 * has been allocated by the LTSY. The call ID needs to be unique across all
 * calls and modes(emergency and non-emergency included). See the documentation for
 * CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp for precise details
 * on how to allocate a valid call ID.
 *
 * @see RMobileCall::DialEmergencyCall()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackDialEmergency(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlDialEmergencyComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlStopDtmfToneComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlStopDtmfTone::HandleStopDtmfToneReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::StopDTMFTone()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackStopDtmfTone(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlStopDtmfToneComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlSetActiveAlsLineComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlSetActiveAlsLine::HandleSetActiveAlsLineReqL()
 *
 * This callback indicates the outcome of the request to set the active ALS line.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetALSLine()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackSetActiveAlsLine(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlSetActiveAlsLineComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesCancelComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlSendDtmfTonesCancel::HandleSendDtmfTonesCancelReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SendDTMFTones()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackSendDtmfTonesCancel(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesCancelComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlHangUpComp(
	TInt aError, TInt aCallId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlHangUp::HandleHangUpReqL()
 *
 * @param aError The error code to return to the CTSY relating to the request to
 * hang up.  This should be KErrNone on success or another error code to indicate
 * the failure otherwise.
 *
 * @param aCallId The Call ID of the call which has just been hung up.
 *
 * @see RCall::HangUp()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackHangUp(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlHangUpComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlResumeComp(
	TInt aError, TInt aCallId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlResume::HandleResumeReqL()
 *
 * The result of the pending resume request on the call specified in aCallId is
 * passed back to the Common TSY.
 *
 * @param aError The error code to be returned to the CTSY by the LTSY
 * indicating the outcome of the attempt to resume the held call.  This should
 * be KErrNone if the resume process could be started successfully.  Otherwise
 * another error code to indicate the failure should be returned.
 *
 * @param aCallId The Call ID of the call which has been requested to be resumed.
 *
 * @see RMobileCall::Resume()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackResume(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlResumeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlSetDynamicHscsdParamsComp(
	TInt aError, TInt aCallId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlSetDynamicHscsdParams::HandleSetDynamicHscsdParamsReqL()
 *
 * This callback indicates that the request to set the dynamic HSCSD parameters
 * has been made (successfully or unsuccessfully). To notify the CTSY of a change
 * in the HSCSD information of the call, see the API
 * CCtsyDispatcherCallback::CallbackCallControlNotifyHscsdInfoChangeInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileCall::SetDynamicHscsdParams()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackSetDynamicHscsdParams(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlSetDynamicHscsdParamsComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp(
	TInt aError, TInt aCallId)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlDialVoice::HandleDialVoiceReqL()
 *
 * This callback is used to inform the CTSY of the outcome of the request to
 * initiate the dialling process. To indicate progress of the call as it becomes
 * connected, use
 * CCtsyDispatcherCallback::CallbackCallControlNotifyCallStatusChangeInd().
 *
 * When a dial attempt is made via the above mentioned interface, the LTSY has the
 * responsibility of allocating a unique Call ID to the call.  For more
 * information, see the documentation on aCallId below.
 *
 * @param aError The error code to be returned to the CTSY by the LTSY
 * indicating the outcome of the attempt to dial the requested call.  This should
 * be KErrNone if the dialling process could be started successfully.  Otherwise
 * another error code to indicate the failure should be returned.
 *
 * @param aCallId The call ID of the call that the dial attempt refers to which
 * has been allocated by the LTSY.  This should be a number between 1 and
 * KMaxCallIdValue and should be unique across all call modes (voice, data etc.
 * including emergency calls). For example, if a request for a voice call is made
 * and the LTSY allocates
 * this call an ID of 1 and a request for a data call is requested, the LTSY
 * should allocate a Call ID which is not the same as that already allocated for
 * the first voice call.
 *
 * @see RCall::Dial()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackDialVoice(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlTransferComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlTransfer::HandleTransferReqL()
 *
 * Informs the CTSY of the outcome of initiating the transfer process.
 *
 * Successful completion of the transfer process is indicated by a change
 * in the call status of the calls which were transferred.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileCall::Transfer()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackTransfer(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlTransferComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesComp(
		TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlSendDtmfTones::HandleSendDtmfTonesReqL()
 *
 * This callback indicates that the DTMF string has been sent.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SendDTMFTones()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackSendDtmfTones(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetIdentityServiceStatusComp(
	TInt aError, RMobilePhone::TMobilePhoneIdServiceStatus aIdentityServiceStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlGetIdentityServiceStatus::HandleGetIdentityServiceStatusReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @param aIdentityServiceStatus The status of the service requested in
 * MLtsyDispatchCallControlGetIdentityServiceStatus::HandleGetIdentityServiceStatusReqL()
 *
 * @see RMobilePhone::GetIdentityServiceStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIdentityServiceStatus=%d"), aError, aIdentityServiceStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetIdentityServiceStatus(aError, aIdentityServiceStatus);

	} // CCtsyDispatcherCallback::CallbackCallControlGetIdentityServiceStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlSwapComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlSwap::HandleSwapReqL()
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher by the LTSY
 * indicating the outcome of the attempt to swap the requested calls.  This should
 * be KErrNone if the swap process could be started successfully.  Otherwise
 * another error code to indicate the failure should be returned.
 *
 * @see RMobileCall::Swap()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackSwap(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlSwapComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlStartDtmfToneComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlStartDtmfTone::HandleStartDtmfToneReqL()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher by the LTSY
 * indicating the outcome of the request to send the DTMF tone.
 *
 * @see RMobilePhone::StartDTMFTone()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackStartDtmfTone(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlStartDtmfToneComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlNotifyIccCallForwardingStatusChangeInd(
	TInt aError, const RMobilePhone::TMobileAddress& aCallForwardingNo,
	RMobilePhone::TCFUIndicatorStatusFlags aCallForwardingStatusFlags,
	RMmCustomAPI::TMultipleSubscriberProfileID aMultipleSubscriberProfileId
	)
/**
 * Callback function to be used to indicate a change in ICC Call Forwarding.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallForwardingNo the number that is being forwarded.
 * @param aCallForwardingStatusFlags flags defined in RMobilePhone::TCFUIndicatorStatus.
 * @param aMultipleSubscriberProfileId in case of Multiple Subscriber (SIM) the Profile ID.
 * @see RMmCustomApi::NotifyIccCallForwardingStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aCFSFlags=%x,aMSPId=%d"), 
			aError,aCallForwardingStatusFlags,static_cast<TUint32>(aMultipleSubscriberProfileId));

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackNotifyIccCallForwardingStatusChange(aError, 
			aCallForwardingNo, aCallForwardingStatusFlags, aMultipleSubscriberProfileId);

	} // CCtsyDispatcherCallback::CallbackCallControlNotifyIccCallForwardingStatusChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetFdnStatusComp(
		TInt aError,
		RMobilePhone::TMobilePhoneFdnStatus aFdnStatus)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetFdnStatus::HandleGetFdnStatusReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aFdnStatus The FDN status to be returned to the CTSY Dispatcher.
 *
 * @see RMobilePhone::GetFdnStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aFdnStatus=%d"), aError, aFdnStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetFdnStatus(aError, aFdnStatus);

	} // CCtsyDispatcherCallback::CallbackPhoneGetFdnStatusComp


EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetNetworkRegistrationStatusComp(
	TInt aError, RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetNetworkRegistrationStatus::HandleGetNetworkRegistrationStatusReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aRegStatus Current network registration status.
 *
 * @see RMobilePhone::GetNetworkRegistrationStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRegStatus=%d"), aError, aRegStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetNetworkRegistrationStatus(aError, aRegStatus);

	} // CCtsyDispatcherCallback::CallbackPhoneGetNetworkRegistrationStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatUssdControlSupportedChangeInd(
	TInt aError, TBool aSupported)
/**
 *
 * This indicator is used by the LTSY to inform the CTSY as to whether USSD TLV
 * are supported by the NAA for control.
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSupported ETrue if USSD control TLV is supported, EFalse otherwise.
 * @see RSat::NotifyCallControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSupported=%d"), aError, aSupported);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackUssdControlSupported(aError, aSupported);

	} // CCtsyDispatcherCallback::CallbackSatUssdControlSupportedChangeInd



EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneBootNotifyModemStatusReadyComp(TInt aError, TRfStateInfo aRfStatus)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneBootNotifyModemStatusReady::HandleBootNotifyModemStatusReadyReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aRfStatus The current RF status. See TRfStateInfo.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRfStatus=%d"), aError, aRfStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackBootNotifyModemStatusReady(aError, aRfStatus);

	} // CCtsyDispatcherCallback::CallbackPhoneBootNotifyModemStatusReady


EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneBootNotifySimStatusReadyComp(TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneBootNotifySimStatusReady::HandleBootNotifySimStatusReadyReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackBootNotifySimStatusReady(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneBootNotifySimStatusReadyComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetHomeNetworkComp(TInt aError,
		const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetHomeNetwork::HandleGetHomeNetworkReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNetworkInfo Contains home network information.
 * request.
 *
 * @see RMobilePhone::GetHomeNetwork()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetHomeNetwork(aError, aNetworkInfo);

	} // CCtsyDispatcherCallback::CallbackPhoneGetHomeNetworkComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetActiveAlsLineComp(
	TInt aError, RMobilePhone::TMobilePhoneALSLine aAlsLine)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlGetActiveAlsLine::HandleGetActiveAlsLineReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aAlsLine The current active ALS line.
 *
 * @see RMobilePhone::GetALSLine()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAlsLine=%d"), aError, aAlsLine);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetActiveAlsLine(aError, aAlsLine);

	} // CCtsyDispatcherCallback::CallbackPhoneGetActiveAlsLineComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetIccMessageWaitingIndicatorsComp(
	TInt aError, const RMobilePhone::TMobilePhoneMessageWaitingV1& aMessageWaiting)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetIccMessageWaitingIndicators::HandleGetIccMessageWaitingIndicatorsReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aMessageWaiting The message waiting indicators to return.
 *
 * @see RMobilePhone::GetIccMessageWaitingIndicators()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetIccMessageWaitingIndicators(aError, aMessageWaiting);

	} // CCtsyDispatcherCallback::CallbackPhoneGetIccMessageWaitingIndicatorsComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetServiceTableComp(
	TInt aError, const RMobilePhone::TMobilePhoneServiceTableV1& aServiceTable)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetServiceTable::HandleGetServiceTableReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aServiceTable The service table to return.
 *
 * @see RMobilePhone::GetServiceTable()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetServiceTable(aError, aServiceTable);

	} // CCtsyDispatcherCallback::CallbackSimGetServiceTable

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkModeChangeInd(
	TInt aError,
	RMobilePhone::TMobilePhoneNetworkMode aNetworkMode)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate changes to the current network mode
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNetworkMode The network mode to be sent to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aNetworkMode=%d"), aError, aNetworkMode);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyNetworkModeChange(aError, aNetworkMode);

	} // CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkModeChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyEgprsInfoChangeInd(
	TInt aError,
	TBool aEdgeSupported)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate an EGPRS/GPRS information change.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aEdgeSupported If EGPRS is supported on the current cell.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aEdgeSupported=%d"), aError, aEdgeSupported);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyEgprsInfoChange(aError, aEdgeSupported);

	} // CCtsyDispatcherCallback::CallbackPhoneNotifyEgprsInfoChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifySignalStrengthChangeInd(
	TInt aError,
	TInt32 aSignalStrength,
	TInt8 aBar)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate change to signal strength.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSignalStrength The signal strength to be sent to the CTSY Dispatcher
 * @param aBar The number of bars to display.
 *
 * @see RMobilePhone::NotifySignalStrengthChange()
 * @see RMobilePhone::GetSignalStrength()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSignalStrength=%d, aBar=%d"), aError, aSignalStrength, aBar);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifySignalStrengthChange(aError, aSignalStrength, aBar);

	} // CCtsyDispatcherCallback::CallbackPhoneNotifySignalStrengthChangeInd


EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkRegistrationStatusChangeInd(
	TInt aError,
	RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus)
/**
 * Callback function to be used by the Licensee LTSY to indicate a change in the
 * network registration status.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aRegStatus Current network registration status.
 *
 * @see RMobilePhone::NotifyNetworkRegistrationStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRegStatus=%d"), aError, aRegStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyNetworkRegistrationStatusChange(aError, aRegStatus);

	} // CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkRegistrationStatusChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyNitzInfoChangeInd(
	TInt aError,
	const RMobilePhone::TMobilePhoneNITZ& aNitzInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate a change in the current
 * Network Information and Time Zone (NITZ).
 *
 *  - The RMobilePhone::TMobilePhoneNITZ::iNitzFieldsUsed field in the
 *  aNitzInfo parameter should be used to reflect which of the data fields are present.
 *  The fields RMobilePhone::TMobilePhoneNITZ::iShortNetworkId and
 *  RMobilePhone::TMobilePhoneNITZ::iLongNetworkId may be left
 *  blank if not known at this time.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @param aNitzInfo The current NITZ information to be sent to the CTSY Dispatcher.
 *
 * @see RMobilePhone::NotifyNITZInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyNitzInfoChange(aError, aNitzInfo);

	} // CCtsyDispatcherCallback::CallbackPhoneNotifyNitzInfoChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyBatteryInfoChangeInd(
	TInt aError,
	const RMobilePhone::TMobilePhoneBatteryInfoV1& aBatteryInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate any changes
 * in the battery information.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aBatteryInfo The battery information to be sent to the CTSY Dispatcher.
 *
 * @see RMobilePhone::GetBatteryInfo()
 * @see RMobilePhone::NotifyBatteryInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyBatteryInfoChange(aError, aBatteryInfo);

	} // CCtsyDispatcherCallback::CallbackPhoneNotifyBatteryInfoChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyCurrentNetworkChangeInd(
	TInt aError,
	const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo,
	const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate any changes in the current network.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNetworkInfo The network information to be sent to the CTSY Dispatcher.
 * @param aLocationArea The mobile phone location area
 *
 * @see RMobilePhone::NotifyCurrentNetworkChange()
 * @see RMobilePhone::GetCurrentNetwork()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyCurrentNetworkChange(aError, aNetworkInfo, aLocationArea);

	} // CCtsyDispatcherCallback::CallbackPhoneNotifyCurrentNetworkChangeInd


EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetCurrentNetworkInfoComp(
	TInt aError,
	const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo,
	const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetCurrentNetworkInfo::HandleGetCurrentNetworkInfoReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNetworkInfo The network information to be returned to the CTSY Dispatcher.
 * @param aLocationArea The mobile phone location area.
 *
 * @see RMobilePhone::NotifyCurrentNetworkChange()
 * @see RMobilePhone::GetCurrentNetwork()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetCurrentNetworkInfo(aError, aNetworkInfo, aLocationArea);

	} // CCtsyDispatcherCallback::CallbackPhoneGetCurrentNetworkInfoComp


EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetNetworkModeComp(
	TInt aError,
	RMobilePhone::TMobilePhoneNetworkMode aNetworkMode)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetNetworkMode::HandleGetNetworkModeReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNetworkMode The network mode to be sent to the CTSY Dispatcher.
 *
 * @see RMobilePhone::GetCurrentMode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aNetworkMode=%d"), aError, aNetworkMode);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetNetworkMode(aError, aNetworkMode);

	} // CCtsyDispatcherCallback::CallbackPhoneGetNetworkModeComp


EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetNitzInfoComp(
	TInt aError,
	const RMobilePhone::TMobilePhoneNITZ& aNitzInfo)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetNetworkMode::HandleGetNitzInfoReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNitzInfo The NITZ information to be sent to the CTSY Dispatcher.
 *
 * @see RMobilePhone::GetNITZInfo()
 * @see RMobilePhone::NotifyNITZInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetNitzInfo(aError, aNitzInfo);

	} // CCtsyDispatcherCallback::CallbackPhoneGetNitzInfoComp


EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetSignalStrengthComp(
	TInt aError,
	TInt32 aSignalStrength,
	TInt8 aBar)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetSignalStrength::HandleGetSignalStrengthReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSignalStrength The signal strength to be sent to the CTSY Dispatcher.
 * @param aBar The number of bars to display.
 *
 * @see RMobilePhone::GetSignalStrength()
 * @see RMobilePhone::NotifySignalStrengthChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSignalStrength=%d, aBar=%d"), aError, aSignalStrength, aBar);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetSignalStrength(aError, aSignalStrength, aBar);

	} // CCtsyDispatcherCallback::CallbackPhoneGetSignalStrengthComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetBatteryInfoComp(
	TInt aError,
	const RMobilePhone::TMobilePhoneBatteryInfoV1& aBatteryInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetBatteryInfo::HandleGetBatteryInfoReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aBatteryInfo The battery information to be sent to the CTSY Dispatcher.
 *
 * @see RMobilePhone::GetBatteryInfo()
 * @see RMobilePhone::NotifyBatteryInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetBatteryInfo(aError, aBatteryInfo);

	} // CCtsyDispatcherCallback::CallbackPhoneGetBatteryInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNspsWakeupComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneNspsWakeup::HandleNspsWakeupReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::NetWakeup()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNspsWakeup(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneNspsWakeupComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkModeComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneSetSystemNetworkMode::HandleSetSystemNetworkModeReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::SetSystemNetworkMode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSetSystemNetworkMode(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkModeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetCurrentSystemNetworkModesComp(
	TInt aError, TUint32 aCurrentSystemNetworkModes)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetCurrentSystemNetworkModes::HandleGetCurrentSystemNetworkModesReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCurrentSystemNetworkModes A bit mask that represent the current
 *                                     network modes with values from RMmCustomAPI::TNetworkModeCaps.
 *
 * @see RMmCustomAPI::GetCurrentSystemNetworkModes()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCurrentSystemNetworkModes=%d"), aError, aCurrentSystemNetworkModes);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetCurrentSystemNetworkModes(aError, aCurrentSystemNetworkModes);

	} // CCtsyDispatcherCallback::CallbackPhoneGetCurrentSystemNetworkModesComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneResetNetServerComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneResetNetServer::HandleResetNetServerReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::ResetNetServer()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackResetNetServer(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneResetNetServerComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSetAlwaysOnModeComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneSetAlwaysOnMode::HandleSetAlwaysOnModeReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::SetAlwaysOn()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSetAlwaysOnMode(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneSetAlwaysOnModeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSetDriveModeComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneSetDriveMode::HandleSetDriveModeReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::SetDriveMode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSetDriveMode(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneSetDriveModeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetServiceProviderNameComp(
	TInt aError, const RMobilePhone::TMobilePhoneServiceProviderNameV2& aServiceProvider)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetServiceProviderName::HandleGetServiceProviderNameReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aServiceProvider The service provider information.
 *
 * @see RMobilePhone::GetServiceProviderName()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetServiceProviderName(aError, aServiceProvider);

	} // CCtsyDispatcherCallback::CallbackPhoneGetServiceProviderNameComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetPhoneIdComp(
	TInt aError, const RMobilePhone::TMobilePhoneIdentityV1& aPhoneId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetPhoneId::HandleGetPhoneIdReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhoneId The mobile phone identity returned by the LTSY.
 *
 * @see RMobilePhone::GetPhoneId()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetPhoneId(aError, aPhoneId);

	} // CCtsyDispatcherCallback::CallbackPhoneGetPhoneIdComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksComp(
	TInt aError, const CMobilePhoneNetworkListV2& aPhoneNetworkList)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetDetectedNetworks::HandleGetDetectedNetworksReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhoneNetworkList A list of the detected networks
 *
 * @see CRetrieveMobilePhoneDetectedNetworks::StartV2()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetDetectedNetworks(aError,aPhoneNetworkList);

	} // CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksCancelComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetDetectedNetworksCancel::HandleGetDetectedNetworksCancelReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see CRetrieveMobilePhoneDetectedNetworks::StartV2()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetDetectedNetworksCancel(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksCancelComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSelectNetworkComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneSelectNetwork::HandleSelectNetworkReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SelectNetwork()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSelectNetwork(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneSelectNetworkComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSelectNetworkCancelComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneSelectNetworkCancel::HandleSelectNetworkCancelReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SelectNetwork()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSelectNetworkCancel(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneSelectNetworkCancelComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSetNetworkSelectionSettingComp(
		TInt aError, RMobilePhone::TMobilePhoneSelectionMethod aSelectionMethod)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneSetNetworkSelectionSetting::HandleSetNetworkSelectionSettingReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSelectionMethod The new network selection method.
 *
 * @see RMobilePhone::SetNetworkSelectionSetting()
 * @see RMobilePhone::NotifyNetworkSelectionSettingChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSelectionMethod=%d"), aError, aSelectionMethod);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSetNetworkSelectionSetting(aError,aSelectionMethod);

	} // CCtsyDispatcherCallback::CallbackPhoneSetNetworkSelectionSettingComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSimRefreshRegisterComp(TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneSimRefreshRegister::HandleSimRefreshRegisterReqL()
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.

 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSimRefreshRegister(aError);
	} // CCtsyDispatcherCallback::CallbackPhoneSimRefreshRegisterComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyNspsStatusChangeInd(TInt aError, TBool aEnabled)
/**
 * Callback function to be used by the Licensee LTSY to indicate any changes
 * in the No Signal Power Save status.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aEnabled The No Signal Power Save status
 *
 * @see RMmCustomAPI::NotifyNSPSStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aEnabled=%d"), aError, aEnabled);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyNspsStatusChange(aError,aEnabled);
	} // CCtsyDispatcherCallback::CallbackPhoneNotifyNspsStatusChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkSelectionSettingChangeInd(TInt aError, RMobilePhone::TMobilePhoneSelectionMethod aSelectionMethod)
/**
 * Callback function to be used by the Licensee LTSY to indicate any changes
 * in the Network Selection.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSelectionMethod The new network selection method.
 *
 * @see RMobilePhone::NotifyNetworkSelectionSettingChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSelectionMethod=%d"), aError, aSelectionMethod);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyNetworkSelectionSettingChange(aError,aSelectionMethod);
	
	} // CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkSelectionSettingChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyRauEventInd(TInt aError, RMmCustomAPI::TRauEventStatus aRauEvent)
/**
 * Callback function to be used by the Licensee LTSY to indicate any RAU (Routing Area Update) event.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aRauEvent The RAU (Routing Area Update) event to be notified (Active or Resumed).
 *
 * @see RMmCustomAPI::NotifyRauEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aRauEvent=%d"), aError, aRauEvent);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyRauEvent(aError,aRauEvent);
	} // CCtsyDispatcherCallback::CallbackPhoneNotifyRauEventInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd(TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to indicate a change of the cell information to an unknown mode.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::NotifyCellInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyCellInfoChanged(aError);
	} // CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd(TInt aError, const RMmCustomAPI::TMmCellInfo::TGSMCellInfo& aInfo)
/**
 * Callback function to be used by the Licensee LTSY to indicate a change of the cell information when the cell is a GSM cell.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aInfo The new GSM cell information.
 *
 * @see RMmCustomAPI::NotifyCellInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyCellInfoChanged(aError, aInfo);
	} // CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd(TInt aError, const RMmCustomAPI::TMmCellInfo::TWCDMACellInfo& aInfo)
/**
 * Callback function to be used by the Licensee LTSY to indicate a change of the cell information when the cell is a WCDMA cell.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aInfo The new WCDMA cell information.
 *
 * @see RMmCustomAPI::NotifyCellInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyCellInfoChanged(aError, aInfo);
	} // CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkConnectionFailureInd(TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to indicate a network connection failure that
 * is so severe that the only way to recover is to restart the phone. 
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::NotifyNetworkConnectionFailure()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyNetworkConnectionFailure(aError);
	}

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyDtmfEventInd(TInt aError, RMmCustomAPI::TDtmfEventType aType,
                                                                       RMmCustomAPI::TDtmfEvent aEvent, TChar aTone)
/**
 * Callback function to be used by the Licensee LTSY to indicate a DTMF (Dual Tone Multiple Frequency) event.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The DTMF event type.
 * @param aEvent 	The DTMF event to be notified.
 * @param aTone The event tone.
 *
 * @see RMmCustomAPI::NotifyDtmfEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aEvent=%d, aTone=%d"), aError, aType, aEvent, (TInt)aTone);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyDtmfEvent(aError, aType, aEvent, aTone);
	}

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyDtmfEventInd(
											TInt aError, RMobilePhone::TMobilePhoneDTMFEvent aEventType)
/**
 * Callback function to be used by the Licensee LTSY to indicate a DTMF (Dual Tone Multiple Frequency) event.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aEventType The DTMF event type. Either EStartDtmfTone or EStopDtmfTone
 *
 * @see RMobilePhone::NotifyDtmfEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aEventType=%d"), aEventType);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyDtmfEvent(aError, aEventType);
	}

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetHspaStatusComp(
	TInt aError, RMmCustomAPI::THSxPAStatus aStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetHspaStatus::HandleGetHspaStatusReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatus The HSxPA status (enabled or disabled).
 *
 * @see RMmCustomAPI::ReadHSxPAStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d"), aError, aStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetHspaStatus(aError, aStatus);

	} // CCtsyDispatcherCallback::CallbackPhoneGetHspaStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSetHspaStatusComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneSetHspaStatus::HandleSetHspaStatusReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::WriteHSxPAStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSetHspaStatus(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneSetHspaStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyHspaStatusChangedInd(TInt aError, RMmCustomAPI::THSxPAStatus aStatus)
/**
 * Callback function to be used by the Licensee LTSY to indicate any change to the phone high speed channel status.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatus The new HSxPA status (enable or disable).
 *
 * @see RMmCustomAPI::NotifyHSxPAStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d"), aError, aStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyHspaStatusChanged(aError, aStatus);
	} // CCtsyDispatcherCallback::CallbackPhoneNotifyHspaStatusChangedInd


EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetNetworkProviderNameComp(
	TInt aError, const TDesC& aName)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetNetworkProviderName::HandleGetNetworkProviderNameReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aName The nmae of the current network provider.
 *
 * @see RMmCustomAPI::GetNetworkProviderName()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetNetworkProviderName(aError, aName);

	} // CCtsyDispatcherCallback::CallbackPhoneGetNetworkProviderNameComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetOperatorNameComp(
	TInt aError, RMmCustomAPI::TOperatorNameType aType, const TDesC& aName)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetOperatorName::HandleGetOperatorNameReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of the operator name.
 * @param aName The name of the current operator.
 *
 * @see RMmCustomAPI::GetOperatorName()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d"), aError, aType);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetOperatorName(aError, aType, aName);

	} // CCtsyDispatcherCallback::CallbackPhoneGetOperatorNameComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetCellInfo::HandleGetCellInfoReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::GetCellInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetCellInfo(aError);

	} // CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp(
	TInt aError, const RMmCustomAPI::TMmCellInfo::TGSMCellInfo& aInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetCellInfo::HandleGetCellInfoReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aInfo The GSM cell information.
 *
 * @see RMmCustomAPI::GetCellInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetCellInfo(aError,aInfo);

	} // CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp(
	TInt aError, const RMmCustomAPI::TMmCellInfo::TWCDMACellInfo& aInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetCellInfo::HandleGetCellInfoReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aInfo The WCDMA cell information.
 *
 * @see RMmCustomAPI::GetCellInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetCellInfo(aError,aInfo);

	} // CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneRegisterCellInfoChangeNotificationComp(TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneRegisterCellInfoChangeNotification::HandleRegisterCellInfoChangeNotificationReqL()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMobilePhone::NotifyCellInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackRegisterCellInfoChangeNotification(aError);
	} //CCtsyDispatcherCallback::CallbackPhoneRegisterCellInfoChangeNotificationComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetPhoneCellInfoComp(TInt aError, const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetPhoneCellInfo::HandleGetPhoneCellInfoReqL()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCellInfo The cell information to be cached in the CTSY and returned to the client
 * 
 * @see RMobilePhone::GetCellInfo()
 * @see RMobilePhone::NotifyCellInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetPhoneCellInfo(aError, aCellInfo);
	} //CCtsyDispatcherCallback::CallbackPhoneGetPhoneCellInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangeInd(TInt aError, const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo)
/**
 * Callback function to be used by the Licensee LTSY to indicate any changes
 * in the cell information.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCellInfo The cell information to be cached in the CTSY and returned to the client
 * 
 * @see RMobilePhone::NotifyCellInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
		
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	
	//this indicator is the same IPC as GetPhoneCellInfoComp, for interface consistency
	//the GetPhoneCellInfoReq should be completed with the GetPhoneCellInfoComp and unsolicited
	//cell info changes calls on this indicator.
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetPhoneCellInfo(aError, aCellInfo);
	} //CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetUsimServiceSupportComp(TInt aError, TInt aApplicationNumber, TBool aSupported)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetUsimServiceSupport::HandleGetGetUsimServiceSupportReqL()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aApplicationNumber The application number to which the support status is related to.
 * @param aSupported The service supported state (True / False)
 * 
 * @see RMmCustomAPI::GetUSIMServiceSupport()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aApplicationNumber=%d, aSupported=%d"), aError, aApplicationNumber, aSupported);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetUsimServiceSupport(aError, aApplicationNumber, aSupported);
	} //CCtsyDispatcherCallback::CallbackPhoneGetUsimServiceSupportComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetCurrentActiveUsimApplicationComp(TInt aError, const TDesC8& aAid)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneGetCurrentActiveUsimApplication::HandleGetCurrentActiveUsimApplicationReqL()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aAid The AID of the currently active USIM Application, this should be no longer than RMobilePhone::KAIDSize.
 * 
 * @see RMmCustomAPI::GetCurrentActiveUSimApplication()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAid=%S"), aError, &aAid);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetCurrentActiveUsimApplication(aError, aAid);
	} //CCtsyDispatcherCallback::CallbackPhoneGetCurrentActiveUsimApplicationComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneTerminateAllCallsComp(TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneTermianteAllCalls::HandleTerminateAllCallsReqL()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMobilePhone::TerminateAllCalls()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackTerminateAllCalls(aError);
	} //CCtsyDispatcherCallback::CallbackPhoneTerminateAllCallsComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneGetSystemNetworkBandComp(TInt aError, 
		RMmCustomAPI::TBandSelection aBand, RMmCustomAPI::TNetworkModeCaps aMode)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneTermianteAllCalls::HandleGetSystemNetworkBandReqL()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aBand the current network band
 * @param aMode the current network mode
 * 
 * @see RMmCustomAPI::GetSystemNetworkBand()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackGetSystemNetworkBand(aError, aBand, aMode);
	} //CCtsyDispatcherCallback::CallbackPhoneGetSystemNetworkBandComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkBandComp(TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhoneTermianteAllCalls::HandleSetSystemNetworkBandReqL()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMobilePhone::SetSystemNetworkBand()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackSetSystemNetworkBand(aError);
	} //CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkBandComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityGetPin1DisableSupportedComp(
	TInt aError, TBool aIsSupported)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityGetPin1DisableSupported::HandleGetPin1DisableSupportedReqL()
 * back to the CTSY Dispatcher.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIsSupported ETrue if PIN1 disable is supported, EFalse otherwise.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIsSupported=%d"), aError, aIsSupported);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackGetPin1DisableSupported(aError, aIsSupported);

	} // CCtsyDispatcherCallback::CallbackSecurityGetPin1DisableSupportedComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityBootNotifySecurityReadyInd(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate that the security functionality
 * is in a booted and ready state.
 * 
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackBootNotifySecurityReady(aError);

	} // CCtsyDispatcherCallback::CallbackSecurityBootNotifySecurityReadyInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityNotifySecurityEventInd(TInt aError, RMobilePhone::TMobilePhoneSecurityEvent aSecEvent)	
/**
 *
 * Completion routine to provide security event identifier back to the CTSY.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSecEvent Notify security event.
 *
 * @see RMobilePhone::NotifySecurityEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSecEvent=%d"), aError, aSecEvent);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackNotifySecurityEvent(aError, aSecEvent);

	} // CCtsyDispatcherCallback::CallbackSecurityNotifySecurityEventInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityNotifyLockInfoChangeInd(
	TInt aError, RMobilePhone::TMobilePhoneLockStatus aStatus, RMobilePhone::TMobilePhoneLockSetting aSetting, RMobilePhone::TMobilePhoneLock aLock)
/**
 *
 * Completion routine to provide the status of a phone lock and lock information back to the CTSY
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatus Status of the lock (unknown, locked, unlocked or blocked).
 * @param aSetting Lock setting (unknown, enabled, disabled or replaced).
 * @param aLock Lock type.
 *
 * @see RMobilePhone::NotifyLockInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d, aSetting=%d, aLock=%d"), aError, aStatus, aSetting, aLock);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackNotifyLockInfoChange(aError, aStatus, aSetting, aLock);

	} // CCtsyDispatcherCallback::CallbackSecurityNotifyLockInfoChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityGetSecurityCodeInfoComp(
	TInt aError, RMobilePhone::TMobilePhoneSecurityCode aSecCode, TInt aRemainingEntryAttempts)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityGetSecurityCodeInfo::HandleGetSecurityCodeInfoReqL()
 *
 * Completion routine to provide the current number or remaining entry attempts of a security code.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSecCode The security code whose information is to be retrieved.
 * @param aRemainingEntryAttempts Number of remaining security code entry attempts before the corresponding lock gets blocked.
 *
 * @see RMobilePhone::GetSecurityCodeInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSecCode=%d, aRemainingEntryAttempts=%d"), aError, aSecCode, aRemainingEntryAttempts);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackGetSecurityCodeInfo(aError, aSecCode, aRemainingEntryAttempts);

	} // CCtsyDispatcherCallback::CallbackSecurityGetSecurityCodeInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityGetLockInfoComp(
	TInt aError, RMobilePhone::TMobilePhoneLockStatus aLockStatus, RMobilePhone::TMobilePhoneLockSetting aLockSetting)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityGetLockInfo::HandleGetLockInfoReqL()
 *
 * Completion routine to provide lock setting and lock status
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aLockStatus Lock status (unknown, locked, unlocked or blocked).
 * @param aLockSetting Lock setting (unknown, enabled, disabled or replaced).
 *
 * @see RMobilePhone::GetLockInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aLockStatus=%d, aLockSetting=%d"), aError, aLockStatus, aLockSetting);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackGetLockInfo(aError, aLockStatus, aLockSetting);

	} // CCtsyDispatcherCallback::CallbackSecurityGetLockInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityAbortSecurityCodeComp(TInt aError)	
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityAbortSecurityCode::HandleAbortSecurityCodeReqL()
 *
 * Completion to the request: user has cancelled an outstanding "get security code" or "verify security code" request. 
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::AbortSecurityCode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackAbortSecurityCode(aError);

	} // CCtsyDispatcherCallback::CallbackSecurityAbortSecurityCodeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityGetCurrentActivePinComp(TInt aError, RMobilePhone::TMobilePhoneSecurityCode aSecCode)	
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityGetCurrentActivePin::HandleGetCurrentActivePinReqL()
 *
 * This callback updates the information of the currently active PIN.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSecCode Security code information for current active pin.
 *
 * @see CMmCustomSecurityTsy::SetActivePin()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSecCode=%d"), aError, aSecCode);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackGetCurrentActivePin(aError, aSecCode);

	} // CCtsyDispatcherCallback::CallbackSecurityGetCurrentActivePinComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecuritySetLockSettingComp(TInt aError)	
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecuritySetLockSetting::HandleSetLockSettingReqL()
 *
 * LTSY notifies about current values for lock status and lock setting.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetLockSetting()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackSetLockSetting(aError);

	} // CCtsyDispatcherCallback::CallbackSecuritySetLockSettingComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityVerifySecurityCodeComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityVerifySecurityCode::HandleVerifySecurityCodeReqL()
 *
 * Completion to the routine sending a security code requiring verification to the phone
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::VerifySecurityCode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackVerifySecurityCode(aError);

	} // CCtsyDispatcherCallback::CallbackSecurityVerifySecurityCodeComp
	
EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityNotifyCipheringInfoChangeInd(TInt aError, const RMmCustomAPI::TCipheringInfo& aInfo)	
/** 
 * Completes notify a client of ciphering info change
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aInfo Ciphering information data structure: ciphering indicator and ciphering status.
 * 
 * @see RMmCustomAPI::NotifyCipheringInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, indStatus=%d, ciphStatus=%d"), aError, aInfo.iIndStatus, aInfo.iCiphStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackNotifyCipheringInfoChange(aError, aInfo);

	} // CCtsyDispatcherCallback::CallbackSecurityNotifyCipheringInfoChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeComp(TInt aError)	
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityCheckSecurityCode::HandleCheckSecurityCodeReqL()
 * 
 * This function completes sending the check security code request
 * 
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMmCustomAPI::CheckSecurityCode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackCheckSecurityCode(aError);

	} // CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityDisablePhoneLockComp(TInt aError)	
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityDisablePhoneLock::HandleDisablePhoneLockReqL()
 * 
 * Completion of the routine, which disables phone lock.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMmCustomAPI::DisablePhoneLock()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackDisablePhoneLock(aError);

	} // CCtsyDispatcherCallback::CallbackSecurityDisablePhoneLockComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityGetCipheringInfoComp(TInt aError, TBool aIndStatus)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityGetCipheringInfo::HandleGetCipheringInfoReqL()
 * 
 * Completion routine to get ciphering info request.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aIndStatus Ciphering indicator status on/off.
 * 
 * @see RMmCustomAPI::GetCipheringInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndStatus=%d"), aError, aIndStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackGetCipheringInfo(aError, aIndStatus);

	} // CCtsyDispatcherCallback::CallbackSecurityGetCipheringInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityIsSecurityCodeBlockedComp(TInt aError, TBool aIsBlocked)	
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityIsSecurityCodeBlocked::HandleIsSecurityCodeBlockedReqL()
 * 
 * This function supports retrieving synchronously the information
 * if the security code is blocked or not.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aIsBlocked Is security code blocked.
 * 
 * @see RMmCustomAPI::IsBlocked()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackIsSecurityCodeBlocked(aError, aIsBlocked);

	} // CCtsyDispatcherCallback::CallbackSecurityIsSecurityCodeBlockedComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeCancelComp(TInt aError)	
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSecurityCheckSecurityCodeCancel::HandleCheckSecurityCodeCancelReqL()
 * 
 * Completion of the routine canceling an check security code request
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMmCustomAPI::CheckSecurityCode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSecurityDispatcher().CallbackCheckSecurityCodeCancel(aError);

	} // CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeCancelComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookEnStoreGetInfoComp(TInt aError, TInt aUsedEntries)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookEnStoreGetInfo::HandleStoreGetInfoReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aUsedEntries The number of used entries in the EN storage.
 *
 * @see RMobileENStore::GetInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aUsedEntries=%d"), aError, aUsedEntries);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookEnDispatcher().CallbackStoreGetInfo(aError, aUsedEntries);

	} // CCtsyDispatcherCallback::CallbackPhonebookEnStoreGetInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadAllComp(
		TInt aError, TInt aIndex, const TDesC& aNumber, TBool aMoreToCome)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookEnStoreReadAll::HandleStoreReadAllReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndex The index of the entry.
 * @param aNumber The number stored in the EN entry. The length of this should not be more than 6.
 * @param aMoreToCome ETrue if there are more entries to be read, False otherwise.
 *
 * @see CRetrieveMobilePhoneENList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d, aMoreToCome=%d"), aError, aIndex, aMoreToCome);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookEnDispatcher().CallbackStoreReadAll(aError, aIndex, aNumber, aMoreToCome);

	} // CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadAllComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadEntryComp(
	TInt aError, TInt aIndex, const TDesC& aNumber)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookEnStoreReadEntry::HandleStoreReadEntryReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndex The index of the entry.
 * @param aNumber The number stored in the EN entry. The length of this should not be more than 6.
 *
 * @see RMobileENStore::Read()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d"), aError, aIndex);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookEnDispatcher().CallbackStoreReadEntry(aError, aIndex, aNumber);

	} // CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCellBroadcastGsmBroadcastNotifyMessageReceivedInd(
	TInt aError, const TDesC8& aCbsMsg)
/**
 *
 * Function will be called by LTSY to pass GSM cell broadcast message. It may be used as a completion
 * of ReceiveMessage() request or by usolicited completion.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher
 * @param aCbsMsg GSM cell broadcast message
 *

 *
 * @see RMobileBroadcastMessaging::ReceiveMessage()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCellBroadcastDispatcher().CallbackGsmBroadcastNotifyMessageReceived(aError, aCbsMsg);

	} // CCtsyDispatcherCallback::CallbackCellBroadcastGsmBroadcastNotifyMessageReceivedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackCellBroadcastWcdmaBroadcastMessageReceivedInd(
	TInt aError, const TDesC8& aWcdmaCbsData, const DispatcherCellBroadcast::TWcdmaCbsMsgBase& aWcdmaCbsMsgBase, TBool aMoreToCome)
/**
 *
 * Function will be called by LTSY to pass array of WCDMA cell broadcast messages. It may be used as a completion
 * of ReceiveMessage() request or by usolicited completion.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aWcdmaCbsData Broadcast message WCDMA.
 * @param aWcdmaCbsMsgBase base wcdma cbs data structure.
 * @param aIsLast Is it the last WCDMA message to pass to CTSY.
 *
 * @see RMobileBroadcastMessaging::ReceiveMessage()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMoreToCome=%d"), aError, aMoreToCome);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCellBroadcastDispatcher().CallbackWcdmaBroadcastMessageReceived(aError, aWcdmaCbsData, aWcdmaCbsMsgBase, aMoreToCome);

	} // CCtsyDispatcherCallback::CallbackCellBroadcastWcdmaBroadcastMessageReceivedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackCellBroadcastStartSimCbTopicBrowsingComp(
	TInt aError, const CArrayFixFlat<RMmCustomAPI::TSimCbTopic>& aSimTopicArray )
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::HandleStartSimCbTopicBrowsingReqL()
 * 
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSimTopicArray array of TSimCbTopic.
 *
 * @see RMmCustomAPI::StartSimCbTopicBrowsing()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSimTopicArray.Count()=%d"), aError, aSimTopicArray.Count());

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCellBroadcastDispatcher().CallbackStartSimCbTopicBrowsing( aError, aSimTopicArray );

	} // CCtsyDispatcherCallback::CallbackCellBroadcastStartSimCbTopicBrowsingComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCellBroadcastDeleteSimCbTopicComp(TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCellBroadcastDeleteSimCbTopic::HandleDeleteSimCbTopicReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::DeleteSimCbTopic()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCellBroadcastDispatcher().CallbackDeleteSimCbTopic(aError);

	} // CCtsyDispatcherCallback::CallbackCellBroadcastDeleteSimCbTopicComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCellBroadcastSetBroadcastFilterSettingComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::HandleSetBroadcastFilterSettingReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileBroadcastMessaging::SetFilterSetting()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCellBroadcastDispatcher().CallbackSetBroadcastFilterSetting(aError);

	} // CCtsyDispatcherCallback::CallbackCellBroadcastSetBroadcastFilterSettingComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCellBroadcastActivateBroadcastReceiveMessageComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a
 * MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::HandleActivateBroadcastReceiveMessageReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileBroadcastMessaging::ReceiveMessage()
 * @see CCtsyDispatcherCallback::CallbackCellBroadcastGsmBroadcastNotifyMessageReceivedInd()
 * @see CCtsyDispatcherCallback::CallbackCellBroadcastWcdmaBroadcastMessageReceivedInd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCellBroadcastDispatcher().CallbackActivateBroadcastReceiveMessage(aError);

	} // CCtsyDispatcherCallback::CallbackCellBroadcastActivateBroadcastReceiveMessageComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCellBroadcastReceiveMessageCancelComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCellBroadcastReceiveMessageCancel::HandleReceiveMessageCancelReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileBroadcastMessaging::ReceiveMessage()
 * @see RMobileBroadcastMessaging::Close()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCellBroadcastDispatcher().CallbackReceiveMessageCancel(aError);

	} // CCtsyDispatcherCallback::CallbackCellBroadcastReceiveMessageCancelComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadComp(
	TInt aError, TInt aIndex, const TDesC& aName, const TDesC& aNumber)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreRead::HandleStoreReadReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndex The index of the entry.
 * @param aName The name stored in the entry. The length of this should not be more than 20.
 * @param aNumber The number stored in the ON entry. The length of this should not be more than 100.
 *
 * @see RMobileONStore::Read()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d"), aError, aIndex);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreRead(aError, aIndex, aName, aNumber);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteAllComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreDeleteAll::HandleStoreDeleteAllReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileONStore::DeleteAll()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreDeleteAll(aError);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteAllComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadEntryComp(
	TInt aError, TInt aIndex, const TDesC& aName, const TDesC& aNumber)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreReadEntry::HandleStoreReadEntryReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndex The index of the entry.
 * @param aName The name stored in the entry. The length of this should not be more than 20.
 * @param aNumber The number stored in the ON entry. The length of this should not be more than 100.
 *
 * @see CRetrieveMobilePhoneONList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d"), aError, aIndex);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));

	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreReadEntry(aError, aIndex, aName, aNumber);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetInfoComp(
		TInt aError, TInt aNumOfEntries, TInt aUsedEntries, TInt aMaxNameLength, TInt aMaxNumberLength)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreGetInfo::HandleStoreGetInfoReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNumOfEntries The number of entries.
 * @param aUsedEntries The number of used entries.
 * @param aMaxNameLength The maximum length of name.
 * @param aMaxNumberLength The maximum length of number.
 *
 * @see RMobileONStore::GetInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aNumOfEntries=%d, aUsedEntries=%d, aMaxNameLength=%d, aMaxNumberLength=%d"), aError, aNumOfEntries, aUsedEntries, aMaxNameLength, aMaxNumberLength);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreGetInfo(aError, aNumOfEntries, aUsedEntries, aMaxNameLength, aMaxNumberLength);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetReadStoreSizeComp(TInt aError, TInt aSize)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreGetReadStoreSize::HandleStoreGetReadStoreSizeReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSize The number of entries stored in the storage.
 *
 * @see CRetrieveMobilePhoneONList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSize=%d"), aError, aSize);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreGetReadStoreSize(aError, aSize);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetReadStoreSizeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteEntryComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreDeleteEntry::HandleStoreDeleteEntryReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileONStore::Delete()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreDeleteEntry(aError);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteEntryComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreWriteEntry::HandleStoreWriteEntryReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileONStore::StoreAllL()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreWriteEntry(aError);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteComp(TInt aError, TInt aLocation)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreWrite::HandleStoreWriteReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aLocation The location where the ON entry was stored.
 * 
 * @see RMobileONStore::Write()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aLocation=%d"), aError, aLocation);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreWrite(aError, aLocation);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetStoreSizeComp(
	TInt aError, TInt aSize)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookOnStoreGetStoreSize::HandleStoreGetStoreSizeReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSize The number of entries that can be stored in the storage.
 *
 * @see RMobileONStore::StoreAll()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSize=%d"), aError, aSize);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookOnDispatcher().CallbackStoreGetStoreSize(aError, aSize);

	} // CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetStoreSizeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreResetCacheInd(
	TInt aError, DispatcherPhonebook::TPhonebook aPhonebook)
/**
 *
 * This indicator resets the state of the cache status in the custom API to
 * RMmCustomAPI::ECacheNotReady for the given phonebook.
 *
 * This is the state value which is read via the RMmCustomAPI::GetPndCacheStatus()
 *
 * This indicator has been left in for legacy reasons and is not expected to be used.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook to reset the cache for.
 *
 * @see RMmCustomAPI::GetPndCacheStatus()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPhonebook=%d"), aError, aPhonebook);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreResetCache(aError,aPhonebook);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreResetCacheInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreSetFdnPhonebookInfoInd(
		TInt aError, TInt aTotalEntries , TInt aMaxTextLength , TInt aMaxNumberLength)
/**
 * This indicator can be used to set the FDN storage information in the case where
 * phonebook initialisation has failed. The information is only updated in aError
 * is KErrNone.
 *
 * This indicator has been left in for legacy reasons and is not expected to be used.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTotalEntries The total number of entries in the Fdn phonebook.
 * @param aMaxTextLength The maximum text length in the Fdn phonebook.
 * @param aMaxNumberLength The maximum number length in the Fdn phonebook.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTotalEntries=%d,aMaxTextLength=%d,aMaxNumberLength=%d"),
							aError,aTotalEntries,aMaxTextLength,aMaxNumberLength);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreSetFdnPhonebookInfoInd(aError,aTotalEntries,aMaxTextLength,aMaxNumberLength);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreSetFdnPhonebookInfoInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreReadEntryComp(
		TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aPhonebookEntries)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookStoreReadEntry::HandleStoreReadEntryReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook of which the read request was made and which the enties are for
 * @param aPhonebookEntries The read entries from the phonebook encoded in a TLV, this can either be encoded directly
 * 	via a CPhoneBookBuffer() or via the CPhoneBookEntry::ExternalizeToTlvEntry() utility.
 *
 * @see RMobilePhoneBookStore::Read()
 * @see CPhoneBookBuffer()
 * @see CPhoneBookEntry::ExternalizeToTlvEntry()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPhonebook=%d"), aError, aPhonebook);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreReadEntry(aError,aPhonebook,aPhonebookEntries);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreReadEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteEntryComp(
	TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aMaxNumberLength)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookStoreDeleteEntry::HandleStoreDeleteEntryReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook of which the delete request was made.
 * @param aMaxNumberLength The new maximum number length possible in the phonebook, this field is
 * only relevant to Fdn, Adn and Vmb phonebooks.
 *
 * @see RMobilePhoneBookStore::Delete()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPhonebook=%d,aMaxNumberLength=%d"), aError, aPhonebook, aMaxNumberLength);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreDeleteEntry(aError,aPhonebook,aMaxNumberLength);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreCacheComp(
		TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aPhonebookEntries)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookStoreCache::HandleStoreCacheReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook of which the cache entries are for (and the request was made from).
 * @param aPhonebookEntries All entries from the phonebook encoded in a TLV, this can either be encoded directly
 * 	via a CPhoneBookBuffer() or via the CPhoneBookEntry::ExternalizeToTlvEntry() utility.
 *
 * @see CPhoneBookBuffer()
 * @see CPhoneBookEntry::ExternalizeToTlvEntry()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPhonebook=%d"), aError, aPhonebook);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreCache(aError,aPhonebook,aPhonebookEntries);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreCacheComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreGetPhonebookInfoComp(
		TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aUsedEntries)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookStoreGetPhonebookInfo::HandleStoreGetPhonebookInfoReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook name of which the phonebook information is for.
 * @param aUsedEntries The number of used entries in the phonebook.
 *
 * @see RMobilePhoneBookStore::GetInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPhonebook=%d,aUsedEntries=%d"), aError, aUsedEntries, aPhonebook);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreGetPhonebookInfo(aError,aPhonebook,aUsedEntries);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreGetPhonebookInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreInitialiseComp(
		TInt aError, const DispatcherPhonebook::TPhonebookStoreInfoV1& aPhonebookStoreInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookStoreInitialise::HandleStoreInitialiseReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebookStoreInfo The information relating to the whole phonebook store.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreInitialise(aError, aPhonebookStoreInfo);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreInitialiseComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteAllComp(
		TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aMaxNumberLength)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookStoreDeleteAll::HandleStoreDeleteAllReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook name for which the delete all request was made.
 * @param aMaxNumberLength The new maximum number length possible in the phonebook, this field is
 * only relevant to Fdn, Adn and Vmb phonebooks.
 *
 * @see RMobilePhoneBookStore::DeleteAll()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPhonebook=%d,aMaxNumberLength=%d"), aError, aPhonebook, aMaxNumberLength);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreDeleteAll(aError,aPhonebook,aMaxNumberLength);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteAllComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadAllComp(
		TInt aError, const DispatcherPhonebook::TSmsData& aSmsData, TInt aIndex, TBool aMoreToCome, TBool aReceivedClass2ToBeResent)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookSmsStoreReadAll::HandleSmsStoreReadAllReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSmsData SMS data. @see TSmsData.
 * @param aIndex index of the SMS entry.
 * @param aMoreToCome ETrue if there will be other SMS entries, EFalse otherwise.
 * @param aReceivedClass2ToBeResent ETrue if received class is to be resent, else EFalse.
 *
 * @see CRetrieveMobilePhoneSmsList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d"), aError, aIndex);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackSmsStoreReadAll(aError, aSmsData, aIndex, aMoreToCome, aReceivedClass2ToBeResent);

	} // CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadAllComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookStoreWriteEntryComp(
		TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TUint16 aIndex, TInt aMaxNumberLength)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookStoreWriteEntry::HandleStoreWriteEntryReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * KErrGsmSimServSneFull, KErrGsmSimServAnrFull, or KErrGsmSimServEmailFull indicates one of the
 * services is full and the field of the entry (e.g. Email) could not be written.
 * In the cache case the field (e.g. Email) is not written to the cache but the other
 * fields are written, the request is then completed with the error code.  In the non-cache case
 * the request is completed with the error code.
 * @param aPhonebook The phonebook name for which the write request was made.
 * @param aIndex The index to which the entry was written in the phonebook.
 * @param aMaxNumberLength The new maximum number length possible in the phonebook, this field is
 * only relevant to Fdn, Adn and Vmb phonebooks.
 *
 * @see RMobilePhoneBookStore::Write()
 * */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPhonebook=%d,aIndex=%d,aMaxNumberLength=%d"), aError, aPhonebook, aIndex, aMaxNumberLength);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackStoreWriteEntry(aError,aPhonebook,aIndex,aMaxNumberLength);

	} // CCtsyDispatcherCallback::CallbackPhonebookStoreWriteEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookGetPhoneStoreInfoComp(
		TInt aError, const RMobilePhoneStore::TMobilePhoneStoreInfoV1& aPhoneStoreInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookGetPhoneStoreInfo::HandleGetPhoneStoreInfoReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhoneStoreInfo The phone store information.
 *
 * @see RMobilePhone::GetPhoneStoreInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackGetPhoneStoreInfo(aError,aPhoneStoreInfo);

	} // CCtsyDispatcherCallback::CallbackPhonebookGetPhoneStoreInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteEntryComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookSmsStoreDeleteEntry::HandleSmsStoreDeleteEntryReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileSmsStore::Delete()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackSmsStoreDeleteEntry(aError);

	} // CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteAllComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookSmsStoreDeleteAll::HandleSmsStoreDeleteAllReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileSmsStore::DeleteAll()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackSmsStoreDeleteAll(aError);

	} // CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteAllComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookSmsStoreGetInfoComp(
	TInt aError, TInt aTotalEntries, TInt aUsedEntries)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookSmsStoreGetInfo::HandleSmsStoreGetInfoReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTotalEntries total number of entries in the SMS storage.
 * @param aUsedEntries total number of used entries in the SMS storage.
 *
 * @see RMobileSmsStore::GetInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aTotalEntries=%d, aUsedEntries=%d"), aError, aTotalEntries, aUsedEntries);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackSmsStoreGetInfo(aError, aTotalEntries, aUsedEntries);

	} // CCtsyDispatcherCallback::CallbackPhonebookSmsStoreGetInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadEntryComp(
		TInt aError, const DispatcherPhonebook::TSmsData& aSmsData)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookSmsStoreReadEntry::HandleSmsStoreReadEntryReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSmsData SMS data. @see TSmsData.
 *
 * @see RMobileSmsStore::Read()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackSmsStoreReadEntry(aError, aSmsData);

	} // CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPhonebookSmsStoreWriteEntryComp(
		TInt aError, TInt aLocation, TBool aReceivedClass2ToBeResent)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPhonebookSmsStoreWriteEntry::HandleSmsStoreWriteEntryReqL()
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aLocation location of the entry.
 * @param aReceivedClass2ToBeResent ETrue if received class is present, else EFalse.
 *
 * @see RMobileSmsStore::Write()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aLocation=%d, aReceivedClass2ToBeResent=%d"), aError, aLocation, aReceivedClass2ToBeResent);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhonebookDispatcher().CallbackSmsStoreWriteEntry(aError, aLocation, aReceivedClass2ToBeResent);

	} // CCtsyDispatcherCallback::CallbackPhonebookSmsStoreWriteEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimRefreshSimFilesInd(TInt aError, TUint16 aRefreshFileList)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate that a refresh
 * of specified cached files held in the CTSY should be done. Depending on the
 * file list sent, the CTSY will begin the refresh.
 *
 * A refresh of the Subscriber Id will always be done independent of the file list sent.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aRefreshFileList Bitmask containing list of files to refresh. The list of possible files is specified in TCacheFileTypes.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRefreshFileList=%d"), aError, aRefreshFileList);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackRefreshSimFiles(aError,aRefreshFileList);

	} // CCtsyDispatcherCallback::CallbackSimRefreshSimFilesInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSimNotifyIccMessageWaitingIndicatorsChangeInd(
	TInt aError,
	const RMobilePhone::TMobilePhoneMessageWaitingV1& aIndicators)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate that the  message waiting indicators 
 * on the current ICC has changed.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIndicators The ICC message waiting indicators to return.
 *
 * @see RMobilePhone::NotifyIccMessageWaitingIndicatorsChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackNotifyIccMessageWaitingIndicatorsChange(aError, aIndicators);

	} // CCtsyDispatcherCallback::CallbackSimNotifyIccMessageWaitingIndicatorsChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSimNotifyApnListChangeInd(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate a change to the APN list.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::NotifyAPNListChanged()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackNotifyApnListChange(aError);

	} // CCtsyDispatcherCallback::CallbackSimNotifyApnListChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSimNotifyApnControlListServiceStatusChangeInd(
	TInt aError,
	RMobilePhone::TAPNControlListServiceStatus aStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate a change to the APN Control List service status.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatus The new service status to return.
 *
 * @see RMobilePhone::NotifyAPNControlListServiceStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackNotifyApnControlListServiceStatusChange(aError, aStatus);

	} // CCtsyDispatcherCallback::CallbackSimNotifyApnControlListServiceStatusChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetApnControlListServiceStatusComp(
	TInt aError,
	RMobilePhone::TAPNControlListServiceStatus aStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetApnControlListServiceStatus::HandleGetApnControlListServiceStatusReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatus The APN Control List Service status stored on the USIM
 *
 * @see RMobilePhone::GetAPNControlListServiceStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetApnControlListServiceStatus(aError, aStatus);

	} // CCtsyDispatcherCallback::CallbackSimGetApnControlListServiceStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimDeleteApnNameComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimDeleteApnName::HandleDeleteApnNameReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::DeleteAPNName()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackDeleteApnName(aError);

	} // CCtsyDispatcherCallback::CallbackSimDeleteApnNameComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimEnumerateApnEntriesComp(
	TInt aError,
	TUint32 aNumEntries)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimEnumerateApnEntries::HandleEnumerateApnEntriesReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNumEntries The total number of available entries.
 *
 * @see RMobilePhone::EnumerateAPNEntries()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aNumEntries=%d"), aError, aNumEntries);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackEnumerateApnEntries(aError, aNumEntries);

	} // CCtsyDispatcherCallback::CallbackSimEnumerateApnEntriesComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimChangeSecurityCodeComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimChangeSecurityCode::HandleChangeSecurityCodeReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::ChangeSecurityCode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackChangeSecurityCode(aError);

	} // CCtsyDispatcherCallback::CallbackSimChangeSecurityCodeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSetFdnSettingComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimSetFdnSetting::HandleSetFdnSettingReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetFdnSetting()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSetFdnSetting(aError);

	} // CCtsyDispatcherCallback::CallbackSimSetFdnSettingComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetCustomerServiceProfileComp(
	TInt aError,
	const RMobilePhone::TMobilePhoneCspFileV1& aCsp)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetCustomerServiceProfile::HandleGetCustomerServiceProfileReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCsp The Customer Service Profile file that is stored on the SIM
 *
 * @see RMobilePhone::GetCustomerServiceProfile()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetCustomerServiceProfile(aError, aCsp);

	} // CCtsyDispatcherCallback::CallbackSimGetCustomerServiceProfileComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetSubscriberIdComp(
	TInt aError,
	const TDesC8& aId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetSubscriberId::HandleGetSubscriberIdReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aId The subscriber id to be returned.
 *
 * @see RMobilePhone::GetSubscriberId()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetSubscriberId(aError, aId);

	} // CCtsyDispatcherCallback::CallbackSimGetSubscriberIdComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimAppendApnNameComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimAppendApnName::HandleAppendApnNameReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::AppendAPNName()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackAppendApnName(aError);

	} // CCtsyDispatcherCallback::CallbackSimAppendApnNameComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetActiveIccApplicationTypeComp(TInt aError, MLtsyDispatchSimGetActiveIccApplicationType::TIccType aIccType)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetActiveIccApplicationType::HandleGetActiveIccApplicationTypeReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIccType Contains the active application type on ICC.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIccType=%d"), aError, aIccType);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetActiveIccApplicationType(aError, aIccType);

	} // CCtsyDispatcherCallback::CallbackSimGetActiveIccApplicationTypeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSetIccMessageWaitingIndicatorsComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimSetIccMessageWaitingIndicators::HandleSetIccMessageWaitingIndicatorsReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetIccMessageWaitingIndicators()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSetIccMessageWaitingIndicators(aError);

	} // CCtsyDispatcherCallback::CallbackSimSetIccMessageWaitingIndicatorsComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSetApnControlListServiceStatusComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimSetApnControlListServiceStatus::HandleSetApnControlListServiceStatusReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetAPNControlListServiceStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSetApnControlListServiceStatus(aError);

	} // CCtsyDispatcherCallback::CallbackSimSetApnControlListServiceStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetApnNameComp(
	TInt aError,
	const RMobilePhone::TAPNEntryV3& aEntry)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetApnName::HandleGetApnNameReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aEntry The Access Point Name (APN) entry
 *
 * @see RMobilePhone::GetAPNname()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetApnName(aError, aEntry);

	} // CCtsyDispatcherCallback::CallbackSimGetApnNameComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimNotifySimCardStatusInd(
	TInt aError, RMmCustomAPI::TSIMCardStatus aSimCardStatus)
/**
 * 
 * This indicator is used to communicate changes to the status of the SIM
 * card - i.e. whether it is inserted / readable etc. 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSimCardStatus Latest SIM card event.
 * @see RMmCustomAPI::NotifySimCardStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSimCardStatus=%d"), aError, aSimCardStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackNotifySimCardStatus(aError, aSimCardStatus);

	} // CCtsyDispatcherCallback::CallbackSimNotifySimCardStatusInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSimLockActivateComp(TInt aError)
/**
* 
* Callback function to be used by the Licensee LTSY to complete a pending
* MLtsyDispatchSimSimLockActivate::HandleSimLockActivateReqL()
* 
*
* @param aError The error code to be returned to the CTSY Dispatcher.
* 
* @see RMmCustomAPI::ActivateSimLock()
*/
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSimLockActivate(aError);

	} // CCtsyDispatcherCallback::CallbackSimSimLockActivateComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSimLockDeActivateComp(TInt aError)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimSimLockDeActivate::HandleSimLockDeActivateReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMmCustomAPI::DeActivateSimLock()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSimLockDeActivate(aError);

	} // CCtsyDispatcherCallback::CallbackSimSimLockDeActivateComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetAnswerToResetComp(TInt aError, const TDesC8& aAnswerToReset)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetAnswerToReset::HandleGetAnswerToResetReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aAnswerToReset The answer to reset.
 * 
 * @see RMmCustomAPI::GetATR()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAnswerToReset=%S"), aError, &aAnswerToReset);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetAnswerToReset(aError, aAnswerToReset);

	} // CCtsyDispatcherCallback::CallbackSimGetAnswerToResetComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetSimCardReaderStatusComp(TInt aError, TUint8 aSimCardReaderStatus)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetSimCardReaderStatus::HandleGetSimCardReaderStatusReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSimCardReaderStatus The sim card reader's status.
 * 							   Bits: 
 *								1-3  Identity of card reader,
 *								4    0=Card reader is not removable,
 *								     1=Card reader is removable,
 *								5    0=Card reader is not present,
 * 									 1=Card reader is present,
 *								6    0=Card reader present is not ID-1 size,
 *									 1=Card reader present is ID-1 size,
 *								7    0=No card present,
 *									 1=Card is present in reader,
 *								8    0=No card powered,
 *									 1=Card in reader is powered.
 * 
 * 
 * @see RMmCustomAPI::GetSimCardReaderStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSimCardReaderStatus=%d"), aError, aSimCardReaderStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetSimCardReaderStatus(aError, aSimCardReaderStatus);

	} // CCtsyDispatcherCallback::CallbackSimGetSimCardReaderStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationDataComp(
	TInt aError, const TDesC8& aSignedResponse, const TDesC8& aCipheringKey)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetSimAuthenticationEapSimData::HandleGetSimAuthenticationEapSimDataReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSignedResponse The signed response. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * @param aCipheringKey The ciphering key. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * 
 * @see RMmCustomAPI::GetWlanSimAuthenticationData()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSignedResponse=%S, aCipheringKey=%S"), aError, &aSignedResponse, &aCipheringKey);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetSimAuthenticationData(aError, aSignedResponse, aCipheringKey);

	} // CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationDataComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationDataComp(
	TInt aError, const TDesC8& aResponse, const TDesC8& aCipheringKey, const TDesC8& aIntegrityKey, const TDesC8& aAUTS)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimGetSimAuthenticationEapAkaData::HandleGetSimAuthenticationEapAkaDataReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponse The response. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * @param aCipheringKey The ciphering key. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * @param aIntegrityKey The integrity key. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * @param aAUTS The AUTS value. A value generated by the peer upon experiencing a synchronization failure, 112 bits. 
 * 		  This is needed only in error case. The length should not be greater than RMmCustomAPI::KMaxParametersLength.
 * 
 * @see RMmCustomAPI::GetWlanSimAuthenticationData()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aResponse=%S, aCipheringKey=%S, aIntegrityKey=%S, aAUTS=%S"), aError, &aResponse, &aCipheringKey, &aIntegrityKey, &aAUTS);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackGetSimAuthenticationData(aError, aResponse, aCipheringKey, aIntegrityKey, aAUTS);

	} // CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationDataComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimPowerSimOffComp(TInt aError)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimPowerSimOff::HandlePowerSimOffReqL()
 *
 *  
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMmCustomAPI::PowerSimOff()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackPowerSimOff(aError);

	} // CCtsyDispatcherCallback::CallbackSimPowerSimOffComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimPowerSimOnComp(TInt aError)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimPowerSimOn::HandlePowerSimOnReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMmCustomAPI::PowerSimOn()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackPowerSimOn(aError);

	} // CCtsyDispatcherCallback::CallbackSimPowerSimOnComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimReadSimFileComp(TInt aError, const TDesC8& aResponseBytes)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimReadSimFile::HandleReadSimFileReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponseBytes The bytes read from the SIM.
 * 
 * @see RMmCustomAPI::ReadSimFile()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackReadSimFile(aError,aResponseBytes);

	} // CCtsyDispatcherCallback::CallbackSimReadSimFileComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSendApduRequestComp(
		TInt aError, TUint8 aServiceType, TUint8 aCardReaderNumber, TUint8 aApplicationType, const TDesC8& aResponseData)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimSendApduRequest::HandleSimSendApduRequestReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aServiceType The value for the service type of the request to be returned to the client.
 * @param aCardReaderNumber The value for the card reader number of the request to be returned to the client.
 * @param aApplicationType The value for the application type of the request to be returned to the client.
 * @param aResponseData The transparent response data conveyed from the baseband to be returned to the client.
 * 
 * @see RMmCustomAPI::SendAPDUReq()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aServiceType=%d,aCardReaderNumber=%d,aApplicationType=%d"), aError, aServiceType, aCardReaderNumber, aApplicationType);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSendApduRequest(aError,aServiceType,aCardReaderNumber,aApplicationType,aResponseData);

	} // CCtsyDispatcherCallback::CallbackSimSendApduRequestComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSendApduRequestV2Comp(
		TInt aError, const TDesC8& aResponseData)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimSendApduRequestV2::HandleSimSendApduRequestV2ReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponseData The transparent response data conveyed from the baseband to be returned to the client.
 * 
 * @see RMmCustomAPI::SendAPDUReq()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSendApduRequestV2(aError, aResponseData);

	} // CCtsyDispatcherCallback::CallbackSimSendApduRequestV2Comp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSimWarmResetComp(TInt aError)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimSimWarmReset::HandleSimWarmResetReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMmCustomAPI::SimWarmReset()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSimWarmReset(aError);

	} // CCtsyDispatcherCallback::CallbackSimSimWarmResetComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSimSetSimMessageStatusReadComp(TInt aError)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSimSetSimMessageStatusRead::HandleSimSetMessageStatusReadReqL()
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMmCustomAPI::SetSimMessageStatusRead()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSimDispatcher().CallbackSetSimMessageStatusRead(aError);

	} // CCtsyDispatcherCallback::CallbackSimSetSimMessageStatusReadComp


EXPORT_C void CCtsyDispatcherCallback::CallbackSmsNotifyReceiveSmsMessageInd(TInt aError, TBool aInd, const TSmsMsg& aSmsMessage)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * RMobileSmsMessaging::ReceiveMessage() or as unsolicited completion
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aInd Set routing activity on/off.
 * @param aSmsMessage Used to buffer incoming SMSes to the TSY.
 *
 * @see RMobileSmsMessaging::ReceiveMessage()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aInd=%d"), aError, aInd);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackNotifyReceiveSmsMessage(aError, aInd, aSmsMessage);

	} // CCtsyDispatcherCallback::CallbackSmsNotifyReceiveSmsMessageInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsSendSatSmsComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsSendSatSms::HandleSendSatSmsReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 *
 * @see RSat::SendMessageNoLogging()
 * @see RSat::NotifySendSmPCmd()
 * @see RSat::NotifyMoSmControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackSendSatSms(aError);

	} // CCtsyDispatcherCallback::CallbackSmsSendSatSmsComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsSendSmsMessageComp(
	TInt aError, TInt aMsgRef, const TDesC8& aSmsSubmitReport)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsSendSmsMessage::HandleSendSmsMessageReqL()
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aMsgRef the message reference assigned to a sent message.
 * @param aSmsSubmitReport The message string, which is expected to be a buffer with
 *        a maximum size of RMobileSmsMessaging::KGsmTpduSize
 * @see RMobileSmsMessaging::SendMessage()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackSendSmsMessage(aError, aMsgRef, aSmsSubmitReport );

	} // CCtsyDispatcherCallback::CallbackSmsSendSmsMessageComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsGetSmsStoreInfoComp(
	TInt aError, TInt aTotalEntries, TInt aUsedEntries)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsGetSmsStoreInfo::HandleGetSmsStoreInfoReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTotalEntries Indicates the total number of entries that may be held in this store.
 * @param aUsedEntries Indicates the current number of entries held in this store.
 *
 * @see RMobileSmsMessaging::GetMessageStoreInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aTotalEntries=%d, aUsedEntries=%d"), aError, aTotalEntries, aUsedEntries);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackGetSmsStoreInfo(aError, aTotalEntries, aUsedEntries);

	} // CCtsyDispatcherCallback::CallbackSmsGetSmsStoreInfoComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsGetSmspListComp(TInt aError, const TDesC& aServiceCenterAddress, const TDesC& aDestinationAddress,
                                       const TDesC& aAlphaTagData, const DispatcherSim::TSmsParameters& aSmsParameters, TBool aMoreToCome)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsGetSmspList::HandleGetSmspListReqL()
 *
 * Sends the SMS parameter list to the CTSY.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aServiceCenterAddress Service center address.
 * @param aDestinationAddress Destination address.
 * @param aAlphaTagData Alpha tag data.
 * @param aSmsParameters Sms parametets.
 * @param aMoreToCome Are more elements to come.
 *
 * @see CMobilePhoneSmspList()
 * @see CRetrieveMobilePhoneSmspList()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMoreToCome=%d"), aError, aMoreToCome);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackGetSmspList(aError, aServiceCenterAddress, aDestinationAddress, aAlphaTagData, aSmsParameters, aMoreToCome);

	} // CCtsyDispatcherCallback::CallbackSmsGetSmspListComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsNackSmsStored::HandleNackSmsStoredReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileSmsMessaging::NackSmsStored()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackNackSmsStored(aError);

	} // CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsAckSmsStored::HandleAckSmsStoredReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileSmsMessaging::AckSmsStored()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackAckSmsStored(aError);

	} // CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsResumeSmsReceptionComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsResumeSmsReception::HandleResumeSmsReceptionReqL()
 * 
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileSmsMessaging::ResumeSmsReception()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackResumeSmsReception(aError);

	} // CCtsyDispatcherCallback::CallbackSmsResumeSmsReceptionComp


EXPORT_C void CCtsyDispatcherCallback::CallbackSmsSendSmsMessageNoFdnCheckComp(
		TInt aError, TInt aMsgRef, const TDesC8& aSmsSubmitReport)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsSendSmsMessageNoFdnCheck::HandleSendSmsMessageNoFdnCheckReqL()
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aMsgRef The message reference assigned to a sent message.
 * @param aSmsSubmitReport The message string, which is expected to be a buffer with a
 *        maximum size of RMobileSmsMessaging::KGsmTpduSize
 *
 * @see RMobileSmsMessaging::SendMessageNoFdnCheck()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackSendSmsMessageNoFdnCheck(aError, aMsgRef, aSmsSubmitReport);

	} // CCtsyDispatcherCallback::CallbackSmsSendSmsMessageNoFdnCheckComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsSetMoSmsBearerComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsSetMoSmsBearer::HandleSetMoSmsBearerReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileSmsMessaging::SetMoSmsBearer()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackSetMoSmsBearer(aError);

	} // CCtsyDispatcherCallback::CallbackSmsSetMoSmsBearerComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSmsStoreSmspListEntryComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSmsStoreSmspList::HandleStoreSmspListEntryReqL()
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 *
 * @see RMobileSmsMessaging::StoreSmspListL()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSmsDispatcher().CallbackStoreSmspListEntry(aError);

	} // CCtsyDispatcherCallback::CallbackSmsStoreSmspListEntryComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceHangUpComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlMultipartyConferenceHangUp::HandleConferenceHangUpReqL()
 *
 * This callback is used to inform the CTSY of the outcome of the request to
 * initiate the hang up the conference call. The LTSY can inform the CTSY of
 * successful completion of the hang up process via call status
 * notifications as the calls disconnect and then become idle.
 * See CCtsyDispatcherCallback::CallbackCallControlNotifyCallStatusChangeInd()
 *
 * @param aError KErrNone if the create conference request was made to the baseband
 * successfully; another error code indicating the error otherwise.
 *
 * @see RMobileConferenceCall::HangUp()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlMultipartyDispatcher().CallbackConferenceHangUp(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceHangUpComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceAddCallComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlMultipartyConferenceAddCall::HandleConferenceAddCallReqL()
 *
 * Indicates to the CTSY the outcome of the request to
 * add a call to a conference call.  If the request was
 * successful, subsequent notifications to indicate that the new call
 * statuses (to RMobileCall::EStatusConnected) are made by completing call status
 * notifications using
 * CCtsyDispatcherCallback::CallbackCallControlNotifyCallStatusChangeInd()
 * for each call in the conference state.
 *
 * @param aError KErrNone if the request to add a call to the conference was
 * successful. Another error code indicating the failure otherwise.
 *
 * @see RMobileConferenceCall::AddCall()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlMultipartyDispatcher().CallbackConferenceAddCall(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceAddCallComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlMultipartyCreateConferenceComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlMultipartyCreateConference::HandleCreateConferenceReqL()
 *
 * This callback is used to inform the CTSY of the outcome of the request to
 * initiate the creation of a conference call.
 *
 * @param aError KErrNone if the create conference request was made to the baseband
 * successfully; another error code indicating the error otherwise.
 *
 * @see RMobileConferenceCall::CreateConference()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlMultipartyDispatcher().CallbackCreateConference(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlMultipartyCreateConferenceComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceSwapComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlMultipartyConferenceSwap::HandleConferenceSwapReqL()
 *
 * Informs the CTSY of the outcome of the request to swap the conference call
 * with another connected / held call.
 *
 * The swap request is fully completed when the LTSY has completed the necessary
 * call status notifications to indicate the new state of the calls inside and
 * outside of the conference using
 * CCtsyDispatcherCallback::CallbackCallControlNotifyCallStatusChangeInd()
 *
 * @param aError KErrNone to indicate success, another error code indicating the error
 * otherwise.
 *
 * @see RMobileConferenceCall::Swap()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlMultipartyDispatcher().CallbackConferenceSwap(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceSwapComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceGoOneToOneComp(TInt aError, TInt aCallId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::HandleConferenceGoOneToOneReqL()
 *
 * Informs the CTSY of the outcome of the request to initiate a private communication
 * with one of the calls in the conference call.  The other call / calls are expected
 * to go on hold whilst the specified call becomes connected. The LTSY informs the
 * CTSY of these call status changes using
 * CCtsyDispatcherCallback::CallbackCallControlNotifyCallStatusChangeInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error
 * otherwise.
 *
 * @param aCallId The Call ID to which the One-To-One request refers.
 *
 * @see RMobileCall::GoOneToOne()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlMultipartyDispatcher().CallbackConferenceGoOneToOne(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceGoOneToOneComp


EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallBarringStatusChangeInd(
	TInt aError, RMobilePhone::TMobilePhoneCBCondition aCondition)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate a change to a call barring status.
 * The Call Barring Supplementary Service is detailed in 3GPP TS 24.088.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCondition The call barring condition that have changed
 *
 * @see RMobilePhone::NotifyCallBarringStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCondition=%d"), aError, aCondition);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyCallBarringStatusChange(aError, aCondition);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallBarringStatusChangeInd


EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesReceiveUssdMessageInd(
	TInt aError, const TDesC8& aMessage, TUint8 aDcs,
	RMobileUssdMessaging::TMobileUssdMessageType aType,
	RMobileUssdMessaging::TMobileUssdDataFormat aFormat)
/**
 * Callback function to be used by the Licensee LTSY to indicate that a
 * USSD message was received from the network. The USSD Supplementary Service
 * is detailed in 3GPP TS 24.090.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aMessage The message that was received
 * @param aDcs The Data Coding Scheme of the received USSD message
 * @param aType The message type (depend on the invoke part of the message:
 * 									UnstructuredSS-Notify - RMobileUssdMessaging::EUssdMTNotify;
 * 									UnstructuredSS-Request - RMobileUssdMessaging::EUssdMTRequest;
 * 									ProcessUnstructuredSS-Request - RMobileUssdMessaging::EUssdMTReply)
 * @param aFormat The message format
 *
 * @see RMobileUssdMessaging::ReceiveMessage()
 */
	{
	//TSYLOGENTRYEXITARGS(_L8("aError=%d, aDcs=%d, aType=%d, aFormat=%d"), aError, aDcs, aType, aFormat);
	//logging the regular method (above) cases the test to hange and enter the debug monitor on hardware.
	//initial invetigation shows a "ExitInfo 2,23,USER"  
	
	TSYLOGENTRYEXIT;
	
	OstTraceDefExt5(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCTSYDISPATCHERCALLBACK_CALLBACKSUPPLEMENTARYSERVICESRECEIVEUSSDMESSAGEIND_1,"aError=%d, aDcs=%d, aType=%d, aFormat=%d aMessage=%s", aError, aDcs, aType, aFormat,aMessage);
		
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackReceiveUssdMessage(aError, aMessage, aDcs, aType, aFormat);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesReceiveUssdMessageInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallWaitingStatusChangeInd(
	TInt aError,
    RMobilePhone::TMobileService aServiceGroup,
    RMobilePhone::TMobilePhoneCWStatus aStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate a change to
 * a call waiting status. The Call Waiting Supplementary Service
 * is detailed in 3GPP TS 24.083.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aServiceGroup The relevant service group
 * @param aStatus The new call waiting status
 *
 * @see RMobilePhone::NotifyCallWaitingStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aServiceGroup=%d, aStatus=%d"), aError, aServiceGroup, aStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyCallWaitingStatusChange(aError, aServiceGroup, aStatus);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallWaitingStatusChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallForwardingStatusChangeInd(
	TInt aError, RMobilePhone::TMobilePhoneCFCondition aCallForwardingCondition)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate a Call Forwarding status changes.
 * The Call Forwarding Supplementary Service is detailed in 3GPP TS 24.082.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCallForwardingCondition The call forwarding condition that changed status
 *
 * @see RMobilePhone::NotifyCallForwardingStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallForwardingCondition=%d"), aError, aCallForwardingCondition);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyCallForwardingStatusChange(aError,aCallForwardingCondition);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallForwardingStatusChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallForwardingActiveInd(
	TInt aError, RMobilePhone::TMobileService aServiceGroup, RMobilePhone::TMobilePhoneCFActive aActiveType)
/**
 *
 * Callback function to be used by the Licensee LTSY to indicate
 * when a call is made on a line and a call forwarding service is active
 * on this line at the time. The Call Forwarding Supplementary Service is
 * detailed in 3GPP TS 24.082.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aServiceGroup The basic service group the call forwarding is active.
 * @param aActiveType Specifies whether CF unconditional or one of the conditional CF services is active
 *
 * @see RMobilePhone::NotifyCallForwardingActive()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyCallForwardingActive(aError, aServiceGroup, aActiveType);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyCallForwardingActiveInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestNoFdnCheckComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::HandleSendNetworkServiceRequestNoFdnCheckReqL()
 *
 * Used to indicate to the Common TSY the outcome of sending a network service request without FDN check.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SendNetworkServiceRequestNoFdnCheck()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSendNetworkServiceRequestNoFdnCheck(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestNoFdnCheckComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallWaitingStatusComp(
	TInt aError, const CMobilePhoneCWList& aCallWaitingList)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::HandleGetCallWaitingStatusReqL()
 *
 * Used to return to the Common TSY the status of the call waiting service.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallWaitingList The list of the waiting calls status
 *
 * @see CRetrieveMobilePhoneCWList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallWaitingList.Enumerate=%d"), aError, aCallWaitingList.Enumerate());

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackGetCallWaitingStatus(aError, aCallWaitingList);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallWaitingStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::HandleSendNetworkServiceRequestReqL()
 *
 * Used to indicate to the Common TSY the outcome of sending a network service request with FDN check.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SendNetworkServiceRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSendNetworkServiceRequest(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallBarringStatusComp(
	TInt aError, const CMobilePhoneCBList& aCallBarringList)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesGetCallBarringStatus::HandleGetCallBarringStatusReqL()
 *
 * Used to return to the Common TSY the status of the call barring service.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallBarringList the call barring list
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see CRetrieveMobilePhoneCBList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallBarringList.Enumerate=%d"), aError, aCallBarringList.Enumerate());

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackGetCallBarringStatus(aError, aCallBarringList);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallBarringStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallBarringStatusComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSetCallBarringStatus::HandleSetCallBarringStatusReqL()
 *
 * Used to indicate to the Common TSY the outcome of setting the call barring service.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetCallBarringStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSetCallBarringStatus(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallBarringStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallForwardingStatusComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::HandleSetCallForwardingStatusReqL()
 *
 * Used to indicate to the Common TSY the outcome of setting the call forwarding service.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetCallForwardingStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSetCallForwardingStatus(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallForwardingStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallWaitingStatusComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::HandleSetCallWaitingStatusReqL()
 *
 * Used to indicate to the Common TSY the outcome of setting the call waiting service.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetCallWaitingStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSetCallWaitingStatus(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallWaitingStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSetSsPasswordComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSetSsPassword::HandleSetSsPasswordReqL()
 *
 * Used to indicate to the Common TSY the outcome of setting the supplementary services password.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobilePhone::SetSSPassword()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSetSsPassword(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSetSsPasswordComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallForwardingStatusComp(
	TInt aError, const CMobilePhoneCFList& aCallForwardingList)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::HandleGetCallForwardingStatusReqL()
 *
 * Used to return to the Common TSY the status of the call forwarding service.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallForwardingList the call forwarding list
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see CRetrieveMobilePhoneCFList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallForwardingList.Enumerate()=%d"), aError, aCallForwardingList.Enumerate());

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackGetCallForwardingStatus(aError,aCallForwardingList);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallForwardingStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSendUssdMessage::HandleSendUssdMessageReqL()
 *
 * Used to indicate to the Common TSY the outcome of sending an USSD message.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileUssdMessaging::SendMessage()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSendUssdMessage(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageNoFdnCheckComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::HandleSendUssdMessageNoFdnCheckReqL()
 *
 * Used to indicate to the Common TSY the outcome of sending an USSD message.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileUssdMessaging::SendMessageNoFdnCheck()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSendUssdMessageNoFdnCheck(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageNoFdnCheckComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdReleaseComp(
	TInt aError, TUint8 aOpCode, const TDesC& aAdditionalInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesSendUssdRelease::HandleSendUssdReleaseReqL()
 *
 * Used to indicate to the Common TSY the outcome of sending an USSD release.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aOpCode The Operation Code.
 * @param aAdditionalInfo Any additional information
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMobileUssdMessaging::SendRelease()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aOpCode=%d"), aError, aOpCode);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackSendUssdRelease(aError, aOpCode, aAdditionalInfo);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdReleaseComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesClearBlacklistComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSupplementaryServicesClearBlacklist::HandleClearBlacklistL()
 *
 * Used to indicate to the Common TSY the outcome of clearing the blacklist.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::ClearCallBlackList()
 * @see RMobilePhone::ClearBlacklist()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackClearBlacklist(aError);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesClearBlacklistComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesGetDiagnosticOctetsInd(
	TInt aError,  TInt aCallId, TUint8 aOctet)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * the 5th octet of the cause information element for supplementary services
 * The cause information element is detailed in 3GPP TS 24.008.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCallId The Call ID of the call whose cause is set.
 * @param aOctet The octet to set.
 *
 * @see RMmCustomAPI::GetDiagnosticInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d, aOctet=0x%x"), aError, aCallId, aOctet);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackGetDiagnosticOctets(aError, aCallId, aOctet);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesGetDiagnosticOctetsInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventForwardModeInd(
	TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsForwMode aForwardMode)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * an NotifySS message from the network about a call forward mode. The NotifySS coding is detailed in 3GPP TS 24.080,
 * and the NotifySS usage for call forward mode change is detailed in 3GPP TS 24.082.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aForwardMode The forward mode (SS-Notification).
 *
 * @see RMmCustomAPI::NotifySsNetworkEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d, aForwardMode=%d") ,
      aError, aType, aMode, aForwardMode);
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyNetworkEventForwardMode(aError, aType, aMode, aForwardMode);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventForwardModeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventCallWaitingInd(
	TInt aError, RMmCustomAPI::TSsMode aMode, TBool aCallIsWaiting)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * an NotifySS message from the network about a call waiting state. The NotifySS coding is detailed in 3GPP TS 24.080,
 * and the NotifySS usage for waiting state is detailed in 3GPP TS 24.083.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aMode The mode of the service.
 * @param aCallIsWaiting An indicator of the call waiting state of the call (callIsWaiting-Indicator).
 *
 * @see RMmCustomAPI::NotifySsNetworkEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMode=%d, aCallIsWaiting=%d") ,
      aError, aMode, aCallIsWaiting);
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyNetworkEventCallWaiting(aError, aMode, aCallIsWaiting);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventCallWaitingInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventHoldModeInd(
		TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsHoldMode aHoldMode)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * an NotifySS message from the network about the hold state of a call. The NotifySS coding is detailed in 3GPP TS 24.080,
 * and the NotifySS usage for hold mode is detailed in 3GPP TS 24.083, 3GPP TS 24.084 and 3GPP TS 24.091.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aHoldMode The hold mode (callOnHold-Indicator).
 *
 * @see RMmCustomAPI::NotifySsNetworkEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d, aHoldMode=%d") ,
      aError, aType, aMode, aHoldMode);
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyNetworkEventHoldMode(aError, aType, aMode, aHoldMode);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventHoldModeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventConfrenceInd(
	TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, TBool aConferenceIndicator)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * an NotifySS message from the network about a confrence call. The NotifySS coding is detailed in 3GPP TS 24.080,
 * and the NotifySS usage for confrence call is detailed in 3GPP TS 24.084.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aConferenceIndicator The confrence call indicator (mpty-Indicator).
 *
 * @see RMmCustomAPI::NotifySsNetworkEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d, aConferenceIndicator=%d") ,
      aError, aType, aMode, aConferenceIndicator);
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyNetworkEventConfrence(aError, aType, aMode, aConferenceIndicator);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventConfrenceInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventCugInd(
	TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, TUint16 aCugIndex)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * an NotifySS message from the network about a closed user group (CUG) service. The NotifySS coding is detailed in 3GPP TS 24.080,
 * and the NotifySS usage for CUG is detailed in 3GPP TS 24.085.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aCugIndex The Index associated with the invoked CUG (cug-Index).
 *
 * @see RMmCustomAPI::NotifySsNetworkEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d, aCugIndex=%d") ,
      aError, aType, aMode, aCugIndex);
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyNetworkEventCug(aError, aType, aMode, aCugIndex);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventCugInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventClirSuppressionInd(
	TInt aError, RMmCustomAPI::TSsMode aMode, TBool aClirSuppressionRejected)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * an NotifySS message from the network about a Calling Line Identification Restriction (CLIR) service. The NotifySS coding is detailed in 3GPP TS 24.080,
 * and the NotifySS usage for CLIR is detailed in 3GPP TS 24.081.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aMode The mode of the service.
 * @param aClirSuppressionRejected An indicator that indicate if the CLIR override has not been performed (clirSuppressionRejected).
 *
 * @see RMmCustomAPI::NotifySsNetworkEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMode=%d, aClirSuppressionRejected=%d") ,
      aError, aMode, aClirSuppressionRejected);
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyNetworkEventClirSuppression(aError, aMode, aClirSuppressionRejected);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventClirSuppressionInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventEctCallStateInd(
	TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode,
	RMmCustomAPI::TSsEctState aEctCallState, RMmCustomAPI::TSsChoice aEctChoice, const TDesC& aRemotePartyNumber)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * an NotifySS message from the network about a Explicit Call Transfer (ECT) service. The NotifySS coding is detailed in 3GPP TS 24.080,
 * and the NotifySS usage for ECT is detailed in 3GPP TS 24.091.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 * @param aEctCallState The ECT state (ect-Indicator.ect-CallState).
 * @param aEctChoice The ECT presentation (ect-Indicator.rdn).
 * @param aRemotePartyNumber The remote party number (ect-Indicator.rdn).
 *
 * @see RMmCustomAPI::NotifySsNetworkEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("err=%d, type=%d ") ,
      aError, aType);
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyNetworkEventEctCallState(aError, aType, aMode, aEctCallState, aEctChoice, aRemotePartyNumber);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventEctCallStateInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventInd(
	TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * an NotifySS message without another specific callback function from the network. The NotifySS coding is detailed in 3GPP TS 24.080,
 * and the NotifySS usage is detailed in all the 3GPP supplementary services specs.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aType The type of service.
 * @param aMode The mode of the service.
 *
 * @see RMmCustomAPI::NotifySsNetworkEvent()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aMode=%d") ,
      aError, aType, aMode);
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyNetworkEvent(aError, aType, aMode);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyNetworkEventInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyAdditionalInfoInd(
	TInt aError, TUint8 aOperationCode, const TDesC8& aAdditionalInfo)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * supplementary services additional info. The supplementary services coding are detailed in 3GPP TS 24.080
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aOperationCode The operational code.
 * @param aAdditionalInfo Any additional information.
 *
 * @see RMmCustomAPI::SsAdditionalInfoNotification()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aOperationCode=%d"), aError, aOperationCode);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyAdditionalInfo(aError, aOperationCode, aAdditionalInfo);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyAdditionalInfoInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyRequestCompleteInd(
	TInt aError, TInt aStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to inform the CTSY about
 * a completion of a supplementary services request. The supplementary services coding are detailed in 3GPP TS 24.080
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatus The completion status.
 *
 * @see RMmCustomAPI::SsRequestCompleteNotification()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d"), aError, aStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifyRequestComplete(aError, aStatus);

	} // CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifyRequestCompleteInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSupplementaryServicesNotifySendNetworkServiceRequestInd(TInt aError, RMobilePhone::TMobilePhoneNotifySendSSOperation aOperationCode, const TDesC& aAdditionalInfo)
/**
 * Callback function to be used by the Licensee LTSY to notify the client for send network service 
 * request invocation and request completion events.
 * 
 *
 * @param The Operation Code for a Send SS Invoke or Return result events, or the Error 
 * or Problem Code for a Send SS Return Error or Reject events.
 * @param The additional information (parameters) for a Send SS Invoke or Return result or Return Error events.
 * This is not used for a Send SS Reject event
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RMobilePhone::NotifySendNetworkServiceRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aOperationCode=%d"), aError, aOperationCode);
		
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSupplementaryServicesDispatcher().CallbackNotifySendNetworkServiceRequest(aError, aOperationCode, aAdditionalInfo);
	} //CCtsyDispatcherCallback::CallbackPhoneNotifySendNetworkServiceRequestInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketAttachModeChangeInd(TInt aError, RPacketService::TPreferredBearer aBearer)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify change in the
 * attach mode (IPC EPacketNotifyAttachModeChange).
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aBearer The new bearer.
 * @see RPacketService::NotifyAttachModeChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aBearer=%d"), aError, aBearer);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyPacketAttachModeChange(aError, aBearer);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketAttachModeChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyQosProfileChangedGPRSInd(
	TInt aError, const TDesC& aContextName,
	const RPacketQoS::TQoSGPRSNegotiated& aQoSGPRSNegotiated)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify change in the GPRS
 * QoS profile (IPCEPacketQoSNotifyProfileChanged).
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aContextName The context name.
 * @param aQoSGPRSNegotiated Context negotiated GPRS QOS profile to be returned to the CTSY Dispatcher.
 *
 * @see RPacketQoS::NotifyProfileChanged()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyQosProfileChange(aError,
			aContextName, aQoSGPRSNegotiated);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyQosProfileChangedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyQosProfileChangedR99Ind(
	TInt aError, const TDesC& aContextName,
	const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify change in the R99R4
 * QoS profile (IPCEPacketQoSNotifyProfileChanged).
 *
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aContextName context name to be returned to the CTSY Dispatcher.
 * @param aQoSR99_R4Negotiated context negotiated R99 QOS profile to be returned to the CTSY Dispatcher.
 *
 * @see RPacketQoS::NotifyProfileChanged()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyQosProfileChange(aError, aContextName, aQoSR99_R4Negotiated);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyQosProfileChangedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyQosProfileChangedR5Ind(
	TInt aError, const TDesC& aContextName,
	const RPacketQoS::TQoSR5Negotiated& aQoSR5Negotiated)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify change in the R5 QoS
 * profile (IPCEPacketQoSNotifyProfileChanged).
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName context name to be returned to the CTSY Dispatcher.
 * @param aQoSR99_R4Negotiated context negotiated R5 QOS profile to be returned to the CTSY Dispatcher.
 *
 * @see RPacketQoS::NotifyProfileChanged()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyQosProfileChange(aError, aContextName, aQoSR5Negotiated);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyQosProfileChangedInd


EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyRatTransferCapsChangeInd(TInt aError, TDynamicTransferCapsFlags aCaps)
/**
 *
 *
 * Callback function to be used by the Licensee LTSY to notify change in the transfer
 * capabalites (EPacketNotifyTransferCapsIPC).
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCaps the capabilities flag
 *
 * @see RPacketContext::NotifyDynamicCapsChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyRatTransferCapsChange(aError, aCaps);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyRatTransferCapsChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketMsClassChangeInd(
	TInt aError, RPacketService::TMSClass aMSClass)
/**
 * Callback function to be used by the Licensee LTSY to notify change in
 * the MS class (EPacketNotifyMSClassChange).
 *
 * @param aError The error code.
 * @param aMSClass MS class to be returned to the CTSY Dispatcher.
 *
 * @see RPacketService::NotifyMSClassChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyPacketMsClassChange(aError, aMSClass);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketMsClassChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketDynamicCapsChangeInd(
	TInt aError, TBool aGprsIsSupportedOnCell, TBool aCsServicesIsSupportedOnCell)
/**
 * Callback function to be used by the Licensee LTSY to notify change in the
 * Dynamic capabalites (EPacketNotifyDynamicCapsChange).
 *
 * @param aError The error code.
 * @param aGprsIsSupportedOnCell Indication whether GPRS is supported and should be returned to the CTSY Dispatcher.
 * @param aCsServicesIsSupportedOnCell Indication whether CS services are supported and should be returned to the CTSY Dispatcher.
 *
 * @see RPacketService::NotifyDynamicCapsChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aGprsIsSupportedOnCell=%d, aCsServicesIsSupportedOnCell=%d"), aError, aGprsIsSupportedOnCell, aCsServicesIsSupportedOnCell);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyPacketDynamicCapsChange(aError, aGprsIsSupportedOnCell, aCsServicesIsSupportedOnCell);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketDynamicCapsChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextAddedInd(TInt aError,
		const TDesC& aContextName)
/**
 * Callback function to be used by the Licensee LTSY to notify an added
 * context(EPacketNotifyContextAdded).
 *
 * @param aError The error code.
 * @param aContextName PDP context name to be returned to the CTSY Dispatcher.
 *
 * @see RPacketService::NotifyContextAdded()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyPdpContextAdded(aError, aContextName);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextAddedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(
	TInt aError, const RPacketContext::TContextConfigGPRS& aContextConfigGPRS)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify context
 * activation requested (IPC EPacketNotifyContextActivationRequested)
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextConfigGPRS The GPRS context configuration to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPdpType=%d"), aError, aContextConfigGPRS.iPdpType);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyNetworkInitiatedContextActivationRequest(aError, aContextConfigGPRS);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(
	TInt aError, const RPacketContext::TContextConfigR99_R4& aContextConfigR99_R4)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify context
 * activation requested (IPC EPacketNotifyContextActivationRequested)
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextConfigR99_R4 The release 99 or release 4 context configuration to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPdpType=%d"), aError, aContextConfigR99_R4.iPdpType);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyNetworkInitiatedContextActivationRequest(aError, aContextConfigR99_R4);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(
	TInt aError, const RPacketContext::TContextConfig_R5& aContextConfig_R5)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify context
 * activation requested (IPC EPacketNotifyContextActivationRequested)
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextConfig_R5 The release 5 context configuration to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPdpType=%d"), aError, aContextConfig_R5.iPdpType);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyNetworkInitiatedContextActivationRequest(aError, aContextConfig_R5);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd



EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextConfigChangedInd(
	TInt aError, const TDesC& aContextName, const TDesC8& aAccessPointName,
	const TDesC8& aPdpAddress, const TUint aPdpCompression, const TDesC8& aPcoBuffer)
/**
 * Callback function to be used by the Licensee LTSY to notify change in the context
 * configuration (IPC EPacketContextNotifyConfigChanged)
 *
 * @param aError The error code.
 * @param aContextName The name of the context which the change in configuration refers to.
 * @param aAccessPointName The name of the access point. This should be a descriptor of maximum length
 * RPacketContext::KGSNNameLength such as a RPacketContext::TGSNAddress
 * @param aPdpAddress The protocol address. This should be a descriptor of maximum length
 * RPacketContext::KMaxPDPAddressLength such as a RPacketContext::TProtocolAddress
 * @param aPdpCompression The PDP compression level.
 * @param aPcoBuffer The PCO buffer.
 *
 * The purpose of the protocol configuration options (PCO) is to transfer external network protocol options
 * associated with a PDP context activation, and transfer additional (protocol) data
 * (e.g. configuration parameters, error codes or messages/events) associated with an external protocol
 * or an application.
 * The protocol configuration options (PCO) is a type 4 information element with a minimum length of 3
 * octets and a maximum length of 253 octets
 * In order to generate the PCO buffer a TTlvStruct object is being used. The TTlvStruct wraps the buffers
 * inserted to the PCO and identifies the buffers with given IDs.
 *
 * RPacketContext::KMiscProtocolBufferLength such as a RPacketContext::TMiscProtocolBuffer
 *
 * @see RPacketContext::NotifyConfigChanged()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));

	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyPdpContextConfigChanged(aError, aContextName,
			aAccessPointName, aPdpAddress, aPdpCompression, aPcoBuffer);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextConfigChangedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextStatusChangeInd(
	TInt aError, const TDesC& aContextName, const TContextMisc& aContextMisc)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify context status
 * change (IPC EPacketContextNotifyStatusChange).
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName Context name, the maximum length of the descriptor should not exceed KMaxInfoName.
 * @param aContextMisc Context miscellaneous information.
 *
 * @see RPacketContext::NotifyStatusChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyPdpContextStatusChange(aError, aContextName, aContextMisc);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextStatusChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextConnectionSpeedChangeInd(
	TInt aError, const TDesC& aContextName, TUint aConnectionSpeed)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify context connection speed
 * change (IPC EPacketContextNotifyConnectionSpeedChange).
 *
 * @param aError The error code to be returned.
 * @param aContextName context name.
 * @param aConnectionSpeed context connection speed in bits per second.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aConnectionSpeed=%d"), aError, &aContextName, aConnectionSpeed);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyPdpContextConnectionSpeedChange(aError, aContextName, aConnectionSpeed);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextConnectionSpeedChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketNetworkRegistrationStatusInd(
	TInt aError, RPacketService::TRegistrationStatus aRegistrationStatus)
/**
 * Callback function to be used by the Licensee LTSY to notify change in the status of the
 * packet network registration (IPC EPacketNotifyChangeOfNtwkRegStatus).
 *
 * @param aError The error code.
 * @param aRegistrationStatus Network registration status.
 *
 * @see RPacketService::NotifyChangeOfNtwkRegStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRegistrationStatus=%d"), aError, aRegistrationStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyPacketNetworkRegistrationStatus(aError, aRegistrationStatus);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketNetworkRegistrationStatusInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyMbmsContextConfigChangedInd(
		TInt aError, const TDesC& aContextName, const RPacketMbmsContext::TContextConfigMbmsV1& aContextConfigMbms)
/**
 *
 * Callback function to be used by the Licensee LTSY to notify the Mbms configuration has
 * changed on a given context. (IPC ECtsyPacketMbmsContextNotifyConfigChanged).
 * 
 * Pending client side RPacketMbmsContext::NotifyConfigChanged() will be completed with the new
 * configuration data.
 *
 * @param aError The error code to be returned.
 * @param aContextName The context name.
 * @param aContextConfigMbms The context configuration data.
 * 
 * @see RPacketMbmsContext::NotifyConfigChanged()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, sId=%d, mcc=%d, mnc=%d, ab=%d, sp=%d, sm=%d, sf=%d"), 
			aError, 
			&aContextName, 
			aContextConfigMbms.iTmgi.GetServiceId(),  
			aContextConfigMbms.iTmgi.GetMCC(),
			aContextConfigMbms.iTmgi.GetMNC(),
			aContextConfigMbms.iMbmsAccessBearer,
			aContextConfigMbms.iMbmsServicePriority,
			aContextConfigMbms.iMbmsServiceMode,
			aContextConfigMbms.iMbmsSessionFlag);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyMbmsContextConfigChanged(aError, aContextName, aContextConfigMbms);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketNetworkRegistrationStatusInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyMbmsNetworkServiceStatusChangeInd(
		TInt aError, TMbmsNetworkServiceStatus aMbmsNetworkServiceStatus)
/**
 * 
 * Callback function to be used by the Licensee LTSY to update the CTSY's cache of the current
 * Mbms network service state (IPC EPacketNotifyMbmsNetworkServiceStatusChange).
 * 
 * This will result in the completion on a pending client side RPacketService::NotifyMbmsNetworkServiceStatusChange()
 *
 * @param aError The error code to be returned.
 * @param aMbmsNetworkServiceStatus The network service status.
 * 
 * @see RPacketService::NotifyMbmsNetworkServiceStatusChange()
 * @see RPacketService::GetMbmsNetworkServiceStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMbmsNetworkServiceStatus=%d"), aError, aMbmsNetworkServiceStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyMbmsNetworkServiceStatusChange(aError, aMbmsNetworkServiceStatus);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyMbmsNetworkServiceStatusChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyMbmsServiceAvailabilityChangeInd(
		TInt aError, const RArray<TUint>& aAvailableServiceIds)
/**
 * 
 * Callback function to be used by the Licensee LTSY to update the CTSYs MBMS monitor service list availabilty state
 * (IPC EPacketNotifyMbmsServiceAvailabilityChange).
 * 
 * Services in the supplied list will be marked as available in the CTSYs MBMS monitor service list, services not in
 * the list will be marked as un-available.
 * 
 * This will result in the completion on a pending client side RPacketService::NotifyMbmsServiceAvailabilityChange()
 *
 * @param aError The error code to be returned.
 * @param aAvailableServiceIds The list of Service Ids which are currently available.
 * 
 * @see RPacketService::NotifyMbmsServiceAvailabilityChange()
 * @see CRetrievePcktMbmsMonitoredServices()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAvailableServiceIds.Count()=%d"), aError,  aAvailableServiceIds.Count());

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyMbmsServiceAvailabilityChange(aError, aAvailableServiceIds);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyMbmsServiceAvailabilityChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesNotifyConnectionInfoChangeInd(
		TInt aError, const TDesC& aContextName, const RPacketContext::TConnectionInfoV1& aConnectionInfo)
/**
 * 
 * Callback function to be used by the Licensee LTSY to update the packet context connection info.
 * 
 * Updated connection information will be returned to the client as a result of this indication
 * 
 * This will result in the completion on a pending client side RPacketService::NotifyConnectionInfoChange()
 *
 * @param aError The error code to be returned.
 * @param aContextName The context name, the maximum length of the descriptor should not exceed KMaxInfoName.
 * @param aConnectionInfo The details of the connection, including HSDPA Category and HSUPA Category
 * 
 * @see RPacketService::NotifyConnectionInfoChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, iHSDPACategory=%d, iHSUPACategory=%d"), 
	    aError, &aContextName, aConnectionInfo.iHSDPACategory, aConnectionInfo.iHSUPACategory);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackNotifyConnectionInfoChange(aError, aContextName, aConnectionInfo);

	} // CCtsyDispatcherCallback::CallbackPacketServicesNotifyConnectionInfoChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesPacketAttachComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesPacketAttach::HandlePacketAttachReqL()
 *
 * Informs the CTSY of the whether the request to attach to the packet service
 * was successful.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RPacketService::Attach()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackPacketAttach(aError);

	} // CCtsyDispatcherCallback::CallbackPacketServicesPacketAttachComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesGetPacketAttachModeComp(
	TInt aError, RPacketService::TAttachMode aAttachMode)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesGetPacketAttachMode::HandleGetPacketAttachModeReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aAttachMode The packet service attach mode.
 *

 *
 * @see RPacketService::GetAttachMode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAttachMode=%d"), aError, aAttachMode);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackGetPacketAttachMode(aError, aAttachMode);

	} // CCtsyDispatcherCallback::CallbackPacketServicesGetPacketAttachModeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesGetPacketNetworkRegistrationStatusComp(
	TInt aError, RPacketService::TRegistrationStatus aRegistrationStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::HandleGetPacketNetworkRegistrationStatusReqL()
 *
 * Returns the current registration status of the packet network.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aRegistrationStatus The current network registration status.
 *
 * @see RPacketService::GetNtwkRegStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRegistrationStatus=%d"), aError, aRegistrationStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackGetPacketNetworkRegistrationStatus(aError, aRegistrationStatus);

	} // CCtsyDispatcherCallback::CallbackPacketServicesGetPacketNetworkRegistrationStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesPacketDetachComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesPacketDetach::HandlePacketDetachReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RPacketService::Detach()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackPacketDetach(aError);

	} // CCtsyDispatcherCallback::CallbackPacketServicesPacketDetachComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp(TInt aError,
		const TDesC& aContextName)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesSetPdpContextConfig::HandleSetPdpContextConfigReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName The context name, the maximum length of the descriptor should not exceed KMaxInfoName.
 *
 * @see RPacketContext::SetConfig()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackSetPdpContextConfig(aError, aContextName);

	} // CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesModifyActivePdpContextComp(
		TInt aError, const TDesC& aContextName, TInt8 aRejectionCode)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesModifyActivePdpContext::HandleModifyActivePdpContextReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName The context name that has been modified.
 * @param aRejectionCode The reason code for any failure of modification.
 *
 * @see RPacketContext::ModifyActiveContext()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aRejectionCode=%d"), aError, &aContextName, aRejectionCode);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackModifyActivePdpContext(aError, aContextName, aRejectionCode);

	} // CCtsyDispatcherCallback::CallbackPacketServicesModifyActivePdpContextComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesInitialisePdpContextComp(
		TInt aError, const TDesC& aContextName, const TDesC& aChannelId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesInitialisePdpContext::HandleInitialisePdpContextReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName The name of the current context, the descriptor should be TBuf<10>
 * @param aChannelId The current channelId descriptor, the descriptor should be a TName
 * @see RPacketServise::InitialiseContext()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aChannelId=%S"), aError, &aContextName, &aChannelId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackInitialisePdpContext(aError, aContextName, aChannelId);

	} // CCtsyDispatcherCallback::CallbackPacketServicesInitialisePdpContextComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesDeletePdpContextComp(
	TInt aError, const TDesC& aContextName)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesDeletePdpContext::HandleDeletePdpContextReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName The context name.
 *
 * @see RPacketContext::Delete()
 */

	{	
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackDeletePdpContext(aError, aContextName);

	} // CCtsyDispatcherCallback::CallbackPacketServicesDeletePdpContextComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesSetPacketAttachModeComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesSetPacketAttachMode::HandleSetPacketAttachModeReqL()
 *
  * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RPacketService::SetAttachMode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackSetPacketAttachMode(aError);

	} // CCtsyDispatcherCallback::CallbackPacketServicesSetPacketAttachModeComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesSetDefaultPdpContextParamsComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesSetDefaultPdpContextParams::HandleSetDefaultPdpContextParamsReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RPacketContext::SetDefaultContextParams()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackSetDefaultPdpContextParams(aError);

	} // CCtsyDispatcherCallback::CallbackPacketServicesSetDefaultPdpContextParamsComp


EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesActivatePdpContextComp(
	TInt aError, const TDesC& aContextName,
	const TDesC8& aAccessPointName,
	RPacketContext::TProtocolType aPdpType)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesActivatePdpContext::HandleActivatePdpContextReqL()
 *
 * Returns the result of the request to activate a packet context and returns
 * details about the activated context to the CTSY.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName The name of the context that has been activated.
 * @param aAccessPointName The name of the access point that has been activated.
 * This should be a descriptor of maximum length RPacketContext::KGSNNameLength such as RPacketContext::TGSNAddress
 * @param aPdpType The protocol used to connect to the packet data gateway.
 * 
 *
 * @see RPacketContext:Activate()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aAccessPointName=%S, aPdpType=%d"), aError, &aContextName, &aAccessPointName, aPdpType);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));

	iDispatcherHolder->GetPacketServicesDispatcher().CallbackActivatePdpContext(aError,
			aContextName, aAccessPointName, aPdpType);

	} // CCtsyDispatcherCallback::CallbackPacketServicesActivatePdpContextComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp(
		TInt aError, const TDesC& aContextName)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete one of the following pending
 * MLtsyDispatchPacketServicesSetPdpContextQosGprs::HandleSetPdpContextQosReqL()
 * MLtsyDispatchPacketServicesSetPdpContextQosR99R4::HandleSetPdpContextQosReqL()
 * MLtsyDispatchPacketServicesSetPdpContextQosR5::HandleSetPdpContextQosReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName The context name, the maximum length of the descriptor should not exceed KMaxInfoName.
 *
 * @see RPacketQoS::SetProfileParameters()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackSetPdpContextQos(aError, aContextName);

	} // CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesRejectNetworkInitiatedContextActivationRequestComp(
	TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::HandleRejectNetworkInitiatedContextActivationRequestReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RPacketContext::RejectActivationRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackRejectNetworkInitiatedContextActivationRequest(aError);

	} // CCtsyDispatcherCallback::CallbackPacketServicesRejectNetworkInitiatedContextActivationRequestComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesDeactivatePdpContextComp(
		TInt aError, const TDesC& aContextName)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesDeactivatePdpContext::HandleDeactivatePdpContextReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName The context name that has been deactivated,
 * the maximum length of the descriptor should not exceed KMaxInfoName.
 *
 * @see RPacketContext::Deactivate()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackDeactivatePdpContext(aError, aContextName);

	} // CCtsyDispatcherCallback::CallbackPacketServicesDeactivatePdpContextComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesGetStatusComp(
	TInt aError, RPacketService::TStatus aPacketStatus, TBool aIsResumed)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesGetStatus::HandleGetStatusReqL()
 *
 * Returns the status of the packet service to the C-TSY.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPacketStatus The packet status as return from the ltsy.
 * @param aIsResumed Flag indicating whether packet service has been resumed from suspended state.
 *
 * @see RPacketService::GetStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPacketStatus=%d, aIsResumed=%d"), aError, aPacketStatus, aIsResumed);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackGetStatus(aError, aPacketStatus, aIsResumed);

	} // CCtsyDispatcherCallback::CallbackPacketServicesGetStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesGetStaticCapabilitiesComp(
	TInt aError, TUint aStaticCapabilities)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesGetStaticCapabilities::HandleGetStaticCapabilitiesReqL()
 *
 * Returns the static capabilities of the packet service to the C-TSY.
 * 
 * E-Tel Packet calls to RPacketService::GetStaticCaps() will then return this set
 * of static capabilities.
 *
 * @param aError The error code.
 * @param aStaticCapabilities A bit field containing the static capabilities to be returned to the CTSY.
 * This can be created from a bit-wise OR (|) of values from RPacketService::TStaticMiscCaps.
 * 
 * @see RPacketService::TStaticMiscCaps
 * @see RPacketService::GetStaticCaps()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStaticCapabilities=%d"), aError, aStaticCapabilities);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackGetStaticCapabilities(aError, aStaticCapabilities);

	} // CCtsyDispatcherCallback::CallbackPacketServicesGetStaticCapabilitiesComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoMonitoredServiceListsComp(
	TInt aError, TInt aMaxNoMonitoredServiceLists)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::HandleGetMaxNoMonitoredServiceListsReqL()
 * 
 * Returns the maximum number of monitored service lists to the C-TSY.
 *
 * Calls to RPacketService::EnumerateMbmsMonitorServiceList() will then return the 
 * maximum number of monitored service lists in the aMaxAllowed field.
 * 
 * @param aError The error code.
 * @param aMaxNoMonitoredServiceLists The maximum number of monitored service lists.
 * 
 * @see RPacketService::EnumerateMbmsMonitorServiceList()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMaxNoMonitoredServiceLists=%d"), aError, aMaxNoMonitoredServiceLists);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackGetMaxNoMonitoredServiceLists(aError, aMaxNoMonitoredServiceLists);

	} // CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoMonitoredServiceListsComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoActiveServicesComp(
		TInt aError, TInt aMaxNoActiveServices)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesGetMaxNoActiveServices::HandleGetMaxNoActiveServicesReqL()
 * 
 * Returns the maximum number of active services allowed to the C-TSY.
 *
 * @param aError The error code.
 * @param aMaxNoActiveServices The maximum number of active services allowed (hence the maximum number of contexts allowed).
 * 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMaxNoActiveServices=%d"), aError, aMaxNoActiveServices);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackGetMaxNoActiveServices(aError,aMaxNoActiveServices);

	} // CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoActiveServicesComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesInitialiseMbmsContextComp(
		TInt aError, const TDesC& aContextName, const TDesC& aChannelId)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesInitialiseMbmsContext::HandleInitialiseMbmsContextReqL()
 * 
 * Returns the MBMS data to the CTSY.
 *
 * @param aError The error code.
 * @param aContextName The name of the current context, the descriptor should be a maximum length of KMaxInfoName.
 * @param aChannelId The current channelId descriptor, the descriptor should be a maximum length of KMaxName.
 * 
 * @see RPacketMbmsContext::InitialiseContext()
 * 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aChannelId=%S"), aError, &aContextName, &aChannelId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackInitialiseMbmsContext(aError, aContextName, aChannelId);

	} // CCtsyDispatcherCallback::CallbackPacketServicesInitialiseMbmsContextComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesGetMbmsNetworkServiceStatusComp(
		TInt aError, TMbmsNetworkServiceStatus aMbmsNetworkServiceStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::HandleGetMbmsNetworkServiceStatusReqL()
 * 
 * The CTSYs cache of the network service state will be updated and a pending client side 
 * RPacketService::GetMbmsNetworkServiceStatus() will be completed.
 * 
 * @param aError The error code to be returned.
 * @param aMbmsNetworkServiceStatus The network service status.
 * 
 * @see RPacketService::GetMbmsNetworkServiceStatus()
 * 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMbmsNetworkServiceStatus=%d"), aError, aMbmsNetworkServiceStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackGetMbmsNetworkServiceStatus(aError, aMbmsNetworkServiceStatus);

	} // CCtsyDispatcherCallback::CallbackPacketServicesGetMbmsNetworkServiceStatusComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsMonitorServiceListComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::HandleUpdateMbmsMonitorServiceListsReqL()
 * 
 * This callback is for the success case. The CTSYs monitor service list will be updated as via the 
 * change list and action.
 * 
 * The pending client side RPacketService::NotifyMbmsServiceAvailabilityChange() will be completed with
 * KErrNone.
 * 
 * @param aError The error code to be returned.
 * 
 * @see RPacketService::UpdateMbmsMonitorServiceListL()
 * @see RPacketService::NotifyMbmsServiceAvailabilityChange()
 * @see CRetrievePcktMbmsMonitoredServices()
 * 
 */
	{
	TSYLOGENTRYEXIT;

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackUpdateMbmsMonitorServiceList(aError);

	} // CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsMonitorServiceListComp


EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsMonitorServiceListComp(TInt aError, const RArray<TUint>& aFailedServiceIds)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::HandleUpdateMbmsMonitorServiceListsReqL()
 * 
 * This callback is for the error case. The CTSYs monitor service list will be updated as via the 
 * change list and action. The failed service Ids list contains the service Id that cannot be
 * removed / added.
 * 
 * The pending client side RPacketService::NotifyMbmsServiceAvailabilityChange() will be completed with
 * KErrMbmsImpreciseServiceEntries.
 * 
 * @param aError The error code to be returned.
 * @param aFailedServiceIds A list containing the service Id which cannot be updated via the action (e.g. Id of
 * a service that could not be added)
 * 
 * @see RPacketService::UpdateMbmsMonitorServiceListL()
 * @see RPacketService::NotifyMbmsServiceAvailabilityChange()
 * @see CRetrievePcktMbmsMonitoredServices()
 * 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aFailedServiceIds.Count()=%d"), aError, aFailedServiceIds.Count());

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackUpdateMbmsMonitorServiceList(aError, aFailedServiceIds);

	} // CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsMonitorServiceListComp

EXPORT_C void CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsSessionListComp(TInt aError, const TDesC& aContextName)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchPacketServicesUpdateMbmsSessionList::HandleUpdateMbmsSessionListReqL()
 * 
 * This will complete a pending client side RPacketService::UpdateMbmsSessionList(), if an error
 * has occurred the CTSYs MBMS services table will not be altered.
 * 
 * @param aError The error code to be returned.
 * @param aContextName The name of the MBMS context, the descriptor should be a maximum length of KMaxInfoName.
 * 
 * @see RPacketService::UpdateMbmsSessionList()
 * @see CRetrievePcktMbmsSessionList()
 * 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPacketServicesDispatcher().CallbackUpdateMbmsSessionList(aError, aContextName);
	} // CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsSessionListComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatPcmdInd(
	TInt aError, const TDesC8& aData)
/**
 *
 * Proactive commands from the UICC are passed via this indicator function
 * to the Common TSY.
 * If proactive commands are received for which notifications have not been set up
 * then a Terminal Response containing the return code "KMeUnableToProcessCmd"
 * will be sent back to the UICC.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aData TLV data received from the SIM that contains the proactive command.
 *
 * @see RSat::TerminalRsp()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackPcmd(aError, aData);

	} // CCtsyDispatcherCallback::CallbackSatPcmdInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatNotifyProactiveSimSessionEndInd(
	TInt aError, TUint8 aStatusWord1, TUint8 aStatusWord2)
/**
 *
 * This indicator if called with both status words as a normal ending (sw1 = 0x90, sw2 =0x00)
 * completes an outstanding RSat::NotifyProactiveSimSessionEnd() notifier.
 *
 * The LTSY should call this when a proactive SIM session ends.
 *
 * (Note: RSat::NotifyProactiveSimSessionEnd() Introduced in version 6.1, marked as
 *	"no longer needed when using the ETelSat API" from version 8.2, thus kept in the
 *  dispatcher layer for legacy reasons only)
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatusWord1 The Status Word 1
 * @param aStatusWord2 The Status Word 2
 *
 * @see RSat::NotifyProactiveSimSessionEnd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aStatusWord1=%d,aStatusWord2=%d"), aError, aStatusWord1, aStatusWord2);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackNotifyProactiveSimSessionEnd(aError,aStatusWord1,aStatusWord2);

	} // CCtsyDispatcherCallback::CallbackSatNotifyProactiveSimSessionEndInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatTimeZoneChangeInd(TInt aError, TUint8 aTimeZone)
/**
 *
 * This indicator is used by the LTSY to communicate a new Time Zone to the Common TSY.
 * The Common TSY caches this value and uses it for PROVIDE LOCAL INFO proactive commands
 *
 * The LTSY should call this indicator when the time zone changes. If this indicator is not
 * called at least once before any PROVIDE LOCAL INFO (timezone) proactive commands are issued
 * by the SIM application, then the timezone provided to the application will be 0xff (not known).
 * Therefore the LTSY should send this up to the CTSY as soon as it knows the timezone (as well as
 * when it changes).
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aTimeZone The new time zone coded as defined in "ETSI TS 123 040" "TP-Service-Centre-Time-Stamp"
 *
 * @see RSat::NotifyLocalInfoPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTimeZone=%d"), aError, aTimeZone);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackTimeZoneChange(aError,aTimeZone);

	} // CCtsyDispatcherCallback::CallbackSatTimeZoneChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatAccessTechnologyChangeInd(TInt aError, TUint8 aCurrentAccessTechnology)
/**
 *
 * This indicator from the LTSY,
 *
 * 1) Updates the CTSYs cache of the current access technology which is used when the CTSY
 * creates local information terminal responses.
 * 2) If the Access Technology Change is part of the current event list, the CTSY sends down
 * an event download ENVELOPE command (Access Technology Change) to the LTSY constructed from the below
 * parameters
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCurrentAccessTechnology The current access technology being used by the terminal
 * See "ETSI TS 102 223" "ENVELOPE commands" "Access technology"
 *
 * @see RSat::NotifySetUpEventListPCmd()
 * @see RSat::EventDownload()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCurrentAccessTechnology=%d"), aError, aCurrentAccessTechnology);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));

	iDispatcherHolder->GetSatDispatcher().CallbackAccessTechnologyChange(aError,aCurrentAccessTechnology);
	} // CCtsyDispatcherCallback::CallbackSatAccessTechnologyChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCallConnectedInd(TInt aError, TUint8 aTransactionId, TBool aNearEnd)
/**
 *
 * This indicator from the LTSY notifies the CTSY that a call connected message has been sent or received
 * (i.e. the terminal receives an incoming call connect message (in the case of an MT call),
 * or the terminal sends an outgoing call connect message (in the case of an MO call))
 *
 * If the call connected event is part of the current event list, the CTSY sends down
 * an event download ENVELOPE command (Call Connected) to the LTSY constructed from the below
 * parameters
 *
 * In the case of a call initiated through a SET UP CALL proactive command while the call connected event is part of the
 * current event list, the terminal shall send both the TERMINAL RESPONSE related to the proactive command, and the
 * EVENT DOWNLOAD command, in the order TERMINAL RESPONSE first, ENVELOPE (EVENT DOWNLOAD - call connected) second.
 *
 * See "ETSI TS 102 223" "ENVELOPE commands" "Call connected event"
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTransactionId The Transaction Identifier in the call connect message.
 * @param aIsNearEnd If True, the case of connecting is at the near end (an MT call), (i.e. source = terminal, destination = UICC)
 * 					 If False (i.e. far end), the case of connecting is at the far end (an MO call), (i.e. source = network, destination = UICC)
 *
 * @see RSat::NotifySetUpEventListPCmd()
 * @see RSat::EventDownload()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aTransactionId=%d, aNearEnd=%d"),aError,aTransactionId,aNearEnd);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackCallConnected(aError,aTransactionId,aNearEnd);

	} // CCtsyDispatcherCallback::CallbackSatCallConnectedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatSsChangeInd(
	TInt aError, DispatcherSat::TSsStatus aStatus)
/**
 *
 * The LTSY should communicate the current availability of SS/USSD using this
 * indicator. Their state is used for the processing of the LAUNCH BROWSER
 * Proactive Command.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aStatus whether or not SS/USSD is busy
 *
 * @see RSat::NotifyLaunchBrowserPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d"), aError, aStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackSsChange(aError, aStatus);

	} // CCtsyDispatcherCallback::CallbackSatSsChangeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCallDisconnectedInd(
		TInt aError, TUint8 aTransactionId, TBool aNearEnd, const TDesC8& aCause)
/**
 *
 * This indicator from the LTSY notifies the CTSY that a call has become disconnected.
 *
 * If the call disconnected event is part of the current event list, the CTSY sends down
 * an event download ENVELOPE command (Call Disconnected) to the LTSY constructed from the below
 * paramenters.
 *
 * If the terminal initiates the disconnection, or in the case of radio link failure, this is considered a "near end"
 * disconnection, whereas a "far end" disconnection is defined as when the network initiates the disconnection.
 *
 * The technology specific toolkit specification will define the appropriate disconnect cause messages.
 *
 * See "ETSI TS 102 223" "ENVELOPE commands" "Call disconnected event"
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTransactionId The Transaction Identifier of the call being disconnected.
 * @param aIsNearEnd If True, the case of disconnection is at the near end, (i.e. source = terminal, destination = UICC)
 * 					If False (i.e. far end), the case of disconnection is at the far end, (i.e. source = network, destination = UICC)
 * @param aCause 	The cause value is defined in the appropriate access technology specification.
 * 					Radio Link Timeout is indicated by the cause having zero length, the maximum length is defined in KMaxCauseLength
 * 					See "ETSI TS 102 223" "COMPREHENSION-TLV data objects" "Cause"
 *
 * @see RSat::NotifySetUpEventListPCmd()
 * @see RSat::EventDownload()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTransactionId=%d,aNearEnd=%d"), aError,aTransactionId,aNearEnd);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackCallDisconnected(aError,aTransactionId,aNearEnd,aCause);

	} // CCtsyDispatcherCallback::CallbackSatCallDisconnectedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatMtCallInd(
	TInt aError, TUint8 aTransactionId, const TDesC8& aAddress, const TDesC8& aSubAddress)
/**
 *
 * This indicator from the LTSY notifies the CTSY that the terminal has received an
 * incoming call setup message.
 *
 * If the MT call event is part of the current event list, the CTSY sends down
 * an event download ENVELOPE command (MT call) to the LTSY constructed from the paramenters.
 *
 * The Dialling Number (part of the address) is the Calling Party number received by the
 * terminal in the call setup message. Being of length zero the address will not be included in the ENVELOPE command,
 * the maximum length is defined in KNumberMaxLength.
 *
 * The Subaddress is the Calling Party Subaddress as received by the terminal in the call setup message. Being of length zero
 * the subaddress will not be included in the ENVELOPE command
 *
 * See "ETSI TS 102 223" "ENVELOPE commands" "MT call event"
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTransactionId The Transaction Identifier in the call setup message from the network.
 * @param aAddress Holds the Calling Party number received by the terminal in the call setup message. If
 * the Calling Party number is included in the call setup message, the Address shall be included, otherwise
 * the address shall not be included  (zero length). See "ETSI TS 102 223" "COMPREHENSION-TLV data objects" "Address"
 * @param aSubAddress Holds the Calling Party Subaddress as received by the terminal in the call setup
 * message. If the Calling Party Subaddress is included in the call setup message, the Subaddress shall be included, otherwise
 * the Subaddress shall not be included (zero length). See "ETSI TS 102 223" "COMPREHENSION-TLV data objects" "Subaddress"
 *
 * @see RSat::NotifySetUpEventListPCmd()
 * @see RSat::EventDownload()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTransactionId=%d"),aError,aTransactionId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackMtCall(aError,aTransactionId,aAddress,aSubAddress);

	} // CCtsyDispatcherCallback::CallbackSatMtCallInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCallControlEnvelopeResponseInd(
	TInt aError, const TDesC8& aResponseTlv)
/**
 *
 * This indicator is used as part of the call control procedures to return the response
 * from an ENVELOPE (CALL CONTROL) to the CTSY.  The CTSY extracts an alpha identifier
 * (if any) from the TLV data and displays it appropriately when the changed call occurs.
 * The control result is used to generate the TERMINAL RESPONSE if a  SET UP CALL or SEND SS
 * proactive command originated the request that lead to Call Control.
 *
 *
 * When the LTSY receives a non-emergency call or SS request (such as via RCall::Dial() or
 * RMobileCall::SendNetworkServiceRequest() )it must first checked with the FDN list, if enabled.
 * USSD requests are not checked in the FDN list.
 *
 * If the call/SS request is allowed or it is a USSD request then the LTSY passes the details of the request
 * to Call/SS/USSD Control, if enabled, by sending an ENVELOPE (CALL CONTROL) message to the UICC.
 * The LTSY should use this indicator to pass the details of the request for which an ENVELOPE (CALL
 * CONTROL) must be constructed to the CTSY.
 *
 * When the CTSY has constructed the ENVELOPE (CALL CONTROL) this will be passed down to the LTSY via
 * CLtsySatHandler::HandleCallAndSmsControlEnvelopeReqL(). The response from the UICC is passed back to CTSY via
 * CallbackSatCallControlEnvelopeResponseInd().
 *
 *
 * For more information see ETSI TS 102 223 section 7.3.1
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aResponseTlv The response from NAA to the ENVELOPE (CALL CONTROL) message. This will be
 * parsed by the CTSY and if an alpha identifier exists in the data, it will be passed to the RSat client
 * for display to the user. As per ETSI 102.223 section 7.3.1.6, this contains 1 byte containing the Call
 * Control result, followed by a 1-2 byte length, then various optional fields depending on the outcome
 * of the Call/SS/USSD Control. If the NAA application does not provide any response data then this is
 * interpreted as "allowed, no modification", in this case the client is not notified.
 *
 *  @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RSat::NotifyCallControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackCallControlEnvelopeResponse(aError,aResponseTlv);

	} // CCtsyDispatcherCallback::CallbackSatCallControlEnvelopeResponseInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCreateCallControlEnvelopeInd(
	TInt aError, TUint8 aTag, const TDesC8& aBcc1, const TDesC8& aBcc2, 
	const TDesC8& aAddress, TUint8 aTonNpi, TUint8 aDcs)
/**
 *
 * This indicator is used by the LTSY as part of the CALL/SS/USSD CONTROL functionality to construct
 * the ENVELOPE (CALL CONTROL) that is sent to the UICC when MO calls or SS/USSD requests are initiated.
 *
 *
 * When the LTSY receives a non-emergency call or SS request (such as via RCall::Dial() or
 * RMobileCall::SendNetworkServiceRequest() )  it must first checked with the FDN list, if enabled.
 * USSD requests are not checked in the FDN list.
 *
 * If the call/SS request is allowed or it is a USSD request then the LTSY passes the details of the request
 * to Call/SS/USSD Control, if enabled, by sending an ENVELOPE (CALL CONTROL) message to the UICC.
 * The LTSY should use this indicator to pass the details of the request for which an ENVELOPE (CALL
 * CONTROL) must be constructed to the CTSY.
 *
 * When the CTSY has constructed the ENVELOPE (CALL CONTROL) this will be passed down to the LTSY via
 * CLtsySatHandler::HandleCallAndSmsControlEnvelopeReqL(). The response from the UICC is passed back to CTSY via
 * CallbackSatCallControlEnvelopeResponseInd().
 *
 *
 * For more information see ETSI TS 102 223 section 7.3.1
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTag Type of the address argument i.e. whether this is a call, SS or USSD request. 
 * Valid values are KTlvAddressTag, KTlvSsStringTag or KTlvUssdStringTag
 * @param aBcc1 Bearer capability 1. The maximum length is 16.
 * @param aBcc2 Bearer capability 2. The maximum length is 16.
 * @param aAddress the number or string being dialled. The maximum length is 255.
 * @param aTonNpi the TON/NPI (Type of Numbering / Numbering Plan Indicator) byte
 * @param aDcs Data Coding Scheme. Only applicable for USSD requests.
 *
 * @see RSat::NotifyCallControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackCreateCallControlEnvelope(aError,aTag,aBcc1,aBcc2,aAddress,aTonNpi,aDcs);

	} // CCtsyDispatcherCallback::CallbackSatCreateCallControlEnvelopeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCreateSmControlEnvelopeInd(TInt aError,
		const TDesC8& aRpDestinationAddress, const TDesC8& aTpDestinationAddress)
/**
 *
 * This indicator from the LTSY notifies the CTSY to create a MO Short Message Control ENVELOPE.
 * The CTSY then sends the ENVELOPE to the LTSY, the LTSY receives this via the
 * CLtsySatHandler::HandleCallAndSmsControlEnvelopeReqL()
 *
 * See 3GPP 11.14 Under "MO Short Message Control by SIM" for details of the Envelope created
 * See "ETSI TS 102 223" "COMPREHENSION-TLV data objects" "Address" for address details
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aRpDestinationAddress RP Destination Address of the Service Center to which the ME is proposing to send the short message.
 * @param aTpDestinationAddress TP Destination Address to which the ME is proposing to send the short message.
 *
 * @see RSat::NotifyMoSmControlRequest()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackCreateSmControlEnvelope(aError,aRpDestinationAddress,aTpDestinationAddress);

	} // CCtsyDispatcherCallback::CallbackSatCreateSmControlEnvelopeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatImsiChangedInd(TInt aError)
/**
 *
 * This indicator is a notification that the Imsi has been changed. This should be called
 * when a change has been detected.
 *
 * From 3GPP TS 11.14 Under "EFIMSI changing procedure"
 * 
 * When EFIMSI is changed via Data Download or a SIM Toolkit application and a REFRESH command 
 * is issued by the SIM the following rules apply to the SIM Toolkit and ME:
 * 
 * -	SIM Initialization. This command shall not be used if EFIMSI is changed, as the behaviour 
 * 		of the MS is unpredictable. 
 * 
 * -	File Change Notification. This command shall not be used if EFIMSI is changed, as the 
 * 		behaviour of the MS is unpredictable.
 * 
 * -	SIM Initialization and File Change Notification. If EFIMSI is part of the file change notification, 
 * 		the ME shall invoke the MM Restart procedure defined in 03.22 [28]. 
 * 
 * -	SIM Initialization and Full File Change Notification. The ME shall invoke the MM Restart procedure 
 * 		defined in 03.22 [28]. 
 * 
 * -	SIM Reset. Normal SIM Reset procedure is carried out.
 * 
 * If EFIMSI is to be updated, neither EFIMSI nor EFLOCI shall be updated in the SIM before the phase request 
 * procedure has been executed by the ME. 
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RSat::NotifyRefreshPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackImsiChanged(aError);

	} // CCtsyDispatcherCallback::CallbackSatImsiChangedInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatLocationStatusInd(
	TInt aError, TUint8 aLcnStatus, const TDesC8& aCountryCode,
	TUint16 aLcnAreaCode, TUint16 aCellId)
/**
 *
 * LTSY uses this indicator to notify the CTSY of changes of location status.
 *
 * If an appropriate SET UP EVENT LIST proactive command has previously been received,
 * and if aLcnStatus indicates "normal service", then this function also constructs an
 * appropriate ENVELOPE (EVENT DOWNLOAD) and sends to LTSY.
 *
 * According to ETSI 102.223 the location information that is provided in the
 * ENVELOPE (EVENT DOWNLOAD) varies depending upon the access technology in use,
 * however the CTSY supports only GSM location information.
 *
 * This callback is also used to cache the location information which is then used to construct
 * the Call/USSD/SS control envelopes.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aLcnStatus the current service state of the terminal -
 * 			one of the values defined in ETSI 102.223 section 8.27
 * @param aCountryCode 3 digit number uniquely identifying the country. Length of aCountryCode should
 * not be greater than 3.
 * @param aLcnAreaCode The Location Area Code uniquely identifies a Location Area
 * 			with a network.
 * @param aCellId The current cell.
 *
 * @see RSat::NotifyCallControlRequest()
 * @see RSat::NotifySetUpEventListPCmd()
 * @see RSat::EventDownload()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aLcnStatus=%d,aLcnAreaCode=%d,aCellId=%d"), aError,aLcnStatus,aLcnAreaCode,aCellId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackLocationStatus(aError, aLcnStatus, aCountryCode, aLcnAreaCode, aCellId);

	} // CCtsyDispatcherCallback::CallbackSatLocationStatusInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatRemoveEventListInd(TInt aError)
/**
 *
 * This indicator is used by the LTSY to communicate to the Common TSY to clear the current
 * events list, thus the set of enabled events will be empty.
 * 
 * See ESTI 102.223 6.4.16 Under "SET UP EVENT LIST"
 * 
 * "The list of events provided by the UICC in the last SET UP EVENT LIST
 * command shall be removed if the terminal is powered off or the UICC is removed or a reset is performed."
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @see RSat::NotifySetUpEventListPCmd()
 * @see RSat::EventDownload()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackRemoveEventList(aError);

	} // CCtsyDispatcherCallback::CallbackSatRemoveEventListInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCreateRefreshTerminalRspInd(
	TInt aError, RSat::TPCmdResult aGeneralResult, const TDesC& aResultAdditionalInfo)
/**
 *
 * This indicator is used to create a terminal response to a Refresh Pro active command initiated from the
 * LTSY rather then the RSat client. For example in an error case when the client cannot be expected to
 * create the terminal response with RSat::TerminalRsp(). The constructed terminal response is presented to the LTSY via
 * the MLtsyDispatchSatTerminalRsp::HandleTerminalRspReqL()
 *
 * @see MLtsyDispatchSatTerminalRsp::HandleTerminalRspReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aGeneralResult The General result that specifies the result and indicates appropriate UICC action (to be included in the
 * refresh terminal response result section, see ETSI 102 223 Under "Structure of TERMINAL RESPONSE" and Under "Result")
 * @param aResultAdditionalInfo Additional information (to be included in the refresh terminal response result section,
 * see ETSI 102 223 Under "Structure of TERMINAL RESPONSE" and Under "Result")
 *
 * @see RSat::NotifyRefreshPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aGeneralResult=%d"), aError, aGeneralResult);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackCreateRefreshTerminalRsp(aError,aGeneralResult,aResultAdditionalInfo);

	} // CCtsyDispatcherCallback::CallbackSatCreateRefreshTerminalRspInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatSmControlEnvelopeResponseInd(TInt aError, const TDesC8& aResponseTlv)
/**
 *
 * This indicator is used by the LTSY to communicate to the Common TSY the Mo Short Message Control
 * response from the SIM. The CTSY parses the data then completes any outstanding RSat::NotifyMoSmControlRequest() 
 * then passes the parsed data down to the LTSY via the HandleSmControlResponseDataReqL().
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aResponseTlv Response data from the SIM encoded as defined in "3GPP 11.14 - MO Short Message Control by SIM"
 *
 * @see RSat::NotifyMoSmControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackSmControlEnvelopeResponse(aError,aResponseTlv);

	} // CCtsyDispatcherCallback::CallbackSatSmControlEnvelopeResponseInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCreateSmsPpDownloadEnvelopeInd(
	TInt aError, const TDesC8& aRpOriginatingAddress, const TDesC8& aSmsDeliveryTpdu)
/**
 *
 * This indicator is used by the LTSY when the ME receives a short Message.
 *
 * If data download via SMS Point-to-point" is allocated and activated in the SIM Service Table the
 * CTSY constructs the (SMS-PP DOWNLOAD) ENVELOPE to be sent to the SIM. This is received
 * by the LTSY via the HandleSmsPpDownloadEnvelopeReqL()
 *
 * If data download via SMS Point-to-point" is NOT allocated and activated in the SIM Service Table
 * the Sms parsed and if it is to be save a EF(Sms) (RMobileSmsStore::TMobileGsmSmsEntryV1)is constructed
 * and received by the LTSY via the HandleSmsStoreReadEntryReqL()
 *
 * See "3GPP 11.14" Under "SMS-PP data download"
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aRpOriginatingAddress RP Originating Address of the Service Centre (TS Service-Centre-Address), as defined in 3GPP TS 24.011)
 * @param aSmsDeliveryTpdu The SMS-DELIVER TPDU (See "3GPP 23.040" under "SMS DELIVER type")
 *
 * @see RSat::NotifySmsPpDownload()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackSatCreateSmsPpDownloadEnvelope(aError,aRpOriginatingAddress,aSmsDeliveryTpdu);

	} // CCtsyDispatcherCallback::CallbackSatCreateSmsPpDownloadEnvelopeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCreateSmsDeliverReportInd(TInt aError)
/**
 *
 * This indicator is called once the writing of a SMS to EF(Sms) is completed. The CTSY
 * then constructs a resulting SMS DELIVER REPORT (See "3GPP 23.040" Under "SMS DELIVER REPORT type")
 *
 * Also see "3GPP TS 23.038" Under "SMS Data Coding Scheme"
 *
 * "When a mobile terminated message is Class 2 ((U)SIM-specific),
 * an  MS shall ensure that the message has been transferred to the
 * SMS data field in the (U)SIM before sending an acknowledgement to the SC"
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackCreateSmsDeliverReport(aError);

	} // CCtsyDispatcherCallback::CallbackSatCreateSmsDeliverReportInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatGetAccessTechnologyComp(
	TInt aError, TUint8 aAccessTechnology)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatGetAccessTechnology::HandleGetAccessTechnologyReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aAccessTechnology the current access technology.
 * See "ETSI TS 102 223" "ENVELOPE commands" "Access technology"
 *
 * @see RSat::NotifyLocalInfoPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aAccessTechnology=%d"), aError, aAccessTechnology);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackGetAccessTechnology(aError, aAccessTechnology);

	} // CCtsyDispatcherCallback::CallbackSatGetAccessTechnologyComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatCreateCellBroadcastEnvelopeInd(TInt aError, const TDesC8& aCellBroadcastPage)
/**
 * Once the ME receives a new Cell Broadcast message and the service, if "data download via SMS-CB" is
 * allocated and activated in the SIM Service Table and if the message identifier of the Cell Broadcast
 * is contained within the message identifiers of the EF(CBMID), the cell broadcast page is passed
 * to the SIM using the ENVELOPE (CELL BROADCAST DOWNLOAD) command.
 *
 * This indicator is called to create a ENVELOPE (CELL BROADCAST DOWNLOAD) command using the
 * parameters provided, The ENVELOPE can be received in HandleCellBroadcastEnvelopeReqL().
 *
 * See "3GPP TS 11.14" Under "Cell Broadcast data download"
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCellBroadcastPage The Cell Broadcast page, formatted in the same way as described in 3GPP TS 23.041
 *
 * @see RSat::NotifyCbDownload()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackCreateCellBroadcastEnvelope(aError,aCellBroadcastPage);

	} // CCtsyDispatcherCallback::CallbackSatCreateCellBroadcastEnvelopeInd

EXPORT_C void CCtsyDispatcherCallback::CallbackSatGetImageInstanceComp(
	TInt aError, const TDesC8& aImageInstanceData, const TDesC8& aImageInstanceBody)
/**
 *
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatGetImageInstance::HandleGetImageInstanceReqL()
 *
 * The aImageInstanceData with the aImageInstanceBody appended completes the EF(IIDF)
 * (IIDF - Image instance data file)
 *
 * @param aError 				KErrNone on success, or another error code indicating the error otherwise.
 * @param aImageInstanceData 	Descriptor containing the image instance coding scheme of the EF(IIDF),
 * 								this must be of length 2 (KBasicIconDataLength) for basic
 * 								or length 6 (KColourIconDataLength) for colour image instances.
 * 								See ETSI TS 131 102 under "Annex B (normative) Image Coding Schemes (B.1 & B.2)"
 * @param aImageInstanceBody	Descriptor containing the image instance body of the EF(IIDF)
 * 								See ETSI TS 131 102 under "Annex B (normative) Image Coding Schemes"
 *
 * @see RSat::GetImageInstance()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackGetImageInstance(aError,aImageInstanceData,aImageInstanceBody);

	} // CCtsyDispatcherCallback::CallbackSatGetImageInstanceComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatGetIconDataComp(
	TInt aError, const TDesC8& aIconEfImgRecord)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatGetIconData::HandleGetIconDataReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aIconEfImgRecord 	The icon EF(Img) record retrieved from the UICC
 * 							See ETSI TS 131 102 under "EFIMG (Image)"
 *
 * @see RSat::GetIcon()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackGetIconData(aError,aIconEfImgRecord);

	} // CCtsyDispatcherCallback::CallbackSatGetIconDataComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatGetClutComp(
	TInt aError, const TDesC8& aClut)
/**
 *
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatGetClut::HandleGetClutReqL()
 *
 * @param aError 	KErrNone on success, or another error code indicating the error otherwise.
 * @param aClut 	The requested CLUT (Colour look up table) for a given icon instance
 * 					See ETSI TS 131 102 under "B.2 Colour Image Coding Scheme"
 *
 * @see RSat::GetClut()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackGetClut(aError,aClut);

	} // CCtsyDispatcherCallback::CallbackSatGetClutComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatGetDefaultBearerCapabilityComp(
	TInt aError, const TDesC8& aCapability)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatGetDefaultBearerCapability::HandleGetDefaultBearerCapabilityReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aCapability The default bearer capability configuration (coding as defined by the access technologies),
 * this will be used by the CTSY when creating a Call Control ENVELOPE (the capability configuration parameters 1)
 * if a zero length capability parameters is passed when creating the Envelope.
 * Note the maximum length capability the CTSY can store is 248, and a maximum length of 16 will be used in the
 * creation of the Call Control ENVELOPE
 * See "ETSI TS 102 223" "Structure of ENVELOPE (CALL CONTROL)"
 *
 * @see RSat::NotifyCallControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackGetDefaultBearerCapability(aError, aCapability);

	} // CCtsyDispatcherCallback::CallbackSatGetDefaultBearerCapabilityComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatGetSmsPpDownloadSupportedComp(
	TInt aError, TBool aSupported)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatGetSmsPpDownloadSupported::HandleGetSmsPpDownloadSupportedReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSupported If the service "data download via SMS Point-to-point" is allocated and activated in the SIM Service Table
 *
 * @see RSat::NotifySmsPpDownload()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSupported=%d"), aError, aSupported);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackGetSmsPpDownloadSupported(aError, aSupported);

	} // CCtsyDispatcherCallback::CallbackSatGetSmsPpDownloadSupportedComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatGetSmsControlActivatedComp(
	TInt aError, TBool aActivated)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatGetSmsControlActivated::HandleGetSmsControlActivatedReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aActivated ETrue if MO-SMS Control by USIM (Service n°31) is currently activated
 * in the USIM Service Table (EF-UST), EFalse otherwise. See 3GPP TS 31.102 section 4.2.8.
 *
 * @see RSat::NotifyMoSmControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aActivated=%d"), aError, aActivated);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackGetSmsControlActivated(aError, aActivated);

	} // CCtsyDispatcherCallback::CallbackSatGetSmsControlActivatedComp


EXPORT_C void CCtsyDispatcherCallback::CallbackSatTimingAdvanceComp(
	TInt aError, TUint8 aTimingAdvance, TUint8 aMeStatus)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatTimingAdvance::HandleTimingAdvanceReqL()
 *
 * This information is used for the PROVIDE LOCAL INFORMATION proactive command.
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTimingAdvance a value relating to the distance between the ME and the base station. The ME shall
 * return the timing advance value that was received from the BTS during the last active dedicated connection
 * (e.g. for call or SMS). Timing advance is defined in TS 04.08 [8].
 * @param aMeStatus current status of ME (i.e. ME is in idle mode or not) - in order for the application to
 * be aware of potential misinterpretation of the timing advance value.
 *
 * @see RSat::NotifyLocalInfoPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aTimingAdvance=%d,aMeStatus=%d"), aError,aTimingAdvance,aMeStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackTimingAdvance(aError, aTimingAdvance, aMeStatus);

	} // CCtsyDispatcherCallback::CallbackSatTimingAdvanceComp


EXPORT_C void CCtsyDispatcherCallback::CallbackSatProvideLocationInfoComp(
	TInt aError, TUint16 aLocationAreaCode, TUint16 aCellId,
	const TDesC8& aOperatorCode)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatProvideLocationInfo::HandleProvideLocationInfoReqL()
 *
 * This completion must have been occurred before Call/USSD/SS control can
 * be expected to work correctly.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aLocationAreaCode the ID of the Location Area (grouping of multiple cells)
 * that the mobile is in
 * @param aCellId Id of the current GSM cell
 * @param aOperatorCode Id of the current operator. This has a maximum length of 3.
 *
 * @see RSat::NotifyLocalInfoPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aLocationAreaCode=%d,aCellId=%d"), aError, aLocationAreaCode, aCellId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));

	iDispatcherHolder->GetSatDispatcher().CallbackProvideLocationInfo(aError, aLocationAreaCode, aCellId, aOperatorCode);

	} // CCtsyDispatcherCallback::CallbackSatProvideLocationInfoComp


EXPORT_C void CCtsyDispatcherCallback::CallbackSatSetPollingIntervalComp(
	TInt aError, TUint8 aPollingInterval)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatSetPollingInterval::HandleSetPollingIntervalReqL()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPollingInterval The polling interval in use by the LTSY (zero) indicate Polling is off.
 *
 * @see RSat::NotifyPollingIntervalPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aPollingInterval=%d"), aError, aPollingInterval);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackSetPolling(aError,aPollingInterval);

	} // CCtsyDispatcherCallback::CallbackSatSetPollingComp


EXPORT_C void CCtsyDispatcherCallback::CallbackSatLocalInformationNmrComp(
	TInt aError, const TDesC8& aNmr, const TDesC& aBcchList)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchSatLocalInformationNmr::HandleLocalInformationNmrReqL()
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aNmr Network Measurement Result (signal strength of BCCH of adjacent cells). Length of aNmr should 
 * not be greater than 16.
 * @param aBcchList Broadcast Control Channels of adjacent cells. Length of aBcchList should not be greater than 32.
 *
 * @see RSat::NotifyLocalInfoPCmd()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackLocalInformationNmr(aError, aNmr,aBcchList);

	} // CCtsyDispatcherCallback::CallbackSatLocalInformationNmrComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatGetUssdControlSupportedComp(TInt aError, TBool aSupported)
/**
 *
 * Callback function to indicate whether USSD TLVs are supported for control.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSupported ETrue if USSD control TLV is supported, EFalse otherwise.
 *
 * @see RSat::NotifyCallControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSupported=%d"), aError,aSupported);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackUssdControlSupported(aError, aSupported);
	} // CCtsyDispatcherCallback::CallbackSatGetUssdControlSupportedComp

EXPORT_C void CCtsyDispatcherCallback::CallbackSatTerminalRspComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a
 * MLtsyDispatchSatTerminalRsp::HandleTerminalRspReqL()
 * back to the CTSY Dispatcher.
 * 
 * Successful completion of the callbak (i.e. KErrNone) allows timer expiration envelopes (if any)
 * from the SIM ATK CTSY to be sent. No timer expiration envelopes can be sent while a 
 * Pro-active command is currently on-going. 
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RSat::TerminalRsp()
 * @see MLtsyDispatchSatTimerExpirationEnvelope::HandleTimerExpirationEnvelopeReqL()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackTerminalRsp(aError);
	}

EXPORT_C void CCtsyDispatcherCallback::CallbackSatRefreshAllowedComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a
 * MLtsyDispatchSatRefreshAllowed::HandleRefreshAllowedReqL()
 * back to the CTSY Dispatcher.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * This error code will be what the client side outstanding RSat::NotifyRefreshPCmd() is 
 * completed with.
 *
 * @see RSat::RefreshAllowed()
 * @see RSat::NotifyRefreshPCmd()
 * @see RSat::NotifyRefreshRequired()
 * @see RMmCustomAPI::SatRefreshCompleteNotification()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackRefreshAllowed(aError);
	}

EXPORT_C void CCtsyDispatcherCallback::CallbackSatReadyComp(TInt aError)
/**
 *
 * Callback function to be used by the Licensee LTSY to complete a
 * MLtsyDispatchSatReady::HandleReadyReqL()
 * back to the CTSY Dispatcher.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackReady(aError);
	}

EXPORT_C void CCtsyDispatcherCallback::CallbackSatUssdControlEnvelopeErrorComp(TInt aError)
/**
 * 
 * Callback function to be used by the Licensee LTSY to complete a 
 * MLtsyDispatchSatUssdControlEnvelopeError::HandleUssdControlEnvelopeErrorReqL()
 * back to the CTSY Dispatcher.
 * 
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * 
 * @see RSat::NotifyCallControlRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetSatDispatcher().CallbackUssdControlEnvelopeError(aError);
	}

EXPORT_C void CCtsyDispatcherCallback::CallbackPhoneNotifyRfStatusChangeInd(TInt aError, TRfStateInfo aRfStatus)
/**
 *
 * Callback function to indicate changes to the RF status.
 * Status values defined by TRfStateInfo.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aStatus RF status.  See TRfStateInfo.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetPhoneDispatcher().CallbackNotifyRfStatusChange(aError, aRfStatus);
	} // CCtsyDispatcherCallback::CallbackPhoneNotifyRfStatusChangeInd


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlDialDataComp(
	TInt aError, TInt aCallId)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlDialData::HandleDialDataReqL()
 *
 * When a dial attempt is made via the above mentioned interface, the LTSY has the
 * responsibility of allocating a unique Call ID to the call.  For more
 * information, see the documentation on aCallId below.
 *
 * For dealing with voice calls, see
 * CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher by the LTSY
 * indicating the outcome of the attempt to dial the requested call.  This should
 * be KErrNone if the dialling process could be started successfully.  Otherwise
 * another error code to indicate the failure should be returned.
 *
 * @param aCallId The call ID of the call that the dial attempt refers to which
 * has been allocated by the LTSY.  This should be a number between 1 and
 * KMaxCallIdValue and should be unique across all call modes (voice, data etc.).
 * For example, if a request for a voice call is made and the LTSY allocates
 * this call an ID of 1 and a request for a data call is requested, the LTSY
 * should allocate a Call ID which is not the same as that already allocated for
 * the first voice call.
 *
 * @see RCall::Dial()
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackDialData(aError, aCallId);

	} // CCtsyDispatcherCallback::CallbackCallControlDialDataComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetAlsPpSupportComp(
	TInt aError, RMmCustomAPI::TAlsSupport aAlsSupport)
/**
 *
 * Callback function to indicate whether the product profile support ALS (Alternate Line Service). 
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aAlsSupport RMmCustomAPI::EAlsSupportOn if ALS is supported, RMmCustomAPI::EAlsSupportOff if not.
 *
 * @see RMmCustomAPI::CheckAlsPpSupport()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAlsStatus=%d"), aError, aAlsSupport);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetAlsPpSupport(aError, aAlsSupport);

	} // CCtsyDispatcherCallback::CallbackCallControlGetAlsPpSupportComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetAlsBlockedStatusComp(
		TInt aError, RMmCustomAPI::TGetAlsBlockStatus aAlsStatus)
/**
 *
 * Callback function to indicate the current ALS blocked status. 
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aAlsStatus The return value from the LTSY.
 *
 * @see RMmCustomAPI::GetAlsBlocked()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAlsStatus=%d"), aError, aAlsStatus);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetAlsBlockedStatus(aError, aAlsStatus);

	} // CCtsyDispatcherCallback::CallbackCallControlGetAlsBlockedStatusComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlSetAlsBlockedComp(
	TInt aError)
/**
 *
 * Callback function to indicate the current ALS blocked status change is complete. 
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::SetAlsBlocked()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackSetAlsBlocked(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlSetAlsBlockedComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp(
	TInt aError)
/**
 *
 * Callback function to indicate a completion of a RMmCustomAPI::GetLifeTime() request in case
 * the lifetime information is not available.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::GetLifeTime()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetLifeTime(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp(
	TInt aError, TUint32 aHours, TUint8 aMinutes)
/**
 *
 * Callback function to indicate the phone lifetime information when the lifetime information
 * does not includes the phone manufacturing date but only the total amount of airtime use from the manufacturing date
 * until the call to RMmCustomAPI::GetLifeTime().
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aHours The amount of hours of airtime use since the manufacturing date. The range of the value should be 0-999999.
 * @param aMinutes The amount of minutes in addition to the amount of hours represented by aHours. The range of the value is 0-59.
 *
 * @see RMmCustomAPI::GetLifeTime()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aHours=%d, aMinutes=%d"), aError, aHours, aMinutes);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetLifeTime(aError, aHours, aMinutes);

	} // CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp(
	TInt aError, const TDateTime &aManufacturingDate)
/**
 *
 * Callback function to indicate the phone lifetime information when the lifetime information
 * includes the phone manufacturing date but not the total amount of airtime use from the manufacturing date
 * until the call to RMmCustomAPI::GetLifeTime().
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aManufacturingDate The date of phone manufacturing. Only the Year, Month and day information is meaningful.
 *
 * @see RMmCustomAPI::GetLifeTime()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, Year()=%d, Month()=%d, Day()=%d"), aError,
			aManufacturingDate.Year(), aManufacturingDate.Month(), aManufacturingDate.Day());

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetLifeTime(aError, aManufacturingDate);

	} // CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp(
	TInt aError, const TDateTime &aManufacturingDate, TUint32 aHours, TUint8 aMinutes)
/**
 *
 * Callback function to indicate the phone lifetime information when the lifetime information
 * includes the phone manufacturing date and the total amount of airtime use from the manufacturing date
 * until the call to RMmCustomAPI::GetLifeTime().
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aManufacturingDate The date of phone manufacturing. Only the Year, Month and day information is meaningful.
 * @param aHours The amount of hours of airtime use since the manufacturing date. The range of the value should be 0-999999.
 * @param aMinutes The amount of minutes in addition to the amount of hours represented by aHours. The range of the value is 0-59.
 *
 * @see RMmCustomAPI::GetLifeTime()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aHours=%d, aMinutes=%d, Year()=%d, Month()=%d, Day()=%d"), aError, aHours, aMinutes,
			aManufacturingDate.Year(), aManufacturingDate.Month(), aManufacturingDate.Day());
	
	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetLifeTime(aError, aManufacturingDate, aHours, aMinutes);

	} // CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlTerminateErrorCallComp(
	TInt aError)
/**
 *
 * Callback function to indicate that the request to terminate the call is complete.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::TerminateCall()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackTerminateErrorCall(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlTerminateErrorCallComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlTerminateAllCallsComp(
	TInt aError)
/**
 *
 * Callback function to indicate that the request to terminate all the calls is complete.
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 *
 * @see RMmCustomAPI::TerminateCall()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackTerminateAllCalls(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlTerminateAllCallsComp


EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlGetCallForwardingIndicatorComp(
	TInt aError, RMobilePhone::TMobileTON aTypeOfNumber,
    RMobilePhone::TMobileNPI aMobilePlan,
    const TDesC &aNumber,
    RMobilePhone::TCFUIndicatorStatusFlags aCFUIndicatorStatusFlags,
    RMobilePhone::TMultipleSubscriberProfileID aMultipleSubscriberProfileId)
/**
 * Complete a MLtsyDispatchCallControlGetCallForwardingIndicator::HandleGetCallForwardingIndicatorL request by returning 
 * to the CTSY the indicator parameters for unconditional call forwarding, with support
 * for MPS (Multiple Subscriber Profile) and Call Forwarding Number. The MPS Supplementary Service
 * is detailed in 3GPP TS 23.097, and the indicator data is detailed in 3GPP TS 31.102 (section 4.2.64 in version 8.2.0).
 *
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTypeOfNumber The call forwarding type of number.
 * @param aMobilePlan The call forwarding mobile plan.
 * @param aNumber The call forwarding telephone number.
 * @param aCFUIndicatorStatusFlags The setting for indicator status. Can contain a number of flags from RMobilePhone::TCFUIndicatorStatus.
 * @param aMultipleSubscriberProfileID The multiple subscriber profile ID (unknown, one, two, three or four).
 *
 * @see RMmCustomAPI::GetIccCallForwardingIndicatorStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, Flags=%d, ProfileId=%d"), aError, aCFUIndicatorStatusFlags, aMultipleSubscriberProfileId);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackGetCallForwardingIndicator(aError, aTypeOfNumber, aMobilePlan, aNumber, aCFUIndicatorStatusFlags, aMultipleSubscriberProfileId);

	} // CCtsyDispatcherCallback::CallbackCallControlGetCallForwardingIndicatorComp

EXPORT_C void CCtsyDispatcherCallback::CallbackCallControlUpdateLifeTimerComp(
	TInt aError)
/**
 * Callback function to be used by the Licensee LTSY to complete a pending
 * MLtsyDispatchCallControlDialData::HandleUpdateLifeTimerReqL()
 *
 * This callback is invoked after the life timer has been updated.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher. This should
 * be KErrNone if the life timer was updated successfully, otherwise another error code 
 * to indicate the failure should be returned.
 *
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Forward completion to correct dispatcher object to handle
	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));
	iDispatcherHolder->GetCallControlDispatcher().CallbackUpdateLifeTimer(aError);

	} // CCtsyDispatcherCallback::CallbackCallControlUpdateLifeTimerComp

