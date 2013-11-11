// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Internal implementation of the SUPL Push API
// 
//

/**
 @file
 @internalComponent
 @deprecated
*/

#ifndef SUPL_PUSH_IMPL_H
#define SUPL_PUSH_IMPL_H

#include <e32std.h>
#include <e32property.h>
#include <lbs/lbssuplpush.h>
#include "lbstimer.h"


/** 
The nested utility class used for storing the message informaton while message is stored in the queue.
*/
NONSHARABLE_CLASS(CLbsSuplPushMsgInfo) : public CBase
	{
public:
	CLbsSuplPushMsgInfo(TInt aReqId, HBufC8* aMsg);
	virtual ~CLbsSuplPushMsgInfo();
			
public:
	/** The id of the message delivery request*/
	TInt iReqId;
	/** The message data with the request id in the beginning*/
	HBufC8* iMsg;
	};
		

/**
The class provides an internal implementation of the functionality exposed 
by the CLbsSuplPush interface.

@see CLbsSuplPush
@see MLbsSuplPushObserver

@internalComponent
@deprecated
*/
NONSHARABLE_CLASS(CLbsSuplPushImpl) : public CActive, public MLbsCallbackTimerObserver
	{
public:
	/**
	The set of the states the class can be  
	
	@see CLbsSuplPushImpl::CheckStateL
	*/
	enum TState { 
		/** The initial state the object gets to after creation*/
		ECreated = 1,
		/** The state after the object got the P&S category UID and attached to the BUSY and ACK props
		 	In that state there may be from zero to several messages in the queue.
		 	The INIT property has never been set for any of the messages in the queue. 
		 	The BUSY property has not been set to ETrue by this object as well.*/
		EInitialized = 2,
		/** The state after the object has tried to deliver a message, has set successfully the INIT property,
		 * subscribed for the ACK property and is waiting for the ACK property to be set.*/
		EWaitingAck = 4,
		/** The state after the object checked that the BUSY property is set to ETrue by another plug-in, 
		    subscribed for the BUSY property and is waiting for the BUSY property to be set to EFalse. */
		EWaitingBusy = 8
	};		
		
public:
	static CLbsSuplPushImpl* NewL(TLbsSuplPushChannel aChannel, MLbsSuplPushObserver& aObserver);
	virtual ~CLbsSuplPushImpl();
	
	virtual void OnTimerEventL(TInt aTimerId);
	virtual TInt OnTimerError(TInt aTimerId, TInt aError);

public:
	TInt SuplInit(TLbsSuplPushRequestId& aReqId, const TDesC8& aMsg, TInt aReserved);
	
#ifdef TE_LBSSUPLPUSHIMPL_ON
	/**
	Checks that the object in one of the possible states and check that the internal state of the object
	is in the agreement with the state it is currently in. 
	
	This function is defined in and used only by the 
	unit test code for this component. When building the SUPL Push API component itself (either UDEB or UREL) 
	the TE_LBSSUPLPUSHIMPL_ON macro is undefined.
	
	@param aPossibleStates  [In] The bitmask of the possible states.
	
	@leave If the object is not in one of the possible states or the internal state of the object
	is not in the agreement with the state it is currently in, this method will leave with error 
	code KErrGeneral.

	@see CLbsSuplPushImpl::TState
	*/
	void CheckStateL(TInt aPossibleStates);
#endif 
	
protected:
	virtual void RunL();
	virtual void DoCancel();
	
private:	
#ifdef TE_LBSSUPLPUSHIMPL_ON
protected:
#endif	
	
	CLbsSuplPushImpl(TLbsSuplPushChannel aChannel, MLbsSuplPushObserver& aObserver);
private:	
	void ConstructL(TLbsSuplPushChannel aChannel);
	
	void SuplInitL(TLbsSuplPushRequestId& aReqId, const TDesC8& aMsg);
	
	TInt ReleaseBusyProp();
	TInt GetBusyData(TBool& aBusy, TInt& aNextReqId) const;
	TInt SetBusyData(TBool aBusy, TInt aNextReqId) const;
	
	TInt SendMessage(TBool aNotifyObserver);
	
private:
	/**The id of the channel used*/
	TLbsSuplPushChannel iChannel;

	/** The state of the CLbsSuplPushImpl object*/
	TState iState;

	/** A reference to a user of the CLbsSuplPushImpl interface 
	waiting for request completion call-backs. */
	MLbsSuplPushObserver& iObserver;
	
	/** The BUSY property key */
	TUint iBusyPropKey;
	
	/** The INIT property key */
	TUint iInitPropKey;
	
	/** The ACK property key */
	TUint iAckPropKey;
	
	/** A BUSY property handle */
	RProperty iBusyProperty;
	
	/** An ACK property handle */
	RProperty iAckProperty;
		
	/** The security id of the process that defines the P&S props used*/
	TUid iPropOwnerSecureId;
		
	/** The message queue */
	RPointerArray<CLbsSuplPushMsgInfo> iMsgQueue;
	
	/** The message timeout timer */
	CLbsCallbackTimer* iTimer;
	};

#endif //SUPL_PUSH_IMPL_H
