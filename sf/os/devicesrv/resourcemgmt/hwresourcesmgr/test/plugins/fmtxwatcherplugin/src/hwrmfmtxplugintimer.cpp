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



#include <hwrmpluginservice.h>
#include <hwrmvibracommands.h>
#include "hwrmfmtxplugintimer.h"
#include "trace.h"

// -----------------------------------------------------------------------------
// CHWRMFmtxPluginTimer::NewL
// -----------------------------------------------------------------------------
//
CHWRMFmtxPluginTimer* CHWRMFmtxPluginTimer::NewL( TInt aCommandId,
                                                  TUint8 aTransId, 
                                                  TInt aRetVal,
                                                  MHWRMFmtxPluginTimerCallback& aCallback )
    {
    FUNC_LOG;
    
    CHWRMFmtxPluginTimer* self = new( ELeave )CHWRMFmtxPluginTimer( aCommandId, 
                                                                    aTransId, 
                                                                    aRetVal, 
                                                                    aCallback );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxPluginTimer::TransId
// -----------------------------------------------------------------------------
//
TUint8 CHWRMFmtxPluginTimer::TransId() const
    {
    FUNC_LOG;

    return iTransId;
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxPluginTimer::~CHWRMFmtxPluginTimer
// -----------------------------------------------------------------------------
//
CHWRMFmtxPluginTimer::~CHWRMFmtxPluginTimer()    
    {
    FUNC_LOG;

    Cancel();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxPluginTimer::CHWRMFmtxPluginTimer
// -----------------------------------------------------------------------------
//
CHWRMFmtxPluginTimer::CHWRMFmtxPluginTimer( TInt aCommandId, TUint8 aTransId,
                            TInt aRetVal, MHWRMFmtxPluginTimerCallback& aCallback)
    : CTimer( EPriorityStandard ),
    iCommandId(aCommandId),
    iTransId(aTransId),
    iRetVal(aRetVal),
    iCallback(aCallback)
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxPluginTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxPluginTimer::ConstructL()
    {
    FUNC_LOG;

    CTimer::ConstructL();
    
    CActiveScheduler::Add( this );
    
    After(0);
    }

    
// -----------------------------------------------------------------------------
// CHWRMFmtxPluginTimer::RunL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxPluginTimer::RunL()
    {
    FUNC_LOG;

    iCallback.GenericTimerFired( iCommandId, iTransId, iRetVal );
    }
    
