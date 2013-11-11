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
* Description:  Implementation of CTFStub class
*
*/


// INCLUDE FILES
#include "ctfstub.h"
#include "tftypes.h"
#include "mtfstubinterface.h"
#include "dsytesttooltrace.h"

// -----------------------------------------------------------------------------
// CTFStub::CTFStub
// -----------------------------------------------------------------------------
EXPORT_C CTFStub::CTFStub( TInt aStubType )
: iStubType( aStubType )
    {
    TRACE_ASSERT( iStubType > 0 );
    __ASSERT_ALWAYS( iStubType > 0, User::Panic( KTFName, ETFPanicInvalidStubType ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFStub::~CTFStub
// -----------------------------------------------------------------------------
EXPORT_C CTFStub::~CTFStub( void )
    {
    iStubControl = NULL;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStub::Type
// -----------------------------------------------------------------------------
TInt CTFStub::Type( void ) const
    {
    return iStubType;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStub::SetStubControl
// -----------------------------------------------------------------------------
void CTFStub::SetStubControl( MTFStubInterface* aStubControl )
    {
    iStubControl = aStubControl;
    }
    
    
// -----------------------------------------------------------------------------
// CTFStub::CurrentTestCase
// -----------------------------------------------------------------------------
EXPORT_C MTFStubTestCase* CTFStub::CurrentTestCase( void )
    {
    TRACE_ASSERT( iStubControl != NULL );
    return iStubControl->CurrentTestCase( iStubType );
    }


// -----------------------------------------------------------------------------
// CTFStub::CurrentTestCaseID
// -----------------------------------------------------------------------------
EXPORT_C TInt CTFStub::CurrentTestCaseID( void )
    {
    TRACE_ASSERT( iStubControl != NULL );
    return iStubControl->CurrentTestCaseID();
    }
    
    
