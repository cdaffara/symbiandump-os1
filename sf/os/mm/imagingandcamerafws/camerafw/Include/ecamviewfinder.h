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
#ifndef  ECAMVIEWFINDER_H
#define  ECAMVIEWFINDER_H

#include <e32base.h>
#include <gdi.h>
#include <ecamadvsettings.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamviewfinderdef.h>
#else
#include <ecamadvsettingsuidsconst.hrh>
#include <ecamconst.h>
#endif

class MCameraViewFinder;
class MCameraV2DirectViewFinder;
class MCameraClientViewFinder;

class MCameraDirectSnapshot;

class MHistogramV2Buffer;

/**
Specific handle used to refer to viewfinder which is started using CCamera methods.
*/
static const TInt KECamDefaultViewFinderHandle = 0;

/** 
The current Version of the TDirectSnapshotParameters class.
*/
static const TUint KECamDirectSnapshotParametersCurrentVersion = 1;

/** 
UID used to identify the CCameraV2DirectViewFinder API.

@see CCamera::CCameraV2DirectViewFinder
*/
static const TUid KECamV2DirectViewFinderUid 	= {KECamV2DirectViewFinderUidValue};

/** 
UID used to identify the CCameraClientViewFinder API.

@see CCamera::CCameraClientViewFinder
*/
static const TUid KECamClientViewFinderUid 	= {KECamClientViewFinderUidValue};

/** 
UID used to identify the CCamera::CCameraDirectSnapshot API.
This is the UID which is used to obtain the CCameraDirectSnapshot interface, 
via a call to CCamera::CustomInterface().

@see KECamDirectSnapshotUidValue
@see CCamera::CCameraDirectSnapshot

@publishedPartner
@prototype
*/
static const TUid KECamDirectSnapshotUid = {KECamDirectSnapshotUidValue};

/**
A mixin class to be implemented by the client in order to use the V2 direct view finder API CCameraV2DirectViewFinder. The
derived class methods are called by the implementation when the direct view finder operations are ready to be notified 
accordingly. Implementation shall keep track of CCameraV2DirectViewFinder object deletion. It has to ensure that a callback 
is not send should the client destroy the particular CCameraV2DirectViewFinder class object when the callback is queued to 
be send across. Ownership of CCameraV2DirectViewFinder is retained by the client.

@see CCamera::CCameraV2DirectViewFinder
*/
class MDirectViewFinderObserver
	{
public:
	/**
	Implementation sends this callback in order to notify the client when the first direct viewfinder frame gets displayed 
	after starting/resuming a particular direct viewfinder.
	
	@param  aDirectViewFinderHandle
			Reference to CCameraV2DirectViewFinder class object which refers a particular direct viewfinder.
						
	@param  aErrorCode
			Appropriate error code.
	*/
	virtual void DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode)=0;
																									  
	/**
	Implementation sends this callback in order to notify the client when the direct histogram data gets displayed on the 
	screen for the first time after starting/resuming a particular direct viewfinder. If a call to CCameraV2Histogram::
	StartHistogram() is made without a previous successful call to CCameraV2Histogram::PrepareDirectHistogramL() then the
	callback returns KErrBadHandle.
	
	@param  aDirectViewFinderHandle
			Reference to CCameraV2DirectViewFinder class object which refers a particular direct viewfinder for which the 
			direct histogram data has been displayed.
	
	@param  aDirectHistogramDisplayed
			Reference to CCameraV2Histogram class object which refers a particular type of histogram being displayed.
	
	@param  aErrorCode
			Appropriate error code. 
	*/
	virtual void DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about availability of the histogram data. If a call 
	to CCameraV2Histogram::StartHistogram() is made without a previous successful call to CCameraV2Histogram::
	PrepareClientHistogramL() then the callback returns KErrBadHandle.
	
	@param  aDirectViewFinderHandle
			Reference to CCameraV2DirectViewFinder class object which refers a particular direct viewfinder for which the 
			client histogram data have been generated.
	
	@param  aClientHistogramBuffer
			Pointer to MHistogramV2Buffer which retrieves a single histogram alongwith relevant information about it. The 
			ownership will be retained by the implementation. Client needs to call Release in order to indicate the 
			implementation that the buffer can be re-used. Client shall never try to delete the pointer. NULL, if error.
				
	@param  aErrorCode
			Appropriate error code. 
	
	@note   Client shall use the currently available histogram for the direct viewfinder frames until a new histogram is 
			available.
	*/
	virtual void ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode)=0;
	
	/** 
	Implementation sends this callback as a notification of the display of direct snapshot for still images on the display
	screen.
	
	@param  aDirectViewFinderHandle
			Reference to CCameraV2DirectViewFinder class object which refers a particular direct viewfinder on which the 
			direct snapshot data for still images have been displayed.

	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
	
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given 
			CCameraImageCapture class object. This is needed to identify the image which is represented by this snapshot.
	
	@param  aErrorCode
			Appropriate error code.
	*/
	virtual void DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about failure with the direct viewfinder.
	
	@param  aDirectViewFinderHandle
			Reference to CCameraV2DirectViewFinder class object which refers a particular direct viewfinder.
	
	@param  aErrorCode
			The reason of failure of the direct viewfinder. 
	*/
	virtual void DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode)=0;
	
	/**
	Gets a custom interface for future callbacks. This method will be called by the implementation to get a new interface
	which would support future callbacks.

	@param  aInterface
		    The Uid of the particular interface function required for callbacks.
		   
	@param  aPtrInterface
		    The implementation has to type-cast the retrieved custom interface pointer to the appropriate type.	

	@return The error code.
	*/
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface)=0;
	};

