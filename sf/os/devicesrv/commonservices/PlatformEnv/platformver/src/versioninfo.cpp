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
* Description:  Interface for quering system version information.
*
*/


//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "versioninfo.h"
#include "platformverdebug.h"

// CONSTANTS
_LIT( KS60ProductIDFiles, "z:\\system\\install\\series60v*.sis" );
const TInt KS60ProductIDPos = 9; // Length of 'series60v'
_LIT( KS60ProductIDSplitMark, "." ); // For splitting 'XX.YY' to 'XX' and 'YY'


// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// GetPlatformVersion
// -----------------------------------------------------------------------------
//
static TInt GetPlatformVersion(
        VersionInfo::TPlatformVersion& aVersion,
        RFs& aFs )
    {
    FUNC_LOG

    CDir* productIds = NULL;
    TFindFile find( aFs );
    TInt ret( find.FindWildByPath( KS60ProductIDFiles, NULL, productIds ) );
    if ( ret == KErrNone && productIds )
        {
        ret = productIds->Sort( ESortByName | EDescending );
        }

    TPtrC ptr;
    TInt splitPos( KErrNotFound );
    if ( ret == KErrNone && productIds )
        {
        // Get version string 'XX.YY'
        TParsePtrC parse( ( *productIds )[ 0 ].iName );
        ptr.Set( parse.Name().Mid( KS60ProductIDPos ) );
        splitPos = ptr.Find( KS60ProductIDSplitMark );
        }

    if ( splitPos != KErrNotFound )
        {
        // Get major version
        TLex lex( ptr.Left( splitPos ) );
        ret = lex.Val( aVersion.iMajorVersion, EDecimal );
        if ( ret == KErrNone )
            {
            // Get minor version
            lex.Assign( ptr.Mid(
                splitPos + KS60ProductIDSplitMark().Length() ) );
            ret = lex.Val( aVersion.iMinorVersion, EDecimal );
            }
        }

    if ( !productIds || splitPos == KErrNotFound || ret == KErrNotFound ||
        ret == KErrPathNotFound )
        {
        ret = KErrNotSupported; // Use not supported if info is unavailable
        }

    delete productIds;
    return ret;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// VersionInfo::GetVersion
// -----------------------------------------------------------------------------
EXPORT_C TInt VersionInfo::GetVersion( TVersionBase& aVersion )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    RFs fs;
    TInt ret( fs.Connect() );
    if ( ret == KErrNone )
        {
        ret = GetVersion( aVersion, fs );
        fs.Close();
        }

    LOG_IF_ERROR2(
        ret,
        "VersionInfo::GetVersion-aType=%d,ret=%d",
        aVersion.iType, ret )

    return ret;
    }

// -----------------------------------------------------------------------------
// VersionInfo::GetVersion
// -----------------------------------------------------------------------------
EXPORT_C TInt VersionInfo::GetVersion( TVersionBase& aVersion, RFs& aFs )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    // Make sure that trap handler exists because used APIs may 
    // require cleanup stack even any leaving methods are not called.
    CTrapCleanup* cs = NULL;
    if ( !User::TrapHandler() )
        {
        // Setup temporary trap handler
        cs = CTrapCleanup::New();
        if ( !cs )
            {
            ERROR_LOG2(
                "VersionInfo::GetVersion-type=%d,ret=%d",
                aVersion.iType, KErrNoMemory )

            return KErrNoMemory;
            }
        }

    TInt ret( KErrNotSupported );
    switch( aVersion.iType )
        {
        case EPlatformVersion:
            {
            ret = GetPlatformVersion(
                static_cast< TPlatformVersion& >( aVersion ), aFs );
            break;
            }
        default:
            {
            break;
            }
        }

    if ( cs )
        {
        // Remove temporary trap handler
        User::SetTrapHandler( NULL );
        delete cs;
        }

    LOG_IF_ERROR2( ret,
        "VersionInfo::GetVersion-type=%d,ret=%d",
        aVersion.iType, ret )

    return ret;
    }

//  End of File  
