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

#include "sgdriver.h"
#include "sgimagecollectionadapter.h"


/**
@internalComponent
*/
const TSurfaceId KNullSurfaceId = {0, 0, 0, 0};


/**
@publishedPartner
@prototype
@deprecated

Default constructor.

@pre None.
@post This RSgImageCollection handle is null.
*/
EXPORT_C RSgImageCollection::RSgImageCollection()
	: iImpl(NULL)
	{}


/**
@publishedPartner
@prototype
@deprecated

Creates an image collection with the specified attributes. The number of images in
the collection cannot be changed after creation. The images in the collection have
identical attributes. The initial contents of all the images are undefined.

@param aInfo The image attributes of the collection to be created.
@param aImageCount The number of images in the collection to be created.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImageCollection handle is null.
@pre aInfo is supported and specifies mutable images.
@pre aImageCount is greater than zero.
@post This RSgImageCollection handle references a newly created image collection with
      the specified attributes. The initial reference count for the image collection
      is one.
@return KErrNone if successful.
@return KErrInUse if this RSgImageCollection handle was not null.
@return KErrArgument if aInfo is invalid or if aImageCount is negative or zero.
@return KErrTooBig if the size specified in aInfo is too large.
@return KErrNotSupported if aInfo is not supported or does not specify mutable images.
@return KErrNoMemory if there is not enough system memory.
@return KErrNoSpecializedMemory if there is not enough specialised graphics memory.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImageCollection::Create(const TSgImageInfo& aInfo, TInt aImageCount)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	TInt err = gPls.iDriver->CreateImageCollection(aInfo, aImageCount, iImpl);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Creates a set of image collections that share a single memory chunk. The number of
images in each collection is the same and cannot be changed after creation. The images
in each collection have identical attributes. The initial contents of all the images
are undefined.

Care must be taken when using image collections created in this way. In particular,
writing to the Nth image of one collection may invalidate the contents of the Nth image
of all other collections, but will not affect any other images.

@param aInfos An array of aCollectionCount elements with the image attributes of each
       of the collections to be created.
@param aImageCount The number of images in each of the collections to be created.
@param aCollections On return, an array of RSgImageCollection handles that reference
       newly created image collections with the specified attributes.
@param aCollectionCount The number of image collections to be created.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre All the elements of aInfos are supported and specify mutable images.
@pre aImageCount is greater than zero.
@pre All the RSgImageCollection handles in aCollections are null.
@pre aCollectionCount is greater than zero.
@post The initial reference count for each of the newly created image collections is one.
@return KErrNone if successful.
@return KErrInUse if any of the RSgImageCollection handles in aCollections was not null.
@return KErrArgument if any element of aInfos is invalid, if aImageCount is negative
        or zero, or if aCollectionCount is negative or zero.
@return KErrTooBig if any of the sizes specified in aInfos is too large.
@return KErrNotSupported if any element of aInfos is not supported or does not specify
        mutable images.
@return KErrNoMemory if there is not enough system memory.
@return KErrNoSpecializedMemory if there is not enough specialised graphics memory.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImageCollection::Create(const TSgImageInfo aInfos[], TInt aImageCount,
                                         RSgImageCollection aCollections[], TInt aCollectionCount)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	TInt err = gPls.iDriver->CreateImageCollections(aInfos, aImageCount,
	                                                reinterpret_cast<MSgImageCollectionAdapter**>(aCollections), aCollectionCount);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Closes a handle to an image collection. If there are no remaining handles to
the image collection, then it can be destroyed by the Graphics Resource driver.
Calling Close() on a null handle is allowed but has no effect.

@pre If this RSgImageCollection handle is not null then the Graphics Resource
     driver is initialised for use in the context of the calling process.
@pre This RSgImageCollection handle is valid.
@post This RSgImageCollection handle is null. The reference count for the
      previously referenced image collection, if any, is decremented by one.
@panic SGRES 4 in debug builds if this RSgImageCollection handle is invalid.
@panic SGRES 5 in debug builds if this RSgImageCollection handle is not null
       and the Graphics Resource driver is not initialised for use in the
       context of the calling process.
*/
EXPORT_C void RSgImageCollection::Close()
	{
	if (iImpl)
		{
#ifdef _DEBUG
		gPls.iMutex.Wait();
		__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
		__ASSERT_DEBUG(gPls.iDriver->CheckImageCollection(*iImpl), Panic(ESgPanicBadImageCollectionHandle));
#endif
		iImpl->Close();
		iImpl = NULL;
#ifdef _DEBUG
		gPls.iMutex.Signal();
#endif
		}
	}


