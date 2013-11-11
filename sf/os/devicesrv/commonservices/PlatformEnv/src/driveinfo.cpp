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
* Description:  Holds the drive information of the system.
*
*/



// INCLUDE FILES
#include "driveinfo.h"
#include "driveinfopatchdata.h"
#include "platformenvdebug.h"

// CONSTANTS
const TInt KMaxFsName = 256;
_LIT( KMsFsName, "MassStorageFileSystem" );


// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// IsUserHiddenDrive
// -----------------------------------------------------------------------------
//
static TBool IsUserHiddenDrive( TInt aDrive )
    {
    FUNC_LOG

    return ( aDrive >= 0 &&
             ( aDrive == KDriveInfoUserHiddenDrive1 ||
               aDrive == KDriveInfoUserHiddenDrive2 ||
               aDrive == KDriveInfoUserHiddenDrive3 ||
               aDrive == KDriveInfoUserHiddenDrive4 ||
               aDrive == KDriveInfoUserHiddenDrive5 ||
               aDrive == KDriveInfoUserHiddenDrive6 ||
               aDrive == KDriveInfoUserHiddenDrive7 ||
               aDrive == KDriveInfoUserHiddenDrive8 ) );
    }

// -----------------------------------------------------------------------------
// IsUsbDrive
// -----------------------------------------------------------------------------
//
static TBool IsUsbDrive( TInt aDrive )
    {
    FUNC_LOG

    return ( aDrive >= 0 &&
             aDrive >= KDriveInfoUsbDrivesStart &&
             aDrive < KDriveInfoUsbDrivesStart + KDriveInfoUsbDrivesMaxCount );
    }

// -----------------------------------------------------------------------------
// ExternallyMountableInternalDrive
// -----------------------------------------------------------------------------
//
static TBool ExternallyMountableInternalDrive( TInt aDrive, TInt aDriveAtt )
    {
    FUNC_LOG

    if ( aDriveAtt & KDriveAttLogicallyRemovable )
        {
        return ETrue;
        }

    return ( aDrive >= 0 &&
             ( aDrive == KDriveInfoLogicallyRemovableDrive1 ||
               aDrive == KDriveInfoLogicallyRemovableDrive2 ||
               aDrive == KDriveInfoLogicallyRemovableDrive3 ||
               aDrive == KDriveInfoLogicallyRemovableDrive4 ) );
    }

