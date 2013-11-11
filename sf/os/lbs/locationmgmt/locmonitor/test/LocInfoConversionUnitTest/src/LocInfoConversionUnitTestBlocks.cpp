/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "LocInfoConversionUnitTest.h"
#include "rlbslocmonitorsession.h"
#include "rlbslocinfoconverter.h"
#include "rlbslocmonitorareapositioner.h"
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationcommon.h>
#include "lbsposition.h"


// Plug in 
const TInt KDummyPluginId = 537007117;
const TInt KSuplPluginId = 537028467;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CLocInfoConversionUnitTest::Delete() 
    {
    
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        ENTRY( "UnitTest1", CLocInfoConversionUnitTest::RLbsLocInfoConverter_Connect1L ),
        ENTRY( "UnitTest2", CLocInfoConversionUnitTest::RLbsLocInfoConverter_Connect2L ),
        ENTRY( "UnitTest3", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo1L ),
        ENTRY( "UnitTest4", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo2L ),
        ENTRY( "UnitTest5", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo3L ),
        ENTRY( "UnitTest6", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo4L ),
        ENTRY( "UnitTest7", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo5L ),
        ENTRY( "UnitTest8", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo6L ),
        ENTRY( "UnitTest9", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo7L ),
        ENTRY( "UnitTest10", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo8L ),
        ENTRY( "UnitTest11", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo9L ),
        ENTRY( "UnitTest12", CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo10L ),
        ENTRY( "UnitTest13", CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel1L ),
        ENTRY( "UnitTest14", CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel2L ),
        ENTRY( "UnitTest15", CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel3L ),
        ENTRY( "UnitTest16", CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel4L ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_Connect1
// simple session and subsession.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_Connect1L(CStifItemParser& /*aItem*/ )
   {
   TInt error;
   RLbsLocMonitorSession locMonitorSession;
   error = locMonitorSession.Connect();
   if( error != KErrNone )
       {
       return error;
       }
   CleanupClosePushL( locMonitorSession );
   RLbsLocInfoConverter locInfoConverter;
   CleanupClosePushL( locInfoConverter );
   locInfoConverter.OpenL( locMonitorSession );
   locInfoConverter.Close();
   CleanupStack::PopAndDestroy( 2 ); // locInfoConverter,locMonitorSession 
   return KErrNone;
   }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_Connect2
// Opening multiple subsession from single session
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_Connect2L(CStifItemParser& /*aItem*/ )
   {
   RLbsLocMonitorSession locMonitorSession;
   TInt error = locMonitorSession.Connect();
   if( error != KErrNone )
       {
       return error;
       }
   CleanupClosePushL( locMonitorSession );
   RLbsLocInfoConverter locInfoConverter;
   CleanupClosePushL( locInfoConverter );
   RLbsLocInfoConverter locInfoConverter1;
   CleanupClosePushL( locInfoConverter1 );
   
   locInfoConverter.OpenL( locMonitorSession );
   locInfoConverter1.OpenL( locMonitorSession );
   locInfoConverter.Close();
   locInfoConverter1.Close();
   
   CleanupStack::PopAndDestroy( 3 ); //locInfoConverter1,locInfoConverter and
                                     //locMonitorSession
   return KErrNone;
   }


// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo1L
// This test case tests the simple and corner case related 
// to RLbsLocInfoConverter::ConvertLocationInfoL
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo1L(CStifItemParser& /*aItem*/ )
    {
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
    
    /**
     * Case 1: where NULL pointer is passed instead of location info
     */
    error = KErrNone;
    CLbsLocationInfo* locationInfo = CLbsLocationInfo::NewL();
    CleanupStack::PushL( locationInfo );
    TRAP( error,locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionNotDefined,ELbsConversionOutputPosition,
                  conversionModuleId,iConversionHandler->iStatus ));
    if( error!= KErrArgument )
        {
        User::Leave( KErrArgument );
        }
    
    /**
     * Case 2: where we pass location info.This is
     * just test where this method completes without leaving.
     */ 
    
    // Here we append gsmcell info object to the existing location info object.
    AppendAreaInfoObjectToLocationInfoL( locationInfo,ELbsAreaGsmCellInfoClass );

    locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    locInfoConverter.CancelConvertLocationInfo();
    iConversionHandler->Cancel();
    CleanupStack::PopAndDestroy( locationInfo );
    
    /**
     * Case 3. Empty location info array is passed.
     */  
    error = KErrNone;
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );
    TRAP( error,locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray,ELbsConversionNotDefined,
            ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus ));

    if( error != KErrArgument )
        {
        User::Leave( KErrArgument );
        }
    
    /**
     * Case 4: where we pass location info array.This is 
     * just test where this method completes without leaving.
     */
    
    // Here we create locationInfo object with gsm cell info object contained in it.
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    locationInfoArray.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL( 
                locationInfoArray,ELbsConversionNotDefined,
                ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus  );
    iConversionHandler->Start();
    locInfoConverter.CancelConvertLocationInfo();
    iConversionHandler->Cancel();
    
    CleanupStack::Pop( 2 ); //locationInfo1,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2 ); // locInfoConverter,locMonitorSession
    return KErrNone;
    }   

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo2L
// This test case tests ConvertLocationInfo when expected information is
// of type ECoordinate
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo2L(CStifItemParser& /*aItem*/ )
    {
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
    
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );

    TUid conversionModuleId = TUid::Uid( KDummyPluginId );
    
    /**
     *  case 1: Any information regarding the area is passed and 
     *  coordinate corresponding to that area info is expected.
     */
    
    // Here GSM cell info is passed as input area.
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionNotDefined,ELbsConversionOutputPosition
                  ,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    CActiveScheduler::Start();
    
    error = iConversionHandler->iStatus.Int();
    // If the conversion status is KErrNone, check for the corresponding 
    // converted info.
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo );
        }
    // KErrNotSupported and KErrNotFound are allowed error codes.
    else if( !( error == KErrNotSupported || error == KErrNotFound ) )
        {
        User::Leave( KErrArgument );
        }
   
    /**
     * case 2: Pass any information related to multiple area and we expect coordinate information
     * corresponding to each area information.
     */
   
    // Create one more location info object which holds wlan info for which corresponding coordinate
    // is expected.
    CLbsLocationInfo* locationInfo2 = CreateLocationInfoObjectLC( ELbsAreaWlanInfoClass );
    
    // Before pushing location info object, remove the converted info from it.
    // We had requested for coordinate info, hence we need to clear it before
    // we use it as input for next conversion.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    locationInfoArray.Append( locationInfo );
    
    locationInfoArray.Append( locationInfo2 );
    
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray,ELbsConversionNotDefined,
            ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    error = iConversionHandler->iStatus.Int();
    // Iterate through the array and check for converted info.
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    // KErrNotSupported and KErrNotFound are allowed error codes.
    else if( !( error == KErrNotSupported || error == KErrNotFound ) )
        {
        User::Leave( KErrArgument );
        }
    
    CleanupStack::Pop( 3 ); // locationInfo,locationInfo1,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2 );  // locInfoConverter,locMonitorSession
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo3L
// This test case tests ConvertLocationInfo when expected information is
// of type EGsm
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo3L(CStifItemParser& /*aItem*/ )
    {
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
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );

    TUid conversionModuleId = TUid::Uid( KDummyPluginId );;

    /**
     * case 1: Pass any area info and we expect GSM cell info
     */
    
    // Here WLAN info is passed as input area.
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaWlanInfoClass );
    
    locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionCacheOnly,ELbsConversionOutputGsm,
                  conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    error = iConversionHandler->iStatus.Int();
    // If the conversion status is success, check for the corresponding 
    // converted info.
    if( error == KErrNone )
        {
        CheckGsmCellConvertedInfoL( locationInfo );
        }
    // KErrNotSupported and KErrNotFound are allowed error codes.
    else if( !( error == KErrNotSupported || error == KErrNotFound ) )
        {
        User::Leave( KErrArgument );
        }

    /**
     * case 2: Pass any information related to multiple area and we expect GSM cell info
     * corresponding to each area information.
     */
    
    // Create one more location info object which holds coordinate info for which corresponding GSM
    // cell info is expected.
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaPositionInfoClass );
    
    // Before pushing location info object, remove the converted info from it.
    // We had requested for GSM cell info, hence we need to clear it before
    // we use it for next conversion.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsGsmInfo );
    locationInfoArray.Append( locationInfo );
    locationInfoArray.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray,ELbsConversionCacheOnly,ELbsConversionOutputGsm,
            conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    error = iConversionHandler->iStatus.Int();
    // Iterate through the array and check for converted info.
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckGsmCellConvertedInfoL( locationInfo );
            }
        }
    // KErrNotSupported and KErrNotFound are allowed error codes.
    else if( !( error == KErrNotSupported || error == KErrNotFound ) )
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::Pop( 3 ); // locationInfo,locationInfo1,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2 );  // locInfoConverter,locMonitorSession
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo4L
//This test case tests ConvertLocationInfo when expected information is
// of type EWcdma
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo4L(CStifItemParser& /*aItem*/ )
    {
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
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );
    
    TUid conversionModuleId = TUid::Uid( KDummyPluginId );

    /**
     *  case 1: Pass any area info pointer and we expect EWcdma type
     */
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaPositionInfoClass );
    
    locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionSilent,ELbsConversionOutputWcdma,
                   conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    error = iConversionHandler->iStatus.Int();
    // If the conversion status is success, check for the corresponding 
    // converted info.
    if( error == KErrNone )
        {
        CheckWcdmaCellConvertedInfoL( locationInfo );
        }
    // KErrNotSupported and KErrNotFound are allowed error codes.
    else if( !( error == KErrNotSupported || error == KErrNotFound ) )
        {
        User::Leave( KErrArgument );
        }
  
    /**
     *  case 2: Pass any information related to multiple area and we expect wcdma information
     * corresponding to each area information.
     */
    
    // Create one more location info object which holds wlan info for which corresponding WCDMA 
    // cell info is expected.
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaWlanInfoClass );
    
    // Before pushing location info object, remove the converted info from it.
    // We had requested for WCDMA info, hence we need to clear it before
    // we use it as input for next conversion.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsWcdmaInfo );
    locationInfoArray.Append( locationInfo );
    locationInfoArray.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray,ELbsConversionSilent,ELbsConversionOutputWcdma,
            conversionModuleId,iConversionHandler->iStatus );
    
    iConversionHandler->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    error = iConversionHandler->iStatus.Int();
    // Iterate through the array and check for converted info.
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckWcdmaCellConvertedInfoL( locationInfo );
            }
        }
    // KErrNotSupported and KErrNotFound are allowed error codes.
    else if( !( error == KErrNotSupported || error == KErrNotFound ) )
        {
        User::Leave( KErrArgument );
        }
    
    CleanupStack::Pop( 3 ); // locationInfo,locationInfo1,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2 );  // locInfoConverter,locMonitorSession
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo5L
// This test case tests ConvertLocationInfo when expected information is
// of type EWlan
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo5L(CStifItemParser& /*aItem*/ )
    {
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
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );
    
    TUid conversionModuleId = TUid::Uid( KDummyPluginId );

    /**
     * case 1: Pass any area info pointer and we expect EWlan type
     */
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaWcmdaCellInfoClass );
    
    locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionCacheOnly,ELbsConversionOutputWlan,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // If the conversion status is success, check for the corresponding 
    // converted info.
    if( iConversionHandler->iStatus == KErrNone )
        {
        CheckWlanConvertedInfoL( locationInfo );
        }
    // KErrNotSupported and KErrNotFound are allowed error codes.
    else if( !( iConversionHandler->iStatus == KErrNotSupported || iConversionHandler->iStatus == KErrNotFound ) )
        {
        User::Leave( KErrArgument );
        }
    
   /**
    * Case 2: Pass any information related to multiple area and we expect Wlan info
    * corresponding to each area information.
    */
   
    // Create one more location info object which holds coordinate info for which corresponding 
    // Wlan info is expected.
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaPositionInfoClass );
    
    // Before pushing location info object, remove the converted info from it.
    // We had requested for wlan info, hence we need to clear it before
    // we use it for next conversion.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsWlanInfo );
    locationInfoArray.Append( locationInfo );
    locationInfoArray.Append( locationInfo1 );
    
    TRequestStatus status;
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray,ELbsConversionCacheOnly,ELbsConversionOutputWlan,conversionModuleId,iConversionHandler->iStatus);
    iConversionHandler->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Iterate through the array and check for converted info.
    if( iConversionHandler->iStatus == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckWlanConvertedInfoL( locationInfo );
            }
        }
    // KErrNotSupported and KErrNotFound are allowed error codes.
    else if( !( iConversionHandler->iStatus == KErrNotSupported || iConversionHandler->iStatus == KErrNotFound ) )
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::Pop( 3 ); // locationInfo,locationInfo1,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2 );  // locInfoConverter,locMonitorSession
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo6L
// This test case tests the scenario when user tries to request new single 
// conversion without waiting for completion of previous one.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo6L(CStifItemParser& /*aItem*/ )
    {
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
    TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0);
    locInfoConverter.ConvertLocationInfoL( 
          *locationInfo1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId,status );
    
    CleanupStack::PopAndDestroy( 2 ); // locationInfo1,locationInfo
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo7L
// This test case tests the scenario when user tries to request new batch 
// conversion without waiting for completion of previous one.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo7L(CStifItemParser& /*aItem*/ )
    {
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
    TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0);
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray1,ELbsConversionNotDefined,
            ELbsConversionOutputPosition,conversionModuleId,status );
    
    CleanupStack::Pop(4); // locationInfo1,locationInfoArray1,locationInfo,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    locationInfoArray1.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2 ); // locInfoConverter,locMonitorSession
    return KErrNone;
    }   


// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo8L
// This test case tests the scenario when user tries to request two conversion
// using two subsessions simultaneously specifying same plugin.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo8L(CStifItemParser& /*aItem*/ )
    {
    RLbsLocMonitorSession locMonitorSession;
    TInt error = locMonitorSession.Connect();
    if( error != KErrNone )
       {
       return error;
       }
    CleanupClosePushL( locMonitorSession );
    
    // Here we need to open two subsessions.
    RLbsLocInfoConverter locInfoConverter,locInfoConverter1;
    CleanupClosePushL( locInfoConverter );
    locInfoConverter.OpenL( locMonitorSession );
    
    CleanupClosePushL( locInfoConverter1 );
    locInfoConverter1.OpenL( locMonitorSession );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray1;
    CleanupClosePushL( locationInfoArray1 );
    
    TUid conversionModuleId = TUid::Uid( KDummyPluginId );
    
    /**
     * Case 1: Users tries to request 2 simultaneous single conversion
     *         using 2 subsession specifying same conversion uid.
     */
    
    // Here type of area input doesnt matter.
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    locInfoConverter1.ConvertLocationInfoL( 
          *locationInfo1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId,iConversionHandler1->iStatus );
    iConversionHandler1->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Here we need to wait till both the request completes.Hence we need to block it once again
    // till the 2nd request completes.
    if( iConversionHandler->iStatus.Int() == KRequestPending || 
        iConversionHandler1->iStatus.Int() == KRequestPending )
        {
        CActiveScheduler::Start();
        }
        
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    error = iConversionHandler1->iStatus.Int();
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo1 );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    /**
     * Case 2: Users tries to request 2 simultaneous bulk conversion
     *         using 2 subsession specifying same conversion uid.
     */
    
    // Before we append the location info object to the info array,
    // we need to clear the converted info in each object.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo);
    locationInfo1->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    
    locationInfoArray.Append( locationInfo );
    locationInfoArray1.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL(  
                  locationInfoArray,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    locInfoConverter1.ConvertLocationInfoL( 
            locationInfoArray1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId,iConversionHandler1->iStatus );
    iConversionHandler1->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Here we need to wait till both the request completes.Hence we need to block it once again
    // till the 2nd request completes.
    if( iConversionHandler->iStatus.Int() == KRequestPending || 
        iConversionHandler1->iStatus.Int() == KRequestPending )
        {
        CActiveScheduler::Start();
        }
    
    // Check the converted values in both the conversions.
    // Iterate through the array and check for converted info.
    error = iConversionHandler->iStatus.Int();
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    error = iConversionHandler1->iStatus.Int();
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray1.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray1[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    CleanupStack::Pop(4); // locationInfo1,locationInfoArray1,locationInfo,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    locationInfoArray1.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 3 ); // locInfoConverter,locInfoConverter1,locMonitorSession
    return KErrNone;
    }   


// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo9L
// This test case tests the scenario when user tries to request two conversion
// using two subsessions simultaneously specifying different plugin.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo9L(CStifItemParser& /*aItem*/ )
    {
    RLbsLocMonitorSession locMonitorSession;
    TInt error = locMonitorSession.Connect();
    if( error != KErrNone )
       {
       return error;
       }
    CleanupClosePushL( locMonitorSession );
    
    // Here we need two subsessions
    RLbsLocInfoConverter locInfoConverter,locInfoConverter1;
    CleanupClosePushL( locInfoConverter );
    locInfoConverter.OpenL( locMonitorSession );
    
    CleanupClosePushL( locInfoConverter1 );
    locInfoConverter1.OpenL( locMonitorSession );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray1;
    CleanupClosePushL( locationInfoArray1 );
    
    TUid conversionModuleId1 = TUid::Uid( KDummyPluginId );
    TUid conversionModuleId2 = TUid::Uid( KSuplPluginId );
    
    /**
     * Case 1: Users tries to request 2 simultaneous single conversion
     *         using 2 subsession specifying different conversion uid.
     */
    
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaWcmdaCellInfoClass );
    
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaWlanInfoClass );

    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId1,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    locInfoConverter1.ConvertLocationInfoL( 
          *locationInfo1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId2,iConversionHandler1->iStatus );
    iConversionHandler1->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Here we need to wait till both the request completes.Hence we need to block it once again
    // till the 2nd request completes.
    if( iConversionHandler->iStatus.Int() == KRequestPending || 
        iConversionHandler1->iStatus.Int() == KRequestPending )
        {
        CActiveScheduler::Start();
        }
        
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo );
        }
    else if( !( error == KErrNotFound || error == KErrNotSupported ) )
        {
        User::Leave( KErrArgument );
        }
    
    error = iConversionHandler1->iStatus.Int();
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo1 );
        }
    else if( !( error == KErrNotFound || error == KErrNotSupported ) )
        {
        User::Leave( KErrArgument );
        }
    
    /**
     * Case 2: Users tries to request 2 simultaneous bulk conversion
     *         using 2 subsession specifying same conversion uid.
     */
    
    // Before we append the location info object to the info array,
    // we need to clear the converted info in each object.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    locationInfo1->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    
    locationInfoArray.Append( locationInfo );
    locationInfoArray1.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL(  
                  locationInfoArray,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId1,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    locInfoConverter1.ConvertLocationInfoL( 
            locationInfoArray1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId2,iConversionHandler1->iStatus );
    iConversionHandler1->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Here we need to wait till both the request completes.Hence we need to block it once again
    // till the 2nd request completes.
    if( iConversionHandler->iStatus.Int() == KRequestPending || 
        iConversionHandler1->iStatus.Int() == KRequestPending )
        {
        CActiveScheduler::Start();
        }
    
    // Check the converted values in both the conversions.
    // Iterate through the array and check for converted info.
    error = iConversionHandler->iStatus.Int();
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    else if( !( error == KErrNotFound || error == KErrNotSupported ) )
        {
        User::Leave( KErrArgument );
        }
    
    error = iConversionHandler1->iStatus.Int();
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray1.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray1[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    else if( !( error == KErrNotFound || error == KErrNotSupported ) )
        {
        User::Leave( KErrArgument );
        }
    
    
    CleanupStack::Pop(4); // locationInfo1,locationInfoArray1,locationInfo,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    locationInfoArray1.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 3 ); // locInfoConverter,locInfoConverter1,locMonitorSession
    return KErrNone;
    }   

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo10L
// This test case tests following unsuccessful conversion scenarios:
// 1. Conversion preference not supported by plugin.
// 2. Conversion request by specifying input area type same as requested type. 
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_ConvertLocationInfo10L(CStifItemParser& /*aItem*/ )
    {
    RLbsLocMonitorSession locMonitorSession;
    TInt error = locMonitorSession.Connect();
    if( error != KErrNone )
       {
       return error;
       }
    CleanupClosePushL( locMonitorSession );
    
    // Here we need two subsessions
    RLbsLocInfoConverter locInfoConverter;
    CleanupClosePushL( locInfoConverter );
    locInfoConverter.OpenL( locMonitorSession );
    
    TUid conversionModuleId = TUid::Uid( KDummyPluginId );
    /**
     * Case 1: Users tries to request conversion by specifying conversion
     *         preference not supported by plugin. Example plugin doesnt 
     *         support conversion using cache.
     */
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaWcmdaCellInfoClass );
    
    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionCacheOnly,
                  ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error != KErrNotSupported )
        {
        User::Leave( KErrArgument );
        }

    /**
     * Case 2: Users tries to request for coordinate info by specifying
     *         only coordinate info.
     */
    locationInfo->ResetAreaInfo();
    AppendAreaInfoObjectToLocationInfoL( locationInfo,ELbsAreaPositionInfoClass );
    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error != KErrNotSupported )
        {
        User::Leave( KErrArgument );    
        }
    
    /**
     * Case 3: Users tries to request for gsm cell info by specifying
     *         only gsm cell info.
     */
    locationInfo->ResetAreaInfo();
    AppendAreaInfoObjectToLocationInfoL( locationInfo,ELbsAreaGsmCellInfoClass );
    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputGsm,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error != KErrNotSupported )
        {
        User::Leave( KErrArgument );    
        }
    
    /**
     * Case 4: Users tries to request for wcdma cell info by specifying
     *         only wcdma cell info.
     */
    locationInfo->ResetAreaInfo();
    AppendAreaInfoObjectToLocationInfoL( locationInfo,ELbsAreaWcmdaCellInfoClass );
    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputWcdma,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error != KErrNotSupported )
        {
        User::Leave( KErrArgument );    
        }
    
    /**
     * Case 5: Users tries to request for wlan info by specifying
     *         only wlan info.
     */
    locationInfo->ResetAreaInfo();
    AppendAreaInfoObjectToLocationInfoL( locationInfo,ELbsAreaWlanInfoClass );
    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputWlan,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error != KErrNotSupported )
        {
        User::Leave( KErrArgument );    
        }
    
    CleanupStack::PopAndDestroy( 3 ); // locationInfo,locInfoConverter,locMonitorSession
    return KErrNone;
    }   



// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel1L
// This test case tests the scenario when user tries to cancel the request before 
// and after the conversion request complete for both single and batch conversion.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel1L(CStifItemParser& /*aItem*/ )
    {

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
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );

    TUid conversionModuleId = TUid::Uid( KDummyPluginId );
    
    // This can  can be done for any area type here EGsm to ECoordinate conversion has 
    // been used.
    
    /**
     * Case 1: Here single location information conversion is requested 
     *         and cancel is called before the conversion completes.
     */
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionNotDefined,ELbsConversionOutputPosition
                  ,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    locInfoConverter.CancelConvertLocationInfo();
    iConversionHandler->Cancel();
    
    error = iConversionHandler->iStatus.Int();
    // Request should be completed with the error code KErrCancel and 
    // location info should not have converted information.
    if( error == KErrCancel )
        {
        CheckPositionConvertedInfoL( locationInfo,ENegative );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    /**
     *  case 2: Here single location information conversion is requested
     *          and cancel is called after the conversion completes.
     */
    locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionNotDefined,ELbsConversionOutputPosition
                  ,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    CActiveScheduler::Start();
    locInfoConverter.CancelConvertLocationInfo();
    iConversionHandler->Cancel();
    
    error = iConversionHandler->iStatus.Int();
    // Request should be completed with the error code KErrNone and 
    // location info should have converted information.
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    // case 3: Here mutiple location information conversion is 
    // requested and cancel is called before the conversion completes.

    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );

    // We had requested for coordinate info, hence we need to clear it before
    // we use it as input for next conversion.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    locationInfoArray.Append( locationInfo );
    
    locationInfoArray.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray,ELbsConversionNotDefined,
            ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    locInfoConverter.CancelConvertLocationInfo();
    iConversionHandler->Cancel();
    
    // Request should be completed with the error code KErrCancel and 
    // location info should not have converted information.
    error = iConversionHandler->iStatus.Int();
    
    if( error == KErrCancel )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo,ENegative );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    
    /**
     *  case 4: Here mutiple location information conversion is 
     * requested and cancel is called after the conversion completes.
     */
    
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray,ELbsConversionNotDefined,
            ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    CActiveScheduler::Start();
    locInfoConverter.CancelConvertLocationInfo();
    iConversionHandler->Cancel();
    
    // Request should be completed with the error code KErrNone and 
    // location info should have converted information.
    error = iConversionHandler->iStatus.Int();
    
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::Pop( 3 ); // locationInfo,locationInfo1,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 2 );  // locInfoConverter,locMonitorSession
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel2L
// This test case tests the scenario when user tries to cancel the conversion request
// from the different subsession for both single and multiple conversion.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel2L(CStifItemParser& /*aItem*/ )
    {

    RLbsLocMonitorSession locMonitorSession;
    TInt error = locMonitorSession.Connect();
    if( error != KErrNone )
        {
        return error;
        }
    CleanupClosePushL( locMonitorSession );
    
    // Here we need to subsessions.
    RLbsLocInfoConverter locInfoConverter,locInfoConverter1;
    CleanupClosePushL( locInfoConverter );
    locInfoConverter.OpenL( locMonitorSession );
   
    CleanupClosePushL( locInfoConverter1 );
    locInfoConverter1.OpenL( locMonitorSession );
    
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );

    TUid conversionModuleId = TUid::Uid( KDummyPluginId );
    
    // This can  can be done for any area type here EGsm to ECoordinate conversion has 
    // been used.
    
    /** 
     * Case 1: Here single location information conversion is 
     *         requested and cancel is called the different subsession.
     */
    
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    locInfoConverter.ConvertLocationInfoL( 
                  *locationInfo,ELbsConversionNotDefined,ELbsConversionOutputPosition
                  ,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    locInfoConverter1.CancelConvertLocationInfo();
    CActiveScheduler::Start();
    
    error = iConversionHandler->iStatus.Int();
    // Request should be completed with the error code KErrNone and 
    // location info should have converted information.
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    /**
     * Case 2: Here mutiple location information conversion is requested 
     *         and cancel is called from the different subsession.
     */
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );

    // We had requested for coordinate info, hence we need to clear it before
    // we use it as input for next conversion.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    locationInfoArray.Append( locationInfo );
    
    locationInfoArray.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL( 
            locationInfoArray,ELbsConversionNotDefined,
            ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    locInfoConverter1.CancelConvertLocationInfo();
    CActiveScheduler::Start();
    
    // Request should be completed with the error code KErrNone and 
    // location info should have converted information.
    error = iConversionHandler->iStatus.Int();
    
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::Pop( 3 ); // locationInfo,locationInfo1,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 3 );  // locInfoConverter,locInfoConverter1,locMonitorSession
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel3L
// This test case tests the scenario when user tries to request two conversion
// using two subsessions simultaneously specifying same plugin and we cancel one request
// of two requests.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel3L(CStifItemParser& /*aItem*/ )
    {
    RLbsLocMonitorSession locMonitorSession;
    TInt error = locMonitorSession.Connect();
    if( error != KErrNone )
       {
       return error;
       }
    CleanupClosePushL( locMonitorSession );
    
    // Here we need to open two subsessions.
    RLbsLocInfoConverter locInfoConverter,locInfoConverter1;
    CleanupClosePushL( locInfoConverter );
    locInfoConverter.OpenL( locMonitorSession );
    
    CleanupClosePushL( locInfoConverter1 );
    locInfoConverter1.OpenL( locMonitorSession );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray1;
    CleanupClosePushL( locationInfoArray1 );
    
    TUid conversionModuleId = TUid::Uid( KDummyPluginId );
    
    /**
     * Case 1: Users tries to request 2 simultaneous single conversion
     *         using 2 subsession specifying same conversion uid and 
     *         anyone conversion request is cancelled.
     */
    
    // Here type of area input doesnt matter.
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaGsmCellInfoClass );
    
    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    locInfoConverter1.ConvertLocationInfoL( 
          *locationInfo1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId,iConversionHandler1->iStatus );
    iConversionHandler1->Start();
    
    // Here first request is called.
    locInfoConverter.CancelConvertLocationInfo();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Here we need to wait till both the request completes.Hence we need to block it once again
    // till the 2nd request completes.
    if( iConversionHandler->iStatus.Int() == KRequestPending || 
        iConversionHandler1->iStatus.Int() == KRequestPending )
        {
        CActiveScheduler::Start();
        }
    
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error == KErrCancel )
        {
        CheckPositionConvertedInfoL( locationInfo,ENegative );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    error = iConversionHandler1->iStatus.Int();
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo1 );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    /**
     * Case 2: Users tries to request 2 simultaneous bulk conversion
     *         using 2 subsession specifying same conversion uid and 
     *         any one conversion request is cancelled.
     */
    
    // Before we append the location info object to the info array,
    // we need to clear the converted info in each object.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    locationInfo1->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    
    locationInfoArray.Append( locationInfo );
    locationInfoArray1.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL(  
                  locationInfoArray,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    locInfoConverter1.ConvertLocationInfoL( 
            locationInfoArray1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId,iConversionHandler1->iStatus );
    iConversionHandler1->Start();
    
    // Here second request is called.
    locInfoConverter1.CancelConvertLocationInfo();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Here we need to wait till both the request completes.Hence we need to block it once again
    // till the 2nd request completes.
    if( iConversionHandler->iStatus.Int() == KRequestPending || 
        iConversionHandler1->iStatus.Int() == KRequestPending )
        {
        CActiveScheduler::Start();
        }
    
    // Check the converted values in both the conversions.
    // Iterate through the array and check for converted info.
    error = iConversionHandler->iStatus.Int();
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    error = iConversionHandler1->iStatus.Int();
    if( error == KErrCancel )
        {
        for( TInt i=0;i<locationInfoArray1.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray1[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo,ENegative );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    CleanupStack::Pop(4); // locationInfo1,locationInfoArray1,locationInfo,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    locationInfoArray1.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 3 ); // locInfoConverter,locInfoConverter1,locMonitorSession
    return KErrNone;
    }   

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel4L
// This test case tests the scenario when user tries to request two conversion
// using two subsessions simultaneously specifying different plugin.
// -----------------------------------------------------------------------------
//
TInt CLocInfoConversionUnitTest::RLbsLocInfoConverter_Cancel4L(CStifItemParser& /*aItem*/ )
    {
    RLbsLocMonitorSession locMonitorSession;
    TInt error = locMonitorSession.Connect();
    if( error != KErrNone )
       {
       return error;
       }
    CleanupClosePushL( locMonitorSession );
    
    // Here we need two subsessions
    RLbsLocInfoConverter locInfoConverter,locInfoConverter1;
    CleanupClosePushL( locInfoConverter );
    locInfoConverter.OpenL( locMonitorSession );
    
    CleanupClosePushL( locInfoConverter1 );
    locInfoConverter1.OpenL( locMonitorSession );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray;
    CleanupClosePushL( locationInfoArray );
    
    RPointerArray<CLbsLocationInfoBase> locationInfoArray1;
    CleanupClosePushL( locationInfoArray1 );
    
    TUid conversionModuleId1 = TUid::Uid( KDummyPluginId );
    TUid conversionModuleId2 = TUid::Uid( KSuplPluginId );
    
    /**
     * Case 1: Users tries to request 2 simultaneous single conversion
     *         using 2 subsession specifying different conversion uid
     *         any one request is cancelled.
     */
    
    CLbsLocationInfo* locationInfo = CreateLocationInfoObjectLC( ELbsAreaWcmdaCellInfoClass );
    
    CLbsLocationInfo* locationInfo1 = CreateLocationInfoObjectLC( ELbsAreaWlanInfoClass );

    locInfoConverter.ConvertLocationInfoL(  
                  *locationInfo,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId1,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    
    locInfoConverter1.ConvertLocationInfoL( 
          *locationInfo1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId2,iConversionHandler1->iStatus );
    iConversionHandler1->Start();
    
    // Here first request is cancelled.
    locInfoConverter.CancelConvertLocationInfo();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
   
    // Here we need to wait till both the request completes.Hence we need to block it once again
    // till the 2nd request completes.
    if( iConversionHandler->iStatus.Int() == KRequestPending || 
        iConversionHandler1->iStatus.Int() == KRequestPending )
        {
        CActiveScheduler::Start();
        }
    
    // Check the converted values in both the conversions.
    error = iConversionHandler->iStatus.Int();
    if( error == KErrCancel )
        {
        CheckPositionConvertedInfoL( locationInfo,ENegative );
        }
    else if( !( error == KErrNotFound || error == KErrNotSupported ) )
        {
        User::Leave( KErrArgument );
        }
    
    error = iConversionHandler1->iStatus.Int();
    if( error == KErrNone )
        {
        CheckPositionConvertedInfoL( locationInfo1 );
        }
    else if( !( error == KErrNotFound || error == KErrNotSupported ) )
        {
        User::Leave( KErrArgument );
        }
    
    /**
     * Case 2: Users tries to request 2 simultaneous bulk conversion
     *         using 2 subsession specifying same conversion uid and 
     *         one conversion request is cancelled.
     */
    
    // Before we append the location info object to the info array,
    // we need to clear the converted info in each object.
    locationInfo->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    locationInfo1->ResetAreaInfo( CLbsLocationInfo::ELbsPosInfo );
    
    locationInfoArray.Append( locationInfo );
    locationInfoArray1.Append( locationInfo1 );
    
    locInfoConverter.ConvertLocationInfoL(  
                  locationInfoArray,ELbsConversionNotDefined,
                  ELbsConversionOutputPosition,conversionModuleId1,iConversionHandler->iStatus );
    iConversionHandler->Start();
    
    locInfoConverter1.ConvertLocationInfoL( 
            locationInfoArray1,ELbsConversionNotDefined,
          ELbsConversionOutputPosition,conversionModuleId2,iConversionHandler1->iStatus );
    iConversionHandler1->Start();
    
    // Here we cancel 2nd request 
    locInfoConverter1.CancelConvertLocationInfo();
    
    // This is just to block the control till the call back method is called.
    // Code after this statement will be resumed when Stop on CActiveScheduler
    // is called in the HandleLocationConversion method.
    CActiveScheduler::Start();
    
    // Here we need to wait till both the request completes.Hence we need to block it once again
     // till the 2nd request completes.
     if( iConversionHandler->iStatus.Int() == KRequestPending || 
         iConversionHandler1->iStatus.Int() == KRequestPending )
         {
         CActiveScheduler::Start();
         }
     
    // Check the converted values in both the conversions.
    // Iterate through the array and check for converted info.
    error = iConversionHandler->iStatus.Int();
    if( error == KErrNone )
        {
        for( TInt i=0;i<locationInfoArray.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo );
            }
        }
    else if( !( error == KErrNotFound || error == KErrNotSupported ) )
        {
        User::Leave( KErrArgument );
        }
    
    error = iConversionHandler1->iStatus.Int();
    if( error == KErrCancel )
        {
        for( TInt i=0;i<locationInfoArray1.Count();i++ )
            {
            CLbsLocationInfoBase* locationInfobase = locationInfoArray1[i];
            CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*> ( locationInfobase );
            CheckPositionConvertedInfoL( locationInfo,ENegative );
            }
        }
    else if( !( error == KErrNotFound || error == KErrNotSupported ) )
        {
        User::Leave( KErrArgument );
        }
    
    
    CleanupStack::Pop(4); // locationInfo1,locationInfoArray1,locationInfo,locationInfoArray
    locationInfoArray.ResetAndDestroy();
    locationInfoArray1.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 3 ); // locInfoConverter,locInfoConverter1,locMonitorSession
    return KErrNone;
    }   


// Helper methods

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CreatePositionObjectLC
// -----------------------------------------------------------------------------
//
CLbsPositionInfo* CLocInfoConversionUnitTest::CreatePositionInfoObjectLC()
    {
    TLocality locality;
    locality.SetCoordinate( 62.5285,23.9385 );
    locality.SetAccuracy( 100,0.0 );
    CLbsPositionInfo* positionInfo = CLbsPositionInfo::NewL( locality );
    CleanupStack::PushL( positionInfo );
    return positionInfo;
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CreateGsmCellInfoObjectLC
// -----------------------------------------------------------------------------
//
CLbsGsmCellInfo* CLocInfoConversionUnitTest::CreateGsmCellInfoObjectLC()
    {
    CLbsGsmCellInfo* gsmCellInfo = CLbsGsmCellInfo::NewL( 100,101,102,103);
    CleanupStack::PushL( gsmCellInfo );
    return gsmCellInfo;
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CreateWcdmaCellInfoObjectLC
// -----------------------------------------------------------------------------
//
CLbsWcdmaCellInfo* CLocInfoConversionUnitTest::CreateWcdmaCellInfoObjectLC()
    {
    CLbsWcdmaCellInfo* wcdmaCellInfo = CLbsWcdmaCellInfo::NewL( 100,101,102 );
    CleanupStack::PushL( wcdmaCellInfo );
    return wcdmaCellInfo;
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CreateWlanInfoObjectLC
// -----------------------------------------------------------------------------
//
CLbsWlanInfo* CLocInfoConversionUnitTest::CreateWlanInfoObjectLC()
    {
    TLbsWlanMacAddress macAddress(_L8("10.0.1"));
    CLbsWlanInfo* wlanInfo = CLbsWlanInfo::NewL( macAddress,10.1 );
    CleanupStack::PushL( wlanInfo );
    return wlanInfo;
    }


// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CreateLocationInfoObjectLC
// -----------------------------------------------------------------------------
//
CLbsLocationInfo* CLocInfoConversionUnitTest::CreateLocationInfoObjectLC(                    
                                                    TLbsAreaInfoClassType aAreaType )
    {
    CLbsLocationInfo* locationInfo = CLbsLocationInfo::NewL();
    CleanupStack::PushL( locationInfo );
    AppendAreaInfoObjectToLocationInfoL( locationInfo,aAreaType );
    return locationInfo;
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::AppendAreaInfoObjectToLocationInfoL
// -----------------------------------------------------------------------------
//
void CLocInfoConversionUnitTest::AppendAreaInfoObjectToLocationInfoL(
                                                    CLbsLocationInfo* aLocationInfo,                    
                                                    TLbsAreaInfoClassType aAreaType )
    {
    switch( aAreaType )
        {
        case ELbsAreaPositionInfoClass:
            CLbsPositionInfo* positionInfo = NULL;
            positionInfo = CreatePositionInfoObjectLC();
            aLocationInfo->AddAreaInfoL( positionInfo );
            CleanupStack::Pop( positionInfo );
            break;
        
        case ELbsAreaGsmCellInfoClass:
            CLbsGsmCellInfo* gsmCellInfo = NULL;
            gsmCellInfo = CreateGsmCellInfoObjectLC();
            aLocationInfo->AddAreaInfoL( gsmCellInfo );
            CleanupStack::Pop( gsmCellInfo );
            break;
            
        case ELbsAreaWcmdaCellInfoClass:
            CLbsWcdmaCellInfo* wcdmaCellInfo = NULL;
            wcdmaCellInfo = CreateWcdmaCellInfoObjectLC();
            aLocationInfo->AddAreaInfoL( wcdmaCellInfo );
            CleanupStack::Pop( wcdmaCellInfo );
            break;
            
        case ELbsAreaWlanInfoClass:
            CLbsWlanInfo* wlanInfo = NULL;
            wlanInfo = CreateWlanInfoObjectLC();
            aLocationInfo->AddAreaInfoL( wlanInfo );
            CleanupStack::Pop( wlanInfo );
            break;
        
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CheckPositionConvertedInfoL
// -----------------------------------------------------------------------------
//
void CLocInfoConversionUnitTest::CheckPositionConvertedInfoL( CLbsLocationInfo* 
                                                              aLocationInfo,
                                                              TTestType 
                                                              aTestType )
    {
    RLbsAreaInfoBaseArray areaInfoArray;
    aLocationInfo->GetAreaInfo( areaInfoArray,CLbsLocationInfo::ELbsPosInfo );
    TInt count = areaInfoArray.Count();
    areaInfoArray.Reset();
    if( aTestType == EPositive )
        {
        if( !count )
            {
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            User::Leave( KErrArgument );
            }
        }
    }


// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CheckGsmCellConvertedInfoL
// -----------------------------------------------------------------------------
//
void CLocInfoConversionUnitTest::CheckGsmCellConvertedInfoL( CLbsLocationInfo* 
                                                             aLocationInfo,
                                                             TTestType 
                                                             aTestType )
    {
    RLbsAreaInfoBaseArray areaInfoArray;
    aLocationInfo->GetAreaInfo( areaInfoArray,CLbsLocationInfo::ELbsGsmInfo );
    TInt count = areaInfoArray.Count();
    areaInfoArray.Reset();
    if( aTestType == EPositive )
        {
        if( !count )
            {
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            User::Leave( KErrArgument );
            }
        }
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CheckWcdmaCellConvertedInfoL
// -----------------------------------------------------------------------------
//
void CLocInfoConversionUnitTest::CheckWcdmaCellConvertedInfoL( CLbsLocationInfo* 
                                                               aLocationInfo,
                                                               TTestType 
                                                               aTestType )
    {
    RLbsAreaInfoBaseArray areaInfoArray;
    aLocationInfo->GetAreaInfo( areaInfoArray,CLbsLocationInfo::ELbsWcdmaInfo );
    TInt count = areaInfoArray.Count();
    areaInfoArray.Reset();
    if( aTestType == EPositive )
        {
        if( !count )
            {
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            User::Leave( KErrArgument );
            }
        }
    }

// -----------------------------------------------------------------------------
// CLocInfoConversionUnitTest::CheckWlanConvertedInfoL
// -----------------------------------------------------------------------------
//
void CLocInfoConversionUnitTest::CheckWlanConvertedInfoL( CLbsLocationInfo* 
                                                          aLocationInfo,
                                                          TTestType 
                                                          aTestType )
    {
    RLbsAreaInfoBaseArray areaInfoArray;
    aLocationInfo->GetAreaInfo( areaInfoArray,CLbsLocationInfo::ELbsWlanInfo );
    TInt count = areaInfoArray.Count();
    areaInfoArray.ResetAndDestroy();
    if( aTestType == EPositive )
        {
        if( !count )
            {
            User::Leave( KErrArgument );
            }
        }
    else 
        {
        if( count )
            {
            User::Leave( KErrArgument );
            }
        }
    }
//  [End of File] 
