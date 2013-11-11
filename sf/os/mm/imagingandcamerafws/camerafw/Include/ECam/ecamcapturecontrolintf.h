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
 @released
*/

#ifndef  ECAMCAPTURECONTROLINTF_H
#define  ECAMCAPTURECONTROLINTF_H

#include <ecamcapturecontrol.h>
#include <ecam/ecamadvsettingsintfuids.hrh>

class MImplementationFactory;

/** 
This is the UID which is used to obtain the interface MCameraPreImageCaptureControl, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface.
 
@publishedPartner
@prototype
*/   
static const TUid KECamMCameraPreImageCaptureControlUid		=  {KECamMCameraPreImageCaptureControlUidValue};

/** 
This is the UID which is used to obtain the interface MCameraImageCapture, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface.
 
@publishedPartner
@prototype
*/   
static const TUid KECamMCameraImageCaptureUid		=  {KECamMCameraImageCaptureUidValue};

/** 
This is the UID which is used to obtain the interface MCameraPostImageCaptureControl, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface.
 
@publishedPartner
@prototype 
*/   
static const TUid KECamMCameraPostImageCaptureControlUid	=  {KECamMCameraPostImageCaptureControlUidValue};

/** 
This is the UID which is used to obtain the interface MCameraVideoCaptureControl, via the 
CCamera::CustomInterface() call, which provides implementation of the M-class interface. 
@publishedPartner
@prototype
*/   
static const TUid KECamMCameraVideoCaptureControlUid 	=  {KECamMCameraVideoCaptureControlUidValue};

