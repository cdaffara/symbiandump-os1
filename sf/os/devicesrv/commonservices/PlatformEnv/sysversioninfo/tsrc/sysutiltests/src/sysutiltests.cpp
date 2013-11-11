/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <stiftestinterface.h>
#include <s32file.h>
#include "sysutiltests.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ---------------------------------------------------------
// CSysUtiltests::CSysUtiltests
// Returns: KErrNone
// ---------------------------------------------------------
CSysUtiltests::CSysUtiltests()
    {

    }

// ---------------------------------------------------------
// CSysUtiltests::ConstructL
// Returns: KErrNone
// ---------------------------------------------------------
void CSysUtiltests::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }

// ---------------------------------------------------------
// CSysUtiltests::NewL
// Returns: KErrNone
// ---------------------------------------------------------
CSysUtiltests* CSysUtiltests::NewL()
    {
    CSysUtiltests* self = new (ELeave) CSysUtiltests;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// ---------------------------------------------------------
// CSysUtiltests::CSysUtiltests
// Returns: KErrNone
// ---------------------------------------------------------
CSysUtiltests::~CSysUtiltests()
    {
    iFs.Close();
    }

// ---------------------------------------------------------
// CSysUtiltests::InitL
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::InitL( TFileName& /*aIniFile*/, TBool /*aFirstTime*/ )
    {
    return KErrNone;

    }

// ---------------------------------------------------------
// CSysUtiltests::Case
// Returns: KErrNone
// ---------------------------------------------------------
const TCaseInfo CSysUtiltests::Case ( const TInt aCaseNumber ) const
     {

    // To add new test cases, implement new test case function and add new
    // line to KCases array specify the name of the case and the function
    // doing the test case
    // In practice, do following
    // 1) Make copy of existing function and change it name
    // 2) Duplicate the last line in cases array
    // 3) Change case name and function name in case array

    static TCaseInfoInternal const KCases[] =
        {

		// To add new test cases, add new items to this array
        ENTRY( "[SYSUTIL] GetVersionInfo test", &CSysUtiltests::GetVersionInfo ),
        ENTRY( "[SYSUTIL] GetVersionInfo2 test", &CSysUtiltests::GetVersionInfo2 ),
        ENTRY( "[SYSUTIL] GetOSVersionInfo test", &CSysUtiltests::GetOSVersionInfo ),
        ENTRY( "[SYSUTIL] GetProductVersionInfo test", &CSysUtiltests::GetProductVersionInfo ),
        ENTRY( "[SYSUTIL] GetOSVersionInfo2 test", &CSysUtiltests::GetOSVersionInfo2 ),
        ENTRY( "[SYSUTIL] GetProductVersionInfo2 test", &CSysUtiltests::GetProductVersionInfo2 ),
        ENTRY( "[SYSUTIL] GetProductCode test", &CSysUtiltests::GetProductCode ),
        
        OOM_ENTRY( "[OOM_SYSUTIL] GetVersionInfo test", &CSysUtiltests::GetVersionInfo ,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_SYSUTIL] GetVersionInfo2 test", &CSysUtiltests::GetVersionInfo2,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_SYSUTIL] GetOSVersionInfo test", &CSysUtiltests::GetOSVersionInfo ,ETrue,1,5),
        OOM_ENTRY( "[OOM_SYSUTIL] GetProductVersionInfo test", &CSysUtiltests::GetProductVersionInfo ,ETrue,1,5),
        OOM_ENTRY( "[OOM_SYSUTIL] GetOSVersionInfo2 test", &CSysUtiltests::GetOSVersionInfo2 ,ETrue,1,5),
        OOM_ENTRY( "[OOM_SYSUTIL] GetProductVersionInfo2 test", &CSysUtiltests::GetProductVersionInfo2 ,ETrue,1,5),
        OOM_ENTRY( "[OOM_SYSUTIL] GetProductCode test", &CSysUtiltests::GetProductCode ,ETrue,1,5)
        
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) /
                               sizeof( TCaseInfoInternal ) )
        {

        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
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

// ---------------------------------------------------------
// CSysUtiltests::GetTestCasesL
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::GetTestCasesL( const TFileName& /*aConfigFile*/,
                                  RPointerArray<TTestCaseInfo>& aTestCases )
    {

    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {

	    // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();

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

// ---------------------------------------------------------
// CSysUtiltests::RunTestCaseL
// Returns: KErrNone
// ---------------------------------------------------------
TInt CSysUtiltests::RunTestCaseL( const TInt aCaseNumber,
                                    const TFileName& /*aConfig*/,
                                    TTestResult& aResult )
    {

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

TInt CSysUtiltests::CheckIsFileExist( const TDesC& aFilename )
{
    RFs fs;
    TInt err;
    err = fs.Connect();
    if (err != KErrNone)
        return err;

    RFile file;
    err = file.Open(fs, aFilename, EFileRead | EFileStreamText | EFileShareReadersOnly);

    file.Close();
    fs.Close();

    if ( err == KErrNotFound || err == KErrPathNotFound )
        {
        err = KErrNotSupported;
        }

	return err;
}


// ---------------------------------------------------------
// CSysUtiltests::LibEntryL
// Returns: Poiter to CSysUtiltests class
// ---------------------------------------------------------
EXPORT_C CSysUtiltests* LibEntryL()
    {
    return CSysUtiltests::NewL();

    }

// -----------------------------------------------------------------------------
// CSysUtiltests::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and 
// which memory allocations should fail.    
// -----------------------------------------------------------------------------
//
TBool CSysUtiltests::OOMTestQueryL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt aCaseNumber, 
                                TOOMFailureType& /* aFailureType */, 
                                TInt& aFirstMemFailure, 
                                TInt& aLastMemFailure ) 
    {
    aFirstMemFailure = Case( aCaseNumber ).iFirstMemoryAllocation;
    aLastMemFailure = Case( aCaseNumber ).iLastMemoryAllocation;

    return Case( aCaseNumber ).iIsOOMTest;

    }

// -----------------------------------------------------------------------------
// CSysUtiltests::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case. 
// Test Modules may use the initialization file to read parameters for Test 
// Module initialization but they can also have their own configure file or 
// some other routine to initialize themselves.  
//
// NOTE: User may add implementation for OOM test environment initialization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CSysUtiltests::OOMTestInitializeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CSysUtiltests::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even 
// inside the tested components which are implemented this way on purpose (by 
// design), so it is important to give the tester a way to bypass allocation 
// failures.
//
// NOTE: User may add implementation for OOM test warning handling. Usually no
// implementation is required.
// -----------------------------------------------------------------------------
//
void CSysUtiltests::OOMHandleWarningL( 
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */, 
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CSysUtiltests::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: User may add implementation for OOM test environment finalization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CSysUtiltests::OOMTestFinalizeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

