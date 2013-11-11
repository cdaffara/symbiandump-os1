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
* Description:  Implementation of CTFStubTestCase and CTFStubTestCaseParam classes
*
*/


// INCLUDE FILES
#include "mtftestcaseinterface.h"
#include "ctfstubtestcase.h"
#include "ctfstubtestcaseparam.h"
#include "dsytesttooltrace.h"

// -----------------------------------------------------------------------------
// CTFStubTestCaseParam::CTFStubTestCaseParam
// -----------------------------------------------------------------------------
EXPORT_C CTFStubTestCaseParam::CTFStubTestCaseParam( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCaseParam::~CTFStubTestCaseParam
// -----------------------------------------------------------------------------
EXPORT_C CTFStubTestCaseParam::~CTFStubTestCaseParam( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// MTFStubTestCase::MTFStubTestCase
// -----------------------------------------------------------------------------
MTFStubTestCase::MTFStubTestCase( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// MTFStubTestCase::~MTFStubTestCase
// -----------------------------------------------------------------------------
MTFStubTestCase::~MTFStubTestCase( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::CTFStubTestCase
// -----------------------------------------------------------------------------
EXPORT_C CTFStubTestCase::CTFStubTestCase( TInt aTestCaseType )
: iTestCaseType( aTestCaseType )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::~CTFStubTestCase
// -----------------------------------------------------------------------------
EXPORT_C CTFStubTestCase::~CTFStubTestCase( void )
    {
    iStub = NULL;
    iCallback = NULL;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::SetCallback
// -----------------------------------------------------------------------------
EXPORT_C void CTFStubTestCase::SetCallback( MTFTestCaseInterface& aCallback )
    {
    iCallback = &aCallback;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::Type
// -----------------------------------------------------------------------------
EXPORT_C TInt CTFStubTestCase::Type( void )
    {
    return iTestCaseType;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::SetupL
// -----------------------------------------------------------------------------
EXPORT_C void CTFStubTestCase::SetupL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubTestCase::SetupL()" ) ) );
    iComplete = EFalse;
    InitL();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubTestCase::SetupL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::IsComplete
// -----------------------------------------------------------------------------
EXPORT_C TBool CTFStubTestCase::IsComplete( void )
    {
    return iComplete;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::CompleteTest
// -----------------------------------------------------------------------------
EXPORT_C void CTFStubTestCase::CompleteTest( TInt aTestResult )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubTestCase::CompleteTest(%d)" ), aTestResult ) );
    if ( !iComplete || aTestResult != KErrNone )
        {
        iComplete = ETrue;
        iCallback->TestComplete( aTestResult );
        }
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::Logger
// -----------------------------------------------------------------------------
EXPORT_C CTFATestLog& CTFStubTestCase::Logger( void )
    {
    return iCallback->Logger();
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubTestCase::SetStub
// -----------------------------------------------------------------------------
EXPORT_C void CTFStubTestCase::SetStub( CTFStub& aStub )
    {
    iStub = &aStub;
    }
    
