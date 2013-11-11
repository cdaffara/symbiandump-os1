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
#include "ctlbsclientpostp194.h"
#include "ctlbsclientperiodictester.h"

// CONSTANTS
_LIT(KServiceName1, " aaaaaaaaaaa");
_LIT(KServiceName2, " bbbbbbbbbbb");

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp194::CT_LbsClientPosTp194(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP194 - Periodic Updates, Duration test");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp194::~CT_LbsClientPosTp194()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp194::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp194::StartL()
    {
    
	SetupPsyL(iUidMultiPsy);

	TInt numberOfRuns = 100;
	TTimeIntervalMicroSeconds interval5 = TTimeIntervalMicroSeconds(4000000);
	TUid uid = iUidMultiPsy;
	
	CT_LbsClientPeriodicTester* periodicTester1 = CT_LbsClientPeriodicTester::NewL(interval5, KServiceName1, iLog, uid, numberOfRuns);
	CleanupStack::PushL(periodicTester1);

	CT_LbsClientPeriodicTester* periodicTester2 = CT_LbsClientPeriodicTester::NewL(interval5, KServiceName2, iLog, uid, numberOfRuns);
	CleanupStack::PushL(periodicTester2);

	periodicTester1->RequestNotification();
	periodicTester2->RequestNotification();
	
	// Active scheduler is stopped two times.
	CActiveScheduler::Start();
	CActiveScheduler::Start();

    //check for errors
    TBool test1 = periodicTester1 -> IsCompletedOK(); 
    TBool test2 = periodicTester2 -> IsCompletedOK();

    CleanupStack::PopAndDestroy(2,periodicTester1);
    
    if (!test1 || !test2)
        {
        _LIT(KError, "Test did not complete with no error, check log file");
        LogErrorAndLeaveL(KError);
        }

	}

// End of File
