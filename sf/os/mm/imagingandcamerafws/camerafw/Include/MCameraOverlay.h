// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
*/

#ifndef MCAMERAOVERLAY_H
#define MCAMERAOVERLAY_H

#include <ecam/cameraoverlay.h>
#include <ecam/ecamcommonuidif.hrh>

/** 
This is the UID which is used to obtain the MCameraOverlay interface, 
via a call to CCamera::CustomInterface(), which provides implementation of the M-class interface.

@see KECamMCameraOverlayUidValue
 */   
static const TUid KECamMCameraOverlayUid = {KECamMCameraOverlayUidValue};

/** 
This is the UID which is used to obtain the MCameraOverlay2 interface, 
via a call to CCamera::CustomInterface(), which provides implementation of the M-class interface.

@see KECamMCameraOverlay2UidValue

@publishedPartner
@prototype
*/   
static const TUid KECamMCameraOverlay2Uid = {KECamMCameraOverlay2UidValue};


/**
This is a mixin class to be implemented by providers of the camera extension API for image overlays.

@note This class is intended for sub classing by licensees only.

@see CCamera::CCameraOverlay

@publishedPartner
@released
*/
class MCameraOverlay
	{
public:		
	/**
	Creates an image overlay object on the ECam implementation, returning a handle to the newly created object.

	@param	aParameters
			The parameters characterizing the overlay to be created. 
	@param	aBitmap
			The image that is to become the overlay.
			By default this is set to NULL, allowing the client to provide the image at some point after 
			the overlay object has been created, by using SetOverlayBitmapL().

	@leave	KErrNoMemory if out of memory; also any system wide error.
	
	@leave  KErrArgument if the member variables in TOverlayParameters are such that they create mutual exclusion.		
	
	@return The overlay handle.
	*/
    virtual TUint CreateOverlayL(const CCamera::CCameraOverlay::TOverlayParameters& aParameters, CFbsBitmap* aBitmap)=0;

	/**
	Allows the overlay image data to be changed for a specified overlay.
	Use this function to set the overlay image data if it was not specified when the overlay
	was created using CreateOverlayL().

	@param	aOverlayHandle
			The handle of the overlay whose overlay image data is to be changed.
	@param	aBitmap
			The new image data for the overlay.
		
	@leave	KErrArgument if aOverlayHandle is out of range; also any system wide error.
	
	@note   Once this method is called, overlay size should not be changed for the given overlay. Hence, 
			SetOverlayParametersL should not be called after this method. 
	*/
    virtual void SetOverlayBitmapL(TUint aOverlayHandle, const CFbsBitmap* aBitmap)=0;
    
	/**
	Gets the overlay image data for a specified overlay.

	@param	aOverlayHandle
			The handle of the overlay whose overlay image data is to be obtained.
	@param	aBitmap
			A CFbsBitmap that will receive the returned image data for the overlay.
	
	@leave	KErrArgument if aOverlayHandle is out of range; also any system wide error.
	
	@leave  KErrNotSupported if a sharing client (which did not create the given overlay) tries to retrieve the overlay 
			bitmap and the implementation may not be interested in providing the overlay.
	
	@note   The ECam implementation will transfer the ownership of the aBitmap to the client.
	*/
    virtual void GetOverlayBitmapL(TUint aOverlayHandle, CFbsBitmap* aBitmap)=0;
    
	/**
	Gets the parameters that characterize a given overlay. 

	@param	aOverlayHandle
			The handle of the overlay whose parameters are required.
	@param	aInfo
			Reference to TOverlayParameters object that will contain the returned overlay parameters.
			
	@leave	KErrArgument if aOverlayHandle is out of range; also any system wide error.
	*/
	virtual void GetOverlayParametersL(TUint aOverlayHandle, CCamera::CCameraOverlay::TOverlayParameters& aInfo)=0;
	
	/**
	Sets new parameters that characterize a given overlay.

	@param	aOverlayHandle
			The handle of the overlay whose parameters are to be changed.
	@param	aParameters
			The new overlay parameters.
	
	@leave	KErrNotSupported if TOverlayCameraMode passed in TOverlayParameters is not supported; 
			also any system wide error.
			
	@leave  KErrArgument if the member variables in TOverlayParameters are such that they create mutual exclusion.
	*/
	virtual void SetOverlayParametersL(TUint aOverlayHandle, const CCamera::CCameraOverlay::TOverlayParameters& aParameters)=0;
	
	/**
	Releases the specified overlay handle.

	@note 	If the handle specified in aOverlayHandle is invalid (out of range) the function 
			call is ignored and no error is reported.

	@param	aOverlayHandle	
			The handle of the overlay that is to be released.
	*/
	virtual void ReleaseOverlay(TUint aOverlayHandle)=0;
	
	/**
	Releases and destroys all overlay handles on the ECam implementation. 
	This function is called from the destructor of CCamera::CCameraOverlay.
	*/
	virtual void Release()=0;
	
	/**
	Gets information on the overlay functionality supported by the ECam implementation.

	@param	aInfo
			A reference to a TOverlaySupportInfo object that will receive the overlay support information.
	*/
    virtual void GetOverlaySupport(CCamera::CCameraOverlay::TOverlaySupportInfo& aInfo)=0;
    
	/**
	Gets all the overlay handles maintained by the ECam implementation, in order of their Z-Value.
	
	@param 	aOverlayHandles
			Returned list, in Z-Value order, of all the overlay handles maintained on the ECam implementation.
			The topmost overlay is the first element of the array. 

	@leave	KErrNoMemory if out of memory; also any system wide error.		
	*/
	virtual void GetAllOverlaysInZOrderL(RArray<TUint>& aOverlayHandles)=0;
	
	/**
	Sets Z-Order of all the overlay handles known by this CCameraOverlay object.
	
	@param 	aOverlayHandles
			The overlay handles in aOverlayHandles array. This must be the complete current set 
			of handles known to this CCameraOverlay object. The client specifies the desired 
			order by placing the topmost overlay in the first element of the array.
			
	@leave	KErrNoMemory if out of memory; also any system wide error.		
	*/
	virtual void SetAllOverlaysInZOrderL(const RArray<TUint>& aOverlayHandles)=0;
	};
	

