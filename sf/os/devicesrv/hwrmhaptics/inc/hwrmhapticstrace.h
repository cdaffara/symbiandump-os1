/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics debug trace macro definition header file
*
*/


#ifndef HWRMHAPTICSTRACE_H
#define HWRMHAPTICSTRACE_H

#include <bldvariant.hrh>
#include <e32svr.h>
#include <e32std.h>
#include <e32svr.h>
#include <flogger.h>

/**
 * Writes formatted string data to a file using RFileLogger.
 *
 * @param aFmt Value-referenced descriptor containing the formatted string. 
 */
inline void HwrmWriteFormat( TRefByValue<const TDesC> aFmt, ... )
    {
    _LIT( KDir, "haptics" );
    _LIT( KName, "haptics.log" );

    // take the ellipsis parameters
    VA_LIST args;
    VA_START( args, aFmt );
    RFileLogger::WriteFormat( KDir, KName, EFileLoggingModeAppend, aFmt, args );
    VA_END( args );
    }

/**
 * Writes data dump in hex format in "[ 00 01 FF ]" style.
 * 
 * @param aPrefix Descriptor containing the string to be appended in 
 *                front of the actual data dump.
 * @param aData   An 8-bit descriptor containing the data buffer for which 
 *                the data dump is written.
 */
inline void DataDumpTrace( const TDesC& aPrefix, const TDesC8& aData )
    {
    _LIT( KSATraceDataDumpStart, " [ " );
    _LIT( KSATraceDataDumpLineAlign, "   " );
    _LIT( KSATraceDataDumpStop, " ] " );
    _LIT( KSATraceDataDumpValue, "%02x " );
    const TInt KSASDataDumpTraceBytesPerLine = 10;
    
    HBufC* buffer = HBufC::New( 255 );

    if ( buffer != NULL )
        {
        buffer->Des().Copy( aPrefix );
        buffer->Des().Append( KSATraceDataDumpStart );
        for ( TInt i = 0; i < aData.Length(); i++)
            {
            buffer->Des().AppendFormat( KSATraceDataDumpValue,  aData[i] );
            
            if( ( i % KSASDataDumpTraceBytesPerLine == ( KSASDataDumpTraceBytesPerLine - 1 ) ) && 
                ( i + 1 < aData.Length() ) )
                {
                RDebug::Print( buffer->Des() );

                buffer->Des().Copy( aPrefix);
                buffer->Des().Append( KSATraceDataDumpLineAlign );
                }
            }
        buffer->Des().Append( KSATraceDataDumpStop );
            
        RDebug::Print( buffer->Des() );
                
        }

    delete buffer;
    }


#ifdef _DEBUG

    #ifdef USE_FILE_LOGGING
       
        #define COMPONENT_TRACE( a ) HwrmWriteFormat a 
        #define DATADUMP_TRACE( a, b ) 
        #define API_TRACE( a ) HwrmWriteFormat a 
        
    #else

        #define COMPONENT_TRACE( a ) RDebug::Print a 
        #define DATADUMP_TRACE( a, b ) DataDumpTrace( a, b ) 
        #define API_TRACE( a ) RDebug::Print a
            
    #endif // #ifdef USE_FILE_LOGGING

#else // #ifdef _DEBUG

    #define COMPONENT_TRACE( a )
    #define API_TRACE( a )
    #define DATADUMP_TRACE( a, b ) 

#endif //#ifdef _DEBUG
#endif //#ifndef HWRMHAPTICSTRACE_H


//  End of File
