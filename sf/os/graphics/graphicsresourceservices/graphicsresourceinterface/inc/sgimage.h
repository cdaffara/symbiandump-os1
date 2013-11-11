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
// Graphics Resource - images
//

#ifndef SGIMAGE_H
#define SGIMAGE_H

#include <sgresource/sgresource.h>

/**
A class that encapsulates the basic attributes of an image.
It is used both to create images and to obtain information about them.
The basic attributes of an image cannot be changed after creation.
For an instance of TSgImageInfo to be valid the following conditions must be satisfied:
	- The width and height in iSizeInPixels must both be greater than zero.
	- iPixelFormat must not be EUidPixelFormatUnknown.
	- iUsage must have at least one usage bit set.
*/
NONSHARABLE_CLASS(TSgImageInfo)
	{
public:
	/**
	Default constructor.
	Data members remain uninitialised.
	*/
	inline TSgImageInfo();

	/**
	Constructor which initialises data members to the given values.
	*/
	inline TSgImageInfo(const TSize& aSizeInPixels, TInt aPixelFormat, TUint32 aUsage);
public:
	/** The size of the image in pixels.*/
	TSize iSizeInPixels;
	/**
	UID representing the pixel format of the image. 
	The values enumerated in TSgPixelFormat are guaranteed to be supported by 
	every implementation of the Graphics Resource API but additional pixel 
	formats from TUidPixelFormat may be supported by some implementations.
	@see RSgImage::GetPixelFormats().
	*/
	TInt iPixelFormat;
	/** The possible usage of the image as a combination of bits from TSgUsageBits.*/
	TUint32 iUsage;
	};

/**
The drawable resource type associated with images.
*/
const TUid KSgImageTypeUid = {0x10285A73};

