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
* Description:  Provides version information
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>
#include <extendedversioninfoplugin.h>
#include <extendedversioninfoplugin.hrh>
#include <data_caging_paths_strings.hrh>
#include "sysversioninfo.h"
#include "sysversioninfodebug.h"

// MACROS
// needed because _LIT macro does not expand parameter, which is also macro
#define _SYSVERSION_CREATE_LIT(a,b) _LIT(a,b)

// CONSTANTS
const TInt KMaxTextLineLength = 256;
_LIT( KNewLinePattern, "\\n" );
_LIT( KNewline, "\n" );

// Model version string
_LIT( KModelVersionFileName, "Z:\\resource\\versions\\model.txt" );

// Firmware version
_LIT( KFWIDVersionFileName, "Z:\\resource\\versions\\fwid*.txt" );
_LIT( KFWIDVersionTag, "version=" );
_LIT( KFWIDVersionSeparator, " " );

// Customer variant version
_LIT( KOPIDVersionFileName, "Z:\\resource\\versions\\customersw.txt" );

// Product version
_LIT( KProductVersionFileName, "Z:\\resource\\versions\\product.txt" );
_LIT( KManufacturerTag, "Manufacturer=" );
_LIT( KModelTag, "Model=" );
_LIT( KProductTag, "Product=" );
_LIT( KRevisionTag, "Revision=" );