/**
@publishedPartner
@prototype
@deprecated

Retrieves the surface identifier of an image collection.

@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImageCollection handle is valid.
@post None.
@return The surface identifier of the image collection or the null surface identifier
        if this RSgImageCollection handle is null.
@panic SGRES 4 in debug builds if this RSgImageCollection handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C const TSurfaceId& RSgImageCollection::SurfaceId() const
	{
	if (!iImpl)
		{
		return KNullSurfaceId;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImageCollection(*iImpl), Panic(ESgPanicBadImageCollectionHandle));
#endif
	const TSurfaceId& id = iImpl->SurfaceId();
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return id;
	}


/**
@publishedPartner
@prototype
@deprecated

Tests whether this RSgImageCollection handle is null.

@pre None.
@post None.
@return ETrue, if this RSgImageCollection handle is null, EFalse otherwise.
*/
EXPORT_C TBool RSgImageCollection::IsNull() const
	{
	return iImpl == NULL;
	}


/**
@publishedPartner
@prototype
@deprecated

Retrieves the values of the attributes of the images in an image collection. This
function can also retrieve the values of selected user-defined attributes attached
to an image collection by passing in the globally unique identifiers of the
user-defined attributes to be retrieved.

@param aInfo On input, the globally unique identifiers of the user-defined attributes
       to be retrieved from the image collection, if any. On return, the values of
       the attributes of the images in the collection and the values of the selected
       user-defined attributes.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImageCollection handle is valid and not null.
@pre If aInfo.iUserAttributes is not null then it points to an array of
     aInfo.iUserAttributeCount elements with globally unique identifiers
     corresponding to user-defined attributes attached to the image collection.
@post None.
@return KErrNone if successful.
@return KErrBadHandle if this RSgImageCollection handle is null.
@return KErrNotFound if any of the user-defined attributes to be retrieved from the
        image collection cannot be found.
@panic SGRES 4 in debug builds if this RSgImageCollection handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImageCollection::GetInfo(TSgImageInfo& aInfo) const
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImageCollection(*iImpl), Panic(ESgPanicBadImageCollectionHandle));
#endif
	TInt err = iImpl->GetInfo(aInfo);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Retrieves the number of images in an image collection.

@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImageCollection handle is valid.
@post None.
@return The number of images in the image collection or zero if this RSgImageCollection
        handle is null.
@panic SGRES 4 in debug builds if this RSgImageCollection handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImageCollection::Count() const
	{
	if (!iImpl)
		{
		return 0;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImageCollection(*iImpl), Panic(ESgPanicBadImageCollectionHandle));
#endif
	TInt count = iImpl->Count();
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return count;
	}


/**
@publishedPartner
@prototype
@deprecated

Opens a new handle to one of the images in an image collection.

@param aIndex The index of the image within the image collection.
@param aResult On return, an RSgImage handle that references the specified image in
       the collection.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImageCollection handle is valid and not null.
@pre aIndex is greater than or equal to zero and less than the number of images in
     the collection.
@pre aResult is a null handle.
@post The reference count for the image collection is incremented by one.
@return KErrNone if successful.
@return KErrBadHandle if this RSgImageCollection handle is null.
@return KErrInUse if aResult was not a null handle.
@return KErrArgument if aIndex is invalid.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 4 in debug builds if this RSgImageCollection handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImageCollection::OpenImage(TInt aIndex, RSgImage& aResult)
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImageCollection(*iImpl), Panic(ESgPanicBadImageCollectionHandle));
#endif
	TInt err = iImpl->OpenImage(aIndex, aResult.iImpl);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}
