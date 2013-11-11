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
#include <e32base.h>
#include "ctlbstestpsysimulateIsa.h"
#include "testpsy.hrh"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsySimulateIsa::CT_LbsTestPsySimulateIsa()
    {
	iLongitud = 10.00;
    iLatitud = 11.00;
	iAltitude = 0;
    }

// EPOC default constructor can leave.
void CT_LbsTestPsySimulateIsa::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
    }

// Two-phased constructor.
CT_LbsTestPsySimulateIsa* CT_LbsTestPsySimulateIsa::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsySimulateIsa* self = new (ELeave) CT_LbsTestPsySimulateIsa;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
	return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsySimulateIsa* CT_LbsTestPsySimulateIsa::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsySimulateIsa* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CT_LbsTestPsySimulateIsa::~CT_LbsTestPsySimulateIsa()
    {
    }

// ---------------------------------------------------------
// CT_LbsTestPsySimulateIsa::NotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsySimulateIsa::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo, 
	TRequestStatus& /*aStatus*/)
    {
    TPositionInfo* iPosition = static_cast<TPositionInfo*> (&aPosInfo);
    TUid implUid = { KPosImplementationUid };
    iPosition->SetModuleId(implUid);
    }

// ---------------------------------------------------------
// CT_LbsTestPsySimulateIsa::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsySimulateIsa::CancelNotifyPositionUpdate()
    {
    }


//  End of File
