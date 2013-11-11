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
//

/**
 @file
 @internalAll 
*/

#ifndef __CCALLCONTROLDISPATCHER_H_
#define __CCALLCONTROLDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>
#include <ctsy/rmmcustomapi.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>


#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

// CONSTANTS
const TInt KInvalidCallId = -1;

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchCallControlAnswer;
class MLtsyDispatchCallControlHold;
class MLtsyDispatchCallControlDialEmergency;
class MLtsyDispatchCallControlStopDtmfTone;
class MLtsyDispatchCallControlSetActiveAlsLine;
class MLtsyDispatchCallControlSendDtmfTonesCancel;
class MLtsyDispatchCallControlHangUp;
class MLtsyDispatchCallControlResume;
class MLtsyDispatchCallControlSetDynamicHscsdParams;
class MLtsyDispatchCallControlDialVoice;
class MLtsyDispatchCallControlTransfer;
class MLtsyDispatchCallControlSendDtmfTones;
class MLtsyDispatchCallControlGetIdentityServiceStatus;
class MLtsyDispatchCallControlSwap;
class MLtsyDispatchCallControlLoanDataPort;
class MLtsyDispatchCallControlRecoverDataPort;
class MLtsyDispatchCallControlStartDtmfTone;
class MLtsyDispatchCallControlGetActiveAlsLine;
class MLtsyDispatchCallControlDialData;
class MLtsyDispatchCallControlQueryIsEmergencyNumber;
class MLtsyDispatchCallControlGetAlsPpSupport;
class MLtsyDispatchCallControlGetAlsBlockedStatus;
class MLtsyDispatchCallControlSetAlsBlocked;
class MLtsyDispatchCallControlGetLifeTime;
class MLtsyDispatchCallControlTerminateErrorCall;
class MLtsyDispatchCallControlTerminateAllCalls;
class MLtsyDispatchCallControlGetCallForwardingIndicator;
class MLtsyDispatchCallControlUpdateLifeTimer;

// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to CallControl related requests to the Licensee LTSY.
 */
class CCallControlDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CCallControlDispatcher();

	static CCallControlDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);

	static CCallControlDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);

	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY

	TInt DispatchAnswerL(const CMmDataPackage* aDataPackage);
	TInt DispatchAnswerIsvL(const CMmDataPackage* aDataPackage);
	TInt DispatchHoldL(const CMmDataPackage* aDataPackage);
	TInt DispatchDialEmergencyL(const CMmDataPackage* aDataPackage);
	TInt DispatchDialNoFdnCheckL(const CMmDataPackage* aDataPackage);
	TInt DispatchStopDtmfToneL(const CMmDataPackage* aDataPackage);
	TInt DispatchDialIsvL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetActiveAlsLineL(const CMmDataPackage* aDataPackage);
	TInt DispatchSendDtmfTonesCancelL(const CMmDataPackage* aDataPackage);
	TInt DispatchHangUpL(const CMmDataPackage* aDataPackage);
	TInt DispatchResumeL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetDynamicHscsdParamsL(const CMmDataPackage* aDataPackage);
	TInt DispatchDialL(const CMmDataPackage* aDataPackage);
	TInt DispatchTransferL(const CMmDataPackage* aDataPackage);
	TInt DispatchSendDtmfTonesL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetIdentityServiceStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchSwapL(const CMmDataPackage* aDataPackage);
	TInt DispatchContinueDtmfStringSendingL(const CMmDataPackage* aDataPackage);
	TInt DispatchLoanDataPortL(const CMmDataPackage* aDataPackage);
	TInt DispatchRecoverDataPortL(const CMmDataPackage* aDataPackage);
	TInt DispatchStartDtmfToneL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetActiveAlsLineL();
	TInt DispatchQueryIsEmergencyNumberL(const CMmDataPackage* aDataPackage);
    TInt DispatchGetAlsPpSupportL();
    TInt DispatchGetAlsBlockedStatusL();
    TInt DispatchSetAlsBlockedL(const CMmDataPackage* aDataPackage);
    TInt DispatchGetLifeTimeL();
    TInt DispatchTerminateCallL(const CMmDataPackage* aDataPackage);
    TInt DispatchGetCallForwardingIndicatorL();
	TInt DispatchUpdateLifeTimeL(const CMmDataPackage* aDataPackage);

	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.

	void CallbackNotifyCallStatusChange(TInt aError, TInt aCallId, RMobileCall::TMobileCallStatus aMobileCallStatus);
	void CallbackReceiveUui(TInt aError, TInt aCallId, const TDesC& aCallUui);
	void CallbackNotifyDataCallCapsChange(TInt aError, TInt aCallId, const RMobileCall::TMobileCallDataCapsV1& aMobileCallDataCaps);
	void CallbackNotifyIncomingCall(TInt aError, const RMobileCall::TMobileCallInfoV1& aCallInfo);
	void CallbackNotifyHscsdInfoChange(TInt aError, TInt aCallId, const RMobileCall::TMobileCallHscsdInfoV8& aCallParams, const RMobileCall::TMobileCallDataCapsV1& aCallCaps);
	void CallbackNotifyCallEvent(TInt aError, TInt aCallId, RMobileCall::TMobileCallEvent aCallEvent);
	void CallbackNotifyCallInfoChange(TInt aError, const RMobileCall::TMobileCallInfoV1& aMobileCallInfo);
    void CallbackRemoteAlertingToneChange(TInt aError, RMmCustomAPI::TRemoteAlertingToneStatus aRemoteAlertingTone);
	void CallbackAnswer(TInt aError, TInt aCallId);
	void CallbackHold(TInt aError, TInt aCallId);
	void CallbackDialEmergency(TInt aError, TInt aCallId);
	void CallbackStopDtmfTone(TInt aError);
	void CallbackSetActiveAlsLine(TInt aError);
	void CallbackSendDtmfTonesCancel(TInt aError);
	void CallbackHangUp(TInt aError, TInt aCallId);
	void CallbackResume(TInt aError, TInt aCallId);
	void CallbackSetDynamicHscsdParams(TInt aError, TInt aCallId);
	void CallbackDialVoice(TInt aError, TInt aCallId);
	void CallbackTransfer(TInt aError);
	void CallbackSendDtmfTones(TInt aError);
	void CallbackGetIdentityServiceStatus(TInt aError, RMobilePhone::TMobilePhoneIdServiceStatus aIdentityServiceStatus);
	void CallbackSwap(TInt aError);
	void CallbackStartDtmfTone(TInt aError);
	void CallbackGetActiveAlsLine(TInt aError, RMobilePhone::TMobilePhoneALSLine aAlsLine);
	void CallbackDialData(TInt aError, TInt aCallId);
	void CallbackQueryIsEmergencyNumber(CRequestQueueOneShot::TIpcDataPackage* aDataPackage);
	void CallbackNotifyIccCallForwardingStatusChange(TInt aError, const RMobilePhone::TMobileAddress& aCallForwardingNo,
			 								RMobilePhone::TCFUIndicatorStatusFlags aCallForwardingStatusFlags,
			 								RMmCustomAPI::TMultipleSubscriberProfileID aMultipleSubscriberProfileId);
	void CallbackGetAlsPpSupport(TInt aError, RMmCustomAPI::TAlsSupport aAlsStatus);
	void CallbackGetAlsBlockedStatus(TInt aError, RMmCustomAPI::TGetAlsBlockStatus aAlsSupport);
	void CallbackSetAlsBlocked(TInt aError);
	void CallbackGetLifeTime(TInt aError);
	void CallbackGetLifeTime(TInt aError, TUint32 aHours, TUint8 aMinutes);
	void CallbackGetLifeTime(TInt aError, const TDateTime &aManufacturingDate);
	void CallbackGetLifeTime(TInt aError, const TDateTime &aManufacturingDate, TUint32 aHours, TUint8 aMinutes);
	void CallbackTerminateErrorCall(TInt aError);
	void CallbackTerminateAllCalls(TInt aError);
	void CallbackGetCallForwardingIndicator(TInt aError, RMobilePhone::TMobileTON aTypeOfNumber,
                                            RMobilePhone::TMobileNPI aMobilePlan,
                                            const TDesC &aNumber,
                                            RMobilePhone::TCFUIndicatorStatusFlags aCFUIndicatorStatusFlags,
                                            RMobilePhone::TMultipleSubscriberProfileID aMultipleSubscriberProfileId);
	void CallbackUpdateLifeTimer(TInt aError);

	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);

private:

	CCallControlDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);

	void ConstructL();

	// Helper functions
	TInt DoDispatchDialL(const CMmDataPackage* aDataPackage, TBool aIsIsvCall, TBool aPerformFdnCheck);
	TInt DoDispatchAnswerL(const CMmDataPackage* aDataPackage,	TBool aIsIsvRequest);
	void DoCallbackDial(TInt aError, TInt aCallId, RMobilePhone::TMobileService aMode);
	TInt DoSendDtmfTonesL(TInt aCallId, const TDesC& aTones);

