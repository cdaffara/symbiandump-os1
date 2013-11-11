// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>

#include "TestFrameworkServer.h"

#include "TestFrameworkServer.inl"

#include <testframeworkipc.h>


/**
 *
 * Initiate server exit when the timer expires
 *
 * @xxxx
 * 
 */
void CTestFrameworkServerShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}

/**
 *
 * Test Framework server static constructor
 * NOTE : only one instance of the server process should run
 * during any Test Framework run; all client sessions
 * will use this one instance.
 *
 * @xxxx
 * 
 */
CMmfIpcServer* CTestFrameworkServer::NewL()
	{
	CTestFrameworkServer* s = new(ELeave) CTestFrameworkServer;
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

/**
 *
 * Test Framework server first-phase constructor
 *
 * @xxxx
 * 
 */
CTestFrameworkServer::CTestFrameworkServer()
	:CMmfIpcServer(0, ESharableSessions)
	{
	}

// set up so that iConsole is NULL. A call to OpenLogL will initialise the console
// and file (if any)

/**
 *
 * Test Framework server second-phase constructor
 * Starts the server; does not initialise the log
 * (this must be done independently with a call to OpenLogL)
 *
 * @xxxx
 * 
 */
void CTestFrameworkServer::ConstructL()
	{
	StartL(KTestFrameworkServerName);
	//Ensure that the server will exit even if the first client fails to connect
	iShutdown.ConstructL();
	iShutdown.Start();

	iLogMode = 0;	// initial value : not running any log
	iConsole = NULL;
	iFileLogger = NULL;
	}

/**
 *
 * Test Framework server destructor
 *
 * @xxxx
 * 
 */
CTestFrameworkServer::~CTestFrameworkServer()
	{
	CloseLog();
	}

/**
 *
 * Create new server session.
 *
 * @param	"const TVersion &aVersion"
 *			Server version (required)
 *
 * @return	"CSharableSession*"
 *			The created server session.
 * 
 * @xxxx
 * 
 */
CMmfIpcSession* CTestFrameworkServer::NewSessionL(const TVersion &aVersion) const
	{
	TVersion ver(KTestFrameworkServerMajorVersionNumber,
			     KTestFrameworkServerMinorVersionNumber,
			     KTestFrameworkServerBuildVersionNumber);
	if (!User::QueryVersionSupported(ver, aVersion))
		User::Leave(KErrNotSupported);
	// make new session
	return new(ELeave) CTestFrameworkServerSession();
	}

/**
 *
 * Add a server session.
 * 
 * @xxxx
 * 
 */
void CTestFrameworkServer::AddSession()
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

/**
 *
 * Drop a server session.
 * 
 * @xxxx
 * 
 */
void CTestFrameworkServer::DropSession()
	{
	if (--iSessionCount <= 0)
		iShutdown.Start();
	}

/**
 *
 * Add an input window (at request from client).
 * 
 * @param	"CTestFrameworkServerSession* aOwner"
 *			Window owning server session
 *
 * @xxxx
 * 
 */
void CTestFrameworkServer::AddInputWindowL(CTestFrameworkServerSession* aOwner)
	{
	if (iInputWindow.HasOwner())
		User::Leave(KErrAlreadyExists);
	// else
	iInputWindow.SetOwner(aOwner);
	TRect rect;
	rect.iTl = TPoint(0,0);
	rect.iBr = TPoint(KConsFullScreen, KConsFullScreen);
	iInputWindow.SetWinRectAndNotifyOwner(rect);
	}

/**
 *
 * Remove an input window (at request from client).
 * 
 * @param	"CTestFrameworkServerSession* aOwner"
 *			Window owning server session
 *
 * @xxxx
 * 
 */
void CTestFrameworkServer::RemoveWindow(CTestFrameworkServerSession* aOwner)
	{
	if (aOwner == iInputWindow.Owner())
		{
		iInputWindow.SetOwner(NULL);
		}
	}

/**
 *
 * Open a log.
 * 
 * @param	"const TDesC& aLogName"
 *			The log name
 *
 * @param	"TInt aLogMode"
 *			The log mode (as bitmask of TTestFrameworkLogMode)
 *
 * @xxxx
 * 
 */
void CTestFrameworkServer::OpenLogL(const TDesC& aLogName, TInt aLogMode)
	{
	// NB we need to check if a console is already open - if so, we do NOT
	// create another one. Ditto with file / port.

	if(aLogMode & ELogToConsole)
		{
		if(!iConsole)
			{
			iConsole = CServerConsole::NewL(aLogName);

			CConsoleBase* theConsole = iConsole->Console();
			theConsole->Printf(_L("%S : Server log starting\n"), &aLogName);

			iLogMode |= ELogToConsole;
			
			if (aLogMode & ELogConsoleFull)
				iLogMode |= ELogConsoleFull;
			}
		}

	// NB relative paths will not work with TParse (there is no file server open).
	// Exception is a bare filename (with no path) : this will be found in root of C:

	// NOTE! We have no mechanism to notify this error. The console will display
	// and then exit. The log file cannot be opened.

	// TO BE ENHANCED - if console is made active, then we can pause

	if(aLogMode & ELogToFile)
	{
		if(!iFileLogger)
			{
			TRAPD(err, iFileLogger = CFileLogger::NewL());
			if(err != KErrNone)
				{
				// if we can't create a logger, we panic
				User::Panic(_L("TestFrameworkServer"), 1);
				}

			_LIT(KLogPath, "C:\\Logs\\TestResults");
			_LIT(KDefault, "C:\\.htm"); 

			TParse parseLogName;
			parseLogName.Set(aLogName, NULL, NULL); 

			TFileName logFilePath;
			logFilePath = KLogPath;

			if(parseLogName.PathPresent())
				logFilePath.Append(parseLogName.Path());
			else
				logFilePath.Append(_L("\\"));


			// overwrite extension if supplied with .htm
			TParse logFileFullName;
			TInt returnCode = logFileFullName.Set(KDefault, &logFilePath, &aLogName);
			if (returnCode == KErrNone)
				{
				TInt ret = iFileLogger->Connect();
				if (ret == KErrNone)
					{
					iFileLogger->CreateLog(logFilePath, logFileFullName.NameAndExt());
					iLogMode |= ELogToFile;
					}
				}
			}
		}

	if(aLogMode & ELogToPort)
		{
		// RDebug::Print will deal with the serial port, we don't do anything special here
		iLogMode |= ELogToPort;
		}
	}

/**
 *
 * Write to the log.
 * 
 * @param	"const TDesC& aMsg"
 *			The message string to write
 *
 * @param	"TInt aLogMode"
 *			The log mode (as bitmask of TTestFrameworkLogMode)
 *
 * @xxxx
 * 
 */
void CTestFrameworkServer::WriteLog(const TDesC& aMsg, TInt aLogMode)
	{
	if(aLogMode & ELogToConsole)
		{
		if(iConsole)
			{
			CConsoleBase* theConsole = iConsole->Console();
			theConsole->Printf(aMsg);
			theConsole->Printf(_L("\n")); // add newline
			}
		}

	if(aLogMode & ELogToFile)
		{
		if(iFileLogger)
			{
			iFileLogger->WriteLog(aMsg);
			}
		}

	if(aLogMode & ELogToPort)
		{
		RDebug::Print(aMsg);
		}
	}

/**
 *
 * Close the log.
 * 
 * @xxxx
 * 
 */
void CTestFrameworkServer::CloseLog()
	{
	if (iFileLogger != NULL)
		iFileLogger->CloseLog();
	delete(iFileLogger);
	iFileLogger = NULL;
	delete(iConsole);
	iConsole = NULL;
	iLogMode = 0;
	}

/**
 *
 * Get the log status.
 *
 * @return	"TInt"
 *			The log status (as bitmask of TTestFrameworkLogMode)
 *
 * @xxxx
 * 
 */
TInt CTestFrameworkServer::LogStatus() const
	{
	return iLogMode;
	}


/**
 *
 * Default window constructor (no owner)
 *
 * @xxxx
 * 
 */
TWindow::TWindow()
	{
	iOwner = NULL;
	}

/**
 *
 * Default window constructor
 *
 * @param	"CTestFrameworkServerSession* aOwner"
 *			The window owner
 * @xxxx
 * 
 */
TWindow::TWindow(CTestFrameworkServerSession* aOwner)
	{
	iOwner = aOwner;
	}

/**
 *
 * Set the window owner
 *
 * @param	"CTestFrameworkServerSession* aOwner"
 *			The window owner
 * @xxxx
 * 
 */
void TWindow::SetOwner(CTestFrameworkServerSession* aOwner)
	{
	iOwner = aOwner;
	}

/**
 *
 * Set the window rectangle, and notify owner
 *
 * @param	"const TRect& aWinRect"
 *			The window rectangle
 * @xxxx
 * 
 */
void TWindow::SetWinRectAndNotifyOwner(const TRect& aWinRect)
	{
	if (HasOwner())
		iOwner->NotifyWindowChanged(aWinRect);
	}

/**
 *
 * Does this window have an owner?
 *
 * @return	"TBool"
 *			ETrue if window has an owner
 * @xxxx
 * 
 */
TBool TWindow::HasOwner()
	{
	if (iOwner)
		return ETrue;
	return EFalse;
	}

/**
 *
 * Server session first-phase constructor
 *
 * @xxxx
 * 
 */
CTestFrameworkServerSession::CTestFrameworkServerSession()
	{
	}

/**
 *
 * Create a server session.
 *
 * @param "aServer"
 *			The server to add this session to
 *
 * @xxxx
 * 
 */
void CTestFrameworkServerSession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);	// does not leave
	//Add session to server first. If anything leaves, it will be removed by the destructor
	iServer = STATIC_CAST(CTestFrameworkServer*, (CONST_CAST(CMmfIpcServer*, &aServer)));
	iServer->AddSession();
	ConstructL();
	}

