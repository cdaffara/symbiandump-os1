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

#ifndef __DEVVIDEORECORD_H__
#define __DEVVIDEORECORD_H__

#include <e32std.h>
#include <mmf/devvideo/devvideobase.h>

class MMMFDevVideoRecordObserver;
class CMMFVideoRecordHwDevice;
class CMMFVideoEncodeHwDevice;
class CMMFVideoPreProcHwDevice;
class TVideoOutputBuffer;

/**
MMMFDevVideoRecordProxy is the interface the CDevVideoRecord implementation provides for video recording 
hardware devices. The hardware devices use this interface to report events and send used pictures and 
new buffers to the client.
@publishedAll
@released
*/
class MMMFDevVideoRecordProxy
	{
public:
	/**
	Delivers a new coded data unit to the client. The CDevVideoRecord implementation will maintain a list 
	of buffers and implement NumDataBuffers() and NextBufferL() based on those. The buffers will be returned
	back to the device using ReturnBuffer().

	@param	"aBuffer"	"The buffer containing the data to send."
	*/
	virtual void MdvrpNewBuffer(TVideoOutputBuffer* aBuffer) = 0;

	/**
	Returns a used input picture back to the client. Called by the encoder hardware device after the picture
	has been encoded.

	@param	"aPicture"	"The picture to return."
	*/
	virtual void MdvrpReturnPicture(TVideoPicture* aPicture) = 0;

	/**
	Sends a notification to the client that the current supplemental info send request has completed.
	*/
	virtual void MdvrpSupplementalInfoSent() = 0;

	/**
	Reports a fatal error to the client. The device must automatically stop processing video data when 
	such errors occur, and may not do further processing before it has been deleted and re-created.

	@param	"aDevice"	"The device that reports the error."
	@param	"aError"	"The error code."
	*/
	virtual void MdvrpFatalError(CMMFVideoHwDevice* aDevice, TInt aError) = 0;

	/**
	Reports that an asynchronous Initialize() method has completed. The device is now ready for recording.

	@param	"aDevice"	"The device that was initialized."
	@param	"aError"	"Initialization result error code, KErrNone if initialization was successful."
	*/
	virtual void MdvrpInitializeComplete(CMMFVideoHwDevice* aDevice, TInt aError) = 0;

	/**
	Reports that the input video data end has been reached and all pictures have been processed. Called 
	by each hardware device after their InputEnd() methods have been called and all data has been processed.
	The proxy implementation will notify the client about stream end when all hardware devices have called 
	this method.
	*/
	virtual void MdvrpStreamEnd() = 0;
	};


/**
A video output buffer for a single output coded data unit from the encoder. In practice the encoder is 
likely to inherit its own buffer class from this class, adding its own internal bookkeeping data.
@publishedAll
@released
*/
class TVideoOutputBuffer
	{
public:
	/**
	Coded video data for this data unit.
	*/
	TPtrC8 iData;

	/**
	Capture timestamp for this data unit. If a single picture is divided into multiple data units, 
	all data units have the same timestamp.
	*/
	TTimeIntervalMicroSeconds iCaptureTimestamp;

	/**
	Identification number indicating the start of the spatial coverage of the data in the output buffer, 
	for example the first macroblock number. 
	*/
	TUint iCoverageStartPosition;

	/**
	End of the spatial coverage of the data in the buffer, for example the last macroblock number. 
	The idenfication numbers can be used to match data partitions with each other.
	*/
	TUint iCoverageEndPosition;

	/**
	Order number of the output buffer having the same spatial coverage as an earlier output buffer 
	of the same picture. Used with video pictures or segments that do not fit into an output buffer. 
	The order number of the first output buffer of a particular spatial coverage area is 0. The order 
	number shall be incremented by 1 for each subsequent output buffer having the same coverage in the 
	same coded picture.
	*/
	TUint iOrderNumber;

	/**
	Lowest unequal error protection level present in the buffer. Level 0 indicates that the buffer can 
	be discarded without causing temporal error propagation in the decoder, higher values indicate more 
	important buffers. If unequal error protection is not used, the value is set to 1.
	*/
	TUint iMinErrorProtectionLevel;

	/**
	Highest unequal error protection level present in the buffer. If unequal error protection is not 
	used, the value is set to 1.
	*/
	TUint iMaxErrorProtectionLevel;

	/**
	True if this buffer contains data that is required for decoding several pictures, for example the 
	only copy of a sequence header. A picture needed for motion compensated prediction is not considered
	"required for decoding several pictures".
	*/
	TBool iRequiredSeveralPictures;

	/**
	True if this buffer contains data that is required for decoding other parts of the same picture, 
	for example the only copy of the picture header.
	*/
	TBool iRequiredThisPicture;

	/**
	The layer number for this picture if layered bit-rate scalability is used. If layers are not used, 
	the layer number is always zero.
	*/
	TUint iLayer;

	/**
	Sub-sequence identifier of the output picture. A sub-sequence is a set of coded pictures within a 
	bit-rate scalability layer. A picture shall reside in one scalability layer and in one sub-sequence 
	only. A sub-sequence shall not depend on any other sub-sequence in the same or in a higher layer. 
	A sub-sequence in layer 0 can be decoded independently of any other sub-sequences. A sub-sequence 
	identifier labels the sub-sequence within a layer. Consecutive sub-sequences within a particular 
	layer in decoding order shall have a different sub-sequence identifier from each other. See Annex D 
	of H.264 | MPEG-4 AVC for further details.
	*/
	TUint iSubSeqId;
	
	/**
	The in-layer scalability step for the data in the buffer if in-layer scalability is used. 
	If in-layer scalability is not in use, the step is set to zero.
	*/
	TUint iInLayerScalabilityStep;

	/**
	iDataPartitionNumber equal to 0 indicates that data partitioning is not in use. Values of 
	iDataPartitionNumber greater than 0 indicate the data partition number in descending order of 
	importance, i.e. data partition 1 is the most important data partition subjectively. Data 
	partitioning can be used with multiple unequal error protection levels.
	*/
	TUint iDataPartitionNumber;

	/**
	True if this buffer contains data for a random access point. 
	*/
	TBool iRandomAccessPoint;

	/**
	Updated HRD/VBV parameters, valid if HRD/VBV is in use and iRandomAccessPoint is true.
	*/
	TPtrC8 iHrdVbvParams;

	/**
	Coding-standard specific data.
	*/
	TPtrC8 iCodingStandardSpecificData;

	/**
	Implementation-specific data.
	*/
	TPtrC8 iImplementationSpecificData;

	/**
	A queue link used internally by the MSL API. The field must not be modified while the buffer is 
	in the MSL API, but can be used by the client before the buffer has been written and after the 
	buffer has been returned.
	*/
	TDblQueLink iLink;
	};



