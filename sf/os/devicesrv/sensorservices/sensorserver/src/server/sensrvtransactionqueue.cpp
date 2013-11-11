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
* Description:  Sensor server transaction queue.
*
*/



#include "sensrvdefines.h"
#include "sensrvtransactionqueue.h"
#include "sensrvtransaction.h"
#include "sensrvsession.h"

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvTransactionQueue* CSensrvTransactionQueue::NewL(TBool aOwned)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::NewL()" ) ) );

    CSensrvTransactionQueue* self = new( ELeave ) CSensrvTransactionQueue(aOwned);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvTransactionQueue::CSensrvTransactionQueue(TBool aOwned)
    : iTransactionPtrList(_FOFF(TLinkableTransactionPtr,iLink)),
      iTransactionPtrIter(iTransactionPtrList),
      iOwned(aOwned)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::CSensrvTransactionQueue()" ) ) );

    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::CSensrvTransactionQueue - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvTransactionQueue::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::ConstructL()" ) ) );

    iHeap = &User::Heap();
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::ConstructL - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvTransactionQueue::~CSensrvTransactionQueue()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::~CSensrvTransactionQueue()" ) ) );

    TLinkableTransactionPtr* ptr = NULL;
    
    while (!iTransactionPtrList.IsEmpty())
        {
        ptr = iTransactionPtrList.First();
        iTransactionPtrList.Remove(*ptr);
        CSensrvTransaction* transaction = ptr->iTransaction;
        
        // If transactions are owned, they are all deleted. 
        if (iOwned)
            {
            transaction->SetErrorCode(KErrCancel);
            transaction->Complete();
            // Transactions are always created in same heap as queue with regular new,
            // so regular delete is ok
            delete transaction;
            }
        
        // Pointers are allocated directly on heap, so free them directly also just to be on safe side.
        if (iHeap)
            {
            iHeap->Free(ptr);
            }
        }
       
    // Reset to clear memory used by the list
    iTransactionPtrList.Reset();
        
    // iHeap is not owned
        
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::~CSensrvTransactionQueue - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Adds transaction to the end of the queue
// ---------------------------------------------------------------------------
//
TInt CSensrvTransactionQueue::Add(CSensrvTransaction* aTransaction, TBool aLast)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Add(aTransaction: 0x%x, aLast: %d)" ), aTransaction, aLast ) );

    TInt err(KErrNone);

    if (aTransaction)
        {
        // Allocate linkable transaction pointer in same heap as queue
        TLinkableTransactionPtr* newPtr = reinterpret_cast<TLinkableTransactionPtr*>(iHeap->Alloc(sizeof(TLinkableTransactionPtr)));

        if (newPtr)
            {
            newPtr->iTransaction = aTransaction;
            if (aLast)
                {
                iTransactionPtrList.AddLast(*newPtr);
                }
            else
                {
                iTransactionPtrList.AddFirst(*newPtr);
                }
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Add - ERROR: No memory to add item" ) ) );
            err = KErrNoMemory;
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Add - ERROR: Attempted to add NULL transaction" ) ) );
        err = KErrArgument;
        }

#ifdef MEMORY_TRACE_DEBUG
    TInt count(0);
    if (!iTransactionPtrList.IsEmpty())
        {
        TLinkableTransactionPtr* ptr = NULL;
        iTransactionPtrIter.SetToFirst(); 
        while ((ptr = iTransactionPtrIter++) != NULL)
            {
            count++;
            // Do not print the entire queue if it has many items.
            if (count < 5)
                {
                MEMORY_TRACE( ( _L( "@TT: %d" ), ptr->iTransaction->Type() ) );
                }
            else if (count == 5)
                {
                MEMORY_TRACE( ( _L( "@..." ) ) );
                }
            };
        }
    MEMORY_TRACE( ( _L( "@Queue count %d" ), count ) );
#endif

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Add - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Remove transaction from queue
// ---------------------------------------------------------------------------
//
void CSensrvTransactionQueue::Remove(CSensrvTransaction* aTransaction,
                                     TRemovalType aRemovalType)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Remove(aTransaction: 0x%x, %d)" ), aTransaction, aRemovalType ) );

    if (aTransaction)
        {
        TLinkableTransactionPtr* ptr = FindTransactionPtr(aTransaction);
        
        if (ptr)
            {
            RemovePtr(ptr, aRemovalType);
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Remove - Attempted to remove nonexisting transaction" ) ) );
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Remove - Attempted to remove NULL transaction" ) ) );
        }
    }

// ---------------------------------------------------------------------------
// Remove all of session's transactions from queue.
// ---------------------------------------------------------------------------
//
void CSensrvTransactionQueue::Remove(CSensrvSession* aSession)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Remove(aSession: 0x%x)" ), aSession ) );

    if (aSession)
        {
        TLinkableTransactionPtr* ptr = NULL;
        iTransactionPtrIter.SetToFirst(); 
        while ((ptr = iTransactionPtrIter++) != NULL)
            {
            if ( ptr->iTransaction->Message() )
                {
                if (ptr->iTransaction->Message()->Session() == aSession)    
                    {
                    RemovePtr(ptr, ERemovalTypeCancel);
                    }
                }
            };
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::Remove - Attempted to remove transactions for NULL session" ) ) );
        }
    }

