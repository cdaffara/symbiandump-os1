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

#ifndef __CBTADDRSUBSCRIBER_H
#define __CBTADDRSUBSCRIBER_H 

#include <e32base.h>
#include <bttypes.h>
#include <e32property.h>
#include "CBnepLocalDevice.h"


/**
   Monitor BT stack for hardware address notifications using publish and subscribe.
*/

NONSHARABLE_CLASS(CBTAddrSubscriber) : public CActive  
    {

public:
    ~CBTAddrSubscriber();
    static CBTAddrSubscriber* NewL (TBTDevAddr& aAddr, CBnepLocalDevice& aLocal);
    TInt SyncFetch (TBool aRestart = EFalse);

private:
    CBTAddrSubscriber (TBTDevAddr& aAddr, CBnepLocalDevice& aLocal);
    void RunL ();
    void DoCancel ();
    TBTDevAddr& iAddr;
    CBnepLocalDevice& iOwner;
    RProperty iDevAddrProperty;
    };
#endif
