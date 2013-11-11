/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Listener for new ECom plugins
*
*/



#include "sensrvdefines.h"
#include "sensrvecomchangelistener.h"
#include "ssycontrol.h"


// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvEcomChangeListener* CSensrvEcomChangeListener::NewL(MSensrvEcomChangeObserver& aObserver)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::NewL(0x%x)" ), &aObserver ) );

    CSensrvEcomChangeListener* self = new (ELeave) CSensrvEcomChangeListener(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvEcomChangeListener::CSensrvEcomChangeListener(MSensrvEcomChangeObserver& aObserver)
    : CActive(EPriorityHigh),
      iObserver(aObserver)
    {
    // Nothing to do
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction.
// ---------------------------------------------------------------------------
//
void CSensrvEcomChangeListener::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::ConstructL()" ) ) );

    CActiveScheduler::Add(this);

    // Start listening to ecom
    iEcom = &(REComSession::OpenL());

    // Create retry timer
    iTimer = CSensrvEcomChangeListenerTimer::NewL( this );

    // Execute initial check for plugins immediately.
    // Plugins must be solved before clients start using the server after connect.
    CheckEcomChanges();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvEcomChangeListener::~CSensrvEcomChangeListener()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::~CSensrvEcomChangeListener()" ) ) );

    Cancel();

    if (iEcom)
        {
        iEcom->Close();
        }

    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::~CSensrvEcomChangeListener - return" ) ) );
    }


// ---------------------------------------------------------------------------
// TimerCallBack
// ---------------------------------------------------------------------------
//
void CSensrvEcomChangeListener::TimerCallBack()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::TimerCallBack()" ) ) );

    CheckEcomChanges();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::TimerCallBack - return" ) ) );
    }


// ---------------------------------------------------------------------------
// CheckEcomChanges
// ---------------------------------------------------------------------------
//
void CSensrvEcomChangeListener::CheckEcomChanges()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::CheckEcomChanges()" ) ) );

    TBool changeOk( ETrue );
    TInt err( KErrNone );

    if ( !IsActive() ) //Not active
        {
        iEcom->NotifyOnChange(iStatus);

        iTimer->Cancel();

        SetActive();
        }

    // Scan for SSY plugins
    RImplInfoPtrArray* implInfoArray = NULL;
    implInfoArray = new RImplInfoPtrArray();
    CleanupStack::PushL( implInfoArray );
    
    if ( implInfoArray )
        {
        TRAP(err, REComSession::ListImplementationsL( TUid::Uid(KSsyControlInterfaceUid),
                                                       *implInfoArray));
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::RunL() - ERROR: Allocating implInfoArray failed" ) ) );
        err = KErrNoMemory;
        }

    // If listing implementations fails, ecom may be busy constructing the index.
    // Retry after one second in that case.
    if ( err == KErrNone )
        {
        // Everything ok, notify owner about new plugins
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::RunL() - Notifying observer" ) ) );
        changeOk = iObserver.EcomChanged(implInfoArray);

        // Do not destroy implementation infos, EcomChanged cleans array up if change is ok,
        // and otherwise they will get cleaned up below
        }

    CleanupStack::Pop( implInfoArray );
    if ( err != KErrNone || !changeOk )
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::RunL() - ERROR: Scanning for plugins failed or server could not handle new plugins just now, starting retry timer" ) ) );

        if ( implInfoArray )
            {
            implInfoArray->ResetAndDestroy();
            delete implInfoArray;
            }

        // Start rescan timer
        iTimer->StartTimer();
        }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::CheckEcomChanges - return" ) ) );
    }


// -----------------------------------------------------------------------------
// Handle notification from the other thread.
// -----------------------------------------------------------------------------
//
void CSensrvEcomChangeListener::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::RunL()" ) ) );

    CheckEcomChanges();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::RunL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Handle error in RunL
// -----------------------------------------------------------------------------
//
#ifdef ERROR_TRACE_DEBUG
TInt CSensrvEcomChangeListener::RunError(TInt aError)
#else
TInt CSensrvEcomChangeListener::RunError(TInt /*aError*/)
#endif
    {
    ERROR_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::RunError(%d)" ), aError) );

    // Panic this thread if there is unhandled error in RunL.
    // This should not be possible.
    User::Panic(KSensrvPanicCategory, ESensrvPanicEcomChangeRunError);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::RunError - return %d" ), KErrNone ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CSensrvEcomChangeListener::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::DoCancel()" ) ) );

    iEcom->CancelNotifyOnChange(iStatus);
    iTimer->Cancel();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListener::DoCancel - return" ) ) );
    }

