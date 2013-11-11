/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Logging macros
*
*/


#ifndef SUPLPSYLOGGING_H
#define SUPLPSYLOGGING_H

//  INCLUDES
#include <flogger.h>
#include <f32file.h>


// CONSTANTS

// MACROS
_LIT( KLoggingFolder, "suplpsy" );
_LIT( KLoggingFile, "log.txt" );
_LIT( KLoggingFullName, "c:\\logs\\suplpsy\\log.TXT" );

#define CLEARTRACELOG \
    { \
    RFs fs;fs.Connect(); \
    CFileMan* fileMan = CFileMan::NewL( fs ); \
    fileMan->Delete( KLoggingFullName ); \
    delete fileMan; \
    fs.Close();\
    }
    
#define TRACETEXT( S1 ) \
    { \
    RFileLogger::Write( \
        KLoggingFolder(), \
        KLoggingFile(), \
        EFileLoggingModeAppend, \
        S1 ); \
    }

#define TRACESTRING( S1 ) \
    { \
    _LIT( tempLogDes, S1 ); \
    RFileLogger::Write( \
        KLoggingFolder(), \
        KLoggingFile(), \
        EFileLoggingModeAppend, \
        tempLogDes() ); \
    }

#define TRACESTRING2( S1, S2 ) \
    { \
    _LIT( tempLogDes, S1 ); \
    RFileLogger::WriteFormat( \
        KLoggingFolder(), \
        KLoggingFile(), \
        EFileLoggingModeAppend,\
        TRefByValue < const TDesC >( tempLogDes() ), S2 ); \
    }

#define TRACEHEX2( PTR, LEN ) \
    { \
    RFileLogger::HexDump( \
        KLoggingFolder(), \
        KLoggingFile(), \
        EFileLoggingModeAppend, \
        NULL, \
        NULL, \
        PTR, \
        LEN ); \
    }



// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif  // SUPLPSYLOGGING_H
// End of File
