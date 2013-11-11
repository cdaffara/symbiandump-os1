// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains placeholder for a removed function to prevent BC break.
//



// INCLUDE FILES
#include <e32def.h>
#include "sysutildebug.h"

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

        buf.Copy( _L( "SysUtil: '" ) );
        buf.Append( caller );
        buf.Append( _L( "' called Dummy::Dummy" ) );

        __SYSUTIL_TRACE2("%S%d",&buf,aDummyMethodOrdinal);
              
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

        IMPORT_C static TInt Dummy1();
        IMPORT_C static void Dummy2();
        IMPORT_C static void Dummy3();
        IMPORT_C static void Dummy4();
        IMPORT_C static void Dummy5();
        IMPORT_C static void Dummy6();
        IMPORT_C static void Dummy7();
        IMPORT_C static void Dummy8();
        IMPORT_C static void Dummy9();
        IMPORT_C static void Dummy10();
        IMPORT_C static void Dummy11();
        IMPORT_C static void Dummy12();
        IMPORT_C static void Dummy13();
        IMPORT_C static void Dummy14();
        IMPORT_C static void Dummy15();
        IMPORT_C static void Dummy16();
        IMPORT_C static void Dummy17();
        IMPORT_C static void Dummy18();
        IMPORT_C static void Dummy19();
        IMPORT_C static void Dummy20();
        IMPORT_C static void Dummy21();
        IMPORT_C static void Dummy22();
        IMPORT_C static void Dummy23();
        IMPORT_C static void Dummy24();
        IMPORT_C static void Dummy25();
        IMPORT_C static void Dummy26();
        IMPORT_C static void Dummy27();
        IMPORT_C static void Dummy28();
        IMPORT_C static void Dummy29();
        IMPORT_C static void Dummy30();
    };

// ================= MEMBER FUNCTIONS =======================


/**
Dummy method
@publishedAll
@deprecated
@return KErrNotSupported
*/
EXPORT_C TInt Dummy::Dummy1()
    {
    UNSUPPORTED_CALL_TRACE(1);
    return KErrNotSupported;
    }

/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy2() { UNSUPPORTED_CALL_TRACE(2); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy3() { UNSUPPORTED_CALL_TRACE(3); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy4() { UNSUPPORTED_CALL_TRACE(4); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy5() { UNSUPPORTED_CALL_TRACE(5); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy6() { UNSUPPORTED_CALL_TRACE(6); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy7() { UNSUPPORTED_CALL_TRACE(7); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy8() { UNSUPPORTED_CALL_TRACE(8); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy9() { UNSUPPORTED_CALL_TRACE(9); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy10() { UNSUPPORTED_CALL_TRACE(10); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy11() { UNSUPPORTED_CALL_TRACE(11); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy12() { UNSUPPORTED_CALL_TRACE(12); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy13() { UNSUPPORTED_CALL_TRACE(13); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy14() { UNSUPPORTED_CALL_TRACE(14); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy15() { UNSUPPORTED_CALL_TRACE(15); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy16() { UNSUPPORTED_CALL_TRACE(16); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy17() { UNSUPPORTED_CALL_TRACE(17); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy18() { UNSUPPORTED_CALL_TRACE(18); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy19() { UNSUPPORTED_CALL_TRACE(19); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy20() { UNSUPPORTED_CALL_TRACE(20); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy21() { UNSUPPORTED_CALL_TRACE(21); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy22() { UNSUPPORTED_CALL_TRACE(22); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy23() { UNSUPPORTED_CALL_TRACE(23); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy24() { UNSUPPORTED_CALL_TRACE(24); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy25() { UNSUPPORTED_CALL_TRACE(25); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy26() { UNSUPPORTED_CALL_TRACE(26); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy27() { UNSUPPORTED_CALL_TRACE(27); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy28() { UNSUPPORTED_CALL_TRACE(28); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy29() { UNSUPPORTED_CALL_TRACE(29); };
/**
Dummy method
@publishedAll
@deprecated
*/
EXPORT_C void Dummy::Dummy30() { UNSUPPORTED_CALL_TRACE(30); };

