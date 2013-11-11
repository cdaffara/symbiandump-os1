/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef DEVVIDEOSURFACEHANDLECUSTOMINTERFACE_H
#define DEVVIDEOSURFACEHANDLECUSTOMINTERFACE_H

#include <graphics/surface.h>
#include <mmf/common/mmfvideo.h>
#include <graphics/surfacemanager.h>

/** 
DevVideo SurfaceHandle Custom Interface UID.

@publishedPartner
@prototype
*/
const TUid KUidMMFVideoSurfaceHandleControl = { 0x10286486 };

/**
Video play surface handle custom interface. This interface can be used by
a H/W device which creates a surface to pass the surface handle to 
another H/W device. Alternatively, a H/W device may create resources for the 
surface, for example, chunks and pass details to another H/W device to 
create surface based on the chunk information.
Typically implemented by the post processor and used by a decoder.

@publishedPartner
@prototype
*/
class MMmfVideoSurfaceHandleControl
	{
	public:

	/**
	@param aSurfaceHandle Surface handle for the new surface.
	*/
	virtual void MmvshcSetSurfaceHandle(const TSurfaceId& aSurfaceHandle) = 0;
	
    /**
    @param  aRedrawBuffer Buffer  for the  redrawing of  the last  picture. The
    memory of this buffer is owned  by the caller and ownerhship of  this memory
    is not transerred as  part of this call.  Also, the memory may  not be valid
    after the call completes.
     */
     virtual void MmvshcRedrawBufferToSurface(TPtrC8& aRedrawBuffer) = 0;
     
     /**
      This is used to get th esurface create dbased on an ixisting chunk. Typically,
      the chunk would be created by the caller of this method and surface would be 
      created by the implementer.
       
      @param aAttributes Info necessary to create surface based on existing chunk.
      @param aHandle Handle of the existing chunk
      @param aSurfaceId Surface Id created as on output of this method.
      
      @return error code 
      */
     virtual TInt MmvshcCreateSurface(const RSurfaceManager::TSurfaceCreationAttributes& aAttributes, TInt aHandle, TSurfaceId& aSurfaceId) = 0;
    
    };


#endif // DEVVIDEOSURFACEHANDLECUSTOMINTERFACE_H