// ---------------------------------------------------------------------------
// Remove all obsolete transactions from queue
// ---------------------------------------------------------------------------
//
TInt CSensrvTransactionQueue::RemoveObsolete(const TTime& aCutoffTime)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::RemoveObsolete(aCutoffTime: %LD)" ), aCutoffTime.Int64() ) );

    TInt count(0);
    TLinkableTransactionPtr* ptr = NULL;
    iTransactionPtrIter.SetToFirst(); 
    while ((ptr = iTransactionPtrIter++) != NULL)
        {
        if ( ptr->iTransaction->TimeStamp() <= aCutoffTime )
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::RemoveObsolete - Removing obsolete transaction: 0x%x" ), ptr->iTransaction) );

            count++;
            
            RemovePtr(ptr, ERemovalTypeCancel);
            }
        };

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::RemoveObsolete - return %d" ), count ) );
    
    return count;
    }

// ---------------------------------------------------------------------------
// Remove all transactions from queue
// ---------------------------------------------------------------------------
//
void CSensrvTransactionQueue::RemoveAll()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::RemoveAll()" ) ) );

    TLinkableTransactionPtr* ptr = NULL;
    iTransactionPtrIter.SetToFirst(); 
    while ((ptr = iTransactionPtrIter++) != NULL)
        {
        RemovePtr(ptr, ERemovalTypeCancel);
        };
    }


// ---------------------------------------------------------------------------
// Gets pointer to first transaction in queue.
// ---------------------------------------------------------------------------
//
CSensrvTransaction* CSensrvTransactionQueue::First()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::First()" ) ) );

    CSensrvTransaction* retval = NULL;

    if (!iTransactionPtrList.IsEmpty())
        {
        retval = iTransactionPtrList.First()->iTransaction;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::First - return 0x%x" ), retval ) );
    
    return retval;
    }


// ---------------------------------------------------------------------------
// Finds the queue item which contains pointer to specified transaction.
// ---------------------------------------------------------------------------
//
CSensrvTransactionQueue::TLinkableTransactionPtr* CSensrvTransactionQueue::FindTransactionPtr(CSensrvTransaction* aTransaction)
    {
    TLinkableTransactionPtr* ptr = NULL;

    if (aTransaction)
        {
        TBool found(EFalse);
        
        iTransactionPtrIter.SetToFirst(); 
        while (!found && (ptr = iTransactionPtrIter++) != NULL )
            {
            if ( ptr->iTransaction == aTransaction )
                {    
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::FindTransactionPtr - Pointer found" ) ) );
                found = ETrue;
                }
            };
        
        if (!found)
            {
            ptr = NULL;
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransactionQueue::FindTransactionPtr - Pointer not found" ) ) );
        }
    
    return ptr;    
    }

// ---------------------------------------------------------------------------
// Removes linkable pointer object from queue.
// ---------------------------------------------------------------------------
//
void CSensrvTransactionQueue::RemovePtr(TLinkableTransactionPtr* aPtr, TRemovalType aRemovalType)
    {
    if (aPtr)
        {
        if (iOwned && aRemovalType != ERemovalTypeTransfer)
            {
            __ASSERT_ALWAYS(aRemovalType != ERemovalTypeUndefined, User::Panic(KSensrvPanicCategory, ESensrvPanicInvalidRemoveType));

            if (aRemovalType == ERemovalTypeCancel)
                {
                aPtr->iTransaction->SetErrorCode(KErrCancel);    
                }
            aPtr->iTransaction->Complete();
            // Owned queues are always owned by server main thread and transactions are
            // only removed from those queues within server main thread,
            // so regular delete is ok.
            delete aPtr->iTransaction;
            }

        iTransactionPtrList.Remove(*aPtr);
        iHeap->Free(aPtr);
        aPtr = NULL;
        }
    }

