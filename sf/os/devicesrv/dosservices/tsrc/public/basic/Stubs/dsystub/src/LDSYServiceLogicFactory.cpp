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
* Description:  Implementation of CLDSYServiceLogicFactory class
*
*/




// INCLUDE FILES 
#include "LDSYTrace.h"

#include "LDSYServiceLogicFactory.h"
#include "LDSYStubServiceLogic.h"


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



// ============================= TRACE FUNCTIONS ===============================

#ifdef _DEBUG
// -----------------------------------------------------------------------------
// TraceAssertFunc
// -----------------------------------------------------------------------------
void TraceAssertFunc( const TDesC8& aFile, TInt aLine )
    {
    HBufC* buffer = HBufC::New( aFile.Length() + 1 );
    HBufC* date = HBufC::New( 32 );
    HBufC* time = HBufC::New( 32 );
    if ( buffer != NULL && date != NULL && time != NULL )
        {
        buffer->Des().Copy( aFile );
        date->Des().Copy( _L8( __DATE__ ) );
        time->Des().Copy( _L8( __TIME__ ) );
        RDebug::Print( _L( "Assertion failed: file=%s, line=%d, compiled=%s %s" ), buffer->Des().PtrZ(), aLine, date->Des().PtrZ(), time->Des().PtrZ() );
        }
    else
        {
        RDebug::Print( _L( "Assertion and memory allocation failed" ) );
        }
    delete buffer;
    delete date;
    delete time;
    }


#ifdef COMPONENT_TRACE_FLAG
// -----------------------------------------------------------------------------
// ThisFileFunc
// -----------------------------------------------------------------------------
void ThisFileFunc( const TDesC8& aFile )
    {
    HBufC* buffer = HBufC::New( aFile.Length() + 1 );
    HBufC* date = HBufC::New( 32 );
    HBufC* time = HBufC::New( 32 );
    if ( buffer != NULL && date != NULL && time != NULL )
        {
        buffer->Des().Copy( aFile );
        date->Des().Copy( _L8( __DATE__ ) );
        time->Des().Copy( _L8( __TIME__ ) );
        RDebug::Print( _L( "File=%s, compiled=%s %s" ), buffer->Des().PtrZ(), date->Des().PtrZ(), time->Des().PtrZ() );
        }
    delete buffer;
    delete date;
    delete time;
    }
#endif
#endif


// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// LibEntryL
// -----------------------------------------------------------------------------
EXPORT_C MCDSYServiceLogicFactory* LibEntryL()
    {
    COMPONENT_TRACE( ( _L( "LDSY - LibEntryL()" ) ) );

    COMPONENT_TRACE_THIS_FILE;
    MCDSYServiceLogicFactory* serviceLogicFactory = CLDSYServiceLogicFactory::NewL();

    COMPONENT_TRACE( ( _L( "LDSY - LibEntryL() - return 0x%x" ), serviceLogicFactory ) );
    return serviceLogicFactory;
    }



// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CLDSYServiceLogicFactory::CLDSYServiceLogicFactory
// -----------------------------------------------------------------------------
CLDSYServiceLogicFactory::CLDSYServiceLogicFactory()
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::CLDSYServiceLogicFactory()" ) ) );

    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::CLDSYServiceLogicFactory - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// CLDSYServiceLogicFactory::~CLDSYServiceLogicFactory
// -----------------------------------------------------------------------------
CLDSYServiceLogicFactory::~CLDSYServiceLogicFactory()
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::~CLDSYServiceLogicFactory()" ) ) );
    
    delete iLDSYStubServiceLogic;

    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::~CLDSYServiceLogicFactory - return void" ) ) );
    }



// -----------------------------------------------------------------------------
// CLDSYServiceLogicFactory::NewL
// -----------------------------------------------------------------------------
CLDSYServiceLogicFactory* CLDSYServiceLogicFactory::NewL()
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::NewL()" ) ) );

    CLDSYServiceLogicFactory* serviceLogicFactory = new ( ELeave ) CLDSYServiceLogicFactory();

    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::NewL - return 0x%x" ), serviceLogicFactory ) );
    return serviceLogicFactory;
    }



// -----------------------------------------------------------------------------
// CLDSYServiceLogicFactory::InitializeL
// -----------------------------------------------------------------------------
void CLDSYServiceLogicFactory::InitializeL( CCDSYController& aCDSYController )
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::InitializeL(0x%x)" ), &aCDSYController ) );
    
    iLDSYStubServiceLogic = CLDSYStubServiceLogic::NewL( aCDSYController );    

    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::InitializeL - return void" ) ) );
    }



// ---------------------------------------------------------
// MCDSYServiceLogicFactory::DeInitialize
// ---------------------------------------------------------
void CLDSYServiceLogicFactory::DeInitialize()
    {
    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::DeInitialize()" ) ) );

    COMPONENT_TRACE( ( _L( "LDSY - CLDSYServiceLogicFactory::DeInitialize - return void" ) ) );
    }

