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

#include "directgdidriver.h"
#include "directgdipaniccodes.h"
#include <graphics/directgdidrawablesource.h>
#include <graphics/sgimage.h>

/**
Default constructor. 
Only for embedding instances of RDirectGdiDrawableSource into other classes as data members. 
Before a RDirectGdiDrawableSource can be used the other constructor must be called. 
*/
EXPORT_C RDirectGdiDrawableSource::RDirectGdiDrawableSource()
	: iDriver(NULL), iHandle(KNullHandle)
	{
	}

/**
Constructor.

@param 	aDriver The CDirectGdiDriver for this thread.

@pre 	The CDirectGdiDriver object has been initialised from the calling thread.
@post 	This handle has a pointer to the CDirectGdiDriver for this thread. 
		This handle is not associated with any DirectGDI adaptation-specific resource.
*/
EXPORT_C RDirectGdiDrawableSource::RDirectGdiDrawableSource(CDirectGdiDriver& aDriver)
	: iDriver(&aDriver), iHandle(KNullHandle)
	{
	}

/**
Creates a DirectGDI adaptation-specific resource that is bound to the specified
drawable resource so it can be drawn using the DirectGDI rendering API.

@param 	aDrawable The drawable resource.

@pre 	CDirectGdiDriver object for this handle has been set. The drawable
		resource has been fully constructed.
@post 	The DirectGDI adaptation-specific resource that is bound to the given
		non-pixel resource is created and this handle is now associated with it.
		The reference counter on the drawable resource is incremented.

@return KErrNone if successful, KErrArgument if the drawable resource is not
		valid, KErrAlreadyExists if this handle is already associated with a
		DirectGDI adaptation-specific resource, otherwise one of the system-wide
		error codes.
@panic  DGDI 18, if this drawable source has no pointer to its assosicated CDirectGdiDriver.
        This may occur if constructed using the default constructor, and has not yet been 
        assigned to another drawable source created with the constructor overload.
*/
EXPORT_C TInt RDirectGdiDrawableSource::Create(const RSgDrawable& aDrawable)
	{
	GRAPHICS_ASSERT_ALWAYS(iDriver, EDirectGdiPanicDrawableSourceWithoutDriver);
	return iDriver->CreateDrawableSource(*this, aDrawable);
	}

/**
Destroys the DirectGDI adaptation-specific resource associated with this handle.
Calling this method on a handle that is not associated with any DirectGDI 
adaptation-specific resource will do nothing. Once Close() is called, this handle can be reused.

@pre 	The CDirectGdiDriver object for this handle has been set.
@post 	The DirectGDI specific resource associated with this handle will be destroyed,
		at any time. This handle is no longer associated with a DirectGDI specific
		resource. The reference counter of the underlying drawable resource is
		decremented.
@panic  DGDI 18, if this drawable source has no pointer to its assosicated CDirectGdiDriver.
        This may occur if constructed using the default constructor, and has not yet been 
        assigned to another drawable source created with the constructor overload.
*/
EXPORT_C void RDirectGdiDrawableSource::Close()
	{
	if (iHandle != KNullHandle)
		{
		GRAPHICS_ASSERT_ALWAYS(iDriver, EDirectGdiPanicDrawableSourceWithoutDriver);
		iDriver->CloseDrawableSource(*this);
		}
	}
