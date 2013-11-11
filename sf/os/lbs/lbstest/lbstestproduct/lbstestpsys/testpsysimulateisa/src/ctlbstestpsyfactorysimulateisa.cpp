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
#include "ctlbstestpsyfactorysimulateisa.h"
#include "ctlbstestpsysimulateisa.h"
#include "EPos_CPositionerSubSession.h"


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CT_LbsTestPsyFactorySimulateIsa::CreateSubSessionL
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPosSubSession* CT_LbsTestPsyFactorySimulateIsa::CreateSubSessionL(
    TUint aType)
	{
    if (aType != KPosCapsPositioner)
        {
        User::Leave(KErrNotSupported);
        }

    CPositionerSubSession* subsession =
         CPositionerSubSession::NewL();
    CleanupClosePushL(*subsession); // subsession inherits CObject

    CT_LbsTestPsySimulateIsa* positioner = CT_LbsTestPsySimulateIsa::NewL();
    subsession->SetPositioner(positioner);

    CleanupStack::Pop(); // subsession
    return subsession;
	}

// ---------------------------------------------------------
// CT_LbsTestPsyFactorySimulateIsa::CreateHandlerL
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPosHandler* CT_LbsTestPsyFactorySimulateIsa::CreateHandlerL(
    TUint aType)
	{
    if (aType != KPosCapsPositioner)
        {
        User::Leave(KErrNotSupported);
        }

    return CT_LbsTestPsySimulateIsa::NewL();
	}


//  End of File
