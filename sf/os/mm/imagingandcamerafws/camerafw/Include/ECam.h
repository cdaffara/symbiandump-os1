// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef ECAM_H
#define ECAM_H

#include <e32base.h>
#include <ecamuids.hrh>
#include <e32property.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamdef.h>
#endif

class MFrameBuffer;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;
class CFbsBitmap;

typedef TInt TPostCaptureControlId;

/** Specifies camera device information.

If either zoom or digital zoom are available then the appropriate entries
in this class will be set to indicate the range of values that they may take.
This should be implemented such that a setting of zero corresponds to no zoom,
and a step of one corresponds to the smallest increment available, in a linear
fashion.

There are also zoom factors that correspond to the actual zoom factor when
at minimum (non-digital only) and maximum zoom. Negative zoom values represent
macro functionality.

Image capture, if supported, is simply a case of transferring the current
image from the camera to the client via MCameraObserver::ImageReady(). The
camera class must set the iImageFormatsSupported bitfield to indicate the
formats available.

@publishedAll
@released
*/
class TCameraInfo
	{
public:
	/** Possible directions in which the camera may point.
	*/
	enum TCameraOrientation
		{
		/** Outward pointing camera for taking pictures.
		Camera is directed away from the user. */
		EOrientationOutwards,
		/** Inward pointing camera for conferencing.
		Camera is directed towards the user. */
		EOrientationInwards,
		/** Mobile camera capable of multiple orientations.
		Camera orientation may be changed by the user. */
		EOrientationMobile,
		/** Camera orientation is not known. */
		EOrientationUnknown
		};

	/** Various flags describing the features available for a particular implementation
	*/
	enum TOptions
		{
		/** View finder display direct-to-screen flag */
		EViewFinderDirectSupported		= 0x0001,
		/** View finder bitmap generation flag */
		EViewFinderBitmapsSupported		= 0x0002,
		/** Still image capture flag */
		EImageCaptureSupported			= 0x0004,
		/** Video capture flag */
		EVideoCaptureSupported			= 0x0008,
		/** View finder display mirroring flag */
		EViewFinderMirrorSupported		= 0x0010,
		/** Contrast setting flag */
		EContrastSupported				= 0x0020,
		/** Brightness setting flag */
		EBrightnessSupported			= 0x0040,
		/** View finder clipping flag */
		EViewFinderClippingSupported	= 0x0080,
		/** Still image capture clipping flag */
		EImageClippingSupported			= 0x0100,
		/** Video capture clipping flag */
		EVideoClippingSupported			= 0x0200
		};
public:
	/** Version number and name of camera hardware. */
	TVersion iHardwareVersion;
	/** Version number and name of camera software (device driver). */
	TVersion iSoftwareVersion;
	/** Orientation of this particular camera device. */
	TCameraOrientation iOrientation;

	/** Bitfield of TOptions available */
	TUint32 iOptionsSupported;
	/** The supported flash modes.
	
	This is a bitfield of CCamera::TFlash values. 
	
	If methods CCamera::New2L() or CCamera::NewDuplicate2L() are not used to create the CCamera object, it is assumed that the 
	application may not be able to cope with any future additions to the enum values. So, any unrecognised enum value passed 
    from the implementation should be filtered by ECAM Implementation.
    To receive unrecognised/extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. This is an indication to ECAM implementation. In this case, application is assumed 
    to be prepared to receive unrecognised enum values.
    @see CCamera::CCameraAdvancedSettings::SupportedFlashModes()
	
	*/
	TUint32 iFlashModesSupported;
	/** The supported exposure modes.
	
	This is a bitfield of CCamera::TExposure values. */
	TUint32 iExposureModesSupported;
	
	/** The supported white balance settings.
	
	This is a bitfield of of CCamera::TWhiteBalance values. 
	
	If methods CCamera::New2L() or CCamera::NewDuplicate2L() are not used to create the CCamera object, it is assumed that the 
	application may not be able to cope with any future additions to the enum values. So, any unrecognised enum value passed 
    from the implementation should be filtered by ECAM Implementation.
    To receive unrecognised/extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. This is an indication to ECAM implementation. In this case, application is assumed 
    to be prepared to receive unrecognised enum values.
    Refer to CCamera::CCameraAdvancedSettings::SupportedWhiteBalanceModes() implementation
    
    @see CCamera::CCameraAdvancedSettings::SupportedWhiteBalanceModes()
	*/
	TUint32 iWhiteBalanceModesSupported;

	/** Minimum zoom value allowed. 
	
	Must be negative, or zero if not supported.
	
	This is the minimum value that may be passed to CCamera::SetZoomFactorL(). */
	TInt iMinZoom;
	/** Maximum zoom value allowed. 
	
	Must be positive, or zero if not supported.
	
	This is the maximum value that may be passed to CCamera::SetZoomFactorL(). */
	TInt iMaxZoom;
	/** Maximum digital zoom value allowed. 
	
	Must be positive, or zero if not supported.

	This is the maximum value that may be passed to CCamera::SetDigitalZoomFactorL(). 
	Digital zoom factor is assumed to be a linear scale from 0 to iMaxDigitalZoom. */
	TInt iMaxDigitalZoom;

	/** Image size multiplier corresponding to minimum zoom value. 
	
	Must be between 0 and 1 inclusive. Both 0 and 1 indicate that macro functionality 
	is not supported. */
	TReal32 iMinZoomFactor;
	/** Image size multiplier corresponding to maximum zoom value. 
	
	May take the value 0, or values greater than or equal to 1. Both 0 and 1 indicate 
	that zoom functionality is not supported. */
	TReal32 iMaxZoomFactor;
	/** Image size multiplier corresponding to maximum digital zoom value. 
		
	Implementation recommendation is to use 'appropriate value' for maximum digital zoom which could cover only values 
	given by new digital zoom methods based on image format and capture mode.
	
	Must be greater than or equal to 1. */
	TReal32 iMaxDigitalZoomFactor;

	/** Count of still image capture sizes allowed.
	
	Number of different image sizes that CCamera::EnumerateCaptureSizes() will 
	support, based on the index passed in. Index must be between 0 and iNumImageSizesSupported-1. */
	TInt iNumImageSizesSupported;
	/** The supported still image formats.
	
	This is a bitfield of CCamera::TFormat values. */
	TUint32 iImageFormatsSupported;

	/** Count of video frame sizes allowed. 
	
	This is the number of different video frame sizes that CCamera::EnumerateVideoFrameSizes() 
	will support, based on the specified index. The index must be between 0 and 
	iNumVideoFrameSizesSupported-1. */
	TInt iNumVideoFrameSizesSupported;
	/** Count of video frame rates allowed.
	
	This is the number of different video frame rates that CCamera::EnumerateVideoFrameRates() 
	will support, based on the specified index. The index must be between 0 and 
	iNumVideoFrameRatesSupported-1. */
	TInt iNumVideoFrameRatesSupported;
	/** The supported video frame formats.

	This is a bitfield of video frame CCamera::TFormat values. 
	
	If methods CCamera::New2L() or CCamera::NewDuplicate2L() are not used to create the CCamera object, it is assumed that the 
	application may not be able to cope with any future additions to the enum values. So, any unrecognised enum value passed 
    from the implementation should be filtered by the ECAM implementation.
    To receive unrecognised/extra added enum values, the application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create the camera object. This is an indication to the ECAM implementation. In this case, the application is assumed 
    to be prepared to receive unrecognised enum values.	
	*/
	TUint32 iVideoFrameFormatsSupported;
	/** Maximum number of frames per buffer that may be requested. */
	TInt iMaxFramesPerBufferSupported;
	/** Maximum number of buffers allowed */
	TInt iMaxBuffersSupported;
	};

