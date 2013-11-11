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

#include "btpsy.h"
#include "btpsy.hrh"


const TImplementationProxy KFactoryPtr =
        {{KBtPsyImplUid}, (TProxyNewLPtr)CBtPsy::NewL};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }


CBtPsy* CBtPsy::NewL(TAny* aConstructionParameters)
    {
    CBtPsy* self = new (ELeave) CBtPsy;

    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

void CBtPsy::ConstructL(TAny* aConstructionParameters)
    {
    // Must call BaseConstructL first thing during
    // construction.
    BaseConstructL(aConstructionParameters);

    User::LeaveIfError(iNotifier.Connect());
    }


CBtPsy::~CBtPsy()
    {
    // No resources to free
    }

//
// Methods overridden from CPositioner
//

void CBtPsy::NotifyPositionUpdate(TPositionInfoBase& /*aPosInfo*/,
    TRequestStatus& aStatus)
    {
    iStatusPtr = &aStatus; // iStatusPtr is a TRequestStatus*

    const TUid KBTPowerStateNotifierUidS60 = {0x100059E2};
    TPckgBuf<TBool> bTStateQueryParameter;
    TPckgBuf<TBool> bTStateQueryResponse;

    TRequestStatus status;

    iNotifier.StartNotifierAndGetResponse(
        status,
        KBTPowerStateNotifierUidS60,
        bTStateQueryParameter,
        bTStateQueryResponse);

    User::WaitForRequest(status);
    User::RequestComplete(iStatusPtr, status.Int());
    }

void CBtPsy::CancelNotifyPositionUpdate()
    {
    // No asynchronous operations, i.e. nothing to cancel
    }


// End of file