/**
This class contains information about the pre-processing capabilities that an encoder or a pre-processor 
hardware has. Although it mainly contains static data, it is defined as a complete CBase-derived class 
since the data is relatively complex and proper memory management is necessary.

The objects are created by the pre-processor or encoder devices, and used by the MSL video client code.

@publishedAll
@released
*/
class CPreProcessorInfo : public CBase
	{
public:
	/**
	Creates and returns a new CPreProcessorInfo object. All data passed into this method is copied.
	@param "aUid"							"The uid of the pre-processor."
	@param "aManufacturer"					"The manufacturer of the pre-processor."
	@param "aIdentifier"					"The manufacturer-specific identifier of the pre-processor."
	@param "aVersion"						"The version of the pre-processor."
	@param "aAccelerated"					"Whether the pre-processor is hw accelerated or not."
	@param "aSupportsDirectCapture"			"Whether the pre-processor supports direct capture."
	@param "aInputFormats"					"An array of the supported input formats."
	@param "aOutputFormats"					"An array of the supported output formats."
	@param "aSupportedCombinations"			"An array of the supported combinations."
	@param "aSupportsArbitraryScaling"		"Whether the pre-processor supports arbitrary scaling."
	@param "aSupportsAntiAliasedScaling"	"Whether the pre-processor supports anti-alias filtering on scaling.""
	@param "aSupportedScaleFactors"			"An array of the supported scale factors if arbitrary scaling isn't suported"
	@param "aYuvToYuvCapabilities"			"The yuv to yuv conversion capabilities of the pre-processor."
	@param "aSupportedRgbRanges"			"The supported rgb ranges."
	@param "aSupportedRotations"			"The supported rotations."
	@param "aImplementationSpecificInfo"	"Implementation-specific information."
	@return"The newly created CPreProcessorInfo object."
	@leave "This method may leave with one of the system-wide error codes.
	*/
	IMPORT_C static CPreProcessorInfo* NewL(TUid aUid,
											const TDesC& aManufacturer,
											const TDesC& aIdentifier,
											TVersion aVersion,
											TBool aAccelerated,
											TBool aSupportsDirectCapture,
											const TArray<TUncompressedVideoFormat>& aInputFormats,
											const TArray<TUncompressedVideoFormat>& aOutputFormats,
											const TArray<TUint32>& aSupportedCombinations,
											TBool aSupportsArbitraryScaling,
											TBool aSupportsAntiAliasedScaling,
											const TArray<TScaleFactor>& aSupportedScaleFactors,
											const TYuvToYuvCapabilities& aYuvToYuvCapabilities,
											TUint32 aSupportedRgbRanges,
											TUint32 aSupportedRotations,
											const TDesC8& aImplementationSpecificInfo);

	/**
	Destructor.
	*/
	IMPORT_C ~CPreProcessorInfo();

	/**
	Returns the pre-processor UID.
	@return "Pre-processor UID."
	*/
	IMPORT_C TUid Uid() const;

	/**
	Returns the hardware device manufacturer.
	@return "The hardware device manufacturer. The reference is valid until this is destroyed."
	*/
	IMPORT_C const TDesC& Manufacturer() const;

	/**
	Returns the hardware device manufacturer-specific identifier. The combination of the manufacturer 
	and identifier uniquely identifies the plug-in.
	@return "The hardware device identifier. The reference is valid until this object is destroyed."
	*/
	IMPORT_C const TDesC& Identifier() const;

	/**
	Returns the pre-processor version.
	@return "Pre-processor version."
	*/
	IMPORT_C TVersion Version() const;

	/**
	Returns whether the plug-in is hardware-accelerated. Hardware-accelerated pre-processors can run on 
	an application DSP or dedicated hardware.
	@return "True if the pre-processor is hardware-accelerated." 
	*/
	IMPORT_C TBool Accelerated() const;

	/**
	Returns whether the hardware device supports direct capture. Pre-processors supporting direct capture 
	can get the input pictures directly from a camera, possibly using an efficient hardware-dependent data 
	path.
	@return "True if the pre-processor supports direct capture."
	*/
	IMPORT_C TBool SupportsDirectCapture() const;

	/**
	Returns whether the pre-processor supports the given input format.
	@param	"aFormat"	"The format to be checked."
	@return "True if the pre-processor supports the given input format."
	*/
	IMPORT_C TBool SupportsInputFormat(const TUncompressedVideoFormat& aFormat) const;

	/**
	Returns the input formats that the pre-processor supports.
	@return "An RArray table of supported video formats (TUncompressedVideoFormat). The reference is valid 
			until the CPreProcessorInfo object is destroyed."
	*/
	IMPORT_C const RArray<TUncompressedVideoFormat>& SupportedInputFormats() const;

	/**
	Returns whether the pre-processor supports the given output format.
	@param	"aFormat"	"The format to be checked."
	@return "True if the pre-processor supports the given input format."
	*/
	IMPORT_C TBool SupportsOutputFormat(const TUncompressedVideoFormat& aFormat) const;

	/**
	Returns the output formats that the pre-processor supports.
	@return "An RArray table of supported video formats (TUncompressedVideoFormat). The reference is valid 
			until the CPreProcessorInfo object is destroyed."
	*/
	IMPORT_C const RArray<TUncompressedVideoFormat>& SupportedOutputFormats() const;

	/**
	Returns whether the pre-processor supports the given pre-processing combination.
	@param	"aCombination"	"Pre-processing combination, a bitwise OR of values from TPrePostProcessType."
	@return "True if the pre-processing combination is supported."
	*/
	IMPORT_C TBool SupportsCombination(TUint32 aCombination) const;

	/**
	Lists all supported pre-processing combinations.
	@return "An RArray table of pre-processing combinations. Each value is a bitwise OR of values from 
			TPrePostProcessType. The reference is valid until the CPreProcessorInfo object is destroyed."
	*/
	IMPORT_C const RArray<TUint32>& SupportedCombinations() const;

	/**
	Returns whether the pre-processor supports arbitrary scaling.
	@return "True if arbitrary scaling is supported. If arbitrary scaling is not supported, some specific 
			scale factors can still be available."
	*/
	IMPORT_C TBool SupportsArbitraryScaling() const;

	/**
	Returns whether the hardware device supports anti-aliasing filtering for scaling.
	@return "True if anti-aliasing filtering is supported."
	*/
	IMPORT_C TBool SupportsAntiAliasedScaling() const;

	/**
	Returns the scaling factors the plug-in supports. If the plug-in supports arbitrary scaling the list 
	is empty - use SupportsArbitraryScaling() first.
	@return "An RArray list of supported scale factors (TScaleFactor). The reference is valid until the 
			CPreProcessorInfo object is destroyed. If the pre-processor supports arbitrary scaling or 
			no scaling at all, the list is empty."
	*/
	IMPORT_C const RArray<TScaleFactor>& SupportedScaleFactors() const;

	/**
	Returns the YUV to YUV conversion capabilities for the pre-processor.
	@return "The conversion capabilities, as a TYuvToYuvCapabilities structure. The reference is valid 
			until the CPreProcessorInfo object is destroyed."
	*/
	IMPORT_C const TYuvToYuvCapabilities& YuvToYuvCapabilities() const;

	/**
	Returns RGB value ranges the hardware device supports for RGB to YUV conversion.
	@return "The supported RGB ranges as a bitwise OR of TRgbRange values."
	*/
	IMPORT_C TUint32 SupportedRgbRanges() const;

	/**
	Returns the rotation types the plug-in supports.
	@return "The supported rotation types as a bitwise OR of TRotationType values. If the pre-processor 
			does not support rotation, the return value is zero."
	*/
	IMPORT_C TUint32 SupportedRotations() const;

	/**
	Returns implementation-specific information about the pre-processor.
	@return "Implementation- specific information about the pre-processor. The data format is 
			implementation-specific, and defined separately by the pre-processor supplier. The reference 
			is valid until the CPreProcessorInfo object is destroyed."
	*/
	IMPORT_C const TDesC8& ImplementationSpecificInfo() const;
private:
	CPreProcessorInfo(TUid aUid,
					  TVersion aVersion,
					  TBool aAccelerated,
					  TBool aSupportsDirectCapture,
					  TBool aSupportsArbitraryScaling,
					  TBool aSupportsAntiAliasedScaling,
					  const TYuvToYuvCapabilities& aYuvToYuvCapabilities,
					  TUint32 aSupportedRgbRanges,
					  TUint32 aSupportedRotations);

	void ConstructL(const TDesC& aManufacturer,
					const TDesC& aIdentifier,
					const TArray<TUncompressedVideoFormat>& aInputFormats,
					const TArray<TUncompressedVideoFormat>& aOutputFormats,
					const TArray<TUint32>& aSupportedCombinations,
					const TArray<TScaleFactor>& aSupportedScaleFactors,
					const TDesC8& aImplementationSpecificInfo);
private:
	TUid iUid;
	HBufC* iManufacturer;
	HBufC* iIdentifier;
	TVersion iVersion;
	TBool iAccelerated;
	TBool iSupportsDirectCapture;
	RArray<TUncompressedVideoFormat> iInputFormats;
	RArray<TUncompressedVideoFormat> iOutputFormats;
	RArray<TUint32> iSupportedCombinations;
	TBool iSupportsArbitraryScaling;
	TBool iSupportsAntiAliasedScaling;
	RArray<TScaleFactor> iSupportedScaleFactors;
	TYuvToYuvCapabilities iYuvToYuvCapabilities;
	TUint32 iSupportedRgbRanges;
	TUint32 iSupportedRotations;
	HBufC8* iImplementationSpecificInfo;
	};

