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
#include <es_sock.h>
#include "BnepTypes.h"
#include "RBnepFilterMultiAddrSetRequestControl.h"

#include "TBnepBTDevAddr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Retrieve one value pair from a multicast address filter control
   @note This method should be called repeatedly until all the values are read out (ie. return value from the last pair is KErrEof)
   @param aStartValue The start value for the multicast address range
   @param aEndValue The end value of the multicast address range
   @param aIndex The index of the multicast address filter pairs, starting from zero.
   @return KErrNone if there any more addresses in the list, or KErrEof when there are no more addresses left
   @internalComponent
*/
TInt RBnepFilterMultiAddrSetRequestControl::MultiAddrRange (TBnepBTDevAddr& aStartAddr, TBnepBTDevAddr& aEndAddr, TUint aIndex)
    {
    LOG_FUNC
    LOG2(_L8("RBnepFilterMultiAddrSet[%x]: Getting multicast address pair, index %d..."), this, aIndex);
    // since this might be (probably is) from another device, check their quoted size against the size of the packet we've got
    TInt length = Length();
    TInt multiAddrLength = static_cast<TInt>(MulticastAddressListLength());
    TInt preamble = static_cast<TInt>(KSizeOfBnepControlType + KSizeOfMultiAddrFilterListLength);

    if(multiAddrLength <= 0)
        {
        return KErrNotFound;
        }

    if(length < multiAddrLength + preamble)
        {
        LOG3(_L8("RBnepFilterMultiAddrSet[%x]: Multicast address list length is invalid - our mbuf chain length (minus control type and length headers) %d, claimed length %d"), this, (Length() - (KSizeOfBnepControlType + KSizeOfMultiAddrFilterListLength)), MulticastAddressListLength());
        return KErrArgument;
        }

    TUint currentPair = (aIndex*(2*KSizeOfMultiAddr) + KMultiAddrListFirstPairFieldOffset);
	TInt rerr = aStartAddr.SetAddress(*this, currentPair);
	if(rerr == KErrNone)
		{
		rerr = aEndAddr.SetAddress(*this, currentPair + KSizeOfMultiAddr);
		}
	
    if(rerr != KErrNone || MulticastAddressListLength() <= (currentPair + (2*KSizeOfMultiAddr))) // check if this is the last pair
        {
        LOG1(_L8("RBnepFilterMultiAddrSet[%x]: ...last item in list - returning KErrEof"), this);
        return KErrEof;
        }
    else
        {
        LOG1(_L8("RBnepFilterMultiAddrSet[%x]: ...more items in list - returning KErrNone"), this);
        return KErrNone;
        }
    }

/**
   Return the length of the multicast address list
   @internalComponent
*/
TUint16 RBnepFilterMultiAddrSetRequestControl::MulticastAddressListLength ()
    {
    LOG_FUNC
    TUint16 length;
    length = BigEndian::Get16((First()->Ptr() + KMultiAddrListLengthFieldOffset));
    LOG1(_L8("...multicast address list length %d..."), length);
    return length;
    }
