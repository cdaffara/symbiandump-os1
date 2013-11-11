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


#ifndef DEVVIDEOCLIENTBUFFERSUPPORT_H
#define DEVVIDEOCLIENTBUFFERSUPPORT_H

#include <e32def.h>

class TVideoFrameBuffer;

/** 
DevVideo Client Buffer Support Custom Interface UID.

@publishedPartner
@released
*/
const TUid KUidMMFVideoClientBufferSupport = { 0x10283417 };

/**
In implementations where the controller creates and manages surfaces, this
DevVideo custom interface lets the decoder decode into those client-supplied
surface buffers.

@publishedPartner
@released
*/
class MMMFVideoClientBufferSupport
	{
public:
	/**
	Enables client buffer mode. This method must be called before Initialize() with
	aClientBuffers=ETrue to use client-supplied buffers. When client-supplied buffers
	are used the decoder will not allocate output buffers, but will instead expect to
	receive them from the client using MvcbsSupplyBuffer().
	
	@param aClientBuffers ETrue to enable using client-supplied buffers, EFalse to
	disable this.
	*/
	virtual void MvcbsUseClientBuffers(TBool aClientBuffers) = 0;
	
	/**
	Supplies a buffer to be used for decoder output. The decoder will decode a
	picture into the buffer and return it to the client through the standard
	DevVideoPlay data flow (MdvppNewPicture() and NextPictureL()). The client can
	identify the buffer by comparing TVideoPicture.iData.iRawData in the output
	picture to TVideoFrameBuffer.Buffer() in the supplied buffer.
	
	Clients can queue multiple buffers in a decoder by calling this method several
	times before receiving output pictures.
	
	The client must call ReturnPicture() for each decoder output picture even when
	using client-supplied buffers, since the decoder must still be able to free other
	DevVideo-related data structures such as the TVideoPicture object itself.
	
	@param aBuffer A TVideoFrameBuffer structure describing the buffer. The buffer
	must remain accessible and the reference valid until the client receives the
	buffer through NextPictureL() or calls MvcbsReleaseBuffers().
	*/
	virtual void MvcbsSupplyBuffer(TVideoFrameBuffer& aBuffer) = 0;
	
	/**
	Requests the decoder to release all buffers previously supplied with 
	MvcbsSupplyBuffer() but not yet used. The decoder must synchronously stop using
	any such buffers before returning from this method.
	
	This method is typically used when the client must delete a surface unexpectedly.

	Note that decoded buffers may also be buffered in DevVideoPlay. Therefore, in
	addition to calling this method the client must also receive all decoded pictures
	from DevVideoPlay by calling NextPictureL() repeatedly until no more pictures are
	available.
	*/
	virtual void MvcbsReleaseBuffers() = 0;
	};

#endif // DEVVIDEOCLIENTBUFFERSUPPORT_H
