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
//

/**
@file
@publishedPartner
@prototype

*/

#include "surfacemanager.h"
#include <e32base.h>
#include "surfacemanagerdriver.h"


/**
Default constructor
*/
EXPORT_C RSurfaceManager::RSurfaceManager()
	{
	__ASSERT_COMPILE(sizeof(RSurfaceManagerDriver) <= sizeof(iDriverBuf));
	new(iDriverBuf) RSurfaceManagerDriver();
	}

/**
Opens a connection to the Surface Manager.

The connection needs to be closed by calling Close() when it is no longer required.
@return Returns KErrNone if successful, otherwise a system wide error code.
*/
EXPORT_C TInt RSurfaceManager::Open()
	{
	TInt err = User::LoadLogicalDevice( RSurfaceManagerDriver::Name() );
	if(! ((KErrNone == err) || (KErrAlreadyExists == err)))
		{
		return err;
		}
	return Driver().Open();
	}

/**
Closes the connection to the Surface Manager.

Cleanup is performed by the Surface Manager when a client process exits or
closes its last connection. It is not guaranteed that the resources held by the
Surface Manager are going to be released synchronously.
*/
EXPORT_C void RSurfaceManager::Close()
	{
	Driver().Close();
	}

/**
Returns information specific to the Surface Manager implementation.
@param aAttrib Attribute to retrieve
@param aValue Output parameter set to the value for the specified attribute
@return KErrNone if successful, KErrNotReady if the Surface Manager hasn't been
opened, KErrArgument if the attribute UID is not recognized, otherwise a system
wide error code.
*/
EXPORT_C TInt RSurfaceManager::GetSurfaceManagerAttrib(TSurfaceManagerAttrib aAttrib, TInt& aValue)
	{
	return Driver().GetSurfaceManagerAttrib(aAttrib, aValue);
	}

/**
Opens a surface.

If the surface is already open in this process, this call increments the
reference count for the surface for this process. If the surface hasn't been opened
in this process, it opens the surface in this process and sets the reference count
to 1 for this process.

A surface will be deleted when all its reference counts are 0.
@param aSurfaceId  The surface id originally returned when the surface was created.
@pre The surface id is for an existing surface.
@post The surface is open in this process.
@return KErrNone if successful otherwise a system wide error code.
*/
EXPORT_C TInt RSurfaceManager::OpenSurface(const TSurfaceId& aSurfaceId)
	{
	return Driver().OpenSurface(aSurfaceId);
	}

/**
Creates a surface and returns its global unique identifier.

Depending on the implementation, this method will allocate the surface in a new
shared chunk or in external memory. Surfaces created in external memory cannot
be mapped (see TSurfaceInfoV01::iMappable for more information). If the creation
succeeds, the surface will be opened with a reference count of 1 in the calling
process.

The Surface Manager implementation should validate at least the following:
	-	The alignment is 1, 2, 4, 8, 16, 32, 64 or TPageAlignment::EPageAligned.
	-	The offset to first buffer is correctly aligned.
	-	The width and height are both greater than zero.
	-	There is at least one buffer.
	-	The calculated chunk size isn't so big that it will exceed a signed int.
	-	The caching attribute is valid
	-	If the offset between the start of one buffer and the next is specified
		(not zero), it must be correctly aligned and at least as big as the 
		buffer size calculated from the height and stride.
	-	A surface hint key of zero is allowed when creating a surface, it is ignored

The Surface Manager implementation will treat iStride, iOffsetToFirstBuffer, and
iOffsetBetweenBuffers as minimum values as opposed to required values. The caller
of this method must not rely on the passed values as it is not guaranteed that
they remain the same after surface creation (SurfaceInfo() and GetBufferOffset()
should be used instead).

If iOffsetBetweenBuffers is zero, the Surface Manager implementation will layout
buffer at its own discretion. Although, a buffer's offset must remain constant
during the surface's lifetime.

If iMappable is EFalse the surface won't be mappable, hence calls to MapSurface()
will fail.

@param aReqs Input parameter, specifies attributes of the surface required.
@param aSurfaceId Output parameter, the surface ID, set if the call succeeds.
@pre The Surface Manager has been successfully opened.
@post The surface is created and opened in the creating process.
@return KErrNone if it succeeds, KErrArgument if the surface attributes are incorrect,
KErrNoMemory if the surface cannot be created due to out of memory, KErrOverflow if
the chunk limit has been exceeded in the moving memory model, otherwise a standard
Symbian error code.
*/
EXPORT_C TInt RSurfaceManager::CreateSurface(const TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId)
	{
	return Driver().CreateSurface(aReqs,aSurfaceId);
	}