/** 
Mixin class for implementation by providers of the PreImageCaptureControl Camera Extension API.
PreImageCaptureControl class exposes an API for controlling advanced still image related settings and control prior
to capturing the images.

@publishedPartner
@prototype 
*/	
class MCameraPreImageCaptureControl
	{
	
public:
	
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/**
	The observer for the pre image capture control is passed to the implementation for passing callbacks on it.
	
	@param  aPreImageCaptureControlObserver
			The reference to the pre image capture control observer.
	*/
	virtual void SetPreImageCaptureControlObserver(MPreImageCaptureControlObserver& aPreImageCaptureControlObserver)=0;
	
	/**
	Retrieves information regarding the direct snapshot feature support. Direct Snapshot, if supported, can be created
	out of version2 direct viewfinder object only.

	@param  aDirectSnapshotSupportInfo
			This is a bit field providing supported direct snapshot of type TDirectSnapshotType

	@leave  May leave with any error code.
	*/
	virtual void GetDirectSnapshotSupportInfoL(TUint& aDirectSnapshotSupportInfo) const=0;
	
	/**
	Retrieves the settings supported for embedded still captures. Possibilty exists that not all the settings supported for
	normal still image captures are supported for embedded still captures as well.

	@param  aSupportedEmbeddedStillCaptureSettings
			Array of TUid which retrieves the supported embedded still captures. Empty list indicated that no settings are
			supported for embedded still captures.
			
	@leave  May leave with any error code.
	*/
	virtual void GetSupportedEmbeddedStillCaptureSettingsL(RArray<TUid>& aSupportedEmbeddedStillCaptureSettings) const=0;
	
	/**
	Retrieves information regarding the supported direct saving state. If used, still images are saved in files rather than 
	providing clients the MCameraImageBuffer. Direct saving to file will continue even if the client application gets closed
	for any reasons.

	@param  aSupportedDirectSavingType
			Retrieves the enum specifying supported TDirectSavingType.
			If EDirectSavingNotUsed, direct saving not supported. Images will be received in buffer MCameraImageBuffer.
			
			If EDirectSavingHighResolutionFileOnly, direct saving to file is supported. But no cut down version of the image 
			will be saved to file. Callback used is MCaptureImageObserver::ImageDirectSavingCompleted().
			
			If EDirectSavingWithLowerResolutionFile, Direct saving to file is supported. Also, a cut down version of the image 
			will be saved to another specified file. Callbacks used are MCaptureImageObserver::ImageDirectSavingCompleted()
			for actual image and MCaptureImageObserver::CutDownImageDirectSavingCompleted() for lower resolution image.

	@leave  May leave with any error code.	
	*/
	virtual void GetSupportedDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType& aSupportedDirectSavingType) const=0;

	/**
	Provides the base name for file used to save images. If there are sequential images, files will be created by 
	implementation with names appearing as base name and appended by increasing integers which would start from 
	aStartingSequenceNumber. This is used for the original image only, but not for the cut down version of the original
	image.

	@param  aFilename
			A const TDesC8&: Base name for files which ECam implementation needs to create in order to collect every sequential
			image.
			
	@param  aStartingSequenceNumber
			The starting sequence number which will be appended to the base name 'aFilename'. The sequence number will keep
			on increasing for every individual still image capture.
			
	@leave  May leave with any error code.

	@note   It is upto the implementation how many digits it uses for sequence number. Accordingly, padding will be done with 
			zeroes.
	*/
	virtual void SetSequentialImageFilenameL(const TDesC8& aFilename, TInt aStartingSequenceNumber)=0;
	
	/**
	Provides the base name for file used to save cut down version of the original images. If there are sequential images, 
	files will be created by implementation with names appearing as base name and appended by increasing integers which would 
	start from aStartingSequenceNumber. This is used for the cut down version of the original image.

	@param  aLowerResolutionFilename
			A const TDesC8&: Base name for files which ECam implementation needs to create in order to collect the cut down version
			of every sequential image.
			
	@param  aStartingSequenceNumber
			The starting sequence number which will be appended to the base name 'aFilename'. The sequence number will keep
			on increasing for every individual still image capture.
			
	@leave  May leave with any error code.

	@note   It is upto the implementation how many digits it uses for sequence number. Accordingly, padding will be done with 
			zeroes.
	*/
	virtual void SetLowerResolutionSequentialImageFilenameL(const TDesC8& aLowerResolutionFilename, TInt aStartingSequenceNumber)=0;

	/**
	Retrieves the type of direct saving currently used. This will be represented as a direct saving state as given by 
	TDirectSavingType. 

	Whether or not the direct saving option is used, client will receive the MCaptureImageObserver::ImageCaptureComplete() 
	callback to mark the completion of the image capture operation.

	@param  aDirectSavingType
			Current type of the direct saving. 
			
			If EDirectSavingNotUsed, direct saving is not used. Images will be received in buffer MCameraImageBuffer through
			callback MCaptureImageObserver::ImageBufferReady().
			
			If EDirectSavingHighResolutionFileOnly, direct saving to file option is currently used. But no cut down version of
			the image will be saved to file. Callback used is MCaptureImageObserver::ImageDirectSavingCompleted().
			
			If EDirectSavingWithLowerResolutionFile, apart from direct saving to file option for the actual image, a cut down 
			version of the image will be saved to another specified file. Callbacks used are MCaptureImageObserver::
			ImageDirectSavingCompleted() for actual image and MCaptureImageObserver::CutDownImageDirectSavingCompleted() 
			for lower resolution image.
			
	@leave  May leave with any error code.
	
	@note   Direct saving to file will continue even if the client application gets closed for any reasons.
	*/
	virtual void GetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType& aDirectSavingType) const=0;

	/**
	Instructs the ECam implementation to use the desired type of direct saving option as specifed by its state.

	Whether or not the direct saving option is used, client will receive the MCaptureImageObserver::ImageCaptureComplete() 
	callback to mark the completion of the image capture operation.

	@param  aDirectSavingType
			The desired type of the direct saving. 
			
			If EDirectSavingNotUsed, direct saving will not be used. Images will be received in buffer MCameraImageBuffer 
			through	callback MCaptureImageObserver::ImageBufferReady().
			
			If EDirectSavingHighResolutionFileOnly, direct saving to file option will be used. But no cut down version of
			the image will be saved to file. Callback to be used is MCaptureImageObserver::ImageDirectSavingCompleted().
			
			If EDirectSavingWithLowerResolutionFile, apart from direct saving to file option for the actual image, a cut down 
			version of the image will be saved to another specified file. Callbacks to be used are MCaptureImageObserver::
			ImageDirectSavingCompleted() for actual image and MCaptureImageObserver::CutDownImageDirectSavingCompleted()
			for lower resolution image.
			
	@leave  May leave with any error code.

	@note   Clients need to provide the filename before capturing still images under direct saving option. Callback 
			MCaptureImageObserver::ImageDirectSavingCompleted() may provide error KErrNotReady if filenames are not 
			provided before images are captured for direct saving. Similarly, if cut down version of image is also to be 
			saved to file, MCaptureImageObserver::CutDownImageDirectSavingCompleted() may provide error KErrNotReady if 
			filenames are not provided before hand.
			
	@note   Direct saving to file will continue even if the client application gets closed for any reasons.
	*/
	virtual void SetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType aDirectSavingType)=0;
    
 	/**
	Retrieves whether the camera device is capable of providing capture event notification to the client. Client may take
	the responsibility of playing the capture sound after receiving the notification. 

	@param  aSupportedDriveModes
			A reference to bit-field of TUint which indicates the drive modes in which the capture event notification is
			supported. If aSupportedDriveModes =0, capture event notification is not supported.
			
	@note   If capture event notification is supported, ECam implementation will use KUidECamEventImageCaptureEvent to notify 
			clients that the image has been exposed to the camera sensor. Clients may play capture sound if they desire to do so.

	@leave  May leave with any error code.
	
	@see    CCamera::CCameraAdvancedSettings::TDriveMode	
	*/
	virtual void GetCaptureEventSupportInfoL(TUint& aSupportedDriveModes) const=0;
	
	/**
	Retrieves the supported image formats for a given resolution.
	
	@param  aImageFormatsSupported
			A bit field which retrieves the supported image formats for a given resolution.
			Formats have been defined as CCamera::TFormat
			
	@param  aSize
			The resolution (or size) for which the total number of supported image formats have to be retrieved.
	
	@leave  May leave with any error code.
	*/
	virtual void GetImageFormatsSupportedL(TUint& aImageFormatsSupported, const TSize& aSize) const=0;
	
	/**
	Retrieves the supported pixel aspect ratio for a given resolution in case of still image.
	
	@param  aPixelAspectsSupported
			A bit field which retrieves the supported pixel aspect ratio for a given resolution.
			Pixel aspect ratio have been defined as CCamera::CCameraAdvancedSettings::TPixelAspectRatio
	
	@param  aImageFormat
			The image format for which the supported pixel aspect ratio have to be retrieved.

	@param  aSize
			The resolution (or size) for which the supported pixel aspect ratio have to be retrieved.
	
	@leave  May leave with any error code.
	*/
	virtual void GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat aImageFormat, const TSize& aSize) const=0;
	
	/**
	Performs setup and allocation of memory for image capture operation. Implementation creates a new CCameraImageCapture*
	object which reflects the prepare image parameters passed by the client. The callback MPreImageCaptureControlObserver::
	PrepareImageComplete() passes the ownership of the CCameraImageCapture* object to the client.

	This needs to be called every time client wishes to change prepare image parameters or if the client wishes to create
	a new CCameraImageCapture* object. 

	@param  aPrepareImageParameters 
	        The desired image parameters to be used for capturing still images using the CCameraImageCapture instance
	        which would be passed by the implementation.
	        
	@param  aCaptureImageObserver
			The Capture image observer which is needed by the implementation in order to pass it to the 
			CCameraImageCapture while creating it.

	@note 	Next PrepareImageCapture can be called only after receiving the notification KUidECamEventReadyForNextPrepare.

	@note   Implementation will call CCameraImageCapture::CreateL(CCamera& aCamera, MCaptureImageObserver& 
			aCaptureImageObserver) method and pass the CCameraImageCapture* to the client through the 
			MPreImageCaptureControlObserver::PrepareImageComplete() callback.

	@note	If drive mode is EDriveModeTimeNudgeCapture the ECam implementation will immediately begin capturing images after
			creating a new CCameraImageCapture object. Once the client initiates image capture, the total amount of images
			requested by the client	(specified by the client in	TDriveModeDependentAttributes) will be returned to the client
			(via MCaptureImageObserver::ImageBufferReady())	or saved to file. The implementation should not create another
			instance of CCameraImageCapture whilst using this drive mode until the first instance is destroyed by the client.

	@see 	CCamera::PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex)
	*/
	virtual void PrepareImageCapture(const CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& 
											aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver)=0;
		
	/**
	Informs whether or not the setting of maximum memory size when encoding to the current format is supported.

	@param  aIsImageMaxMemorySizeSettingSupported
			ETrue indicates that setting of maximum memory size is supported.
			EFalse indicates that setting of maximum memory size is not supported.

	@leave  May leave with any error code.
	*/
	virtual void GetImageMaxMemorySizeSettingSupportInfoL(TBool& aIsImageMaxMemorySizeSettingSupported) const=0;

	/**
	Get the maximum memory size in kilo bytes when encoding to the current format.
	
	@param  aMemorySize
			Retrieves the maximum memory size in kilo bytes.
	
	@note   In case of JPEG, the maximum memory size will take preference over JPEG quality if the maximum memory size is 
			not sufficient to achieve the desired quality. Refer CCamera::JpegQuality()
			
	@leave  May leave with any error code.
	*/
	virtual void GetImageMaxMemorySizeL(TUint& aMemorySize) const=0;

	/**
	Retrieves the supported processing options.

	@param  aEcamProcessingOptionsSupported
			Bitfield containing the available processing options.

	@leave  May leave with error code.
	*/
	virtual void GetSupportedProcessingOptionsL(TUint& aEcamProcessingOptionsSupported) const=0;
	};