/** Mixin base class for camera clients.

An application must implement an MCameraObserver or MCameraObserver2 (recommended)
in order to use the camera API. This derived class is called when the camera is 
ready for use, an image has been captured or a buffer of video data is ready, including 
when errors occur.

Implementations of the camera API should use MCameraObserver::FrameBufferReady()
and MFrameBuffer::Release() to co-ordinate the mapping of any special memory
objects.

@publishedAll
@released
*/
class MCameraObserver
	{
public:
	/** Camera reservation is complete.

	Called asynchronously when CCamera::Reserve() completes.

	@param  aError
	        An error on failure and KErrNone on success.
	*/
	virtual void ReserveComplete(TInt aError)=0;

	/** Indicates camera power on is complete.

	Called on completion of CCamera:PowerOn().

	@param  aError
	        KErrNone on success, KErrInUse if the camera is in
	        use by another client or KErrNoMemory if insufficient system memory is available.
	*/
	virtual void PowerOnComplete(TInt aError)=0;

	/** Tests whether transfer of view finder data has completed.

	Called periodically in response to the use of CCamera::StartViewFinderBitmapsL().

	@param  aFrame
	        The view finder frame.
	*/
	virtual void ViewFinderFrameReady(CFbsBitmap& aFrame)=0;

	/** Transfers the current image from the camera to the client.

	Called asynchronously when CCamera::CaptureImage() completes.

	@param  aBitmap
	        On return, a pointer to an image held in CFbsBitmap form if
	        this was the format specified in CCamera::PrepareImageCaptureL().
	@param  aData
	        On return, a pointer to an HBufC8 if this was the format specified
	        in CCamera::PrepareImageCaptureL(). NULL if there was an error.
	@param  aError
	        KErrNone on success or an error code on failure.
	*/
	virtual void ImageReady(CFbsBitmap* aBitmap,HBufC8* aData,TInt aError)=0;

	/** Passes a filled frame buffer to the client.

	Called asynchronously, when a buffer has been filled with the required number
	of video frames by CCamera::StartVideoCapture().

	@param  aFrameBuffer
	        On return, a pointer to an MFrameBuffer if successful,
	        or NULL if not successful.
	@param  aError
	        KErrNone if successful, or an error code if not successful.
	*/
	virtual void FrameBufferReady(MFrameBuffer* aFrameBuffer,TInt aError)=0;
	};


/**
Class used for passing camera picture data between camera and client in the V2 observer.
Used for viewfinder, image capture and video capture.

The class offers APIs for the client to access the data as a descriptor, a bitmap
or a handle to a kernel chunk. Depending on the format previously requested by
the client, one or more of the API choices may be inappropriate e.g. an image will
not be avaiable as a bitmap in the FBS unless the client has specifically requested
it.

The buffer may contain multiple frames.

@publishedAll
@released
*/
class MCameraBuffer
	{
public:
	/** 
	Returns the number of frames of image data contained within the buffer. This
	would be 1 for a image capture, and match the requested count
	for video capture. For other methods in this class that take a aFrameIndex
	param, 0 <= aFrameIndex < NumFrames()

	@return The number of frames of image data in the buffer. 
	*/
	virtual TInt NumFrames()=0;
	/**
	Returns a pointer to a descriptor containing a frame of camera data. The format
	will have been previously specified by a CCamera class method.

	@param  aFrameIndex
	        The index of the required frame. For a still image this should be 0.

	@leave  KErrArgument if aIndex is out of range and 
	@leave  KErrNotSupported if the camera data is actually bitmaps in the FBS.

	@return A pointer to a descriptor containing a frame of image data.
	*/
	virtual TDesC8* DataL(TInt aFrameIndex)=0;

	/** 
	Returns a reference to a FBS bitmap containing a frame of image data.

	@param  aFrameIndex 
	        The index of the required frame. For a still image this should be 0.

	@leave  KErrArgument if aIndex is out of range and 
	@leave  KErrNotSupported if the picture data is not a FBS bitmap.

	@return A reference to a FBS bitmap containing a frame of picture data. 
	*/
	virtual CFbsBitmap& BitmapL(TInt aFrameIndex)=0;

	/** 
	Returns a handle for the chunk that contains the camera data.
	The RChunk is exposed so that it can potentially be shared between multiple
	processes.
	The ptr returned by DataL(aFrameIndex) is effectively derived from this
	RChunk (where both are supported simulataneously). The equivalent ptr would be:
	TPtrC8* ptr;
	ptr.Set(ChunkL().Base() + ChunkOffset(aFrameIndex), FrameSize(aFrameIndex));

	@leave  KErrNotSupported if the chunk is not available.

	@return A reference to a handle to the chunk that contains the buffer of picture data. 
	*/
	virtual RChunk& ChunkL()=0;

	/** 
	Returns the offset into the chunk that contains the frame specified by aFrameIndex.
	The client would add this offset to the ptr returned by ChunkL().Base() to
	get the address of the start of the frame data. 
	
	@param  aIndex 
	        The index of the required frame. For a still image this should be 0.

	@leave  KErrNotSupported if the chunk is not available 
	@leave  KErrArgument if aIndex is out of range.

	@return The offset into the chunk for the start of the frame. 
	*/
	virtual TInt ChunkOffsetL(TInt aFrameIndex)=0;

	/** 
	Returns the size of the data in bytes that comprises the frame specified by aIndex.
	
	@param  aFrameIndex 
	        The index of the required frame. For a still image this should be 0.

	@leave  KErrArgument 
			if aIndex is out of range.

	@return Returns the size of the data in bytes that comprises the frame. 
	*/
	virtual TInt FrameSize(TInt aFrameIndex)=0;

	/** 
	Releases the buffer. Once the client has processed
	the picture data it should use this method to signal to CCamera that the
	buffer can be re-used.
	*/
	virtual void Release()=0;
public:

	/** 
	Sequential frame number of the first frame in the buffer, counting from when
	CCamera::StartVideoCapture() was called and including frames dropped due to
	lack of buffers. Always zero for still images. May also be used by client viewfinders.
	*/
	TInt iIndexOfFirstFrameInBuffer;

	/** 
	Time elapsed from when CCamera::StartVideoCapture() was called until the first
	frame in the buffer was captured. Always zero for still images.
	*/
	TTimeIntervalMicroSeconds iElapsedTime;
	};

/** 
	Uid used to identify the event that the request to Reserve() has completed
*/
static const TUid  KUidECamEventReserveComplete = {0x101F7D3B};

/** 
	Uid used to identify the event that the request to PowerOn() has completed
*/
static const TUid  KUidECamEventPowerOnComplete = {0x101F7D3C};

/** 
	Uid used to identify the event that the client has lost
	control of the camera
*/
static const TUid  KUidECamEventCameraNoLongerReserved = {0x101F7D3D};

/** 
Uid used to notify the client who made the new reserve request. 

Error value KErrECamHighPriorityReserveRequesterExists indicates another such reserve request is outstanding and
has higher priority than this one.
Error value KErrCancel indicates a new reserve requester with higher priority than the current requester has 
been made. That's why, this one got cancelled.
Any other error may also occur.
@see CCamera::CCameraAdvancedSettings::ReserveL(const TTimeIntervalMicroseconds32& aMaxTimeToWait, TBool aKickOut);
*/
static const TUid  KUidECamEventNewReserveComplete = {KUidECamEventNewReserveCompleteUidValue};

/**
@publishedAll
@released

General purpose class to describe an ECam event.

Contains a UID to define the actual event type, and an integer to define the event code.

*/

class TECAMEvent
	{
public:
	
	/**
	Constructor.
	
	@param  aEventType
	        A UID to define the type of event.
	@param  aErrorCode
	        The error code associated with the event.

	*/
	IMPORT_C TECAMEvent(TUid aEventType, TInt aErrorCode);

	/**
	Default constructor.
	
	Provided so this class can be packaged in a TPckgBuf<>.
	*/
	IMPORT_C TECAMEvent();
	
	/**
	A UID to define the event type.
	*/
	TUid iEventType;

	/**
	The error code associated with the event.
	*/
	TInt iErrorCode;
	};

