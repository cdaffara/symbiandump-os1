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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include <e32math.h>
#include <e32base.h>
#include <s32mem.h>
#include <e32property.h>
#include <tspclientmapper.h>
#include <e32property.h>
#include "tspclientTestModule.h"
#include "tspclientmapperprivatepskeys.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
const TProcessId iProcessId0( 0 );
const TProcessId iProcessId1( 1 );
const TProcessId iProcessId2( 2 );
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
// CAccMonitoringTestModule::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CTspClientTestModule::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    headsetTestModule.cpp file and to headsetTestModule.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        ENTRY( "Set a process id playing", 	            &CTspClientTestModule::SetProcessIdPlayingL ),
        ENTRY( "Set and move a process id playing", 	&CTspClientTestModule::SetAndMoveProcessIdPlayingL ),
        ENTRY( "Set and remove a process id playing", 	&CTspClientTestModule::SetAndRemoveProcessIdPlayingL ),
        ENTRY( "Set two and set first process id playing", 	&CTspClientTestModule::SetTwoAndSetFirstProcessIdPlayingL ),
        ENTRY( "Set a process id registered", 	            &CTspClientTestModule::SetProcessIdRegisteredL ),
        ENTRY( "Set and move a process id registered", 	&CTspClientTestModule::SetAndMoveProcessIdRegisteredL ),
        ENTRY( "Set and remove a process id registered", 	&CTspClientTestModule::SetAndRemoveProcessIdRegisteredL ),
        ENTRY( "Set two and set first process id registered", 	&CTspClientTestModule::SetTwoAndSetFirstProcessIdRegisteredL ),
        ENTRY( "Set a process id stopped", 	            &CTspClientTestModule::SetProcessIdStoppedL ),
        ENTRY( "Set and move a process id stopped", 	&CTspClientTestModule::SetAndMoveProcessIdStoppedL ),
        ENTRY( "Set and remove a process id stopped", 	&CTspClientTestModule::SetAndRemoveProcessIdStoppedL ),
        ENTRY( "Set two and set first process id stopped", 	&CTspClientTestModule::SetTwoAndSetFirstProcessIdStoppedL ),
        ENTRY( "Check out of range", 	&CTspClientTestModule::CheckOutOfRangeL ),
        ENTRY( "AddAPI Usage from Multiple Threads Simultaneously", &CTspClientTestModule::MultiThreadTestSyncL ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
        
    return tmp;

    }


// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestNoL
// A simple test which creates an instance of the CAccMonitor and uses
// GetConnectedAccessoriesL method. Expected result is KErrNotFound since no
// accessories are connected.
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetProcessIdPlayingL( 
    TTestResult& aResult )
    {
    // Create an instance of the CAccMonitor
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    mapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::EPlayingClients, array );
    
    if( array[ 0 ] == iProcessId1 )
        {
        err = KErrNone;
        }
    
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::EPlayingClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
        
    // Case was executed
    return err;
    }

// -----------------------------------------------------------------------------
// CTspClientTestModule::ResetRegisteredClients
// Reset registered client array to avoid other registered clients than
// test code to affect test cases.
// -----------------------------------------------------------------------------
//
void CTspClientTestModule::ResetRegisteredClients()
    {
    TFixedArray< TProcessId, 10 > array;
    TPckg< TFixedArray< TProcessId, 10 > > buf( array );
    const TProcessId processIdNull( 0 );
    for( TInt i = 0; i != 10; i++ )
        {
        array[ i ] = processIdNull;
        }

    RProperty::Set(
        KTspClientMapperProperty,
        KTspClientMapperKeyRegistered,
        buf );
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestMultiL
// A test to connect multiple accessories and then getting the information from
// Accessory Monitoring API. The information is then checked to be correct.
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetAndMoveProcessIdPlayingL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    ResetRegisteredClients();
    mapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    mapper->SetTspTargetClientToOtherType( CTspClientMapper::ERegisteredClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    
    if( array[ 0 ] != iProcessId1 )
        {
        User::Leave( err );
        }
    else
        {
        err = KErrNone;
        }
    RArray< TProcessId > array2;
    CheckTspTargetClients( CTspClientMapper::EPlayingClients, array2 );
    if( array2[ 0 ] != iProcessId0 )
        {
        User::Leave( err );
        }
    else
        {
        err = KErrNone;
        }
        
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryCapa
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetAndRemoveProcessIdPlayingL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    mapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    mapper->RemoveTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::EPlayingClients, array );
    
    if( array[ 0 ] == iProcessId0 )
        {
        err = KErrNone;
        }
        
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
        
    // Case was executed
    return err;
    }
    
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryCapa
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetTwoAndSetFirstProcessIdPlayingL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    mapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    mapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId2 );
    mapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::EPlayingClients, array );
    
    if( ( array[ 0 ] == iProcessId1 ) && ( array[ 2 ] == iProcessId0 ) )
        {
        err = KErrNone;
        }
    
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    mapper1->RemoveTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId2 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::EPlayingClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;
    }
    
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestNoL
// A simple test which creates an instance of the CAccMonitor and uses
// GetConnectedAccessoriesL method. Expected result is KErrNotFound since no
// accessories are connected.
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetProcessIdRegisteredL( 
    TTestResult& aResult )
    {
    // Create an instance of the CAccMonitor
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    ResetRegisteredClients();
    mapper->SetTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    
    if( array[ 0 ] == iProcessId1 )
        {
        err = KErrNone;
        }
    
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;
    }


// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestMultiL
// A test to connect multiple accessories and then getting the information from
// Accessory Monitoring API. The information is then checked to be correct.
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetAndMoveProcessIdRegisteredL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    ResetRegisteredClients();
    mapper->SetTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    mapper->SetTspTargetClientToOtherType( CTspClientMapper::EPlayingClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::EPlayingClients, array );
    
    if( array[ 0 ] != iProcessId1 )
        {
        User::Leave( err );
        }
    else
        {
        err = KErrNone;
        }
    RArray< TProcessId > array2;
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array2 );
    if( array2[ 0 ] != iProcessId0 )
        {
        User::Leave( err );
        }
    else
        {
        err = KErrNone;
        }
    
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::EPlayingClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryCapa
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetAndRemoveProcessIdRegisteredL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    ResetRegisteredClients();
    mapper->SetTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    mapper->RemoveTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    
    if( array[ 0 ] == iProcessId0 )
        {
        err = KErrNone;
        }
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryCapa
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetTwoAndSetFirstProcessIdRegisteredL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    ResetRegisteredClients();
    mapper->SetTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    mapper->SetTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId2 );
    mapper->SetTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    
    if( ( array[ 0 ] == iProcessId1 ) && ( array[ 2 ] == iProcessId0 ) )
        {
        err = KErrNone;
        }
    
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    mapper1->RemoveTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId2 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;
    }
    
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestNoL
// A simple test which creates an instance of the CAccMonitor and uses
// GetConnectedAccessoriesL method. Expected result is KErrNotFound since no
// accessories are connected.
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetProcessIdStoppedL( 
    TTestResult& aResult )
    {
    // Create an instance of the CAccMonitor
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    mapper->SetTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::EStoppedClients, array );
    
    if( array[ 0 ] == iProcessId1 )
        {
        err = KErrNone;
        }
    
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::EStoppedClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;
    }


// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestMultiL
// A test to connect multiple accessories and then getting the information from
// Accessory Monitoring API. The information is then checked to be correct.
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetAndMoveProcessIdStoppedL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    ResetRegisteredClients();
    mapper->SetTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId1 );
    mapper->SetTspTargetClientToOtherType( CTspClientMapper::ERegisteredClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    
    if( array[ 0 ] != iProcessId1 )
        {
        User::Leave( err );
        }
    else
        {
        err = KErrNone;
        }
    RArray< TProcessId > array2;
    CheckTspTargetClients( CTspClientMapper::EStoppedClients, array2 );
    if( array2[ 0 ] != iProcessId0 )
        {
        User::Leave( err );
        }
    else
        {
        err = KErrNone;
        }
    
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::ERegisteredClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::ERegisteredClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryCapa
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetAndRemoveProcessIdStoppedL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    mapper->SetTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId1 );
    mapper->RemoveTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::EStoppedClients, array );
    
    if( array[ 0 ] == iProcessId0 )
        {
        err = KErrNone;
        }
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::EStoppedClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
    
    // Case was executed
    return err;
    }
    
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryCapa
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::SetTwoAndSetFirstProcessIdStoppedL( 
    TTestResult& aResult )
    {
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    mapper->SetTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId1 );
    mapper->SetTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId2 );
    mapper->SetTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId1 );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    CheckTspTargetClients( CTspClientMapper::EStoppedClients, array );
    
    if( ( array[ 0 ] == iProcessId1 ) && ( array[ 2 ] == iProcessId0 ) )
        {
        err = KErrNone;
        }
    
    CTspClientMapper* mapper1 = CTspClientMapper::NewLC();
    
    mapper1->RemoveTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId1 );
    mapper1->RemoveTspTargetClient( CTspClientMapper::EStoppedClients, iProcessId2 );
    
    CleanupStack::PopAndDestroy( mapper1 );
    
    array.Reset();
    CheckTspTargetClients( CTspClientMapper::EStoppedClients, array );
    if( !CheckArrayZero( array ) )
        {
        User::Leave( KErrGeneral );
        }
        
    // Case was executed
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryCapa
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::CheckOutOfRangeL( 
    TTestResult& aResult )
    {
    TInt err( KErrNone );
    
    CTspClientMapper* mapper = CTspClientMapper::NewL();
    
    err = mapper->SetTspTargetClientToOtherType( CTspClientMapper::EPlayingClients, iProcessId1 );
    if( err != KErrNotFound )
        {
        User::Leave( KErrGeneral );
        }
    
    err = mapper->RemoveTspTargetClient( CTspClientMapper::EPlayingClients, iProcessId1 );
    if( err != KErrNotFound )
        {
        User::Leave( KErrGeneral );
        }
    else
        {
        err = KErrNone;
        }
    
    delete mapper;
    
    return err;
    }

