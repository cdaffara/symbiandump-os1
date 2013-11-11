// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "lbspositioningstatus.h"
#include "lbspositioningstatusimpl.h"
#include "lbsdevloggermacros.h"



//=============================================================================
// MLbsPositioningStatusObserver
//=============================================================================
/**
Provides the version of the observer interface. This can be used by the library 
to determine the functionality supported by the client application.

@return Version of the observer interface.
*/  
EXPORT_C TVersion MLbsPositioningStatusObserver::Version() const
    {
    LBSLOG(ELogP1, "MLbsPositioningStatusObserver::Version() Begin\n");
    LBSLOG(ELogP1, "MLbsPositioningStatusObserver::Version() End\n");
    return TVersion(1, 0, 0);
    }

EXPORT_C CLbsPositioningStatus* CLbsPositioningStatus::NewL(MLbsPositioningStatusObserver& aObserver)
    {
    LBSLOG(ELogP1, "CLbsPositioningStatus::NewL() Begin\n");
    LBSLOG(ELogP8, "->S CLbsPositioningStatus::NewL() CLbsPositioningStatus\n");
    CLbsPositioningStatus* me = new (ELeave) CLbsPositioningStatus();
    CleanupStack::PushL(me);
    me->ConstructL(aObserver);
    LBSLOG(ELogP1, "CLbsPositioningStatus::NewL() End\n");
    CleanupStack::Pop(me);
    return me;
    }

CLbsPositioningStatus::~CLbsPositioningStatus()
    {   
    LBSLOG(ELogP1, "CLbsPositioningStatus::~CLbsPositioningStatus() Begin\n");
    LBSLOG(ELogP8, "->S CLbsPositioningStatus::~CLbsPositioningStatus() CLbsPositioningStatus\n");
    delete iImpl;
    iImpl = NULL;
    LBSLOG(ELogP1, "CLbsPositioningStatus::~CLbsPositioningStatus() End\n");
    }

CLbsPositioningStatus::CLbsPositioningStatus()
    {
    LBSLOG(ELogP1, "CLbsPositioningStatus::CLbsPositioningStatus() Begin\n");
    LBSLOG(ELogP1, "CLbsPositioningStatus::CLbsPositioningStatus() End\n");
    }

void CLbsPositioningStatus::ConstructL(MLbsPositioningStatusObserver& aObserver)
    {
    LBSLOG(ELogP1, "CLbsPositioningStatus::ConstructL() Begin\n");
    iImpl = CLbsPositioningStatusImpl::NewL(aObserver);
    LBSLOG(ELogP1, "CLbsPositioningStatus::ConstructL() End\n");
    }

	
EXPORT_C CLbsPositioningStatus::TLbsPositioningStatus CLbsPositioningStatus::CurrentPositioningStatus()
    {
    LBSLOG(ELogP1, "CLbsPositioningStatus::CurrentPositioningStatus() Begin\n");
    LBSLOG(ELogP1, "CLbsPositioningStatus::CurrentPositioningStatus() Begin\n");
    return iImpl->CurrentPositioningStatus();
    }


