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

#ifndef SGIMAGE_H
#define SGIMAGE_H

#include <graphics/sgresource.h>
#include <pixelformats.h>


/**
@publishedPartner
@prototype
@deprecated

This class is used to create images and to return information about them.

An image is created as constant if the requested CPU access is ESgCpuAccessNone or
ESgCpuAccessReadOnly and the requested usage does not include any usage as target.
Otherwise it is created as mutable.

For an instance of TSgImageInfo to be valid, the following conditions must be true:
	- The width and height must both be greater than zero.
	- The screen identifier must be greater than or equal to -1 (-1 is reserved to
	  mean screen-agnostic).
	- If the usage includes ESgUsageScreenSource then the screen identifier must not
	  be -1.
	- The number of user-defined attributes must be greater than or equal to zero.
	- If the number of user-defined attributes is greater than zero then the pointer
	  to the array of user-defined attributes must not be null.

For an image creation request to succeed, the following conditions must be true:
	- The instance of TSgImageInfo passed in as a parameter must be valid.
	- If the screen identifier is not -1 then it must refer to an existing screen.
	- If the usage includes ESgUsageScreenSource then the width and height must be
	  compatible with the specified screen.
	- The pixel format must be compatible with the shareability, CPU access, usage and
	  screen identifier. Compatibility is device-dependent, with the exception that
	  to allow generic applications to exist, some level of compatibility must be
	  guaranteed across all systems. To allow efficient implementation on the widest
	  range of hardware, the number of compatibility guarantees is limited.
	  See Image Compatibility Guarantees for more information.

@see RSgImage::GetPixelFormats()
@see RSgImage::Create()
*/
NONSHARABLE_CLASS(TSgImageInfo)
	{
public:
	IMPORT_C TSgImageInfo();
public:
	/**
	The size of the image in pixels.
	*/
	TSize iSizeInPixels;
	/**
	The pixel format of the image. Note that this value is only guaranteed to be
	the actual pixel format if the image is mappable. Otherwise it is acceptable
	for the image to be stored internally in a different format so long as there
	is no loss of information. In all cases, if the user passes in some initial
	data to populate the image during creation, this value is assumed to be the
	exact pixel format of the data passed in.
	*/
	TUidPixelFormat iPixelFormat;
	/**
	Defines the possible usage of the image in terms of the rendering pipelines, and
	purposes within those pipelines, that it will be used for. It is interpreted as a
	combination of the bit flags defined in TSgUsageFlags. An image with limited usage
	is likely to give higher performance than an image with more general usage.
	*/
	TUint32 iUsage;
	/**
	Defines whether the image is shareable between processes. A non-shareable image
	is likely to give higher performance than a shareable image.
	*/
	TBool iShareable;
	/**
	Defines whether and how the image is mappable for CPU access. An image that is not
	mappable for CPU access is likely to give higher performance than a mappable image.
	*/
	TSgCpuAccess iCpuAccess;
	/**
	Defines whether the image is usable on all screens or just on a specific screen.
	A value of -1 is interpreted as meaning that the image is usable on all screens.
	Zero and positive values are interpreted as meaning that the image is only
	valid for use on the specified screen. A screen-specific image is likely to
	give higher performance than a screen-agnostic image.
	*/
	TInt iScreenId;
	/**
	In image creation requests, a pointer to an array with the user-defined attributes
	to be attached to the image or null if the image is to have no user-defined
	attributes.

	In information queries, a pointer to an array that on input contains the globally
	unique identifiers of the user-defined attributes to be retrieved from the image
	and on return will contain the values of the selected user-defined attributes.
	If null then the information query will not retrieve any user-defined attributes.
	*/
	TSgUserAttribute* iUserAttributes;
	/**
	In image creation requests, the number of user-defined attributes to be attached
	to the image.

	In information queries, the number of user-defined attributes to be retrieved
	from the image.
	*/
	TInt iUserAttributeCount;
	};


/**
@publishedPartner
@prototype
@deprecated

This globally unique identifier represents both the handle type for instances of
RSgImage and the drawable resource type associated with images.
*/
const TUid KSgImageTypeUid = {0x10285A73};


/**
@publishedPartner
@prototype
@deprecated

A handle to a reference-counted, bi-dimensional pixel array that can be used for
various purposes, such as being a source or a target of different rendering pipelines,
according to its attributes, which are set at creation time and cannot be changed
afterwards.

An image can be shared between processes by passing its unique identifier across.
Alternatively it can be created as not shareable, or process-specific, and this may
have performance advantages. Sharing is achieved by using the value returned by Id()
in a call to Open() on another instance of RSgImage to open a new handle to the image.
Since images are reference-counted they are guaranteed to exist while there are open
handles referencing them.

An image can be created for use with any screen. Alternatively it can be created as
screen-specific and this may have performance advantages.

An image can be created as mappable. This means that the CPU can potentially read
and/or write directly to the pixel data. It is recommended to use mappable images only
when absolutely necessary because they can be less efficient than non-mappable images.

An image can be created as constant or mutable. Constant images, also known as immutable
images, do not allow modification after creation and this may have performance advantages.
A mutable image can be modified after creation, e.g. by using it as a rendering target.
A constant image cannot be used as a rendering target.

A new RSgImage handle does not refer to an image until a successful call to Create()
or Open(). Before that point, the handle is said to be a null handle. Instances of
RSgImage can be shared among threads in the same process.

An RSgImage handle is said to be invalid if it is not null but it does not reference
an existing image. Copying an instance of RSgImage must be done with extreme care,
since it does not increment the reference count of the referenced image and may
therefore allow some RSgImage handle to become invalid when the image is destroyed.
*/
NONSHARABLE_CLASS(RSgImage): public RSgDrawable
	{
	friend class RSgImageCollection;
public:
	IMPORT_C RSgImage();
	IMPORT_C TInt Create(const TSgImageInfo& aInfo, const TAny* aDataAddress, TInt aDataStride);
	IMPORT_C TInt Create(const TSgImageInfo& aInfo, const RSgImage& aImage);
	IMPORT_C TInt GetInfo(TSgImageInfo& aInfo) const;
	IMPORT_C TInt MapReadOnly(const TAny*& aDataAddress, TInt& aDataStride) const;
	IMPORT_C TInt MapWriteOnly(TAny*& aDataAddress, TInt& aDataStride);
	IMPORT_C TInt MapReadWrite(TAny*& aDataAddress, TInt& aDataStride);
	IMPORT_C TInt Unmap() const;
	IMPORT_C static TInt GetPixelFormats(const TSgImageInfo& aInfo, TUidPixelFormat* aPixelFormats, TInt& aCount);
	};


#endif // SGIMAGE_H
