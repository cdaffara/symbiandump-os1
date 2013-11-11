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

#ifndef __VIDEOPLAYHWDEVICE_H__
#define __VIDEOPLAYHWDEVICE_H__

#include <mmf/devvideo/devvideobase.h>
#include <mmf/devvideo/devvideoplay.h>
#include <mmf/common/mmfvideo.h>

/**
A base class for all video playback (decoder and post-processor) hardware devices. 
Since both decoders and post-processors can implement post-processing functionality, 
this class includes all post-processing related methods. The main difference between decoder 
and post-processor devices is that decoders can input coded data and write decoded pictures 
to another device, while post-processor devices can accept decoded pictures from the client 
or from another device.
@publishedAll
@released
*/
class CMMFVideoPlayHwDevice : public CMMFVideoHwDevice
	{
public:
	/**
	Retrieves post-processing information about this hardware device. 
	The device creates a CPostProcessorInfo structure, fills it with correct data, pushes it 
	to the cleanup stack and returns it. The client will delete the object when it is no 
	longer needed.

	@return "Post-processor information as a CPostProcessorInfo object. 
			The object is pushed to the cleanup stack, and must be deallocated by the caller."
	@leave  "This method may leave with one of the system-wide error codes.
	*/
	virtual CPostProcessorInfo* PostProcessorInfoLC() = 0;

	/**
	Retrieves the list of the output formats that the device supports. The list is ordered in 
	plug-in preference order, with the preferred formats at the beginning of the list. The list 
	can depend on the device source format, and therefore SetSourceFormatL() must be called before 
	calling this method.

	@param "aFormats" "An array for the result format list. The array must be created and destroyed by the caller."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void GetOutputFormatListL(RArray<TUncompressedVideoFormat>& aFormats) = 0;

	/**
	Sets the device output format.

	@param  "aFormat" "The format to use."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method may only be called before the hwdevice has been initialized using Initialize()."
	*/
	virtual void SetOutputFormatL(const TUncompressedVideoFormat &aFormat) = 0;

	/**
	Sets the clock source to use for video timing. If no clock source is set. video playback 
	will not be synchronized, but will proceed as fast as possible, depending on input data 
	and output buffer availability.	

	@param  "aClock" "The clock source to be used."
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetClockSource(MMMFClockSource* aClock) = 0;

	/**
	Sets the device video output destination. The destination can be the screen (using direct 
	screen access) or memory buffers. By default memory buffers are used. If data is written 
	to another device, this method is ignored, and suitable memory buffers are always used.
	
	@param  "aScreen" "True if video output destination is the screen, false if memory buffers."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetVideoDestScreenL(TBool aScreen) = 0;

	/**
	Sets the post-processing types to be used.

	@param  "aPostProcCombination" "The post-processing steps to perform, a bitwise OR of values from TPostProcessType."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetPostProcessTypesL(TUint32 aPostProcCombination) = 0;

	/**
	Sets post-processing options for input (pan-scan) cropping.

	@param  "aRect" "The cropping rectangle to use."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetInputCropOptionsL(const TRect& aRect) = 0;

	/**
	Sets post-processing options for YUV to RGB color space conversion. 
	Specifies the input YUV and output RGB formats to use explicitly. SetSourceFormatL(), 
	SetOutputFormatL(), and SetPostProcessTypesL() must be called before this method is used.

	@param  "aOptions"		"The conversion options to use."
	@param  "aYuvFormat"	"Conversion source YUV format"
	@param  "aRgbFormat"	"Conversion target RGB format"
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat) = 0;

	/**
	Sets post-processing options for YUV to RGB color space conversion.
	Uses the device input and output formats. For decoder devices the default YUV format used is 
	the format specified in the input bitstream. SetSourceFormatL(), SetOutputFormatL(), and 
	SetPostProcessTypesL() must be called before this method is used.

	@param  "aOptions"		"The conversion options to use."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions) = 0;

	/**
	Sets post-processing options for rotation. SetPostProcessTypesL() must be called before 
	this method is used.

	@param  "aRotationType" "The rotation to perform."	
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetRotateOptionsL(TRotationType aRotationType) = 0;

	/**
	Sets post-processing options for scaling. SetPostProcessTypesL() must be called before 
	this method is used.
	
	@param  "aTargetSize"			"Scaling target size. If a fixed scale factor size is used, 
									the new dimensions must be set to width=floor(factor*width), 
									height=floor(factor*height). For example, scaling a 
									QCIF (176x144) picture up by a factor of 4/3 yields a size 
									of 234x192."
	@param  "aAntiAliasFiltering"	"True if anti-aliasing filtering should be used. 
									If the post-processor does not support anti-aliased scaling, 
									or supports anti-aliased scaling only, this argument is ignored."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering) = 0;

	/**
	Sets post-processing options for output cropping. SetPostProcessTypesL() must be called before 
	this method is used.
	
	@param  "aRect" "Output cropping area."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetOutputCropOptionsL(const TRect& aRect) = 0;

	/**
	Sets post-processing plug-in specific options. SetPostProcessTypesL() must be called before 
	this method is used.

	@param  "aOptions" "The options. The format is plug-in specific."
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can be called either before or after the hwdevice has been initialized with Initialize().
			If called after initialization, the change must only be committed when CommitL() is called."
	*/
	virtual void SetPostProcSpecificOptionsL(const TDesC8& aOptions) = 0;

	/**
	Initializes the device. This method is asynchronous, the device will call 
	MMFVideoPlayProxy::MdvppInitializeComplete() after initialization has completed. After this 
	method has successfully completed, further configuration changes are not possible except where 
	separately noted.
	*/
	virtual void Initialize() = 0;

	/**
	Commit all changes since the last CommitL(), Revert() or Initialize()
	to the hardware device.  This only applies to methods which can be called both
	before AND after DevVideoPlay has been initialized.

	@see	SetPostProcessTypesL
	@see	SetInputCropOptionsL
	@see	SetYuvToRgbOptionsL
	@see	SetRotateOptionsL
	@see	SetScaleOptionsL
	@see	SetOutputCropOptionsL
	@see	SetPostProcSpecificOptionsL

	@leave  "The method will leave if an error occurs."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void CommitL() = 0;

	/**
	Revert all changes since the last CommitL(), Revert() or Initialize()
	back to their previous settings.  This only applies to methods which can 
	be called both before AND after DevVideoPlay has been initialized.

	@see	SetPostProcessTypesL
	@see	SetInputCropOptionsL
	@see	SetYuvToRgbOptionsL
	@see	SetRotateOptionsL
	@see	SetScaleOptionsL
	@see	SetOutputCropOptionsL
	@see	SetPostProcSpecificOptionsL

	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Revert() = 0;

	/**
	Starts writing output directly to the display frame buffer using Direct Screen Access.

	@param  "aVideoRect"	"The video output rectangle on screen."
	@param  "aScreenDevice"	"The screen device to use. The screen device object must be valid in the current thread."
	@param  "aClipRegion"	"Initial clipping region to use."
	
	@leave  "This method may leave with one of the system-wide error codes.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void StartDirectScreenAccessL(const TRect& aVideoRect, 
		CFbsScreenDevice& aScreenDevice, const TRegion& aClipRegion) = 0;

	/**
	Sets a new clipping region for Direct Screen Access. After the method returns, no video will 
	be drawn outside of the region. If clipping is not supported, or the clipping region is too 
	complex, either playback will pause or will resume without video display, depending on the 
	current setting of SetPauseOnClipFail(), and the result can be verified with IsPlaying(). 
	Clipping can be disabled by setting a new clipping region that includes the whole video window.

	@param  "aRegion" "The new clipping region. After the method returns, no video will be drawn outside the region."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetScreenClipRegion(const TRegion& aRegion) = 0;

	/**
	Sets whether the system should pause playback when it gets a clipping region it cannot handle, 
	or Direct Screen Access is aborted completely. If not, processing will proceed normally, but no 
	video will be drawn. By default, playback is paused.

	@param "aPause" "True if playback should be paused when clipping fails, false if not. 
					If playback is not paused, it will be continued without video display."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetPauseOnClipFail(TBool aPause) = 0;

	/**
	Aborts Direct Screen Access completely, to be called from MAbortDirectScreenAccess::AbortNow() 
	and similar methods. DSA can be resumed by calling StartDirectScreenAccessL().
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void AbortDirectScreenAccess() = 0;

	/**
	Indicates whether playback is proceeding. This method can be used to check whether playback was 
	paused or not in response to a new clipping region or DSA abort.

	@return "ETrue if video is still being played (even if not necessarily displayed)."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TBool IsPlaying() = 0;

	/**
	Re-draws the latest video picture. Only available when DSA is being used. If DSA is aborted or a 
	non-supported clipping region has been set, the request may be ignored.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Redraw() = 0;


	/**
	Starts video playback, including decoding, post-processing, and rendering. Playback will proceed 
	until it has been stopped or paused, or the end of the bitstream is reached.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Start() = 0;

	/**
	Stops video playback. No new pictures will be decoded, post-processed, or rendered.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Stop() = 0;

	/**
	Pauses video playback, including decoding, post-processing, and rendering. No pictures will be 
	decoded, post-processed, or rendered until playback has been resumed.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Pause() = 0;

	/**
	Resumes video playback after a pause.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void Resume() = 0;

	/**
	Changes to a new decoding and playback position, used for randomly accessing (seeking) the 
	input stream. The position change flushes all input and output buffers. Pre-decoder and 
	post-decoder buffering are handled as if a new bitstream was being decoded. If the device still has buffered 
	pictures that precede the new playback position, they will be discarded. If playback is 
	synchronized to a clock source, the client is responsible for setting the clock source to the 
	new position.
	
	@param "aPlaybackPosition" "The new playback position in the video stream."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition) = 0;

	/**
	Freezes a picture on the screen. After the picture has been frozen, no new pictures are 
	displayed until the freeze is released with ReleaseFreeze(). If the device output is being 
	written to memory buffers or to another plug-in, instead of the screen, no decoded pictures 
	will be delivered while the freeze is active, and they are simply discarded.

	@param "aTimestamp" "The presentation timestamp of the picture to freeze. The frozen picture 
						will be the first picture with a timestamp greater than or equal to this 
						parameter."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void FreezePicture(const TTimeIntervalMicroSeconds& aTimestamp) = 0;

	/**
	Releases a picture frozen with FreezePicture().

	@param "aTimestamp" "The presentation timestamp of the picture to release. The first picture 
						displayed after the release will be the first picture with a timestamp 
						greater than or equal to this parameter. To release the freeze immediately, 
						set the timestamp to zero."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void ReleaseFreeze(const TTimeIntervalMicroSeconds& aTimestamp) = 0;


	/**
	Returns the current playback position, i.e. the timestamp for the most recently displayed or 
	virtually displayed picture. If the device output is written to another device, the most recent 
	output picture is used.

	@return "Current playback position."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TTimeIntervalMicroSeconds PlaybackPosition() = 0;

	/**
	Returns the total amount of memory allocated for uncompressed pictures. This figure only 
	includes the pictures actually allocated by the plug-in itself, so that the total number of 
	bytes allocated in the system can be calculated by taking the sum of the values from all plug-ins.

	@return "Total number of bytes of memory allocated for uncompressed pictures."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TUint PictureBufferBytes() = 0;

	/**
	Reads various counters related to decoded pictures. The counters are reset when Initialize() 
	or this method is called, and thus they only include pictures processed since the last call.

	Post-processor devices return the number of input pictures in iPicturesDecoded and 
	iTotalPictures. If the decoded pictures are written to another plug-in, they are considered 
	to be "virtually displayed".

	@param "aCounters" "The counter structure to fill."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& aCounters) = 0;


	/**
	Sets the computational complexity level to use. If separate complexity levels are not available, 
	the method call is ignored. If the level specified is not available, the results are undefined. 
	Typically the device will either ignore the request or use the nearest suitable level.

	The complexity level can be changed at any point during playback.

	@param "aLevel" "The computational complexity level to use. Level zero (0) is the most complex 
					one, with the highest quality. Higher level numbers require less processing 
					and may have lower quality."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetComplexityLevel(TUint aLevel) = 0;

	/**
	Gets the number of complexity levels available.
	
	@return "The number of complexity control levels available, or zero if the information is not 
			available yet. The information may not be available if the number of levels depends on 
			the input data, and enough input data has not been read yet. In that case, using level 
			zero is safe."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TUint NumComplexityLevels() = 0;

	/**
	Gets information about a computational complexity level. This method can be called after 
	NumComplexityLevels() has returned a non-zero value - at that point the information is guaranteed 
	to be available. Some hardware device implementations may not be able to provide all values, 
	in that case the values will be approximated.

	@param "aLevel"	"The computational complexity level to query. The level numbers range from zero 
					(the most complex) to NumComplexityLevels()-1."
	@param "aInfo"	"The information structure to fill."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetComplexityLevelInfo(TUint aLevel, CMMFDevVideoPlay::TComplexityLevelInfo& aInfo) = 0;

	/**
	Returns a picture back to the device. This method is called by CMMFDevVideoPlay to return pictures 
	from the client (after they have been written with NewPicture()), or by the output device when 
	it has finished using a picture.

	@param "aPicture" "The picture to return. The device can re-use the memory for the picture."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void ReturnPicture(TVideoPicture* aPicture) = 0;

	/**
	Gets a copy of the latest picture sent to output.

	@param "aPictureData"	"Target picture. The memory for the picture must be allocated by the 
							caller, and initialized properly. The data formats must match the snapshot 
							format requested."
	@param "aFormat"		"The picture format to use for the snapshot."

	@return "ETrue if the snapshot was taken, EFalse if a picture is not available. The picture may not 
			be available if decoding has not progressed far enough yet."

	@leave	"The method will leave if an error occurs. Typical error codes used:
			* KErrNotSupported - The requested data format or picture size is not supported, or the 
			plug-in does not support snapshots."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TBool GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat) = 0;

	/**
	When the snapshot is available, it will be returned to the client using the TimedSnapshotComplete()
	callback. To cancel a timed snapshot request, use CancelTimedSnapshot(). Only one timed snapshot 
	request can be active at a time.

	@param "aPictureData"			"Target picture. The memory for the picture must be allocated by 
									the caller, and initialized properly. The data formats must match 
									the snapshot format requested. The picture must remain valid until 
									the snapshot has been taken or until the request has been cancelled 
									with CancelTimedSnapshot()."
	@param "aFormat"				"The picture format to use for the snapshot."
	@param "aPresentationTimestamp"	"Presentation timestamp for the picture to copy."

	@leave	"The method will leave if an error occurs. Typical error codes used:
			* KErrNotSupported - The requested data format or picture size is not supported or 
			the plug-in does not support timed snapshots."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, 
		const TTimeIntervalMicroSeconds& aPresentationTimestamp) = 0;

	/**
	When the snapshot is available, it will be returned to the client using the TimedSnapshotComplete()
	callback. To cancel a timed snapshot request, use CancelTimedSnapshot(). Only one timed snapshot 
	request can be active at a time.

	@param "aPictureData"			"Target picture. The memory for the picture must be allocated by 
									the caller, and initialized properly. The data formats must match 
									the snapshot format requested. The picture must remain valid until 
									the snapshot has been taken or until the request has been cancelled 
									with CancelTimedSnapshot()."
	@param "aFormat"				"The picture format to use for the snapshot."
	@param "aPictureId"				"Picture identifier for the picture to copy."

	@leave	"The method will leave if an error occurs. Typical error codes used:
			* KErrNotSupported - The requested data format or picture size is not supported or 
			the plug-in does not support timed snapshots."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, 
		const TPictureId& aPictureId) = 0;

	/**
	Cancels a timed snapshot request.
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void CancelTimedSnapshot() = 0;

	/**
	Gets a list of the supported snapshot picture formats.

	@param "aFormats" "An array for the result format list. The array must be created and destroyed by 
	the caller."

	@leave "This method may leave with one of the standard error codes."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& aFormats) = 0;


	/**

	Notifies the hardware device that the end of input data has been reached and no more input data 
	will be written. The hardware device can use this signal to ensure that the remaining data gets 
	processed, without waiting for new data. For example when the data type is not EDuCodedPicture,
	calling this method is necessary otherwise a hardware device implementation might be looking for 
	the start code for the next picture to ensure it has a complete picture before starting to decode
	the previous one. 
	
	  
	After the remaining data has been processed (and displayed, if applicable), the hardware 
	device must notify the proxy with the MdvppStreamEnd() callback.

	DevVideo clients are encouraged to call this method, but its use is not mandatory for synchronized
	processing.  For synchronized playback, all video pictures are processed or discarded according to 
	their timestamps, and so the client can easily infer when processing is complete.  However, it 
	should be noted that the last picture might not be displayed if this method is not called and the 
	input data type is not EDuCodedPicture.

	For non-synchronized playback (e.g. file conversion), a client must call this method otherwise it
	will never find out when the hardware device has finished processing the data.
	
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void InputEnd() = 0;
	};




/**
CMMFVideoDecodeHwDevice is the MSL video decoder hardware device interface. 
All video decoders must implement this interface.
@publishedAll
@released
*/
class CMMFVideoDecodeHwDevice : public CMMFVideoPlayHwDevice
	{
public:
	/**
	Creates a new video decoder hardware device object, based on the implementation UID.

	@param	"aUid"		"Decoder implementation UID."
	@param	"aProxy"	"The proxy implementation to use."
	@return	"A new CMMFVideoDecodeHwDevice object."
	@leave	"This method may leave with one of the system-wide error codes.
	*/
	IMPORT_C static CMMFVideoDecodeHwDevice* NewL(TUid aUid, MMMFDevVideoPlayProxy& aProxy);
	
	/**
	Creates a new video decoder hardware device adapter object, based on the Implementation Information of a Processing Unit.

	@param	"aImplInfo"	"The registration data relating to the Interface Implementation of the Processing Unit."
	@param	"aProxy"	"The proxy implementation to use."
	@return	"A new CMMFVideoDecodeHwDevice object."
	@leave	"This method will leave if an error occurs."
	*/
	IMPORT_C static CMMFVideoDecodeHwDevice* NewPuAdapterL(const CImplementationInformation& aImplInfo, MMMFDevVideoPlayProxy& aProxy);


	/**
	Destructor.
	*/
	IMPORT_C virtual ~CMMFVideoDecodeHwDevice();


	/**
	Retrieves decoder information about this hardware device. The device creates a CVideoDecoderInfo 
	structure, fills it with correct data, pushes it to the cleanup stack and returns it. The client 
	will delete the object when it is no longer needed.
	
	@return	"Decoder information as a CVideoDecoderInfo object. The object is pushed to the cleanup 
			stack, and must be deallocated by the caller."
	@leave	"This method may leave with one of the system-wide error codes."

	@see CVideoDecoderInfo
	*/
	virtual CVideoDecoderInfo* VideoDecoderInfoLC() = 0;

	/**
	Reads header information from a coded data unit.
	@param	"aDataUnitType"		"The type of the coded data unit that is contained in aDataUnit. If the data is a 
								simple piece of bitstream, use EDuArbitraryStreamSection."
	@param	"aEncapsulation"	"The encapsulation type used for the coded data. If the data is a 
								simple piece of bitstream, use EDuElementaryStream."
	@param	"aDataUnit"			"The coded data unit, contained in a TVideoInputBuffer."
	@return "Header information for the data unit, or NULL if the coded data unit did not contain 
			enough data to parse the header. The header data must be returned to the device using 
			ReturnHeader() before Initialize() is called or the decoder is destroyed. The data remains 
			valid until it is returned."
	@leave	"The method will leave if an error occurs. Running out of data is not considered an error, 
			as described above. Typical error codes used:
				* KErrNotSupported - The data is not in a supported format.
				* KErrCorrupt - The data appears to be in a supported format, but is corrupted."
	*/
	virtual TVideoPictureHeader* GetHeaderInformationL(TVideoDataUnitType aDataUnitType, 
		TVideoDataUnitEncapsulation aEncapsulation, TVideoInputBuffer* aDataUnit) = 0;


	/**
	Returns a header from GetHeaderInformationL() back to the decoder so that the memory can be freed.

	@param  "aHeader" "The header to return."
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void ReturnHeader(TVideoPictureHeader* aHeader) = 0;

	/**
	Sets the device input format to a compressed video format.

	@param	"aFormat"			"The input format to use."
	@param	"aDataUnitType"		"The data unit type for input data."
	@param	"aEncapsulation"	"The encapsulation type used for the coded data."
	@param	"aDataInOrder"		"ETrue if the input data is written in correct decoding order, 
								 EFalse if will be written in arbitrary order."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The source format is not supported."

	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetInputFormatL(const CCompressedVideoFormat& aFormat, 
		TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, 
		TBool aDataInOrder) = 0;

	/**
	Sets whether decoding should be synchronized to the current clock source, if any, or if pictures 
	should instead be decoded as soon as possible. If decoding is synchronized, decoding timestamps 
	are used if available, presentation timestamps are used if not. When decoding is not synchronized, 
	pictures are decoded as soon as source data is available for them, and the decoder has a free 
	output buffer. If a clock source is not available, decoding will not be synchronized.

	@param "aSynchronize" "True if decoding should be synchronized to a clock source."
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SynchronizeDecoding(TBool aSynchronize) = 0;

	/**
	Sets decoder buffering options. See [3] for a description of the options available.
	
	@param "aOptions" "Buffering options."
	@leave "The method will leave if an error occurs. Typical error codes used:
			* KErrNotSupported - The specified buffering options are not supported. 
			  If the client receives this error code, it can call GetBufferOptions() 
			  to determine the options the decoder is able to support."

	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetBufferOptionsL(const CMMFDevVideoPlay::TBufferOptions& aOptions) = 0;

	/**
	Gets the video decoder buffer options actually in use. This can be used before calling 
	SetBufferOptions() to determine the default options, or afterwards to check the values 
	actually in use (if some default values were used).

	@param "aOptions" "Buffering options structure to fill."
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetBufferOptions(CMMFDevVideoPlay::TBufferOptions& aOptions) = 0;

	/**
	Indicates which HRD/VBV specification is fulfilled in the input stream and any related parameters.

	@param "aHrdVbvSpec"	"The HRD/VBV specification fulfilled."
	@param "aHrdVbvParams"	"HRD/VBV parameters. The data format depends on the parameters chosen. 
							For 3GPP TS 26.234 parameters (aHrdVbvSpec=EHrdVbv3GPP), the data in the 
							descriptor is a package of type TPckC<T3gppHrdVbvParams> 
							(see T3gppHrdVbvParams). If no HRD/VBV parameters are used, the descriptor 
							is zero length."
	@see THrdVbvSpecification
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetHrdVbvSpec(THrdVbvSpecification aHrdVbvSpec, const TDesC8& aHrdVbvParams) = 0;

	/**
	Sets the output post-processor device to use. If an output device is set, all decoded pictures 
	are delivered to that device, and not drawn on screen or returned to the client. Pictures are 
	written using CMMDVideoPostProcDevice::WritePictureL() or a custom interface after they have been 
	decoded. The post-processor must then synchronize rendering to the clock source if necessary.

	@param "aDevice" "The output post-processor device to use."
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetOutputDevice(CMMFVideoPostProcHwDevice* aDevice) = 0;

	/**
	Returns the current decoding position, i.e. the timestamp for the most recently decoded picture.

	@return "Current decoding position."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TTimeIntervalMicroSeconds DecodingPosition() = 0;

	/**
	Returns the current pre-decoder buffer size.

	@return "The number of bytes of data in the pre-decoder buffer."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TUint PreDecoderBufferBytes() = 0;

	/**
	Reads various counters related to the received input bitstream and coded data units. The counters 
	are reset when Initialize() or this method is called, and thus they only include data processed 
	since the last call.

	@param "aCounters" "The counter structure to fill."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void GetBitstreamCounters(CMMFDevVideoPlay::TBitstreamCounters& aCounters) = 0;

	/**
	Retrieves the number of free input buffers the decoder has available.

	@return "Number of free input buffers the decoder has available."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual TUint NumFreeBuffers() = 0;

	/**
	Retrieves an empty video input buffer from the decoder. After input data has been written to the 
	buffer, it can be written to the decoder using WriteCodedDataL(). The number of buffers the decoder
	must be able to provide before expecting any back, and the maximum size for each buffer, are 
	specified in the buffer options.

	The decoder maintains ownership of the buffers even while they have been retrieved by the client, 
	and will take care of deallocating them.

	@param	"aBufferSize"	"Required buffer size, in bytes. The resulting buffer can be larger than 
							this, but not smaller."
	@return "A new input data buffer. The buffer is at least as large as requested, but it may be 
			larger. If no free buffers are available, the return value is NULL."
	@leave  "The method will leave if an error occurs. Lack of free buffers is not considered an error."
	*/
	virtual TVideoInputBuffer* GetBufferL(TUint aBufferSize) = 0;

	/**
	Writes a piece of coded video data to the decoder. The data buffer must be retrieved from the 
	decoder with GetBufferL().

	@param "aBuffer" "The coded data unit to write."
	@leave "This method may leave with one of the system-wide error codes."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void WriteCodedDataL(TVideoInputBuffer* aBuffer) = 0;
	
	/**
	Configures the Decoder using header information known by the client.
	@param	"aVideoPictureHeader"	"Header information to configure the decoder with"
	@leave	"The method will leave if an error occurs. Running out of data is not considered an error, 
			as described above.
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void ConfigureDecoderL(const TVideoPictureHeader& aVideoPictureHeader);
		


protected:
	/**
	Constructor.
	*/
	IMPORT_C CMMFVideoDecodeHwDevice();

	/**
	Set the proxy implementation to be used. Called just after the object is constructed.
	@param	"aProxy"	"The proxy to use."
	*/
	virtual void SetProxy(MMMFDevVideoPlayProxy& aProxy) = 0;
private:
	TUid iDtor_ID_Key;
	};


/**
CMMFVideoPostProcHwDevice is the MSL video post-processor plug-in interface. All MSL video 
post-processors must implement this interface.
@publishedAll
@released
*/
class CMMFVideoPostProcHwDevice : public CMMFVideoPlayHwDevice
	{
public:
	/**
	Creates a new video post-processor hardware device object, based on the implementation UID.

	@param	"aUid"		"Post-processor implementation UID."
	@param	"aProxy"	"The proxy implementation to use."
	@return	"A new CMMFVideoPostProcHwDevice object."
	@leave	"This method may leave with one of the system-wide error codes."
	*/
	IMPORT_C static CMMFVideoPostProcHwDevice* NewL(TUid aUid, MMMFDevVideoPlayProxy& aProxy);

	/**
	Destructor.
	*/
	IMPORT_C virtual ~CMMFVideoPostProcHwDevice();

	/**
	Sets the device input format to an uncompressed video format.

	@param	"aFormat"	"The input format to use."
	@leave	"The method will leave if an error occurs. Typical error codes used:
			* KErrNotSupported - The input format is not supported."	
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetInputFormatL(const TUncompressedVideoFormat& aFormat) = 0;

	/**
	Sets the decoder device that will write data to this post-processor. Decoded pictures will be 
	written with WritePictureL() or through a custom interface. After pictures have been processed, 
	they must be returned to the decoder using ReturnPicture().

	@param	"aDevice"	"The decoder source plug-in to use."
	@pre	"This method can only be called before the hwdevice has been initialized with Initialize()."
	*/
	virtual void SetInputDevice(CMMFVideoDecodeHwDevice* aDevice) = 0;

	/**
	Writes an uncompressed video picture to the post-processor. The picture must be returned to the 
	client or source plug-in after it has been used.

	@param	"aPicture"	"The picture to write."
	@leave	"This method may leave with one of the system-wide error codes."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void WritePictureL(TVideoPicture* aPicture) = 0;
protected:
	/**
	Constructor.
	*/
	IMPORT_C CMMFVideoPostProcHwDevice();

	/**
	Set the proxy implementation to be used. Called just after the object is constructed.
	@param	"aProxy"	"The proxy to use."
	*/
	virtual void SetProxy(MMMFDevVideoPlayProxy& aProxy) = 0;
private:
	TUid iDtor_ID_Key;
	};



/**
A custom interface extending the functionality of CMMFVideoPlayHwDevice, adding support for the decoder to handle the 
copying of the bitstream data into the buffers, combining this with a scan of the data and support for the passing of 
information from the client to the decoder describing what part of a frame the data contains.
@publishedAll
@released
*/
class MMMFVideoPlayHwDeviceExtensionScanCopy
	{
public:
	/**
	Writes a piece of coded video data to the decoder. The data buffer must be retrieved from the
	decoder with GetBufferL().

	@param	"aBuffer"	"The coded data unit to write."
	@param	"aPortion"	"The portion of the frame that the data contains. Defaults to EFramePortionUnknown."
	@leave	"This method may leave with one of the system-wide error codes."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void WriteCodedDataL(TVideoInputBuffer* aBuffer, TFramePortion aPortion = EFramePortionUnknown) = 0;
	/**
	Passes a pointer to a piece of coded video data to the decoder. The data buffer, which is left empty by the client,
	must be retrieved from the decoder with GetBufferL().

	@param	"aCodedData"	"A pointer to the coded data unit to scan and copy."
	@param	"aBuffer"		"The empty data buffer."
	@param	"aPortion"		"The portion of the frame that the data contains. Defaults to EFramePortionUnknown."
	@leave	"This method may leave with one of the system-wide error codes."
	@pre	"This method can only be called after the hwdevice has been initialized with Initialize()."
	*/
	virtual void ScanAndCopyCodedDataL(TPtr8 aCodedData, TVideoInputBuffer* aBuffer, TInt& aConsumed, TFramePortion aPortion = EFramePortionUnknown) = 0;
	};

inline void CMMFVideoDecodeHwDevice::ConfigureDecoderL(const TVideoPictureHeader& /*aVideoPictureHeader*/) 
	{
	User::Leave(KErrNotSupported);
	}
		


#endif
