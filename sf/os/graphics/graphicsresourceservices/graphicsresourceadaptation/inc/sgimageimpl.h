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

#ifndef SGIMAGEIMPL_H
#define SGIMAGEIMPL_H

#include "sgimageadapter.h"
#include "sgdriverimpl.h"
#include "sgimage_sw.h"
#include "sgimage_chunk.h"


/**
@internalComponent

This class encapsulates the adapter-specific metadata associated with an image.
User attributes are excluded.
*/
class TSgImageMetaData
	{
public:
	TSgImageMetaData(const TSgImageInfo& aInfo, TArray<TSgPixelFormatTableEntry> aPixelFormatTable, TBool aIsCached = ETrue);
	void GetInfo(TSgImageInfo& aInfo, TBool aGetPotentialUsage = EFalse) const;
public:
	/**
	The identifier of the process that created the image.
	*/
	TProcessId iCreatorProcess;
	/**
	The size of the image in pixels.
	*/
	TSize iSizeInPixels;
	/**
	The pixel format of the image.
	*/
	TUidPixelFormat iPixelFormat;
	/**
	The intended usage of the image as requested during creation.
	*/
	TUint32 iRequestedUsage;
	/**
	The potential usage of the image as allowed by the implementation of the Graphics
	subsystem. This is calculated from the image attributes, excluding the requested
	usage, and the pixel format support table.
	*/
	TUint32 iPotentialUsage;
	/**
	Whether the image is shareable between processes.
	*/
	TBool iShareable;
	/**
	Whether and how the image is mappable for CPU access.
	*/
	TSgCpuAccess iCpuAccess;
	/**
	The identifier of the screen on which the image is usable or -1 if the image
	is usable on all screens.
	*/
	TInt iScreenId;
	/**
	Whether the image is CPU-cached.
	*/
	TBool iIsCached;
	};


/**
@internalComponent

Base class for all classes representing image state. The state of an image consists
of the following items:
	- The situation in memory of the pixel data. In platforms with Unified Memory
	  Architecture this is simply the address and stride of the pixel data in system
	  memory, since the image is always located in system memory. In other memory
	  architectures the image might change location between specialised graphics
	  memory and system memory, for example.
	- The situation in memory of the metadata and the user attributes. In the current
	  image implementation based on the Surface Manager the metadata is stored in
	  the same memory chunk as the pixel data and the user attributes are stored
	  as surface hints.
	- Whether and how the pixel data is being accessed by the CPU.

@see XSgImageImplBase
*/
class XSgImageStateBase: public XSgBase
	{
public:
	virtual const TSgImageMetaData& MetaData() const = 0;
	virtual TInt GetUserAttributes(TSgUserAttribute* aUserAttributes, TInt aUserAttributeCount) const = 0;
	virtual TAny* DataAddress() const = 0;
	inline TInt DataStride() const;
	virtual TInt BeginDataAccess(TSgCpuAccess aCpuAccess, TBool aIsUserAccess);
	virtual TInt EndDataAccess(TBool aIsUserAccess);
protected:
	inline XSgImageStateBase(XSgDriverImpl& aDriverImpl);
	inline XSgImageStateBase(XSgDriverImpl& aDriverImpl, TInt aDataStride);
protected:
	/**
	The number of bytes between rows of the pixel data.
	*/
	TInt iDataStride;
	/**
	Whether the pixel data is being accessed by the CPU for reading only, for
	writing only, for reading and writing or not at all.
	*/
	TSgCpuAccess iCpuAccess;
	/**
	The level of CPU access, if any:
	- If ETrue the pixel data is being accessed by the CPU from the application
	  level through a call to RSgImage::MapReadOnly(), RSgImage::MapWriteOnly()
	  or RSgImage::MapReadWrite().
	- If EFalse the pixel data is being accessed by the CPU from the Graphics
	  subsystem level through the MSgImage_Sw extension interface.
	*/
	TBool iIsUserAccess;
	};


