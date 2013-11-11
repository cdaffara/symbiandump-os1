/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
 @file te_locmonitorStep7.cpp
*/
 
#include "te_geolocmonitorStep7.h"
#include "te_geolocmonitorsuitedefs.h"
#include "rlbslocinfoconverter.h"
#include "LocInfoConversionHandler.h"
#include <lbs/lbslocdatasourceclasstypes.h>

// Plug in 
const TInt KDummyPluginId = 537007117;
const TInt KSuplPluginId = 537028467;


CTe_LocMonitorStep7::~CTe_LocMonitorStep7()
	{
	}

CTe_LocMonitorStep7::CTe_LocMonitorStep7()
	{
	SetTestStepName(KLocMonitorStep7);
	}

TVerdict CTe_LocMonitorStep7::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	iConversionHandler = CLocInfoConversionHandler::NewL();
	iConversionHandler1 = CLocInfoConversionHandler::NewL();
	
	return TestStepResult();
	}

//
// This test checks that it is possible to connect a session and
// subsession with the Location Monitor and then close both.
//
TVerdict CTe_LocMonitorStep7::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
        // Test 7 panics with 0, ELocMonitorDuplicateRequest
        RLbsLocInfoConverter_ConvertLocationInfo7L();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep7::doTestStepPostambleL()
	{
    delete iConversionHandler;
    delete iConversionHandler1;
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}




// -----------------------------------------------------------------------------
// CTe_LocMonitorStep7::RLbsLocInfoConverter_ConvertLocationInfo7L
// This test case tests the scenario when user tries to request new batch 
// conversion without waiting for completion of previous one.
// -----------------------------------------------------------------------------
//
TInt CTe_LocMonitorStep7::RLbsLocInfoConverter_ConvertLocationInfo7L()
    {
    INFO_PRINTF1(_L("*** RLbsLocInfoConverter_ConvertLocationInfo7L()"));
    RLbsLocMonitorSession locMonitorSession;
    TInt error = locMonitorSession.Connect();
    if( error != KErrNone )
       {
       return error;
       }
    CleanupClosePushL( locMonitorSession );
    RLbsLocInfoConverter locInfoConverter;
    CleanupClosePushL( locInfoConverter );
    locInfoConverter.OpenL( locMonitorSession );
    
    TUid conversionModuleId = TUid::Uid( KNullUidValue );;
    
    // Here in this case input area doesnt matter 
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray1;
    CleanupClosePushL( locationInfoArray1 );
      
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaWlanInfoClass );
    
    locationInfoArray.Append( locationInfo );
    
    locInfoConverter.ConvertLocationInfoL( 
                locationInfoArray,ELbsConversionNotDefined,
                ELbsConversionOutputPosition,conversionModuleId ,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    locationInfoArray1.Append( locationInfo1 );
    
    TRequestStatus status;
    
    #pragma message("GEOTAGGING TEST ISSUE: How to replicate next lines behaviour: TestModuleIf().SetExitReason(CTestModuleIf::EPanic, 0)")
    // TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0);
    
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray1,ELbsConversionNotDefined,
            ELbsConversionOutputPosition,conversionModuleId,status );
    
    CleanupStack::Pop(4); // locationInfo1,locationInfoArray1,locationInfo,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    locationInfoArray1.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2 ); // locInfoConverter,locMonitorSession
    return KErrNone;
    }   




//  [End of File] 