/** 
Mixin class for implementation by providers of the ImageCapture Camera Extension API. 
CameraImageCapture class exposes an API for capturing the image and controlling the overall capture.

This class also provides concrete implementation of API exposed by CCameraPostImageCaptureControl.
CameraPostImageCaptureControl class exposes an API for retrieving the image data from individual images (in case 
continuous drive mode is used for capturing the images) and also to apply control on the captured images individually.

@publishedPartner
@prototype 
*/	
class MCameraImageCapture
	{
	
public:
	
	/** 
	Releases the interface. 
	
	@param  aCameraImageCapture
			The pointer to the image capture class object which would be destroyed by the client.
	*/
	virtual void Release(CCamera::CCameraImageCapture* aCameraImageCapture)=0;
	
	/**
	Retrieves the concrete factory handle for the histogram implementation in order to use it specifically for a
	specific still image capture.
		   
	@param  aImplFactoryPtr
		    The concrete factory handle for the histogram implementation specific to the still image capture.
		   
	@leave  May leave with any error code.
	*/
	virtual void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	
	/**
	Retrieves the concrete factory handle for the snapshot implementation in order to 
	use it specifically for a specific still image capture.
		   
	@param  aImplFactoryPtr
		    The concrete factory handle for the snapshot implementation specific to the still image capture.
		   
	@leave  May leave with any error code.
	*/
	virtual void GetSnapshotImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	
	/**
	Retrieves the concrete factory handle for the image processing implementation in order to use it specifically for 
	a specific still image capture.
		   
	@param  aImplFactoryPtr
		    The concrete factory handle for the image processing implementation specific to the still image capture.
		   
	@leave  May leave with any error code.
	*/
	virtual void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	
	/**
	The observer for the image capture is passed to the implementation for passing callbacks on it.
	
	@param  aCaptureImageObserver
			The reference to the capture image observer.
	*/
	virtual void SetCaptureImageObserver(MCaptureImageObserver& aCaptureImageObserver)=0;	
	
	/**
	Retrieves the prepare image parameters tied with this image capture class object.

	@param aPrepareImageParameters
		   TPrepareImageParameters tied with this image capture class object.
		   
	@leave May leave with any error code
	*/
	virtual void GetPrepareImageParametersL(CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters) const=0;
	
	/**
	The prepare image parameters needed to capture images using the particular capture image class object.
	
	@param aPrepareImageParameters
		   The prepare image parameters used to capture images.
	*/
	virtual void SetPrepareImageParameters(const CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters)=0;
		
	/**
	Provides the interface pointers for the concrete implementation of CCameraPostImageCaptureControl.
	Every time this method is called, a newly created interface is provided since it represents the individual image.
	The ID maps the individual image with the CCameraPostImageCaptureControl instance and may also be stored by the 
	concrete implementation of CCameraPostImageCaptureControl.	
	
	@param  aInterface
			The uid representing the interface required.
	
	@param  aPostCaptureControlId
			The ID which maps the individual image with the CCameraPostImageCaptureControl object. This may be stored
			by the concrete implementation of CCameraPostImageCaptureControl.
			
	@return Interface pointer. 
	
	@note   It is this implementation which creates/owns CCameraPostImageCaptureControl class object after receiving image
			capture	requests by the client. Implementation may store all such mappings between aPostCaptureControlId and 
			CCameraPostImageCaptureControl* for a particular instance of CCameraImageCapture. 
	*/
	virtual TAny* CreatePostImageCaptureControlImpl(TUid aInterface, TPostCaptureControlId aPostCaptureControlId)=0;

	/**
	Performant image capture. This postpones the processing options involved with current image captured in order to 
	capture/prepare for next images. 
	
	Previously created CCameraPostImageCaptureControl objects for this CCameraImageCapture object will become unavailable 
	after this call.

	@param  aCameraImageCapture
			Pointer to aCameraImageCapture. This will be used by the implementation in order to pass in callbacks and to
			create CCameraPostImageCaptureControl object for every image and pass it to the client. Implementation is not
			supposed to not destroy this object.

	@note   Further images (either still or video) can be captured only after receiving the notification 
			KUidECamEventReadyForNextCapture. Current image may be outstanding in order to undergo any processing options.

	@note   Callback MCaptureImageObserver::ImageCaptureComplete() informs the client that the image capture operation has 
			been completed. The CCameraImageCapture& can be further used for next image captures.
			In case of continuous drive modes, this callback will be received by the client only once in order to provide the 
			result of the whole image capture operation. 
			
	@note   Callback MCaptureImageObserver::ImageBufferReady() provides client the link to CCameraPostImageCaptureControl& 
			which helps retrieving the image buffer. In case of continuous drive modes, this callback will be received by the
			client for every individual image. In case of single shots, this will be received by the client only once in 
			order to retrieve the image buffer.
	*/	
	virtual void CaptureImage(CCamera::CCameraImageCapture* aCameraImageCapture)=0;
	
	/**
	Cancels the outstanding Capture Image operation. This will also cancel any outstanding processing options associated
	with the concerned Capture Image operation. This method is synchronous. Hence, no callback shall be issued for the 
	concerned capture image operation.
	*/
	virtual void CancelCaptureImage()=0;
	
	/**
	Retrieves the number of images exposed to sensor with respect to a specific image capture command. Any Processing
	options associated with these images may be pending.

	@param  aNumImagesExposed
			Retrieves the number of images exposed to sensor till now. For single shot type of drive modes, this value will
			be one after the image gets exposed to sensor. For multiple shot type of drive modes, this value will be the 
			number of images exposed till now.
			
	@note   May leave with any error code.
	*/
	virtual void GetNumImagesExposedL(TUint& aNumImagesExposed) const=0;

	/**
	Retrieves the total number of images which is supposed to be captured with respect to a specific image capture command. 

	@param  aNumTotalImages
			Retrieves the total number of images supposed to be captured. For single shot type of drive modes, this value 
			will be	always be one. For multiple shot type of drive modes, this value will be greater than one.
			
	@note   May leave with any error code.
	*/
	virtual void GetNumTotalImagesL(TUint& aNumTotalImages) const=0;

	/**
	Retrieves the post capture control handle based on the ID passed to it. Client may use this handle for post capture 
	operations of the image identified by the ID. The ID is received by the clients through MCaptureImageObserver callbacks.

	@param  aPostCaptureControlHandle
			Retrieves pointer to the post capture control class object.
			
	@param  aPostCaptureControlId
			The ID used to retrieve the post capture control handle of the captured image.
			
	@note   May leave with any error code.
	
	@note   It is this implementation which creates/owns CCameraPostImageCaptureControl class object after receiving image
			capture	requests by the client. Implementation may store all such mappings between aPostCaptureControlId and 
			CCameraPostImageCaptureControl* for a particular instance of CCameraImageCapture. 
	*/	
	virtual void GetPostCaptureControlHandleL(CCamera::CCameraPostImageCaptureControl*& aPostCaptureControlHandle, 
																TPostCaptureControlId aPostCaptureControlId) const=0;
	/**
	Prioritises the Capture Image operation. This implies that the processing options associated with the concerned 
	Capture	Image operation will be prioritised over other pending processing options.

	@param  aCaptureImagePriority
			The desired level of priority.

	@leave  May leave with any error code.
	*/
	virtual void SetCaptureImagePriorityL(TECamImagePriority aCaptureImagePriority)=0;
		
	/**
	Retrieves the priority of the Capture Image operation.

	@param  aCaptureImagePriority
			Retrieves the current level of priority.

	@leave  May leave with any error code.
	*/
	virtual void GetCaptureImagePriorityL(TECamImagePriority& aCaptureImagePriority) const=0;

	/**
	Pauses processing options associated with the concerned Capture Image operation.

	@param  aProcessingTypes
			The processing options which need to be paused.
	*/
	virtual void PauseProcessing(TUint aProcessingTypes)=0;

	/**
	Resumes processing options associated with the concerned Capture Image operation.

	@param  aProcessingTypes
			The processing options which need to be resumed.

	@leave May leave with any error code.
	*/
	virtual void ResumeProcessingL(TUint aProcessingTypes)=0;
	};
	
