// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
 
// INCLUDE FILES
#include <e32def.h>
#include "activitymanagerdebug.h"

#ifdef _DEBUG
    #define UNSUPPORTED_CALL_TRACE(_dummyMethodOrdinal) TraceCaller(_dummyMethodOrdinal)
#else // _DEBUG
    #define UNSUPPORTED_CALL_TRACE(_dummyMethodOrdinal)
#endif // _DEBUG

// ================= LOCAL FUNCTIONS =======================

#ifdef _DEBUG
/**
Print out which method dummy method was called.
@internalComponent
@param aDummyMethodOrdinal Integer identifying the dummy method called
*/
static void TraceCaller(TInt aDummyMethodOrdinal)
    {
    aDummyMethodOrdinal = aDummyMethodOrdinal; // To avoid compiler warning;
    
    TName caller = RThread().Name();
    HBufC* hbuf = HBufC::New( 256 );
    if ( hbuf )
        {
        TPtr buf = hbuf->Des();

        buf.Copy( _L( "ActivityManager: '" ) );
        buf.Append( caller );
        buf.Append( _L( "' called Dummy::Dummy" ) );

        __ACTIVITYMANAGER_TRACE2("%S%d",&buf,aDummyMethodOrdinal);
              
        delete hbuf;
        }
    }

#endif // _DEBUG

// CLASS DECLARATION

/**
Dummy class. Contains placeholder for a removed function to prevent BC break.
@publishedAll
@deprecated
*/
class Dummy
    {

    public: // New functions

        IMPORT_C static void Dummy1();
        IMPORT_C static void Dummy2();
#if defined __EABI__ || defined __X86GCC__
        IMPORT_C static void Dummy3();
#endif
    };

// ================= MEMBER FUNCTIONS =======================


/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy1() { UNSUPPORTED_CALL_TRACE(1); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy2() { UNSUPPORTED_CALL_TRACE(2); };

#if defined __EABI__ || defined __X86GCC__
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy3() { UNSUPPORTED_CALL_TRACE(3); };
#endif