/** 
Uid used to identify a particular version of TECAMEvent base class being used i.e. TECAMEvent2 . 
Useful for MCameraObserver2::HandleEvent implementation to detect the version of TECAMEvent base class.
*/
static const TUid  KUidECamEventClass2 = {KUidECamEventClass2UidValue};

/** 
event indicating setting of color entry in the color swap operation. This is a part of class CCamera::CCameraImageProcessing. 
This event should be packed in TECAMEvent2 class.

@note  TECAMEvent2::iParam represents color entry.
*/
static const TUid  KUidECamEventCIPSetColorSwapEntry 		   = {KUidECamEventCIPSetColorSwapEntryUidValue};
static const TUid  KUidECamEvent2CIPSetColorSwapEntry 		   = {KUidECamEventCIPSetColorSwapEntryUidValue};

/** 
event indicating removal of color entry in the color swap operation. This is a part of class CCamera::CCameraImageProcessing. 
This event should be packed in TECAMEvent2 class.

@note  TECAMEvent2::iParam represents color entry.
*/
static const TUid  KUidECamEventCIPRemoveColorSwapEntry 	   = {KUidECamEventCIPRemoveColorSwapEntryUidValue};
static const TUid  KUidECamEvent2CIPRemoveColorSwapEntry 	   = {KUidECamEventCIPRemoveColorSwapEntryUidValue};

/** 
event indicating setting of color entry in the color accent operation. This is a part of class CCamera::CCameraImageProcessing. 
This event should be packed in TECAMEvent2 class.

@note  TECAMEvent2::iParam represents color entry.
*/
static const TUid  KUidECamEventCIPSetColorAccentEntry 		   = {KUidECamEventCIPSetColorAccentEntryUidValue};
static const TUid  KUidECamEvent2CIPSetColorAccentEntry 	   = {KUidECamEventCIPSetColorAccentEntryUidValue};

/** 
event indicating removal of color entry in the color accent operation. This is a part of class CCamera::CCameraImageProcessing. 
This event should be packed in TECAMEvent2 class.

@note  TECAMEvent2::iParam represents color entry.
*/
static const TUid  KUidECamEventCIPRemoveColorAccentEntry 	   = {KUidECamEventCIPRemoveColorAccentEntryUidValue};
static const TUid  KUidECamEvent2CIPRemoveColorAccentEntry 	   = {KUidECamEventCIPRemoveColorAccentEntryUidValue};

/** 
event indicating issue of pre capture warnings. This is a part of class CCamera::CCameraAdvancedSettings. 
This event should be packed in TECAMEvent2 class.

This TUid is available from the following methods only to the API clients using CCamera::New2L() or CCamera::
NewDuplicate2L():-
void CCamera::CCameraAdvancedSettings::GetSupportedSettingsL(RArray<TUid>& aSettings) const;
void CCamera::CCameraAdvancedSettings::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
void CCamera::CCameraAdvancedSettings::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;

@note  TECAMEvent2::iParam represents bit field describing all PreCaptureWarnings currently issued.
*/
static const TUid  KUidECamEventCameraSettingPreCaptureWarning 	= {KUidECamEventCameraSettingPreCaptureWarningUidValue};
static const TUid  KUidECamEvent2CameraSettingPreCaptureWarning = {KUidECamEventCameraSettingPreCaptureWarningUidValue};
	
/**
Special type of TECAMEvent class used to retrieve some extra information from particular events received

@publishedAll
@released
*/	
class TECAMEvent2 : public TECAMEvent
	{
public:

	IMPORT_C static TBool IsEventEncapsulationValid(const TECAMEvent& aECAMEvent);

	IMPORT_C TECAMEvent2(TUid aEventType, TInt aErrorCode, TInt aParam);
	
	IMPORT_C const TUid& EventClassUsed() const;
	
private:
	/** 
	Uid representing this version of TECAMEvent base class. Uid used is KUidECamEventClass2 
	*/
	TUid iEventClassUsed;
	
	/**
	Reserved for future
	TInt iReserved1; -> Made Public TInt iParam1
	*/
	
	/**
	Reserved for future	
	*/
	TInt iReserved2; 
	
	
public:
	/**
	iParam1 will be used to provide extra information if iParam is not sufficient.
	This signifies different things for different valid events.
				
	Future events may also use this class member variable.
	*/
	TInt iParam1;
	
	/**
	This signifies different things for different valid events.
				
	Future events may also use this class member variable.
	*/	
	TInt iParam;
	};

/** Mixin base class V2 for camera clients.

An application must implement an MCameraObserver2 (or MCameraObserver) in order to use the camera
API. This derived class is called when the camera is ready for use, an image
has been captured or a buffer of video data is ready, including when errors
occur.

@publishedAll
@released
*/

class MCameraObserver2
	{
public:
	/** 
	A camera event has completed.
	@note Implementations of MCameraObserver2 should ignore events which are not recognised and should not leave.

	@param  aEvent
	        A reference to a TECAMEvent. This can be completion of a call to Reserve() 
			or a call to PowerOn() or a notification that the client has lost control 
			of the camera.
			The event contains a uid identifying the event and an accompanying
			error code (KErrNone for the successful completion of a request).
			The error will be KErrNotReady for a request that was made out of the
			correct sequence.
			The error will be KErrAccessDenied for a Reserve() request that failed
			because a higher priority client already controls the camera.
	
	@note   This may internally call TECAMEvent2::IsEventEncapsulationValid(aEvent) and also for any other derived version of TECAMEvent 
			class to know whether correct version of TECAMEvent base class has been used. 
	*/
	virtual void HandleEvent(const TECAMEvent& aEvent)=0;

	/** 
	Notifies client of new view finder data. Called periodically in response to 
	the use of CCamera::StartViewFinderL().

	@param  aCameraBuffer
	        A reference to an MCameraBuffer if successful, or NULL if not successful.

	@param  aError
	        KErrNone if successful, or an error code if not successful.
	*/
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError)=0;

	/** 
	Notifies the client of a new captured camera image. Called asynchronously 
	when CCamera::CaptureImage() completes.

	@param  aCameraBuffer
	        A reference to an MCameraBuffer if successful, or NULL if not successful.

	@param  aError
	        KErrNone if successful, or an error code if not successful.
  
	@note   If new image capture classes used, then this callback will not be used. Refer MCaptureImageObserver
	*/
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)=0;

	/** 
	Notifies the client of new captured video. Called asynchronously and periodically
	after CCamera::StartVideoCapture() is called. The buffer has been filled with the 
	required number of video frames specified PrepareVideoCaptureL().

	@param  aCameraBuffer
	        A reference to an MCameraBuffer if successful, or NULL if not successful.

	@param  aError
	        KErrNone if successful, or an error code if not successful.
	*/
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)=0;
	};