/**
 *
 * Server session second-phase constructor
 *
 * @xxxx
 * 
 */
void CTestFrameworkServerSession::ConstructL()
	{
	}

/**
 *
 * Server session destructor
 *
 * @xxxx
 * 
 */
CTestFrameworkServerSession::~CTestFrameworkServerSession()
	{
	iServer->RemoveWindow(this);
	iServer->DropSession();
	}

/**
 *
 * Server session service function
 *
 * @param "aMessage"
 *			The message to be serviced.
 *
 * @xxxx
 * 
 */
void CTestFrameworkServerSession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	switch (aMessage.Function())
		{
	case ECreateInputWindow:
		SetOwnCopyOfWindowMessageL(aMessage);
		iServer->AddInputWindowL(this);
		break;
	case ENotifyIfWindowChange:
		SetOwnCopyOfWindowMessageL(aMessage);
		break;
	case ECancelNotifyIfWindowChange:
		CompleteOwnCopyOfWindowMessage(KErrCancel);
		aMessage.Complete(KErrNone);
		break;

	// logging messages

	case EOpenLog:
		{
		TBuf<KMaxLogFilenameLength> msgBuf;

		TInt r = MmfMessageUtilX::Read(aMessage, 0, msgBuf);
		if (r != KErrNone)
			RunError(aMessage, r);
		else
			{
			iServer->OpenLogL(msgBuf, aMessage.Int1());
			aMessage.Complete(KErrNone);
			}
		}
		break;

	case EWriteLog:
		{
		TBuf<KMaxLogLineLength> msgBuf;

		TInt r = MmfMessageUtilX::Read(aMessage, 0, msgBuf);

		if (r != KErrNone)
			RunError(aMessage, r);
		else
			{
			iServer->WriteLog(msgBuf, aMessage.Int1());
			aMessage.Complete(KErrNone);
			}
		}
		break;

	case ECloseLog:
		iServer->CloseLog();
		aMessage.Complete(KErrNone);
		break;

	case ELogStatus:
		{
		TPckgBuf<TInt> countPckg(iServer->LogStatus());
		TInt r = MmfMessageUtilX::Write(aMessage, 0, countPckg);
		if (r != KErrNone)
			RunError(aMessage, r);
		else
			aMessage.Complete(KErrNone);
		}
		break;

	default:
		// ? should panic here
		break;
		}
	}

