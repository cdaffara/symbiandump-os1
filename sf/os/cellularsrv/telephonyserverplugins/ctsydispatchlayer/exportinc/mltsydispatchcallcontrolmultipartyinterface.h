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
// the Licensee LTSY relating to CallControlMultiparty related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHCALLCONTROLMULTIPARTYINTERFACE_H_
#define MLTSYDISPATCHCALLCONTROLMULTIPARTYINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>

class MLtsyDispatchCallControlMultipartyConferenceHangUp : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlMultipartyConferenceHangUpApiId = KDispatchCallControlMultipartyFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileConferenceCallHangUp
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceHangUpComp()
	 *
	 * Implementation of this interface should terminate the active conference call.
	 *
	 * @param aCallIds Array of all Call IDs, which belong to the conference.
	 * This array will be destroyed after the function call returns.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileConferenceCall::HangUp()
	 */
	virtual TInt HandleConferenceHangUpReqL(const RArray<TInt>& aCallIds) = 0;

	}; // class MLtsyDispatchCallControlMultipartyConferenceHangUp



class MLtsyDispatchCallControlMultipartyConferenceAddCall : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlMultipartyConferenceAddCallApiId = KDispatchCallControlMultipartyFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileConferenceCallAddCall
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceAddCallComp()
	 *
	 * Implementation of this interface should add the specified call to the conference
	 * call.  Typically, there will be an ongoing conference call containing at least
	 * 2 remote parties.  The conference call gets swapped into the held state and
	 * either a new call is dialled or an incoming call is answered.  This call can
	 * then be added to the conference call using this API.
	 *
	 *
	 * @param aCallId The Call ID of the connected call to be added to the multiparty (conference) call.
	 * @param aExistingCallId The Call ID of one of the calls which is already a part of the conference call.
	 * This can be the Call ID of any of the existing calls in the conference, not necessarily the first one that was added.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileconferenceCall::AddCall()
	 */
	virtual TInt HandleConferenceAddCallReqL(TInt aCallId, TInt aExistingCallId) = 0;

	}; // class MLtsyDispatchCallControlMultipartyConferenceAddCall



class MLtsyDispatchCallControlMultipartyCreateConference : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlMultipartyCreateConferenceApiId = KDispatchCallControlMultipartyFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileConferenceCallCreateConference
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlMultipartyCreateConferenceComp()
	 *
	 * Implementation of this interface should join the two specified calls into a
	 * multiparty (conference) call.
	 *
	 *
	 * @param aHeldCallId The Call ID of the held call which will be one of the calls
	 * joined to the conference.
	 *
	 * @param aSecondCallId The Call ID of the active / alerting call which will be
	 * the other call joined to the conference.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileConferenceCall::CreateConference()
	 */
	virtual TInt HandleCreateConferenceReqL(TInt aHeldCallId, TInt aSecondCallId) = 0;

	}; // class MLtsyDispatchCallControlMultipartyCreateConference



class MLtsyDispatchCallControlMultipartyConferenceSwap : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlMultipartyConferenceSwapApiId = KDispatchCallControlMultipartyFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileConferenceCallSwap
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceSwapComp()
	 *
	 * Implementation of this interface should swap the ongoing conference call and
	 * the other call. If the conference call is on hold,
	 * it will become connected and the other call will be held.
	 *
	 * If it is connected, it will be put on hold and the other call will become
	 * connected.
	 *
	 * @param aHeldCallId The Call ID of the currently held call. If the conference
	 * call is on hold then this will be the call ID of one of the calls in the
	 * conference.
	 *
	 * @param aConnectedCallId The Call ID of the currently connected call.
	 * If the conference call is connected then this will be the call ID of one
	 * of the calls in the conference.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileConferenceCall::Swap()
	 */
	virtual TInt HandleConferenceSwapReqL(TInt aHeldCallId, TInt aConnectedCallId) = 0;

	}; // class MLtsyDispatchCallControlMultipartyConferenceSwap


class MLtsyDispatchCallControlMultipartyConferenceGoOneToOne : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId = KDispatchCallControlMultipartyFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileCallGoOneToOne
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCallControlMultipartyConferenceGoOneToOneComp()
	 *
	 * Implementation of this interface should setup a private communication with the specified call
	 * in the conference.  This call will then become connected when the call status notification
	 * arrives from the LTSY and the other call(s) will go on hold.
	 *
	 * If at the time of the go one-to-one request, there are two remote parties in the conference
	 * call, after the one to one is successful, the other call will go on hold and the conference
	 * will go idle (as there are not enough participants to be a valid conference anymore). If there
	 * are more than two remote parties in the conference call, then the conference call should go
	 * on hold.
	 *
	 * @param aCallId The Call ID of the call which the client wishes to set up a private communication with.
	 * This should be a call currently in the conference.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 *
	 * @see RMobileCall::GoOneToOne()
	 */
	virtual TInt HandleConferenceGoOneToOneReqL(TInt aCallId) = 0;

	}; // class MLtsyDispatchCallControlMultipartyConferenceGoOneToOne

#endif /*MLTSYDISPATCHCALLCONTROLMULTIPARTYINTERFACE_H_*/