/** Base class for camera devices.

Provides the interface that an application uses to control, and acquire images
from, the camera.

An application must supply an implementation of MCameraObserver2 (or MCameraObserver).

@publishedAll
@released
*/
class CCamera : public CBase

	{
	
	friend class CCameraPlugin;

public:
	class CCameraPresets;
	class CCameraAdvancedSettings;
	class CCameraImageProcessing;
	class CCameraHistogram;
	class CCameraV2Histogram;
	class CCameraOverlay;
	class CCameraSnapshot;
	class CCameraDirectViewFinder;
	class CCameraV2DirectViewFinder;
	class CCameraClientViewFinder;
	class CCameraPreImageCaptureControl;
	class CCameraImageCapture;
	class CCameraPostImageCaptureControl;
	class CCameraVideoCaptureControl;
    class CCameraDirectSnapshot;
    class CCameraContinuousZoom;

public:
	/** Possible still image and video frame formats

	Formats are read from left to right, starting at the top of the image. YUV
	format is as defined by ITU-R BT.601-4. */
	enum TFormat
		{
		/** 8 bit greyscale values, 0=black, 255=white. */
		EFormatMonochrome			= 0x0001,
		/** Packed RGB triplets, 4 bits per pixel with red in the least significant bits
		and the 4 most significant bits unused. */
		EFormat16bitRGB444			= 0x0002,
		/** Packed RGB triplets, 5 bits per pixel for red and blue and 6 bits for green,
		with red in the least significant bits. */
		EFormat16BitRGB565			= 0x0004,
		/** Packed RGB triplets, 8 bits per pixel with red in the least significant bits
		and the 8 most significant bits unused. */
		EFormat32BitRGB888			= 0x0008,
		/** JFIF JPEG. */
		EFormatJpeg					= 0x0010,
		/** EXIF JPEG */
		EFormatExif					= 0x0020,
		/** CFbsBitmap object with display mode EColor4K. */
		EFormatFbsBitmapColor4K		= 0x0040,
		/** CFbsBitmap object with display mode EColor64K. */
		EFormatFbsBitmapColor64K	= 0x0080,
		/** CFbsBitmap object with display mode EColor16M. */
		EFormatFbsBitmapColor16M	= 0x0100,
		/** Implementation dependent. */
		EFormatUserDefined			= 0x0200,
		/** 4:2:0 format, 8 bits per sample, Y00Y01Y10Y11UV. */
		EFormatYUV420Interleaved	= 0x0400,
		/** 4:2:0 format, 8 bits per sample, Y00Y01Y02Y03...U0...V0... */
		EFormatYUV420Planar			= 0x0800,
		/** 4:2:2 format, 8 bits per sample, UY0VY1.  Maps to Graphics' EUidPixelFormatYUV_422Interleaved in pixelformats.h. */
		EFormatYUV422				= 0x1000,
		/** 4:2:2 format, 8 bits per sample, Y1VY0U.  Maps to Graphics' EUidPixelFormatYUV_422InterleavedReversed in pixelformats.h. */
		EFormatYUV422Reversed		= 0x2000,
		/** 4:4:4 format, 8 bits per sample, Y00U00V00 Y01U01V01... */
		EFormatYUV444				= 0x4000,
		/** 4:2:0 format, 8 bits per sample, Y00Y01Y02Y03...U0V0... */
		EFormatYUV420SemiPlanar		= 0x8000,
		/** CFbsBitmap object with display mode EColor16MU. */
		EFormatFbsBitmapColor16MU 	= 0x00010000,
		/** Motion JPEG for video 
		@note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L().
		@internalTechnology
		*/
		EFormatMJPEG				= 0x00020000,
		
		/** 
		Compressed H264 video format. 
		@note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L().
		@prototype
		*/
		EFormatEncodedH264          = 0x00040000,

		/**
		4:2:2 format, 8 bits per sample, Y0UY1V. Maps to Graphics' EUidPixelFormatYUV_422Reversed in pixelformats.h.
		@note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L().
		@prototype
		*/
		EFormatYUV222ReversedV2		= 0x00080000
		};
		
	/** Specifies whether contrast is set automatically. */
	enum TContrast
		{
		/** Sets the contrast automatically. */
		EContrastAuto		= KMinTInt
		};
	/** Specifies whether brightness is set automatically. */
	enum TBrightness
		{
		/** Sets the brightness automatically. */
		EBrightnessAuto		= KMinTInt
		};
	/** Specifies the type of flash. */
	enum TFlash
		{
		/** No flash, always supported. */
		EFlashNone			= 0x0000,
		/** Flash will automatically fire when required. */
		EFlashAuto			= 0x0001,
		/** Flash will always fire. */
		EFlashForced		= 0x0002,
		/** Reduced flash for general lighting */
		EFlashFillIn		= 0x0004,
		/** Red-eye reduction mode. */	
		EFlashRedEyeReduce	= 0x0008,
		/** Flash at the moment when shutter opens. */
		EFlashSlowFrontSync = 0x0010,
		/** Flash at the moment when shutter closes. */
		EFlashSlowRearSync  = 0x0020, 
		/** User configurable setting */	
		EFlashManual		= 0x0040,
		/** Constant emission of light during video mode  
            @note This value is available only to the API clients using CCamera::New2L() or CCamera::NewDuplicate2L()
		*/
		EFlashVideoLight    = 0x0080
		};
	/** Specifies the type of exposure. - EExposureAuto is the default value. */
	enum TExposure
		{
		/** Set exposure automatically. Default, always supported.
		This may imply auto aperture so clients may receive a KUidEcamEvent2CameraSettingAutoAperture event notification some time later. */
		EExposureAuto		= 0x0000,
		/** Night-time setting for long exposures. */
		EExposureNight		= 0x0001,
		/** Backlight setting for bright backgrounds. */
		EExposureBacklight	= 0x0002,
		/** Centered mode for ignoring surroundings. */
		EExposureCenter		= 0x0004,
		/** Sport setting for very short exposures. */
		EExposureSport 		= 0x0008,
		/** Generalised setting for very long exposures. */
		EExposureVeryLong 	= 0x0010,
		/** Snow setting for daylight exposure. */
		EExposureSnow 		= 0x0020,
		/** Beach setting for daylight exposure with reflective glare. */
		EExposureBeach 		= 0x0040,
		/** Programmed exposure setting. */
		EExposureProgram 	= 0x0080,
		/** Aperture setting is given priority. */
		EExposureAperturePriority 	= 0x0100,
		/** Shutter speed setting is given priority.
		This may imply auto aperture so clients may receive a KUidEcamEvent2CameraSettingAutoAperture event notification some time later. */
		EExposureShutterPriority	= 0x0200,
		/** User selectable exposure value setting. */	
		EExposureManual				= 0x0400,
		/** Exposure night setting with colour removed to get rid of colour noise. */
		EExposureSuperNight			= 0x0800,
		/** Exposure for infra-red sensor on the camera */
		EExposureInfra				= 0x1000
		};
		
	/** Specifies how the white balance is set. */
	enum TWhiteBalance
		{
		/** Set white balance automatically. Default, always supported. */
		EWBAuto				= 0x0000,
		/** Normal daylight. */
		EWBDaylight			= 0x0001,
		/** Overcast daylight. */
		EWBCloudy			= 0x0002,
		/** Tungsten filament lighting. */
		EWBTungsten			= 0x0004,
		/** Fluorescent tube lighting */
		EWBFluorescent		= 0x0008,
		/** Flash lighting. */
		EWBFlash			= 0x0010,
		/** High contrast daylight primarily snowy */
		EWBSnow 			= 0x0020,
		/** High contrast daylight primarily near the sea */
		EWBBeach 			= 0x0040,
		/** User configurable mode */
		EWBManual 			= 0x0080,
		/** Shade */
 		EWBShade			= 0x0100,
 		/** auto skin

 		If New2L()/NewDuplicate2L() are not used to create camera object, this
		enum value would be considered as unrecognized and filtered out in 'supported'
		or 'getter' methods.
 		*/
 		EWBAutoSkin			= 0x0200,
 		/** horizon

 		If New2L()/NewDuplicate2L() are not used to create camera object, this
		enum value would be considered as unrecognized and filtered out in 'supported'
		or 'getter' methods.
		*/
 		EWBHorizon 			= 0x0400,
 		/** Daylight Under Water

 		If New2L()/NewDuplicate2L() are not used to create camera object, this
		enum value would be considered as unrecognized and filtered out in 'supported'
		or 'getter' methods.
		*/
 		EWBDaylightUnderWater  = 0x0800
		};

public:
	/**
	Determines the number of cameras on the device.

    @return Count of cameras present on the device.
	*/
	IMPORT_C static TInt CamerasAvailable();

    /**
    @deprecated Use static CCamera* New2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority);

	Creates an object representing a camera.

	@param  aObserver
	        Reference to class derived from MCameraObserver2 designed to receive
	        notification of asynchronous event completion.
	@param	aCameraIndex
	        Index from 0 to CamerasAvailable()-1 inclusive specifying the
	        camera device to use.
	@param	aPriority
	        Value from -100 to 100 indicating relative priority of client to
	        use camera.

	@return Pointer to a fully constructed CCamera object. Ownership is passed
	        to the caller.

	@leave  KErrNoMemory if out of memory.
	@leave  KErrNotSupported if aCameraIndex is out of range.
	@leave  KErrPermissionDenied if the application does not have
	        the UserEnvironment capability.

	@capability	UserEnvironment
			An application that creates a CCamera object must have
			the UserEnvironment capability.

    @capability MultimediaDD
	        A process requesting or using this method that has MultimediaDD capability will
			always have precedence over a process that does not have MultimediaDD.

	@note   Applications using this method to create camera object may not receive enums/uids added in future(after being baselined).
			To receive them, they should rather use New2L() or NewDuplicate2L(), in which case, they should prepare
			themselves to receive unrecognised values.
	*/
	IMPORT_C static CCamera* NewL(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority);

	/**
	Creates an object representing a camera.
	Clients prepare themselves to receive unrecognised enum, uids etc.

	@param  aObserver
	        Reference to class derived from MCameraObserver2 designed to receive
	        notification of asynchronous event completion.
	@param	aCameraIndex
	        Index from 0 to CamerasAvailable()-1 inclusive specifying the
	        camera device to use.
	@param	aPriority
	        Value from -100 to 100 indicating relative priority of client to
	        use camera.

	@return Pointer to a fully constructed CCamera object. Ownership is passed
	        to the caller.

	@leave  KErrNoMemory if out of memory.
	@leave  KErrNotSupported if aCameraIndex is out of range.
	@leave  KErrPermissionDenied if the application does not have
	        the UserEnvironment capability.

	@capability	UserEnvironment
			An application that creates a CCamera object must have
			the UserEnvironment capability.

    @capability MultimediaDD
	        A process requesting or using this method that has MultimediaDD capability will
			always have precedence over a process that does not have MultimediaDD.

	@note   Clients using this creation method should prepare themselves to receive any unrecognised enum values, uids
			from 'supported' or 'getter' methods
	*/
	IMPORT_C static CCamera* New2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority);

	/**
	Creates an object representing a camera.

	@param  aObserver
	        Reference to class derived from MCameraObserver designed to receive
	        notification of asynchronous event completion.
	@param	aCameraIndex
	        Index from 0 to CamerasAvailable()-1 inclusive specifying the
	        camera device to use.

	@leave  KErrNoMemory if out of memory.
	@leave  KErrNotSupported if aCameraIndex is out of range.
	@leave  KErrPermissionDenied if the application does not have
	        the UserEnvironment capability.

	@return Pointer to a fully constructed CCamera object. Ownership is passed
	        to the caller.

	@capability	UserEnvironment
				An application that creates a CCamera object must have
				the UserEnvironment capability.

	@note   Applications using this method to create camera object may not receive enums/uids added in future(after being baselined).
			To receive them, they should rather use New2L() or NewDuplicate2L(), in which case, they should prepare
			themselves to receive unrecognised values.
	*/
	IMPORT_C static CCamera* NewL(MCameraObserver& aObserver,TInt aCameraIndex);

	/**
	@deprecated Use static CCamera* NewDuplicate2L(MCameraObserver2& aObserver,TInt aCameraHandle);

	Duplicates the original camera object for use by, for example, multimedia systems.

	May leave with KErrNoMemory or KErrNotFound if aCameraHandle is not valid.

	@param  aObserver
	        Reference to an observer.
	@param  aCameraHandle Handle of an existing camera object.

	@leave  KErrNoMemory if out of memory.
	@leave  KErrNotFound if aCameraHandle is not valid.	   
	@leave  KErrPermissionDenied if the application does not have
	        the UserEnvironment capability.

	@return Duplicate of the original camera object. 
	
	@capability	UserEnvironment
				An application that creates a CCamera object must have
				the UserEnvironment capability.
	
	@note   Applications using this method to create camera object may not receive enums/uids added in future(after being baselined). 
			To receive them, they should rather use New2L() or NewDuplicate2L(), in which case, they should prepare 
			themselves to receive unrecognised values.
	*/
	IMPORT_C static CCamera* NewDuplicateL(MCameraObserver2& aObserver,TInt aCameraHandle);
	
	/** 
	Duplicates the original camera object for use by, for example, multimedia systems.
	Clients prepare themselves to receive unrecognised enum, uids etc.

	May leave with KErrNoMemory or KErrNotFound if aCameraHandle is not valid.

	@param  aObserver
	        Reference to an observer.
	@param  aCameraHandle Handle of an existing camera object.

	@leave  KErrNoMemory if out of memory.
	@leave  KErrNotFound if aCameraHandle is not valid.	   
	@leave  KErrPermissionDenied if the application does not have
	        the UserEnvironment capability.

	@return Duplicate of the original camera object. 
	
	@capability	UserEnvironment
				An application that creates a CCamera object must have
				the UserEnvironment capability.
				
	@note   Clients using this creation method should prepare themselves to receive any unrecognised enum values, uids 
			from 'supported' or 'getter' methods
	*/
	IMPORT_C static CCamera* NewDuplicate2L(MCameraObserver2& aObserver,TInt aCameraHandle);
	
	/** 
	Duplicates the original camera object for use by, for example, multimedia systems.

	@leave  KErrNoMemory if out of memory.
	@leave  KErrNotFound if aCameraHandle is not valid.	   
	@leave  KErrPermissionDenied if the application does not have
	        the UserEnvironment capability.

	@param  aObserver
	        Reference to an observer.
	@param  aCameraHandle Handle of an existing camera object.

	@return Duplicate of the original camera object. 
	
	@capability	UserEnvironment
				An application that creates a CCamera object must have
				the UserEnvironment capability.
				
	@note   Applications using this method to create camera object may not receive enums/uids added in future(after being baselined). 
			To receive them, they should rather use New2L() or NewDuplicate2L(), in which case, they should prepare 
			themselves to receive unrecognised values.
	*/
	IMPORT_C static CCamera* NewDuplicateL(MCameraObserver& aObserver,TInt aCameraHandle);	

	/** 
	Gets information about the camera device.

	@param  aInfo 
	        On return, information about the camera device. See TCameraInfo. 
	*/
	virtual void CameraInfo(TCameraInfo& aInfo) const=0;

	/** 
	Asynchronous function that performs any required initialisation and reserves
	the camera for exclusive use.

	Calls MCameraObserver:: ReserveComplete() when complete. 
	*/
	virtual void Reserve()=0;

	/** 
	De-initialises the camera, allowing it to be used by other clients. 
	*/
	virtual void Release()=0;

	/** 
	Asynchronous method to switch on camera power.

	User must have successfully called Reserve() prior to calling this function.

	Calls MCameraObserver::PowerOnComplete() when power on is complete. 
	*/
	virtual void PowerOn()=0;

	/** 
	Synchronous function for switching off camera power. 
	*/
	virtual void PowerOff()=0;

	/**
	Gets the device-unique handle of this camera object.

	@return  The device-unique handle of this camera object. 
	*/
	virtual TInt Handle()=0;

	/** 
	Sets the zoom factor.

	This must be in the range of TCameraInfo::iMinZoom to TCameraInfo::iMaxZoom
	inclusive. May leave with KErrNotSupported if the specified zoom factor is
	out of range.

	@param aZoomFactor 
	       Required zoom factor.
	*/
	virtual void SetZoomFactorL(TInt aZoomFactor = 0)=0;

	/** 
	Gets the currently set zoom factor.

	@return  The currently set zoom factor.
	*/
	virtual TInt ZoomFactor() const=0;

	/** 
	Sets the digital zoom factor.

	This must be in the range of 0 to TCameraInfo::iMaxDigitalZoom inclusive.

	May leave with KErrNotSupported if the zoom factor is out of range.

	@param  aDigitalZoomFactor
	        The required digital zoom factor. 
	*/
	virtual void SetDigitalZoomFactorL(TInt aDigitalZoomFactor = 0)=0;

	/** 
	Gets the currently set digital zoom factor.

	@return  The currently set digital zoom factor. 
	*/
	virtual TInt DigitalZoomFactor() const=0;

	/**
	Sets the contrast adjustment of the device.

	This must be in the range of -100 to +100 or EContrastAuto. May leave with
	KErrNotSupported if the specified contrast value is out of range.

	@param  aContrast 
	        Required contrast value. See TCameraInfo::iContrastSupported 
	*/
	virtual void SetContrastL(TInt aContrast)=0;

	/** 
	Gets the currently set contrast value.

	@return  The currently set contrast value.
	*/
	virtual TInt Contrast() const=0;

	/** 
	Sets the brightness adjustment of the device.

	No effect if this is not supported, see TCameraInfo::iBrightnessSupported.

	This must be in the range of -100 to +100 or EBrightnessAuto. May leave
	with KErrNotSupported if the brightness adjustment is out of range.

	@param  aBrightness
	        The required brightness adjustment. 
	*/
	virtual void SetBrightnessL(TInt aBrightness)=0;

	/** 
	Gets the currently set brightness adjustment value.

	@return  The currently set brightness adjustment value. 
	*/
	virtual TInt Brightness() const=0;

	/** 
	Sets the flash mode.

	No effect if this is not supported, see TCameraInfo::iFlashModesSupported.

	May leave with KErrNotSupported if the specified flash mode is invalid.

	@param  aFlash
	        The required flash mode. 
	*/
	virtual void SetFlashL(TFlash aFlash = EFlashNone)=0;

	/** 
	Gets the currently set flash mode.

	@return  The currently set flash mode. 
	@note	if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added enum values (unrecognised). So, any extra enum value(unrecognised)
    (set in the ECAM implementation because of sharing clients) should not be returned from the ECAM implementation.
    To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. ECAM implementation, after verifying this,(by checking version no.) may send new values, if set.
    In this case, application is assumed to be prepared to receive unrecognised enum values.
    
    @see CCamera::CCameraAdvancedSettings::FlashMode()	
	*/
	virtual TFlash Flash() const=0;

	/** 
	Sets the exposure adjustment of the device.

	No effect if this is not supported, see CameraInfo::iExposureModesSupported.

	May leave with KErrNotSupported if the specified exposure adjustment is invalid.

	@param  aExposure
	        The required exposure adjustment. 
	*/
	virtual void SetExposureL(TExposure aExposure = EExposureAuto)=0;

	/** 
	Gets the currently set exposure setting value.

	@return  The currently set exposure setting value. 
	*/
	virtual TExposure Exposure() const=0;

	/** 
	Sets the white balance adjustment of the device.

	No effect if this is not supported, see TCameraInfo::iWhiteBalanceModesSupported.

	@param  aWhiteBalance
	        The required white balance adjustment.

	@leave  KErrNotSupported if the specified white balance adjustment
	        is invalid.
	*/
	virtual void SetWhiteBalanceL(TWhiteBalance aWhiteBalance = EWBAuto)=0;

	/** 
	Gets the currently set white balance adjustment value.

	@return  The currently set white balance adjustment value.
	
	@note	if CCamera::New2L() or CCamera::NewDuplicate2L() is not used to create CCamera object, it is assumed that 
    application is not prepared to receive extra added enum values (unrecognised). So, any extra enum value(unrecognised)
    (set in the ECAM implementation because of sharing clients) should not be returned from the ECAM implementation.
    To receive extra added enum values, application should rather use CCamera::New2L() or CCamera::NewDuplicate2L() 
    to create camera object. ECAM implementation, after verifying this,(by checking version no.) may send new values, if set.
    In this case, application is assumed to be prepared to receive unrecognised enum values.
    Refer  CCamera::CCameraAdvancedSettings::WhiteBalanceMode() implementation
    
    @see CCamera::CCameraAdvancedSettings::WhiteBalanceMode()
    */
	virtual TWhiteBalance WhiteBalance() const=0;

	/** 
	Starts transfer of view finder data to the given portion of the screen using
	direct screen access.

	The aScreenRect parameter is in screen co-ordinates and may be modified if,
	eg, the camera requires the destination to have a certain byte alignment, etc.

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

	@leave  KErrNotReady if PowerOn() has either not
	        been called, or has not yet completed.
	
	@note   This method is assumed to be meant for default display only. KECamDefaultViewFinderHandle should be used to 
			refer viewfinders started using CCamera methods. 
	
	@see	CCamera::CCameraV2DirectViewFinder
	*/
	virtual void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect)=0;

	/** 
	Starts transfer of view finder data to the given portion of the screen using
	direct screen access and also clips to the specified portion of the screen.

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
	        transferred. This is in screen co-ordinates and may be modified if, for example,
	        the camera requires the destination to have a certain byte alignment.
	@param  aClipRect
	        The rectangle to which the screen will be clipped.

    @leave  KErrNotReady if PowerOn() hasn't been called successfully. 
    
    @note   This method is assumed to be meant for default display only. KECamDefaultViewFinderHandle should be used to 
    		refer viewfinders started using CCamera methods.
	
	@see	CCamera::CCameraClientViewFinder
	*/
	virtual void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect)=0;

	/** 
	Starts transfer of view finder data.

	Bitmaps are returned by MCameraObserver::ViewFinderFrameReady().

	@param  aSize 
	        On return, the size used.
	@leave  KErrNotReady if PowerOn() has not been called, or has not yet completed.
	
	@note   This method is assumed to be meant for default display only. 
	
	@see	CCamera::CCameraClientViewFinder 
	*/
	virtual void StartViewFinderBitmapsL(TSize& aSize)=0;	

	/** 
	Starts transfer of view finder data and clips the bitmap to the specified clip
	rectangle.

	The bitmap is the size of the intersection of aSize and aClipRect, not simply
	aSize padded with white space.

	@param  aSize
	        On return, the size used.
	@param  aClipRect 
	        Required clip rectangle. May be modified if, for example,
	        the camera only supports certain byte alignments.

	@leave  KErrInUse if Reserve() hasn't been called successfully.
	@leave  KErrNotReady if PowerOn() hasn't been called successfully.
	
	@note   This method is assumed to be meant for default display only. 
	
	@see	CCamera::CCameraClientViewFinder
	*/
	virtual void StartViewFinderBitmapsL(TSize& aSize,TRect& aClipRect)=0;

	/** 
	Starts transfer of view finder data.

	Picture data is returned by MCameraObserver2::ViewFinderReady().

	@param  aImageFormat 
	        The image format requested by the client.
	@param  aSize 
	        On return, the size used.
	@leave  KErrNotSupported 
	@leave  KErrNotReady if Reserve() has not been
	        called, or has not yet completed. 
	
	@note   This method is assumed to be meant for default display only. KECamDefaultViewFinderHandle should be used to 
			refer viewfinders started using CCamera methods.
	
	@see	CCamera::CCameraClientViewFinder
	*/
	virtual void StartViewFinderL(TFormat aImageFormat,TSize& aSize)=0;

	/** 
	Starts transfer of view finder data and clips the picture to the specified clip
	rectangle. Picture data is returned by MCameraObserver2::ViewFinderReady().

	The picture is the size of the intersection of aSize and aClipRect, not simply
	aSize padded with white space.

	@param  aImageFormat 
	        The image format.
	@param  aSize
	        On return, the size used.
	@param  aClipRect 
	        Required clip rectangle. May be modified if, for example,
	        the camera only supports certain byte alignments.

	@leave  KErrInUse if Reserve() hasn't been called successfully,
	@leave  KErrNotReady if PowerOn() hasn't been called successfully.
	
	@note   This method is assumed to be meant for default display only. KECamDefaultViewFinderHandle should be used to 
			refer viewfinders started using CCamera methods.
	
	@see	CCamera::CCameraClientViewFinder
	*/
	virtual void StartViewFinderL(TFormat aImageFormat,TSize& aSize,TRect& aClipRect)=0;

	/** 
	Stops transfer of view finder data to the screen. 
	
	@note   This method is assumed to be meant for default display only. KECamDefaultViewFinderHandle should be used to 
			refer viewfinders started using CCamera methods.
	
	@see	CCamera::CCameraV2DirectViewFinder
	@see	CCamera::CCameraClientViewFinder
	*/
	virtual void StopViewFinder()=0;

	/** 
	Queries whether the view finder is active.

	@return  ETrue if the view finder is active. EFalse if the view finder is not
	         active. 
	         
	@note   This method is assumed to be meant for default display only. KECamDefaultViewFinderHandle should be used to 
			refer viewfinders started using CCamera methods.
	
	@see	CCamera::CCameraV2DirectViewFinder
	@see	CCamera::CCameraClientViewFinder
	*/
	virtual TBool ViewFinderActive() const=0;

	/** 
	Sets whether view finder mirroring is on.

	Used to switch between what the camera sees and what you would see if the
	device were a mirror.

	@param  aMirror 
	        ETrue to set mirroring on, EFalse to set mirroring off.

	@leave  KErrNotSupported.
	
	@note   This method is assumed to be meant for default display only. KECamDefaultViewFinderHandle should be used to 
			refer viewfinders started using CCamera methods.
	
	@see	CCamera::CCameraV2DirectViewFinder
	@see	CCamera::CCameraClientViewFinder
	*/
	virtual void SetViewFinderMirrorL(TBool aMirror)=0;

	/** 
	Gets whether view finder mirroring is active.

	@return  ETrue if mirroring is set, EFalse if mirroring is not set. 
	
	@note   This method is assumed to be meant for default display only. KECamDefaultViewFinderHandle should be used to 
			refer viewfinders started using CCamera methods.
	
	@see	CCamera::CCameraV2DirectViewFinder
	@see	CCamera::CCameraClientViewFinder
	*/
	virtual TBool ViewFinderMirror() const=0;

	/** 
	Performs setup and allocation of memory.

	Called prior to calling CaptureImage() to keep the latency of that function
	to a minimum.

	Needs to be called only once for multiple CaptureImage() calls. May leave
	with KErrNotSupported or KErrNoMemory or KErrInUse or KErrNotReady.

	The specified image format must be one of the formats supported
	(see TCameraInfo::iImageFormatsSupported).

	The specified size index must be in the range of 0 to TCameraInfo::iNumImageSizesSupported-1
	inclusive.

	@param  aImageFormat 
	        The image format.
	@param  aSizeIndex 
	        Size index.

	@leave  KErrNotSupported
	@leave  KErrNoMemory
	@leave  KErrNotReady if PowerOn() hasn't been called successfully.
	
	@note  Depending on the ECAM implementation and underlying hardware, preparing both image capture and video capture
		   at the same time may not be possible. In this case, the recommendation is to unprepare video capture before preparing
		   image capture if PrepareVideoCaptureL() has already been called.
	
	@see 	CCamera::CCameraPreImageCaptureControl::PrepareImageCapture(TPrepareImageParameters aPrepareImageParameters)
	*/
	virtual void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex)=0;

	/** 
	Performs setup and allocation of memory and clips the image to the specified
	rectangle.

	No effect unless TCameraInfo::iImageClippingSupported is set to ETrue. The
	image captured is the intersection of aClipRect and the rectangle from (0,0)
	to aSize. Needs to be called only once for multiple CaptureImage() calls.
	May leave with KErrNotSupported or KErrNoMemory.

	The specified image format must be one of the formats supported (see TCameraInfo::iImageFormatsSupported).

	The specified size index must be in the range of 0 to TCameraInfo::iNumImageSizesSupported-1
	inclusive.

	@param  aImageFormat 
	        The image format.
	@param  aSizeIndex 
	        Size index.
	@param  aClipRect
	        The rectangle to which the image is to be clipped.

	@leave   KErrNotSupported
	@leave   KErrNoMemory
	@leave   KErrInUse if Reserve() hasn't been called successfully
	@leave   KErrNotReady if PowerOn()
	         hasn't been called successfully.
	
	@note  Depending on the ECAM implementation and underlying hardware, preparing both image capture and video capture
		   at the same time may not be possible. In this case, the recommendation is to unprepare video capture before preparing
		   image capture if PrepareVideoCaptureL() has already been called.
	
	@see 	CCamera::CCameraPreImageCaptureControl::PrepareImageCapture(TPrepareImageParameters aPrepareImageParameters)
	*/
	virtual void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex,const TRect& aClipRect)=0;

	/** 
	Asynchronously performs still image capture.

	Calls MCameraObserver::ImageReady() when complete. 
	
	@see CCamera::CCameraImageCapture
	@see CCamera::CCameraPreImageCaptureControl
	*/
	virtual void CaptureImage()=0;

	/** 
	Cancels the asynchronous still image capture. 
	
	@see CCamera::CCameraImageCapture
	*/
	virtual void CancelCaptureImage()=0;

	/** 
	Enumerates through the available image capture sizes, based on the specified
	size index and format

	The size index must be in the range 0 to TCameraInfo::iNumImageSizesSupported-1
	inclusive.

	@param  aSize 
	        Image size.
	@param  aSizeIndex 
	        Size index.
	@param  aFormat 
	        The image format.
	*/
	virtual void EnumerateCaptureSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const=0;

	/** 
	Prepares for video capture.

	Performs setup and allocation of memory prior to calling StartVideoCapture()
	to keep the latency of that function to a minimum.

	May leave with KErrNotSupported or KErrNoMemory.

	@param  aFormat 
	        Format must be one of the video frame formats supported (see
	        TCameraInfo::iVideoFrameFormatsSupported).
	@param  aSizeIndex 
	        Size index  must be in the range 0 to TCameraInfo::iNumVideoFrameSizesSupported-1
	        inclusive.
	@param  aRateIndex 
	        The rate must be in the range 0 to TCameraInfo::iNumVideoFrameRatesSupported-1
	        inclusive.
	@param  aBuffersToUse 
	        The number of discrete buffers to use.
	@param  aFramesPerBuffer 
	        How large the buffers are to be. Must be less than
	        or equal to TCameraInfo::iMaxFramesPerBufferSupported. One buffer is returned
	        to MCameraObserver::FrameBufferReady() at a time.

	@leave  May leave with KErrNotSupported, KErrNoMemory, or KErrNotReady if PowerOn() 
			hasn't been called successfully.
	
	@note  Depending on the ECAM implementation and underlying hardware, preparing both image capture and video capture
		   at the same time may not be possible. In this case, the recommendation is to unprepare image capture before preparing
		   video capture if PrepareImageCaptureL() has already been called.
	
	@see 	CCamera::CCameraVideoCaptureControl::PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)
	*/
	virtual void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer)=0;

	/** 
	Prepares for video capture and clips the frames to the given rectangle.

	Performs setup and allocation of memory prior to calling StartVideoCapture()
	to keep the latency of that function to a minimum.

	May leave with KErrNotSupported or KErrNoMemory.

	@param  aFormat 
	        Format must be one of the video frame formats supported (see
	        TCameraInfo::iVideoFrameFormatsSupported).
	@param  aSizeIndex 
	        Size index must be in the range 0 to TCameraInfo::iNumVideoFrameSizesSupported-1
	        inclusive.
	@param  aRateIndex 
	        The rate must be in the range 0 to TCameraInfo::iNumVideoFrameRatesSupported-1
	        inclusive.
	@param  aBuffersToUse 
	        The number of discrete buffers to use.
	@param  aFramesPerBuffer 
	        How large the buffers are to be. Must be less than
	        or equal to TCameraInfo::iMaxFramesPerBufferSupported. One buffer is returned
	        to MCameraObserver::FrameBufferReady() at a time.
	@param  aClipRect 
	        The rectangle to which the image is to be clipped.
	@leave  KErrNotSupported
	@leave  KErrNoMemory, 
	@leave  KErrInUse if Reserve() hasn't been called successfully
	@leave  KErrNotReady if PowerOn() hasn't been called successfully.
	
	@note  Depending on the ECAM implementation and underlying hardware, preparing both image capture and video capture
		   at the same time may not be possible. In this case, the recommendation is to unprepare image capture before preparing
		   video capture if PrepareImageCaptureL() has already been called.

	@see 	CCamera::CCameraVideoCaptureControl::PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)
	*/
	virtual void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer,const TRect& aClipRect)=0;

	/** 
	Starts capturing video.

	Calls MCameraObserver::FrameBufferReady() when each buffer has been filled
	with the required number of frames, as set by PrepareVideoCaptureL(). 
	*/
	virtual void StartVideoCapture()=0;

	/** 
	Stops video capture. 
	*/
	virtual void StopVideoCapture()=0;

	/** 
	Tests whether video capture is active.

	@return  ETrue if video capture is active. EFalse if video capture is not active 
	*/
	virtual TBool VideoCaptureActive() const=0;

	/** 
	Enumerates through the available video frame sizes, based on the specified
	size index and format.

	@param  aSize 
	        On return the available video frame sizes. Sizes should be returned
	        in order, largest first, so clients do not have to iterate through every one.
	@param  aSizeIndex 
	        Size index. Must be in the range 0 to TCameraInfo::iNumVideoFrameSizesSupported-1
	        inclusive
	@param  aFormat 
	        Image format. 
	*/
	virtual void EnumerateVideoFrameSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const=0;

	/** 
	Enumerates through the available video frame rates, based on the specified
	rate index, video frame format, size index and exposure mode.

	@param  aRate 
	        On return, the available video frame rates. Some rates may not
	        be available due to, for example, current flash mode setting. In those cases
	        a rate of 0 will be returned. Rates should be returned in order, highest first,
	        so clients do not have to iterate through every one.
	@param  aRateIndex 
	        The rate index. Must be in the range 0 to TCameraInfo::iNumVideoFrameRatesSupported-1
	        inclusive.
	@param  aFormat 
	        The format.
	@param  aSizeIndex 
	        The size index.
	@param  aExposure 
	        The exposure mode. 
	*/
	virtual void EnumerateVideoFrameRates(TReal32& aRate,TInt aRateIndex,TFormat aFormat,TInt aSizeIndex,TExposure aExposure = EExposureAuto) const=0;

	/** 
	Gets the frame size currently in use.

	@param  aSize 
	        The frame size currently in use. 
	*/
	virtual void GetFrameSize(TSize& aSize) const=0;

	/** 
	Gets the frame rate currently in use.

	@return  The frame rate currently in use. 
	*/
	virtual TReal32 FrameRate() const=0;

	/** 
	Gets the number of buffers currently in use.

	@return  The number of buffers currently in use. 
	*/
	virtual TInt BuffersInUse() const=0;

	/** 
	Gets the number of frames per buffer currently in use.

	@return  The number of frames per buffer currently in use. 
	*/
	virtual TInt FramesPerBuffer() const=0;

	/** 
	Sets the quality value to use if jpeg is a supported image for video format.

	Ignored if jpeg is not a supported image for video format.

	@param  aQuality
	        The quality value to use, clamped to the range 1 to 100.
	        
	@see	CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters::iImageMaxMemorySize
	*/
	virtual void SetJpegQuality(TInt aQuality)=0;

	/**
	Gets the currently set jpeg quality value.

	Returns 0 if not supported.

	@return The currently set jpeg quality value.
	
	@see    CCamera::CCameraPreImageCaptureControl::GetImageMaxMemorySizeL(TUint& aMemorySize)
	*/
	virtual TInt JpegQuality() const=0;

    /**
	Gets a custom interface. The client has to cast the returned pointer
	to the appropriate type.

	@param aInterface
		   The Uid of the particular interface function required.

	@return Custom interface pointer. NULL if the requested interface is not supported.
	*/
	virtual TAny* CustomInterface(TUid aInterface)=0;
	
	/**
	@internalComponent
	Returns the camera API version no.
	*/
	IMPORT_C TInt CameraVersion();
	
