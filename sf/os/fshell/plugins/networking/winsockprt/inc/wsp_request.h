// wsp_request.h
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

#ifndef __WSP_REQUEST_H__
#define __WSP_REQUEST_H__

#include <e32std.h>
#include <e32base.h>
#include "wsp_def.h"


class TWin32Message
	{
public:
	void Set(TInt aOppCode);
	void Set(TInt aOppCode, TRequestStatus& aCompletionStatus);
	void Set(TInt aOppCode, const TDesC8& aReadBuf);
	void Set(TInt aOppCode, const TDesC8& aReadBuf, TRequestStatus& aCompletionStatus);
	void Set(TInt aOppCode, TDes8& aWriteBuf);
	void Set(TInt aOppCode, TDes8& aWriteBuf, TRequestStatus& aCompletionStatus);
	void Set(TInt aOppCode, const TDesC8& aReadBuf, TDes8& aWriteBuf);
	void Set(TInt aOppCode, const TDesC8& aReadBuf, TDes8& aWriteBuf, TRequestStatus& aCompletionStatus);
	TInt OppCode() const;
	const TDesC8& ReadBuffer() const;
	TDes8& WriteBuffer();
	void Complete(TInt aReason);
private:
	friend class TWin32Request;
	void SetParentThreadHandle(RThread aParentThread);
private:
	TInt iOppCode;
	const TDesC8* iReadBuf;
	TDes8* iWriteBuf;
	TRequestStatus* iCompletionStatus;
	RThread iParentThread;
	};


class TWin32Request
	{
public: // Methods used by the ESock thread.
	TWin32Request();
	TInt DuplicateParentThreadHandle(RThread aChildThread);
	TInt MakeRequest(TWin32Message& aMessage);
	TInt MakeRequest(TWin32Message& aMessage, TInt aSubSession);
public:	// Methods used by the WinSock thread.
	void SetEventHandle(WIN32_HANDLE aEvent);
	void Requested(TInt aReason);
	TInt SubSession() const;
	TWin32Message& Message();
private:
	TInt MakeRequestAndWait();
private:
	RThread iParentThread;
	WIN32_HANDLE iEvent;
	TRequestStatus iRequestStatus;
	TWin32Message* iMessage;
	TInt iSubSession;
	};

#endif // __WSP_REQUEST_H__
