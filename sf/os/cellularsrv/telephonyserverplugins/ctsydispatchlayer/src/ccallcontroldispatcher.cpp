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
#include "ccallcontroldispatcherTraces.h"
#endif

#include "ccallcontroldispatcher.h"

#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"
#include "csatdispatcher.h"

CCallControlDispatcher::CCallControlDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot),
		iFoundDtmfStop(EFalse), iModeUsedForVoiceCallDial(RMobilePhone::EServiceUnspecified),
		iFdnCheckPerformed(EFalse),iSwapCallId(KInvalidCallId),
		iSecondSwapCallId(KInvalidCallId),iTransferHeldCallId(KInvalidCallId),
		iTransferSecondCallId(KInvalidCallId)
	{
	iDtmfString.Zero();
	} // CCallControlDispatcher::CCallControlDispatcher


CCallControlDispatcher::~CCallControlDispatcher()
	{
	} // CCallControlDispatcher::~CCallControlDispatcher


CCallControlDispatcher* CCallControlDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CCallControlDispatcher* self =
		new (ELeave) CCallControlDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CCallControlDispatcher::NewLC


CCallControlDispatcher* CCallControlDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CCallControlDispatcher* self =
		CCallControlDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CCallControlDispatcher::NewL


void CCallControlDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;

	// Get the Licensee LTSY interfaces related to CallControl functionality
	// from the factory


	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId))
		{
		TAny* answerInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId,
		       	answerInterface);
		iLtsyDispatchCallControlAnswer =
				static_cast<MLtsyDispatchCallControlAnswer*>(answerInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlAnswer, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId))
		{
		TAny* holdInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId,
		       	holdInterface);
		iLtsyDispatchCallControlHold =
				static_cast<MLtsyDispatchCallControlHold*>(holdInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlHold, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId))
		{
		TAny* dialEmergencyInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId,
		       	dialEmergencyInterface);
		iLtsyDispatchCallControlDialEmergency =
				static_cast<MLtsyDispatchCallControlDialEmergency*>(dialEmergencyInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlDialEmergency, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId))
		{
		TAny* stopDtmfToneInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId,
		       	stopDtmfToneInterface);
		iLtsyDispatchCallControlStopDtmfTone =
				static_cast<MLtsyDispatchCallControlStopDtmfTone*>(stopDtmfToneInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlStopDtmfTone, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId))
		{
		TAny* setActiveAlsLineInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId,
		       	setActiveAlsLineInterface);
		iLtsyDispatchCallControlSetActiveAlsLine =
				static_cast<MLtsyDispatchCallControlSetActiveAlsLine*>(setActiveAlsLineInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlSetActiveAlsLine, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId))
		{
		TAny* sendDtmfTonesCancelInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId,
		       	sendDtmfTonesCancelInterface);
		iLtsyDispatchCallControlSendDtmfTonesCancel =
				static_cast<MLtsyDispatchCallControlSendDtmfTonesCancel*>(sendDtmfTonesCancelInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlSendDtmfTonesCancel, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId))
		{
		TAny* hangUpInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId,
		       	hangUpInterface);
		iLtsyDispatchCallControlHangUp =
				static_cast<MLtsyDispatchCallControlHangUp*>(hangUpInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlHangUp, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId))
		{
		TAny* resumeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId,
		       	resumeInterface);
		iLtsyDispatchCallControlResume =
				static_cast<MLtsyDispatchCallControlResume*>(resumeInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlResume, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId))
		{
		TAny* setDynamicHscsdParamsInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId,
		       	setDynamicHscsdParamsInterface);
		iLtsyDispatchCallControlSetDynamicHscsdParams =
				static_cast<MLtsyDispatchCallControlSetDynamicHscsdParams*>(setDynamicHscsdParamsInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlSetDynamicHscsdParams, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId))
		{
		TAny* dialInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId,
		       	dialInterface);
		iLtsyDispatchCallControlDialVoice =
				static_cast<MLtsyDispatchCallControlDialVoice*>(dialInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlDialVoice, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId))
		{
		TAny* transferInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId,
		       	transferInterface);
		iLtsyDispatchCallControlTransfer =
				static_cast<MLtsyDispatchCallControlTransfer*>(transferInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlTransfer, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId))
		{
		TAny* sendDtmfTonesInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId,
		       	sendDtmfTonesInterface);
		iLtsyDispatchCallControlSendDtmfTones =
				static_cast<MLtsyDispatchCallControlSendDtmfTones*>(sendDtmfTonesInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlSendDtmfTones, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId))
		{
		TAny* getIdentityServiceStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId,
		       	getIdentityServiceStatusInterface);
		iLtsyDispatchCallControlGetIdentityServiceStatus =
				static_cast<MLtsyDispatchCallControlGetIdentityServiceStatus*>(getIdentityServiceStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlGetIdentityServiceStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId))
		{
		TAny* swapInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId,
		       	swapInterface);
		iLtsyDispatchCallControlSwap =
				static_cast<MLtsyDispatchCallControlSwap*>(swapInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlSwap, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId))
		{
		TAny* swapInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId,
		       	swapInterface);
		iLtsyDispatchCallControlSingleSwap =
				static_cast<MLtsyDispatchCallControlSwap*>(swapInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlSingleSwap, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
		
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId))
		{
		TAny* loanDataPortInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId,
		       	loanDataPortInterface);
		iLtsyDispatchCallControlLoanDataPort =
				static_cast<MLtsyDispatchCallControlLoanDataPort*>(loanDataPortInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlLoanDataPort, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId))
		{
		TAny* recoverDataPortInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId,
		       	recoverDataPortInterface);
		iLtsyDispatchCallControlRecoverDataPort =
				static_cast<MLtsyDispatchCallControlRecoverDataPort*>(recoverDataPortInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlRecoverDataPort, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId))
		{
		TAny* startDtmfToneInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId,
		       	startDtmfToneInterface);
		iLtsyDispatchCallControlStartDtmfTone =
				static_cast<MLtsyDispatchCallControlStartDtmfTone*>(startDtmfToneInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlStartDtmfTone, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId))
		{
	    TAny* GetActiveAlsLineInterface = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	       		MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId,
	       		GetActiveAlsLineInterface);
		iLtsyDispatchCallControlGetActiveAlsLine =
			static_cast<MLtsyDispatchCallControlGetActiveAlsLine*>(GetActiveAlsLineInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlGetActiveAlsLine, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId))
		{
		TAny* dialDataInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId,
		       	dialDataInterface);
		iLtsyDispatchCallControlDialData =
				static_cast<MLtsyDispatchCallControlDialData*>(dialDataInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlDialData, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId))
			{
			TAny* queryIsEmergencyNumberInterface = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId,
					queryIsEmergencyNumberInterface);
			iLtsyDispatchCallControlQueryIsEmergencyNumber =
					static_cast<MLtsyDispatchCallControlQueryIsEmergencyNumber*>(queryIsEmergencyNumberInterface);
	        __ASSERT_DEBUG(iLtsyDispatchCallControlQueryIsEmergencyNumber, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId))
		{
		TAny* getAlsPpSupportInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId,
		       	getAlsPpSupportInterface);
		iLtsyDispatchCallControlGetAlsPpSupport =
				static_cast<MLtsyDispatchCallControlGetAlsPpSupport*>(getAlsPpSupportInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlGetAlsPpSupport, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId))
		{
		TAny* getAlsBlockedStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId,
		       	getAlsBlockedStatusInterface);
		iLtsyDispatchCallControlGetAlsBlockedStatus =
				static_cast<MLtsyDispatchCallControlGetAlsBlockedStatus*>(getAlsBlockedStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlGetAlsBlockedStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId))
		{
		TAny* setAlsBlockedInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId,
		       	setAlsBlockedInterface);
		iLtsyDispatchCallControlSetAlsBlocked =
				static_cast<MLtsyDispatchCallControlSetAlsBlocked*>(setAlsBlockedInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlSetAlsBlocked, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId))
		{
		TAny* getLifeTimeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId,
		       	getLifeTimeInterface);
		iLtsyDispatchCallControlGetLifeTime =
				static_cast<MLtsyDispatchCallControlGetLifeTime*>(getLifeTimeInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlGetLifeTime, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId))
		{
		TAny* terminateErrorCallInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId,
		       	terminateErrorCallInterface);
		iLtsyDispatchCallControlTerminateErrorCall =
				static_cast<MLtsyDispatchCallControlTerminateErrorCall*>(terminateErrorCallInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlTerminateErrorCall, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId))
		{
		TAny* terminateAllCallsInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId,
		       	terminateAllCallsInterface);
		iLtsyDispatchCallControlTerminateAllCalls =
				static_cast<MLtsyDispatchCallControlTerminateAllCalls*>(terminateAllCallsInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlTerminateAllCalls, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId))
		{
		TAny* getCallForwardingIndicatorInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId,
		       	getCallForwardingIndicatorInterface);
		iLtsyDispatchCallControlGetCallForwardingIndicator =
				static_cast<MLtsyDispatchCallControlGetCallForwardingIndicator*>(getCallForwardingIndicatorInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlGetCallForwardingIndicator, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlFuncUnitId, MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId))
			{
			TAny* updateLifeTimerInterface = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId,
					updateLifeTimerInterface);
			iLtsyDispatchCallControlUpdateLifeTimer =
					static_cast<MLtsyDispatchCallControlUpdateLifeTimer*>(updateLifeTimerInterface);
	        __ASSERT_DEBUG(iLtsyDispatchCallControlUpdateLifeTimer, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}
	} // CCallControlDispatcher::ConstructL


void CCallControlDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.
 *
 * @param aDispatcherHolder Pointer to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;

	iDispatcherHolder = &aDispatcherHolder;
	} // CCallControlDispatcher::SetDispatcherHolder


TInt CCallControlDispatcher::DispatchAnswerL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EEtelCallAnswer
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = DoDispatchAnswerL(aDataPackage, EFalse);

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchAnswerL

TInt CCallControlDispatcher::DispatchAnswerIsvL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EEtelCallAnswerIsv
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = DoDispatchAnswerL(aDataPackage, ETrue);

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchAnswerIsvL


TInt CCallControlDispatcher::DispatchHoldL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallHold
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlHold)
		{
		const CCallDataPackage* dataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
		TInt callId;
		RMobilePhone::TMobileService mode;
		dataPackage->GetCallIdAndMode(callId, mode);
		ret = iLtsyDispatchCallControlHold->HandleHoldReqL(callId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchHoldL

TInt CCallControlDispatcher::DispatchDialEmergencyL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallDialEmergencyCall
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlDialEmergency)
		{
		RMobileENStore::TEmergencyNumber* emergencyNumber;
		aDataPackage->UnPackData(&emergencyNumber);

		__ASSERT_DEBUG(emergencyNumber, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchCallControlDialEmergency->HandleDialEmergencyReqL(*emergencyNumber);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchDialEmergencyL

TInt CCallControlDispatcher::DispatchDialNoFdnCheckL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallDialNoFdnCheck
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TInt ret = DoDispatchDialL(aDataPackage, EFalse, EFalse);

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchDialNoFdnCheckL

TInt CCallControlDispatcher::DispatchStopDtmfToneL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneStopDTMFTone
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlStopDtmfTone)
		{
		TInt* callId;
		aDataPackage->UnPackData(&callId);
		ret = iLtsyDispatchCallControlStopDtmfTone->HandleStopDtmfToneReqL(*callId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchStopDtmfToneL

TInt CCallControlDispatcher::DispatchDialIsvL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallDialISV
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TInt ret = DoDispatchDialL(aDataPackage, ETrue, ETrue);

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchDialIsvL

TInt CCallControlDispatcher::DispatchSetActiveAlsLineL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetALSLine
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlSetActiveAlsLine)
		{
		RMobilePhone::TMobilePhoneALSLine* alsLine;
		aDataPackage->UnPackData(&alsLine);
		ret = iLtsyDispatchCallControlSetActiveAlsLine->HandleSetActiveAlsLineReqL(*alsLine);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchSetActiveAlsLineL

TInt CCallControlDispatcher::DispatchSendDtmfTonesCancelL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSendDTMFTonesCancel
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlSendDtmfTonesCancel)
		{
		TInt callId;
		aDataPackage->UnPackData(callId);
		ret = iLtsyDispatchCallControlSendDtmfTonesCancel->HandleSendDtmfTonesCancelReqL(callId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchSendDtmfTonesCancelL

TInt CCallControlDispatcher::DispatchHangUpL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EEtelCallHangUp
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	if (iLtsyDispatchCallControlHangUp)
		{
		TInt callId;
		RMobilePhone::TMobileService mode;
		const CCallDataPackage* callDataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
		callDataPackage->GetCallIdAndMode(callId, mode);

		TInt hangupCause;
		TBool autoStChangeDisable;
		callDataPackage->UnPackData(hangupCause, autoStChangeDisable);

		ret = iLtsyDispatchCallControlHangUp->HandleHangUpReqL(callId, hangupCause);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchHangUpL

TInt CCallControlDispatcher::DispatchResumeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallResume
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlResume)
		{
		const CCallDataPackage* dataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
		TInt callId;
		RMobilePhone::TMobileService mode;
		dataPackage->GetCallIdAndMode(callId, mode);
		ret = iLtsyDispatchCallControlResume->HandleResumeReqL(callId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchResumeL

TInt CCallControlDispatcher::DispatchSetDynamicHscsdParamsL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallSetDynamicHscsdParams
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchCallControlSetDynamicHscsdParams)
		{
		const CCallDataPackage* callDataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
		TInt callId;
		RMobilePhone::TMobileService mode;
		callDataPackage->GetCallIdAndMode(callId, mode);
		RMobileCall::TMobileHscsdCallParamsV1* hscsdParams;
		aDataPackage->UnPackData(&hscsdParams);
		ret = iLtsyDispatchCallControlSetDynamicHscsdParams->HandleSetDynamicHscsdParamsReqL(callId, *hscsdParams);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchSetDynamicHscsdParamsL

TInt CCallControlDispatcher::DispatchDialL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EEtelCallDial
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TInt ret = DoDispatchDialL(aDataPackage, EFalse, ETrue);

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchDialL

TInt CCallControlDispatcher::DispatchTransferL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallTransfer
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	if (iLtsyDispatchCallControlTransfer)
		{
		const CCallDataPackage* callDataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
		RMobilePhone::TMobileService mode;
		callDataPackage->GetCallIdAndMode(iTransferHeldCallId, mode);
		callDataPackage->UnPackData(iTransferSecondCallId);
		ret = iLtsyDispatchCallControlTransfer->HandleTransferReqL(iTransferHeldCallId, iTransferSecondCallId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchTransferL

TInt CCallControlDispatcher::DispatchSendDtmfTonesL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSendDTMFTones
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	if (iLtsyDispatchCallControlSendDtmfTones)
		{
		TInt* callId;
		TDesC* tones;
		aDataPackage->UnPackData(&callId, &tones);

		ret = DoSendDtmfTonesL(*callId, *tones);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchSendDtmfTonesL

TInt CCallControlDispatcher::DoSendDtmfTonesL(TInt aCallId, const TDesC& aTones)
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Locate a 'w' (wait) in the DTMF string
	// Send the part of the string before the 'w' to LTSY,
	// cache the rest.
	TChar w('w');
	TInt wPos = aTones.LocateF(w);
	if (wPos == KErrNotFound)
		{
		if (iLtsyDispatchCallControlSendDtmfTones)
			{
			ret = iLtsyDispatchCallControlSendDtmfTones->HandleSendDtmfTonesReqL(aCallId, aTones);
			}
		}
	else
		{
		iFoundDtmfStop = ETrue;
		iCallIdForDtmf = aCallId;

		// Send string before 'w' to LTSY
		TPtrC firstPartOfString = aTones.Left(wPos);
		if (iLtsyDispatchCallControlSendDtmfTones)
			{
			ret = iLtsyDispatchCallControlSendDtmfTones->HandleSendDtmfTonesReqL(aCallId, firstPartOfString);
			}

		// Cache string after the 'w' so that client can choose to continue sending
		// or not.

		// Buffer needs to be big enough to fit the rest of the string after the first 'w'
		// (not including the 'w' itself).
		iDtmfString = aTones.Right(aTones.Length() - wPos - 1);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DoSendDtmfTonesL


TInt CCallControlDispatcher::DispatchGetIdentityServiceStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneGetIdentityServiceStatus
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlGetIdentityServiceStatus)
		{
		RMobilePhone::TMobilePhoneIdService idService;
		aDataPackage->UnPackData(idService);
		ret = iLtsyDispatchCallControlGetIdentityServiceStatus->HandleGetIdentityServiceStatusReqL(idService);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchGetIdentityServiceStatusL

TInt CCallControlDispatcher::DispatchSwapL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallSwap
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchCallControlSwap)
		{
		const CCallDataPackage* dataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
		RMobilePhone::TMobileService mode;
		dataPackage->GetCallIdAndMode(iSwapCallId, mode);
		dataPackage->UnPackData(iSecondSwapCallId);

		if (iSwapCallId == KInvalidCallId)
			{
			// Swapping a single call.
			ret = iLtsyDispatchCallControlSwap->HandleSwapReqL(iSecondSwapCallId);
			}
		else if (iSecondSwapCallId == KInvalidCallId)
			{
			// Swapping a single call.
			ret = iLtsyDispatchCallControlSwap->HandleSwapReqL(iSwapCallId);
			}		
		else
			{
			ret = iLtsyDispatchCallControlSwap->HandleSwapReqL(iSwapCallId, iSecondSwapCallId);
			}
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchSwapL

TInt CCallControlDispatcher::DispatchContinueDtmfStringSendingL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneContinueDTMFStringSending
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TDesC* stringToSend;
	aDataPackage->UnPackData(&stringToSend);
	if (stringToSend)
		{
		__ASSERT_DEBUG(stringToSend, CtsyDispatcherPanic(EInvalidNullPtr));
		ret = DoSendDtmfTonesL(iCallIdForDtmf, *stringToSend);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchContinueDtmfStringSendingL

TInt CCallControlDispatcher::DispatchLoanDataPortL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EEtelCallLoanDataPort
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	if (iLtsyDispatchCallControlLoanDataPort)
		{
		const CCallDataPackage* callDataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
		TInt callId;
		RMobilePhone::TMobileService mode;
		callDataPackage->GetCallIdAndMode(callId, mode);
		RCall::TCommPort* commPort;
		callDataPackage->UnPackData(&commPort);
		ret = iLtsyDispatchCallControlLoanDataPort->HandleLoanDataPortSyncL(callId, *commPort);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchLoanDataPortL

TInt CCallControlDispatcher::DispatchRecoverDataPortL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EEtelCallRecoverDataPort
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	if (iLtsyDispatchCallControlRecoverDataPort)
		{
		const CCallDataPackage* callDataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
		TInt callId;
		RMobilePhone::TMobileService mode;
		callDataPackage->GetCallIdAndMode(callId, mode);
		RCall::TCommPort* commPort;
		callDataPackage->UnPackData(&commPort);
		ret = iLtsyDispatchCallControlRecoverDataPort->HandleRecoverDataPortSyncL(callId, *commPort);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchRecoverDataPortL

TInt CCallControlDispatcher::DispatchStartDtmfToneL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneStartDTMFTone
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlStartDtmfTone)
		{
		TChar* tone;
		TInt* callId;
		aDataPackage->UnPackData(&callId, &tone);

		ret = iLtsyDispatchCallControlStartDtmfTone->HandleStartDtmfToneReqL(*callId, *tone);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchStartDtmfToneL


TInt CCallControlDispatcher::DispatchGetActiveAlsLineL()
/**
 * Unpack data related to RMobilePhone::GetActiveAlsLine
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlGetActiveAlsLine)
		{
		ret = iLtsyDispatchCallControlGetActiveAlsLine->HandleGetActiveAlsLineReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchGetActiveAlsLineL

TInt CCallControlDispatcher::DispatchQueryIsEmergencyNumberL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomCheckEmergencyNumberIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlQueryIsEmergencyNumber)
		{
		__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

		RMmCustomAPI::TEmerNumberCheckMode* numberCheckMode;
		aDataPackage->UnPackData(&numberCheckMode);

		TBool isEmergencyNumber = ETrue;

		ret = iLtsyDispatchCallControlQueryIsEmergencyNumber->HandleQueryIsEmergencyNumberSyncL(
				numberCheckMode->iNumber, isEmergencyNumber);

		// Trigger async one shot completer
		if (ret == KErrNone)
			{
			// CTSY handling of ECustomCheckEmergencyNumberIPC is as follows...
			// It expects the lower layer to do the following:
			//
			//  - If the number is an emergency number, return KErrNone as error code,
			//	  and CMmDataPackage containing the number.
			//
			//  - If the number is not an emergency number, return KErrNone as error code,
			//	  and NULL in the CMmDataPackage.
			//
			//  - If there was a problem checking the number, return error != KErrNone.

			if (isEmergencyNumber)
				{
				iQueryEmergencyNumber = numberCheckMode->iNumber;
				}
			else
				{
				iQueryEmergencyNumber.Zero();
				}

			// For this particular IPC, the CMmDataPackage expected by CTSY on
			// callback contains the emergency number sent down originally rather
			// than anything explicitly returned by the LTSY in the sync call,
			// therefore the number is cached and the CMmDataPackage package is
			// constructed on callback.
			// dataPackage->iDataPackage is NULL here.
			CRequestQueueOneShot::TIpcDataPackage* dataPackage =
							new (ELeave) CRequestQueueOneShot::TIpcDataPackage;
			dataPackage->iIpc = ECustomCheckEmergencyNumberIPC;
			dataPackage->iResultCode = KErrNone;
			dataPackage->iDataPackage = NULL;
			dataPackage->iDispatcherCallback = this;
			iRequestAsyncOneShot.QueueRequest(*dataPackage);
			}
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchQueryIsEmergencyNumberL

TInt CCallControlDispatcher::DispatchGetAlsPpSupportL()
/**
 * Pass ECustomCheckAlsPpSupportIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlGetAlsPpSupport)
		{
		ret = iLtsyDispatchCallControlGetAlsPpSupport->HandleGetAlsPpSupportL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchGetAlsPpSupportL

TInt CCallControlDispatcher::DispatchGetAlsBlockedStatusL()
/**
 * Pass ECustomGetAlsBlockedIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlGetAlsBlockedStatus)
		{
		ret = iLtsyDispatchCallControlGetAlsBlockedStatus->HandleGetAlsBlockedStatusL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchGetAlsBlockedStatusL

TInt CCallControlDispatcher::DispatchSetAlsBlockedL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSetAlsBlockedIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlSetAlsBlocked)
		{
        RMmCustomAPI::TSetAlsBlock alsBlock;
		aDataPackage->UnPackData(alsBlock);
		ret = iLtsyDispatchCallControlSetAlsBlocked->HandleSetAlsBlockedL(alsBlock);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchSetAlsBlockedL

TInt CCallControlDispatcher::DispatchGetLifeTimeL()
/**
 * Pass ECustomGetLifeTimeIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlGetLifeTime)
		{
		ret = iLtsyDispatchCallControlGetLifeTime->HandleGetLifeTimeL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchGetLifeTimeL

TInt CCallControlDispatcher::DispatchTerminateCallL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomTerminateCallIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlTerminateErrorCall && iLtsyDispatchCallControlTerminateAllCalls)
		{
        TInt callId;
        TBool isError;
		aDataPackage->UnPackData(callId,isError);
        if ((isError) && (callId <= 0))
            {
            ret = KErrCorrupt;
            }
        else if ((!isError) && (callId > 0))
            {
            ret = KErrCorrupt;
            }
        else if(isError)
            {
            ret = iLtsyDispatchCallControlTerminateErrorCall->HandleTerminateErrorCallL(callId);
            }
        else
            {
            // The callId is the negative id of a call in a connecting state (if there is one)
            // we shall not pass it to the LTSY since our assumption is that it will know all calls (whether active or in the process of connecting).  
            ret = iLtsyDispatchCallControlTerminateAllCalls->HandleTerminateAllCallsL();
            }
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchTerminateCallL

TInt CCallControlDispatcher::DispatchGetCallForwardingIndicatorL()
/**
 * Pass ECustomGetIccCallForwardingStatusIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlGetCallForwardingIndicator)
		{
		ret = iLtsyDispatchCallControlGetCallForwardingIndicator->HandleGetCallForwardingIndicatorL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchGetCallForwardingIndicatorL

TInt CCallControlDispatcher::DispatchUpdateLifeTimeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECtsyUpdateLifeTimeReq / ECtsyUpdateLifeTimeComp IPCs
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlUpdateLifeTimer)
		{
		__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

		TUint32 duration;
		aDataPackage->UnPackData(duration);

		ret = iLtsyDispatchCallControlUpdateLifeTimer->HandleUpdateLifeTimerReqL(duration);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DispatchUpdateLifeTimeL

//
// Callback handlers follow
//


void CCallControlDispatcher::CallbackNotifyCallStatusChange(TInt aError,
		TInt aCallId, RMobileCall::TMobileCallStatus aMobileCallStatus)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlNotifyCallStatusChangeInd()
 *
 * Packs data into a format expected by the Common TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 *
 * @param aCallId The Call ID of the call aMobileCallStatus refers to.
 *
 * @param aMobileCallStatus The new status of the call refered to by aCallId.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d, aMobileCallStatus=%d"), aError, aCallId, aMobileCallStatus);

	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);
	dataPackage.PackData(const_cast<RMobileCall::TMobileCallStatus*>(&aMobileCallStatus));

	iMessageManagerCallback.Complete(EMobileCallNotifyMobileCallStatusChange, &dataPackage, aError);

	// For data calls, CTSY does not complete dial request back to client
	if (aMobileCallStatus == RMobileCall::EStatusConnected)
		{
		if (iFdnCheckPerformed)
			{
			iMessageManagerCallback.Complete(EEtelCallDial, &dataPackage, aError);
			}
		else
			{
			iMessageManagerCallback.Complete(EMobileCallDialNoFdnCheck, &dataPackage, aError);
			}
		}

	} // CCallControlDispatcher::CallbackNotifyCallStatusChange

void CCallControlDispatcher::CallbackReceiveUui(
		TInt aError, TInt aCallId, const TDesC& aCallUui)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlReceiveUuiInd().
 *
 * @param aError The error code to be returned to the CTSY.
 * @param aCallId The Call ID of the call.
 * @param aCallUui The UUI information received from the LTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	TPtrC callUui(aCallUui);

	// Pack the data to return to the Common TSY
	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);
	dataPackage.PackData(&callUui);

	iMessageManagerCallback.Complete(EMobileCallReceiveUUI, &dataPackage, aError);
	} // CCallControlDispatcher::CallbackReceiveUui

void CCallControlDispatcher::CallbackNotifyDataCallCapsChange(TInt aError,
	TInt aCallId, const RMobileCall::TMobileCallDataCapsV1& aMobileCallDataCaps)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlNotifyDataCallCapsChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId The Call ID of the call whose capabilities have changed.
 * @param aMobileCallDataCaps The new capabilities of the data call.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Pack the data to return to the Common TSY
	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);
	dataPackage.PackData(const_cast<RMobileCall::TMobileCallDataCapsV1*>(&aMobileCallDataCaps));

	iMessageManagerCallback.Complete(EMobileCallNotifyMobileDataCallCapsChange, &dataPackage, aError);
	} // CCallControlDispatcher::CallbackNotifyDataCallCapsChange

void CCallControlDispatcher::CallbackNotifyIncomingCall(TInt aError,
		const RMobileCall::TMobileCallInfoV1& aCallInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlNotifyIncomingCallInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallInfo Call information from LTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallInfo.iCallId, aCallInfo.iService);
	dataPackage.PackData(const_cast<RMobileCall::TMobileCallInfoV1*>(&aCallInfo));

	iMessageManagerCallback.Complete(EEtelLineNotifyIncomingCall, &dataPackage, aError);

	CallbackNotifyCallStatusChange(aError, aCallInfo.iCallId, RMobileCall::EStatusRinging);

	} // CCallControlDispatcher::CallbackNotifyIncomingCall


void CCallControlDispatcher::CallbackNotifyHscsdInfoChange(TInt aError,
		TInt aCallId, const RMobileCall::TMobileCallHscsdInfoV8& aCallParams,
		const RMobileCall::TMobileCallDataCapsV1& aCallCaps)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlNotifyHscsdInfoChangeInd().
 *
 * @param aError The error code returned from the LTSY.
 * @param aCallId The Call ID returned from the LTSY.
 * @param aCallParams The call params returned from the LTSY.
 * @param aCallInfo The call information returned from the LTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// Pack the data to return to the Common TSY
	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::ECircuitDataService);
	dataPackage.PackData(const_cast<RMobileCall::TMobileCallHscsdInfoV8*>(&aCallParams),
						 const_cast<RMobileCall::TMobileCallDataCapsV1*>(&aCallCaps));

	iMessageManagerCallback.Complete(EMobileCallNotifyHscsdInfoChange, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackNotifyHscsdInfoChange

void CCallControlDispatcher::CallbackNotifyCallEvent(TInt aError,
		TInt aCallId, RMobileCall::TMobileCallEvent aCallEvent)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlNotifyCallEventInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId Call ID returned from LTSY
 * @param aCallEvent Call event returned from LTSY
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d, aCallEvent=%d"), aError, aCallId, aCallEvent);

	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);
	dataPackage.PackData(const_cast<RMobileCall::TMobileCallEvent*>(&aCallEvent));

	iMessageManagerCallback.Complete(EMobileCallNotifyCallEvent, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackNotifyCallEvent

void CCallControlDispatcher::CallbackNotifyCallInfoChange(
	TInt aError, const RMobileCall::TMobileCallInfoV1& aMobileCallInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlNotifyCallInfoChangeInd().
 *
 * @param aError The error code returned by the LTSY.
 * @param aMobileCallInfo The call information relating to the call identified by aCallId.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, callId=%d, callMode=%d"), aError, aMobileCallInfo.iCallId, aMobileCallInfo.iService);

	// Pack the data to return to the Common TSY
	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aMobileCallInfo.iCallId, aMobileCallInfo.iService);
	dataPackage.PackData(const_cast<RMobileCall::TMobileCallInfoV1*>(&aMobileCallInfo));

	iMessageManagerCallback.Complete(EMobileCallGetMobileCallInfo, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackNotifyCallInfoChange

void CCallControlDispatcher::CallbackRemoteAlertingToneChange(
	TInt aError, RMmCustomAPI::TRemoteAlertingToneStatus aRemoteAlertingTone)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlRemoteAlertingToneChangeInd().
 *
 * @param aError The error code returned by the LTSY.
 * @param aRemoteAlertingTone The new remote alerting status (either RMmCustomAPI::EUiRbtTone
 *                            when it is a ringback tone or RMmCustomAPI::EUiStopTone when it's a stop tone)
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRemoteAlertingTone=%d"), aError, aRemoteAlertingTone);

	CMmDataPackage dataPackage;
	
	// Pack the data to return to the Common TSY
	if(aRemoteAlertingTone != RMmCustomAPI::EUiNoTone)
		{
		TBool playRemoteAlertToneLocally = (aRemoteAlertingTone == RMmCustomAPI::EUiRbtTone) ? TRUE : FALSE;
		dataPackage.PackData(&playRemoteAlertToneLocally);

		iMessageManagerCallback.Complete(ECustomGetRemoteAlertingToneStatusIPC, &dataPackage, aError);
		}

	dataPackage.PackData(&aRemoteAlertingTone);
	iMessageManagerCallback.Complete(ECustomNotifyRemoteAlertingToneStatusChangeIPC, &dataPackage, aError);      
      

	} // CCallControlDispatcher::CallbackRemoteAlertingToneChange


void CCallControlDispatcher::CallbackAnswer(TInt aError, TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlAnswerComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId The Call ID returned from the LTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	// For cases where the answer completes with no error, call status change
	// notifications come from the LTSY to indicate the call is connected.
	// The EStatusConnected status will complete the client's answer request.
	if (aError != KErrNone)
		{
		CCallDataPackage dataPackage;
		dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);
		iMessageManagerCallback.Complete(EEtelCallAnswer, &dataPackage, aError);
		}

	} // CCallControlDispatcher::CallbackAnswer


void CCallControlDispatcher::CallbackHold(TInt aError, TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlHoldComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId The Call ID of the call which was requested to be held.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);

	if (aError != KErrNone)
		{
		iMessageManagerCallback.Complete(EMobileCallHold, &dataPackage, aError);
		}

	} // CCallControlDispatcher::CallbackHold

void CCallControlDispatcher::CallbackDialEmergency(TInt aError,
	TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlDialEmergencyComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId The call ID returned from the LTSY
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	if (aError == KErrNone)
		{
		DoCallbackDial(aError, aCallId, RMobilePhone::EVoiceService);
		iModeUsedForVoiceCallDial = RMobilePhone::EServiceUnspecified;
		}
	else
		{
		// When dialling of an emergency call fails, the CTSY expects a particular
		// series of completions

		RMobileCall::TMobileCallInfoV1 callInfo;
		callInfo.iService = RMobilePhone::EVoiceService;
		callInfo.iCallId = aCallId;

		CCallDataPackage dataPackage;
		dataPackage.SetCallIdAndMode(aCallId, callInfo.iService);
		dataPackage.PackData(&callInfo);

		CallbackNotifyCallInfoChange(aError, callInfo);

		CallbackNotifyCallStatusChange(aError, aCallId, RMobileCall::EStatusIdle);
		}

	} // CCallControlDispatcher::CallbackDialEmergency


void CCallControlDispatcher::CallbackStopDtmfTone(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlStopDtmfToneComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneStopDTMFTone, aError);

	} // CCallControlDispatcher::CallbackStopDtmfTone

void CCallControlDispatcher::CallbackSetActiveAlsLine(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlSetActiveAlsLineComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSetALSLine, aError);

	} // CCallControlDispatcher::CallbackSetActiveAlsLine

void CCallControlDispatcher::CallbackSendDtmfTonesCancel(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesCancelComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSendDTMFTonesCancel, aError);
	} // CCallControlDispatcher::CallbackSendDtmfTonesCancel

void CCallControlDispatcher::CallbackHangUp(TInt aError, TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlHangUpComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId The Call ID of the call which has been hung up.
 *
 * @see RCall::HangUp
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);

	iMessageManagerCallback.Complete(EEtelCallHangUp, &dataPackage, aError);
	} // CCallControlDispatcher::CallbackHangUp

void CCallControlDispatcher::CallbackResume(TInt aError, TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlResumeComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId The Call ID returned by CCtsyDispatcherCallback.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);

	if (aError != KErrNone)
		{
		iMessageManagerCallback.Complete(EMobileCallResume, &dataPackage, aError);
		}

	} // CCallControlDispatcher::CallbackResume

void CCallControlDispatcher::CallbackSetDynamicHscsdParams(TInt aError,
	TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlSetDynamicHscsdParamsComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallId The Call ID returned to the CTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	CCallDataPackage dataPackage;
	dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);

	iMessageManagerCallback.Complete(EMobileCallSetDynamicHscsdParams, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackSetDynamicHscsdParams


void CCallControlDispatcher::CallbackDialVoice(TInt aError, TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlDialVoiceComp()
 *
 * @param aError The error code to be returned to the CTSY.
 *
 * @param aCallId The Call ID returned to the CTSY.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	DoCallbackDial(aError, aCallId, iModeUsedForVoiceCallDial);
	iModeUsedForVoiceCallDial = RMobilePhone::EServiceUnspecified;

	} // CCallControlDispatcher::CallbackDialVoice

void CCallControlDispatcher::CallbackTransfer(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlTransferComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// If the transfer was not called on this call ID, the CTSY will have
	// no reqHandle for this IPC so calling both here is OK.

	// Check in case LTSY has completed a transfer when no transfer was called
	// Don't complete back to CTSY in this case
	CCallDataPackage dataPackage;

	if (iTransferHeldCallId != KInvalidCallId)
		{
		dataPackage.SetCallIdAndMode(iTransferHeldCallId, RMobilePhone::EServiceUnspecified);
		iTransferHeldCallId = KInvalidCallId;
		iMessageManagerCallback.Complete(EMobileCallTransfer, &dataPackage, aError);
		}

	if (iTransferSecondCallId != KInvalidCallId)
		{
		dataPackage.SetCallIdAndMode(iTransferSecondCallId, RMobilePhone::EServiceUnspecified);
		iSecondSwapCallId = KInvalidCallId;
		iMessageManagerCallback.Complete(EMobileCallTransfer, &dataPackage, aError);
		}

	} // CCallControlDispatcher::CallbackTransfer

void CCallControlDispatcher::CallbackSendDtmfTones(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlSendDtmfTonesComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	if (iFoundDtmfStop)
		{
		// Part of the DTMF string (after the 'w') has been cached, and there is
		// still more to send so notify the client of the stop character
		// Make a copy of the cached data as CTSY deletes previous pointer
		CMmDataPackage dataPackage;
		dataPackage.PackData(&iDtmfString);
		iMessageManagerCallback.Complete(EMobilePhoneNotifyStopInDTMFString, &dataPackage, aError);
		iDtmfString.Zero();
		iFoundDtmfStop = EFalse;
		}
	else
		{
		// There was no 'w' in the string, normal completion of send
		iMessageManagerCallback.Complete(EMobilePhoneSendDTMFTones, aError);
		}

	} // CCallControlDispatcher::CallbackSendDtmfTones

void CCallControlDispatcher::CallbackGetIdentityServiceStatus(TInt aError,
		RMobilePhone::TMobilePhoneIdServiceStatus aIdentityServiceStatus)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlGetIdentityServiceStatusComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aIdentityServiceStatus The identity service status returned to the CTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIdentityServiceStatus=%d"), aError, aIdentityServiceStatus);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneIdServiceStatus*>(&aIdentityServiceStatus));

	iMessageManagerCallback.Complete(EMobilePhoneGetIdentityServiceStatus, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackGetIdentityServiceStatus

void CCallControlDispatcher::CallbackSwap(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlSwapComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	if (aError != KErrNone)
		{
		CCallDataPackage dataPackage;

		// If the swap was not called on this call ID, the CTSY will have
		// no reqHandle for this IPC so calling both here is OK.

		// Check in case LTSY has completed a swap when no swap was called
		// Don't complete back to CTSY in this case

		if (iSwapCallId != KInvalidCallId)
			{
			dataPackage.SetCallIdAndMode(iSwapCallId, RMobilePhone::EServiceUnspecified);
			iSwapCallId = KInvalidCallId;
			iMessageManagerCallback.Complete(EMobileCallSwap, &dataPackage, aError);
			}

		if (iSecondSwapCallId != KInvalidCallId)
			{
			dataPackage.SetCallIdAndMode(iSecondSwapCallId, RMobilePhone::EServiceUnspecified);
			iSecondSwapCallId = KInvalidCallId;
			iMessageManagerCallback.Complete(EMobileCallSwap, &dataPackage, aError);
			}

		}

	} // CCallControlDispatcher::CallbackSwap

void CCallControlDispatcher::CallbackStartDtmfTone(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlStartDtmfToneComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneStartDTMFTone, aError);

	} // CCallControlDispatcher::CallbackStartDtmfTone

void CCallControlDispatcher::CallbackGetActiveAlsLine(
						TInt aError, RMobilePhone::TMobilePhoneALSLine aAlsLine)
/**
 * Packages the data returned by the Licensee LTSY and completes the
 * request back to the Common TSY.
 *
 * @param aError The error code to complete back.
 * @param aAlsLine The ALS line returned by the Licensee LTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d aAlsLine=%d"), aError, aAlsLine);

	// Package up data to return to Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneALSLine*>(&aAlsLine));

	// Now complete the actual get request
	iMessageManagerCallback.Complete(EMobilePhoneGetALSLine, &dataPackage, aError);

	} // CPhoneDispatcher::CallbackGetActiveAlsLine

void CCallControlDispatcher::CallbackDialData(TInt aError, TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlDialDataComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 * @param aCallId The Call ID returned to the CTSY.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	DoCallbackDial(aError, aCallId, RMobilePhone::ECircuitDataService);
	iModeUsedForVoiceCallDial = RMobilePhone::EServiceUnspecified;

	} // CCallControlDispatcher::CallbackDialData

void CCallControlDispatcher::CallbackGetAlsPpSupport(TInt aError, RMmCustomAPI::TAlsSupport aAlsSupport)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlGetAlsPpSupportComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 * @param aAlsSupport The return value from the LTSY.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAlsSupport=%d"), aError, aAlsSupport);

	// Package up data to return to Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aAlsSupport);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomCheckAlsPpSupportIPC, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackGetAlsPpSupport

void CCallControlDispatcher::CallbackGetAlsBlockedStatus(TInt aError, RMmCustomAPI::TGetAlsBlockStatus aAlsStatus)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlGetAlsBlockedStatusComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 * @param aAlsStatus The return value from the LTSY.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAlsStatus=%d"), aError, aAlsStatus);

	// Package up data to return to Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aAlsStatus);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomGetAlsBlockedIPC, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackGetAlsBlockedStatus

void CCallControlDispatcher::CallbackSetAlsBlocked(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlSetAlsBlockedComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomSetAlsBlockedIPC, aError);

	} // CCallControlDispatcher::CallbackSetAlsBlocked

void CCallControlDispatcher::CallbackGetLifeTime(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Package up data to return to Common TSY
    RMmCustomAPI::TLifeTimeData data;
    data.iCaps = 0;
	CMmDataPackage dataPackage;
	dataPackage.PackData(&data);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomGetLifeTimeIPC, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackGetLifeTime

void CCallControlDispatcher::CallbackGetLifeTime(TInt aError, TUint32 aHours, TUint8 aMinutes)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 * @param aHours The amount of hours of airtime use since the manufacturing date. The range of the value should be 0-999999.
 * @param aMinutes The amount of minutes in addition to the amount of hours represented by aHours. The range of the value is 0-59.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aHours=%d, aMinutes=%d"), aError, aHours, aMinutes);

	// Package up data to return to Common TSY
    RMmCustomAPI::TLifeTimeData data;
    data.iCaps = RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsLifeTime;
    data.iHours = aHours;
    data.iMinutes = aMinutes;
	CMmDataPackage dataPackage;
	dataPackage.PackData(&data);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomGetLifeTimeIPC, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackGetLifeTime

void CCallControlDispatcher::CallbackGetLifeTime(TInt aError, const TDateTime &aManufacturingDate)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 * @param aManufacturingDate The date of phone manufacturing. Only the Year, Month and day information is meaningful.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Package up data to return to Common TSY
    RMmCustomAPI::TLifeTimeData data;
    data.iCaps = RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsManufacturerDate;
    data.iManufacturingDate = aManufacturingDate;
	CMmDataPackage dataPackage;
	dataPackage.PackData(&data);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomGetLifeTimeIPC, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackGetLifeTime

void CCallControlDispatcher::CallbackGetLifeTime(TInt aError, const TDateTime &aManufacturingDate, TUint32 aHours, TUint8 aMinutes)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlGetLifeTimeComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 * @param aManufacturingDate The date of phone manufacturing. Only the Year, Month and day information is meaningful.
 * @param aHours The amount of hours of airtime use since the manufacturing date. The range of the value should be 0-999999.
 * @param aMinutes The amount of minutes in addition to the amount of hours represented by aHours. The range of the value is 0-59.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aHours=%d, aMinutes=%d"), aError, aHours, aMinutes);

	// Package up data to return to Common TSY
    RMmCustomAPI::TLifeTimeData data;
    data.iCaps = RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsLifeTime | RMmCustomAPI::TLifeTimeData::ELifeTimeDataCapsManufacturerDate;
    data.iHours = aHours;
    data.iMinutes = aMinutes;
    data.iManufacturingDate = aManufacturingDate;
	CMmDataPackage dataPackage;
	dataPackage.PackData(&data);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomGetLifeTimeIPC, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackGetLifeTime

void CCallControlDispatcher::CallbackTerminateErrorCall(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlTerminateErrorCallComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomTerminateCallIPC, aError);

	} // CCallControlDispatcher::CallbackTerminateErrorCall

void CCallControlDispatcher::CallbackTerminateAllCalls(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlTerminateAllCallsComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomTerminateCallIPC, aError);

	} // CCallControlDispatcher::CallbackTerminateAllCalls

void CCallControlDispatcher::CallbackGetCallForwardingIndicator(TInt aError, RMobilePhone::TMobileTON aTypeOfNumber,
                                                                RMobilePhone::TMobileNPI aMobilePlan,
                                                                const TDesC &aNumber,
                                                                RMobilePhone::TCFUIndicatorStatusFlags aCFUIndicatorStatusFlags,
                                                                RMobilePhone::TMultipleSubscriberProfileID aMultipleSubscriberProfileId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlGetCallForwardingIndicatorComp().
 *
 * @param aError The error code to be returned to the CTSY.
 *
 * @param aTypeOfNumber The call forwarding type of number.
 * @param aMobilePlan The call forwarding mobile plan.
 * @param aNumber The call forwarding telephone number.
 * @param aCFUIndicatorStatusFlags The setting for indicator status. Can contain a number of flags from RMobilePhone::TCFUIndicatorStatus.
 * @param aMultipleSubscriberProfileID The multiple subscriber profile ID.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, Flags=%d, ProfileId=%d"), aError, aCFUIndicatorStatusFlags, aMultipleSubscriberProfileId);

	// Package up data to return to Common TSY
    RMmCustomAPI::TCFIndicators data;
    data.iCFNumber.iTypeOfNumber = aTypeOfNumber;
    data.iCFNumber.iNumberPlan = aMobilePlan;
    data.iCFNumber.iTelNumber = aNumber;
    data.iIndicator = aCFUIndicatorStatusFlags;
    switch(aMultipleSubscriberProfileId)
    	{
    	case RMobilePhone::KProfileIdentityUnknown:
    		break;
    	case RMobilePhone::KProfileIdentityOne:
    		data.iMultipleSubscriberProfileID = RMmCustomAPI::KProfileIdentityOne;
    		break;
    	case RMobilePhone::KProfileIdentityTwo:
    		data.iMultipleSubscriberProfileID = RMmCustomAPI::KProfileIdentityTwo;
    		break;
    	case RMobilePhone::KProfileIdentityThree:
    		data.iMultipleSubscriberProfileID = RMmCustomAPI::KProfileIdentityThree;
    		break;
    	case RMobilePhone::KProfileIdentityFour:
    		data.iMultipleSubscriberProfileID = RMmCustomAPI::KProfileIdentityFour;
    		break;
    		
    	}
	CMmDataPackage dataPackage;
	dataPackage.PackData(&data);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(ECustomGetIccCallForwardingStatusIPC, &dataPackage, aError);

	} // CCallControlDispatcher::CallbackGetCallForwardingIndicator


void CCallControlDispatcher::CallbackQueryIsEmergencyNumber(
						CRequestQueueOneShot::TIpcDataPackage* aDataPackage)
/**
 * Callback function to be used by the request to complete
 * MLtsyDispatchCallControlQueryIsEmergencyNumber::HandleQueryIsEmergencyNumberSyncL.
 *
 * @param aDataPackage Structure encapulsating data about the request.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("iResultCode=%d"), aDataPackage->iResultCode);

	if (!aDataPackage->iCleanupOnly)
		{
		RMmCustomAPI::TMobileTelNumber* numberPtr = NULL;
		if (iQueryEmergencyNumber.Length() > 0)
			{
			numberPtr = &iQueryEmergencyNumber;
			}

		CMmDataPackage mmDataPackage;
		mmDataPackage.PackData(&numberPtr);

		iMessageManagerCallback.Complete(ECustomCheckEmergencyNumberIPC, &mmDataPackage, aDataPackage->iResultCode);

		iQueryEmergencyNumber.Zero();
		}

	delete aDataPackage;
	} // CCallControlDispatcher::CallbackQueryIsEmergencyNumber

void CCallControlDispatcher::CallbackUpdateLifeTimer(TInt aError)
/**
 * Callback function to be used by the request to complete
 * MLtsyDispatchCallControlUpdateLifeTimer::HandleUpdateLifeTimerReqL.
 *
 * @param aError The error code to be returned to the CTSY
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECtsyUpdateLifeTimeComp, aError);

	} // CCallControlDispatcher::CallbackUpdateLifeTimer

TInt CCallControlDispatcher::DoDispatchDialL(const CMmDataPackage* aDataPackage,
		TBool aIsIsvCall, TBool aPerformFdnCheck)
	{
	TSYLOGENTRYEXITARGS(_L8("aIsIsvCall=%d,aPerformFdnCheck=%d"), aIsIsvCall, aPerformFdnCheck);

	TInt ret = KErrNotSupported;
	
	iFdnCheckPerformed = aPerformFdnCheck;

	TInt callId;
	RMobilePhone::TMobileService mode;
	const CCallDataPackage* callDataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
	callDataPackage->GetCallIdAndMode(callId, mode);

	iModeUsedForVoiceCallDial = mode;

	RMobileCall::TMobileCallInfoV1* callInfo;
	RMobileCall::TMobileCallParamsV1Pckg* callParamsPckg;
	aDataPackage->UnPackData(&callParamsPckg, &callInfo);

	__ASSERT_DEBUG(iDispatcherHolder, CtsyDispatcherPanic(EInvalidNullPtr));

	RMobileCall::TCallParamOrigin callOrigin;

	CSatDispatcher& satDispatcher = iDispatcherHolder->GetSatDispatcher();
	if (satDispatcher.IsSimOriginatedCall(callInfo->iDialledParty))
		{
		callOrigin = RMobileCall::EOriginatorSIM;
		}
	else
		{
		callOrigin = RMobileCall::EOriginatorEtelClient;
		}

	// Call Handle... method in Licensee LTSY
	if (mode == RMobilePhone::EVoiceService &&
			iLtsyDispatchCallControlDialVoice)
		{
		if (!aIsIsvCall)
			{
			ret = iLtsyDispatchCallControlDialVoice->HandleDialVoiceReqL(
				RMobilePhone::EAlternateLinePrimary, callInfo->iDialledParty,
				((*callParamsPckg)()), aIsIsvCall, callOrigin, aPerformFdnCheck);
			}
		else
			{
			aDataPackage->UnPackData(&callParamsPckg, &callInfo);

			RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg* callParams3rdPartyPckg =
			reinterpret_cast<RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg*>(callParamsPckg);
			RMobileCall::TEtel3rdPartyMobileCallParamsV1* callParams3rdParty = &((*callParams3rdPartyPckg)());

			RMobileCall::TMobileCallParamsV1 newCallParamsV1;
			RMobileCall::TMobileCallParamsV1Pckg newCallParamsV1Pckg(newCallParamsV1);

			// Pack params from 3rd party to V1
			newCallParamsV1.iAutoRedial = callParams3rdParty->iAutoRedial;
			newCallParamsV1.iIdRestrict = callParams3rdParty->iIdRestrict;

			ret = iLtsyDispatchCallControlDialVoice->HandleDialVoiceReqL(
				RMobilePhone::EAlternateLinePrimary, callInfo->iDialledParty,
				newCallParamsV1, aIsIsvCall, callOrigin, aPerformFdnCheck);

			}
		}
	else if (mode == RMobilePhone::EAuxVoiceService &&
			iLtsyDispatchCallControlDialVoice)
		{
		ret = iLtsyDispatchCallControlDialVoice->HandleDialVoiceReqL(
			RMobilePhone::EAlternateLineAuxiliary, callInfo->iDialledParty,
			((*callParamsPckg)()), aIsIsvCall, callOrigin, aPerformFdnCheck);

		}
	else if (mode == RMobilePhone::ECircuitDataService &&
		iLtsyDispatchCallControlDialData)
		{
		RMobileCall::TMobileCallInfoV1* callInfo;
		RMobileCall::TMobileDataCallParamsV1Pckg* callParamsPckg;
				aDataPackage->UnPackData(&callParamsPckg, &callInfo);

		ret = iLtsyDispatchCallControlDialData->HandleDialDataReqL(
			callInfo->iDialledParty, ((*callParamsPckg)()), aPerformFdnCheck);
		}
	else
		{
		CtsyDispatcherPanic(EInvalidParameter);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DoDispatchDialL

TInt CCallControlDispatcher::DoDispatchAnswerL(
		const CMmDataPackage* aDataPackage,	TBool aIsIsvRequest)
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	if (iLtsyDispatchCallControlAnswer)
			{
			const CCallDataPackage* dataPackage = static_cast<const CCallDataPackage*>(aDataPackage);
			TInt callId;
			RMobilePhone::TMobileService mode;
			dataPackage->GetCallIdAndMode(callId, mode);

			ret = iLtsyDispatchCallControlAnswer->HandleAnswerReqL(callId, aIsIsvRequest);
			}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlDispatcher::DoDispatchAnswerL

void CCallControlDispatcher::DoCallbackDial(TInt aError, TInt aCallId,
	RMobilePhone::TMobileService aMode)
	{
	TSYLOGENTRYEXIT;

	RMobileCall::TMobileCallInfoV1 callInfo;
	callInfo.iService = aMode;
	CCallDataPackage dataPackage;

	if (aError == KErrNone)
		{
		// CTSY expects  completion of EMobileCallGetMobileCallInfo when a
		// dial is requested to get the call ID in the CTSY call object to be set
		// correctly.
		dataPackage.SetCallIdAndMode(aCallId, callInfo.iService);
		dataPackage.PackData(&callInfo);
		iMessageManagerCallback.Complete(EMobileCallGetMobileCallInfo, &dataPackage, aError);
		}
	else
		{
		dataPackage.SetCallIdAndMode(0, callInfo.iService);
		dataPackage.PackData(&callInfo);

		// CTSY only expects EEtelCallDial IPC on a failure to dial
		if (iFdnCheckPerformed)
			{
			iMessageManagerCallback.Complete(EEtelCallDial, &dataPackage, aError);
			}
		else
			{
			iMessageManagerCallback.Complete(EMobileCallDialNoFdnCheck, &dataPackage, aError);
			}
		}

	} // CCallControlDispatcher::DoCallbackDial

void CCallControlDispatcher::CallbackNotifyIccCallForwardingStatusChange(TInt aError,
		 const RMobilePhone::TMobileAddress& aCallForwardingNo,
		 RMobilePhone::TCFUIndicatorStatusFlags aCallForwardingStatusFlags,
		 RMmCustomAPI::TMultipleSubscriberProfileID aMultipleSubscriberProfileId
		 )
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackCallControlNotifyIccCallForwardingStatusChangeInd().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCallForwardingNo the number that is being forwarded.
 * @param aCallForwardingStatusFlags flags defined in RMobilePhone::TCFUIndicatorStatus.
 * @param aMultipleSubscriberProfileId the Profile ID, in the case of Multiple Subscribers (SIM). 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	// Pack the data to return to the Common TSY
	RMmCustomAPI::TCFIndicators callForwardingIndicator;
	callForwardingIndicator.iCFNumber = aCallForwardingNo;
	callForwardingIndicator.iIndicator = aCallForwardingStatusFlags;
	callForwardingIndicator.iMultipleSubscriberProfileID = aMultipleSubscriberProfileId;
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMmCustomAPI::TCFIndicators*>(&callForwardingIndicator));

	iMessageManagerCallback.Complete(ECustomNotifyIccCallForwardingStatusChangeIPC, &dataPackage, aError);
	
	} // CSimDispatcher::CallbackNotifyIccCallForwardingStatusChange

void CCallControlDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
/**
 * Part of the MDispatcherCallback interface. Used to complete requests handled
 * synchronously by the Licensee LTSY asynchronously back to the Common TSY.
 *
 * @param aIpcDataPackage Package encapsulating the request.
 *
 * @see MDispatcherCallback::CallbackSync()
 */
	{
	TSYLOGENTRYEXIT;

	switch (aIpcDataPackage.iIpc)
		{
	case ECustomCheckEmergencyNumberIPC:
		CallbackQueryIsEmergencyNumber(&aIpcDataPackage);
		break;
	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCALLCONTROLDISPATCHER_CALLBACKSYNC_1, "WARNING: CCallControlDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;
		} // switch (aIpcDataPackage.iIpc)

	} // CCallControlDispatcher::CallbackSync