/**
This class contains information about a video encoder hardware device and its capabilities. 
Although it mainly contains static data, it is defined as a complete CBase-derived class since the
data is relatively complex and proper memory management is necessary.

The objects are created by the encoder devices, and used by the MSL video client code.

@publishedAll
@released
*/
class CVideoEncoderInfo : public CBase
	{
public:
	/**
	Creates and returns a new CVideoEncoderInfo object. 
	All data passed in is copied on construction of the object.

	@param  aUid
	        The uid of the encoder.
	@param  aManufacturer
	        The video encoder manufacturer.
	@param  aIdentifier
	        The manufacturer-specific identifier for this encoder.
	@param  aVersion
	        The version of this encoder.
	@param  aAccelerated
	        Whether this encoder is accelerated.
	@param  aSupportsDirectCapture
	        Whether this encoder supports direct capture.
	@param  aSupportedInputFormats
	        An array of the supported input formats.
	@param  aSupportedOutputFormats
	        An array of the supported output formats.
	@param  aMaxPictureSize
	        The maximum supported picture size.
	@param  aSupportedDataUnitTypes
	        The supported data unit types.
	@param  aSupportedDataUnitEncapsulations
	        The supported data unit encapsulations.
	@param  aMaxBitrateLayers
	        The maximum number of bitrate layers supported.
	@param  aSupportsSupplementalEnhancementInfo
	        Whether supplemental enhancement info is supported.
	@param  aMaxUnequalErrorProtectionLevels
	        The maximum unequal error protection level supported.
	@param  aMaxBitRate
	        The maximum bit rate supported.
	@param  aMaxPictureRates
	        An array of the maximum picture size/rates supported.
	@param  aMaxInLayerScalabilitySteps
	        The maximum in-layer scalability steps supported.
	@param  aSupportedPictureOptions
	        The picture options supported.
	@param  aCodingStandardSpecificInfo
	        Coding standard specific info.
	@param  aImplementationSpecificInfo
	        Implementation specific info.

	@return A new CVideoEncoderInfo object.
	@leave  This method may leave with one of the system-wide error codes.
	*/
	IMPORT_C static CVideoEncoderInfo* NewL(TUid aUid,
											const TDesC& aManufacturer,
											const TDesC& aIdentifier,
											TVersion aVersion,
											TBool aAccelerated,
											TBool aSupportsDirectCapture,
											const TArray<TUncompressedVideoFormat>& aSupportedInputFormats,
											const TArray<CCompressedVideoFormat*>& aSupportedOutputFormats,
											const TSize& aMaxPictureSize,
											TUint32 aSupportedDataUnitTypes,
											TUint32 aSupportedDataUnitEncapsulations,
											TUint aMaxBitrateLayers,
											TBool aSupportsSupplementalEnhancementInfo,
											TUint aMaxUnequalErrorProtectionLevels,
											TUint aMaxBitRate,
											const TArray<TPictureRateAndSize>& aMaxPictureRates,
											TUint aMaxInLayerScalabilitySteps,
											TUint32 aSupportedPictureOptions,
											TBool aSupportsPictureLoss,
											TBool aSupportsSliceLoss,
											const TDesC8& aCodingStandardSpecificInfo,
											const TDesC8& aImplementationSpecificInfo);

	/**
	Destructor.
	*/
	IMPORT_C ~CVideoEncoderInfo();

	/**
	Returns the encoder UID.
	@return "The encoder UID."
	*/
	IMPORT_C TUid Uid() const;

	/**
	Returns the encoder version.
	@return "The encoder version."
	*/
	IMPORT_C TVersion Version() const;

	/**
	Returns the encoder hardware device manufacturer.
	@return "The manufacturer name. The reference is valid until the CVideoEncoderInfo object is destroyed."
	*/
	IMPORT_C const TDesC& Manufacturer() const;

	/**
	Returns the encoder hardware device manufacturer-specific identifier. The combination of the 
	manufacturer and identifier uniquely identifies the plug-in.
	@return "The identifier. The reference is valid until the CVideoEncoderInfo object is destroyed."
	*/
	IMPORT_C const TDesC& Identifier() const;

	/**
	Returns whether the encoder is hardware-accelerated. Hardware-accelerated encoders can run on an 
	application DSP or dedicated hardware.
	@return "True if the encoder is hardware-accelerated."
	*/
	IMPORT_C TBool Accelerated() const;

	/**
	Returns whether the encoder supports direct capture. Encoders supporting direct capture can get the 
	input pictures directly from a camera, possibly using an efficient hardware-dependent data path.
	@return "True if the encoder supports direct capture."
	*/
	IMPORT_C TBool SupportsDirectCapture() const;

	/**
	Returns whether the encoder supports the given input format.
	@param  "aFormat" "The format to check."
	@return "True if the encoder supports the given input format."
	*/
	IMPORT_C TBool SupportsInputFormat(const TUncompressedVideoFormat& aFormat) const;

	/**
	Returns the input formats that the encoder supports.
	@return "An RArray table of supported video formats (TUncompressedVideoFormat). The reference is 
			valid until the CVideoEncoderInfo object is destroyed."
	*/
	IMPORT_C const RArray<TUncompressedVideoFormat>& SupportedInputFormats() const;

	/**
	Returns whether the encoder supports the given output format.
	@param	"aFormat" "The format to check."
	@return "True if the encoder supports the given output format."
	*/
	IMPORT_C TBool SupportsOutputFormat(const CCompressedVideoFormat& aFormat) const;

	/**
	Returns the output formats that the encoder supports.
	@return "An RArray table of supported video formats (CCompressedVideoFormat). The reference is 
			valid until the CVideoEncoderInfo object is destroyed."
	*/
	IMPORT_C const RPointerArray<CCompressedVideoFormat>& SupportedOutputFormats() const;

	/**
	Returns the maximum picture size the encoder supports.
	@return "The maximum picture size supported. The reference is valid until the CVideoEncoderInfo 
			object is destroyed."
	*/
	IMPORT_C const TSize& MaxPictureSize() const;

	/**
	Returns the data unit types supported by the encoder.
	@return "Supported data unit types. The value is a binary OR of values from TVideoDataUnitType."
	*/
	IMPORT_C TUint32 SupportedDataUnitTypes() const;

	/**
	Returns the data unit encapsulation types that the encoder supports.
	@return "Supported data unit encapsulation types. The value is a binary OR of values from 
			TVideoDataUnitEncapsulation."
	*/
	IMPORT_C TUint32 SupportedDataUnitEncapsulations() const;

	/**
	Returns the maximum number of bit-rate scalability layers supported.
	@return "Maximum number of bit-rate scalability layers supported, one (1) if layered scalability 
			is not supported."
	*/
	IMPORT_C TUint MaxBitrateLayers() const;

	/**
	Returns whether the encoder implements SendSupplementalInfoL(). If SendSupplementalInfoL() is 
	implemented, the client can send supplemental enhancement information messages as binary strings
	using that method. If SendSupplementalInfoL() is not implemented, this is not possible, but the 
	encoder can still generate and send coding standard or implementation specific supplemental 
	enhancement information automatically.
	@return "True if the encoder supports supplemental enhancement information."
	*/
	IMPORT_C TBool SupportsSupplementalEnhancementInfo() const;

	/**
	Returns the maximum number of unequal error protection levels supported.
	@return "Maximum number of unequal error protection levels supported, one (1) if unequal error 
			protection is not supported."
	*/
	IMPORT_C TUint MaxUnequalErrorProtectionLevels() const;

	/**
	Returns the maximum bit-rate supported by the encoder.
	@return "Maximum bit-rate supported, in bits per second. KMaxTUint32 can be used if the encoder has no 
			bit-rate restrictions."
	*/
	IMPORT_C TUint MaxBitrate() const;

	/**
	Returns the maximum picture size/rate combinations supported by the encoder.  Video encoders can have
	different maximum picture rate limitations depending on the picture size used.
	@return "A reference to an array of picture size/rate combinations.  The reference remains valid until
			this object is deleted."
	*/
	IMPORT_C const RArray<TPictureRateAndSize>& MaxPictureRates() const;

	/**
	Returns the maximum number of in-layer scalability steps supported.
	@return "Maximum number of in-layer scalability steps supported, one (1) if in-layer scalability is 
			not supported."
	*/
	IMPORT_C TUint MaxInLayerScalabilitySteps() const;

	/**
	Returns the input picture options that the encoder supports.
	@return "Supported input picture options, a bitwise OR of values from TVideoPicture::TVideoPictureOptions."
	*/
	IMPORT_C TUint32 SupportedPictureOptions() const;

	/**
	Returns whether the encoder supports picture loss indications. If true, the encoder implements
	PictureLoss(), and recovers lost picture contents when it receives the indication.
	@return "True if the encoder supports picture loss indications."
	*/
	IMPORT_C TBool SupportsPictureLoss() const;

	/**
	Returns whether the encoder supports slice loss indications. If true, the encoder implements
	SliceLoss(), and recovers lost or damaged macroblocks when it receives the indication.
	@return "True if the encoder supports slice loss indications."
	*/
	IMPORT_C TBool SupportsSliceLoss() const;

	/**
	Returns coding-standard specific information about the encoder.
	@return "Coding-standard specific information about the encoder. The data format is coding-standard 
			specific, and defined separately. The reference is valid until the CVideoEncoderInfo object 
			is destroyed."
	*/
	IMPORT_C const TDesC8& CodingStandardSpecificInfo() const;

	/**
	Returns implementation-specific information about the encoder.
	@return "Implementation-specific information about the encoder. The data format is 
	implementation-specific, and defined separately by the encoder supplier. The reference is valid until 
	the CVideoEncoderInfo object is destroyed."
	*/
	IMPORT_C const TDesC8& ImplementationSpecificInfo() const;
private:
	CVideoEncoderInfo(TUid aUid,
					  TVersion aVersion,
					  TBool aAccelerated,
					  TBool aSupportsDirectCapture,
				 	  const TSize& aMaxPictureSize,
					  TUint32 aSupportedDataUnitTypes,
					  TUint32 aSupportedDataUnitEncapsulations,
					  TUint aMaxBitrateLayers,
					  TBool aSupportsSupplementalEnhancementInfo,
					  TUint aMaxUnequalErrorProtectionLevels,
					  TUint aMaxBitRate,
					  TUint aMaxInLayerScalabilitySteps,
					  TUint32 aSupportedPictureOptions,
					  TBool aSupportsPictureLoss,
					  TBool aSupportsSliceLoss);
	void ConstructL(const TDesC& aManufacturer,
					const TDesC& aIdentifier,
					const TArray<TUncompressedVideoFormat>& aSupportedInputFormats,
					const TArray<CCompressedVideoFormat*>& aSupportedOutputFormats,
					const TArray<TPictureRateAndSize>& aMaxPictureRates,
					const TDesC8& aCodingStandardSpecificInfo,
					const TDesC8& aImplementationSpecificInfo);
private:
	TUid iUid;
	TVersion iVersion;
	HBufC* iManufacturer;
	HBufC* iIdentifier;
	TBool iAccelerated;
	TBool iSupportsDirectCapture;
	RArray<TUncompressedVideoFormat> iSupportedInputFormats;
	RPointerArray<CCompressedVideoFormat> iSupportedOutputFormats;
	TSize iMaxPictureSize;
	TUint32 iSupportedDataUnitTypes;
	TUint32 iSupportedDataUnitEncapsulations;
	TUint iMaxBitrateLayers;
	TBool iSupportsSupplementalEnhancementInfo;
	TUint iMaxUnequalErrorProtectionLevels;
	TUint iMaxBitRate;
	RArray<TPictureRateAndSize> iMaxPictureRates;
	TUint iMaxInLayerScalabilitySteps;
	TUint32 iSupportedPictureOptions;
	TBool iSupportsPictureLoss;
	TBool iSupportsSliceLoss;
	HBufC8* iCodingStandardSpecificInfo;
	HBufC8* iImplementationSpecificInfo;
	};


