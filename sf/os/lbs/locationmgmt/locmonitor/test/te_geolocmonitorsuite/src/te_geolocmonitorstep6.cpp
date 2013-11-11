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
 @file te_locmonitorStep6.cpp
*/
 
#include "te_geolocmonitorStep6.h"
#include "te_geolocmonitorsuitedefs.h"
#include "rlbslocinfoconverter.h"
#include "LocInfoConversionHandler.h"
#include <lbs/lbslocdatasourceclasstypes.h>

// Plug in 
const TInt KDummyPluginId = 537007117;
const TInt KSuplPluginId = 537028467;


CTe_LocMonitorStep6::~CTe_LocMonitorStep6()
	{
	}

CTe_LocMonitorStep6::CTe_LocMonitorStep6()
	{
	SetTestStepName(KLocMonitorStep6);
	}

TVerdict CTe_LocMonitorStep6::doTestStepPreambleL()
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
TVerdict CTe_LocMonitorStep6::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
         // Test 6  panics with 0, ELocMonitorDuplicateRequest
        RLbsLocInfoConverter_ConvertLocationInfo6L();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep6::doTestStepPostambleL()
	{
    delete iConversionHandler;
    delete iConversionHandler1;
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// CTe_LocMonitorStep6::RLbsLocInfoConverter_ConvertLocationInfo6L
// This test case tests the scenario when user tries to request new single 
// conversion without waiting for completion of previous one.
// -----------------------------------------------------------------------------
//
TInt CTe_LocMonitorStep6::RLbsLocInfoConverter_ConvertLocationInfo6L()
    {
    INFO_PRINTF1(_L("*** RLbsLocInfoConverter_ConvertLocationInfo6L()"));
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
    
    TUid conversionModuleId = TUid::Uid( KDummyPluginId );
    
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    // Here in this case input area doesnt matter 
    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaWcmdaCellInfoClass );
    
    error = KErrNone;
    TRequestStatus status;
    
 
    locInfoConverter.ConvertLocationInfoL( 
          *locationInfo1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId,status );
    
    CleanupStack::PopAndDestroy( 2 ); // locationInfo1,locationInfo
    return KErrNone;
    }


//  [End of File] 
