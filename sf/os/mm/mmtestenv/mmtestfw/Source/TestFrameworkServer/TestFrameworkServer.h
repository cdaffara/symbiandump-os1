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
// TestFrameworkServer provides a console and/or log
// which will handle all output from test threads.
// 
//

#ifndef __TESTFRAMEWORKSERVER_H__
#define __TESTFRAMEWORKSERVER_H__

#include <testframework.h>
#include "LogFile.h"
#include "ServerConsole.h"

#include <mmf/common/mmfipc.h> // for MmfIpc classes
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif

class CTestFrameworkServerSession;

/**
 *
 * A timer used to shut the server down after all clients 
 * have exited.
 *
 * @xxxx
 *
 */
class CTestFrameworkServerShutdown : public CTimer
	{
	enum
		{
		KTestFrameworkServerShutdownDelay = 5000000 // 5 seconds
		};	
public:
	CTestFrameworkServerShutdown();
	void ConstructL();
	void Start();
private:
	void RunL();
	};

/**
 *
 * Window helper class, allows client and server to set/
 * modify (console) windows
 *
 * @xxxx
 *
 */
class TWindow
	{
public:
	TWindow();
	TWindow(CTestFrameworkServerSession* aOwner);
	void SetOwner(CTestFrameworkServerSession* aOwner);
	void SetWinRectAndNotifyOwner(const TRect& aWinRect);
	CTestFrameworkServerSession* Owner() {return iOwner;};
	TBool HasOwner();
private:
	CTestFrameworkServerSession* iOwner;
	};

/**
 *
 * Test Framework Server.
 * Responsible for opening/closing console and writing to it.
 * Ditto log file.
 *
 * To be adapted to write to a COM port.
 *
 * @xxxx
 *
 */
class CTestFrameworkServer : public CMmfIpcServer
	{
public:
	//construct / destruct
	static CMmfIpcServer* NewL();
	~CTestFrameworkServer();

	void AddSession();
	void DropSession();

	void OpenLogL(const TDesC& aLogName, TInt aLogMode);
	void WriteLog(const TDesC& aMsg, TInt aLogMode);
	void CloseLog();
	TInt LogStatus() const;

	void AddInputWindowL(CTestFrameworkServerSession* aOwner);
	void RemoveWindow(CTestFrameworkServerSession* aOwner);

private:
	CTestFrameworkServer();
	void ConstructL();
	//open/close a session
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;

private:
	TInt iSessionCount;	// The number of sessions
	CTestFrameworkServerShutdown iShutdown;// A timer used to shut the server down after all clients have exited.

	// An active console, receiving output from client messages and input from server
	CServerConsole* iConsole;

	// logging parms
	TUint iLogMode;
	TBuf<KMaxLogFilenameLength> iLogName;
	CFileLogger* iFileLogger;

	// last keypress
	TInt iInputKey;

	TWindow iInputWindow;
	};

/**
 *
 * Test Framework Server Session :
 * handles message passing between Test Framework client / server
 *
 * @xxxx
 *
 */
class CTestFrameworkServerSession : public CMmfIpcSession
	{
public:
	CTestFrameworkServerSession();
	void CreateL(const CMmfIpcServer& aServer);
	void ConstructL();
	~CTestFrameworkServerSession();

	void ServiceL(const RMmfIpcMessage &aMessage);
	TInt RunError(const RMmfIpcMessage& aMessage, TInt aError);

	void NotifyWindowChanged(const TRect& aWinRect);

private:
	// async window message handling
	void SetOwnCopyOfWindowMessageL(const RMmfIpcMessage& aMessage);
	void CompleteOwnCopyOfWindowMessage(TInt aReason);

private:
	CTestFrameworkServer* iServer;	//<pointer to owning server

	// async window message data
	RMmfIpcMessage iWindowMessage;
	TBool iCanCompleteWindowMessage;
	TBool iNeedToNotifyClientOfWindowSizeChange;
	TRectBuf iWinRectBuf;
	};



#endif //__TESTFRAMEWORKSERVER_H__
