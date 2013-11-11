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
* Description:  Interface for quering the drive information of the system.
*
*/



#ifndef DRIVE_INFO_H
#define DRIVE_INFO_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>

// CLASS DECLARATION
/**
* Class holds the drive information of the system. Platform Environment API provides 
* interface for quering the drive information of the system. Methods provided by the API should be
* used instead of the hard coded drive identifiers. 
* The API consist of the DriveInfo class, PathInfo class and system paths are defined
* in PathConfiguration.hrh. The PathInfo class is defined in PathInfo.h.
*
* Usage:
*
* @code
*  #include <DriveInfo.h>
*
*  // Get the drive identifier of the default removable mass storage.
*  TInt drive;
*  User::LeaveIfError( DriveInfo::GetDefaultDrive(
*      DriveInfo::EDefaultRemovableMassStorage, drive ) );
*
*  // 'drive' contains now the drive identifier of the default removable mass storage.
*
*  // Get the drive status of the default removable mass storage.
*  TUint status;
*  User::LeaveIfError( DriveInfo::GetDriveStatus( fs, drive, status ) );
*
*  // 'status' contains now the drive status of the default removable mass storage.
*
*  // Get all drives that are visible to the user in TDriveList.
*  TDriveList driveList;
*  TInt driveCount;
*  User::LeaveIfError( DriveInfo::GetUserVisibleDrives( fs, driveList, driveCount ) );
*
*  // 'driveList' contains now the user visible drives.
*  // 'driveCount'contains now the drive count i.e. number of non zero items in driveList.
*
*  // Access the drives stored in 'driveList'
*  TInt driveListLen( driveList.Length() ); // The length of 'driveList'
*  for( TInt i( 0 ); i < driveListLen; ++i )
*      {
*      if ( driveList[ i ] ) // Non zero items are valid drives
*          {
*          // 'i' contains drive identifier specified by TDriveNumber
*          // ...
*          }
*      }
*
*  // Get all drives that are visible to the user in DriveInfo::TDriveArray.
*  DriveInfo::TDriveArray driveArray;
*  User::LeaveIfError( DriveInfo::GetUserVisibleDrives( fs, driveArray ) );
*
*  // 'driveArray' contains now the user visible drives.
*
*  // Access the drives stored in 'driveArray'
*  driveCount = driveArray.Count() ); // The number of drives stored in 'driveArray'
*  for( TInt i( 0 ); i < driveCount; ++i )
*      {
*      TDriveNumber drive( driveArray[ i ] ); // The drive identifier at position 'i'
*      TChar driveLetter( driveArray.LetterAt( i ) ); // The drive letter at position 'i'
*      // ...
*      }
*
* @endcode
*
* Error handling:
*
* System wide error codes are returned to indicate errors in methods that can fail.
*
*  @lib PlatformEnv.dll
*  @since 3.2
*/

