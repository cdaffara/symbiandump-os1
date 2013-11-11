// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\NAND_PLAT.H
// 
// WARNING: This file contains some APIs which are internal and are subject
//          to change without notice. Such APIs should therefore not be used
//          outside the Kernel and Hardware Services package.
//

#ifndef __NAND_PLAT_H__
#define __NAND_PLAT_H__

#include <nanddevice.h>
#include <nand_fbr_offset.h>

/**
This table contains a list of all of the NAND flash devices currently
supported by the NAND flash solution.
@internalTechnology
*/
const TNandDeviceInfo gDeviceTable[] = {
		{ ESamsungId, 0x73, 1024, 32, 512, 16, 9, 14,  20, 5, 0, 6, TDeviceFlags(0) },				// 16MB
		// Last entry
		{ TManufacturerId(0),    0,    0,  0,   0,  0, 0,  0, TDeviceFlags(0) }
		};

/**
Defining __USE_CUSTOM_ALLOCATOR enables the use of the NAND Flash Allocator.
@internalTechnology
*/
#define __USE_CUSTOM_ALLOCATOR

/**
The following constants support relocation of the NAND FBR for this platform
KNandFbrSectorOffset is the third of three constants required - it is defined in nand_fbr_offset.h
@publishedPartner
@deprecated Emulator Deprecated
*/
const TInt KNandMinibootUsed		=	ETrue;
const TInt KNandCoreldrRelocatable	=	ETrue;

#endif
