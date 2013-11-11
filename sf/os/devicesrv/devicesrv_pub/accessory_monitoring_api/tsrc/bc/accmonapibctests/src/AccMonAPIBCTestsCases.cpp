/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   AccMonitoringAPI BC test driver.
*
*/



// INCLUDE FILES
#include "AccMonAPIBCTests.h"
#include <e32math.h>
#include <e32base.h>
#include <AccMonitorInfo.h>
#include <AccMonitor.h>
#include <AccMonitorCapabilities.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS

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
        ENTRY( "Get connected accessories", 				AccMonitorTestNoL ),
        ENTRY( "Start Observing test (All connections)",    StartObserving1L ),
        ENTRY( "Start Observing test (Defined Type)",   	StartObserving2L ),
        ENTRY( "Start Observing test (Defined Array)",   	StartObserving3L ),
        ENTRY( "Stop Observing test",   					StopObserving ),
        ENTRY( "Is Observing test",   						IsObserving ),
        ENTRY( "Get Capability Value", 						GetCapabilityValue ),
        ENTRY( "Get DeviceType Info", 						GetDeviceTypeInfo ),
        ENTRY( "Get DeviceAddress Info", 					GetDeviceAddressInfo ),
        ENTRY( "Get PhysicalConnection Info", 				GetPhysicalConInfo ),
        ENTRY( "Get AccCapabilityAtIndex Info", 			AccCapabilityAtIndex ),
        ENTRY( "Get Count Info", 							CountInfo ),
        ENTRY( "Get Exists Info", 							Exists ),
        ENTRY( "Get Exists Array Info", 					ExistsArray )
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
// CAccMonitoringTestModule::ConnectedL
// -----------------------------------------------------------------------------
//							   
void CAccMonitoringTestModule::ConnectedL( CAccMonitorInfo* aInfo )
    {
    TInt arraySize = aInfo->Count();
 	for( TInt i = 0; i != arraySize; i++ )
        {
 		TAccMonCapability accCapa = aInfo->AccCapabilityAtIndex( i );
 		if( accCapa == KAccMonStereoAudio )
            {
    	    // Accessory has stereo capability
    	    }
        }	
    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::DisconnectedL
// -----------------------------------------------------------------------------
//							   
void CAccMonitoringTestModule::DisconnectedL( CAccMonitorInfo* aInfo )
    {
    TInt arraySize = aInfo->Count();
 	for( TInt i = 0; i != arraySize; i++ )
	    {
 		TAccMonCapability accCapa = aInfo->AccCapabilityAtIndex( i );
 		if( accCapa == KAccMonStereoAudio )
		    {
		    // Accessory has stereo capability
		    }
 	    }	
    CActiveScheduler::Stop();
    }
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorObserverError
// -----------------------------------------------------------------------------
//
void CAccMonitoringTestModule::AccMonitorObserverError( TInt /*aError*/ )
    {
    iRunErrorCheck = ETrue;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccMonitorTestNoL
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::AccMonitorTestNoL( 
    TTestResult& aResult )
    {
   
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    TInt err( KErrNone );
    
    RConnectedAccessories array;
    err = api->GetConnectedAccessoriesL( array );
    
    CleanupStack::PopAndDestroy( api );
		
	// Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        _LIT( KDescription, "Connected Accessories were fetched none were connected" );
    	aResult.SetResult( err, KDescription );
    	}
    else 
    	{
    	if( err > KErrNone )
	    	{
	    	_LIT( KDescription, "Connected Accessories were fetched and some accessory was connected" );
	    	aResult.SetResult( err, KDescription );
	    	}
	    else
		    {
		    _LIT( KDescription, "Some error occured and test case failed" );
	    	aResult.SetResult( err, KDescription );
		    }
    	}
    // Case was executed
    
    return err;
    }


// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StartObserving1L
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StartObserving1L( 
    TTestResult& aResult )
    {
    TInt err( KErrNone );
    
    CAccMonitor* api = CAccMonitor::NewLC();
    api->StartObservingL( this );
    CleanupStack::PopAndDestroy( api );
    
    if( err == KErrNone )
    	{
    	_LIT( KDescription, "Started observing correctly" );
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Error occurred, test case failed" );
    	aResult.SetResult( err, KDescription );
    	}
    
    // Case was executed
    return err;
	}

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StartObserving2L
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StartObserving2L( 
    TTestResult& aResult )
    {
    TInt err( KErrNone );
    
    CAccMonitor* api = CAccMonitor::NewLC();
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
    info->Reset();
    api->StartObservingL( this, info );
    CleanupStack::PopAndDestroy( info );
    CleanupStack::PopAndDestroy( api );
    if( err == KErrNone )
    	{
    	_LIT( KDescription, "Started observing correctly with type" );
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Error occurred, test case failed" );
    	aResult.SetResult( err, KDescription );
    	}
    
    // Case was executed
    return err;
    }



// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StartObserving3L
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StartObserving3L( 
    TTestResult& aResult )
    {
    TInt err( KErrNone );
    
    CAccMonitor* api = CAccMonitor::NewL();
    RAccMonCapabilityArray array;
    array.Append( KAccMonHeadset );
    CleanupStack::PushL( api );
    api->StartObservingL( this, array );
    CleanupStack::PopAndDestroy( api );
    
    if( err == KErrNone )
    	{
    	_LIT( KDescription, "Started observing correctly with array and headset capability defined" );
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Error occurred, test case failed" );
    	aResult.SetResult( err, KDescription );
    	}
    
    // Case was executed
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::StopObserving
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::StopObserving( 
    TTestResult& aResult )
    {
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    api->StartObservingL( this );
    TRAPD( err, api->StopObserving() );
    CleanupStack::PopAndDestroy( api );
    
    if( err == KErrNone )
    	{
    	_LIT( KDescription, "Started observing correctly with array and headset capability defined" );
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "Error occurred, test case failed" );
    	aResult.SetResult( err, KDescription );
    	}
    
    // Case was executed
    return err;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::IsObserving
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::IsObserving(
 	TTestResult& aResult )
	{
	TInt err( KErrNone );
    
    CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    TBool isObserving = api->IsObserving();
    CleanupStack::PopAndDestroy( api );
    
    if( isObserving )
    	{
    	_LIT( KDescription, "Observer was observing and it shouldn't be, some error" );
    	err = KErrGeneral;
    	aResult.SetResult( err, KDescription );
    	}
    else
    	{
    	_LIT( KDescription, "No errors, test passed" );
    	aResult.SetResult( err, KDescription );
    	}
    
    // Case was executed
    return err;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::GetCapabilityValue
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::GetCapabilityValue(
 	TTestResult& aResult )
	{
	CAccMonitor* api = CAccMonitor::NewL();
    CleanupStack::PushL( api );
    CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
    info->Reset();
    const TUint32 capability( KAccMonHeadset );
    TInt aValue;
    TRAPD( err, api->GetCapabilityValueL( info, capability, aValue ) );
    CleanupStack::PopAndDestroy( info );
    CleanupStack::PopAndDestroy( api );
	
	switch ( err )
		{
		case KErrNone:
			{
			_LIT( KDescription, "Value fetched successfully" );
			aResult.SetResult( err, KDescription );
			break;
			}
		case KErrArgument:
			{
			_LIT( KDescription, "This accessory was not connected, test OK" );
			err = KErrNone;
			aResult.SetResult( err, KDescription );
			break;
			}
		case KErrNotFound:
			{
			_LIT( KDescription, "This capability was not defined for this accessory, test OK" );
			err = KErrNone;
			aResult.SetResult( err, KDescription );
			break;
			}
		default:
			{
			_LIT( KDescription, "Error occured, test case failed" );
			aResult.SetResult( err, KDescription );
			}
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::GetDeviceTypeInfo
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::GetDeviceTypeInfo(
 	TTestResult& aResult )
	{
	TInt err( KErrNone );
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();
	info->Reset();
	TAccMonCapability capa = info->AccDeviceType();
	if( capa != KAccMonNoDevice )
	{
		_LIT( KDescription, "Error occured, test case failed" );
		err = KErrGeneral;
		aResult.SetResult( err, KDescription );
	}
	return err;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::GetDeviceAddressInfo
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::GetDeviceAddressInfo(
 	TTestResult& aResult )
	{
	TInt err( KErrNone );
	CAccMonitorInfo* info = CAccMonitorInfo::NewLC();
	info->Reset();
	TUint64 address = info->AccDeviceAddress();
	CleanupStack::PopAndDestroy( info );
	if( address != KAccMonNoDevice )
	{
		_LIT( KDescription, "Error occured, test case failed" );
		err = KErrGeneral;
		aResult.SetResult( err, KDescription );
	}
	return err;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::GetPhysicalConInfo
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::GetPhysicalConInfo(
 	TTestResult& aResult )
	{
	TInt err( KErrNone );
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();
	info->Reset();
	TAccMonCapability capa = info->AccPhysicalConnection();
	if( capa != KAccMonNoDevice )
	    {
		_LIT( KDescription, "Error occured, test case failed" );
		err = KErrGeneral;
		aResult.SetResult( err, KDescription );
        }
	return err;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::AccCapabilityAtIndex
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::AccCapabilityAtIndex(
 	TTestResult& aResult )
	{
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();
	info->Reset();
	TInt i( 1 );
	if( info->Count() != 0 )
	    {
	    TInt err = info->AccCapabilityAtIndex( i );
    	if( err == KErrArgument )
    		{
    		err = KErrNone;
    		_LIT( KDescription, "Test case passed" );
    		aResult.SetResult( err, KDescription );
    		}
    	else
    		{
    		_LIT( KDescription, "Error occured, test case failed" );
    		aResult.SetResult( err, KDescription );
    		}
	    }
	else
	    {
	    _LIT( KDescription, "Error occured, test case failed" );
    	aResult.SetResult( KErrNone, KDescription );
	    }
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::CountInfo
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::CountInfo(
 	TTestResult& aResult )
	{
	TInt err( KErrNone );
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();
	info->Reset();
	err = info->Count();
	if( err != KErrNone )
		{
		_LIT( KDescription, "Error occured, test case failed" );
		aResult.SetResult( err, KDescription );
		}
	else
		{
		_LIT( KDescription, "Test case passed" );
		aResult.SetResult( err, KDescription );
		}
	
	return err;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::Exists
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::Exists(
 	TTestResult& aResult )
	{
	TInt err( KErrNone );
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();;
	info->Reset();
	TBool exists = info->Exists( KAccMonHeadset );
	if( exists )
		{
		_LIT( KDescription, "Error occured, test case failed" );
		err = KErrGeneral;
		aResult.SetResult( err, KDescription );
		}
	else
		{
		_LIT( KDescription, "Test case passed" );
		err = KErrNone;
		aResult.SetResult( err, KDescription );
		}
	return err;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::ExistsArray
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::ExistsArray(
 	TTestResult& aResult )
	{
	TInt err( KErrNone );
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();;
	info->Reset();
	RAccMonCapabilityArray array;
	array.Append( KAccMonHeadset );
	TBool exists = info->Exists( array );
	if( exists )
		{
		_LIT( KDescription, "Error occured, test case failed" );
		err = KErrGeneral;
		aResult.SetResult( err, KDescription );
		}
	else
		{
		_LIT( KDescription, "Test case passed" );
		err = KErrNone;
		aResult.SetResult( err, KDescription );
		}
	return err;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::CreateInstances
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::CreateInstances(
 	TTestResult& aResult )
	{
	TInt retVal( KErrNone );
	
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();
	CAccMonitorInfo* info1 = CAccMonitorInfo::NewL( info );
	delete info;
	delete info1;
	
	CAccMonitorInfo* info2 = CAccMonitorInfo::NewLC();
	CAccMonitorInfo* info3 = CAccMonitorInfo::NewLC( info );
	
	CleanupStack::PopAndDestroy( info2 );
	CleanupStack::PopAndDestroy( info3 );
	
	_LIT( KDescription, "Test case passed" );
	aResult.SetResult( retVal, KDescription );
	
	return retVal;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::CopyTest
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::CopyTest(
 	TTestResult& aResult )
	{
	TInt retVal( KErrNone );
	
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();
	CAccMonitorInfo* info1 = CAccMonitorInfo::NewL();
	
	info->CopyL( info );
	
	_LIT( KDescription, "Test case passed" );
	aResult.SetResult( retVal, KDescription );
	
	return retVal;
	}
	
// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::CompareTest
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::CompareTest(
 	TTestResult& aResult )
	{
	TInt retVal( KErrNone );
	
	CAccMonitorInfo* info = CAccMonitorInfo::NewL();
	CAccMonitorInfo* info1 = CAccMonitorInfo::NewL();
	
	info->CompareL( info1 );
	
	_LIT( KDescription, "Test case passed" );
	aResult.SetResult( retVal, KDescription );
	
	return retVal;
	}
//  End of File
