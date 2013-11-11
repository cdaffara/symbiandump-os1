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
 @publishedAll
 @released
*/

#include <ecam/mcameraoverlay.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecam/cameraoverlayconst.h>
#endif
#include "ecamversion.h"

const TUint KBaselinedOverlayModeMask = (CCamera::CCameraOverlay::EModeVideo << 1) - 1;
const TUint KOverlayGlobalStillMode = (CCamera::CCameraOverlay::EModeStillImageContinuous 
									 | CCamera::CCameraOverlay::EModeStillImageBracket 
									 | CCamera::CCameraOverlay::EModeStillImageBracketMerge 
									 | CCamera::CCameraOverlay::EModeStillImageTimed 
									 | CCamera::CCameraOverlay::EModeStillImageTimeLapse 
									 | CCamera::CCameraOverlay::EModeStillImageBurst);
/**
Factory function that creates a new camera overlay object on the heap.

@param	aCamera
		A reference to the camera object for which a camera overlay object is to be created.

@leave  KErrNoMemory if out of memory; also any system wide error.

@return A pointer to the newly created camera overlay object.

@note  Clients using MCameraObserver are not recommended to use this extension class since they cannot handle events.
*/

EXPORT_C CCamera::CCameraOverlay* CCamera::CCameraOverlay::NewL(CCamera& aCamera)
	{
	CCamera::CCameraOverlay* self = new (ELeave) CCamera::CCameraOverlay(aCamera);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self; 
	}
	
/**
CCameraOverlay second phase constructor. 

This function used to initialise internal state of the object.
It uses reference to the camera to retrieve overlay interface pointer.

@leave KErrNotSupported if this functionality is not supported; also any system wide error.
*/ 
void CCamera::CCameraOverlay::ConstructL()
	{
	iImpl = static_cast<MCameraOverlay*>(iOwner.CustomInterface(KECamMCameraOverlayUid));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
		
	iImpl2 = static_cast<MCameraOverlay2*>(iOwner.CustomInterface(KECamMCameraOverlay2Uid));
	}

/**
Constructor for the CCamera::CCameraOverlay class.

@param aOwner
		A reference to the camera object for which a camera overlay object is to be created.
*/
CCamera::CCameraOverlay::CCameraOverlay(CCamera& aOwner):iOwner(aOwner), iImpl(NULL), iImpl2(NULL)
	{
	}

/**
Destructor for the CCamera::CCameraOverlay class.
*/
EXPORT_C CCamera::CCameraOverlay::~CCameraOverlay()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();	
		}
	if (iImpl2 != NULL)
		{
		iImpl2->Release();	
		}
	}

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
EXPORT_C TUint CCamera::CCameraOverlay::CreateOverlayL(const TOverlayParameters& aParameters, CFbsBitmap* aBitmap)
	{
	CCamera::CCameraOverlay::TOverlayParameters overlayParameters = aParameters;
	if (aParameters.iCurrentModes & EModeViewfinder)
		{
		overlayParameters.iCurrentModes |= EModeClientViewfinder;
		overlayParameters.iCurrentModes |= EModeDirectViewfinder;
		}
	if (aParameters.iCurrentModes & EModeStillImage)
		{
		overlayParameters.iCurrentModes |= KOverlayGlobalStillMode;
		}
	return iImpl->CreateOverlayL(overlayParameters, aBitmap);
	}

/**
Allows the overlay image data to be changed for a specified overlay.
Use this function to set the overlay image data if it was not specified when the overlay
was created using CreateOverlayL().

@param	aOverlayHandle
		The handle of the overlay whose overlay image data is to be changed.
@param	aBitmap
		The new image data for the overlay.
		
@leave	KErrArgument if aOverlayHandle is out of range; also any system wide error.

@note   Once this method is called, overlay size should not be changed for the given overlay. Hence, SetOverlayParametersL 
		should not be called after this method. 
*/
EXPORT_C void CCamera::CCameraOverlay::SetOverlayBitmapL(TUint aOverlayHandle, const CFbsBitmap* aBitmap)
	{
	iImpl->SetOverlayBitmapL(aOverlayHandle, aBitmap);
	}
	
/**
@publishedPartner
@prototype

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

@note   SetOverlayParametersL should not be called after this method for the given overlay since it may change the overlay
		parameters considerably. In such a case, SetOverlayParametersL may leave with error KErrArgument. 
*/
EXPORT_C void CCamera::CCameraOverlay::SetModifiableOverlayBitmapL(TUint aOverlayHandle, CFbsBitmap* aBitmap)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->SetModifiableOverlayBitmapL(aOverlayHandle, aBitmap);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Gets the overlay image data for a specified overlay.

@param	aOverlayHandle
		The handle of the overlay whose overlay image data is to be obtained.
@param	aBitmap
		A CFbsBitmap that will receive the returned image data for the overlay.
	
@leave	KErrArgument if aOverlayHandle is out of range; also any system wide error.

@leave  KErrNotSupported if a sharing client (which did not create the given overlay) tries to retrieve the overlay bitmap
		and the implementation may not be interested in providing the overlay.

