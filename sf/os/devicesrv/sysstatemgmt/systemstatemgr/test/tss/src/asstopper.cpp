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




#include "asstopper.h"
#include "tss_step_base.h"

/**
 A little AO which supplies its TRS and stops the AS when it is completed,
 allowing a pause before doing so.
*/
CAsStopper::CAsStopper( TInt aTimeout )
: CActive( EPriorityStandard ), iTimeout( aTimeout )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();
	}


/**
 Overload of constructor for notifications
 */
CAsStopper::CAsStopper( TInt aTimeout, MSsTestAsyncNotifier* aNotify )
: CActive( EPriorityStandard ), iTimeout( aTimeout ), iNotify( aNotify )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();	
	}



CAsStopper::~CAsStopper()
	{
	Cancel();
	iTimer.Close();
	}



TRequestStatus& CAsStopper::Trs()
	{
	return iStatus;
	}



TInt CAsStopper::CompletionCode()
	{
	return iCompletionCode;
	}



void CAsStopper::RunL()
	{
	if( !iRunning )
		{
		iRunning = ETrue;
		iCompletionCode = iStatus.Int();
		
		if( iNotify )
			{
			iNotify->SsTestNotify( iCompletionCode );
			}
		
		SetActive();
		iTimer.After( iStatus, iTimeout );
		}
	else
		{
		CActiveScheduler::Stop();
		}

	}



void CAsStopper::DoCancel()
	{
	iTimer.Close();
	}

//
/**
 Another little AO which allows a pause of aTimeout before stopping the sched.
*/
CAsPauseStopper::CAsPauseStopper( TInt aTimeout )
: CActive( EPriorityStandard ), iTimeout( aTimeout )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, iTimeout );
	}



CAsPauseStopper::~CAsPauseStopper()
	{
	Cancel();
	iTimer.Close();
	}



void CAsPauseStopper::RunL()
	{
	CActiveScheduler::Stop();
	}



void CAsPauseStopper::DoCancel()
	{
	iTimer.Close();
	}


//
/**
 AO which allows a pause of aTimeout before deleting the CSsmStartSafe object.
*/
CSsPauseDeleter::CSsPauseDeleter( TInt aTimeout, CSsmStartSafe* aSsmStartSafe )
: CActive( EPriorityStandard ), iTimeout( aTimeout ), iSsmStartSafe( aSsmStartSafe )
	{
	CActiveScheduler::Add( this );
	SetActive();
	
	iTimer.CreateLocal();
	iTimer.After( iStatus, iTimeout );
	}



CSsPauseDeleter::~CSsPauseDeleter()
	{
	Cancel();
	iTimer.Close();
	}



void CSsPauseDeleter::RunL()
	{
	delete iSsmStartSafe;
	}



void CSsPauseDeleter::DoCancel()
	{
	iTimer.Close();
	}


