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
#include <e32base.h> 
#include "ctlbstestpsy256.h"
#include "testpsy256.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy256::CT_LbsTestPsy256()
    {
        iLongitud = 10.00;
        iLatitud = 11.00;
    }

// EPOC default constructor can leave.
void CT_LbsTestPsy256::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
    }

// Two-phased constructor.
CT_LbsTestPsy256* CT_LbsTestPsy256::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy256* self = new (ELeave) CT_LbsTestPsy256;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
	return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy256* CT_LbsTestPsy256::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsy256* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CT_LbsTestPsy256::~CT_LbsTestPsy256()
    {
    }

// ---------------------------------------------------------
// CT_LbsTestPsy256::NotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy256::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo, 
	TRequestStatus& aStatus)
    {
    // Maximum value for name TESTPSY256_12345678901234567890.rSC
    // Approximately 211 characters
    TInt err = 0;
    
	TPositionInfo* iPosition = static_cast<TPositionInfo*> (&aPosInfo);
	TUid implUid = { KPosImplementationUid };
	iPosition->SetModuleId(implUid);

	TPosition position;
	position.SetCoordinate(iLatitud, iLongitud, iAltitude);
	TTime now;
    now.UniversalTime();
    position.SetTime(now);
	
	iPosition->SetPosition(position);

    TRequestStatus* status = &aStatus;
    User::RequestComplete(status, err);
    }

// ---------------------------------------------------------
// CT_LbsTestPsy256::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy256::CancelNotifyPositionUpdate()
    {
    }

//  End of File
