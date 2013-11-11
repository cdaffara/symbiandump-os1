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
#include "ctlbstestPsy3RequestHandler.h"
#include "ctlbstestPsy3Positioner.h"
#include "TestPsy3.hrh"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPsy3Positioner::CT_LbsPsy3Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsPsy3Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsPsy3RequestHandler::InstanceL();		
    TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	}

// Two-phased constructor.
CT_LbsPsy3Positioner* CT_LbsPsy3Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsPsy3Positioner* self = new(ELeave) CT_LbsPsy3Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsPsy3Positioner* CT_LbsPsy3Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsPsy3Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsPsy3Positioner::~CT_LbsPsy3Positioner()
    {
    iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CTestPsyPositioner3::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy3Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
    	
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CTestPsyPositioner3::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy3Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}
