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
#include "sensrvchannelfinder.h"
#include "sensrvchannel.h"
#include "SensrvInternalPSKeys.h"
#include "sensrvtestclient.h"
#include "sensrvtestcases.h"
#include "sensrvchanneldatatypes.h"
#include "sensrvchannelcondition.h"
//#include "sensrvchannelconditionsetimpl.h"
#include "sensrvtesttypes.h"
#include "sensrvgeneralproperties.h"

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
// CSensrvTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CSensrvTest::Delete()
    {
    RDebug::Print( _L("CSensrvTest::Delete - START") );

    if( iTestCases )
        {
        TRequestStatus status;
        iThread.Logon( status );
        iTestCases->StopTest();
        RDebug::Print( _L("CSensrvTest::Delete - Waiting thread...") );
        User::WaitForRequest( status );
        }


    RDebug::Print( _L("CSensrvTest::Delete - END") );
    }

// -----------------------------------------------------------------------------
// CSensrvTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
            ENTRY( "SetPSValue", CSensrvTest::SetPSValue ),
            ENTRY( "SetCompletedKey", CSensrvTest::SetCompletedKeyL ),
            ENTRY( "SetGetDataDelay", CSensrvTest::SetGetDataDelay ),
            ENTRY( "WaitCompletedKey", CSensrvTest::WaitCompletedKey ),
            ENTRY( "ShutdownServer", CSensrvTest::ShutdownServer ),
            ENTRY( "StartServer", CSensrvTest::StartServer ),
            ENTRY( "CreateAndDestroyChannelFinder", CSensrvTest::CreateAndDestroyChannelFinderL ),
            ENTRY( "FindChannels", CSensrvTest::FindChannelsL ),
            ENTRY( "OpenChannel", CSensrvTest::OpenChannelL ),
            ENTRY( "CloseChannel", CSensrvTest::CloseChannelL ),
            ENTRY( "OpenClosePerformance", CSensrvTest::OpenClosePerformanceL ),
            ENTRY( "StartDataListening", CSensrvTest::StartDataListeningL ),
            ENTRY( "StopDataListening", CSensrvTest::StopDataListeningL ),
            ENTRY( "StartListeningWhenChannelIsNotOpen", CSensrvTest::StartListeningWhenChannelIsNotOpenL ),
            ENTRY( "PanicClient", CSensrvTest::PanicClient ),
            ENTRY( "GetProperty", CSensrvTest::GetPropertyL ),
            ENTRY( "SetProperty", CSensrvTest::SetPropertyL ),
            ENTRY( "StartPropertyListening", CSensrvTest::StartPropertyListeningL ),
            ENTRY( "StopPropertyListening", CSensrvTest::StopPropertyListeningL ),
            ENTRY( "GetAllProperties", CSensrvTest::GetAllPropertiesL ),
            ENTRY( "GetAllPropertiesWithPropertyId", CSensrvTest::GetAllPropertiesWithPropertyIdL ),
            ENTRY( "GetAllPropertiesWithArrayIndex", CSensrvTest::GetAllPropertiesWithArrayIndexL ),
            ENTRY( "WaitSSYUnloading", CSensrvTest::WaitSSYUnloading ),
            ENTRY( "CheckPropertyChangedCount", CSensrvTest::CheckPropertyChangedCount ),
            ENTRY( "ConditionUnitTest", CSensrvTest::ConditionUnitTestL ),
            ENTRY( "ConditionSetUnitTest", CSensrvTest::ConditionSetUnitTestL ),
            ENTRY( "AddCondition", CSensrvTest::AddConditionL ),
            ENTRY( "AddConditionSet", CSensrvTest::AddConditionSetL ),
            ENTRY( "RemoveConditions", CSensrvTest::RemoveConditionsL ),
            ENTRY( "StartConditionListening", CSensrvTest::StartConditionListeningL ),
            ENTRY( "CreateNewProcess", CSensrvTest::CreateNewProcess ),
            ENTRY( "KillProcess", CSensrvTest::KillProcess ),
            ENTRY( "SetProcessPSValue", CSensrvTest::SetProcessPSValue ),
            ENTRY( "StopConditionListening", CSensrvTest::StopConditionListeningL ),
            ENTRY( "CheckConditionMetCount", CSensrvTest::CheckConditionMetCount ),
            ENTRY( "CheckFoundChannelCount", CSensrvTest::CheckFoundChannelCount ),
            ENTRY( "StartChannelChangeListening", CSensrvTest::StartChannelChangeListeningL ),
            ENTRY( "StopChannelChangeListening", CSensrvTest::StopChannelChangeListeningL ),
            ENTRY( "AdjustChannels", CSensrvTest::AdjustChannelsL ),
            ENTRY( "GetPsResult", CSensrvTest::GetPsResult ),
            ENTRY( "StoreFoundChannels", CSensrvTest::StoreFoundChannelsL ),
            ENTRY( "GetPSValue", CSensrvTest::GetPSValue ),
            ENTRY( "CheckListenedSetPropertySuccessIndicator", CSensrvTest::CheckListenedSetPropertySuccessIndicator ),
            ENTRY( "CheckServerAlive", CSensrvTest::CheckServerAlive )
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );


    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CSensrvTest::SetPSValue
// Parameters: SRKey form SensrvInternalPSKeys.h(TUint32), value(TInt)
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::SetPSValue( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::SetPSValue") );

    TUint srKey;
    TInt value;

    TInt err = aItem.GetNextInt( srKey );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::SetPSValue: srKey error: %d"), err );
        return err;
        }

    err = aItem.GetNextInt( value );
    if( err )
        {
        RDebug::Print(  _L("CSensrvTest::SetPSValue: value error: %d"), err );
        return err;
        }

    RDebug::Print( _L("CSensrvTest::SetPSValue - RProperty::Set( %d, %d )"), srKey, value );

    return RProperty::Set( KPSUidSensrvTest, srKey, value );

    }