/**
@internalComponent

Base class for all image adapter objects. This class currently implements the
MSgImage_Sw extension interface, since it assumes a platform with Unified Memory
Architecture and therefore availability of the extension interface on all images.

@see XSgImageStateBase
*/
class XSgImageImplBase: public XSgBase, public MSgImageAdapter, public MSgImage_Sw
	{
public:
	~XSgImageImplBase();
	inline const TSgImageMetaData& MetaData() const;
	static TInt Compare(const TSgDrawableId* aId, const XSgImageImplBase& aImage);
	static TInt Compare(const XSgImageImplBase& aImage1, const XSgImageImplBase& aImage2);
	static TInt CompareIgnoringFlags(const TSgDrawableId* aId, const XSgImageImplBase& aImage);
	// From MSgResourceAdapter
	void Close();
	// From MSgDrawableAdapter
	const TSgDrawableId& Id() const;
	TUid DrawableType() const;
	TInt GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr);
	// From MSgImageAdapter
	TInt GetInfo(TSgImageInfo& aInfo) const;
	TInt MapReadOnly(const TAny*& aDataAddress, TInt& aDataStride);
	TInt MapWriteOnly(TAny*& aDataAddress, TInt& aDataStride);
	TInt MapReadWrite(TAny*& aDataAddress, TInt& aDataStride);
	TInt Unmap();
	// From MSgImage_Sw
	TAny* DataAddress() const;
	TInt DataStride() const;
	TInt BeginDataAccess(TSgCpuAccess aCpuAccess);
	TInt EndDataAccess();
protected:
	inline XSgImageImplBase(XSgDriverImpl& aDriverImpl);
	inline XSgImageImplBase(XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId);
	XSgImageImplBase(const XSgImageImplBase& aImage, TUint32 aFlags);
	TInt SetData(const TAny* aDataAddress, TInt aDataStride);
protected:
	/** The unique identifier of the image. */
	TSgDrawableId iId;
	/** The state of the image. */
	XSgImageStateBase* iState;
	};


/**
@internalComponent

The position, as index into an array of 32-bit words, of the 32 bits reserved
for flags in the unique identifier of an image.
*/
const TInt KSgImageIdFlagsIndex = 5;


#ifndef SYMBIAN_GRAPHICS_USE_GPU

/**
@internalComponent

The structure of the unique identifier of an image allocated in memory that is
accessible only from the user-side of the creator process.

@see XSgImageImpl_SwLocal
*/
class TSgImageId_SwLocal
	{
public:
	inline static TBool IsMatch(const TSgDrawableId& aId);
public:
	/** 64 bits reserved for the identifier of the creator process. */
	TUint64 iProcessId;
	/** 64 random bits. */
	TUint32 iRandom[2];
	/** 32 bits set to "1". */
	TInt32 iMinusOne;
	/** 32 bits reserved for flags. */
	TUint32 iFlags;
	};


/**
@internalComponent

This class represents the state of an image allocated in memory that is accessible
only from the user-side of the creator process.

@see XSgImageImpl_SwLocal
*/
class XSgImageState_SwLocal: public XSgImageStateBase
	{
public:
	static TInt New(XSgImageState_SwLocal*& aPtr, XSgDriverImpl& aDriverImpl, const TSgImageInfo& aInfo);
	const TSgImageMetaData& MetaData() const;
	TInt GetUserAttributes(TSgUserAttribute* aUserAttributes, TInt aUserAttributeCount) const;
	TAny* DataAddress() const;
private:
	XSgImageState_SwLocal(XSgDriverImpl& aDriverImpl, const TSgImageInfo& aInfo, TInt aDataStride);
private:
	/**
	The metadata of the image.
	*/
	TSgImageMetaData iMetaData;
	/**
	The number of user attributes.
	*/
	TInt iUserAttributeCount;
	/**
	The user attributes of the image. The pixel data of the image is stored after
	them in the same heap cell.
	*/
	TSgUserAttribute iUserAttributes[1];
	};


/**
@internalComponent

An adapter object representing an image allocated in memory that is accessible only
from the user-side of the creator process. Non-shareable images are allocated in
this way on platforms without hardware acceleration. The pixel data, the metadata
and the user attributes of such an image are stored in the same heap cell.

@see XSgImageState_SwLocal
*/
class XSgImageImpl_SwLocal: public XSgImageImplBase
	{
public:
	static TInt New(XSgImageImpl_SwLocal*& aPtr, XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId,
	                const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride);
	static TInt New(XSgImageImpl_SwLocal*& aPtr, const XSgImageImpl_SwLocal& aImage, TUint32 aFlags);
private:
	inline XSgImageImpl_SwLocal(XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId);
	inline XSgImageImpl_SwLocal(const XSgImageImpl_SwLocal& aImage, TUint32 aFlags);
	TInt Construct(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride);
	};

