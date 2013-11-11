/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Holds information of system paths.
*                These utility methods should be used instead of hard coded.
*
*/



// INCLUDE FILES
#include    "pathinfo.h"
#include    "pathconfiguration.hrh"
#include    <e32std.h>
#include    <f32file.h>
#include    <badesca.h>
#include    "platformenvdebug.h"

// MACROS

// needed because _LIT macro does not expand parameter, which is also macro
#define _CREATE_LIT(a,b) _LIT(a,b)

#define ARRAY_LEN( a ) ( sizeof( a ) / sizeof( a[ 0 ] ) )

// CONSTANTS

_CREATE_LIT( KRomRootPath,              text_rom_root_path );
_CREATE_LIT( KPhoneMemoryRootPath,      text_phone_memory_root_path );
_CREATE_LIT( KMemoryCardRootPath,       text_memory_card_root_path );

_CREATE_LIT( KGamesPath,                text_games_path );
_CREATE_LIT( KInstallsPath,             text_installs_path );
_CREATE_LIT( KOthersPath,               text_others_path );
_CREATE_LIT( KVideosPath,               text_videos_path );
_CREATE_LIT( KImagesPath,               text_images_path );
_CREATE_LIT( KImagesThumbnailPath,      text_images_thumbnail_path );
_CREATE_LIT( KPicturesPath,             text_pictures_path );
_CREATE_LIT( KGmsPicturesPath,          text_gms_pictures_path );
_CREATE_LIT( KMmsBackgroundImagesPath,  text_mms_background_images_path );
_CREATE_LIT( KPresenceLogosPath,        text_presence_logos_path );
_CREATE_LIT( KSoundsPath,               text_sounds_path );
_CREATE_LIT( KDigitalSoundsPath,        text_digital_sounds_path );
_CREATE_LIT( KSimpleSoundsPath,         text_simple_sounds_path );
_CREATE_LIT( KMemoryCardContactsPath,   text_memory_card_contacts_path );

_LIT( KPanicCategory, "PATHINFO" );

_LIT( KDriveLetterMatch, "?:\\*" );
_LIT( KDriveDefaultPath, ":\\" );
const TInt KDriveNameLen = 3; // Length of drive letter, colon and backslash

// Path property bit mask definitions
enum TPathInfoProperty
    {
    EPathInfoPropNone = 0x0, // No special handling is required
    EPathInfoPropAppendDenied = 0x1, // Append to root path is denied
    EPathInfoPropDriveSpecific = 0x2, // Path is strictly drive specific
    EPathInfoPropUnavailableForPhoneMem = 0x4 // Path is unavailable for default phone memory
    };

// Path property definitions that must exist for each path
const TUint KPathProperties[] =
    {
    EPathInfoPropDriveSpecific,  // PathInfo::ERomRootPath, 0
    EPathInfoPropDriveSpecific,  // PathInfo::EPhoneMemoryRootPath, 1
    EPathInfoPropDriveSpecific,  // PathInfo::EMemoryCardRootPath, 2
    EPathInfoPropNone,  // PathInfo::EGamesPath, 3
    EPathInfoPropNone,  // PathInfo::EInstallsPath, 4
    EPathInfoPropNone,  // PathInfo::EOthersPath, 5
    EPathInfoPropNone,  // PathInfo::EVideosPath, 6
    EPathInfoPropNone,  // PathInfo::EImagesPath, 7
    EPathInfoPropNone,  // PathInfo::EGsmPicturesPath, 8
    EPathInfoPropNone,  // PathInfo::EMmsBackgroundImagesPath, 9
    EPathInfoPropNone,  // PathInfo::EPresenceLogosPath, 10
    EPathInfoPropNone,  // PathInfo::ESoundsPath, 11
    EPathInfoPropNone,  // PathInfo::EDigitalSoundsPath, 12
    EPathInfoPropNone,  // PathInfo::ESimpleSoundsPath, 13
    EPathInfoPropAppendDenied, // PathInfo::EImagesThumbnailPath, 14
    EPathInfoPropUnavailableForPhoneMem // PathInfo::EMemoryCardContactsPath, 15
    };

// DATA TYPES

