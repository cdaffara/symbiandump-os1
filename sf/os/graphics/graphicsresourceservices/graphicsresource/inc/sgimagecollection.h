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
 @publishedPartner
 @prototype
*/

#ifndef SGIMAGECOLLECTION_H
#define SGIMAGECOLLECTION_H

#include <graphics/sgimage.h>
#include <graphics/surface.h>


class MSgImageCollectionAdapter;

/**
@publishedPartner
@prototype
@deprecated

A handle to a reference-counted collection of images with the same attributes which
is allocated in such a way that it can be used with the Graphics Composition Engine.
Users can access each of the images in the collection individually through RSgImage
handles. Interoperability with the Graphics Composition Engine is achieved by
passing the surface identifier returned by SurfaceId() in calls to the Graphics
Composition Engine API or the Surface Update API.

Since both images and image collections are reference-counted, and an image collection
can only be destroyed as a whole, then an image collection is guaranteed to exist
while there is an open RSgImageCollection handle referencing it or there are open
RSgImage handles referencing any of the individual images in the collection.

A new RSgImageCollection handle does not refer to an image collection until a
successful call to Create(). Before that point, the handle is said to be a null
handle. Instances of RSgImageCollection can be shared among threads in the same
process.

An RSgImageCollection handle is said to be invalid if it is not null but it does not
reference an existing image collection. Copying an instance of RSgImageCollection
must be done with extreme care, since it does not increment the reference count of
the referenced image collection and may therefore allow some RSgImageCollection or
RSgImage handle to become invalid when the image collection is destroyed.

@see RSgImage
*/
NONSHARABLE_CLASS(RSgImageCollection)
	{
public:
	IMPORT_C RSgImageCollection();
	IMPORT_C TInt Create(const TSgImageInfo& aInfo, TInt aImageCount);
	IMPORT_C static TInt Create(const TSgImageInfo aInfos[], TInt aImageCount,
	                            RSgImageCollection aCollections[], TInt aCollectionCount);
	IMPORT_C void Close();
	IMPORT_C const TSurfaceId& SurfaceId() const;
	IMPORT_C TBool IsNull() const;
	IMPORT_C TInt GetInfo(TSgImageInfo& aInfo) const;
	IMPORT_C TInt Count() const;
	IMPORT_C TInt OpenImage(TInt aIndex, RSgImage& aResult);
private:
	MSgImageCollectionAdapter* iImpl;
	};


#endif // SGIMAGECOLLECTION_H
