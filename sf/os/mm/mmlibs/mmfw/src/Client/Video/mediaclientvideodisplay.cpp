// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mediaclientvideodisplay.h>
#include "mediaclientvideodisplaybody.h"


/**
Constructs and initialises a new instance of the client video data to display image 
on window or graphics surface.

The function leaves if the video display object cannot be created.

@param aDisplayId
		A display ID.
		
@return A pointer to the new video display object.
*/

EXPORT_C CMediaClientVideoDisplay* CMediaClientVideoDisplay::NewL(TInt aDisplayId)
	{
	CMediaClientVideoDisplay* self = new (ELeave) CMediaClientVideoDisplay();
	CleanupStack::PushL(self);
	self->ConstructL(aDisplayId);
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructs and initialises a new instance of the client video data to display image 
on window or graphics surface.

The function leaves if the video display object cannot be created.

@param aDisplayId
		A display ID.
		
@param	aSurfaceId
		The surface that has been created.
		
@param  aCropRect
		The dimensions of the crop rectangle, relative to the video image.

@param  aAspectRatio
       	The pixel aspect ratio to display video picture.
       	
@return A pointer to the new video display object.
*/

EXPORT_C CMediaClientVideoDisplay* CMediaClientVideoDisplay::NewL(TInt aDisplayId, const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio)
	{
	CMediaClientVideoDisplay* self = new (ELeave) CMediaClientVideoDisplay();
	CleanupStack::PushL(self);
	self->ConstructL(aDisplayId, aSurfaceId, aCropRect, aAspectRatio);
	CleanupStack::Pop(self);
	return self;
	}

CMediaClientVideoDisplay::CMediaClientVideoDisplay()
	{	
	}


void CMediaClientVideoDisplay::ConstructL(TInt aDisplayId)
	{
	iBody = CMediaClientVideoDisplayBody::NewL(aDisplayId, ETrue);
	}

void CMediaClientVideoDisplay::ConstructL(TInt aDisplayId, const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio)
	{
	iBody = CMediaClientVideoDisplayBody::NewL(aDisplayId, aSurfaceId, aCropRect, aAspectRatio, ETrue);
	}

/**
Destructor.

Releases resources owned by the object prior to its destruction.
*/
	
EXPORT_C CMediaClientVideoDisplay::~CMediaClientVideoDisplay()
	{
	delete iBody;
	}

/**
Adds a new window for displaying the video picture. 
	
@param  aWindow
       	The display window.

@param  aClipRect
       	Window clipping rectangle, relative to the window. The clipping rectangle specifies 
       	the part of the window used for video display. The rectangle must be contained 
       	completely within the window.

@param  aCropRegion
		The dimensions of the crop region, relative to the video image.
		
@param  aVideoExtent
       	Video extent on the screen, relative to the window. Video picture position within 
       	the extent depends on the scaled picture and content alignment or offset. The video 
       	extent can be partially or completely outside the window.

@param  aScaleWidth       	
       	Scaling the width of the video frame.

@param  aScaleHeight       	
       	Scaling the height of the video frame.

@param  aRotation       	
       	The desired rotation to apply in 90 degree increments.

@param  aAutoScaleType       	
       	Automatic scaling type.

@param  aHorizPos       	
       	Video picture horizontal position, relative to the video window. The value can be either 
       	a pixel offset (positive or negative) from the top left corner of the window to the top left 
       	corner of the picture, or an alignment constant from enum THorizontalAlign.

@param  aVertPos       	
       	Video picture vertical position, relative to the video window. The value can be either a pixel 
       	offset (positive or negative) from the top left corner of the window to the top left corner of 
       	the picture, or an alignment constant from enum TVerticalAlign.

@param  aWindow2
       	The display window, aWindow2 is used when using CVideoPlayerUtility2. This is used to avoid 
		casting iWindow from RWindowBase* to RWindow*.
     	       	       	       	
*/
	
EXPORT_C void CMediaClientVideoDisplay::AddDisplayWindowL(const RWindowBase* aWindow, const TRect& aClipRect, const TRect& aCropRegion, const TRect& aVideoExtent, 
															TReal32 aScaleWidth, TReal32 aScaleHeight, TVideoRotation aRotation, 
															TAutoScaleType aAutoScaleType, TInt aHorizPos, TInt aVertPos, RWindow* aWindow2)
	{
	iBody->AddDisplayWindowL(aWindow, aClipRect, aCropRegion, aVideoExtent, aScaleWidth, aScaleHeight, aRotation,aAutoScaleType, aHorizPos, aVertPos, aWindow2); 															 															
	}

/**
Removes a window that is currently being used to display the video picture. The window must
have previously been added with AddDisplayWindowL(). 

Note Depending on underlying implementation it may also remove any graphics resources associated
with video playback on this window. 

Note removing the last window on a display will deregister the display
	
@param  aWindow
       	The display window.

@return The window position that has been removed.

*/

EXPORT_C TInt CMediaClientVideoDisplay::RemoveDisplayWindow(const RWindowBase& aWindow)
	{
	return iBody->RemoveDisplayWindow(aWindow);
	}

/**
Must be called when a Surface has been Created.

@param	aSurfaceId
		The surface that has been created.
		
@param  aCropRect
		The dimensions of the crop rectangle, relative to the video image.

@param  aAspectRatio
       	The pixel aspect ratio to display video picture.

@param  aCropRegion
		The dimensions of the crop region, relative to the video image.
		
@return KErrNone is succesfully created, otherwise returns system wide error.

*/

EXPORT_C TInt CMediaClientVideoDisplay::SurfaceCreated(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio, const TRect& aCropRegion)
	{
	return iBody->SurfaceCreated(aSurfaceId, aCropRect, aAspectRatio, aCropRegion);
	}

/**
Must be called when the current surface is no longer being used.

*/
	
EXPORT_C void CMediaClientVideoDisplay::RemoveSurface()
	{
	iBody->RemoveSurface(ETrue);
	}

/** 

Must be called when the video surface parameters have been changed.

@param	aSurfaceId
		The surface to be created for composition.
		
@param  aCropRect
		The dimensions of the crop rectangle, relative to the video image.

@param  aAspectRatio
       	The pixel aspect ratio to display video picture.

@return KErrNone is succesfully created, otherwise returns KErrInUse if surface is not created.

*/

EXPORT_C TInt CMediaClientVideoDisplay::SurfaceParametersChanged(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio)
	{
	return iBody->SurfaceParametersChanged(aSurfaceId, aCropRect, aAspectRatio);
	}

/**
Redraw all the windows with new crop region.
	
@param  aCropRegion
		The dimensions of the crop region, relative to the video image.

@return KErrNone is succesfully created, otherwise returns system wide error.
*/	

EXPORT_C TInt CMediaClientVideoDisplay::RedrawWindows(const TRect& aCropRegion)
	{
	return iBody->RedrawWindows(aCropRegion);
	}

/** 
Set video automatic scaling. When automatic scaling is active, the
video picture is scaled automatically to match the video window,
based on the scaling type, and positioned according to the
parameters.

Not all video controller support automatic scaling. 

This function quits or exits leaving any of the system wide error codes.
Common error codes are listed below.

@param  aWindow
       	The display window

@param  aAutoScaleType       	
       	Automatic scaling type

@param  aHorizPos       	
       	Video picture horizontal position, relative to the video window. 
       	
@param  aVertPos       	
       	Video picture vertical position, relative to the video window. 

@param  aCropRegion
		The dimensions of the crop region, relative to the video image.
 
@leave Leaves with system wide error code.
*/

EXPORT_C void CMediaClientVideoDisplay::SetAutoScaleL(const RWindowBase& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion)
	{
	iBody->SetAutoScaleL(aWindow, aScaleType, aHorizPos, aVertPos, aCropRegion);
	}

/**
Rotates the video image within the window. This is the preferred method to use with CVideoPlayerUtility2.

The rotation will replace any rotation set with CVideoPlayerUtility::SetRotationL. 
Likewise with setting the rotation with CVideoPlayerUtility::SetRotationL after a call to CVideoPlayerUtility2::SetRotationL has been
made, then the rotation specified will replace any rotation set with CVideoPlayerUtility2::SetRotationL.

@param aWindow 
		Window to set rotation for.

@param aRotation 
		The video rotation to use for aWindow.

@param  aCropRegion
		The dimensions of the crop region, relative to the video image.
 
*/	

EXPORT_C void CMediaClientVideoDisplay::SetRotationL(const RWindowBase& aWindow, TVideoRotation aRotation, const TRect& aCropRegion)
	{
	iBody->SetRotationL(aWindow, aRotation, aCropRegion);
	}

/**
Rotates the video image within the window. This is the preferred method to use with CVideoPlayerUtility2.

The rotation will replace any rotation set with CVideoPlayerUtility::SetRotationL. 
Likewise with setting the rotation with CVideoPlayerUtility::SetRotationL after a call to CVideoPlayerUtility2::SetRotationL has been
made, then the rotation specified will replace any rotation set with CVideoPlayerUtility2::SetRotationL.

@param aRotation 
		The video rotation to use for aWindow.

@param  aCropRegion
		The dimensions of the crop region, relative to the video image.
 
*/

EXPORT_C void CMediaClientVideoDisplay::SetRotationL(TVideoRotation aRotation, const TRect& aCropRegion)
	{
	iBody->SetRotationL(aRotation, aCropRegion);
	}

/**
Retrieves the video rotation set for a window. This is the preferred method to use with CVideoPlayerUtility2.

@param aWindow 
		Window to retrieve rotation for.
		
@return The video rotation.

*/	

EXPORT_C TVideoRotation CMediaClientVideoDisplay::RotationL(const RWindowBase& aWindow)
	{
	return iBody->RotationL(aWindow);
	}

/**
Scales the video image to a specified percentage of its original size within the window. 

@param aWindow 
		Window to set scale factor for.
		
@param aWidthPercentage
        The percentage (100 = original size) to be used to scale the width of the video image
        
@param aHeightPercentage
        The percentage (100 = original size) to be used to scale the height of the video image. 
        If this is not equal to aWidthPercentage then the image may be distorted.
        
@param  aCropRegion
		The dimensions of the crop region, relative to the video image.

@see CVideoPlayerUtility2::SetScaleFactorL
*/
	
EXPORT_C void CMediaClientVideoDisplay::SetScaleFactorL(const RWindowBase& aWindow, TReal32 aWidthPercentage, TReal32 aHeightPercentage, const TRect& aCropRegion)
	{
	iBody->SetScaleFactorL(aWindow, aWidthPercentage, aHeightPercentage, aCropRegion);
	}

/**
Scales the video image to a specified percentage of its original size within the window. 

@param aWidthPercentage
        The percentage (100 = original size) to be used to scale the width of the video image
        
@param aHeightPercentage
        The percentage (100 = original size) to be used to scale the height of the video image. 
        If this is not equal to aWidthPercentage then the image may be distorted.

@param  aCropRegion
		The dimensions of the crop region, relative to the video image.
		
@see CVideoPlayerUtility2::SetScaleFactorL
*/
	
EXPORT_C void CMediaClientVideoDisplay::SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, const TRect& aCropRegion)
	{
	iBody->SetScaleFactorL(aWidthPercentage, aHeightPercentage, aCropRegion);
	}

