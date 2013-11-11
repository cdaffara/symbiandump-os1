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
#include "ctlbstestpsy6positioner.h"
#include "testpsy6.hrh"
#include "ctlbstestPsy6RequestHandler.h"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy6Positioner::CT_LbsTestPsy6Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy6Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestPsy6RequestHandler::NewL();		
	iRequestHandler->SetRequestsHandling(CT_LbsTestPsyBase::EBundling);
    TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	}

// Two-phased constructor.
CT_LbsTestPsy6Positioner* CT_LbsTestPsy6Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy6Positioner* self = new(ELeave) CT_LbsTestPsy6Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy6Positioner* CT_LbsTestPsy6Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy6Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsy6Positioner::~CT_LbsTestPsy6Positioner()
    {
	//iRequestHandler->Release();
	delete iRequestHandler;
    }

// ---------------------------------------------------------
// CT_LbsTestPsy6Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy6Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
    
    iRequestHandler->SetPartialSupport(IsPartialUpdateAllowed());
    //iRequestHandler->SetPartialSupport(ETrue);

	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestPsy6Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy6Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}