/**
A mixin class to be implemented by the client in order to use the client view finder API CCameraClientViewFinder. The 
derived class methods are called by the implementation when the client view finder operations are ready to be notified 
accordingly. Implementation shall keep track of CCameraClientViewFinder object deletion. It has to ensure that a callback
is not send should the client destroy the particular CCameraClientViewFinder class object when the callback is queued to
be send across. Ownership of CCameraClientViewFinder is retained by the client.

@see CCamera::CCameraClientViewFinder
*/
class MClientViewFinderObserver
	{
public:
	/**
	Implementation sends this callback in order to notify the client about the availability of viewfinder frames.
	Viewfinder frames can be retrieved by the client using the CCameraClientViewFinder::GetViewFinderBufferL
	(MCameraBuffer2& aClientViewFinderBuffer).
	
	@param  CCameraClientViewFinder
			Reference to CCameraClientViewFinder class object which refers a particular client viewfinder.
						
	@param  aErrorCode
			Appropriate error code.
	*/
	virtual void ViewFinderBufferReady(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, TInt aErrorCode)=0;
																									  
	/**
	Implementation sends this callback in order to notify the client when the direct histogram data gets displayed on the 
	screen for the first time after starting a particular client viewfinder. If a call to CCameraV2Histogram::
	StartHistogram() is made without a previous successful call to CCameraV2Histogram::PrepareDirectHistogramL() then the
	callback returns KErrBadHandle.
	
	@param  aClientViewFinderHandle
			Reference to CCameraClientViewFinder class object which refers a particular client viewfinder.
	
	@param  aDirectHistogramDisplayed
			Reference to CCameraV2Histogram class object which refers a particular type of histogram being displayed.
	
	@param  aErrorCode
			Appropriate error code. 
	*/
	virtual void DirectHistogramDisplayed(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about availability of the histogram data. If a call 
	to CCameraV2Histogram::StartHistogram() is made without a previous successful call to CCameraV2Histogram::
	PrepareClientHistogramL() then the callback returns KErrBadHandle.
	
	@param  aClientViewFinderHandle
			Reference to CCameraClientViewFinder class object which refers a particular client viewfinder for which the 
			client histogram data have been generated.
	
	@param  aClientHistogramBuffer
			Pointer to MHistogramV2Buffer which retrieves a single histogram alongwith relevant information about it. The 
			ownership will be retained by the implementation. Client needs to call Release in order to indicate the 
			implementation that the buffer can be re-used. Client shall never try to delete the pointer. NULL, if error.
			
	@param  aErrorCode
			Appropriate error code. 
	
	@note   Client shall use the currently available histogram for the viewfinder frames until a new histogram is 
			available.
	*/
	virtual void ClientHistogramReady(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about failure of image processing for the client 
	viewfinder frames.
	
	@param  aClientViewFinderHandle
			Reference to CCameraClientViewFinder class object which refers a particular client viewfinder.
	
	@param  aErrorCode
			The reason of failure of the image processing. 
	*/
	virtual void ImageProcessingFailed(CCamera::CCameraClientViewFinder& aClientViewFinderHandle, TInt aErrorCode)=0;
	
	/**
	Gets a custom interface for future callbacks. This method will be called by the implementation to get a new interface
	which would support future callbacks.

	@param  aInterface
		    The Uid of the particular interface function required for callbacks.
		   
	@param  aPtrInterface
		    The implementation has to type-cast the retrieved custom interface pointer to the appropriate type.	

	@return The error code.
	*/
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface)=0;
	};

/** 
CCamera view finder base class is used to provide features common to both client view finders and direct view finders.

@note This class is not intended for stand alone use. One of the derived class shall be used instead.
Features of this class will automatically be used when either direct view finder or client based view finder are 
created by clients.

@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedPartner
@prototype
*/
class CCameraViewFinder : public CBase
	{
	/* so can use internal factory functions etc. */
	friend class CCamera;

public:
	/** 
	Different types of available viewfinder fading.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TViewFinderFadingType
		{
		/** Fading not supported. */
		EVFFadingTypeNone	 		 		=0x00,
		/** Fading from a single frame of color given by the client. */
		EVFFadingTypeStaticColorFading      =0x01,
		/** Fading from a single frame given by the client. */
		EVFFadingTypeStaticFrameFading      =0x02,
		/** Fading from a number of previous VF frames. */
		EVFFadingTypeCross     	 			=0x04
		};

	/** 
	Different ways to control the viewfinder fading.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TViewFinderFadingControl
		{
		/** Fading not supported. */
		EVFFadingControlNone			= 0x00,
		/** Switch on (enable) the fading. */
		EVFFadingControlSwitchedOn		= 0x01,
		/** Switch off (disable) the fading. */
		EVFFadingControlSwitchedOff		= 0x02,
		/** Automatic fading. */
		EVFFadingControlSwitchAuto		= 0x04
		};
	
	/**
	Different directions in which the fading may appear.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/		
	enum TViewFinderFadingDirection
		{
		/** Fading not supported. */
		EVFFadingDirectionNone			= 0x00,
		/** Automatic direction or may be even custom direction decided by implementation. */
		EVFFadingDirectionAuto			= 0x01,
		/** Uniform fading. */ 
		EVFFadingDirectionUniform		= 0x02,
		/** Start to fade from up to down. */
		EVFFadingDirectionUpDown		= 0x04,
		/** Start to fade from down to up. */
		EVFFadingDirectionDownUp		= 0x08,
		/** Start to fade from left to right. */
		EVFFadingDirectionLeftRight		= 0x10,
		/** Start to fade from right to left. */
		EVFFadingDirectionRightLeft		= 0x20
		};
		
	/**
	Image enhancement options to be applied for a particular display.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/	
	enum TImageEnhancement
		{
		/** Not Supported. */
		EImageEnhancementNone 			= 0x00,
		/** Image Enhancement switches off. */
		EImageEnhancementSwitchOFF 		= 0x01,
		/** Image Enhancement switches to auto mode. */
		EImageEnhancementAuto 			= 0x02,
		/** Image Enhancement switches on. */
		EImageEnhancementSwitchOn 		= 0x04
		};
	
	/**
	Retrieves the capabilites of viewfinder fading.
	
	@internalTechnology
	*/
	class TViewFinderFadingCapabilities
		{
	public:
		IMPORT_C TViewFinderFadingCapabilities();
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;
		
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
		
		// reserved for future expansion
		TInt iReserved1;
		TInt iReserved2;
		
	public:
		/** Bit-field retrieving the supported fading type TViewFinderFadingType. */
		TUint iSupportedFadingType;
		/** Bit-field retrieving the supported fading control TViewFinderFadingControl. */
		TUint iSupportedFadingControl;
		/** Bit-field retrieving the supported fading direction TViewFinderFadingDirection. */
		TUint iSupportedFadingDirection;
		};
		
	/**
	Viewfinder fading effects characterizing a particular viewfinder fading.
	
	@internalTechnology
	*/
	class TViewFinderFadingEffect
		{
	public:
		IMPORT_C TViewFinderFadingEffect();
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;
	
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8; 
		
		// reserved for future expansion
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3[6];
		
	public:
		/** The type of viewfinder fading to be used. */
		TViewFinderFadingType 		iViewFinderFadingType;
		/** The control for the viewfinder fading, that is, on/off/auto. */
		TViewFinderFadingControl 	iViewFinderFadingControl;
		/** The direction of viewfinder fading. */
		TViewFinderFadingDirection 	iViewFinderFadingDirection;
		/** The color used to fade from/to in case of static color fading. This is ignored if iViewFinderFadingType is 
		not EVFFadingTypeStaticColorFading. */
		TRgb  						iStaticFadingColor;
		/** The handle of CFbsBitmap used to obtain the frame used to fade from/to in case of static frame fading. 
		This is KECamStaticFrameFadingTypeNotUsed if iViewFinderFadingType is not EVFFadingTypeStaticFrameFading.
		This is KECamSpecialStaticFrameFormatHandle if CFBsBitmap handle is not to be used. */
		TInt						iStaticFadingFrameBitmapHandle;
		/** The number of previous frames to be saved for cross fading. 
		This is KECamCrossFadingFramesNotUsed if iViewFinderFadingType is not EVFFadingTypeCross. */
		TUint						iCrossFadingFrames;
		/** The fading time. This may be used both for auto and manual fading as per implementation. */
		TTimeIntervalMicroSeconds32 iFadingTime;
		};

