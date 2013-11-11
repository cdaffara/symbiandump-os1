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
 @internalComponent
*/

#ifndef SGDRIVERIMPL_H
#define SGDRIVERIMPL_H

#include "sgresourceadapter.h"
#include <graphics/surfacemanager.h>


/**
@internalComponent

The category name of the panics raised by Graphics Resource Adapter.
*/
_LIT(KSgResourceAdapterPanicCategory, "SGRES-ADAPTER");


/**
The reason numbers of the panics raised by Graphics Resource Adapter.
*/
enum TSgResourceAdapterPanicReason
	{
	/**
	@internalComponent

	SGRES-ADAPTER 1 In debug builds, an instance of TSgImageInfo is invalid.
	*/
	ESgPanicBadImageInfo = 1,
	/**
	@internalComponent

	SGRES-ADAPTER 2 In debug builds, the reference count of an object is invalid.
	*/
	ESgPanicBadReferenceCount = 2,
	/**
	@internalComponent

	SGRES-ADAPTER 3 In debug builds, an internal function in the Graphics Resource
	driver has been called without proper inter-thread synchronisation.
	*/
	ESgPanicMutexNotHeld = 3,
	/**
	@internalTechnology
	@prototype

	SGRES-ADAPTER 4 In debug builds, an image has not been prepared for CPU access
	to its pixel data.
	*/
	ESgPanicNoCpuAccess = 4,
	/**
	@internalComponent

	SGRES-ADAPTER 5 In debug builds, an internal function has failed unexpectedly.
	*/
	ESgPanicResourceAdapterGeneral = 5
	};


/**
@internalComponent

Panics the current thread specifying a panic reason from Graphics Resource Adapter.
*/
inline void Panic(TSgResourceAdapterPanicReason aReason);


/**
@internalComponent

Calculates the minimum number of bytes between rows of pixel data for a given
pixel width and pixel format, regardless of alignment.
*/
TInt SgMinDataStride(TInt aWidth, TUidPixelFormat aPixelFormat);


/**
@internalComponent

Calculates the number of bytes between rows of pixel data given the width in 
pixels and the pixel format, taking into consideration the alignment 
requirements of the hardware platform.
*/
TInt SgAlignedDataStride(TInt aWidthInPixels, TUidPixelFormat aPixelFormat);


/**
@internalComponent

Calculates the offset in bytes from the base of the underlying memory chunk to
the first buffer of a surface used to store images.
*/
TInt SgOffsetToFirstBuffer(TInt aMetaDataSize);


/**
@internalComponent

Calculates the size in bytes of a buffer able to store an image with the given
row size in bytes and number of rows.
*/
TInt SgOffsetBetweenBuffers(TInt aDataStride, TInt aScanLineCount);


/**
@internalComponent

Tests whether an instance of TSgImageInfo is valid.
*/
TBool SgIsValidImageInfo(const TSgImageInfo& aInfo);


/**
@internalComponent

Tests whether an instance of TSgImageInfo specifies mutable images.
*/
TBool SgIsMutableImage(const TSgImageInfo& aInfo);


/**
@internalComponent

Tests whether an instance of TSgImageInfo specifies CPU-cached images.
*/
TBool SgIsCachedImage(const TSgImageInfo& aInfo);


class XSgDriverImpl;

/**
@internalComponent

Base class for all reference-counted classes to be instantiated on the heap for
adapter objects. Instances of derived classes must be allocated on the heap first
and then initialised using the placement new operator.

This class provides:
	- Initialisation of instances of derived classes to binary zeroes through a
	  specialised placement new operator.
	- A virtual destructor and a Delete() function that allow instances of derived
	  classes to be properly destroyed and deallocated through a pointer to this class.
	- A reference count.
*/
class XSgBase
	{
public:
	virtual ~XSgBase();
	void Delete();
	inline void IncRefCount();
	inline TInt DecRefCount();
	inline TInt RefCount() const;
	inline TAny* operator new(TUint aSize, TAny* aBase);
protected:
	inline XSgBase(XSgDriverImpl& aDriverImpl);
private:
	XSgBase(const XSgBase&);
	const XSgBase& operator =(const XSgBase&);
	TAny* operator new(TUint);
protected:
	XSgDriverImpl& iDriverImpl;
private:
	TInt iRefCount;
	};


/**
@internalComponent

An entry in the pixel format support table.
*/
class TSgPixelFormatTableEntry
	{
public:
	TBool IsMatch(const TSgImageInfo& aInfo) const;
	TBool IsMatchIgnoringPixelFormat(const TSgImageInfo& aInfo) const;
	TBool IsMatchIgnoringUsage(const TSgImageInfo& aInfo) const;
public:
	/**
	The supported pixel format.
	*/
	TUidPixelFormat iPixelFormat;
	/**
	The supported usages.
	*/
	TUint32 iUsage;
	/**
	The supported CPU access.
	*/
	TSgCpuAccess iCpuAccess;
	/**
	The supported screens. A value of -1 is interpreted as meaning that all screens
	are supported. Zero and positive values are interpreted as meaning that only
	the specified screen is supported.
	*/
	TInt iScreenId;
	};


class XSgImageImplBase;
class XSgImageCollectionImpl;

