// wsp_factory.cpp
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


#include "wsp_factory.h"
#include "wsp_resolver.h"
#include "wsp_socket.h"
#include "wsp_panic.h"
#include "wsp_log.h"
#include <emulator.h>


//
// RWin32Factory.
//

TInt RWin32Factory::Open()
	{
	return RWin32Session::Open();
	}

void RWin32Factory::Close()
	{
	RWin32Session::Close();
	}


//
// CWin32Factory.
//

CWin32Factory* CWin32Factory::NewL(TWin32Request& aRequest, CWin32Scheduler& aScheduler)
	{
	CWin32Factory* self = new(ELeave) CWin32Factory(aRequest, aScheduler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWin32Factory::~CWin32Factory()
	{
	WSACleanup();
	}

void CWin32Factory::ServiceL(TWin32Message& aMessage)
	{
	CWin32SubSession* subSession = NULL;

	switch (aMessage.OppCode())
		{
		case ENewSocket:
			{
			WSP_LOG(WspLog::Write(_L("CWin32Factory::ServiceL: ENewSocket")));
			RWin32Socket::TType type;
			TPckg<RWin32Socket::TType> typePckg(type);
			typePckg.Copy(aMessage.ReadBuffer());
			subSession = CWin32Socket::NewL(iScheduler, type);
			break;
			}
		case ENewResolver:
			{
			WSP_LOG(WspLog::Write(_L("CWin32Factory::ServiceL: ENewResolver")));
			subSession = CWin32Resolver::NewL(iScheduler);
			break;
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtInvalidFactoryOppCode));
			}
		}

	AddSubSessionL(subSession);
	}

CWin32Factory::CWin32Factory(TWin32Request& aRequest, CWin32Scheduler& aScheduler)
	: CWin32Session(aRequest, aScheduler)
	{
	}

void CWin32Factory::ConstructL()
	{
	InitWinSockL();
	CWin32Session::ConstructL();
	}

void CWin32Factory::InitWinSockL()
	{
	WORD version = MAKEWORD (2,2);
	WSADATA wsaData;

	Emulator::Lock();
	int res = WSAStartup(version, &wsaData);
	Emulator::Unlock();
	if (res == SOCKET_ERROR)
		{
		User::Leave(KErrGeneral);
		}

	if (version != wsaData.wVersion)
		{
		User::Leave(KErrNotSupported);
		}
	}
