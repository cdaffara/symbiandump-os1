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
* Description:  Base class for Active objects to mediate control flow
*                 between threads.
*
*/



#include "sensrvdefines.h"
#include "sensrvmediatorbase.h"
#include "sensrvpluginproxy.h"
#include "sensrvtransaction.h"
#include "sensrvtransactionqueue.h"
#include "sensrvsession.h"

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvMediatorBase::CSensrvMediatorBase()
    : CActive(EPriorityHigh)
    {
    // Nothing to do
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction.
// ---------------------------------------------------------------------------
//
void CSensrvMediatorBase::BaseConstructL(CSensrvPluginProxy* aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::BaseConstructL(aProxy: 0x%x)" ), aProxy->ImplementationUid().iUid ) );   
            
    iProxy = aProxy;
    iQueue = CSensrvTransactionQueue::NewL(EFalse);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::BaseConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvMediatorBase::~CSensrvMediatorBase()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::~CSensrvMediatorBase(), mediated thread: %S" ), &iThreadName ) );
    
    Cancel(); 
    
    BaseDestruct();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::~CSensrvMediatorBase - return" ) ) );
    }

// ---------------------------------------------------------------------------
// BaseDestruct
// ---------------------------------------------------------------------------
//
void CSensrvMediatorBase::BaseDestruct()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::BaseDestruct()" ) ) );
    
    // Empty the transaction link list and delete any mediator transactions in queue
    delete iQueue;
    iQueue = NULL;
    
    // iProxy not owned so not cleaned.
    
    iThread.Close();
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::BaseDestruct - return void" ) ) );
    }


