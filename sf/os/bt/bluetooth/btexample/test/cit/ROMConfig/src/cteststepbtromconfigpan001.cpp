// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cteststepbtromconfigpan001.h"
#include <es_sock.h>
#include <bt_sock.h>
#include <connpref.h>
#include <commdbconnpref.h>

namespace
	{
	const TInt KTestTimeoutMillis = 10000000;
	}

CTestStepBtRomConfigPan001::~CTestStepBtRomConfigPan001()
	{
	delete iTestTimeout;
	delete iConnStart;
    }	

CTestStepBtRomConfigPan001::CTestStepBtRomConfigPan001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNamePan001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigPan001* CTestStepBtRomConfigPan001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigPan001(aParent); 
	}
	
/**
See BT_ROMCONFIG_RCONNECTION_001.script
*/
TVerdict CTestStepBtRomConfigPan001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigPan001::doTestStepL()"));
	// Behaviour will be either a KErrNotFound if BT is excluded, or a test timeout
	// if BT is included (as no link layer will come up)
	
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	iConnStart = CActiveConnStart::NewL();
	iTestTimeout = CPan001TestTimeout::NewL();
	
	iConnStart->DoRConnectionStart();
	iTestTimeout->After(KTestTimeoutMillis);
	
	if ( iBtExcluded )
		{
		INFO_PRINTF1(_L("Our config leads us to believe that BT is NOT in the ROM"));
		}
	else
		{
		INFO_PRINTF1(_L("Our config leads us to believe that BT IS in the ROM"));
		}

	// The Active Scheduler will be stopped either by the RConnection::Start 
	// completing or by a test timeout
	sched->Start();

	INFO_PRINTF2(_L("RConnection::Start status = %d"), iConnStart->iStatus.Int());
	INFO_PRINTF2(_L("Test timeout status = %d"), iTestTimeout->iStatus.Int());				

	// if Bluetooth has been excluded then we expect the RConnection::Start to be
	// completed with an error code
	if ( iBtExcluded )
		{
		if ( iConnStart->iStatus.Int() == KErrNotFound )
			{
			INFO_PRINTF1(_L("Test passes"));
			}
		else
			{
			INFO_PRINTF1(_L("Test fails"));
			SetTestStepResult(EFail);
			}
		}
	// if Bluetooth is included then we expect the RConnection::Start to timeout
	// as we will not get a link layer up	
	else
		{
		if ( iConnStart->iStatus.Int() == KRequestPending )
			{
			INFO_PRINTF1(_L("Test passes"));
			}
		else
			{
			INFO_PRINTF1(_L("Test fails"));
			SetTestStepResult(EFail);
			}		
		}
		
	if ( iTestTimeout->IsActive() )	
		{
		iTestTimeout->Cancel();
		}
	if ( iConnStart->IsActive() )	
		{
		iConnStart->Cancel();
		}
		
	CleanupStack::PopAndDestroy(sched);
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigPan001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

CPan001TestTimeout* CPan001TestTimeout::NewL()
	{
	CPan001TestTimeout* self = new (ELeave) CPan001TestTimeout();
	CleanupStack::PushL(self);
	self->CTimer::ConstructL();
	CleanupStack::Pop();
	return self;
	}

CPan001TestTimeout::CPan001TestTimeout()
	: CTimer(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}
	
CPan001TestTimeout::~CPan001TestTimeout()
	{
	Cancel();
	}

void CPan001TestTimeout::RunL()
	{
	CActiveScheduler::Stop();
	}

void CPan001TestTimeout::DoCancel()
	{
	}
	
CActiveConnStart* CActiveConnStart::NewL()
	{
	CActiveConnStart* self = new(ELeave) CActiveConnStart;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CActiveConnStart::CActiveConnStart()
	: CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

void CActiveConnStart::ConstructL()
	{
	User::LeaveIfError( iSocketServ.Connect() );
	User::LeaveIfError( iConnection.Open(iSocketServ) );
	}
	
	
CActiveConnStart::~CActiveConnStart()
	{
	Cancel();
	if ( iSocketServ.Handle() )
		{
		if ( iConnection.SubSessionHandle() )
			{
			iConnection.Close();
			}
		iSocketServ.Close();
		}
	}

void CActiveConnStart::DoRConnectionStart()
	{
	if ( !IsActive() )
		{
		TCommDbConnPref connPref;
		connPref.SetDirection(ECommDbConnectionDirectionIncoming);
		iConnection.Start(connPref, iStatus);
		SetActive();
		}
	}
	
void CActiveConnStart::RunL()
	{
	CActiveScheduler::Stop();
	}

void CActiveConnStart::DoCancel()
	{
	if ( iSocketServ.Handle() && iConnection.SubSessionHandle() )
		{
		iConnection.Stop();
		}
	}

// EOF

