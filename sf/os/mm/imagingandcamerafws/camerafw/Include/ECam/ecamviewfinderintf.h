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
 @publishedPartner
 @prototype
*/

#ifndef  ECAMVIEWFINDERINTF_H
#define  ECAMVIEWFINDERINTF_H

#include <ecamviewfinder.h>
#include <ecam/ecamadvsettingsintfuids.hrh>

class MImplementationFactory;

/** This is the UID which is used to obtain the interface MCameraV2DirectViewFinder, via the 
CCamera::CustomInterface() call. */   
static const TUid KECamMCameraV2DirectViewFinderUid 		=  {KECamMCameraV2DirectViewFinderUidValue};

/** This is the UID which is used to obtain the interface MCameraClientViewFinder, via the 
CCamera::CustomInterface() call. */   
static const TUid KECamMCameraClientViewFinderUid 			=  {KECamMCameraClientViewFinderUidValue};

/** This is the UID which is used to obtain the interface MCameraViewFinder for v2 direct viewfinder, via the 
CCamera::CustomInterface() call. */   
static const TUid KECamMCameraBaseV2DirectViewFinderUid		=  {KECamMCameraBaseV2DirectViewFinderUidValue};

/** This is the UID which is used to obtain the interface MCameraViewFinder for client viewfinder, via the 
CCamera::CustomInterface() call. */   
static const TUid KECamMCameraBaseClientViewFinderUid 		=  {KECamMCameraBaseClientViewFinderUidValue};

/** 
This is the UID which is used to obtain the MCameraDirectSnapshot interface, 
via a call to CCamera::CustomInterface(), which provides implementation of the M-class interface. 

@see KECamMCameraDirectSnapshotUidValue

@publishedPartner
@prototype
*/   
static const TUid KECamMCameraDirectSnapshotUid = {KECamMCameraDirectSnapshotUidValue};

/** Mixin class for implementation of viewfinder methods common to both client and direct viewfinder. These methods will 
	be called in the context of either client viewfinder or direct viewfinder.
*/	
class MCameraViewFinder
	{

public:
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/**
	Retrieves the fading capabilites for a particular viewfinder.
	
	@param  aVFFadingCapabilities
			Retrieves the fading capabilities associated with the given viewfinder.
			
	@leave  May leave with any error code.
	
	@internalTechnology
	*/
	virtual void GetViewFinderFadingCapabilitiesL(CCameraViewFinder::TViewFinderFadingCapabilities& aVFFadingCapabilities) const=0;
	
	/**
	Retrieves the current fading effect/control being used for a particular viewfinder.
	
	@param  aCurrentVFFadingEffect
			Retrieves the fading effects associated with the given viewfinder.

	@leave  May leave with any error code.
	
	@internalTechnology
	*/
	virtual void GetViewFinderFadingEffectL(CCameraViewFinder::TViewFinderFadingEffect& aCurrentVFFadingEffect) const=0;
	
	/**
	Sets the fading effect/control for a particular viewfinder.
	
	@param  aVFFadingEffect
			The desired fading effects associated with the given viewfinder.
	
	@note   Event KUidECamEvent2ViewFinderFadingEffect is used to notify the requesting client about setting of the 
			fading effect.
			
	@internalTechnology
	*/
	virtual void SetViewFinderFadingEffect(const CCameraViewFinder::TViewFinderFadingEffect& aVFFadingEffect)=0;
	
	/**
	Retrieves the handle number which represents the given viewfinder object. 

	@param  aVFHandle
			The viewfinder handle
			
	@leave  May leave with any error code.

	@note   KECamDefaultViewFinderHandle is used to refer to the viewfinder started using CCamera methods. 
	*/
	virtual void GetViewFinderHandleL(TInt& aVFHandle) const=0;
	};

