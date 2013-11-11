/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "SensrvTest.h"
#include "sensrvsensorchannel.h"
#include "SensrvInternalPSKeys.h"
#include "sensrvchanneldatatypes.h"

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
// CSensrvTest::OpenChannelL
// Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt), 
//              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
// 
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::OpenChannelL( CStifItemParser& aItem )
    {
    RDebug::Print(RThread().Name());
    RDebug::Print( _L("CSensrvTest::OpenChannelL") );
    RDebug::Print( _L("CSensrvTest::OpenChannelL: Get parapeters") );
    
    TSensrvChannelInfo channelInfo;
    
    TInt err = ParseChannelInfo( channelInfo, aItem );
    
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::OpenChannelL: parameter error: %d"), err );
        return err;
        }
    
    iChannelInfoList.Reset();
        
    err = FindChannels( iChannelInfoList, channelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::OpenChannelL: FindChanels error: %d"), err );
        return err;
        }
    if( iChannelInfoList.Count == 0 )
        {
        RDebug::Print( _L("CSensrvTest::FindChannelsL: Channel not found" ) );
        return KErrNotFound;
        }
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTest::OpenChannelL - Create iSensorChannel...") );
        RDebug::Print( _L("CSensrvTest::OpenChannelL - ChannelId = %d "), iChannelInfoList[ 0 ].iChannelId );
        iSensorChannel = CSensrvSensorChannel::NewL( iChannelInfoList[ 0 ] );
        RDebug::Print( _L("CSensrvTest::OpenChannelL - iSensorChannel Created") );
        }
        
    RDebug::Print( _L("CSensrvTest::OpenChannelL - Open channel %d"), iChannelInfoList[ 0 ].iChannelId );
    TRAP( err, iSensorChannel->OpenChannelL() );
    TInt openedChannel( 0 );
    RProperty::Get( KPSUidSensrvTest, KSensrvLatestOpenedChannel, openedChannel );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::OpenChannelL - Open channel error %d"), err );
        }
    else if( openedChannel != iChannelInfoList[ 0 ].iChannelId )
        {
        RDebug::Print( _L("CSensrvTest::OpenChannelL - Incorrect channel opened."), err );
        RDebug::Print( _L("CSensrvTest::OpenChannelL - opened hannel: %d"), openedChannel );
        err = KErrGeneral;
        }
    else
        {
        RDebug::Print( _L("CSensrvTest::OpenChannelL - Channel open") );
        }
   
    return err;
    }
   

// -----------------------------------------------------------------------------
// CSensrvTest::CloseChannelL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CloseChannelL( CStifItemParser& /*aItem*/ )
    {
    RDebug::Print(RThread().Name());
    RDebug::Print( _L("CSensrvTest::CloseChannelL") );
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTest::CloseChannelL - Channel not found. Return KErrNotFound") );
        return KErrNotFound;
        }
    
    RDebug::Print( _L("CSensrvTest::CloseChannelL - Close channel %d"), iChannelInfoList[ 0 ].iChannelId );
    TRAPD( err, iSensorChannel->CloseChannelL() );
    
    TInt closedChannel( 0 );
    RProperty::Get( KPSUidSensrvTest, KSensrvLatestClosedChannel, closedChannel );
    
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CloseChannelL - Close channel error %d"), err );
        }
    else if( closedChannel != iChannelInfoList[ 0 ].iChannelId )
        {
        RDebug::Print( _L("CSensrvTest::CloseChannelL - Incorrect channel closed."), err );
        RDebug::Print( _L("CSensrvTest::OpenChannelL - closed hannel: %d"), iChannelInfoList[ 0 ].iChannelId );
        err = KErrGeneral;
        }
    else
        {
        RDebug::Print( _L("CSensrvTest::CloseChannelL - Channel closed") );
        }

    return err;
    }
    
