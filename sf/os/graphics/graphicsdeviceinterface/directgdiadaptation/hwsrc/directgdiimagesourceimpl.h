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

/**
 @file
 @internalComponent Reference implementation of OpenVG hardware accelerated DirectGDI adaptation.
*/

#ifndef DIRECTGDIIMAGESOURCEIMPL_H
#define DIRECTGDIIMAGESOURCEIMPL_H

#include "directgdiimageref.h"
#include <e32base.h>
#include <EGL/egl.h>
#include <VG/openvg.h>
#include <graphics/sgimage.h>

class CDirectGdiDriverImpl;
class XDirectGdiDriverProcessState;

/**
Container for holding an RDirectGdiDrawableSource and its associated EGLSurface.
May be reference counted if more than one CDirectGdiContext is allowed to 
use the same RDirectGdiDrawableSource.
*/
NONSHARABLE_CLASS(CDirectGdiImageSourceImpl): public CDirectGdiImageRef
	{
public:
	static TInt New(CDirectGdiImageSourceImpl*& aImage, CDirectGdiDriverImpl& aDriver, const RSgImage& aSgImage);
	virtual ~CDirectGdiImageSourceImpl();
	inline VGImage VgImage() const;

private:
	CDirectGdiImageSourceImpl(CDirectGdiDriverImpl& aDriver);
	virtual TInt Construct(const RSgImage& aSgImage);
	
private:		
	CDirectGdiDriverImpl& iDriver;
	VGImage iVgImage;
	};
	
inline VGImage CDirectGdiImageSourceImpl::VgImage() const
	{
	return iVgImage;
	}

#endif /*DIRECTGDIIMAGESOURCEIMPL_H_*/
