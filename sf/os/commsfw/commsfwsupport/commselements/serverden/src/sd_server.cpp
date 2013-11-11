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
//

/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include "sd_log.h"
#include "sd_std.h"
#include "sd_roles.h"

using namespace Den;
using namespace CommsFW;

EXPORT_C CCommonServer::CCommonServer(TInt aPriority, CCommonWorkerThread* aOwnerThread, const TPolicy& aPolicy,TServerType aType, const TDesC& aName)
:	CPolicyServer(aPriority, aPolicy, aType), iOwnerThread(aOwnerThread),
	iServerName(aName)
	{
	__DECLARE_NAME(_S("CCommonServer"));
	}

EXPORT_C CCommonServer::~CCommonServer()
//
// Destructor.
//
	{
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonServer %08x:\t~CCommonServer()"), this));
	/* This part actually duplicated that of the CServer destructor, but we'd like a bit of logging
	first if we delete sessions, in contrast to CServer which does it silently */
	iSessionIter.SetToFirst();

#ifdef SYMBIAN_TRACE_ENABLE
	CSession2* pSession;
	while((pSession=iSessionIter++)!=NULL)
		{
		COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonServer %08x:\t~CCommonServer(): Session(%08x): Remaining. BAD."), this, pSession));
		}
#endif
	}

EXPORT_C TBool CCommonServer::CanShutdown()
	{
#ifdef SYMBIAN_TRACE_ENABLE
	SessionIterator().SetToFirst();
	CSession2* sess;
	while((sess = iSessionIter++) != NULL)
		{
		COMMONLOG((WorkerId(), KECommonBootingTag, _L8("CCommonDealer::CanShutdown() - session(%08x) remains, containing:"), sess));
		CWorkerSession* workerSess = static_cast<CWorkerSession*>(sess);

		workerSess->SubSessions().Lock();
		workerSess->ProcessSubSessions(TWorkerThreadPublicInfo::ENullWorkerId, CWorkerSession::LogSubSession, NULL);
		workerSess->SubSessions().Unlock();
		}
#endif
	return iNumSessions <= 0;
	}

EXPORT_C CSession2* CCommonServer::NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const
	{
	if(WorkerThread().PitBoss().IsShuttingDown() || WorkerThread().ShuttingDown())
		{
	    User::Leave(KErrServerBusy);
		}
	TBool r=User::QueryVersionSupported(CurrentVersion(), aVersion);
	if (r==EFalse)
		{
		User::Leave(KErrNotSupported);
		}

	/* Get PID and UID to check for eligibility id this is WorkerDealer and
	to store in new session. */
	RProcess process;
	RThread thread;
	User::LeaveIfError(aMessage.Client(thread));
	TInt err=thread.Process(process);
	thread.Close();
	User::LeaveIfError(err);
	TProcessId pid = process.Id();
	TUidType uid = process.Type();
	process.Close();

	// Only eligible clients can create sessions on workerdealers, so if workerthread has a workerdealer
	// make sure the client is eligible.
	if(!WorkerThread().IsMainThread() && (WorkerThread().WorkerDealer() && !WorkerThread().WorkerDealer()->IsEligible(pid)))
		{
		User::Leave(KErrPermissionDenied);
		}

	CSession2* session = DoNewSessionL(pid, uid);
	iNumSessions++;
	COMMONLOG((WorkerId(), KECommonBootingTag, _L8("CCommonServer %08x:\tNewSessionL() iNumSessions=%d"), this, iNumSessions));

	return session;
	}


