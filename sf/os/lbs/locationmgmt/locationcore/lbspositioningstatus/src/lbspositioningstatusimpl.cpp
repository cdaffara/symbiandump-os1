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

#include "lbspositioningstatusimpl.h"
#include "lbsdevloggermacros.h"

EXPORT_C CLbsPositioningStatusImpl* CLbsPositioningStatusImpl::NewL(MLbsPositioningStatusObserver& aObserver)
    {
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::NewL() Begin\n");
    LBSLOG(ELogP8, "->S CLbsPositioningStatusImpl::NewL() CLbsPositioningStatusImpl\n");
    CLbsPositioningStatusImpl* me = new (ELeave) CLbsPositioningStatusImpl(aObserver);
    CleanupStack::PushL(me);
    me->ConstructL();
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::NewL() End\n");
    CleanupStack::Pop(me);
    return me;
    }

CLbsPositioningStatusImpl::~CLbsPositioningStatusImpl()
    {
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::~CLbsPositioningStatusImpl() Begin\n");
    LBSLOG(ELogP8, "->S CLbsPositioningStatusImpl::~CLbsPositioningStatusImpl() CLbsPositioningStatus\n");
    delete iMoWatcher;
    delete iNiWatcher;
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::~CLbsPositioningStatusImpl() End\n");
    }

CLbsPositioningStatusImpl::CLbsPositioningStatusImpl(MLbsPositioningStatusObserver& aObserver)
 : iObserver(aObserver) 
    {
	LBSLOG(ELogP1, "CLbsPositioningStatusImpl::CLbsPositioningStatusImpl() Begin\n");
    iLastKnownMoStatus = CLbsPositioningStatus::ELbsPositioningStatusNotActive;
    iLastKnownNiStatus = CLbsPositioningStatus::ELbsPositioningStatusNotActive;
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::CLbsPositioningStatusImpl() End\n");
    }

void CLbsPositioningStatusImpl::ConstructL()
    {
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::ConstructL() Begin\n");
    iMoWatcher = CLbsInternalPosStatusWatcher::NewL(ELbsPositioningStatusMO, *this);
    iNiWatcher = CLbsInternalPosStatusWatcher::NewL(ELbsPositioningStatusNI, *this);
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::ConstructL() End\n");
    }

	
EXPORT_C CLbsPositioningStatus::TLbsPositioningStatus CLbsPositioningStatusImpl::CurrentPositioningStatus()
    {
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::CurrentPositioningStatus() Begin\n");
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::CurrentPositioningStatus() End\n");
    return CombinedStatus();
    }

/** from MLbsInternalPosStatusObserver 
 * 
 * @param aType  mobile or network originated positioning status
 * @param aCount count current service requests of the type being reported 
 * */
void CLbsPositioningStatusImpl::OnInternalStatusUpdate(const TLbsPositioningStatusType& aType, const TInt& aCount)
    {
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::OnInternalStatusUpdate() Begin\n");
    CLbsPositioningStatus::TLbsPositioningStatus previous = CombinedStatus();

	// update the appropriate status
	if (aType==ELbsPositioningStatusMO)
		{
		iLastKnownMoStatus = aCount;	
		}
	else
		{
		iLastKnownNiStatus = aCount;
		}

    // update the observer if the combined status has changed.
    if (previous != CombinedStatus())
        {
        iObserver.OnPositioningStatusUpdate(CombinedStatus());
        }
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::OnInternalStatusUpdate() End\n");
    }


CLbsPositioningStatus::TLbsPositioningStatus CLbsPositioningStatusImpl::CombinedStatus()
    {
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::CombinedStatus() Begin\n");
    if (iLastKnownMoStatus + iLastKnownNiStatus != 0)
        {
        LBSLOG(ELogP1, "CLbsPositioningStatusImpl::CombinedStatus() Active End\n");
        return CLbsPositioningStatus::ELbsPositioningStatusActive;
        }
    else
        {
    LBSLOG(ELogP1, "CLbsPositioningStatusImpl::CombinedStatus() NOT Active End\n");
        return CLbsPositioningStatus::ELbsPositioningStatusNotActive;
        }
    }

