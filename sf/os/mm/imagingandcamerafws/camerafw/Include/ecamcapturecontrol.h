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
#ifndef  ECAMCAPTURECONTROL_H
#define  ECAMCAPTURECONTROL_H

#include <e32base.h>
#include <ecam.h>
#include <ecamadvsettings.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamconst.h>
#include <ecamadvsettingsconst.h>
#endif

#include <mm/conversioncoefficient.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamcapturecontrolconst.h>
#endif

class MCameraPreImageCaptureControl;
class MCameraImageCapture;
class MCameraPostImageCaptureControl;
class MCameraVideoCaptureControl;

class MHistogramV2Buffer;

/** 
Special ImageMaxMemorySize when client does not have any specific preference. Also used when 
this setting of maximum memory size is not supported. 
*/
static const TInt KECamNoSpecificMaxMemorySize = -1;

/** 
The current Version of the TPrepareImageParameters class.
*/
static const TUint KECamPrepareImageParametersCurrentVersion = 1;

/** 
The current Version of the TPrepareVideoParameters class.
*/
static const TUint KECamPrepareVideoParametersCurrentVersion = 1;

/**
The current Version of the TDriveModeDependentAttributes class. 

@publishedPartner
@prototype
*/
static const TUint KECamDriveModeDependentAttributesCurrentVersion = 1;

/**
	Uid used to identify the CCameraPreImageCaptureControl API.
	This API is used to control the pre-imagecapture operations.
	
	@see CCamera::CCameraPreImageCaptureControl
*/
static const TUid  KECamPreImageCaptureControlUid 	= {KECamPreImageCaptureControlUidValue};

/**
	Uid used to identify the CCameraImageCapture API.
	This API is used to capture the image and perform other control operations after capture the image.
	
	@see CCamera::CCameraImageCapture
*/
static const TUid  KECamImageCaptureUid 	= {KECamImageCaptureUidValue};

/**
	Uid used to identify the CCameraPostImageCaptureControl API.
	This API is used to control the post-imagecapture operations.
	
	@see CCamera::CCameraPostImageCaptureControl
*/
static const TUid  KECamPostImageCaptureControlUid 	= {KECamPostImageCaptureControlUidValue};

/**
	Uid used to identify the CCamera Video Capture Control API.
	This API is used to control the video capture operations.
	
	@see CCamera::CCameraVideoCaptureControl
*/
static const TUid  KECamVideoCaptureControlUid 	= {KECamVideoCaptureControlUidValue};

/** 
Notification that the camera is ready for next prepare. Next prepare can either be still or video.
@note  If drive mode is EDriveModeTimeNudgeCapture this callback will only be received when the client deletes the 
	   existing CCameraImageCapture object as more than one instance can not be created if this drive mode is used.
*/
static const TUid  KUidECamEventReadyForNextPrepare = {KUidECamEventReadyForNextPrepareUidValue};

/** 
Notification that the camera is ready for next capture. Next capture can either be still or video.
@note  If drive mode is EDriveModeTimeNudgeCapture this callback will only be received once the total required amount
	   of images are captured (ie. pre-capture images + 1 + post-capture images) and the implementation is ready with
	   another set of pre-capture images (so implementation is prepared for next CaptureImage() call).
*/
static const TUid  KUidECamEventReadyForNextCapture = {KUidECamEventReadyForNextCaptureUidValue};

/** 
Notification that the image has been exposed to the sensor.
ECam implementation will notify the client that the image has been exposed to the sensor.
@see CCamera::CCameraPreImageCaptureControl::GetCaptureEventSupportInfoL
*/
static const TUid  KUidECamEventImageCaptureEvent = {KUidECamEventImageCaptureEventUidValue};

/**
Specifies the priority which may be assigned to the captured images which are still pending.

The enumeration list may be extended in future.
*/
enum TECamImagePriority
	{
	/** Low Priority */
	EECamImagePriorityLow,
	/** Medium Priority */
	EECamImagePriorityMedium,
	/** Standard Priority */
	EECamImagePriorityStandard,
	/** High Priority */
	EECamImagePriorityHigh,
	/** Very High Priority */
	EECamImagePriorityVeryHigh
	};

