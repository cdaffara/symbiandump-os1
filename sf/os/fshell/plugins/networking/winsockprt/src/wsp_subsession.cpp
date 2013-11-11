// wsp_subsession.cpp
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


#include "wsp_subsession.h"
#include "wsp_session.h"
#include "wsp_panic.h"


//
// RWin32SubSession.
//

void RWin32SubSession::Close()
	{
	if (iSession->iThread.Handle())
		{
		TPckgC<TInt> handlePckg(iHandle);
		iWin32Message.Set(CWin32Session::ECloseSubSession, handlePckg);
		iSession->iRequest.MakeRequest(iWin32Message);
		}
	}

TInt RWin32SubSession::CreateSubSession(RWin32Session& aSession, TInt aOppCode)
	{
	TPckg<TInt> handleBuf(iHandle);
	iWin32Message.Set(aOppCode, handleBuf);
	TInt err = aSession.MakeRequest(iWin32Message);
	if (err == KErrNone)
		{
		iSession = &aSession;
		}
	return err;
	}

TInt RWin32SubSession::CreateSubSession(RWin32Session& aSession, TInt aOppCode, const TDesC8& aReadBuf)
	{
	TPckg<TInt> handleBuf(iHandle);
	iWin32Message.Set(aOppCode, aReadBuf, handleBuf);
	TInt err = aSession.MakeRequest(iWin32Message);
	if (err == KErrNone)
		{
		iSession = &aSession;
		}
	return err;
	}


TInt RWin32SubSession::MakeRequest(TWin32Message& aMessage) const
	{
	return iSession->iRequest.MakeRequest(aMessage, iHandle);
	}


//
// CWin32SubSession.
//

CWin32SubSession::CWin32SubSession(CWin32Scheduler& aScheduler)
	: CWin32ActiveObject(aScheduler)
	{
	}

void CWin32SubSession::Complete(TWin32Message*& aMessage, TInt aReason) const
	{
	__ASSERT_DEBUG(aMessage, Panic(EWinSockPrtSubSessionInvalidMessage));
	aMessage->Complete(aReason);
	aMessage = NULL;
	}