/**
This is a mixin class to be implemented by providers of the camera extension API for extra features for image overlays.

@see CCamera::CCameraOverlay

@publishedPartner
@prototype
*/
class MCameraOverlay2
	{
public:		
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/**
	Allows the overlay image data to be changed for a specified overlay. Ownership of the bitmap is passed to the 
	implementation.
	Use this function to set the overlay image data if it was not specified when the overlay was created using 
	CreateOverlayL().

	@param	aOverlayHandle
			The handle of the overlay whose overlay image data is to be changed.
	@param	aBitmap
			The new image data for the overlay.
		
	@leave  May leave with any error code.

	@note   If required, implementation is free to modify the overlay bitmap passed to it.
	
	@note   SetOverlayParametersL should not be called after this method for the given overlay since it may change the 
			overlay parameters considerably. In such a case, SetOverlayParametersL may leave with error KErrArgument.
	*/
	virtual void SetModifiableOverlayBitmapL(TUint aOverlayHandle, CFbsBitmap* aBitmap)=0;
	
	/**
	Gets all the overlay handles maintained by the ECam implementation, in order of their z-value, for a particular camera mode.
	If for viewfinder, then the handle number is used to get z-value for the viewfinder whose handle number is passed.
	
	@param  aOverlayCameraMode
			The specific camera mode whose overlays' z-value information is required.
			
	@param  aViewFinderHandle
			The specific viewfinder handle, if overlays' z-value information is required for viewfinder camera mode.
	
	@param 	aOverlayHandles
			Returned list, in z-value order, of all the overlay handles maintained on the ECam implementation.
			The topmost overlay is the first element of the array. 

	@leave  May leave with any error code.
	*/
	virtual void GetAllOverlaysInZOrderL(CCamera::CCameraOverlay::TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, RArray<TUint>& aOverlayHandles) const=0;
	
	/**
	Sets all the overlay handles maintained by the ECam implementation, in order of their z-value, for a particular camera mode.
	If for viewfinder, then the handle number is used to set the z-value for the viewfinder whose handle number is passed.
	
	@param  aOverlayCameraMode
			The specific camera mode whose overlays' z-value is to be set.
			
	@param  aViewFinderHandle
			The specific viewfinder handle, if overlays' z-value is required to be set for viewfinder camera mode.
	
	@param 	aOverlayHandles
			The overlay handles in aOverlayHandles array. This must be the complete current set 
			of handles known to this CCameraOverlay object for the given camera mode (and for given viewfinder, if applicable). 
			The client specifies the desired order by placing the topmost overlay in the first element of the array.

	@leave  May leave with any error code.
	*/
	virtual void SetAllOverlaysInZOrderL(CCamera::CCameraOverlay::TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, const RArray<TUint>& aOverlayHandles)=0;
	};

#endif // MCAMERAOVERLAY_H