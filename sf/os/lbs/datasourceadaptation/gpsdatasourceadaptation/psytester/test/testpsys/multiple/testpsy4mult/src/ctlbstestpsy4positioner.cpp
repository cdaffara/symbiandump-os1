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
#include "ctlbstestpsy4positioner.h"
#include "testpsy4.hrh"
#include "ctlbstestPsy4RequestHandler.h"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy4Positioner::CT_LbsTestPsy4Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy4Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestPsy4RequestHandler::InstanceL();	
    TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	}

// Two-phased constructor.
CT_LbsTestPsy4Positioner* CT_LbsTestPsy4Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy4Positioner* self = new(ELeave) CT_LbsTestPsy4Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy4Positioner* CT_LbsTestPsy4Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy4Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsy4Positioner::~CT_LbsTestPsy4Positioner()
    {
	iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CT_LbsTestPsy4Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy4Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
    	
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);

	}

// ---------------------------------------------------------
// CT_LbsTestPsy4Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy4Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);	
	}

