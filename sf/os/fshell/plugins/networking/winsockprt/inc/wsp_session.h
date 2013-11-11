// wsp_session.h
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

#ifndef __WSP_SESSION_H__
#define __WSP_SESSION_H__

#include "wsp_request.h"
#include "wsp_scheduler.h"

class CWin32SubSession;

class RWin32Session
	{
public:
	TInt Open();
protected:
	void Close();
	TInt MakeRequest(TWin32Message& aMessage);
private:
	TInt CreateThread();
private:
	friend class RWin32SubSession;
	RThread iThread;
	TWin32Request iRequest;
	TWin32Message iWin32Message;
	};


class CWin32Session : public CWin32ActiveObject
	{
public:
	enum
		{
		EClose = 0,
		ECloseSubSession
		};
public:
	void AddSubSessionL(CWin32SubSession* aSubSession);
protected:
	virtual void ServiceL(TWin32Message& aMessage) = 0;
protected:
	~CWin32Session();
	CWin32Session(TWin32Request& aRequest, CWin32Scheduler& aScheduler);
	void ConstructL();
protected: // From CWin32ActiveObject.
	virtual void Run();
private:
	void Close();
	void CloseSubSession();
private:
	TWin32Request& iRequest;
	RPointerArray<CWin32SubSession> iSubSessions;
	};


GLDEF_C TInt WinSockPrtThreadStart(TAny* aPtr);
GLDEF_C void InitialiseAndRunThreadL(TAny* aPtr);

#endif // __WSP_SESSION_H__