enum TPathInfoPanic
    {
    EInvalidParameter        = 0 // Invalid parameter.
    };

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// ThumbnailPath
// -----------------------------------------------------------------------------
//
static TBool ThumbnailPath( const TDesC& aFullPath )
    {
    FUNC_LOG

    TInt len( KImagesThumbnailPath().Length() );
    if ( aFullPath.Length() >= len &&
        !aFullPath.Right( len ).CompareF( KImagesThumbnailPath ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PathInfo::RomRootPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::RomRootPath()
    {
    return KRomRootPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::PhoneMemoryRootPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::PhoneMemoryRootPath()
    {
    return KPhoneMemoryRootPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::MemoryCardRootPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::MemoryCardRootPath()
    {
    return KMemoryCardRootPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::GamesPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::GamesPath()
    {
    return KGamesPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::InstallsPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::InstallsPath()
    {
    return KInstallsPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::OthersPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::OthersPath()
    {
    return KOthersPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::VideosPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::VideosPath()
    {
    return KVideosPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::ImagesPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::ImagesPath()
    {
    return KImagesPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::ImagesThumbnailPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::ImagesThumbnailPath()
    {
    return KImagesThumbnailPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::PicturesPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::PicturesPath()
    {
    return KPicturesPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::GmsPicturesPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::GmsPicturesPath()
    {
    return KGmsPicturesPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::MmsBackgroundImagesPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::MmsBackgroundImagesPath()
    {
    return KMmsBackgroundImagesPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::PresenceLogosPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::PresenceLogosPath()
    {
    return KPresenceLogosPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::SoundsPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::SoundsPath()
    {
    return KSoundsPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::DigitalSoundsPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::DigitalSoundsPath()
    {
    return KDigitalSoundsPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::SimpleSoundsPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::SimpleSoundsPath()
    {
    return KSimpleSoundsPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::MemoryCardContactsPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::MemoryCardContactsPath()
    {
    return KMemoryCardContactsPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::GetPath
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& PathInfo::GetPath( TInt aPath )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    switch( aPath )
        {
        case PathInfo::ERomRootPath:
            return KRomRootPath;

        case PathInfo::EPhoneMemoryRootPath:
            return KPhoneMemoryRootPath;

	    case PathInfo::EMemoryCardRootPath: 
            return KMemoryCardRootPath;

        case PathInfo::EGamesPath:
            return KGamesPath;

        case PathInfo::EInstallsPath:
            return KInstallsPath;
            
        case PathInfo::EOthersPath:
            return KOthersPath;

        case PathInfo::EVideosPath:
            return KVideosPath;

        case PathInfo::EImagesPath:
            return KImagesPath;

        case PathInfo::EGsmPicturesPath:
            return KGmsPicturesPath;

        case PathInfo::EMmsBackgroundImagesPath:
            return KMmsBackgroundImagesPath;

        case PathInfo::EPresenceLogosPath:
            return KPresenceLogosPath;

        case PathInfo::ESoundsPath:   
            return KSoundsPath;
         
        case PathInfo::EDigitalSoundsPath:
            return KDigitalSoundsPath;

        case PathInfo::ESimpleSoundsPath:
            return KSimpleSoundsPath;

        case PathInfo::EImagesThumbnailPath:
            return KImagesThumbnailPath;

        case PathInfo::EMemoryCardContactsPath:  
            return KMemoryCardContactsPath; 
        
        default:
            User::Panic( KPanicCategory, EInvalidParameter );
            // To get rid of compilation warnings.
            return KNullDesC;

        }
    }

// -----------------------------------------------------------------------------
// PathInfo::GetRootPath
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PathInfo::GetRootPath( TDes& aRootPath, TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    aRootPath.Zero();
    TChar ch( 0 );
    TInt ret( RFs::DriveToChar( aDrive, ch ) );
    if ( ret == KErrNone )
        {
        if ( ch == KRomRootPath()[ 0 ] )
            {
            aRootPath.Copy( KRomRootPath );
            }
        else if ( ch == KPhoneMemoryRootPath()[ 0 ] )
            {
            aRootPath.Copy( KPhoneMemoryRootPath );
            }
        else if ( ch == KMemoryCardRootPath()[ 0 ] )
            {
            aRootPath.Copy( KMemoryCardRootPath );
            }
        else
            {
            aRootPath.Append( ch );
            aRootPath.Append( KDriveDefaultPath );
            }
        }

    INFO_LOG2( "PathInfo::GetRootPath-aRootPath=%S,aDrive=%d",
        &aRootPath, aDrive )

    LOG_IF_ERROR2( ret, "PathInfo::GetRootPath-aDrive=%d,ret=%d",
        aDrive, ret )

    return ret;
    }

// -----------------------------------------------------------------------------
// PathInfo::GetFullPath
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PathInfo::GetFullPath( TDes& aFullPath, TInt aDrive, TInt aPath )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    aFullPath.Zero();

    // Check if requested path is valid or not
    TInt count( ARRAY_LEN( KPathProperties ) );
    if ( aPath < 0 || aPath >= count )
        {
        ERROR_LOG3( "PathInfo::GetFullPath-aDrive=%d,aPath=%d,ret=%d",
            aDrive, aPath, KErrArgument )

        return KErrArgument; // Invalid path value
        }

    TUint pathProp( KPathProperties[ aPath ] );

    INFO_LOG2( "PathInfo::GetFullPath-aPath=%d,pathProp=0x%x",
        aPath, pathProp )

    if ( pathProp & EPathInfoPropAppendDenied )
        {
        return KErrNotFound; // Cannot append path to root path
        }

    TChar ch( 0 );
    TInt ret( RFs::DriveToChar( aDrive, ch ) );
    if ( ret != KErrNone )
        {
        ERROR_LOG3( "PathInfo::GetFullPath-aDrive=%d,aPath=%d,ret=%d",
            aDrive, aPath, ret )

        return ret; // No drive letter
        }

    if ( pathProp & EPathInfoPropUnavailableForPhoneMem )
        {
        if ( ch == KPhoneMemoryRootPath()[ 0 ] )
            {
            return KErrNotFound; // Drive is phone memory
            }
        }

    TPtrC path( GetPath( aPath ) );
    if ( !path.MatchF( KDriveLetterMatch ) )
        {
        if ( pathProp & EPathInfoPropDriveSpecific )
            {
            // The path is drive specific
            if ( ch == path[ 0 ] )
                {
                aFullPath.Copy( path );
                }
            else
                {
                ret = KErrNotFound;
                }
            }
        else
            {
            // Apply path to requested drive
            ret = GetRootPath( aFullPath, aDrive );
            if ( ret == KErrNone )
                {
                aFullPath.Append( path.Mid( KDriveNameLen ) );
                }
            }
        }
    else
        {
        // Apply path to requested drive
        ret = GetRootPath( aFullPath, aDrive );
        if ( ret == KErrNone )
            {
            aFullPath.Append( path );
            }
        }

    INFO_LOG3( "PathInfo::GetFullPath-aFullPath=%S,aPath=%d,ret=%d",
        &aFullPath, aPath, ret )

#ifdef PLATFORM_ENV_ERROR_LOG
    // Trace only unexpected errors, KErrNotFound is ok at this point
    if ( ret != KErrNone && ret != KErrNotFound )
        {
        ERROR_LOG3( "PathInfo::GetFullPath-aDrive=%d,aPath=%d,ret=%d",
            aDrive, aPath, ret )
        }
#endif // PLATFORM_ENV_ERROR_LOG

    if ( ret != KErrNone )
        {
        aFullPath.Zero();
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// PathInfo::PathType
// -----------------------------------------------------------------------------
//
EXPORT_C TInt PathInfo::PathType( const TDesC& aFullPath )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    if ( aFullPath.MatchF( KDriveLetterMatch ) )
        {
        INFO_LOG2( "PathInfo::PathType-aFullPath=%S,ret=%d",
            &aFullPath, ENotSystemPath )

        return ENotSystemPath;
        }

    // Thumbnail path needs different handling because
    // it is used differently compared to other paths
    if ( ThumbnailPath( aFullPath ) )
        {
        INFO_LOG2( "PathInfo::PathType-aFullPath=%S,ret=%d",
            &aFullPath, EImagesThumbnailPath )

        return EImagesThumbnailPath;
        }

    TInt drive( 0 );
    if ( RFs::CharToDrive( aFullPath[ 0 ], drive ) != KErrNone )
        {
        INFO_LOG2( "PathInfo::PathType-aFullPath=%S,ret=%d",
            &aFullPath, ENotSystemPath )

        return ENotSystemPath;
        }

    // Get the path count from the property array
    TInt count( ARRAY_LEN( KPathProperties ) );

    TFileName path;
    for( TInt i( 0 ); i < count; ++i )
        {
        if ( GetFullPath( path, drive, i ) == KErrNone )
            {
            if( !aFullPath.CompareF( path ) )
                {
                INFO_LOG2( "PathInfo::PathType-aFullPath=%S,ret=%d",
                    &aFullPath, i )

                return i;
                }
            }
        }

    INFO_LOG2( "PathInfo::PathType-aFullPath=%S,ret=%d",
        &aFullPath, ENotSystemPath )

    return ENotSystemPath;
    }

// -----------------------------------------------------------------------------
// PathInfo::GetListOfPathsLC
// -----------------------------------------------------------------------------
//
EXPORT_C CDesCArray* PathInfo::GetListOfPathsLC( TInt aDrive )
    {
    CDesCArray* ret = NULL;
    FUNC_LOG_WITH_CLIENT_NAME_LC( ret )

    ret = GetListOfPathsL( aDrive );
    CleanupStack::PushL( ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// PathInfo::GetListOfPathsL
// -----------------------------------------------------------------------------
//
EXPORT_C CDesCArray* PathInfo::GetListOfPathsL( TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    // Get the path count from the property array
    TInt count( ARRAY_LEN( KPathProperties ) );
    CDesCArray* ret = new( ELeave ) CDesCArrayFlat( count );
    CleanupStack::PushL( ret );
    TFileName path;
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( GetFullPath( path, aDrive, i ) == KErrNone )
            {
            ret->AppendL( path );
            }
        }
    CleanupStack::Pop( ret );
    return ret;
    }

//  End of File  