// -----------------------------------------------------------------------------
// CTspClientMapperImpl::GetTspTargetClientsL
// Provides implementation of this (TSP) interface. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTspClientTestModule::CheckTspTargetClients( 
    TInt aMappingType,
    RArray< TProcessId >& aPidArray
    )
	{
	TInt returnErr( KErrNone );
	if( aMappingType > CTspClientMapper::EStoppedClients )
        {
        returnErr = KErrNotSupported;
        }
    TFixedArray< TProcessId, 10 > array;
    TPckgBuf< TFixedArray< TProcessId, 10 > > buf( array );
    //COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() aMappingType = %d", aMappingType );
	TInt count( 0 );
    
    if( aMappingType == CTspClientMapper::EPlayingClients )
        {
        //COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() - Getting playing clients" );
        returnErr = RProperty::Get(
            KTspClientMapperProperty,
            KTspClientMapperKeyPlaying,
            buf );
        //COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() playing clients count = %d", buf().Count() );
        count = buf().Count();
        }
   if( aMappingType == CTspClientMapper::ERegisteredClients )
        {
        //COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() - Getting registered clients" );
        returnErr = RProperty::Get(
            KTspClientMapperProperty,
            KTspClientMapperKeyRegistered,
            buf );
        //COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() registered clients count = %d", buf().Count() );
        count = buf().Count();
        }
    if( aMappingType == CTspClientMapper::EStoppedClients )
        {
        //COM_TRACE_( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() - Getting stopped clients" );
        returnErr = RProperty::Get(
            KTspClientMapperProperty,
            KTspClientMapperKeyStopped,
            buf );
        //COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() stopped clients count = %d", buf().Count() );
        count = buf().Count();
        }
        
    for( TInt i = 0; i != count; i++ )
        {
        //COM_TRACE_1( "[TspClientMapperImpl] CTspClientMapperImpl::GetTspTargetClientsL() first client's Pid = %d", (TInt)buf().operator[]( i ) );
        aPidArray.Append( buf().operator[]( i ) );
        }
    
    return returnErr;
	
	}

// -----------------------------------------------------------------------------
// CTspClientMapperImpl::GetTspTargetClientsL
// Provides implementation of this (TSP) interface. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CTspClientTestModule::CheckArrayZero( RArray< TProcessId >& aPidArray )
    {
    TBool ret( ETrue );
    for( TInt i = 0; aPidArray.Count() != i; i++ )
        {
        TProcessId pid = aPidArray[ i ];
        if( aPidArray[ i ] != iProcessId0 )
            {
            ret = EFalse;
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CTspClientTestModule::MultiThreadTestSyncL
// This Test creates a child thread and tests the TSPClientMapper API usage from two threads simultaneously.
// ---------------------------------------------------------
TInt CTspClientTestModule::MultiThreadTestSyncL( TTestResult& /*aResult*/ )
    {
    StartThreadFunctionL ();
    
    TInt err = DoSyncLockTestL ( iProcessId1 );
    return err;
    }


// ---------------------------------------------------------
// CTspClientTestModule::StartThreadFunctionL
// ---------------------------------------------------------
void CTspClientTestModule::StartThreadFunctionL()
    {
    RThread thread;
    User::LeaveIfError( thread.Create(
        KNullDesC, ThreadEntryFunction, KDefaultStackSize, NULL, this ) );
    thread.SetPriority( EPriorityNormal );
    thread.Resume();
    thread.Close();
    }

// ---------------------------------------------------------
// CTspClientTestModule::ThreadEntryFunction
// ---------------------------------------------------------
TInt CTspClientTestModule::ThreadEntryFunction( TAny* /*aPtr*/ )
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if ( !cleanup )
            return KErrNoMemory;
    
    TInt err = KErrNone;
    TRAP ( err, DoSyncLockTestL ( iProcessId2 ) );
    
    delete cleanup;
    
    return KErrNone;
    }
    
// ---------------------------------------------------------
// CTspClientTestModule::DoSyncLockTestL
// ---------------------------------------------------------
TInt CTspClientTestModule::DoSyncLockTestL ( TProcessId aProcessId )
    {
    // Create an instance of the CAccMonitor
    TInt err( KErrGeneral );
    CTspClientMapper* mapper = CTspClientMapper::NewLC();
    
    mapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, aProcessId );
    
    CleanupStack::PopAndDestroy( mapper );
    
    RArray< TProcessId > array;
    err = CheckTspTargetClients( CTspClientMapper::EPlayingClients, array );
    
    if( array[ 0 ] == aProcessId )
        {
        err = KErrNone;
        }
    return err;
    }


// End of file
