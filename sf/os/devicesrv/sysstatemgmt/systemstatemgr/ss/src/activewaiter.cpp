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

#include "activewaiter.h"
#include "ssmdebug.h"

#ifdef _DEBUG
_LIT( KSsmActiveWaiter, "SsmaSsActiveWaiter" );
const TInt KSsmActiveWaiterPanic = 100;
#endif


CActiveWaiter::CActiveWaiter()
	: CActive( CActive::EPriorityStandard )
	{
	CActiveScheduler::Add( this );
	}
	


CActiveWaiter::~CActiveWaiter()
	{ 
	Cancel(); 
	}



void CActiveWaiter::WaitActive()
	{ 
	__ASSERT_DEBUG( !iScheduler.IsStarted(), User::Panic(KSsmActiveWaiter, KSsmActiveWaiterPanic) );
	SetActive();	
	iScheduler.Start();
	}



void CActiveWaiter::RunL()
	{
	if (iScheduler.IsStarted())
		{
		__ASSERT_DEBUG( iScheduler.CanStopNow(), User::Panic(KSsmActiveWaiter, KSsmActiveWaiterPanic) );

		iScheduler.AsyncStop();
		}
	else
		{
		__ASSERT_DEBUG( 0, User::Panic(KSsmActiveWaiter, KSsmActiveWaiterPanic) );
		}
	}



void CActiveWaiter::DoCancel()
	{
	}



TInt CActiveWaiter::RunError( TInt aError )
	{
	DEBUGPRINT2A("CActiveWaiter::RunError called with error %d", aError);

	// Current CActiveWaiter::RunL()'s implementation doesn't leave so we are just returning KErrNone.
	// In case further derived implementations of this RunL() leaves, then this method can be changed 
	// to handle the error properly.
	aError = KErrNone;
	return aError;
	}

