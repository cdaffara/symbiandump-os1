// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <e32std.h>
#include "CMultiAddrFilterTable.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Always guarantee the state of the table.
   @internalComponent
*/
CMultiAddrFilterTable::CMultiAddrFilterTable()
    {
    Reset();
    }

/**
   Insert a filter specification into the table. This method uses an insertion 
   sort algorithm that guarantees that the filter specs are in ascending order. 
   This is inefficient on insertion but ultimately speeds up the lookup test for 
   each packet passing through the filter.
   @note aEnd must be greater than or equal to (>=) aStart or this method will leave. 
   If an attempt is made to add a filter range beyond the capacity of the table 
   (KMaxMultiAddrFilters) the method will also leave.
   @param aStart BNEP representation of a BT Address of the start of the multicast address range that may be passed through this filter (inclusive).
   @param aEnd BNEP representation of a BT Address of the end of the multicast address range that may be passed through this filter (inclusive).
   @internalComponent
*/
TInt CMultiAddrFilterTable::Insert (const TBnepBTDevAddr& aStart, const TBnepBTDevAddr& aEnd)
    {
    LOG_FUNC
    LOG2(_L8("Start %016Lx End %016Lx"),aStart.Address(), aEnd.Address());
    if(KMaxMultiAddrFilters == iCtr) // Attempting to add too many filters. We don't allow that.
        {
        LOG(_L8("Attempting to exceed KMaxMultiAddrFilters - leaving"));
        return KErrTooBig;
        }
    if(aEnd < aStart)// This should never happen, in that it should be caught by higher-level logic, but just in case ...
        {
        LOG(_L8("End of range less than start - leaving"));
        return KErrArgument;
        }
    if(!iCtr)
        {
        iTable[0] = aStart; // First insertion so we always insert at the "top"
        iTable[1] = aEnd;
        LOG2(_L8("First entry - Start %016Lx End %016Lx"),iTable[0].Address(),iTable[1].Address());
        ++iCtr;
        }
    else
        {
        // Okay, we already have entries so we need to do an insertion sort. 
        // It may not be efficient but we don't have to do it often and 
        //it leaves the table in a suitable state for efficient searching.
        TUint aCtr;
        for(aCtr = 0; aCtr < iCtr-1; ++aCtr)
            {
            if(aStart < iTable[2*aCtr])
                {
                // We've found a place in the middle of the table in which we should 
                // insert, so we have to shuffle the lower end down a bit to 
                // free up a slot.
                TUint bCtr;
                for(bCtr = KMaxMultiAddrFilters - 1; bCtr > aCtr; --bCtr)
                    {
                    iTable[2*bCtr] = iTable[2*(bCtr-1)];
                    iTable[2*bCtr+1] = iTable[2*(bCtr-1)+1];
                    }
                // Now our insertion slot is free for our new values.
                iTable[2*aCtr] = aStart;
                iTable[2*aCtr+1] = aEnd;
                LOG3(_L8("Entry %d - Start %016Lx End %016Lx"),aCtr,iTable[2*aCtr].Address(),iTable[2*aCtr+1].Address());
                ++iCtr;
                return KErrNone;
                }
            }
        // If we get here then we didn't find an address range that started 
        // later than this one, so we can insert at the "bottom".
        iTable[2*iCtr] = aStart;
        iTable[2*iCtr+1] = aEnd;
        LOG3(_L8("Entry %d - Start %04x End %04x"),iCtr,iTable[2*iCtr].Address(),iTable[2*iCtr+1].Address());
        ++iCtr;
        }
    return KErrNone;
    }

/**
   Resets the filter table counter to zero. Since all subsequent insertions and 
   searches are bounded by the counter, it is not necessary to remove stale data 
   from the table itself..
   @internalComponent
*/
void CMultiAddrFilterTable::Reset ()
    {
    LOG_FUNC
    iCtr = 0;
    }

/**
   General method for testing the validity of the supplied address
   @param aAddr The BNEP representation of the BT address to be tested
   @return ETrue if the address is filtered out, EFalse if the address is not filtered out.
   @internalComponent
*/
TBool CMultiAddrFilterTable::FilteredOut (const TBnepBTDevAddr& aAddr)
    {
    LOG_FUNC
    LOG1(_L8("Checking %016Lx"),aAddr.Address());
    TBool retval = ETrue; // Assume the address is automatically blocked
    TUint aCtr;
    if(!iCtr) // No filters set, so we let everything through.
        {
        retval = EFalse;
        }
    else
        {
        for(aCtr = 0; aCtr < iCtr; ++aCtr)
            {
            if(aAddr < iTable[2*aCtr])
                {
                // The table is sorted, so that if the start value exceeds 
                // the candidate address then the search can be terminated.
                break;
                }
            else
                {
                if((aAddr >= iTable[2*aCtr]) &&
                   (aAddr <= iTable[2*aCtr+1]))
                    {
                    // This is within the specified range so we can 
                    // set the return to false and stop searching.
                    retval = EFalse;
                    break;
                    }
                }
            }
        }
    return retval;
    }
