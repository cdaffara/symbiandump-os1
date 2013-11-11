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

#include "endstatetimer.h"

CEndStateTimer* CEndStateTimer::NewL(MTimerExpired& aObserver)
    {
    CEndStateTimer* self = new( ELeave )CEndStateTimer(aObserver);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

CEndStateTimer::~CEndStateTimer()    
    {
    Cancel();
    }
    
CEndStateTimer::CEndStateTimer(MTimerExpired& aObserver)
    : CTimer(EPriorityStandard), iTimerObserver(aObserver)  
    {
    }

void CEndStateTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }
    
void CEndStateTimer::Start(TReal aTimeOutSecs)
	{
	After(aTimeOutSecs*1000000);
	}
	
void CEndStateTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

    
void CEndStateTimer::RunL()
    {
    iTimerObserver.EndStateTimerExpired(iStatus.Int());	
    }
