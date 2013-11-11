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
* Description:  Declaration of test tool macros
*
*/


#ifndef __TFTESTMACROS_H__
#define __TFTESTMACROS_H__

#include <e32base.h>
#include "ctfatestsuite.h"
#include "ctfstubtestcase.h"
#include "ctftestcasewrapper.h"
#include "ctftestcaserepeater.h"

/**
* Starts test case declarations. The root test suite given as parameter
* is set as the current test suite.
* @param rootTestSuite The root test suite
*/
#define TF_START_TEST_DECLARATION_L( rootTestSuite ) \
    RPointerArray<CTFATestSuite> suiteList( 4 ); \
    User::LeaveIfError( suiteList.Append( rootTestSuite ) ); \
    CTFATestSuite* currentTestSuite = rootTestSuite

/**
* Ends test case declarations
*/
#define TF_END_TEST_DECLARATION() \
    suiteList.Remove( suiteList.Count() - 1 ); \
    suiteList.Close()

/**
* Adds a test suite to the current test suite and sets the new suite as current suite
* @param testSuiteName The name of the new test suite
*/
#define TF_ADD_TEST_SUITE_L( testSuiteName ) \
    User::LeaveIfError( suiteList.Append( currentTestSuite ) ); \
    currentTestSuite = new ( ELeave ) CTFATestSuite( testSuiteName ); \
    CleanupStack::PushL( currentTestSuite ); \
    currentTestSuite->SetEnvironment( suiteList[suiteList.Count() - 1]->Environment() ); \
    suiteList[suiteList.Count() - 1]->AddL( *currentTestSuite ); \
    CleanupStack::Pop()

/**
* Ends the current test suite and sets the suite that owns the current test suite as the
* current test suite.
*/
#define TF_END_TEST_SUITE() \
    currentTestSuite = suiteList[suiteList.Count() - 1]; \
    suiteList.Remove( suiteList.Count() - 1 )

/**
* Adds a test suite by calling the given function. The prototype of the function is:
* void functionL( CTFATestSuite* newTestSuite );
* @param testSuiteName The name of the new test suite
* @param functionL The test suite function
*/
#define TF_ADD_TEST_SUITE_FUNCTION_L( testSuiteName, functionL ) \
    User::LeaveIfError( suiteList.Append( currentTestSuite ) ); \
    currentTestSuite = new ( ELeave ) CTFATestSuite( testSuiteName ); \
    CleanupStack::PushL( currentTestSuite ); \
    currentTestSuite->SetEnvironment( suiteList[suiteList.Count() - 1]->Environment() ); \
    functionL( currentTestSuite ); \
    suiteList[suiteList.Count() - 1]->AddL( *currentTestSuite ); \
    CleanupStack::Pop(); \
    currentTestSuite = suiteList[suiteList.Count() - 1]; \
    suiteList.Remove( suiteList.Count() - 1 )

/**
* Initializes a test suite function. This needs to be set to the beginning
* of a test suite function.
* @param testSuite The test suite passed to the test suite function
*/
#define TF_INIT_TEST_SUITE_FUNCTION( testSuite ) \
    if ( testSuite == NULL ) \
        { \
        return; \
        } \
    CTFATestSuite* currentTestSuite = testSuite; \
    CTFATestCase* currentTestCase = NULL

/**
* Adds a test case to the current test suite. 
* The current test case will point to the new test case after this.
* @param testCaseId The test case identifier
* @param testCaseName The name of the test case
*/
#define TF_ADD_TEST_CASE_L( testCaseId, testCaseName ) \
    currentTestCase = new ( ELeave ) CTFTestCaseWrapper( testCaseId, testCaseName ); \
    CleanupStack::PushL( currentTestCase ); \
    currentTestCase->SetEnvironment( currentTestSuite->Environment() ); \
    currentTestSuite->AddL( *currentTestCase ); \
    CleanupStack::Pop()

/**
* Sets a timeout for the current test case.
* @param timeoutValue The timeout in seconds
*/
#define TF_SET_TEST_TIMEOUT( timeoutValue ) \
    currentTestCase->SetTimeout( timeoutValue )

/**
* Sets test case flags.
* @param flags The flags from TTFTestCaseFlags enumeration
*/
#define TF_SET_TEST_FLAGS( flags ) \
    currentTestCase->SetFlags( flags )

/**
* Adds a regression test case for the current test suite.
* A regression test case runs each test case within the current test suite.
* @param testCaseId The ID for the regression test case
* @param testCaseName The name for the regression test case
* @param aRepeatCount The number of times to repeat each test case in the test suite
*/
#define TF_ADD_TEST_SUITE_REGRESSION_L( testCaseId, testCaseName, aRepeatCount ) \
    currentTestCase = new ( ELeave ) CTFTestCaseRepeater( testCaseId, testCaseName, currentTestSuite, aRepeatCount, EFalse ); \
    CleanupStack::PushL( currentTestCase ); \
    currentTestCase->SetEnvironment( currentTestSuite->Environment() ); \
    currentTestSuite->AddL( *currentTestCase ); \
    CleanupStack::Pop()
    
/**
* Adds a randomizer test case for the current test suite.
* A randomizer test case runs random test cases within the current test suite.
* @param testCaseId The ID for the regression test case
* @param testCaseName The name for the regression test case
* @param aRepeatCount The number of times to repeat each test case in the test suite
*/
#define TF_ADD_TEST_SUITE_RANDOMIZE_L( testCaseId, testCaseName, aRepeatCount ) \
    currentTestCase = new ( ELeave ) CTFTestCaseRepeater( testCaseId, testCaseName, currentTestSuite, aRepeatCount, ETrue ); \
    CleanupStack::PushL( currentTestCase ); \
    currentTestCase->SetEnvironment( currentTestSuite->Environment() ); \
    currentTestSuite->AddL( *currentTestCase ); \
    CleanupStack::Pop()

/**
* Starts stub test case definitions. This is called in the beginning 
* of CTFStubModuleInterface::GetStubTestCaseL
* @param aTestId The test case identifier that is passed to CTFStubModuleInterface::GetStubTestCaseL
*/
#define TF_START_STUB_TEST_CASES( aTestId ) \
    CTFStubTestCase* returnValue; \
    switch ( aTestId ) \
        { \
        case 0: \
            returnValue = NULL; \
            break

/**
* Ends stub test case definitions. This is called in the end
* of CTFStubModuleInterface::GetStubTestCaseL
*/
#define TF_END_STUB_TEST_CASES() \
        default:\
            returnValue = NULL; \
            break; \
        } \
        return returnValue

#endif
