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

#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmockcallcontrolmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>

#include "mockltsyindicatorids.h"

// Example emergency numbers, used here for testing purposes only.
_LIT(KEmergencyNumber999, "999");
_LIT(KEmergencyNumber112, "112");

/**
 * Factory function
 */
CMockCallControlMessHandler* CMockCallControlMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockCallControlMessHandler* self = new (ELeave)CMockCallControlMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockCallControlMessHandler::CMockCallControlMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {
    }

/**
 * 2nd phase contructor
 */
void CMockCallControlMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockCallControlMessHandler::~CMockCallControlMessHandler()
    {

    }

TBool CMockCallControlMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		case KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId:
		case KMockLtsyDispatchCallControlReceiveUuiIndId:
		case KMockLtsyDispatchCallControlNotifyDataCallCapsChangeIndId:
		case KMockLtsyDispatchCallControlNotifyIncomingCallIndId:
		case KMockLtsyDispatchCallControlNotifyHscsdInfoChangeIndId:
		case KMockLtsyDispatchCallControlNotifyCallEventIndId:
		case KMockLtsyDispatchCallControlNotifyCallInfoChangeIndId:
		case KMockLtsyDispatchCallControlRemoteAlertingToneChangeIndId:
		case MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId:
		case MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId:
		case MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId:
		case MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId:
		case MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId:
		case MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId:
		case MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId:
		case MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId:
		case MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId:
		case MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId:
		case MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId:
		case MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId:
		case MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId:
		case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId:
		case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId:
		case MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId:
		case MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId:
		case MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId:
		case MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId:
		case MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId:
		case MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId:
		case KMockLtsyDispatchCallControlNotifyIccCallForwardingStatusChangeIndId:
		case MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId:
		case MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId:
		case MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId:
		case MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId:
		case MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId:
		case MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId:
		case MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId:
		case MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockCallControlMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockCallControlMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TBool isIsvCall = VA_ARG(aList, TBool);
    		TMockLtsyData2<TInt, TInt> data(callId, isIsvCall);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(callId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId:
    		{
    		RMobileENStore::TEmergencyNumber& number = *VA_ARG(aList, RMobileENStore::TEmergencyNumber*);
    		TMockLtsyData1<RMobileENStore::TEmergencyNumber> data(number);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(callId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId:
    		{
    		RMobilePhone::TMobilePhoneALSLine& alsLine = *VA_ARG(aList, RMobilePhone::TMobilePhoneALSLine*);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> data(alsLine);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(callId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TInt hangUpCause = VA_ARG(aList, TInt);
    		TMockLtsyData2<TInt, TInt> data(callId, hangUpCause);
  			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(callId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		RMobileCall::TMobileHscsdCallParamsV8& hscsdParams = *VA_ARG(aList, RMobileCall::TMobileHscsdCallParamsV8*);
    		TMockLtsyData2<TInt, RMobileCall::TMobileHscsdCallParamsV8> data(callId, hscsdParams);
  			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId:
    		{
			RMobilePhone::TMobilePhoneALSLine& line = *VA_ARG(aList, RMobilePhone::TMobilePhoneALSLine*);
    		RMobilePhone::TMobileAddress& dialledParty = *VA_ARG(aList, RMobilePhone::TMobileAddress*);
    		RMobileCall::TMobileCallParamsV1& callParams = *VA_ARG(aList, RMobileCall::TMobileCallParamsV1*);
    		TBool isIsvCall = VA_ARG(aList, TBool);
    		RMobileCall::TCallParamOrigin& callOrigin = *VA_ARG(aList, RMobileCall::TCallParamOrigin*);
    		TBool performFdnCheck = VA_ARG(aList, TBool);

    		TMockLtsyData6<RMobilePhone::TMobilePhoneALSLine, RMobilePhone::TMobileAddress,
    			RMobileCall::TMobileCallParamsV1, TBool, RMobileCall::TCallParamOrigin, TBool>
    			data(line, dialledParty, callParams, isIsvCall, callOrigin, performFdnCheck);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TInt callId2 = VA_ARG(aList, TInt);
    		TMockLtsyData2<TInt, TInt> data(callId, callId2);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		const TDesC& tones = *(VA_ARG(aList, const TDesC*));
    		TInfoName tonesBuf;
    		tonesBuf.Copy(tones.Left(KMaxInfoName));
    		TMockLtsyData2<TInt, TInfoName> data(callId, tonesBuf);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId:
    		{
    		RMobilePhone::TMobilePhoneIdService& service = *VA_ARG(aList, RMobilePhone::TMobilePhoneIdService*);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneIdService> data(service);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId:
    		{
    		TInt heldCallId = VA_ARG(aList, TInt);
    		TInt connectedCallId = VA_ARG(aList, TInt);
    		TMockLtsyData2<TInt, TInt> data(heldCallId, connectedCallId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(callId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
		    RCall::TCommPort& commPort = *VA_ARG(aList, RCall::TCommPort*);
		    _LIT(KDataPort, "DATAPORT::0");
		    commPort.iPort = KDataPort;
		    //sync call but data can still be checked.
    		TMockLtsyData2<TInt, RCall::TCommPort> data(callId, commPort);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		RCall::TCommPort& commPort = *VA_ARG(aList, RCall::TCommPort*);
		    //sync call but data can still be checked.
    		TMockLtsyData2<TInt, RCall::TCommPort> data(callId, commPort);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId:
    		{
    		TInt callId = VA_ARG(aList, TInt);
    		TChar& tone = *(VA_ARG(aList, TChar*));
    		TMockLtsyData2<TInt, TChar> data(callId, tone);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId:
    		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId:
    		{
    		RMobilePhone::TMobileAddress& dialledParty = *VA_ARG(aList, RMobilePhone::TMobileAddress*);
    		RMobileCall::TMobileDataCallParamsV1& callParams = *VA_ARG(aList, RMobileCall::TMobileDataCallParamsV1*);
    		TBool performFdnCheck = VA_ARG(aList, TBool);
    		TMockLtsyData3<RMobilePhone::TMobileAddress, RMobileCall::TMobileDataCallParamsV1, TBool>
    			    	data(dialledParty, callParams, performFdnCheck);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId:
    		{
    		TDesC* emergencyNumber = VA_ARG(aList, TDesC*);
    		TBool& isEmergencyNumber = *VA_ARG(aList, TBool*);
    		if (*emergencyNumber == KEmergencyNumber999 || *emergencyNumber == KEmergencyNumber112)
    			{
    			isEmergencyNumber = ETrue;
    			}
    		else
    			{
    			isEmergencyNumber = EFalse;
    			}
    		//sync call but data can still be checked.
    		TMockLtsyData2<TDesC*, TBool> data(emergencyNumber, isEmergencyNumber);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId:
    	case MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId:
    	case MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId:
        case MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId:
    	case MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId:
    		{
    		RMmCustomAPI::TSetAlsBlock blocked = VA_ARG_ENUM(aList, RMmCustomAPI::TSetAlsBlock);
    		TMockLtsyData1<RMmCustomAPI::TSetAlsBlock> data(blocked);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId:
    		{
     		TInt callId = VA_ARG(aList, TInt);
    		TMockLtsyData1<TInt> data(callId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId:
    		{
    		TUint32 duration =VA_ARG(aList, TUint32);
    		TMockLtsyData1<TUint32> data(duration);
    		//The number of seconds that may have passed during a call is a timing measurement
    		//and could vary if logging is enabled or the CPU is busy. Thus data is not checked.
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data, EFalse); 
			}
    	default:
    		{
    		ASSERT(NULL);
    		TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}

    } // CMockCallControlMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockCallControlMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		case KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId:
		    {
		    TMockLtsyCallData1Buf<RMobileCall::TMobileCallStatus> callStatusData;
		    callStatusData.DeserialiseL(aData);
		    RMobileCall::TMobileCallStatus callStatus = callStatusData.Data1();
			iCompletionCallback.CallbackCallControlNotifyCallStatusChangeInd(aResult, callStatusData.CallId(), callStatus);
			}
			break;
		case KMockLtsyDispatchCallControlReceiveUuiIndId:
		    {
		    TMockLtsyCallData1Buf<RMobileCall::TMobileCallUUI> uuiData;
		    uuiData.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlReceiveUuiInd(aResult, uuiData.CallId(), uuiData.Data1());
			}
			break;
		case KMockLtsyDispatchCallControlNotifyDataCallCapsChangeIndId:
		    {
		    TMockLtsyCallData1Buf<RMobileCall::TMobileCallDataCapsV1> capsData;
		    capsData.DeserialiseL(aData);
		    RMobileCall::TMobileCallDataCapsV1 mobileCallDataCaps = capsData.Data1();
			iCompletionCallback.CallbackCallControlNotifyDataCallCapsChangeInd(aResult, capsData.CallId(), mobileCallDataCaps);
			}
			break;
		case KMockLtsyDispatchCallControlNotifyIncomingCallIndId:
		    {
		    TMockLtsyCallData1Buf<RMobileCall::TMobileCallInfoV1> mockCallData1;
		    mockCallData1.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlNotifyIncomingCallInd(aResult, mockCallData1.Data1());
			}
			break;
		case KMockLtsyDispatchCallControlNotifyHscsdInfoChangeIndId:
		    {
		    TMockLtsyCallData2Buf<RMobileCall::TMobileCallHscsdInfoV8, RMobileCall::TMobileCallDataCapsV1> infoData;
		    infoData.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlNotifyHscsdInfoChangeInd(aResult, infoData.CallId(), infoData.Data1(), infoData.Data2());
			}
			break;
		case KMockLtsyDispatchCallControlNotifyCallEventIndId:
		    {
		    TMockLtsyCallData1Buf<RMobileCall::TMobileCallEvent> callEventData;
		    callEventData.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlNotifyCallEventInd(aResult, callEventData.CallId(), callEventData.Data1());
			}
			break;
		case KMockLtsyDispatchCallControlNotifyCallInfoChangeIndId:
		    {
		    TMockLtsyData1Buf<RMobileCall::TMobileCallInfoV1> data;
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlNotifyCallInfoChangeInd(aResult, data.Data1());
			}
			break;
		case KMockLtsyDispatchCallControlRemoteAlertingToneChangeIndId:
		    {
		    TMockLtsyData1Buf<RMmCustomAPI::TRemoteAlertingToneStatus> data;
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlRemoteAlertingToneChangeInd(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId:
		    {
		    TMockLtsyData1Buf<TInt> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlAnswerComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId:
		    {
		    TMockLtsyData1Buf<TInt> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlHoldComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId:
		    {
		    TMockLtsyData1Buf<TInt> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlDialEmergencyComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId:
		    {
			iCompletionCallback.CallbackCallControlStopDtmfToneComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId:
		    {
			iCompletionCallback.CallbackCallControlSetActiveAlsLineComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId:
		    {
			iCompletionCallback.CallbackCallControlSendDtmfTonesCancelComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId:
		    {
		    TMockLtsyData1Buf<TInt> callData;
		    callData.DeserialiseL(aData);
		    TInt callId = callData.Data1();
			iCompletionCallback.CallbackCallControlHangUpComp(aResult, callId);
			}
			break;
		case MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId:
		    {
		    TMockLtsyData1Buf<TInt> callData;
		    callData.DeserialiseL(aData);
		    TInt callId = callData.Data1();
			iCompletionCallback.CallbackCallControlResumeComp(aResult, callId);
			}
			break;
		case MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId:
		    {
		    TMockLtsyData1Buf<TInt> callData;
		    callData.DeserialiseL(aData);
		    TInt callId = callData.Data1();
			iCompletionCallback.CallbackCallControlSetDynamicHscsdParamsComp(aResult, callId);
			}
			break;
		case MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId:
		    {
		    TMockLtsyCallData0 data;
   			data.DeserialiseL(aData);
   			iCompletionCallback.CallbackCallControlDialVoiceComp(aResult, data.CallId());
			}
			break;
		case MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId:
		    {
			iCompletionCallback.CallbackCallControlTransferComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId:
		    {
			iCompletionCallback.CallbackCallControlSendDtmfTonesComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneIdServiceStatus> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlGetIdentityServiceStatusComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId:
		    {
			iCompletionCallback.CallbackCallControlSwapComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId:
		    {
			iCompletionCallback.CallbackCallControlSwapComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId:
			{
			iCompletionCallback.CallbackCallControlStartDtmfToneComp(aResult);
			break;
			}
		case MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneALSLine> alsLineData;
			alsLineData.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlGetActiveAlsLineComp(aResult, alsLineData.Data1());
			}
			break;
		case MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId:
		    {
		    TMockLtsyCallData0 data;
		    data.DeserialiseL(aData);
   			iCompletionCallback.CallbackCallControlDialDataComp(aResult, data.CallId());
			}
			break;
		case KMockLtsyDispatchCallControlNotifyIccCallForwardingStatusChangeIndId:
		    {
		    TMockLtsyData3Buf<RMobilePhone::TMobileAddress,
		    	RMobilePhone::TCFUIndicatorStatusFlags,
		    	RMmCustomAPI::TMultipleSubscriberProfileID> data;
		    data.DeserialiseL(aData);
		    RMobilePhone::TMobileAddress addr = data.Data1();
		    RMobilePhone::TCFUIndicatorStatusFlags flags = data.Data2();
		    RMmCustomAPI::TMultipleSubscriberProfileID id = data.Data3();
			iCompletionCallback.CallbackCallControlNotifyIccCallForwardingStatusChangeInd(aResult, addr, flags, id);
			}
			break;
		case MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId:
		    {
			TMockLtsyData1Buf<RMmCustomAPI::TAlsSupport> alsSupported;
			alsSupported.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlGetAlsPpSupportComp(aResult, alsSupported.Data1());
			}
			break;
		case MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId:
		    {
			TMockLtsyData1Buf<RMmCustomAPI::TGetAlsBlockStatus> alsStatus;
			alsStatus.DeserialiseL(aData);
			iCompletionCallback.CallbackCallControlGetAlsBlockedStatusComp(aResult, alsStatus.Data1());
			}
			break;
		case MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId:
		    {
   			iCompletionCallback.CallbackCallControlSetAlsBlockedComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId:
		    {
   			iCompletionCallback.CallbackCallControlTerminateErrorCallComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId:
		    {
   			iCompletionCallback.CallbackCallControlTerminateAllCallsComp(aResult);
			}
			break;
		case MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId:
		    {
			TMockLtsyData1Buf<RMmCustomAPI::TLifeTimeData> lifeTileData;
			lifeTileData.DeserialiseL(aData);
            switch(lifeTileData.Data1().iCaps)
                {
                case RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsLifeTime:
                    iCompletionCallback.CallbackCallControlGetLifeTimeComp(aResult, lifeTileData.Data1().iHours, lifeTileData.Data1().iMinutes);
                    break;
                case RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsManufacturerDate:
                    iCompletionCallback.CallbackCallControlGetLifeTimeComp(aResult, lifeTileData.Data1().iManufacturingDate);
                    break;
                case (RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsLifeTime | RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsManufacturerDate):
                    iCompletionCallback.CallbackCallControlGetLifeTimeComp(aResult, lifeTileData.Data1().iManufacturingDate, lifeTileData.Data1().iHours, lifeTileData.Data1().iMinutes);
                    break;
                case 0:
                default:
                    iCompletionCallback.CallbackCallControlGetLifeTimeComp(aResult);
                    break;
                };
			}
			break;
		case MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCFUIndicatorV6> indicatorData;
			indicatorData.DeserialiseL(aData);
            iCompletionCallback.CallbackCallControlGetCallForwardingIndicatorComp(aResult,
                                                                                  indicatorData.Data1().iCFNumber.iTypeOfNumber,
                                                                                  indicatorData.Data1().iCFNumber.iNumberPlan,
                                                                                  indicatorData.Data1().iCFNumber.iTelNumber,
                                                                                  indicatorData.Data1().iIndicator,
                                                                                  indicatorData.Data1().iMultipleSubscriberProfileID);
			}
			break;
		case MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId:
			{
			iCompletionCallback.CallbackCallControlUpdateLifeTimerComp(aResult);
			}
			break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockCallControlMessHandler::CompleteL
