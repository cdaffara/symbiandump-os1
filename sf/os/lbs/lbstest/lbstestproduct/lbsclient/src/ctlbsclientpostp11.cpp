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
#include "ctlbsclientpostp11.h"
#include "ctlbsclientobserver.h"
#include <eikenv.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp11::CT_LbsClientPosTp11(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP11 - EPos start and termination");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp11::~CT_LbsClientPosTp11()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp11::CheckEPosActivityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp11::CheckEPosActivityL(TBool aExpectEPosLiveness)
    {
    if (aExpectEPosLiveness && !CT_LbsClientObserver::EPosAliveL())
        {
        _LIT(KError, "Expected EPos server to be alive but it wasn't");
        LogErrorAndLeaveL(KError);
        }
    if (!aExpectEPosLiveness && CT_LbsClientObserver::EPosAliveL())
        {
        _LIT(KError, "Expected EPos server to be dead but it wasn't");
        LogErrorAndLeaveL(KError);
        }
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp11::ServerAliveL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientPosTp11::ServerAliveL(const TDesC& aServerName)
    {
	TFindServer find(aServerName);
    TFullName   name;
    if (find.Next(name) == KErrNone) // server found
		{
		TBuf<100> buf;
		_LIT(KFoundServer, "Found server: ");
		buf.Append(KFoundServer);
		buf.Append(name);
		INFO_PRINTF1(buf);
		return ETrue;
		}
	else 
		{
		return EFalse;
		}
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp11::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp11::StartL()
    {
    const TInt KTimeEPosIsStillActive = 1000000;
    const TInt KTimeEPosHasShutDown = 4000000;
    
    User::After(2000000);

    CT_LbsClientObserver::WaitForEPosShutdown();

    RPositionServer client1;
    RPositionServer client2;
    RPositionServer client3;

    CheckEPosActivityL(EFalse);

    User::LeaveIfError(client1.Connect());
    CleanupClosePushL(client1);
    CheckEPosActivityL(ETrue);

    User::LeaveIfError(client2.Connect());
    CleanupClosePushL(client2);
    CheckEPosActivityL(ETrue);

    User::LeaveIfError(client3.Connect());
    CleanupClosePushL(client3);
    CheckEPosActivityL(ETrue);

    client1.Close();
    CheckEPosActivityL(ETrue);
    client2.Close();
    CheckEPosActivityL(ETrue);
    client3.Close();

    User::After(TTimeIntervalMicroSeconds32(KTimeEPosIsStillActive));
    CheckEPosActivityL(ETrue);
    User::After(TTimeIntervalMicroSeconds32(
        KTimeEPosHasShutDown - KTimeEPosIsStillActive));
    CheckEPosActivityL(EFalse);

    CleanupStack::Pop(3); // client1-3 
    }
