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

#ifndef SGRESOURCEADAPTER_H
#define SGRESOURCEADAPTER_H

#include <graphics/sgresource.h>
#include <pixelformats.h>


/**
@publishedPartner
@prototype
@deprecated

The file name of the Graphics Resource Adapter DLL.
*/
_LIT(KSgResourceAdapterLibraryName, "graphicsresourceadapter");


/**
@publishedPartner
@prototype
@deprecated

The UID3 value in the compound identifier of the Graphics Resource Adapter DLL.
*/
const TUid KSgResourceAdapterLibraryUid = {0x10285A71};


/**
@publishedPartner
@prototype
@deprecated

The category name of the panics raised by the Graphics Resource API.
*/
_LIT(KSgResourcePanicCategory, "SGRES");


/**
@publishedPartner
@prototype
@deprecated

The reason numbers of the panics raised by the Graphics Resource API.
*/
enum TSgResourcePanicReason
	{
	/**
	SGRES 1 In debug builds, there still are open handles to graphics resources in
	a process when the process termination tasks are carried out.
	*/
	ESgPanicUnclosedResources = 1,
	/**
	SGRES 2 In debug builds, an RSgDrawable handle is invalid.
	*/
	ESgPanicBadDrawableHandle = 2,
	/**
	SGRES 3 In debug builds, an RSgImage handle is invalid.
	*/
	ESgPanicBadImageHandle = 3,
	/**
	SGRES 4 In debug builds, an RSgImageCollection handle is invalid.
	*/
	ESgPanicBadImageCollectionHandle = 4,
	/**
	SGRES 5 In debug builds, the Graphics Resource driver is not initialised for
	use in the context of the calling process.
	*/
	ESgPanicNoDriver = 5
	};


/**
@publishedPartner
@prototype
@deprecated

Panics the current thread specifying a panic reason from the Graphics Resource API.
*/
inline void Panic(TSgResourcePanicReason aReason);


/**
@publishedPartner
@prototype
@deprecated

This constant consists of all the flags specifying usage of a graphics resource
as a source of any rendering pipeline.
*/
const TUint32 KSgUsageAllSources = ESgUsageDirectGdiSource | ESgUsageCompositionSource
	| ESgUsageScreenSource | ESgUsageOpenGlesTexture2D | ESgUsageOpenVgImage
	| ESgUsageOpenGles2Texture2D | ESgUsageWindowGcSource;


/**
@publishedPartner
@prototype
@deprecated

This constant consists of all the flags specifying usage of a graphics resource
as a target of any rendering pipeline.
*/
const TUint32 KSgUsageAllTargets = ESgUsageDirectGdiTarget | ESgUsageCompositionTarget
	| ESgUsageOpenGlesTarget | ESgUsageOpenVgTarget | ESgUsageEglCopyBuffersTarget
	| ESgUsageOpenGles2Target;


/**
@publishedPartner
@prototype
@deprecated

The default open mode for drawable resources.
*/
const TUint32 KSgDefaultOpenMode = 0;


/**
@publishedPartner
@prototype
@deprecated

This interface must be implemented by all the user-side objects in the adaptation
layer of the Graphics subsystem which are referenced by instances of any handle
class in the Graphics Resource API. The interface between the user-side and the
kernel-side parts of the adaptation layer is outside the scope of the specification
of the Graphics Resource API.

Each resource referenced by a handle opened in the context of a process must be
represented by an adapter object inside the process. Both the adapter object and
the resource itself have reference counts. The reference count for the adapter object
equals the number of handles to the resource open in the process, while the adapter
object counts as a single reference to the resource.

Adapter objects can be shared between all the threads in a process. This has two
consequences:
	- Adapter objects must be allocated in a multi-threaded heap owned by the Graphics
	  Resource Adapter singleton.
	- Access to adapter objects must be synchronised by means of a mutual exclusion
	  mechanism.
*/
class MSgResourceAdapter
	{
public:
	/**
	@publishedPartner
	@prototype
	@deprecated

	Closes this adapter object by decrementing its reference count by one and, if the
	count becomes zero, destroying the adapter object. If the adapter object is
	destroyed then the reference count for the represented resource is decremented
	by one.

	@see RSgDrawable::Close()
	@see RSgImageCollection::Close()
	*/
	virtual void Close() = 0;
	};


