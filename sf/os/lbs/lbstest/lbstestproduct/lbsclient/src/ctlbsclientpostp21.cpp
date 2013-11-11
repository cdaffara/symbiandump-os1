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



//  INCLUDES
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include "ctlbsclientpostp21.h"
// CONSTANTS

// ================= MEMBER FUNCTIONS ======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp21::CT_LbsClientPosTp21(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP21 - Enable/Disable PSY");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp21::~CT_LbsClientPosTp21()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp21::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp21::CloseTest()
    {
    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp21::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp21::StartL()
    {
    _LIT(KTestFailedText1, "Unable to open positioner, error code = %d");
    _LIT(KRequestor, "SAAA");

    TUid myPsyId = iUidStubPositioner;

    // Setup - copy files

    ConnectL();

    // Enable myPsyId, disable all others
    SetupPsyL(myPsyId, ETrue);
    // Open subsession to enabled PSY
    TInt error = OpenPositionerByName(myPsyId); // Psy is enabled by default
    if (error != KErrNone)
        {
        TBuf<100> errMsg;
        errMsg.Format(KTestFailedText1, error);
        LogErrorAndLeaveL(errMsg);
        }
    // Make a request
    PerformRequestL(KRequestor, CRequestor::ERequestorService, CRequestor::EFormatTelephone);
    CheckRequestResultL(KErrNone);

    // Disable the PSY and perform another request
	SetupPsyL(myPsyId, EFalse);
	User::After(10000000);
    PerformRequestL(KRequestor, CRequestor::ERequestorService, CRequestor::EFormatTelephone);
	CheckRequestResultL(KErrNotFound);
	
    // Close the subsession and create another one
    ClosePositioner();
        
    error = OpenPositionerByName(myPsyId);
    if (error != KErrNotFound)
        {
        TBuf<100> errMsg;
        errMsg.Format(KTestFailedText1, error);
        LogErrorAndLeaveL(errMsg);
        }
    
    // Enable the module and make a request
	SetupPsyL(myPsyId, ETrue);

	error = OpenPositionerByName(myPsyId);
    PerformRequestL(KRequestor, CRequestor::ERequestorService, CRequestor::EFormatTelephone);
    CheckRequestResultL(KErrNone);

    ClosePositioner();
    Disconnect();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp21::SetupPsyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp21::SetupPsyL(const TUid aPsyUid,
                           TBool aChoice)
	{
	CPosModules* db = CPosModules::OpenL();
	CleanupStack::PushL(db);
	CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
	  
	// Disable PSY
	moduleUpdate->SetUpdateAvailability(EFalse);
	CPosModuleIdList* prioList = db->ModuleIdListLC();
	  
	// Disable all PSY:s
	TInt size = prioList->Count();
	for (TInt i = 0 ; i < size; i++)
		{
	    db->UpdateModuleL((*prioList)[i], *moduleUpdate);
	    }
	
	// Enable/Disable the PSY that came as an in parameter
	if (aChoice) 
		{	
	    moduleUpdate->SetUpdateAvailability(ETrue);
		} 
	else 
		{ 
	    moduleUpdate->SetUpdateAvailability(EFalse);
		}
	
	db->UpdateModuleL(aPsyUid, *moduleUpdate);
	
	CleanupStack::PopAndDestroy(prioList);
	CleanupStack::PopAndDestroy(moduleUpdate);
	CleanupStack::PopAndDestroy(db);

	}

// End of File
