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
#include "ctlbsdelayedresponsepsy.h"
#include "ctlbsdelayedresponsepsy.hrh"
#include "epos_mpositionerstatus.h"

// CONSTANTS


// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsDelayedResponsePsy::CT_LbsDelayedResponsePsy()
    {
    }

// EPOC default constructor can leave.
void CT_LbsDelayedResponsePsy::ConstructL(TAny* aConstructionParameters)
	{
	BaseConstructL(aConstructionParameters);
    }

// Two-phased constructor.
CT_LbsDelayedResponsePsy* CT_LbsDelayedResponsePsy::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsDelayedResponsePsy* self = new (ELeave) CT_LbsDelayedResponsePsy;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsDelayedResponsePsy* CT_LbsDelayedResponsePsy::NewL(TAny* aConstructionParameters)
    {
    CT_LbsDelayedResponsePsy* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsDelayedResponsePsy::~CT_LbsDelayedResponsePsy()
    {
	}

// ---------------------------------------------------------
// CT_LbsDelayedResponsePsy::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsDelayedResponsePsy::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
	{
    TPositionInfo* position = NULL;
    position = static_cast<TPositionInfo*> (&aPosInfo);
    iStatus = &aStatus;
    *iStatus = KRequestPending;

  	TUid implUid = { KPosImplementationUid };
	position->SetModuleId(implUid);

	// User passes us the required delay using the input param: 
	iDelay = position->UpdateType();
	
	// Set the position 
	TCoordinate coor(65.0, 65.0, 65.0);
	TLocality loc (coor, 1.0, 1.0);
	TPosition pos (loc, TTime(0));
	position -> SetPosition(pos);

	if(iDelay)
		{
		User::After(iDelay);
		}
	User::RequestComplete(iStatus, KErrNone);
    }


// ---------------------------------------------------------
// CT_LbsDelayedResponsePsy::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsDelayedResponsePsy::CancelNotifyPositionUpdate()
	{
    User::RequestComplete(iStatus, KErrCancel);
	}

// End of file
