/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Listener for PS key changes
*
*/



#include "siftrace.h"
#include "ssypslistener.h"

_LIT( KPanicCategory, "SsyPsListener" );

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSsyPsListener* CSsyPsListener::NewL(MSsyPsObserver& aObserver,
                                     TUid aCategory, 
                                     TUint aKey)
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::NewL(0x%x)" ), &aObserver ) );

    CSsyPsListener* self = new (ELeave) CSsyPsListener(aObserver, aCategory, aKey);
    CleanupStack::PushL(self);
    self->ConstructL(); 
    CleanupStack::Pop(self); 

    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::NewL - return 0x%x" ), self ) );

    return self;
    }
    
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSsyPsListener::CSsyPsListener(MSsyPsObserver& aObserver,
                               TUid aCategory, 
                               TUint aKey)
    : CActive(EPriorityHigh),
      iObserver(aObserver),
      iCategory(aCategory),
      iKey(aKey)
    {
    // Nothing to do
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction.
// ---------------------------------------------------------------------------
//
void CSsyPsListener::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::ConstructL()" ) ) );   

    CActiveScheduler::Add(this);

    // Start listening specified PS key
    User::LeaveIfError(iProperty.Attach(iCategory, iKey));
    
    iProperty.Subscribe(iStatus);
    SetActive();

    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSsyPsListener::~CSsyPsListener()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::~CSsyPsListener()" ) ) );
    
    Cancel(); 

    iProperty.Close();
    
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::~CSsyPsListener - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Handle notification from the other thread.
// -----------------------------------------------------------------------------
//
void CSsyPsListener::RunL()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::RunL()" ) ) );
    
    TInt err(KErrNone);
    
    // Restart listening
    iProperty.Subscribe(iStatus);
    SetActive();

    // Get value
    err = iProperty.Get(iValue);

    if (err == KErrNone)
        {
        // Everything ok, notify owner about new plugins
        COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::RunL() - Notifying observer: iCategory: %d, iKey: %d, iValue: %d" ), iCategory, iKey, iValue ) );
        iObserver.PsValueSet(iCategory, iKey, iValue);
        }
    
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::RunL - return" ) ) );
    }
    
// -----------------------------------------------------------------------------
// Handle error in RunL
// -----------------------------------------------------------------------------
//
#ifdef COMPONENT_TRACE_DEBUG
TInt CSsyPsListener::RunError(TInt aError)
#else
TInt CSsyPsListener::RunError(TInt /*aError*/)
#endif
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::RunError(%d)" ), aError) );

    // Panic this thread if there is unhandled error in RunL.
    // This should not be possible.
    User::Panic(KPanicCategory, 0);
    
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::RunError - return %d" ), KErrNone ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CSsyPsListener::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::DoCancel()" ) ) );

    iProperty.Cancel();

    COMPONENT_TRACE( ( _L( "SSYStub - CSsyPsListener::DoCancel - return" ) ) );
    }

