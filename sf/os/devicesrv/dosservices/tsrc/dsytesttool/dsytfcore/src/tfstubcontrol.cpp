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
* Description:  Implementation of CTFStubControl class
*
*/


// INCLUDE FILES
#include "ctfstub.h"
#include "tftypes.h"
#include "mtftestcase.h"
#include "dsytesttooltrace.h"
#include "ctfstubcontrol.h"

// -----------------------------------------------------------------------------
// MTFStubInterface::MTFStubInterface
// -----------------------------------------------------------------------------
MTFStubInterface::MTFStubInterface( void )
    {
    }


// -----------------------------------------------------------------------------
// MTFStubInterface::~MTFStubInterface
// -----------------------------------------------------------------------------
MTFStubInterface::~MTFStubInterface( void )
    {
    }


// -----------------------------------------------------------------------------
// MTFStubControl::MTFStubControl
// -----------------------------------------------------------------------------
MTFStubControl::MTFStubControl( void )
    {
    }


// -----------------------------------------------------------------------------
// MTFStubControl::~MTFStubControl
// -----------------------------------------------------------------------------
MTFStubControl::~MTFStubControl( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFStubControl::NewL
// -----------------------------------------------------------------------------
CTFStubControl* CTFStubControl::NewL( void )
    {
    CTFStubControl* control = new ( ELeave ) CTFStubControl();
    return control;
    }


// -----------------------------------------------------------------------------
// CTFStubControl::CTFStubControl
// -----------------------------------------------------------------------------
CTFStubControl::CTFStubControl( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubControl::~CTFStubControl
// -----------------------------------------------------------------------------
CTFStubControl::~CTFStubControl( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubControl::~CTFStubControl()" ) ) );
    // Stubs are deleted in reverse order they were added.
    // The stub needs to be removed from the array before deletion,
    // since the stub destructor may call the Stub method that 
    // accesses the array.
    TInt i( iStubs.Count() - 1 );
    while ( i >= 0 )
        {
        CTFStub* stub = iStubs[i];
        iStubs.Remove( i );
        delete stub;
        i--;
        }
    iStubs.Reset();
    iCurrentTestCase = NULL;
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubControl::~CTFStubControl - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubControl::CurrentTestCase
// -----------------------------------------------------------------------------
MTFStubTestCase* CTFStubControl::CurrentTestCase( TInt aTestCaseId )
    {
    MTFStubTestCase* retval;
    if ( iCurrentTestCase != NULL )
        {
        retval = iCurrentTestCase->TestCase( aTestCaseId );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubControl::CurrentTestCase - No test case available" ) ) );
        retval = NULL;
        }
    return retval;
    }


// -----------------------------------------------------------------------------
// CTFStubControl::CurrentTestCaseID
// -----------------------------------------------------------------------------
TInt CTFStubControl::CurrentTestCaseID( void )
    {
    TInt retval;
    if ( iCurrentTestCase != NULL )
        {
        retval = iCurrentTestCase->TestCaseID();
        }
    else
        {
        COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubControl::CurrentTestCaseID - No test case available" ) ) );
        retval = -1;
        }
    return retval;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubControl::SetCurrentTestCase
// -----------------------------------------------------------------------------
void CTFStubControl::SetCurrentTestCase( MTFTestCase* aTestCase )
    {
    iCurrentTestCase = aTestCase;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubControl::AddStubL
// -----------------------------------------------------------------------------
void CTFStubControl::AddStubL( CTFStub& aStub )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubControl::AddStubL(0x%x)" ), &aStub ) );
    for ( TInt i = 0; i < iStubs.Count(); i++ )
        {
        if ( iStubs[i]->Type() == aStub.Type() )
            {
            TRACE_ASSERT_ALWAYS;
            User::Panic( KTFName, ETFPanicDuplicateStub );
            }
        }
    User::LeaveIfError( iStubs.Append( &aStub ) );
    aStub.SetStubControl( this );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFStubControl::AddStubL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubControl::Stub
// -----------------------------------------------------------------------------
CTFStub* CTFStubControl::Stub( TInt aStubType )
    {
    CTFStub* retval = NULL;
    for ( TInt i = 0; i < iStubs.Count(); i++ )
        {
        if ( iStubs[i]->Type() == aStubType )
            {
            retval = iStubs[i];
            i = iStubs.Count();
            }
        }
    return retval;
    }


// -----------------------------------------------------------------------------
// CTFStubControl::Count
// -----------------------------------------------------------------------------
TInt CTFStubControl::Count( void ) const
    {
    return iStubs.Count();
    }
    
    
// -----------------------------------------------------------------------------
// CTFStubControl::StubAt
// -----------------------------------------------------------------------------
CTFStub* CTFStubControl::StubAt( TInt aIndex )
    {
    TRACE_ASSERT( aIndex >= 0 && aIndex < iStubs.Count() );
    return iStubs[aIndex];
    }

