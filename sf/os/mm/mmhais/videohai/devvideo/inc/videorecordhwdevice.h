// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __VIDEORECORDHWDEVICE_H__
#define __VIDEORECORDHWDEVICE_H__

#include <mmf/devvideo/devvideobase.h>
#include <mmf/devvideo/devvideorecord.h>

/**
A base class for all video recording (encoding and pre-processing) hardware devices. Since both encoders 
and pre-processors can implement pre-processing functionality, this class includes all pre-processing 
related methods.
@publishedAll
@released
*/
class CMMFVideoRecordHwDevice : public CMMFVideoHwDevice
	{
public:
	/**
	Retrieves information about the pre-processing capabilities of this hardware device.

	@return	"Pre-processor information as a CPreProcessorInfo object. The object is pushed to the 
			cleanup stack, and must be deallocated by the client."
	@leave	"The method will leave if an error occurs."
	*/
	virtual CPreProcessorInfo* PreProcessorInfoLC() = 0;

	/**
	Sets the hardware device input format. If both a pre-processor and an encoder are used, the 
	pre-processor output format and the encoder input format must be the same. The input format for 
	the first device in the system is the input format for video input data. The method has to be called 
	for both direct capture as well as memory buffer input.

	@param	"aFormat"		"The input format to use."
	@param	"aPictureSize"	"The input picture size in pixels."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The input format specified is not supported."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetInputFormatL(const TUncompressedVideoFormat& aFormat, const TSize& aPictureSize) = 0;

	/**
	Sets the data source to be a camera, and sets the device to use direct capture for input.

	@param	"aCameraHandle"	"A camera handle for the camera to use. The handle is passed 
							to CCamera::NewDuplicateL() in the camera API "
	@param	"aPictureRate"	"Video capture picture rate."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - Direct capture is not supported or the picture rate specified is not 
									 supported."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetSourceCameraL(TInt aCameraHandle, TReal aPictureRate) = 0;

	/**
	Sets the data source to be memory buffers.

	@param	"aMaxPictureRate"		"The maximum picture rate for input pictures."
	@param	"aConstantPictureRate"	"True if the input picture rate is constant. In that case, 
									aMaxPictureRate specifies the picture rate."
	@param	"aProcessRealtime"		"True if real-time processing is needed, false if not. Real-time 
									processing is typically neede for video recording applications, while
									video conversion and off-line processing applications do not require it."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The picture rate specified is not supported."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetSourceMemoryL(TReal aMaxPictureRate, TBool aConstantPictureRate, TBool aProcessRealtime) = 0;

	/**
	Sets the clock source to use for video timing. When video recording is synchronized with audio, 
	the clock source is implemented by the audio playback subsystem, otherwise the clock source should 
	get the time from the system clock.

	If no clock source is set, video recording will not be synchronized, but will proceed as fast as
	possible, depending on input data and output buffer availability. If direct capturing is used without
	a clock source, the timestamps in the output data may not be valid.

	@param	"aClock"	"The clock source to use."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetClockSource(MMMFClockSource* aClock) = 0;

	/**
	Sets pre-processing options for RGB to YUV color space conversion. By default, input RGB data is 
	assumed to use the full value range ([0…255]), and the output YUV format is the hardware device output 
	format, so typically calling this method is not necessary.

	@param	"aRange"		"Input RGB data range"
	@param	"aOutputFormat"	"Conversion output YUV format."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The formats specified are not supported"
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetRgbToYuvOptionsL(TRgbRange aRange, const TYuvFormat& aOutputFormat) = 0;

	/**
	Sets pre-processing options for YUV to YUV data format conversion. By default, the hardware device input 
	and output data formats are used. For encoder devices, the device input format and a the closest matching
	format supported by the encoding process are used. Typically calling this method is not necessary.

	@param	"aInputFormat"	"Conversion input format."
	@param	"aOutputFormat"	"Conversion output format."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The formats specified are not supported"
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetYuvToYuvOptionsL(const TYuvFormat& aInputFormat, const TYuvFormat& aOutputFormat) = 0;

	/**
	Sets the pre-processing types to be used.

	@param	"aPreProcessTypes"	"The pre-processing steps to perform, a bitwise OR of values from
								TPrePostProcessType."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The pre-processing combination is not supported"
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetPreProcessTypesL(TUint32 aPreProcessTypes) = 0;

	/**
	Sets pre-processing options for rotation.

	@param	"aRotationType"	"The rotation to perform."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The rotation type is not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetRotateOptionsL(TRotationType aRotationType) = 0;

	/**
	Sets pre-processing options for scaling.

	@param	"aTargetSize"			"Target picture size. If a fixed scale factor size is used, the new 
									dimensions must be set to: 
										width=floor(factor*width), height=floor(factor*height). 
									For example, scaling a QCIF (176x144) picture up by a factor of 4/3 
									yields a size of 234x192."
	@param	"aAntiAliasFiltering"	"True if anti-aliasing filtering should be used. If the pre-processor 
									does not support anti-aliased scaling, or supports anti-aliased scaling
									only, this argument is ignored."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified target size is not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering) = 0;

	/**
	Sets pre-processing options for input cropping. Input cropping is typically used for digital zooming.

	@param	"aRect"	"The input cropping rectangle specifying the area of the picture to use. The rectangle 
					must fit completely inside the input picture."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified cropping rectangle is not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetInputCropOptionsL(const TRect& aRect) = 0;

	/**
	Sets pre-processing options for output cropping. Output cropping is performed after other 
	pre-processing operations but before output padding. Output cropping and padding can be used in 
	combination to prepare the picture size to suit the encoder, typically video encoders only support
	picture sizes that are multiples of 16 pixels.

	@param	"aRect"	"The output cropping rectangle specifying the area of the picture to use. The 
					rectangle must fit completely inside the picture."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified cropping rectangle is not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetOutputCropOptionsL(const TRect& aRect) = 0;

	/**
	Sets pre-processing options for output padding. Output padding is performed as the last pre-processing 
	operation, and typically used to prepare the picture size to suit the encoder. The image is padded with 
	black pixels.

	@param	"aOutputSize"	"The padded output picture size. The output size must be large enough for the 
							picture in its new position."
	@param	"aPicturePos"	"The position for the original picture in the new padded picture. The original 
							picture in its new position must fit completely inside the new picture."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified padding settings are not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetOutputPadOptionsL(const TSize& aOutputSize, const TPoint& aPicturePos) = 0;

	/**
	Sets color enhancement pre-processing options.

	@param	"aOptions"	"Color enchancement options."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetColorEnhancementOptionsL(const TColorEnhancementOptions& aOptions) = 0;

	/**
	Sets frame stabilisation options.

	@param	"aOutputSize"			"Output picture size. The output picture size must be smaller than 
									or equal to the hardware device input picture size."
	@param	"aFrameStabilisation"	"True if frame stabilisation should be used. By default stabilisation is 
									not used."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetFrameStabilisationOptionsL(const TSize& aOutputSize, TBool aFrameStabilisation) = 0;

	/**
	Sets custom implementation-specific pre-processing options.

	@param	"aOptions"	"Post-processing options. The data format is implementation-specific."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The options are not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetCustomPreProcessOptionsL(const TDesC8& aOptions) = 0;

	/**
	Initializes the device, and reserves hardware resources. If direct capture is used, this method also 
	prepares the camera API for capture by calling PrepareVideoCaptureL(). This method is asynchronous, 
	the device will call MMMFDevVideoRecordProxy::MdvrpInitializeComplete() after initialization has completed.
	Video capturing and encoding can be started with Start() with a relatively low delay since the hardware
	has already been set up.

	Error handling: Errors are reported using the MdvrpInitializeComplete() callback method. Typical error 
	codes used:
		- KErrHardwareNotAvailable - Not enough free video processing hardware resources
		- KErrNotSupported - The current configuration is not supported.
	@pre	"This method can only be called before the hwdevice has been initialized."
	*/
	virtual void Initialize() = 0;

	/**
	Commit all changes since the last CommitL(), Revert() or Initialize()
	to the hardware device.  This only applies to methods which can be called both
	before AND after DevVideoPlay has been initialized.

	@see	CMMFVideoEncodeHwDevice::SetOutputRectL
	@see	CMMFVideoRecordHwDevice::SetPreProcessTypesL
	@see	CMMFVideoRecordHwDevice::SetRotateOptionsL
	@see	CMMFVideoRecordHwDevice::SetScaleOptionsL
	@see	CMMFVideoRecordHwDevice::SetInputCropOptionsL
	@see	CMMFVideoRecordHwDevice::SetOutputCropOptionsL
	@see	CMMFVideoRecordHwDevice::SetOutputPadOptionsL
	@see	CMMFVideoRecordHwDevice::SetColorEnhancementOptionsL
	@see	CMMFVideoRecordHwDevice::SetFrameStabilisationOptionsL
	@see	CMMFVideoRecordHwDevice::SetCustomPreProcessOptionsL
	@see	CMMFVideoEncodeHwDevice::SetCodingStandardSpecificOptionsL
	@see	CMMFVideoEncodeHwDevice::SetImplementationSpecificEncoderOptionsL

	@leave  "The method will leave if an error occurs."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void CommitL() = 0;

	/**
	Revert all changes since the last CommitL(), Revert() or Initialize()
	back to their previous settings.  This only applies to methods which can 
	be called both before AND after DevVideoPlay has been initialized.

	@see	CMMFVideoEncodeHwDevice::SetOutputRectL
	@see	CMMFVideoRecordHwDevice::SetPreProcessTypesL
	@see	CMMFVideoRecordHwDevice::SetRotateOptionsL
	@see	CMMFVideoRecordHwDevice::SetScaleOptionsL
	@see	CMMFVideoRecordHwDevice::SetInputCropOptionsL
	@see	CMMFVideoRecordHwDevice::SetOutputCropOptionsL
	@see	CMMFVideoRecordHwDevice::SetOutputPadOptionsL
	@see	CMMFVideoRecordHwDevice::SetColorEnhancementOptionsL
	@see	CMMFVideoRecordHwDevice::SetFrameStabilisationOptionsL
	@see	CMMFVideoRecordHwDevice::SetCustomPreProcessOptionsL
	@see	CMMFVideoEncodeHwDevice::SetCodingStandardSpecificOptionsL
	@see	CMMFVideoEncodeHwDevice::SetImplementationSpecificEncoderOptionsL

	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Revert() = 0;

	/**
	Writes an uncompressed input picture. When the picture has been used, it must be returned to the client
	with MMMFDevVideoRecordProxy::MdvrpReturnPicture(). This method must not be called if direct capture is 
	used.

	@param	"aPicture"	"The picture to write."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void WritePictureL(TVideoPicture* aPicture) = 0;

	/**
	Notifies the hardware device that the end of input data has been reached and no more input pictures
	will be written. The hardware device can use this signal to ensure that the remaining data gets 
	processed, without waiting for new data. After the remaining data has been processed, the hardware
	device must call the proxy callback MdvrpStreamEnd().
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void InputEnd() = 0;

	/**
	Starts recording video. This includes capturing pictures from the camera (if direct capture is used), 
	pre-processing and encoding. Recording will proceed until it is stopped or paused. Initally recording 
	is stopped.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Start() = 0;

	/**
	Stops recording video. No new pictures will be captured, pre-processed, or encoded. If input pictures
	are written while recording is stopped, they will be returned immediately.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Stop() = 0;

	/**
	Pauses video recording. Recording can be resumed using Resume().
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Pause() = 0;

	/**
	Resumes video recording after a pause.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Resume() = 0;

	/**
	Freezes the input picture. Normal encoding can be continued using ReleaseFreeze().
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Freeze() = 0;

	/**
	Releases a frozen input picture. Video capturing and encoding continues normally.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void ReleaseFreeze() = 0;

	/**
	Returns the current recording position. The position is the capture timestamp from the latest input 
	picture, or the capture timestamp for the latest picture captured from the camera when direct capture
	is used.

	@return	"The current recording position."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TTimeIntervalMicroSeconds RecordingPosition() = 0;

	/**
	Reads various counters related to processed video pictures. See the definition of TPictureCounters
	for a description of the counters. The counters are reset when Initialize() or this method is called,
	and thus they only include pictures processed since the last call.

	@param	"aCounters"	"The counter structure to fill."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetPictureCounters(CMMFDevVideoRecord::TPictureCounters& aCounters) = 0;

	/**
	Reads the frame stabilisation output picture position. This information can be used for positioning the
	viewfinder. The position returned is the stabilisation result for the most recent input picture.

	@param	""aRect	"The position of the stabilisation output picture inside the input picture. If frame 
					stabilisation is not used, the rectangle is set to cover the entire input picture."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetFrameStabilisationOutput(TRect& aRect) = 0;

	/**
	Retrieves the number of complexity control levels available for this hardware device. Devices can 
	support processing the same input data with different computational complexity levels. The complexity
	level can affect, for example, the motion vector search range used in an encoder.

	@return	"The number of complexity control levels available, one if multiple levels are not supported."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TUint NumComplexityLevels() = 0;

	/**
	Sets the complexity level to use for video processing in a hardware device. The level can be changed 
	at any time.

	@param	"aLevel"	"The computational complexity level to use. Level zero (0) is the most complex one, 
						with the highest quality. Higher level numbers require less processing and may have
						lower quality."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetComplexityLevel(TUint aLevel) = 0;
	};


/**
CMMFVideoEncodeHwDevice is the MSL video encoder hardware device interface. All video encoders must 
implement this interface.
@publishedAll
@released
*/
class CMMFVideoEncodeHwDevice : public CMMFVideoRecordHwDevice
	{
public:
	/**
	Creates a new video encoder hardware device object, based on the implementation UID.

	@param	"aUid"		"Encoder implementation UID."
	@param	"aProxy"	"The proxy implementation to use."
	@return	"A new CMMFVideoEncodeHwDevice object."
	@leave	"This method will leave if an error occurs."
	*/
	IMPORT_C static CMMFVideoEncodeHwDevice* NewL(TUid aUid, MMMFDevVideoRecordProxy& aProxy);

	/**
	Creates a new video encoder hardware device adapter object, based on the Interface Implementation of a Processing Unit.

	@param	"aImplInfo"	"The registration data relating to the Interface Implementation of the Processing Unit."
	@param	"aProxy"	"The proxy implementation to use."
	@return	"A new CMMFVideoEncodeHwDevice object."
	@leave	"This method will leave if an error occurs."
	*/
	IMPORT_C static CMMFVideoEncodeHwDevice* NewPuAdapterL(const CImplementationInformation& aImplInfo, MMMFDevVideoRecordProxy& aProxy);

	/**
	Destructor.
	*/
	IMPORT_C virtual ~CMMFVideoEncodeHwDevice();

	/**
	Retrieves information about the video encoder.

	@return	"Encoder information as a CVideoEncoderInfo object. The object is pushed to the cleanup stack,
			and must be deallocated by the caller."
	@leave	"This method will leave if an error occurs."
	*/
	virtual CVideoEncoderInfo* VideoEncoderInfoLC() = 0;

	/**
	Sets the encoder output format. The picture size depends on the input data format and possible 
	scaling performed.

	@param	"aFormat"				"The video format to use."
	@param	"aDataUnitType"			"The type of output coded data units."
	@param	"aDataEncapsulation"	"Data encapsulation type for output encoded data units."
	@param	"aSegmentationAllowed"	"True if a coded data unit can be segmented into multiple output buffers
									if a single buffer is not large enough."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The format specified is not supported."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetOutputFormatL(const CCompressedVideoFormat& aFormat, 
								  TVideoDataUnitType aDataUnitType, 
								  TVideoDataUnitEncapsulation aDataEncapsulation,
								  TBool aSegmentationAllowed=EFalse) = 0;

	/**
	Sets the pre-processor device that will write data to this encoder. Uncompressed pictures will be 
	written with WritePictureL() or through a custom interface. After pictures have been processed, 
	they must be returned to the pre-processor using ReturnPicture().

	@param	"aDevice"	"The input pre-processor device to use."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetInputDevice(CMMFVideoPreProcHwDevice* aDevice) = 0;

	/**
	Sets the number of bit-rate scalability layers to use. Set to 1 to disable layered scalability. 

	Bit-rate scalability refers to the ability of a coded stream to be decoded at different bit-rates. 
	Scalable video is typically ordered into hierarchical layers of data. A base layer contains an 
	individual representation of a video stream and enhancement layers contain refinement data in addition
	to the base layer. The quality of the decoded video stream progressively improves as enhancement layers
	are added to the base layer.

	@param	"aNumLayers"	"The number of bit-rate scalability layers to use, set to 1 to disable 
							scalability."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The scalability layers are not supported or too many layers specified."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetNumBitrateLayersL(TUint aNumLayers) = 0;

	/**
	Sets the scalability type for a bit-rate scalability layer.

	@param	"aLayer"			"Layer number. Layers are numbered [0...n-1], where n is the number of layers
	available. The				first layer is the base layer, it can be decoded independently from the other
								layers, and it has the lowest total bitrate."
	@param	"aScalabilityType"	"Layer scalability type."
	@leave	" The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The scalability layers or the specified scalability type are not 
									 supported."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetScalabilityLayerTypeL(TUint aLayer, TScalabilityType aScalabilityType) = 0;

	/**
	Sets the reference picture options to be used for all scalability layers. The settings can be overridden
	for an individual scalability layer by using SetLayerReferenceOptions().

	@param	"aMaxReferencePictures"	"The maximum number of reference pictures to be used. More than one 
									reference frame can be used in the H.264 | MPEG-4 AVC and in some 
									advanced profiles of MPEG-4 Part 2 and H.263. The minimum value is one."
	@param	"aMaxPictureOrderDelay"	"The maximum picture order delay, in number of pictures. This specifies
									the maximum amount of  pictures that precede any picture in the sequence
									in decoding order and follow the picture in presentation order. Pictures
									may be coded/decoded in different order from their capture/display order.
									Thus, decoded pictures have to be buffered to order them in correct 
									display order. For example, if one conventional B picture is coded 
									between P pictures, a one-picture display ordering delay has to be 
									applied in the decoder. The minimum value is zero, which indicates 
									that pictures must be coded in capture/display order."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetGlobalReferenceOptions(TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay) = 0;

	/**
	Sets the reference picture options to be used for a single scalability layer. These settings override 
	those set with SetGlobalReferenceOptions().

	@param	"aLayer"				"Layer number."
	@param	"aMaxReferencePictures"	"The maximum number of reference pictures to be used."
	@param	"aMaxPictureOrderDelay"	"The maximum picture order delay, in number of pictures."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetLayerReferenceOptions(TUint aLayer, TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay) = 0;

	/**
	Sets encoder buffering options.

	@param	"aOptions"	"The buffering options."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetBufferOptionsL(const TEncoderBufferOptions& aOptions) = 0;

	/**
	Sets the encoder output rectangle. This rectangle specifies the part of the input and output pictures 
	which is displayed after encoding. Many video codecs process data in 16x16 pixel units but enable 
	specifying and coding the encoder output rectangle for image sizes that are not multiple of 16 pixels 
	(e.g 160x120).

	@param	"aRect"	"The encoder output rectangle."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetOutputRectL(const TRect& aRect) = 0;

	/**
	Sets whether bit errors or packets losses can be expected in the video transmission. The video encoder 
	can use this information to optimize the bitstream.

	@param	"aBitErrors"	"True if bit errors can be expected."
	@param	"aPacketLosses"	"True if packet losses can be expected."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetErrorsExpected(TBool aBitErrors, TBool aPacketLosses) = 0;

	/**
	Sets the minimum frequency (in time) for instantaneous random access points in the bitstream. An 
	instantaneous random access point is such where the decoder can achieve a full output picture 
	immediately by decoding data starting from the random access point. The random access point frequency 
	may be higher than signalled, if the sequence contains scene cuts which typically cause a coding 
	of a random access point.

	@param	"aRate"	"Random access point rate, in pictures per second. For example, to request a random 
					access point every ten seconds, set the value to 0.1."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetMinRandomAccessRate(TReal aRate) = 0;

	/**
	Sets coding-standard specific encoder options.

	@param	"aOptions"	"The options to use. The data format for the options is coding-standard specific, 
						and defined separately."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetCodingStandardSpecificOptionsL(const TDesC8& aOptions) = 0;

	/**
	Sets implementation-specific encoder options.

	@param	"aOptions"	"The options to use. The data format for the options is specific to the encoder 
						implementation, and defined separately by the encoder implementer."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetImplementationSpecificEncoderOptionsL(const TDesC8& aOptions) = 0;

	/**
	Returns coding-standard specific initialization output from the encoder. The information can contain, 
	for example, the MPEG-4 VOL header. This method can be called after Initialize() has been called.

	@return	"Coding-standard specific initialization output. The data format is coding-standard specific 
			and defined separately. The buffer is pushed to the cleanup stack, and the caller is responsible
			for deallocating it."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual HBufC8* CodingStandardSpecificInitOutputLC() = 0;

	/**
	Returns implementation-specific initialization output from the encoder. This method can be called after
	Initialize() has been called.

	@return	"Implementation-specific initialization output. The data format is specific to the encoder 
			implementation, and defined by the encoder supplier. The buffer is pushed to the cleanup stack,
			and the caller is responsible for deallocating it."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual HBufC8* ImplementationSpecificInitOutputLC() = 0;

	/**
	Sets the number of unequal error protection levels. By default unequal error protection is not used.

	@param	"aNumLevels"		"The number of unequal error protection levels. To disable unequal error
								protection, set this value to one"
	@param	"aSeparateBuffers"	"True if each unequal error protection level of a coded data unit shall be
								encapsulated in its own output buffer. Ignored if unequal error protection
								is not used."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - Unequal error protection is not supported."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetErrorProtectionLevelsL(TUint aNumLevels, TBool aSeparateBuffers) = 0;

	/**
	Sets up an unequal error protection level. If unequal error protection is not used, this method can be
	used to control settings for the whole encoded bitstream.

	@param	"aLevel"	"Error protection level number. This argument is ignored if unequal error protection
						is not in use."
	@param	"aBitrate"	"Target bit-rate for this error protection level."
	@param	"aStrength"	"Forward error control strength for this error protection level. The strength
						can be specified using values from TErrorControlStrength (EFecStrengthNone, 
						EFecStrengthLow, EFecStrengthNormal, EFecStrengthHigh), or with intermediate 
						values between those constants."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified bit-rate cannot be supported. "
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetErrorProtectionLevelL(TUint aLevel, TUint aBitrate, TUint aStrength) = 0;

	/**
	Sets the expected or prevailing channel conditions for an unequal error protection level, in terms of 
	expected packet loss rate. The video encoder can use this information to optimize the generated bitstream.

	@param	"aLevel"			"Error protection level number. This argument is ignored if unequal error 
								protection is not in use."
	@param	"aLossRate"			"Packet loss rate, in number of packets lost per second. Set to 0.0 if
								packet losses are not expected."
	@param	"aLossBurstLength"	"Expected average packet loss burst length. Set to zero if the information is 
								not available."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetChannelPacketLossRate(TUint aLevel, 
										  TReal aLossRate, 
										  TTimeIntervalMicroSeconds32 aLossBurstLength) = 0;

	/**
	Sets the expected or prevailing channel conditions for an unequal error protection level, in terms of 
	expected bit error rate. The video encoder can use this information to optimize the generated bitstream.

	@param	"aLevel"		"Error protection level number. This argument is ignored if unequal error 
							protection is not in use."
	@param	"aErrorRate"	"Expected bit error rate, as a fraction of the total bits transmitted. Set 
							to 0.0 if bit errors are not expected."
	@param	"aStdDeviation"	"Expected bit error rate standard deviation."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetChannelBitErrorRate(TUint aLevel, TReal aErrorRate, TReal aStdDeviation) = 0;

	/**
	Sets the target size of each coded video segment. The segment target size can be specified in terms of 
	number of bytes per segment, number of macroblocks per segment, or both.

	@param	"aLayer"			"Layer number. Layers are numbered [0...n-1], where n is the number of 
								layers available. Use zero if layered bit-rate scalability is not used."
	@param	"aSizeBytes"		"Segment target size in bytes. Set to zero to use unlimited segment size. The
								segment size in bytes should include all data that is typically stored or
								transmitted for each segment in the format currently in use. This includes all
								related headers."
	@param	"aSizeMacroblocks"	"Segment target size in number of macroblocks per segment. Set to zero to
								use unlimited segment size."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetSegmentTargetSize(TUint aLayer, TUint aSizeBytes, TUint aSizeMacroblocks) = 0;

	/**
	Sets the bit-rate control options for a layer. If layered bit-rate scalability is not used, the options
	are set for the whole bitstream.

	@param	"aLayer"	"Layer number. Layers are numbered [0...n-1], where n is the number of layers 
						available. Use zero if layered bit-rate scalability is not used."
	@param	"aOptions"	"Bit-rate control options."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetRateControlOptions(TUint aLayer, const TRateControlOptions& aOptions) = 0;

	/**
	Sets in-layer scalability options for a layer. In-layer bit-rate scalability refers to techniques where
	a specific part of a single-layer coded stream can be decoded correctly without decoding the leftover 
	part. For example, B-pictures can be used for this. By default in-layer scalability is not used.

	@param	"aLayer"			"Layer number. Layers are numbered [0…n-1], where n is the number of layers
								available. Use zero if layered bit-rate scalability is not used."
	@param	"aNumSteps"			"The number of in-layer scalability steps to use. Set to one to disable 
								in-layer scalability."
	@param	"aScalabilityType"	"The scalability type to use."
	@param	"aBitrateShare"		"Bit-rate share for each scalability step. The bit-rate shares are defined 
								as fractions of total layer bit-rate, with the share for one layer being 
								aBitrateShare[i]/sum(aBitrateShare). For example, to use 2/3 of the total
								bitrate for the first layer and the remaining 1/3 for the second, the array
								contents would be {2,1}."
	@param	"aPictureShare"		"Picture rate share for each scalability step. The picture rate shares are 
								defined similarly to the bit-rate shares. For example, a client wishing to
								use two B-pictures between each pair of reference pictures should set the 
								array contents to {1,2}."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported	- In-layer scalability is not supported.
				- KErrArgument		- Some of the arguments are out of range. For example, it is not
									  possible to use the specified in-layer scalability setup due to other 
									  constraints (such as the maximum picture order delay)."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetInLayerScalabilityL(TUint aLayer, 
										TUint aNumSteps,
										TInLayerScalabilityType aScalabilityType,
										const TArray<TUint>& aBitrateShare,
										const TArray<TUint>& aPictureShare) = 0;

	/**
	Sets the period for layer promotions points for a scalability layer. A layer promotion point is a 
	picture where it is possible to start decoding this enhancement layer if only the lower layers were 
	decoded earlier.

	@param	"aLayer"	"Layer number."
	@param	"aPeriod"	"Layer promotion point period. A value of one signals that each picture should be a
						layer promotion point, value two that there is one picture between each promotion
						point etc."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetLayerPromotionPointPeriod(TUint aLayer, TUint aPeriod) = 0;

	/**
	Returns coding-standard specific settings output from the encoder. The information can contain, for 
	example, some bitstream headers that can change based on settings modified while encoding is in progress.

	@return	"Coding-standard specific initialization output. The data format is coding-standard specific 
			and defined separately. The buffer is pushed to the cleanup stack, and the caller is responsible 
			for deallocating it."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual HBufC8* CodingStandardSpecificSettingsOutputLC() = 0;

	/**
	Returns implementation-specific settings output from the encoder. The information can contain, for
	example, some bitstream headers that can change based on settings modified while encoding is in progress.

	@return	"Implementation-specific initialization output. The data format is implementation-specific and 
			defined separately by the encoder supplier. The buffer is pushed to the cleanup stack, and the
			caller is responsible for deallocating it."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual HBufC8* ImplementationSpecificSettingsOutputLC() = 0;

	/**
	Requests the encoder to sends supplemental information in the bitstream. The information data format is 
	coding-standard dependent. Only one supplemental information send request can be active at a time. This
	variant encodes the information to the next possible picture.

	The client must be notified after then information has been sent by calling 
	MMMFDevVideoRecordProxy::MdvrpSupplementalInfoSent().

	@param	"aData"	"Supplemental information data to send."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - Supplemental information is not supported"
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SendSupplementalInfoL(const TDesC8& aData) = 0;

	/**
	Requests the encoder to sends supplemental information in the bitstream. The information data format is
	coding-standard dependent. Only one supplemental information send request can be active at a time. This
	variant encodes the information to the picture specified.

	@param	"aData"			"Supplemental information data to send."
	@param	"aTimestamp"	"Timestamp for the picture in which the supplemental information should be 
							included. If a picture with the matching timestamp is never encoded, or the
							timestamp is in the past, the supplemental information will not be sent."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - Supplemental information is not supported"
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SendSupplementalInfoL(const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp) = 0;

	/**
	Cancels the current supplemental information send request. The memory buffer reserved for supplemental 
	information data can be reused or deallocated after the method returns.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void CancelSupplementalInfo() = 0;

	/**
	Gets the current output buffer status. The information includes the number of free output buffers and 
	the total size of free buffers in bytes.

	@param	"aNumFreeBuffers"	"Target for the number of free output buffers."
	@param	"aTotalFreeBytes"	"Target for the total free buffer size in bytes."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetOutputBufferStatus(TUint& aNumFreeBuffers, TUint& aTotalFreeBytes) = 0;

	/**
	Returns a used output buffer back to the encoder. The buffer can be reused or deallocated.

	@param	"aBuffer"	"The buffer to return."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void ReturnBuffer(TVideoOutputBuffer* aBuffer) = 0;

	/**
	Indicates a picture loss to the encoder, without specifying the lost picture. The encoder can react 
	to this by transmitting an intra-picture.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void PictureLoss() = 0;

	/**
	Indicates to the encoder the pictures that have been lost. The encoder can react to this by
	transmitting an intra-picture.
	@param	"aPictures"	"Picture identifiers for the lost pictures."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void PictureLoss(const TArray<TPictureId>& aPictures) = 0;

	/**
	Indicates a slice loss to the encoder.

	@param	"aFirstMacroblock"	"The first lost macroblock. The macroblocks are numbered such
								that the macroblock in the upper left corner of the picture is considered 
								macroblock number 1 and the number for each macroblock increases from left
								to right and then from top to bottom in raster-scan order."
	@param	"aNumMacroblocks"	"The number of macroblocks in the lost slice."
	@param	"aPicture"			"The picture identified for the picture where the slice was lost. If the 
								picture is not known, aPicture.iIdType is set to ENone."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture) = 0;

	/**
	Sends a reference picture selection request to the encoder. The request is delivered as a 
	coding-standard specific binary message. Reference picture selection can be used to select a previous
	correctly transmitted picture to use as a reference in case later pictures have been lost.

	@param	"aSelectionData"	"The reference picture selection request message. The message format is 
								coding-standard specific, and defined separately."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void ReferencePictureSelection(const TDesC8& aSelectionData) = 0;

protected:
	/**
	Constructor.
	*/
	IMPORT_C CMMFVideoEncodeHwDevice();

	/**
	Set the proxy implementation to be used. Called just after the object is constructed.
	@param	"aProxy"	"The proxy to use."
	*/
	virtual void SetProxy(MMMFDevVideoRecordProxy& aProxy) = 0;
private:
	TUid iDtor_ID_Key;
	};


/**
CMMFVideoPreProcHwDevice is the MSL video pre-processor plug-in interface. All MSL video pre-processors 
must implement this interface.
@publishedAll
@released
*/
class CMMFVideoPreProcHwDevice : public CMMFVideoRecordHwDevice
	{
public:
	/**
	Creates a new video pre-processor hardware device object, based on the implementation UID.
	@param	"aUid"		"Pre-processor implementation UID."
	@param	"aProxy"	"The proxy implementation to use."
	@return	"A new CMMFVideoPreProcHwDevice object."
	@leave	"This method will leave if an error occurs."
	*/
	IMPORT_C static CMMFVideoPreProcHwDevice* NewL(TUid aUid, MMMFDevVideoRecordProxy& aProxy);

	/**
	Destructor.
	*/
	IMPORT_C virtual ~CMMFVideoPreProcHwDevice();

	/**
	Sets the device output format. The picture size depends on the input data format and possible 
	scaling performed.

	@param	"aFormat"	"The video format to use."
	@leave	" The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The format specified is not supported."
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetOutputFormatL(const TUncompressedVideoFormat& aFormat) = 0;

	/**
	Sets the video encoder device that will receive data from this pre-processor. Pre-processed pictures 
	will be written with WritePictureL() or through a custom interface, and the encoder will return used 
	pictures using ReturnPicture().

	@param	"aDevice"	"The output encoder device to use."
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetOutputDevice(CMMFVideoEncodeHwDevice* aDevice) = 0;

	/**
	Returns a used picture back to the pre-processor. Called by an encoder device when used as output 
	device from a pre-processor.

	@param	"aPicture"	"The picture to return."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void ReturnPicture(TVideoPicture* aPicture) = 0;

protected:
	/**
	Constructor.
	*/
	IMPORT_C CMMFVideoPreProcHwDevice();

	/**
	Set the proxy implementation to be used. Called just after the object is constructed.
	@param	"aProxy"	"The proxy to use."
	*/
	virtual void SetProxy(MMMFDevVideoRecordProxy& aProxy) = 0;
private:
	TUid iDtor_ID_Key;
	};

#endif
