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
#include "ctlbstestsingpsypositioner.h"
#include "ctlbstestsingpsyrequesthandler.h"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestSingPsyPositioner::CT_LbsTestSingPsyPositioner() 
    {

	}

// EPOC default constructor can leave.
void CT_LbsTestSingPsyPositioner::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
	iRequestHandler = CT_LbsTestSingPsyRequestHandler::InstanceL(PositionerStatus());
	}

// Two-phased constructor.
CT_LbsTestSingPsyPositioner* CT_LbsTestSingPsyPositioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestSingPsyPositioner* self = new(ELeave) CT_LbsTestSingPsyPositioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestSingPsyPositioner* CT_LbsTestSingPsyPositioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestSingPsyPositioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
	return self;
    }
    
// Destructor
CT_LbsTestSingPsyPositioner::~CT_LbsTestSingPsyPositioner()
    {
    iRequestHandler->Release();
	}

// ---------------------------------------------------------
// CT_LbsTestSingPsyPositioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestSingPsyPositioner::NotifyPositionUpdate(TPositionInfoBase& /*aPosInfo*/, TRequestStatus& /*aStatus*/)
	{
	
	}

// ---------------------------------------------------------
// CT_LbsTestSingPsyPositioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestSingPsyPositioner::CancelNotifyPositionUpdate()
	{
	
	}