/**	
A mixin class to be implemented by the client in order to use the PreImageCaptureControl API. The derived class methods 
are called by the implementation when the pre image capture operations are ready to be notified accordingly.

@see CCamera::CCameraPreImageCaptureControl
*/	
class MPreImageCaptureControlObserver
	{
public:
	/**
	Implementation sends this callback as a result of PrepareImageCapture completion. Every time client calls 
	CCameraPreImageCaptureControl::PrepareImageCapture, a new CCameraImageCapture* will be passed to the client for
	image capture operations. Implementation will create the CCameraImageCapture* object after allocating the memory 
	resources required. Ownership of CCameraImageCapture* object will be passed to the client.
	
	@param aCaptureImageHandle
		   Retrieves pointer to the CCameraImageCapture object created by the implementation.
		   
	@param aErrorCode
		   Appropriate error code. 
		   KErrECamImageResourceNotReleased when camera device is capable of preparing only still or video.
		   KErrECamVideoResourceNotReleased when camera device is capable of preparing only still or video.
	
	@note  Every time client needs to change the prepare image settings, a new instance of CCameraImageCapture will be 
		   provided to it by the implementation.
		   
	@note  If error is KErrECamImageResourceNotReleased and client wants to successfully call the Prepare method again, 
		   client needs to delete all CCameraImageCapture objects and any Snapshot and Histogram objects associated with
		   it as well.   
	
	@note  If error is KErrECamVideoResourceNotReleased and client wants to successfully call the Prepare method again,
		   client needs to call ReleaseVideoResource to unprepare video and then delete any Snapshot and Histogram 
		   objects associated with it as well.
	
	@note  If drive mode is set to EDriveModeTimeNudgeCapture the client will only receive this callback once the implementation
		   is ready with the number of pre-capture images specified in TDriveModeDependentAttributes.
	*/
	virtual void PrepareImageComplete(CCamera::CCameraImageCapture* aCaptureImageHandle, TInt aErrorCode)=0;
	
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
A mixin class to be implemented by the client in order to use the Capture Image API. The derived class methods are 
called by the implementation when the image capture operations are ready to be notified accordingly.
Implementation shall keep track of CCameraImageCapture object deletion. It has to ensure that a callback is not send 
should the client destroy the CCameraImageCapture class object when the callback is queued to be send across. 
Ownership of CCameraImageCapture is retained by the client.

@see CCamera::CCameraImageCapture
@see CCamera::CCameraPostImageCaptureControl
*/
class MCaptureImageObserver
	{
public:
	/**
	Implementation sends this callback to provide client the handle to control individual images to be captured. For
	example, client may want to destroy the image even before the completion for some reasons.
	This callback may be send to the client after the image is exposed to the sensor.
	
	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
			CCameraPostImageCaptureControl instances will be owned by CCameraImageCapture and would be available until 
			new capture command is issued using the owning CCameraImageCapture instance. If client wishes to initiate new 
			capture but preserve the CCameraPostImageCaptureControl instances from previous capture, it would have to 
			create a new CCameraImageCapture instance and use that for the new capture.
			
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given 
			CCameraImageCapture class object. This will be used to control the individual image.
	
	@note   If, in case, there is some problem at implementation level while the image capture operation, for example, not
			enough memory to create the class object CCameraPostImageCaptureControl, this callback may not be send. In such
			cases, ImageCaptureComplete callback can be send with appropriate error code.
			
	@note   Whether direct saving option is used or buffers are used for images, this callback will be received in both 
			the cases.
	*/
	virtual void IndividualImageControlHandle(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId)=0;
																									  
	/**
	Implementation sends this callback when the individual image is ready and direct saving option is not used.
	
	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
			CCameraPostImageCaptureControl instances will be owned by CCameraImageCapture and would be available until 
			new capture command is issued using the owning CCameraImageCapture instance. If client wishes to initiate new 
			capture but preserve the CCameraPostImageCaptureControl instances from previous capture, it would have to 
			create a new CCameraImageCapture instance and use that for the new capture.
	
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given 
			CCameraImageCapture class object. This will be used to retrieve the individual image buffer.
	
	@param  aErrorCode
			Appropriate error code. 
	
	@note	If direct saving option is not used, this callback will be received by the client. 
	*/
	virtual void ImageBufferReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode)=0;
	
	/** 
	Implementation sends this callback when a particular client snapshot data is available. The snapshot data represents
	the individual image which may be in any drive mode. 

	If a call to CCameraSnapshot::EnableSnapshotL() (on the CCameraImageCapture instance) is made without a successful call 
	to CCameraSnapshot::PrepareSnapshotL(const TSnapshotParameters& aSnapshotParameters) (on the CCameraImageCapture 
	instance), then the callback returns KErrBadHandle.

	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
	
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given 
			CCameraImageCapture class object. This is needed to identify the image which is represented by this snapshot.
	
	@param  aSnapshotBuffer
			Pointer to MCameraBuffer2 which retrieves the snapshot data for the individual image. The ownership will be 
			retained by the implementation. Client needs to call Release in order to indicate the implementation
			that the buffer can be re-used. Client shall never try to delete the pointer. NULL, if error.
	
	@param  aErrorCode
			Appropriate error code.
	*/
	virtual void ClientSnapshotForImageReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MCameraBuffer2* aSnapshotBuffer, TInt aErrorCode)=0;
	
	/** 
	Implementation sends this callback when the cut down version(lower resolution) of the individual image has
	been directly saved to the file.
	Client may use the cut down version of the actual image to view the image beforehand and may cancel the actual
	individual image.
	
	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
	
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given 
			CCameraImageCapture class object. This may be used to cancel the actual image which might be currently 
			undergoing any processing options or even pause/resume ongoing processing options.
	
	@param  aErrorCode
			Appropriate error code.
	
	@note	If direct saving option is being used, this callback may be received by the client.
	*/
	virtual void CutDownImageDirectSavingCompleted(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode)=0;
	
	/** 
	Implementation sends this callback when the individual image has been directly saved to the file.
	This implies that the processing options associated with the image has been finally completed.

	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
			CCameraPostImageCaptureControl instances will be owned by CCameraImageCapture and would be available until 
			new capture command is issued using the owning CCameraImageCapture instance. If client wishes to initiate new 
			capture but preserve the CCameraPostImageCaptureControl instances from previous capture, it would have to 
			create a new CCameraImageCapture instance and use that for the new capture.
	
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given 
			CCameraImageCapture class object.
	
	@param  aErrorCode
			Appropriate error code.
	
	@note	If direct saving option is being used, this callback will be received by the client.
	*/
	virtual void ImageDirectSavingCompleted(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode)=0;

	/**
	Implementation sends this callback when the Capture Image operation has been completed. This will be send to mark the 
	completion of the image capture even if direct saving option is being used.
	
	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
			CCameraPostImageCaptureControl instances will be owned by CCameraImageCapture and would be available until 
			new capture command is issued using the owning CCameraImageCapture instance. If client wishes to initiate new 
			capture but preserve the CCameraPostImageCaptureControl instances from previous capture, it would have to 
			create a new CCameraImageCapture instance and use that for the new capture. 
			
	@param  aErrorCode
			Appropriate error code. 
			
	@note	This callback marks the completion of image capture operation. So, whether direct saving option is used or 
			buffers are used for images, this callback will be received in both the cases.
			
	@note  If drive mode is EDriveModeTimeNudgeCapture this callback will only be received once the total required amount
		   of images are captured (ie. pre-capture images + 1 + post-capture images) and the implementation is ready with
		   another set of pre-capture images (so implementation is prepared for next CaptureImage() call).
	*/
	virtual void ImageCaptureComplete(CCamera::CCameraImageCapture& aCaptureImageHandle, TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about availability of the histogram data for the
	individual image. If a call to CCameraV2Histogram::StartHistogram() (on the CCameraImageCapture instance) is made 
	without	a previous successful call to CCameraV2Histogram::PrepareClientHistogramL() (on the CCameraImageCapture 
	instance) then the callback returns KErrBadHandle.
	
	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
	
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given 
			CCameraImageCapture class object. This provides the individual image for which the client histogram data 
			is available.
	
	@param  aClientHistogramBuffer
			Pointer to MHistogramV2Buffer which retrieves a single histogram for the individual image alongwith 
			relevant information about it. The ownership will be retained by the implementation. Client needs to call 
			Release in order to indicate the implementation that the buffer can be re-used. Client shall never try to 
			delete the pointer. NULL, if error.
	
	@param  aErrorCode
			Appropriate error code.
	*/
	virtual void ClientHistogramForImageReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about availability of the histogram data for the
	snapshot of the individual image. If a call to CCameraV2Histogram::StartHistogram() (on the snapshot) is made without
	a previous successful call to CCameraV2Histogram::PrepareClientHistogramL() (on the snapshot) then the callback 
	returns KErrBadHandle.
	
	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation. This 
			provides handle to the snapshot(CCameraImageCapture::GetSnapshotHandleL()) for which the histogram data 
			is available.
	
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given 
			CCameraImageCapture class object. This provides a mapping between the individual image and the snapshot for 
			which the client histogram data is available.
	
	@param  aClientHistogramBuffer
			Pointer to MHistogramV2Buffer which retrieves a single histogram for the snapshot of the individual image 
			alongwith relevant information about it. The ownership will be retained by the implementation. Client needs to call 
			Release in order to indicate the implementation that the buffer can be re-used. Client shall never try to 
			delete the pointer. NULL, if error.
	
	@param  aErrorCode
			Appropriate error code. 
	*/
	virtual void ClientHistogramForSnapshotReady(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode)=0;

	/**
	Implementation sends this callback in order to notify the client about the failure of processing options for the 
	individual image.
	
	@param  aCaptureImageHandle
			Reference to CCameraImageCapture class object which was used to issue the capture image operation.
	
	@param  aPostCaptureControlId
			Id used to identify a particular CCameraPostImageCaptureControl object associated with the given
			CCameraImageCapture class object. This provides the individual image for which the processing options
			has failed.

	@param  aProcessingTypes
			Bitfield of TEcamProcessingOptions associated with the image that have failed.
	
	@param  aErrorCode
			Appropriate error code.
	*/
	virtual void ProcessingFailed(CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TUint aProcessingTypes, TInt aErrorCode) = 0;
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
CCameraPreImageCaptureControl class exposes an API for controlling image operations/settings prior to image capture. 

@note  This class is not intended for sub-classing and used to standardise existing
       varieties of implementations.
       
@note  If the class methods leave, the output type parameter value is not guaranteed to be valid.

@note  Client should not use old Capture methods present in class CCamera. Class CCameraImageCapture and class 
	   CCameraPostImageCaptureControl should be used rather.

@publishedPartner
@prototype
*/
class CCamera::CCameraPreImageCaptureControl : public CBase
	{
	/* so can use internal factory functions etc. */
	friend class CCamera;

public:

	/**
	Specifies the type of direct saving.
	The enumeration list may be extended in future.
	*/
	enum TDirectSavingType
		{
		/** Image saved in buffers. Direct saving to file not being used. Callback used is 
		MCaptureImageObserver::ImageBufferReady. */
		EDirectSavingNotUsed					= 0x00,
		/** Image saved to file as per the format specified. Image saved after any processing options have completed. 
		Callback used is MCaptureImageObserver::ImageDirectSavingCompleted(). */
		EDirectSavingHighResolutionFileOnly		= 0x01,
		/** Image saved to file as per the format specified. Also, a lower resolution image gets saved in a separately 
		specified file. This helps in retrieving the lower resolution image sooner than the higher resolution
		image gets saved. Callback used for lower resolution image saving is MCaptureImageObserver::
		CutDownImageDirectSavingCompleted(). */
		EDirectSavingWithLowerResolutionFile	= 0x02
		};
	
	/**
	Specifies the various type of direct snapshot support.
	The enumeration list may be extended in future.
	*/	
	enum TDirectSnapshotType
		{
		/** Direct snapshot not supported. */
		EDirectSnapshotNotSupported			= 0x00,
		/** Direct Snapshot supported. It will be displayed on the same direct viewfinder screen, out of which it has 
		been created */
		EDirectSnapshotSupported			= 0x01
		};

	/** Provides any extra drive mode dependent parameters to be used for image capture. */ 
	class TDriveModeDependentAttributes
		{
	public:
		IMPORT_C TDriveModeDependentAttributes();

		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;

	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;

		//for future use
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;

	public:
		/** If drive mode is EDriveModeTimeNudgeCapture this represents the number of images to be captured
		(pre-capture images) before client initiates actual image capture operation. */
		TInt iParam1;
		/** If drive mode is EDriveModeTimeNudgeCapture this represents the number of images to be captured
		(post-capture images) after client initiates actual image capture operation. */
		TInt iParam2;
		};

	/**
	Specifies the available processing options.
	The enumeration list may be extended in future.
	
	@publishedPartner
	@prototype
	*/	
	enum TEcamProcessingOptions
		{
		/** Processing options are not supported.*/
		EEcamProcessingNone 		= 0x00000000,
		/** Enable normal processing when image capture is initiated.*/
		EEcamNormalProcessing 		= 0x00000001,
		/** Enable background processing when image capture is initiated.*/
		EEcamBackgroundProcessing 	= 0x00000002
		};
		
	/** Provides the parameters necessary to perform set-up and allocation of memory for the images to be captured.	*/
	class TPrepareImageParameters
		{
	public:
		IMPORT_C TPrepareImageParameters();
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;
		
		IMPORT_C void SetImageProcessingOptions(TUint aImageProcessingOptions);
		IMPORT_C void GetImageProcessingOptions(TUint& aImageProcessingOptions) const;
				
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
		
		//for future use
		TInt iReserved1;
		TInt iReserved2;
		/** Bitfield of selected TEcamProcessingOptions which should be used after the images are captured from the sensor. */
		TUint iImageProcessingOptions;
		
	public:
		/** The image format. */
		CCamera::TFormat iImageFormat;
		/** The image size. */
		TSize iImageSize;
		/** Pixel aspect ratio to be used for the image. */
		CCamera::CCameraAdvancedSettings::TPixelAspectRatio iPixelAspectRatio;
		/** The drive mode in which the images will be captured. */
		CCamera::CCameraAdvancedSettings::TDriveMode iDriveMode;
		/** The number of images to be captured if the drive mode is burst. */
		TInt iBurstImages;
		/** The maximum memory size in kilo bytes when encoding to the image format. This shall be 
		KECamNoSpecificMaxMemorySize if client has no specific preference or if this setting is not supported.
		In case of JPEG, the maximum memory size will take preference over JPEG quality if the maximum memory size is 
		not sufficient to achieve the desired quality. Refer CCamera::JpegQuality(). */ 
		TInt iImageMaxMemorySize;
		/** Identifies the rectangle to which the image is to be clipped.
		If no specifc clipping rectangle desired by the client, the clipping region can be given by 
		origin as the top-left corner and size as iImageSize. */
		TRect iClipRect;
		/** The attributes dependent on the type of drive mode used. */
		TDriveModeDependentAttributes iDriveModeAttributes;
		};
	
private:
	/**
	Different types of color space. Camera will print or capture images in given color space.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/	
	enum TColorSpace
		{
		/** not known */
		EColorSpaceUnknown 				= 0x0000,
		/** sRGB color space */
		EColorSpacesRGB					= 0x0001,
		/** adobeRGB color space */
		EColorSpaceAdobeRGB				= 0x0002,
		/** adobeWideGamutRGB color space */
		EColorSpaceAdobeWideGamutRGB	= 0x0004,
		/** CMY(K) color space */
		EColorSpaceCMYK					= 0x0008,
		/** YIQ color space*/
		EColorSpaceYIQ					= 0x0010,
		/** YUV color space */
		EColorSpaceYUV					= 0x0020,
		/** YDbDr color space */
		EColorSpaceYDbDr				= 0x0040,
		/** YCbCr color space */
		EColorSpaceYCbCr				= 0x0080,
		/** HSB color space */
		EColorSpaceHSB					= 0x0100,
		/** HSL color space */
		EColorSpaceHSL					= 0x0200
		};
		
	/** 
	Specifies direction of panning. 
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TPanoDirection
		{
		/** Viewfinder displays towards the right of already captured images */
		EPanoRight,
		/** Viewfinder displays towards the left of already captured images */
		EPanoLeft,
		/** Viewfinder displays towards the top of already captured images */
		EPanoUp,
		/** Viewfinder displays towards the bottom of already captured images */
		EPanoDown  
		};
	
	/** 
	Specifies the stitching options when panning is started.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TStitchingOption
		{
		/** Stitching of panned images is not supported by the ECAM Implementation.*/
		EStitchingOptionNone			= 0x00,
		/** Stitching of panned images can be done by the ECAM Implementation.*/
		EStitchingOptionEnable			= 0x01,
		/** Stitching of panned images can be disabled/discarded by the ECAM Implementation.*/
		EStitchingOptionDisable			= 0x02
		};
		
	/** 
	Specifies whether the panned images captured under 'stitching enabled option' would be discarded by the implementation.
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TStitchedImageRetrieval
		{
		/** allow ecam implementation to provide the stitched images. */
		EStitchedImageRetrieve,
		/** instruct ECAM Implementation to discard the stitched images. */
		EStitchedImageDiscard
		};
	