NONSHARABLE_CLASS(DriveInfo)
    {
    public:
        /**
         * Enumeration Default Drives to be used with GetDefaultDrive() method.
         * @since 3.2
         */
        enum TDefaultDrives
            {
            /** To get the default ROM drive.
            */
            EDefaultRom = 0,

            /** To get the default RAM drive.
            */
            EDefaultRam,

            /** To get the default system drive.
            * The default system drive is the preferred drive for system specific functionalities.
            */
            EDefaultSystem,

            /** To get the default phone memory like FLASH memory, internal memory card, hard drive etc.
            */
            EDefaultPhoneMemory,

            /** To get the default mass storage like any kind of memory card, hard drive, USB stick etc.
            * This definition should be always used, unless a removable mass storage is explicitly required.
            * The default mass storage is the preferred drive for a large amount of user files.
            */
            EDefaultMassStorage,

            /** To get the default removable mass storage like memory card, USB stick etc.
            * This definition should be used only when explicitly removable mass storage is required.
            */
            EDefaultRemovableMassStorage
            };

       /**
        * This method gets the default drive of requested type.
        * If the device does not have the requested default drive, then KErrNotSupported is returned.
        * This happens for example if device supports only internal drives and
        * EDefaultRemovableMassStorage is requested.
        *
        * @since 3.2
        * @param aDefaultDrive A default drive identifier specified by TDefaultDrives
        * @param aDrive Stores a drive identifier specified by TDriveNumber
        * @return A system wide error code.
        *
        * @see TDefaultDrives
        * @see TDriveNumber
        */
        IMPORT_C static TInt GetDefaultDrive( TInt aDefaultDrive, TInt& aDrive );

       /**
        * This method gets the default drive of requested type.
        * If the device does not have the requested default drive, then KErrNotSupported is returned.
        * This happens for example if device supports only internal drives and
        * EDefaultRemovableMassStorage is requested.
        *
        * @since 3.2
        * @param aDefaultDrive A default drive identifier specified by TDefaultDrives
        * @param aDriveLetter Stores a drive letter
        * @return A system wide error code.
        *
        * @see TDefaultDrives
        */
        IMPORT_C static TInt GetDefaultDrive( TInt aDefaultDrive, TChar& aDriveLetter );

        /**
         * Enumeration bit mask Status used by GetDriveStatus() method.
         * @since 3.2
         */
        enum TStatus
            {
            /** To indicate that the drive is internal and 
            * cannot be physically removed.
            */
            EDriveInternal = 0x1,

            /** To indicate that the drive is physically removable.
            */
            EDriveRemovable = 0x2,

            /** To indicate that the drive is remote.
            */
            EDriveRemote = 0x4,

            /** To indicate that the drive is present.
            */
            EDrivePresent = 0x8,

            /** To indicate that the drive is locked.
            */
            EDriveLocked = 0x10,

            /** To indicate that the drive is corrupt.
            */
            EDriveCorrupt = 0x20,

            /** To indicate that the drive is in use i.e.
            * reserved for some exclusive usage.
            */
            EDriveInUse = 0x40,

            /** To indicate that the drive is readonly.
            */
            EDriveReadOnly = 0x80,

            /** To indicate that the drive is substed.
            */
            EDriveSubsted = 0x100,

            /** To indicate that the drive is user visible.
            * The UI is allowed to show the drive to the user.
            */
            EDriveUserVisible = 0x200,

            /** To indicate that the drive is externally mountable i.e.
            * can be mounted from PC or from other devices.
            */
            EDriveExternallyMountable = 0x400,

            /** To indicate that the drive is software ejectable.
            * The user can select software based eject from the UI for this drive.
            */
            EDriveSwEjectable = 0x800,

            /** To indicate that the drive is ROM drive.
            */
            EDriveRom = 0x1000,

            /** To indicate that the drive is RAM drive.
            */
            EDriveRam = 0x2000,

            /** To indicate that the drive has been formatted.
            */
            EDriveFormatted = 0x4000,

            /** To indicate that the drive is formattable.
            */
            EDriveFormattable = 0x8000,

            /** To indicate that the drive is lockable i.e. password can be set.
            */
            EDriveLockable = 0x10000,

            /** To indicate that the drive is password protected.
            */
            EDriveHasPassword = 0x20000,

            /** To indicate that the drive is USB memory.
            */
            EDriveUsbMemory = 0x40000

            };

       /**
        * This method gets the drive status, a bit mask specified by TStatus.
        *
        * @since 3.2
        * @param aFs An opened file server session
        * @param aDrive A drive identifier specified by TDriveNumber
        * @param aStatus Stores the drive status bit mask specified by TStatus
        * @return A system wide error code.
        *
        * @see RFs
        * @see TDriveNumber
        * @see TStatus
        */
        IMPORT_C static TInt GetDriveStatus( RFs& aFs, TInt aDrive, TUint& aStatus );

       /**
        * This method gets all the drives that are visible to the user.
        *
        * @since 3.2
        * @param aFs An opened file server session
        * @param aDriveList Stores the user visible drives in the same format than RFs::DriveList() method.
        * @param aDriveCount Stores the drive count i.e. number of non zero items in aDriveList.
        * @return A system wide error code.
        *
        * @see RFs
        * @see TDriveList
        */
        IMPORT_C static TInt GetUserVisibleDrives(
            RFs& aFs, TDriveList& aDriveList, TInt& aDriveCount );

       /**
        * This method gets the user visible drives with specified file server drive 
        * attributes.
        *
        * Note that file server drive attributes are not equal with TStatus definitions.
        *
        * @since 3.2
        * @param aFs An opened file server session
        * @param aDriveList Stores the user visible drives in the same format than RFs::DriveList() method.
        * @param aDriveCount Stores the drive count i.e. number of non zero items in aDriveList.
        * @param aFlags The mask flags specified for RFs::DriveList() method.
        * @return A system wide error code.
        *
        * @see RFs
        * @see TDriveList
        */
        IMPORT_C static TInt GetUserVisibleDrives(
            RFs& aFs, TDriveList& aDriveList, TInt& aDriveCount, TUint aFlags );

       /**
        * This method methods checks the given drive list and removes the drives hidden from the user.
        * It is intended to be used with the drive lists that are not read by using GetUserVisibleDrives() method
        * e.g. the drive list has been got as a parameter elsewhere.
        *
        * @since 3.2
        * @param aDriveList A drive list where to remove the drives hidden from the user.
        * @return A drive count i.e. number of non zero items in the given drive list after removal.
        *
        * @see TDriveList
        */
        IMPORT_C static TInt StripUserHiddenDrives( TDriveList& aDriveList );

       /**
        * This method returns the number of drives in the given drive list.
        *
        * @since 3.2
        * @param aDriveList A drive list where to count the number of drives.
        * @return A drive count i.e. number of non zero items in the given drive list.
        *
        * @see TDriveList
        */
        IMPORT_C static TInt DriveCount( const TDriveList& aDriveList );

        /**
         * Class TDriveArray provides easy-to-use access to drive identifiers.
         * @since S60 5.0
         */
        class TDriveArray
            {
            public: // Constructors
                inline TDriveArray();
                inline TDriveArray( const TDriveList& aDriveList );

            public:
                /**
                 * This method sets the drive array data from given drive list.
                 *
                 * @since 3.2
                 * @param aDriveList A drive list where to set the data from.
                 *
                 * @see TDriveList
                 */
                IMPORT_C void Set( const TDriveList& aDriveList );

                /**
                 * This method resets the array.
                 *
                 * @since 3.2
                 */
                inline void Reset();

                /**
                 * This method gets the number of drives in the array.
                 *
                 * @since 3.2
                 * @return A number of drives in the array.
                 */
                inline TInt Count() const;

                /**
                 * This method gets the drive identifier at given index within the array.
                 *
                 * @since 3.2
                 * @param aIndex The position of drive within the array.
                 * @return A drive identifier specified by TDriveNumber.
                 *
                 * @panic USER 21 if aIndex is negative or is greater than 
                 * the number of drives in the array
                 *
                 * @see TDriveNumber
                 */
                inline TDriveNumber operator[]( TInt aIndex ) const;

                /**
                 * This method gets the drive letter at given index within the array.
                 *
                 * @since 3.2
                 * @param aIndex The position of drive within the array.
                 * @return A drive letter.
                 *
                 * @panic USER 21 if aIndex is negative or is greater than 
                 * the number of drives in the array
                 */
                IMPORT_C TChar LetterAt( TInt aIndex ) const;

            private:
                TBuf8< KMaxDrives > iArray;
            };

       /**
        * This method gets all the drives that are visible to the user.
        *
        * @since 3.2
        * @param aFs An opened file server session
        * @param aDriveArray Stores the user visible drives
        * @return A system wide error code.
        *
        * @see RFs
        * @see TDriveArray
        */
        IMPORT_C static TInt GetUserVisibleDrives( RFs& aFs, TDriveArray& aDriveArray );

       /**
        * This method gets the user visible drives with specified file server drive 
        * attributes.
        *
        * Note that file server drive attributes are not equal with TStatus definitions.
        *
        * @since 3.2
        * @param aFs An opened file server session
        * @param aDriveArray Stores the user visible drives
        * @param aFlags The mask flags specified for RFs::DriveList() method.
        * @return A system wide error code.
        *
        * @see RFs
        * @see TDriveArray
        */
        IMPORT_C static TInt GetUserVisibleDrives(
            RFs& aFs, TDriveArray& aDriveArray, TUint aFlags );

    private:

        /**
        * C++ default constructor.
        */
        DriveInfo();
    };

#include "driveinfo.inl"

#endif      // DRIVE_INFO_H   

// End of File
