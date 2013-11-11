/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


// INCLUDE FILES
#include <lbspositioninfo.h>
#include "ctlbstestproxypsy5positioner.h"
#include "ctlbstestproxypsybase.h"
#include "testproxypsy5.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestProxyPsy5Positioner::CT_LbsTestProxyPsy5Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestProxyPsy5Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = new (ELeave) CT_LbsTestProxyPsyBase(PositionerStatus());
              
    TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	
	}

// Two-phased constructor.
CT_LbsTestProxyPsy5Positioner* CT_LbsTestProxyPsy5Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy5Positioner* self = new(ELeave) CT_LbsTestProxyPsy5Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestProxyPsy5Positioner* CT_LbsTestProxyPsy5Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy5Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestProxyPsy5Positioner::~CT_LbsTestProxyPsy5Positioner()
    {
    delete iRequestHandler;
	}

// ---------------------------------------------------------
// CTestPsy4Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy5Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
    TPositionInfo* posInfo = static_cast<TPositionInfo*> (&aPosInfo);
    TPosition pos;
	pos.SetCoordinate(99, 99, 99);
	posInfo->SetPosition(pos);
	
    TUint32 request = posInfo->UpdateType();
    switch (request)
        {
        case 304:
            iRequestHandler->SetErrorCode(KErrNone);
            break;
        default:
            iRequestHandler->SetErrorCode(KErrUnknown);
            break;
        }

	iRequestHandler->NotifyPositionUpdate(posInfo, &aStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsy5Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy5Positioner::CancelNotifyPositionUpdate()
	{
	iRequestHandler->CancelNotifyPositionUpdate();
	}

