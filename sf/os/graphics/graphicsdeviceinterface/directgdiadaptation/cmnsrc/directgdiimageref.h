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

#ifndef _DIRECTGDIIMAGEREF_H_
#define _DIRECTGDIIMAGEREF_H_

/** 
@file
@internalComponent
 */

#include "directgdidrawableref.h"
#include <graphics/sgimage.h>


/**
Class to allow an RSgImage to be created and used by CDirectGdiImageTargetImpl
and CDirectGdiImageSourceImpl.

@internalComponent
 */
NONSHARABLE_CLASS(CDirectGdiImageRef) : public CDirectGdiDrawableRef
{
public:
	virtual ~CDirectGdiImageRef();
	inline const TSize& Size() const;
	inline const TUidPixelFormat& PixelFormat() const;
	inline const RSgImage& Image() const;
	
protected:
	CDirectGdiImageRef();
	virtual TInt Construct(const RSgImage& aSgImage);

protected:
	RSgImage iSgImage;
	
private:
	TBool iSgImageOpen;
	TSize iImageSize;	
	TUidPixelFormat iImagePixelFormat;
};

/** 
@return The size of the RSgImage.
 */
inline const TSize& CDirectGdiImageRef::Size() const
	{ 
	return iImageSize; 
	}

/** 
@return The pixel format of the RSgImage.
 */
inline const TUidPixelFormat& CDirectGdiImageRef::PixelFormat() const
	{ 
	return iImagePixelFormat; 
	}

/**
@return A const reference to the RSgImage contained in this class.
 */
inline const RSgImage& CDirectGdiImageRef::Image() const
	{ 
	return iSgImage; 
	}

#endif // _DIRECTGDIIMAGEREF_H_
