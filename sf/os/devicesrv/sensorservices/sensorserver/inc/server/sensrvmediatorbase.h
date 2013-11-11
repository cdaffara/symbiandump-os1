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


#ifndef SENSRVMEDIATORBASE_H
#define SENSRVMEDIATORBASE_H

#include <e32base.h>

class CSensrvPluginProxy;
class CSensrvTransaction;
class CSensrvTransactionQueue; 
class CSensrvSession; 
    
/**
* Mediates control flow between sensor server main thread (aka server thread)
* and the plugin threads. Each plugin has one mediator for server thread 
* and one mediator for plugin thread, and control is passed
* to different thread by notifying the mediator of that thread.
*
* @since S60 5.0
*/
class CSensrvMediatorBase : public CActive
    {
    public:
        
        /**
        * Destructor.
        */
        virtual ~CSensrvMediatorBase();
        
        /**
         * Used to destruct object without calling destructor.
         */
        void BaseDestruct();

        /**
        * Notifies the mediator that new transaction needs handling
        * by the thread mediated by this mediator.
        * Called from any thread.
        *
        * @since S60 5.0
        * @param aTransaction The new transaction to be handled. 
        *        If transaction is NULL, Notify() is guaranteed to
        *        return KErrNone.
        * @return KErrNoMemory Out of memory
        */
        TInt Notify(CSensrvTransaction* aTransaction);
        
        /**
        * Initializes the mediator.
        * Must be called from thread this mediator mediates.
        * If child class overloads this, base implementation must
        * be called also.
        *
        * @since S60 5.0
        * @return Symbian error code
        */
        virtual TInt Initialize();

        /**
        * Removes all transactions initiated by specified session from queue.
        * This method is only called from server thread.
        *
        * @since S60 5.0
        * @param aSession The session which is being deleted.
        */
        void SessionTerminated(CSensrvSession* aSession);

        /**
        * Removes all transactions from queue.
        * This method is only called from server thread.
        *
        * @since S60 5.0
        */
        void RemoveAllTransactions();

        /**
        * Removes single transaction from queue.
        * This method is only called from server thread.
        *
        * @since S60 5.0
        */
        void RemoveSingleTransaction(CSensrvTransaction* aTransaction);

    protected:

        // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();
        
        /**
        * C++ constructor
        *
        * @since S60 5.0
        */
        CSensrvMediatorBase();

        /**
        * 2nd phase of construction.
        * This method must be called during child class construction.
        * @param aProxy The proxy controlling related plugin.
        *
        * @since S60 5.0
        */
        void BaseConstructL(CSensrvPluginProxy* aProxy);

        /**
        * Handles transaction. Handling depends on mediator type,
        * so this must be implemented in inheriting class.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to be handled.
        */
        virtual void HandleTransaction(CSensrvTransaction* aTransaction) = 0;
        
        /**
        * Handles notify failure
        *
        * @since S60 5.0
        */
        virtual void HandleNotifyFailure() = 0;

        /**
        * Completes this object's request, if it is active and notifications are 
        * enabled.
        *
        * @since S60 5.0
        * @param aReason Reason of completion
        */
        void CompleteRequest(TInt aReason);

    protected: // Member variables
    
        /**
        * Parent proxy of this mediator.
        * Not own.
        */
        CSensrvPluginProxy* iProxy;

        /**
        * Thread handle for owning thread
        */
        RThread iThread;

#if defined (COMPONENT_TRACE_FLAG) || defined(ERROR_TRACE_FLAG)
        /**
        * Mediated thread name for debugging 
        * (to suppress "not a lvalue" warnings that come when using iThread.Name() in RDebug)
        */
        TName iThreadName;
#endif
        
        /**
        * Mutex pointer. 
        * If mediator is created in server main thread, this
        * points to proxy's mutex, otherwise it is a duplicate handle.
        *
        * Must be initialized and cleaned by child class.
        */
        RMutex* iMutex;

        /** 
        * Indicates if this object has already been initialized
        */
        TBool iInitialized;
        
        /**
        * Holds transaction pointers of transactions 
        * queued at this mediator.
        * Own.
        */
        CSensrvTransactionQueue* iQueue;

    private:


        /**
        * Indicates that at least one notify has failed
        */
        TBool iNotifyFailed;
    };



#endif // SENSRVMEDIATORBASE_H