@note   The ECam implementation will transfer the ownership of the aBitmap to the client.
*/
EXPORT_C void CCamera::CCameraOverlay::GetOverlayBitmapL(TUint aOverlayHandle, CFbsBitmap* aBitmap)
	{
	iImpl->GetOverlayBitmapL(aOverlayHandle, aBitmap);
	}

/**
Gets the parameters that characterize a given overlay. 

@param	aOverlayHandle
		The handle of the overlay whose parameters are required.
@param	aInfo
		Reference to TOverlayParameters object that will contain the returned overlay parameters.
			
@leave	KErrArgument if aOverlayHandle is out of range; also any system wide error.
*/
EXPORT_C void CCamera::CCameraOverlay::GetOverlayParametersL(TUint aOverlayHandle, TOverlayParameters& aInfo)
	{
	iImpl->GetOverlayParametersL(aOverlayHandle, aInfo);
	
	// for clients not using CCamera::New2L() turn new overlay modes into old ones
    if (iOwner.CameraVersion() == KCameraDefaultVersion)
	    {
        // specific viewfinder modes into "old" EModeViewfinder
	    if (aInfo.iCurrentModes & (EModeClientViewfinder | EModeDirectViewfinder))
	        {
	        aInfo.iCurrentModes |= EModeViewfinder;
	        }
	    // turn different drive modes into "old" EModeStillImage
	    if (aInfo.iCurrentModes & ( EModeStillImageContinuous | EModeStillImageBracket | 
	                                EModeStillImageBracketMerge | EModeStillImageTimed |
	                                EModeStillImageTimeLapse | EModeStillImageBurst
	                                )
	            )
	        {
	        aInfo.iCurrentModes |= EModeStillImage;
	        }
	    // for old clients we present only old set of features
	    aInfo.iCurrentModes &= KBaselinedOverlayModeMask;
	    }
	 else
	 	{
	 	if (aInfo.iCurrentModes & EModeViewfinder)
			{
			aInfo.iCurrentModes |= EModeClientViewfinder;
			aInfo.iCurrentModes |= EModeDirectViewfinder;
			}
		if (aInfo.iCurrentModes & EModeStillImage)
			{
			aInfo.iCurrentModes |= KOverlayGlobalStillMode;
			}
	 	}	
	}
		
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
EXPORT_C void CCamera::CCameraOverlay::SetOverlayParametersL(TUint aOverlayHandle, const TOverlayParameters& aParameters)
	{
	CCamera::CCameraOverlay::TOverlayParameters overlayParameters = aParameters;
	if (aParameters.iCurrentModes & EModeViewfinder)
		{
		overlayParameters.iCurrentModes |= EModeClientViewfinder;
		overlayParameters.iCurrentModes |= EModeDirectViewfinder;
		}
	if (aParameters.iCurrentModes & EModeStillImage)
		{
		overlayParameters.iCurrentModes |= KOverlayGlobalStillMode;
		}	
		
	iImpl->SetOverlayParametersL(aOverlayHandle, overlayParameters);
	}
	
/**
Releases the specified overlay handle.

@note 	If the handle specified in aOverlayHandle is invalid (out of range) the function 
		call is ignored and no error is reported.

@param	aOverlayHandle	
		The handle of the overlay that is to be released.
*/
EXPORT_C void CCamera::CCameraOverlay::ReleaseOverlay(TUint aOverlayHandle)
	{
	iImpl->ReleaseOverlay(aOverlayHandle);
	}
	
/**
Gets information on the overlay functionality supported by the ECam implementation.

@param	aInfo
		A reference to a TOverlaySupportInfo object that will receive the overlay support information.
*/
EXPORT_C void CCamera::CCameraOverlay::GetOverlaySupport(TOverlaySupportInfo& aInfo)
	{
	iImpl->GetOverlaySupport(aInfo);
	// we hide new overlay modes for clients not using New2L()/NewDuplicate2L()
    if (iOwner.CameraVersion() == KCameraDefaultVersion)
	    {
	    aInfo.iSupportedModes &= KBaselinedOverlayModeMask;
	    }
	else
		{
		if (aInfo.iSupportedModes & EModeViewfinder)
			{
			aInfo.iSupportedModes |= EModeClientViewfinder;
			aInfo.iSupportedModes |= EModeDirectViewfinder;
			}
		if (aInfo.iSupportedModes & EModeStillImage)
			{
			aInfo.iSupportedModes |= KOverlayGlobalStillMode;
			}
		}
	}

/**
Gets all the overlay handles maintained by the ECam implementation, in order of their Z-Value.
	
@param 	aOverlayHandles
		Returned list, in Z-Value order, of all the overlay handles maintained on the ECam implementation.
		The topmost overlay is the first element of the array. 

@leave	KErrNoMemory if out of memory; also any system wide error.	

@note 	Implementation shall give preference to the sequence in which the client has passed the overlay handles in case of 
		any mis-match with the 'z-order' of each such overlay. Implementation shall amend the 'z-order' of the overlays if
		required to remove any ambiguity.
*/
EXPORT_C void CCamera::CCameraOverlay::GetAllOverlaysInZOrderL(RArray<TUint>& aOverlayHandles)
	{
	iImpl->GetAllOverlaysInZOrderL(aOverlayHandles);
	}
	