/**
CMMFDevVideoRecord is the main client class of DevVideoRecord.
@publishedAll
@released
*/
class CMMFDevVideoRecord : public CBase, private MMMFDevVideoRecordProxy
	{
public:
	/**
	Class to define the picture counters available through GetPictureCounters.
	@publishedAll
	@released
	*/
	class TPictureCounters
		{
 	public:
		/**
		Default constructor.  Zeros all members.
		*/
		inline TPictureCounters();
   public:
		/** The number of pictures skipped due to lack of output buffers. */
		TUint iPicturesSkippedBufferOverflow;

		/** The number of pictures skipped due to lack of processing power. */
		TUint iPicturesSkippedProcPower;

		/** The number of pictures skipped for bit-rate control purposes. */
		TUint iPicturesSkippedRateControl;

		/** The number of processed (i.e. pre-processed and encoded) input pictures. */
		TUint iPicturesProcessed;

		/** The total number of input pictures. When using direct capture, this is the total number of 
			pictures captured, otherwise it is the total number of input pictures written by the client. */
		TUint iInputPictures;
		};


public:
	/**
	Creates a new CMMFDevVideoRecord object.
	@param	"aObserver"	"The observer object to use. The observer callbacks are used to return input buffers 
						back to the client."
	@return "A new CMMFDevVideoRecord object.
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrHardwareNotAvailable - Not enough free video processing hardware resources
				- KErrNoMemory - Not enough free memory available"
	*/
	IMPORT_C static CMMFDevVideoRecord* NewL(MMMFDevVideoRecordObserver& aObserver);

	/**
	Destructor.
	*/
	IMPORT_C ~CMMFDevVideoRecord();

	/**
	Finds all available encoders for a given video type with support for certain pre-processing operations. 
	The video type is specified using its MIME type, which may include parameters specifying the supported 
	level, version, and other information. Encoder hardware devices can use wildcards when listing the 
	supported video types in the ECom registration information, so it is possible that all the encoders 
	returned do not support the specified submode, e.g. profile and level, unless aExactMatch is set.
	
	The encoder capabilities can be checked with VideoEncoderInfoLC().

	@param	"aMimeType"		"The video type that will be encoded."
	@param	"aPreProcType"	"The pre-processing types that the encoder has to support, a binary OR of 
							TPrePostProcessType values. If no pre-processing support is needed, set this 
							value to zero."
	@param	"aEncoders"		"An array for the result encoder UIDs. The array must be created and destroyed 
							by the caller."
	@param	"aExactMatch"	"True if exact matching should be used. In this case only encoders that support 
							exactly the MIME-type given will be returned. Since verifying this may require 
							loading the encoders into memory, this can be a fairly expensive operation, and 
							if the user needs to verify their capabilities further in any case this parameter
							should be set to EFalse."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				-KErrNotFound - No encoders were found matching the search parameters."
	*/
	IMPORT_C void FindEncodersL(const TDesC8& aMimeType, 
								TUint32 aPreProcType, 
								RArray<TUid>& aEncoders, 
								TBool aExactMatch=ETrue);

	/**
	Finds all available pre-processors for a given set of pre-processing operations.

	@param	"aPreProcType"		"The pre-processing types that the device has to support, a binary OR of 
								TPrePostProcessType values."
	@param	"aPreProcessors"	"An array for the result pre-processor UIDs. The array must be created and 
								destroyed by the caller."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotFound - No pre-processors were found matching the search parameters."
	*/
	IMPORT_C void FindPreProcessorsL(TUint32 aPreProcType, RArray<TUid>& aPreProcessors);

	/**
	Retrieves a list of available video encoders in the system.
	@param	"aEncoders"	"An array for the result encoder UIDs. The array must be created and destroyed by 
						the caller."
	@leave	"The method will leave if an error occurs. Not finding any encoders is not treated as an error 
			condition: in this situation, aEncoders will be empty."
	*/
	IMPORT_C void GetEncoderListL(RArray<TUid>& aEncoders);

	/**
	Retrieves a list of available video pre-processor hardware devices in the system.
	@param	"aPreProcessors"	"An array for the result pre-processor UIDs. The array must be created and 
								destroyed by the caller."
	@leave	"The method will leave if an error occurs. Not finding any pre-processors is not treated as an 
			error condition: in this situation, aPreProcessors will be empty."
	*/
	IMPORT_C void GetPreProcessorListL(RArray<TUid>& aPreProcessors);

	/**
	Retrieves information about an installed video encoder. Note that this method will need to load the 
	encoder hardware device into memory, and can thus be relatively expensive.
	@param	"aVideoEncoder"	"The video encoder device to query."
	@return	"Encoder information as a CVideoEncoderInfo object. The object is pushed to the cleanup stack, 
			and must be deallocated by the caller."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotFound - No encoder was found with the given UID"
	*/
	IMPORT_C CVideoEncoderInfo* VideoEncoderInfoLC(TUid aVideoEncoder);

	/**
	Retrieves information about the pre-processing capabilities of an installed pre-processor or encoder 
	hardware device. Note that this method will need to load the device into memory, and can thus be 
	relatively expensive.
	@param	"aPreProcessor"	"The device to query."
	@return	"Pre-processor information as a CPreProcessorInfo object. The object is pushed to the cleanupstack, 
			and must be deallocated by the caller."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotFound - No pre-processor was found with the given UID"
	*/
	IMPORT_C CPreProcessorInfo* PreProcessorInfoLC(TUid aPreProcessor);

	/**
	Selects the video encoder to be used. This method must be called before any other video encoder related 
	methods are used. The encoder can be changed by calling this method again before the system has been 
	initialized with Initialize().

	All video encoder settings are reset to their default values. By default no pre-processing is performed.

	@param	"aEncoder"	"The video encoder to use."
	@return	"Hardware device ID, used in other methods for configuring the encoder."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotFound - No encoder was found with the given UID"
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C THwDeviceId SelectEncoderL(TUid aEncoder);

	/**
	Selects the video pre-processor to be used. This method must be called before any other pre-processor 
	related methods are used. The pre-processor to use can be changed by calling this method again before 
	the API has been initialized with Initialize().

	All pre-processor settings are reset to their default values. By default no pre-processing is performed.

	@param	"aPreProcessor"	"The pre-processor to use."
	@return	"Hardware device ID, used in other methods for configuring the pre-processor."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotFound - No pre-processor was found with the given UID"
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C THwDeviceId SelectPreProcessorL(TUid aPreProcessor);

	/**
	Sets the input format for a hardware device. If both a pre-processor and an encoder are used, the 
	pre-processor output format and the encoder input format must be the same. The input format for the 
	first device in the system is the input format for video input data.

	The method has to be called for both direct capture as well as memory buffer input. The camera API 
	must be initialized by the device using it for capture, since there is no way to query the current 
	format from the camera API.
	
	@param	"aHwDevice"		"The hardware device to configure. The value is returned from SelectEncoderL() 
							or SelectPreProcessorL() when the device is selected."
	@param	"aFormat"		"The input format to use."
	@param	"aPictureSize"	"The input picture size in pixels."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The input format specified is not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetInputFormatL(THwDeviceId aHwDevice, 
								  const TUncompressedVideoFormat& aFormat, 
								  const TSize& aPictureSize);

	/**
	Sets the data source to be a camera, and sets the system to use direct capture for input. The first 
	hardware device (pre-processor if both encoder and pre-processor are used, encoder otherwise) must 
	support direct capture.
	
	@param	"aCameraHandle"	"A camera handle for the camera to use. The handle is passed to 
							CCamera::NewDuplicateL() in the camera API."
	@param	"aPictureRate"	"Video capture picture rate."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - Direct capture is not supported or the picture rate specified is not 
				  supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetSourceCameraL(TInt aCameraHandle, TReal aPictureRate);

	/**
	Sets the data source to be memory buffers.
	
	@param	"aMaxPictureRate"		"The maximum picture rate for input pictures."
	@param	"aConstantPictureRate"	"True if the input picture rate is constant. In that case, 
									aMaxPictureRate specifies the picture rate. If pictures may be skipped 
									in the input data due to performance reasons, this flag cannot be set."
	@param	"aProcessRealtime"		"True if real-time processing is needed, false if not. Real-time 
									processing is typically needed for video recording applications, while 
									video conversion and off-line processing applications do not require it."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The picture rate specified is not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetSourceMemoryL(TReal aMaxPictureRate, TBool aConstantPictureRate, TBool aProcessRealtime);

	/**
	Sets the output format for a hardware device to a compressed video format. Only applicable for encoder 
	devices. The picture size depends on the input data format and possible scaling performed.
	
	@param	"aHwDevice"				"The hardware device to configure. The value is returned from 
									SelectEncoderL()when the device is selected."
	@param	"aFormat"				"The video format to use."
	@param	"aDataUnitType"			"The type of output coded data units."
	@param	"aDataEncapsulation"	"Data encapsulation type for output encoded data units."
	@param	"aSegmentationAllowed"	"True if a coded data unit can be segmented into multiple output buffers 
									if a single buffer is not large enough."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The format specified is not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetOutputFormatL(THwDeviceId aHwDevice, 
								   const CCompressedVideoFormat& aFormat, 
								   TVideoDataUnitType aDataUnitType, 
								   TVideoDataUnitEncapsulation aDataEncapsulation,
								   TBool aSegmentationAllowed=EFalse);

	/**
	Sets the output format for a hardware device to an uncompressed video format. Only applicable for 
	pre-processor devices. The picture size depends on the input data format and possible scaling performed.
	
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectPreProcessorL()
						when the device is selected."
	@param	"aFormat"	"The video format to use."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The format specified is not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetOutputFormatL(THwDeviceId aHwDevice, const TUncompressedVideoFormat& aFormat);

	/**
	Sets the clock source to use for video timing. When video recording is synchronized with audio, the clock
	source is implemented by the audio playback subsystem, otherwise the clock source should get the time 
	from the system clock. This method can be called after all hardware devices have been selected, but 
	before calling Initialize().

	If no clock source is set, video recording will not be synchronized, but will proceed as fast as
	possible, depending on input data and output buffer availability. If direct capturing is used without a 
	clock source, the timestamps in the output data may not be valid. All encoders must support 
	synchronization with an external clock source, as well as unsynchronized non-realtime operation.
	
	@param	"aClock"	"The clock source to use."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetClockSource(MMMFClockSource* aClock);

	/**
	Sets pre-processing options for RGB to YUV color space conversion. By default, input RGB data is 
	assumed to use the full value range ([0…255]), and the output YUV format is the hardware device 
	output format, so typically calling this method is not necessary.
	
	@param	"aHwDevice"		"The hardware device to configure. The value is returned from SelectEncoderL() 
							or SelectPreProcessorL() when the device is selected."
	@param	"aRgbRange"		"Input RGB data range"
	@param	"aOutputFormat"	"Conversion output YUV format."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The formats specified are not supported"
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetRgbToYuvOptionsL(THwDeviceId aHwDevice, TRgbRange aRgbRange, const TYuvFormat& aOutputFormat);

	/**
	Sets pre-processing options for YUV to YUV data format conversion. By default, the hardware device 
	input and output data formats are used. For encoder devices, the device input format and the closest 
	matching format supported by the encoding process are used. Typically calling this method is not 
	necessary.
	
	@param	"aHwDevice"		"The hardware device to configure. The value is returned from SelectEncoderL() 
							or SelectPreProcessorL() when the device is selected."
	@param	"aInputFormat"	"Conversion input format."
	@param	"aOutputFormat"	"Conversion output format."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The formats specified are not supported"
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetYuvToYuvOptionsL(THwDeviceId aHwDevice, 
							 const TYuvFormat& aInputFormat, 
							 const TYuvFormat& aOutputFormat);

	/**
	Sets the number of bit-rate scalability layers to use. Set to 1 to disable layered scalability. 

	Bit-rate scalability refers to the ability of a coded stream to be decoded at different bit-rates. 
	Scalable video is typically ordered into hierarchical layers of data. A base layer contains an 
	individual representation of a video stream and enhancement layers contain refinement data in 
	addition to the base layer. The quality of the encoded video stream progressively improves as 
	enhancement layers are added to the base layer.

	@param	"aNumLayers"	"The number of bit-rate scalability layers to use, set to 1 to disable
							scalability."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The scalability layers are not supported or too many layers specified."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetNumBitrateLayersL(TUint aNumLayers);

	/**
	Sets the scalability type for a bit-rate scalability layer.
	
	@param	"aLayer"			"Layer number. Layers are numbered [0…n-1], where n is the number of layers 
								available. The first layer is the base layer, it can be decoded independently
								from the other layers, and it has the lowest total bitrate."
	@param	"aScalabilityType"	"Layer scalability type."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The scalability layers or the specified scalability type are not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetScalabilityLayerTypeL(TUint aLayer, TScalabilityType aScalabilityType);
	

	/**
	Sets the reference picture options to be used for all scalability layers. The settings can be 
	overridden for an individual scalability layer by using SetLayerReferenceOptions().
	
	@param	"aMaxReferencePictures"	"The maximum number of reference pictures to be used. More than one 
									reference frame can be used in the H.264 | MPEG-4 AVC and in some 
									advanced profiles of MPEG-4 Part 2 and H.263. The minimum value is one."
	@param	"aMaxPictureOrderDelay"	"The maximum picture order delay, in number of pictures. This specifies 
									the maximum number of  pictures that precede any picture in the sequence 
									in decoding order and follow the picture in presentation order. Pictures 
									may be coded/decoded in different order from their capture/display order.
									Thus, decoded pictures have to be buffered to order them in correct 
									display order. For example, if one conventional B picture is coded 
									between P pictures, a one-picture display ordering delay has to be 
									applied in the decoder. The minimum value is zero, which indicates that 
									pictures must be coded in capture/display order."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetGlobalReferenceOptions(TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay);

	/**
	Sets the reference picture options to be used for a single scalability layer. These settings override 
	those set with SetGlobalReferenceOptions().
	
	@param	"aLayer"				"Layer number."
	@param	"aMaxReferencePictures"	"The maximum number of reference pictures to be used."
	@param	"aMaxPictureOrderDelay"	"The maximum picture order delay, in number of pictures."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetLayerReferenceOptions(TUint aLayer, TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay);

	/**
	Sets encoder buffering options.
	
	@param	"aOptions"	"The buffering options."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetBufferOptionsL(const TEncoderBufferOptions& aOptions);

	/**
	Sets the encoder output rectangle for encoded video output. This rectangle specifies the part of the 
	input and output pictures which is displayed after encoding. Many video codecs process data in 16x16 
	pixel units but enable specifying and coding the decoder output rectangle for image sizes that are not 
	multiple of 16 pixels (e.g 160x120).
	
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectEncoderL()
						when the device is selected."
	@param	"aRect"		"The encoder output rectangle."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetOutputRectL(THwDeviceId aHwDevice, const TRect& aRect);

	/**
	Sets the pre-processing types to be used in a hardware device. [1 #191] If separate encoder and 
	pre-processor devices are used, both can be configured to perform different pre-processing operations.

	Pre-processing operations are carried out in the following order:

		1. Frame stabilisation
		2. Input cropping
		3. Mirroring
		4. Rotating
		5. Scaling
		6. Output cropping
		7. Output padding

	Color space conversion and color enhancement can be performed at any point in the pre-processing flow.
	
	@param	"aHwDevice"			"The hardware device to configure. The value is returned from 
								SelectEncoderL() or SelectPreProcessorL() when the device is selected."
	@param	"aPreProcessTypes"	"The pre-processing steps to perform, a bitwise OR of values from 
								TPrePostProcessType."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The pre-processing combination is not supported"
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetPreProcessTypesL(THwDeviceId aHwDevice, TUint32 aPreProcessTypes);

	/**
	Sets pre-processing options for rotation.
	
	@param	"aHwDevice"		"The hardware device to configure. The value is returned from SelectEncoderL() 
							or SelectPreProcessorL() when the device is selected."
	@param	"aRotationType"	"The rotation to perform."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The rotation type is not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetRotateOptionsL(THwDeviceId aHwDevice, TRotationType aRotationType);

	/**
	Sets pre-processing options for scaling. 
	
	@param	"aHwDevice"				"The hardware device to configure. The value is returned from 
									SelectEncoderL() or SelectPreProcessorL() when the device is selected."
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
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetScaleOptionsL(THwDeviceId aHwDevice, const TSize& aTargetSize, TBool aAntiAliasFiltering);

	/**
	Sets pre-processing options for input cropping. Input cropping is typically used for digital zooming.
	
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectEncoderL() or 
						SelectPreProcessorL() when the device is selected."
	@param	"aRect"		"The input cropping rectangle specifying the area of the picture to use. The 
						rectangle must fit completely inside the input picture."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified cropping rectangle is not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetInputCropOptionsL(THwDeviceId aHwDevice, const TRect& aRect);

	/**
	Sets pre-processing options for output cropping. Output cropping is performed after other pre-processing
	operations but before output padding. Output cropping and padding can be used in combination to prepare
	the picture size to suit the encoder, typically video encoders only support picture sizes that are 
	multiples of 16 pixels.
	
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectEncoderL() or 
						SelectPreProcessorL() when the device is selected."
	@param	"aRect"		"The output cropping rectangle specifying the area of the picture to use. The 
						rectangle must fit completely inside the picture."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified cropping rectangle is not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetOutputCropOptionsL(THwDeviceId aHwDevice, const TRect& aRect);

	/**
	Sets pre-processing options for output padding. Output padding is performed as the last pre-processing 
	operation, and typically used to prepare the picture size to suit the encoder. The image is padded with 
	black pixels.
	
	@param	"aHwDevice"		"The hardware device to configure. The value is returned from SelectEncoderL() 
							or SelectPreProcessorL() when the device is selected."
	@param	"aOutputSize"	"The padded output picture size. The output size must be large enough for the 
							picture in its new position."
	@param	"aPicturePos"	"The position for the original picture in the new padded picture. The original 
							picture in its new position must fit completely inside the new picture."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified padding settings are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetOutputPadOptionsL(THwDeviceId aHwDevice, const TSize& aOutputSize, const TPoint& aPicturePos);

	/**
	Sets color enhancement pre-processing options.
	
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectEncoderL() 
						or SelectPreProcessorL() when the device is selected."
	@param	"aOptions"	"Color enchancement options."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetColorEnhancementOptionsL(THwDeviceId aHwDevice, const TColorEnhancementOptions& aOptions);

	/**
	Sets frame stabilisation options.

	Frame stabilisation is performed as the first pre-processing operation in the hardware device. The 
	stabilisation process gets the complete hardware device input picture as its input, and it produces a 
	smaller stabilised output picture. The rest of the processing in the hardware device is done using the 
	stabilisation output picture.
	
	@param	"aHwDevice"				"The hardware device to configure. The value is returned from SelectEncoderL() 
									or SelectPreProcessorL() when the device is selected."
	@param	"aOutputSize"			"Output picture size. The output picture must size must be smaller than 
									or equal to the hardware device input picture size."
	@param	"aFrameStabilisation"	"True if frame stabilisation should be used. By default stabilisation 
									is not used."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetFrameStabilisationOptionsL(THwDeviceId aHwDevice, 
												const TSize& aOutputSize,
												TBool aFrameStabilisation);

	/**
	Sets custom implementation-specific pre-processing options.
	
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectEncoderL() 
						or SelectPreProcessorL() when the device is selected."
	@param	"aOptions"	"Post-processing options. The data format is implementation-specific."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The options are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetCustomPreProcessOptionsL(THwDeviceId aHwDevice, const TDesC8& aOptions);
	
	/**
	Sets whether bit errors or packets losses can be expected in the video transmission channel. The 
	video encoder can use this information to optimize the bitstream.
	
	@param	"aBitErrors"	"True if bit errors can be expected."
	@param	"aPacketLosses"	"True if packet losses can be expected."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetErrorsExpected(TBool aBitErrors, TBool aPacketLosses);

	/**
	Sets the minimum frequency (in time) for instantaneous random access points in the bitstream. 
	An instantaneous random access point is such where the encoder can achieve a full output picture 
	immediately by encoding data starting from the random access point. The random access point frequency
	may be higher than signalled, if the sequence contains scene cuts which typically cause a coding of 
	a random access point.
	
	@param	"aRate"	"Random access point rate, in pictures per second. For example, to request a random 
					access point every ten seconds, set the value to 0.1."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetMinRandomAccessRate(TReal aRate);

	/**
	Sets coding-standard specific encoder options.
	
	@param	"aOptions"	"The options to use. The data format for the options is coding-standard specific, 
						and defined seperately."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetCodingStandardSpecificOptionsL(const TDesC8& aOptions);

	/**
	Sets implementation-specific encoder options.
	
	@param	"aOptions"	"The options to use. The data format for the options is specific to the encoder
						implementation, and defined separately by the encoder implementor."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified settings are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetImplementationSpecificEncoderOptionsL(const TDesC8& aOptions);

	/**
	Initializes the video devices, and reserves hardware resources. This method is asynchronous, 
	DevVideoRecord will call MMMFDevVideoRecordObserver::MdvroInitializeComplete() after initialization has 
	completed. If direct capture is used, this method also prepares the camera API for capture by calling 
	PrepareVideoCaptureL(). No DevVideoRecord method may be called while initialization is in progress, the 
	initialization process can only be cancelled by destroying the DevVideoRecord object. 

	After initialization has successfully been completed, video capturing and encoding can be started with 
	Start() with a relatively low delay since the hardware has already been set up.

	If Initialize() fails, the DevVideoRecord object must be destroyed, and set up from scratch.

	Error handling: Errors are reported using the MdvroInitializeComplete() callback method. Typical error 
	codes used:
		- KErrHardwareNotAvailable - Not enough free video processing hardware resources
		- KErrNotSupported - The current configuration is not supported.

	@pre	"This method can only be called before the API has been initialized."
	*/
	IMPORT_C void Initialize();
	
	/**
	Commit all configuration changes since the last CommitL(), Revert() or Initialize(). This only applies 
	to methods that can be called both before AND after DevVideoRecord has been initialized. 
	See the following methods for details.

	@see	SetOutputRectL
	@see	SetPreProcessTypesL
	@see	SetRotateOptionsL
	@see	SetScaleOptionsL
	@see	SetInputCropOptionsL
	@see	SetOutputCropOptionsL
	@see	SetOutputPadOptionsL
	@see	SetColorEnhancementOptionsL
	@see	SetFrameStabilisationOptionsL
	@see	SetCustomPreProcessOptionsL
	@see	SetCodingStandardSpecificOptionsL
	@see	SetImplementationSpecificEncoderOptionsL

	@leave  "The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void CommitL();

	/**
	Revert any configuration changes that have not yet been committed using CommitL().  This only applies 
	to methods that can be called both before AND after DevVideoRecord has been initialized. 
	See the following methods for details.

	@see	SetOutputRectL
	@see	SetPreProcessTypesL
	@see	SetRotateOptionsL
	@see	SetScaleOptionsL
	@see	SetInputCropOptionsL
	@see	SetOutputCropOptionsL
	@see	SetOutputPadOptionsL
	@see	SetColorEnhancementOptionsL
	@see	SetFrameStabilisationOptionsL
	@see	SetCustomPreProcessOptionsL
	@see	SetCodingStandardSpecificOptionsL
	@see	SetImplementationSpecificEncoderOptionsL

	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Revert();

	/**
	Returns coding-standard specific initialization output from the encoder. The information can contain, 
	for example, the MPEG-4 VOL header. This method can be called after Initialize() has been called.
	
	@return	"Coding-standard specific initialization output. The data format is coding-standard specific
			and defined separately. The buffer is pushed to the cleanup stack, and the caller is responsible 
			for deallocating it."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C HBufC8* CodingStandardSpecificInitOutputLC();

	/**
	Returns implementation-specific initialization output from the encoder. This method can be called after 
	Initialize() has been called.
	
	@return	"Implementation-specific initialization output. The data format is specific to the encoder 
			implementation, and defined by the encoder supplier. The buffer is pushed to the cleanup stack, 
			and the caller is responsible for deallocating it."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C HBufC8* ImplementationSpecificInitOutputLC();

	/**
	Sets the number of unequal error protection levels. By default unequal error protection is not used.

	@param	"aNumLevels"		"The number of unequal error protection levels. To disable unequal error 
								protection, set this value to one. When unequal error protection is used, 
								the encoder should code pictures so that they can be divided into number of 
								unequal error protection levels having an ascending order of importance in 
								the quality of decoded pictures, with level zero indicating the least important level. 
								The encoder should map the requested bit-rate scalability layers and in-layer
								bit-rate scalability steps to the unequal error protection levels. In 
								addition, the encoder may use coded sub-pictures to divide coded pictures 
								to different regions of interest or data partitions to divide coded segments 
								into pieces of different importance."
	@param	"aSeparateBuffers"	"True if each unequal error protection level of a coded data unit shall be 
								encapsulated in its own output buffer. Ignored if unequal error protection 
								is not used. If each unequal error protection level (e.g. a data partition) 
								of coded data unit is encapsulated in its own output buffer, the caller can 
								transmit the output buffers in different logical channels or can apply a 
								different amount of application-level forward error coding for different 
								output buffers. If all unequal error protection levels (e.g. data partitions)
								of coded data unit are encapsulated in the same output buffer, the caller can
								apply a forward error control mechanism that protects the beginning of the 
								output buffer better than the remaining of the output buffer."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - Unequal error protection is not supported."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetErrorProtectionLevelsL(TUint aNumLevels, TBool aSeparateBuffers);

	/**
	Sets up an unequal error protection level. If unequal error protection is not used, this method can be 
	used to control settings for the whole encoded bitstream.

	@param	"aLevel"	"Error protection level number. This argument is ignored if unequal error 
						protection is not in use."
	@param	"aBitrate"	"Target bit-rate for this error protection level."
	@param	"aStrength"	"Forward error control strength for this error protection level. The strength can be 
						specified using values from TErrorControlStrength (EFecStrengthNone, EFecStrengthLow,
						EFecStrengthNormal, EFecStrengthHigh), or with intermediate values between those 
						constants."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - The specified bit-rate cannot be supported. "
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetErrorProtectionLevelL(TUint aLevel, TUint aBitrate, TUint aStrength);

	/**
	Sets the expected or prevailing channel conditions for an unequal error protection level, in terms of 
	expected packet loss rate. The video encoder can use this information to optimize the generated 
	bitstream.

	@param	"aLevel"			"Error protection level number. This argument is ignored if unequal error 
								protection is not in use."
	@param	"aLossRate"			"Packet loss rate, in number of packets lost per second. Set to 0.0 if 
								packet losses are not expected."
	@param	"aLossBurstLength"	"Expected average packet loss burst length. Set to zero if the information 
								is not available."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetChannelPacketLossRate(TUint aLevel, TReal aLossRate, TTimeIntervalMicroSeconds32 aLossBurstLength);

	/**
	Sets the expected or prevailing channel conditions for an unequal error protection level, in terms of 
	expected bit error rate. The video encoder can use this information to optimize the generated bitstream.

	@param	"aLevel"		"Error protection level number. This argument is ignored if unequal error 
							protection is not in use."
	@param	"aErrorRate"	"Expected bit error rate, as a fraction of the total bits transmitted. Set 
							to 0.0 if bit errors are not expected."
	@param	"aStdDeviation"	"Expected bit error rate standard deviation."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetChannelBitErrorRate(TUint aLevel, TReal aErrorRate, TReal aStdDeviation);

	/**
	Sets the target size of each coded video segment. The segment target size can be specified in terms 
	of number of bytes per segment, number of macroblocks per segment, or both.

	@param	"aLayer"			"Layer number. Layers are numbered [0…n-1], where n is the number of layers 
								available. Use zero if layered bit-rate scalability is not used."
	@param	"aSizeBytes"		"Segment target size in bytes. Set to zero to use unlimited segment size. The
								segment size in bytes should include all data that is typically stored or
								transmitted for each segment in the format currently in use. This includes all
								related headers."
	@param	"aSizeMacroblocks"	"Segment target size in number of macroblocks per segment. Set to zero to 
								use unlimited segment size."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetSegmentTargetSize(TUint aLayer, TUint aSizeBytes, TUint aSizeMacroblocks);

	/**
	Sets the bit-rate control options for a layer. If layered bit-rate scalability is not used, the options
	are set for the whole bitstream.

	@param	"aLayer"	"Layer number. Layers are numbered [0…n-1], where n is the number of layers available. 
						Use zero if layered bit-rate scalability is not used."
	@param	"aOptions"	"Bit-rate control options."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetRateControlOptions(TUint aLayer, const TRateControlOptions& aOptions);

	/**
	Sets in-layer scalability options for a layer. In-layer bit-rate scalability refers to techniques where 
	a specific part of a single-layer coded stream can be decoded correctly without decoding the leftover 
	part. For example, B-pictures can be used for this. By default in-layer scalability is not used.

	@param	"aLayer"			"Layer number. Layers are numbered [0…n-1], where n is the number of layers 
								available. Use zero if layered bit-rate scalability is not used."
	@param	"aNumSteps"			"The number of in-layer scalability steps to use. Set to one to disable 
								in-layer scalability."
	@param	"aScalabilityType"	"The scalability type to use. See the definition of TInLayerScalabilityType 
								for more information."
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
				- KErrArgument		- Some of the arguments are out of range. For example, it is not possible
									  to use the specified in-layer scalability setup due to other 
									  constraints (such as the maximum picture order delay)."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetInLayerScalabilityL(TUint aLayer, 
										 TUint aNumSteps, 
										 TInLayerScalabilityType aScalabilityType, 
										 const TArray<TUint>& aBitrateShare, 
										 const TArray<TUint>& aPictureShare);

	/**
	Sets the period for layer promotions points for a scalability layer. A layer promotion point is a 
	picture where it is possible to start encoding this enhancement layer if only the lower layers were 
	encoded earlier.

	@param	"aLayer"	"Layer number."
	@param	"aPeriod"	"Layer promotion point period. A value of one signals that each picture should be a 
						layer promotion point, value two that there is one picture between each promotion 
						point etc."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetLayerPromotionPointPeriod(TUint aLayer, TUint aPeriod);

	/**
	Returns coding-standard specific settings output from the encoder. The information can contain, for 
	example, some bitstream headers that can change based on settings modified while encoding is in progress.
	
	@return	"Coding-standard specific output. The data format is coding-standard specific and defined 
			separately. The buffer is pushed to the cleanup stack, and the caller is responsible for 
			deallocating it."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C HBufC8* CodingStandardSpecificSettingsOutputLC();

	/**
	Returns implementation-specific settings output from the encoder. The information can contain, for 
	example, some bitstream headers that can change based on settings modified while encoding is in progress.
	
	@return	"Implementation-specific initialization output. The data format is implementation-specific and 
			defined separately by the encoder supplier. The buffer is pushed to the cleanup stack, and the 
			caller is responsible for deallocating it."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C HBufC8* ImplementationSpecificSettingsOutputLC();

	/**
	Writes an uncompressed input picture. The picture must remain valid and unmodified until it is returned 
	with the MdvroReturnPicture() callback. This method must not be called if direct capture is used.

	@param	"aPicture"	"The picture to write."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void WritePictureL(TVideoPicture* aPicture);

	/**
	Requests the encoder to sends supplemental information in the bitstream. The information data format is 
	coding-standard dependent. Only one supplemental information send request can be active at a time. 
	This variant encodes the information to the next possible picture.

	@param	"aData"	"Supplemental information data to send. The buffer can be reused immediately after the 
					method returns."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - Supplemental information is not supported"
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SendSupplementalInfoL(const TDesC8& aData);

	/**
	Requests the encoder to send supplemental information in the bitstream. The information data format is 
	coding-standard dependent. Only one supplemental information send request can be active at a time. This 
	variant encodes the information to the picture whose presentation timestamp is equal to or greater than 
	and closest to the value of aTimestamp.

	@param	"aData"			"Supplemental information data to send. The buffer can be reused immediately 
							after the method returns."
	@param	"aTimestamp"	"Timestamp for the picture in which the supplemental information should be 
							included. If the timestamp is in the past, the supplemental information will
							not be sent."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				- KErrNotSupported - Supplemental information is not supported"
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SendSupplementalInfoL(const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp);

	/**
	Cancels the current supplemental information send request. The memory buffer reserved for supplemental 
	information data can be reused or deallocated after the method returns.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void CancelSupplementalInfo();

	/**
	Notifies the system that the end of input data has been reached. No more input data can be written 
	through the API. The encoder and pre-processor can use this signal to ensure that the remaining data 
	gets processed, without waiting for new data. After the remaining data has been processed, the client 
	gets notified by the MdvroStreamEnd() callback.

	This method is mainly useful for file-to-file conversions and other non-realtime processing. For 
	real-time recording all pictures are processed or discarded according to their timestamps.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void InputEnd();

	/**
	Starts recording video. This includes capturing pictures from the camera (if direct capture is used), 
	pre-processing and encoding. Recording will proceed until it is stopped or paused. Initally recording 
	is stopped.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Start();

	/**
	Stops recording video. No new pictures will be captured, pre-processed, or encoded. If input pictures 
	are written while recording is stopped, they will be returned immediately.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Stop();

	/**
	Pauses video recording. Recording can be resumed using Resume().

	While video is paused, new pictures are not captured, and input pictures written using WritePictureL() 
	are discarded. Timestamps are not incremented - the encoder assumes that the clock source is paused as 
	well, and input picture timestamps are adjusted accordingly. Pause is typically used in video recording 
	applications to pause recording, conversational applications should use Freeze() instead.

	Note: The client has to ensure that the clock source is paused properly. If the paused picture should 
	be shown longer than normal, the client should either adjust input picture timestamps, or start 
	recording (when using direct capture) some time after restarting the clock source.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Pause();

	/**
	Resumes video recording after a pause.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Resume();

	/**
	Freezes the input picture. Freezing is similar to a pause, except that timestamps are incremented 
	normally during a freeze. Normal encoding can be continued using ReleaseFreeze(). 

	Freeze is typically used in video telephony applications to stop sending new pictures. In that situation
	the audio encoding can still continue normally, and thus the clock source timestamps are incremented 
	normally. When the freeze is released, the encoder assumes that input picture timestamps have been 
	incremented normally. If direct capture is used, the picture timestamps are updated as if encoding had 
	been going on throughout the freeze.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Freeze();

	/**
	Releases a frozen input picture. Video capturing and encoding continues normally.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void ReleaseFreeze();

	/**
	Returns the current recording position. The position is the capture timestamp from the latest input 
	picture, or the capture timestamp for the latest picture captured from the camera when direct capture 
	is used.
	
	@return	"The current recording position."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TTimeIntervalMicroSeconds RecordingPosition();

	/**
	Gets the current output buffer status. The information includes the number of free output buffers and 
	the total size of free buffers in bytes.

	@param	"aNumFreeBuffers"	"Target for the number of free output buffers."
	@param	"aTotalFreeBytes"	"Target for the total free buffer size in bytes."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetOutputBufferStatus(TUint& aNumFreeBuffers, TUint& aTotalFreeBytes);

	/**
	Reads various counters related to processed video pictures. See the definition of TPictureCounters 
	for a description of the counters. The counters are reset when Initialize() or this method is called, 
	and thus they only include pictures processed since the last call.

	@param	"aCounters"	"The counter structure to fill."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetPictureCounters(TPictureCounters& aCounters);

	/**
	Reads the frame stabilisation output picture position. This information can be used for positioning 
	the viewfinder. The position returned is the stabilisation result for the most recent input picture.

	@param	"aHwDevice"	"The hardware device to query. The value is returned from SelectEncoderL() or 
						SelectPreProcessorL() when the device is selected."
	@param	"aRect"		"The position of the stabilisation output picture inside the input picture. If 
						frame stabilisation is not used, the rectangle is set to cover the entire input 
						picture."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetFrameStabilisationOutput(THwDeviceId aHwDevice, TRect& aRect);

	/**
	Returns the number of output buffers currently containing valid output data.
	
	@return	"The number of output buffers with valid output data."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TUint NumDataBuffers();

	/**
	Retrieves the next output buffer, in coding order. The buffer must be returned using ReturnBuffer() when
	it is no longer used. The maximum amount of buffers that the caller can keep in its use is controlled by
	the iMinNumOutputBuffers and iMinTotalOutputBufferSize fields in the TEncoderBufferOptions settings. 
	
	@return	"The next output buffer, in coding order. If no new buffers are available, the return value 
			is NULL."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TVideoOutputBuffer* NextBufferL();

	/**
	Returns a used output buffer back to the encoder. The buffer can no longer be used by the client.

	@param	"aBuffer"	"The buffer to return."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void ReturnBuffer(TVideoOutputBuffer* aBuffer);

	/**
	Indicates a picture loss to the encoder, without specifying the lost picture. The encoder can react to
	this by transmitting an intra-picture.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void PictureLoss();

	/**
	Indicates to the encoder the pictures that have been lost. The encoder can react to this by 
	transmitting an intra-picture.

	@param	"aPictures"	"Picture identifiers for the lost pictures. See [6] for a definition of TPictureId."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void PictureLoss(const TArray<TPictureId>& aPictures);

	/**
	Indicates a loss of consecutive macroblocks in raster scan order to the encoder. 

	@param	"aFirstMacroblock"	"The first lost macroblock. The macroblocks are numbered such 
								that the macroblock in the upper left corner of the picture is considered 
								macroblock number 1 and the number for each macroblock increases from left 
								to right and then from top to bottom in raster-scan order."
	@param	"aNumMacroblocks"	"The number of lost macroblocks that are consecutive in raster scan order."
	@param	"aPicture"			"The picture number for the picture where the macroblocks were lost. 
								If the picture is not known, aPicture.iIdType is set to ENone."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);

	/**
	Sends a reference picture selection request to the encoder. The request is delivered as a coding-standard
	specific binary message. Reference picture selection can be used to select a previous correctly 
	transmitted picture to use as a reference in case later pictures have been lost.

	@param	"aSelectionData"	"The reference picture selection request message. The message format is 
								coding-standard specific, and defined separately."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void ReferencePictureSelection(const TDesC8& aSelectionData);

	/**
	Retrieves the number of complexity control levels available for a hardware device. Devices can support 
	processing the same input data with different computational complexity levels. The complexity level 
	can affect, for example, the motion vector search range used in an encoder.
	
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectEncoderL() or 
						SelectPreProcessorL() when the device is selected."
	@return	"The number of complexity control levels available, one if multiple levels are not supported."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TUint NumComplexityLevels(THwDeviceId aHwDevice);

	/**
	Sets the complexity level to use for video processing in a hardware device. The level can be changed at 
	any time. 

	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectEncoderL() or 
						SelectPreProcessorL() when the device is selected."
	@param	"aLevel"	"The computational complexity level to use. Level zero (0) is the most complex one,
						with the highest quality. Higher level numbers require less processing and may have 
						lower quality."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetComplexityLevel(THwDeviceId aHwDevice, TUint aLevel); 

	/**
	Retrieves a custom interface to the specified hardware device.
	@param	"aHwDevice"		"The hardware device from which the custom interface shall be requested. 
							The value is returned from SelectDecoderL() or SelectPostProcessorL() when
							the device is selected."
	@param  "aInterface"	"Interface UID, defined with the custom interface."
	@return "Pointer to the interface implementation, or NULL if the device does not 
			implement the interface requested. The return value must be cast to the 
			correct type by the user."
	*/
	IMPORT_C TAny* CustomInterface(THwDeviceId aHwDevice, TUid aInterface);

private:
	enum TInitializationState 
		{
		ENotInitialized = 0x01, 
		EInitializing = 0x02,
		EInitialized = 0x04,
		EInitializationFailed = 0x08
		};

private:
	CMMFDevVideoRecord(MMMFDevVideoRecordObserver& aObserver);

	// Methods to check aHwDevice is valid and return the appropriate HwDevice
	CMMFVideoRecordHwDevice& VideoRecordHwDevice(THwDeviceId aHwDevice) const;
	CMMFVideoRecordHwDevice& CapturingHwDevice() const; //returns the first plugin in the chain
	CMMFVideoEncodeHwDevice& VideoEncodeHwDevice(THwDeviceId aHwDevice) const;// Checks that aHwDevice is valid
	CMMFVideoPreProcHwDevice& VideoPreProcHwDevice(THwDeviceId aHwDevice) const;// Checks that aHwDevice is valid
	CMMFVideoEncodeHwDevice& VideoEncodeHwDevice() const;
	CMMFVideoPreProcHwDevice& VideoPreProcHwDevice() const;
	CMMFVideoHwDevice& VideoHwDevice(THwDeviceId aHwDevice) const;

	// Connects the plugins together
	void ConnectPlugins();

	// Check that we are in a valid initialization state
	// Panics if iInitializationState is not one of aExpected
	void CheckInitializationState(TUint aExpected);

	// Methods to handle init complete callbacks from the hw devices
	void HandlePreProcInitializeComplete(TInt aError);
	void HandleEncoderInitializeComplete(TInt aError);
	
	CMMFVideoEncodeHwDevice* CreateEncoderL(TUid aVideoEncoder);

	// from MMMFDevVideoRecordProxy
	virtual void MdvrpNewBuffer(TVideoOutputBuffer* aBuffer);
	virtual void MdvrpReturnPicture(TVideoPicture* aPicture);
	virtual void MdvrpSupplementalInfoSent();
	virtual void MdvrpFatalError(CMMFVideoHwDevice* aDevice, TInt aError);
	virtual void MdvrpInitializeComplete(CMMFVideoHwDevice* aDevice, TInt aError);
	virtual void MdvrpStreamEnd();

private:
	MMMFDevVideoRecordObserver& iObserver;
 	CMMFVideoEncodeHwDevice* iVideoEncodeHwDevice;
	CMMFVideoPreProcHwDevice* iVideoPreProcHwDevice;
	TUint iInitializationState;
	TUint iNumberOfMdvrpStreamEndCallbacks;

	TDblQue<TVideoOutputBuffer> iVideoOutputBufferQue;
	TDblQueIter<TVideoOutputBuffer> iVideoOutputBufferQueIter;
	TUint iNumberOfVideoOutputBuffers;
	TBool iIsPreProcComplete;
	
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	TBool iPuListCreated;
	RImplInfoPtrArray iPuImplementations;
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	};