public:
	/** 
	Provides information regarding streamed image buffers or sub-frames. 
	@publishedPartner
	*/
	class TImageBufferInfo
		{
	public:
		IMPORT_C TImageBufferInfo(); 
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;

		IMPORT_C TBool IsSubFrameUsed() const ;
		
		IMPORT_C void SetSubFrameState(TBool aIsSubFrameUsed);
		
		IMPORT_C TBool IsParallelStreamedBufferUsed() const;
		
		IMPORT_C void SetParallelStreamedBufferState(TBool aIsParallelStreamedBufferUsed);
		
		IMPORT_C TUint SubFrameSequenceNumber() const;
		
		IMPORT_C TInt SetSubFrameSequenceNumber(TUint aSubFrameSequenceNumber);
		
		IMPORT_C TUint TotalSubFrames() const;
		
		IMPORT_C TInt SetTotalSubFrames(TUint aTotalSubFrames);
				
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8; 
		
		//for future use
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;
		TInt iReserved4;
		TInt iReserved5;
		TInt iReserved6;
		TUint iReservedBits:11;
				
		/** Indicates whether the streamed image scheme or sub-frame scheme is being used. 
		One bit is enough to provide such information */
		TUint iIsSubFrameUsed:1;
		/** Indicates whether the parallel buffering is being used by the implementation in order to speed up the streamed
		image operation, as a whole. One bit is enough to provide such information. 
		Parallel buffering indicates that implementation is using more than one buffer to handle the various sub-frames;
		hence speeding up the operation.*/
		TUint iIsParallelBufferUsed:1;
		/** Sequence number of the sub-frame. 
		9 bits used for sequence no. assuming that KECamMaxTotalSubFrames sub-frames would be used at max */
		TUint iSubFrameSequenceNumber:9;
		/** Total number of sub-frames to be retrieved by the client in order to properly reconstruct the actual image.
		It does not give the number of outstanding sub-frames needed to reconstruct the image. 
		This value will be same for every sub-frames needed to re-construct the actual image. 
		Maximum no. of total sub-frames is KECamMaxTotalSubFrames.*/ 
		TUint iTotalSubFrames:10;
		
	public:
		/** The exact position of the sub-frame within the actual image frame. The actual image frame's bounding rectangle
		is defined by origin as top-left and TSize parameter used to prepare the image capture. */
		TRect iSubFramePosition;
		
		/** The parameters used for this image */
		TPrepareImageParameters iImageParameters;
		};
	
