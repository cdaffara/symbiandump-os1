/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mediates control flow from plugin thread to server main thread
*
*/


#include "sensrvdefines.h"
#include "sensrvservermediator.h"
#include "sensrvpluginproxy.h"
#include "sensrvtransaction.h"
#include "sensrvchannel.h"

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvServerMediator* CSensrvServerMediator::NewL(CSensrvPluginProxy* aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::NewL(aProxy: 0x%x)" ), aProxy->ImplementationUid().iUid ) );

    __ASSERT_ALWAYS(aProxy, User::Panic(KSensrvPanicCategory, ESensrvPanicNullProxy));

    CSensrvServerMediator* self = new( ELeave ) CSensrvServerMediator();
    
    CleanupStack::PushL( self );
    self->BaseConstructL(aProxy);
    self->ConstructL(aProxy);
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvServerMediator::CSensrvServerMediator()
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvServerMediator::ConstructL(CSensrvPluginProxy* aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::ConstructL()" ) ) );

    // Init mutex pointer
    iMutex = &aProxy->Mutex();
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvServerMediator::~CSensrvServerMediator()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::~CSensrvServerMediator()" ) ) );

    // Nothing to do 

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::~CSensrvServerMediator - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Handles transcation
// ---------------------------------------------------------------------------
//
void CSensrvServerMediator::HandleTransaction(CSensrvTransaction* aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::HandleTransaction(aTransaction: 0x%x), Mediated thread: %S" ), aTransaction, &iThreadName ) );
    
    // Notify proxy that transaction has been handled by SSY
    iProxy->TransactionHandledAtSsy(aTransaction);
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::HandleTransaction - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Handles notification failure
// ---------------------------------------------------------------------------
//
void CSensrvServerMediator::HandleNotifyFailure()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::HandleNotifyFailure()" ) ) );

    iProxy->HandleSsyNotifyFailure();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServerMediator::HandleNotifyFailure - return" ) ) );
    }
    
