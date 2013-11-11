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
 @internalTechnology
 @prototype
*/

#ifndef SGIMAGE_SW_H
#define SGIMAGE_SW_H

#include <e32def.h>


/**
@internalTechnology
@prototype

This interface allows direct access to the pixel data of an image from user-side
code. It is intended for use by software implementations of functions in the
Graphics subsystem.

This interface is only supported if the image is or can be stored in system memory.
This is always the case on platforms without hardware acceleration and also on
platforms with Unified Memory Architecture (UMA) hardware accelerators.
*/
class MSgImage_Sw
	{
public:
	enum { EInterfaceUid = 0x102858F0 };
	/**
	@internalTechnology
	@prototype

	Retrieves the base address of the pixel data in system memory.

	@pre In builds with SYMBIAN_GRAPHICS_AUTOFLUSH_CACHE, the image has been prepared
	     for CPU access to its pixel data by a previous call to BeginDataAccess().
	@post None.
	@return The base address of the pixel data in system memory.
	@panic SGRES-ADAPTER 4 in debug builds with SYMBIAN_GRAPHICS_AUTOFLUSH_CACHE,
	       if the image has not been prepared for CPU access to its pixel data by
	       a previous call to BeginDataAccess().
	*/
	virtual TAny* DataAddress() const = 0;
	/**
	@internalTechnology
	@prototype

	Retrieves the number of bytes between rows of the pixel data in system memory.

	@pre None.
	@post None.
	@return The number of bytes between rows of the pixel data in system memory.
	*/
	virtual TInt DataStride() const = 0;
	/**
	@internalTechnology
	@prototype

	Marks the beginning of CPU access to the pixel data. This function must be
	called before DataAddress() in builds with SYMBIAN_GRAPHICS_AUTOFLUSH_CACHE
	and prepares the image for CPU access to its pixel data in system memory. Calls
	to BeginDataAccess() must be coupled with subsequent calls to EndDataAccess().

	@pre aCpuAccess is not ESgCpuAccessNone.
	@pre The image has not been mapped for CPU access to its pixel data by a
	     call to RSgImage::MapReadOnly(), RSgImage::MapWriteOnly() or
	     RSgImage::MapReadWrite().
	@pre The image has not been prepared for CPU access to its pixel data by a
	     call to BeginDataAccess().
	@post The image is prepared for CPU access to its pixel data.
	@param aCpuAccess Whether the pixel data is going to be only read, only
	       written or read and written by the CPU until the corresponding call
	       to EndDataAccess().
	@return KErrNone if successful.
	@return KErrArgument if aCpuAccess is ESgCpuAccessNone.
	@return KErrInUse if the image was already mapped or prepared for CPU access
	        to its pixel data.
	@return KErrNoMemory if there is not enough system memory.
	*/
	virtual TInt BeginDataAccess(TSgCpuAccess aCpuAccess) = 0;
	/**
	@internalTechnology
	@prototype

	Marks the end of CPU access to the pixel data. This function must be called
	when finished using the values returned by DataAddress() in builds with
	SYMBIAN_GRAPHICS_AUTOFLUSH_CACHE and ensures that, if the CPU has modified
	the pixel data, any subsequent usage of the image by the GPU will reflect
	its new state. Calls to EndDataAccess() must correspond to prior calls to
	BeginDataAccess().

	@pre The image has been prepared for CPU access to its pixel data by a
	     successful call to BeginDataAccess().
	@post The image is no longer prepared for CPU access to its pixel data.
	@return KErrNone if successful.
	@return KErrGeneral if the image had not been prepared for CPU access to its
	        pixel data by a successful call to BeginDataAccess().
	*/
	virtual TInt EndDataAccess() = 0;
	};


#endif // SGIMAGE_SW_H
