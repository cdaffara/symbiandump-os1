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
#include "sgimageadapter.h"


/**
@publishedPartner
@prototype
@deprecated

Default constructor.

@pre None.
@post All the data members of this instance of TSgImageInfo are zero, false or null.
*/
EXPORT_C TSgImageInfo::TSgImageInfo()
	{
	Mem::FillZ(this, sizeof(TSgImageInfo));
	}


/**
@publishedPartner
@prototype
@deprecated

Default constructor.

@pre None.
@post This RSgImage handle is null.
*/
EXPORT_C RSgImage::RSgImage()
	{
	iHandleType = KSgImageTypeUid;
	}


/**
@publishedPartner
@prototype
@deprecated

Creates an image with the specified attributes and, optionally, the specified
initial contents.

@param aInfo The attributes of the image to be created.
@param aDataAddress The base address of the pixel data used to populate the new image.
       If this value is null, the initial contents of the image are undefined.
       If aInfo specifies that the new image is constant, this value must not be null.
@param aDataStride The number of bytes between rows of the pixel data used to populate
       the new image.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImage handle is null.
@pre aInfo is supported.
@post This RSgImage handle references a newly created image with the specified
      attributes and initial contents. The initial reference count for the image
      is one.
@return KErrNone if successful.
@return KErrInUse if this RSgImage handle was not null.
@return KErrArgument if aInfo is invalid.
@return KErrNoInitializationData if aInfo requests a constant image and aDataAddress
        is null.
@return KErrTooBig if the size specified in aInfo is too large.
@return KErrNotSupported if aInfo is not supported.
@return KErrNoMemory if there is not enough system memory.
@return KErrNoSpecializedMemory if there is not enough specialised graphics memory.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImage::Create(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	TInt err = gPls.iDriver->CreateImage(aInfo, aDataAddress, aDataStride, iImpl);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Creates an image with the specified attributes and initial contents copied from an
existing image. The size and the pixel format of the image to be created must be the
same as the size and the pixel format of the existing image.

@param aInfo The attributes of the image to be created.
@param aImage A handle to the existing image to be copied.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImage handle is null.
@pre aInfo is supported.
@pre aImage is a valid and not null handle.
@pre The size and the pixel format specified in aInfo must be the same as the size
     and the pixel format of aImage.
@post This RSgImage handle references a newly created image with the specified
      attributes and initial contents. The initial reference count for the image
      is one.
@return KErrNone if successful.
@return KErrInUse if this RSgImage handle was not null.
@return KErrArgument if aInfo is invalid or if aImage is a null handle.
@return KErrNotSupported if aInfo is not supported or if the size and the pixel format
        specified in aInfo are not the same as the size and the pixel format of aImage.
@return KErrNoMemory if there is not enough system memory.
@return KErrNoSpecializedMemory if there is not enough specialised graphics memory.
@panic SGRES 3 in debug builds if aImage is an invalid handle.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImage::Create(const TSgImageInfo& aInfo, const RSgImage& aImage)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	TInt err = gPls.iDriver->CreateImage(aInfo, static_cast<MSgImageAdapter*>(aImage.iImpl), iImpl);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Retrieves the values of the attributes of an image. This function can also retrieve
the values of selected user-defined attributes attached to an image by passing in
the globally unique identifiers of the user-defined attributes to be retrieved.

@param aInfo On input, the globally unique identifiers of the user-defined attributes
       to be retrieved from the image, if any. On return, the values of the attributes
       of the image and the values of the selected user-defined attributes.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImage handle is valid and not null.
@pre If aInfo.iUserAttributes is not null then it points to an array of
     aInfo.iUserAttributeCount elements with globally unique identifiers
     corresponding to user-defined attributes attached to the image.
@post None.
@return KErrNone if successful.
@return KErrBadHandle if this RSgImage handle is null.
@return KErrNotFound if any of the user-defined attributes to be retrieved from the
        image cannot be found.
@panic SGRES 3 in debug builds if this RSgImage handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
@see TSgImageInfo
*/
EXPORT_C TInt RSgImage::GetInfo(TSgImageInfo& aInfo) const
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImage(*iImpl), Panic(ESgPanicBadImageHandle));
#endif
	TInt err = static_cast<MSgImageAdapter*>(iImpl)->GetInfo(aInfo);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Temporarily makes the pixel data of an image accessible for reading by the CPU.
