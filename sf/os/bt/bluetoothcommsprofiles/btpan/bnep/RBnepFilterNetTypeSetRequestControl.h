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

#ifndef __RBNEPFILTERNETTYPESETREQUESTCONTROL_H
#define __RBNEPFILTERNETTYPESETREQUESTCONTROL_H 

#include "RBnepControl.h"

NONSHARABLE_CLASS(RBnepFilterNetTypeSetRequestControl) : public RBnepControl  
    {
    typedef enum
        {
        KNetworkProtocolTypeListLengthFieldOffset = KSizeOfBnepControlType,
        KNetworkProtocolTypeFirstPairFieldOffset = KSizeOfBnepControlType+KSizeOfNetTypeFilterListLength,
        KMaxSizeOfFilterNetTypeSet = KSizeOfBnepControlType + KSizeOfNetTypeFilterListLength + (KMaxNetworkTypeFilterPairs*2*KSizeOfNetType) // control type (1) + list length (2) + n*2*network type - one start and one end (2*2)
        } NetTypeSetRequestConstants;
public:
    TInt NetworkProtocolTypeListLength (TUint16& aLength) const;
    TInt NetworkProtocolTypeRange (TUint16& aStartValue, TUint16& aEndValue, TUint aIndex);
    };

#endif
