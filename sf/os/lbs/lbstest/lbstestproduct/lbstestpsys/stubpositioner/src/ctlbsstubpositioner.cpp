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
#include <lbspositioninfo.h>
#include "ctlbsstubpositioner.h"
#include "stubpositioner.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsStubPositioner::CT_LbsStubPositioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsStubPositioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
    }



// Two-phased constructor.
CT_LbsStubPositioner* CT_LbsStubPositioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsStubPositioner* self = new (ELeave) CT_LbsStubPositioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsStubPositioner* CT_LbsStubPositioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsStubPositioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsStubPositioner::~CT_LbsStubPositioner()
    {
    }

// ---------------------------------------------------------
// CT_LbsStubPositioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsStubPositioner::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
	{
    TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);

	TUid implUid = { KPosImplementationUid };
	position->SetModuleId(implUid);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }



// ---------------------------------------------------------
// CT_LbsStubPositioner::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsStubPositioner::CancelNotifyPositionUpdate()
	{

	}

