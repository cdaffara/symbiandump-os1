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
// MessageInterceptClient.cpp
// 
//

/**
 @file 
*/

#include "msginterceptclient.h"
#include <elements/sd_mintercept.h>
#include <elements/sd_minterceptmsgs.h>

using namespace Den;

EXPORT_C void RMessageInterceptor::Close()
	{
	RApiExtSessionBase::Close();
	iReqAndRespEventBuf.Close();
	}

// fallible messages control
EXPORT_C TInt RMessageInterceptorFM::Open(RExtendableSessionBase& aExtendableInterface)
	{
	TInt error =
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	iReqAndRespEventBuf.Create(TMIMessages::KMaxExternalisedReqAndRespMsgSize);
	if (KErrNone == error)
		{
		error = RApiExtSessionBase::Open(aExtendableInterface, AFallibleTestControl::KInterfaceId);
		}
#else
	KErrNotSupported;

	// Fixing unused local variable warnings.
	(void)aExtendableInterface;
#endif
	return error;
	}

EXPORT_C void RMessageInterceptorFM::ConfigureFallibleMessagesL(RArray<Messages::TNodeSignal::TMessageId>& aArray /*TODO: TAction& aAction = TDefaultAction?*/)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TInitFallibleMsgModeMsg msg;
	msg.InitL(aArray);
	TRequestStatus status;
	RApiExtSessionBase::SendReceive(msg, iReqAndRespEventBuf, status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
#else
	// Fixing unused local variable warnings.
	(void)aArray;

	User::Leave(KErrNotSupported);
#endif
	}

EXPORT_C TInt RMessageInterceptorFM::EnableFallibleMode(TInt aCount)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TEnableFallibleMsgModeMsg msg(aCount);
	TRequestStatus status;
	RApiExtSessionBase::SendReceive(msg, iReqAndRespEventBuf, status);
	User::WaitForRequest(status);
	return status.Int();
#else
	// Fixing unused local variable warnings.
	(void)aCount;

	return KErrNotSupported;
#endif
	}

EXPORT_C TBool RMessageInterceptorFM::CheckFinished()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE

	TCheckFallibleMsgModeMsg msg;
	TRequestStatus status;
	RApiExtSessionBase::SendReceive(msg, iReqAndRespEventBuf, status);
	User::WaitForRequest(status);
	if (KErrNone != status.Int())
		{
		return ETrue;
		}

	TBool checkResult;
	TCheckFallibleMsgModeResponseMsg* respmsg = NULL;
	TRAP_IGNORE(respmsg = static_cast<TCheckFallibleMsgModeResponseMsg*>(TApiExtRespMsg::NewL(iReqAndRespEventBuf)));
	if (respmsg)
		{
		checkResult = respmsg->iCheckResult;
		}
	delete respmsg;
	return checkResult;
#else
	return EFalse;
#endif
	}



// pattern control
EXPORT_C TInt RMessageInterceptorPM::Open(RExtendableSessionBase& /*aExtendableInterface*/)
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	return KErrNotSupported;
#else
	return KErrNotSupported;
#endif
	}

/*
EXPORT_C TInt RMessageInterceptorPM::Enable()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TEnableMsg msg;
	TRequestStatus status;
	RApiExtSessionBase::SendRequest(msg, iReqAndRespEventBuf, status);
	User::WaitForRequest(status);
	return status.Int();
#else
	return KErrNotSupported;
#endif
	}

EXPORT_C void RMessageInterceptorPM::Disable()
	{
#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	TDisableMsg msg;
	RApiExtSessionBase::SendMessage(msg);
#endif
	}
*/