public:
	
	IMPORT_C void GetViewFinderFadingCapabilitiesL(TViewFinderFadingCapabilities& aVFFadingCapabilities) const;
	IMPORT_C void GetViewFinderFadingEffectL(TViewFinderFadingEffect& aCurrentVFFadingEffect) const;
	IMPORT_C void SetViewFinderFadingEffectL(const TViewFinderFadingEffect& aVFFadingEffect);
	
	IMPORT_C void GetViewFinderHandleL(TInt& aVFHandle) const;
	
	~CCameraViewFinder();
	
protected:
	CCameraViewFinder(CCamera& aOwner);
	
protected: 
	void GetSpotsPositionL(TUint aSpotCombination, RArray<TRect>& aSpotsPosition) const;
	
	void GetSupportedImageEnhancementL(TUint& aSupportedImageEnhancement) const;
	void GetImageEnhancementL(TImageEnhancement& aImageEnhancement) const;
	void SetImageEnhancementL(TImageEnhancement aImageEnhancement);
	
	void GetSupportedVFMagnificationL(RArray<TUint>& aSupportedVFMagnification) const;
	void GetViewfinderMagnificationL(TUint& aVFMagnification) const; 
	void SetViewfinderMagnificationL(TUint aVFMagnification);
		   	
protected:	
	CCamera&        	iOwner; 
	MCameraViewFinder* 	iImplBase;  // not owned
    };	