/** 
This mixin class provides concrete implementation of API exposed by CCameraPostImageCaptureControl.
CameraPostImageCaptureControl class exposes an API for retrieving the image data from individual images (in case 
continuous drive mode is used for capturing the images) and also to apply control on the captured images individually.

@publishedPartner
@prototype 
*/	
class MCameraPostImageCaptureControl
	{
	
public:
	
	/** 
	Releases the interface. 
	
	@param  aPostCaptureControlId
			The ID useful to indentify the post capture control handle of the captured image being deleted.
	*/
	virtual void Release(TPostCaptureControlId aPostCaptureControlId)=0;
	
	/**
	Retrieves the sequence number of the image being represented by this post capture control object. The sequence number
	specifies the index of the individual image if, in case, the multiple shot type of drive mode is being used. The first 
	individual image which is exposed to the sensor has a sequence number of zero. In case of single shot type of drive mode,
	the sequence number will be zero.

	@param  aSequenceNumber
			Retrieves the sequence number of the image.

	@leave  May leave with any error code.
	*/
	virtual void GetImageSequenceNumberL(TUint& aSequenceNumber) const=0;
	
	/**
	CCancels the outstanding individual image which the object of this class is referring to. This will also cancel any outstanding 
	processing options associated with this individual image. This method is synchronous. Hence, no callback shall be 
	issued for this individual image.
	*/
	virtual void CancelImage()=0;
	
	/**
	Prioritises the individual image which the object of this class is referring to. This implies that any processing
	options associated with this individual image will be prioritised over other pending processings options.

	@param  aImagePriority
			The desired level of priority.
			
	@leave  May leave with any error code.
	*/
	virtual void SetImagePriorityL(TECamImagePriority aImagePriority)=0;
		
	/**
	Retrieves the priority of the individual image which the object of this class is referring to.

	@param  aImagePriority
			The current level of priority.
			
	@leave  May leave with any error code.
	*/
	virtual void GetImagePriorityL(TECamImagePriority& aImagePriority) const=0;
	
	/**
	Pauses processing options associated with the concerned Capture Image operation.

	@param  aProcessingTypes
			The processing options which need to be paused.
	*/
	virtual void PauseProcessing(TUint aProcessingTypes)=0;

	/**
	Resumes processing options associated with the concerned Capture Image operation.

	@param  aProcessingTypes
			The processing options which need to be resumed.

	@leave May leave with any error code.
	*/
	virtual void ResumeProcessingL(TUint aProcessingTypes)=0;
	
	/**
	Retrieves the image buffer which contains the individual image captured. This method should be called by the client as a 
	result of receiving the callback MCaptureImageObserver::ImageBufferReady(), if the 
	error information received is satisfactory.

	@param  aCameraImageBuffer
			A reference to an MCameraImageBuffer if successful, or NULL if not successful.

	@leave  May leave with any error code.
	*/
	virtual void GetImageBufferL(MCameraImageBuffer& aCameraImageBuffer) const=0;
	
	/**
	Retrieves the state of the individual image which the object of this class is referring to.

	@param  aImageState
			Retrieves the current state of the individual image.
			
	@leave  May leave with any error code.
	*/	
	virtual void GetImageStateL(CCamera::CCameraPostImageCaptureControl::TImageState& aImageState) const=0;
	
	/**
	Retrieves the state of the individual image buffer.

	@param  aBufferState
			Retrieves the current state of the individual image buffer.
			
	@leave  May leave with any error code.
	*/	
	virtual void GetBufferStateL(CCamera::CCameraPostImageCaptureControl::TBufferState& aBufferState) const=0;
	};