public:
	
    IMPORT_C static CCameraPreImageCaptureControl* NewL(CCamera& aCamera, MPreImageCaptureControlObserver& aPreImageCaptureControlObserver);
    
   	IMPORT_C void GetDirectSnapshotSupportInfoL(TUint& aIsDirectSnapshotSupported) const;
	
	IMPORT_C void GetSupportedEmbeddedStillCaptureSettingsL(RArray<TUid>& aSupportedEmbeddedStillCaptureSettings) const;
	
	IMPORT_C void GetSupportedDirectSavingTypeL(TDirectSavingType& aSupportedDirectSavingType) const;
	
	IMPORT_C void SetSequentialImageFilenameL(const TDesC8& aFilename, TInt aStartingSequenceNumber);
	
	IMPORT_C void SetLowerResolutionSequentialImageFilenameL(const TDesC8& aLowerResolutionFilename, TInt aStartingSequenceNumber);

    IMPORT_C void GetDirectSavingTypeL(TDirectSavingType& aDirectSavingType) const;
    
    IMPORT_C void SetDirectSavingTypeL(TDirectSavingType aDirectSavingType);
    
    IMPORT_C void GetCaptureEventSupportInfoL(TUint& aSupportedDriveModes) const;
	
	IMPORT_C void GetImageFormatsSupportedL(TUint& aImageFormatsSupported, const TSize& aSize) const;
	
	IMPORT_C void GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat aImageFormat, const TSize& aSize) const;
	
	IMPORT_C void PrepareImageCapture(const TPrepareImageParameters& aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver);
	
	IMPORT_C void GetImageMaxMemorySizeSettingSupportInfoL(TBool& aIsImageMaxMemorySizeSettingSupported) const;
	
	IMPORT_C void GetImageMaxMemorySizeL(TUint& aMemorySize) const;
	
    IMPORT_C ~CCameraPreImageCaptureControl();
	
    IMPORT_C void GetSupportedProcessingOptionsL(TUint& aECamProcessingOptionsSupported) const;
	
