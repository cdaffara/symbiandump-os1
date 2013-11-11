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

#ifndef __RBNEPFILTERMULTIADDRSETREQUESTCONTROL_H
#define __RBNEPFILTERMULTIADDRSETREQUESTCONTROL_H 

#include "BnepTypes.h"
#include "RBnepControl.h"

class TBnepBTDevAddr;

NONSHARABLE_CLASS(RBnepFilterMultiAddrSetRequestControl) : public RBnepControl  
    {
public:
    typedef enum
        {
        KMultiAddrListLengthFieldOffset = KSizeOfBnepControlType,
        KMultiAddrListFirstPairFieldOffset = KSizeOfBnepControlType + KSizeOfMultiAddrFilterListLength,
        KMaxSizeOfMultiAddrSet = KSizeOfBnepControlType + KSizeOfMultiAddrFilterListLength + (KMaxMultiAddrFilterPairs*2*KSizeOfMultiAddr)
        } MultiAddrSetRequestConstants;

    TInt MultiAddrRange (TBnepBTDevAddr& aStartAddr, TBnepBTDevAddr& aEndAddr, TUint aIndex);
    TUint16 MulticastAddressListLength ();
    };
#endif