// -----------------------------------------------------------------------------
// CSensrvTest::SetPSValue to test process
// Parameters:
//             TUInt   - process ID
//             TUInt   - command
//             TInt    - expected result
//             TInt    - PropertyId
//             TInt    - Property index
//             TInt    - Property value
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::SetProcessPSValue( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::SetProcessPSValue") );

    TInt  valueTInt;
    TUint valueTUInt;
    TPckgBuf< TSensrvProcessData > buf;
/*
    TInt err = aItem.GetNextInt( srKey );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::SetProcessPSValue: srKey error: %d"), err );
        return err;
        }
*/
    TInt err = aItem.GetNextInt( valueTUInt );
    if( err )
        {
        RDebug::Print(  _L("CSensrvTest::SetProcessPSValue: value error1: %d"), err );
        return err;
        }
    buf().iProcessID = valueTUInt;

    err = aItem.GetNextInt( valueTUInt );
    if( err )
        {
        RDebug::Print(  _L("CSensrvTest::SetProcessPSValue: value error2: %d"), err );
        return err;
        }
    buf().iCommand = valueTUInt;

    err = aItem.GetNextInt( valueTInt );
    if( err )
        {
        RDebug::Print(  _L("CSensrvTest::SetProcessPSValue: value error3: %d"), err );
        return err;
        }
    buf().iExpectedResult = valueTInt;


    //Property
    TInt propertyId(0);
    TInt propertyIndex(0);
    TInt propertyValue(0);

    err = aItem.GetNextInt( propertyId );
    err = aItem.GetNextInt( propertyIndex );

    if( !err )
        {
        if( propertyId == KSensrvPropIdDataRate )
            {
            aItem.GetNextInt( propertyValue );
            buf().iProperty.SetPropertyId( KSensrvPropIdDataRate );
            buf().iProperty.SetValue( propertyValue );
            buf().iProperty.SetItemIndex( propertyIndex );
            }
        else if( propertyId == KSensrvPropIdAvailability )
            {
            aItem.GetNextInt( propertyValue );
            buf().iProperty.SetPropertyId( KSensrvPropIdAvailability  );
            buf().iProperty.SetValue( propertyValue );
            buf().iProperty.SetItemIndex( propertyIndex );
            }
        else
            {
            RDebug::Print(  _L("CSensrvTest::SetProcessPSValue: property set to zero") );
            buf().iProperty.SetPropertyId( 0 );
            buf().iProperty.SetValue( 0 );
            buf().iProperty.SetItemIndex( 0 );
            }
        }


    return RProperty::Set( KPSUidSensrvTest, KSensrvTestProcessCommand, buf );
    }

// -----------------------------------------------------------------------------
// CSensrvTest::SetCompletedKey
// Parameters: SRKey form SensrvInternalPSKeys.h(TUint32)
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::SetCompletedKeyL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::SetCompletedKey") );
    TInt err = aItem.GetNextInt( iCompletedKey );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::SetCompletedKey:  error: %d"), err );
        }

    DefinePSKeyL( iCompletedKey, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrCompletion );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::SetGetDataDelay
