// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Surface Manager API
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef __SURFACEMANAGER_H__
#define __SURFACEMANAGER_H__

#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif

#include <e32cmn.h>
#include <e32ver.h>
#include <pixelformats.h>
#include <graphics/surface.h>

class RSurfaceManagerDriver;

/**
RSurface Manager User API.
*/
class RSurfaceManager
	{
public:

	class THintPair
		{
	public:
		/** UID key number */
		TUid iKey;
		/** Integer key value */
		TInt iValue;
		/** Is the value modifiable */
		TBool iMutable;
	public:
		inline void Set(TUid aKey,TInt aValue,TBool aMutable);
		};
	
	enum TCacheAttribute
		{
		/** CPU cached */
		ECached = 0,
		/** Non CPU cached */
		ENotCached = 1
		};

	class TSurfaceCreationAttributes
		{
	public:
		/** Width and height of the surface in pixels. */
		TSize iSize;
		/** Number of buffers in the surface. */
		TInt iBuffers;
		/** The pixel format. */
		TUidPixelFormat iPixelFormat;
		/** Minimum or required number of bytes between start of one line and
		start of next. */
		TInt iStride;
		/** Minimum or required offset to the first buffer from the base of the
		chunk. Typically this will be set to 0. The value specified for the
		offset must comply with the alignment specified in iAlignment.
		
		If iAlignment is page aligned, this value will be rounded up to a
		multiple of the page size when the surface is created, therefore the
		surface info must be queried for the actual value used. */
		TInt iOffsetToFirstBuffer;
		/** Alignment applied to the base address of each buffer in the surface:
		1, 2, 4, 8 ,16, 32, 64 bytes or EPageAligned. */
		TInt iAlignment;
		/** Require physically contiguous memory. This value will be ignored if
		using a chunk which already exists. */
		TBool iContiguous;
		/** Caching attribute to create chunk memory. This value will be 
		ignored if using a chunk which already exists. */
		TCacheAttribute iCacheAttrib;
		/** Minimum or required offset between the start of one buffer and the
		start of the next one in bytes. When set to 0 the surface manager will
	   	choose how buffers are laid out within the chunk. If it is too small
		and doesn't fit with the alignment, CreateSurface() will return
		KErrArgument. */
		TInt iOffsetBetweenBuffers;
		/** Array of hints which should be associated with the surface. This
		array must not contain duplicate hint keys. */
		THintPair* iSurfaceHints;
		/** Number of hints in the array iSurfaceHints. The number should not
		exceed the maximum number supported by the surface manager, see 
		GetSurfaceManagerAttrib(EMaxNumberOfHints). */
		TInt iHintCount;
		/** Should the surface be mappable. If EFalse any call to MapSurface()
		will fail with KErrNotSupported -- Note, some architectures may not
		support mappable surfaces. */
		TBool iMappable;
	public:
		inline TSurfaceCreationAttributes();
		};

	class TSurfaceInfoV01
		{
	public:
		/** Width and height of the surface in pixels */
		TSize iSize;
		/** Number of buffers in the surface */
		TInt iBuffers;
		/** The pixel format */
		TUidPixelFormat iPixelFormat;
		/** Number of bytes between start of one line and start of next */
		TInt iStride;
		/** Has physically contiguous memory */
		TBool iContiguous;
		/** Specified if the underlying chunk is CPU cached or not */
		TCacheAttribute iCacheAttrib;
		/** ETrue if the surface can be mapped */
		TBool iMappable;
		};
	
	enum TSurfaceManagerAttrib
		{
		/** Maximum number of hints per surface */
		EMaxNumberOfHints = 0x0 
		};

	/** Package buf used to pass information about a surface from the driver */
	typedef TPckgBuf<TSurfaceInfoV01> TInfoBuf;
	/** Package buf used to pass the surface creation attributes to the device driver */
	typedef TPckgBuf<TSurfaceCreationAttributes> TSurfaceCreationAttributesBuf;

	enum TSyncOperation
		{
		/** Synchronize before non CPU hardware reads from the memory, i.e. if the
		buffer is cached and may have been written to by the CPU, this type of
		synchronisation should be used before a peripheral is used to read from the
		buffer's memory */
		ESyncBeforeNonCPURead,
		/** Synchronize before non CPU hardware writes to the memory, i.e. if the
		buffer is cached and may have been written to by the CPU, this type of
		synchronisation should be used before a peripheral is used to write to the
		buffer's memory */
		ESyncBeforeNonCPUWrite,
		/** Synchronize after non CPU hardware writes to the memory, i.e. if the
		buffer is cached, this type of synchronisation should be used after a
		peripheral has been used to write to the buffer's memory */
		ESyncAfterNonCPUWrite
		};

	enum TPageAlignment
		{
		/** Specifies iAlignment is a page alignment */
		EPageAligned = -1
		};
	
public:
#ifndef __KERNEL_MODE__
	IMPORT_C RSurfaceManager();
	IMPORT_C TInt Open();
	IMPORT_C void Close();
	IMPORT_C TInt CreateSurface(const TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId);
	IMPORT_C TInt CreateSurface(const TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId, const RChunk& aChunkHandle);
	IMPORT_C TInt OpenSurface(const TSurfaceId& aSurfaceId);
	IMPORT_C TInt CloseSurface(const TSurfaceId& aSurfaceId);
	IMPORT_C TInt MapSurface(const TSurfaceId& aSurfaceId, RChunk& aHandle);
	IMPORT_C TInt SurfaceInfo(const TSurfaceId& aSurfaceId, TInfoBuf& aInfo);
	IMPORT_C TInt SynchronizeCache(const TSurfaceId& aSurfaceId, TInt aBuffer, TSyncOperation aOperation);
	IMPORT_C TInt GetSurfaceManagerAttrib(TSurfaceManagerAttrib aAttrib, TInt& aValue);
	IMPORT_C TInt GetSurfaceHint(const TSurfaceId& aSurfaceId, THintPair& aHint);
	IMPORT_C TInt SetSurfaceHint(const TSurfaceId& aSurfaceId, const THintPair& aHint);
	IMPORT_C TInt AddSurfaceHint(const TSurfaceId&aSurface, const THintPair& aHint);
	IMPORT_C TInt GetBufferOffset(const TSurfaceId& aSurfaceId, TInt aBuffer, TInt& aOffset);
#endif	//__KERNEL_MODE__	
private:
	inline RSurfaceManagerDriver& Driver();
private:
	TInt32 iDriverBuf[4];
	};

//
// THintPair inline
//

/**
Sets key, value and mutability of the hint.
@param aKeyUid The UID of the key
@param aValue The value of the hint
@param aMutable ETrue if the hint value is mutable
*/
inline void RSurfaceManager::THintPair::Set(TUid aKeyUid,TInt aValue,TBool aMutable)
	{
	iKey = aKeyUid;
	iValue = aValue;
	iMutable = aMutable;
	}

//
// TSurfaceCreationAttributes inline
//

/**
Default constructor, zero initializes all attributes.
*/
inline RSurfaceManager::TSurfaceCreationAttributes::TSurfaceCreationAttributes()
	{
	memclr(this, sizeof(TSurfaceCreationAttributes));
	}

#endif //__SURFACEMANAGER_H__
