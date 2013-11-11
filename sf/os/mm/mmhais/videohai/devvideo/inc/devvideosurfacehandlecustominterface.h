// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DEVVIDEOSURFACEHANDLECUSTOMINTERFACE_H
#define DEVVIDEOSURFACEHANDLECUSTOMINTERFACE_H

#include <graphics/surface.h>
#include <mmf/common/mmfvideo.h>

/** 
DevVideo SurfaceHandle Custom Interface UID.

@publishedPartner
@prototype
*/
const TUid KUidMMFVideoSurfaceHandleControl = { 0x10286486 };

/**
Video play surface handle custom interface.
This interface can be used by a H/W device which creates a surface to pass
information about the surface (such as the surface handle) to another H/W device.
This is typically implemented by a post processor and used by a decoder.

@publishedPartner
@prototype
*/
class MMmfVideoSurfaceHandleControl
	{
	public:

	/**
	Set the handle for the surface

	@param aSurfaceHandle Surface handle for the new surface.
	*/
	virtual void MmvshcSetSurfaceHandle(const TSurfaceId& aSurfaceHandle) = 0;

	/**
	Pass a buffer to redraw on the surface.
	One situation here this might be used is where resources for a decoder are
	temporarily removed. The decoder can call this routine in order to pass the
	last picture to a postprocessor for redrawing.

	@param  aRedrawBuffer Buffer containing picture to redraw. The memory for
	this buffer is owned by the caller and ownership of the memory is not
	transferred as part of this call. The memory may not be valid	after the
	call completes.
	*/
	virtual void MmvshcRedrawBufferToSurface(TPtrC8& /*aRedrawBuffer*/) {};
	};

#endif // DEVVIDEOSURFACEHANDLECUSTOMINTERFACE_H
