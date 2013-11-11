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
#include "ctlbstestpsy2positioner.h"
#include "TestPsy2.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy2Positioner::CT_LbsTestPsy2Positioner() :
					 iHeap(User::Heap())
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy2Positioner::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestPsyBase::InstanceL(PositionerStatus());		
	TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);		
    }

// Two-phased constructor.
CT_LbsTestPsy2Positioner* CT_LbsTestPsy2Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy2Positioner* self = new(ELeave) CT_LbsTestPsy2Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy2Positioner* CT_LbsTestPsy2Positioner::NewL(TAny* aConstructionParameters)
    {      
	CT_LbsTestPsy2Positioner* self = NewLC(aConstructionParameters);
	CleanupStack::Pop();
    
	return self;
    }
    
// Destructor
CT_LbsTestPsy2Positioner::~CT_LbsTestPsy2Positioner()
    {
    iRequestHandler->Release();

	//RHeap heap = User::Heap();
	iHeap.FreeZ(iAny);
	}

// ---------------------------------------------------------
// CTestPsyPositioner2::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy2Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
//    *iStatus = KRequestPending;
    
    ++iRequests;
	
    //TInt tmp;
	//TInt size = heap.Available(tmp);
    
	if(iRequests == 2)
		{
		iAny = iHeap.Alloc(100);
		}
			
	iRequestHandler->NotifyPositionUpdate(static_cast<TPositionInfo*> (&aPosInfo), &aStatus);
	}

// ---------------------------------------------------------
// CTestPsyPositioner2::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy2Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate(iStatus);
	}
