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
// servermain.cpp
// THIS FILE MUST BE RENAMED TO THE NAME OF OF YOUR SERVER MAIN ENTRY POINT
// DO NOT EDIT THE FILE CONTENTS
// 
//
	
	
#include "te_serverframework_server.h"
#include "lbsmessageenums.h"
#include "te_processlaunchtestsuitedefs.h"
//#include "opensessionparams.h"
#include <e32debug.h>
#include "e32base.h"
#include "csecureasbase.h"
#include "tserverstartparams.h"

// sample E32Main
// all servers should use this E32main(!), but in a renamed file.
// Derived classes make use of the abstract method CSecureAsBase::CreateServerLC(const TServerStartParams& aParams)
// For an implmenter that is your "E32Main"

TInt E32Main()
	{
	TServerStartParams params;
	TPtr ptr(reinterpret_cast<TText*>(&params), 0, sizeof(TServerStartParams)/sizeof(TText16));
	User::CommandLine(ptr);
	TInt err = CSecureASBase::ServerMain(params);
	return err;
	}

const TUint myRangeCount = 6;
const TInt myRanges[myRangeCount] = 
    {
    0, //range is 0-1 inclusive
    2, //range is 2 only
    3, //range is 3-18 inclusive
    19, // range is 19-21 inclusive
    22, // range is 22-23 inclusive
    24  // range 24-KMaxInt
    };
    
const TUint8 myElementsIndex[myRangeCount] = 
    {
    CPolicyServer::EAlwaysPass,     //applies to 0th range (req num: 0-1)
    CPolicyServer::ENotSupported,   //applies to 1st range (req num: 2)
    CPolicyServer::ENotSupported,		// applies to the 2nd range (req num: 3-18)
    0,                              //applies to 3rd range (req num: 19-21)
    CPolicyServer::EAlwaysPass,	//applies to 4th range (req num: 22-23)
    CPolicyServer::ENotSupported	//applies to 5th range (req num: 24-KMaxInt)
    };
    
const CPolicyServer::TPolicyElement myElements[] = 
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient}
    };
    
const CPolicyServer::TPolicy myPolicy =
    {
    CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
    myRangeCount,                   
    myRanges,
    myElementsIndex,
    myElements,
    };


MCreateServerImpl* CSecureASBase::GetImplLC()
	{
	CTestDummyServerImpl* impl = new(ELeave) CTestDummyServerImpl();
	CleanupStack::PushL(impl);
	return impl;
	}

void CTestDummyServerImpl::CreateServerLC(TServerStartParams& aParams)
	{
	CTestDummyServer* s = new(ELeave) CTestDummyServer(CActive::EPriorityStandard, myPolicy);
	CleanupStack::PushL(s);
	s->ConstructL(aParams.GetServerName());
	// leave the server object on the CS
	}
	
//-------------------------------------------------------------

CTestDummyServer::CTestDummyServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy) : 
																		CSecureServerBase(aPriority, aSecurityPolicy),
																		iVersion(0,0,1)
	{
	LBSLOG(ELogP1,"->CTestDummyServer::CTestDummyServer");	
	}
	
CTestDummyServer::~CTestDummyServer()
	{
	LBSLOG(ELogP1,"->CTestDummyServer::CTestDummyServer");
	
	delete iSuicideWatch;
	}
	
void CTestDummyServer::ConstructL(const TDesC& aServerName)
	{
	StartL(aServerName);
	BaseConstructL(); // MUST BE CALLED	
	iSuicideWatch = CSuicideWatch::NewL(this);
	}
	
void CTestDummyServer::OnSuicideFlag()
	{
	CActiveScheduler::Stop();
	}
	
CSession2* CTestDummyServer::DoNewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	return new (ELeave) CTestDummyServerSession(); // version number already checked at this point
	}	
	
TVersion CTestDummyServer::GetServerVersion() const
	{
	return iVersion;
	}

// called by the CServer2 code to complete construction of the session 
void CTestDummyServerSession::CreateL()
	{
	CSecureSessionSubSessionBase::CreateL(); // MUST do this
	}
	
void CTestDummyServerSession::ServiceMessageL(const RMessage2& /*aMessage*/)
	{
	// service session level messages in here
	}
	
void CTestDummyServerSession::ServiceMessageError(const RMessage2& /*aMessage*/, const TInt /*aError*/)
	{
	// service leaves from the above in here
	}
	

MSubSessionImpl* CSubSessionBase::GetImplL(const RMessage2& /*aMessage*/)
	{
	return(NULL);
	}

CSuicideWatch* CSuicideWatch::NewL(MSuicideWatch* aObserver)
	{
	CSuicideWatch* self = new(ELeave)CSuicideWatch(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);	
	return(self);
	}

CSuicideWatch::~CSuicideWatch()
	{
	iSuicideFlag.Cancel();
	iSuicideFlag.Close();
	Cancel();
	}

CSuicideWatch::CSuicideWatch(MSuicideWatch* aObserver)
:	CActive(EPriorityStandard),
	iObserver(aObserver)
	{	
	}

void CSuicideWatch::ConstructL()
	{
	CActiveScheduler::Add(this);

	iSuicideFlag.Attach(KProcessLaunchTestUid, KServerFrameworkTestSuicideKey);
	iSuicideFlag.Subscribe(iStatus);
	SetActive();
	}

void CSuicideWatch::RunL()
	{
	iObserver->OnSuicideFlag();
	}
void CSuicideWatch::DoCancel()
	{
	
	}
