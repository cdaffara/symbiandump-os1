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
* Description:  Implementation of DosServer control ECom export
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <implementationproxy.h>
#include "ctfdosserverplugin.h"

// CONSTANTS
const TImplementationProxy ImplementationTable[] = 
    {
//lint -e611 Suspicious cast
        IMPLEMENTATION_PROXY_ENTRY( 0x10200c77, CTFDosServerPlugin::NewL )
//lint +e611
    };

// ================= TRACE FUNCTIONS ==============
#ifdef _DEBUG

const TInt KTFAssertDateSize = 32;


// -----------------------------------------------------------------------------
// TraceAssertFunc
// -----------------------------------------------------------------------------
void TraceAssertFunc( const TDesC8& aFile, TInt aLine )
    {
    HBufC* buffer = HBufC::New( aFile.Length() + 1 );
    HBufC* date = HBufC::New( KTFAssertDateSize );
    HBufC* time = HBufC::New( KTFAssertDateSize );
    if ( buffer && date && time )
        {
        buffer->Des().Copy( aFile );
        date->Des().Copy( _L8( __DATE__ ) );
        time->Des().Copy( _L8( __TIME__ ) );
        RDebug::Print( _L( "Assertion failed: file=%s, line=%d, compiled=%s %s" ), 
            buffer->Des().PtrZ(), aLine, date->Des().PtrZ(), time->Des().PtrZ() );
        }
    else
        {
        RDebug::Print( _L( "Assertion and memory allocation failed" ) );
        }
    delete buffer;
    delete date;
    delete time;
    }
#endif

// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// ImplementationGroupProxy
// Returns: TImplementationProxy*: ?description
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof ( ImplementationTable ) / sizeof ( TImplementationProxy );

    return ImplementationTable;
    }
    