/**
An image handle. 
It inherits all the general handle functionality from RSgDrawable. 
An image is a drawable resource containing a two-dimensional pixel array. 
Its basic attributes are the size in pixels, the pixel format and the usage.
The usage for which an image is created must be declared so that it can be properly allocated.
The attributes of an image cannot be changed after creation.
Instances of RSgImage can be constructed before RSgDriver::Open() is called and 
the implementation of the Graphics Resource API is initialised in the context 
of the process, but most attempts to call a function of RSgImage will panic 
with category “SGRES” and code 1 both in debug and release builds if the 
implementation of the Graphics Resource API is not initialised in the context 
of the process. Any attempt to call a function of RSgImage on an invalid handle 
will panic with category “SGRES” and code 3 both in debug and release builds.
*/
NONSHARABLE_CLASS(RSgImage): public RSgDrawable
	{
public:
	/**
	Default constructor which sets iHandleType to KSgImageTypeUid and 
	creates null image handles.
	*/
	inline RSgImage();

	/**
	Creates an image with the basic attributes given by the parameter aInfo 
	and the initial contents given by the parameters aDataAddress and 
	aDataStride, and returns KErrNone if successful.

	@pre An RSgDriver handle has been opened in the context of the process.
	@pre The instance of RSgImage is a null handle.
	@pre The parameter aInfo is valid.
	@pre If the parameter aDataAddress is not NULL then the parameter aDataStride 
		is not zero and its absolute value is equal to or greater than the 
		minimum number of bytes needed for a row of pixel data.
	@post The created image has an initial reference count of one.
	@param aInfo An instance of TSgImageInfo with the basic attributes of the 
		image to be created.
	@param aDataAddress The base address of the pixel data used to populate the 
		image to be created. The pixel format of the data must be the exact 
		pixel format given in aInfo but the implementation of Graphics 
		Resource may convert the data to the internal pixel format of the image, 
		which could be any pixel format without loss of data. 
		If aDataAddress is NULL the initial contents of the image are undefined.
	@param aDataStride The number of bytes between the rows of the pixel data 
		pointed to by aDataAddress. It can be a positive value to indicate 
		top-down ordering of the rows of pixel data or a negative value to 
		indicate bottom-up ordering of the rows of pixel data. Inside each row 
		of pixel data, ordering of pixels is always left-to-right.
	@param aAttributes A pointer to an array of extension attributes, if allowed 
		by any extension of the Graphics Resource API, or NULL otherwise.
	@return KErrNone if successful;
		KErrInUse if the instance of RSgImage is an open handle;
		KErrArgument if either 
			1. the parameter aInfo is not valid or 
			2. the parameter aDataAddress is not NULL and the parameter aDataStride 
				is zero or its absolute value is less than the minimum number of bytes 
				needed for a row of pixel data;
		KErrTooBig if the size given by the parameter aInfo is greater than 
			the maximum image size supported by the implementation of Graphics 
			Resource API. The maximum image size supported by an implementation of 
			the Graphics Resource API is at least 2048 by 2048 pixels;
		KErrNotSupported if either 
			1. the combination of pixel format and usages given by the parameter 
				aInfo is not supported by the implementation of the Graphics Resource 
				API or 
			2. the parameter aAttributes is not NULL and one or more of the extension 
				attributes in the array is not defined by any extension of the Graphics 
				Resource API;
		KErrNoMemory if there is not enough system memory to create the image;
		KErrNoGraphicsMemory if there is not enough specialised graphics memory 
			to create the image.
	@panic SGRES 1 No RSgDriver handle has been opened in the context of the process.
	*/
	IMPORT_C TInt Create(const TSgImageInfo& aInfo,
	                     const TAny* aDataAddress = NULL,
	                     TInt aDataStride = 0,
	                     const TSgAttributeArrayBase* aAttributes = NULL);

	/**
	Creates an image with the basic attributes given by the parameter aInfo 
	and the initial contents copied from an existing image given by the 
	parameter aImage, and returns KErrNone if successful.

	@pre An RSgDriver handle has been opened in the context of the process.
	@pre The instance of RSgImage is a null handle.
	@pre The parameter aInfo is valid.
	@pre The parameter aImage is an open handle
	@pre The size and the pixel format given by aInfo must be the same as 
		the size and the pixel format of the existing image.
	@post The created image has an initial reference count of one.
	@param aInfo An instance of TSgImageInfo with the basic attributes of the 
		image to be created.
	@param aImage A handle to the existing image.
	@param aAttributes A pointer to an array of extension attributes, if allowed 
		by any extension of the Graphics Resource API, or NULL otherwise.
	@return KErrNone if successful;
		KErrInUse if the instance of RSgImage is an open handle;
		KErrArgument if either 
			1. the parameter aInfo is not valid or 
			2. the parameter aImage is a null handle;
		KErrNotSupported if either 
			1. the combination of pixel format and usages given by the parameter 
				aInfo is not supported by the implementation of the Graphics Resource 
				API or 
			2. the size and the pixel format given by the parameter aInfo are 
				not the same as the size and the pixel format of the existing image or 
			3. the parameter aAttributes is not NULL and one or more of the extension 
				attributes in the array is not defined by any extension of the Graphics 
				Resource API;
		KErrNoMemory if there is not enough system memory to create the image;
		KErrNoGraphicsMemory if there is not enough specialised graphics memory 
			to create the image.
	@panic SGRES 1 No RSgDriver handle has been opened in the context of the process.
	@panic SGRES 3 aImage is an invalid handle.
	*/
	IMPORT_C TInt Create(const TSgImageInfo& aInfo,
	                     const RSgImage& aImage,
	                     const TSgAttributeArrayBase* aAttributes = NULL);

	/**
	Retrieves the values of the basic attributes of an image and returns 
	KErrNone if successful.

	@pre An RSgDriver handle has been opened in the context of the process.
	@pre The instance of RSgImage is an open handle.
	@param[out] aInfo An instance of TSgImageInfo that on return contains the 
		values of the basic attributes of the image.
	@return KErrNone if successful;
		KErrBadHandle if the instance of RSgImage is a null handle.
	@panic SGRES 1 No RSgDriver handle has been opened in the context of the process.
	@panic SGRES 3 The instance of RSgImage is an invalid handle.
	*/
	IMPORT_C TInt GetInfo(TSgImageInfo& aInfo) const;

	/**
	Retrieves the value of an extension attribute of an image and returns 
	KErrNone if successful.

	@pre An RSgDriver handle has been opened in the context of the process.
	@pre The instance of RSgImage is an open handle.
	@param[in] aUid The UID of the extension attribute.
	@param[out] aInfo A reference to a variable that on return holds the value 
		of the extension attribute.
	@return KErrNone if successful;
		KErrBadHandle if the instance of RSgImage is a null handle;
		KErrNotSupported if no extension of the Graphics Resource API defines 
		an extension attribute that applies to the image with the given UID.
	@panic SGRES 1 No RSgDriver handle has been opened in the context of the process.
	@panic SGRES 3 The instance of RSgImage is an invalid handle.
	*/
	IMPORT_C TInt GetAttribute(TUid aUid, TInt& aValue) const;

	/**
	Retrieves the list of pixel formats supported by the implementation 
	of the Graphics Resource API for images with the usage given by the 
	parameter aUsage and returns KErrNone if successful. 
	This is a utility function typically called before creating images.

	@pre The parameter aUsage has at least one usage bit set.
	@pre The number of elements in the array referenced by the parameter 
		aPixelFormats is zero.
	@param[in] aUsage A combination of usages from TSgUsageBits.
	@param[out] aPixelFormats A reference to an array that on input must be empty 
		and on return contains the list of supported pixel formats.
	@param[in] aAttributes A pointer to an array with extension image attributes, 
		if any extension of the Graphics Resource API defines extension image 
		attributes that have an impact on the list of supported pixel formats, 
		or NULL otherwise.
	@return KErrNone if successful;
		KErrArgument if either 
			1. the parameter aUsage does not have at least one usage bit set or 
			2. the number of elements in the array referenced by the parameter 
				aPixelFormats was not zero before calling this function;
		KErrNotSupported if the parameter aAttributes is not NULL and one or 
			more of the extension attributes in the array is not defined by any 
			extension of the Graphics Resource API;
		KErrNoMemory if there is not enough system memory to add a pixel format 
			to the array referenced by the parameter aPixelFormats.
	*/
	IMPORT_C static TInt GetPixelFormats(TUint32 aUsage,
	                                     RArray<TInt>& aPixelFormats,
	                                     const TSgAttributeArrayBase* aAttributes = NULL);
	};

#include <sgresource/sgimage.inl>

#endif // SGIMAGE_H
