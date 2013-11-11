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

#ifndef DIRECTGDIIMAGETARGET_H
#define DIRECTGDIIMAGETARGET_H

/**
@file
@publishedPartner
@prototype
@deprecated
*/

#include <e32def.h>

// Forward declarations.
//
class CDirectGdiDriver;
class RSgImage;

/**
RDirectGdiImageTarget is a handle to a DirectGDI adaptation-specific resource which 
abstracts the image-based rendering target. The main intention in creating this resource 
is to allow a global sharable image resource (RSgImage) to be used as a target of 
DirectGDI rendering. For example, in a DirectGDI adaptation where OpenVG is used to 
implement DirectGDI functionality, the creation of RDirectGdiImageTarget may be associated 
with the creation of an underlying EGL surface from the given RSgImage. This handle class 
and its DirectGDI adaptation-specific resource are non-sharable and local within the 
creating thread.

This class is part of a generic layer and will forward any request to the CDirectGdiDriver
singleton for the calling thread.

CDirectGdiContext is activated on an RDirectGdiImageTarget. This causes all subsequent
context drawing operations to to draw to the image resource referred to by the 
RDirectGdiImageTarget.

@see CDirectGdiContext
*/
NONSHARABLE_CLASS(RDirectGdiImageTarget)
	{
	friend class CDirectGdiDriver;
	friend class CTPanicTests;
	
public:
	IMPORT_C RDirectGdiImageTarget();
	IMPORT_C RDirectGdiImageTarget(CDirectGdiDriver& aDriver);
	IMPORT_C TInt Create(const RSgImage& aImage);
	IMPORT_C void Close();
	inline TInt Handle() const;

private:
	CDirectGdiDriver* iDriver;
	TInt iHandle;
	};
	
#include <graphics/directgdiimagetarget.inl>

#endif //DIRECTGDIIMAGETARGET_H
