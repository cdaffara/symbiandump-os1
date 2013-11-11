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
#include "ctlbsclientpostp204.h"
#include "ctlbsclientobserver.h"
#include <connect/sbeclient.h>
#include <babackup.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include "ctlbsclientpositionconstants.h"

// CONSTANTS

// Constructor.
CT_LbsClientPosTp204::CT_LbsClientPosTp204(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    { 
    _LIT(KTestName, "TP204 - BackUpTest");
    SetTestStepName(KTestName); 
    }

// Destructor
CT_LbsClientPosTp204::~CT_LbsClientPosTp204()
    {
    }

// ---------------------------------------------------------
// CPosTp215::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp204::CloseTest()
    {
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPosTp204::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp204::StartL()
    {
    
    TDriveList nullDriveList;
	nullDriveList.Zero();
	
	using namespace conn;    
    CSBEClient* SbeClient = CSBEClient::NewL();
    CleanupStack::PushL(SbeClient);
    
    iCompletedRequests = 0;
    
    RPositionServer server;
    CleanupClosePushL(server);
    TInt err = server.Connect();
    if (err != KErrNone)
        {
	  	_LIT(KBackupError1, "Cannot connect to Server");
		INFO_PRINTF1(KBackupError1);				
        User::Leave(KErrGeneral);
        }

  	_LIT(KBackupLog1, "connect to server success");
	INFO_PRINTF1(KBackupLog1);				
	server.Close();
	
    SbeClient->SetBURModeL(nullDriveList, EBURBackupFull, EBackupBase);
	User::After(500000); // Wait for 500 milli seconds
	
    err = server.Connect();
    if (err == KErrNone)
        {
	  	_LIT(KBackupError2, "connect to server works :-(");
		INFO_PRINTF1(KBackupError2);				
        User::Leave(KErrGeneral);
        }

	server.Close();

    SbeClient->SetBURModeL(nullDriveList, EBURNormal, ENoBackup);
	User::After(500000); // Wait for 500 milli seconds
	
    err = server.Connect();
    if (err != KErrNone)
        {
	  	_LIT(KBackupError3, "connect to server doesn't work :-(");
		INFO_PRINTF1(KBackupError3);				
        User::Leave(KErrGeneral);
        }

    CleanupStack::PopAndDestroy(2);
    
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp25::RequestCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp204::RequestCompleteL()
    {
    const TInt KNofRequesters = 3;
    if (++iCompletedRequests == KNofRequesters)
        {
        CActiveScheduler::Stop();
        }
    }

//  End of File
