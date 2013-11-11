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
* Description:  Event table and compensation controller
*
*/


// INCLUDE FILES
#include <e32debug.h>
#include <sensordatacompensatorplugintypes.h>
#include <sensordatacompensatorplugin.h>
#include "sensordatacompensatorpluginstub.h"
#include "sensordatacompensatortesttypes.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorPluginStub::CSensorDataCompensatorPluginStub(
    TSensrvChannelDataTypeId aDataType,
    TSensorCompensationType aType,
    MSensorDataCompensatorObserver& aObserver ) :
    iDataType( aDataType ),
    iType( aType ),
    iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorPluginStub::~CSensorDataCompensatorPluginStub()
    {
    delete iPsWatcherDataAppend;
    delete iPsWatcherDataReset;
    delete iPsWatcherReturnErrorOnDataUpdate;
    iDataArray.ResetAndDestroy();
    RDebug::Printf("[SensDataCompTest] Stub deleted");
    }

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorPluginStub* CSensorDataCompensatorPluginStub::NewL(
    TSensrvChannelDataTypeId aDataType,
    TSensorCompensationType aType,
    MSensorDataCompensatorObserver& aObserver )
    {
    CSensorDataCompensatorPluginStub* self =
        new (ELeave) CSensorDataCompensatorPluginStub( aDataType, aType, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensorDataCompensatorPluginStub::ConstructL()
    {
    TInt val( 0 );
    TInt err = RProperty::Get( KTestPSUid, ETestKeyPluginAction, val );
    if ( err == KErrNone && val == ETestPluginFailCreation )
        {
        RDebug::Printf("[SensDataCompTest] Stub creation failure requested");
        User::Leave( KErrGeneral );
        }

    iPsWatcherDataAppend = CPsWatcher::NewL(*this,KTestPSUid,ETestKeyPluginDataAppend);
    iPsWatcherDataReset = CPsWatcher::NewL(*this,KTestPSUid,ETestKeyPluginAction);
    iPsWatcherReturnErrorOnDataUpdate = CPsWatcher::NewL(*this,KTestPSUid,ETestPluginReturnErrorOnDataUpdate);
    
    RDebug::Printf("[SensDataCompTest] Stub constructed (datatype 0x%x type %d)", iDataType, iType);
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::GetCompensationItemCount
// ---------------------------------------------------------------------------
//
TInt CSensorDataCompensatorPluginStub::GetCompensationItemCount()
    {
    if ( iErrorOnUpdate != KErrNone )
        {
        return iErrorOnUpdate;
        }
    return iDataArray.Count();
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::GetCompensationItem
// ---------------------------------------------------------------------------
//
TPtrC8 CSensorDataCompensatorPluginStub::GetCompensationItem( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iDataArray.Count())
        {
        return iDataArray[aIndex]->Des();
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::PropertyChangedL
// ---------------------------------------------------------------------------
//
void CSensorDataCompensatorPluginStub::PropertyChangedL( CPsWatcher& aWatcher )
    {
    switch(aWatcher.Key())
        {
        case ETestKeyPluginDataAppend:
            {
            HBufC8* buffer = HBufC8::NewLC(KTestMaxDataSize);
            TPtr8 ptr(buffer->Des());
            aWatcher.Get(ptr);
            iDataArray.AppendL(buffer);
            CleanupStack::Pop(buffer);
            iObserver.CompensationValuesUpdated();
            RDebug::Printf("[SensDataCompTest] Stub data appended (%d total items)", iDataArray.Count());
            break;
            }
        case ETestKeyPluginAction:
            {
            TInt value(0);
            aWatcher.Get(value);
            if ( value == ETestPluginResetData )
                {
                iDataArray.ResetAndDestroy();
                iErrorOnUpdate = KErrNone;
                RDebug::Printf("[SensDataCompTest] Stub data reset");
                }
            break;
            }
        case ETestPluginReturnErrorOnDataUpdate:
            {
            aWatcher.Get(iErrorOnUpdate);
            RDebug::Printf("[SensDataCompTest] Error on update %d", iErrorOnUpdate);
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::CPsWatcher::CPsWatcher
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorPluginStub::CPsWatcher::CPsWatcher(
        CSensorDataCompensatorPluginStub& aObserver, const TUid& aUid, TInt aKey) :
    CActive(CActive::EPriorityStandard),
    iObserver(aObserver),
    iUid(aUid),
    iKey(aKey)
    {
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::CPsWatcher::NewL
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorPluginStub::CPsWatcher* CSensorDataCompensatorPluginStub::CPsWatcher::NewL(
        CSensorDataCompensatorPluginStub& aObserver, const TUid& aUid, TInt aKey)
    {
    CPsWatcher* self = new (ELeave) CPsWatcher(aObserver,aUid,aKey);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::CPsWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CSensorDataCompensatorPluginStub::CPsWatcher::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iProperty.Attach(iUid,iKey));
    iProperty.Subscribe(iStatus);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::CPsWatcher::~CPsWatcher
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorPluginStub::CPsWatcher::~CPsWatcher()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::CPsWatcher::DoCancel
// ---------------------------------------------------------------------------
//
void CSensorDataCompensatorPluginStub::CPsWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorPluginStub::CPsWatcher::RunL
// ---------------------------------------------------------------------------
//
void CSensorDataCompensatorPluginStub::CPsWatcher::RunL()
    {
    iProperty.Subscribe(iStatus);
    SetActive();
    iObserver.PropertyChangedL(*this);
    }