/**
@publishedPartner
@prototype
@deprecated

This interface must be implemented by all the user-side objects in the adaptation
layer of the Graphics subsystem which are referenced by instances of RSgDrawable.
The interface between the user-side and the kernel-side parts of the adaptation
layer is outside the scope of the specification of the Graphics Resource API.

@see RSgDrawable
*/
class MSgDrawableAdapter: public MSgResourceAdapter
	{
public:
	/**
	@publishedPartner
	@prototype
	@deprecated

	Retrieves the unique identifier of the drawable resource represented by this
	adapter object.

	@return The unique identifier of the drawable resource.
	@see RSgDrawable::Id()
	*/
	virtual const TSgDrawableId& Id() const = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Retrieves the actual type of the drawable resource represented by this adapter
	object as a globally unique identifier.

	@return The actual type of the drawable resource as a globally unique identifier.
	@see RSgDrawable::DrawableType()
	*/
	virtual TUid DrawableType() const = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Makes an extension interface available on the drawable resource represented by
	this adapter object.

	@param aInterfaceUid Globally unique identifier of the interface to be made
	       available.
	@param aInterfacePtr On return, a pointer to the specified interface.
	@pre The specified interface is supported on the drawable resource.
	@post The specified interface is available until this adapter object is destroyed.
	@return KErrNone if successful.
	@return KErrExtensionNotSupported if the specified interface is not supported
	        on the drawable resource.
	@return KErrNoMemory if there is not enough system memory.
	@see RSgDrawable::GetInterface()
	*/
	virtual TInt GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr) = 0;
	};


class TSgImageInfo;
class RSgImage;
class MSgImageAdapter;
class MSgImageCollectionAdapter;

