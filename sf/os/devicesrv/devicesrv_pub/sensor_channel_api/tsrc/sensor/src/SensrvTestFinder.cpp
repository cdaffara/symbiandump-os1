/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "SensrvTest.h"
#include "sensrvsensorchannelfinder.h"
#include "SensrvInternalPSKeys.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSensrvTest::CreateAndDestroyChannelFinderL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CreateAndDestroyChannelFinderL( CStifItemParser& /*aItem*/ )
    {

    RDebug::Print( _L("CSensrvTest::CreateAndDestroyChannelFinderL" ) );
    
    // Create channel finder
    RDebug::Print( _L("CSensrvTest::CreateAndDestroyChannelFinderL: Create iChannelFinder") );
    iChannelFinder = CSensrvSensorChannelFinder::NewL();
    RDebug::Print( _L("CSensrvTest::CreateAndDestroyChannelFinderL: iChannelFinder created") );
        
    RDebug::Print( _L("CSensrvTest::CreateAndDestroyChannelFinderL: Delete iChannelFinder") );
    delete iChannelFinder;
    iChannelFinder = NULL;
    RDebug::Print( _L("CSensrvTest::CreateAndDestroyChannelFinderL: iChannelFinder deleted") );

    return KErrNone;

    }
   
// -----------------------------------------------------------------------------
// CSensrvTest::FindChannelsL
// Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt), 
//              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
// 
// -----------------------------------------------------------------------------
//    
TInt CSensrvTest::FindChannelsL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::FindChannelsL") );

    
    TInt err;
    
    
    RDebug::Print( _L("CSensrvTest::FindChannelsL: Get parapeters") );
    
    TSensrvChannelInfo channelInfo;
    
    err = ParseChannelInfo( channelInfo, aItem );
    
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::FindChannelsL: parameter error: %d"), err );
        return err;
        }
    iChannelInfoList.Reset();
    
    err = FindChannels( iChannelInfoList, channelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::FindChannelsL: FindChanels error: %d"), err );
        return err;
        }
    err = CheckFindResult( channelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::FindChannelsL: CheckFindResult error: %d"), err );
        return err;
        }
    
    
    return KErrNone;
    
    
    }

// -----------------------------------------------------------------------------
// CSensrvTest::FindChannels
// 
// -----------------------------------------------------------------------------
//        
TInt CSensrvTest::FindChannels( RSensrvChannelInfoList& aChannelInfoList, TSensrvChannelInfo& aSearchConditions )
    {
    RDebug::Print( _L("CSensrvTest::FindChanels: Find channels...") );
    if( !iChannelFinder )
        {
        // Create channel finder
        RDebug::Print( _L("CSensrvTest::FindChanels: Create iChannelFinder") );
        iChannelFinder = CSensrvSensorChannelFinder::NewL();
        }
        
    TRAPD( err, iChannelFinder->FindChannelsL( aChannelInfoList, aSearchConditions ) );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::FindChanels: Leave: %d"), err );
        return err;
        }
    RDebug::Print( _L("CSensrvTest::FindChanels: Channel count: %d"), aChannelInfoList.Count() );
    
    return KErrNone;
    }
 

// -----------------------------------------------------------------------------
// CSensrvTest::FindChannelsL
// 
// -----------------------------------------------------------------------------
//    
TInt CSensrvTest::CheckFindResult( TSensrvChannelInfo& aSearchConditions )
    {
    RDebug::Print( _L("CSensrvTest::CheckFindResult") );
    RDebug::Print( _L("### conditions: ChannelId=%d, ContextType=%d, Quantity=%d, ChannelType=%d, Location=%S, VendorId=%S, DataItemSize=%d"),
                   aSearchConditions.iChannelId, aSearchConditions.iContextType, aSearchConditions.iQuantity, aSearchConditions.iChannelType, 
                   &aSearchConditions.iLocation, &aSearchConditions.iVendorId, aSearchConditions.iDataItemSize );
                   
    // Find all channels
    TBufC8<1> nullBuf( KNullDesC8 );
    TSensrvChannelInfo channelInfo( 0, ESensrvContextNotdefined, ESensrvQuantityNotdefined, 0, nullBuf, nullBuf, 0 );
    RSensrvChannelInfoList allChannelList(10);
    TInt err = FindChannels( allChannelList, channelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CheckFindResult: Find all channels Leave: %d"), err );
        return err;
        }
    
    // 
    RSensrvChannelInfoList acceptedChannels(10);
    for( TInt i = 0; i < allChannelList.Count(); i++ )
        {
        if( allChannelList[i].IsMatch( aSearchConditions ) )
            {
            acceptedChannels.Append( allChannelList[i] );
            }
        }
        
    if( acceptedChannels.Count() != iChannelInfoList.Count() )
        {
        // Test fails.
        RDebug::Print( _L("CSensrvTest::CheckFindResult: Test fails") );
        RDebug::Print( _L("   required count %d"), acceptedChannels.Count() );
        RDebug::Print( _L("   found channels count %d"), iChannelInfoList.Count() );
        return KErrGeneral;
        }
     
    TBool testFail( EFalse );
    // Check all channels infos    
    for( TInt i = 0; i < iChannelInfoList.Count(); i++ )
        {
        RDebug::Print( _L("     ###channel info [%d]:"), i );
        RDebug::Print( _L("     ### ChannelId=%d, ContextType=%d, Quantity=%d, ChannelType=%d, Location=%S, VendorId=%S, DataItemSize=%d"),
                   iChannelInfoList[i].iChannelId, iChannelInfoList[i].iContextType, iChannelInfoList[i].iQuantity, iChannelInfoList[i].iChannelType, 
                   &iChannelInfoList[i].iLocation, &iChannelInfoList[i].iVendorId, iChannelInfoList[i].iDataItemSize );
        
        TBool found( EFalse );           
        for( TInt x = 0; x < acceptedChannels.Count(); x++ )
            {
            if( acceptedChannels[x].iChannelId == iChannelInfoList[i].iChannelId )
                {
                found = ETrue;
                break;
                }
            
            }
        if( found )
            {
            RDebug::Print( _L("     -OK-") );
            }
        else
            {
            RDebug::Print( _L("     -FAIL-") );
            testFail = ETrue;
            }
        
        }
    
    if( testFail )
        {
        return KErrGeneral;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CSensrvTest::?member_function( CStifItemParser& aItem )
   {
    // Print to UI
    _LIT( KSensrvTest, "?" );
    _LIT( KTestcase, "?" );
    TestModuleIf().Printf( 0, KSensrvTest, KTestcase );
    // Print to log file
    RDebug::Print( KTestcase );
    
    
    return KErrNone;
   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File