/** 
CCamera direct view finder v2 class is used to provide support for multiple Direct View Finder.
A new instance of this class will be created for every new direct viewfinder.

@note This class is not intended for sub-classing and used to standardise existing varieties of implementations.

@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@note  Clients must implement an MDirectViewFinderObserver in order to use this CCameraV2DirectViewFinder API.

@publishedPartner
@prototype
*/
class CCamera::CCameraV2DirectViewFinder : public CCameraViewFinder
	{
	/* so can use internal factory functions etc. */
	friend class CCamera;
	friend class CCamera::CCameraDirectSnapshot;

public:
   
	/** Specifies direct viewfinder state. */ 
	enum TViewFinderState
		{
		/** View Finder is activated */  
		EViewFinderActive, 	 
		/** View Finder has been paused */
		EViewFinderPause,	 
		/** View Finder has been stopped or hasn't yet started. Default state. */
		EViewFinderInActive
		};
	
public:

	IMPORT_C static CCameraV2DirectViewFinder* NewL(CCamera& aOwner, MDirectViewFinderObserver& aDirectViewFinderObserver);
	
	IMPORT_C CCamera::CCameraV2Histogram* CreateHistogramHandleL() const;
	IMPORT_C CCamera::CCameraImageProcessing* GetTransformationHandleL() const;
	
	IMPORT_C CCamera::CCameraDirectSnapshot* GetDirectSnapshotHandleL() const; 
	
	IMPORT_C void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, TRect& aScreenRect);
										
	IMPORT_C void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, 
																			TRect& aScreenRect, TRect& aClipRect);
	
	IMPORT_C void GetDirectViewFinderPropertiesL(TInt& aScreenNumber, TRect& aScreenRect, TRect& aClipRect) const;
    
    IMPORT_C void PauseViewFinderDirect();
	IMPORT_C void ResumeViewFinderDirect();
	IMPORT_C void GetViewFinderStateL(TViewFinderState& aViewFinderState) const;
	
	IMPORT_C void StopDirectViewFinder();
	
	IMPORT_C ~CCameraV2DirectViewFinder();
	
