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

#include "testandstopper.h"



void CTestAndStopper::RunL()
	{
	if( !iRunning )
		{
		iRunning = ETrue;	
		DoStartsL();	
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


void CTestAndStopper::DoStartsL()
	{

	if( iStartupProperties.IsMonitoringRequired() )
		{
		iSsmStartSafe.StartAndMonitorL( iStartupProperties, iProcess );
		}
	else
		{
		iSsmStartSafe.StartL( iStartupProperties, iProcess );
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



CTestAndStopper::CTestAndStopper( CSsmStartSafe& aSsmStartSafe, 
								  const CSsmStartupProperties& aStartupProperties, 
								  RProcess& aProcess,
								  TInt aTimeout,
								  MSsTestAsyncNotifier* aSsTestAsyncNotifier )
: CActive( EPriorityStandard), 
  iSsmStartSafe( aSsmStartSafe ), 
  iStartupProperties( aStartupProperties ), 
  iProcess( aProcess ),
  iTimeout( aTimeout ),
  iSsTestAsyncNotifier( aSsTestAsyncNotifier )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, 1000000 ); // TKAS May not need this delay.
	}





CTestAndStopper::~CTestAndStopper()
	{
	Cancel();
	iTimer.Close();
	}

//
// Similar to above n=but used for making duff starts to test errors
//
//
void CDuffTestAndStopper::RunL()
	{
	if( !iRunning )
		{
		iRunning = ETrue;	
		DoStartsL();	
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


void CDuffTestAndStopper::DoStartsL()
	{
	// Call S&M even though monitoring may not be specified
	iSsmStartSafe.StartAndMonitorL( iStartupProperties, iProcess );
	}




TInt CDuffTestAndStopper::RunError( TInt aError )
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

void CDuffTestAndStopper::DoCancel()
	{
	iTimer.Close();
	
	iRunning = EFalse;
	iFinish = EFalse;
	}



CDuffTestAndStopper::CDuffTestAndStopper( CSsmStartSafe& aSsmStartSafe, 
								  const CSsmStartupProperties& aStartupProperties, 
								  RProcess& aProcess,
								  TInt aTimeout,
								  MSsTestAsyncNotifier* aSsTestAsyncNotifier )
: CActive( EPriorityStandard), 
  iSsmStartSafe( aSsmStartSafe ), 
  iStartupProperties( aStartupProperties ), 
  iProcess( aProcess ),
  iTimeout( aTimeout ),
  iSsTestAsyncNotifier( aSsTestAsyncNotifier )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, 1000000 ); // TKAS May not need this delay.
	}





CDuffTestAndStopper::~CDuffTestAndStopper()
	{
	Cancel();
	iTimer.Close();
	}



//
// Test two sync calls. Currently starts two of the same iStarupProperties
//
// Should give this an array of RProcess to start an arbitrary number of instances
//

void CTestTwoAndStopper::RunL()
	{
	if( !iRunning )
		{
		iRunning = ETrue;	
		DoStartsL();	
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


void CTestTwoAndStopper::DoStartsL()
	{

	if( iStartupProperties.IsMonitoringRequired() )
		{
		iSsmStartSafe.StartAndMonitorL( iStartupProperties, iProcess_0 );
		iSsmStartSafe.StartAndMonitorL( iStartupProperties, iProcess_1 );
		}
	else
		{
		iSsmStartSafe.StartL( iStartupProperties, iProcess_0 );
		iSsmStartSafe.StartL( iStartupProperties, iProcess_1 );
		}	
	
	}




TInt CTestTwoAndStopper::RunError( TInt aError )
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

void CTestTwoAndStopper::DoCancel()
	{
	iTimer.Close();
	
	iRunning = EFalse;
	iFinish = EFalse;
	}



CTestTwoAndStopper::CTestTwoAndStopper( CSsmStartSafe& aSsmStartSafe, 
								  const CSsmStartupProperties& aStartupProperties, 
								  RProcess& aProcess_0, RProcess& aProcess_1,
								  TInt aTimeout,
								  MSsTestAsyncNotifier* aSsTestAsyncNotifier )
: CActive( EPriorityStandard), 
  iSsmStartSafe( aSsmStartSafe ), 
  iStartupProperties( aStartupProperties ), 
  iProcess_0( aProcess_0 ),
  iProcess_1( aProcess_1 ),
  iTimeout( aTimeout ),
  iSsTestAsyncNotifier( aSsTestAsyncNotifier )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, 1000000 ); // TKAS May not need this delay.
	}



CTestTwoAndStopper::~CTestTwoAndStopper()
	{
	Cancel();
	iTimer.Close();
	}