private:
	CCameraPreImageCaptureControl(CCamera& aOwner);
	void ConstructL(MPreImageCaptureControlObserver& aPreImageCaptureControlObserver);
	
private:
	void GetMaximumSpotsL(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode, TInt& aMaximumSpots) const;
	void GetSupportedSpotsCombinationL(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode, RArray<TUint>& aPossibleSpotCombinations) const;
	void GetSpotsCombinationL(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode, TUint& aSpotsCombination) const;
	void SetSpotsCombination(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode, TUint aSpotsCombination);

	void GetStreamedImageSupportInfoL(TBool& aIsStreamedImageSupported) const;
    void EnableSubFramesL();
    void DisableSubFramesL();
	void GetSubFramesStateL(TBool& aIsSubFrameEnabled) const;
	void GetStreamedImageSupportedTransformationsL(RArray<TUid>& aStreamedImageSupportedTransformations) const;
	
	void GetPanoModeSupportInfoL(TBool& aIsPanoModeSupported, TInt& aSupportedStitchingOption) const;
	void StartPanoMode(TStitchingOption aStitchingOption);
	void StartPanoMode(TStitchingOption aStitchingOption, const RArray<TInt>& aVFHandle);
	void GetPanoDirectionL(TPanoDirection& aPanoDirection) const;
	void SetPanoDirection(TPanoDirection aPanoDirection);
	void StopPanoModeL(TStitchedImageRetrieval aStitchedImageRetrieval);
	
	void GetSupportedColorSpaceL(TUint& aSupportedColorSpace) const;
	void GetColorSpaceL(TColorSpace& aColorSpace) const;
	void SetColorSpace(TColorSpace aColorSpace);
	
	void StartEmbeddedStillCaptureSettingsL();
	void EndEmbeddedStillCaptureSettingsL();
	void GetFailedEmbeddedStillCaptureSettingsL(RArray<TUid>& aFailedEmbeddedStillCaptureSettings) const;
	
private:
	CCamera& iOwner;
	MCameraPreImageCaptureControl* iImpl;  // not owned
	};
	
/** 
CCameraImageCapture class exposes an API for capturing the image and controlling the overall capture. This class gets 
created by the implementation of MCameraPreImageCaptureControl::PrepareImageCapture and passed to the client through 
callback MPreImageCaptureControlObserver::PrepareImageComplete.
Destruction of this class is equivalent to releasing the resources owned in order to prepare and allocate memory for 
capturing images.

@note  This class is not intended for sub-classing and used to standardise existing
       varieties of implementations.
       
@note  If the class methods leave, the output type parameter value is not guaranteed to be valid.

@note  The use of this class implies that client will be able to issue image capture call even if previous
	   captures are still outstanding. 
	   
@note  Clients must implement an MCaptureImageObserver in order to use this CCameraImageCapture API. 
	   MCameraObserver2::ImageBufferReady will not be used with this class.

@publishedPartner
@prototype
*/
class CCamera::CCameraImageCapture : public CBase
	{
	/* so can use internal factory functions etc. */
	friend class CCamera;
	
	/* so can use the MCameraImageCapture* as well. */
	friend class CCameraPostImageCaptureControl;
	
public:
	IMPORT_C static CCameraImageCapture* CreateL(CCamera& aCamera, const CCamera::CCameraPreImageCaptureControl::
				        TPrepareImageParameters& aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver);

	IMPORT_C CCamera::CCameraV2Histogram* CreateHistogramHandleL() const;
	
	IMPORT_C CCamera::CCameraSnapshot* GetSnapshotHandleL(TInt aClientViewFinderId) const;
	
	IMPORT_C CCamera::CCameraImageProcessing* GetTransformationHandleL() const;
	
	IMPORT_C void GetPrepareImageParametersL(CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters) const;

	IMPORT_C void CaptureImage();
	
	IMPORT_C void CancelCaptureImage();
	
	IMPORT_C void GetNumImagesExposedL(TUint& aNumImagesExposed) const;
	
	IMPORT_C void GetNumTotalImagesL(TUint& aNumTotalImages) const;
	
	IMPORT_C void GetPostCaptureControlHandleL(CCamera::CCameraPostImageCaptureControl*& aPostCaptureControlHandle, TPostCaptureControlId aPostCaptureControlId) const;

	IMPORT_C void SetCaptureImagePriorityL(TECamImagePriority aCaptureImagePriority);
	
	IMPORT_C void GetCaptureImagePriorityL(TECamImagePriority& aCaptureImagePriority) const;
	
	IMPORT_C void PauseProcessing(TUint aProcessingTypes);
	
	IMPORT_C void ResumeProcessingL(TUint aProcessingTypes);
	
	IMPORT_C ~CCameraImageCapture();

private:
	CCameraImageCapture(CCamera& aOwner);
	void ConstructL(const CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters, 
												MCaptureImageObserver& aCaptureImageObserver);
	
	MCameraImageCapture* Impl() const;
	
private:
	CCamera& iOwner;
	MCameraImageCapture* iImpl;  // not owned
	};
	
