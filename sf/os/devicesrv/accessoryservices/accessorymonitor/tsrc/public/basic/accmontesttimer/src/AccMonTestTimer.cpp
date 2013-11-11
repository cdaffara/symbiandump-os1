/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "AccMonTestTimer.h"
#include <e32uid.h>

// construct/destruct

EXPORT_C CAccMonTestTimer* CAccMonTestTimer::NewLC()
	{
	CAccMonTestTimer* self = new (ELeave) CAccMonTestTimer();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CAccMonTestTimer::~CAccMonTestTimer() // destruct - virtual, so no export
	{
	Cancel();
	iTimer.Close();
	}


CAccMonTestTimer::CAccMonTestTimer() : CActive( CActive::EPriorityStandard )
	{
	TInt ret( KErrNone );
	ret = iTimer.CreateLocal();
	CActiveScheduler::Add( this );
	}

void CAccMonTestTimer::ConstructL() // second-phase constructor
	{
    }

EXPORT_C void CAccMonTestTimer::WairForSecondsTest( TInt aSeconds )
	{
	if( !IsActive() )
		{
		TTimeIntervalMicroSeconds32 interval( aSeconds );
		iTimer.After( iStatus, interval );
		SetActive();
		CActiveScheduler::Start();
		}
	}

void CAccMonTestTimer::RunL()
	{
	CActiveScheduler::Stop();
	}
	
void CAccMonTestTimer::DoCancel()
	{
	
	}

// End of file
