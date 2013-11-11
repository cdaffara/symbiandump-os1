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

#include "testandcanceler.h"



void CTestAndCanceler::RunL()
	{

	switch( iCycle )
		{
	case 0:
		{
		iCycle++;
		SetActive();
		
		iSsmStartSafe.Start( iStartupProperties, iProcess, iStatus, iSsIndex );
		switch( iDuff )
			{
			case ESsTestDuffBehaviourGood:
				iStartCanceler = new(ELeave) CStartCanceler( iSsIndex, iSsmStartSafe, iTimeout );
				 break;
				 
			case ESsTestDuffBehaviourBadIndex:
				{
				iSsIndex = 2012;
				iStartCanceler = new(ELeave) CStartCanceler( iSsIndex, iSsmStartSafe, iTimeout );
				}
				break;
				
			default:
				break;
			}
		
		}
		break;
		
	case 1:
		{
		switch( iDuff )
			{
			case ESsTestDuffBehaviourGood:
				{
				iSsTestAsyncNotifier->SsTestNotify( iStatus.Int() );
				CActiveScheduler::Stop();
				}
				break;
			
			case ESsTestDuffBehaviourBadIndex:
				{
				iSsTestAsyncNotifier->SsTestNotify( iSsIndex );
				CActiveScheduler::Stop();				
				}
				break;	

			default:
				break;
			}

		}
		break;
		
	default:
		User::Leave( KErrOverflow );
		}	
			
	}



TInt CTestAndCanceler::RunError( TInt aError )
	{
	// Defaulted to NULL in c'tor param
	if( iSsTestAsyncNotifier )
		{
		iSsTestAsyncNotifier->SsTestNotify( aError );
		}
	
	CActiveScheduler::Stop();
	
	return KErrNone;
	}

void CTestAndCanceler::DoCancel()
	{
	iTimer.Close();
	}



CTestAndCanceler::CTestAndCanceler( CSsmStartSafe& aSsmStartSafe, 
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



// The constructor specifying a type of duff behaviour in order to invoke error reporting
CTestAndCanceler::CTestAndCanceler( CSsmStartSafe& aSsmStartSafe, 
		  const CSsmStartupProperties& aStartupProperties, 
		  RProcess& aProcess,
		  TInt aTimeout,
		  MSsTestAsyncNotifier* aSsTestAsyncNotifier,
		  TSsTestDuffBehaviour aDuffBehaviour )
: CActive( EPriorityStandard), 
  iSsmStartSafe( aSsmStartSafe ), 
  iStartupProperties( aStartupProperties ), 
  iProcess( aProcess ),
  iTimeout( aTimeout ),
  iSsTestAsyncNotifier( aSsTestAsyncNotifier ),
  iDuff( aDuffBehaviour )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, 1000000 ); // TKAS May not need this delay.
	}



CTestAndCanceler::~CTestAndCanceler()
	{
	Cancel();
	iTimer.Close();
	delete iStartCanceler;
	}



/**
 StartCanceler
 */
//
CStartCanceler::CStartCanceler( TInt& aSsIndex, CSsmStartSafe& aSsmStartSafe, TInt aTimeout )
: CActive( EPriorityStandard ), iSsmStartSafe( aSsmStartSafe ), iSsIndex( aSsIndex ), iTimeout( aTimeout )
	{
	CActiveScheduler::Add( this );
	SetActive();
	iTimer.CreateLocal();
	iTimer.After( iStatus, iTimeout );
	}



void CStartCanceler::RunL()
	{
	iSsmStartSafe.StartCancel( iSsIndex );
	}


/**
 StartAcknowledge 
 */
//
CStartAcknowledge::CStartAcknowledge( MSsTestAsyncNotifier* aSsTestAsyncNotifier )
:CActive( EPriorityStandard), iSsTestAsyncNotifier( aSsTestAsyncNotifier )
	{
	CActiveScheduler::Add( this );
	SetActive();	
	}



CStartAcknowledge::~CStartAcknowledge()
	{
	Cancel();
	}



void CStartAcknowledge::RunL()
	{
	iSsTestAsyncNotifier->SsTestNotify( iStatus.Int() );
	}
