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
// cltsydispatchcallcontrolhandler.h
//

/**
 @file
 @internalAll 
*/

#ifndef __CLTSYDISPATCHCALLCONTROLHANDLER_H_
#define __CLTSYDISPATCHCALLCONTROLHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include "misdispatchinterfacesupported.h"
#include "ltsymacros.h"

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

// FORWARD DECLARATIONS
class CCtsyDispatcherCallback;

// Can be removed in real LTSY implementation
MTEST_FORWARD_DECLARE_MOCKLTSYENGINE

// CLASS DECLARATION

/**
 * Implements interfaces from the CTSY Dispatcher. The CTSY Dispatcher uses these
 * interfaces to make requests to the Licensee LTSY.
 */
class CLtsyCallControlHandler : 	public CBase,
							public MIsDispatchInterfaceSupported,
							public MLtsyDispatchCallControlAnswer,
							public MLtsyDispatchCallControlHold,
							public MLtsyDispatchCallControlDialEmergency,
							public MLtsyDispatchCallControlStopDtmfTone,
							public MLtsyDispatchCallControlSetActiveAlsLine,
							public MLtsyDispatchCallControlSendDtmfTonesCancel,
							public MLtsyDispatchCallControlHangUp,
							public MLtsyDispatchCallControlResume,
							public MLtsyDispatchCallControlSetDynamicHscsdParams,
							public MLtsyDispatchCallControlDialVoice,
							public MLtsyDispatchCallControlTransfer,
							public MLtsyDispatchCallControlSendDtmfTones,
							public MLtsyDispatchCallControlGetIdentityServiceStatus,
							public MLtsyDispatchCallControlSwap,
							public MLtsyDispatchCallControlLoanDataPort,
							public MLtsyDispatchCallControlRecoverDataPort,
							public MLtsyDispatchCallControlStartDtmfTone,
							public MLtsyDispatchCallControlGetActiveAlsLine,
							public MLtsyDispatchCallControlDialData,
                            public MLtsyDispatchCallControlQueryIsEmergencyNumber,
                            public MLtsyDispatchCallControlGetAlsPpSupport,
                            public MLtsyDispatchCallControlGetAlsBlockedStatus,
                            public MLtsyDispatchCallControlSetAlsBlocked,
                            public MLtsyDispatchCallControlGetLifeTime,
                            public MLtsyDispatchCallControlTerminateErrorCall,
                            public MLtsyDispatchCallControlTerminateAllCalls,
                            public MLtsyDispatchCallControlGetCallForwardingIndicator,
							public MLtsyDispatchCallControlUpdateLifeTimer
    

	{
public:

	virtual ~CLtsyCallControlHandler();
	static CLtsyCallControlHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsyCallControlHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);



    // From MLtsyDispatchCallControlAnswer
    virtual TInt HandleAnswerReqL(TInt aCallId, TBool aIsIsvCall);

    // From MLtsyDispatchCallControlHold
    virtual TInt HandleHoldReqL(TInt aCallId);

    // From MLtsyDispatchCallControlDialEmergency
    virtual TInt HandleDialEmergencyReqL(const TDes& aEmergencyNumber);

    // From MLtsyDispatchCallControlStopDtmfTone
    virtual TInt HandleStopDtmfToneReqL(TInt aCallId);

    // From MLtsyDispatchCallControlSetActiveAlsLine
    virtual TInt HandleSetActiveAlsLineReqL(RMobilePhone::TMobilePhoneALSLine aAlsLine);

    // From MLtsyDispatchCallControlSendDtmfTonesCancel
    virtual TInt HandleSendDtmfTonesCancelReqL(TInt aCallId);

    // From MLtsyDispatchCallControlHangUp
    virtual TInt HandleHangUpReqL(TInt aCallId, TInt aHangupCause);

    // From MLtsyDispatchCallControlResume
    virtual TInt HandleResumeReqL(TInt aCallId);

    // From MLtsyDispatchCallControlSetDynamicHscsdParams
    virtual TInt HandleSetDynamicHscsdParamsReqL(TInt aCallId, const RMobileCall::TMobileHscsdCallParamsV1& aHscsdParams);

    // From MLtsyDispatchCallControlDialVoice
    virtual TInt HandleDialVoiceReqL(RMobilePhone::TMobilePhoneALSLine aLine, const RMobilePhone::TMobileAddress& aDialledParty, const RMobileCall::TMobileCallParamsV1& aCallParamsV1, TBool aIsIsvCall, RMobileCall::TCallParamOrigin aCallOrigin, TBool aPerformFdnCheck);

    // From MLtsyDispatchCallControlTransfer
    virtual TInt HandleTransferReqL(TInt aHeldCallId, TInt aSecondCallId);

    // From MLtsyDispatchCallControlSendDtmfTones
    virtual TInt HandleSendDtmfTonesReqL(TInt aCallId, const TDesC &aTones);

    // From MLtsyDispatchCallControlGetIdentityServiceStatus
    virtual TInt HandleGetIdentityServiceStatusReqL(RMobilePhone::TMobilePhoneIdService aService);

    // From MLtsyDispatchCallControlSwap
    virtual TInt HandleSwapReqL(TInt aCallId, TInt aSecondCallId);

    // From MLtsyDispatchCallControlSwap
    virtual TInt HandleSwapReqL(TInt aCallId);

    // From MLtsyDispatchCallControlLoanDataPort
    virtual TInt HandleLoanDataPortSyncL(TInt aCallId, RCall::TCommPort& aCommPort);

    // From MLtsyDispatchCallControlRecoverDataPort
    virtual TInt HandleRecoverDataPortSyncL(TInt aCallId, RCall::TCommPort& aCommPort);

    // From MLtsyDispatchCallControlStartDtmfTone
    virtual TInt HandleStartDtmfToneReqL(TInt aCallId, const TChar& aTone);

	// From MLtsyDispatchCallControlGetActiveAlsLine
	virtual TInt HandleGetActiveAlsLineReqL();

	// From MLtsyDispatchCallControlDialData
	virtual TInt HandleDialDataReqL(const RMobilePhone::TMobileAddress& aDialledParty, const RMobileCall::TMobileDataCallParamsV1& aCallParamsV1, TBool aPerformFdnCheck);

	// From MLtsyDispatchCallControlQueryIsEmergencyNumber
	virtual TInt HandleQueryIsEmergencyNumberSyncL(const TDesC& aNumber, TBool& aIsEmergencyNumber);
	
	// From MLtsyDispatchCallControlUpdateLifeTimer
	virtual TInt HandleUpdateLifeTimerReqL(TUint32 aDuration);

    // From MLtsyDispatchCallControlGetAlsPpSupport
    virtual TInt HandleGetAlsPpSupportL();

    // From MLtsyDispatchCallControlGetAlsBlockedStatus
    virtual TInt HandleGetAlsBlockedStatusL();

    // From MLtsyDispatchCallControlSetAlsBlocked
    virtual TInt HandleSetAlsBlockedL(RMmCustomAPI::TSetAlsBlock aBlocked);

    // From MLtsyDispatchCallControlGetLifeTime
    virtual TInt HandleGetLifeTimeL();

    // From MLtsyDispatchCallControlTerminateErrorCall
    virtual TInt HandleTerminateErrorCallL(TInt aCallId);

    // From MLtsyDispatchCallControlTerminateAllCalls
    virtual TInt HandleTerminateAllCallsL();

    // From MLtsyDispatchCallControlGetCallForwardingIndicatorStatus
    virtual TInt HandleGetCallForwardingIndicatorL();
	
private:

	CLtsyCallControlHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	void ConstructL();

private:

	// Not owned

	/**
	 * Callback object in the CTSY Dispatcher.
	 *
	 * Used to complete a request back to the CTSY Dispatcher.
	 */
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;

public:
	// Can be removed in real LTSY implementation.
	MTEST_DECLARE_MOCKLTSYENGINE

	}; // class CLtsyCallControlHandler

#endif // __CLTSYDISPATCHCALLCONTROLHANDLER_H_

