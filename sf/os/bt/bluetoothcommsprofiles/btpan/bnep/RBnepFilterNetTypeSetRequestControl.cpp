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
#include "RBnepFilterNetTypeSetRequestControl.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Obtain the length of the net protocol type list
   @internalComponent
*/
TInt RBnepFilterNetTypeSetRequestControl::NetworkProtocolTypeListLength (TUint16& aLength) const
    {
    TInt preamble = static_cast<TInt>(KSizeOfNetTypeFilterListLength + KSizeOfBnepControlType);
    if(Length() < preamble)
        {
        return KErrCorrupt;
        }
    TBuf8<KSizeOfNetTypeFilterListLength> buf;
    buf.SetMax();
    CopyOut(buf, KNetworkProtocolTypeListLengthFieldOffset);
    aLength = BigEndian::Get16(&buf[0]);
    LOG2(_L8("RBnepFilterNetTypeSet[%x]: ...getting net type filter list length - length %d..."), this, aLength);
    return KErrNone;
    }

/**
   Retrieve one value pair from a network packet type filter control
   @note This method should be called repeatedly until all the values are read out (ie. return value from the last pair is KErrEof)
   @param aStartValue The start value for the network filter range
   @param aEndValue The end value of the network filter range
   @param aIndex The index of the network protocol types, starting from zero.
   @return KErrNone if there a more addresses in the list, or KErrEof when there are no more addresses left
   @internalComponent
*/
TInt RBnepFilterNetTypeSetRequestControl::NetworkProtocolTypeRange (TUint16& aStartValue, TUint16& aEndValue, TUint aIndex)
    {
    LOG_FUNC
    TInt length = Length();
    TUint16 tempLength;
    TInt err = NetworkProtocolTypeListLength(tempLength);
    if(err)
        {
        return err;
        }
    else
        {
        TInt netTypeListLength = static_cast<TInt>(tempLength);

        if(netTypeListLength <= 0)
            {
            return KErrNotFound;
            }

        TBuf8<KSizeOfNetType> buffer;
        buffer.SetMax();
        TUint currentPair = (aIndex*(2*KSizeOfNetType) + KNetworkProtocolTypeFirstPairFieldOffset);
        if(length < currentPair+2*KSizeOfNetType)
        	{
        	return KErrCorrupt;
        	}
        CopyOut(buffer,currentPair);
        aStartValue = BigEndian::Get16(&buffer[0]);
        CopyOut(buffer,currentPair + KSizeOfNetType);
        aEndValue = BigEndian::Get16(&buffer[0]);
        LOG2(_L8("RBnepFilterNetTypeSet[%x]: ...start protocol type: %x"), this, aStartValue);
        LOG2(_L8("RBnepFilterNetTypeSet[%x]: ...end protocol type: %x"), this, aEndValue);
        if(aStartValue > aEndValue) 
            {
            LOG1(_L8("...error: start protocol type is greater than end protocol type"), this);		
            return KErrArgument; 
            }
        if(static_cast<TInt>((currentPair + (2*KSizeOfNetType))) > netTypeListLength) // check if this is the last pair
            {
            LOG1(_L8("...last item in list - returning KErrEof"), this);
            return KErrEof;
            }
        else
            {
            LOG1(_L8("...more items in list - returning KErrNone"), this);
            return KErrNone;
            }
        }
    }
