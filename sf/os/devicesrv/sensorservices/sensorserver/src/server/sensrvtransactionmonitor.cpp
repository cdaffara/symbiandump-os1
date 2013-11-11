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
* Description:  Sensor server transaction monitor 
*
*/



#include "sensrvdefines.h"
#include "sensrvtransactionmonitor.h"
#include "sensrvtransactionqueue.h"
#include "sensrvtransaction.h"
#include "sensrvsession.h"
#include "sensrvtimer.h"
#include "sensrvpluginproxy.h"
#include "sensrvproxymanager.h"


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvTransactionMonitor* CSensrvTransactionMonitor::NewL(CSensrvPluginProxy& aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::NewL(aProxy: 0x%x)" ), &aProxy ) );

    CSensrvTransactionMonitor* self = new( ELeave ) CSensrvTransactionMonitor(aProxy);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvTransactionMonitor::CSensrvTransactionMonitor(CSensrvPluginProxy& aProxy)
    : iProxy(aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::CSensrvTransactionMonitor()" ) ) );

    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::CSensrvTransactionMonitor - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvTransactionMonitor::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::ConstructL()" ) ) );

    // Monitor doesn't own the transactions it monitors.
    iMonitorQueue = CSensrvTransactionQueue::NewL(EFalse);
    
    iTransactionTimer = CSensrvTimer::NewL(*this, 
                                           iProxy.ProxyManager().TransactionTimeout(),
                                           CSensrvTimer::ETimerIdTransactionTimer);
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::ConstructL - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvTransactionMonitor::~CSensrvTransactionMonitor()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::~CSensrvTransactionMonitor()" ) ) );

    delete iTransactionTimer;
    delete iMonitorQueue;
        
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::~CSensrvTransactionMonitor - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Adds transaction to monitoring
// ---------------------------------------------------------------------------
//
TInt CSensrvTransactionMonitor::AddTransaction(CSensrvTransaction* aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::AddTransaction" ) ) );
    TInt err(KErrNone);
    if( aTransaction )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::AddTransaction(aTransaction: 0x%x)" ), &aTransaction ) );
        err = iMonitorQueue->Add(aTransaction);
        
        if (err == KErrNone)
            {
            aTransaction->ResetTimeStamp();
    
            if (!iTransactionTimer->IsActive())
                {
                // Activate timer
                iTransactionTimer->Set(KSensrvTimerMaxTime);
                }
            }
        }
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::AddTransaction - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Remove transaction from monitoring
// ---------------------------------------------------------------------------
//
void CSensrvTransactionMonitor::RemoveTransaction(CSensrvTransaction* aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::RemoveTransaction" ) ) );

    if( aTransaction )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::RemoveTransaction(aTransaction: 0x%x)" ), aTransaction ) );
        iMonitorQueue->Remove(aTransaction);
        
        if (iMonitorQueue->IsEmpty())
            {
            iTransactionTimer->Cancel();
            }
        }
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::RemoveTransaction - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Remove all of session's transactions from monitoring.
// ---------------------------------------------------------------------------
//
void CSensrvTransactionMonitor::RemoveSessionTransactions(CSensrvSession* aSession)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::RemoveSessionTransactions" ) ) );

    if( aSession )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::RemoveSessionTransactions(aSession: 0x%x)" ), aSession ) );
        iMonitorQueue->Remove(aSession);
        
        if (iMonitorQueue->IsEmpty())
            {
            iTransactionTimer->Cancel();
            }
        }
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::RemoveSessionTransactions - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Remove all transactions from monitoring.
// ---------------------------------------------------------------------------
//
void CSensrvTransactionMonitor::RemoveAllTransactions()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::RemoveAllTransactions()" )) );

    iMonitorQueue->RemoveAll();
    iTransactionTimer->Cancel();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::RemoveAllTransactions - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Check for obsolete transactions
// ---------------------------------------------------------------------------
//
void CSensrvTransactionMonitor::TimerFired(TInt /*aTimerId*/)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::TimerFired()" ) ) );

    iProxy.Mutex().Wait();
    
    TTime cutoffTime;
    cutoffTime.HomeTime();
    cutoffTime -= iProxy.ProxyManager().TransactionTimeout();

    TInt count(iMonitorQueue->RemoveObsolete(cutoffTime));

    // Reset/Cancel timer
    if (iMonitorQueue->IsEmpty())
        {
        iTransactionTimer->Cancel();
        }
    else
        {
        iTransactionTimer->Set(KSensrvTimerMaxTime);
        }
        
    // Handle obsolete transactions
    if (count)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::TimerFired - Some transactions timeoutted! (count: %d)" ), count ) );
        iProxy.HandleTransactionTimeout();
        }

    iProxy.Mutex().Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionMonitor::TimerFired - return " ) ) );
    }


