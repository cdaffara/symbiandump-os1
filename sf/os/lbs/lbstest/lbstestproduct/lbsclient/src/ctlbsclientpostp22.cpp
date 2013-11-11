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
#include "ctlbsclientpostp22.h"
#include "ctlbsclientlog.h"
#include "ctlbsclientperiodictester.h"

// CONSTANTS
_LIT(KServiceName1, " aaaaaaaaaaa");
_LIT(KServiceName2, " bbbbbbbbbbb");
_LIT(KServiceName3, " ccccccccccc");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp22::CT_LbsClientPosTp22(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP22 - Periodic Location Updates");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp22::~CT_LbsClientPosTp22()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp22::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp22::StartL()
    {
    TInt numberOfRuns = 5;
	TTimeIntervalMicroSeconds interval0 = TTimeIntervalMicroSeconds(0);
	TTimeIntervalMicroSeconds interval2 = TTimeIntervalMicroSeconds(2000000);
	TTimeIntervalMicroSeconds interval4 = TTimeIntervalMicroSeconds(4000000);
	TTimeIntervalMicroSeconds interval5 = TTimeIntervalMicroSeconds(5000000);
	TTimeIntervalMicroSeconds interval7 = TTimeIntervalMicroSeconds(7000000);
	TTimeIntervalMicroSeconds interval8 = TTimeIntervalMicroSeconds(8000000);
    TTimeIntervalMicroSeconds timeoutValue = TTimeIntervalMicroSeconds(10000000);
    
    TUid uidWithTimer = iUidTestTimerPsy;
	TUid uidWithoutTimer = iUidTestPsy1;
    TInt errorsFound = KErrNone;
    TBuf<150> buf;

    SetupPsyL(iUidTestTimerPsy);

    // Prepare for test
    iPeriodicTester1 = CT_LbsClientPeriodicTester::NewL(interval0, KServiceName1, iLog, uidWithTimer, 0);
    iPeriodicTester1->RequestNotification();
    CActiveScheduler::Start();
    iPeriodicTester1->ClosePositioner();

	// 1.
    _LIT(KPart1, "<<<<<<<<< PART1 >>>>>>>>>>");
    INFO_PRINTF1(KPart1);
    iPeriodicTester1->OpenPositionerL(KServiceName1);
	iPeriodicTester1->ChangeTestParametersL(interval5, numberOfRuns);

    iPeriodicTester1->RequestNotification();
    CActiveScheduler::Start();
    // Check that correct fix is returned
    TPositionInfo posInfo;
    iPeriodicTester1->GetPosition(posInfo);

    TPosition pos;
    posInfo.GetPosition(pos);
    // TestTimerPsy returns 55.0 for Tracking requests and 20.0 for ordinary position requests
    if (pos.Latitude() != 55.0|| pos.Longitude() != 55.0 || pos.Altitude() != 55.0)
        {
        _LIT(KErrorAndLeave, "Wrong position returned from tracking PSY");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

	iPeriodicTester1->ClosePositioner();
    if (!iPeriodicTester1->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester1 returned error (1): %d");
        buf.Format(KErr,iPeriodicTester1->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

    //Make another ordinary request
    // Not included in TC
    iPeriodicTester1->OpenPositionerL(KServiceName1);
	iPeriodicTester1->ChangeTestParametersL(interval0, 0);
	iPeriodicTester1->RequestNotification();

	CActiveScheduler::Start();
    iPeriodicTester1->GetPosition(posInfo);
    posInfo.GetPosition(pos);
    // TestTimerPsy returns 55.0 for Tracking requests and 20.0 for ordinary position requests
    if (pos.Latitude() != 20.0|| pos.Longitude() != 20.0 || pos.Altitude() != 20.0)
        {
        _LIT(KErrorAndLeave, "Wrong position returned from PSY");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    iPeriodicTester1->ClosePositioner();
    if (!iPeriodicTester1->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester1 returned error (1_1): %d");
        buf.Format(KErr,iPeriodicTester1->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

    //

    // 2.
    _LIT(KPart2, "<<<<<<<<< PART2 >>>>>>>>>>");
    INFO_PRINTF1(KPart2);
	SetupPsyL(uidWithoutTimer);
	iPeriodicTester2 = CT_LbsClientPeriodicTester::NewL(interval5, KServiceName2, iLog, uidWithoutTimer, numberOfRuns);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
	iPeriodicTester2->ClosePositioner();

    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (1): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
	
    //Make another ordinary request
    // Not included in TC
    iPeriodicTester2->OpenPositionerL(KServiceName1);
	iPeriodicTester2->ChangeTestParametersL(interval0, 0);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    iPeriodicTester2->ClosePositioner();
    if (!iPeriodicTester1->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (1_1): %d");
        buf.Format(KErr,iPeriodicTester1->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

    //

	// 3.
    _LIT(KPart3, "<<<<<<<<< PART3 >>>>>>>>>>");
    INFO_PRINTF1(KPart3);
	SetupPsyL(uidWithTimer);
	iPeriodicTester1->OpenPositionerL(KServiceName1);

	iPeriodicTester1->ChangeTestParametersL(interval7, 3);
	iPeriodicTester1->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester1->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester1 returned error (2): %d");
        buf.Format(KErr,iPeriodicTester1->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
	
	iPeriodicTester1->ChangeTestParametersL(interval4, 5);
	iPeriodicTester1->RequestNotification();
	CActiveScheduler::Start();
	iPeriodicTester1->ClosePositioner();

    if (!iPeriodicTester1->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester1 returned error (3): %d");
        buf.Format(KErr,iPeriodicTester1->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

	// 4.
    _LIT(KPart4, "<<<<<<<<< PART4 >>>>>>>>>>");
    INFO_PRINTF1(KPart4);
	SetupPsyL(uidWithoutTimer);
	iPeriodicTester2->OpenPositionerL(KServiceName2);
	iPeriodicTester2->ChangeTestParametersL(interval7, 3);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();

    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (2): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
	
	iPeriodicTester2->ChangeTestParametersL(interval4, 5);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
	iPeriodicTester2->ClosePositioner();

    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (3): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
	
	// 5.
    _LIT(KPart5, "<<<<<<<<< PART5 >>>>>>>>>>");
    INFO_PRINTF1(KPart5);
	SetupPsyL(uidWithTimer);
    iPeriodicTester1->OpenPositionerL(KServiceName1);
	iPeriodicTester1->ChangeTestParametersL(interval4, 3);
	iPeriodicTester1->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester1->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester1 returned error (4): %d");
        buf.Format(KErr,iPeriodicTester1->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

    iPeriodicTester1->ChangeTestParametersL(interval0, 0);

    iPeriodicTester1->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester1->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester1 returned error (4_1): %d");
        buf.Format(KErr,iPeriodicTester1->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
    
    TPositionInfo posInfo1;
    iPeriodicTester1->GetPosition(posInfo1);

    TPosition pos1;
    posInfo1.GetPosition(pos1);
    // TestTimerPsy returns 55.0 for Tracking requests and 20.0 for ordinary position requests
    if (pos1.Latitude() != 20.0 || pos1.Longitude() != 20.0 || pos1.Altitude() != 20.0)
        {
        _LIT(KErrorAndLeave, "Wrong position returned from PSY, StopTracking has not been called");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    iPeriodicTester1->ClosePositioner();

	
	// 6.
    _LIT(KPart6, "<<<<<<<<< PART6 >>>>>>>>>>");
    INFO_PRINTF1(KPart6);
	SetupPsyL(uidWithoutTimer);
    iPeriodicTester2->OpenPositionerL(KServiceName2);
    // Make two requests (note that nr of requests made is 1 + 1, since one is
    // made directly when calling RequestNotification)
    iPeriodicTester2->ChangeTestParametersL(interval0, 1);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    iPeriodicTester2->ClosePositioner();


    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (4): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
		
	// 7.
    _LIT(KPart7, "<<<<<<<<< PART7 >>>>>>>>>>");
    INFO_PRINTF1(KPart7);
    SetupPsyL(uidWithoutTimer);
	iPeriodicTester2->OpenPositionerL(KServiceName2);
    iPeriodicTester2->ChangeTestParametersL(interval4, 3);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (5): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

	iPeriodicTester2->ChangeTestParametersL(interval0, 0);
    iPeriodicTester2->ClosePositioner();
    
	// 8.
    _LIT(KPart8, "<<<<<<<<< PART8 >>>>>>>>>>");
    INFO_PRINTF1(KPart8);
	SetupPsyL(uidWithTimer);
    iPeriodicTester1->OpenPositionerL(KServiceName1);
    // Make two requests (note that nr of requests made is 1 + 1, since one is
    // made directly when calling RequestNotification)
    iPeriodicTester1->ChangeTestParametersL(interval0, 1);
	iPeriodicTester1->RequestNotification();
	CActiveScheduler::Start();

    // Check that correct fix is returned
    iPeriodicTester1->GetPosition(posInfo1);
    posInfo1.GetPosition(pos1);
    // TestTimerPsy returns 55.0 for Tracking requests and 20.0 for ordinary position requests
    if (pos1.Latitude() != 20.0 || pos1.Longitude() != 20.0 || pos1.Altitude() != 20.0)
        {
        _LIT(KErrorAndLeave, "Wrong position returned from PSY, StopTracking has not been called");
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    iPeriodicTester1->ClosePositioner();
    if (!iPeriodicTester1->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester1 returned error (5): %d");
        buf.Format(KErr,iPeriodicTester1->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
    
	// 9.
    _LIT(KPart9, "<<<<<<<<< PART9 >>>>>>>>>>");
    INFO_PRINTF1(KPart9);
	SetupPsyL(uidWithoutTimer);
	iPeriodicTester2->OpenPositionerL(KServiceName2);
    iPeriodicTester2->ChangeTestParametersL(interval4, 3);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (6): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
	
	iPeriodicTester2->ChangeRequestorInformation(KServiceName3);
	iPeriodicTester2->ChangeTestParametersL(interval4, 3);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (7): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

	iPeriodicTester2->ChangeTestParametersL(interval0, 0);
    iPeriodicTester2->ClosePositioner();
    
	// 10.
    _LIT(KPart10, "<<<<<<<<< PART10 >>>>>>>>>>");
    INFO_PRINTF1(KPart10);
	iPeriodicTester2->OpenPositionerL(KServiceName2);
    iPeriodicTester2->ChangeTestParametersL(interval4, 0);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (8): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

	iPeriodicTester2->ChangeTestParametersL(interval0, 0);
    iPeriodicTester2->ClosePositioner();

	// 11.
    _LIT(KPart11, "<<<<<<<<< PART11 >>>>>>>>>>");
    INFO_PRINTF1(KPart11);
	iPeriodicTester2->OpenPositionerL(KServiceName2);
    iPeriodicTester2->ChangeTestParametersL(interval4, 0); //only one periodic
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
	iPeriodicTester2->ClosePositioner();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (9): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

	// 12.
    _LIT(KPart12, "<<<<<<<<< PART12 >>>>>>>>>>");
    INFO_PRINTF1(KPart12);
	iPeriodicTester2->OpenPositionerL(KServiceName2);
	iPeriodicTester2->ChangeTestParametersL(interval8, 2);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (10): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }
	
    // Check that setting MaxAge does not "screw" things up
    iPeriodicTester2->SetMaxAgeL(interval2);

	iPeriodicTester2->ChangeTestParametersL(interval8, 2);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (11): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

	iPeriodicTester2->SetPartialUpdateL(ETrue);
	iPeriodicTester2->ChangeTestParametersL(interval8, 2);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (12): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

	iPeriodicTester2->SetTimeOutL(timeoutValue); //must be greater than the interval
	iPeriodicTester2->ChangeTestParametersL(interval8, 2);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    iPeriodicTester2->ClosePositioner();
    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (13): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

	//13.
    _LIT(KPart13, "<<<<<<<<< PART13 >>>>>>>>>>");
    INFO_PRINTF1(KPart13);
	iPeriodicTester2->OpenPositionerL(KServiceName2);
    iPeriodicTester2->ChangeTestParametersL(interval4, 4);
	iPeriodicTester2->SetChangeParametersPrematurely(ETrue);
	iPeriodicTester2->RequestNotification();
	CActiveScheduler::Start();
    iPeriodicTester2->ClosePositioner();

    if (!iPeriodicTester2->IsCompletedOK())
        {
        _LIT(KErr, "iPeriodicTester2 returned error (8): %d");
        buf.Format(KErr,iPeriodicTester2->GetError());
        INFO_PRINTF1(buf);
        errorsFound++;
        }

    if (errorsFound != KErrNone)
        {
        _LIT(KErrorAndLeave, "Errors found in TP22");
        LogErrorAndLeaveL(KErrorAndLeave);
        }
	}

void CT_LbsClientPosTp22::CloseTest()
	{
	delete iPeriodicTester1;
	iPeriodicTester1 = NULL;
	delete iPeriodicTester2;
	iPeriodicTester2 = NULL;
	iRequestArray.Reset();
	iRequestArray.Close();
	}

// End of file
