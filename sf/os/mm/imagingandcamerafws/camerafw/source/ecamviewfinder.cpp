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

#include <ecamviewfinder.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamviewfinderdef.h>
#endif
#include <ecam/camerahistogram.h>
#include <ecam/ecamviewfinderintf.h>
#include <ecam/implementationfactoryintf.h>
#include "ecamversion.h"

/**	
CCameraViewFinder Constructor.

@param aOwner
       a reference to a CCamera object providing the settings. 

@internalComponent
@prototype
*/
CCameraViewFinder::CCameraViewFinder(CCamera& aOwner): iOwner(aOwner), iImplBase(NULL)
	{
	}

/**
Destructor
*/	
CCameraViewFinder::~CCameraViewFinder()
	{
	}

/**
Retrieves the fading capabilites for a particular viewfinder.

@param  aVFFadingCapabilities
		Retrieves the fading capabilities associated with the given viewfinder.
		
@leave  May leave with any error code.

@internalTechnology
*/
EXPORT_C void CCameraViewFinder::GetViewFinderFadingCapabilitiesL(CCameraViewFinder::TViewFinderFadingCapabilities& aVFFadingCapabilities) const
	{
	if(iImplBase != NULL)
		{
		iImplBase->GetViewFinderFadingCapabilitiesL(aVFFadingCapabilities);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	}

/**
Retrieves the current fading effect/control being used for a particular viewfinder.

@param  aCurrentVFFadingEffect
		Retrieves the fading effects associated with the given viewfinder.
		
@leave  May leave with any error code.

@internalTechnology
*/
EXPORT_C void CCameraViewFinder::GetViewFinderFadingEffectL(CCameraViewFinder::TViewFinderFadingEffect& aCurrentVFFadingEffect) const
	{
	if(iImplBase != NULL)
		{
		iImplBase->GetViewFinderFadingEffectL(aCurrentVFFadingEffect);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Sets the fading effect/control for a particular viewfinder.

@param  aVFFadingEffect
		The desired fading effects associated with the given viewfinder.
	
@leave  KErrNotSupported if the implementation of this method is not present.
		
@note   Event KUidECamEvent2ViewFinderFadingEffect is used to notify the requesting client about the setting of the 
		fading effect.
		
@internalTechnology
*/
EXPORT_C void CCameraViewFinder::SetViewFinderFadingEffectL(const CCameraViewFinder::TViewFinderFadingEffect& aVFFadingEffect)
	{
	if(iImplBase != NULL)
		{
		iImplBase->SetViewFinderFadingEffect(aVFFadingEffect);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}
	
/**
Retrieves the handle number which represents the given viewfinder object. 

@param  aVFHandle
		The viewfinder handle
		
@leave  May leave with any error code.

@note   KECamDefaultViewFinderHandle is used to refer to the viewfinder started using CCamera methods. 
*/
EXPORT_C void CCameraViewFinder::GetViewFinderHandleL(TInt& aVFHandle) const
	{
	if(iImplBase != NULL)
		{
		iImplBase->GetViewFinderHandleL(aVFHandle);	
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

/**
Constructor for the TViewFinderFadingCapabilities class.
Sets the size and version of this class.

@internalTechnology
*/
EXPORT_C CCameraViewFinder::TViewFinderFadingCapabilities::TViewFinderFadingCapabilities()
	{
	iSize = sizeof(CCameraViewFinder::TViewFinderFadingCapabilities);
	iVersion = KECamViewFinderFadingCapabilitiesCurrentVersion;
	}
	
/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this can be 
handled correctly if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of the class.

@note The size will be modified when the T-class gets updated.

@internalTechnology
*/
EXPORT_C TUint CCameraViewFinder::TViewFinderFadingCapabilities::Size() const
	{
	return iSize;
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of the class.

@note The version will be modified when the T-class gets updated.

@internalTechnology
*/
EXPORT_C TUint CCameraViewFinder::TViewFinderFadingCapabilities::Version() const
	{
	return iVersion;
	}
	
/**
Constructor for the TViewFinderFadingEffect class.
Sets the size and version of this class.

@internalTechnology
*/
EXPORT_C CCameraViewFinder::TViewFinderFadingEffect::TViewFinderFadingEffect()
	{
	iSize = sizeof(CCameraViewFinder::TViewFinderFadingEffect);
	iVersion = KECamViewFinderFadingEffectCurrentVersion;
	}
	
/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this can be 
handled correctly if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of the class.

@note The size will be modified when the T-class gets updated.

@internalTechnology
*/
EXPORT_C TUint CCameraViewFinder::TViewFinderFadingEffect::Size() const
	{
	return iSize;
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of the class.

@note The version will be modified when the T-class gets updated.

@internalTechnology
*/
EXPORT_C TUint CCameraViewFinder::TViewFinderFadingEffect::Version() const
	{
	return iVersion;
	}
	
/**
Retrieves the position of spots for a given spot combination which is to be displayed on a particular viewfinder.

@param  aSpotCombination
		A given spot combination. It is a bitfield where every bit represents the presence or absence of a particular spot.
		
@param  aSpotsPosition
		An array of TRect which represents the position of every spot present in the given spot combination. 
		The first member of this array maps the first spot present in the given spot combination from the least significant bit side.
		The position of a spot is relative to the top-left corner of viewfinder.

@leave  May leave with any error code.
		
@note   These TRect specify the position of the spots relative to the targeted viewfinder space. Note 
        that spots could also be represented as points since point is a special case of rectangle.
		
@see 	CCamera::CCameraImageCaptureControl::GetMaximumSpotsL
@see	CCamera::CCameraImageCaptureControl::GetSupportedSpotsCombinationL
@see    CCamera::CCameraImageCaptureControl::GetSpotsCombinationL
@see    CCamera::CCameraImageCaptureControl::SetSpotsCombination
*/
void CCameraViewFinder::GetSpotsPositionL(TUint /*aSpotCombination*/, RArray<TRect>& /*aSpotsPosition*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Retrieves the supported image enhancements.

@param  aSupportedImageEnhancement
		A bitfield of all supported TImageEnhancement to be provided to the client.

@leave  May leave with any error code.
*/
void CCameraViewFinder::GetSupportedImageEnhancementL(TUint& /*aSupportedImageEnhancement*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Get the current image enhancement for the particular viewfinder.

@param  aImageEnhancement
		Currently used TImageEnhancement for the viewfinder display.
		
@leave  May leave with any error code. 
*/
void CCameraViewFinder::GetImageEnhancementL(CCameraViewFinder::TImageEnhancement& /*aImageEnhancement*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
/**
Set the image enhancement for a particular viewfinder.

@param  aImageEnhancement 
		The TImageEnhancement value to be set
		
@leave  KErrNotSupported If the implementation of this method is not present.

@note   Event KUidECamEvent2ViewFinderImageEnhancement is used to notify clients about the image enhancement setting operation.
*/
void CCameraViewFinder::SetImageEnhancementL(CCameraViewFinder::TImageEnhancement /*aImageEnhancement*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the supported viewfinder magnification factor in terms of percentage. Changing the magnification factor allows 
the user to view more or less area than what would normally appear on the captured image.

@param	aSupportedVFMagnification
		An array of TUint which retrieves the supported magnification factor. Every member of this array is a 
		magnification factor and represents a percentage unit. Note that every array member is multiplied by 
		KECamFineResolutionFactor to support fractional percentages as well. For example, 90.5% will be represented by 9050.
		
@leave  May leave with any error code.
*/
void CCameraViewFinder::GetSupportedVFMagnificationL(RArray<TUint>& /*aSupportedVFMagnification*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Retrieves the current magnification factor for a particular viewfinder.

@param  aVFMagnification
		The current magnification factor used for the given viewfinder. It represents a percentage unit. 
		Note that the magnification factor is multiplied by KECamFineResolutionFactor to support fractional 
		percentages as well. For example, 90.5% will be represented by 9050.
		
@leave  May leave with any error code.
*/
void CCameraViewFinder::GetViewfinderMagnificationL(TUint& /*aVFMagnification*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Set a particular magnification factor for the given viewfinder.

@param  aVFMagnification
		The magnification factor to be set. It represents a percentage unit. 
		Note that the magnification factor is multiplied by KECamFineResolutionFactor to support fractional 
		percentages as well. For example, 90.5% will be represented by 9050.
		
@leave  KErrNotSupported If the implementation of this method is not present.

@note   Event KUidECamEvent2ViewFinderMagnification is used to notify the requesting client about the setting of the 
		magnification factor.
*/
void CCameraViewFinder::SetViewfinderMagnificationL(TUint /*aVFMagnification*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Factory function for creating the CCameraV2DirectViewFinder object.

@param aCamera 
	   A reference to a CCamera object providing the settings.
	   
@param aDirectViewFinderObserver
	   Reference to the direct view finder observer.

@return A pointer to a fully constructed CCameraV2DirectViewFinder object.

@leave KErrNoMemory Out of memory Or any other system-wide error code.

@leave KErrExtensionNotSupported When NewL/NewDuplicateL used instead of New2L/NewDuplicate2L.
*/
EXPORT_C CCamera::CCameraV2DirectViewFinder* CCamera::CCameraV2DirectViewFinder::NewL(CCamera& aCamera, MDirectViewFinderObserver& aDirectViewFinderObserver)
	{
 	if(aCamera.CameraVersion() == KCameraDefaultVersion)
 		{
 		User::Leave(KErrExtensionNotSupported);
 		}
 		
	CCamera::CCameraV2DirectViewFinder* self = new (ELeave)CCamera::CCameraV2DirectViewFinder(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL(aDirectViewFinderObserver);
	CleanupStack::Pop(self);
	return self; 
	}
	
	
void CCamera::CCameraV2DirectViewFinder::ConstructL(MDirectViewFinderObserver& aDirectViewFinderObserver)
	{
	SetImplHandle();
	if(iImpl == NULL)
		{
		User::LeaveIfError(KErrNotSupported);		
		}
	SetImplBaseHandle();
	
	iImpl->SetDirectViewFinderObserver(aDirectViewFinderObserver);
	iImpl->SetDirectViewFinderHandle(this);
	}
	
/**	
CCameraV2DirectViewFinder Constructor.

@param aOwner
       A reference to a CCamera object providing the settings. 
*/
CCamera::CCameraV2DirectViewFinder::CCameraV2DirectViewFinder(CCamera& aOwner): CCameraViewFinder(aOwner),iImpl(NULL)
	{
	}

/**
Destructor
@note  The child objects created out of this direct viewfinder class object shall be delete beforehand. Various child 
	   objects are snapshot and histograms.
*/	
EXPORT_C CCamera::CCameraV2DirectViewFinder::~CCameraV2DirectViewFinder()
	{
	if (iImpl != NULL)
		{
		iImpl->Release(this);
		}
		
	if (iImplBase != NULL)
		{
		iImplBase->Release();
		}
	}	
	
/**
Retrieve pointer to histogram API in order to use it specifically for a particular viewfinder.

@return Pointer to use histogram API specifically for the given viewfinder.
		
@leave  May leave with any error code.

@note   Different types of histogram may be used for a specific viewfinder. Every time this method will be called
		on the CCameraV2DirectViewFinder class object, a new type of histogram will be created.
*/	
EXPORT_C CCamera::CCameraV2Histogram* CCamera::CCameraV2DirectViewFinder::CreateHistogramHandleL() const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->CreateHistogramImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraV2Histogram* histogram = CCamera::CCameraV2Histogram::CreateL(iOwner, *implFactory);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	return histogram;	
	}

/**
Retrieve pointer to image processing API in order to use it specifically for a particular viewfinder.

@return Pointer to use image processing API specifically for the given viewfinder.
		
@leave  May leave with any error code.
*/
EXPORT_C CCamera::CCameraImageProcessing* CCamera::CCameraV2DirectViewFinder::GetTransformationHandleL() const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->GetImageProcessingImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraImageProcessing* imgProc = CCamera::CCameraImageProcessing::CreateL(iOwner, *implFactory);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	return imgProc;	
	}

/**
Retrieve pointer to direct snapshot API in order to use it for displaying the snapshots for still images directly on the 
given direct viewfinder screen. 

@return Pointer to use direct snapshot API specifically for the given viewfinder.
		
@leave  May leave with any error code.

@note  Direct snapshot API is not supposed to be used for snapshots for video.
*/
EXPORT_C CCamera::CCameraDirectSnapshot* CCamera::CCameraV2DirectViewFinder::GetDirectSnapshotHandleL() const
	{
	return CCamera::CCameraDirectSnapshot::CreateL(const_cast<CCamera::CCameraV2DirectViewFinder&>(*this));
	}

/** 
Creates a new viewfinder and starts transfer of view finder data to the given portion of the screen using direct 
screen access.

The aScreenRect parameter is in screen coordinates and may be modified if,
for example, the camera requires the destination to have a certain byte alignment, and so on.

@param  aWs 
        Window server session.
@param  aScreenDevice 
        Screen device.
@param  aWindow 
        Displayable window.
@param  aScreenRect 
        Portion of the screen to which view finder data is to be
        transferred. This is in screen co-ordinates and may be modified if, for example,
        the camera requires the destination to have a certain byte alignment.

@leave  KErrInUse if another direct viewfinder is running with the same set of window parameters. May leave with any other error code.

@note   MDirectViewFinderObserver::DirectViewFinderFirstFrameDisplayed() will be used to notify clients that the first 
		frame has been displayed.
*/
EXPORT_C void CCamera::CCameraV2DirectViewFinder::StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, 
										RWindowBase& aWindow, TRect& aScreenRect)
	{
	iImpl->StartViewFinderDirectL(aWs, aScreenDevice, aWindow, aScreenRect);	
	}

/** 
Creates a new viewfinder and starts transfer of view finder data to the given portion of the screen using direct screen
access and also clips to the specified portion of the screen.

The view finder has the same size and position as aScreenRect but is only
visible in the intersection of aScreenRect and aClipRect. May leave with KErrNotSupported
or KErrNotReady if Reserve() has not been called, or has not yet completed.

@param  aWs 
        Window server session.
@param  aScreenDevice 
        Screen device.
@param  aWindow 
        Displayable window.
@param  aScreenRect 
        Portion of the screen to which view finder data is to be
        transferred. This is in screen coordinates and may be modified if, for example,
        the camera requires the destination to have a certain byte alignment.
@param  aClipRect
        The rectangle to which the screen will be clipped.

@leave  KErrInUse if another direct viewfinder is running with the same set of window parameters. May leave with any other error code.

@note   MDirectViewFinderObserver::DirectViewFinderFirstFrameDisplayed() will be used to notify clients that the first 
		frame has been displayed.
*/									
EXPORT_C void CCamera::CCameraV2DirectViewFinder::StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice,
										RWindowBase& aWindow, TRect& aScreenRect, TRect& aClipRect)
	{
	iImpl->StartViewFinderDirectL(aWs, aScreenDevice, aWindow, aScreenRect, aClipRect);	
	}

/**
Retrieves the parameters associated with the direct viewfinder.

@param  aScreenNumber
		Retrieves the screen number on which the direct viewfinder is supposed to run.
		
@param  aScreenRect
		Retrieves the TRect which identifies the portion of the screen to which view finder data is to be transferred.

@param  aClipRect
		Retrieves the TRect which identifies the rectangle to which the screen will be clipped.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraV2DirectViewFinder::GetDirectViewFinderPropertiesL(TInt& aScreenNumber, TRect& aScreenRect, 
																						TRect& aClipRect) const
	{
	iImpl->GetDirectViewFinderPropertiesL(aScreenNumber, aScreenRect, aClipRect);	
	}

/**
Pauses the direct viewfinder.
*/
EXPORT_C void CCamera::CCameraV2DirectViewFinder::PauseViewFinderDirect()
	{
	iImpl->PauseViewFinderDirect();	
	}

/**
Resumes the direct viewfinder

@note   MDirectViewFinderObserver::DirectViewFinderFirstFrameDisplayed() will be used to notify clients that the first
		frame has been displayed.
*/
EXPORT_C void CCamera::CCameraV2DirectViewFinder::ResumeViewFinderDirect()
	{
	iImpl->ResumeViewFinderDirect();	
	}

/**
Retrieves the state of the direct viewfinder.

@param  aViewFinderState
		Retrieves the state of the direct viewfinder.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraV2DirectViewFinder::GetViewFinderStateL(CCamera::CCameraV2DirectViewFinder::TViewFinderState& aViewFinderState) const
	{
	iImpl->GetViewFinderStateL(aViewFinderState);	
	}

/**
Stops the direct viewfinder.
*/
EXPORT_C void CCamera::CCameraV2DirectViewFinder::StopDirectViewFinder()
	{
	iImpl->StopDirectViewFinder();	
	}

/** 
Gets the handle to the implementer's MCameraV2DirectViewFinder derived class.
*/	
void CCamera::CCameraV2DirectViewFinder::SetImplHandle()
	{
	if(iImpl == NULL)
		{
		iImpl = static_cast<MCameraV2DirectViewFinder*>(iOwner.CustomInterface(KECamMCameraV2DirectViewFinderUid));
		}
	}

/** 
Gets the handle to the implementer's MCameraViewFinder derived class for v2 direct viewfinder.
*/	
void CCamera::CCameraV2DirectViewFinder::SetImplBaseHandle()
	{
	if(iImplBase == NULL)
		{
		iImplBase = static_cast<MCameraViewFinder*>(iOwner.CustomInterface(KECamMCameraBaseV2DirectViewFinderUid));
		}
	}
/**
Provides the interface pointer for the MCameraV2DirectViewFinder implementation.

@return  Interface pointer for MCameraV2DirectViewFinder implementation.
*/	
MCameraV2DirectViewFinder* CCamera::CCameraV2DirectViewFinder::Impl()
	{
	return iImpl;
	}

/**
Factory function for creating the CCameraClientViewFinder object.

@param aCamera 
	   A reference to a CCamera object providing the settings.
	   
@param aClientViewFinderObserver
	   Reference to the client view finder observer.

@return A pointer to a fully constructed CCameraClientViewFinder object.

@leave KErrNoMemory Out of memory or any other system-wide error code.

@leave KErrExtensionNotSupported When NewL/NewDuplicateL used instead of New2L/NewDuplicate2L.
*/
EXPORT_C CCamera::CCameraClientViewFinder* CCamera::CCameraClientViewFinder::NewL(CCamera& aCamera, MClientViewFinderObserver& aClientViewFinderObserver)
	{
 	if(aCamera.CameraVersion() == KCameraDefaultVersion)
 		{
 		User::Leave(KErrExtensionNotSupported);
 		}
 		
	CCamera::CCameraClientViewFinder* self = new (ELeave)CCamera::CCameraClientViewFinder(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL(aClientViewFinderObserver);
	CleanupStack::Pop(self);
	return self; 
	}
		
void CCamera::CCameraClientViewFinder::ConstructL(MClientViewFinderObserver& aClientViewFinderObserver)
	{
	SetImplHandle();
	if(iImpl == NULL)
		{
		User::LeaveIfError(KErrNotSupported);		
		}
	SetImplBaseHandle();
	
	iImpl->SetClientViewFinderObserver(aClientViewFinderObserver);
	iImpl->SetClientViewFinderHandle(this);
	}
	
/**	
CCameraClientViewFinder Constructor.

@param aOwner
       a reference to a CCamera object providing the settings. 
*/
CCamera::CCameraClientViewFinder::CCameraClientViewFinder(CCamera& aOwner): CCameraViewFinder(aOwner), iImpl(NULL)
	{
	}
	
/**
Retrieve pointer to histogram API in order to use it specifically for a particular viewfinder.

@return Pointer to use histogram API specifically for the given viewfinder.
		
@leave  May leave with any error code.

@note   Different types of histogram may be used for a specific viewfinder. Every time this method will be called
		on the CCameraClientViewFinder class object, a new type of histogram will be created.
*/	
EXPORT_C CCamera::CCameraV2Histogram* CCamera::CCameraClientViewFinder::CreateHistogramHandleL() const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->CreateHistogramImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraV2Histogram* histogram = CCamera::CCameraV2Histogram::CreateL(iOwner, *implFactory);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	return histogram;	
	}

/**
Retrieve pointer to image processing API in order to use it specifically for a particular viewfinder.

@return Pointer to use image processing API specifically for the given viewfinder.
		
@leave  May leave with any error code.
*/
EXPORT_C CCamera::CCameraImageProcessing* CCamera::CCameraClientViewFinder::GetTransformationHandleL() const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->GetImageProcessingImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraImageProcessing* imgProc = CCamera::CCameraImageProcessing::CreateL(iOwner, *implFactory);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	return imgProc;	
	}

/** 
Creates a new viewfinder and starts transfer of view finder data.

Availability of viewfinder data is notified by MClientViewFinderObserver::ViewFinderBufferReady().

@param  aScreenNumber
		The screen number on which the client viewfinder is supposed to run.
@param  aImageFormat 
        The image format requested by the client.
@param  aSize 
        On return, the size used. The size may be modified by the implementation because of hardware constraints and 
        byte alignment.

@leave  May leave with any error code.

@note   KECamDefaultViewFinderHandle is used to refer to the viewfinder started using CCamera methods.

@see	CCamera::StartViewFinderL(TFormat aImageFormat,TSize& aSize)
*/
EXPORT_C void CCamera::CCameraClientViewFinder::StartClientViewFinderL(TInt aScreenNumber, CCamera::TFormat aImageFormat, TSize& aSize)
	{
	iImpl->StartClientViewFinderL(aScreenNumber, aImageFormat, aSize);
	}

/** 
Creates a new viewfinder and starts transfer of view finder data. Also, clips the picture to the specified clip	rectangle. 

Availability of viewfinder data is notified by MClientViewFinderObserver::ViewFinderBufferReady().

The picture is the size of the intersection of aSize and aClipRect, not simply
aSize padded with white space.

@param  aScreenNumber
		The screen number on which the client viewfinder is supposed to run.
@param  aImageFormat 
        The image format.
@param  aSize
        On return, the size used. The size may be modified by the implementation because of hardware constraints and
        byte alignment.
@param  aClipRect 
        Required clip rectangle. May be modified if, for example,
        the camera only supports certain byte alignments.

@leave  May leave with any error code.

@note   KECamDefaultViewFinderHandle is used to refer viewfinder started using CCamera methods.

@see	CCamera::StartViewFinderL(TFormat aImageFormat,TSize& aSize,TRect& aClipRect)
*/
EXPORT_C void CCamera::CCameraClientViewFinder::StartClientViewFinderL(TInt aScreenNumber, CCamera::TFormat aImageFormat, TSize& aSize, TRect& aClipRect)
	{
	iImpl->StartClientViewFinderL(aScreenNumber, aImageFormat, aSize, aClipRect);
	}

/**
Retrieves the state of the client viewfinder.

@param  aIsActive
		ETrue indicates that client viewfinder is active.
		EFalse indicates that client viewfinder is not active.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraClientViewFinder::GetClientViewFinderStateL(TBool& aIsActive) const
	{
	iImpl->GetClientViewFinderStateL(aIsActive);	
	}

/**
Retrieves the parameters associated with the client viewfinder.

@param  aScreenNumber
		Retrieves the screen number on which the client viewfinder is supposed to run.
		
@param  aImageFormat
		Retrieves the image format of the client viewfinder.

@param  aSize
		Retrieves the size used for the client viewfinder.
		
@param	aClipRect
		Retrieves the clip rectangle. If not specified, the clipping is retrieved as a TRect having origin as top left 
		corner and the size as 'aSize'.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraClientViewFinder::GetClientViewFinderPropertiesL(TInt& aScreenNumber, CCamera::TFormat& aImageFormat, TSize& aSize, TRect& aClipRect) const
	{
	iImpl->GetClientViewFinderPropertiesL(aScreenNumber, aImageFormat, aSize, aClipRect);
	}

/**
Stop the client viewfinder.
*/
EXPORT_C void CCamera::CCameraClientViewFinder::StopClientViewFinder()
	{
	iImpl->StopClientViewFinder();
	}

/**
Retrieves the client viewfinder buffer. Client calls this method after being notified about availability of viewfinder
data via MClientViewFinderObserver::ViewFinderBufferReady().

@param  aClientViewFinderBuffer
		Reference to MCameraBuffer2 which retrieves the client viewfinder data.

@leave  May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraClientViewFinder::GetViewFinderBufferL(MCameraBuffer2& aClientViewFinderBuffer) const
	{
	iImpl->GetViewFinderBufferL(aClientViewFinderBuffer);	
	}

/**
Destructor
@note  The child objects created out of this client viewfinder class object shall be delete beforehand. Various child 
	   objects are snapshot and histograms.
*/	
EXPORT_C CCamera::CCameraClientViewFinder::~CCameraClientViewFinder()
	{
	if (iImpl != NULL)
		{
		iImpl->Release(this);
		}
		
	if (iImplBase != NULL)
		{
		iImplBase->Release();
		}
	}

/** 
Gets the handle to the implementer's MCameraClientViewFinder derived class.
*/	
void CCamera::CCameraClientViewFinder::SetImplHandle()
	{
	if(iImpl == NULL)
		{
		iImpl = static_cast<MCameraClientViewFinder*>(iOwner.CustomInterface(KECamMCameraClientViewFinderUid));
		}
	}

/** 
Gets the handle to the implementer's MCameraViewFinder derived class for client viewfinder.
*/	
void CCamera::CCameraClientViewFinder::SetImplBaseHandle()
	{
	if(iImplBase == NULL)
		{
		iImplBase = static_cast<MCameraViewFinder*>(iOwner.CustomInterface(KECamMCameraBaseClientViewFinderUid));
		}
	}

/**
@internalComponent

Factory function that creates a new camera direct snapshot object.

@param	aDirectViewFinder
		A reference to the camera direct viewfinder object for which a camera direct snapshot object is to be created.
		
@leave  KErrNoMemory if out of memory; also any system wide error.

@return A pointer to a fully constructed camera direct snapshot object.
*/	
EXPORT_C CCamera::CCameraDirectSnapshot* CCamera::CCameraDirectSnapshot::CreateL(CCamera::CCameraV2DirectViewFinder& aDirectViewFinder)
	{
	CCamera::CCameraDirectSnapshot* self = new (ELeave) CCamera::CCameraDirectSnapshot();
	CleanupStack::PushL(self);
	self->ConstructL(aDirectViewFinder);
	CleanupStack::Pop(self);
	return self; 
	}

/**
@internalComponent

CCameraDirectSnapshot second phase constructor 

Function used to initialise internal state of the object. 

@param	aDirectViewFinder
		A reference to the camera direct viewfinder object on which the direct snapshot will be displayed.

@leave KErrNoMemory Out of memory; or any other error code as well.

@note This method is supposed to be used by this class only.
*/ 
void CCamera::CCameraDirectSnapshot::ConstructL(CCamera::CCameraV2DirectViewFinder& aDirectViewFinder) 
	{
	iImpl = static_cast<MCameraDirectSnapshot*>(DirectViewFinderImpl(aDirectViewFinder)->GetDirectSnapshotImpl(KECamMCameraDirectSnapshotUid));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
		
	iImpl->SetDirectViewFinder(&aDirectViewFinder);
	}
	
/**
Constructor for the CCameraDirectSnapshot class.
*/
CCamera::CCameraDirectSnapshot::CCameraDirectSnapshot():iImpl(NULL)
	{
	}

/**
Provides the interface pointer for the concrete implementation of CCameraV2DirectViewFinder. 

@param	aDirectViewFinder
		A reference to the camera direct viewfinder object

@return interface pointer for the concrete implementation of CCameraV2DirectViewFinder.
*/
MCameraV2DirectViewFinder* CCamera::CCameraDirectSnapshot::DirectViewFinderImpl(CCamera::CCameraV2DirectViewFinder& aDirectViewFinder)
	{
	return aDirectViewFinder.Impl();
	}
	
/**
Destructor
*/
EXPORT_C CCamera::CCameraDirectSnapshot::~CCameraDirectSnapshot()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();
		}
	}

/**
Retrieve the version2 direct viewfinder object out of which this direct snapshot has been created. This information may be
required by the client in order to map the direct snapshot object with its parent version2 direct viewfinder object or also
to retrieve properties of its parent version2 direct viewfinder object.

@param	aDirectViewFinder
		A pointer to the camera version2 direct viewfinder object on which the direct snapshot will be displayed.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraDirectSnapshot::GetDirectViewFinderL(CCamera::CCameraV2DirectViewFinder*& aDirectViewFinder) const
	{
	iImpl->GetDirectViewFinderL(aDirectViewFinder);
	}

/**
Enable the direct snapshot. The direct snapshot will be displayed on the parent direct viewfinder screen. The screen size
of the snapshot will be the same as that of the parent direct viewfinder screen.

The direct snapshot can be displayed only for still images.

@param  aDirectSnapshotParameters
		The desired direct snapshot parameter.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraDirectSnapshot::EnableDirectSnapshotL(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters)
	{
	iImpl->EnableDirectSnapshotL(aDirectSnapshotParameters);
	}

/**
Disable the direct snapshot. The direct snapshot will not be displayed on the parent direct viewfinder screen.
*/
EXPORT_C void CCamera::CCameraDirectSnapshot::DisableDirectSnapshot()
	{
	iImpl->DisableDirectSnapshot();
	}

/**
Retrieves the current state for direct snapshot.

@param  aDirectSnapshotParameters
		Retrieves the current state for direct snapshot.
		
@leave  May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraDirectSnapshot::GetDirectSnapshotStateL(CCamera::CCameraDirectSnapshot::TDirectSnapshotState& aDirectSnapshotState) const
	{
	iImpl->GetDirectSnapshotStateL(aDirectSnapshotState);
	}

/**
Retrieves the direct snapshot parameters. 

@param  aDirectSnapshotParameters
		Retrieves the direct snapshot parameter of type TDirectSnapshotParameters.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraDirectSnapshot::GetDirectSnapshotParametersL(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters) const
	{
	iImpl->GetDirectSnapshotParametersL(aDirectSnapshotParameters);	
	}

/**
Sets the direct snapshot parameters. 

@param  aDirectSnapshotParameters
		The desired direct snapshot parameter of type TDirectSnapshotParameters.
		
@leave  May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraDirectSnapshot::SetDirectSnapshotParametersL(const CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters)
	{
	iImpl->SetDirectSnapshotParametersL(aDirectSnapshotParameters);		
	}

/**
Constructor for the TDirectSnapshotParameters class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters::TDirectSnapshotParameters()
	{
	iSize = sizeof(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters);
	iVersion = KECamViewFinderFadingEffectCurrentVersion;
	}
	
/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this can be 
handled correctly if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/	
EXPORT_C TUint CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters::Size() const
	{
	return iSize;
	};

/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of the class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters::Version() const
	{
	return iVersion;
	};
		
/** 
Indicates whether the aspect ratio of the direct snapshot image has to maintained (if ETrue) or not (if EFalse) 
while scaling down.

@return TBool: ETrue implies aspect ratio has to be maintained, EFalse otherwise.

@see CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters::iIsAspectRatioMaintained
*/
EXPORT_C TBool CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters::IsAspectRatioMaintained() const
	{
	if (iIsAspectRatioMaintained)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}
	
/** 
Sets the state to inform whether the aspect ratio of the direct snapshot image has to be maintained or not while scaling
down.

@param  aIsAspectRatioMaintained
		ETrue implies must be maintained, EFalse otherwise.
		
@see CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters::iIsAspectRatioMaintained
*/
EXPORT_C void CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters::SetAspectRatioState(TBool aIsAspectRatioMaintained)
	{
	iIsAspectRatioMaintained = static_cast<TUint>(aIsAspectRatioMaintained);
	}