/**
Retrieves the scale factor currently set for a window. This is the preferred method to use with CVideoPlayerUtility2.

@param aWindow 
		Window to retrieve scale factor for.
		
@param aWidthPercentage
        On function return, contains the current scaling percentage applied to the width of the
        video image (100 = original size).
        
@param aHeightPercentage
        On function return, contains the current scaling percentage applied to the height
        of the video image (100 = original size).
        
@see CVideoPlayerUtility2::GetScaleFactorL
*/
	
EXPORT_C void CMediaClientVideoDisplay::GetScaleFactorL(const RWindowBase& aWindow, TReal32& aWidthPercentage, TReal32& aHeightPercentage)
	{
	iBody->GetScaleFactorL(aWindow, aWidthPercentage, aHeightPercentage);
	}

/** 
Set video automatic scaling.

@param  aHorizPos       	
       	Video picture horizontal position, relative to the video window. 
       	
@param  aVertPos       	
       	Video picture vertical position, relative to the video window. 

@param  aCropRegion
		The dimensions of the crop region, relative to the video image.

@see CVideoPlayerUtility2::SetAutoScaleL

*/	

EXPORT_C void CMediaClientVideoDisplay::SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion)
	{
	iBody->SetAutoScaleL(aScaleType, aHorizPos, aVertPos, aCropRegion);
	}