/**
@internalComponent

The Graphics Resource Adapter singleton class. The initialisation of the Graphics
Resource Adapter singleton consists of the following steps:
	- The heap for adapter objects is created.
	- An instance of the singleton class is allocated on the heap for adapter objects.
	- The C++ constructor is called using the placement new operator and ownership
	  of the heap for adapter objects is transferred to the singleton.
	- The second-phase constructor is called to complete construction of the singleton.

This class owns a single mutex and provides Wait() and Signal() functions to synchronise
access to all the adapter objects. An alternative could be for each adapter object to
have an associated mutex, but the possible improvement in concurrency does not justify
the costs in systems with only one CPU.
*/
class XSgDriverImpl: public MSgDriverAdapter
	{
public:
	inline XSgDriverImpl(RHeap* aHeap);
	TInt Construct();
	~XSgDriverImpl();
	inline TAny* operator new(TUint aSize, TAny* aBase);
	inline void Wait();
	inline void Signal();
	inline TBool IsMutexHeld() const;
	inline TAny* Alloc(TInt aSize);
	inline void Free(TAny* aCell);
	inline TInt CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId);
	inline TInt CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId, const RChunk& aChunk);
	inline TInt OpenSurface(const TSurfaceId& aSurfaceId);
	inline TInt CloseSurface(const TSurfaceId& aSurfaceId);
	TInt MapSurface(const TSurfaceId& aSurfaceId, RChunk& aChunk);
	inline TInt SurfaceInfo(const TSurfaceId& aSurfaceId, RSurfaceManager::TInfoBuf& aInfo);
	inline TInt SynchronizeCache(const TSurfaceId& aSurfaceId, TInt aBuffer, RSurfaceManager::TSyncOperation aOperation);
	inline TInt GetSurfaceHint(const TSurfaceId& aSurfaceId, RSurfaceManager::THintPair& aHint);
	TArray<TSgPixelFormatTableEntry> PixelFormatTable() const;
	TInt CanCreateImage(const TSgImageInfo& aInfo) const;
	TInt OpenImage(const TSgDrawableId& aId, TUint32 aMode, MSgDrawableAdapter*& aResult);
	void DeleteImage(XSgImageImplBase* aImage);
	void DeleteImageCollection(XSgImageCollectionImpl* aImageCollection);
	// From MSgDriverAdapter
	void Delete();
	TInt GetPixelFormats(const TSgImageInfo& aInfo, TUidPixelFormat* aPixelFormats, TInt& aCount);
	TInt CreateImage(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride, MSgDrawableAdapter*& aResult);
	TInt CreateImage(const TSgImageInfo& aInfo, MSgImageAdapter* aImage, MSgDrawableAdapter*& aResult);
	TInt CreateImageCollection(const TSgImageInfo& aInfo, TInt aImageCount, MSgImageCollectionAdapter*& aResult);
	TInt CreateImageCollections(const TSgImageInfo aInfos[], TInt aImageCount,
	                            MSgImageCollectionAdapter* aCollections[], TInt aCollectionCount);
	TInt OpenDrawable(const TSgDrawableId& aId, TUint32 aMode, TUid aHandleType, MSgDrawableAdapter*& aResult);
	TBool CheckDrawable(const MSgResourceAdapter& aDrawable) const;
	TBool CheckImage(const MSgResourceAdapter& aImage) const;
	TBool CheckImageCollection(const MSgResourceAdapter& aImageCollection) const;
	TInt ResourceCount() const;
	void AllocMarkStart();
	void AllocMarkEnd(TInt aCount);
	void SetAllocFail(RAllocator::TAllocFail aType, TInt aRate);
	TInt GetBufferOffset(const TSurfaceId& aSurfaceID, TInt aBuffer, TInt &aOffset);
	TInt GetSurfaceManagerAttrib(RSurfaceManager::TSurfaceManagerAttrib aAttrib, TInt& aValue);
private:
	XSgDriverImpl(const XSgDriverImpl&);
	TInt ConstructPixelFormatTable();
	const XSgDriverImpl& operator =(const XSgDriverImpl&);
	TAny* operator new(TUint);
private:
	/** Handle to the mutex used to synchronize access to the adapter objects. */
	mutable RMutex iMutex;
	/** Heap on which the adapter objects are allocated. */
	RHeap* iHeap;
	/** Handle to the surface manager. */
	RSurfaceManager iSurfaceManager;
	/** Pixel format support table. */
	RArray<TSgPixelFormatTableEntry> iPixelFormatTable;
	/** Image adapter objects ordered by identifier. */
	RPointerArray<XSgImageImplBase> iImages;
	/** Image collection adapter objects ordered by address. */
	RPointerArray<XSgImageCollectionImpl> iImageCollections;
	/** Size in pixels passed in the last call to GetPixelFormats(). */
	TSize iLastSizeInPixels;
	/** Usage passed in the last call to GetPixelFormats(). */
	TUint32 iLastUsage;
	/** CPU access passed in the last call to GetPixelFormats(). */
	TSgCpuAccess iLastCpuAccess;
	/** Screen identifier passed in the last call to GetPixelFormats(). */
	TInt iLastScreenId;
	/** Results of the last call to GetPixelFormats(). */
	RArray<TUidPixelFormat> iLastPixelFormats;
	};


#include "sgdriverimpl.inl"


#endif // SGDRIVERIMPL_H