// -----------------------------------------------------------------------------
// MassStorageFsInUse
// -----------------------------------------------------------------------------
//
static TBool MassStorageFsInUse( TInt aDrive, TInt aDriveAtt, RFs& aFs )
    {
    FUNC_LOG

    if ( !( aDriveAtt & KDriveAttRemovable ) )
        {
        return EFalse;
        }
    HBufC* buffer = HBufC::New( KMaxFsName );
    if ( !buffer )
        {
        return EFalse;
        }
    TBool ret( EFalse );
    TPtr ptr( buffer->Des() );
    if ( aFs.FileSystemName( ptr, aDrive ) == KErrNone )
        {
        if ( !ptr.CompareF( KMsFsName ) )
            {
            ret = ETrue;
            }
        }
    delete buffer;
    return ret;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DriveInfo::GetDefaultDrive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::GetDefaultDrive( TInt aDefaultDrive, TInt& aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    TInt ret( KErrNone );
    switch ( aDefaultDrive )
        {
        case DriveInfo::EDefaultRom:
            ret = KDriveInfoDefaultRomDrive;
            break;
        case DriveInfo::EDefaultRam:
            ret = KDriveInfoDefaultRamDrive;
            break;
        case DriveInfo::EDefaultSystem:
            ret = KDriveInfoDefaultSystemDrive >= 0 ?
                  KDriveInfoDefaultSystemDrive : RFs::GetSystemDrive();
            break;
        case DriveInfo::EDefaultPhoneMemory:
            ret = KDriveInfoDefaultPhoneMemory;
            break;
        case DriveInfo::EDefaultMassStorage:
            ret = KDriveInfoDefaultMassStorage;
            break;
        case DriveInfo::EDefaultRemovableMassStorage:
            ret = KDriveInfoDefaultRemovableMassStorage;
            break;
        default:
            ret = KErrArgument;
            break;
        }

    if ( ret >= 0 )
        {
        aDrive = ret;
        ret = KErrNone;
        }

    INFO_LOG2( "DriveInfo::GetDefaultDrive-aDefaultDrive=%d,aDrive=%d",
        aDefaultDrive, aDrive )

    LOG_IF_ERROR2( ret,
        "DriveInfo::GetDefaultDrive-aDefaultDrive=%d,ret=%d",
        aDefaultDrive, ret )

    return ret;
    }

// -----------------------------------------------------------------------------
// DriveInfo::GetDriveStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::GetDriveStatus(
        RFs& aFs, TInt aDrive, TUint& aStatus )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    aStatus = 0;

    if ( !RFs::IsValidDrive( aDrive ) )
        {
        ERROR_LOG2( "DriveInfo::GetDriveStatus-aDrive=%d,IsValidDriveRet=%d",
            aDrive, KErrArgument )

        return KErrArgument;
        }

    // Get volume info
    TVolumeInfo volumeInfo;
    TInt ret( aFs.Volume( volumeInfo, aDrive ) );

    INFO_LOG2( "DriveInfo::GetDriveStatus-aDrive=%d,VolumeInfoRet=%d",
        aDrive, ret )

    switch( ret )
        {
        case KErrNone:
            {
            if ( volumeInfo.iDrive.iType != EMediaNotPresent )
                {
                aStatus |= EDrivePresent;
                if( volumeInfo.iSize )
                    {
                    aStatus |= EDriveFormatted;
                    }
                }
            else
                {
                ERROR_LOG1(
                    "DriveInfo::GetDriveStatus-aDrive=%d,InvalidVolume",
                    aDrive )
                ret = KErrNotReady;
                }
            break;
            }
        case KErrLocked:
            {
            aStatus |= EDrivePresent | EDriveLocked | EDriveFormatted |
                EDriveLockable | EDriveHasPassword;
            break;
            }
        case KErrCorrupt:
            {
            aStatus |= EDrivePresent | EDriveCorrupt;
            break;
            }
        case KErrInUse:
            {
            aStatus |= EDrivePresent | EDriveInUse;
            break;
            }
        default:
            {
            break;
            }
        }

    // Get drive info if not already done by volume info read
    TDriveInfo& driveInfo( volumeInfo.iDrive );
    if ( ret != KErrNone )
        {
        TInt err( aFs.Drive( driveInfo, aDrive ) );

        INFO_LOG2( "DriveInfo::GetDriveStatus-aDrive=%d,DriveInfoRet=%d",
            aDrive, err )

        if ( err != KErrNone )
            {
            ERROR_LOG2( "DriveInfo::GetDriveStatus-aDrive=%d,DriveInfoRet=%d",
                aDrive, err )

            return err; // Nothing can be done
            }

        // Mass storage mode may return some other error than KErrInUse in 
        // volume info read. Check is mass storage file system in use.
        if( driveInfo.iType != EMediaNotPresent &&
            ret != KErrInUse &&
            MassStorageFsInUse( aDrive, driveInfo.iDriveAtt, aFs ) )
            {
            INFO_LOG1( "DriveInfo::GetDriveStatus-aDrive=%d,DriveInUse",
                aDrive )

            aStatus |= EDrivePresent | EDriveInUse;
            }
        }

    INFO_LOG3(
        "DriveInfo::GetDriveStatus-DriveAttr=%d,MediaAttr=%d,MediaType=%d",
        driveInfo.iDriveAtt,
        driveInfo.iMediaAtt,
        driveInfo.iType )

    // If drive attributes are set, suppress volume read error
    if ( driveInfo.iDriveAtt )
        {
        ret = KErrNone;
        }

    if ( !IsUserHiddenDrive( aDrive ) )
        {
        aStatus |= EDriveUserVisible;
        }

    // Check media type
    TInt defaultRom( KErrNotFound );
    GetDefaultDrive( EDefaultRom, defaultRom );
    TInt defaultPhoneMem( KErrNotFound );
    GetDefaultDrive( EDefaultPhoneMemory, defaultPhoneMem );

    if ( driveInfo.iType == EMediaRom || defaultRom == aDrive )
        {
        aStatus |= EDriveRom | EDriveReadOnly;
        }
    // Default phone memory should always be non volatile media.
    // If it is defined to RAM media, there is something wrong in 
    // media configurations.
    else if ( aDrive != defaultPhoneMem )
        {
        TInt defaultRam( KErrNotFound );
        GetDefaultDrive( EDefaultRam, defaultRam );
        if ( driveInfo.iType == EMediaRam || defaultRam == aDrive )
            {
            aStatus |= EDriveRam;
            }
        }

    // Check drive attributes
