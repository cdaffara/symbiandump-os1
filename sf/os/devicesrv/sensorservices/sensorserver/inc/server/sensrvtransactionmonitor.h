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
* Description:  Transaction monitor implementation
*
*/


#ifndef SENSRVTRANSACTIONMONITOR_H
#define SENSRVTRANSACTIONMONITOR_H

#include <e32base.h>
#include "sensrvtimer.h"

class CSensrvTransaction;
class CSensrvTransactionQueue;
class CSensrvSession;
class CSensrvTimer;
class CSensrvPluginProxy;

/**
 * Provides lifecycle monitoring service for transactions. 
 * Transaction timer is only running when there are active transactions.
 *
 * @since S60 5.0
 */
class CSensrvTransactionMonitor : public CBase,
                                  public MSensrvTimerCallback
    {
    public:

        /**
        * Two phase constructor.
        *
        * @since S60 5.0
        * @param aProxy Parent proxy.
        * @return New CSensrvTransactionMonitor instance
        * @exception KErrNoMemory Out of memory.
        */
        static CSensrvTransactionMonitor* NewL(CSensrvPluginProxy& aProxy);

        /**
        * Destructor.
        */
        virtual ~CSensrvTransactionMonitor();

        /**
        * Adds transaction to be monitored.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to be added.
        * @param aChannel The channel related to tranasction. 
        *        Can be NULL for proxy level transactions.
        * @return KErrNoMemory: Out of memory.
        *         KErrArgument: NULL transaction.
        */
        TInt AddTransaction(CSensrvTransaction* aTransaction);

        /**
        * Removes specified transaction from monitoring.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to be removed.
        */
        void RemoveTransaction(CSensrvTransaction* aTransaction);

        /**
        * Removes all transactions initiated by specified session
        * from monitoring.
        *
        * @since S60 5.0
        * @param aSession The session to match.
        */
        void RemoveSessionTransactions(CSensrvSession* aSession);

        /**
        * Removes all transactions.
        *
        * @since S60 5.0
        */
        void RemoveAllTransactions();

    // From base classes

        /** 
        * Called when the timer fires
        *
        * @since S60 5.0
        * @param aTimerId Timer identifier to id which timer fired.
        */
        void TimerFired(TInt aTimerId);
        
    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        * @param aProxy Parent proxy.
        */
        CSensrvTransactionMonitor(CSensrvPluginProxy& aProxy);

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();

    private:
    
        /**
        * Owning proxy.
        */
        CSensrvPluginProxy& iProxy;

        /**
        * Queue for monitored transactions.
        */
        CSensrvTransactionQueue* iMonitorQueue;

        /**
        * Transaction timer
        * Own.
        */
        CSensrvTimer* iTransactionTimer;
        
    };



#endif // SENSRVTRANSACTIONMONITOR_H