/**
The MMMFDevVideoObserver class defines the observer mixin-interface that any client using CMMFDevVideoRecord 
must implement. 
@publishedAll
@released
*/
class MMMFDevVideoRecordObserver
	{
public:
	/**
	Returns a used input video picture back to the caller. The picture memory can be re-used or freed.
	@param	"aPicture"	"The picture to return."
	*/
	virtual void MdvroReturnPicture(TVideoPicture* aPicture) = 0;

	/**
	Signals that the supplemental info send request has completed. The buffer used for supplemental 
	information can be re-used or freed.
	*/
	virtual void MdvroSupplementalInfoSent() = 0;

	/**
	Notifies the client that one or more new output buffers are available. The client can then use 
	NextBufferL() and related methods to fetch the data. 
	*/
	virtual void MdvroNewBuffers() = 0;

	/**
	Reports a fatal encoding or capturing error to the client. When these
	errors occur, capturing and encoding is stopped automatically. The client
	must destroy the CMMFDevVideoRecord object and create a new instance before
	attempting to continue. Note that scenarios have been identified where
	MdvroFatalError may get referenced at some point during the execution of a
	CMMFDevVideoRecord procedure. Therefore CMMFDevVideoRecord object should be
	deleted outside of MdvroFatalError context in order to avoid accidental
	access to de-allocated CMMFDevVideoRecord data members.
	@param "aError" "The error code."
	*/
	virtual void MdvroFatalError(TInt aError) = 0;

	/**
	Reports that DevVideoRecord initialization has completed. The interface can now be used for video 
	recording.
	@param	"aError"	"Initialization error code, KErrNone if no error occurred."
	*/
	virtual void MdvroInitializeComplete(TInt aError) = 0;

	/**
	Reports that the input video data end has been reached and all pictures have been processed. This
	method is only called after the client has called InputEnd(). No more output data will be available.
	*/
	virtual void MdvroStreamEnd() = 0;
	};


#include <mmf/devvideo/devvideorecord.inl>

#endif
