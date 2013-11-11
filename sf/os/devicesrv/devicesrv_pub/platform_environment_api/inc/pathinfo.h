/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for quering system paths.
*
*/



#ifndef PATH_INFO_H
#define PATH_INFO_H

//  INCLUDES
#include <e32std.h>
#include <badesca.h>

// CLASS DECLARATION
/**
* Class holds information of system paths. Platform Environment API provides
* interface for quering system paths. Methods provided by the API should be
* used instead of hard coded path names. All paths have the trailing backslash
* included. The API consist of the PathInfo class, DriveInfo class and system paths are defined
* in PathConfiguration.hrh. The DriveInfo class is defined in DriveInfo.h.
*
* Usage:
*
* @code
*  #include <PathInfo.h>
*
*  // Get the root path of Phone Memory.
*  TFileName path = PathInfo::PhoneMemoryRootPath();
*
*  // Get the games path and append the path to the root path of Phone Memory.
*  path.Append( PathInfo::GamesPath() );
*
*  // 'path' contains now the games path in Phone Memory.
* @endcode
*
* Error handling:
*
* The panic mechanism is used to handle programming errors. 
* GetPath(TInt aPath) method will panic if invalid parameter is given as input. 
* The panic category is named PATHINFO and panic code is:
*
* - EInvalidParameter = 0 (Invalid parameter.)
*
*  @lib PlatformEnv.dll
*  @since 2.0
*/

