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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// code at the __EDIT_ME__ tags
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file captestinfoserver.cpp
*/

#include "captestinfoserver.h"

CCapTestInfoServer* CCapTestInfoServer::NewL()
/**
 * @return - Instance of the test server
 * CServer2 derived server.
 */
	{
	CCapTestInfoServer * server = new (ELeave) CCapTestInfoServer();
	CleanupStack::PushL(server);
	
	server->StartL(KInfoServerName); 
	CleanupStack::Pop(server);
	return server;
	}

CCapTestInfoServer::CCapTestInfoServer() :
	CServer2(EPriorityStandard)
	{
	iSessionConnected = EFalse;
	}
	
CCapTestInfoServer::~CCapTestInfoServer()
	{
	}

CSession2* CCapTestInfoServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
	{
	CCapTestInfoServerSession* session = CCapTestInfoServerSession::NewL();
	return session;
	}


CCapTestInfoServerSession* CCapTestInfoServerSession::NewL()
	{
	CCapTestInfoServerSession* self = new (ELeave) CCapTestInfoServerSession();
	return self;
	}

CCapTestInfoServerSession::CCapTestInfoServerSession()
	: CSession2()
	{
	}

CCapTestInfoServerSession::~CCapTestInfoServerSession()
	{
	}

void CCapTestInfoServerSession::ServiceL(const RMessage2 &aMessage)
	{
	TInt err = KErrNone;
	switch (aMessage.Function())
        {
    case ECapTestInfoThreadId:
        DoGetThreadIdL(aMessage);
        break;
    default:
        User::Leave(KErrNotSupported);
        break;
        }
	aMessage.Complete(err);
	}

void CCapTestInfoServerSession::DoGetThreadIdL(const RMessage2 &aMessage)
	{
	RThread thisThread;
	TPckgBuf<TThreadId> tid(thisThread.Id());
	aMessage.WriteL(0, tid);
	}

