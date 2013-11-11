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
#include "ctlbstestpsy19trackmain.h"
#include "ctlbstestpsy19track.h"
#include "testpsy19.hrh"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CTestPsy19Positioner::CTestPsy19Positioner() 
    {
    }

// EPOC default constructor can leave.
void CTestPsy19Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestPsyBase::InstanceL(PositionerStatus());		
	iRequestHandler->SetRequestsHandling(CT_LbsTestPsyBase::EBundling);
        
	TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	}

// Two-phased constructor.
CTestPsy19Positioner* CTestPsy19Positioner::NewLC(TAny* aConstructionParameters)
    {
    CTestPsy19Positioner* self = new(ELeave) CTestPsy19Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CTestPsy19Positioner* CTestPsy19Positioner::NewL(TAny* aConstructionParameters)
    {
    CTestPsy19Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CTestPsy19Positioner::~CTestPsy19Positioner()
    {
	iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CTestPsy19Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CTestPsy19Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
	
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CTestPsy19Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CTestPsy19Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}

TBool CTestPsy19Positioner::TrackingOverridden() const
{
	return ETrue;
}

void CTestPsy19Positioner::StartTrackingL(const TTimeIntervalMicroSeconds& aInterval)
{
	iRequestHandler->SetTimerDelay(aInterval);
}

void CTestPsy19Positioner::StopTracking()
{
	iRequestHandler->SetTimerDelay(0.1);//Set timer to default (0.1)
}
