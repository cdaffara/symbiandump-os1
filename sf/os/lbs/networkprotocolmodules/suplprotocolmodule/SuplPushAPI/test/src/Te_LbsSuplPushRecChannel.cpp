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
// A unit test wrapper over the CLbsSuplPushRecChannel class
// 
//

#include <e32property.h>

#include "Te_LbsSuplPushRecChannel.h"
#include "Te_LbsSuplPushRProperty.h"

TBool CTe_LbsSuplPushRecChannel::iEnableAsserts = ETrue;

/**
A public wrapper over the protected CLbsSuplPushRecChannel::RunL.

@param aChannel [In] A reference on the object the method is called for.
@param aStatus [In] The status to "complete" the request with.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushRecChannel::RunL(CLbsSuplPushRecChannel& aChannel, TInt aStatus)
	{
	CTe_LbsSuplPushRecChannel& testChan = static_cast<CTe_LbsSuplPushRecChannel&>(aChannel);
	
	__ASSERT_ALWAYS(testChan.IsActive(), User::Invariant());
	testChan.Cancel();
	testChan.iStatus = aStatus;
	
	testChan.CLbsSuplPushRecChannel::RunL();
	}

/** Redefines the ASSERT_DEBUG macro to be able to turn ot onn/off at run-time. */
#undef __ASSERT_DEBUG
#define __ASSERT_DEBUG(c,p) (void)((c || !CTe_LbsSuplPushRecChannel::AssertsEnabled()) ||(p,0))

/** Replaces the RProperty class with its test wrapper - RTe_LbsSuplPushRProperty. */
#define RProperty RTe_LbsSuplPushRProperty

#include "lbssuplpushreceiverchannel.cpp"
