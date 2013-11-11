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
* Description:  Patchable data definitions
*
*/



#ifndef DRIVE_INFO_PATCH_DATA_H
#define DRIVE_INFO_PATCH_DATA_H

//  INCLUDES
#include <e32std.h>

/** 
* Patchable default drive identifiers.
* Contain values specified by TDriveNumber.
*/
IMPORT_C extern const TInt KDriveInfoDefaultRomDrive;
IMPORT_C extern const TInt KDriveInfoDefaultRamDrive;
IMPORT_C extern const TInt KDriveInfoDefaultSystemDrive;
IMPORT_C extern const TInt KDriveInfoDefaultPhoneMemory;
IMPORT_C extern const TInt KDriveInfoDefaultMassStorage;
IMPORT_C extern const TInt KDriveInfoDefaultRemovableMassStorage;

/**
* Patchable user hidden drive identifiers and place holders.
* Contain values specified by TDriveNumber or KErrNotFound.
*/
IMPORT_C extern const TInt KDriveInfoUserHiddenDrive1;
IMPORT_C extern const TInt KDriveInfoUserHiddenDrive2;
IMPORT_C extern const TInt KDriveInfoUserHiddenDrive3;
IMPORT_C extern const TInt KDriveInfoUserHiddenDrive4;
IMPORT_C extern const TInt KDriveInfoUserHiddenDrive5;
IMPORT_C extern const TInt KDriveInfoUserHiddenDrive6;
IMPORT_C extern const TInt KDriveInfoUserHiddenDrive7;
IMPORT_C extern const TInt KDriveInfoUserHiddenDrive8;

/** Patchable USB drive definitions. */
/** Contains value specified by TDriveNumber or KErrNotFound. */
IMPORT_C extern const TInt KDriveInfoUsbDrivesStart;
/** Contains maximum number of supported USB drives. */
IMPORT_C extern const TInt KDriveInfoUsbDrivesMaxCount;

/**
* DEPRICATED, DO NOT USE.
* Instead, use the estart text file to set drive attribute KDriveAttLogicallyRemovable.
* For example, following definition sets the attribute to E drive.
* [DriveE]
* AddDriveAttributes KDRIVEATTLOGICALLYREMOVABLE
*
* Patchable logically removable drive identifiers and place holders.
* Contain values specified by TDriveNumber or KErrNotFound.
*/
IMPORT_C extern const TInt KDriveInfoLogicallyRemovableDrive1;
IMPORT_C extern const TInt KDriveInfoLogicallyRemovableDrive2;
IMPORT_C extern const TInt KDriveInfoLogicallyRemovableDrive3;
IMPORT_C extern const TInt KDriveInfoLogicallyRemovableDrive4;

#endif // DRIVE_INFO_PATCH_DATA_H   

// End of File
