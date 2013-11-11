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
//

/**
 @file
 @publishedPartner
*/

#include <bluetoothav.h>
#include "gavdpinterface.h"
	
EXPORT_C TAvdtpServiceCategories::TAvdtpServiceCategories()
	{
	iCategories = 0;
	}
	
EXPORT_C TAvdtpServiceCatBitMask TAvdtpServiceCategories::operator()() const
	{
	return iCategories;
	}

EXPORT_C void TAvdtpServiceCategories::SetCapability(TAvdtpServiceCategory aCategory)
	{
	if (aCategory == EAllServiceCategories)
		{
		iCategories = KMaxTUint;
		}
	else if (aCategory != EServiceCategoryNull)
		{
		iCategories = iCategories | (1<<(aCategory-1));
		}
	}
	
EXPORT_C void TAvdtpServiceCategories::ClearCapability(TAvdtpServiceCategory aCategory)
	{
	if (aCategory != EServiceCategoryNull)
		{
		iCategories = (aCategory == EAllServiceCategories) ? 0 : iCategories &~(1<<(aCategory-1));
		}												 
	}
	
EXPORT_C TBool TAvdtpServiceCategories::CapabilityPresent(TAvdtpServiceCategory aCategory) const
	{
	TBool capabilityPresent = EFalse;

	if (aCategory != EServiceCategoryNull)
		{
		capabilityPresent = (iCategories & (1<<(aCategory-1))) ? ETrue : EFalse;
		}

	return capabilityPresent;

	}
