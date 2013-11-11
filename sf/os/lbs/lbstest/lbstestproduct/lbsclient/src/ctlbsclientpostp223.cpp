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
#include "ctlbsclientpostp223.h"
#include <badesca.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp223::CT_LbsClientPosTp223(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP223 - Range check in PSY");
    SetTestStepName(KTestName); 
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp223::~CT_LbsClientPosTp223()
	{
	}


// ---------------------------------------------------------
// CT_LbsClientPosTp223::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp223::StartL()
    {
  
    _LIT(KErrorMsg, "Get Module did not return KErrNotFound, error code = %d");
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
   
    TPositionModuleInfo moduleInfo;

    TRAPD(err,db->GetModuleInfoL(iUidTestRangePsy, 
                       moduleInfo));

    AssertTrueSecL(err == KErrNotFound,KErrorMsg, err);
 
    CleanupStack::PopAndDestroy(db); // db

	}


// ---------------------------------------------------------
// CT_LbsClientPosTp223::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp223::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf);
		}
    }


