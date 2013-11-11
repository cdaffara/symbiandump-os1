// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "LcfCommonProcedures.h"

#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>


#define _INDIVIDUAL_PRIVACY_ON_ 0

const TInt KWatchdogTimer=120000000;

// ---------------------------------------------------------
// CLcfCommonProcedures::SetupGlobalPrivacyL
// (other items were commented in a header).
// ---------------------------------------------------------
//

CLcfCommonProcedures::CLcfCommonProcedures()
    {
    }

CLcfCommonProcedures::~CLcfCommonProcedures()
    {
    delete iIdle;
    delete iTimer;
    }

CLcfCommonProcedures* CLcfCommonProcedures::NewL()
    {
    CLcfCommonProcedures* self = new (ELeave) CLcfCommonProcedures();
    CleanupStack::PushL(self);
    
    self->ConstructL();
    
    CleanupStack::Pop(self);
    
    return self;
    }

void CLcfCommonProcedures::ConstructL()
    {
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
    iTimer = CPeriodic::NewL(CActive::EPriorityIdle);
    iTimer->Start(
        KWatchdogTimer,
        KWatchdogTimer, 
        TCallBack(TimerCallback, this));
    

    }
//
void CLcfCommonProcedures::FlushEventsL()
    {
    CIdle* idle = CIdle::NewL(CActive::EPriorityIdle);
    idle->Start(TCallBack(IdleCallback,this));
    if(!iWait.IsStarted())
    	{
    	iWait.Start();
    	}
    delete idle;
    
    iTimer->Cancel();
    iTimer->Start(
        KWatchdogTimer,
        KWatchdogTimer,
        TCallBack(TimerCallback, this));
    }

TInt CLcfCommonProcedures::IdleCallback(TAny* aThisObj)
    {
    TBool beIdle = ETrue;
    CLcfCommonProcedures* thisPtr = (CLcfCommonProcedures*)aThisObj;
    if (!beIdle)
        {
        return 1;
        }
    else 
        {
        thisPtr->StopWaiting();
        return 0;
        }
    }

void CLcfCommonProcedures::StopWaiting()
    {
    iWait.AsyncStop();
    }

void CLcfCommonProcedures::AsyncWait(TRequestStatus& aStatus)
    {
    for (;;)
        {
        FlushEvents();
        
        if (aStatus.Int() != KRequestPending)
            {
            User::WaitForRequest(aStatus);
            
            return;
            }
        }
    }
//
// Non leaving version of above function to be used in OOM
// situation.
void CLcfCommonProcedures::FlushEvents()
    {
    iIdle->Start(TCallBack(IdleCallback, this));
    iWait.Start();
    }

TInt CLcfCommonProcedures::TimerCallback(TAny* /*aAny*/)
    {
    return KErrNone;
    }


TInt CLcfCommonProcedures::GetHeapUsageL(TInt& aHeapSize)
    {
    TInt size = 0;
    TInt used = 0;
    TInt tmp;


    RHeap& heap = User::Heap();
    
    if (!heap.Base())
        {
        User::Leave(KErrGeneral);
        }

    size = heap.Size();
    used = heap.Size() - (heap.Available(tmp));

    aHeapSize = size;

    return used;
    }


// End of file