// Parameters: Delay in millisecond
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::SetGetDataDelay( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::SetGetDataDelay") );
    TInt delay;
    TInt err = aItem.GetNextInt( delay );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::SetGetDataDelay:  error: %d"), err );
        return err;
        }

    err = CheckTest();

    if( err == KErrNone )
        {
        RDebug::Print( _L("CSensrvTest::SetGetDataDelay: delay: %d"), delay );
        iTestCases->SetGetDataDelay( delay );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::WaitCompletedKey
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::WaitCompletedKey( CStifItemParser& /*aItem*/ )
    {
    RDebug::Print( _L("CSensrvTest::WaitCompletedKey") );
    TInt err( KErrNone );
    TInt ret( KErrNone );
    if( iTestCases == NULL )
        {
        RDebug::Print( _L("CSensrvTest::WaitCompletedKey: ERROR - iTestCases = NULL") );
        ret = KErrGeneral;
        }
    else
        {
        iTestCases->Mutex().Wait();
        err = RProperty::Get( KPSUidSensrvTest, iCompletedKey, ret );

        if( err == KErrNone )
            {
            if( ret == KErrCompletion )
                {
                RProperty complete;
                err = complete.Attach( KPSUidSensrvTest, iCompletedKey );
                if( err == KErrNone )
                    {
                    // wait for test case complete
                    TRequestStatus status;
                    complete.Subscribe( status );
                    iTestCases->Mutex().Signal();
                    RDebug::Print( _L("CSensrvTest::WaitCompletedKey - Waiting...") );
                    User::WaitForRequest( status );
                    RDebug::Print( _L("CSensrvTest::WaitCompletedKey - Waiting completed") );
                    complete.Get( ret );
                    }
                else
                    {
                    RDebug::Print( _L("CSensrvTest::WaitCompletedKey:  Attach error: %d"), err );
                    ret = err;
                    iTestCases->Mutex().Signal();
                    }
                }
            else
                {
                RDebug::Print( _L("CSensrvTest::WaitCompletedKey:  Test completed: %d"), ret );
                iTestCases->Mutex().Signal();
                }
            }
        else
            {
            RDebug::Print( _L("CSensrvTest::WaitCompletedKey:  Get error: %d"), err );
            ret = err;
            iTestCases->Mutex().Signal();
            }
        }

    // Clear completion value
    RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrCompletion );
    RDebug::Print( _L("CSensrvTest::WaitCompletedKey - return %d"), ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::ShutdownServer
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::ShutdownServer( CStifItemParser& /*aItem*/ )
    {
    RDebug::Print( _L("CSensrvTest::ShutdownServer") );

    RSensrvTestClient testClient;
    TInt err( testClient.Connect() );
    if( err == KErrNone )
        {
        err = testClient.ShutdownServer();
        testClient.Close();
        }
    else
        {
        err = KErrNone;
        }


    RDebug::Print( _L("CSensrvTest::ShutdownServer - return %d " ), err );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StartServer
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StartServer( CStifItemParser& /*aItem*/ )
    {
    RDebug::Print( _L("CSensrvTest::StartServer") );
    TInt err( KErrNone );
    RSensrvTestClient testClient;
    if( testClient.Connect() )
        {
        RDebug::Print( _L("CSensrvTest::StartServer - Start server...") );
        err = testClient.StartServer();
        if( err )
            {
            RDebug::Print( _L("CSensrvTest::StartServer - Start server error: %d"), err );
            }
        else
            {
            RDebug::Print( _L("CSensrvTest::StartServer - Server started") );
            }
        }
    else
        {
        RDebug::Print( _L("CSensrvTest::StartServer - Server already started") );
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::CheckTest
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CheckTest()
    {
    TInt ret( KErrNone );
    if( !iTestCases )
        {
        // Wait second
        User::After( 1000000 );
        if( !iTestCases )
            {
            ret = KErrNotReady;
            }
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CSensrvTest::DoRunAndWaitTest
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::DoRunAndWaitTest( CSensrvTestCases::TSensrvTestCase aTestCase,
                                        CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::DoRunAndWaitTest") );
    RProperty complete;
    TInt waitCompletion;
    TInt err( KErrNone );
    TRequestStatus status;
    RProperty::Get( KPSUidSensrvTest, KSensrvWaitTestCompletion, waitCompletion );
    if( waitCompletion )
        {
        err = complete.Attach( KPSUidSensrvTest, iCompletedKey );
        if( err )
            {
            RDebug::Print( _L("CSensrvTest::DoRunAndWaitTest - Error RProperty::Attach: %d "), err );
            return err;
            }

        complete.Subscribe( status );
        }


    // Run test
    RDebug::Print( _L("CSensrvTest::DoRunAndWaitTest - Start Test Case") );
    err = iTestCases->StartTestCase( aTestCase, aItem, iCompletedKey );

    if( err )
        {
        RDebug::Print( _L("CSensrvTest::DoRunAndWaitTest - StartTestCase: Error = %d"), err );
        if( waitCompletion )
            {
            complete.Cancel();
            }
        return err;
        }



    if( waitCompletion )
        {
        RDebug::Print( _L("CSensrvTest::DoRunAndWaitTest - Waiting...") );
        User::WaitForRequest( status );

        complete.Get( err );

        RDebug::Print( _L("CSensrvTest::DoRunAndWaitTest - Test completed: %d"), err );

        // Clear completion value
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrCompletion );
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTest::CreateAndDestroyChannelFinderL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CreateAndDestroyChannelFinderL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::CreateAndDestroyChannelFinderL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CreateAndDestroyChannelFinderL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::ECreateAndDestroyChannelFinder, aItem );

    RDebug::Print( _L("CSensrvTest::CreateAndDestroyChannelFinderL - Completed: %d"), err );

    return err;

    }

// -----------------------------------------------------------------------------
// CSensrvTest::FindChannelsL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::FindChannelsL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::FindChannelsL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::FindChannelsL - Test creation error %d"), err );
        return err;
        }


    err = DoRunAndWaitTest( CSensrvTestCases::EFindChannels, aItem );

    RDebug::Print( _L("CSensrvTest::FindChannelsL - Completed: %d"), err );

    return err;

    }

// -----------------------------------------------------------------------------
// CSensrvTest::OpenChannelL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::OpenChannelL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::OpenChannelL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::OpenChannelL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EOpenChannel, aItem );

    RDebug::Print( _L("CSensrvTest::OpenChannelL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::CloseChannelL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CloseChannelL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::CloseChannelL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CloseChannelL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::ECloseChannel, aItem );

    RDebug::Print( _L("CSensrvTest::CloseChannelL - Completed %d"), err );

    return err;
    }
// -----------------------------------------------------------------------------
// CSensrvTest::OpenClosePerformanceL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::OpenClosePerformanceL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EOpenClosePerformance, aItem );

    RDebug::Print( _L("CSensrvTest::OpenClosePerformanceL - Completed: %d"), err );

    return err;

    }

// -----------------------------------------------------------------------------
// CSensrvTest::StartDataListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StartDataListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StartDataListeningL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StartDataListeningL - Test creation error %d"), err );
        return err;
        }



    err = DoRunAndWaitTest( CSensrvTestCases::EStartDataListening, aItem );

    RDebug::Print( _L("CSensrvTest::StartDataListeningL - Completed: %d"), err );


    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StopDataListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StopDataListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StopDataListeningL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StopDataListeningL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EStopDataListening, aItem );

    RDebug::Print( _L("CSensrvTest::StopDataListeningL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StopDataListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StartListeningWhenChannelIsNotOpenL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StartListeningWhenChannelIsNotOpenL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StartListeningWhenChannelIsNotOpenL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EStartListeningWhenChannelIsNotOpen, aItem );

    RDebug::Print( _L("CSensrvTest::StartListeningWhenChannelIsNotOpenL - Completed: %d"), err );

    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTest::PanicClient
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::PanicClient( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::PanicClient") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::PanicClient - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EPanicClient, aItem );

    RDebug::Print( _L("CSensrvTest::PanicClient - Completed: %d"), err );

    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTest::WaitSSYUnloading
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::WaitSSYUnloading( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::WaitSSYUnloading") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::WaitSSYUnloading - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EWaitSSYUnloading, aItem );

    RDebug::Print( _L("CSensrvTest::WaitSSYUnloading - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::CreateNewThread
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CreateNewThread()
    {
    RDebug::Print( _L("CSensrvTest::CreateNewThread") );
    TTime now;
    now.HomeTime();

    _LIT( KSensrvTestThreadName, "SenSrvTestThread[%LD]" );

    TBuf<50> threadName;
    threadName.Format( KSensrvTestThreadName, now.Int64() );

    RDebug::Print( _L("CSensrvTest::CreateNewThread: threadName = %S"), &threadName );

    TInt err = iThread.Create(threadName,
                                StartThread,
                                0x2000,
                                &(User::Heap()),
                                (TAny*)this);
    if( err == KErrAlreadyExists )
        {
        User::After( 100000 ); // Wait 100 ms
        now.HomeTime();
        threadName.Format( KSensrvTestThreadName, now.Int64() );
        RDebug::Print( _L("CSensrvTest::CreateNewThread: newthreadName = %S"), &threadName );
        err = iThread.Create(threadName,
                                StartThread,
                                0x2000,
                                &(User::Heap()),
                                (TAny*)this);
        }

    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CreateNewThread: Thread creation error: %d"), err );
        return err;
        }
    iThread.SetPriority( EPriorityNormal );

    iThread.Resume();

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CSensrvTest::StartThread
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StartThread( TAny* aParameter )
    {
    RDebug::Print( _L("CSensrvTest::StartThread") );
    CSensrvTest* test = static_cast<CSensrvTest*>(aParameter);

    TRAPD( err, test->DoStartThreadL() );

    return err;

    }

// -----------------------------------------------------------------------------
// CSensrvTest::DoStartThreadL
//
// -----------------------------------------------------------------------------
//
void CSensrvTest::DoStartThreadL()
    {
    // Create new cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();

    CActiveScheduler* scheduler = new( ELeave ) CActiveScheduler();
    CActiveScheduler::Install( scheduler );

    TRAPD( err, iTestCases = CSensrvTestCases::NewL() );

    if( !err )
        {
        CActiveScheduler::Start();
        }

    // Cleanup
    delete scheduler;
    scheduler = NULL;

    delete cleanup;
    cleanup = NULL;

    delete iTestCases;
    iTestCases = NULL;
    RDebug::Print( _L("CSensrvTest::DoStartThreadL - END") );
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StartPropertyListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StartPropertyListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StartPropertyListeningL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StartPropertyListeningL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EStartPropertyListening, aItem );

    RDebug::Print( _L("CSensrvTest::StartPropertyListeningL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StopPropertyListeningL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StopPropertyListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StopPropertyListeningL") );
    TInt err = CheckTest();

    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StopPropertyListeningL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EStopPropertyListening, aItem );

    RDebug::Print( _L("CSensrvTest::StopPropertyListeningL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::GetPropertyL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::GetPropertyL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::GetPropertyL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::GetPropertyL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EGetProperty, aItem );

    RDebug::Print( _L("CSensrvTest::GetPropertyL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::SetPropertyL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::SetPropertyL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::SetPropertyL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::SetPropertyL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::ESetProperty, aItem );

    RDebug::Print( _L("CSensrvTest::SetPropertyL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::GetAllPropertiesL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::GetAllPropertiesL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::GetAllPropertiesL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::GetAllPropertiesL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EGetAllProperties, aItem );

    RDebug::Print( _L("CSensrvTest::GetAllPropertiesL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::GetAllPropertiesWithPropertyIdL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::GetAllPropertiesWithPropertyIdL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::GetAllPropertiesWithPropertyIdL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::GetAllPropertiesWithPropertyIdL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EGetAllPropertiesWithPropertyIdL, aItem );

    RDebug::Print( _L("CSensrvTest::GetAllPropertiesWithPropertyIdL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::GetAllPropertiesWithArrayIndexL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::GetAllPropertiesWithArrayIndexL( CStifItemParser& aItem )
     {
    RDebug::Print( _L("CSensrvTest::GetAllPropertiesWithArrayIndexL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::GetAllPropertiesWithArrayIndexL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EGetAllPropertiesWithArrayIndexL, aItem );

    RDebug::Print( _L("CSensrvTest::GetAllPropertiesWithArrayIndexL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::CheckPropertyChangedCount
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CheckPropertyChangedCount( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::CheckPropertyChangedCount") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CheckPropertyChangedCount - Test creation error %d"), err );
        return err;
        }

    TInt expectedCount( 0 );
    err = aItem.GetNextInt( expectedCount );
    if( err )
        {
        if( iTestCases->PropertyChangedCount() > 0 )
            {
            err = KErrNone;
            }
        else
            {
            RDebug::Print( _L("CSensrvTest::CheckPropertyChangedCount - Incorrect propertyChangedCount %d"), iTestCases->PropertyChangedCount() );
            err = KErrGeneral;
            }
        }
    else
        {
        if( iTestCases->PropertyChangedCount() == expectedCount )
            {
            err = KErrNone;
            }
        else
            {
            RDebug::Print( _L("CSensrvTest::CheckPropertyChangedCount - Incorrect propertyChangedCount %d"), iTestCases->PropertyChangedCount() );
            err = KErrGeneral;
            }
        }

    iTestCases->ClearPropertyChangedCount();

    RDebug::Print( _L("CSensrvTest::CheckPropertyChangedCount - Completed: %d"), err );
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::CreateNewProcess
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CreateNewProcess( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::CreateNewProcess") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CreateNewProcess - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::ECreateNewProcess, aItem );

    RDebug::Print( _L("CSensrvTest::CreateNewProcess - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::KillProcess
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::KillProcess( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::KillProcess") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::KillProcess - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EKillProcess, aItem );

    RDebug::Print( _L("CSensrvTest::KillProcess - Completed: %d"), err );

    return err;
    }

    // -----------------------------------------------------------------------------
// CSensrvTest::ConditionUnitTestL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::ConditionUnitTestL( CStifItemParser& /*aItem*/ )
    {
    RDebug::Print( _L("CSensrvTest::ConditionUnitTestL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::ConditionUnitTestL - Test creation error %d"), err );
        return err;
        }

    TSensrvAccelerometerAxisData dataItemX;
    dataItemX.iAxisX = 1000;
    dataItemX.iAxisY = 100;
    dataItemX.iAxisZ = 10;
    TPckgBuf<TSensrvAccelerometerAxisData> dataBufX(dataItemX);

    //
    // Condition creation tests
    //

    TInt expectedErr(KErrNone);
    CSensrvChannelCondition* testCondition = NULL;
    for (TInt type=0; type <= ESensrvBinaryCondition; type++)
        {
        for (TInt oper=0; oper <= ESensrvOperatorBinaryAll; oper++)
            {
            TRAP(err,testCondition =
                    CSensrvChannelCondition::NewL(static_cast<TSensrvConditionType>(type),
                                                  static_cast<TSensrvConditionOperator>(oper),
                                                  0,
                                                  dataBufX));

            delete testCondition;
            testCondition = NULL;

            if (   (type == ESensrvSingleLimitCondition && oper == ESensrvOperatorBinaryAnd)
                || (type == ESensrvSingleLimitCondition && oper == ESensrvOperatorBinaryAll)
                || (type == ESensrvRangeConditionLowerLimit && oper == ESensrvOperatorEquals)
                || (type == ESensrvRangeConditionLowerLimit && oper == ESensrvOperatorBinaryAnd)
                || (type == ESensrvRangeConditionLowerLimit && oper == ESensrvOperatorBinaryAll)
                || (type == ESensrvRangeConditionUpperLimit && oper == ESensrvOperatorEquals)
                || (type == ESensrvRangeConditionUpperLimit && oper == ESensrvOperatorBinaryAnd)
                || (type == ESensrvRangeConditionUpperLimit && oper == ESensrvOperatorBinaryAll)
                || (type == ESensrvBinaryCondition && oper == ESensrvOperatorGreaterThan)
                || (type == ESensrvBinaryCondition && oper == ESensrvOperatorGreaterThanOrEquals)
                || (type == ESensrvBinaryCondition && oper == ESensrvOperatorLessThan)
                || (type == ESensrvBinaryCondition && oper == ESensrvOperatorLessThanOrEquals)
               )
                {
                expectedErr = KErrArgument;
                }
            else
                {
                expectedErr = KErrNone;
                }

            if ( err != expectedErr)
                {
                RDebug::Print( _L("CSensrvTest::ConditionUnitTestL - Condition creation failure - type: %d, operator: %d, err: %d, expectedErr: %d"), type, oper, err, expectedErr );
                return KErrArgument;
                }
            }
        }

    //
    // Getter tests
    //

    testCondition = CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                                  ESensrvOperatorLessThanOrEquals,
                                                  3,
                                                  dataBufX);

    TPckgBuf<TSensrvAccelerometerAxisData> getBuf;
    User::LeaveIfError(testCondition->GetConditionValue(getBuf));
    TSensrvAccelerometerAxisData getItem1 = getBuf();

    HBufC8* smallBuf = HBufC8::NewL(sizeof(TSensrvAccelerometerAxisData)-1);
    TPtr8 smallPtr(0,0);
    smallPtr.Set(smallBuf->Des());
    TInt overflowErr = testCondition->GetConditionValue(smallPtr);
    delete smallBuf;

    if (overflowErr != KErrOverflow)
        {
        return KErrOverflow;
        }

    if (   testCondition->ConditionType() != ESensrvRangeConditionLowerLimit
        || testCondition->ConditionOperator() != ESensrvOperatorLessThanOrEquals
        || testCondition->ConditionItemIndex() != 3
        || getItem1.iAxisX != dataItemX.iAxisX
        || getItem1.iAxisY != dataItemX.iAxisY
        || getItem1.iAxisZ != dataItemX.iAxisZ
        || getItem1.iTimeStamp != dataItemX.iTimeStamp
       )
        {
        RDebug::Print( _L("CSensrvTest::ConditionUnitTestL - Getters testing failed") );
        return KErrArgument;
        }

    TTime startTime;
    startTime.HomeTime();

    // Test getting data items speed
    TInt i=0;
    for (i=0; i<100000; i++)
        {
        TPckgBuf<TSensrvAccelerometerAxisData> getBuf;
        testCondition->GetConditionValue(getBuf);
        TSensrvAccelerometerAxisData getItem1 = getBuf();
        if (getItem1.iAxisX == 1234)
            {
            return KErrArgument;
            }
        }

    TTime endTime;
    endTime.HomeTime();

    TTimeIntervalMicroSeconds overallTime =  endTime.MicroSecondsFrom( startTime );

    RDebug::Print( _L("CSensrvTestCases::ConditionUnitTestL - Overall time for %d GetConditionValue %d microseconds"), i, overallTime.Int64() );

    startTime.HomeTime();

    TInt j=0;
    for (j=0; j<100000; j++)
        {
        const TSensrvAccelerometerAxisData* getItem1 =
            reinterpret_cast<const TSensrvAccelerometerAxisData*>(
                testCondition->ConditionValue().Ptr());

        if (getItem1->iAxisX == 1234)
            {
            return KErrArgument;
            }
        }

    endTime.HomeTime();

    TTimeIntervalMicroSeconds overallTime2 =  endTime.MicroSecondsFrom( startTime );

    RDebug::Print( _L("CSensrvTestCases::ConditionUnitTestL - Overall time for %d ConditionValue %d microseconds"), i, overallTime2.Int64() );

    RDebug::Print( _L("CSensrvTest::ConditionUnitTestL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::ConditionSetUnitTestL
// -----------------------------------------------------------------------------
//

TInt CSensrvTest::ConditionSetUnitTestL( CStifItemParser& /*aItem*/ )
    {
    /*
    RDebug::Print( _L("CSensrvTest::ConditionSetUnitTestL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::ConditionSetUnitTestL - Test creation error %d"), err );
        return err;
        }

    TSensrvAccelerometerAxisData dataItemX;
    dataItemX.iAxisX = 1000;
    dataItemX.iAxisY = 0;
    dataItemX.iAxisZ = 0;
    TPckgBuf<TSensrvAccelerometerAxisData> dataBufX(dataItemX);

    TSensrvAccelerometerAxisData dataItemY1;
    dataItemY1.iAxisX = 0;
    dataItemY1.iAxisY = 100;
    dataItemY1.iAxisZ = 0;
    TPckgBuf<TSensrvAccelerometerAxisData> dataBufY1(dataItemY1);

    TSensrvAccelerometerAxisData dataItemY2;
    dataItemY2.iAxisX = 0;
    dataItemY2.iAxisY = 200;
    dataItemY2.iAxisZ = 0;
    TPckgBuf<TSensrvAccelerometerAxisData> dataBufY2(dataItemY2);

    CSensrvChannelConditionSet* set = NULL;
    CSensrvChannelCondition* testCondition =NULL;
    CSensrvChannelCondition* testCondition2 =NULL;
    CSensrvChannelCondition* testCondition3 =NULL;
    CSensrvChannelCondition* testCondition4 =NULL;
    CSensrvChannelCondition* testCondition5 =NULL;
    CSensrvChannelCondition* testCondition6 =NULL;

    // Add NULL condition to set
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    TRAP(err,set->AddChannelConditionL(NULL));

    if (err != KErrArgument)
        {
        return -1000;
        }
    else
        {
        err = KErrNone;
        }

    if (set->AllConditions().Count() != 0)
        {
        return -1000;
        }

    // Empty set should not be complete
    if ((static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1000;
        }

    delete set;

    // Create set with one limit condition
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorEquals,
                                          0,
                                          dataBufX);
    set->AddChannelConditionL(testCondition);
    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1001;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1001;
        }

    delete set;

    // Create set with one complete range condition
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    set->AddChannelConditionL(testCondition);
    set->AddChannelConditionL(testCondition2);
    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1002;
        }

    if (set->AllConditions().Count() != 2)
        {
        return -1002;
        }

    delete set;

    // Create set with one incomplete range condition, lower
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    set->AddChannelConditionL(testCondition);
    if ((static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1003;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1003;
        }

    delete set;

    // Create set with one incomplete range condition, upper
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    set->AddChannelConditionL(testCondition);
    if ((static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -10031;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -10031;
        }

    delete set;

    // Create set with one limit and one complete range condition
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorEquals,
                                          1,
                                          dataBufX);

    set->AddChannelConditionL(testCondition);
    set->AddChannelConditionL(testCondition2);
    set->AddChannelConditionL(testCondition3);
    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1004;
        }

    if (set->AllConditions().Count() != 3)
        {
        return -1004;
        }

    delete set;

    // Create set with two limit and one complete range condition, limit conditions added in between range parts
    set = CSensrvChannelConditionSet::NewL(ESensrvAndConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorEquals,
                                          1,
                                          dataBufX);

    testCondition4 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorLessThanOrEquals,
                                          2,
                                          dataBufX);

    set->AddChannelConditionL(testCondition2);
    set->AddChannelConditionL(testCondition3);
    set->AddChannelConditionL(testCondition4);
    set->AddChannelConditionL(testCondition);
    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1005;
        }

    if (set->AllConditions().Count() != 4)
        {
        return -1005;
        }

    delete set;

    // Create set with three limit conditions
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorLessThanOrEquals,
                                          2,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorEquals,
                                          1,
                                          dataBufX);

    set->AddChannelConditionL(testCondition2);
    set->AddChannelConditionL(testCondition3);
    set->AddChannelConditionL(testCondition);
    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1006;
        }

    if (set->AllConditions().Count() != 3)
        {
        return -1006;
        }

    delete set;

    // Create set with three complete range conditions
    set = CSensrvChannelConditionSet::NewL(ESensrvAndConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          1,
                                          dataBufX);
    testCondition4 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorGreaterThanOrEquals,
                                          1,
                                          dataBufX);

    testCondition5 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorLessThan,
                                          2,
                                          dataBufX);
    testCondition6 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorGreaterThanOrEquals,
                                          2,
                                          dataBufX);

    set->AddChannelConditionL(testCondition2);
    set->AddChannelConditionL(testCondition);
    set->AddChannelConditionL(testCondition5);
    set->AddChannelConditionL(testCondition3);
    set->AddChannelConditionL(testCondition4);
    set->AddChannelConditionL(testCondition6);
    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1007;
        }

    if (set->AllConditions().Count() != 6)
        {
        return -1007;
        }

    delete set;

    // Create set with two complete range conditions and one incomplete range condition at the end
    set = CSensrvChannelConditionSet::NewL(ESensrvAndConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          1,
                                          dataBufX);
    testCondition4 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorGreaterThanOrEquals,
                                          1,
                                          dataBufX);

    testCondition5 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThan,
                                          2,
                                          dataBufX);

    set->AddChannelConditionL(testCondition2);
    set->AddChannelConditionL(testCondition);
    set->AddChannelConditionL(testCondition3);
    set->AddChannelConditionL(testCondition4);
    set->AddChannelConditionL(testCondition5);

    if ((static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1008;
        }

    if (set->AllConditions().Count() != 5)
        {
        return -1008;
        }

    delete set;

    // Create set with double less than range condition
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorLessThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    set->AddChannelConditionL(testCondition);
    TRAP(err,set->AddChannelConditionL(testCondition2));
    if (err != KErrArgument)
        {
        return -1009;
        }
    else
        {
        err = KErrNone;
        }

    if ((static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1009;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1009;
        }

    delete set;

    // Create set with double greater than range condition
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorGreaterThanOrEquals,
                                          0,
                                          dataBufX);
    set->AddChannelConditionL(testCondition);
    TRAP(err,set->AddChannelConditionL(testCondition2));
    if (err != KErrArgument)
        {
        return -1010;
        }
    else
        {
        err = KErrNone;
        }

    if ((static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1010;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1010;
        }

    delete set;

    // Create set with two same index limit conditions
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorLessThan,
                                          0,
                                          dataBufX);
    set->AddChannelConditionL(testCondition);
    TRAP(err,set->AddChannelConditionL(testCondition2));
    if (err != KErrArgument)
        {
        return -1011;
        }
    else
        {
        err = KErrNone;
        }

    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1011;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1011;
        }

    delete set;

    // Create set with three same index range conditions
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorGreaterThanOrEquals,
                                          0,
                                          dataBufX);

    set->AddChannelConditionL(testCondition);
    set->AddChannelConditionL(testCondition2);
    TRAP(err,set->AddChannelConditionL(testCondition3));
    if (err != KErrArgument)
        {
        return -1012;
        }
    else
        {
        err = KErrNone;
        }

    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1012;
        }

    if (set->AllConditions().Count() != 2)
        {
        return -1012;
        }

    delete set;

    // Create set with same index range and limit condition
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorLessThan,
                                          0,
                                          dataBufX);

    set->AddChannelConditionL(testCondition);
    set->AddChannelConditionL(testCondition2);
    TRAP(err,set->AddChannelConditionL(testCondition3));
    if (err != KErrArgument)
        {
        return -1013;
        }
    else
        {
        err = KErrNone;
        }

    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1013;
        }

    if (set->AllConditions().Count() != 2)
        {
        return -1013;
        }

    delete set;

    // Create set with same index limit and range condition
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorLessThan,
                                          0,
                                          dataBufX);

    set->AddChannelConditionL(testCondition3);

    TRAP(err,set->AddChannelConditionL(testCondition));
    if (err != KErrArgument)
        {
        return -1014;
        }
    else
        {
        err = KErrNone;
        }

    TRAP(err,set->AddChannelConditionL(testCondition2));
    if (err != KErrArgument)
        {
        return -1014;
        }
    else
        {
        err = KErrNone;
        }

    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1014;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1014;
        }

    delete set;

    // Serialize and deserialize set with single condition
    CSensrvChannelConditionSet* set1 =
        static_cast<CSensrvChannelConditionSet*>(
            CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet) );

    testCondition =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorLessThan,
                                          0,
                                          dataBufX);

    set1->AddChannelConditionL(testCondition);

    if (!set1->IsComplete())
        {
        return -1015;
        }

    if (set1->AllConditions().Count() != 1)
        {
        return -1015;
        }

    set1->SetConditionSetId(3);

    HBufC8* serializedSet = set1->SerializeLC();

    CSensrvChannelConditionSet* set2 =
        static_cast<CSensrvChannelConditionSet*>(
            CSensrvChannelConditionSet::NewL(ESensrvAndConditionSet) );

    set2->Deserialize(*serializedSet);

    CompareConditionSetsL(set1,
                          set2);

    CleanupStack::PopAndDestroy(serializedSet);
    delete set1;
    delete set2;

    // Serialize and deserialize set with multiple conditions
    set1 =
        static_cast<CSensrvChannelConditionSet*>(
            CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet) );

    testCondition =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorLessThan,
                                          0,
                                          dataBufX);

    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          1,
                                          dataBufY1);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionLowerLimit,
                                          ESensrvOperatorLessThanOrEquals,
                                          1,
                                          dataBufY2);

    testCondition4 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorGreaterThan,
                                          4,
                                          dataBufX);

    set1->AddChannelConditionL(testCondition);
    set1->AddChannelConditionL(testCondition2);
    set1->AddChannelConditionL(testCondition3);
    set1->AddChannelConditionL(testCondition4);

    if (!set1->IsComplete())
        {
        return -1016;
        }

    if (set1->AllConditions().Count() != 4)
        {
        return -1016;
        }

    set1->SetConditionSetId(3);

    serializedSet = set1->SerializeLC();

    set2 =
        static_cast<CSensrvChannelConditionSet*>(
            CSensrvChannelConditionSet::NewL(ESensrvAndConditionSet) );

    set2->Deserialize(*serializedSet);

    CompareConditionSetsL(set1,
                          set2);

    CleanupStack::PopAndDestroy(serializedSet);
    delete set1;
    delete set2;

    // Create set with same index partial range and limit condition
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvSingleLimitCondition,
                                          ESensrvOperatorLessThan,
                                          0,
                                          dataBufX);

    set->AddChannelConditionL(testCondition);
    TRAP(err,set->AddChannelConditionL(testCondition3));
    if (err != KErrArgument)
        {
        return -1017;
        }
    else
        {
        err = KErrNone;
        }

    if ((static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1017;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1017;
        }

    delete set;

    // Create set with same index binary condition and partial range
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvBinaryCondition,
                                          ESensrvOperatorBinaryAll,
                                          0,
                                          dataBufX);
    testCondition2 =
            CSensrvChannelCondition::NewL(ESensrvBinaryCondition,
                                          ESensrvOperatorBinaryAll,
                                          0,
                                          dataBufX);

    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);

    set->AddChannelConditionL(testCondition);
    TRAP(err,set->AddChannelConditionL(testCondition2));
    if (err != KErrArgument)
        {
        return -1018;
        }
    else
        {
        err = KErrNone;
        }

    TRAP(err,set->AddChannelConditionL(testCondition3));
    if (err != KErrArgument)
        {
        return -1018;
        }
    else
        {
        err = KErrNone;
        }

    if (!(static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1018;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1018;
        }

    delete set;

    // Create set with same index binary condition with partial range
    set = CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet);
    testCondition =
            CSensrvChannelCondition::NewL(ESensrvRangeConditionUpperLimit,
                                          ESensrvOperatorGreaterThan,
                                          0,
                                          dataBufX);
    testCondition3 =
            CSensrvChannelCondition::NewL(ESensrvBinaryCondition,
                                          ESensrvOperatorBinaryAll,
                                          0,
                                          dataBufX);

    set->AddChannelConditionL(testCondition);
    TRAP(err,set->AddChannelConditionL(testCondition3));
    if (err != KErrArgument)
        {
        return -1019;
        }
    else
        {
        err = KErrNone;
        }

    if ((static_cast<CSensrvChannelConditionSet*>(set))->IsComplete())
        {
        return -1019;
        }

    if (set->AllConditions().Count() != 1)
        {
        return -1019;
        }

    delete set;

    RDebug::Print( _L("CSensrvTest::ConditionSetUnitTestL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// Compares two condition sets for equality
// -----------------------------------------------------------------------------
//
void CSensrvTest::CompareConditionSetsL(CSensrvChannelConditionSet* aSet1,
                                         CSensrvChannelConditionSet* aSet2)
    {
    const RSensrvChannelConditionList* list1 = &(aSet1->AllConditions());
    const RSensrvChannelConditionList* list2 = &(aSet2->AllConditions());

    if (
          aSet1->ConditionSetId() != aSet2->ConditionSetId()
       || aSet1->IsJustAdded() != aSet2->IsJustAdded()
       || aSet2->IsComplete() != aSet2->IsComplete()
       || list1->Count() != list2->Count()
       )
        {
        User::Leave(KErrArgument);
        }

    for (TInt i = 0; i < list1->Count(); i++)
        {
        TPckgBuf<TSensrvAccelerometerAxisData> getBuf1;
        User::LeaveIfError((*list1)[i]->GetConditionValue(getBuf1));
        TSensrvAccelerometerAxisData getItem1 = getBuf1();

        TPckgBuf<TSensrvAccelerometerAxisData> getBuf2;
        User::LeaveIfError((*list2)[i]->GetConditionValue(getBuf2));
        TSensrvAccelerometerAxisData getItem2 = getBuf2();

        if (
               (*list1)[i]->ConditionType() != (*list2)[i]->ConditionType()
            || (*list1)[i]->ConditionOperator() != (*list2)[i]->ConditionOperator()
            || (*list1)[i]->ConditionItemIndex() != (*list2)[i]->ConditionItemIndex()
            || getItem1.iAxisX != getItem2.iAxisX
            || getItem1.iAxisY != getItem2.iAxisY
            || getItem1.iAxisZ != getItem2.iAxisZ
            || getItem1.iTimeStamp != getItem2.iTimeStamp
           )
            {
            User::Leave(KErrArgument);
            }
        }
        */
    }

// -----------------------------------------------------------------------------
// CSensrvTest::AddConditionSetL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::AddConditionSetL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::AddConditionSetL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::AddConditionSetL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EAddConditionSet, aItem );

    RDebug::Print( _L("CSensrvTest::AddConditionSetL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::RemoveConditionsL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::RemoveConditionsL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::RemoveConditionsL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::RemoveConditionsL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::ERemoveConditions, aItem );

    RDebug::Print( _L("CSensrvTest::RemoveConditionsL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::AddConditionL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::AddConditionL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::AddConditionL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::AddConditionL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EAddCondition, aItem );

    RDebug::Print( _L("CSensrvTest::AddConditionL - Completed: %d"), err );

    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTest::StartConditionListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StartConditionListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StartConditionListeningL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StartConditionListeningL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EStartConditionListening, aItem );

    RDebug::Print( _L("CSensrvTest::StartConditionListeningL - Completed: %d"), err );


    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StopConditionListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StopConditionListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StopConditionListeningL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StopConditionListeningL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EStopConditionListening, aItem );

    RDebug::Print( _L("CSensrvTest::StopConditionListeningL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::GetPSValue
// Parameters: SRKey form SensrvInternalPSKeys.h(TUint32), value(TInt)
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::GetPSValue( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::GetPSValue") );

    TUint srKey;
    TInt value;
    TInt expectedValue;

    TInt err = aItem.GetNextInt( srKey );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::GetPSValue: srKey error: %d"), err );
        return err;
        }

    err = aItem.GetNextInt( expectedValue );
    if( err )
        {
        RDebug::Print(  _L("CSensrvTest::GetPSValue: value error: %d"), err );
        return err;
        }

    err = RProperty::Get( KPSUidSensrvTest, srKey, value );
    if( err )
        {
        RDebug::Print(  _L("CSensrvTest::GetPSValue: value Get error: %d"), err );
        return err;
        }

    RDebug::Print( _L("CSensrvTest::GetPSValue - RProperty::Get( %d, %d ), expected value %d"),
        srKey, value, expectedValue );

    return !( value == expectedValue );
    }

// -----------------------------------------------------------------------------
// CSensrvTest::CheckConditionMetCount
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CheckConditionMetCount( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::CheckConditionMetCount") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CheckConditionMetCount - Test creation error %d"), err );
        return err;
        }

    TInt expectedCount( KErrNotFound );
    err = aItem.GetNextInt( expectedCount );
    if( err == KErrNone )
        {
        if( iTestCases->ConditionMetCount() == expectedCount )
            {
            RDebug::Print( _L("CSensrvTest::CheckConditionMetCount - Correct ConditionMetCount %d"), iTestCases->ConditionMetCount() );
            err = KErrNone;
            }
        else
            {
            RDebug::Print( _L("CSensrvTest::CheckConditionMetCount - Incorrect ConditionMetCount %d"), iTestCases->ConditionMetCount() );
            err = KErrGeneral;
            }
        }

    RDebug::Print( _L("CSensrvTest::CheckConditionMetCount - Completed: %d"), err );
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::CheckFoundChannelCount
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CheckFoundChannelCount( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::CheckFoundChannelCount") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CheckFoundChannelCount - Test creation error %d"), err );
        return err;
        }

    TInt expectedCount( KErrNotFound );
    err = aItem.GetNextInt( expectedCount );
    if( err == KErrNone )
        {
        if( iTestCases->FoundChannelCount() == expectedCount )
            {
            RDebug::Print( _L("CSensrvTest::CheckFoundChannelCount - Correct FoundChannelCount %d"), iTestCases->FoundChannelCount() );
            err = KErrNone;
            }
        else
            {
            RDebug::Print( _L("CSensrvTest::CheckFoundChannelCount - Incorrect FoundChannelCount %d"), iTestCases->FoundChannelCount() );
            err = KErrGeneral;
            }
        }

    RDebug::Print( _L("CSensrvTest::CheckFoundChannelCount - Completed: %d"), err );
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StartChannelChangeListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StartChannelChangeListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StartChannelChangeListeningL") );

    TInt err = CheckTest();
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StartChannelChangeListeningL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EStartChannelChangeListening, aItem );

    RDebug::Print( _L("CSensrvTest::StartChannelChangeListeningL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StopChannelChangeListeningL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StopChannelChangeListeningL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StopChannelChangeListeningL") );
    TInt err = CheckTest();

    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StopChannelChangeListeningL - Test creation error %d"), err );
        return err;
        }

    err = DoRunAndWaitTest( CSensrvTestCases::EStopChannelChangeListening, aItem );

    RDebug::Print( _L("CSensrvTest::StopChannelChangeListeningL - Completed: %d"), err );

    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTest::AdjustChannelsL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::AdjustChannelsL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::AdjustChannelsL") );
    TInt err = CheckTest();

    if( err )
        {
        RDebug::Print( _L("CSensrvTest::AdjustChannelsL - Test creation error %d"), err );
        return err;
        }

    err = iTestCases->AdjustChannels(aItem);

    RDebug::Print( _L("CSensrvTest::AdjustChannelsL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::GetPsResult
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::GetPsResult( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::GetPsResult") );
    TInt err = CheckTest();

    if( err )
        {
        RDebug::Print( _L("CSensrvTest::GetPsResult - Test creation error %d"), err );
        return err;
        }

    // Check ps value
    RProperty property;
    TInt id;

    err = aItem.GetNextInt( id );

    if (!err)
        {
        TRequestStatus status;
        RProperty::Get(KPSUidSensrvTest, id, err);
        }

    RDebug::Print( _L("CSensrvTest::GetPsResult - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::StoreFoundChannelsL
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::StoreFoundChannelsL( CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::StoreFoundChannelsL") );
    TInt err = CheckTest();

    if( err )
        {
        RDebug::Print( _L("CSensrvTest::StoreFoundChannelsL - Test creation error %d"), err );
        return err;
        }

    err = iTestCases->StoreFoundChannelsL(aItem);

    RDebug::Print( _L("CSensrvTest::StoreFoundChannelsL - Completed: %d"), err );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTest::CheckListenedSetPropertySuccessIndicator
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CheckListenedSetPropertySuccessIndicator( CStifItemParser& aItem )
    {
    // First test argument is the expected value.
    // Second test argument is the expected count.
    // Negative argument value means that the argument is not used.

    TInt expectedIndValue( 0 );
    TInt err( aItem.GetNextInt( expectedIndValue ) );
    if( err )
        {
        RDebug::Print( _L("CSensrvTest::CheckListenedSetPropertySuccessIndicator: expected value error: %d"), err );
        }
    else
        {
        TInt expectedIndCount( 0 );
        err = aItem.GetNextInt( expectedIndCount );
        if( err )
            {
            RDebug::Print(  _L("CSensrvTest::CheckListenedSetPropertySuccessIndicator: expected count error: %d"), err );
            }
        else
            {
            TInt indValue( iTestCases->GetSetPropertySuccessIndicator() );
            if ( expectedIndValue >= 0 && expectedIndValue != indValue )
                {
                RDebug::Print(  _L("CSensrvTest::CheckListenedSetPropertySuccessIndicator: invalid value: %d"), indValue );
                err = KErrGeneral;
                }
            }
        if ( !err )
            {
            TInt indCount( iTestCases->GetSetPropertySuccessIndicatorCount() );
            if ( expectedIndCount >= 0 && expectedIndCount != indCount )
                {
                RDebug::Print(  _L("CSensrvTest::CheckListenedSetPropertySuccessIndicator: invalid count: %d"), indCount );
                err = KErrGeneral;
                }
            }
        }
    iTestCases->ClearSetPropertySuccessIndicator();
    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTest::CheckServerAlive
// -----------------------------------------------------------------------------
//
TInt CSensrvTest::CheckServerAlive( CStifItemParser& aItem )
	{
    // Print to UI
	RDebug::Print( _L("CSensrvTest::CheckServerAlive") );
    TInt expectedIndValue( 0 );
    TInt err( aItem.GetNextInt( expectedIndValue ) );
    if( err )
        {
        RDebug::Print(  _L("CSensrvTest::CheckServerAlive: invalid value: %d"), err );
        }
    else
    	{
		RSensrvTestClient test;
		TInt processExists = test.Connect() == KErrNone;
		if( expectedIndValue != processExists)
			{
			err = KErrGeneral;
			}
		test.Close();
    	}
    return err;
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
