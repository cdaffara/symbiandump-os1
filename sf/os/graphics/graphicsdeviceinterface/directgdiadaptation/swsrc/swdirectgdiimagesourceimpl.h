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

#ifndef SWDIRECTGDIIMAGESOURCEIMPL_H_
#define SWDIRECTGDIIMAGESOURCEIMPL_H_

/**
@file
@internalComponent
*/

#include "directgdiimageref.h"
#include <e32def.h>

class CSwDirectGdiDriverImpl;

/**
A concrete software implementation of an image source represented by an RSgImage.
*/
NONSHARABLE_CLASS(CSwDirectGdiImageSourceImpl) : public CDirectGdiImageRef
{
public:
	static TInt New(CSwDirectGdiImageSourceImpl*& aImage, CSwDirectGdiDriverImpl& aDriver, const RSgImage& aSgImage);
	virtual ~CSwDirectGdiImageSourceImpl();
	inline TInt Stride() const { return iStride; }	
	inline TAny* DataBuffer() const { return iDataBuffer; }

private:
	CSwDirectGdiImageSourceImpl(CSwDirectGdiDriverImpl& aDriver);
	TInt Construct(const RSgImage& aSgImage);
	
private:
	CSwDirectGdiDriverImpl& iDriver;
	TAny* iDataBuffer;
	TInt iStride;
};

#endif /*SWDIRECTGDIIMAGESOURCEIMPL_H_*/
