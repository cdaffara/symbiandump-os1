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

#include "OomTests.h"
#include "CStubbedAgent.h"
#include "DbAccess.h"
#include "NetConLog.h"

//
//  Test Case NC-5-1
//

CNetConTest0501* CNetConTest0501::NewLC()
	{

	CNetConTest0501* self = new(ELeave) CNetConTest0501();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0501::CNetConTest0501()
	{}

CNetConTest0501::~CNetConTest0501()
	{}

const TDesC& CNetConTest0501::Name() const
	{

	_LIT(KTestName, "Test Case NC-5-1");
	return KTestName();
	}

void CNetConTest0501::StartTestL()
	{

	iNetCon->SelectAgent(this, NULL, EConnStartExplicit, 1, KErrNone);
	}

void CNetConTest0501::SelectComplete(const TDesC&)
	{

	CleanupAgent();

	CompleteTest(MNetConTest::EPassed);
	}

void CNetConTest0501::SelectComplete(TInt)
	{

	CleanupAgent();

	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0501::CleanupAgent()
//
//  Free agent allocated in dummy Nif class
//  (normally this would be done by Nifman)
//
	{
	
	CNetwork* network;
	TInt ret = iNetCon->FindNetworkById(1, network);
	if(ret==KErrNone)
		{
		CNifAgentBase* agent;
		ret = network->FindAgentByIap(1, agent);
		if(ret==KErrNone)
			{
			LOG( NetConLog::Printf(_L("CleanupAgent() deleting agent at [0x%08x]"), agent); )
			
			CleanupStack::PopAndDestroy();  // CStubbedAgent allocated by Nif::CreateAgent()
			}
		}
	}


//
//  Test Case NC-5-2
//

CNetConTest0502* CNetConTest0502::NewLC()
	{

	CNetConTest0502* self = new(ELeave) CNetConTest0502();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0502::CNetConTest0502()
	{}

void CNetConTest0502::ConstructL()
	{

	COomTestBase::ConstructL();	

	iDatabase = CCommsDbAccess::NewL();
	iAgent = new(ELeave) CStubbedAgent();
	}

CNetConTest0502::~CNetConTest0502()
	{
	
	if(iAgent)
		delete iAgent;

	if(iDatabase)
		delete iDatabase;
	}

const TDesC& CNetConTest0502::Name() const
	{

	_LIT(KTestName, "Test Case NC-5-2");
	return KTestName();
	}

void CNetConTest0502::StartTestL()
	{

	// setup agent's connection settings
	TConnectionSettings settings;
	iDatabase->GetCurrentSettingsL(settings, ECommDbConnectionDirectionOutgoing, 1);
	iAgent->SetConnectionSettingsL(settings);

	iNetCon->Reconnect(this, iAgent);
	}

void CNetConTest0502::ReconnectComplete(TInt aError)
	{

	if(aError == KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}

