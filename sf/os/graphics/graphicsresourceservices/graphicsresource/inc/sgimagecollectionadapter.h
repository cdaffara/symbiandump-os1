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

#ifndef SGIMAGECOLLECTIONADAPTER_H
#define SGIMAGECOLLECTIONADAPTER_H

#include <graphics/sgresourceadapter.h>
#include <graphics/sgimagecollection.h>


/**
@publishedPartner
@prototype
@deprecated

This interface must be implemented by all the user-side objects in the adaptation
layer of the Graphics subsystem which are referenced by instances of RSgImageCollection.
The interface between the user-side and the kernel-side parts of the adaptation
layer is outside the scope of the specification of the Graphics Resource API.

@see RSgImageCollection
*/
class MSgImageCollectionAdapter: public MSgResourceAdapter
	{
public:
	/**
	@publishedPartner
	@prototype

	Retrieves the surface identifier of the image collection represented by this
	adapter object.

	@return The surface identifier of the image collection.
	@see RSgImageCollection::SurfaceId()
	*/
	virtual const TSurfaceId& SurfaceId() const = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Retrieves the values of the attributes of the images in the image collection
	represented by this adapter object.

	@param aInfo On input, the globally unique identifiers of the user-defined
	       attributes to be retrieved from the image collection, if any. On return,
	       the values of the attributes of the images in the collection and the
	       values of the selected user-defined attributes.
	@pre If aInfo.iUserAttributes is not null then it points to an array of
	     aInfo.iUserAttributeCount elements with globally unique identifiers
	     corresponding to user-defined attributes attached to the image collection.
	@return KErrNone if successful.
	@return KErrNotFound if any of the user-defined attributes to be retrieved from
	        the image collection cannot be found.
	@see RSgImageCollection::GetInfo()
	*/
	virtual TInt GetInfo(TSgImageInfo& aInfo) const = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Retrieves the number of images in the image collection represented by this
	adapter object.

	@return The number of images in the image collection.
	@see RSgImageCollection::Count()
	*/
	virtual TInt Count() const = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Opens a new handle to one of the images in the image collection represented by
	this adapter object. If there are no handles to the image open in the calling
	process then this function creates a new adapter object that represents the image
	in the context of the calling process. Otherwise this function just increments
	the reference count of the existing adapter object that represents the image
	in the context of the calling process.

	@param aIndex The index of the image within the image collection.
	@param aResult On return, a pointer to the adapter object that represents the
	       specified image in the collection.
	@pre aIndex is greater than or equal to zero and less than the number of images
	     in the collection.
	@pre aResult is null.
	@post aResult points to either a newly created or an existing adapter object
	      that represents the specified image in the collection. If a new adapter
	      object is created then its initial reference count is one and the reference
	      count for the image collection is incremented by one. Otherwise only the
	      reference count for the adapter object is incremented by one.
	@return KErrNone if successful.
	@return KErrInUse if aResult was not null.
	@return KErrArgument if aIndex is invalid.
	@return KErrNoMemory if there is not enough system memory.
	@see RSgImageCollection::OpenImage()
	*/
	virtual TInt OpenImage(TInt aIndex, MSgDrawableAdapter*& aResult) = 0;
	};


#endif // SGIMAGECOLLECTIONADAPTER_H
