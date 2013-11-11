// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "directgdiimageref.h"
#include <graphics/sgresourceinternal.h>

CDirectGdiImageRef::CDirectGdiImageRef()
	{
	}

/**
Second phase construction. If the object has not been opened before,
the passed RSgImage handle is shared with the member RSgImage and the size of the 
shared RSgImage is stored.

@return KErrNone if successful, otherwise an error from RSgDrawable::Open() or 
RSgImage::GetInfo()
 */
TInt CDirectGdiImageRef::Construct(const RSgImage& aSgImage)
	{		
	TInt err = KErrNone;	
	
	// Share the passed RSgImage handle with our own RSgImage
	err = iSgImage.Open(aSgImage.Id(), ESgDoNotRestrictUsage);
	
	if (err == KErrNone)
		{
		// Save our image size, only need to do this the first time we are created
		TSgImageInfo info;
		err = iSgImage.GetInfo(info);
		if (err == KErrNone)
			{
			iImageSize = info.iSizeInPixels;
			iImagePixelFormat = info.iPixelFormat;
			iSgImageOpen = ETrue;
			}
		else
			{
			iSgImage.Close();
			}
		}
	
	return err;
	}

/**
Destructor, close the RSgImage being held open if it had been opened it successfully.
 */
CDirectGdiImageRef::~CDirectGdiImageRef()
	{
	if (iSgImageOpen)
		{
		iSgImage.Close();
		}
	}
