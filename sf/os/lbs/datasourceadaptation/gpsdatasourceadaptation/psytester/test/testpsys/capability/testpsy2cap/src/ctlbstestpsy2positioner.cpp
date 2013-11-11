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
#include "ctlbstestPsy2Positioner.h"
#include "ctlbstestPsy2RequestHandler.h"
#include "TestPsy2.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPsy2Positioner::CT_LbsPsy2Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsPsy2Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsPsy2RequestHandler::InstanceL();		
	TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);

    }

// Two-phased constructor.
CT_LbsPsy2Positioner* CT_LbsPsy2Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsPsy2Positioner* self = new(ELeave) CT_LbsPsy2Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsPsy2Positioner* CT_LbsPsy2Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsPsy2Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsPsy2Positioner::~CT_LbsPsy2Positioner()
    {
    iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CTestPsyPositioner2::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy2Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
    *iStatus = KRequestPending;
	
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CTestPsyPositioner2::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy2Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}