/**
Creates a surface in an existing shared chunk specified by the caller and returns
its global unique identifier.

If the creation succeeds, the surface will be opened with a reference count of 1 in the calling
process.

The Surface Manager implementation should validate at least the following:
	-	The alignment is 1, 2, 4, 8, 16, 32, 64 or TPageAlignment::EPageAligned.
	-	The offset to first buffer is correctly aligned.
	-	The width and height are both greater than zero.
	-	There is at least one buffer.
	-	The calculated chunk size isn't so big that it will exceed a signed int.
	-	The caching attribute is valid
	-	If the offset between the start of one buffer and the next is specified
		(not zero), it must be correctly aligned and at least as big as the 
		buffer size calculated from the height and stride.
	-	A surface hint key of zero is allowed when creating a surface, it is ignored

If iOffsetBetweenBuffers is zero, the Surface Manager implementation will layout
buffer at its own discretion. Although, a buffer's offset must remain constant
during the surface's lifetime.

The attributes iContiguous and iCacheAttribute are ignored for existing chunks.

Also, note that surface manager will not do any rounding for offset to first
buffer and offset between buffers. It is up to the user, to give exact aligned
values for this attributes, otherwise surface creation will fail.

If iMappable is EFalse the surface won't be mappable, hence calls to MapSurface()
will fail.

@param aReqs Input parameter, specifies attributes of the surface required.
@param aSurfaceId Output parameter, the surface ID, set if the call succeeds.
@param aChunkHandle Handle of the existing shared chunk.
@pre The Surface Manager has been successfully opened.
@pre A valid shared chunk exists. The shared chunk type should be Shared Kernel
multiple and should be right the size, i.e. the size should be calculated as
Offset to first buffer + (the number of buffers * ((stride * height *
pixel depth in bytes) rounded up to the specified alignment) all rounded to a page size
@post The Surface Manager has opened the chunk.
@post The surface is created and opened in the creating process.
@return KErrNone if it succeeds, KErrArgument if the surface attributes are incorrect. 
KErrNotReady if the handle is of an invalid shared chunk memory
*/ 
EXPORT_C TInt RSurfaceManager::CreateSurface(const TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId, const RChunk& aChunkHandle)
	{
	return Driver().CreateSurface(aReqs,aSurfaceId,aChunkHandle);
	}

/**
Closes the surface. Decrements the reference count for the surface for the calling
process.

If the surface has other owners, it will not be deleted from memory. If this is
the last process to close the surface, the surface will be deleted and the
surface ID will become invalid. Resources associated with the surface are not
guaranteed to be released synchronously.
@param aSurfaceId The surface identifier originally returned when the surface was created.
@pre The surface is open.
@post The surface is closed.
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a surface,
KErrAccessDenied if the surface is not open in the current process, otherwise a system wide
error code.
*/
EXPORT_C TInt RSurfaceManager::CloseSurface(const TSurfaceId& aSurfaceId)
	{
	return Driver().CloseSurface(aSurfaceId);
	}

/**
Maps the surface into the current client process address space.

The actual memory will remain mapped into the calling process for as long as
the RChunk handle aChunk is open.

Whether or not a surface is mappable is determined at creation time, see 
TSurfaceCreationAttributes::iMappable.

The address of the pixel data in buffer N is chunk.Base() + GetBufferOffset(N).

Surfaces created with existing shared chunks will get the same chunk handle.

The RChunk handle is owned by the calling thread, so will need to be duplicated
if passed to other threads in the process.
@param aSurfaceId The surface identifier originally returned when the surface was created.
@param aHandle Output parameter, handle to the implementation specific Shared Chunk.
@pre The surface is open.
@post The surface memory will be mapped into the calling process's address space.
The surface is mapped.
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a
surface, KErrAccessDenied if the surface is not open in the current process,
KErrNotSupported if the surface is not mappable, KErrOverflow if the chunk limit has been
exceeded in the moving memory model, otherwise a system wide error code.
@see RChunk
@see RHandleBase::Duplicate
@see TSurfaceCreationAttributes::iMappable
*/
EXPORT_C TInt RSurfaceManager::MapSurface(const TSurfaceId& aSurfaceId, RChunk& aHandle)
	{
	return Driver().MapSurface(aSurfaceId,aHandle);
	}

/**
Returns information about a particular surface identified by its surface ID.
@param aSurfaceId The surface identifier originally returned when the surface
was created.
@param aInfo TInfoBuf to receive the information about the surface.
@pre The surface is open in the calling process.
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a surface,
KErrAccessDenied if the surface is not open in the current process, otherwise a system wide
error code.
*/
EXPORT_C TInt RSurfaceManager::SurfaceInfo(const TSurfaceId& aSurfaceId, TInfoBuf& aInfo)
	{
	return Driver().SurfaceInfo(aSurfaceId,aInfo);
	}

