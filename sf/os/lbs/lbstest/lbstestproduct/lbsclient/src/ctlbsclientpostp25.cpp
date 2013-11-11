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
#include "ctlbsclientpostp25.h"
#include "ctlbsrequester.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>

// CONSTANTS

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// EnablePsyL. Enables a specific psy
// ---------------------------------------------------------
//
void EnablePsyL(
    CPosModules&   aDb,
    TUid             aPsy)
    {
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    // Enable Psy
    moduleUpdate->SetUpdateAvailability(ETrue);
    aDb.UpdateModuleL(aPsy, *moduleUpdate);
    CleanupStack::PopAndDestroy(moduleUpdate);

    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp25::CT_LbsClientPosTp25(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP25 - Request location from multiple PSYs");
	SetTestStepName(KTestName); 
	}

// Destructor
CT_LbsClientPosTp25::~CT_LbsClientPosTp25()
    {
    delete iFastPsy;
    delete iMediumPsy;
    delete iSlowPsy;
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp25::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp25::StartL()
    {
    
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    EnablePsyL(*db, iUidTestPsy12);// 0 sec
    EnablePsyL(*db, iUidTestPsy4);// 3 sec
    EnablePsyL(*db, iUidTestPsy9);// 6 sec

    CleanupStack::PopAndDestroy(); // db

    iFastPsy = CT_LbsRequester::NewL(iUidTestPsy12, this, Logger());   
    iMediumPsy = CT_LbsRequester::NewL(iUidTestPsy4, this, Logger()); 
    iSlowPsy = CT_LbsRequester::NewL(iUidTestPsy9, this, Logger());    
    iCompletedRequests = 0;

    TTime starttime,stoptime;
    starttime.UniversalTime();
    
    // Make request to slow psy first
    iSlowPsy->StartRequest();
    iFastPsy->StartRequest();
    iMediumPsy->StartRequest();

    // Wait for completion
    CActiveScheduler::Start();
    
    stoptime.UniversalTime();
    
    // Substract total delay incurred due to PSY's...i.e 6sec
    TInt duration=stoptime.Int64()-starttime.Int64()-6000000;
    
        
    _LIT(KMessage,"3 simultaneous requests from 3 different sessions completed in %d microsec");
    TBuf<256> error;
    error.Format(KMessage,duration);
    INFO_PRINTF1(error);
    
    if(duration>3000000)
    {
    _LIT(KTime, "Time taken>3 sec");
    INFO_PRINTF1(KTime);
    }
    
       // Evaluate result
    _LIT(KPsyFast, "testpsy12");  
    _LIT(KPsyMedium, "testpsy4"); 
    _LIT(KPsySlow, "testpsy9");   

    _LIT(KReturnedErrorCode, "Positioner %S returned error code %d");
    TBuf<256> output;
    output.Format(KReturnedErrorCode, &KPsyFast, iFastPsy->CompletionCode());
   AssertTrueL(iFastPsy->CompletionCode() == KErrNone, output);

    output.Format(KReturnedErrorCode, &KPsyMedium, iMediumPsy->CompletionCode());
    AssertTrueL(iMediumPsy->CompletionCode() == KErrNone, output);

    output.Format(KReturnedErrorCode, &KPsySlow, iSlowPsy->CompletionCode());
    AssertTrueL(iSlowPsy->CompletionCode() == KErrNone, output);
    _LIT(KWrongComplOrder, "Positioners completed out of order");
    
    AssertTrueL(
        iFastPsy->CompletionTimeUTC() < iMediumPsy->CompletionTimeUTC() &&
        iMediumPsy->CompletionTimeUTC() < iSlowPsy->CompletionTimeUTC(),
        KWrongComplOrder);

    delete iFastPsy;
    delete iMediumPsy;
    delete iSlowPsy;
    iFastPsy = NULL;
    iMediumPsy = NULL;
    iSlowPsy = NULL;

    _LIT(KTEXT, "TP25 Passed! :)");
    INFO_PRINTF1(KTEXT);
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp25::RequestCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp25::RequestCompleteL()
    {
    const TInt KNofRequesters = 3;
    if (++iCompletedRequests == KNofRequesters)
        {
        CActiveScheduler::Stop();
        }
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp25::AssertTrueL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp25::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg)
    {
  	if (!aCondition)
		{
		LogErrorAndLeaveL(aErrorMsg);
		}
    }
