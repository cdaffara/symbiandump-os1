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
* Description:  Transaction queue implementation
*
*/


#ifndef SENSRVTRANSACTIONQUEUE_H
#define SENSRVTRANSACTIONQUEUE_H

#include <e32base.h>

class CSensrvTransaction;
class CSensrvSession;

/**
 * Simple to use transaction queue.
 *
 * @since S60 5.0
 */
class CSensrvTransactionQueue : public CBase
    {
    public:
        
        /**
        * Removal type specifies what should be done to removed transaction.
        */
        enum TRemovalType
            {
            ERemovalTypeUndefined, ///< Only valid removal type for non-owning queues
            ERemovalTypeComplete,  ///< Completes and deletes transaction
            ERemovalTypeCancel,    ///< Completes with KErrCancel and deletes transaction
            ERemovalTypeTransfer   ///< Transaction is just removed, not deleted or completed
            };
        
        /**
        * Linkable transaction pointer.
        */
        class TLinkableTransactionPtr
            {
            public:
                TLinkableTransactionPtr()
                    {
                    }

                /**
                * Link item for queueing transaction pointers
                */
                TSglQueLink iLink;
                
                /**
                * Transaction pointer
                * Not owned.
                */
                CSensrvTransaction* iTransaction;
            };


        /**
        * Two phase constructor.
        *
        * @since S60 5.0
        * @param aOwned Indicates if queue owns transactions or is
        *        it used simply to store the pointers.
        *        If transactions are owned, deleting queue will
        *        also delete all transactions.
        * @return New CSensrvTransactionQueue instance
        * @exception KErrNoMemory Out of memory.
        */
        static CSensrvTransactionQueue* NewL(TBool aOwned);

        /**
        * Destructor.
        */
        virtual ~CSensrvTransactionQueue();

        /**
        * Adds transaction to the the queue
        *
        * @since S60 5.0
        * @param aTransaction The transaction to be added.
        * @param aLast If ETrue, transaction is added to the end of
        *        the queue, otherwise it is added to the beginning.
        * @return KErrNoMemory: Out of memory.
        *         KErrArgument: NULL transaction.
        */
        TInt Add(CSensrvTransaction* aTransaction, TBool aLast = ETrue);

        /**
        * Removes specified transaction from queue, if it is there.
        * If queue owns the transactions, the removed transaction
        * may be completed and deleted, depending on removal type. 
        *
        * @since S60 5.0
        * @param aTransaction The transaction to be removed.
        * @param aRemovalType Determines removal type.
        * @see TRemovalType
        */
        void Remove(CSensrvTransaction* aTransaction,
                    TRemovalType aRemovalType = ERemovalTypeUndefined);

        /**
        * Removes all transactions initiated by specified session
        * from queue.
        * If queue owns the transactions, the transactions
        * are completed with KErrCancel and are deleted. Otherwise 
        * does not delete or complete transactions.
        *
        * @since S60 5.0
        * @param aSession The session to match.
        */
        void Remove(CSensrvSession* aSession);

        /**
        * Removes all transactions with timestamp older than
        * specified cutoff time. 
        *
        * @since S60 5.0
        * @param aCutoffTime Transactions older that this time
        *        will be removed.
        * @return Number of transactions removed.
        */
        TInt RemoveObsolete(const TTime& aCutoffTime);

        /**
        * Removes all transactions.
        *
        * @since S60 5.0
        */
        void RemoveAll();

        /**
        * Gets pointer to first transaction in queue.
        * Transaction stays in the queue.
        *
        * @since S60 5.0
        * @return Pointer to first transaction in queue or NULL if empty.
        */
        CSensrvTransaction* First();
        
        /**
        * Checks if queue is empty
        *
        * @since S60 5.0
        * @return ETrue if queue is empty.
        */
        inline TBool IsEmpty() {return iTransactionPtrList.IsEmpty(); };
        

    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        * @param aOwner Indicates if queue owns transactions or is
        *        it used simply to store the pointers.
        *        Deleting queue will delete all transactions if transactions
        *        are owned.
        */
        CSensrvTransactionQueue(TBool aOwned);

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();

        /**
        * Gets pointer to the TLinkableTransactionPtr in queue
        * corresponding to specified transaction.
        * Transaction stays in the queue.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to be found.
        * @return Pointer to TLinkableTransactionPtr or NULL if not found.
        */
        TLinkableTransactionPtr* FindTransactionPtr(CSensrvTransaction* aTransaction);  
        
        /**
        * Removes linkable transaction pointer and deletes it. 
        * If transactions are owned, they are completed with KErrCancel
        * or whatever error code they currently have.
        *
        * @since S60 5.0
        * @param aPtr Linkable transaction pointer to remove 
        * @param aRemovalType Determines removal type.
        * @see TRemovalType
        */
        void RemovePtr(TLinkableTransactionPtr* aPtr, TRemovalType aRemovalType);
             

    private:
    
        /**
        * Holds transaction pointers of transactions.
        * Queued pointer objects are owned.
        */
        TSglQue<TLinkableTransactionPtr> iTransactionPtrList;

        /**
        * Iterator for transaction queue
        */        
        TSglQueIter<TLinkableTransactionPtr> iTransactionPtrIter;
        
        /**
        * Indicates whether or not this queue owns the transactions it holds.
        */
        TBool iOwned;
        
        /**
        * Heap where the queue resides. 
        * Transactions must reside in the same heap.
        * Not own.
        */
        RHeap* iHeap;
    };



#endif // SENSRVTRANSACTIONQUEUE_H
