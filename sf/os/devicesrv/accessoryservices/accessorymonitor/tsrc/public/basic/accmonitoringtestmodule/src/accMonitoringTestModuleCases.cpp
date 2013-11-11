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
#include <AccMonitorInfo.h>
#include <AccMonitor.h>
#include <AccMonitorCapabilities.h>
#include <AccMonConnectionEmulator.h>
#include <AccMonTestTimer.h>
#include "accMonitoringTestModule.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//_LIT( KEnter, "Enter" );
//_LIT( KOnGoing, "On-going" );
//_LIT( KExit, "Exit" );

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
const TCaseInfo CAccMonitoringTestModule::Case ( 
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
        ENTRY( "Get connected accessories (no accessories connected)", 	&CAccMonitoringTestModule::AccMonitorTestNoL ),
        ENTRY( "Get connected accessories test(one accessory)", 	    &CAccMonitoringTestModule::AccMonitorTestOneL ),
        ENTRY( "Get connected accessories test(multiple accessories)", 	&CAccMonitoringTestModule::AccMonitorTestMultiL ),
        ENTRY( "Test has accessory capa", 								&CAccMonitoringTestModule::TestHasAccessoryCapaL ),
        ENTRY( "Test has accessory different capas", 					&CAccMonitoringTestModule::TestHasAccessoryDifferentCapasL ),
        ENTRY( "Start Observer test(all connections)",      			&CAccMonitoringTestModule::StartObservingL ),
        ENTRY( "Start Observer test(defined type)",   					&CAccMonitoringTestModule::StartObserving1L ),
        ENTRY( "Start Observer test(defined accessory)",   				&CAccMonitoringTestModule::StartObserving2L ),
        ENTRY( "Start Observer test(wrong accessory)",   				&CAccMonitoringTestModule::StartObserving3L ),
        ENTRY( "Start Observer test(monitor non-existing accessory)",   &CAccMonitoringTestModule::StartObserving4L ),
        ENTRY( "StopObserving test",    								&CAccMonitoringTestModule::StopObservingL ),
        ENTRY( "Compare Infos test",    								&CAccMonitoringTestModule::CompareInfosL ),
        ENTRY( "Get value test",    									&CAccMonitoringTestModule::GetValueTestL ),
        ENTRY( "Run error test",    									&CAccMonitoringTestModule::RunErrTestL ),
        ENTRY( "Start observing all capas test",						&CAccMonitoringTestModule::StartObserverAllCapasL )        
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
// CAccMonitoringTestModule::AccMonitoringStatusChangedL
// Get the information from the connected accessory. Inherited from 
// MAccMonitorObserver. After receiving information stop Active Scheduler.
// -----------------------------------------------------------------------------
//							   
void CAccMonitoringTestModule::ConnectedL( CAccMonitorInfo* aInfo )
    {
    if( iLeaveFlag )
    	{
    	User::Leave( KErrGeneral );
    	}
    else
    	{
    	/**
	    * Get the capabilities from the connected accessory and store 'em to 
	    * iConnectedAccessory
	    */
	    iConnectedAccessory->CopyL( aInfo );
	    
	    if( iTestTimer->IsActive() )
	    	{
	    	iTestTimer->Cancel();
	    	}
	 	// Stop the scheduler
	 	CActiveScheduler::Stop();
    	}
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitoringStatusChangedL
// Get the information from the disconnected accessory. Inherited from 
// MAccMonitorObserver. After receiving information stop Active Scheduler.
// -----------------------------------------------------------------------------
//							   
void CAccMonitoringTestModule::DisconnectedL( CAccMonitorInfo* aInfo )
    {
    /**
    * Get the capabilities from the connected accessory and store 'em to 
    * iConnectedAccessory
    */
    if( iTestTimer->IsActive() )
    	{
    	iTestTimer->Cancel();
    	}
    iConnectedAccessory->CopyL( aInfo );
 	// Stop the scheduler
    CActiveScheduler::Stop();
    }
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::HeadsetStatusError
// Test just that this function works.
// -----------------------------------------------------------------------------
//
void CAccMonitoringTestModule::AccMonitorObserverError( TInt /*aError*/ )
    {
    iRunErrorCheck = ETrue;
    if( iTestTimer->IsActive() )
	    {
	    iTestTimer->Cancel();
	    }
	iLeaveFlag = EFalse;
    // Stop the scheduler
 	CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestNoL
// A simple test which creates an instance of the CAccMonitor and uses
// GetConnectedAccessoriesL method. Expected result is KErrNotFound since no
// accessories are connected.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::AccMonitorTestNoL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    // Create an instance of the CAccMonitor
    TInt err( 0 );
    CAccMonitor* api = CAccMonitor::NewLC();
    RConnectedAccessories array;
    CleanupClosePushL ( array );
    TInt amount;
    // Get the connected accessories, no accessories should be connected.
    amount = api->GetConnectedAccessoriesL( array );
    
    CAccMonitorInfo* info = CAccMonitorInfo::NewL();
    CleanupStack::PushL ( info );
    TRAPD( err1, api->StartObservingL( this, info ) );
    api->StopObserving();
    CleanupStack::PopAndDestroy( info );
    
    if( amount == KErrNone )
    	{
    	err = KErrNone;
    	_LIT( KDescription, "Function used properly, test passed");
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	err = KErrGeneral;
    	_LIT( KDescription, "Some accessory was connected or something went wrong, test failed" );
    	aResult.SetResult( err, KDescription );
    	}
    
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( api );
    
    __UHEAP_MARKEND;
    // Case was executed
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestOneL
// A test to connect some accessory and then getting the information from 
// Accessory Monitoring API. The information is then checked to be correct.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::AccMonitorTestOneL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    
    TInt amount;
    TInt err( 0 );
    // Connect a wired headset.
	CConnectionEmulator* con = CConnectionEmulator::NewL();
	CleanupStack::PushL( con );
	TBool sync( ETrue );
	TRAPD( conErr, con->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
	
	RConnectedAccessories array;
    CleanupClosePushL( array );
	// Create an instance of the api.
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    // Get the connected accessories.
    err = GetConnectedAccessoriesL(api, array, amount);
    CleanupStack::PopAndDestroy( api );
    if( amount == 1 )
        {
        err = KErrNone;
        }
    else
        {
        err = KErrGeneral;
        }
    // Get information about the connected accessory to info instance.
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC( array[ 0 ] );
    
    // Check if capabilities were correct.
    TBool result( EFalse );
    result = info->Exists( KAccMonHeadset );
    array.ResetAndDestroy();
    if( ( err == KErrNone ) && result )
    	{
    	_LIT( KDescription, "Accessory was connected, test passed" );
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	err = KErrNone;
    	_LIT( KDescription, "Something went wrong, test failed" );
    	aResult.SetResult( err, KDescription );
    	}
    
    CleanupStack::PopAndDestroy( info );
    // Disconnect the accessory that was connected.
    TRAPD( disconErr, con->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( con );
	if( ( conErr != KErrNone ) && ( disconErr != KErrNone ) )
    	{
    	_LIT( KDescription, "Error in connection" );
    	if( conErr != KErrNone )
    		{
    		aResult.SetResult( conErr, KDescription );
    		}
    	else
    		{
    		aResult.SetResult( disconErr, KDescription );
    		}
    	}
	__UHEAP_MARKEND;
    // Case was executed
    return err;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestMultiL
// A test to connect multiple accessories and then getting the information from
// Accessory Monitoring API. The information is then checked to be correct.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::AccMonitorTestMultiL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    TInt amount;
    TInt err;
    TInt disconErr( KErrNone );
    CConnectionEmulator* con = CConnectionEmulator::NewL();
	CleanupStack::PushL( con );
	TBool sync( ETrue );
	// Connect a wired headset
	TRAPD( conErr, con->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
	// Connect a wireless headset
	//TRAPD( conErr1, con->ConnectAccessoryL( CConnectionEmulator::EWirelessHeadSetCon, sync ) );
    
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    RConnectedAccessories array;
    CleanupClosePushL( array );
    // Get the connected accessories from the API
    err = GetConnectedAccessoriesL(api, array, amount);
    TBool result( EFalse );
    // Check the connected accessories and disconnect after being checked.
    for( TInt i = 0; i != array.Count(); i++ )
    	{
    	CAccMonitorInfo* info = CAccMonitorInfo::NewLC( array[ i ] );
    	if( ( info->AccDeviceType() == KAccMonHeadset ) && ( info->AccPhysicalConnection() == KAccMonWired ) )
    		{
    		result = info->Exists( iWiredHeadsetAccessory );
    		}
    	if( ( info->AccDeviceType() == KAccMonHeadset ) && ( info->AccPhysicalConnection() == KAccMonBluetooth ) )
    		{
    		result = info->Exists( iWirelessHeadsetAccessory );
    		}
    	if( ( info->AccDeviceType() == KAccMonCarKit ) && ( info->AccPhysicalConnection() == KAccMonWired ) )
    		{
    		result = info->Exists( iWiredCarKitAccessory );
    		}
    	if( ( info->AccDeviceType() == KAccMonCarKit ) && ( info->AccPhysicalConnection() == KAccMonBluetooth ) )
    		{
    		result = info->Exists( iWirelessCarKitAccessory );
    		}
    	if( ( info->AccDeviceType() == KAccMonAVDevice ) && ( info->AccPhysicalConnection() == KAccMonWired ) )
    		{
    		result = info->Exists( iTVOutAccessory );
    		}
    	if( ( info->AccDeviceType() == KAccMonOffice ) && ( info->AccPhysicalConnection() == KAccMonWired ) )
    		{
    		result = info->Exists( iMusicStandAccessory );
    		}
    	if( !result )
    		{
    		break;
    		}
    	TRAPD( disconErr, con->DisconnectAccessoryL( info, sync ) );
    	CleanupStack::PopAndDestroy( info );
    	}
    
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &array ); 
    CleanupStack::PopAndDestroy( api );
    CleanupStack::PopAndDestroy( con );       
    
    // Check result
    if( ( err == KErrNone ) && result )
    	{
    	_LIT( KDescription, "Accessory was connected, test passed" );
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Something went wrong, test failed" );
    	aResult.SetResult( err, KDescription );
    	}
    
    if( ( conErr != KErrNone ) && ( disconErr != KErrNone ) )
    	{
    	_LIT( KDescription, "Error in connection" );
    	if( conErr != KErrNone )
    		{
    		aResult.SetResult( conErr, KDescription );
    		}
    	else
    		{
    		aResult.SetResult( disconErr, KDescription );
    		}
    	}
    __UHEAP_MARKEND;
    // Case was executed
    return err;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryCapa
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::TestHasAccessoryCapaL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    TInt amount;
    TInt err(-999);
	CConnectionEmulator* con = CConnectionEmulator::NewL();
	CleanupStack::PushL( con );
	TBool sync( ETrue );
	TRAPD( conErr, con->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
	COM_TRACE_1( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL - conErr = %d", conErr );
	
	if(conErr == KErrNone)
	    {
		RConnectedAccessories array;
		CleanupClosePushL( array );
	
		// Create an instance of the api and get the connected accessories
		CAccMonitor* api = CAccMonitor::NewL();
    	CleanupStack::PushL( api );    
		COM_TRACE_1( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL before - err = %d", err );
	    err = GetConnectedAccessoriesL(api, array, amount);
	 
	    CleanupStack::PopAndDestroy( api );
	    
	    if( array.Count() == 0 )
	        return KErrGeneral;
	    
	    if(err == KErrNone)
	        {
	        COM_TRACE_( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL - Inside if(err == KErrNone)" );
	        // Get the information about the accessory
	        CAccMonitorInfo* info = CAccMonitorInfo::NewL( array[ 0 ] );
	        COM_TRACE_( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL - CAccMonitorInfo object created using array index 0" );
	 
	        CleanupStack::PushL( info );
	        TBool result( EFalse );
	        // Use Exists inline function to check if the capability existed
	        result = info->Exists( KAccMonStereoAudio );
	        COM_TRACE_1( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL after - result = %d", result );
	                    
	        /**
	        * Check that accessories were fetched correctly and that it had the
	        * capability.
	        */
	        if( result || ( err != KErrNone ) )
	            {
	            _LIT( KDescription, "Accessory has that capability" );
	            aResult.SetResult( err, KDescription );
	            }
	        else
	            {
	            _LIT( KDescription, "Something went wrong, test case failed" );
	            err = KErrGeneral;
	            aResult.SetResult( err, KDescription );
	            }
	        CleanupStack::PopAndDestroy( info );
	        COM_TRACE_( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL - After CleanupStack::PopAndDestroy( info ) " );
	        }
	    array.ResetAndDestroy();
	    CleanupStack::PopAndDestroy( &array );
	    }
	
    // Disconnect the accessory	
	COM_TRACE_( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL - Before Disconnect Accessory" );
    TRAPD( disconErr, con->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
    COM_TRACE_1( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL disconErr = %d", disconErr );
    
    CleanupStack::PopAndDestroy( con );
    
    if( ( conErr != KErrNone ) && ( disconErr != KErrNone ) )
    	{
    	_LIT( KDescription, "Error in connection" );
    	if( conErr != KErrNone )
    		{
    		aResult.SetResult( conErr, KDescription );
    		}
    	else
    		{
    		aResult.SetResult( disconErr, KDescription );
    		}
    	}
    __UHEAP_MARKEND;
    // Case was executed
    COM_TRACE_1( "[AccFw:AccMon] CAccMonitoringTestModule::TestHasAccessoryCapaL test case executed successfully, returning err = %d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::TestHasAccessoryDifferentCapasL
// In this test a wired headset is connected and then the information is fetched
// from the API. 
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::TestHasAccessoryDifferentCapasL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    TInt amount;
    TInt err;
	CConnectionEmulator* con = CConnectionEmulator::NewL();
	CleanupStack::PushL( con );
	TBool sync( ETrue );
	TRAPD( conErr, con->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
	RConnectedAccessories array;
	CleanupClosePushL( array );
	
	// Create an instance of the api and get the connected accessories
	CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );    
    err = GetConnectedAccessoriesL(api, array, amount);
    CleanupStack::PopAndDestroy( api );
    
    // Get the information about the accessory
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC( array[ 0 ] );
    TBool result( EFalse );
    TBool result1( EFalse );
    TBool result2( EFalse );
    TBool result3( ETrue );
    TBool result4( ETrue );
    TBool result5( ETrue );
    TBool result6( ETrue );
    
    TUint64 address = info->AccDeviceAddress();
    
    // Use Exists inline function to check if the capability existed
    result = info->Exists( KAccMonStereoAudio );
    result1 = info->Exists( KAccMonWired );
    
    RAccMonCapabilityArray capaArray;
    CleanupClosePushL( capaArray );
    capaArray.Append( KAccMonWired );
    capaArray.Append( KAccMonStereoAudio );
    result2 = info->Exists( capaArray );
    
    RAccMonCapabilityArray capaArray1;
    CleanupClosePushL( capaArray1 );
    capaArray1.Append( KAccMonBluetooth );
    capaArray1.Append( KAccMonVideoOut );
    result3 = info->Exists( capaArray1 );
    
    RAccMonCapabilityArray capaArray2;
    CleanupClosePushL( capaArray2 );
    capaArray2.Append( KAccMonWired );
    capaArray2.Append( KAccMonVideoOut );
    result4 = info->Exists( capaArray2 );
    
    RAccMonCapabilityArray capaArray3;
    CleanupClosePushL( capaArray3 );
    capaArray3.Append( KAccMonVideoOut );
    capaArray3.Append( KAccMonWired );
    result5 = info->Exists( capaArray3 );
    
    result6 = info->Exists( KAccMonVideoOut );
    
    /**
    * Check that accessories were fetched correctly and that it had the
    * capability.
    */
    if( result && result1 && result2 && !result3 && !result4 && !result5 && !result6 )
    	{
    	_LIT( KDescription, "Capability checks OK" );
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Something went wrong, test case failed" );
    	err = KErrGeneral;
    	aResult.SetResult( err, KDescription );
    	}
    // Disconnect the accessory	
    TRAPD( disconErr, con->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
    
    CleanupStack::PopAndDestroy( &capaArray3 );
    CleanupStack::PopAndDestroy( &capaArray2 );
    CleanupStack::PopAndDestroy( &capaArray1 );
    CleanupStack::PopAndDestroy( &capaArray );
        
    CleanupStack::PopAndDestroy( info );
    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( con );
    
    if( ( conErr != KErrNone ) && ( disconErr != KErrNone ) )
    	{
    	_LIT( KDescription, "Error in connection" );
    	if( conErr != KErrNone )
    		{
    		aResult.SetResult( conErr, KDescription );
    		}
    	else
    		{
    		aResult.SetResult( disconErr, KDescription );
    		}
    	}
    
    __UHEAP_MARKEND;
    // Case was executed
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StartObservingL
// This test case just creates the observer and then destroys it.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StartObservingL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    TBool is( EFalse );
    // Create an instance of the API
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    
    // Create the observer by StartObserving function
	is = api->IsObserving();
    TRAPD( err, api->StartObservingL( this ) );
    
    CConnectionEmulator* con = CConnectionEmulator::NewLC();
	TBool sync( ETrue );
	TRAPD( conErr, con->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
	
	TInt seconds( 1000000 );
    iTestTimer->WairForSecondsTest( seconds );
        
	TRAPD( disconErr, con->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
	CleanupStack::PopAndDestroy( con );
    
    // Check results
    if( err == KErrNone )
    	{
    	_LIT( KDescription, "Observer set without errors");
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Something went wrong, test failed");
    	aResult.SetResult( err, KDescription );
    	}
    TRAPD( err1, api->StartObservingL( this ) );
    if( err1 == KErrAlreadyExists )
    	{
    	err = KErrNone;
    	}
    else
        {
        err = KErrGeneral;
        }
    CleanupStack::PopAndDestroy( api );
    
    iConnectedAccessory->Reset();
    
    __UHEAP_MARKEND;
    // Case executed
    
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StartObserving1L
// In this test case the observer is set to listen for headset connections that
// have stereo audio capability. After the accessory is connected the
// capabilities are checked.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StartObserving1L( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    
	iConnectedAccessory->Reset();
	// Enter
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    RAccMonCapabilityArray someArray;
    CleanupClosePushL ( someArray );
    someArray.Append( KAccMonStereoAudio );
    someArray.Append( KAccMonHeadset );
    // Start observing for stereo headsets
    TRAPD( err, api->StartObservingL( this, someArray ) );
    if( err == KErrNone )
    	{
    	TRAPD( err, api->StartObservingL( this, someArray ) );
	    if( err == KErrAlreadyExists )
	    	{
	    	err = KErrNone;
	    	}
    	}
	CConnectionEmulator* con = CConnectionEmulator::NewLC();
	TBool sync( ETrue );
	TRAPD( conErr, con->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
    
//	CActiveScheduler::Start();
	TInt seconds( 10000000 );
    iTestTimer->WairForSecondsTest( seconds );
	// Disconnect accessory and destroy the pointters
	TRAPD( disconErr, con->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
	CleanupStack::PopAndDestroy( con );
	CleanupStack::PopAndDestroy( &someArray );
	CleanupStack::PopAndDestroy( api );    
    
    // Check if mono audio and headset capabilities exist
    TBool result( EFalse );
    result = iConnectedAccessory->Exists( KAccMonStereoAudio );
    if( result )
    	{
    	result = iConnectedAccessory->Exists( KAccMonHeadset );
    	}
    
    // Check results
    if( ( err == KErrNone ) && result )
    	{
    	_LIT( KDescription, "Observer set without errors");
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Something went wrong, test failed");
    	aResult.SetResult( err, KDescription );
    	}
    
    if( ( conErr != KErrNone ) && ( disconErr != KErrNone ) )
    	{
    	_LIT( KDescription, "Error in connection" );
    	if( conErr != KErrNone )
    		{
    		aResult.SetResult( conErr, KDescription );
    		}
    	else
    		{
    		aResult.SetResult( disconErr, KDescription );
    		}
    	
    	}
    iConnectedAccessory->Reset();
    __UHEAP_MARKEND;
    // Case executed
    return err;
    }
    
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StartObserving2L
// In this test case first an accessory is connected. After connection the
// accessory information is set to an instance of CAccMonitorInfo. This instance
// is then forwarded to observer and the observer starts to listen just
// connection/disconnection notifications from this accessory. This accessory is
// then connected and checked if the accessory was the correct one.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StartObserving2L( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
	TBool sync( ETrue );
	TInt amount;
	TInt err;

	iConnectedAccessory->Reset();
	TRAPD( conErr, iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
	
	// Conncect a wired headset so that this accessory can be observed.
	CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
	
	RConnectedAccessories accessoriesArray;
    CleanupClosePushL( accessoriesArray );
	err = GetConnectedAccessoriesL(api, accessoriesArray, amount);
    
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC( accessoriesArray[ 0 ] );
    TRAPD( err1, api->StartObservingL( this, info ) );
    if( err1 == KErrNone )
    	{
    	TRAPD( err2, api->StartObservingL( this, info ) );
	    if( err2 == KErrAlreadyExists )
	    	{
	    	err = KErrNone;
	    	}
    	}
    TRAPD( disconErr, iConnectionEmulator->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
    // Start scheduler so that RunL in AccMonitor will get to run
    TInt seconds( 1000000 );
    iTestTimer->WairForSecondsTest( seconds );	
    
 	// Check results
    TBool result( EFalse );
    RAccMonCapabilityArray array;
    CleanupClosePushL( array );
    for( TInt i = 0; i != iConnectedAccessory->Count(); i++ )
    	{
    	TAccMonCapability capa = iConnectedAccessory->AccCapabilityAtIndex( i );
    	array.Append( capa );
    	}
    
    result = iConnectedAccessory->Exists( array );
    if( ( err == KErrNone ) && result )
    	{
    	_LIT( KDescription, "Observer set without errors");
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Something went wrong, test failed");
    	aResult.SetResult( err, KDescription );
    	}
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( info );
    accessoriesArray.ResetAndDestroy();
 	CleanupStack::PopAndDestroy( &accessoriesArray );	
    CleanupStack::PopAndDestroy( api );
 	
 	
 	if( ( conErr != KErrNone ) && ( disconErr != KErrNone ) )
    	{
    	_LIT( KDescription, "Error in connection" );
    	if( conErr != KErrNone )
    		{
    		aResult.SetResult( conErr, KDescription );
    		}
    	else
    		{
    		aResult.SetResult( disconErr, KDescription );
    		}
    	}
 	
 	iConnectedAccessory->Reset();
 	    
 	__UHEAP_MARKEND;
 	    
    // Case executed
    return err;
    }
    
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StartObserving3L
// In this test case first an accessory is connected. After connection the
// accessory information is set to an instance of CAccMonitorInfo. This instance
// is then forwarded to observer and the observer starts to listen just
// connection/disconnection notifications from this accessory. This accessory is
// then connected and checked if the accessory was the correct one.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StartObserving3L( 
    TTestResult& aResult )
    {    
    __UHEAP_MARK;
    TInt amount;
    TInt err;
	iWrongAccessory = ETrue;
	TBool sync( ETrue );
	iConnectedAccessory->Reset();
	
	iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync );
	
	// Conncect a wired headset so that this accessory can be observed.
	CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
	
	RConnectedAccessories accessoriesArray;
    CleanupClosePushL( accessoriesArray );
	err = GetConnectedAccessoriesL(api, accessoriesArray, amount);
    
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC( accessoriesArray[ 0 ] );
    TRAPD( err1, api->StartObservingL( this, info ) );
    
    //iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::EWirelessCarKitCon, sync );

    TInt seconds( 1000000 );
    iTestTimer->WairForSecondsTest( seconds );
            
    TRAPD( conErr, iConnectionEmulator->DisconnectAccessoryL( info, sync ) );
    // Start scheduler so that RunL in AccMonitor will get to run
	iTestTimer->WairForSecondsTest( seconds );
		
 	// Check results
    TBool result( EFalse );
    RAccMonCapabilityArray array;
    CleanupClosePushL ( array );
    for( TInt i = 0; i != iConnectedAccessory->Count(); i++ )
    	{
    	TAccMonCapability capa = iConnectedAccessory->AccCapabilityAtIndex( i );
    	array.Append( capa );
    	}
    
    result = iConnectedAccessory->Exists( array );
    if( ( err == KErrNone ) && result )
    	{
    	_LIT( KDescription, "Observer set without errors");
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Something went wrong, test failed");
    	aResult.SetResult( err, KDescription );
    	}
    
    CleanupStack::PopAndDestroy( &array );
    //TRAPD( disconErr, iConnectionEmulator->DisconnectAccessoryL( CConnectionEmulator::EWirelessCarKitCon, sync ) );

 	CleanupStack::PopAndDestroy( info );
 	accessoriesArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &accessoriesArray );
    CleanupStack::PopAndDestroy( api );
 	iWrongAccessory = EFalse;
 	
 	if( ( conErr != KErrNone ) )//&& ( disconErr != KErrNone ) )
    	{
    	_LIT( KDescription, "Error in connection" );
    	if( conErr != KErrNone )
    		{
    		aResult.SetResult( conErr, KDescription );
    		}
    	else
    		{
    	//	aResult.SetResult( disconErr, KDescription );
    		}
    	}
 	
 	iConnectedAccessory->Reset();
 	
 	 __UHEAP_MARKEND;
    // Case executed
    return err;
    }

// -------------------------------------------------------------------------------------------------------
// CAccMonitoringTestModule::StartObserving4L
// This is a Negative Test, where observer is set to listen for headset connections that
// has VideoOut capability. After the accessory is connected the
// capabilities are checked. If the Accessory Monitor reports HeadSet Accessory test is failed else passed.
// --------------------------------------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StartObserving4L( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    iConnectedAccessory->Reset();
    
    // Enter
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    RAccMonCapabilityArray someArray;
    CleanupClosePushL ( someArray );
    someArray.Append ( KAccMonVideoOut );
    // Start observing for Video Out
    TRAPD( err, api->StartObservingL( this, someArray ) );
    if( err == KErrNone )
        {
        TRAPD( err, api->StartObservingL( this, someArray ) );
        if( err == KErrAlreadyExists )
            {
            err = KErrNone;
            }
        }
    
    // Connect Accessory Other than the one that is being observed.
    CConnectionEmulator* con = CConnectionEmulator::NewLC();
    TBool sync( ETrue );
    TRAPD( conErr, con->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
    
    TInt seconds( 10000000 );
    iTestTimer->WairForSecondsTest( seconds );
    // Disconnect accessory and destroy the pointters
    TRAPD( disconErr, con->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
    CleanupStack::PopAndDestroy( con );
    CleanupStack::PopAndDestroy( &someArray );
    CleanupStack::PopAndDestroy( api ); 
    
    // Check if mono audio and headset capabilities exist
    TBool result( EFalse );
    result = iConnectedAccessory->Exists( KAccMonVideoOut );
    
    _LIT( KDescription, "Error in Acc Monitor Observation" );
    if ( result )
        {
        aResult.SetResult( KErrGeneral, KDescription );
        }
    
    iConnectedAccessory->Reset();
    __UHEAP_MARKEND;
    // Case executed
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StopObservingL
// A test case for just testing the StobObserving method.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StopObservingL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    TInt err( KErrNone );
    // Create an instance of the api an stop the observer.
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    api->StopObserving();
    CleanupStack::PopAndDestroy( api );
    
    // Test was executed so test OK
    _LIT( KDescription, "Observer stopped, test case passed" );
    aResult.SetResult( err, KDescription );
    __UHEAP_MARKEND;
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::CompareInfosL
// A test case for comparing different kind of CAccMonitorInfos.
// -----------------------------------------------------------------------------
//    
TInt CAccMonitoringTestModule::CompareInfosL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    TInt amount;
    TInt err;
    // Create an instance of the CAccMonitorInfo.
    
    TRAPD( conErr, iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, ETrue ) );
        
    iTestTimer->WairForSecondsTest( 10000000 );
    
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    RConnectedAccessories acc;
    CleanupClosePushL( acc );
    err = GetConnectedAccessoriesL(api, acc, amount);
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
    CAccMonitorInfo* info1 = CAccMonitorInfo::NewLC();
    info->CopyL( acc[ 0 ] );
    info1->CopyL( info );
    if( info->CompareL( info1 ) )
    	{
    	err = KErrNone;
    	}
    else
    	{
    	err = KErrGeneral;
    	}
    TRAPD( disconErr, iConnectionEmulator->DisconnectAccessoryL( info, ETrue ) );
    CleanupStack::PopAndDestroy( info1 );
    CleanupStack::PopAndDestroy( info );
    acc.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &acc );	
    CleanupStack::PopAndDestroy( api );
    
    // Test was executed so test OK
    if( ( conErr != KErrNone ) && ( disconErr != KErrNone ) )
    	{
    	_LIT( KDescription, "Error in connection" );
    	if( conErr != KErrNone )
    		{
    		aResult.SetResult( conErr, KDescription );
    		}
    	else
    		{
    		aResult.SetResult( disconErr, KDescription );
    		}
    	}
    else
    	{
    	_LIT( KDescription, "Comparison succesful" );
    	aResult.SetResult( err, KDescription );
    	}
    __UHEAP_MARKEND;
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::GetValueTestL
// A test case for comparing different kind of CAccMonitorInfos.
// -----------------------------------------------------------------------------
//    
TInt CAccMonitoringTestModule::GetValueTestL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    RAccMonCapabilityArray array;
    CleanupClosePushL( array );
    TInt amount;
    TInt err;
    array.Append( KAccMonHeadset );
    array.Append( KAccMonWired );
    array.Append( KAccMonMonoAudio );
    array.Append( KAccMonMicAudioInput );
    array.Append( KAccMonSpeakerAudioOutput );
    TRAPD( conErr, iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, ETrue ) );
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    RConnectedAccessories acc;
    CleanupClosePushL( acc );
    err = GetConnectedAccessoriesL(api, acc, amount);
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
    TInt value;
    if( acc.Count() != 0 )
        {
        info->CopyL( acc[ 0 ] );
        TRAPD( err2, api->GetCapabilityValueL( info, KAccMonVideoOut, value ) );
    	if( err2 == KErrNotFound )
    		{
    		CAccMonitorInfo* info1 = CAccMonitorInfo::NewLC();
    		TRAPD( err3, api->GetCapabilityValueL( info1, KAccMonVideoOut, value ) );
    		if( err3 == KErrArgument )
    			{
    			TRAPD( err5, api->GetCapabilityValueL( info1, KAccMonWired, value ) );
    			_LIT( KDescription, "Test passed" );
    			err = KErrNone;
    			aResult.SetResult( err, KDescription );
    			}
    		else
    			{
    			_LIT( KDescription, "Test failed in accessory info" );
    			aResult.SetResult( err, KDescription );
    			}
    		CleanupStack::PopAndDestroy( info1 );
    		}
    	else
    		{
    		_LIT( KDescription, "Test failed in capability" );
    		aResult.SetResult( err, KDescription );
	    	}
        }
    TRAPD( conErr1, iConnectionEmulator->DisconnectAccessoryL( info, ETrue ) );
    TRAPD( err4, api->GetCapabilityValueL( info, KAccMonVideoOut, value ) );
    CleanupStack::PopAndDestroy( info );
    acc.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &acc );
    CleanupStack::PopAndDestroy( api );
    CleanupStack::PopAndDestroy( &array );
    
    __UHEAP_MARKEND;
    return err;
    }
    
TInt CAccMonitoringTestModule::RunErrTestL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    iLeaveFlag = ETrue;
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    api->StartObservingL( this );
    TRAPD( err, iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, ETrue ) );
    TInt seconds( 1000000 );
    iTestTimer->WairForSecondsTest( seconds );	
    CleanupStack::PopAndDestroy( api );
    iLeaveFlag = EFalse;
    TRAPD( err1, iConnectionEmulator->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, ETrue ) );
    if( err == KErrNone )
        {
        _LIT( KDescription, "Test passed" );
		aResult.SetResult( err, KDescription );
        }
    __UHEAP_MARKEND;
    return err;
    }
    
TInt CAccMonitoringTestModule::StartObserverAllCapasL( 
    TTestResult& aResult )
    {
    __UHEAP_MARK;
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    RAccMonCapabilityArray array;
    CleanupClosePushL( array );
    array.Append( KAccMonHeadset );
    array.Append( KAccMonCarKit );
    array.Append( KAccMonOffice );
    array.Append( KAccMonLoopset );
    array.Append( KAccMonTTY );
    array.Append( KAccMonAVDevice );
    array.Append( KAccMonWired );
    array.Append( KAccMonBluetooth );
    array.Append( KAccMonStereoAudio );
    array.Append( KAccMonMonoAudio );
    array.Append( KAccMonSpeakerAudioOutput );
    array.Append( KAccMonMicAudioInput );
    array.Append( KAccMonAudioOut );
    array.Append( KAccMonBTHSP );
    array.Append( KAccMonBTHFP );
    array.Append( KAccMonBTAVRCP );
    array.Append( KAccMonBTA2DP );
    array.Append( KAccMonVideoOut );
    TRAPD( err, api->StartObservingL( this, array ) );
    TInt seconds( 1000000 );
    TRAPD( err1, iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, ETrue ) );
    iTestTimer->WairForSecondsTest( seconds );	
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( api );
    
    TRAPD( err2, iConnectionEmulator->DisconnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, ETrue ) );
    if( err == KErrNone )
        {
        _LIT( KDescription, "Test passed" );
		aResult.SetResult( err, KDescription );
        }
    __UHEAP_MARKEND;
    return err;
    }
    
TInt CAccMonitoringTestModule::ConnectHeadsetL( 
    TTestResult& aResult )
    {
    /*__UHEAP_MARK;
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    RAccMonCapabilityArray someArray;
    someArray.Append( KAccMonStereoAudio );
    someArray.Append( KAccMonHeadset );
    // Start observing for stereo headsets
    TRAPD( err, api->StartObservingL( this, someArray ) );
    */
	TBool sync( ETrue );
	TRAPD( err, iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::EWiredHeadSetCon, sync ) );
    /*
    TInt seconds( 10000000 );
    iTestTimer->WairForSecondsTest( seconds );
    CActiveScheduler::Start();
    
    CleanupStack::Pop( api );
    delete api;
    someArray.Close();
    __UHEAP_MARKEND;*/
    return err;
    }

TInt CAccMonitoringTestModule::ConnectBTHeadsetL( 
    TTestResult& aResult )
    {
    TRAPD( err, iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::ETTY, ETrue ) );
    RConnectedAccessories array;
    
	// Create an instance of the api.
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    // Get the connected accessories.
    //TInt amount = api->GetConnectedAccessoriesL( array );
    TInt amount;
    err = GetConnectedAccessoriesL(api, array, amount);
    CleanupStack::PopAndDestroy( api );
    
    TRAPD( err1, iConnectionEmulator->DisconnectAccessoryL( CConnectionEmulator::ETTY, ETrue ) );
    return KErrNone;
    }

TInt CAccMonitoringTestModule::DisconnectHeadsetL( 
    TTestResult& aResult )
    {
    //TRAPD( err, iConnectionEmulator->ConnectAccessoryL( CConnectionEmulator::ELoopset, ETrue ) );
    //RConnectedAccessories array;
    
	// Create an instance of the api.
    //CAccMonitor* api = CAccMonitor::NewL();
    //CleanupStack::PushL( api );
    // Get the connected accessories.
    //TInt amount = api->GetConnectedAccessoriesL( array );
    //CleanupStack::PopAndDestroy( api );
    
    TRAPD( err1, iConnectionEmulator->DisconnectAccessoryL( CConnectionEmulator::ELoopset, ETrue ) );
    return err1;
    }
    
TInt CAccMonitoringTestModule::DisconnectBTHeadsetL( 
    TTestResult& aResult )
    {
   // TRAPD( err, iConnectionEmulator->DisconnectAccessoryL( CConnectionEmulator::EWirelessHeadSetCon, ETrue ) );
    return KErrNone;
    }
   
TInt CAccMonitoringTestModule::GetConnectedAccessoriesL( CAccMonitor* aApi, RConnectedAccessories& aArray, TInt& aAmount )
    {
    for( ; ; )
        {
        aAmount = aApi->GetConnectedAccessoriesL( aArray );
        User::After(1000000);
        if ( aAmount )
            {
            break;
            }
        }
    
    if((aArray.Count() == 0))
        User::Leave(KErrGeneral);
    
    return KErrNone;
    }

//  End of File