/** 
Mixin class for implementation of CCameraV2DirectViewFinder methods.
*/
class MCameraV2DirectViewFinder
	{

public:
	
	/** 
	Releases the interface. 
	
	@param  aDirectViewFinderHandle
			The pointer to the direct viewfinder class object which would be destroyed by the client.
	*/
	virtual void Release(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle)=0;
	
	/**
	Retrieves the concrete factory handle for the histogram implementation in order to 
	use it specifically for a particular viewfinder.
		   
	@param aImplFactoryPtr
		   The concrete factory handle for the histogram implementation specific to the given view finder.
		   
	@leave  May leave with any error code.
	*/
	virtual void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;

	/**
	Retrieves the concrete factory handle for the image processing implementation in order to 
	use it specifically for a particular viewfinder.
		   
	@param aImplFactoryPtr
		   The concrete factory handle for the image processing implementation specific to the given view finder.
		   
	@leave  May leave with any error code.
	*/
	virtual void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	
	/**
	Provides the interface pointers for the concrete implementation of CCameraDirectSnapshot.
	
	@param aInterface
		   Interface uid for a particular M-class of direct snapshot.
	
	@return Interface pointer. 
	*/
	virtual TAny* GetDirectSnapshotImpl(TUid aInterface) const=0; 
	
	/**
	The observer for the direct view finder is passed to the implementation for passing callbacks on it.
	
	@param  aDirectViewFinderObserver
			The reference to the direct view finder observer.
	*/
	virtual void SetDirectViewFinderObserver(MDirectViewFinderObserver& aDirectViewFinderObserver)=0;	
	
	/**
	The pointer to the client view finder class used to pass back to the client via observer callbacks. Implementation is 
	not	supposed to destroy this object.
	
	@param  aDirectViewFinderHandle
			The pointer to the direct view finder class object.
			
	@see    MDirectViewFinderObserver
	*/
	virtual void SetDirectViewFinderHandle(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle)=0;
	
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
	virtual void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, TRect& aScreenRect)=0;
	
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
	virtual void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice,
											RWindowBase& aWindow, TRect& aScreenRect, TRect& aClipRect)=0;
	
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
	virtual void GetDirectViewFinderPropertiesL(TInt& aScreenNumber, TRect& aScreenRect, TRect& aClipRect) const=0;
	
	/**
	Pauses the direct viewfinder.
	*/
	virtual void PauseViewFinderDirect()=0;
	
	/**
	Resumes the direct viewfinder
	
	@note   MDirectViewFinderObserver::DirectViewFinderFirstFrameDisplayed() will be used to notify clients that the first
			frame has been displayed.
	*/
	virtual void ResumeViewFinderDirect()=0;
	
	/**
	Retrieves the state of the direct viewfinder.
	
	@param  aViewFinderState
			Retrieves the state of the direct viewfinder.
			
	@leave  May leave with any error code.
	*/
	virtual void GetViewFinderStateL(CCamera::CCameraV2DirectViewFinder::TViewFinderState& aViewFinderState) const=0;
	
	/**
	Stops the direct viewfinder.
	*/
	virtual void StopDirectViewFinder()=0;
	};
	
/** 
Mixin class for implementation of camera client viewfinder.
*/	
class MCameraClientViewFinder
	{

public:
	
	/** 
	Releases the interface. 
	
	@param  aClientViewFinderHandle
			The pointer to the client viewfinder class object which would be destroyed by the client.
	*/
	virtual void Release(CCamera::CCameraClientViewFinder* aClientViewFinderHandle)=0;
	
	/**
	Retrieves the concrete factory handle for the histogram implementation in order to 
	use it specifically for a particular viewfinder.
		   
	@param aImplFactoryPtr
		   The concrete factory handle for the histogram implementation specific to the given view finder.
		   
	@leave  May leave with any error code.
	*/
	virtual void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;

	/**
	Retrieves the concrete factory handle for the image processing implementation in order to 
	use it specifically for a particular viewfinder.
		   
	@param aImplFactoryPtr
		   The concrete factory handle for the image processing implementation specific to the given view finder.
		   
	@leave  May leave with any error code.
	*/
	virtual void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	
	/**
	The observer for the client view finder is passed to the implementation for passing callbacks on it.
	
	@param  aClientViewFinderObserver
			The reference to the client view finder observer.
	*/
	virtual void SetClientViewFinderObserver(MClientViewFinderObserver& aClientViewFinderObserver)=0;	
	
	/**
	The pointer to the client view finder class used to pass back to the client via observer callbacks. Implementation is 
	not	supposed to destroy this object.
	
	@param  aClientViewFinderObserver
			The pointer to the client view finder class object.
			
	@see    MClientViewFinderObserver
	*/
	virtual void SetClientViewFinderHandle(CCamera::CCameraClientViewFinder* aClientViewFinderHandle)=0;
	
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
	virtual void StartClientViewFinderL(TInt aScreenNumber, CCamera::TFormat aImageFormat, TSize& aSize)=0;

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
	virtual void StartClientViewFinderL(TInt aScreenNumber, CCamera::TFormat aImageFormat, TSize& aSize, TRect& aClipRect)=0;
	
	/**
	Retrieves the state of the client viewfinder.
	
	@param  aIsActive
			ETrue indicates that client viewfinder is active.
			EFalse indicates that client viewfinder is not active.
	
	@leave  May leave with any error code.
	*/
	virtual void GetClientViewFinderStateL(TBool& aIsActive) const=0;
	
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
	virtual void GetClientViewFinderPropertiesL(TInt& aScreenNumber, CCamera::TFormat& aImageFormat, TSize& aSize, TRect& aClipRect) const=0;
	
	/**
	Stop the client viewfinder.
	*/
	virtual void StopClientViewFinder()=0;
	
	/**
	Retrieves the client viewfinder buffer. Client calls this method after being notified about availability of viewfinder
	data via MClientViewFinderObserver::ViewFinderBufferReady().

	@param  aClientViewFinderBuffer
			Reference to MCameraBuffer2 which retrieves the client viewfinder data.

	@leave  May leave with any error code.
	*/	
	virtual void GetViewFinderBufferL(MCameraBuffer2& aClientViewFinderBuffer) const=0;
	};

