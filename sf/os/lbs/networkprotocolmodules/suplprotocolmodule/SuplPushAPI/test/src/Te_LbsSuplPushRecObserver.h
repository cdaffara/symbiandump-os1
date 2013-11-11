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
// A unit test observer class implementing MLbsSuplPushRecObserver interface
// 
//

#if (!defined __TE_LBSSUPLPUSHRECOBSERVER__)
#define __TE_LBSSUPLPUSHRECOBSERVER__

#include <lbs/lbssuplpushreceiver.h>

/**
A unit test observer class implementing MLbsSuplPushRecObserver interface

@see MLbsSuplPushRecObserver
*/
class TLbsSuplPushRecObserver : public MLbsSuplPushRecObserver
	{
public:	
	TLbsSuplPushRecObserver();

	virtual void OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId,
			TDesC8& aMsg);						
	virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	
	inline TBool HasNewMessage();
	inline TLbsSuplPushChannel Channel();
	inline TLbsSuplPushRequestId ReqId();
	inline TDesC8& Message();
	
	inline void Reset();
	
private:
	/** The flag if a new message received */
	TBool iHasNewMessage;
	/** The channel the message received by*/
	TLbsSuplPushChannel iChannel;
	/** The internal id of the message request*/
	TLbsSuplPushRequestId iReqId;
	/** The message received */
	TBufC8<64> iMessage;
	};
	
inline TBool TLbsSuplPushRecObserver::HasNewMessage()
	{
	return iHasNewMessage;
	}

inline TLbsSuplPushChannel TLbsSuplPushRecObserver::Channel()
	{
	return iChannel;
	}

inline TLbsSuplPushRequestId TLbsSuplPushRecObserver::ReqId()
	{
	return iReqId;
	}

inline TDesC8& TLbsSuplPushRecObserver::Message()
	{
	return iMessage;
	}

inline void TLbsSuplPushRecObserver::Reset()
	{
	iHasNewMessage = EFalse;
	}

#endif //__TE_LBSSUPLPUSHRECOBSERVER__
