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
 @file te_geolocmonitorstepbase.cpp
*/
#include "te_geolocmonitorstepbase.h"
#include "te_geolocmonitorsuitedefs.h"
#include "tserverstartparams.h"
#include "cserverlaunch.h"
#include "lbslocmonitorserverdata.h"
#include "lbsdevloggermacros.h"

void CleanUpAreaInfoArray(TAny* aArray)
    {
	 	static_cast<RLbsAreaInfoBaseArray*>(aArray)->ResetAndDestroy();
    }

TVerdict CTe_LocMonitorStepBase::doTestStepPreambleL()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::doTestStepPreambleL()\n");
	// START MEMORY TEST
	__UHEAP_MARK;

	// Construct a new ActiveScheduler and install it
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LocMonitorStepBase::doTestStepPostambleL()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::doTestStepPostambleL()\n");

	delete iActiveScheduler;
	
	// END MEMORY TEST
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

CTe_LocMonitorStepBase::~CTe_LocMonitorStepBase()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::~CTe_LocMonitorStepBase()\n");
	}

CTe_LocMonitorStepBase::CTe_LocMonitorStepBase()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::CTe_LocMonitorStepBase()\n");
	}


// Helper methods

// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CreatePositionObjectLC
// -----------------------------------------------------------------------------
//
CLbsPositionInfo* CTe_LocMonitorStepBase::CreatePositionInfoObjectLC()
    {
    INFO_PRINTF1(_L("CreatePositionInfoObjectLC()"));
    TLocality locality;
    locality.SetCoordinate( 62.5285,23.9385 );
    locality.SetAccuracy( 100,0.0 );
    CLbsPositionInfo* positionInfo = CLbsPositionInfo::NewL( locality );
    CleanupStack::PushL( positionInfo );
    return positionInfo;
    }

// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CreateGsmCellInfoObjectLC
// -----------------------------------------------------------------------------
//
CLbsGsmCellInfo* CTe_LocMonitorStepBase::CreateGsmCellInfoObjectLC()
    {
    INFO_PRINTF1(_L("CreateGsmCellInfoObjectLC()"));
    CLbsGsmCellInfo* gsmCellInfo = CLbsGsmCellInfo::NewL( 100,101,102,103);
    CleanupStack::PushL( gsmCellInfo );
    return gsmCellInfo;
    }

// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CreateWcdmaCellInfoObjectLC
// -----------------------------------------------------------------------------
//
CLbsWcdmaCellInfo* CTe_LocMonitorStepBase::CreateWcdmaCellInfoObjectLC()
    {
    INFO_PRINTF1(_L("CreateWcdmaCellInfoObjectLC()"));
    CLbsWcdmaCellInfo* wcdmaCellInfo = CLbsWcdmaCellInfo::NewL( 100,101,102 );
    CleanupStack::PushL( wcdmaCellInfo );
    return wcdmaCellInfo;
    }

// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CreateWlanInfoObjectLC
// -----------------------------------------------------------------------------
//
CLbsWlanInfo* CTe_LocMonitorStepBase::CreateWlanInfoObjectLC()
    {
    INFO_PRINTF1(_L("CreateWlanInfoObjectLC()"));
    TLbsWlanMacAddress macAddress(_L8("10.0.1"));
    CLbsWlanInfo* wlanInfo = CLbsWlanInfo::NewL( macAddress,10.1 );
    CleanupStack::PushL( wlanInfo );
    return wlanInfo;
    }


// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CreateLocationInfoObjectLC
// -----------------------------------------------------------------------------
//
CLbsLocationInfo* CTe_LocMonitorStepBase::CreateLocationInfoObjectLC(                    
                                                    TLbsAreaInfoClassType aAreaType )
    {
    INFO_PRINTF1(_L("CreateLocationInfoObjectLC()"));
    CLbsLocationInfo* locationInfo = CLbsLocationInfo::NewL();
    CleanupStack::PushL( locationInfo );
    AppendAreaInfoObjectToLocationInfoL( locationInfo,aAreaType );
    return locationInfo;
    }

// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::AppendAreaInfoObjectToLocationInfoL
// -----------------------------------------------------------------------------
//
void CTe_LocMonitorStepBase::AppendAreaInfoObjectToLocationInfoL(
                                                    CLbsLocationInfo* aLocationInfo,                    
                                                    TLbsAreaInfoClassType aAreaType )
    {
    INFO_PRINTF1(_L("AppendAreaInfoObjectToLocationInfoL()"));

	CLbsPositionInfo* positionInfo = NULL;
	CLbsGsmCellInfo* gsmCellInfo = NULL;
	CLbsWcdmaCellInfo* wcdmaCellInfo = NULL;
	CLbsWlanInfo* wlanInfo = NULL;

	switch( aAreaType )
        {
        case ELbsAreaPositionInfoClass:
            positionInfo = CreatePositionInfoObjectLC();
            aLocationInfo->AddAreaInfoL( positionInfo );
            CleanupStack::Pop( positionInfo );
            break;
        
        case ELbsAreaGsmCellInfoClass:
            gsmCellInfo = CreateGsmCellInfoObjectLC();
            aLocationInfo->AddAreaInfoL( gsmCellInfo );
            CleanupStack::Pop( gsmCellInfo );
            break;
            
        case ELbsAreaWcmdaCellInfoClass:
            wcdmaCellInfo = CreateWcdmaCellInfoObjectLC();
            aLocationInfo->AddAreaInfoL( wcdmaCellInfo );
            CleanupStack::Pop( wcdmaCellInfo );
            break;
            
        case ELbsAreaWlanInfoClass:
            wlanInfo = CreateWlanInfoObjectLC();
            aLocationInfo->AddAreaInfoL( wlanInfo );
            CleanupStack::Pop( wlanInfo );
            break;
        
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CheckPositionConvertedInfoL
// -----------------------------------------------------------------------------
//
void CTe_LocMonitorStepBase::CheckPositionConvertedInfoL( CLbsLocationInfo* 
                                                              aLocationInfo,
                                                              TTestType 
                                                              aTestType )
    {
    INFO_PRINTF1(_L("CheckPositionConvertedInfoL()"));
    RLbsAreaInfoBaseArray areaInfoArray;
    aLocationInfo->GetAreaInfoL( areaInfoArray,CLbsLocationInfo::ELbsPosInfo );
    TInt count = areaInfoArray.Count();
    areaInfoArray.ResetAndDestroy();
 
    if( aTestType == EPositive )
        {
        if( !count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() NOT to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    // and to check out proper ownership of the 
    // heap items do it again!
    
    aLocationInfo->GetAreaInfoL( areaInfoArray,CLbsLocationInfo::ELbsPosInfo );
    count = areaInfoArray.Count();
    areaInfoArray.ResetAndDestroy();
  
    if( aTestType == EPositive )
        {
        if( !count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() NOT to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    }


// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CheckGsmCellConvertedInfoL
// -----------------------------------------------------------------------------
//
void CTe_LocMonitorStepBase::CheckGsmCellConvertedInfoL( CLbsLocationInfo* 
                                                             aLocationInfo,
                                                             TTestType 
                                                             aTestType )
    {
    INFO_PRINTF1(_L("CheckGsmCellConvertedInfoL()"));
    RLbsAreaInfoBaseArray areaInfoArray;
    aLocationInfo->GetAreaInfoL( areaInfoArray,CLbsLocationInfo::ELbsGsmInfo );
    TInt count = areaInfoArray.Count();
    areaInfoArray.ResetAndDestroy();
    if( aTestType == EPositive )
        {
        if( !count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() NOT to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    }

// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CheckWcdmaCellConvertedInfoL
// -----------------------------------------------------------------------------
//
void CTe_LocMonitorStepBase::CheckWcdmaCellConvertedInfoL( CLbsLocationInfo* 
                                                               aLocationInfo,
                                                               TTestType 
                                                               aTestType )
    {
    INFO_PRINTF1(_L("CheckWcdmaCellConvertedInfoL()"));
    RLbsAreaInfoBaseArray areaInfoArray;
    aLocationInfo->GetAreaInfoL( areaInfoArray,CLbsLocationInfo::ELbsWcdmaInfo );
    TInt count = areaInfoArray.Count();
    areaInfoArray.ResetAndDestroy();
    if( aTestType == EPositive )
        {
        if( !count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() NOT to return a value, got %d)"), count);   
            User::Leave( KErrArgument );
            }
        }
    }

// -----------------------------------------------------------------------------
// CTe_LocMonitorStepBase::CheckWlanConvertedInfoL
// -----------------------------------------------------------------------------
//
void CTe_LocMonitorStepBase::CheckWlanConvertedInfoL( CLbsLocationInfo* 
                                                          aLocationInfo,
                                                          TTestType 
                                                          aTestType )
    {
    INFO_PRINTF1(_L("CheckWlanConvertedInfoL()"));
    RLbsAreaInfoBaseArray areaInfoArray;
    aLocationInfo->GetAreaInfoL( areaInfoArray,CLbsLocationInfo::ELbsWlanInfo );
    TInt count = areaInfoArray.Count();
    areaInfoArray.ResetAndDestroy();
    if( aTestType == EPositive )
        {
        if( !count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            INFO_PRINTF2(_L("Expected areaInfoArray.Count() NOT to return a value, got %d)"), count);
            User::Leave( KErrArgument );
            }
        }
    }


