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
// Internal implementation of the SUPL Push API receiver channel
// 
//

/**
 @file
 @internalComponent
 @deprecated
*/
#ifndef SUPL_PUSH_RECEIVER_CHANNEL_H
#define SUPL_PUSH_RECEIVER_CHANNEL_H

#include <e32std.h>
#include <e32property.h>

#include <lbs/lbssuplpushcommon.h>

class MLbsSuplPushRecObserver;

/**
The class provides an implementation of the SUPL Push API SMS or WAP Push channel.

@internalComponent
@deprecated

@see CLbsSuplPushRecImpl
@see MLbsSuplPushRecObserver
*/
NONSHARABLE_CLASS(CLbsSuplPushRecChannel) : public CActive
	{
public:
	static CLbsSuplPushRecChannel* NewL(TLbsSuplPushChannel aChannel, MLbsSuplPushRecObserver& aObserver,
			TUid aPropOwnerSecureId);
	virtual ~CLbsSuplPushRecChannel();
	
protected:
	virtual void RunL();
	virtual void DoCancel();
	
private:
	
#ifdef TE_LBSSUPLPUSHIMPL_ON
protected:
#endif	

	CLbsSuplPushRecChannel(TLbsSuplPushChannel aChannel, MLbsSuplPushRecObserver& aObserver, 
			TUid aPropOwnerSecureId);
	void ConstructL();
	
private:
	/** A reference to a user of the CLbsSuplPushRec interface 
	waiting for new message notification call-backs. */
	MLbsSuplPushRecObserver& iObserver;
	
	/** An INIT property handle */
	RProperty iInitProperty;
	
	/** The type of the channel */
	TLbsSuplPushChannel iChannel;
	
	/** The INIT property key */
	TUint iInitPropKey;
	
	/** The ACK property key */
	TUint iAckPropKey;
	
	/** The security id of the process that defines the P&S props used*/
	TUid iPropOwnerSecureId;
	};

#endif //SUPL_PUSH_RECEIVER_CHANNEL_H
