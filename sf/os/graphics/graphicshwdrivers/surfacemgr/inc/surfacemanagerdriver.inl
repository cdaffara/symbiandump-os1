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
// Surface Manager driver
// 
//

/**
@file
@internalComponent
@prototype
*/


/**
The driver's name
@return The name of the driver
@internalComponent
*/
inline const TDesC& RSurfaceManagerDriver::Name()
	{
	_LIT(KSurfaceManagerName, "surfacemanagerdriver");
	return KSurfaceManagerName;
	}

/**
The driver's version
@return The version number of the driver
@internalComponent
*/
inline TVersion RSurfaceManagerDriver::VersionRequired()
	{
	const TInt KMajorVersionNumber=1;
	const TInt KMinorVersionNumber=0;
	const TInt KBuildVersionNumber=KE32BuildVersionNumber;
	return TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	}


#ifndef __KERNEL_MODE__


inline TInt RSurfaceManagerDriver::GetSurfaceManagerAttrib(RSurfaceManager::TSurfaceManagerAttrib aAttrib, TInt& aValue)
	{return DoControl(RSurfaceManagerDriver::EControlGetSurfaceManagerAttrib, (TAny*)&aAttrib, (TAny*)&aValue);}


/**
Opens a surface. If the surface is already open in this process, this call increments the reference count for the surface for this process.
If the surface hasn't been opened in this process, it opens the surface in this process and sets the reference count to 1 for this process.
A surface will be deleted when all its reference counts are 0.
@param aSurfaceId  The surface id originally returned when the surface was created.
@pre The surface id is for an existing surface.
@post The surface is open in this process.
@return KErrNone if successful otherwise a system wide error code.
*/
inline TInt RSurfaceManagerDriver::OpenSurface(const TSurfaceId& aSurfaceId)
	{return DoControl(RSurfaceManagerDriver::EControlOpenSurface, (TAny*)&aSurfaceId);}
	


/**
Creates a surface and allocates the memory for the surface.
It does not map the surface memory into this process.
Sets the surface ID, aSurfaceId, to a globally unique identifier for the surface.
The most significant byte of the 4th TUint in the TSurfaceId will be set to a value to identify the surface type.
The surface will be open with a reference count of 1 in this process after a successful call to CreateSurface.

The creation attributes are validated as follows:
		The alignment is 1,2,4, 8,16,32,64 or TPageAlignment::EPageAligned.
		The offset to first buffer is correctly aligned.
		The width and height are both greater than zero.
		There is at least one buffer.
		The calculated chunk size isn't so big that it will exceed a signed int.
		The caching attribute is valid
		If the offset between the start of one buffer and the next is specified, it must be correctly aligned and at least as big as the buffer size calculated from the height and stride.
		A surface hint key of zero is allowed when creating a surface, it is ignored

If offset to first buffer or offset between buffers is not aligned properly, then surface manager will do the alignment, based on surface is CPU cached or not.
	-	If surface is CPU cached it will be aligned with minimum of the specified alignment 32(for alignment 1, 2, 4, 8, 16, 32) or 64.
	-	If surface is not CPU cached it will be aligned based on creation attribute's alignment.
For page alignment it will be rounded to page size.

@param aReqs  Input parameter, specifies attributes of the surface required.
@param aSurfaceId  Output parameter, the surface ID, set if the call succeeds.
@pre The device driver is loaded and a channel to it has been opened.
@post The surface is created and opened in the creating process.
@return KErrNone if it succeeds, KErrArgument if the surface attributes are incorrect,
KErrNoMemory if the surface cannot be created due to out of memory,
KErrOverflow if the chunk limit has been exceeded in the moving memory model, otherwise a standard Symbian error code.
*/
inline TInt RSurfaceManagerDriver::CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId)
	{return DoControl(RSurfaceManagerDriver::EControlCreateSurface, (TAny*)&aReqs, (TAny*)&aSurfaceId);}
	