private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;

	// Interfaces in the Licensee LTSY, not owned by this object

    MLtsyDispatchCallControlAnswer* iLtsyDispatchCallControlAnswer;
    MLtsyDispatchCallControlHold* iLtsyDispatchCallControlHold;
    MLtsyDispatchCallControlDialEmergency* iLtsyDispatchCallControlDialEmergency;
    MLtsyDispatchCallControlStopDtmfTone* iLtsyDispatchCallControlStopDtmfTone;
    MLtsyDispatchCallControlSetActiveAlsLine* iLtsyDispatchCallControlSetActiveAlsLine;
    MLtsyDispatchCallControlSendDtmfTonesCancel* iLtsyDispatchCallControlSendDtmfTonesCancel;
    MLtsyDispatchCallControlHangUp* iLtsyDispatchCallControlHangUp;
    MLtsyDispatchCallControlResume* iLtsyDispatchCallControlResume;
    MLtsyDispatchCallControlSetDynamicHscsdParams* iLtsyDispatchCallControlSetDynamicHscsdParams;
    MLtsyDispatchCallControlDialVoice* iLtsyDispatchCallControlDialVoice;
    MLtsyDispatchCallControlTransfer* iLtsyDispatchCallControlTransfer;
    MLtsyDispatchCallControlSendDtmfTones* iLtsyDispatchCallControlSendDtmfTones;
    MLtsyDispatchCallControlGetIdentityServiceStatus* iLtsyDispatchCallControlGetIdentityServiceStatus;
    MLtsyDispatchCallControlSwap* iLtsyDispatchCallControlSwap;
    MLtsyDispatchCallControlSwap* iLtsyDispatchCallControlSingleSwap;
    MLtsyDispatchCallControlLoanDataPort* iLtsyDispatchCallControlLoanDataPort;
    MLtsyDispatchCallControlRecoverDataPort* iLtsyDispatchCallControlRecoverDataPort;
    MLtsyDispatchCallControlStartDtmfTone* iLtsyDispatchCallControlStartDtmfTone;
    MLtsyDispatchCallControlGetActiveAlsLine* iLtsyDispatchCallControlGetActiveAlsLine;
    MLtsyDispatchCallControlDialData* iLtsyDispatchCallControlDialData;
    MLtsyDispatchCallControlQueryIsEmergencyNumber* iLtsyDispatchCallControlQueryIsEmergencyNumber;
    MLtsyDispatchCallControlGetAlsPpSupport* iLtsyDispatchCallControlGetAlsPpSupport;
    MLtsyDispatchCallControlGetAlsBlockedStatus* iLtsyDispatchCallControlGetAlsBlockedStatus;
    MLtsyDispatchCallControlSetAlsBlocked* iLtsyDispatchCallControlSetAlsBlocked;
    MLtsyDispatchCallControlGetLifeTime* iLtsyDispatchCallControlGetLifeTime;
    MLtsyDispatchCallControlTerminateErrorCall* iLtsyDispatchCallControlTerminateErrorCall;
    MLtsyDispatchCallControlTerminateAllCalls* iLtsyDispatchCallControlTerminateAllCalls;
    MLtsyDispatchCallControlGetCallForwardingIndicator* iLtsyDispatchCallControlGetCallForwardingIndicator;
	MLtsyDispatchCallControlUpdateLifeTimer* iLtsyDispatchCallControlUpdateLifeTimer;

    // Other data items

    /**
     * Cache of phone number passed to dispatcher in a
     * ECustomCheckEmergencyNumberIPC request.
     */
    RMmCustomAPI::TMobileTelNumber iQueryEmergencyNumber;

    /**
     * Buffer containing remains of DTMF string to be sent.
     * This is used for handling 'w' (wait) characters passed into
     * RMobilePhone::SendDTMFTones
     */
    TBuf<KMaxInfoName> iDtmfString; // Created by this object but ownership passes to CTSY

    /**
     * The call ID of the call iDtmfString was sending tones to.
     * Used if client decides to continue sending the string.
     */
    TInt iCallIdForDtmf;

    /**
     * ETrue if a stop character has been found in the DTMF string,
     * therefore client needs to be notified of stop on completion of
     * send.
     */
    TBool iFoundDtmfStop;

    /**
     * Caches the mode used to dial a voice call.  This is needed for when the
     * auxiliary line is used to dial a call.  On the callback from the dial,
     * the CTSY needs the call mode to locate the call object to set the Call ID.
	 */
    RMobilePhone::TMobileService iModeUsedForVoiceCallDial;
    
    /**
     * Caches whether an FDN check was performed, to allow the correct IPC to be 
     * completed.
     */
    TBool iFdnCheckPerformed;

    /**
     * These are the two Call IDs of the calls on which a swap operation
     * has been requested.  It allows the dispatcher to return a Call ID to the CTSY
     * on completion of a swap (if the swap failed) so that the CTSY can locate the
     * call object to complete the swap back to the client.
     *
     * These Call IDs should only have a value != -1 when there is a pending swap
     * operation.
     */
    TInt iSwapCallId;
    TInt iSecondSwapCallId;

    /**
     * These are the two Call IDs of the calls on which a transfer operation
     * has been requested. It is needed for the same reason as iSwapCallId
     * and iSecondSwapCallId above.
     */
    TInt iTransferHeldCallId;
    TInt iTransferSecondCallId;
	}; // class CCallControlDispatcher

#endif // __CCALLCONTROLDISPATCHER_H_

