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
#include "ctlbstestPsy5Positioner.h"
#include "testpsy5.hrh"

// CONSTANTS
const TUid incorrectImplUid = {0x66666666};

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy5Positioner::CT_LbsTestPsy5Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy5Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestPsyBase::InstanceL(PositionerStatus());		
    iRequestHandler->SetRequestsHandling(CT_LbsTestPsyBase::EBundling);
    }

// Two-phased constructor.
CT_LbsTestPsy5Positioner* CT_LbsTestPsy5Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy5Positioner* self = new(ELeave) CT_LbsTestPsy5Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy5Positioner* CT_LbsTestPsy5Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy5Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsy5Positioner::~CT_LbsTestPsy5Positioner()
    {
	iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CT_LbsTestPsy5Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy5Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
   
        //+ smysore
        //CT_LbsTestPsy5Positioner should return a wrong UID
        // in any case. This does not work with the emulator. 
	//if(++iRequests % 2)
		//{
		//iRequestHandler->SetModuleId(implUid);
		//}
	//else
		//{
		iRequestHandler->SetModuleId(incorrectImplUid);
		//}
	
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestPsy5Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy5Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}

