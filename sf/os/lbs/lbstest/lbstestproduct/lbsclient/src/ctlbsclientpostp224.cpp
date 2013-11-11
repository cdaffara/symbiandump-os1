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
#include "ctlbsclientpostp224.h"
#include <lbscommon.h>
#include <lbspositioninfo.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp224::CT_LbsClientPosTp224(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, 
    		"TP224 - MPositionerStatus out of scope after closing PSY subsession");
    SetTestStepName(KTestName); 
    }

// Destructor
CT_LbsClientPosTp224::~CT_LbsClientPosTp224() 
	{
    iPositioner.Close();
    iPositioner2.Close();
    iServer.Close();
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp224::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CT_LbsClientPosTp224::CloseTest()
    {
    iPositioner.Close();
    iPositioner2.Close();
    iServer.Close();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp224::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp224::StartL()
    {   
    _LIT(KConnectErr, "Error when connecting to EPos server, %d");
    _LIT(KOpenErr, "Error when opening positioner, %d");

    TInt err;
    err = iServer.Connect();	
    AssertTrueSecL(err == KErrNone, KConnectErr, err);

	SetupPsyL(iUidTestSingPsy);
    err = iPositioner.Open(iServer, iUidTestSingPsy);
    AssertTrueSecL(err == KErrNone, KOpenErr, err);

    err = iPositioner2.Open(iServer, iUidTestSingPsy);
	AssertTrueSecL(err == KErrNone, KOpenErr, err);

    iPositioner.Close();
    iPositioner2.Close(); // Before error correction -> Panic because of MPositionerStatus deleted  

    err = iPositioner.Open(iServer, iUidTestSingPsy);
    AssertTrueSecL(err == KErrNone, KOpenErr, err);
    err = iPositioner2.Open(iServer, iUidTestSingPsy);
    AssertTrueSecL(err == KErrNone, KOpenErr, err);

    iPositioner2.Close();
    iPositioner.Close();

    iServer.Close();
    }


// ---------------------------------------------------------
// CT_LbsClientPosTp224::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp224::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf);
		}
    }


