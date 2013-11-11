// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "GraphicsAccelerator.h"

EXPORT_C TInt RHardwareBitmap::GetInfo(TAcceleratedBitmapInfo& /*aInfo*/) const
/** Fills a structure with data which describes the basic details of this bitmap.

@param aInfo Provides the information needed to draw the bitmap.
@return KErrBadHandle if a value has not been assigned to iHandle. */
	{
	if (!iHandle)
		return KErrBadHandle;

	return KErrNotSupported;
	}

EXPORT_C TInt RHardwareBitmap::SetAsScreenReference(TInt /*aScreen*/)
	{
	return KErrNotSupported;
	}

EXPORT_C TInt RHardwareBitmap::Create(TDisplayMode /*aDisplayMode*/, TSize /*aSize*/, TUid /*aCreatorUid*/)
	{
	return KErrNotSupported;
	}

EXPORT_C void RHardwareBitmap::Destroy()
	{}