class PathInfo
    {
    public:

        /**
		* Enumeration System Paths defines values to be used for the aPath parameter
		* in GetPath and GetFullPath methods.
        * @since 3.2
		*/
		enum TSystemPaths
			{
            /** This value is used only as a return value of PathType() to indicate that
            * the path is not a system path. It is not a valid system path to be given
            * as a parameter for GetPath() or GetFullPath()
            */
			ENotSystemPath = -1,
            /** To get the root path in ROM.
            */
			ERomRootPath = 0,
			/** To get the root path in Phone Memory..
            */
            EPhoneMemoryRootPath,
            /** To get the root path in Memory Card.
            */
			EMemoryCardRootPath, 
			/** To get the games path to be appended to a root path.
            */
            EGamesPath,
            /** To get the installs path to be appended to a root path.
            */
            EInstallsPath,
            /** To get the others path to be appended to a root path.
            */
            EOthersPath,
            /** To get the videos path to be appended to a root path.
            */
            EVideosPath,
            /** To get the images path to be appended to a root path.
            */
            EImagesPath,
            /**  To get the GSM pictures path to be appended to a root path.
            */
            EGsmPicturesPath,
            /** To get the MMS pictures path to be appended to a root path.
            */
            EMmsBackgroundImagesPath,
            /** To get the presence logos path to be appended to a root path.
            */
            EPresenceLogosPath,
            /** To get the sounds path to be appended to a root path.
            */
            ESoundsPath,            
            /** To get the digital sounds path to be appended to a root path.
            */  
            EDigitalSoundsPath,
            /** To get the simple sounds path to be appended to a root path.
            */
            ESimpleSoundsPath,
            /** To get the images thumbnail path.
            * The thumbnail images directory exists under the same directory
            * where the corresponding image is. 
            * Do not try to append this to a root directory.
            */
            EImagesThumbnailPath,
            /** To get the full path of the contacts folder in the memory card.
            * The path also contains the drive letter. Do not try to append 
            * this to any root directory.
            */ 
            EMemoryCardContactsPath

			};

        /**
        * This method returns the root path in ROM.
        * Corresponding TSystemPaths value of the returned path is ERomRootPath.
        *
        * @return The root path in ROM.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& RomRootPath();
        /**
        * This method returns the root path in Phone Memory.
        * Corresponding TSystemPaths value of the returned path is EPhoneMemoryRootPath.
        *
        * @return The root path in Phone Memory.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& PhoneMemoryRootPath();
        /**
        * This method returns the root path in Memory Card.
        * Corresponding TSystemPaths value of the returned path is EMemoryCardRootPath.
        *
        * @return The root path in Memory Card.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& MemoryCardRootPath();


        /**
        * This method returns the games path to be appended to a root path.
        * Corresponding TSystemPaths value of the returned path is EGamesPath.
        *
        * @return The games path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& GamesPath();
        /**
        * This method returns the installs path to be appended to a root path.
        * Corresponding TSystemPaths value of the returned path is EInstallsPath.
        *
        * @return The installs path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& InstallsPath();
        /**
        * This method returns the others path to be appended to a root path.
        * Corresponding TSystemPaths value of the returned path is EOthersPath.
        *
        * @return The installs path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& OthersPath();
        /**
        * This method returns the videos path to be appended to a root path.
        * Corresponding TSystemPaths value of the returned path is EVideosPath.
        *
        * @return The videos path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& VideosPath();
        /**
        * This method returns the images path to be appended to a root path.
        * Corresponding TSystemPaths value of the returned path is EImagesPath.
        *
        * @return The images path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& ImagesPath();  
        /**
        * This method returns the pictures path to be appended to a root path.
        * Corresponding TSystemPaths value of the returned path is EGsmPicturesPath.
        *
        * @return The pictures path.
        *
        * @deprecated Use GmsPicturesPath() instead.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& PicturesPath();       
        /**
        * This method returns the GMS pictures path to be appended to 
        * a root path.
        * Corresponding TSystemPaths value of the returned path is EGsmPicturesPath.
        *
        * @return The GSM pictures path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& GmsPicturesPath();
        /**
        * This method returns the MMS background images path to be appended to
        * a root path.
        * Corresponding TSystemPaths value of the returned path is EMmsBackgroundImagesPath.
        *
        * @return The MMS background images path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& MmsBackgroundImagesPath();
        /**
        * This method returns the presence logos path to be appended to 
        * a root path.
        * Corresponding TSystemPaths value of the returned path is EPresenceLogosPath.
        *
        * @return The presence logos path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& PresenceLogosPath();
        /**
        * This method returns the sounds path to be appended to a root path.
        * Corresponding TSystemPaths value of the returned path is ESoundsPath.
        *
        * @return The sounds path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& SoundsPath();
        /**
        * This method returns the digital sounds path to be appended to 
        * a root path.
        * Corresponding TSystemPaths value of the returned path is EDigitalSoundsPath.
        *
        * @return The digital sounds path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& DigitalSoundsPath();
        /**
        * This method returns the simple sounds path to be appended to 
        * a root path.
        * Corresponding TSystemPaths value of the returned path is ESimpleSoundsPath.
        *
        * @return The simple sound path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& SimpleSoundsPath();

        // ---------------------------------------------------------------------
        // Paths that are not necessarily under root directories
        // ---------------------------------------------------------------------

        /**
        * This method returns a thumbnail images path. The thumbnail images 
        * directory exists under the same directory where the corresponding 
        * image is. Do not try to append this to a root directory.
        * Corresponding TSystemPaths value of the returned path is EImagesThumbnailPath.
        *
        * @return The thumbnail images path.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& ImagesThumbnailPath();

        /**
        * This method returns the full path of the contacts folder in 
        * the memory card. The path also contains the drive letter. 
        * Do not try to append this to any root directory.
        * Corresponding TSystemPaths value of the returned path is EMemoryCardContactsPath.
        *
        * @return The full path of the contacts folder in the memory card.
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& MemoryCardContactsPath();

        /**
        * This method returns the requested system path.
        *
        * @since 3.2
        * @param aPath Defines the requested system path.
        * @return The requested system path.
        *
        * @panic EInvalidParameter Parameter aPath is invalid.
        *
        * One small sample describing the usage of the method.
        * @code
        *  #include <PathInfo.h>
        *
        *  // Get the the full path of the contacts folder in the memory card.
        *  TFileName path = PathInfo::GetPath( PathInfo::EMemoryCardContactsPath );
        *
        *  // 'path' contains now the full path of the contacts folder in the memory card..
        * @endcode
        *
        * @see TSystemPaths
        */
        IMPORT_C static const TDesC& GetPath( TInt aPath );

        /**
        * This method gets the root path of the requested drive.
        * The root path is the path where the system paths are located.
        *
        * @since 3.2
        * @param aRootPath Stores the path, the maximum path length is KMaxPath.
        * @param aDrive A drive identifier specified by TDriveNumber.
        * @return A system wide error code.
        * 
        * One small sample describing the usage of the method.
        * @code
        *  #include <PathInfo.h>
        *  #include <DriveInfo.h>
        *
        *  // Get the root path of the default phone memory.
        *  TInt drive;
        *  User::LeaveIfError( DriveInfo::GetDefaultDrive(
        *      DriveInfo::EDefaultPhoneMemory, drive ) );
        *  TFileName path;
        *  User::LeaveIfError( PathInfo::GetRootPath( path, drive ) );
        *
        *  // 'path' contains now the default folder root path of the default phone memory.
        * @endcode
        *
        * @see TDriveNumber
        * @see KMaxPath
        * @see DriveInfo
        */
        IMPORT_C static TInt GetRootPath( TDes& aRootPath, TInt aDrive );

        /**
        * This method gets the full path of the requested system path in the requested drive.
        * KErrNotFound is returned when the drive has no requested system path or 
        * the requested path cannot be added to the root path.
        *
        * @since 3.2
        * @param aFullPath Stores the requested path, the maximum path length is KMaxPath.
        * @param aDrive A drive identifier specified by TDriveNumber.
        * @param aPath Defines the requested system path.
        * @return A system wide error codes.
        *
        * One small sample describing the usage of the method.
        * @code
        *  #include <PathInfo.h>
        *  #include <DriveInfo.h>
        *
        *  // Get the full path of the images folder in the default
        *  // phone memory drive.
        *  TFileName path;
        *  TInt drive;
        *  User::LeaveIfError( DriveInfo::GetDefaultDrive(
        *      DriveInfo::EDefaultPhoneMemory, drive ) );
        *  User::LeaveIfError( PathInfo::GetFullPath( path, drive, PathInfo::EImages ) );
        *
        *  // 'path' contains now the full path of the images folder in the default
        *  // phone memory drive.
        * @endcode
        *
        * @see TDriveNumber
        * @see KMaxPath
        * @see TSystemPaths
        * @see DriveInfo
        */
        IMPORT_C static TInt GetFullPath( TDes& aFullPath, TInt aDrive, TInt aPath );

        /**
        * This method returns the system path type of the given path.
        * Thumbnail system path can exist in any folder.
        * Other paths must be exact system paths, not subfolders of a system path
        * ENotSystemPath is returned, if the path is not a system path.
        * The given path must have backslash ending.
        *
        * @since 3.2
        * @param aFullPath A path to be type checked
        * @return A value specified by TSystemPaths
        * 
        * One small sample describing the usage of the method.
        * @code
        *  #include <PathInfo.h>
        *
        *  // Check the type of the system path.
        * _LIT( KImagesPath, "E:\\Images\\" );
        *  TInt type( PathInfo::PathType( KImagesPath ) );
        *
        *  // 'type' contains now the EImagesPath value.
        * @endcode
        *
        * @see TSystemPaths
        */
        IMPORT_C static TInt PathType( const TDesC& aFullPath );

        /**
        * This method gets the list of full system paths in the requested drive 
        * and leaves the returned pointer in cleanup stack.
        *
        * @since 3.2
        * @param aDrive A drive identifier specified by TDriveNumber.
        * @return A list of the system paths. Ownership is transferred.
        *
        * One small sample describing the usage of the method.
        * @code
        *  #include <PathInfo.h>
        *  #include <DriveInfo.h>
        *
        *  // Create the default path structure for default mass storage drive
        *  TInt drive;
        *  User::LeaveIfError( DriveInfo::GetDefaultDrive(
        *      DriveInfo::EDefaultMassStorage, drive ) );
        *  CDesCArray* paths = PathInfo::GetListOfPathsLC( drive );
        *  TInt count( paths->MdcaCount() );
        *  for ( TInt i( 0 ); i < count; ++i )
        *      {
        *      User::LeaveIfError( iFs.MkDirAll( paths->MdcaPoint( i ) );
        *      }
        * CleanupStack::PopAndDestroy( paths );
        * // The default mass storage drive contains now the default path structure
        * @endcode
        *
        * @see TDriveNumber
        * @see DriveInfo
        */
        IMPORT_C static CDesCArray* GetListOfPathsLC( TInt aDrive );

        /**
        * This method gets the list of full system paths in the requested drive.
        *
        * @since 3.2
        * @param aDrive A drive identifier specified by TDriveNumber.
        * @return A list of the system paths. Ownership is transferred.
        *
        * @see TDriveNumber
        */
        IMPORT_C static CDesCArray* GetListOfPathsL( TInt aDrive );

    private:

        /**
        * C++ default constructor.
        */
        PathInfo();
    };

#endif      // PATH_INFO_H   
            
// End of File
