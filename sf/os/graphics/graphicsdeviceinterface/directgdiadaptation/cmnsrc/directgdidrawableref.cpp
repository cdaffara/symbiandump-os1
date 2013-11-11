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

#include "directgdidrawableref.h"


/**
Destructor. Checks that the reference count is zero to make sure all references to this object have
been closed properly before it is destroyed.

@panic DGDIAdapter 44, if the reference count is not zero. The
driver that created this object is responsible for decrementing the reference count.
 */
CDirectGdiDrawableRef::~CDirectGdiDrawableRef()
	{
	GRAPHICS_ASSERT_DEBUG(iRefCount == 0, EDirectGdiPanicDrawableRefDestructorError);
	}

/**
Increments the reference count for this object.
*/
void CDirectGdiDrawableRef::Open()
	{
	++iRefCount;
	}

/**
Decrements the reference count for this object. When the count reaches zero, it deletes itself.
Therefore no operations should be called on an object of this type after calling DecRefCount().

@return The decremented open count.

@panic DGDIAdapter 30, when attempting to decrement a reference count that is zero or less.
 */
void CDirectGdiDrawableRef::Close()
	{
	GRAPHICS_ASSERT_DEBUG(iRefCount > 0, EDirectGdiPanicDrawableRefCountError);
	if (--iRefCount == 0)
		{
		delete this;
		}
	}
