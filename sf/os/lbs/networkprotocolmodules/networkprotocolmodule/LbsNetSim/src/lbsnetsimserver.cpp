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

#include <ecom/ecom.h>
#include "lbsnetsim.h"
#include "lbsnetsimserver.h"
#include "lbsnetsimsession.h"
#include "lbsnetsimtesthandler.h"
#include "lbsnetsimgatewayhandler.h"
#include "lbsnetsimassistancedataprovider.h"


//
// Policy Stuff
const TUint KRangeCount = 6;

const TInt KRanges[KRangeCount] = {0, 3, 1000, 1017, 2000, 2020};
const TUint8 KElementsIndex[KRangeCount] = { CPolicyServer::EAlwaysPass, 
											 CPolicyServer::ENotSupported, 
											 CPolicyServer::EAlwaysPass,
											 CPolicyServer::ENotSupported,
											 CPolicyServer::EAlwaysPass,
											 CPolicyServer::ENotSupported };
const CPolicyServer::TPolicyElement KPolicies[1] = 
	{{_INIT_SECURITY_POLICY_C1(ECapabilityDiskAdmin), CPolicyServer::EFailClient}};
const CPolicyServer::TPolicy KPolicy = 
	{
	CPolicyServer::EAlwaysPass,
	KRangeCount,
	KRanges,
	KElementsIndex,
	KPolicies
	};
//


/**
*/
CSession2* CLbsNetSimServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	// Need to castaway the const
	CLbsNetSimServer* self = const_cast<CLbsNetSimServer*>(this);
	CSession2* newSession = new (ELeave) CLbsNetSimSession(self);
	self->IncClientCount();	
	return newSession;
	}


/**
*/
TInt CLbsNetSimServer::StartServer()
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanupStack = CTrapCleanup::New();

	TInt ret = KErrNoMemory;
	if (cleanupStack)
		{
		TRAP(ret, StartServerL());
		delete cleanupStack;
		}
	
	__UHEAP_MARKEND;
	
	return ret;
	}
	
CLbsNetSimServer::~CLbsNetSimServer()
	{
	delete iAssistanceDataProvider;
	delete iTestHandler;
	delete iGatewayHandler;
	
	REComSession::FinalClose();
	}

		
/**
*/		
MLbsNetSimSessionHandler* CLbsNetSimServer::GetGatewayHandler()
	{
	return iGatewayHandler;
	}


/**
*/
MLbsNetSimSessionHandler* CLbsNetSimServer::GetTestHandler()
	{
	return iTestHandler;
	}

void CLbsNetSimServer::IncClientCount()
	{
	++iClientCount;
	}
	
void CLbsNetSimServer::DecClientCount()
	{
	--iClientCount;
	if (iClientCount <=0)
		{
		CActiveScheduler::Stop();
		}
	}

//
// Private Methods

/**
*/
CLbsNetSimServer::CLbsNetSimServer(TInt aPriority) :
	CPolicyServer(aPriority, KPolicy)
	{
	}


/**
*/
CLbsNetSimServer* CLbsNetSimServer::NewL()
	{
	CLbsNetSimServer* self = new(ELeave) CLbsNetSimServer(EPriorityStandard);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}


/**
*/
void CLbsNetSimServer::ConstructL()
	{
	iAssistanceDataProvider = CLbsNetSimAssistanceDataProvider::NewL();
	iTestHandler = CLbsNetSimTestHandler::NewL(*iAssistanceDataProvider);
	iGatewayHandler = CLbsNetSimGatewayHandler::NewL(*iAssistanceDataProvider);
	
	iTestHandler->SetGatewayHandler(iGatewayHandler);
	iGatewayHandler->SetTestHandler(iTestHandler);
	}

/**
*/
void CLbsNetSimServer::StartServerL()
	{
#ifndef EKA2
	// Rename thread to aid debugging
	User::LeaveIfError(RThread().Rename(KLbsNetSimServerName));
#endif

	// Start scheduler
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Start server
	CLbsNetSimServer* server = CLbsNetSimServer::NewL();
	CleanupStack::PushL(server);

	server->StartL(KLbsNetSimServerName);

	// Signal we have started
	RProcess().Rendezvous(KErrNone);

	// Start the active schedular
	CActiveScheduler::Start();

	// Cleanup
	CleanupStack::PopAndDestroy(2, scheduler);
	}


//
// Global start up
int E32Main(void)
	{
	return CLbsNetSimServer::StartServer();
	}
