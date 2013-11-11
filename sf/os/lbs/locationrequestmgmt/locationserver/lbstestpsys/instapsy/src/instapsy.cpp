/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#include <e32std.h>              // basic types

#include <ecom/implementationproxy.h> // for TImplementationProxy
#include <lbspositioninfo.h>     // for TPositionInfoBase

#include <lbs/epos_mpositionerstatus.h>

#include "instapsy.h"
#include "instapsy.hrh"


const TImplementationProxy KFactoryPtr =
        {{KInstaPsyImplUid}, (TProxyNewLPtr)CInstaPsy::NewL};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }


CInstaPsy* CInstaPsy::NewL(TAny* aConstructionParameters)
    {
    CInstaPsy* self = new (ELeave) CInstaPsy;

    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

void CInstaPsy::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
    }


CInstaPsy::~CInstaPsy()
    {
    // No resources to free
    }

//////////////////////////
// Methods overridden from CPositioner
//////////////////////////

void CInstaPsy::NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
    {
    iStatusPtr = &aStatus; // iStatusPtr is a TRequestStatus*

    // NotifyPositionUpdate must return quickly. It should
    // be implemented as asynchronous but since Example PSY
    // is very fast, we fetch the position synchronously.
    TRAPD(err, GetPositionInfoL(aPosInfo));

    if (err)
        {
        User::RequestComplete(iStatusPtr, err);
        }
    }


void CInstaPsy::GetPositionInfoL(TPositionInfoBase& aPosInfo)
    {
    // check that requested class is supported
    if (aPosInfo.PositionClassType() & ~(EPositionInfoClass))
        {
        // some more bits are set
        User::RequestComplete(iStatusPtr, KErrArgument);
        return;
        }

    // Check which position information has been requested
    // and provide that information.
    if (aPosInfo.PositionClassType() & EPositionInfoClass) // was EPositionClassInfo
        {
        GetBasicPositionInfoL(aPosInfo);
        }

    // Complete the request
    User::RequestComplete(iStatusPtr, KErrNone);
    }


void CInstaPsy::GetBasicPositionInfoL(TPositionInfoBase& aPosInfo)
    {
    // The position info object is at least a TPositionInfo
    TPositionInfo* posInfo =
        static_cast<TPositionInfo*>(&aPosInfo);

    TPosition pos;
    // Calculate the position and fill in the position info
    // object
    pos.SetCoordinate(28, 28, 28);

    // set horizontal and vertical accuracy
    pos.SetAccuracy(27, 28);

    // set time of fix
    pos.SetCurrentTime();

    // Set position in position info.
    posInfo->SetPosition(pos);

    // Set the implementation uid

    posInfo->SetModuleId(ImplementationUid());
    }
void CInstaPsy::CancelNotifyPositionUpdate()
    {
    // No asynchronous operations, i.e. nothing to cancel
    }


// End of file

