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
* Description:  Implementation of CTFATestEnvironment class
*
*/


// INCLUDE FILES
#include "ctfatestlog.h"
#include "ctfatestenvironment.h"
#include "ctffactory.h"
#include "ctfactivescheduler.h"
#include "dsytesttooltrace.h"

// -----------------------------------------------------------------------------
// CTFATestLog::CTFATestLog
// -----------------------------------------------------------------------------
EXPORT_C CTFATestLog::CTFATestLog( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFATestLog::~CTFATestLog
// -----------------------------------------------------------------------------
EXPORT_C CTFATestLog::~CTFATestLog( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFATestEnvironment::CTFATestEnvironment
// -----------------------------------------------------------------------------
EXPORT_C CTFATestEnvironment::CTFATestEnvironment( CTFATestLog& aLogger )
: iLogger( &aLogger )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestEnvironment::~CTFATestEnvironment
// -----------------------------------------------------------------------------
EXPORT_C CTFATestEnvironment::~CTFATestEnvironment( void )
    {
    delete iFactory;
    if ( iScheduler )
        {
        COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFATestEnvironment::~CTFATestEnvironment() - Deleting active scheduler" ) ) );
        CActiveScheduler::Install( NULL );
        delete iScheduler;
        }
    delete iLogger;
    }
    

// -----------------------------------------------------------------------------
// CTFATestEnvironment::SetFactory
// -----------------------------------------------------------------------------
void CTFATestEnvironment::SetFactory( CTFFactory* aFactory )
    {
    iFactory = aFactory;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestEnvironment::Factory
// -----------------------------------------------------------------------------
EXPORT_C CTFFactory* CTFATestEnvironment::Factory( void )
    {
    return iFactory;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestEnvironment::SetScheduler
// -----------------------------------------------------------------------------
void CTFATestEnvironment::SetScheduler( CTFActiveScheduler* aScheduler )
    {
    iScheduler = aScheduler;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestEnvironment::Logger
// -----------------------------------------------------------------------------
EXPORT_C CTFATestLog& CTFATestEnvironment::Logger( void )
    {
    return *iLogger;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestEnvironment::SetTestCaseFlags
// -----------------------------------------------------------------------------
EXPORT_C void CTFATestEnvironment::SetTestCaseFlags( TUint32 aTestCaseFlags )
    {
    iTestCaseFlags = aTestCaseFlags;
    }


// -----------------------------------------------------------------------------
// CTFATestEnvironment::TestCaseFlags
// -----------------------------------------------------------------------------
TUint32 CTFATestEnvironment::TestCaseFlags( void ) const
    {
    return iTestCaseFlags;
    }