/**
This function ensures the memory is updated consistently before and/or after
triggering non CPU hardware access. Also ensures the CPU cache and the physical
memory are in a consistent state before and after non CPU hardware or DMA access
to the physical memory.
@param aSurfaceId The surface identifier originally returned when the surface
was created.
@param aBuffer The buffer number indexed from 0 within the surface whose memory
region is to be flushed. 
@param aOperation Specifies the sync operation as before non CPU hardware reads
or before non CPU hardware writes or after non CPU hardware writes between
physical memory of the surface and the cache contents.
@pre The surface is open in the calling process.
@post The surface buffer memory will be synchronized properly with cache contents.
@return KErrNone if successful, KErrArgument if the surface ID is invalid or
buffer number is invalid, KErrAccessDenied if the surface is not open in this
process, otherwise a system wide error code.
*/ 
EXPORT_C TInt RSurfaceManager::SynchronizeCache(const TSurfaceId& aSurfaceId, TInt aBuffer, TSyncOperation aOperation)
	{
	return Driver().SynchronizeCache(aSurfaceId,aBuffer,aOperation);
	}

/**
Get the surface hint value for the given surface ID and hint pair key.
@param aSurfaceId  The surface identifier originally returned when the surface
was created.
@param aHint The hint value for the requested hint pair key.
@pre The surface is open in the calling process.
@pre Hint key should be a key for a hint set for this surface.
@post The hint value will be updated in the hint pair.
@return KErrNone if successful, KErrArgument if the surface ID is invalid or
invalid hint pair key used, KErrAccessDenied if the surface is not open in the
current process, otherwise a system wide error code.
*/ 
EXPORT_C TInt RSurfaceManager::GetSurfaceHint(const TSurfaceId& aSurfaceId, THintPair& aHint)
	{
	return Driver().GetSurfaceHint(aSurfaceId,aHint);
	}

/**
Set the surface hint value for the surface ID.
@param aSurfaceId The surface identifier originally returned when the surface
was created.
@param aHint The value of the hint pair to set.
@pre The surface is open in the calling process.
@pre The hint key should be a key for a hint set for this surface.
@pre Only mutable hints can be updated.
@post The hint value will be updated in the surface hint pair.
@return KErrNone if successful, KErrArgument if the surface ID is invalid or if invalid
hint key used, KErrAccessDenied if the hint pair is immutable or the surface is not open
in the current process, otherwise a system wide error code.
*/
EXPORT_C TInt RSurfaceManager::SetSurfaceHint(const TSurfaceId& aSurfaceId, const THintPair& aHint)
	{
	return Driver().SetSurfaceHint(aSurfaceId,aHint);
	}

/**
Adds a new surface hint to the surface.

This function will fail if the surface already has its maximum number of hints 
or if the hint key is a duplicate or invalid.
@param aSurfaceId The surface identifier originally returned when the surface
was created.
@param aHint The value of the hint pair to add.
@pre The surface is open in the calling process.
@pre At least one free space to add a hint pair.
@pre The new hint key should be non zero and unique for this surface.
@post New hint pair will be added in the surface.
@return Returns KErrNone if successful, KErrArgument if the surface ID is invalid or the
hint pair has invalid key UID, KErrAccessDenied if the surface is not open in the current
process, KErrAlreadyExists if duplicate hint key used, KErrOverflow if no space to add new
pair, otherwise a system wide error code.
*/
EXPORT_C TInt RSurfaceManager::AddSurfaceHint(const TSurfaceId& aSurfaceId, const THintPair& aHint)
	{
	return Driver().AddSurfaceHint(aSurfaceId,aHint);
	}

/**
Get the offset of the specified buffer from the base address of the underlying
chunk.

To obtain the address of the buffer, the offset returned must be added onto the
base address of the RChunk returned in a call to MapSurface(). Note that
buffer offsets are immutable during the lifetime of the surface.
@param aSurfaceId The surface identifier originally returned when the surface
was created.
@param aBuffer The buffer for which the offset is requested. Indexed from 0.
@param aOffset Output parameter set to the offset within the chunk.
@pre The surface is open in the calling process.
@return KErrNone if successful, KErrArgument if aSurfaceId or aBuffer are invalid,
KErrAccessDenied if the surface is not open in the current process, KErrNotSupported if
the surface is not mappable, otherwise a system wide error code.
*/
EXPORT_C TInt RSurfaceManager::GetBufferOffset(const TSurfaceId& aSurfaceId, TInt aBuffer, TInt& aOffset)
	{
	return Driver().GetBufferOffset(aSurfaceId, aBuffer, aOffset);
	}

/**
Get a reference of the implementation of the surface manager.
@return reference of the implementation of the surface manager
*/
inline RSurfaceManagerDriver& RSurfaceManager::Driver()
	{
	return reinterpret_cast<RSurfaceManagerDriver&>(*iDriverBuf);
	}

