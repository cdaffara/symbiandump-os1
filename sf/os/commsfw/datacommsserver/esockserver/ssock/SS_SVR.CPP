// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <ss_pman.h>
#include <es_ver.h>
#include <commschan.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_roles.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_internal.h>
#endif
#include <comms-infras/sockmes.h>
#include "ss_secpol.h"

using namespace CommsFW;

//_LIT(KSocketServerName, "!Socket-Server");

/*static*/ CSocketServer* CSocketServer::NewL(CWorkerThread* aOwnerThread)
//
// Create a new CSocketServer.
//
	{
	CSocketServer* self = new (ELeave) CSocketServer(aOwnerThread, EActiveIpcPriority);
	return self;
	}

CSocketServer::CSocketServer(CWorkerThread* aOwnerThread, TInt aPriority)
/**
 Constructor.
*/
:	CCommonServer(aPriority, aOwnerThread, ESock::SocketServerPolicy, ESharableSessions, SOCKET_SERVER_NAME)
	{
	}

CSocketServer::~CSocketServer()
//
// Destructor.
//
	{
	}
	
CSession2* CSocketServer::DoNewSessionL(const TProcessId& aProcessId, const TUidType& aProcessUid) const
/*
 Create a new client on this server.
*/
	{
#ifdef _DEBUG
	if(!iPostBootChecksRun)
		{
		iPostBootChecksRun = ETrue;
		if(WorkerThread().Player())
			{
			static_cast<CPlayer*>(WorkerThread().Player())->RunPostBootChecks();
			}
		}
#endif
	return CSockSession::NewL(aProcessId, aProcessUid, WorkerThread().PitBoss().NextSessionUniqueId());
	}

TVersion CSocketServer::CurrentVersion() const
/*
 Create a new client on this server.
*/
	{
	return TVersion(KES32MajorVersionNumber,KES32MinorVersionNumber,KES32BuildVersionNumber);
	}

CSocketScheduler* CSocketScheduler::New()
/**
 Create and install the active scheduler.
*/
	{
	
	CSocketScheduler *pA=new CSocketScheduler;
	__ASSERT_ALWAYS(pA!=NULL,Fault(EMainSchedulerError));
	CSocketScheduler::Install(pA);
	return pA;
	}