private:
	CCameraV2DirectViewFinder(CCamera& aOwner);
	void ConstructL(MDirectViewFinderObserver& aDirectViewFinderObserver);
	
	void SetImplHandle();
			
	void SetImplBaseHandle();
	
	MCameraV2DirectViewFinder* Impl();
	
private:	
	MCameraV2DirectViewFinder* 	iImpl;  // not owned
    };	

/** 
CCamera client view finder class is used to provide support for multiple client View Finder.
A new instance of this class will be created for every new client viewfinder.

@note  This class is not intended for sub-classing and used to standardise existing
       varieties of implementations.
       
@note  If the class methods leave, the output type parameter value is not guaranteed to be valid.

@note  Clients must implement an MClientViewFinderObserver in order to use this CCameraClientViewFinder API. 
	   MCameraObserver2::ViewFinderReady will not be used with this class.

@publishedPartner
@prototype
*/
class CCamera::CCameraClientViewFinder : public CCameraViewFinder
	{
	/* so can use internal factory functions etc. */
	friend class CCamera;

public:

	IMPORT_C static CCameraClientViewFinder* NewL(CCamera& aOwner, MClientViewFinderObserver& aClientViewFinderObserver);
	
	IMPORT_C CCamera::CCameraV2Histogram* CreateHistogramHandleL() const;
	IMPORT_C CCamera::CCameraImageProcessing* GetTransformationHandleL() const;
	
	IMPORT_C void StartClientViewFinderL(TInt aScreenNumber, CCamera::TFormat aImageFormat, TSize& aSize);
	IMPORT_C void StartClientViewFinderL(TInt aScreenNumber, CCamera::TFormat aImageFormat, TSize& aSize, TRect& aClipRect);
	
	IMPORT_C void GetClientViewFinderStateL(TBool& aIsActive) const;
	IMPORT_C void GetClientViewFinderPropertiesL(TInt& aScreenNumber, CCamera::TFormat& aImageFormat, TSize& aSize, TRect& aClipRect) const;

	IMPORT_C void StopClientViewFinder();
	
	IMPORT_C void GetViewFinderBufferL(MCameraBuffer2& aClientViewFinderBuffer) const;
	
	IMPORT_C ~CCameraClientViewFinder();
	   	
private:
	CCameraClientViewFinder(CCamera& aOwner);
	void ConstructL(MClientViewFinderObserver& aClientViewFinderObserver);
	
	void SetImplHandle();
			
	void SetImplBaseHandle();
	
private:	
	MCameraClientViewFinder* 	iImpl;  // not owned
    };
    
