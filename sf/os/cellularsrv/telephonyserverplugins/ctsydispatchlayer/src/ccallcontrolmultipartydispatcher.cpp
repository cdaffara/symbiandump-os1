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
#include "ccallcontrolmultipartydispatcherTraces.h"
#endif

#include "ccallcontrolmultipartydispatcher.h"

#include <ctsy/ltsy/mltsydispatchcallcontrolmultipartyinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>

#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"

CCallControlMultipartyDispatcher::CCallControlMultipartyDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CCallControlMultipartyDispatcher::CCallControlMultipartyDispatcher


CCallControlMultipartyDispatcher::~CCallControlMultipartyDispatcher()
	{
	} // CCallControlMultipartyDispatcher::~CCallControlMultipartyDispatcher


CCallControlMultipartyDispatcher* CCallControlMultipartyDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CCallControlMultipartyDispatcher* self =
		new (ELeave) CCallControlMultipartyDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CCallControlMultipartyDispatcher::NewLC


CCallControlMultipartyDispatcher* CCallControlMultipartyDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CCallControlMultipartyDispatcher* self =
		CCallControlMultipartyDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CCallControlMultipartyDispatcher::NewL


void CCallControlMultipartyDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;

	// Get the Licensee LTSY interfaces related to CallControlMultiparty functionality
	// from the factory


	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlMultipartyFuncUnitId, MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId))
		{
		TAny* conferenceHangUpInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId,
		       	conferenceHangUpInterface);
		iLtsyDispatchCallControlMultipartyConferenceHangUp =
				static_cast<MLtsyDispatchCallControlMultipartyConferenceHangUp*>(conferenceHangUpInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlMultipartyConferenceHangUp, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlMultipartyFuncUnitId, MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId))
		{
		TAny* conferenceAddCallInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId,
		       	conferenceAddCallInterface);
		iLtsyDispatchCallControlMultipartyConferenceAddCall =
				static_cast<MLtsyDispatchCallControlMultipartyConferenceAddCall*>(conferenceAddCallInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlMultipartyConferenceAddCall, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlMultipartyFuncUnitId, MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId))
		{
		TAny* createConferenceInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId,
		       	createConferenceInterface);
		iLtsyDispatchCallControlMultipartyCreateConference =
				static_cast<MLtsyDispatchCallControlMultipartyCreateConference*>(createConferenceInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlMultipartyCreateConference, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlMultipartyFuncUnitId, MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId))
		{
		TAny* conferenceSwapInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId,
		       	conferenceSwapInterface);
		iLtsyDispatchCallControlMultipartyConferenceSwap =
				static_cast<MLtsyDispatchCallControlMultipartyConferenceSwap*>(conferenceSwapInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlMultipartyConferenceSwap, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchCallControlMultipartyFuncUnitId, MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId))
		{
		TAny* conferenceGoOneToOneInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId,
		       	conferenceGoOneToOneInterface);
		iLtsyDispatchCallControlMultipartyConferenceGoOneToOne =
				static_cast<MLtsyDispatchCallControlMultipartyConferenceGoOneToOne*>(conferenceGoOneToOneInterface);
        __ASSERT_DEBUG(iLtsyDispatchCallControlMultipartyConferenceGoOneToOne, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	} // CCallControlMultipartyDispatcher::ConstructL

void CCallControlMultipartyDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.
 *
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;

	iDispatcherHolder = &aDispatcherHolder;
	} // CCallControlMultipartyDispatcher::SetDispatcherHolder