/**
This is a mixin class to be implemented by providers of the extension API for camera direct snapshot.

@see CCamera::CCameraDirectSnapshot

@publishedPartner
@prototype
*/
class MCameraDirectSnapshot
	{
	
public:	
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/**
	Sets the version2 direct viewfinder object out of which this direct snapshot has been created. This is called only
	during the creation time of CCameraDirectSnapshot.

	@param	aDirectViewFinder
			A pointer to the camera version2 direct viewfinder object on which the direct snapshot will be displayed.
	*/
	virtual void SetDirectViewFinder(const CCamera::CCameraV2DirectViewFinder* aDirectViewFinder)=0;
	
	/**
	Retrieve the version2 direct viewfinder object out of which this direct snapshot has been created. This information 
	may be required by the client in order to map the direct snapshot object with its parent version2 direct viewfinder
	object or also to retrieve properties of its parent version2 direct viewfinder object.

	@param	aDirectViewFinder
			A pointer to the camera version2 direct viewfinder object on which the direct snapshot will be displayed.

	@leave  May leave with any error code.
	*/
	virtual void GetDirectViewFinderL(CCamera::CCameraV2DirectViewFinder*& aDirectViewFinder) const=0;

	/**
	Enable the direct snapshot. The direct snapshot will be displayed on the parent direct viewfinder screen. The screen 
	size of the snapshot will be the same as that of the parent direct viewfinder screen.

	The direct snapshot can be displayed only for still images.

	@param  aDirectSnapshotParameters
			The desired direct snapshot parameter.
			
	@leave  MAy leave with any error code.
	*/
	virtual void EnableDirectSnapshotL(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters)=0;

	/**
	Disable the direct snapshot. The direct snapshot will not be displayed on the parent direct viewfinder screen.
	*/
	virtual void DisableDirectSnapshot()=0;

	/**
	Retrieves the current state for direct snapshot.

	@param  aDirectSnapshotParameters
			Retrieves the current state for direct snapshot.
			
	@leave  May leave with any error code.
	*/	
	virtual void GetDirectSnapshotStateL(CCamera::CCameraDirectSnapshot::TDirectSnapshotState& aDirectSnapshotState) const=0;

	/**
	Retrieves the direct snapshot parameters. 

	@param  aDirectSnapshotParameters
			Retrieves the direct snapshot parameter of type TDirectSnapshotParameters.
			
	@leave  May leave with any error code.
	*/
	virtual void GetDirectSnapshotParametersL(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters) const=0;

	/**
	Sets the direct snapshot parameters. 

	@param  aDirectSnapshotParameters
			The desired direct snapshot parameter of type TDirectSnapshotParameters.
			
	@leave  May leave with any error code.
	*/	
	virtual void SetDirectSnapshotParametersL(const CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters)=0;
	};
	
#endif // ECAMVIEWFINDERINTF_H
