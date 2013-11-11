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



//  INCLUDES
#include "ctlbsclientpostp274.h"

#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp274::CT_LbsClientPosTp274(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "Tp274-Disable PSY, Epos server crash (ESLI-649HQH)");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp274::~CT_LbsClientPosTp274()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp274::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp274::StartL()
    {
    ConnectL();

    SetupPsyL(iUidMultiPsy);

    User::LeaveIfError(OpenPositionerByName(iUidMultiPsy));

    RPositioner positioner;
    User::LeaveIfError(positioner.Open(iPosServer, iUidMultiPsy));
    CleanupClosePushL(positioner);    

    iPositioner.Close();
    
    CPosModules* modulesDb = CPosModules::OpenL();
    CleanupStack::PushL(modulesDb);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC(); 
    moduleUpdate->SetUpdateAvailability(EFalse);
    modulesDb->UpdateModuleL(iUidMultiPsy, *moduleUpdate);
    CleanupStack::PopAndDestroy(2, modulesDb);

    CleanupStack::PopAndDestroy(&positioner);
    
    iPosServer.Close();

    }

// ---------------------------------------------------------
// CT_LbsClientPosTp274::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp274::CloseTest()
    {
    iPositioner.Close();
    iPosServer.Close();
    }

//  End of File
