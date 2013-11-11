// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <es_sock.h>
#include <es_panic.h>
#include "ss_connectionserver.h"
#include <comms-infras/es_connectionservermessages.h>
#include "ss_connectionsession.h"
#include <ss_pman.h>
#include <es_ver.h>
#include <commschan.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_roles.h>
#include "ss_secpol.h"

using namespace ESock;
using namespace CommsFW;



/*static*/ CConnectionServer* CConnectionServer::NewL(CWorkerThread* aOwnerThread)
//
// Create a new CConnectionServer.
//
	{
	CConnectionServer* server = new (ELeave) CConnectionServer(aOwnerThread, EActiveIpcPriority);
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}



void CConnectionServer::ConstructL()
	{
	}

CConnectionServer::CConnectionServer(CWorkerThread* aOwnerThread, TInt aPriority)
/**
 Constructor.
*/
	: CCommonServer(aPriority, aOwnerThread, ConnectionServerPolicy, ESharableSessions, KConnectionServerName)
	{
	__DECLARE_NAME(_S("CConnectionServer"));
	}

CConnectionServer::~CConnectionServer()
//
// Destructor.
//
	{
	}

TVersion CConnectionServer::CurrentVersion() const
	{
	return TVersion(KES32MajorVersionNumber,KES32MinorVersionNumber,KES32BuildVersionNumber);
	}

CSession2* CConnectionServer::DoNewSessionL(const TProcessId& aProcessId, const TUidType& aProcessUid) const
/*
 Create a new client on this server.
*/
	{
		return CConnectionSession::NewL(aProcessId, aProcessUid, WorkerThread().PitBoss().NextSessionUniqueId());
	}