TInt CCallControlMultipartyDispatcher::DispatchConferenceHangUpL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileConferenceCallHangUp
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
	if (iLtsyDispatchCallControlMultipartyConferenceHangUp)
		{		
		RArray<TInt>* pCallIds = NULL;
		aDataPackage->UnPackData(&pCallIds);
		ret = iLtsyDispatchCallControlMultipartyConferenceHangUp->HandleConferenceHangUpReqL(*pCallIds);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlMultipartyDispatcher::DispatchConferenceHangUpL

TInt CCallControlMultipartyDispatcher::DispatchConferenceAddCallL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileConferenceCallAddCall
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
	if (iLtsyDispatchCallControlMultipartyConferenceAddCall)
		{
		TInt callId;
		RMobilePhone::TMobileService mode;
		const CCallDataPackage& callDataPackage = *static_cast<const CCallDataPackage*>(aDataPackage);
		callDataPackage.GetCallIdAndMode(callId, mode);
		TInt existingCallId;
		callDataPackage.UnPackData(existingCallId);

		ret = iLtsyDispatchCallControlMultipartyConferenceAddCall->HandleConferenceAddCallReqL(callId, existingCallId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlMultipartyDispatcher::DispatchConferenceAddCallL

TInt CCallControlMultipartyDispatcher::DispatchCreateConferenceL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileConferenceCallCreateConference
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
	if (iLtsyDispatchCallControlMultipartyCreateConference)
		{
		TInt heldCallId;
		TInt secondCallId;
		RMobilePhone::TMobileService mode;
		const CCallDataPackage& callDataPackage = *static_cast<const CCallDataPackage*>(aDataPackage);
		callDataPackage.GetCallIdAndMode(heldCallId, mode);
		callDataPackage.UnPackData(secondCallId);

		ret = iLtsyDispatchCallControlMultipartyCreateConference->HandleCreateConferenceReqL(heldCallId, secondCallId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlMultipartyDispatcher::DispatchCreateConferenceL

TInt CCallControlMultipartyDispatcher::DispatchConferenceSwapL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileConferenceCallSwap
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
	if (iLtsyDispatchCallControlMultipartyConferenceSwap)
		{
		TInt callId;
		TInt heldCallId;
		TInt connectedCallId;
		RMobilePhone::TMobileService mode;
		const CCallDataPackage& callDataPackage = *static_cast<const CCallDataPackage*>(aDataPackage);
		callDataPackage.GetCallIdAndMode(callId, mode);
		callDataPackage.UnPackData(heldCallId, connectedCallId);
		ret = iLtsyDispatchCallControlMultipartyConferenceSwap->HandleConferenceSwapReqL(heldCallId, connectedCallId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlMultipartyDispatcher::DispatchConferenceSwapL

TInt CCallControlMultipartyDispatcher::DispatchConferenceGoOneToOneL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobileCallGoOneToOne
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 *
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchCallControlMultipartyConferenceGoOneToOne)
		{
		TInt callId;
		RMobilePhone::TMobileService mode;
		const CCallDataPackage& callDataPackage = *static_cast<const CCallDataPackage*>(aDataPackage);
		callDataPackage.GetCallIdAndMode(callId, mode);
		ret = iLtsyDispatchCallControlMultipartyConferenceGoOneToOne->HandleConferenceGoOneToOneReqL(callId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CCallControlMultipartyDispatcher::DispatchConferenceGoOneToOneL


//
// Callback handlers follow
//

void CCallControlMultipartyDispatcher::CallbackConferenceHangUp(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceHangUpComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Only need to complete IPC to CTSY on a failure because
	// call status change notifications (to idle) will signify that the
	// conference has been hung up.
	if (aError != KErrNone)
		{
		iMessageManagerCallback.Complete(EMobileConferenceCallHangUp, aError);
		}

	} // CCallControlMultipartyDispatcher::CallbackConferenceHangUp

void CCallControlMultipartyDispatcher::CallbackConferenceAddCall(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceAddCallComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Only complete failure to CTSY. Success is indicated by change in call
	// status via mobile call status change notifications.
	if (aError != KErrNone)
		{
		iMessageManagerCallback.Complete(EMobileConferenceCallAddCall, aError);
		}

	} // CCallControlMultipartyDispatcher::CallbackConferenceAddCall

void CCallControlMultipartyDispatcher::CallbackCreateConference(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyCreateConferenceComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Only complete IPC back to CTSY in the case of an error.
	// If the create conference request was made successfully, CTSY will be
	// notified of successful completion of this request when the call statuses
	// of the calls in the conference call change.
	if (aError != KErrNone)
		{
		iMessageManagerCallback.Complete(EMobileConferenceCallCreateConference, aError);
		}

	} // CCallControlMultipartyDispatcher::CallbackCreateConference

void CCallControlMultipartyDispatcher::CallbackConferenceSwap(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceSwapComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 *
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Only complete IPC back to CTSY in the case of an error.
	// If the swap request was made successfully, CTSY will be
	// notified of successful completion of this request when the call statuses
	// of the calls in the conference call change.
	if (aError != KErrNone)
		{
		iMessageManagerCallback.Complete(EMobileConferenceCallSwap, aError);
		}

	} // CCallControlMultipartyDispatcher::CallbackConferenceSwap

void CCallControlMultipartyDispatcher::CallbackConferenceGoOneToOne(TInt aError, TInt aCallId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceGoOneToOneComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 *
 * @param aCallId The Call ID to which the One-To-One request refers.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCallId=%d"), aError, aCallId);

	if (aError != KErrNone)
		{
		CCallDataPackage dataPackage;
		dataPackage.SetCallIdAndMode(aCallId, RMobilePhone::EServiceUnspecified);
		iMessageManagerCallback.Complete(EMobileCallGoOneToOne, &dataPackage, aError);
		}

	} // CCallControlMultipartyDispatcher::CallbackConferenceGoOneToOne



void CCallControlMultipartyDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
/**
 * Part of the MDispatcherCallback interface. Used to complete requests handled
 * synchronously by the Licensee LTSY asynchronously back to the Common TSY.
 *
 * @param aIpcDataPackage Package encapsulating the request.
 *
 * @see MDispatcherCallback::CallbackSync
 */
	{
	TSYLOGENTRYEXIT;

	switch (aIpcDataPackage.iIpc)
		{

	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCALLCONTROLMULTIPARTYDISPATCHER_CALLBACKSYNC_1, "WARNING: CCallControlMultipartyDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;
		} // switch (aIpcDataPackage.iIpc)

	} // CCallControlMultipartyDispatcher::CallbackSync



