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
* Description:  Common trace-information file.
*
*/


#ifndef DEBUG_H
#define DEBUG_H

/**
 *  NOTE: Link to flogger.lib in MMP file. Use DEBUGLIBRARY -keyword to avoid
 *        warnings in release builds.
 *        Example:  DEBUGLIBRARY    flogger.lib
 */

/**
 *  Usage:  LOG(_L("[MODULE_NAME]\t Trace text here"));
 *          TRACE(Print(_L("[MODULE_NAME]\t Trace text here with parameter %d"), iCount));
 *
 *          Trace target can be changed below (file logging needs directory c:\logs\upnp)
 *          #define __FLOGGING -row uncommented (default)  = File logging
 *          #define __CLOGGING -row uncommented            = Console logging
 */

// undefine for sure
#undef __FLOGGING__
#undef __CLOGGING__


// Remove comments to get module internal logss
#ifdef _DEBUG

// Uncomment this in order to get more data than just errors
//#define _DEBUG_MEDIATOR

// Uncomment this to get memory allocation prints from module
//#define MEMORY_PRINT

// Define one of these flags:
// FLOGGING = File logging
// CLOGGING = Console logging
//#define __FLOGGING__
#define __CLOGGING__

#endif



// Then actual definitions depending on the 
// flag values.

#ifdef _DEBUG

    #include <e32std.h>

    // Actual log file name
    _LIT(KLogFile,"MediatorServer.txt");

    // Subdirectory under c:\logs -directory
    _LIT(KLogDir, "MediatorServer");

    #include <f32file.h>
    #include <flogger.h>


    // Define the top level macros
    #ifdef _DEBUG_MEDIATOR 
        #ifdef MEMORY_PRINT
            #define LOG(a) {Print(a); MemPrint();}
            #define TRACE(a) {a; MemPrint();}
        #else // MEMORY_PRINT
            #define LOG(a) {Print(a);}
            #define TRACE(a) {a;}
        #endif // MEMORY_PRINT
    #else // _DEBUG_MEDIATOR
        // No logs in udeb-builds
        #define LOG(a)
        #define TRACE(a)
    #endif // _DEBUG_MEDIATOR        
    
    // Error logs and traces are always compiled in udeb-builds
    #ifdef MEMORY_PRINT
        #define ERROR_LOG(a) {Print(a); MemPrint();}
        #define ERROR_TRACE(a) {a; MemPrint();}
    #else // MEMORY_PRINT
        #define ERROR_LOG(a) {Print(a);}
        #define ERROR_TRACE(a) {a;}
    #endif // MEMORY_PRINT        

    inline void MemPrint()
        {
        TUint32 id = RThread().SecureId().iId;
        TInt allocSize = 0;
        User::Heap().AllocSize( allocSize );
        RDebug::Print(_L("[MEMORY] ID: %d\t AllocSize: %d"), id, allocSize );
        }
            
    #ifdef __FLOGGING__

        inline void Print(const TRefByValue<const TDesC> aFmt, ...)
            {
            VA_LIST list;
            VA_START(list,aFmt);
            TBuf<32> logFile(KLogFile);
            RFileLogger::WriteFormat(KLogDir, logFile, EFileLoggingModeAppend, aFmt, list);
            }
       
    #else
        // Console Logging on
        #define Print RDebug::Print

    #endif  // __FLOGGING__

#else

    // DEBUG build is not on --> no logging at all
    #define LOG(a)
    #define TRACE(a)
    #define ERROR_LOG(a)
    #define ERROR_TRACE(a)

#endif  // _DEBUG

#endif      // DEBUG_H
            
// End of File
