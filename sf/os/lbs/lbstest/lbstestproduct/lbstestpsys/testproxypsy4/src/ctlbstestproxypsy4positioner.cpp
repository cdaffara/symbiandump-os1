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
#include "ctlbstestproxypsy4positioner.h"
#include "ctlbstestproxypsybase.h"
#include "testproxypsy4.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestProxyPsy4Positioner::CT_LbsTestProxyPsy4Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestProxyPsy4Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = new (ELeave) CT_LbsTestProxyPsyBase(PositionerStatus());
              
    TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	
	}

// Two-phased constructor.
CT_LbsTestProxyPsy4Positioner* CT_LbsTestProxyPsy4Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy4Positioner* self = new(ELeave) CT_LbsTestProxyPsy4Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestProxyPsy4Positioner* CT_LbsTestProxyPsy4Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy4Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestProxyPsy4Positioner::~CT_LbsTestProxyPsy4Positioner()
    {
    delete iRequestHandler;
	}

// ---------------------------------------------------------
// CTestPsy4Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy4Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
    TPositionInfo* posInfo = static_cast<TPositionInfo*> (&aPosInfo);

    TUint32 request = posInfo->UpdateType();
    switch (request)
        {
        case 12:
            iRequestHandler->SetErrorCode(KErrNone);
            break;
        default:
            iRequestHandler->SetErrorCode(KErrUnknown);
            break;
        }

	iRequestHandler->NotifyPositionUpdate(posInfo, &aStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsy4Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy4Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate();
	}

