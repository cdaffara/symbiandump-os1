// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements a Symbian OS configuration server.
// 
//

/**
 @file
 @internalComponent
*/


#include <e32svr.h>
#include <e32base.h>
#include <e32std.h>
#include "Config_Std.h"
#include "ConfigServer.h"
#include "ConfigSess.h"


LOCAL_C void MainL()
/**
 * Dll main. Do the funky stuff, and use Rendezvous to signal completion.
 *
 * @internalComponent
 * 
 * @leave Leaves from CConfigServer::NewL.
 */
	{
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("----------------Config Daemon Started--------------------"));
	
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif

	User::LeaveIfError(RProcess::RenameMe(KConfigServerName));

	CActiveScheduler* pScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(pScheduler);
	CleanupStack::PushL(pScheduler);
	CConfigServer* pServer = CConfigServer::NewL();
	CleanupStack::PushL(pServer);

	// Sync with the starter and enter the active scheduler
	RProcess::Rendezvous(KErrNone);
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(pServer);
	CleanupStack::PopAndDestroy(pScheduler);

	__FLOG_STATIC(KConfigLoggingTag1(),KConfigLoggingTag2(),_L("----------------Config Daemon Stopped--------------------"));
	}

EXPORT_C TInt E32Main()
/**
 * Entry point into the dll.
 *
 * @internalComponent
 * 
 * @return Standard Epoc error code on exit.
 */
	{
__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TInt err = KErrNone;
	TRAP(err,MainL());
	delete cleanup;
__UHEAP_MARKEND;
	return err;
    }




CConfigServer::CConfigServer(): 
	CServer2(EPriorityStandard)
/**
 * Constructor.
 *
 * @internalComponent
 */
	{
	}

CConfigServer::~CConfigServer()
/**
 * Destructor, closes the esock server and deletes the timer.
 *
 * @internalComponent
 */
	{
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::~CConfigServer Start"));

	iEsock.Close();	
	delete iTimer;

	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::~CConfigServer End"));
	}

CConfigServer* CConfigServer::NewL()
/**
 * Called inside the MainL() function to create and start the
 * CServer2 derived server.
 *
 * @internalComponent
 *
 * @return Instance of the server
 * @leave Various system leave codes.
 */
	{
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::NewL Start"));

	CConfigServer* server = new (ELeave) CConfigServer();
	CleanupStack::PushL(server);
	server->ConstructL();

	// CServer2 base class call
	server->StartL(KConfigServerName);
	CleanupStack::Pop(server);

	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::NewL End"));
	return server;
	}

void CConfigServer::ConstructL()
/**
 * Second stage construction.
 *
 * @internalComponent
 */
	{
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::ConstructL Start"));
	User::LeaveIfError(iEsock.Connect());
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::ConstructL End"));
	}

CSession2* CConfigServer::NewSessionL(
	const TVersion& /*aVersion*/,
	const RMessage2& /*aMessage*/) const
/**
 * The CConfigServer::NewSessionL method. Create a new If on this server.
 *
 * @internalComponent
 *
 * @param aVersion Not used.
 * @param aMessage Not used.
 * @return A new session to be used for the connection.
 */
	{
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::NewSessionL Start"));

	CConfigSession* ses = CConfigSession::NewL();
	SessionConnected();

	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::NewSessionL End"));
	return ses;
	}

void CConfigServer::SessionConnected() const
/**
 * Called by session to alert server that something has connected so
 * cancel the shutdown timer if there is one running.
 *
 * @internalComponent
 */
  	{
	if (iTimer)
		iTimer->Cancel();
  	}

void CConfigServer::Close(
	CConfigSession* aConfigSession)
/**
 * Reference counting for the server. When there 
 * are no sessions open then we shutdown, unless 
 * the only one open is oursleves, then we still 
 * shutdown.
 *
 * @internalComponent
 *
 * @param aConfigSession The session to close.
 */
	{
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::Close Start"));
	
	iSessionIter.SetToFirst();

   	TInt count = 0;
   	while (iSessionIter++ != NULL)
   		count++;

	// if we have one session then it is the current one
	// and ok to shutdown...as this function is called
	// before the session is destroyed...
	if ((!iSessionIter) || (1 == count))
		{
		iSessionIter.SetToFirst();
		if (iSessionIter == aConfigSession)
			{
			// wait for 10 seconds before shutting down completely...
			// in case anyone wants to connect
			if (!iTimer)
				{
				TRAPD(ret, iTimer = CExpireTimer::NewL());
				if (ret!=KErrNone)
					{
					// OOM so why not just shut down to free some
					// seeing as that's the process that's been set
					// in motion anyway...
					TimerExpired();
					return;
					}
				}
			iTimer->After(KOneSecond * 10, *this);
			}
		}

	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(),_L("CConfigServer::Close End"));
	}

void CConfigServer::TimerExpired()
/**
 * Invoked whtn the timer has expired.
 *
 * @internalComponent
 */
  	{
  	// just stop the server if we get here.
	CActiveScheduler::Stop();
  	}