/**
Sets the Z-Order of all the overlay handles known by this CCameraOverlay object.
	
@param 	aOverlayHandles
		The overlay handles in aOverlayHandles array. This must be the complete current set 
		of handles known to this CCameraOverlay object. The client specifies the desired 
		order by placing the topmost overlay in the first element of the array.
			
@leave	KErrNoMemory if out of memory; also any system wide error.	

@note 	Implementation shall give preference to the sequence in which the client has passed the overlay handles in case of 
		any mis-match with the 'z-order' of each such overlay. Implementation shall amend the 'z-order' of the overlays if
		required to remove any ambiguity.	
*/
EXPORT_C void CCamera::CCameraOverlay::SetAllOverlaysInZOrderL(const RArray<TUint>& aOverlayHandles)
	{
	iImpl->SetAllOverlaysInZOrderL(aOverlayHandles);
	}

/**
Constructor for the TOverlaySupportInfo class.
*/
EXPORT_C CCamera::CCameraOverlay::TOverlaySupportInfo::TOverlaySupportInfo()
	{
	iDesiredCameraMode = CCamera::CCameraOverlay::EModeNone;
	iViewFinderHandle = KECamOverlayInvalidViewFinderHandle;
	}
	
/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if an old application is made to run on a new implementation, this can be 
corrrectly handled if the derived class variables handling is done in a proper 'if-else' statement. 

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraOverlay::TOverlaySupportInfo::Size() const
	{
	return sizeof(CCamera::CCameraOverlay::TOverlaySupportInfo);
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of the class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraOverlay::TOverlaySupportInfo::Version() const
	{
	return KECamOverlaySupportInfoCurrentVersion;
	}
	
/**
Constructor for the TOverlayParameters class.
*/	
EXPORT_C CCamera::CCameraOverlay::TOverlayParameters::TOverlayParameters()
	{
	iViewFinderHandle = KECamOverlayInvalidViewFinderHandle;
	}
	
/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if an old application is made to run on a new implementation, this can be 
corrrectly handled if the derived class variables handling is done in a proper 'if-else' statement. 

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraOverlay::TOverlayParameters::Size() const
	{
	return sizeof(CCamera::CCameraOverlay::TOverlayParameters);
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of the class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraOverlay::TOverlayParameters::Version() const
	{
	return KECamOverlayParametersCurrentVersion;
	}

/**
@publishedPartner
@prototype

Gets all the overlay handles maintained by the ECam implementation, in order of their z-value, for a particular camera mode.
If for viewfinder, then the handle number is used to get the z-value for the viewfinder whose handle number is passed.

@param  aOverlayCameraMode
		The specific camera mode whose overlays' z-value information is required.
		
@param  aViewFinderHandle
		The specific viewfinder handle, if overlays' z-value information is required for viewfinder camera mode.

@param 	aOverlayHandles
		Returned list, in z-value order, of all the overlay handles maintained on the ECam implementation.
		The topmost overlay is the first element of the array. 

@leave  May leave with any error code.

@note 	Implementation shall give preference to the sequence in which the client has passed the overlay handles in case of 
		any mis-match with the 'z-order' of each such overlay. Implementation shall amend the 'z-order' of the overlays if
		required to remove any ambiguity.
*/
EXPORT_C void CCamera::CCameraOverlay::GetAllOverlaysInZOrderL(CCamera::CCameraOverlay::TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, RArray<TUint>& aOverlayHandles) const
	{
	if(iImpl2 != NULL)
		{
		iImpl2->GetAllOverlaysInZOrderL(aOverlayCameraMode, aViewFinderHandle, aOverlayHandles);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
@publishedPartner
@prototype

Sets all the overlay handles maintained by the ECam implementation, in order of their z-value for a particular camera mode.
If for viewfinder, then the handle number is used to set the z-value for the viewfinder whose handle number is passed.

@param  aOverlayCameraMode
		The specific camera mode whose overlays' z-value is to be set.
		
@param  aViewFinderHandle
		The specific viewfinder handle, if overlays' z-value is required to be set for viewfinder camera mode.

@param 	aOverlayHandles
		The overlay handles in aOverlayHandles array. This must be the complete current set 
		of handles known to this CCameraOverlay object for the given camera mode (and for the given viewfinder, if applicable). 
		The client specifies the desired order by placing the topmost overlay in the first element of the array.

@leave  May leave with any error code.

@note 	Implementation shall give preference to the sequence in which the client has passed the overlay handles in case of 
		any mis-match with the 'z-order' of each such overlay. Implementation shall amend the 'z-order' of the overlays if
		required to remove any ambiguity.
*/
EXPORT_C void CCamera::CCameraOverlay::SetAllOverlaysInZOrderL(CCamera::CCameraOverlay::TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, const RArray<TUint>& aOverlayHandles)
	{
	if(iImpl2 != NULL)
		{
		iImpl2->SetAllOverlaysInZOrderL(aOverlayCameraMode, aViewFinderHandle, aOverlayHandles);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}
