// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CResponseTimerStoreTraces.h"
#endif

#include "CResponseTimerStore.h"
#include "cmmphonetsy.h"
#include "CResponseTimer.h"
#include "MmTsy_timeoutdefs.h"

#ifdef REQHANDLE_TIMER

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CResponseTimerStore::CTableEntry::OrderEntries 
// Compares two entries and returns which is the order between them.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CResponseTimerStore::CTableEntry::OrderEntries (
    const CResponseTimerStore::CTableEntry& arg1, 
    const CResponseTimerStore::CTableEntry& arg2 ) 
    {
    TInt ret ( -1 );

    //we are interested only in the time stamp
    if ( arg1.iTimeStamp == arg2.iTimeStamp )
        {
            ret = 0;
        }
    else if( arg1.iTimeStamp > arg2.iTimeStamp )
        {
            ret = 1;
        }

    return ret;
  }

// ---------------------------------------------------------------------------
// CResponseTimerStore::CTableEntry::CompareEntries 
// Compares two entries are return if the are same of not.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CResponseTimerStore::CTableEntry::CompareEntries(
    const CResponseTimerStore::CTableEntry& arg1, 
    const CResponseTimerStore::CTableEntry& arg2 ) 
    {
     TBool ret ( EFalse ); 

     // we are interested in the handle type and the HandleStore
     if ( arg1.iHandleType == arg2.iHandleType &&  
          arg1.iHandleStore  == arg2.iHandleStore )
        {
         ret =  ETrue;
        }
     return ret;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::CTableEntry::GetTimeStamp
// gets the value of iTimeStamp
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint32 CResponseTimerStore::CTableEntry::GetTimeStamp()
    {
    return iTimeStamp;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::CTableEntry::GetHandleStore
// Gets the value of iHandleStore
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmTsyReqHandleStore* CResponseTimerStore::CTableEntry::GetHandleStore()
    {
    return iHandleStore;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::CTableEntry::GetUserObject
// Gets the value of iUserObject
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CResponseTimerStore::CTableEntry::GetUserObject()
    {
    return iUserObject;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::CTableEntry::GetHandleType
// Gets the value of iHandleType
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CResponseTimerStore::CTableEntry::GetHandleType()
    {
    return iHandleType;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::CTableEntry::GetIPC
// Returns IPC value
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CResponseTimerStore::CTableEntry::GetIPC()
    {
    return iIPC;
    }

CResponseTimerStore::CTableEntry::CTableEntry(
    TInt aHandleType, // Handle type
    TUint32 aTimeStamp, // time stamp
    CMmTsyReqHandleStore* aHandleStore, // Pointer to handle store
    CTelObject* aUserObject, // Pointer to user object
    TInt aIPC ) // value of IPC request
    {
    //fill the internal attributes
    iHandleType = aHandleType;
    iTimeStamp = aTimeStamp;
    iHandleStore = aHandleStore;
    iUserObject = aUserObject;
    iIPC = aIPC;
    }

CResponseTimerStore* CResponseTimerStore::NewL(
      CMmPhoneTsy *aPhone ) // pointer to phone object
    {
    CResponseTimerStore* responseTimerStore
            = new ( ELeave ) CResponseTimerStore();
 
    CleanupStack::PushL( responseTimerStore );
    
    //start the table for the entries
    responseTimerStore->iEntryTable  = new ( ELeave ) 
            RArray<CTableEntry>( KMultimodeTsyTimeStoreLength ); 

    //get the pointer to the phone object
    responseTimerStore->iPhone = aPhone;

    CleanupStack::Pop(); //responseTimerStore

    return responseTimerStore;
    }

CResponseTimerStore::CResponseTimerStore()
    {
    }

CResponseTimerStore::~CResponseTimerStore()
    {
    //delete the RArray
    delete iEntryTable;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::AddTimeEntry
// Adds a new entry in the entry table
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CResponseTimerStore::AddTimeEntry(
    TInt aHandleIndex, 
    CMmTsyReqHandleStore *aHandleStore, 
    CTelObject *aUserObject, 
    TUint32 aWaitingTime, 
    TInt aIPC ) 
    {
    TInt ret( KErrNone );

    //if entries are 0 re-start the timer
    if ( 0 == iEntryTable->Count() )
        {
        iPhone->GetTimer()->Start();
        }

    //calculate the trigger time (actual beat amount + timeout)
    TUint32 timeStamp = iPhone->GetTimer()->GetBeatCounter() + aWaitingTime;
    //create the entry
    CTableEntry aEntry( aHandleIndex, timeStamp, aHandleStore, 
        aUserObject, aIPC );

    // Check that the entry is not already in the table 
    if ( !CheckEntry( aHandleIndex, aHandleStore ) )
        {
        //insert the entry in the entry table
        ret = iEntryTable->InsertInOrderAllowRepeats( aEntry, 
            CTableEntry::OrderEntries );
        }
    else
        {
        ResetEntryTimeout( aHandleIndex, aHandleStore, aUserObject, 
            aWaitingTime );
        }
    
    //check the result
    if ( KErrNone != ret )
        {
        //log problem
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRESPONSETIMERSTORE_ADDTIMEENTRY_1, "Error inserting entry in the Timer table.");
        }
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::DeleteEntryByHandle
// Deletes an entry from the entry table using the handle type 
// and the handle store as identificators.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CResponseTimerStore::DeleteEntryByHandle(
    TInt aHandleIndex, 
    CMmTsyReqHandleStore *aHandleStore ) 
    {
    TInt ret( KErrNotFound );

    //create the entry with meaningful values only
    CTableEntry aEntry( aHandleIndex, 0, aHandleStore, NULL );

    //find the entry for this Handle in the list 
    TInt position = iEntryTable->Find( aEntry, CTableEntry::CompareEntries );
    //Check if element is found
    if ( KErrNotFound != position ) 
        {
        iEntryTable->Remove( position );
        iEntryTable->Compress();
        //check if empty
        if ( 0 == iEntryTable->Count() )
            {
            //restart the beatCounter
            iPhone->GetTimer()->ResetBeatCounter();
            //stop timer till new entry is done
            iPhone->GetTimer()->Stop();
            }
        ret = KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::DeleteEntryByIndex
// Deletes an entry from iEntryTable using an index as identificator
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CResponseTimerStore::DeleteEntryByIndex(
    TInt aIndex ) 
    {
    TInt ret( KErrNotFound );

    //Check if the index is inside the bounds
    if ( aIndex >= 0 && 
         aIndex < iEntryTable->Count() )
        {
        //remove the entry
        iEntryTable->Remove( aIndex );
        //compress the table
        iEntryTable->Compress();
        //check if empty
        if ( 0 == iEntryTable->Count() )
            {
             //restart the beatCounter of the timer
            iPhone->GetTimer()->ResetBeatCounter();
            //stop timer till new entry is done
            iPhone->GetTimer()->Stop ();
            }
        //set return to KErrNone
        ret = KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::GetEntry
// Gets a pointer to the entry of iEntryTable located in the given index.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CResponseTimerStore::CTableEntry* CResponseTimerStore::GetEntry(
    TInt aIndex ) 
    {
    CResponseTimerStore::CTableEntry* ret = NULL;

    //Check if the index is inside the bounds
    if ( aIndex >= 0 && 
         aIndex < iEntryTable->Count() )
        {
        //get the pointer to the Entry  
        ret = &( ( *iEntryTable )[ aIndex ] );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::ResetEntryTimeout
// Resets the value of the timeout of an entry in iEntryTable
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CResponseTimerStore::ResetEntryTimeout(
    TInt aHandleIndex, 
    CMmTsyReqHandleStore *aHandleStore, 
    CTelObject *aUserObject, 
    TUint32 aWaitingTime ) 
    {
    //Delete the old entry
    TInt ret = DeleteEntryByHandle( aHandleIndex, aHandleStore );

    //Proceed only if there was an existing entry
    if ( KErrNotFound != ret )
        {
        //Add new entry
        AddTimeEntry( aHandleIndex, aHandleStore, aUserObject, aWaitingTime );
        }
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::NumberOfEntries
// Returns the Number of entries of iEntryTable
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CResponseTimerStore::NumberOfEntries()
    {
    return iEntryTable->Count();
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::CheckEntry
// Check if an entry is in iEntryTable
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CResponseTimerStore::CheckEntry(
    TInt aHandleIndex, 
    CMmTsyReqHandleStore *aHandleStore ) 
    {
    TBool ret( EFalse );

    //create the entry with meaningful values only
    CTableEntry aEntry( aHandleIndex, 0, aHandleStore, NULL );

    //find the entry for this Handle in the list 
    TInt position = iEntryTable->Find( aEntry, CTableEntry::CompareEntries );
    
    if ( KErrNotFound != position )
        {
        ret = ETrue;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CResponseTimerStore::DeleteByHandleStoreAllEntries
// Deletes all the entries related with a given Handle Store.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CResponseTimerStore::DeleteByHandleStoreAllEntries(
    CMmTsyReqHandleStore *aHandleStore )
    {
    TInt ret( KErrNone );
    for ( TInt index = 0; index < iEntryTable->Count(); index++ )
        {
        //get the entry in this index
        CTableEntry aEntry = ( *iEntryTable ) [ index ];
        //check the object
        if( aEntry.GetHandleStore() == aHandleStore )
            {
            //delete the object
            ret = DeleteEntryByIndex( index );
            //check the result
            if( KErrNone != ret )
                {
                //error deleting entry break and return value
                break;
                }
            }
        }
    return ret;
    }   

#endif // REQHANDLE_TIMER

//  End of File
