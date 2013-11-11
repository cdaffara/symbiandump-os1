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
#include <graphics/directgdiimagetarget.h>
#include "directgdipaniccodes.h"

/**
Default constructor. 
Only for embedding instances of RDirectGdiImageTarget into other classes as data members. 
Before a RDirectGdiImageTarget can be used the other constructor must be called. 
*/
EXPORT_C RDirectGdiImageTarget::RDirectGdiImageTarget()
	: iDriver(NULL), iHandle(KNullHandle)
	{
	}

/**
Constructor.

@param 	aDriver The CDirectGdiDriver for this thread.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
@post 	This handle class has a pointer to the CDirectGdiDriver object for this
		thread, and is not associated with any DirectGDI adaptation specific resource.
*/
EXPORT_C RDirectGdiImageTarget::RDirectGdiImageTarget(CDirectGdiDriver& aDriver)
	: iDriver(&aDriver), iHandle(KNullHandle)
	{
	}

/**
Creates a DirectGDI adaptation-specific resource from the given image resource
so it can be used as a target of DirectGDI rendering.

@param 	aImage The image resource.

@pre 	The CDirectGdiDriver object has been set. The image resource has been
		fully constructed and created with the correct usage that allows it to
		be used as a DirectGDI target.
@post 	The DirectGDI adaptation-specific resource that is bound to the given
		image resource is created and this handle is now associated with it.
		The reference counter on the image resource is incremented.

@return KErrNone if successful, KErrArgument if the image resource is not valid,
		KErrAlreadyExists if this handle is already associated with a DirectGDI
		specific resource,  KErrNotSupported if the image resource is not created
		with the correct usage, otherwise one of the system-wide error codes.
*/
EXPORT_C TInt RDirectGdiImageTarget::Create(const RSgImage& aImage)
	{
	GRAPHICS_ASSERT_ALWAYS(iDriver, EDirectGdiPanicImageTargetWithoutDriver);
	return iDriver->CreateImageTarget(*this, aImage);
	}

/**
Destroys the DirectGDI adaptation-specific resource associated with this handle. 
Calling this method on a handle that is not associated with any DirectGDI 
adaptation-specific resource will do nothing. Once Close() is called, this handle can be reused.

@pre	The CDirectGdiDriver object for this handle has been set.
@post	The DirectGDI specific resource associated with this handle will be destroyed
		(at any time preferred by the adaptation). This handle is no longer associated
		with a DirectGDI specific resource. The reference counter of the underlying
		image resource is decremented. 
*/
EXPORT_C void RDirectGdiImageTarget::Close()
	{
	if (iHandle != KNullHandle)
		{
		GRAPHICS_ASSERT_ALWAYS(iDriver, EDirectGdiPanicImageTargetWithoutDriver);
		iDriver->CloseImageTarget(*this);
		}
	}