// -----------------------------------------------------------------------------
// CSensrvTest::OpenChannelL
// Parameters: Open-Close Count(TInt)
// 
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::OpenClosePerformanceL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL") );
    
    // Get parameter
    TInt count;
    TInt err = aItem.GetNextInt( count );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL: parameter error = %d"), err );
        return err;
        }
    
    TBufC8<1> nullBuf( KNullDesC8 );
    TSensrvChannelInfo channelInfo( 0, ESensrvContextNotdefined, ESensrvQuantityNotdefined, 0, nullBuf, nullBuf, 0 );
    iChannelInfoList.Reset();
    err = FindChannels( iChannelInfoList, channelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL: FindChanels error: %d"), err );
        return err;
        }
    if( iChannelInfoList.Count == 0 )
        {
        RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL: Channel not found" ) );
        return KErrNotFound;
        }
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - Create iSensorChannel...") );
        RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - ChannelId = %d "), iChannelInfoList[ 0 ].iChannelId );
        iSensorChannel = CSensrvSensorChannel::NewL( iChannelInfoList[ 0 ] );
        RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - iSensorChannel Created") );
        }
        
    TTime startTime;
    startTime.HomeTime();
    
    for( TInt i = 1; i <= count; ++i )
        {
        TRAP( err, iSensorChannel->OpenChannelL() );
        if( err )
            {
            //RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - Open channel error %d"), err );
            }
            
        TRAP( err, iSensorChannel->CloseChannelL() );
        if( err )
            {
            //RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - Close channel error %d"), err );
            }
        
        //RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - Open-Close [%d]"), i );
        
        }
        
    TTime endTime;
    endTime.HomeTime();  
    
    TTimeIntervalMicroSeconds overallTime =  endTime.MicroSecondsFrom( startTime );
    
    RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - Overall time %d micro seconds"), overallTime );
     
    
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StartDataListening
// Parameters: dataBlockCount, timeout, repeatCount
// 
// -----------------------------------------------------------------------------
//	
TInt CSensrvTest::StartDataListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StartDataListeningL") );
    
    TInt count;
    TInt timeout;
    TInt err = aItem.GetNextInt( count );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL: count parameter error: %d"), err );
        return err;
        }
        
    err = aItem.GetNextInt( timeout );    
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StartDataListeningL- timeout parameter error: %d"),err );
        return err;
        }    
    
    err = aItem.GetNextInt( iRepeatCount );    
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StartDataListeningL- RepeatCount parameter error: %d"),err );
        return err;
        }    
        
    iRepeatCounter = 0;
        
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTest::StartDataListeningL- Channel not found ") );
        return KErrNotFound;
        }
    RDebug::Print( _L("CSensrvTest::StartDataListeningL - count=%d, timeout=%d"), count, timeout );
    TRAP( err, iSensorChannel->StartDataListeningL( this, count, timeout ) );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StartDataListeningL-  StartDataListening error %d"), err );
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StopDataListening
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StopDataListeningL( CStifItemParser& /*aItem*/ )
    {
    RDebug::Print( _L("CSensrvTest::StopDataListeningL") );
    TRAPD( err, iSensorChannel->StopDataListeningL() );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StopDataListeningL-  StopDataListeningL error %d"), err );
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTest::DataReceived
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvTest::DataReceived( CSensrvSensorChannel* aChannel, TInt aCount, TInt aDataLost)
    {
    iRepeatCounter++;
    
    RDebug::Print( _L("CSensrvTest::DataReceived[%d]: aCount = %d, aDataLost = %d"), iRepeatCounter, aCount, aDataLost );
    
    
    for( TInt i = 0; i < aCount; i++ )
        {
        TPckgBuf<TSensrvAccelerometerAxisData> dataBuf;
        iSensorChannel->GetData( dataBuf );
    
        TSensrvAccelerometerAxisData data = dataBuf();
        RDebug::Print( _L(" ### data[%d]: iAxisX = %d, iAxisY = %d, iAxisZ = %d"), i, data.iAxisX, data.iAxisY, data.iAxisZ );
        }
    
    TInt signal;
    RProperty::Get( KPSUidSensrvTest, KSensrSignalAftedDataReseived, signal );
    if( signal )
        {
        Signal();
        }
    else if( iRepeatCounter >= iRepeatCount )
        {
        Signal();
        }
    
    
    }
    
// -----------------------------------------------------------------------------
// CSensrvTest::DataError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvTest::DataError(CSensrvSensorChannel* aChannel, TSensrvErrorSeverity aError)
    {
    RDebug::Print( _L("CSensrvTest::DataError: aError = %d"), aError );
    
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