/**
Creates a surface in the shared chunk memory specified by the user.
Sets a new surface ID aSurfaceId to a globally unique identifier for the surface.  
The most significant byte of the 4th TUint in the TSurfaceId will be set to a value to identify 
the surface type.  The surface will be open with a reference count of 1 in this process after a 
successful call to CreateSurface.
The creation attributes are validated as follows:
		The alignment is 1,2,4, 8,16,32,64 or page alignment.
		The offset to first buffer is correctly aligned.
		The width and height are both greater than zero.
		There is at least one buffer.
		The chunk should be a valid shared chunk of the necessary size.
		If the offset between the start of one buffer and the next is specified, it must be correctly 
		aligned and at least as big as the buffer size calculated from the height and stride.
		A surface hint key of zero is allowed when creating a surface, it is ignored

The offset can be 0 and that means the surface manager will calculate the offset
The TSurfaceCreationAttributes members iContiguous and iCacheAttribute will not be used for already existing chunks
Also, note that surface manager will not do any rounding for offset to first buffer and offset between buffer. 
Surface manager will expect exact aligned values for this buffer attributes, otherwise surface creation will fail.

@param aReqs  Input parameter, specifies attributes of the surface required.
@param aSurfaceId  Output parameter, the surface ID, set if the call succeeds.
@param aChunkHandle  Handle of the existing shared chunk.

Preconditions:
@pre The device driver is loaded and a channel has been opened. A valid shared chunk should be created and 
big enough for the requested size. The shared chunk type should be Shared  Kernel multiple and should be right the size,
ie., the size should be calculated as Offset to first buffer + (the number of buffers * ((stride * height * pixel depth in bytes) rounded up to the specified alignment) all rounded to a page size
@post The surface manager opens the chunk.
@return KErrNone if it succeeds, KErrArgument if the surface attributes are incorrect. 
KErrBadHandle If the handle is of an invalid shared chunk memory
*/ 
inline TInt RSurfaceManagerDriver::CreateSurface(const RSurfaceManager::TSurfaceCreationAttributesBuf& aReqs, TSurfaceId& aSurfaceId, const RChunk& aChunkHandle)
	{
	TDeviceParam create((TAny*)&aSurfaceId, (TAny*)&aReqs);
	return DoControl(RSurfaceManagerDriver::EControlCreateSurfaceEx, (TAny*)&create, (TAny*)aChunkHandle.Handle());
	}


/**
Closes the surface. Decrements the reference count for the surface for the calling process.
If the surface has other owners, it will not be deleted from memory.
If this is the last process to close the surface, the surface will be deleted and the surface ID will become invalid.
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@pre The surface is open.
@post The surface is closed.
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a surface,
KErrAccessDenied if the surface is not open in the current process, otherwise a system wide
error code.
*/
inline TInt RSurfaceManagerDriver::CloseSurface(const TSurfaceId& aSurfaceId)
	{return DoControl(RSurfaceManagerDriver::EControlCloseSurface, (TAny*)&aSurfaceId);}



/**
Maps the surface into the current client process address space.
The actual memory will remain mapped into the calling process for as long as the RChunk handle aHandle is open.
The RChunk handle is owned by the calling thread, so will need to be duplicated if passed to other threads in the process.

@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aHandle  Output parameter, handle to the implementation specific Shared Chunk.
@pre The surface is open.
@post The surface memory will be mapped into the calling porcesses address space.
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a
surface, KErrAccessDenied if the surface is not open in the current process,
KErrNotSupported if the surface is not mappable, KErrOverflow if the chunk limit has been
exceeded in the moving memory model, otherwise a system wide error code.
@see RChunk
@see RHandleBase::Duplicate
*/
inline TInt RSurfaceManagerDriver::MapSurface(const TSurfaceId& aSurfaceId, RChunk& aHandle)
	{return aHandle.SetReturnedHandle(DoControl(RSurfaceManagerDriver::EControlAccessSurfaceData, (TAny*)&aSurfaceId));}



/**
Returns information about a particular surface identified by its surface ID.
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aInfo  TInfoBuf to receive the information about the surface.
@pre The surface is open in the calling process.
@return KErrNone if successful, KErrArgument if the surface ID does not refer to a surface,
KErrAccessDenied if the surface is not open in the current process, otherwise a system wide
error code.
*/
inline TInt RSurfaceManagerDriver::SurfaceInfo(const TSurfaceId& aSurfaceId, RSurfaceManager::TInfoBuf& aInfo)
	{return DoControl(RSurfaceManagerDriver::EControlSurfaceInfo, (TAny*)&aSurfaceId, (TAny*)&aInfo);}



/**
Opens a channel to the surface manager logical device driver. 
@pre The logical device driver has been loaded
@return Returns KErrNone if successful, KErrNotFound if the driver hasn't been loaded, otherwise a system wide error code.
@see User::LoadLogicalDevice
*/
inline TInt RSurfaceManagerDriver::Open()
	{return DoCreate(Name(),VersionRequired(),KNullUnit,NULL,NULL,EOwnerProcess);}