/**
This class allows the client to enable direct snapshot feature for still images only. The direct snapshot API can not be 
used for video.

The direct snapshot can be created out of CCameraV2DirectViewFinder object only. Its lifetime is dependent on the parent 
direct viewfinder object. So, the client is supposed to destroy the direct snapshot object before deleting the parent 
direct viewfinder object. The MDirectViewFinderObserver observer class provides callback to notify the client whenever
direct snapshot data gets displayed on the given direct viewfinder screen. The callback also provides information regarding
the CCameraImageCapture object used to issue the image capture calls. It also provides the Id for the individual images 
which get represented by the snapshot displayed.

@note This class provides a standardised client interface for the direct snapshot. Classes cannot be derived from it.

@note If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedPartner
@prototype
*/ 
class CCamera::CCameraDirectSnapshot : public CBase
	{
	friend class CCamera::CCameraV2DirectViewFinder;

public:
	/**
	Describes the state of direct snapshot. 
	The enumeration list may be extended in future.
	*/
	enum TDirectSnapshotState 
		{
		/** The direct snapshot has been enabled. */
		EDirectSnapshotEnabled, 
		/** The direct snapshot has been disabled. */
		EDirectSnapshotDisabled
		};
		
	/**
	Specifes the parameters necessary for direct snapshots. The screen co-ordinates used for direct snapshot will be the 
	same as used for the parent direct viewfinder screen co-ordinates. This screen co-ordinate for direct snapshot may be 
	changed by the implementation in certain cases for alignment.
	*/
	class TDirectSnapshotParameters
		{
	public:
		IMPORT_C TDirectSnapshotParameters();
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;
		
		IMPORT_C TBool IsAspectRatioMaintained() const;
		IMPORT_C void SetAspectRatioState(TBool aIsAspectRatioMaintained);
				
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
		
		// reserved for future expansion
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;
		
		TUint iReserved4:31;
		
		/** Set to ETrue if the aspect ratio of the direct snapshot image must be maintained when scaling down */
		TUint iIsAspectRatioMaintained:1;
		
	public:
	    /** The bit field representing the drive modes for which the direct snapshot will be displayed if enabled. 
	    Refer CCamera::CCameraAdvancedSettings::TDriveMode. */
		TUint iDriveModes; 
		
		/** The background colour to be used if the snapshot has been scaled (maintaining its aspect ratio)
  		and does not fully fill the dimension as per the direct view finder screen size. The TRect supposed to be used 
  		for direct snapshot may be modified by the implementation in certain cases for alignment. */
		TRgb iBgColor; 
		
		/** The time duration during which the snapshot data should appear on the screen. The implementation may 
		internally change this display time if the client sets iDisplayTime to TTimeIntervalMicroSeconds32 
		(KECamSnapshotDefaultDisplayTime). This may happen when the client doesn't want to specify a specific duration 
		and would rather let the implementation decide. */
		TTimeIntervalMicroSeconds32 iDisplayTime;
		};
		
public:
	
	IMPORT_C void GetDirectViewFinderL(CCamera::CCameraV2DirectViewFinder*& aDirectViewFinder) const;
	
	IMPORT_C void EnableDirectSnapshotL(TDirectSnapshotParameters& aDirectSnapshotParameters);
	
	IMPORT_C void DisableDirectSnapshot();
	
	IMPORT_C void GetDirectSnapshotStateL(TDirectSnapshotState& aDirectSnapshotParameters) const;
	
	IMPORT_C void GetDirectSnapshotParametersL(TDirectSnapshotParameters& aDirectSnapshotParameters) const;
	
	IMPORT_C void SetDirectSnapshotParametersL(const TDirectSnapshotParameters& aDirectSnapshotParameters);
	
	IMPORT_C ~CCameraDirectSnapshot();
	
private:
	IMPORT_C static CCameraDirectSnapshot* CreateL(CCamera::CCameraV2DirectViewFinder& aDirectViewFinder);
	
	CCameraDirectSnapshot();
	void ConstructL(CCamera::CCameraV2DirectViewFinder& aDirectViewFinder);
	
	MCameraV2DirectViewFinder* DirectViewFinderImpl(CCamera::CCameraV2DirectViewFinder& aDirectViewFinder);
	
private:
	MCameraDirectSnapshot*  iImpl;   // not owned
	}; 
	 
#endif // ECAMVIEWFINDER_H