#endif


/**
@internalComponent

The structure of the unique identifier of an image stored in a surface buffer.

@see XSgImageImpl_SurfaceManager
*/
class TSgImageId_SurfaceManager
	{
public:
	inline static TBool IsMatch(const TSgDrawableId& aId);
public:
	/** 128 bits reserved for the surface identifier. */
	TSurfaceId iSurfaceId;
	/** 16 bits reserved for the index to the surface buffer. */
	TInt16 iBufferIndex;
	/** 16 bits reserved for the index to the metadata. */
	TInt16 iMetaDataIndex;
	/** 32 bits reserved for flags. */
	TUint32 iFlags;
	};


/**
@internalComponent

This class represents the state of an image stored in a surface buffer.

@see XSgImageImpl_SurfaceManager
*/
class XSgImageState_SurfaceManager: public XSgImageStateBase
	{
public:
	static TInt New(XSgImageState_SurfaceManager*& aPtr, XSgDriverImpl& aDriverImpl, const TSgImageInfo& aInfo, TBool aIsCached);
	static TInt New(XSgImageState_SurfaceManager*& aPtr, XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId);
	~XSgImageState_SurfaceManager();
	const TSgImageMetaData& MetaData() const;
	TInt GetUserAttributes(TSgUserAttribute* aUserAttributes, TInt aUserAttributeCount) const;
	TAny* DataAddress() const;
#ifdef SYMBIAN_GRAPHICS_AUTOFLUSH_CACHE
	TInt BeginDataAccess(TSgCpuAccess aCpuAccess, TBool aIsUserAccess);
	TInt EndDataAccess(TBool aIsUserAccess);
#endif
	inline const TSurfaceId& SurfaceId() const;
	inline const RChunk& DataChunk() const;
	inline TInt DataOffset() const;
private:
	inline XSgImageState_SurfaceManager(XSgDriverImpl& aDriverImpl);
	TInt Construct(const TSgImageInfo& aInfo, TBool aIsCached);
	TInt Construct(const TSgDrawableId& aId);
private:
	/** The identifier of the surface in which the image is stored. */
	TSurfaceId iSurfaceId;
	/** Index to the surface buffer in which the image is stored. */
	TInt iBufferIndex;
	/** Offset to the metadata from the base of the underlying memory chunk. */
	TInt iMetaDataOffset;
	/** Handle to the underlying memory chunk. */
	RChunk iDataChunk;
	/** Offset to the pixel data from the base of the underlying memory chunk. */
	TInt iDataOffset;
	};


/**
@internalComponent

An adapter object representing an image stored in a surface buffer. The metadata
of such an image is stored at the beginning of the underlying memory chunk.

@see XSgImageState_SurfaceManager
*/
class XSgImageImpl_SurfaceManager: public XSgImageImplBase, public MSgImage_Chunk
	{
public:
	static TInt New(XSgImageImpl_SurfaceManager*& aPtr, XSgDriverImpl& aDriverImpl,
	                const TSgImageInfo& aInfo, TBool aIsCached, const TAny* aDataAddress, TInt aDataStride);
	static TInt New(XSgImageImpl_SurfaceManager*& aPtr, XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId);
	static TInt New(XSgImageImpl_SurfaceManager*& aPtr, const XSgImageImpl_SurfaceManager& aImage, TUint32 aFlags);
	// From MSgDrawableAdapter
	TInt GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr);
	// From MSgImage_Chunk
	const RChunk& DataChunk() const;
	TInt DataOffset() const;
	TInt DataStride() const;
private:
	inline XSgImageImpl_SurfaceManager(XSgDriverImpl& aDriverImpl);
	inline XSgImageImpl_SurfaceManager(XSgDriverImpl& aDriverImpl, const TSgDrawableId& aId);
	inline XSgImageImpl_SurfaceManager(const XSgImageImpl_SurfaceManager& aImage, TUint32 aFlags);
	TInt Construct(const TSgImageInfo& aInfo, TBool aIsCached, const TAny* aDataAddress, TInt aDataStride);
	TInt Construct(const TSgDrawableId& aId);
	};


#include "sgimageimpl.inl"


#endif // SGIMAGEIMPL_H
