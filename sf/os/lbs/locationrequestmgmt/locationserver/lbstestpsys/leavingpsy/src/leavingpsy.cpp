// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32std.h>              // basic types

#include <ecom/implementationproxy.h> // for TImplementationProxy
#include <lbspositioninfo.h>     // for TPositionInfoBase

#include <lbs/epos_mpositionerstatus.h>

#include "leavingpsy.h"
#include "leavingpsy.hrh"


const TImplementationProxy KFactoryPtr =
        {{KLeavingPsyImplUid}, (TProxyNewLPtr)CLeavingPsy::NewL};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }


CLeavingPsy* CLeavingPsy::NewL(TAny* aConstructionParameters)
    {
    CLeavingPsy* self = new (ELeave) CLeavingPsy;

    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

void CLeavingPsy::ConstructL(TAny* aConstructionParameters)
    {
    // Must call BaseConstructL first thing during
    // construction.
    BaseConstructL(aConstructionParameters);
    User::Leave(KErrGeneral);
    }


CLeavingPsy::~CLeavingPsy()
    {
    // No resources to free
    }

//
// Methods overridden from CPositioner
//

void CLeavingPsy::NotifyPositionUpdate(TPositionInfoBase& /*aPosInfo*/,
    TRequestStatus& aStatus)
    {
    iStatusPtr = &aStatus; // iStatusPtr is a TRequestStatus*
    User::RequestComplete(iStatusPtr, KErrGeneral);
    }

void CLeavingPsy::CancelNotifyPositionUpdate()
    {
    // No asynchronous operations, i.e. nothing to cancel
    }


// End of file

