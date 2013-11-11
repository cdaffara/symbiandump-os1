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

#ifndef SUPL_PUSH_RECEIVER_IMPL_H
#define SUPL_PUSH_RECEIVER_IMPL_H



#include <e32std.h>
#include <e32property.h>

#include <lbs/lbssuplpushreceiver.h>
#include <lbs/lbssuplpushcommon.h>


class CLbsSuplPushRecChannel;

/**
The class provides an internal implementation of the functionality exposed 
by the CLbsSuplPushRec interface.

@see CLbsSuplPushRec
@see MLbsSuplPushRecObserver

@internalComponent
@deprecated
*/
NONSHARABLE_CLASS(CLbsSuplPushRecImpl) : public CBase
	{
public:
	static CLbsSuplPushRecImpl* NewL(MLbsSuplPushRecObserver& aObserver);
	virtual ~CLbsSuplPushRecImpl();

public:
	TInt SuplInitComplete(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId, TInt aError, TInt aReserved);
	
	
private:
	void ConstructL(MLbsSuplPushRecObserver& aObserver);
	
private:
	/* The SMS channel listening for SMS messages*/
	CLbsSuplPushRecChannel* iSmsChannel;
	
	/* The WAP Push channel listening for WAP Push messages*/
	CLbsSuplPushRecChannel* iWapPushChannel;
	
	};

#endif //SUPL_PUSH_RECEIVER_IMPL_H
