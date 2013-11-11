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

#ifndef DEVVIDEOVIDEOSURFACECUSTOMINTERFACE_H
#define DEVVIDEOVIDEOSURFACECUSTOMINTERFACE_H

#include <graphics/surface.h>
#include <mmf/common/mmfvideo.h>

/** 
DevVideo Surface Support Custom Interface UID.

@publishedPartner
@prototype
*/
const TUid KUidMMFVideoSurfaceSupport = { 0x1028340E };

/**
Video graphics surface change observer that is used with the MMMFVideoSurfaceSupport 
custom interface. Controllers or other DevVideo clients using that custom interface 
must implement this observer interface.

@publishedPartner
@prototype
*/
class MMMFVideoSurfaceObserver
    {
public:
    /** 
    Video surface created. The media device calls this method when a surface has been created 
    after (1) a surface has been created after a play command and the first frame for the stream
    has been passed to the surface, and (2) when an existing surface that has already been created
    by the media device is to be replaced with a new surface also created by the mdeai device. MMF
    controllers typically send a KMMFEventCategoryVideoSurfaceCreated event to the client utility
    when they receive this callback.
    */
    virtual void MmvsoSurfaceCreated() = 0;
    
    /** 
    Video surface parameters changed. The client must get the updated surface parameters with 
    MmvssGetSurfaceParametersL() and recalculate its display layout. MMF controllers typically 
    send a KMMFEventCategoryVideoSurfaceParametersChanged event to the client utility when 
    they receive this callback.
    */
    virtual void MmvsoSurfaceParametersChanged() = 0;
    
    /** 
    The client must remove the video surface. The media device calls this callback when the surface
    needs to be replaced. This typically happens if the video picture size changes too much to fit in 
    existing surface buffers.
    
	The client must remove all surfaces from its current windows and when it has done so must call
	MmvssSurfaceRemovedL().
	
	MMF controllers typically send a KMMFEventCategoryVideoRemoveSurface event to the client 
	utility when they receive this callback and let the client utility remove the surface.
    */
    virtual void MmvsoRemoveSurface() = 0;
    };

/**
DevVideo custom interface. Video decoders and post-processor media devices 
that support rendering to video surfaces implements this custom interface to 
provide surface management support to the controller. Most of the methods in 
this custom interface map directly to MMF custom commands, and controllers 
typically implement the MMF custom commands using these methods.

@publishedPartner
@prototype
*/
class MMMFVideoSurfaceSupport
    {
public:
	/** 
    Requests the media device to use graphics surfaces for video rendering. 
    The client must call this method before Initialize() to ensure the media 
    device allocates the right types of resources for rendering.
    */
    virtual void MmvssUseSurfaces() = 0;
    
    /** 
    Sets a new video surface support observer to receive surface management events 
    from the media device.
     
    @param aObserver New observer object to use.
    */
    virtual void MmvssSetObserver(MMMFVideoSurfaceObserver& aObserver) = 0;
  
    /** 
    Retrieves parameters for the current surface. The client typically calls this in response 
    to a MmvsoSurfaceCreated() or MmvsoSurfaceParametersChanged() observer callback.

	@param  aSurfaceId
	        Surface ID for the display.
	@param  aCropRect
	        Cropping rectangle within the surface. The crop rectangle identifies the area of 
	        the surface that should be shown on the screen.
	@param  aPixelAspectRatio
	        Video picture pixel aspect ratio.

	@leave KErrNotReady if no surface is available for the display.
    */
    
    virtual void MmvssGetSurfaceParametersL(TSurfaceId& aSurfaceId, TRect& aCropRect, 
    					TVideoAspectRatio& aPixelAspectRatio) = 0;

    /** 
    Indicates that the surface is no longer in use and can be destroyed. The client typically calls
    this in response to MmvsoSurfaceCreated() (where the newly created surface is replacing an
    existing one) or MmvsoRemoveSurface() observer callback.  
         
    @param aSurfaceId Surface ID that is no longer in use
    
    @leave KErrNotFound if the surface does not exist
    */
    virtual void MmvssSurfaceRemovedL(const TSurfaceId& aSurfaceId) = 0;
    };	

#endif // DEVVIDEOVIDEOSURFACECUSTOMINTERFACE_H
