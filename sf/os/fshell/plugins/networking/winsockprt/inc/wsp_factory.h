// wsp_factory.h
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

#ifndef __WSP_FACTORY_H__
#define __WSP_FACTORY_H__

#include "wsp_session.h"
#include "wsp_subsession.h"


class RWin32Factory : public RWin32Session
	{
public:
	enum TSubSessionType
		{
		ESocket,
		EResolver
		};
public:
	TInt Open();
	void Close();
	};


class CWin32Factory : public CWin32Session
	{
public:
	enum TOppCode
		{
		ENewSocket = 0x100,	// CWin32Session owns from 0 to 0x100.
		ENewResolver
		};
public:
	static CWin32Factory* NewL(TWin32Request& aRequest, CWin32Scheduler& aScheduler);
	~CWin32Factory();
protected: // From CWin32Session.
	virtual void ServiceL(TWin32Message& aMessage);
private:
	CWin32Factory(TWin32Request& aRequest, CWin32Scheduler& aScheduler);
	void ConstructL();
	void InitWinSockL();
	};


#endif // __WSP_FACTORY_H__