Undefined behaviour occurs if the CPU tries to modify the pixel data.

When finished with the pixel data, the caller must end the mapping by calling Unmap()
on this RSgImage handle.

If an image is shared across processes, only the creator process is allowed to map
the image for access to its pixel data.

Depending upon the hardware architecture, whilst an image is mapped the GPU may be
denied access to the pixel data, so this function may cause GPU operations that use
this image to fail if proper synchronisation is not provided by means of the
mechanisms available from the different rendering pipelines or from specialised
synchronisation objects. Likewise, if the GPU is using the image at the time this
function is called, the mapping may fail with KErrInUse. Note that even if operations
do not fail, the results are not guaranteed to be correct unless proper synchronisation
is provided.

@param aDataAddress On return, the base address of the pixel data in the address
       space of the calling process.
@param aDataStride On return, the number of bytes between rows of the pixel data.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImage handle is valid and not null.
@pre The image is not mapped yet.
@pre The image was created with CPU access ESgCpuAccessReadOnly or ESgCpuAccessReadWrite.
@pre The image was created by the calling process.
@post The pixel data of the image is directly accessible in the address space of the
      calling process for reading only, until Unmap() is called.
@return KErrNone if successful.
@return KErrBadHandle if this RSgImage handle is null.
@return KErrInUse if the image is already mapped or in exclusive use by the GPU.
@return KErrAccessDenied if the image was not created with CPU access ESgCpuAccessReadOnly
        or ESgCpuAccessReadWrite.
@return KErrPermissionDenied if the image was created by another process.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 3 in debug builds if this RSgImage handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImage::MapReadOnly(const TAny*& aDataAddress, TInt& aDataStride) const
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImage(*iImpl), Panic(ESgPanicBadImageHandle));
#endif
	TInt err = static_cast<MSgImageAdapter*>(iImpl)->MapReadOnly(aDataAddress, aDataStride);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Temporarily makes the pixel data of an image accessible for writing by the CPU.
Any pre-existing content is discarded, meaning that the mapped memory initially
contains undefined pixel data.

When finished with the pixel data, the caller must end the mapping by calling Unmap()
on this RSgImage handle. The caller is required to write to every pixel of the image
before unmapping.

If an image is shared across processes, only the creator process is allowed to map
the image for access to its pixel data.

Depending upon the hardware architecture, whilst an image is mapped the GPU may be
denied access to the pixel data, so this function may cause GPU operations that use
this image to fail if proper synchronisation is not provided by means of the
mechanisms available from the different rendering pipelines or from specialised
synchronisation objects. Likewise, if the GPU is using the image at the time this
function is called, the mapping may fail with KErrInUse. Note that even if operations
do not fail, the results are not guaranteed to be correct unless proper synchronisation
is provided.

@param aDataAddress On return, the base address of the pixel data in the address
       space of the calling process.
@param aDataStride On return, the number of bytes between rows of the pixel data.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImage handle is valid and not null.
@pre The image is not mapped yet.
@pre The image was created with CPU access ESgCpuAccessWriteOnly or ESgCpuAccessReadWrite.
@pre The image was created by the calling process.
@post The pixel data of the image is directly accessible in the address space of the
      calling process for writing only, until Unmap() is called.
@return KErrNone if successful.
@return KErrBadHandle if this RSgImage handle is null.
@return KErrInUse if the image is already mapped or in exclusive use by the GPU.
@return KErrAccessDenied if the image was not created with CPU access ESgCpuAccessWriteOnly
        or ESgCpuAccessReadWrite.
@return KErrPermissionDenied if the image was created by another process.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 3 in debug builds if this RSgImage handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImage::MapWriteOnly(TAny*& aDataAddress, TInt& aDataStride)
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImage(*iImpl), Panic(ESgPanicBadImageHandle));
#endif
	TInt err = static_cast<MSgImageAdapter*>(iImpl)->MapWriteOnly(aDataAddress, aDataStride);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Temporarily makes the pixel data of an image accessible for reading and writing