#ifdef __WINS__
    // On emulator, handle memory card defined as local drive in epoc ini
    // that has invalid drive attributes
    TInt defaultRemovable( KErrNotFound );
    GetDefaultDrive( EDefaultRemovableMassStorage, defaultRemovable );
#endif // __WINS__

    if ( ( driveInfo.iDriveAtt & KDriveAttInternal )
#ifdef __WINS__
          && ( defaultRemovable != aDrive ) 
#endif // __WINS__
          )
        {
        aStatus |= EDriveInternal;

        // Note that internal mass storage may have 'internal' file
        // server attribute
        if ( ExternallyMountableInternalDrive( aDrive, driveInfo.iDriveAtt ) )
            {
            aStatus |= EDriveExternallyMountable;
            }
        }
    else if ( ( driveInfo.iDriveAtt & KDriveAttRemovable )
#ifdef __WINS__
             || ( defaultRemovable == aDrive ) 
#endif // __WINS__
             )
        {
        // Note that internal mass storage may have 'removable' file
        // server attribute
        if ( ExternallyMountableInternalDrive( aDrive, driveInfo.iDriveAtt ) )
            {
            aStatus |= EDriveInternal | EDriveExternallyMountable;
            }
        else
            {
            aStatus |= EDriveRemovable | EDriveExternallyMountable |
                EDriveSwEjectable;
            if ( IsUsbDrive( aDrive ) )
                {
                aStatus |= EDriveUsbMemory;
                }
            }
        }
    else if ( driveInfo.iDriveAtt & KDriveAttRemote )
        {
        aStatus |= EDriveRemote | EDriveExternallyMountable;
        }

    if ( driveInfo.iDriveAtt & KDriveAttSubsted )
        {
        aStatus |= EDriveSubsted;
        }

    // Check media attributes
    if ( driveInfo.iMediaAtt & KMediaAttLocked )
        {
        aStatus |= EDriveLocked;
        }

    if ( driveInfo.iMediaAtt & KMediaAttWriteProtected )
        {
        aStatus |= EDriveReadOnly;
        }

    if ( !( aStatus & EDriveReadOnly ) &&
        ( driveInfo.iMediaAtt & KMediaAttFormattable ) )
        {
        aStatus |= EDriveFormattable;
        }

    if ( !( aStatus & EDriveInternal ) &&
        ( driveInfo.iMediaAtt & KMediaAttLockable ) )
        {
        aStatus |= EDriveLockable;
        }

    if ( driveInfo.iMediaAtt & EDriveHasPassword )
        {
        aStatus |= EDriveHasPassword;
        }

    INFO_LOG3( "DriveInfo::GetDriveStatus-aDrive=%d,aStatus=0x%x,ret=%d",
        aDrive, aStatus, ret )

    LOG_IF_ERROR2( ret, "DriveInfo::GetDriveStatus-aDrive=%d,ret=%d",
        aDrive, ret )

    return ret;
    }

// -----------------------------------------------------------------------------
// DriveInfo::GetUserVisibleDrives
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::GetUserVisibleDrives(
        RFs& aFs, TDriveList& aDriveList, TInt& aDriveCount )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    return GetUserVisibleDrives(
        aFs, aDriveList, aDriveCount, KDriveAttExclude | KDriveAttRemote );
    }

