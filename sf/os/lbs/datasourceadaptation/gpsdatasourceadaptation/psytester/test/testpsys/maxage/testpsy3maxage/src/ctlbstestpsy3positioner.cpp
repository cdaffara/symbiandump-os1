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

#include "ctlbstestpsy3positioner.h"
#include "ctlbstestpsy3requesthandler.h"
#include "testpsy3.hrh"

//CONSTANTS


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy3Positioner::CT_LbsTestPsy3Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy3Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestPsy3RequestHandler::InstanceL();		
	TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
    }

// Two-phased constructor.
CT_LbsTestPsy3Positioner* CT_LbsTestPsy3Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy3Positioner* self = new(ELeave) CT_LbsTestPsy3Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy3Positioner* CT_LbsTestPsy3Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy3Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsy3Positioner::~CT_LbsTestPsy3Positioner()
    {
    iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CTestPsyPositioner3::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy3Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
    ++iRequests;

	TTime maxAge;
	/*
	maxAge.UniversalTime();
	
	if (iRequests == 1)
		{
		TTimeIntervalSeconds seconds = 30;
		maxAge += seconds;
		iRequestHandler->SetMaxAge(maxAge);
		}
	*/
	GetMaxAge(maxAge);	
	iRequestHandler->SetMaxAge(maxAge);
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CTestPsyPositioner3::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy3Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}