by the CPU.

When finished with the pixel data, the caller must end the mapping by calling Unmap()
on this RSgImage handle. Any modification performed by the CPU will be retained upon
unmapping.

If an image is shared across processes, only the creator process is allowed to map
the image for access to its pixel data.

Depending upon the hardware architecture, whilst an image is mapped the GPU may be
denied access to the pixel data, so this function may cause GPU operations that use
this image to fail if proper synchronisation is not provided by means of the
mechanisms available from the different rendering pipelines or from specialised
synchronisation objects. Likewise, if the GPU is using the image at the time this
function is called, the mapping may fail with KErrInUse. Note that even if operations
do not fail, the results are not guaranteed to be correct unless proper synchronisation
is provided.

@param aDataAddress On return, the base address of the pixel data in the address
       space of the calling process.
@param aDataStride On return, the number of bytes between rows of the pixel data.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImage handle is valid and not null.
@pre The image is not mapped yet.
@pre The image was created with CPU access ESgCpuAccessReadWrite.
@pre The image was created by the calling process.
@post The pixel data of the image is directly accessible in the address space of the
      calling process for reading and writing, until Unmap() is called.
@return KErrNone if successful.
@return KErrBadHandle if this RSgImage handle is null.
@return KErrInUse if the image is already mapped or in exclusive use by the GPU.
@return KErrAccessDenied if the image was not created with CPU access
        ESgCpuAccessReadWrite.
@return KErrPermissionDenied if the image was created by another process.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 3 in debug builds if this RSgImage handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImage::MapReadWrite(TAny*& aDataAddress, TInt& aDataStride)
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImage(*iImpl), Panic(ESgPanicBadImageHandle));
#endif
	TInt err = static_cast<MSgImageAdapter*>(iImpl)->MapReadWrite(aDataAddress, aDataStride);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Makes the pixel data of an image no longer accessible by the CPU. If the image was
mapped for writing by the CPU, any written data is retained and any subsequent usage
of the image by the GPU will reflect its new state.

If the last handle to an image in a process is closed while the image is still mapped
by the process then the image will be automatically unmapped.

@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgImage handle is valid and not null.
@pre The image is mapped for CPU access by a previous call to MapReadOnly(),
     MapWriteOnly() or MapReadWrite().
@post The GPU is guaranteed to be able to get access to the image.
@post The address range in the calling process used for the mapping is no longer
      valid. Attempts by the calling process to access any address in this range
      will result in undefined behaviour.
@return KErrNone if successful.
@return KErrBadHandle if this RSgImage handle is null.
@return KErrGeneral if the image was not mapped for CPU access.
@panic SGRES 3 in debug builds if this RSgImage handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImage::Unmap() const
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckImage(*iImpl), Panic(ESgPanicBadImageHandle));
#endif
	TInt err = static_cast<MSgImageAdapter*>(iImpl)->Unmap();
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Retrieves the list of image pixel formats supported on this device for a specified
usage. This function is often called before creating images.

@param aInfo The image attributes. aInfo.iPixelFormat is ignored.
@param aPixelFormats A pointer to an array that on return will contain the supported
       pixel formats. If this parameter is null, then this function will just
       return the number of supported pixel formats in aCount.
@param aCount On input, the number of elements in the array pointed to by aPixelFormats
       if not null, ignored otherwise. On return, the actual number of supported pixel
       formats. If this number is greater than the number of elements in the array,
       then the array will be filled with as many pixel formats as it can hold and the
       function will return KErrOverflow.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre aInfo is valid.
@pre If aPixelFormats is not null then aCount is greater than zero.
@post None.
@return KErrNone if successful.
@return KErrArgument if aInfo is invalid or if aPixelFormats is not null and aCount
        is negative.
@return KErrOverflow if the actual number of supported pixel formats is greater than
        the number of elements in the array pointed to by aPixelFormats.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgImage::GetPixelFormats(const TSgImageInfo& aInfo, TUidPixelFormat* aPixelFormats, TInt& aCount)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	TInt err = gPls.iDriver->GetPixelFormats(aInfo, aPixelFormats, aCount);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}