/** This class is used to provide further information about still image data.

@see MCameraBuffer
@see MCameraBuffer2

@publishedPartner
@prototype
*/
class MCameraImageBuffer : public MCameraBuffer2
	{
public:
	/**
	Retrieves information regarding the image data received.
	
	@param aImageBufferInfo
		   Retrieves the info regarding the image data received.
	
	@return KErrArgument if implementation finds a different extended version of the TImageBufferInfo class.

	@return May leave with other error codes.
		    
	@note  When there is a sub-frame, each image buffer retrieved contains only one sub-frame. If any of the subframe 
		   delivery reports an error, then no further subframes of this image would be delivered to the client.
	
	@note  Also retrieves information in order to distinguish the retrieved image buffer when shot-to-shot reduced latency 
		   scheme is used.
	*/
	virtual TInt GetImageBufferInfo(CCamera::CCameraPreImageCaptureControl::TImageBufferInfo& aImageBufferInfo) const=0;
	};
	
/** 
CCameraPostImageCaptureControl class exposes an API for retrieving the image data from individual images (in case 
continuous drive mode is used for capturing the images) and also to apply control on the captured images individually.

CCameraPostImageCaptureControl instances will be owned by CCameraImageCapture and would be available until new capture 
command is issued using the owning CCameraImageCapture instance. If client wishes to initiate new capture but preserve 
the CCameraPostImageCaptureControl instances from previous capture, it would have to create a new CCameraImageCapture 
instance and use that for the new capture.

@note  This class is not intended for sub-classing and used to standardise existing
       varieties of implementations.
       
@note  If the class methods leave, the output type parameter value is not guaranteed to be valid.

@note  The use of this class implies that client will be able to issue image capture call even if previous
	   captures are still outstanding. 
	   
@note  Clients must implement an MCaptureImageObserver in order to use this CCameraPostImageCaptureControl API. 
	   MCameraObserver2::ImageBufferReady will not be used with this class.

@publishedPartner
@prototype
*/
class CCamera::CCameraPostImageCaptureControl : public CBase
	{
public:
	/**
	Specifies the various states in which the individual images could be.
	The enumeration list may be extended in future.
	*/
	enum TImageState
		{
		/** The image has been exposed to the sensor but still pending for any processing options. */
		EProcessingPending,
		/** The image is undergoing processing for the required processing options. */
		EProcessingOngoing,
		/** The processing options associated with the image has been cancelled. */
		EProcessingCancelled,
		/** The processing options associated with the image has been completed. This implies that either the
			image buffers are ready or the image is directly saved to the file. */
		EProcessingCompleted
		};
		
	/**
	Specifies the various states in which the individual image buffers could be.
	The enumeration list may be extended in future.
	*/
	enum TBufferState
		{
		/** If Direct Saving option used, then user accessible buffers are not present. Hence buffer state will always be EBufferNotPresent.
			If Direct Saving option not used, this buffer state implies that processing options have not been completed. */
		EBufferNotPresent,
		/** This buffer state implies that processing options have been completed. Client may retrieve the image buffers 
			once this state is reached. */
		EBufferReady,
		/** This buffer state implies that client has released the image buffers after retrieving the data. */
		EBufferReleased
		};
		
public:
	IMPORT_C static CCameraPostImageCaptureControl* CreateL(CCameraImageCapture* aCameraImageCapture, TPostCaptureControlId aPostCaptureControlId);
	
	IMPORT_C void GetPostCaptureControlId(TPostCaptureControlId& aPostCaptureControlId) const;
	
	IMPORT_C CCamera::CCameraImageCapture* ImageCaptureHandle() const;
	
	IMPORT_C void GetImageSequenceNumberL(TUint& aSequenceNumber) const;
	
	IMPORT_C void CancelImage();

	IMPORT_C void SetImagePriorityL(TECamImagePriority aImagePriority);
	
	IMPORT_C void GetImagePriorityL(TECamImagePriority& aImagePriority) const;
	
	IMPORT_C void PauseProcessing(TUint aProcessingTypes);
	
	IMPORT_C void ResumeProcessingL(TUint aProcessingTypes);

	IMPORT_C void GetImageBufferL(MCameraImageBuffer& aCameraImageBuffer) const;
	
	IMPORT_C void GetImageStateL(TImageState& aImageState) const;
	
	IMPORT_C void GetBufferStateL(TBufferState& aBufferState) const;
	
	IMPORT_C ~CCameraPostImageCaptureControl();

private:
	CCameraPostImageCaptureControl(CCamera::CCameraImageCapture* aCameraImageCapture, TPostCaptureControlId aPostCaptureControlId);
	void ConstructL();
	
private:
	TPostCaptureControlId iPostCaptureControlId;
	CCamera::CCameraImageCapture* iCameraImageCapture;	// not owned
	MCameraPostImageCaptureControl* iImpl;  // not owned
	};
	
/** 
Notification of set-up completion before video capture.
@note  If error is KErrECamImageResourceNotReleased and client wants to successfully call the Prepare method again, 
	   client needs to delete all CCameraImageCapture objects and any Snapshot and Histogram objects associated with
	   it as well.   
	
@note  If error is KErrECamVideoResourceNotReleased and client wants to successfully call the Prepare method again,
	   client needs to call ReleaseVideoResource to unprepare video and then delete any Snapshot and Histogram 
	   objects associated with it as well.
*/
static const TUid KUidECamEventVideoCaptureControlPrepareComplete = {KUidECamEventVideoCaptureControlPrepareCompleteUidValue};

