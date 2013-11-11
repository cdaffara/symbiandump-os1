/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Video play reclaim external buffers custom interface. 
*
*/



#ifndef DEVVIDEORECLAIMEXTBUFFERSCUSTOMINTERFACE_H
#define DEVVIDEORECLAIMEXTBUFFERSCUSTOMINTERFACE_H


#include <mmf/common/mmfvideo.h>

/** 
DevVideo Reclaim External Buffers Custom Interface UID.

@publishedPartner
@prototype
*/
const TUid KUidMMFVideoReclaimExtBuffersControl = { 0x10204C33 };	

/**
Video play reclaim external buffers custom interface. This interface is used in situation 
when the buffers used at the output of the decoder are shared with the rest of the 
pipeline (post-processor and display-posting surface). This interface can be used by a 
decoder H/W device to signal the situations when buffers will be unavailable to 
the rest of the pipeline. Typically implemented by the post processor and used by a decoder.


@publishedPartner
@prototype
*/
class MMmfVideoReclaimExtBuffersControl
	{
	public:

	/**
		This method is called when the owner of the buffers needs to free the buffers. 
        In order to do it in a clean way it needs to notify this to the other parties using the buffers
	@param 
	*/
	virtual void MmvrebReclaimExtBuffers() = 0;

	};


#endif // DEVVIDEORECLAIMEXTBUFFERSCUSTOMINTERFACE_H