/**
 *
 * Error handler for server session.
 * Completes the pending message using the error code
 *
 * @param	"TInt aError"
 *			The error code
 *
 * @return	"TInt"
 *			The error code (always KErrNone)
 *
 * @xxxx
 *
 */
 TInt CTestFrameworkServerSession::RunError(const RMmfIpcMessage& aMessage, TInt aError)
	{
	aMessage.Complete(aError);
	return KErrNone;
	}

/**
 *
 * Set own copy of message.
 * Helper function to enable window change notification.
 *
 * @param "aMessage"
 *			The message to be serviced.
 *
 * @xxxx
 *
 */
 void CTestFrameworkServerSession::SetOwnCopyOfWindowMessageL(const RMmfIpcMessage& aMessage)
	{
	if (iCanCompleteWindowMessage)
		User::Leave(KErrAlreadyExists);
	//else
	iWindowMessage = aMessage;
	iCanCompleteWindowMessage = ETrue;
	
	if (iNeedToNotifyClientOfWindowSizeChange)
		{
		TInt err = MmfMessageUtil::Write(iWindowMessage, 0, iWinRectBuf);
		CompleteOwnCopyOfWindowMessage(err);
		iNeedToNotifyClientOfWindowSizeChange = EFalse;
		}
	}

/**
 *
 * Complete own copy of message.
 * Helper function to enable window change notification.
 *
 * @param "TInt aReason"
 *			The message return code.
 *
 * @xxxx
 *
 */
 void CTestFrameworkServerSession::CompleteOwnCopyOfWindowMessage(TInt aReason)
	{
	if (iCanCompleteWindowMessage)
		iWindowMessage.Complete(aReason);
	iCanCompleteWindowMessage = EFalse;
	}

