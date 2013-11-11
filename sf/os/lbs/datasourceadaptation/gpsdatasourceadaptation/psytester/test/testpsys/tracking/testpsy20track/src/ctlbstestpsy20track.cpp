// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <lbspositioninfo.h>
#include "ctlbstestpsybase.h"
#include "ctlbstestpsy20trackmain.h"
#include "ctlbstestpsy20track.h"
#include "testpsy20.hrh"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy20Positioner::CT_LbsTestPsy20Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy20Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestPsyBase::InstanceL(PositionerStatus());		
	iRequestHandler->SetRequestsHandling(CT_LbsTestPsyBase::EBundling);
        
	TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	}

// Two-phased constructor.
CT_LbsTestPsy20Positioner* CT_LbsTestPsy20Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy20Positioner* self = new(ELeave) CT_LbsTestPsy20Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy20Positioner* CT_LbsTestPsy20Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy20Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsy20Positioner::~CT_LbsTestPsy20Positioner()
    {
	iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CTestPsy6Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy20Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
	
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestPsy20Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy20Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}

TBool CT_LbsTestPsy20Positioner::TrackingOverridden() const
{
	return ETrue;
}

void CT_LbsTestPsy20Positioner::StartTrackingL(const TTimeIntervalMicroSeconds& /*aInterval */)
{
	User::Leave(KErrNotFound);
}

void CT_LbsTestPsy20Positioner::StopTracking()
{
	iRequestHandler->SetTimerDelay(0.1);//Set timer to default (0.1)
}
