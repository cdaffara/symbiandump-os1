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
#include "ctlbstestpsy1positioner.h"
#include "testpsy1.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy1Positioner::CT_LbsTestPsy1Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy1Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestPsyBase::InstanceL(PositionerStatus());
    TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	User::Leave(KErrGeneral);
	}

// Two-phased constructor.
CT_LbsTestPsy1Positioner* CT_LbsTestPsy1Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy1Positioner* self = new(ELeave) CT_LbsTestPsy1Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy1Positioner* CT_LbsTestPsy1Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy1Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsy1Positioner::~CT_LbsTestPsy1Positioner()
    {
    iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CTestPsyPositioner1::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy1Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
    	
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CTestPsyPositioner1::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy1Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}

