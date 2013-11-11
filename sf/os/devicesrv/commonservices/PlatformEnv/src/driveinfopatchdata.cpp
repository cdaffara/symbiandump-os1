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

#include <f32file.h>

EXPORT_C extern const TInt KDriveInfoDefaultRomDrive = EDriveZ;
EXPORT_C extern const TInt KDriveInfoDefaultRamDrive = EDriveD;
EXPORT_C extern const TInt KDriveInfoDefaultSystemDrive = KErrNotFound; // To use RFs::GetSystemDrive()
EXPORT_C extern const TInt KDriveInfoDefaultPhoneMemory = EDriveC;
EXPORT_C extern const TInt KDriveInfoDefaultMassStorage = EDriveE;
#if defined( __WINS__ ) && defined( PLATFORM_ENV_MASS_STORAGE_CONFIG )
EXPORT_C extern const TInt KDriveInfoDefaultRemovableMassStorage = EDriveG; // F is not visible in emulator
#else
EXPORT_C extern const TInt KDriveInfoDefaultRemovableMassStorage = EDriveE;
#endif

EXPORT_C extern const TInt KDriveInfoUserHiddenDrive1 = EDriveD;
EXPORT_C extern const TInt KDriveInfoUserHiddenDrive2 = EDriveZ;
EXPORT_C extern const TInt KDriveInfoUserHiddenDrive3 = KErrNotFound;
EXPORT_C extern const TInt KDriveInfoUserHiddenDrive4 = KErrNotFound;
EXPORT_C extern const TInt KDriveInfoUserHiddenDrive5 = KErrNotFound;
EXPORT_C extern const TInt KDriveInfoUserHiddenDrive6 = KErrNotFound;
EXPORT_C extern const TInt KDriveInfoUserHiddenDrive7 = KErrNotFound;
EXPORT_C extern const TInt KDriveInfoUserHiddenDrive8 = KErrNotFound;

EXPORT_C extern const TInt KDriveInfoUsbDrivesStart = EDriveG;
EXPORT_C extern const TInt KDriveInfoUsbDrivesMaxCount = 2;

EXPORT_C extern const TInt KDriveInfoLogicallyRemovableDrive1 = KErrNotFound;
EXPORT_C extern const TInt KDriveInfoLogicallyRemovableDrive2 = KErrNotFound;
EXPORT_C extern const TInt KDriveInfoLogicallyRemovableDrive3 = KErrNotFound;
EXPORT_C extern const TInt KDriveInfoLogicallyRemovableDrive4 = KErrNotFound;

//  End of File  
