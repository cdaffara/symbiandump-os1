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

#ifndef SGIMAGE_CHUNK_H
#define SGIMAGE_CHUNK_H

#include <e32std.h>


/**
@internalTechnology
@prototype

This interface allows access to the data chunk where an image is stored. The chunk
handle obtained through this interface can be passed to device drivers that need
access to the pixel data of the image.

This interface is only supported if the image is stored in a shared chunk.
*/
class MSgImage_Chunk
	{
public:
	enum { EInterfaceUid = 0x102858F1 };
	/**
	@internalTechnology
	@prototype

	Retrieves a handle to the chunk where the pixel data is stored. The returned
	chunk handle is valid until the RSgImage handle is closed.

	@return A handle to the chunk where the pixel data is stored.
	*/
	virtual const RChunk& DataChunk() const = 0;
	/**
	@internalTechnology
	@prototype

	Retrieves the base offset of the pixel data from the base of the chunk.

	@return The base offset of the pixel data from the base of the chunk.
	*/
	virtual TInt DataOffset() const = 0;
	/**
	@internalTechnology
	@prototype

	Retrieves the number of bytes between rows of the pixel data in the chunk.

	@return The number of bytes between rows of the pixel data in the chunk.
	*/
	virtual TInt DataStride() const = 0;
	};


#endif // SGIMAGE_CHUNK_H