// -----------------------------------------------------------------------------
// DriveInfo::GetUserVisibleDrives
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::GetUserVisibleDrives(
        RFs& aFs, TDriveList& aDriveList, TInt& aDriveCount, TUint aFlags )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    aDriveCount = 0;

    TInt ret( aFs.DriveList( aDriveList, aFlags ) );
    if ( ret == KErrNone )
        {
        aDriveCount = StripUserHiddenDrives( aDriveList );
        }

    INFO_LOG2( "DriveInfo::GetUserVisibleDrives-aDriveCount=%d,aFlags=%d",
        aDriveCount, aFlags )

    LOG_IF_ERROR1( ret, "DriveInfo::GetUserVisibleDrives-ret=%d", ret )

    return ret;
    }

// -----------------------------------------------------------------------------
// DriveInfo::StripUserHiddenDrives
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::StripUserHiddenDrives( TDriveList& aDriveList )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    TInt ret( 0 );
    TInt count( aDriveList.Length() );
    for( TInt i( 0 ); i < count; ++i )
        {
        if ( aDriveList[ i ] )
            {
            if ( !IsUserHiddenDrive( i ) )
                {
                ++ret;
                }
            else
                {
                aDriveList[ i ] = 0; // Remove user hidden drive
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// DriveInfo::DriveCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::DriveCount( const TDriveList& aDriveList )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    TInt ret( 0 );
    TInt count( aDriveList.Length() );
    for( TInt i( 0 ); i < count; ++i )
        {
        if ( aDriveList[ i ] )
            {
            ++ret;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// DriveInfo::TDriveArray::Set
// -----------------------------------------------------------------------------
//
EXPORT_C void DriveInfo::TDriveArray::Set( const TDriveList& aDriveList )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iArray.Zero();
    TInt count( Min( aDriveList.Length(), KMaxDrives ) );
    for( TInt i( 0 ); i < count; ++i )
        {
        if ( aDriveList[ i ] )
            {
            iArray.Append( static_cast< TUint8 >( i ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// DriveInfo::TDriveArray::LetterAt
// -----------------------------------------------------------------------------
//
EXPORT_C TChar DriveInfo::TDriveArray::LetterAt( TInt aIndex ) const
    {
    FUNC_LOG_WITH_CLIENT_NAME

    TChar ret( 0 );
    // Ignore return value, Set() guarantees that valid drives are used
    RFs::DriveToChar( iArray[ aIndex ], ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// DriveInfo::GetUserVisibleDrives
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::GetUserVisibleDrives(
        RFs& aFs, TDriveArray& aDriveArray )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    return GetUserVisibleDrives( aFs, aDriveArray, KDriveAttAll );
    }

// -----------------------------------------------------------------------------
// DriveInfo::GetUserVisibleDrives
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::GetUserVisibleDrives(
        RFs& aFs, TDriveArray& aDriveArray, TUint aFlags )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    TDriveList drvList;
    TInt ret( aFs.DriveList( drvList, aFlags ) );
    if ( ret == KErrNone )
        {
        StripUserHiddenDrives( drvList );
        aDriveArray.Set( drvList );
        }
    else
        {
        aDriveArray.Reset();
        }

    INFO_LOG2( "DriveInfo::GetUserVisibleDrives-aDriveArrayCount=%d,aFlags=%d",
        aDriveArray.Count(), aFlags )

    LOG_IF_ERROR1( ret, "DriveInfo::GetUserVisibleDrives-ret=%d", ret )

    return ret;
    }

// -----------------------------------------------------------------------------
// DriveInfo::GetDefaultDrive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt DriveInfo::GetDefaultDrive(
        TInt aDefaultDrive, TChar& aDriveLetter )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    TInt drive( 0 );
    TInt ret( GetDefaultDrive( aDefaultDrive, drive ) );
    if ( ret == KErrNone )
        {
        ret = RFs::DriveToChar( drive, aDriveLetter );
        }

    LOG_IF_ERROR1( ret, "DriveInfo::GetDefaultDrive-ret=%d", ret )

    return ret;
    }

//  End of File  
