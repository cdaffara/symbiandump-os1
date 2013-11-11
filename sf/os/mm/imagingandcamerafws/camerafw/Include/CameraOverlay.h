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

#ifndef CAMERAOVERLAY_H
#define CAMERAOVERLAY_H

#include <ecam.h>
#include <ecam/ecamcommonuids.hrh>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecam/cameraoverlayconst.h>
#endif

class MCameraOverlay;
class MCameraOverlay2;


/** 
This is the UID which is used to obtain the CCameraOverlay interface, 
via a call to CCamera::CustomInterface().

@see KECamOverlayUidValue
@see CCamera::CCameraOverlay
*/
static const TUid  KECamOverlayUid = {KECamOverlayUidValue};

	
/**
This class provides support for image overlays. 
The client can use it to overlay an image onto the viewfinder, snapshots, still images and video. 
A client can create multiple overlays, where each overlay can be in a different format. 
The properties of an overlay can be changed after it has been created.

@note This class provides a standardised client interface for the camera overlay. Classes cannot be derived from it.

@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedAll
@released
*/
class CCamera::CCameraOverlay: public CBase
	{
	
	friend class CCamera;
	
public:	
	/** 
	Overlay camera mode types 
	 
	Represents the possible camera modes in which the overlay could be used.
	Several types can be combined when returning ECam implementation support for various modes. 
	*/
	enum TOverlayCameraMode
		{
		/**	Overlays are not supported for any camera mode. */
		EModeNone 		=  0x00,
		/**	The image can be overlaid on captured images.
		The camera is in still image mode. This effectively means all the still image drive modes and
		its interpretation may be implementation-specific. 
		Explicit definition of drive modes is recommended instead.
		*/
		EModeStillImage	=  0x01,
		/**	The image can be overlaid on a snapshot.
		The camera has snapshot functionality set on. 
		*/
		EModeSnapshot	=  0x02,		
		/**	
		The image can be overlaid on a viewfinder. 
		The camera is displaying directly to viewfinder. 
		This mode shall not be used if any of the viewfinder submodes is specified.
		    @note Overlay visibility for different viewfinder modes (direct/client-based) is 
		    implementation-specific; viewfinder modes should be explicitly specified by clients instead.
		*/
		EModeViewfinder	=  0x04,
		/**	The image can be overlaid on a video frame. 
		The camera is in video mode. */
		EModeVideo		=  0x08,
		/**
		    The image is to be overlaid on direct viewfinder
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/
		EModeDirectViewfinder       = 0x00010,
		/**
		    The image is to be overlaid on client-based viewfinder
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/
		EModeClientViewfinder       = 0x00020,
		/**
		    The image is to be overlaid when Continuous Still Image driving mode is active.
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/		
		EModeStillImageContinuous   = 0x00080,
		/**
		    The image is to be overlaid when Still Image Bracketing driving mode is active.
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/		
        EModeStillImageBracket		= 0x00100,
		/**
		    The image is to be overlaid when Still Image Bracketing with Merge option driving mode is active.
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/        
        EModeStillImageBracketMerge	= 0x00200,
		/**
		    The image is to be overlaid when Timed Still Image driving mode is active.
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/         
        EModeStillImageTimed		= 0x00400,
		/**
		    The image is to be overlaid when Timed Still Image with Lapse option driving mode is active.
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/         
        EModeStillImageTimeLapse	= 0x00800,
		/**
		    The image is to be overlaid when Still Image Burst driving mode is active.
		    @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/         
        EModeStillImageBurst		= 0x01000
		};
	
	/** 
	Overlay types 
	 
	Type in which alpha value will be provided.
	*/
	enum TOverlayType
		{
		/** Does not support overlays. */
		EOverlayNone  	=  0x00,
		/** Blending is on a per pixel basis, where the alpha value information is provided in the overlay bitmap itself. 
		The alpha value is specified in each pixel of the bitmap (it is the alpha componenet of the TRgb object). The 
		display mode of the bitmap should be such that alpha value is supported. TOverlayParameters::iAlphaValue is 
		neglected in this case. */
		EPerPixel		=  0x01,
		/** Blending is on a per plane basis, where all pixels are affected by the same alpha value. The alpha value is
		provided through TOverlayParameters::iAlphaValue. */
		EPerPlane		=  0x02	
		};
	
	/** 
	Blending types 
	 
	Type of supported blending.
	*/
	enum TBlendingType
		{
		/** Does not support blending. */
		EBlendNone,
		/** Supports only binary blending. If alpha value is 0xFF, it is opaque, otherwise transparent. */
		EBinary,
		/** Full support for blending - all values in the range [0:255]. */
		EFullRange,
		/** Supports only dynamic binary blending - 
		allows values to be changed when overlay is being displayed. If alpha value is 0xFF, it is opaque, 
		otherwise transparent. Since the blending is dynamic, SetModifiableOverlayBitmapL shall be used such that the
		bitmap could be changed by the implementation. */
		EBinaryDynamic,
		/** Full support for dynamic blending - all values in the range [0:255] 
		- allows values to be changed when overlay is being displayed. Since the blending is dynamic, 
		SetModifiableOverlayBitmapL shall be used such that the bitmap could be changed by the implementation. */
		EFullRangeDynamic
		};
		
	/**
 	Overlay support information characterizing the overlay functionality as a whole.
 	@publishedPartner
	@prototype
 	*/
	class TOverlaySupportInfo
	{
	public:
		IMPORT_C explicit TOverlaySupportInfo();
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;
		
	public:
		/** The camera modes that the ECam implementation supports when applying overlays.
		The modes are held as a bitwise logical OR of the relevant individual modes
		defined in CCamera::CCameraOverlay::TOverlayCameraMode. */
		TUint 		  iSupportedModes; 
		/** The camera types that the ECam implementation supports when applying overlays.
		The types are held as a bitwise logical OR of the relevant individual types
		defined in CCamera::CCameraOverlay::TOverlayType. */
		TUint 		  iSupportedTypes; 
		/** Represents blending type for EPerPlane overlay Type.*/
		TBlendingType iPerPlane;
		/** Represents blending type for EPerPixel overlay Type.*/
		TBlendingType iPerPixel;
		/** Whether overlapping overlays are supported. */
		TBool         iCanOverlap;  
		
		/** This is an input parameter which the client needs to provide. It represents the specific camera mode for which
		the overlay support information is required.
		Default values for iDesiredCameraMode (that is, CCamera::CCameraOverlay::EModeNone) and iViewFinderHandle (that is, 
		KECamOverlayInvalidViewFinderHandle) implies that the client is using the TOverlaySupportInfo as before and 
		iSupportedModes will not be neglected. Refer to TOverlaySupportInfo().
		*/
		TOverlayCameraMode iDesiredCameraMode;
		/** This is also another input parameter which the client needs to provide. It represents the specific viewfinder 
		handle for which the overlay support information is required. 
		If iViewFinderHandle is KECamOverlayNoSpecificViewFinderHandle, then generic overlay support is required which will 
		be valid for every viewfinder handle of type iDesiredCameraMode.
		Default values for iDesiredCameraMode (that is, CCamera::CCameraOverlay::EModeNone) and iViewFinderHandle (that is, 
		KECamOverlayInvalidViewFinderHandle) implies that the client is using the TOverlaySupportInfo as before and 
		iSupportedModes will not be neglected. Refer to TOverlaySupportInfo().
		*/
		TInt iViewFinderHandle; 
		
		private:
		// reserved for future expansion
		TInt iReserved3;	 
	};
		
	/**
 	Overlay parameters characterizing a particular overlay.
 	@publishedPartner
	@prototype
 	*/
	class TOverlayParameters
	{
	public:
		IMPORT_C explicit TOverlayParameters();
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;		
		
	public:
		/** The camera modes in which this image overlay can be used.
		The modes are held as a bitwise logical OR of the relevant individual modes
		defined in CCamera::CCameraOverlay::TOverlayCameraMode. */
		TUint 	iCurrentModes; 	
		/** The camera types in which this image overlay can be used.
		The types are held as a bitwise logical OR of the relevant individual types
		defined in CCamera::CCameraOverlay::TOverlayType. */
		TUint 	iCurrentTypes; 	
		/** This is the alpha value to be applied when iCurrentTypes contains type 
		CCamera::CCameraOverlay::TOverlayType::EPerPlane. The alpha value for red, green and blue is packed into this
		TInt. The layout of this TInt is such that the most significant byte (from left side) is not used at all and the
		remaining three bytes (after the most significant byte) contains the alpha value for red, green and blue.
		Otherwise, if iCurrentTypes only contains type CCamera::CCameraOverlay::TOverlayType::EPerPixel, 
		then iAlphaValue will not be used. */
		TInt 	iAlphaValue; 	
		/** Top left corner within the original image, where the overlay image is to be blended. */
		TPoint 	iPosition; 		
		/** The z-order of the overlay to indicate relative depth when several overlays are applied.
		Values are in the range 0 to 100. The overlay with iZOrder of 0 is the deepest.*/
		TUint   iZOrder;       
		/** The handle for the viewfinder on which the overlay is supposed to be applied. Only one viewfinder handle can be passed.
		If KECamOverlayNoSpecificViewFinderHandle is provided by the client, then the overlay is supposed to be applied for every
		viewfinder handle of type as given by iCurrentModes.
		If KECamOverlayInvalidViewFinderHandle, then the default implementation is being used where 
		TOverlayParameters::iCurrentModes will not be neglected and overlay will be applied for every viewfinder alongwith
		other camera modes.
		*/
		TInt iViewFinderHandle;
	private:
		// reserved for future expansion.
		TInt iReserved2;
		TInt iReserved3;
		// Reserved members which came into existence as a result of removing Tsize public member variable.
		TInt iReserved4;
		TInt iReserved5;	 
	};
		
public:
	
	IMPORT_C static CCameraOverlay* NewL(CCamera& aCamera);
	IMPORT_C ~CCameraOverlay();
	IMPORT_C void GetOverlaySupport(TOverlaySupportInfo& aInfo);   

	IMPORT_C TUint CreateOverlayL(const TOverlayParameters& aParameters, CFbsBitmap* aBitmap);
	IMPORT_C void ReleaseOverlay(TUint aOverlayHandle);

	IMPORT_C void SetOverlayBitmapL(TUint aOverlayHandle, const CFbsBitmap* aBitmap);
	IMPORT_C void SetModifiableOverlayBitmapL(TUint aOverlayHandle, CFbsBitmap* aBitmap);
		
	IMPORT_C void GetOverlayBitmapL(TUint aOverlayHandle, CFbsBitmap* aBitmap);
	IMPORT_C void GetOverlayParametersL(TUint aOverlayHandle, TOverlayParameters& aInfo);
	IMPORT_C void SetOverlayParametersL(TUint aOverlayHandle, const TOverlayParameters& aParameters);

	IMPORT_C void GetAllOverlaysInZOrderL(RArray<TUint>& aOverlayHandles);
	IMPORT_C void SetAllOverlaysInZOrderL(const RArray<TUint>& aOverlayHandles);
	
	IMPORT_C void GetAllOverlaysInZOrderL(TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, RArray<TUint>& aOverlayHandles) const;
	IMPORT_C void SetAllOverlaysInZOrderL(TOverlayCameraMode aOverlayCameraMode, TInt aViewFinderHandle, const RArray<TUint>& aOverlayHandles);

private:
	CCameraOverlay(CCamera& aOwner);
	void ConstructL();

private:
	CCamera&        iOwner; 
	MCameraOverlay* iImpl;    // not owned
	MCameraOverlay2* iImpl2;  // not owned
	};	

#endif // CAMERAOVERLAY_H
