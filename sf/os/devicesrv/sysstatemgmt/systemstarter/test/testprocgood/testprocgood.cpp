// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32base.h>
#include <sysmonclisess.h>
#include <startupproperties.h>
#include "testprocgoodsession.h"
#include "testprocgood.h"
#include "testapps.h"

//
// Launch code
//

/** Check if a aSwitch was given in the commandline when starting this process */
static TBool CmdLineOptionL(const TDesC& aSwitch)
	{
	const TInt KMaxCommandLength = 256;
	TBuf<KMaxCommandLength> commandLine;
	if(User::CommandLineLength() > commandLine.MaxLength())
		User::Leave(KErrTooBig);
	
	User::CommandLine(commandLine);
	
	TLex flagLex(commandLine);
	while(!flagLex.Eos())
		{
		TPtrC token(flagLex.NextToken());
		if(token == aSwitch)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

static void RunL()
	{
	User::LeaveIfError(RThread::RenameMe(KTestProcGood));

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	if(CmdLineOptionL(KLaunchServerCommandLineOption))
		{
		CTestServerGood::NewLC();
		}
		
	if(!CmdLineOptionL(KTestProcGoodNoRendevouz))
		{
		RProcess::Rendezvous(KErrNone);
		}

	if(CmdLineOptionL(KFailAfterRendevouzCommandLineOption))
		{
		User::Leave(KErrAbort);
		}
		
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2);
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunL());
		delete cleanup;
		}

	__UHEAP_MARKEND;
	return r;
	}

//
// Server
//

CTestServerGood::CTestServerGood() : CServer2(0)
	{
	}
	
CTestServerGood::~CTestServerGood()
	{
	}

CServer2* CTestServerGood::NewLC()
	{
	CTestServerGood * self=new(ELeave) CTestServerGood;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTestServerGood::ConstructL()
	{
	StartL(KTestProcGood);
	}

static CStartupProperties* StartupPropertiesLC(TInt aRecoveryMethod)	
	{
	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KLaunchServerCommandLineOption);
	prop->SetMonitored(ETrue);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetRecoveryParams((TRecoveryMethod)aRecoveryMethod, 0);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(0);
	return prop;
	}

void CTestServerGoodSession::ServiceL(const RMessage2& aMessage)
	{
	TInt err = KErrNone;
	switch (aMessage.Function())
		{
		case EMonitorSelf:
			{
			RSysMonSession sysmons;
			sysmons.OpenL();
			CleanupClosePushL(sysmons);
			CStartupProperties* prop = StartupPropertiesLC(aMessage.Int0());
			TRAP(err, sysmons.MonitorSelfL(*prop));
			CleanupStack::PopAndDestroy(prop);
			CleanupStack::PopAndDestroy(&sysmons);
			break;
			}
		case ECancelMonitor:
			{
			RSysMonSession sysmons;
			sysmons.OpenL();
			CleanupClosePushL(sysmons);
			TRAP(err, sysmons.CancelMonitorSelfL());
			CleanupStack::PopAndDestroy(&sysmons);
			break;
			}
		case EShutDown:
			CActiveScheduler::Stop();
			break;
		default:
			break;
		}
	aMessage.Complete(err);
	}
	
//
// Session
//
	
CSession2* CTestServerGood::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new(ELeave) CTestServerGoodSession();
	}

