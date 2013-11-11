// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// wins\inc\variantmediadef.h
// Media definitions for the Wins Variant.
// Each Media Driver requires the following definitions
// DRIVECOUNT - The total number of local drive object to be assigned to the Media Driver (1-KMaxLocalDrives)
// DRIVELIST - A list of the local drive numbers (each separated with a comma) to be assigned to the Media Driver.
// Each in the range 0 - (KMaxLocalDrives-1). Total number of drive numbers must equal the value
// of DRIVECOUNT.
// NUMMEDIA - The total number of media objects to be assigned to the Media Driver.
// DRIVENAME - A name for the drive group.
// For the complete set of media definitions
// - The total number of local drive objects assigned should not exceed KMaxLocalDrives.
// - Each Media Driver should be assigned a unique set of drive numbers - no conflicts between Media Drivers.
// - The total number of media objects assigned should not exceed KMaxLocalDrives.
// 
// WARNING: This file contains some APIs which are internal and are subject
//          to change without notice. Such APIs should therefore not be used
//          outside the Kernel and Hardware Services package.
//
 
#ifndef __VARIANTMEDIADEF_H__
#define __VARIANTMEDIADEF_H__

// Variant parameters for IRAM Media Driver (MEDINT.PDD)
#define IRAM_DRIVECOUNT 1
#define IRAM_DRIVELIST 0
#define IRAM_NUMMEDIA 1	
#define IRAM_DRIVENAME "IRam"

// Variant parameters for LFFS Media Driver (MEDLFS.PDD)
#define LFFS_DRIVECOUNT 1
#define LFFS_DRIVELIST 8
#define LFFS_NUMMEDIA 1	
#define LFFS_DRIVENAME "Flash"

// Variant parameters for the MMC Controller (EPBUSMMC.DLL)
#define MMC0_DRIVECOUNT 4
#define MMC0_DRIVELIST 1,2,3,4
#define MMC0_NUMMEDIA 4	
#define MMC0_DRIVENAME "MultiMediaCard0"

// Variant parameters for NAND flash media driver 
#define NAND_DRIVECOUNT 3
#define NAND_DRIVELIST 5,6,9
#define NAND_NUMMEDIA 1
#define NAND_DRIVENAME "Nand"

// Variant parameters for test NFE media extension driver 
#define NFE_DRIVECOUNT 2
#define NFE_DRIVELIST 1,5
#define NFE_DRIVELETTERLIST 23,20			// EDRive? IDs of the each instance. (EDriveX,EDriveU from estart.txt)

#define NFE_INSTANCE_COUNT 2				// the number of NFE media driver instances
#define NFE_INSTANCE_DRIVE_COUNTS 1,1		// the number of drives in NFE_DRIVELIST for each instance of the driver

// Variant parameters for production NFE media extension driver (same as NFE_DRIVELETTERLIST)
#define NFE_INSTANCE_UI_DRIVE_ID 23,20			// EDRive? IDs of the each instance. (EDriveX,EDriveU from estart.txt)



#endif
