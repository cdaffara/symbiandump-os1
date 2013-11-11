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
#include "ctlbsclientpostp275.h"
#include <babackup.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp275::CT_LbsClientPosTp275(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{ 
	_LIT(KTestName, "TP275 - BackUpTest");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp275::~CT_LbsClientPosTp275()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp275::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp275::CloseTest()
    {
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp275::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp275::StartL()
    {
    //
    CBaBackupSessionWrapper* backupWrapper = CBaBackupSessionWrapper::NewL();
    CleanupStack::PushL(backupWrapper);
    TBackupOperationAttributes backStart(MBackupObserver::EReleaseLockReadOnly,
        MBackupOperationObserver::EStart);
    backupWrapper->NotifyBackupOperationL(backStart);	
    RPositionServer server;
    CleanupClosePushL(server);
    User::LeaveIfError(server.Connect());
    User::After(2000000);

    TBackupOperationAttributes backEnd(MBackupObserver::ETakeLock,
    MBackupOperationObserver::EEnd);
    backupWrapper->NotifyBackupOperationL(backEnd);
    CleanupStack::PopAndDestroy(2, backupWrapper);    
    }

//  End of File
