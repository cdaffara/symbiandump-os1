// wsp_request.cpp
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#include "_windows.h"
#include "wsp_request.h"
#include "wsp_panic.h"
#include "wsp_log.h"


//
// TWin32Message.
//

void TWin32Message::Set(TInt aOppCode)
	{
	iOppCode = aOppCode;
	iReadBuf = NULL;
	iWriteBuf = NULL;
	iCompletionStatus = NULL;
	iParentThread.SetHandle(0);
	}

void TWin32Message::Set(TInt aOppCode, TRequestStatus& aCompletionStatus)
	{
	iOppCode = aOppCode;
	iReadBuf = NULL;
	iWriteBuf = NULL;
	iCompletionStatus = &aCompletionStatus;
	aCompletionStatus = KRequestPending;
	iParentThread.SetHandle(0);
	}

void TWin32Message::Set(TInt aOppCode, const TDesC8& aReadBuf)
	{
	iOppCode = aOppCode;
	iReadBuf = &aReadBuf;
	iWriteBuf = NULL;
	iCompletionStatus = NULL;
	iParentThread.SetHandle(0);
	}

void TWin32Message::Set(TInt aOppCode, const TDesC8& aReadBuf, TRequestStatus& aCompletionStatus)
	{
	iOppCode = aOppCode;
	iReadBuf = &aReadBuf;
	iWriteBuf = NULL;
	iCompletionStatus = &aCompletionStatus;
	aCompletionStatus = KRequestPending;
	iParentThread.SetHandle(0);
	}

void TWin32Message::Set(TInt aOppCode, TDes8& aWriteBuf)
	{
	iOppCode = aOppCode;
	iReadBuf = NULL;
	iWriteBuf = &aWriteBuf;
	iCompletionStatus = NULL;
	iParentThread.SetHandle(0);
	}

void TWin32Message::Set(TInt aOppCode, TDes8& aWriteBuf, TRequestStatus& aCompletionStatus)
	{
	iOppCode = aOppCode;
	iReadBuf = NULL;
	iWriteBuf = &aWriteBuf;
	iCompletionStatus = &aCompletionStatus;
	aCompletionStatus = KRequestPending;
	iParentThread.SetHandle(0);
	}

void TWin32Message::Set(TInt aOppCode, const TDesC8& aReadBuf, TDes8& aWriteBuf)
	{
	iOppCode = aOppCode;
	iReadBuf = &aReadBuf;
	iWriteBuf = &aWriteBuf;
	iCompletionStatus = NULL;
	iParentThread.SetHandle(0);
	}

void TWin32Message::Set(TInt aOppCode, const TDesC8& aReadBuf, TDes8& aWriteBuf, TRequestStatus& aCompletionStatus)
	{
	iOppCode = aOppCode;
	iReadBuf = &aReadBuf;
	iWriteBuf = &aWriteBuf;
	iCompletionStatus = &aCompletionStatus;
	aCompletionStatus = KRequestPending;
	iParentThread.SetHandle(0);
	}

TInt TWin32Message::OppCode() const
	{
	return iOppCode;
	}

const TDesC8& TWin32Message::ReadBuffer() const
	{
	__ASSERT_DEBUG(iReadBuf, Panic(EWinSockPrtInvalidMessageReadBuffer));
	return *iReadBuf;
	}

TDes8& TWin32Message::WriteBuffer()
	{
	__ASSERT_DEBUG(iWriteBuf, Panic(EWinSockPrtInvalidMessageWriteBuffer));
	return *iWriteBuf;
	}

void TWin32Message::Complete(TInt aReason)
	{
	__ASSERT_DEBUG(iCompletionStatus, Panic(EWinSockPrtInvalidCompletionStatus));
	iParentThread.RequestComplete(iCompletionStatus, aReason);
	}

void TWin32Message::SetParentThreadHandle(RThread aParentThread)
	{
	__ASSERT_DEBUG(iParentThread.Handle() == 0, Panic(EWinSockPrtInvalidMessageParentThreadHandle));
	iParentThread = aParentThread;
	}


//
// TWin32Request.
//

TWin32Request::TWin32Request()
	: iEvent(0), iMessage(NULL), iSubSession(0)
	{
	}

TInt TWin32Request::DuplicateParentThreadHandle(RThread aChildThread)
	{
	return iParentThread.Duplicate(aChildThread);
	}

TInt TWin32Request::MakeRequest(TWin32Message& aMessage)
	{
	WSP_LOG(WspLog::Printf(_L("TWin32Request::MakeRequest: aMessage: 0x%x"), &aMessage));
	__ASSERT_DEBUG(iMessage == NULL, Panic(EWinSockPrtMultipleRequests));
	iMessage = &aMessage;
	iMessage->SetParentThreadHandle(iParentThread);
	iSubSession = 0;
	iRequestStatus = KRequestPending;
	return MakeRequestAndWait();
	}

TInt TWin32Request::MakeRequest(TWin32Message& aMessage, TInt aSubSession)
	{
	WSP_LOG(WspLog::Printf(_L("TWin32Request::MakeRequest: aMessage: 0x%x, aSubSession: 0x%x"), &aMessage, aSubSession));
	__ASSERT_DEBUG(iMessage == NULL, Panic(EWinSockPrtMultipleRequests));
	iMessage = &aMessage;
	iMessage->SetParentThreadHandle(iParentThread);
	iSubSession = aSubSession;
	iRequestStatus = KRequestPending;
	return MakeRequestAndWait();
	}

void TWin32Request::SetEventHandle(WIN32_HANDLE aEvent)
	{
	iEvent = aEvent;
	}

void TWin32Request::Requested(TInt aReason)
	{
	WSP_LOG(WspLog::Printf(_L("TWin32Request::Requested: aReason: %d, iMessage: 0x%x, iSubSession: 0x%x"), aReason, iMessage, iSubSession));
	TRequestStatus* requestStatusPtr = &iRequestStatus;
	iParentThread.RequestComplete(requestStatusPtr, aReason);
	}

TInt TWin32Request::SubSession() const
	{
	return iSubSession;
	}

TWin32Message& TWin32Request::Message()
	{
	__ASSERT_DEBUG(iMessage, Panic(EWinSockPrtInvalidMessage));
	return *iMessage;
	}

TInt TWin32Request::MakeRequestAndWait()
	{
	if (!SetEvent(iEvent))
		{
		return KErrBadHandle;
		}
	User::WaitForRequest(iRequestStatus);
	iMessage = NULL;
	return iRequestStatus.Int();
	}
