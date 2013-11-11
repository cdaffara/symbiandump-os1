// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @publishedPartner
*/

#ifndef HCIPANIC_H
#define HCIPANIC_H

#include <e32def.h>

/**
The panic category for all code in the corehci_base dll.
*/
_LIT(KCoreHciBasePanicCat, "CoreHCIBase");

/**
Panic codes for Core HCI Base.
*/
enum TCoreHciBasePanic
	{
	ECommandBufferOverflow = 1,
	EBadArgumentValueOutOfBounds = 2,
	ECommandNotFinalised = 3,
	EInvalidDataFrameLength = 4,
	EInvalidCommandFrameLength = 5,
	EInvalidAclDataFrameLength = 6,
	EInvalidScoDataFrameLength = 7,
	EMoreCommandDataThanRequired = 8,
	};

#endif // HCIPANIC_H
