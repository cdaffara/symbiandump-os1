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

#include <e32std.h>

#include "testserver.h"
#include "teststepfinish.h"
#include "teststepinit.h"
#include "teststepdisconnectall.h"

_LIT( KTestServerName, "LbsBluetoothUtilsTestServer");

CBluetoothUtilsTestServer* CBluetoothUtilsTestServer::NewL()
	{
	CBluetoothUtilsTestServer* server = new (ELeave) CBluetoothUtilsTestServer;
	CleanupStack::PushL(server);
	server->ConstructL(KTestServerName);
	CleanupStack::Pop(server);
	return server; 
	}

CBluetoothUtilsTestServer::CBluetoothUtilsTestServer()
	{
	}	

void CBluetoothUtilsTestServer::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	Logger().ShareAuto();	// enable the log system for all test threads via iParent.Logger()
	
	iScheduler = new (ELeave) CActiveScheduler();

	}

CBluetoothUtilsTestServer::~CBluetoothUtilsTestServer()
	{
	delete iScheduler;
	}

CTestStep* CBluetoothUtilsTestServer::CreateTestStep(const TDesC& aStepName)
	{
	if( aStepName == KTestStepFinish )
		{
		return CTestStepFinish::New(*this);
		}
	if( aStepName == KTestStepInit )
		{
		return CTestStepInit::New(*this);
		}
	if( aStepName == KTestStepDisconnectAll )
		{
		return CTestStepDisconnectAll::New(*this);
		}
	return NULL;
	}

void CBluetoothUtilsTestServer::Complete(TBluetoothEvent aEvent, TInt aErr)
	{
	iLastEvent = aEvent;
	iLastError = aErr;
	CActiveScheduler::Current()->Stop();
	}

void CBluetoothUtilsTestServer::HandleCreateConnectionCompleteL(TInt /* aErr */)
	{
	ASSERT(0);
	}

void CBluetoothUtilsTestServer::HandleDisconnectCompleteL(TInt /* aErr */)
	{
	ASSERT(0);
	}

void CBluetoothUtilsTestServer::HandleDisconnectAllCompleteL(TInt aErr)
	{
	Complete(EDisconnectAllComplete, aErr);
	}