/**
@publishedPartner
@prototype
@deprecated

This interface must be implemented by the Graphics Resource Adapter singleton.
There must be a single instance of the adaptation layer class that implements
this interface in each process using the Graphics Resource API.
*/
class MSgDriverAdapter
	{
public:
	/**
	@publishedPartner
	@prototype
	@deprecated

	Creates a new instance of the singleton class and carries out the initialisation
	tasks needed to use the Graphics Resource API in the context of the calling
	process. This is the only function that must be exported by the Graphics Resource
	Adapter DLL, at ordinal 1.

	@param aPtr On return, a pointer to the new instance of the singleton class.
	@return KErrNone if successful.
	@return KErrNoMemory if there is not enough system memory.
	@see SgDriver::Open()
	*/
	IMPORT_C static TInt New(MSgDriverAdapter*& aPtr);
	/**
	@publishedPartner
	@prototype
	@deprecated

	Deletes an instance of the singleton class and carries out the termination tasks
	needed to release the internal resources allocated for the calling process.

	@see SgDriver::Close()
	*/
	virtual void Delete() = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Retrieves the list of image pixel formats supported on this device for a
	specified usage.

	@param aInfo The image attributes. aInfo.iPixelFormat is ignored.
	@param aPixelFormats A pointer to an array that on return will contain the
	       supported pixel formats. If this parameter is null, then this function
	       will just return the number of supported pixel formats in aCount.
	@param aCount On input, the number of elements in the array pointed to by
	       aPixelFormats if not null, ignored otherwise. On return, the actual number
	       of supported pixel formats. If this number is greater than the number of
	       elements in the array, then the array will be filled with as many pixel
	       formats as it can hold and the function will return KErrOverflow.
	@pre aInfo is valid.
	@pre If aPixelFormats is not null then aCount is greater than zero.
	@return KErrNone if successful.
	@return KErrArgument if aInfo is invalid or if aPixelFormats is not null and
	        aCount is negative.
	@return KErrOverflow if the actual number of supported pixel formats is greater
	        than the number of elements in the array pointed to by aPixelFormats.
	@return KErrNoMemory if there is not enough system memory.
	@see RSgImage::GetPixelFormats()
	*/
	virtual TInt GetPixelFormats(const TSgImageInfo& aInfo, TUidPixelFormat* aPixelFormats, TInt& aCount) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Creates an image with the specified attributes and, optionally, the specified
	initial contents.

	@param aInfo The attributes of the image to be created.
	@param aDataAddress The base address of the pixel data used to populate the
	       new image. If this value is null, the initial contents of the image are
	       undefined. If aInfo specifies that the new image is constant, this value
	       must not be null.
	@param aDataStride The number of bytes between rows of the pixel data used to
	       populate the new image.
	@param aResult On return, a pointer to the adapter object that represents the
	       new image.
	@pre aInfo is supported.
	@pre aResult is null.
	@post aResult points to an adapter object that represents a newly created image
	      with the specified attributes and initial contents. The initial reference
	      count for the image is one.
	@return KErrNone if successful.
	@return KErrInUse if aResult was not null.
	@return KErrArgument if aInfo is invalid.
	@return KErrNoInitializationData if aInfo requests a constant image and aDataAddress
	        is null.
	@return KErrTooBig if the size specified in aInfo is too large.
	@return KErrNotSupported if aInfo is not supported.
	@return KErrNoMemory if there is not enough system memory.
	@return KErrNoSpecializedMemory if there is not enough specialised graphics memory.
	@see RSgImage::Create()
	*/
	virtual TInt CreateImage(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride, MSgDrawableAdapter*& aResult) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Creates an image with the specified attributes and initial contents copied
	from an existing image.

	@param aInfo The attributes of the image to be created.
	@param aImage A pointer to the adapter object that represents the existing
	       image to be copied.
	@param aResult On return, a pointer to the adapter object that represents the
	       new image.
	@pre aInfo is supported.
	@pre aImage is not null.
	@pre The size and the pixel format specified in aInfo must be the same as the
	     size and the pixel format of the image represented by aImage.
	@pre aResult is null.
	@post aResult points to an adapter object that represents a newly created image
	      with the specified attributes and initial contents. The initial reference
	      count for the image is one.
	@return KErrNone if successful.
	@return KErrInUse if aResult was not null.
	@return KErrArgument if aInfo is invalid or if aImage is null.
	@return KErrNotSupported if aInfo is not supported or if the size and the pixel
	        format specified in aInfo are not the same as the size and the pixel
	        format of the image represented by aImage.
	@return KErrNoMemory if there is not enough system memory.
	@return KErrNoSpecializedMemory if there is not enough specialised graphics memory.
	@see RSgImage::Create()
	*/
	virtual TInt CreateImage(const TSgImageInfo& aInfo, MSgImageAdapter* aImage, MSgDrawableAdapter*& aResult) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Creates an image collection with the specified attributes.

	@param aInfo The image attributes of the collection to be created.
	@param aImageCount The number of images in the collection to be created.
	@param aResult On return, a pointer to the adapter object that represents the
	       new image collection.
	@pre aInfo is supported and specifies mutable images.
	@pre aImageCount is greater than zero.
	@pre aResult is null.
	@post aResult points to an adapter object that represents a newly created image
	      collection with the specified attributes. The initial reference count
	      for the image collection is one.
	@return KErrNone if successful.
	@return KErrInUse if aResult was not null.
	@return KErrArgument if aInfo is invalid or if aImageCount is negative or zero.
	@return KErrTooBig if the size specified in aInfo is too large.
	@return KErrNotSupported if aInfo is not supported or does not specify mutable
	        images.
	@return KErrNoMemory if there is not enough system memory.
	@return KErrNoSpecializedMemory if there is not enough specialised graphics memory.
	@see RSgImageCollection::Create()
	*/
	virtual TInt CreateImageCollection(const TSgImageInfo& aInfo, TInt aImageCount, MSgImageCollectionAdapter*& aResult) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Creates a set of image collections that share a single memory chunk.

	@param aInfos An array of aCollectionCount elements with the image attributes
	       of each of the collections to be created.
	@param aImageCount The number of images in each of the collections to be created.
	@param aCollections On return, an array of pointers to the adapter objects that
	       represent the new image collections.
	@param aCollectionCount The number of image collections to be created.
	@pre All the elements of aInfos are supported and specify mutable images.
	@pre aImageCount is greater than zero.
	@pre All the pointers in aCollections are null.
	@pre aCollectionCount is greater than zero.
	@post The elements of aCollections point to aCollectionCount adapter objects that
	      represent newly created image collections with the specified attributes.
	      The initial reference count for each of the image collections is one.
	@return KErrNone if successful.
	@return KErrInUse if any of the pointers in aCollections was not null.
	@return KErrArgument if any element of aInfos is invalid, if aImageCount is
	        negative or zero, or if aCollectionCount is negative or zero.
	@return KErrTooBig if any of the sizes specified in aInfos is too large.
	@return KErrNotSupported if any element of aInfos is not supported or does not
	        specify mutable images.
	@return KErrNoMemory if there is not enough system memory.
	@return KErrNoSpecializedMemory if there is not enough specialised graphics memory.
	@see RSgImageCollection::Create()
	*/
	virtual TInt CreateImageCollections(const TSgImageInfo aInfos[], TInt aImageCount,
	                                    MSgImageCollectionAdapter* aCollections[], TInt aCollectionCount) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Opens a new handle to a drawable resource. If there are no handles to the drawable
	resource open in the calling process then this function creates a new adapter
	object that represents the drawable resource in the context of the calling process.
	Otherwise this function just increments the reference count of the existing adapter
	object that represents the drawable resource in the context of the calling process.

	@param aId The unique identifier of the drawable resource.
	@param aMode Flags controlling how the drawable resource is opened. The only value
	       of this parameter which it is mandatory to support is KSgDefaultOpenMode.
	       Extra opening options may be defined by an implementation of the Graphics
	       Resource API and made available through additional functions.
	@param aHandleType The type of the handle which is to store the reference to
	       the specified drawable resource as a globally unique identifier.
	@param aResult On return, a pointer to the adapter object that represents the
	       specified drawable resource.
	@pre aId identifies an existing drawable resource.
	@pre All of the requested opening options in aMode are supported.
	@pre aHandleType specifies an instance of RSgDrawable or any other handle type
	     that is compatible with the actual type of the specified drawable resource.
	@pre aResult is null.
	@post aResult points to either a newly created or an existing adapter object
	      that represents the drawable resource specified by its unique identifier.
	      If a new adapter object is created then its initial reference count is
	      one and the reference count for the drawable resource itself is incremented
	      by one. Otherwise only the reference count for the adapter object is
	      incremented by one.
	@return KErrNone if successful.
	@return KErrInUse if aResult was not null.
	@return KErrArgument if aId is the null drawable resource identifier.
	@return KErrNotFound if aId cannot be found to refer to an existing drawable
	        resource.
	@return KErrPermissionDenied if this process is not permitted to access the
	        drawable resource specified by aId.
	@return KErrNotSupported if any of the requested opening options in aMode is
	        not supported or if aHandleType is not compatible with the actual type
	        of the drawable resource specified by aId.
	@return KErrNoMemory if there is not enough system memory.
	@see RSgDrawable::Open()
	*/
	virtual TInt OpenDrawable(const TSgDrawableId& aId, TUint32 aMode, TUid aHandleType, MSgDrawableAdapter*& aResult) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Tests whether aDrawable references an existing adapter object representing a
	drawable resource. This function is called in debug builds to detect invalid
	RSgDrawable handles.

	@return ETrue if aDrawable is a valid reference to an adapter object representing
	        a drawable resource, EFalse otherwise.
	*/
	virtual TBool CheckDrawable(const MSgResourceAdapter& aDrawable) const = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Tests whether aImage references an existing adapter object representing an image.
	This function is called in debug builds to detect invalid RSgImage handles.

	@return ETrue if aImage is a valid reference to an adapter object representing
	        an image, EFalse otherwise.
	*/
	virtual TBool CheckImage(const MSgResourceAdapter& aImage) const = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Tests whether aImageCollection references an existing adapter object representing
	an image collection. This function is called in debug builds to detect invalid
	RSgImageCollection handles.

	@return ETrue if aImageCollection is a valid reference to an adapter object
	        representing an image collection, EFalse otherwise.
	*/
	virtual TBool CheckImageCollection(const MSgResourceAdapter& aImageCollection) const = 0;
	/**
	@publishedPartner
	@deprecated
	@test

	Retrieves the number of handles to graphics resources open in the calling process.

	@return The number of handles to graphics resources open in the calling process.
	*/
	virtual TInt ResourceCount() const = 0;
	/**
	@publishedPartner
	@deprecated
	@test

	Marks the start of cell checking on the heap for adapter objects. Calls to this
	function can be nested but each call must be matched by a corresponding call to
	AllocMarkEnd().

	@see RAllocator::__DbgMarkStart()
	*/
	virtual void AllocMarkStart() = 0;
	/**
	@publishedPartner
	@deprecated
	@test

	Marks the end of cell checking at the current nesting level on the heap for
	adapter objects. Each call to this function must match an earlier call to
	AllocMarkStart().

	This function checks that the number of cells allocated in the heap for adapter
	objects, at the current nesting level, is aCount. If the check fails then an
	SGALLOC:nnnnnnnn panic is raised, where nnnnnnnn is the hexadecimal address
	of the first orphaned cell.

	@param aCount The number of allocated heap cells expected.
	@see RAllocator::__DbgMarkEnd()
	*/
	virtual void AllocMarkEnd(TInt aCount) = 0;
	/**
	@publishedPartner
	@deprecated
	@test

	Sets the type and rate of simulated allocation failure on the heap for adapter
	objects.

	@param aType The type of allocation failure requested.
	@param aRate The rate of allocation failure requested.
	@see RAllocator::__DbgSetAllocFail()
	*/
	virtual void SetAllocFail(RAllocator::TAllocFail aType, TInt aRate) = 0;
	};


#include <graphics/sgresourceadapter.inl>


#endif // SGRESOURCEADAPTER_H
