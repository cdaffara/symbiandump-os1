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
* Description:  Implementation of CTFAStifTestLog class
*
*/


// INCLUDE FILES
#include "dsytesttooltrace.h"
#include "tfastiftestlog.h"
#include <stiflogger.h>

#define __LOG_HTML__

class TTFAOverflowHandler : public TDes16Overflow, public TDes8Overflow
    {
public:
    void Overflow( TDes16& /*aDes*/ )
        {
        TRACE_ASSERT_ALWAYS;
        }

    void Overflow( TDes8& /*aDes*/ )
        {
        TRACE_ASSERT_ALWAYS;
        }
    };
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::NewLC
// -----------------------------------------------------------------------------
CTFAStifTestLog* CTFAStifTestLog::NewLC( void )
    {
    CTFAStifTestLog* log = new ( ELeave ) CTFAStifTestLog();
    CleanupStack::PushL( log );
    log->ConstructL();
    return log;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::CTFAStifTestLog
// -----------------------------------------------------------------------------
CTFAStifTestLog::CTFAStifTestLog( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::ConstructL
// -----------------------------------------------------------------------------
void CTFAStifTestLog::ConstructL( void )
    {
    TFileName fileName;
    TTime time;
    time.HomeTime();
    TDateTime dateTime = time.DateTime();
    RThread thread;
#ifdef __LOG_HTML__
    _LIT( KSuffix, "html" );
#else
    _LIT( KSuffix, "txt" );
#endif
    fileName.Format( _L( "%02d%02d%02d_%02d%02d%02d_%x.%S" ), 
        dateTime.Year() - 2000, dateTime.Month() + 1, dateTime.Day() + 1, 
        dateTime.Hour(), dateTime.Minute(), dateTime.Second(), 
        (TUint)thread.Id(), &KSuffix );
    iLogger = CStifLogger::NewL( _L( "c:\\logs\\testframework\\" ), fileName,
        CStifLogger::ETxt, CStifLogger::EFile, ETrue, EFalse, EFalse, EFalse, EFalse );
    iOverflowHandler = new ( ELeave ) TTFAOverflowHandler;
#ifdef __LOG_HTML__
    iLogger->Log( _L8( "<html><head><title>TFA Log</title></head>\r\n<body>\r\n" ) );
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::~CTFAStifTestLog
// -----------------------------------------------------------------------------
CTFAStifTestLog::~CTFAStifTestLog( void )
    {
#ifdef __LOG_HTML__
    if ( iLogger != NULL )
        {
        iLogger->Log( _L8( "\r\n</body></html>" ) );
        }
#endif
    delete iLogger;
    delete iOverflowHandler;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::SetStyle
// -----------------------------------------------------------------------------
void CTFAStifTestLog::SetStyle( TTFLogStyle aStyle )
    {
#ifdef __LOG_HTML__
    switch ( iStyle )
        {
        case ETFLogStyleBold:
            iLogger->Log( _L8( "</b>" ) );
            break;
        case ETFLogStyleItalic:
            iLogger->Log( _L8( "</i>" ) );
            break;
        case ETFLogStyleNormal:
        default:
            break;
        }
    iStyle = aStyle;
    switch ( iStyle )
        {
        case ETFLogStyleBold:
            iLogger->Log( _L8( "<b>" ) );
            break;
        case ETFLogStyleItalic:
            iLogger->Log( _L8( "<i>" ) );
            break;
        case ETFLogStyleNormal:
        default:
            break;
        }
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::SetColor
// -----------------------------------------------------------------------------
void CTFAStifTestLog::SetColor( TUint32 aColor )
    {
#ifdef __LOG_HTML__
    if ( iColor != 0 )
        {
        iLogBuffer8.Format( _L8( "</font>" ) );
        }
    iColor = aColor;
    if ( iColor != 0 )
        {
        iLogBuffer8.Format( _L8( "<font color=\"0x%x\">" ), aColor );
        }
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::Write
// -----------------------------------------------------------------------------
void CTFAStifTestLog::Write( const TDesC& aDes )
    {
    iLogger->Log( aDes );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::Write
// -----------------------------------------------------------------------------
void CTFAStifTestLog::Write( const TDesC8& aDes )
    {
    iLogger->Log( aDes );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::Write
// -----------------------------------------------------------------------------
void CTFAStifTestLog::Write( TRefByValue<const TDesC> aDes, ... )
    {
    VA_LIST list;
    //lint -esym(960, 42) Comma operator used outside of 'for' expression
    VA_START( list, aDes );
    //lint +esym(960, 42)
    iLogBuffer.SetLength( 0 );
    iLogBuffer.AppendFormatList( aDes, list, iOverflowHandler );
    iLogger->Log( iLogBuffer );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::Write
// -----------------------------------------------------------------------------
void CTFAStifTestLog::Write( TRefByValue<const TDesC8> aDes, ... )
    {
    VA_LIST list;
    //lint -esym(960, 42) Comma operator used outside of 'for' expression
    VA_START( list, aDes );
    //lint +esym(960, 42)
    iLogBuffer8.SetLength( 0 );
    iLogBuffer8.AppendFormatList( aDes, list, iOverflowHandler );
    iLogger->Log( iLogBuffer8 );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::WriteBlock
// -----------------------------------------------------------------------------
void CTFAStifTestLog::WriteBlock( const TDesC& aDes )
    {
#ifdef __LOG_HTML__
    iLogger->Log( _L8( "<pre><blockquote>" ) );
    iLogger->Log( aDes );
    iLogger->Log( _L8( "</blockquote></pre>" ) );
#else
    iLogger->Log( aDes );
    iLogger->Log( _L8( "\r\n" ) );
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::WriteBlock
// -----------------------------------------------------------------------------
void CTFAStifTestLog::WriteBlock( const TDesC8& aDes )
    {
#ifdef __LOG_HTML__
    iLogger->Log( _L8( "<pre><blockquote>" ) );
    iLogger->Log( aDes );
    iLogger->Log( _L8( "</blockquote></pre>" ) );
#else
    iLogger->Log( aDes );
    iLogger->Log( _L8( "\r\n" ) );
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::EndLine
// -----------------------------------------------------------------------------
void CTFAStifTestLog::EndLine( void )
    {
#ifdef __LOG_HTML__
    iLogger->Log( _L8( "<br>\r\n" ) );
#else
    iLogger->Log( _L8( "\r\n" ) );
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::StartChapter
// -----------------------------------------------------------------------------
void CTFAStifTestLog::StartChapter( void )
    {
#ifdef __LOG_HTML__
    iLogger->Log( _L8( "<p>" ) );
#else
    iLogger->Log( _L8( "\r\n\r\n" ) );
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::StartList
// -----------------------------------------------------------------------------
void CTFAStifTestLog::StartList( void )
    {
    iCurrentDepth = 0;
#ifdef __LOG_HTML__
    iLogger->Log( _L8( "<ul>" ) );
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::WriteListEntryStart
// -----------------------------------------------------------------------------
void CTFAStifTestLog::WriteListEntryStart( TInt aDepth )
    {
#ifdef __LOG_HTML__
    if ( iCurrentDepth > aDepth )
        {
        while ( iCurrentDepth > aDepth )
            {
            iLogger->Log( _L8( "</ul>\r\n" ) );
            iCurrentDepth--;
            }
        }
    else if ( iCurrentDepth < aDepth )
        {
        while ( iCurrentDepth < aDepth )
            {
            iLogger->Log( _L8( "<ul>" ) );
            iCurrentDepth++;
            }
        }
    else
        {
        }
#else
    if ( aDepth > iCurrentDepth )
        {
        iLogger->Log( _L8( ":\r\n" ) );
        }
    else
        {
        iLogger->Log( _L8( "\r\n" ) );
        }
    for ( TInt i = 0; i <= aDepth; i++ )
        {
        iLogger->Log( _L8( "  " ) );
        }
    iLogger->Log( _L8( "- " ) );
    iCurrentDepth = aDepth;
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::WriteList
// -----------------------------------------------------------------------------
void CTFAStifTestLog::WriteList( TInt aDepth, TRefByValue<const TDesC> aDes, ... )
    {
    WriteListEntryStart( aDepth );
    VA_LIST list;
    //lint -esym(960, 42) Comma operator used outside of 'for' expression
    VA_START( list, aDes );
    //lint +esym(960, 42)
#ifdef __LOG_HTML__
    iLogBuffer.Copy( _L( "<li>" ) );
#else
    iLogBuffer.SetLength( 0 );
#endif
    iLogBuffer.AppendFormatList( aDes, list, iOverflowHandler );
    iLogger->Log( iLogBuffer );
#ifdef __LOG_HTML__
    iLogger->Log( _L8( "</li>\r\n" ) );
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::WriteList
// -----------------------------------------------------------------------------
void CTFAStifTestLog::WriteList( TInt aDepth, TRefByValue<const TDesC8> aDes, ... )
    {
    WriteListEntryStart( aDepth );
    VA_LIST list;
    //lint -esym(960, 42) Comma operator used outside of 'for' expression
    VA_START( list, aDes );
    //lint +esym(960, 42)
#ifdef __LOG_HTML__
    iLogBuffer8.Copy( _L8( "<li>" ) );
#else
    iLogBuffer8.SetLength( 0 );
#endif
    iLogBuffer8.AppendFormatList( aDes, list, iOverflowHandler );
    iLogger->Log( iLogBuffer8 );
#ifdef __LOG_HTML__
    iLogger->Log( _L8( "</li>\r\n" ) );
#endif
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestLog::EndList
// -----------------------------------------------------------------------------
void CTFAStifTestLog::EndList( void )
    {
#ifdef __LOG_HTML__
    while ( iCurrentDepth >= 0 )
        {
        iLogger->Log( _L8( "</ul>\r\n" ) );
        iCurrentDepth--;
        }
#else
    iLogger->Log( _L8( "\r\n" ) );
#endif
    }
    
