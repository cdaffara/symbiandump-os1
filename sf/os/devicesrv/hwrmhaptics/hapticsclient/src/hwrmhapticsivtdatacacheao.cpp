/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of haptics client's IVT-data cache's internal
*                Active Object.
*
*/


#include "hwrmhapticsivtdatacache.h"
#include "hwrmhapticsivtdatacacheao.h"
#include "hwrmhapticssession.h"
#include "hwrmhapticsclientserver.h"

// ---------------------------------------------------------------------------
// Two-phase constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTDataCacheAO* CHWRMHapticsIVTDataCacheAO::NewL(
        TInt aFileHandle, 
        CHWRMHapticsIVTDataCache* aCache,
        TRequestStatus& aStatus )
    {        
    CHWRMHapticsIVTDataCacheAO* self =
        CHWRMHapticsIVTDataCacheAO::NewLC( aFileHandle, aCache, aStatus );
    CleanupStack::Pop( self );

    return self;      
    }
    
// ---------------------------------------------------------------------------
// Two-phase asynchronous constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTDataCacheAO* CHWRMHapticsIVTDataCacheAO::NewLC(
        TInt aFileHandle, 
        CHWRMHapticsIVTDataCache* aCache,
        TRequestStatus& aStatus )
    {
    CHWRMHapticsIVTDataCacheAO* self = 
        new ( ELeave ) CHWRMHapticsIVTDataCacheAO( 
            aFileHandle, aCache, aStatus );
    CleanupStack::PushL( self );
    
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTDataCacheAO::~CHWRMHapticsIVTDataCacheAO()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Makes the async play call using this AO's own status
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCacheAO::PlayEffectAsynch( 
        RHWRMHapticsSession* aClientSession,
        const TIpcArgs& aArgs )
    {
    aClientSession->ExecuteAsyncOperation( EHWRMHapticsPlayEffect,
                                           aArgs,
                                           iStatus );
    SetActive();                                       
    }

// ---------------------------------------------------------------------------
// RunL for this active object.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCacheAO::RunL()
    {
    TInt status( iStatus.Int() );
    if ( KErrNone == status )
        {
        iCache->UpdateCacheItem( iFileHandle, ETrue );
        }

    User::RequestComplete( iClientStatus, status );
    
    iCache->RequestGarbageCollection();
    }
    
// ---------------------------------------------------------------------------
// DoCancel for this active object.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCacheAO::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// DoCancel for this active object.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTDataCacheAO::RunError( TInt /* aError */ )
    {   
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTDataCacheAO::CHWRMHapticsIVTDataCacheAO(
        TInt aFileHandle, 
        CHWRMHapticsIVTDataCache* aCache, 
        TRequestStatus& aStatus )
    : CActive( EPriorityStandard ), 
      iFileHandle( aFileHandle ),
      iCache( aCache ), 
      iClientStatus( &aStatus )
    {
    aStatus = KRequestPending;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTDataCacheAO::ConstructL()
    {
    }

//  End of File  
