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

#ifndef SGIMAGEADAPTER_H
#define SGIMAGEADAPTER_H

#include <graphics/sgresourceadapter.h>
#include <graphics/sgimage.h>


/**
@publishedPartner
@prototype
@deprecated

This interface must be implemented by all the user-side objects in the adaptation
layer of the Graphics subsystem which are referenced by instances of RSgImage.
The interface between the user-side and the kernel-side parts of the adaptation
layer is outside the scope of the specification of the Graphics Resource API.

@see RSgImage
*/
class MSgImageAdapter: public MSgDrawableAdapter
	{
public:
	/**
	@publishedPartner
	@prototype
	@deprecated

	Retrieves the values of the attributes of the image represented by this adapter
	object.

	@param aInfo On input, the globally unique identifiers of the user-defined
	       attributes to be retrieved from the image, if any. On return, the values
	       of the attributes of the image and the values of the selected user-defined
	       attributes.
	@pre If aInfo.iUserAttributes is not null then it points to an array of
	     aInfo.iUserAttributeCount elements with globally unique identifiers
	     corresponding to user-defined attributes attached to the image.
	@return KErrNone if successful.
	@return KErrNotFound if any of the user-defined attributes to be retrieved from
	        the image cannot be found.
	@see RSgImage::GetInfo()
	*/
	virtual TInt GetInfo(TSgImageInfo& aInfo) const = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Temporarily makes the pixel data of the image represented by this adapter object
	accessible for reading by the CPU.

	@param aDataAddress On return, the base address of the pixel data in the address
	       space of the calling process.
	@param aDataStride On return, the number of bytes between rows of the pixel data.
	@pre The image is not mapped yet.
	@pre The image was created with CPU access ESgCpuAccessReadOnly or
	     ESgCpuAccessReadWrite.
	@pre The image was created by the calling process.
	@post The pixel data of the image is directly accessible in the address space
	      of the calling process for reading only, until Unmap() is called.
	@return KErrNone if successful.
	@return KErrInUse if the image is already mapped or in exclusive use by the GPU.
	@return KErrAccessDenied if the image was not created with CPU access
	        ESgCpuAccessReadOnly or ESgCpuAccessReadWrite.
	@return KErrPermissionDenied if the image was created by another process.
	@return KErrNoMemory if there is not enough system memory.
	@see RSgImage::MapReadOnly()
	*/
	virtual TInt MapReadOnly(const TAny*& aDataAddress, TInt& aDataStride) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Temporarily makes the pixel data of the image represented by this adapter object
	accessible for writing by the CPU.

	@param aDataAddress On return, the base address of the pixel data in the address
	       space of the calling process.
	@param aDataStride On return, the number of bytes between rows of the pixel data.
	@pre The image is not mapped yet.
	@pre The image was created with CPU access ESgCpuAccessWriteOnly or
	     ESgCpuAccessReadWrite.
	@pre The image was created by the calling process.
	@post The pixel data of the image is directly accessible in the address space
	      of the calling process for writing only, until Unmap() is called.
	@return KErrNone if successful.
	@return KErrInUse if the image is already mapped or in exclusive use by the GPU.
	@return KErrAccessDenied if the image was not created with CPU access
	        ESgCpuAccessWriteOnly or ESgCpuAccessReadWrite.
	@return KErrPermissionDenied if the image was created by another process.
	@return KErrNoMemory if there is not enough system memory.
	@see RSgImage::MapWriteOnly()
	*/
	virtual TInt MapWriteOnly(TAny*& aDataAddress, TInt& aDataStride) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Temporarily makes the pixel data of the image represented by this adapter object
	accessible for reading and writing by the CPU.

	@param aDataAddress On return, the base address of the pixel data in the address
	       space of the calling process.
	@param aDataStride On return, the number of bytes between rows of the pixel data.
	@pre The image is not mapped yet.
	@pre The image was created with CPU access ESgCpuAccessReadWrite.
	@pre The image was created by the calling process.
	@post The pixel data of the image is directly accessible in the address space
	      of the calling process for reading and writing, until Unmap() is called.
	@return KErrNone if successful.
	@return KErrInUse if the image is already mapped or in exclusive use by the GPU.
	@return KErrAccessDenied if the image was not created with CPU access
	        ESgCpuAccessReadWrite.
	@return KErrPermissionDenied if the image was created by another process.
	@return KErrNoMemory if there is not enough system memory.
	@see RSgImage::MapReadWrite()
	*/
	virtual TInt MapReadWrite(TAny*& aDataAddress, TInt& aDataStride) = 0;
	/**
	@publishedPartner
	@prototype
	@deprecated

	Makes the pixel data of the image represented by this adapter object no longer
	accessible by the CPU.

	@pre The image is mapped for CPU access by a previous call to MapReadOnly(),
	     MapWriteOnly() or MapReadWrite().
	@post The GPU is guaranteed to be able to get access to the image.
	@post The address range in the calling process used for the mapping is no longer
	      valid.
	@return KErrNone if successful.
	@return KErrGeneral if the image was not mapped for CPU access.
	@see RSgImage::Unmap()
	*/
	virtual TInt Unmap() = 0;
	};


#endif // SGIMAGEADAPTER_H
