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
#include "CNetTypeFilterTable.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Always guarantee the state of the table.
   @internalComponent
*/
CNetTypeFilterTable::CNetTypeFilterTable()
        :iCtr(0)
    {   
    }

/**
   Testing the validity of the supplied protocol
   @param aProtocol The protocol to be tested
   @return ETrue if the protocol is filtered out, EFalse if the protocol is not filtered out.
   @internalComponent
*/
TBool CNetTypeFilterTable::FilteredOut (const TUint16 aProtocol)
    {
    LOG_FUNC
    LOG1(_L8("Checking %04x"),aProtocol);
    TBool retval = ETrue;
    TUint aCtr;
    if(!iCtr) // No filters set, so we let everything through.
        {
        retval = EFalse;
        }
    else
        {
        for(aCtr = 0; aCtr < iCtr; ++aCtr)
            {
            if(aProtocol < iTable[2*aCtr])
                {
                // The table is sorted, so that if the start value exceeds 
                // the candidate value then the search should be terminated.
                break;
                }
            else
                {
                if((aProtocol >= iTable[2*aCtr]) &&
                   (aProtocol <= iTable[2*aCtr+1]))
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

/**
   Insert a filter specification into the table. 

   @note aEnd must be greater than or equal to (>=) aStart or the method will leave. 
   If an attempt is made to add a filter range beyond the capacity of the table 
   (KMaxNetTypeFilters) the method will also leave.
   @param aStart start of the protocol range that may be passed through this filter (inclusive).
   @param aEnd end of the protocol range that may be passed through this filter (inclusive).
   @internalComponent
*/
TInt CNetTypeFilterTable::Insert (const TUint16 aStart, const TUint16 aEnd)
    {
    LOG_FUNC
    LOG2(_L8("Start %04x End %04x"),aStart, aEnd);
    if(KMaxNetTypeFilters == iCtr)
        {
        // Attempting to add too many filters. We don't allow that.
        LOG(_L8("Attempting to exceed KMaxMultiAddrFilters"));
        return KErrTooBig;
        }
    if(aEnd < aStart)
        {
        // This should never happen, in that it should be caught 
        // by higher-level logic, but just in case ...
        LOG(_L8("End of range less than start"));
        return KErrArgument;
        }
    if(!iCtr)
        {
        iTable[0] = aStart; // First insertion so we always insert at the "top"
        iTable[1] = aEnd;
        LOG2(_L8("First entry - Start %04x End %04x"),iTable[0],iTable[1]);
        ++iCtr;
        }
    else
        {
        // Okay, we already have entries so we need to do an insertion sort. 
        // It may not be efficient but we don't have to do it often and 
        // it leaves the table in a suitable state for efficient searching.
        TUint aCtr;
        for(aCtr = 0; aCtr < iCtr-1; ++aCtr)
            {
            if(aStart < iTable[2*aCtr])
                {
                // We've found a place in the middle of the table in which we should 
                // insert, so we have to shuffle the lower end down a bit to 
                // free up a slot.
                TUint bCtr;
                for(bCtr = KMaxNetTypeFilters - 1; bCtr > aCtr; --bCtr)
                    {
                    iTable[2*bCtr] = iTable[2*(bCtr-1)];
                    iTable[2*bCtr+1] = iTable[2*(bCtr-1)+1];
                    }
                // Now our insertion slot is free for our new values.
                iTable[2*aCtr] = aStart;
                iTable[2*aCtr+1] = aEnd;
                LOG3(_L8("Entry %d - Start %04x End %04x"),aCtr,iTable[2*aCtr],iTable[2*aCtr+1]);
                ++iCtr;
                return KErrNone;
                }
            }
        // If we get here then we didn't find an address range that started 
        // later than this one, so we can insert at the "bottom".
        iTable[2*iCtr] = aStart;
        iTable[2*iCtr+1] = aEnd;
        LOG3(_L8("Entry %d - Start %04x End %04x"),iCtr,iTable[2*iCtr],iTable[2*iCtr+1]);
        ++iCtr;
        }
    return KErrNone;
    }

/**
   Resets the filter table counter to zero. All table values default to 
   range 0x0000 to 0xffff which is the same as saying that all protocols 
   are valid. This ensures that if the table is used incorrectly after 
   being reset, the effect will be the same as if a filter had not been set.
   @internalComponent
*/
void CNetTypeFilterTable::Reset ()
    {
    LOG_FUNC
    //if(iCtr)
    //    {
    //   TUint aCtr;
    //   for(aCtr = 0; aCtr < iCtr; ++aCtr)
    //       {
    //       iTable[2*aCtr] = 0x0000;
    //        iTable[2*aCtr+1] = 0xffff;
    //       }
    //   }
    iCtr = 0;
    }
