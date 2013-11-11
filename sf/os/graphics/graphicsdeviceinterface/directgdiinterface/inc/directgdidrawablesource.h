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

#ifndef DIRECTGDIDRAWABLESOURCE_H
#define DIRECTGDIDRAWABLESOURCE_H

/**
@file
@publishedPartner
@prototype 
*/

#include <e32def.h>

// Forward declarations.
//
class RSgDrawable;
class CDirectGdiDriver;

/**
This class is a handle to a DirectGDI adaptation-specific resource that is bound 
to a non-image resource (RSgDrawable). The intention of creating this class is to allow 
a non-image resource (global and sharable between processes) to be used as a source from 
the DirectGDI rendering API. This class and its DirectGDI adaptation-specific resource are 
non-sharable and will be local to the calling thread.

This class is part of generic code and will forward creation and deletion requests
to the CDirectGdiDriver singleton object for the calling thread.

@publishedPartner
@prototype
@deprecated
*/
NONSHARABLE_CLASS(RDirectGdiDrawableSource)
	{
	friend class CDirectGdiDriver;
	
public:
	IMPORT_C RDirectGdiDrawableSource();
	IMPORT_C RDirectGdiDrawableSource(CDirectGdiDriver& aDriver);
	IMPORT_C TInt Create(const RSgDrawable& aDrawable);
	IMPORT_C void Close();
	inline TInt Handle() const;
	
private:
	CDirectGdiDriver* iDriver;
	TInt iHandle;
	};
	
#include <graphics/directgdidrawablesource.inl>

#endif // DIRECTGDIDRAWABLESOURCE_H
