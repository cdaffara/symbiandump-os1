// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ctlbsstubpositioner.h"
#include "ctlbsstubcommunicator.h"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsDynStubPositioner::CT_LbsDynStubPositioner() : iStubCommunicator(NULL)
    {
    }

// EPOC default constructor can leave.
void CT_LbsDynStubPositioner::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
    iStubCommunicator = CT_LbsStubCommunicator::GetInstanceL();
    }

// Two-phased constructor.
CT_LbsDynStubPositioner* CT_LbsDynStubPositioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsDynStubPositioner* self = new (ELeave) CT_LbsDynStubPositioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }

// Two-phased constructor.
CT_LbsDynStubPositioner* CT_LbsDynStubPositioner::NewL(TAny* aConstructionParameters)
    {
	CT_LbsDynStubPositioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CT_LbsDynStubPositioner::~CT_LbsDynStubPositioner()
    {
	if (iStubCommunicator != NULL)
		{
		iStubCommunicator->Close();
		}
    }

// ---------------------------------------------------------
// CT_LbsDynStubPositioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsDynStubPositioner::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo, 
	TRequestStatus& aStatus)
	{
    iStubCommunicator->NotifyPositionUpdate(aPosInfo,
                                       aStatus);
	}

// ---------------------------------------------------------
// CT_LbsDynStubPositioner::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsDynStubPositioner::CancelNotifyPositionUpdate()
	{
    iStubCommunicator->CancelNotifyPositionUpdate();
	}

//  End of File  