/** 
Mixin class for implementation by providers of the Video Capture Control Camera Extension API.
CCamera Video Capture Control class exposes an API for controlling advanced video capture related settings and control.

@publishedPartner
@prototype 	
*/	
class MCameraVideoCaptureControl
	{
	
public:
	
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;
	
	/**
	Retrieves the concrete factory handle for the histogram implementation in order to 
	use it specifically for the video capture.
		   
	@param aImplFactoryPtr
		   The concrete factory handle for the histogram implementation specific to the video capture.
		   
	@leave  May leave with any error code.
	*/
	virtual void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
    	
	/**
	Retrieves the concrete factory handle for the snapshot implementation in order to 
	use it specifically for the video capture.
		   
	@param  aImplFactoryPtr
		    The concrete factory handle for the snapshot implementation specific to the video capture.
		   
	@leave  May leave with any error code.
	*/
	virtual void GetSnapshotImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	
	/**
	Retrieves the concrete factory handle for the image processing implementation in order to 
	use it specifically for the video capture.
	
	@param aImplFactoryPtr
		   The concrete factory handle for the image processing implementation specific to the video capture.
		   
	@leave  May leave with any error code.
	*/
	virtual void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const=0;
	
	/**
	The observer for the video capture is passed to the implementation for passing callbacks on it.
	
	@param  aCaptureVideoObserver
			The reference to the capture video observer.
	*/
	virtual void SetCaptureVideoObserver(MCaptureVideoObserver& aCaptureVideoObserver)=0;
	
    /**
	Retrieves the supported video formats for a given resolution.
	
	@param  aVideoFormatsSupported
			A bit field which retrieves the supported video formats for a given resolution.
			Formats have been defined as CCamera::TFormat
			
	@param  aSize
			The resolution (or size) for which the total number of supported video formats have to be retrieved.
	
	@leave  May leave with any error code.
	*/
	virtual void GetVideoFormatsSupportedL(TUint& aVideoFormatsSupported, const TSize& aSize) const=0;

	/**
	Retrieves the supported pixel aspect ratio for a given resolution in case of video.
	
	@param  aPixelAspectsSupported
			A bit field which retrieves the supported pixel aspect ratio for a given resolution.
			Pixel aspect ratio have been defined as CCamera::CCameraAdvancedSettings::TPixelAspectRatio
	
	@param	aVideoFormat
			The video format for which the supported pixel aspect ratio have to be retrieved.
		
	@param  aSize
			The resolution (or size) for which the supported pixel aspect ratio have to be retrieved.
	
	@leave  May leave with any error code.
	*/
	virtual void GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat aVideoFormat, const TSize& aSize) const=0;

	/**
	Informs whether or not the 'embedded still capture' feature is supported. Allowing still image capture in between 
	on-going video capture is referred to as embedded still capture.

	@param  aSupportedEmbeddedStillCaptureTypes
			Retrieves the supported embedded still capture. The TInt is retrieved as a bit field of supported 
			TEmbeddedStillCaptureTypes.

			
	@leave  May leave with any error code.
	*/
	virtual void GetEmbeddedStillCaptureSupportInfoL(TInt& aSupportedEmbeddedStillCaptureTypes) const=0;

	/**
	Asynchronous method to prepare for video capture.

	Performs setup and allocation of memory prior to calling StartVideoCapture()
	to keep the latency of that function to a minimum.

	@param  aPrepareVideoParameters 
	 		Parameters necessary to prepare for video capture.
	 		
	@note   Event KUidECamEventVideoCaptureControlPrepareComplete is used to notify clients about completing the preparation
			for video capture.
			
	@note 	Next PrepareVideoCapture can be called only after receiving the notification KUidECamEventReadyForNextPrepare.
			
	@note   If some camera settings get affected because of desired video settings such as frame rate/frame size, 
			specific notifications will be sent to the client about camera settings being affected.
			
	@note	Event KUidECamEventVideoCaptureControlSettingsRangeChanged: informs that range of certain camera settings have been changed.
			Client may call GetRangeAffectedSettingsL(RArray<TUid>& aRangeAffectedSettings) const to get the list of affected camera settings.

	@note	Event KUidECamEventVideoCaptureControlSettingsValueChanged: informs that value of certain camera settings have been changed. 
			Client may call GetValueAffectedSettingsL(RArray<TUid>& aValueAffectedSettings) const to get the list of affected camera settings.

	@note	Event KUidECamEventVideoCaptureControlSettingsDisabled: informs that certain camera settings have been disabled. 
			Client may call GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const to get the list of affected camera settings.

	@see 	CCamera::PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer)
	
	@see	ReleaseVideoResource()
	*/
	virtual void PrepareVideoCapture(const CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters& aPrepareVideoParameters)=0;
	
	/**
	Retrieves the camera settings whose range got affected once the desired video settings (frame rate/frame size) are in place. 
	This method may be called by the client after receiving the notification KUidECamEventVideoCaptureControlSettingsRangeChanged.

	@param  aRangeAffectedSettings
			Retrieves the list of range affected settings
			
	@leave  May leave with any error code.  

	@see 	PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)
	*/
	virtual void GetRangeAffectedSettingsL(RArray<TUid>& aRangeAffectedSettings) const=0;
	
	/**
	Retrieves the camera settings whose value got affected once the desired video settings (frame rate/frame size) are in place. 
	This method may be called by the client after receiving the notification KUidECamEventVideoCaptureControlSettingsValueChanged.

	@param  aValueAffectedSettings
			Retrieves the list of value affected settings

	@leave  May leave with any error code.

	@see 	PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)	
	*/
	virtual void GetValueAffectedSettingsL(RArray<TUid>& aValueAffectedSettings) const=0;
	
	/**
	Retrieves the camera settings whose value got affected once the desired video settings (frame rate/frame size) are in place. 
	This method may be called by the client after receiving the notification KUidECamEventVideoCaptureControlSettingsDisabled.

	@param  aDisabledSettings
			Retrieves the list of disabled settings

	@leave  May leave with any error code.

	@see 	PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)
	*/
	virtual void GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const=0;
	
	/**
	Frees the video resources which were set up as a result of CCameraVideoCaptureControl::PrepareVideoCapture call.
	If this methid is called while PrepareVideoCapture call is outstanding, then this will be equivalent to cancelling the
	PrepareVideoCapture call and release any allocated resources.
	*/
	virtual void ReleaseVideoResource()=0;
	
	/**
	Starts the video capture. This operation gives priority to the low latency aspects.

	Video frames are send to client via MCaptureVideoObserver::VideoBufferReady(MCameraBuffer2& aVideoBuffer, TInt aErrorCode).

	@leave  May leave with any error code.

	@note	This method is recommended to be used rather than CCamera::StartVideoCaptureL().
	*/												
	virtual void StartVideoCaptureL()=0;

	/**
	Stops the video capture. This operation gives priority to the low latency aspects.

	@note	This method is recommended to be used rather than CCamera::StopVideoCapture().
	*/												
	virtual void StopVideoCapture()=0;
	
	/**
	Pauses the on-going video capture. MCaptureVideoObserver::VideoBufferReady(MCameraBuffer2& aVideoBuffer, TInt aErrorCode) 
	callback will not be received by the client until the video capture is resumed.
	*/
	virtual void PauseVideoCapture()=0;
		
	/**
	Resumes the on-going video capture. MCaptureVideoObserver::VideoBufferReady(MCameraBuffer2& aVideoBuffer, TInt aErrorCode) 
	callback will again be received by the client.

	@leave  May leave with any error code.
	*/
	virtual void ResumeVideoCaptureL()=0;
	
	/**
	Retrieves the fading effect state for video capture.

	@param  aFadingEffectState
			Retrieves the current fading effect state for video capture.

	@leave  May leave with any error code.
	
	@internalTechnology
	*/
	virtual void GetFadingEffectStateL(CCamera::CCameraVideoCaptureControl::TFadingEffectState& aFadingEffectState) const=0;
		
	/**
	Sets the fading effect state for video capture.

	@param  aFadingEffectState
			The desired fading effect state for video capture.
			
	@note   Triggers a KUidECamEventVideoCaptureControlFadingEffect event notification.
	
	@internalTechnology
	*/	
	virtual void SetFadingEffectState(CCamera::CCameraVideoCaptureControl::TFadingEffectState aFadingEffectState)=0;
		
	/**
	Retrieves the current video capture state.
	
	@param aVideoCaptureState
		   Retrieves the current video capture state.
	
	@leave  May leave with any error code.
	*/
	virtual void GetVideoCaptureStateL(CCamera::CCameraVideoCaptureControl::TVideoCaptureState& aVideoCaptureState) const=0;
	
	/**
	Retrieves the various types of video capture supported. 
	@param  aSupportedVideoCaptureTypes
			Retrieves the supported video capture type. The TInt is retrieved as a bit field of supported 
			TVideoCaptureType.
			
	@leave  May leave with any error code.
	*/
	virtual void GetVideoCaptureSupportInfoL(TInt& aSupportedVideoCaptureTypes) const=0;
	
	/**
	Retrieves the current prepare video parameters.
	
	@param aPrepareVideoParameters
		   Retrieves the current prepare video parameters.
		   	
	@leave  May leave with any error code.
	*/
	virtual void GetPrepareVideoParametersL(CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters& aPrepareVideoParameters) const=0;
	};
	
#endif // ECAMCAPTURECONTROLINTF_H
