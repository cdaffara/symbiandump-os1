/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*  %version:  bh1dsgas#3 %, % date_modified: % by %derived_by: ravindrr %
*
*/


// INCLUDE FILES
#include <stiftestinterface.h>
#include "hwrmtargetmodifierplugintest.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::CHwrmTargetModifierPluginTests
// Returns: KErrNone
// ---------------------------------------------------------
CHwrmTargetModifierPluginTests::CHwrmTargetModifierPluginTests()
    {
    }

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::ConstructL
// Returns: KErrNone
// ---------------------------------------------------------
void CHwrmTargetModifierPluginTests::ConstructL()
    {
    }

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::NewL
// Returns: KErrNone
// ---------------------------------------------------------
CHwrmTargetModifierPluginTests* CHwrmTargetModifierPluginTests::NewL()
    {        
    CHwrmTargetModifierPluginTests* self = new (ELeave) CHwrmTargetModifierPluginTests;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::CHwrmTargetModifierPluginTests
// Returns: KErrNone
// ---------------------------------------------------------
CHwrmTargetModifierPluginTests::~CHwrmTargetModifierPluginTests()
    {
    }

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::InitL
// Returns: KErrNone
// ---------------------------------------------------------
TInt CHwrmTargetModifierPluginTests::InitL( TFileName& /*aIniFile*/, TBool /*aFirstTime*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::Case
// Returns: KErrNone
// ---------------------------------------------------------
const TCaseInfo CHwrmTargetModifierPluginTests::Case ( const TInt aCaseNumber ) const
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
		{ (TText*) L"HWRM Target Modifier Plugin [CreateAndDestroy]",
		        GETPTR CHwrmTargetModifierPluginTests::CreateAndDestroy },
		{ (TText*) L"HWRM Target Modifier Plugin [GetFullSystemTarget]",
		        GETPTR CHwrmTargetModifierPluginTests::GetFullSystemTarget },
		{ (TText*) L"HWRM Target Modifier Plugin [ModifyTarget]",
		        GETPTR CHwrmTargetModifierPluginTests::ModifyTarget }
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
    return tmp;

    }

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::GetTestCasesL
// Returns: KErrNone
// ---------------------------------------------------------
TInt CHwrmTargetModifierPluginTests::GetTestCasesL( const TFileName& /*aConfigFile*/,
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
// CHwrmTargetModifierPluginTests::RunTestCaseL
// Returns: KErrNone
// ---------------------------------------------------------
TInt CHwrmTargetModifierPluginTests::RunTestCaseL( const TInt aCaseNumber,
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

// ---------------------------------------------------------
// CHwrmTargetModifierPluginTests::LibEntryL
// Returns: Poiter to CHwrmTargetModifierPluginTests class
// ---------------------------------------------------------
EXPORT_C CHwrmTargetModifierPluginTests* LibEntryL()
    {
    return CHwrmTargetModifierPluginTests::NewL();
    }
