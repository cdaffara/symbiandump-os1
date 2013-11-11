/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name        : EPos_CPosTrace.h
* Part of     : Location Framework
* Interface   : Location Framework Trace API
* Provides functionality to write trace output to debug output (serial
* port in ARMI case) or predefined file.
* Version     :
*
*/



#ifndef CPOSTRACE_H
#define CPOSTRACE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <bautils.h>

#if defined _DEBUG
#include <flogger.h>
#endif

// CONSTANTS
// Default filename that is traced
_LIT(KPosTraceDefaultTracedFile, "<NO FILE>");

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * 
 * Provides functionality to write trace output to debug output (serial
 * port in ARMI case) or predefined file. Will only write traces in debug builds.
 * 
 * @publishedPartner
 * @released	
 */
class CPosTrace :public CBase
    {
    private:  // Constructors and destructor
        CPosTrace();

    public:  // Constructors and destructor
        IMPORT_C static CPosTrace* NewL();
        IMPORT_C static CPosTrace* NewL(const TDesC& aTraceLogDir,
                                        const TDesC& aTraceLogFile);
        IMPORT_C virtual ~CPosTrace();

    private:  // Constructors and destructor
        void ConstructL();
        void ConstructL(const TDesC& aTraceLogDir,
                        const TDesC& aTraceLogFile);
        // By default, prohibit copy constructor
        CPosTrace( const CPosTrace& );
        // Prohibit assigment operator
        CPosTrace& operator= ( const CPosTrace& );

    public: // New functions
        IMPORT_C void TraceL(
            const TDesC& aDescription,
            const TDesC& aFilename = KPosTraceDefaultTracedFile,
            const TInt aLineNumber = 0 );
        IMPORT_C static void Log( 
            const TDesC& aTraceLogDir,
            const TDesC& aTraceLogFile,
            TRefByValue<const TDesC> aFmt, ... );

    private:    // Data
#if defined _DEBUG
        RFileLogger  iLogger;
        HBufC*  iTraceDir;
        HBufC*  iTraceFile;
#endif
    };

#ifdef _DEBUG

#define LOG( a )             { CPosTrace::Log( KTraceDir, KTraceFile, _L( a ) ); }
#define LOG1( a, b )         { CPosTrace::Log( KTraceDir, KTraceFile, _L( a ), b ); }
#define LOG2( a, b, c )      { CPosTrace::Log( KTraceDir, KTraceFile, _L( a ), b, c ); }
#define LOG3( a, b, c, d )   { CPosTrace::Log( KTraceDir, KTraceFile, _L( a ), b, c, d ); }
#define LOG4( a, b, c, d, e )   { CPosTrace::Log( KTraceDir, KTraceFile, _L( a ), b, c, d, e ); }

#else // _DEBUG

// Release version - no logging
#define LOG( a )
#define LOG1( a, b )
#define LOG2( a, b, c )
#define LOG3( a, b, c, d )
#define LOG4( a, b, c, d, e )

#endif      // _DEBUG

#endif      // CPOSTRACE_H

// End of File
