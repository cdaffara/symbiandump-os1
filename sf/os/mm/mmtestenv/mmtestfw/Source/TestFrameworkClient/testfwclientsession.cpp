// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Client / server logging for Test Framework
// NOTE : does NOT include secure API changes in EKA2
// 
//


// Test system includes
#include <testframework.h>

/**
 *
 * Global : start a server
 * NOTE. Function is global static as only one server will ever run at any time
 * (there may be multiple client sessions)
 *
 * @return	"TInt"
 *			Error code (KErrNone if successful)
 *
 * @xxxx
 *
 */
GLDEF_C TInt StartServer()
// Start the server process/thread which lives in an (EPOC)EXE object
	{
	const TUidType serverUid(KNullUid, KNullUid, KTestFrameworkServerUid3);


// EPOC is easy, we just create a new server process. Simultaneous launching
// of two such processes should be detected when the second one attempts to
// create the server object, failing with KErrAlreadyExists.

	RProcess server;
	TInt r = server.Create(KTestFrameworkServerImg, KNullDesC, serverUid);

	if (r != KErrNone)
		return r;

	
	TRequestStatus rendezvous;
	server.Rendezvous(rendezvous);

	if (rendezvous!=KRequestPending)
		{
		server.Kill(0);
		}
	else
		{
		server.Resume();
		}


	User::WaitForRequest(rendezvous);		// wait for start or death

	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	if (rendezvous!=KErrNone)
		{
		server.Close();
		}

	// server started (at last). Cancel and consume the death-notification
	// before reporting success
	return rendezvous.Int();
	}

/**
 *
 * Constructor for RTestFrameworkClientSession
 *
 * @xxxx
 *
 */
RTestFrameworkClientSession::RTestFrameworkClientSession()
	{
	}

/**
 *
 * Client session : connect to server.
 * Will start a new server session if no server exists
 *
 * @return	"TInt"
 *			Error code (KErrNone if connect successful)
 *
 * @xxxx
 *
 */
TInt RTestFrameworkClientSession::Connect()
	{
	// NOTE : this loop is ugly and can probably be rewritten to be more graceful
	const TInt KNumRetries = 2;

	TInt retry = KNumRetries;
	for (;;)
		{
		TInt r = CreateSession(KTestFrameworkServerName, TVersion(KTestFrameworkServerMajorVersionNumber,
																  KTestFrameworkServerMinorVersionNumber,
																  KTestFrameworkServerBuildVersionNumber));
		if (r == KErrNone)
			{
#ifdef __IPC_V2_PRESENT__
			r = ShareAuto();
#else
			r = Share(RSessionBase::EAutoAttach);
#endif
			if (r!=KErrNone)
				Close();
			return r;
			}
		if (r != KErrNotFound && r != KErrServerTerminated)
			{
			return r;
			}
		if (--retry == 0)
			return r;
		r = StartServer();
		if (r != KErrNone && r != KErrAlreadyExists)
			return r;
		}
	}

/**
 *
 * Request creation of an input window.
 * NOTE. For initialisation of input console only - unlikely to
 * be required by user
 *
 * @param	"TRectBuf& aAllocatedWindow"
 *			Window dimensions
 *
 * @param	"TRequestStatus& aReqStat"
 *			Request status
 *
 * @xxxx
 *
 */
void RTestFrameworkClientSession::CreateInputWindow(TRectBuf& aAllocatedWindow, TRequestStatus& aReqStat)
	{
	SendReceiveResult(ECreateInputWindow, aAllocatedWindow, aReqStat);
	}

/**
 *
 * Request window change notifications
 * NOTE. For initialisation of input console only - unlikely to
 * be required by user
 *
 * @param	"TRectBuf& aNewWindow"
 *			New window dimensions
 *
 * @param	"TRequestStatus& aReqStat"
 *			Request status
 *
 * @xxxx
 *
 */
void RTestFrameworkClientSession::NotifyIfWindowChange(TRectBuf& aNewWindow, TRequestStatus& aReqStat)
	{
	SendReceiveResult(ENotifyIfWindowChange, aNewWindow, aReqStat);
	}

/**
 *
 * Cancel window change notifications
 * NOTE. For initialisation of input console only - unlikely to
 * be required by user
 *
 * @return	"TInt"
 *			SendReceive error code
 *
 * @xxxx
 *
 */
TInt RTestFrameworkClientSession::CancelNotifyIfWindowChange()
	{
	return SendReceive(ECancelNotifyIfWindowChange);
	}


/**
 *
 * Open a log server session
 *
 * @param	"const TDesC& aLogName"
 *			The log name
 *
 * @param	"TInt aLogMode"
 *			The log mode (a bitmask of TTestFrameworkLogMode)
 *
 * @xxxx
 *
 */
void RTestFrameworkClientSession::OpenLog(const TDesC& aLogName, TInt aLogMode)
	{
	(void) SendReceive(EOpenLog, aLogName, aLogMode);
	}

/**
 *
 * Write message string to log server session
 *
 * @param	"const TDesC& aMsg"
 *			The message string
 *
 * @param	"TInt aLogMode"
 *			The log mode (a bitmask of TTestFrameworkLogMode)
 *
 * @xxxx
 *
 */
void RTestFrameworkClientSession::WriteLog(const TDesC& aMsg, TInt aLogMode)
	{
	(void) SendReceive(EWriteLog, aMsg, aLogMode);
	}

/**
 *
 * Send close log message to server
 *
 * @xxxx
 *
 */
void RTestFrameworkClientSession::CloseLog()
	{
	SendReceive(ECloseLog);
	}

/**
 *
 * Retrieve log status from server
 *
 * @return	"TInt"
 *			The log status (a bitmask of TTestFrameworkLogMode)
 *
 * @xxxx
 *
 */
TInt RTestFrameworkClientSession::LogStatus()
	{
	TInt res = 0;
	TPckgBuf<TInt> pckg;
	TInt r = SendReceiveResult(ELogStatus, pckg);
	if (r != KErrNone)
	{
		// RTestFrameworkClientSession does not log - 
		// we can however return 0 to indicate an error (no outputs)
		res = 0;
	}
	else
		res = pckg();
	return res;
	}