/**
 *
 * Window change notification.
 *
 * @param "const TRect& aWinRect"
 *			The window rectangle.
 *
 * @xxxx
 *
 */
void CTestFrameworkServerSession::NotifyWindowChanged(const TRect& aWinRect)
	{
	iWinRectBuf() = aWinRect;

	if (iCanCompleteWindowMessage)
		{
		TInt err = MmfMessageUtilX::Write(iWindowMessage, 0, iWinRectBuf);
		CompleteOwnCopyOfWindowMessage(err);
		}
	else
		{
		iNeedToNotifyClientOfWindowSizeChange = ETrue;
		}
	}

// EXE/DLL initialisation

// NOTE! None of this should be built when compiling for Unit Testing
#if !defined (__TSU_TESTFRAMEWORK__)


/**
 *
 * Perform all server initialisation, in particular creation of the
 * scheduler and server; then run the scheduler
 *
 *
 * @xxxx
 *
 */
static void RunServerL()
	{
	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CleanupStack::PushL(CTestFrameworkServer::NewL());
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

/**
 *
 * Main entry point for the server thread
 *
 * @param "TTestFrameworkServerStart& aStart"
 *			The server starter.
 *
 * @xxxx
 *
 */
static TInt RunServer()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt r = KErrNoMemory;
	if (cleanup)
		{
		TRAP(r, RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}


GLDEF_C TInt E32Main()

// Server process entry-point
// Recover the startup parameters and run the server

	{
	TInt r = RunServer();
	return r;
	}


#endif // __TSU_TESTFRAMEWORK__
