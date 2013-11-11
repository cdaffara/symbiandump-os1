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
#include "ctlbsclientpostp93.h"
#include "ctlbsclientperiodictester.h"

// CONSTANTS
_LIT(KServiceName1, " aaaaaaaaaaa");
_LIT(KServiceName2, " bbbbbbbbbbb");
_LIT(KServiceName3, " ccccccccccc");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp93::CT_LbsClientPosTp93(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName,"TP93 - Periodic Updates, Multiple clients");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp93::~CT_LbsClientPosTp93()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp93::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp93::CloseTest()
    {
	iRequestArray.Reset();
	iNameArray.Reset();
	iRequestArray.Close();
	iNameArray.Close();

    }

// ---------------------------------------------------------
// CT_LbsClientPosTp93::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp93::StartL()
    {
	SetupPsyL(iUidMultiPsy);
	
	TPtrC name1(KServiceName1);
	TPtrC name2(KServiceName2);
	TPtrC name3(KServiceName3);
	
	iNameArray.Append(name1);
	iNameArray.Append(name2);
	iNameArray.Append(name3);

	TInt numberOfRuns = 25;
	TTimeIntervalMicroSeconds interval1 = TTimeIntervalMicroSeconds(1000000);
	TTimeIntervalMicroSeconds interval3 = TTimeIntervalMicroSeconds(3000000);
	TTimeIntervalMicroSeconds interval5 = TTimeIntervalMicroSeconds(5000000);
	TTimeIntervalMicroSeconds interval11 = TTimeIntervalMicroSeconds(11000000);
	TTimeIntervalMicroSeconds interval13 = TTimeIntervalMicroSeconds(13000000);
	TTimeIntervalMicroSeconds interval17 = TTimeIntervalMicroSeconds(17000000);

	
	TUid uid = iUidMultiPsy;
    TInt errorsFound = KErrNone;
	
	CT_LbsClientPeriodicTester* periodicTester1 = 
					CT_LbsClientPeriodicTester::NewL(interval1, 
														KServiceName1, 
														iLog, 
														uid, 
														numberOfRuns);
	CleanupStack::PushL(periodicTester1);

	CT_LbsClientPeriodicTester* periodicTester2 = 
					CT_LbsClientPeriodicTester::NewL(interval3, 
														KServiceName2, 
														iLog, 
														uid, 
														numberOfRuns);
	CleanupStack::PushL(periodicTester2);

	CT_LbsClientPeriodicTester* periodicTester3 = 
					CT_LbsClientPeriodicTester::NewL(interval5, 
														KServiceName3, 
														iLog, 
														uid, 
														numberOfRuns);
	CleanupStack::PushL(periodicTester3);

	periodicTester1->RequestNotification();
	periodicTester2->RequestNotification();
	periodicTester3->RequestNotification();

	// Active scheduler is stopped three times.
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	CActiveScheduler::Start();

    if (!periodicTester1->IsCompletedOK() || 
    		!periodicTester2->IsCompletedOK() || 
    		!periodicTester3->IsCompletedOK())
        {
        _LIT(KErr, "ERROR: periodicTester1-3 returned error");
        INFO_PRINTF1(KErr);
        errorsFound++;
        }

	CleanupStack::PopAndDestroy(3, periodicTester1);
	
	CT_LbsClientPeriodicTester* periodicTester4 = 
						CT_LbsClientPeriodicTester::NewL(interval3, 
															KServiceName1, 
															iLog, 
															uid, 
															numberOfRuns);
	CleanupStack::PushL(periodicTester4);

	CT_LbsClientPeriodicTester* periodicTester5 = 
						CT_LbsClientPeriodicTester::NewL(interval3, 
															KServiceName2, 
															iLog, 
															uid, 
															numberOfRuns);
	CleanupStack::PushL(periodicTester5);

	CT_LbsClientPeriodicTester* periodicTester6 = 
						CT_LbsClientPeriodicTester::NewL(interval3, 
															KServiceName3, 
															iLog, 
															uid, 
															numberOfRuns);
	CleanupStack::PushL(periodicTester6);

	periodicTester4->RequestNotification();
	periodicTester5->RequestNotification();
	periodicTester6->RequestNotification();

	// Active scheduler is stopped three times.
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	CActiveScheduler::Start();

    if (!periodicTester4->IsCompletedOK() || 
    		!periodicTester5->IsCompletedOK() || 
    		!periodicTester6->IsCompletedOK())
        {
        _LIT(KErr, "ERROR: periodicTester4-6 returned error");
        INFO_PRINTF1(KErr);
        errorsFound++;
        }

	CleanupStack::PopAndDestroy(3, periodicTester4);
	
	CT_LbsClientPeriodicTester* periodicTester7 = 
						CT_LbsClientPeriodicTester::NewL(interval11, 
															KServiceName1, 
															iLog, 
															uid, 
															numberOfRuns);
	CleanupStack::PushL(periodicTester7);

	CT_LbsClientPeriodicTester* periodicTester8 = 
						CT_LbsClientPeriodicTester::NewL(interval13, 
															KServiceName2, 
															iLog, 
															uid, 
															numberOfRuns);
	CleanupStack::PushL(periodicTester8);

	CT_LbsClientPeriodicTester* periodicTester9 = 
						CT_LbsClientPeriodicTester::NewL(interval17, 
															KServiceName3, 
															iLog, 
															uid, 
															numberOfRuns);
	CleanupStack::PushL(periodicTester9);

	periodicTester7->RequestNotification();
	periodicTester8->RequestNotification();
	periodicTester9->RequestNotification();

	// Active scheduler is stopped three times.
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	CActiveScheduler::Start();

    if (!periodicTester7->IsCompletedOK() || 
    		!periodicTester8->IsCompletedOK() || 
    		!periodicTester9->IsCompletedOK())
        {
        _LIT(KErr, "ERROR : periodicTester7-9 returned error");
        INFO_PRINTF1(KErr);
        errorsFound++;
        }

	CleanupStack::PopAndDestroy(3, periodicTester7);

    if (errorsFound != KErrNone)
        {
        _LIT(KErr, "Errors returned from some periodicTester");
        LogErrorAndLeaveL(KErr);
        }
	}

// End of File