/**
This function ensures the memory is updated consistently before and/or after triggering non CPU hardware access. 
Also ensures the CPU cache and the physical memory are in a consistent state before and after non CPU hardware or DMA access
to the physical memory.
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aBuffer  The buffer number indexed from 0 within the surface whose memory region is to be flushed. 
@param aOperation  Specifies the sync operation as before non CPU hardware reads or before non CPU hardware writes or after non CPU hardware
writes between physical memory of the surface and the cache contents.
@pre The surface is open in the calling process.
@post The surface buffer memory will be synchronized properly with cache contents.
@return KErrNone if successful, KErrArgument if the surface ID is invalid or
buffer number is invalid, KErrAccessDenied if the surface is not open in this
process, otherwise a system wide error code.
*/ 
inline TInt RSurfaceManagerDriver::SynchronizeCache(const TSurfaceId& aSurfaceId, TInt aBuffer, RSurfaceManager::TSyncOperation aOperation)
	{
	TDeviceParam buff((TAny*)&aSurfaceId, (TAny*)aBuffer);
	return DoControl(RSurfaceManagerDriver::EControlSynchronizeCache, (TAny*)&buff, (TAny*)aOperation);
	}

/**
Get the surface hint value for the given surface ID and hint pair key.
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aHint  The hint value for the requested hint pair key.
@pre The surface is open in the calling process.
@pre Hint key should be a key for a hint set for this surface.
@post The hint value will be updated in the hint pair.
@return KErrNone if successful, KErrArgument if the surface ID is invalid or
invalid hint pair key used, KErrAccessDenied if the surface is not open in the
current process, otherwise a system wide error code.
*/ 
inline TInt RSurfaceManagerDriver::GetSurfaceHint(const TSurfaceId& aSurfaceId, RSurfaceManager::THintPair& aHint)
	{return DoControl(RSurfaceManagerDriver::EControlGetSurfaceHint, (TAny*)&aSurfaceId, (TAny*)&aHint);}


/**
Set the surface hint value for the surface Id.
Parameters:
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aHint  The value of the hint pair to set.
@pre The surface is open in the calling process.
@pre The Hint key should be a key for a hint set for this surface.
@pre Only mutable hints can be updated.
@post The hint value will be updated in the surface hint pair.
@return KErrNone if successful, KErrArgument if the surface ID is invalid or if invalid
hint key used, KErrAccessDenied if the hint pair is immutable or the surface is not open
in the current process, otherwise a system wide error code.
*/
inline TInt RSurfaceManagerDriver::SetSurfaceHint(const TSurfaceId& aSurfaceId, const RSurfaceManager::THintPair& aHint)
	{return DoControl(RSurfaceManagerDriver::EControlSetSurfaceHint, (TAny*)&aSurfaceId, (TAny*)&aHint);}


/**
Adds a new surface hint to the surface. This function will fail if the surface already has its maximum number of hints 
or if the hint key is a duplicate or invalid.
Parameters:
@param aSurfaceId  The surface identifier originally returned when the surface was created.
@param aHint  The value of the hint pair to add.
@pre The surface is open in the calling process.
@pre Atleast one free space to add a hint pair.
@pre The new hint key should be unique.
@post New hint pair will be added in the surface.
@return Returns KErrNone if successful, KErrArgument if the surface ID is invalid or the
hint pair has invalid key UID, KErrAccessDenied if the surface is not open in the current
process, KErrAlreadyExists if duplicate hint key used, KErrOverflow if no space to add new
pair, otherwise a system wide error code.
*/
inline TInt RSurfaceManagerDriver::AddSurfaceHint(const TSurfaceId& aSurfaceId, const RSurfaceManager::THintPair& aHint)
	{return DoControl(RSurfaceManagerDriver::EControlAddSurfaceHint, (TAny*)&aSurfaceId, (TAny*)&aHint);}

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
inline TInt RSurfaceManagerDriver::GetBufferOffset(const TSurfaceId& aSurfaceId, TInt aBuffer, TInt& aOffset)
	{
	TDeviceParam buff((TAny*)&aSurfaceId, (TAny*)aBuffer);
	return DoControl(RSurfaceManagerDriver::EControlGetBufferOffset, (TAny*)&buff, (TAny*)&aOffset);
	}

#endif //__KERNEL_MODE__