// Platform version
_LIT( KPlatformVersionFileName, "Z:\\resource\\versions\\platform.txt" );
_LIT( KSymbianOSMajorVersionTag, "SymbianOSMajorVersion=" );
_LIT( KSymbianOSMinorVersionTag, "SymbianOSMinorVersion=" );


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// AppendTillMax
// ---------------------------------------------------------------------------
//
static TInt AppendTillMax( TDes& aDst, const TDesC& aSrc )
    {
    FUNC_LOG

    TInt ret( KErrNone );
    TInt maxLen( aDst.MaxLength() - aDst.Length() );
    if ( aSrc.Length() <= maxLen )
        {
        aDst.Append( aSrc );
        }
    else
        {
        aDst.Append( aSrc.Left( maxLen ) );
        ret = KErrTooBig;
        }
    LOG_IF_ERROR1( ret, "AppendTillMax-ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// RemoveNewLines
// ---------------------------------------------------------------------------
//
static void RemoveNewLines( TDes& aValue )
    {
    FUNC_LOG

    // Replace new-line patterns with real ones
    TInt lnPatLen( KNewLinePattern().Length() );
    TInt lnLen( KNewline().Length() );
    TInt lnDiff( lnPatLen - lnLen );
    TInt len( aValue.Length() );
    TPtr ptr( aValue.MidTPtr( 0 ) );
    TInt pos( ptr.Find( KNewLinePattern ) );
    while ( pos != KErrNotFound )
        {
        ptr.Replace( pos, lnPatLen, KNewline );
        len -= lnDiff;
        ptr.Set( ptr.MidTPtr( pos ) );
        pos = ptr.Find( KNewLinePattern );
        }
    aValue.SetLength( len );
    }

// ---------------------------------------------------------------------------
// GetTextFromFile
// ---------------------------------------------------------------------------
//
static TInt GetTextFromFile(
        RFile& aFile,
        TDes& aValue,
        const TDesC& aTag = KNullDesC )
    {
    FUNC_LOG

    // Create line buffer and fetch text from file
    HBufC* buffer = HBufC::New( KMaxTextLineLength );
    if ( !buffer )
        {
        ERROR_LOG1( "GetTextFromFile-ret=%d", KErrNoMemory )
        return KErrNoMemory;
        }
    TPtr ptr( buffer->Des() );
    TFileText tf;
    tf.Set( aFile );
    TInt tagLen( aTag.Length() );
    TInt ret( KErrNone );
    for( ;; )
        {
        // Fetch next text line
        ptr.Zero();
        ret = tf.Read( ptr );
        if ( ret != KErrNone && ret != KErrTooBig )
            {
            break; // Unexpected error did happen
            }
        if ( !tagLen || !ptr.Left( tagLen ).CompareF( aTag ) )
            {
            TInt ret2( AppendTillMax( aValue, ptr.Mid( tagLen ) ) );
            if ( ret == KErrNone && ret2 != KErrNone )
                {
                ret = ret2;
                }
            break; // Text read was successful
            }
        }
    if ( ret == KErrEof )
        {
        ret = KErrNone; // Suppress end of file
        }
    delete buffer;
    LOG_IF_ERROR1( ret, "GetTextFromFile-ret=%d", ret )
    return ret;
    }
// ---------------------------------------------------------------------------
// GetTextFromFile
// ---------------------------------------------------------------------------
//
static TInt GetTextFromFile(
        RFs& aFs,
        const TDesC& aFilename,
        TDes& aValue,
        TBool aRemoveNewLines,
        const TDesC& aTag = KNullDesC )
    {
    FUNC_LOG

    RFile file;
    TInt ret( file.Open( aFs, aFilename,
        EFileRead | EFileStreamText | EFileShareReadersOnly ) );
    if ( ret == KErrNone )
        {
        ret = GetTextFromFile( file, aValue, aTag );
        file.Close();
        }
    else if ( ret == KErrNotFound || ret == KErrPathNotFound )
        {
        ret = KErrNotSupported;
        }
    if ( aRemoveNewLines &&
        ( ret == KErrNone || ret == KErrTooBig ) )
        {
        RemoveNewLines( aValue );
        }
    LOG_IF_ERROR1( ret, "GetTextFromFile-2-ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// GetFWIDTextFromFile
// ---------------------------------------------------------------------------
//
static TInt GetFWIDTextFromFile( RFs& aFs, TDes& aValue )
    {
    FUNC_LOG

    // Solve file names and concatenate tagged info from each file
    HBufC* buffer = HBufC::New( KMaxFileName );
    if ( !buffer )
        {
        ERROR_LOG1( "GetFWIDTextFromFile-ret=%d", KErrNoMemory )
        return KErrNoMemory;
        }
    TPtr ptr( buffer->Des() );
    TFindFile find( aFs );
    CDir* files = NULL;
    TInt ret( find.FindWildByPath( KFWIDVersionFileName, NULL, files ) );
    if ( ret == KErrNone && files )
        {
        TPtrC driveAndPath(
            TParsePtrC( KFWIDVersionFileName ).DriveAndPath() );
        TInt count( files->Count() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            // Setup file name and read info
            ptr.Copy( driveAndPath );
            ptr.Append( ( *files )[ i ].iName );
            TInt ret2( GetTextFromFile(
                aFs, ptr, aValue, EFalse, KFWIDVersionTag ) );
            if ( ret2 != KErrNone )
                {
                ret = ret2;
                }
            else if ( i + 1 < count )
                {
                // Separate infos read from different files
                ret = AppendTillMax( aValue, KFWIDVersionSeparator );
                }
            if ( ret != KErrNone )
                {
                break;
                }
            }
        if ( !count )
            {
            ret = KErrNotSupported;
            }
        }
    else if ( ret == KErrNotFound || ret == KErrPathNotFound )
        {
        ret = KErrNotSupported;
        }
    delete files;
    delete buffer;
    if ( ret == KErrNone || ret == KErrTooBig )
        {
        RemoveNewLines( aValue );
        }
    LOG_IF_ERROR1( ret, "GetFWIDTextFromFile-ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// ReadTextFile
// ---------------------------------------------------------------------------
//
static TInt ReadTextFile(
        RFile& aFile,
        TDes& aBuffer,
        RArray< TPtrC >& aLineBuffer )
    {
    FUNC_LOG

    // Read text file into buffers
    TInt ret( KErrNone );
    TFileText tf;
    tf.Set( aFile );
    aBuffer.SetMax();
    TPtr ptr( aBuffer.MidTPtr( 0 ) );
    TInt used( 0 );
    do
        {
        ret = tf.Read( ptr );
        TInt len( ptr.Length() );
        if ( ( ret == KErrNone || ret == KErrEof ) && len > 0 )
            {
            // Store non empty text line
            TInt err( aLineBuffer.Append( ptr ) );
            if ( err == KErrNone )
                {
                ptr.SetMax();
                ptr.Set( ptr.MidTPtr( len ) );
                ptr.Zero();
                used += len;
                }
            else
                {
                ret = err;
                }
            }
        }
    while ( ret == KErrNone );
    if ( ret == KErrEof )
        {
        ret = KErrNone;
        }
    aBuffer.SetLength( used );
    LOG_IF_ERROR1( ret, "ReadTextFile-ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// ReadTextFile
// ---------------------------------------------------------------------------
//
static HBufC* ReadTextFile(
        RFs& aFs,
        const TDesC& aFullPath,
        RArray< TPtrC >& aLineBuffer,
        TInt& aError )
    {
    FUNC_LOG

    RFile file;
    aError = file.Open( aFs, aFullPath,
        EFileRead | EFileStreamText | EFileShareReadersOnly );
    if ( aError != KErrNone )
        {
        ERROR_LOG1( "ReadTextFile-2-aError=%d", aError )
        return NULL;
        }
    TInt size( 0 );
    aError = file.Size( size );
    if ( aError != KErrNone )
        {
        file.Close();
        ERROR_LOG1( "ReadTextFile-3-aError=%d", aError )
        return NULL;
        }
    // Get text size, create buffer for text and read text file
    HBufC* ret = HBufC::New( ( size + sizeof( TText ) - 1 ) / sizeof( TText ) );
    if ( !ret )
        {
        aError = KErrNoMemory;
        file.Close();
        ERROR_LOG2( "ReadTextFile-4-aError=%d,size=%d", aError, size )
        return NULL;
        }
    TPtr ptr( ret->Des() );
    aError = ReadTextFile( file, ptr, aLineBuffer );
    file.Close();
    if ( aError != KErrNone )
        {
        delete ret;
        aLineBuffer.Reset();
        ERROR_LOG1( "ReadTextFile-5-aError=%d", aError )
        return NULL;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// GetTagValue
// ---------------------------------------------------------------------------
//
static TPtrC GetTagValue(
        const TDesC& aTag,
        const RArray< TPtrC >& aLineBuffer,
        TInt& aError )
    {
    FUNC_LOG

    aError = KErrNotFound;
    TPtrC ret( KNullDesC );
    TInt tagLen( aTag.Length() );
    TInt count( aLineBuffer.Count() );
    for( TInt i( 0 ); i < count; ++i )
        {
        TPtrC line( aLineBuffer[ i ] );
        if ( !line.Left( tagLen ).CompareF( aTag ) )
            {
            ret.Set( line.Mid( tagLen ) );
            aError = KErrNone;
            break;
            }
        }
    LOG_IF_ERROR1( aError, "GetTagValue-aError=%d", aError )
    return ret;
    }

// ---------------------------------------------------------------------------
// GetTagValue
// ---------------------------------------------------------------------------
//
static TInt GetTagValue(
        TDes& aValue,
        const TDesC& aTag,
        const RArray< TPtrC >& aLineBuffer )
    {
    FUNC_LOG

    aValue.Zero();
    TInt ret( KErrNotFound );
    TPtrC value( GetTagValue( aTag, aLineBuffer, ret ) );
    if ( ret == KErrNone )
        {
        aValue.Copy( value.Left(
            Min( aValue.MaxLength(), value.Length() ) ) );
        RemoveNewLines( aValue );
        }
    LOG_IF_ERROR1( ret, "GetTagValue-ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// GetTagValue
// ---------------------------------------------------------------------------
//
static TInt GetTagValue(
        TInt& aValue,
        const TDesC& aTag,
        const RArray< TPtrC >& aLineBuffer )
    {
    FUNC_LOG

    aValue = 0;
    TInt ret( KErrNotFound );
    TPtrC value( GetTagValue( aTag, aLineBuffer, ret ) );
    if ( ret == KErrNone )
        {
        TLex lex( value );
        ret = lex.Val( aValue );
        if ( ret != KErrNone )
            {
            aValue = 0;
            }
        }
    LOG_IF_ERROR1( ret, "GetTagValue-2-ret=%d", ret )
    return ret;
    }

// ----------------------------------------------------------------------------
// ReadSymbianOSVersion
// ----------------------------------------------------------------------------
//
static TInt ReadSymbianOSVersion(
        SysVersionInfo::TSymbianOSVersion& aOSVersion, RFs& aFs )
    {
    FUNC_LOG

    RArray< TPtrC > lineBuffer;
    TInt value( 0 );
    TInt ret( KErrNone );
    HBufC* buffer = ReadTextFile(
        aFs, KPlatformVersionFileName, lineBuffer, ret );
    if ( buffer )
        {
        ret = GetTagValue( value, KSymbianOSMajorVersionTag, lineBuffer );
        if ( ret == KErrNone )
            {
            aOSVersion.iMajorVersion = value;
            }
        if ( ret == KErrNone )
            {
            ret = GetTagValue( value, KSymbianOSMinorVersionTag, lineBuffer );
            if ( ret == KErrNone )
                {
                aOSVersion.iMinorVersion = value;
                }
            }
        delete buffer;
        }
    lineBuffer.Close();
    if ( ret == KErrNotFound || ret == KErrPathNotFound )
        {
        ret = KErrNotSupported;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// ReadProductVersion
// ---------------------------------------------------------------------------
//
static TInt ReadProductVersion(
        SysVersionInfo::TProductVersion& aProductVersion, RFs& aFs )
    {
    FUNC_LOG

    RArray< TPtrC > lineBuffer;
    TInt ret( KErrNone );
    HBufC* buffer = ReadTextFile(
        aFs, KProductVersionFileName, lineBuffer, ret );
    if ( buffer )
        {
        // Ignore value not found errors, value just stays empty
        GetTagValue(
            aProductVersion.iManufacturer, KManufacturerTag, lineBuffer );
        GetTagValue(
            aProductVersion.iModel, KModelTag, lineBuffer );
        GetTagValue(
            aProductVersion.iProduct, KProductTag, lineBuffer );
        GetTagValue(
            aProductVersion.iRevision, KRevisionTag, lineBuffer );
        delete buffer;
        }
    lineBuffer.Close();
    if ( ret == KErrNotFound || ret == KErrPathNotFound )
        {
        ret = KErrNotSupported;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// ReadProductInformation
// ---------------------------------------------------------------------------
//
static TInt ReadProductInformation(
    SysVersionInfo::TVersionInfoType aType,
    TDes& aValue )
    {
    FUNC_LOG;

    using namespace ExtendedVersionInfoPluginApi;

    TFileName prodInfoDllPath;

    _SYSVERSION_CREATE_LIT( KPluginDllName, EXTVERSIONINFO_PLUGIN_STRING );
    _SYSVERSION_CREATE_LIT( KPluginFolder, SHARED_LIB_DIR );
    _LIT( KExtPluginPathFormat, "z:%S\\%S" );

    prodInfoDllPath.Format(
        KExtPluginPathFormat, &KPluginFolder, &KPluginDllName );

    RLibrary prodInfoDll;

    TInt ret = prodInfoDll.Load( prodInfoDllPath );


    if ( ret == KErrNone )
        {
        TLibraryFunction libFunc =
            prodInfoDll.Lookup( EGetProductInformationFuncL );

        if ( libFunc )
            {
            GetProductInformationFuncL getL =
                reinterpret_cast<GetProductInformationFuncL>(libFunc);
            TRAP( ret, ((*getL)( aType, aValue )) );
            }
        else
            {
            ret = KErrNotSupported;
            }
        prodInfoDll.Close();
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// SysVersionInfo::GetVersionInfo
// ---------------------------------------------------------------------------
//
EXPORT_C TInt SysVersionInfo::GetVersionInfo(
        const TVersionInfoType aType, TDes& aValue )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    RFs fs;
    TInt ret( fs.Connect() );
    if ( ret == KErrNone )
        {
        ret = GetVersionInfo( aType, aValue, fs );
        fs.Close();
        }
    else
        {
        aValue.Zero();
        }
    LOG_IF_ERROR2(
        ret,
        "SysVersionInfo::GetVersionInfo-aType=%d,ret=%d",
        aType, ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// SysVersionInfo::GetVersionInfo
// ---------------------------------------------------------------------------
//
EXPORT_C TInt SysVersionInfo::GetVersionInfo(
        const TVersionInfoType aType, TDes& aValue, RFs& aFs )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    aValue.Zero();

    TInt ret( KErrNotSupported );
    // if a version info text file does not exist attempt to read from
    // the adaptation interface
    TBool readProductInfo( EFalse );

    switch ( aType )
        {
        case EFWVersion:
            {
            ret = GetFWIDTextFromFile( aFs, aValue );
            readProductInfo = (ret == KErrNotSupported);
            break;
            }
        case EOPVersion:
            {
            ret = GetTextFromFile(
                aFs, KOPIDVersionFileName, aValue, ETrue );
            readProductInfo = (ret == KErrNotSupported);
            break;
            }
        case EModelVersion:
            {
            ret = GetTextFromFile(
                aFs, KModelVersionFileName, aValue, ETrue );
            readProductInfo = (ret == KErrNotSupported);
            break;
            }
        case EProductCode: // product code is fetched only from the plugin
            readProductInfo = ETrue;
            break;

        default:
            {
            break;
            }
        }

    if ( readProductInfo )
        {
        ret = ReadProductInformation( aType, aValue );
        }

    if ( ret != KErrNone && ret != KErrTooBig )
        {
        aValue.Zero();
        }
    LOG_IF_ERROR2(
        ret,
        "SysVersionInfo::GetVersionInfo-2-aType=%d,ret=%d",
        aType, ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// SysVersionInfo::GetVersionInfo
// ---------------------------------------------------------------------------
//
EXPORT_C TInt SysVersionInfo::GetVersionInfo( TVersionBase& aVersion )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    RFs fs;
    TInt ret( fs.Connect() );
    if ( ret == KErrNone )
        {
        ret = GetVersionInfo( aVersion, fs );
        fs.Close();
        }
    LOG_IF_ERROR2(
        ret,
        "SysVersionInfo::GetVersionInfo-3-iType=%d,ret=%d",
        aVersion.iType, ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// SysVersionInfo::GetVersionInfo
// ---------------------------------------------------------------------------
//
EXPORT_C TInt SysVersionInfo::GetVersionInfo(
        TVersionBase& aVersion, RFs& aFs )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    TInt ret( KErrNotSupported );
    switch( aVersion.iType )
        {
        case ESymbianOSVersion:
            {
            ret = ReadSymbianOSVersion(
                static_cast< TSymbianOSVersion& >( aVersion ), aFs );
            break;
            }
        case EProductVersion:
            {
            ret = ReadProductVersion(
                static_cast< TProductVersion& >( aVersion ), aFs );
            break;
            }
        default:
            {
            break;
            }
        }
    LOG_IF_ERROR2(
        ret,
        "SysVersionInfo::GetVersionInfo-4-iType=%d,ret=%d",
        aVersion.iType, ret )
    return ret;
    }

// End of File
