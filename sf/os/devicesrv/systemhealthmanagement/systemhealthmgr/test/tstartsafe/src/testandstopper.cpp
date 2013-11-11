// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testandstopper.h"


void CTestAndStopper::RunL()
	{
	if( !iRunning )
		{
		iRunning = ETrue;	
		iStartSafe.StartL( iStartupProperties, iProcess, iNumTried );
		SetActive();
		
		TRequestStatus* trs = &iStatus;
		User::RequestComplete( trs, KErrNone );
		}
	else if( !iFinish )
		{
		iFinish = ETrue;
		SetActive();
		
		iTimer.After( iStatus, iTimeout );
		}
	else
		{
		CActiveScheduler::Stop();
		}
	
	}


TInt CTestAndStopper::RunError( TInt aError )
	{
	if( iSsTestAsyncNotifier )
		{
		iSsTestAsyncNotifier->SsTestNotify( aError );
		}
	
	if( !iFinish )
		{
		// Need to go round once more to stop the sched.
		iFinish = ETrue;
		SetActive();
		
		iTimer.After( iStatus, iTimeout );
		}	
	
	return KErrNone;
	}

void CTestAndStopper::DoCancel()
	{
	iTimer.Close();
	
	iRunning = EFalse;
	iFinish = EFalse;
	}



CTestAndStopper::CTestAndStopper( CStartSafe& aStartSafe, 
								  const CStartupProperties& aStartupProperties, 
								  RProcess& aProcess,
								  TInt aTimeout,
								  TInt& aNumTried,
								  MSsTestAsyncNotifier* aSsTestAsyncNotifier )
: CActive( EPriorityStandard), 
  iStartSafe( aStartSafe ), 
  iStartupProperties( aStartupProperties ), 
  iProcess( aProcess ),
  iTimeout( aTimeout ),
  iNumTried ( aNumTried ),
  iSsTestAsyncNotifier( aSsTestAsyncNotifier )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, 1000000 );  
	}

CTestAndStopper::~CTestAndStopper()
	{
	Cancel();
	iTimer.Close();
	}
