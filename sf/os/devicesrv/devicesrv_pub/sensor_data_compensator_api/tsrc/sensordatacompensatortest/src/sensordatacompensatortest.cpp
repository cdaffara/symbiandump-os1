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
* Description:  CSensorDataCompensatorTest class implementation.
*
*/


#include <e32debug.h>
#include <e32property.h>
#include <stifparser.h>
#include <stiftestinterface.h>

#include "sensordatacompensatortest.h"
#include "trace.h"


// ---------------------------------------------------------
// CSensorDataCompensatorTest::LibEntryL
// Returns: Poniter to CSensorDataCompensatorTest class
// ---------------------------------------------------------
EXPORT_C CSensorDataCompensatorTest* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
    FUNC_LOG;
    
    CSensorDataCompensatorTest* libEntry( CSensorDataCompensatorTest::NewL( aTestModuleIf ) );
    return libEntry;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::CSensorDataCompensatorTest
// ---------------------------------------------------------
CSensorDataCompensatorTest::CSensorDataCompensatorTest( CTestModuleIf& /*aTestModuleIf*/ )
    {
    FUNC_LOG;    
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::ConstructL
// ---------------------------------------------------------
void CSensorDataCompensatorTest::ConstructL()
    {
    FUNC_LOG;
    
    // Construct active scheduler
    iActiveScheduler = new ( ELeave ) CActiveScheduler;
    CActiveScheduler::Install( iActiveScheduler );
    }    

// ---------------------------------------------------------
// CSensorDataCompensatorTest::NewL
// ---------------------------------------------------------
CSensorDataCompensatorTest* CSensorDataCompensatorTest::NewL( CTestModuleIf& aTestModuleIf )
    {
    FUNC_LOG;
    
    CSensorDataCompensatorTest* self = new (ELeave) CSensorDataCompensatorTest( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::~CSensorDataCompensatorTest
// ---------------------------------------------------------
CSensorDataCompensatorTest::~CSensorDataCompensatorTest()
    {
    FUNC_LOG;
    
    delete iActiveScheduler;
    iActiveScheduler = NULL;
    }

// -----------------------------------------------------------------------------
// CSensorDataCompensatorTest::Case
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
const TCaseInfo CSensorDataCompensatorTest::Case( const TInt aCaseNumber ) const
    {
    FUNC_LOG;
    
    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    CenRepNotifyHandlerTest.cpp file and to CenRepNotifyHandlerTest.h 
    *    header file.
    * 2) Add entry to following KCases array either by using FUNCENTRY or 
    *    ENTRY macro. 
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    * FUNCENTRY macro takes only test case function name as a parameter and uses
    * that as a test case name and test case function name.
    */
    
    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        
		{ (TText*) L"Create and destroy compensator", GETPTR CSensorDataCompensatorTest::CreateAndDestroyCompensator },
	    { (TText*) L"Check channel type", GETPTR CSensorDataCompensatorTest::CheckChannelTypeL },
	    { (TText*) L"Check compensation type", GETPTR CSensorDataCompensatorTest::CheckCompensationTypeL },
	    { (TText*) L"Compensate", GETPTR CSensorDataCompensatorTest::CompensateL}
        };    
    
    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[aCaseNumber].iCaseName );
    tmp.iMethod = KCases[aCaseNumber].iMethod;
    
    return tmp;
    }