/**
Notifies that range of certain camera settings have been changed because of desired video settings.
Client may call GetRangeAffectedSettingsL(RArray<TUid>& aRangeAffectedSettings) const to get the list of affected camera settings.
*/
static const TUid KUidECamEventVideoCaptureControlSettingsRangeChanged = {KUidECamEventVideoCaptureControlSettingsRangeChangedUidValue};

/**
Notifies that value of certain camera settings have been changed because of desired video settings.
Client may call GetValueAffectedSettingsL(RArray<TUid>& aValueAffectedSettings) const to get the list of affected camera settings.
*/
static const TUid KUidECamEventVideoCaptureControlSettingsValueChanged = {KUidECamEventVideoCaptureControlSettingsValueChangedUidValue};

/**
Notifies that value of certain camera settings have been disabled because of desired video settings.
Client may call GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const to get the list of affected camera settings.
*/
static const TUid KUidECamEventVideoCaptureControlSettingsDisabled = {KUidECamEventVideoCaptureControlSettingsDisabledUidValue};

/**
A mixin class to be implemented by the client in order to use the video capture control API(CCameraVideoCaptureControl). 
The derived class methods are called by the implementation when the video capture operations are ready to be notified 
accordingly.

@see CCamera::CCameraVideoCaptureControl
*/
class MCaptureVideoObserver
	{
public:
	/**
	Implementation sends this callback in order to notify the client about the availability of video frames.
	
	@param  aVideoBuffer
			Pointer to MCameraBuffer2 class object which retrieves the video frames. The ownership will be retained by 
			the implementation. Client needs to call Release in order to indicate the implementation that the buffer can 
			be re-used. Client shall never try to delete the pointer. NULL, if error.
						
	@param  aErrorCode
			Appropriate error code.
	*/
	virtual void VideoBufferReady(MCameraBuffer2* aVideoBuffer, TInt aErrorCode)=0;
																									  
	/**
	Implementation sends this callback in order to notify the client about the availability of client snapshot data for the
	video being captured.
	
	If a call to CCameraSnapshot::EnableSnapshotL() (on the CCameraVideoCaptureControl instance) is made without a 
	successful call to CCameraSnapshot::PrepareSnapshotL(const TSnapshotParameters& aSnapshotParameters) (on the 
	CCameraVideoCaptureControl instance), then the callback returns KErrBadHandle.
	
	@param  aSnapshotBuffer
			Pointer to MCameraBuffer2 class object which retrieves the snapshot for the video being captured. The 
			ownership will be retained by the implementation. Client needs to call Release in order to indicate the 
			implementation that the buffer can be re-used. Client shall never try to delete the pointer. NULL, if error.
	
	@param  aErrorCode
			Appropriate error code. 
	*/
	virtual void ClientSnapshotReady(MCameraBuffer2* aSnapshotBuffer, TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about availability of the histogram data. If a call 
	to CCameraV2Histogram::StartHistogram() is made without a previous successful call to CCameraV2Histogram::
	PrepareClientHistogramL() then the callback returns KErrBadHandle.
	
	@param  aClientHistogramBuffer
			Pointer to MHistogramV2Buffer which retrieves a single histogram alongwith relevant information about it. The 
			ownership will be retained by the implementation. Client needs to call Release in order to indicate the 
			implementation that the buffer can be re-used. Client shall never try to delete the pointer. NULL, if error.
			
	@param  aErrorCode
			Appropriate error code. 
	
	@note   Client shall use the currently available histogram for the video frames until a new histogram is available.
	*/
	virtual void ClientHistogramReady(MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about the failure of image processing for the video 
	frames.
	
	@param  aErrorCode
			The reason of failure of the image processing. 
	*/
	virtual void ImageProcessingFailed(TInt aErrorCode)=0;
	
	/**
	Implementation sends this callback in order to notify the client about the failure of direct video capture operation.
	
	@param  aErrorCode
			The reason of failure of the direct video capture. The error code could be ECam specific. Client shall 
			be prepared to handle unrecognized error code.
	*/
	virtual void DirectVideoCaptureFailed(TInt aErrorCode)=0;
	
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
CCamera Video Capture Control class exposes an API for controlling operations related to video capture. This gives priority
to the low latency aspect by postponing the post processing activity involved with current video captured data(while 
stopping the video capture) in order to capture/prepare for next still image.

@note This class is not intended for sub-classing and used to standardise existing varieties of implementations.
       
@note  If the class methods leave, any reference argument is not guaranteed to be valid.

@note  Clients must implement an MCaptureVideoObserver in order to use this CCameraVideoCaptureControl API. 
	   MCameraObserver2::VideoBufferReady will not be used with this class.

@publishedPartner
@prototype
*/
class CCamera::CCameraVideoCaptureControl : public CBase
	{
	/* so can use internal factory functions etc. */
	friend class CCamera;
	
public:
	
	/** 
	Specifies video capture state. 
	The enumeration list may be extended in future.
	*/ 
	enum TVideoCaptureState
		{
		/** Video Capture has been prepared but not yet started */  
		EVideoCapturePrepared, 	 
		/** Video Capture has been started and is running */
		EVideoCaptureActive,	 
		/** Video Capture has been started and is paused */
		EVideoCapturePaused,
		/** Video Capture has been stopped or not yet prepared */
		EVideoCaptureInActive
		};
	
	/** 
	Specifies the various embedded still capture support. 
	The enumeration list may be extended in future.
	*/	
	enum TEmbeddedStillCaptureTypes
		{
		/** Embedded still capture not supported */
		EEmbeddedStillCaptureNotSupported = 0x00,
		/** Embedded still capture supported */
		EEmbeddedStillCaptureSupported	= 0x01
		};
		
	/** 
	Specifies the various video capture types. 
	The enumeration list may be extended in future.
	*/	
	enum TVideoCaptureType
		{
		/** Video capture not supported. */
		EVideoCaptureNotSupported = 0x00,
		/** Client video capture - Client video capture is a concept under which
		video buffers will be available to the clients of ECam through the 
		dedicated observer MCaptureVideoObserver::VideoBufferReady(). Video frame 
		formats used under this client video capture can be either compressed video 
		frames or uncompressed video frames which are available through CCamera::TFormat. */
		EClientVideoCapture = 0x01,
		/** Direct video capture - Direct video capture is a concept under which
		video buffers will not be available to the clients of ECam through the dedicated observers. Video frames will be used
		by the ECam adaptation and could be passed internally to other components. Video frame formats used under this direct
		video capture can be either compressed video frames or uncompressed video frames which are available through 
		CCamera::TFormat. */
		EDirectVideoCapture = 0x02
		};
		
	/** 
	Specifies the fading effect for video captured frames. 
	The enumeration list may be extended in future.
	
	@internalTechnology
	*/
	enum TFadingEffectState
		{
		/** The fading effect will be visible on video captured frames as well, 
		whenever the viewfinder fading is enabled */
		EFadingEffectEnabled,
		/** The fading effect will not be visible on video captured frames, 
		whenever the viewfinder fading is enabled */
		EFadingEffectDisabled
		};
		
	/** 
	Provides information regarding parameters needed to prepare for video capture. 
	*/
	class TPrepareVideoParameters
		{
	public:
		IMPORT_C TPrepareVideoParameters(); 
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;
		
		IMPORT_C TBool IsEmbeddedStillCaptureEnabled() const;
		IMPORT_C void  SetEmbeddedStillCaptureState(TBool aIsEmbeddedStillCaptureEnabled);
		
		IMPORT_C CCamera::CCameraVideoCaptureControl::TVideoCaptureType VideoCaptureType() const;
		IMPORT_C void SetVideoCaptureType(CCamera::CCameraVideoCaptureControl::TVideoCaptureType aVideoCaptureType);
					
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8; 
		
		//for future use
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;
	
		TUint iReservedBits:26;
		
		TUint iVideoCaptureType:3;
		
		TUint iReservedBits2:2;
				
		/** Indicates whether the embedded still capture is enabled. 
		One bit is enough to provide such information */
		TUint iIsEmbeddedStillCaptureEnabled:1;

	public:
		/** Format must be one of the video frame formats supported (see TCameraInfo::iVideoFrameFormatsSupported). */
		CCamera::TFormat iFormat;
		
		/** Pixel aspect ratio to be used for the image. */
		CCamera::CCameraAdvancedSettings::TPixelAspectRatio iPixelAspectRatio;
		
		/** Size index must be in the range 0 to TCameraInfo::iNumVideoFrameSizesSupported-1 inclusive. */
		TInt iSizeIndex;
		
		/** The rate must be in the range 0 to TCameraInfo::iNumVideoFrameRatesSupported-1 inclusive. */
		TInt iRateIndex;
		
		/** The number of discrete buffers to use. */
		TInt iBuffersToUse;
		
		/** This indirectly indicates how large the buffers are to be. Number of frames per buffer must be less than or 
		equal to TCameraInfo::iMaxFramesPerBufferSupported. One buffer is returned to MCaptureVideoObserver::
		VideoBufferReady() at a time. */
		TInt iFramesPerBuffer;
		
		/** Identifies the rectangle to which the video frame has to be clipped.
		If no specifc clipping rectangle desired by the client, the clipping region can be given by 
		origin as the top-left corner and size as given by iSizeIndex. */
		TRect iClipRect;
		};
		
public:
	
    IMPORT_C static CCameraVideoCaptureControl* NewL(CCamera& aCamera, MCaptureVideoObserver& aCaptureVideoObserver);
    
    IMPORT_C CCamera::CCameraV2Histogram* CreateHistogramHandleL() const;
    
    IMPORT_C CCamera::CCameraSnapshot* GetSnapshotHandleL(TInt aClientViewFinderId) const;
    
    IMPORT_C CCamera::CCameraImageProcessing* GetTransformationHandleL() const;
    
    IMPORT_C void GetVideoFormatsSupportedL(TUint& aVideoFormatsSupported, const TSize& aSize) const;
	
	IMPORT_C void GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat aVideoFormat, const TSize& aSize) const;
	
	IMPORT_C void GetEmbeddedStillCaptureSupportInfoL(TInt& aSupportedEmbeddedStillCaptureTypes) const;
	
    IMPORT_C void PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters);
	
	IMPORT_C void GetRangeAffectedSettingsL(RArray<TUid>& aRangeAffectedSettings) const;
	
	IMPORT_C void GetValueAffectedSettingsL(RArray<TUid>& aValueAffectedSettings) const;
	
	IMPORT_C void GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;
	
	IMPORT_C void ReleaseVideoResource();
	
	IMPORT_C void StartVideoCaptureL();												
	
	IMPORT_C void StopVideoCapture();
	
	IMPORT_C void PauseVideoCapture();
	
	IMPORT_C void ResumeVideoCaptureL();
	
	IMPORT_C void GetFadingEffectStateL(TFadingEffectState& aFadingEffectState) const;
	
	IMPORT_C void SetFadingEffectState(TFadingEffectState aFadingEffectState);
	
	IMPORT_C void GetVideoCaptureStateL(TVideoCaptureState& aVideoCaptureState) const;
    
    IMPORT_C ~CCameraVideoCaptureControl();
    
    IMPORT_C void GetVideoCaptureSupportInfoL(TInt& aSupportedVideoCaptureTypes) const;
    
    IMPORT_C void GetPrepareVideoParametersL(TPrepareVideoParameters& aPrepareVideoParameters) const;
    
private:
	CCameraVideoCaptureControl(CCamera& aOwner);
	void ConstructL(MCaptureVideoObserver& aCaptureVideoObserver);
	
private:
	void GetSupportedConversionCoefficientsL(TUint& aSupportedConversionCoefficients) const;
    void GetConversionCoefficientL(TYuvCoefficients& aConversionCoefficients) const;
    void SetConversionCoefficient(TYuvCoefficients aConversionCoefficients);
 	
private:
	CCamera& iOwner;
	MCameraVideoCaptureControl* iImpl;  // not owned
	};
	
#endif //ECAMCAPTURECONTROL_H
