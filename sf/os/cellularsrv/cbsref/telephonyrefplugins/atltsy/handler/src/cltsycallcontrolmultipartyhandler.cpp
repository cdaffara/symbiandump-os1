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
// CLtsyCallControlMultipartyHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>



#include "cltsycallcontrolmultipartyhandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"



CLtsyCallControlMultipartyHandler::CLtsyCallControlMultipartyHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}


CLtsyCallControlMultipartyHandler::~CLtsyCallControlMultipartyHandler()
	{
	}


CLtsyCallControlMultipartyHandler* CLtsyCallControlMultipartyHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyCallControlMultipartyHandler* self = new (ELeave) CLtsyCallControlMultipartyHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyCallControlMultipartyHandler* CLtsyCallControlMultipartyHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyCallControlMultipartyHandler* self=
			CLtsyCallControlMultipartyHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyCallControlMultipartyHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;


	} // CLtsyCallControlMultipartyHandler::ConstructL

TBool CLtsyCallControlMultipartyHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 *
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{

	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;

		default:
			return EFalse;
		}
	}

void CLtsyCallControlMultipartyHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
/**
 * Returns the set of callback indicators that are supported.
 *
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{

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


TInt CLtsyCallControlMultipartyHandler::HandleConferenceHangUpReqL(const RArray<TInt>& /*aCallIds*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceHangUpComp()
 *
 * @param aCallIds Array of all Call IDs, which belong to the conference.
 * This array will be destroyed after the function call returns. 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlMultipartyHandler::HandleConferenceHangUpReqL


TInt CLtsyCallControlMultipartyHandler::HandleConferenceAddCallReqL(TInt /*aCallId*/, TInt /*aExistingCallId*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceAddCallComp()
 *
 * @param aCallId The Call ID of the call to be added to the conference.
 * @param aExistingCallId The Call ID of one of the calls which is already a part of the conference call.
 * This can be the Call ID of any of the existing calls in the conference, not necessarily the first one that was added.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlMultipartyHandler::HandleConferenceAddCallReqL


TInt CLtsyCallControlMultipartyHandler::HandleCreateConferenceReqL(TInt /*aHeldCallId*/, TInt /*aSecondCallId*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyCreateConferenceComp()
 *
 * @param aHeldCallId The Call ID of the held call.
 *
 * @param aSecondCallId The Call ID of the other call to create a conference with.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlMultipartyHandler::HandleCreateConferenceReqL


TInt CLtsyCallControlMultipartyHandler::HandleConferenceSwapReqL(
		TInt /*aHeldCallId*/, TInt /*aConnectedCallId*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceSwapComp()
 *
 * @param aHeldCallId The Call ID of the held call.
 *
 * @param aConnectedCallId The Call ID of the connected call.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlMultipartyHandler::HandleConferenceSwapReqL


TInt CLtsyCallControlMultipartyHandler::HandleConferenceGoOneToOneReqL(TInt /*aCallId*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceGoOneToOneComp()
 *
 * @param aCallId The Call ID of the call with which to go one to one.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsyCallControlMultipartyHandler::HandleConferenceGoOneToOneReqL



