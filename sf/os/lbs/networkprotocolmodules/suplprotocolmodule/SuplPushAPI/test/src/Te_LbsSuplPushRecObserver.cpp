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

#include "Te_LbsSuplPushRecObserver.h"


TLbsSuplPushRecObserver::TLbsSuplPushRecObserver() : iHasNewMessage(EFalse)
	{
	//Intentionally left blank
	}

/**
Receives notification about an incoming SUPL INIT message. Overrides the pure virtual 
MLbsSuplPushRecObserver::OnSuplInit. Stores the info about the message received.
	
@param aChannel  [In] The channel the call-back is related to.
@param aReqId    [In] An Id of the request the call-back is related to.
@param aMsg      [In] A buffer containing a SUPL INIT message.

@see MLbsSuplPushRecObserver::OnSuplInit	
*/	
void TLbsSuplPushRecObserver::OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId,
		TDesC8& aMsg)
	{
	iHasNewMessage = ETrue;
	iChannel = aChannel;
	iReqId = aReqId;
	iMessage = aMsg;
	}

/**
A public test wrapper over the protected MLbsSuplPushRecObserver::ExtendedInterface.

@see MLbsSuplPushRecObserver::ExtendedInterface
*/
TAny* TLbsSuplPushRecObserver::ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2)
	{
	return MLbsSuplPushRecObserver::ExtendedInterface(aFunctionNumber,aPtr1,aPtr2);
	}