/**
Sets the window clipping rectangle, relative to the window. 

@param	aWindow
		Window to set clipping rectangle for.
		
@param  aWindowClipRect
       	The window server session for this window.
       	
@param  aCropRegion
		The dimensions of the crop region, relative to the video image.

@see CVideoPlayerUtility2::SetWindowClipRectL

*/

EXPORT_C void CMediaClientVideoDisplay::SetWindowClipRectL(const RWindowBase& aWindow, const TRect& aWindowClipRect, const TRect& aCropRegion)
	{
	iBody->SetWindowClipRectL(aWindow, aWindowClipRect, aCropRegion);
	}

/**
Sets the video extent on the screen, relative to the window. 

@param  aWindow
		Window to set video extent for.
		
@param  aVideoExtent
       	The new video extent, relative to the video window.

@param  aCropRegion
		The dimensions of the crop region, relative to the video image.
      	
@see CVideoPlayerUtility2::SetWindowClipRectL
*/
		
EXPORT_C void CMediaClientVideoDisplay::SetVideoExtentL(const RWindowBase& aWindow, const TRect& aVideoExtent, const TRect& aCropRegion)
	{
	iBody->SetVideoExtentL(aWindow, aVideoExtent, aCropRegion);
	}

/**
Enables automatic switching of surface to/from external display when it is connected/disconnected from the device.

To use this function the client thread must have an Active Scheduler installed otherwise it will leave with KErrNotReady.

@param  aEnable
        ETrue to enable. EFalse to disable.
@leave  KErrNotSupported Device does not support external displays
@leave  KErrNotReady CActiveScheduler is not installed
*/

EXPORT_C void CMediaClientVideoDisplay::SetExternalDisplaySwitchingL(TBool aControl)
    {
    iBody->SetExternalDisplaySwitchingL(aControl);
    }