// -----------------------------------------------------------------------------
// CSensorDataCompensatorTest::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt CSensorDataCompensatorTest::GetTestCasesL( const TFileName& /*aConfig*/, 
    RPointerArray<TTestCaseInfo>& aTestCases )
    {
    FUNC_LOG;
    
    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {
 
	    // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo;
    
	    // PushL TTestCaseInfo to CleanupStack.    
        CleanupStack::PushL( newCase );

	    // Set number for the testcase.
	    // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

	    // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( Case(i).iCaseName );

	    // Append TTestCaseInfo to the testcase array. After appended 
	    // successfully the TTestCaseInfo object is owned (and freed) 
	    // by the TestServer. 
        User::LeaveIfError(aTestCases.Append ( newCase ) );
        
	    // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensorDataCompensatorTest::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CSensorDataCompensatorTest::RunTestCaseL( const TInt aCaseNumber,   
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {    
    FUNC_LOG;
    
    // Return value
    TInt execStatus = KErrNone;

    // Get the pointer to test case function
    TCaseInfo tmp = Case ( aCaseNumber );

    // Check that case number was valid
    if ( tmp.iMethod != NULL )
        {
        // Valid case was found, call it via function pointer
        iMethod = tmp.iMethod;        
        execStatus  = ( this->*iMethod )( aResult );                  
        }
    else
        {        
        // Valid case was not found, return error.
        execStatus = KErrNotFound;
        }

    // Return case execution status (not the result of the case execution)
    return execStatus;
    }

//------------------------------------------------------------------------------
// TEST CASE IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CSensorDataCompensatorTest::CreateAndDestroyCompensator
//------------------------------------------------------------------------------
TInt CSensorDataCompensatorTest::CreateAndDestroyCompensator( TTestResult& /*aResult*/ )
    {
    FUNC_LOG;
    
    CreateCompensatorL();
    DestroyCompensatorL();

    return KErrNone;
    }

//------------------------------------------------------------------------------
// CSensorDataCompensatorTest::CreateCompensatorL
//------------------------------------------------------------------------------
TInt CSensorDataCompensatorTest::CreateCompensatorL()
    {
    FUNC_LOG;
        
    // Accelerometer data type
    TInt dataType( 270553214 );
    iDataType = (TSensrvChannelDataTypeId)dataType;
    TInt compensationType(0);
    compensationType = ESensorCompensateDeviceOrientation;

    iCompensatorApi = CSensorDataCompensator::NewL( iDataType,
                                  (TSensorCompensationType)compensationType );

    if ( !iCompensatorApi )
        {
        INFO( "CSensorDataCompensatorTest::CreateCompensatorL() Failed to create compensator" );
        User::Leave( KErrAbort );
        }
    
    return KErrNone;
    }
    
//------------------------------------------------------------------------------
// CSensorDataCompensatorTest::DestroyCompensatorL
//------------------------------------------------------------------------------
TInt CSensorDataCompensatorTest::DestroyCompensatorL()
    {
    FUNC_LOG;
    
    delete iCompensatorApi;
    iCompensatorApi = NULL;
    if (iCompensatorApi)
        {
        INFO( "CSensorDataCompensatorTest::DestroyCompensatorL() Failed to destroy compensator" );
        User::Leave( KErrAbort );
        }

    return KErrNone;
    }
    
//------------------------------------------------------------------------------
// CSensorDataCompensatorTest::CheckChannelTypeL
//------------------------------------------------------------------------------
TInt CSensorDataCompensatorTest::CheckChannelTypeL( TTestResult& /*aResult*/ )
    {
    FUNC_LOG;  
    
    CreateCompensatorL();

    if ( iDataType != iCompensatorApi->GetChannelDataType() )
        {
        INFO( "CSensorDataCompensatorTest::CheckChannelTypeL() Channel data type check failed" );
        User::Leave( KErrAbort );
        }
    
    DestroyCompensatorL();
    
    return KErrNone;
    }
    
//------------------------------------------------------------------------------
// CSensorDataCompensatorTest::CheckCompensationTypeL
//------------------------------------------------------------------------------
TInt CSensorDataCompensatorTest::CheckCompensationTypeL( TTestResult& /*aResult*/ )
    {
    FUNC_LOG;  

    CreateCompensatorL();
    
    if ( iCompensationType != iCompensatorApi->GetCompensationType() )
        {
        INFO( "CSensorDataCompensatorTest::CheckCompensationTypeL() Compensation type check failed" );
        User::Leave( KErrAbort );
        }
    
    DestroyCompensatorL();
    
    return KErrNone;
    }

//------------------------------------------------------------------------------
// CSensorDataCompensatorTest::CompensateL
//------------------------------------------------------------------------------
TInt CSensorDataCompensatorTest::CompensateL( TTestResult& /*aResult*/ )
    {
    FUNC_LOG;
    
    CreateCompensatorL();
    
    TBuf8<1> buf; // Empty buffer to test unknown handler
    TDes8& aDes = buf;
    TInt err = 0;
    err = iCompensatorApi->Compensate( aDes );
    if ( buf.Length() > 0 ) // Must be unchanged
        {
        INFO( "CSensorDataCompensatorTest::CompensateL() Compensation with empty buffer failed" );
        User::Leave( KErrAbort );
        }

    /**
    * In emulator "Compensation not ready or needed" (no errors),
    * because no stub exists that would include appended compensation data (check module tests)
    * In HW compensation error -18, because sensordatacompensatorplugin is not ready (not needed in BC tests)
    */
    if ( err != KErrNone &&  err != KErrNotReady)
        {
        INFO_1("CSensorDataCompensatorTest::CompensateL() Compensation failed, err %d", err );
        User::Leave( KErrAbort );
        }
    
    DestroyCompensatorL();

    return KErrNone;
    }
