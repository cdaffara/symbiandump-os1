/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef BTGPSLOGGING_H
#define BTGPSLOGGING_H

//  INCLUDES
#include <flogger.h>
#include <f32file.h>


// CONSTANTS

// MACROS
#ifdef _DEBUG
_LIT( KLoggingFolder, "lbs" );
_LIT( KLoggingFile, "psylogging.txt" );
_LIT( KLoggingFullName, "c:\\logs\\lbs\\psylogging.txt");

#define CLEARTRACELOG \
    {\
    RFs fs; \
    User::LeaveIfError(fs.Connect());\
    CleanupClosePushL(fs);\
    CFileMan* fileMan=CFileMan::NewL(fs); \
    fileMan->Delete(KLoggingFullName); \
    delete fileMan; \
    CleanupStack::PopAndDestroy(&fs);\
    }
    
#define TRACETEXT( S1 )         { RFileLogger::Write( KLoggingFolder(),\
KLoggingFile(), EFileLoggingModeAppend, S1 ); }

#define TRACESTRING( S1 )       { _LIT( tempLogDes, S1 ); RFileLogger::Write(\
KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend, tempLogDes()); }

#define TRACESTRING2( S1, S2 )  { _LIT( tempLogDes, S1 ); RFileLogger::\
WriteFormat( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend,\
TRefByValue<const TDesC>( tempLogDes()), S2 ); }


#define TRACESTRING8_old( S1 )          { _LIT8( tempLogDes, S1 ); RFileLogger::\
Write( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend, tempLogDes());}

#define TRACESTRING8( S1 )          { RFileLogger::\
Write( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend, S1);}

#define TRACESTRING28( S1, S2 )     { _LIT8( tempLogDes, S1 ); RFileLogger::\
WriteFormat( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend,\
TRefByValue<const TDesC8>( tempLogDes()), S2 ); }

#define TRACESTRING38( S1, S2, S3 ) { _LIT8( tempLogDes, S1 ); RFileLogger::\
WriteFormat( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend,\
TRefByValue<const TDesC8>( tempLogDes()), S2, S3 ); }

//
#define BTGPSLOGSTRING2 TRACESTRING2;
#define BTGPSLOGSTRING TRACESTRING;
#define BTGPSLOGTEXT TRACETEXT;
//

#else   // _DEBUG
#define CLEARTRACELOG 
#define TRACETEXT( S1 )
#define TRACESTRING( S1 )
#define TRACESTRING2( S1, S2 )
#define TRACESTRING8( S1 )
#define TRACESTRING28( S1, S2 )
#define TRACESTRING38( S1, S2, S3 )
#endif  // _DEBUG

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif  // BTGPSLOGGING_H
// End of File