// ---------------------------------------------------------------------------
// Adds transaction pointer to the end of queue and completes
// this mediator's request on the other thread, causing RunL to be executed
// there.
// ---------------------------------------------------------------------------
//
TInt CSensrvMediatorBase::Notify(CSensrvTransaction* aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::Notify(aTransaction: 0x%x), mediated thread: %S" ), aTransaction, &iThreadName ) );
        
    TInt err(KErrNone);

#ifndef __SENSRV_TEST_ALWAYS_FAIL_SERVER_MEDIATOR_NOTIFY
    // Add transaction pointer to queue, unless it is NULL
    if (aTransaction)
        {
        err = iQueue->Add(aTransaction);
        }
#else // !__SENSRV_TEST_ALWAYS_FAIL_SERVER_MEDIATOR_NOTIFY
    // Always fail adding transaction when on server mediator, unless it is NULL
    if (aTransaction)
        {
        if (iMutex == &iProxy->Mutex())
            {
            err = KErrNoMemory;
            }
        else
            {
            err = iQueue->Add(aTransaction);
            }
        }
#endif // !__SENSRV_TEST_ALWAYS_FAIL_SERVER_MEDIATOR_NOTIFY
    
    // If adding transaction failed, set notify failed state to indicate this.
    if (err != KErrNone)
        {
        iNotifyFailed = ETrue;
        aTransaction->SetState(CSensrvTransaction::ETransStateNotifyFailed);
        } 

    CompleteRequest(KErrNone);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::Notify - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Initializes mediator.
// Completion is not yet enabled. It will be enabled when scheduler has
// run at least one idle loop to ensure mediator request is not completed 
// before scheduler is activated.
// ---------------------------------------------------------------------------
//
TInt CSensrvMediatorBase::Initialize()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::Initialize()" ) ) );
    
    TInt err(KErrNone);

    if ( !iInitialized )
        {
        err = iThread.Open(iThread.Id());
        
        if (err == KErrNone)
            {
#if defined (COMPONENT_TRACE_FLAG) || defined(ERROR_TRACE_FLAG)
            iThreadName = iThread.Name();
#endif

            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::Initialize - mediated thread: %S" ), &iThreadName ) );

            // iMutex must be set by child class before calling Initialize
            __ASSERT_ALWAYS(iMutex, User::Panic(KSensrvPanicCategory, ESensrvPanicNullMutex));
            __ASSERT_ALWAYS(CActiveScheduler::Current()!= NULL, User::Panic(KSensrvPanicCategory, ESensrvPanicNullScheduler));

            CActiveScheduler::Add(this);
            iStatus = KRequestPending;
            SetActive();
            iInitialized = ETrue;
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::Initialize - ERROR: Mediator initialize failed" ) ) );
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::Initialize - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Removes session initiated transactions from queue
// ---------------------------------------------------------------------------
//
void CSensrvMediatorBase::SessionTerminated(CSensrvSession* aSession)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::SessionTerminated(), mediated thread: %S" ), &iThreadName ) );

    iQueue->Remove(aSession);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::SessionTerminated - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Removes all transactions from queue
// ---------------------------------------------------------------------------
//
void CSensrvMediatorBase::RemoveAllTransactions()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::RemoveAllTransactions(), mediated thread: %S" ), &iThreadName ) );

    iQueue->RemoveAll();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::RemoveAllTransactions - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Removes single transaction from queue
// ---------------------------------------------------------------------------
//
void CSensrvMediatorBase::RemoveSingleTransaction(CSensrvTransaction* aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::RemoveSingleTransaction(), mediated thread: %S" ), &iThreadName ) );

    iQueue->Remove(aTransaction);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::RemoveSingleTransaction - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Handle notification from the other thread.
// -----------------------------------------------------------------------------
//
void CSensrvMediatorBase::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::RunL(), mediated thread: %S" ), &iThreadName ) );
    
    // Check if there are any transactions in queue, and if so,
    // get the first one and request handling for it.
    iMutex->Wait();
    
    // Before handling any transactions, check notify failure
    if (iNotifyFailed)
        {
        HandleNotifyFailure();
        iNotifyFailed = EFalse;
        }
    else
        {
        CSensrvTransaction* transaction = iQueue->First();
                       
        // Handle the received transaction (even if it is NULL), 
        HandleTransaction(transaction);

        // Remove transaction from queue. Since mediator queues don't own transactions,
        // this does not yet complete it, unless it is a mediator transaction.
        if (transaction)
            {
            iQueue->Remove(transaction);
            }
        }
        
    // Reactivate 
    iStatus = KRequestPending;
    SetActive();

    // Complete right away if there are more transactions
    if (!iQueue->IsEmpty())
        {
        CompleteRequest(KErrNone);
        }

    // SSY mediator mutex will be destroyed already if this is cleanup activation
    if(iMutex)
        {
        iMutex->Signal();
        }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::RunL - return" ) ) );
    }
    
// -----------------------------------------------------------------------------
// Handle error in RunL
// -----------------------------------------------------------------------------
//
#ifdef COMPONENT_TRACE_DEBUG
TInt CSensrvMediatorBase::RunError(TInt aError)
#else
TInt CSensrvMediatorBase::RunError(TInt /*aError*/)
#endif
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::RunError(%d), mediated thread: %S" ), aError, &iThreadName) );

    // Panic this thread if there is unhandled error in RunL.
    // This should not be possible.
    User::Panic(KSensrvPanicCategory, ESensrvPanicMediatorRunError);
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::RunError - return %d" ), KErrNone ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CSensrvMediatorBase::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::DoCancel(), mediated thread: %S" ), &iThreadName ) );

    // Request needs to be completed with KErrCancel
    CompleteRequest(KErrCancel);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::DoCancel - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Completes request if active.
// If not active, there is probably some problem with thread mutexing,
// but we cannot complete in that case anyway.
// -----------------------------------------------------------------------------
//
void CSensrvMediatorBase::CompleteRequest(TInt aReason)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::CompleteRequest(aReason: %d), mediated thread: %S" ), aReason, &iThreadName ) );

    if (!IsActive())
        {
        // Mediator is not active, which means it is probably waiting for mutex at RunL, so no need to complete again.
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::CompleteRequest - Not active when attempted completion." ) ) );
        }
    else if (iStatus==KRequestPending)
        {
        // Cause RunL to be executed in the owner thread.    
        TRequestStatus* status = &iStatus;
        iThread.RequestComplete(status, aReason);
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::CompleteRequest - Completion done." ) ) );
        }
    else
        {
        // Already completed but not yet handled, do nothing.
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::CompleteRequest - Completion not necessary" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMediatorBase::CompleteRequest - return" ) ) );
    }


