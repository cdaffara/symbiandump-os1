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

#include <ss_std.h>
#include <ss_glob.h>
#include <comms-infras/ss_log.h>
//#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_roles.h>
#include <es_ini.h>
#include <f32file.h>
#include <commdb.h>
#include <ss_subconnprov.h>
#include <ss_protprov.h>
#include <ss_connprov.h>
#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_subconnflow.h>
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbufpondop.h>
#else
#include <es_mbman.h>
#endif // SYMBIAN_ZERO_COPY_NETWORKING


using namespace Den;

void SocketServer::InitL(CWorkerThread* aWorker)
	{
	if(aWorker->WorkerId() == TWorkerThreadPublicInfo::EMainThread)
		{
		// ESock Startup
		// Check for the existence of ESock sub-directories

		TAutoClose<RFs> fs;
		User::LeaveIfError(fs.iObj.Connect());
		fs.PushL();

		// Create Private Path for ESock in system drive
		TInt retCode = 0;
		if(fs.iObj.CreatePrivatePath(RFs::GetSystemDrive()) != KErrNone)
			{
			// Error creating Private Path
			User::Leave(retCode);
			}

		// Create ESock Directories in the system drive if they're not already there..
		// We rely on undocumented behavior that the default path is on the system drive
		retCode = fs.iObj.MkDirAll(KEsockIniFileDir);
		if(retCode != KErrNone && retCode != KErrAlreadyExists)
			{
			// Error creating \esock Directory
			User::Leave(retCode);
			}

		retCode = fs.iObj.MkDirAll(KEsockNoBackupDir);
		if(retCode != KErrNone && retCode != KErrAlreadyExists)
			{
			// Error creating \nobackup directory
			User::Leave(retCode);
			}

		fs.Pop();
		}

	LOG( ESockLog::Printf(_L("SocketServer::InitL() Done!")); )
	}

void SocketServer::ShutDown()
/**
free data structures and close the protocol manager
*/
	{
	// Mark ourself as on the way out
	CSockManData* globals=SockManGlobals::Get();
	LOG( ESockLog::Printf(_L("--------------- SocketServer W%d starting shutdown ---------------"), globals->iWorkerThread->WorkerId()) );

	// The sub-connection and connection (specially) factories needs to be deleted
	// before ShutdownExtensions() is called since connection factories may hold a
	// valid NifSession ref which may become invalid if Nifman is unloaded through
	// ShutdownExtensions()
	//destroy factories before anything else gets destroyed
	globals->UninstallFactoryContainers();

	ShutdownExtensions(); // including Nifman

	ProtocolManager::ShutDown();

	__CFLOG_STMT(TInt id = globals->iWorkerThread->WorkerId());

	__CFLOG_1(KESockComponentTag, KESockServerTag, _L8("SocketServer W%d: objects destroyed, waiting to drop Scheduler"), id);
	}

void SocketServer::ShutdownExtensions()
/**
Shutdown extention DLLs
*/
	{
	TDblQue<CSocketServExtRef>& list = SockManGlobals::Get()->iExtensions;
	CSocketServExtRef *xref;
	TDblQueIter<CSocketServExtRef> iter(list);
	iter.SetToLast();
	while (xref = iter--, xref!=NULL)
		{
		xref->Remove();
		}

	while (!list.IsEmpty())
		{
		xref = list.Last();
		xref->Close();
		delete xref;
		}
	}

TBool SocketServer::IsShuttingDown()
/**
Used for Protocol shutdown to tell us whether protocols must be closed
immediately.
*/
	{
	return (SockManGlobals::Get()->SelfWorker()->ShuttingDown());
	}
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
RCommsBufPond SocketServer::GetCommsBufPond()
/**
return context for the Mbuf manager.
*/
	{
	return TCommsBufPondTLSOp::Get();
	}
#else
CMBufManager* SocketServer::GetMBufManager()
/**
return context for the Mbuf manager.
*/
  	{
    return CMBufManager::Context();
    }
#endif

CDeltaTimer* SocketServer::GetTimer()
/**
return context for the global timer.
*/
	{
	// PERF: doesn't appear to be used by us now? switched to just-in-time addition
	CDeltaTimer* timer = SockManGlobals::Get()->iTimer;
	if(!timer->IsAdded())
		{
		LOG( ESockLog::Printf(_L8("SocketServer::GetTimer() - adding upon first use")); )
		CActiveScheduler::Add(timer);
		}
	return timer;
	}

CSocketServer* SocketServer::GetSocketServer()
/**
return context for the CServer

*/
	{
	return SockManGlobals::Get()->iSocketServer;
	}

#if defined(_DEBUG_SOCKET_FUNCTIONS)
EXPORT_C void SocketServer::__DbgForceLoadMBufManagerL()
/**
 Force loading the MBuf manager and, just in case it gets lonely, make a timer.
 DEPRECATED
*/
	{
	}

EXPORT_C void SocketServer::__DbgForceKillMBufManager()
/**
 Force unloading the MBuf manager
 DEPRECATED
*/
	{
	}
#else
EXPORT_C void SocketServer::__DbgForceLoadMBufManagerL()
/**
do nothing for UREL builds
*/
	{
	}

EXPORT_C void SocketServer::__DbgForceKillMBufManager()
/**
do nothing for UREL builds
*/
	{
	}

#endif

void SocketServer::InstallExtensionL(const TDesC& aDllName, const TDesC& aArgs)
/**
Installs an Esock extension dll

*/
	{
	CSocketServExtRef* xr = new (ELeave) CSocketServExtRef;
	CleanupStack::PushL(xr);
	xr->InstallL(aDllName, aArgs);
	CSockManData* globals = SockManGlobals::Get();
	globals->iExtensions.AddLast(*xr);	//lint -e429	// extension list does have ownership
	CleanupStack::Pop();
	}

void SocketServer::NewSession()
/**
Increment the session counter - if we were about to shutdown, stop.
*/
	{
	LOG( ESockLog::Printf(_L8("SocketServer::NewSession()")) );
	}

void SocketServer::SessionClosing()
/**
A session is closing, if it's the last one take our stumps home.
*/
	{
	LOG( ESockLog::Printf(_L8("SocketServer::SessionClosing()")) );
	}