private:

	/**
	Default Constructor
	
	Prevents implementers from deriving from CCamera.
	Implementation requires derivation from CCameraPlugin instead.
	*/
	IMPORT_C CCamera();
	};

/**
Buffer class for passing video frames between camera and client.

May contain multiple frames.

@publishedAll
@released
*/
class MFrameBuffer
	{
public:
	/**
	Gets a non-bitmap frame in the buffer.

	@param  aIndex
	        The index of the required, non-bitmap, frame.

	@leave  KErrArgument if aIndex is out of range 
	@leave  KErrNotSupported if the frame format is bitmap.
	@return A pointer to the specified non-bitmap format frame of video data.
	*/
	virtual TDesC8* DataL(TInt aIndex)=0;

	/** 
	Gets a bitmap frame in the buffer.

	@param  aIndex 
	        The index of the required, bitmap format, frame.

	@leave  KErrArgument if aIndex is out of range and 
	@leave  KErrNotSupported if
	        the frame format is not a bitmap.
	@return A pointer to the specified bitmap format frame of video data. 
	*/
	virtual CFbsBitmap* FrameL(TInt aIndex)=0;

	/** 
	Releases the buffer for re-use by the camera once the client has processed
	the frame data.

	Signals to CCamera that the buffer data has been used and that the buffer
	is free for re-use. 
	*/
	virtual void Release()=0;
	
public:

	/** 
	Sequential frame number of the first frame in the buffer, counting from when
	CCamera::StartVideoCapture() was called and including frames dropped due to
	lack of buffers. 
	*/
	TInt iIndexOfFirstFrameInBuffer;

	/** 
	Time elapsed from when CCamera::StartVideoCapture() was called until the first
	frame in the buffer was captured. 
	*/
	TTimeIntervalMicroSeconds iElapsedTime;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamconst.h>
#endif

class MReserveObserver;


/**
An active object class implemented by Symbian and used to subscribe for the updates in the Properties, retrieve the 
Properties and forward the notification to the client.

@note Serialized part of the ECam implementation is assumed to define the Properties and publish them whenever there is 
	  change in the reserve status.
*/
class CCameraStatusWatch : public CActive
	{
public:
	IMPORT_C ~CCameraStatusWatch();
	static CCameraStatusWatch* NewL(MReserveObserver& aReserveObserver, TInt aCameraIndex, TInt aSecureId);
private:
	
	CCameraStatusWatch(MReserveObserver& aReserveObserver, TInt aCameraIndex, TInt aSecureId);
	void ConstructL();
	void RunL();
	void DoCancel();
	
private:
	RProperty iProperty;
	MReserveObserver& iReserveObserver;
	TInt iCameraIndex;
	TInt iSecureId;
	};

	
#endif // ECAM_H
