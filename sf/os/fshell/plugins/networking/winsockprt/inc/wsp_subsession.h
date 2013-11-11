// wsp_subsession.h
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

#ifndef __WSP_SUBSESSION_H__
#define __WSP_SUBSESSION_H__

#include <e32std.h>
#include <e32base.h>
#include "wsp_active.h"
#include "wsp_session.h"


class TWin32Request;
class CWin32Scheduler;


class RWin32SubSession
	{
protected:
	void Close();
	TInt CreateSubSession(RWin32Session& aSession, TInt aOppCode);
	TInt CreateSubSession(RWin32Session& aSession, TInt aOppCode, const TDesC8& aReadBuf);
	TInt MakeRequest(TWin32Message& aMessage) const;
protected:
	mutable TWin32Message iWin32Message;
	mutable RWin32Session* iSession;
	TInt iHandle;
	};


class CWin32SubSession : public CWin32ActiveObject
	{
public:
	virtual void ServiceL(TWin32Message& aMessage) = 0;
protected:
	CWin32SubSession(CWin32Scheduler& aScheduler);
	void Complete(TWin32Message*& aMessage, TInt aReason) const;
	};


#endif // __WSP_SUBSESSION_H__
