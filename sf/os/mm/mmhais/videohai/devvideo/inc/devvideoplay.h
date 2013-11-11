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

#ifndef __DEVVIDEOPLAY_H__
#define __DEVVIDEOPLAY_H__

#include <e32std.h>
#include <mmf/devvideo/devvideobase.h>

class MMMFDevVideoPlayObserver;
class CMMFVideoPlayHwDevice;
class CMMFVideoDecodeHwDevice;
class CMMFVideoPostProcHwDevice;


/**
MMMFDevVideoPlayProxy is the interface to the CMMFDevVideoPlay API implementation that the hardware devices 
use to deliver data back to the client and report their progress to the API implementation.
@publishedAll
@released
*/
class MMMFDevVideoPlayProxy
	{
public:
	/**
	Delivers a new decoded picture to the client. The CMMFDevVideoPlay implementation will maintain 
	a list of decoded pictures and implement GetNewPictureInfo() and NextPictureL() based on those. 
	The pictures will be returned back to the hardware device using ReturnPicture().

	@param "aPicture" "The newly processed picture."
	*/
	virtual void MdvppNewPicture(TVideoPicture* aPicture) = 0;

	/**
	Notifies the client that one or more new empty input buffers are available. Called by the decoder 
	hardware device.
	*/
	virtual void MdvppNewBuffers() = 0;

	/**
	Returns a used input video picture back to the caller. Called by a post-processor hardware device 
	after the picture has been processed and the picture source was the client, not another plug-in.

	@param "aPicture" "The picture to return."
	*/
	virtual void MdvppReturnPicture(TVideoPicture* aPicture) = 0;

	/**
	Delivers supplemental information from a decoder hardware device to the client. 
	The information is codec-dependent. The method is synchronous - the client 
	MMMFDevVideoPlayObserver::MdvppSupplementalInformation() method is called immediately, 
	and the memory for the supplemental information can be re-used when the call returns.

	@param "aData"		"The supplemental data."
	@param "aTimestamp" "The presentation timestamp for the picture that the supplemental data is part of."
	@param "aPictureId" "Picture identifier for the picture. If a picture ID is not available, 
						aPictureId.iIdType is set to ENone."
	*/
	virtual void MdvppSupplementalInformation(const TDesC8& aData, 
		const TTimeIntervalMicroSeconds& aTimestamp, const TPictureId& aPictureId) = 0;

	/**
	Back channel information, indicating a picture loss without specifying the lost picture.
	*/
	virtual void MdvppPictureLoss() = 0;

	/**
	Back channel information, indicating the pictures that have been lost.
	
	@param "aPictures"	"Picture identifiers for the lost pictures. The reference is only valid
						until the method returns."
	*/
	virtual void MdvppPictureLoss(const TArray<TPictureId>& aPictures) = 0;

	/**
	Back channel information, indicating the loss of consecutive macroblocks in raster scan order.

	@param "aFirstMacroblock"	"The first lost macroblock.  The macroblocks are numbered 
								such that the macroblock in the upper left corner of the picture is 
								considered macroblock number 1 and the number for each macroblock increases 
								from left to right and then from top to bottom in raster-scan order."
	@param "aNumMacroblocks"	"The number of lost macroblocks that are consecutive in raster-scan order."
	@param "aPicture"			"The picture identifier for the picture where the macroblocks were lost.  
								If the picture is not known, aPicture.iIdType is set to ENone.  The 
								reference is only valid until the method returns."
	*/
	virtual void MdvppSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture)= 0;

	/**
	Back channel information from the decoder, indicating a reference picture selection request. 
	The request is delivered as a coding-standard specific binary message.  Reference picture selection
	can be used to select a pervious correctly transmitted picture to use as a reference in case later 
	pictures have been lost.
	
	@param "aSelectionData"	"The reference picture selection request message. The message format is 
							coding-standard specific, and defined separately. The reference is only 
							valid until the method returns."
	*/
	virtual void MdvppReferencePictureSelection(const TDesC8& aSelectionData)= 0;

	/**
	Delivers a timed snapshot result to the client. The memory reserved for the snapshot picture 
	can no longer be used in the device.

	@param "aError"					"An error code, KErrNone if no errors occurred. 
									If an error occurred, the data in the snapshot may not be valid, 
									but the memory can still be freed."
	@param "aPictureData"			"The snapshot picture data."
	@param "aPresentationTimestamp"	"The presentation timestamp for the snapshot picture."
	@param "aPictureId"				"Picture identifier for the picture. If a picture ID is not 
									available, aPictureId.iIdType is set to ENone."
	*/
	virtual void MdvppTimedSnapshotComplete(TInt aError, TPictureData* aPictureData, 
		const TTimeIntervalMicroSeconds& aPresentationTimestamp, const TPictureId& aPictureId) = 0;

	/**
	Reports a fatal error to the client. The device must automatically stop processing 
	video data when such errors occur, and may not do further processing before it has 
	been deleted and re-created.

	@param "aDevice" "The device that reported the error."
	@param "aError"	 "The error code."	
	*/
	virtual void MdvppFatalError(CMMFVideoHwDevice* aDevice, TInt aError) = 0;

	/**
	Reports that an asynchronous Initialize() method has completed. 
	The device is now ready for playback.

	@param "aDevice" "The device that was initialzied."
	@param "aError"	 "Initialization result error code, KErrNone if initialization was successful."	
	*/
	virtual void MdvppInitializeComplete(CMMFVideoHwDevice* aDevice, TInt aError) = 0;

	/**
	Reports that the input video stream end has been reached and all pictures have been processed. 
	Called by each hardware device after their InputEnd() methods have been called and all data has 
	been processed. The proxy implementation will notify the client about stream end when all 
	hardware devices have called this method.
	*/
	virtual void MdvppStreamEnd() = 0;
	};


/**
A buffer for compressed video data, contains one coded data unit. Video buffers are used for writing 
video data to the API.

@publishedAll
@released
*/
class TVideoInputBuffer
	{
public:
	/**
	Default constructor. Zeroes all members (including iData which will point to garbage until manually
	set to point to the real video buffer memory data area by the user).
	*/
	IMPORT_C TVideoInputBuffer();

public:
	/**
	 */
	enum TVideoBufferOptions
		{
		/** The sequence number field is valid. */
		ESequenceNumber	 = 0x00000001,
		/** The decoding timestamp field is valid */
		EDecodingTimestamp  = 0x00000002,
		/** The presentation timestamp field is valid.*/
		EPresentationTimestamp = 0x00000004,
		/** Content protected pictures cannot be displayed on unprotected 
		    external displays such as TV-out.
		*/
		EContentProtected = 0x00000008
		};

	/**
	Pointer to the video data.
	*/
	TPtr8 iData;

	/**
	Data unit options. The value is a bitfield combined from values from TVideoBufferOptions.
	@see TVideoBufferOptions
	*/
	TUint32 iOptions;

	/**
	Data unit decoding timestamp. Valid if EDecodingTimestamp is set in the options.
	*/
	TTimeIntervalMicroSeconds iDecodingTimestamp;

	/**
	Data unit presentation timestamp. Valid if EPresentationTimestamp is set in the options. 
	If the input bitstream does not contain timestamp information, this field should be valid, 
	otherwise pictures cannot be displayed at the correct time.   If the input bitstream contains 
	timestamp information (such as the TR syntax element of H.263 bitstreams) and valid 
	iPresentationTimestamp is provided, the value of iPresentationTimestamp is used in playback.
	*/
	TTimeIntervalMicroSeconds iPresentationTimestamp;

	/**
	True if the data is part of a pre-roll period and may not be drawn. The decoder may skip 
	display-related operations, but must still decode normally since pre-roll may not end in a key 
	frame.
	*/
	TBool iPreRoll;

	/**
	Data unit sequence number. Valid if ESequenceNumber is set in the options. If present, the 
	sequence number is incremented once per coded data unit, a gap in the numbers indicates missing 
	data.
	*/
	TUint iSequenceNumber;

	/**
	True if the data unit is known to contain erroneous data.
	*/
	TBool iError;

	/**
	A queue link used internally by the MSL API. The field must not be modified while the buffer is 
	in the MSL API, but can be used by the client before the buffer has been written and after the 
	buffer has been returned.
	*/
	TDblQueLink iLink;

	/**
	A pointer for free-form user data. The pointer is set by the module that created the buffer, and 
	is usually used for memory management purposes.
	*/
	TAny* iUser;
	};


/**
This class contains information about the post-processing functionality that a single post-processor 
or decoder hardware device has. Although it mainly contains static data, it is defined as a complete 
CBase-derived class since the data is relatively complex and proper memory management is necessary.

The objects are created by the post-processor or decoder devices, and used by the MSL video client code.
@publishedAll
@released
*/
class CPostProcessorInfo : public CBase
	{
public:
	/**
	Creates and returns a new CPostProcessorInfo object.

	@param	"aUid"							"The UID of the post-processor."
	@param	"aManufacturer"					"The manufacturer of the post-processor hw device."
	@param	"aIdentifier"					"The post-processor hw device manufacturer-specific 
											identifier."
	@param	"aVersion"						"The post-processor version."
	@param	"aSupportedFormats"				"The source formats supported by the post-processor."
	@param	"aSupportedCombinations"		"The supported post-processing combinations.  An array of 
											values created by the bitwise OR-ing of values from 
											TPrePostProcessType."
	@param	"aAccelerated"					"Whether the post processor is hardware-accelerated. 
											Accelerated post-processors can run on an application DSP 
											or dedicated hardware."
	@param	"aSupportsDirectDisplay"		"Whether the hw device supports output directly to the 
											screen."
	@param	"aYuvToRgbCapabilities"			"The post-processor YUV to RGB conversion capabilities."
	@param	"aSupportedRotations"			"A bitwise OR of values of TRotationType to indicate the 
											supported rotation types."
	@param	"aSupportArbitraryScaling"		"Whether the post-processor supports arbitrary scaling."
	@param	"aSupportedScaleFactors"		"A list of the discrete scaling factors supported. If the
											post-processor supports arbitrary scaling, this list should
											be left zero-length."
	@param	"aAntiAliasedScaling"			"Whether anti-aliasing filtering for scaling is supported."
	@param	"aImplementationSpecificInfo"	"Implementation-specific information."

	@return "A new CPostProcessorInfo object."
	@leave	"This method may leave with one of the system-wide error codes."
	*/
	IMPORT_C static CPostProcessorInfo* NewL(TUid aUid,
											 const TDesC& aManufacturer,
											 const TDesC& aIdentifier,
											 TVersion aVersion,
											 const TArray<TUncompressedVideoFormat>& aSupportedFormats,
											 const TArray<TUint32>& aSupportedCombinations,
											 TBool aAccelerated,
											 TBool aSupportsDirectDisplay,
											 const TYuvToRgbCapabilities& aYuvToRgbCapabilities,
											 TUint32 aSupportedRotations,
											 TBool aSupportArbitraryScaling,
											 const TArray<TScaleFactor>& aSupportedScaleFactors,
											 TBool aAntiAliasedScaling,
											 const TDesC8& aImplementationSpecificInfo = KNullDesC8);

	/**
	Destructor
	*/
	IMPORT_C ~CPostProcessorInfo();

	/**
	Returns the post-processor UID.
	@return "Post-processor UID"
	*/
	IMPORT_C TUid Uid() const;

	/**
	Returns the post-processor hardware device manufacturer.
	@return "The manufacturer name as a standard Symbian descriptor. The reference is valid until the 
			CPostProcessorInfo object is destroyed."
	*/
	IMPORT_C const TDesC& Manufacturer() const;

	/**
	Returns the post-processor hardware device manufacturer-specific identifier. 
	The combination of the manufacturer and identifier uniquely identifies the device.
	@return "The identifier as a standard Symbian descriptor. The reference is valid until the 
			CPostProcessorInfo object is destroyed."
	*/
	IMPORT_C const TDesC& Identifier() const;

	/**
	Returns the post-processor version.
	@return "Post-processor version."
	*/
	IMPORT_C TVersion Version() const;

	/**
	Checks if the post-processor supports the given format as a source format.
	@param	"aFormat"	"The format to check. The reference is not used after the method returns."
	@return "ETrue if the post-processor supports the given format, EFalse if not."
	*/
	IMPORT_C TBool SupportsFormat(const TUncompressedVideoFormat& aFormat) const;

	/**
	Lists the source formats supported by the post-processor.
	@return "A RArray table of supported video formats (TUncompressedVideoFormat). The reference is 
			valid until the CPostProcessorInfo object is destroyed."
	*/
	IMPORT_C const RArray<TUncompressedVideoFormat>& SupportedFormats() const;

	/**
	Checks if the post-processor supports the given post-processing combination.
	@param	"aCombination" "Post-processing combination, a bitwise OR of values from TPrePostProcessType."
	@return "ETrue if the post-processing combination is supported, EFalse if not."
	*/
	IMPORT_C TBool SupportsCombination(TUint32 aCombination) const;

	/**
	Lists all supported post-processing combinations.
	@return "A RArray table or post-processing combinations. Each value is a bitwise OR of values from 
			TPrePostProcessType. The reference is valid until the CPostProcessorInfo object is destroyed."
	*/
	IMPORT_C const RArray<TUint32>& SupportedCombinations() const;

	/**
	Returns whether the hardware device is hardware-accelerated. Hardware-accelerated post-processors 
	can run on an application DSP or dedicated hardware.
	@return "ETrue if the device is hardware-accelerated."
	*/
	IMPORT_C TBool Accelerated() const;

	/**
	Returns whether the hardware device supports output directly to the screen. Output to memory buffers 
	is always supported.
	@return "ETrue if the post-processor supports direct screen output."
	*/
	IMPORT_C TBool SupportsDirectDisplay() const;

	/**
	Returns the post-processor YUV to RGB color conversion capabilities.
	@return "The conversion capabilities as a TYuvToRgbCapabilities structure. The reference is valid 
			until the CPostProcessorInfo object is destroyed. If the post-processor does not support 
			YUV to RGB conversion, the contents are undefined."
	*/
	IMPORT_C const TYuvToRgbCapabilities& YuvToRgbCapabilities() const;

	/**
	Returns the rotation types the post-processor supports.
	@return "The supported rotation types as a bitwise OR of TRotationType values. If the 
			post-processor does not support rotation, the return value is zero."
	*/
	IMPORT_C TUint32 SupportedRotations() const;

	/**
	Returns whether the post-processor supports arbitrary scaling. If arbitrary scaling is not 
	supported, a limited selection of scaling factors may still be available, use 
	SupportedScaleFactors() to retrieve those.
	@return "ETrue if the post-processor supports arbitrary scaling, EFalse if not."
	*/
	IMPORT_C TBool SupportsArbitraryScaling() const;

	/**
	Returns the scaling factors the post-processor supports. If the post-processor supports arbitrary 
	scaling the list is empty - use SupportsArbitraryScaling() first.
	@return "A RArray list of supported scale factors (TScaleFactor). The reference is valid until the 
			CPostProcessorInfo object is destroyed. If the post-processor supports arbitrary scaling 
			or no scaling at all, the list is empty."
	*/
	IMPORT_C const RArray<TScaleFactor>& SupportedScaleFactors() const;

	/**
	Returns whether the hardware device supports anti-aliasing filtering for scaling.
	@return "True if anti-aliasing filtering is supported."
	*/
	IMPORT_C TBool AntiAliasedScaling() const;

	/**
	Returns implementation-specific information about the post-processor.
	@return "Implementation- specific information about the post-processor. The data format is 
			implementation-specific, and defined separately by the post-processor supplier. The 
			reference is valid until the CPostProcessorInfo object is destroyed."
	*/
	IMPORT_C const TDesC8& ImplementationSpecificInfo() const;
	
	/**
	Adds the screen number into the list of screens supported by the post processor.
	@leave	"KErrNoMemory when there is no memory to expand the list of supported screens. 
			 KErrNotSupported if the secondary screen display is not supported in Multimedia Framework."
	*/
	IMPORT_C void AddSupportedScreenL(TInt aScreenNo);
	
	/**
	Lists the screens supported by the post processor.
	@param	"aSupportedScreens"		"An array to retrieve the list of supported screens. 
									 This method resets	the array before adding elements to it. 
									 The array must be created and destroyed by the caller."
	@leave "KErrNotSupported if the secondary screen display is not supported in Multimedia Framework.
			KErrNoMemory when there is no memory to expand the list."
	*/
	IMPORT_C void GetSupportedScreensL(RArray<TInt>& aSupportedScreens) const;
	
	/** Sets a flag indicating whether the PostProcessor supports per picture content protection.
	    E.g. Where content protection within a video stream can alter.
	 @param "aSetting" "Set to TRUE to indicate PostProcessor supports content protection. 
	 @See TVideoPicture::TVideoPictureOptions::EContentProtected 
	 @See TVideoInputBuffer::TVideoBufferOptions::EContentProtected 
	*/
	IMPORT_C void SetSupportsContentProtected(const TBool aSetting);
		
	/** Returns whether the PostProcessor supports per picture content protection.
	    E.g. Where content protection within a video stream can alter.
	 @return "True if the PostProcessor supports Content Protection." 
	 @See TVideoPicture::TVideoPictureOptions::EContentProtected
	 @See TVideoInputBuffer::TVideoBufferOptions::EContentProtected
	*/
	IMPORT_C TBool SupportsContentProtected() const;
	
private:
	CPostProcessorInfo(TUid aUid,
					   TVersion aVersion,
					   TBool aAccelerated,
					   TBool aSupportDirectDisplay,
					   const TYuvToRgbCapabilities& aYuvToRgbCapabilities,
					   TUint32 aSupportedRotations,
					   TBool aSupportArbitraryScaling,
					   TBool aAntiAliasedScaling);

	void ConstructL(const TDesC& aManufacturer,
					const TDesC& aIdentifier,
					const TArray<TUncompressedVideoFormat>& aSupportedFormats,
					const TArray<TUint32>& aSupportedCombinations,
					const TArray<TScaleFactor>& aSupportedScaleFactors,
					const TDesC8& aImplementationSpecificInfo);
private:
	TUid iUid;
	TVersion iVersion;
	TBool iAccelerated;
	TBool iSupportDirectDisplay;
	TYuvToRgbCapabilities iYuvToRgbCapabilities;
	TUint32 iSupportedRotations;
	TBool iSupportArbitraryScaling;
	TBool iAntiAliasedScaling;
	HBufC* iManufacturer;
	HBufC* iIdentifier;
	HBufC8* iImplementationSpecificInfo;
 	RArray<TUncompressedVideoFormat> iSupportedFormats;
	RArray<TUint32> iSupportedCombinations;
	RArray<TScaleFactor> iSupportedScaleFactors;
	RArray<TInt> iSupportedScreens;
	TBool iSupportsContentProtected;
	};


/**
This class contains information about a single video decoder. Although it mainly contains static data, 
it is defined as a complete CBase-derived class since the data is relatively complex and proper memory 
management is necessary.

The objects are created by the video decoder hardware devices, and used by the MSL video client code.
@publishedAll
@released
*/
class CVideoDecoderInfo : public CBase
	{
public:
	/**
	Creates and returns a new CVideoDecoderInfo object.

	@param	"aUid"							"The uid of the decoder."
	@param	"aManufacturer"					"The video decoder manufacturer."
	@param	"aIdentifier"					"The manufacturer-specific identifier for this video decoder."
	@param	"aVersion"						"The version of this video decoder."
	@param	"aSupportedFormats"				"An array of the formats supported by the decoder. 
											A copy will be taken of the array and the referenced 
											CCompressedVideoFormat objects"
	@param	"aAccelerated"					"Whether this decoder is accelerated or not."
	@param	"aSupportsDirectDisplay"		"Whether this decoder supports direct display or not."
	@param	"aMaxPictureSize"				"The maximum picture size supported by the decoder."
	@param	"aMaxBitrate"					"The maximum bit rate supported by the decoder. Use KMaxTUint32 if there are no bit-rate restrictions."
	@param	"aMaxPictureRates"				"An array of the maximum picture size/rate combinations supported by the decoder."
	@param	"aSupportsPictureLoss"			"Whether the decoder supports picture loss indications."
	@param	"aSupportsSliceLoss"			"Whether the decoder supports slice loss indications."
	@param	"aCodingStandardSpecificInfo"	"Coding-standard specific information about the decoder."
	@param	"aImplementationSpecificInfo"	"Implementation-specific information about the decoder."

	@return "A new CVideoDecoderInfo object."
	@leave	"This method may leave with one of the system-wide error codes."
	*/
	IMPORT_C static CVideoDecoderInfo* NewL(TUid aUid,
											const TDesC& aManufacturer,
											const TDesC& aIdentifier,
											TVersion aVersion,
											const TArray<CCompressedVideoFormat*>& aSupportedFormats,
											TBool aAccelerated,
											TBool aSupportsDirectDisplay,
											const TSize& aMaxPictureSize,
											TUint aMaxBitrate,
											const TArray<TPictureRateAndSize>& aMaxPictureRates,
											TBool aSupportsPictureLoss,
											TBool aSupportsSliceLoss,
											const TDesC8& aCodingStandardSpecificInfo = KNullDesC8,
											const TDesC8& aImplementationSpecificInfo = KNullDesC8);

	/**
	Destructor.
	*/
	IMPORT_C ~CVideoDecoderInfo();

	/**
	Checks if the decoder supports the given format.
	@param	"aFormat"	"The format to check. The reference is not used after the method returns."
	@return "ETrue if the codec supports the given format, EFalse if not."
	*/
	IMPORT_C TBool SupportsFormat(const CCompressedVideoFormat& aFormat) const;

	/**
	Lists the video formats, including submodes, supported by the decoder.
	@return "A RPointerArray table of supported video formats (CCompressedVideoFormat). The reference 
			is valid until the CVideoDecoderInfo object is destroyed."
	*/
	IMPORT_C const RPointerArray<CCompressedVideoFormat>& SupportedFormats() const;

	/**
	Returns the codec device manufacturer.
	@return "The manufacturer name as a standard Symbian descriptor. The reference is valid until the 
	CVideoDecoderInfo object is destroyed."
	*/
	IMPORT_C const TDesC& Manufacturer() const;

	/**
	Returns the codec device manufacturer-specific identifier. The combination of the manufacturer 
	and identifier uniquely identifies the hardware device.
	@return "The identifier as a standard Symbian descriptor. The reference is valid until the 
			CVideoDecoderInfo object is destroyed."
	*/
	IMPORT_C const TDesC& Identifier() const;

	/**
	Returns the decoder version.
	@return "Decoder version."
	*/
	IMPORT_C TVersion Version() const;

	/**
	Returns the decoder UID.
	@return "Decoder UID."
	*/
	IMPORT_C TUid Uid() const;

	/**
	Returns whether the decoder is hardware-accelerated. A hardware-accelerated decoder can run on 
	an application DSP or dedicated hardware.
	@return "True if the decoder is hardware-accelerated."
	*/
	IMPORT_C TBool Accelerated() const;

	/**
	Returns whether the hardware device supports output directly to the screen. Output to memory 
	buffers is always supported.
	@return "True if the hardware device supports direct screen output."
	*/
	IMPORT_C TBool SupportsDirectDisplay() const;

	/**
	Returns the maximum picture size the decoder supports.

	Note that if the decoder reports that it supports a certain profile and level, then it 
	shall support all bitstreams corresponding to that profile/level.  This method can be used 
	to specify capabilities that are beyond the standard levels (for example some MPEG-4 bitstreams 
	are encoded with picture sizes that are larger than those specified by the profile/level of the 
	bitstream).

	@return "The maximum picture size supported. The reference is valid until the CVideoDecoderInfo 
			object is destroyed."
	*/
	IMPORT_C const TSize& MaxPictureSize() const;

	/**
	Returns the maximum bit-rate supported by the decoder.

	Note that if the decoder reports that it supports a certain profile and level, then it shall 
	support all bitstreams corresponding to that profile/level.  This method can be used to 
	specify capabilities that are beyond the standard levels (for example some MPEG-4 bitstreams 
	are encoded with bit rates that are higher than those specified by the profile/level of the
	bitstream).

	@return "Maximum bit-rate supported, in bits per second. KMaxTUint32 can be used if the decoder 
			has no bit-rate restrictions."
	*/
	IMPORT_C TUint MaxBitrate() const;

	/**
	Returns the maximum picture size/rate combinations supported by the decoder.  
	
	Video decoders can have different maximum picture rate limitations depending on the picture size used.
	Note that if the decoder reports that it supports a certain profile and level, then it shall 
	support all bitstreams corresponding to that profile/level.  This method can be used to specify 
	capabilities that are beyond the standard levels (for example some MPEG-4 bitstreams are encoded
	with picture rates that are beyond those specified by the profile/level of the bitstream).

	@return "A reference to an array of picture size/rate combinations.  The reference remains valid
			until this object is deleted."
	*/
	IMPORT_C const RArray<TPictureRateAndSize>& MaxPictureRates() const;

	/**
	Returns whether the decoder supports picture loss indications. If true, the decoder indicates
	lost pictures by calling MdvpoPictureLoss().

	@return "True if the decoder supports picture loss indications."
	*/
	IMPORT_C TBool SupportsPictureLoss() const;

	/**
	Returns whether the decoder supports slice loss indications. If true, the decoder indicates 
	lost macroblocks by calling MdvpoSliceLoss().

	@return "True if the decoder supports slice loss indications."
	*/
	IMPORT_C TBool SupportsSliceLoss() const;

	/**
	Returns coding-standard specific information about the decoder.
	@return "Coding-standard specific information about the decoder. The data format is coding-standard
			specific, and defined separately. The reference is valid until the CVideoDecoderInfo object
			is destroyed."
	*/
	IMPORT_C const TDesC8& CodingStandardSpecificInfo() const;

	/**
	Returns implementation-specific information about the decoder.
	@return "Implementation- specific information about the decoder. The data format is 
			implementation-specific, and defined separately by the decoder supplier. The reference 
			is valid until the CVideoDecoderInfo object is destroyed."
	*/
	IMPORT_C const TDesC8& ImplementationSpecificInfo() const;
	
	/**
	Adds the screen number into the list of screens supported by the decoder.
	@leave	"KErrNoMemory when there is no memory to expand the list of supported screens. 
			 KErrNotSupported if the secondary screen display is not supported in Multimedia Framework."
	*/
	IMPORT_C void AddSupportedScreenL(TInt aScreenNo);
	
	/**
	Lists the screens supported by the decoder.
	@param	"aSupportedScreens"		"An array to retrieve the list of supported screens. 
									 This method resets	the array before adding elements to it. 
									 The array must be created and destroyed by the caller."
	@leave "KErrNotSupported if the secondary screen display is not supported in Multimedia Framework.
			KErrNoMemory when there is no memory to expand the list."
	*/
	IMPORT_C void GetSupportedScreensL(RArray<TInt>& aSupportedScreens) const;

	/** Sets a flag indicating whether the Decoder supports per picture content protection.
	    E.g. Where content protection within a video stream can alter.
	 @param "aSetting" "Set to TRUE to indicate decoder supports content protection. 
	 @See TVideoPicture::TVideoPictureOptions::EContentProtected 
	 @See TVideoInputBuffer::TVideoBufferOptions::EContentProtected 
	*/
	IMPORT_C void SetSupportsContentProtected(const TBool aSetting);
		
	/** Returns whether the Decoder supports per picture content protection.
	    E.g. Where content protection within a video stream can alter.
	 @return "True if the Decoder supports Content Protection." 
	 @See TVideoPicture::TVideoPictureOptions::EContentProtected
	 @See TVideoInputBuffer::TVideoBufferOptions::EContentProtected
	*/
	IMPORT_C TBool SupportsContentProtected() const;

private:
	CVideoDecoderInfo(TUid aUid,
					  TVersion aVersion,
					  TBool aAccelerated,
					  TBool aSupportsDirectDisplay,
					  const TSize& aMaxPictureSize,
					  TUint aMaxBitrate,
					  TBool aSupportsPictureLoss,
					  TBool aSupportsSliceLoss);

	void ConstructL(const TDesC& aManufacturer,
					const TDesC& aIdentifier,
					const TArray<CCompressedVideoFormat*>& aSupportedFormats,
					const TArray<TPictureRateAndSize>& aMaxPictureRates,
					const TDesC8& aCodingStandardSpecificInfo,
					const TDesC8& aImplementationSpecificInfo);
private:
	TUid iUid;
	TVersion iVersion;
	TBool iAccelerated;
	TBool iSupportsDirectDisplay;
	TSize iMaxPictureSize;
	TUint iMaxBitrate;
	TBool iSupportsPictureLoss;
	TBool iSupportsSliceLoss;
	HBufC* iManufacturer;
	HBufC* iIdentifier;
	RPointerArray<CCompressedVideoFormat> iSupportedFormats;
	RArray<TPictureRateAndSize> iMaxPictureRates;
	HBufC8* iCodingStandardSpecificInfo;
	HBufC8* iImplementationSpecificInfo;
	RArray<TInt> iSupportedScreens;
	TBool iSupportsContentProtected;
	};




/**
CMMFDevVideoPlay is the main client API for DevVideoPlay.
@publishedAll
@released
*/
class CMMFDevVideoPlay : public CBase, private MMMFDevVideoPlayProxy
	{
public:
	/**
	Picture statistic counters. Used for following playback progress. The counters can be retrieved 
	using GetPictureCounters() and are reset after each call. The client must keep track of the 
	cumulative values for counters and picture processing rates itself if necessary.
	*/
	class TPictureCounters
		{
	public:
		/**
		Default constructor.  Zeros all members.
		*/
		inline TPictureCounters();
	public:
		/**
		The number of pictures skipped due to lack of processing power. This does not include pictures 
		inside data bytes discarded due to buffer overflows, but includes all pictures skipped at 
		picture decoding, post-processing and rendering phase.
		*/
		TUint iPicturesSkipped;

		/**
		The number of pictures decoded.
		*/
		TUint iPicturesDecoded;

		/**
		The number of pictures "virtually" displayed. "Virtually" displayed pictures are pictures 
		that have been drawn on the screen, when using direct rendering, or pictures that have been 
		decoded, processed, and delivered to the client when not using direct rendering.
		*/
		TUint iPicturesDisplayed;

		/**
		The total number of pictures in the input bitstream. This figure does not include pictures that 
		have been lost due to transmission errors, since those have not been processed by the MSL 
		hardware devices, but does include pictures that have been discarded by the HW devices due 
		to buffer overflows or other reasons.
		*/
		TUint iTotalPictures;
		};

	/**
	Bitstream statistic counters, used for following decoding progress. The counters can be retrieved 
	using GetBitstreamCounters() and are reset after each call. The client must keep track of the 
	cumulative values for counters itself if necessary.
	*/
	class TBitstreamCounters
		{
	public:
		/**
		Default constructor.  Zeros all members.
		*/
		inline TBitstreamCounters();
	public:
		/**
		Number of lost packets.  This figure includes all packets that have been dropped by the hardware 
		devices due to buffer overruns, but it does not include packets lost due to transmission errors.
		*/
		TUint iLostPackets;

		/**
		Total number of packets. This figure includes all the packets that have been received by the 
		decoder, including packets that have been dropped due to buffer overruns.
		*/
		TUint iTotalPackets;
		};

	/**
	Buffer options used with SetBufferOptionsL().
	*/
	class TBufferOptions
		{
	public:
		/**
		Default constructor.  Zeros all members.
		*/
		inline TBufferOptions();

	public:
		/**
		Pre-decoder buffer size in bytes. Set to zero to use decoder default value.
		*/
		TUint iPreDecodeBufferSize;

		/**
		Maximum post-decoder buffer size in bytes. Set to zero to remove limitations.
		*/
		TUint iMaxPostDecodeBufferSize;

		/**
		Initial pre-decoder buffering period, the amount of coded data to be buffered before decoding 
		starts. If the value is set to zero, decoding begins immediately when all data associated with 
		the first decoding timestamp is received. Default value is zero.
		*/
		TTimeIntervalMicroSeconds iPreDecoderBufferPeriod;

		/**
		The amount of data buffered after the decoding before playback starts. If the value is zero, 
		playback begins immediately when the first picture has been decoded. The default value is zero.
		*/
		TTimeIntervalMicroSeconds iPostDecoderBufferPeriod;

		/**
		The maximum input buffer size that the client will request. If the buffer options have been 
		set successfully, the decoder must be able to supply buffers of this size. If no information 
		is available about the bitstream, the client may have to set this value to a relatively large 
		value, and thus the decoder should not by default allocate buffers of this size before they 
		are explicitly requested.
		*/
		TUint iMaxInputBufferSize;

		/**
		The minimum number of input buffers the decoder needs to have available. This is the number of 
		buffers the client can request through GetBufferL() before writing any back using 
		WriteCodedDataL().
		*/
		TUint iMinNumInputBuffers;
		};

	/**
	Information about a single computational complexity level.
	*/
	class TComplexityLevelInfo
		{
	public:
		enum TOptions
			{
			/** The average picture rate field is valid. */
			EAvgPictureRate	   = 0x00000001,
			/** The picture size field is valid. */
			EPictureSize		  = 0x00000002,
			/** The relative image quality field is valid. */
			ERelativeImageQuality = 0x00000004,
			/** The required MIPS field is valid. */
			ERequiredMIPS		 = 0x00000008,
			/** The relative processing time field is valid. */
			ERelativeProcessTime  = 0x00000010
			};
		/**
		Structure options. The value is a bitfield combined from values from TOptions.
		*/
		TUint32 iOptions;

		/**
		The average picture rate, in pictures per second. Valid only if EAvgPictureRate is set in the
		options. This value depends on the input bitstream, and may not be available if enough 
		bitstream has not been read.
		*/
		TReal iAvgPictureRate;

		/**
		Picture size (spatial resolution), in pixels. Valid only if EPictureSize is set in the options.
		*/
		TSize iPictureSize;

		/**
		Relative image quality, compared to the best available level. 1.0 is the quality at the 
		maximum quality level (level zero). Valid only if ERelativeImageQuality is set in the options.
		*/
		TReal iRelativeImageQuality;

		/**
		The number of MIPS required to process the data. Valid only if ERequiredMIPS is set in 
		the options.
		*/
		TUint iRequiredMIPS;

		/**
		Relative amount of processing time needed compared to standard-compliant decoding of all data. 
		1.0 is the processing time required for full processing, which usually corresponds to 
		complexity level zero. Valid only if ERelativeProcessTime is set in the options.
		*/
		TReal iRelativeProcessTime;
		};

public:

	/**
	Constructs a new MSL video client instance. Each client instance supports a single video bitstream.

	@param  aObserver
	        The observer object to use.

	@return A pointer to a new CMMFDevVideoPlay object.
	@leave  This method may leave with one of the system-wide error codes.
	*/
	IMPORT_C static CMMFDevVideoPlay* NewL(MMMFDevVideoPlayObserver& aObserver);

	/**
	Destructor.
	*/
	IMPORT_C ~CMMFDevVideoPlay();

	/**
	Finds a common format from two lists of uncompressed video formats. Used typically to find a 
	suitable intermediate format between a video decoder and a post-processor. If multiple common 
	formats are available, the lowest-cost format is selected, assuming that the cost of each format 
	is equal to its position in the input array.
	@param	"aFormats1"		"The first format list."
	@param	"aFormats2"		"The second format list."
	@param	"aCommonFormat"	"The target variable where the common format found (if any) is stored."
	@return	"True if a common format was found, false if not. If no common format was found, 
			aCommonFormat is not modified."
	*/
	IMPORT_C static TBool FindCommonFormat(const TArray<TUncompressedVideoFormat>& aFormats1, 
										   const TArray<TUncompressedVideoFormat>& aFormats2, 
										   TUncompressedVideoFormat& aCommonFormat);

	/**
	Finds all available decoders for a given video type with support for certain post-processing 
	operations. The video type is specified using its MIME type, which may include parameters 
	specifying the supported level, version, and other information. Decoder HW devices can use 
	wildcards when listing the supported video types in the ECom registration information, so it is 
	possible that all the decoders returned do not support the specified submode, e.g. profile and 
	level, unless aExactMatch is set.
	The decoder capabilities can be checked with VideoCodecInfoLC().
 
	@param	"aMimeType"		"The video type that will be decoded."
	@param	"aPostProcType"	"The post-processing types that the decoder has to support, a binary OR 
							of TPrePostProcessType values. If no post-processing support is needed, 
							set this value to zero."
	@param	"aDecoders"		"An array for the result decoder UIDs. The array must be created and 
							destroyed by the caller."
	@param	"aExactMatch"	"True if exact matching should be used. In this only decoders that support 
							exactly the MIME-type given will be returned. Since verifying this may 
							require loading the decoders into memory, this can be a fairly expensive 
							operation, and if the user needs to verify their capabilities further in 
							any case this parameter should be set to EFalse."
	@leave	"This method may leave with one of the system-wide error codes. Typical error codes used:
				* KErrNotFound:	No decoders were found matching the search parameters."
	*/
	IMPORT_C void FindDecodersL(const TDesC8& aMimeType, 
								TUint32 aPostProcType, 
								RArray<TUid>& aDecoders, 
								TBool aExactMatch=ETrue);

	/**
	Finds all available post-processors for a given set of post-processing operations.
	@param	"aPostProcType"		"The post-processing types that the hardware device has to support, 
								a binary OR of TPrePostProcessType values."
	@param	"aPostProcessors"	"An array for the result post-processor UIDs. The array must be 
								created and destroyed by the caller."
	@leave	"This method may leave with one of the system-wide error codes. Typical error codes used:
				* KErrNotFound:	No post-processors were found matching the search parameters."
	*/
	IMPORT_C void FindPostProcessorsL(TUint32 aPostProcType, RArray<TUid>& aPostProcessors);

	/**
	Retrieves a list of available video decoders in the system.
	@param	"aDecoders"	"An array for the result decoder UIDs. The array must be created and 
						destroyed by the caller."
	@leave	"This method may leave with one of the system-wide error codes.  Not finding any decoders 
			is not treated as an error condition: in this situation, aDecoders will be empty."
	*/
	IMPORT_C void GetDecoderListL(RArray<TUid>& aDecoders);

	/**
	Retrieves a list of available video post-processors in the system.

	@param  aPostProcessors
	        An array for the result post-processor UIDs. The array must be created and
	        destroyed by the caller.

	@leave  This method may leave with one of the system-wide error codes.  Not finding any post-processors
	        is not treated as an error condition: in this situation, aDecoders will be empty.
	*/
	IMPORT_C void GetPostProcessorListL(RArray<TUid>& aPostProcessors);

	/**
	Retrieves information about an installed video decoder. Note that this method will need to load 
	the codec hardware device into memory, and can thus be relatively expensive.
	@param	"aVideoDecoder"	"The video decoder to query."
	@return "Decoder information as a CVideoDecoderInfo object. The object is pushed to the cleanup
			stack, and must be deallocated by the caller."
	@leave  "This method may leave with one of the system-wide error codes."
	*/
	IMPORT_C CVideoDecoderInfo* VideoDecoderInfoLC(TUid aVideoDecoder);

	/**
	Retrieves information about the post-processing capabilities of an installed post-processor or 
	decoder hardware device. Note that this method will need to load the device into memory, and can 
	thus be relatively expensive.
	@param	"aPostProcessor"	"The post-processor to query."
	@return "Post-processor information as a CPostProcessorInfo object. The object is pushed to the 
			cleanup stack, and must be deallocated by the caller."
	@leave  "This method may leave with one of the system-wide error codes."
	*/
	IMPORT_C CPostProcessorInfo* PostProcessorInfoLC(TUid aPostProcessor);

	/**
	Selects the video decoder to be used. This method must be called before any other video decoder 
	related methods are used. The decoder to use can be changed by calling this method again before 
	the API has been initialized with Initialize().
	All video decoder settings are reset to their default values, which are up to the implementation 
	to decide if not specified in any of the MSL specifications. By default no post-processing is 
	performed.

	@param  "aDecoder" "The video decoder to use."
	@return "Hardware device ID, used in other methods for configuring the decoder."
	@leave  "The method will leave if an error occurs. Typical error codes used:
			* KErrNotFound - No decoder was found with the given UID"
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C THwDeviceId SelectDecoderL(TUid aDecoder);

	/**
	Selects the video post-processor to be used. This method must be called before any other 
	post-processor related methods are used. The post-processor to use can be changed by calling
	this method again before the API has been initialized with Initialize().
	All post-processor settings are reset to their default values, which are up to the implementation
	to decide if not specified in any of the MSL specifications.

	@param  "aPostProcessor" "The post-processor to use."
	@return "Hardware device ID, used in other methods for configuring the post-processor."
	@leave  "The method will leave if an error occurs. Typical error codes used:
				* KErrNotFound - No post-processor was found with the given UID"
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C THwDeviceId SelectPostProcessorL(TUid aPostProcessor);

	/**
	Reads header information from a coded data unit. [1 #59] This method can be called only after 
	SelectDecoderL().
	@param	"aDataUnitType"				"The type of coded data unit that is contained in aDataUnit. If the 
										data is a simple piece of bitstream, use 
										EDuArbitraryStreamSection."
	@param	"aDataUnitEncapsulation"	"The encapsulation type used for the coded data. If the data 
										is a simple piece of bitstream, use EDuElementaryStream."
	@param	"aDataUnit"					"The coded data unit, contained in a TVideoInputBuffer."
	@return "Header information for the data unit, or NULL if the coded data unit did not contain 
			enough data to parse the header. The header data must be returned to the API using 
			ReturnHeader() before the API is shut down or the decoder is changed. The data remains 
			valid until it is returned."
	@leave	"The method will leave if an error occurs. Running out of data is not considered an error, 
			as described above. Typical error codes used:
				* KErrNotSupported - The data is not in a supported format.
				* KErrCorrupt - The data appears to be in a supported format, but is corrupted."
	*/
	IMPORT_C TVideoPictureHeader* GetHeaderInformationL(TVideoDataUnitType aDataUnitType, 
														TVideoDataUnitEncapsulation aDataUnitEncapsulation, 
														TVideoInputBuffer* aDataUnit);

	IMPORT_C void ConfigureDecoderL(const TVideoPictureHeader& aVideoPictureHeader);

	/**
	Returns a header from GetHeaderInformationL() back to the decoder so that the memory can be freed.
	@param	"aHeader"	"The header to return."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void ReturnHeader(TVideoPictureHeader* aHeader);

	/**
	Sets a hardware device input format to an uncompressed video format. Only post-processors support 
	uncompressed video input.
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from
						SelectPostProcessorL() when the device is selected."
	@param	"aFormat"	"The input format to use."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The input format is not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetInputFormatL(THwDeviceId aHwDevice, const TUncompressedVideoFormat& aFormat);

	/**
	Sets a hardware device Input format to a compressed video format. Only decoders support compressed 
	video input.
	@param	"aHwDevice"			"The hardware device to configure. The value is returned from 
								SelectDecoderL() when the device is selected."
	@param	"aFormat"			"The input format to use."
	@param	"aDataUnitType"		"The data unit type for input data."
	@param	"aEncapsulation"	"The encapsulation type used for the coded data."
	@param	"aDataInOrder"		"True if the input data is written in correct decoding order, false if 
								the input data may be written in arbitrary order."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The input format is not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetInputFormatL(THwDeviceId aHwDevice, 
								  const CCompressedVideoFormat& aFormat, 
								  TVideoDataUnitType aDataUnitType, 
								  TVideoDataUnitEncapsulation aEncapsulation, 
								  TBool aDataInOrder);

	/**
	Retrieves the list of the output formats a hardware device supports. The list is ordered in 
	preference order, with the preferred formats at the beginning of the list. The list can depend 
	on the device source format, and therefore SetSourceFormatL() must be called for the device 
	before calling this method.
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectDecoderL() 
						or SelectPostProcessorL() when the device is selected."
	@param	"aFormats"	"An array for the result format list. The array must be created and destroyed 
						by the caller."
	@leave	"This method may leave with one of the system-wide error codes."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetOutputFormatListL(THwDeviceId aHwDevice, RArray<TUncompressedVideoFormat>& aFormats);

	/**
	Sets the output format for a hardware device. If a decoder and a post-processor are used, the 
	decoder output format must match the post-processor source format.

    If direct screen access is being used, then it is not necessary to call this method on the 
	hwdevice performing the direct screen access.
	
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectDecoderL() 
						or SelectPostProcessorL() when the device is selected."
	@param	"aFormat"	"The format to use."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The output format is not supported."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetOutputFormatL(THwDeviceId aHwDevice, const TUncompressedVideoFormat &aFormat);

	/**
	Sets the clock source to use for video timing. When video playback is synchronized with audio, the 
	clock source is implemented by the audio playback subsystem, otherwise the clock source should get 
	the time from the system clock. If no clock source is set, video playback will not be synchronized,
	but will proceed as fast as possible, depending on input data and output buffer availability. This 
	method can be called after all hardware devices have been selected, but before calling Initialize().

	All decoders must support synchronization with an external clock source, as well as unsynchronized 
	non-realtime operation. When a clock source is set, the decoder can skip pictures to maintain 
	synchronization. When non-realtime operation is used and no clock source has been set, pictures 
	may not be skipped unless a lower complexity level is used that requires this.

	@param	"aClock"	"The clock source to use"
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetClockSource(MMMFClockSource* aClock);

	/**
	Sets the video output destination. The destination can be the screen (using direct screen access) 
	or memory buffers. By default memory buffers are used. This method must be called after the 
	decoder and post-processor have been selected and their options set, since support for direct 
	screen access can vary between hardware devices.
	@param	"aScreen"	"True if video output destination is the screen, false if memory buffers."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The selected video destination is not supported with the 
				  current codec and post-processor settings."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetVideoDestScreenL(TBool aScreen);

	/**
	Sets whether the decoder should synchronize decoding to the current clock source, if any, or 
	should decode all pictures as soon as possible. If decoding is synchronized, decoding timestamps 
	are used if available, presentation timestamps are used if not. When decoding is not synchronized, 
	pictures are decoded as soon as source data is available for them and the decoder has a free 
	output buffer. If a clock source is not available, decoding will not be synchronized.

	@param	"aSynchronize"	"True if decoding should be synchronized to a clock source."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SynchronizeDecoding(TBool aSynchronize);

	/**
	Sets video decoder buffering options.
	@param	"aOptions"	"Buffering options."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The specified buffering options are not supported. If the 
				  client receives this error code, it can call GetBufferOptions() to determine 
				  the options the decoder is able to support."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetBufferOptionsL(const TBufferOptions& aOptions);

	/**
	Gets the video decoder buffer options actually in use. This can be used before calling 
	SetBufferOptionsL() to determine the default options, or afterwards to check the values actually 
	in use (if some default values were used).
	@param	"aOptions"	"Buffering options structure to fill."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetBufferOptions(TBufferOptions& aOptions);

	/**
	Indicates which HRD/VBV specification is fulfilled in the input stream and any related parameters.
	@param	"aHrdVbvSpec"	"The HRD/VBV specification fulfilled, see the definition of 
							THrdVbvSpecification for details."
	@param	"aHrdVbvParams"	"HRD/VBV parameters. The data format depends on the parameters chosen. 
							For 3GPP TS 26.234 parameters (aHrdVbvSpec=EHrdVbv3GPP), the data in the 
							descriptor is a package of type TPckC<T3gppHrdVbvParams> 
							(see T3gppHrdVbvParams). If no HRD/VBV parameters are used, the descriptor 
							is empty."
	@pre	"This method can only be called before the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetHrdVbvSpec(THrdVbvSpecification aHrdVbvSpec, const TDesC8& aHrdVbvParams);

	/**
	Sets the post-processing types to be used. This method, like all post-processing configuration 
	methods, must be called before Initialize().

	Post-processing operations are carried out in the following order:
		1. Input cropping
		2. Mirroring
		3. Rotating
		4. Scaling
		5. Output cropping
	Color space conversion can be performed at any point in the post-processing flow.

	@param	"aHwDevice"				"The hardware device to configure. The value is returned from 
									SelectDecoderL() or SelectPostProcessorL() when the device is 
									selected."
	@param	"aPostProcCombination"	"The post-processing steps to perform, a bitwise OR of values 
									from TPrePostProcessType."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The post-processing combination is not supported"
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetPostProcessTypesL(THwDeviceId aHwDevice, TUint32 aPostProcCombination);

	/**
	Sets post-processing options for input (pan-scan) cropping. SetPostProcessTypesL() must be called 
	before this method is used.
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectDecoderL()
						or SelectPostProcessorL() when the device is selected."
	@param	"aRect"		"The cropping rectangle to use."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - Pan-scan cropping is not supported"
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetInputCropOptionsL(THwDeviceId aHwDevice, const TRect& aRect);

	/**
	Sets post-processing options for YUV to RGB color space conversion. Specifies the input YUV and 
	output RGB formats to use explicitly. SetSourceFormatL() and SetOutputFormatL(), and 
	SetPostProcessTypesL() must be called before this method is used.
	@param	"aHwDevice"		"The hardware device to configure. The value is returned from 
							SelectDecoderL() or SelectPostProcessorL() when the device is selected."
	@param	"aOptions"		"The conversion options to use."
	@param	"aYuvFormat"	"Conversion source YUV format."
	@param	"aRgbFormat"	"Conversion target RGB format."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The conversion options are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetYuvToRgbOptionsL(THwDeviceId aHwDevice, 
									  const TYuvToRgbOptions& aOptions, 
									  const TYuvFormat& aYuvFormat, 
									  TRgbFormat aRgbFormat);

	/**
	Sets post-processing options for YUV to RGB color space conversion. Uses the device input and 
	output formats. For decoders the default YUV format used is the format specified in the input 
	bitstream. SetSourceFormatL() and SetOutputFormatL(), and SetPostProcessTypesL() must be called 
	before this method is used.
	@param	"aHwDevice"		"The hardware device to configure. The value is returned from 
							SelectDecoderL() or SelectPostProcessorL() when the device is selected."
	@param	"aOptions"		"The conversion options to use."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The conversion options are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetYuvToRgbOptionsL(THwDeviceId aHwDevice, const TYuvToRgbOptions& aOptions);

	/**
	Sets post-processing options for rotation. SetPostProcessTypesL() must be called before this 
	method is used.
	@param	"aHwDevice"		"The hardware device to configure. The value is returned from 
							SelectDecoderL() or SelectPostProcessorL() when the device is selected."
	@param	"aRotationType"	"The rotation to perform."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The rotation options are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetRotateOptionsL(THwDeviceId aHwDevice, TRotationType aRotationType);

	/**
	Sets post-processing options for scaling. SetPostProcessTypesL() must be called before this method 
	is used.
	@param	"aHwDevice"				"The hardware device to configure. The value is returned from 
									SelectDecoderL() or SelectPostProcessorL() when the device is 
									selected."
	@param	"aTargetSize"			"Scaling target size. If a fixed scale factor size is used, the 
									new dimensions must be set to width=floor(factor*width), 
									height=floor(factor*height). For example, scaling a QCIF (176x144) 
									picture up by a factor of 4/3 yields a size of 234x192."
	@param	"aAntiAliasFiltering"	"True if anti-aliasing filtering should be used. If the 
									post-processor does not support anti-aliased scaling, or supports 
									anti-aliased scaling only, this argument is ignored."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The scale options are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetScaleOptionsL(THwDeviceId aHwDevice, 
								   const TSize& aTargetSize, 
								   TBool aAntiAliasFiltering);

	/**
	Sets post-processing options for output cropping. SetPostProcessTypesL() must be called before this
	method is used.
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectDecoderL()
						# SelectPostProcessorL() when the device is selected."
	@param	"aRect"		"Output cropping area."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - Output cropping is not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetOutputCropOptionsL(THwDeviceId aHwDevice, const TRect& aRect);

	/**
	Sets post-processing hardware device specific options. SetPostProcessTypesL() must be called 
	before this method is used.
	@param	"aHwDevice"	"The hardware device to configure. The value is returned from SelectDecoderL()
						or SelectPostProcessorL() when the device is selected."
	@param	"aOptions"	"The options. The format is plug-in specific."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				* KErrNotSupported - The options are not supported."
	@pre	"This method can be called either before or after the API has been initialized with Initialize().
			If called after initialization, the change will only be committed once CommitL() is called."
	*/
	IMPORT_C void SetPostProcSpecificOptionsL(THwDeviceId aHwDevice, const TDesC8& aOptions);

	/**
	Initializes the video device. This method is asynchronous, DevVideoPlay will call 
	MMMFDevVideoPlayObserver::MdvpoInitializeComplete() after initialization has completed. No 
	DevVideoPlay method may be called while initialization is in progress, the initialization process 
	can only be cancelled by destroying the DevVideoPlay object. After this initialization has been 
	successfully completed, further configuration changes are not possible except where separately 
	noted.

	If initialization fails for any reason, the DevVideoPlay object must be destroyed and set up from
	scratch.
	*/
	IMPORT_C void Initialize();

	/**
	Commit all configuration changes since the last CommitL(), Revert() or Initialize(). This only applies 
	to methods that can be called both before AND after DevVideoPlay has been initialized. 
	See the following methods for details.

	@see	SetPostProcessTypesL
	@see	SetInputCropOptionsL
	@see	SetYuvToRgbOptionsL
	@see	SetRotateOptionsL
	@see	SetScaleOptionsL
	@see	SetOutputCropOptionsL
	@see	SetPostProcSpecificOptionsL

	@leave  "The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void CommitL();

	/**
	Revert any configuration changes that have not yet been committed using CommitL().  This only applies 
	to methods that can be called both before AND after DevVideoPlay has been initialized. 
	See the following methods for details.

	@see	SetPostProcessTypesL
	@see	SetInputCropOptionsL
	@see	SetYuvToRgbOptionsL
	@see	SetRotateOptionsL
	@see	SetScaleOptionsL
	@see	SetOutputCropOptionsL
	@see	SetPostProcSpecificOptionsL

	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Revert();

	/**
	Starts video decoding directly to the display frame buffer using Direct Screen Access. 
	Playback to the display can start immediately.
	@param	"aVideoRect"	"The video output rectangle on screen. The rectangle size should be the 
							same as the output video size, otherwise results are undefined. Some 
							decoders may set limitations to the position and alignment of the 
							rectangle. The rectangle must be aligned to a 32-bit boundary on screen 
							(even X-coordinate on 12-bpp and 16-bpp displays) and must fit completely 
							on the screen."
	@param	"aScreenDevice"	"The screen device to use. The screen device will supply frame buffer 
							information, and is used to update the frame buffer to screen. The screen 
							device object must be valid in the current thread."
	@param	"aClipRegion"	"Initial clipping region to use."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				*KErrNotSupported - Direct Screen Access is not supported"
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void StartDirectScreenAccessL(const TRect& aVideoRect, 
										   CFbsScreenDevice& aScreenDevice, 
										   const TRegion& aClipRegion);

	/**
	Sets a new clipping region for Direct Screen Access. After the method returns, no video will be 
	drawn outside of the region. (Note that in Symbian OS clipping regions are "positive" - that is, 
	they define the legal area inside which an application may draw.) 

	If clipping is not supported, or the clipping region is too complex, either playback will pause 
	or will resume without video display, depending on the current setting of SetPauseOnClipFail(), 
	and the result can be verified with IsPlaying(). Clipping can be disabled by setting a new 
	clipping region that includes the whole video window.

	@param	"aRegion"	"The new clipping region. After the method returns, no video will be drawn 
						outside the region."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetScreenClipRegion(const TRegion& aRegion);

	/**
	Sets whether the system should pause playback when it gets a clipping region it cannot handle, 
	or Direct Screen Access is aborted completely. If not, decoding will proceed normally, but no 
	video will be drawn. By default, playback is paused.
	@param	"aPause"	"True if playback should be paused when clipping fails, false if not. If 
						playback is not paused, it will be continued without video display."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetPauseOnClipFail(TBool aPause);

	/**
	Aborts Direct Screen Access completely, to be called from MAbortDirectScreenAccess::AbortNow() and 
	similar methods. DSA can be resumed by calling StartDirectScreenAccessL().
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void AbortDirectScreenAccess();

	/**
	Indicates whether playback is proceeding. This method can be used to check whether playback was 
	paused or not in response to a new clipping region or DSA abort.
	@return "ETrue if video is still being played (even if not necessarily displayed)."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TBool IsPlaying();

	/**
	Re-draws the latest video picture. Only available when DSA is being used. If DSA is aborted 
	or a non-supported clipping region has been set, the request may be ignored.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Redraw();

	/**
	Starts video playback, including decoding, post-processing, and rendering. Playback will proceed 
	until it has been stopped or paused, or the end of the bitstream is reached.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Start();

	/**
	Stops video playback. No new pictures will be decoded, post-processed, or rendered.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Stop();

	/**
	Pauses video playback, including decoding, post-processing, and rendering. No pictures will be 
	decoded, post-processed, or rendered until playback has been resumed. The client is responsible 
	for pausing the clock source (typically by pausing audio output) if necessary.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Pause();

	/**
	Resumes video playback after a pause. The client is responsible for restarting the clock source 
	if necessary.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void Resume();

	/**
	Changes to a new decoding and playback position, used for randomly accessing (seeking) the input 
	stream. The position change flushes all input and output buffers. Pre-decoder and post-decoder 
	buffering are handled as if a new bitstream was started. If the MSL video subsystem still has 
	buffered pictures that precede the new playback position, they will be discarded. If playback is 
	synchronized to a clock source, the client is responsible for setting the clock source to the new 
	position.
	@param	"aPlaybackPosition"	"The new playback position in the video stream."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition);

	/**
	Freezes a picture on the screen. After the picture has been frozen, no picture is displayed until 
	the freeze is released with ReleaseFreeze(). If the video output is being written to memory 
	buffers, not the screen, decoded pictures will not be delivered to the client when a freeze is 
	active, but are simply discarded.
	@param	"aPlaybackPosition"	"The presentation timestamp of the picture to freeze. The frozen 
								picture will be the first picture with a timestamp greater than or 
								equal to this parameter."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void FreezePicture(const TTimeIntervalMicroSeconds& aPlaybackPosition);

	/**
	Releases a picture frozen with FreezePicture().
	@param	"aPlaybackPosition"	"The presentation timestamp of the picture to release. The first 
								picture displayed after the release will be the first picture with a 
								timestamp greater than or equal to this parameter. To release the 
								freeze immediately, set the timestamp to zero."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void ReleaseFreeze(const TTimeIntervalMicroSeconds& aPlaybackPosition);

	/**
	Returns the current decoding position, i.e. the timestamp for the most recently decoded picture.
	@return "Current decoding position."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TTimeIntervalMicroSeconds DecodingPosition();

	/**
	Returns the current playback position, i.e. the timestamp for the most recently displayed or 
	virtually displayed picture.
	@return "Current playback position."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TTimeIntervalMicroSeconds PlaybackPosition();
	
	/**
	Returns the current pre-decoder buffer size.
	@return "The number of bytes of data in the pre-decoder buffer."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TUint PreDecoderBufferBytes();

	/**
	Returns the total amount of memory allocated for uncompressed pictures.
	@return "Total number of bytes of memory allocated for uncompressed pictures."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TUint PictureBufferBytes();

	/**
	Reads various counters related to decoded pictures. See the definition of TPictureCounters for a 
	description of the counters. The counters are reset when Initialize() or this method is called, 
	and thus they only include pictures processed since the last call.
	@param	"aCounters"	"The counter structure to fill."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetPictureCounters(TPictureCounters& aCounters);

	/**
	Reads various counters related to the received input bitstream and coded data units. See the 
	definition of  TBitstreamCounters for a description about the counters. The counters are reset 
	when Initialize() or this method is called, and thus they only include data processed since the 
	last call.
	@param	"aCounters"	"The counter structure to fill."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetBitstreamCounters(TBitstreamCounters& aCounters);

	/**
	Retrieves the number of free input buffers the decoder has available. If only a post-processor 
	is used, the return value is undefined.
	@return "Number of free input buffers the decoder has available."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TUint NumFreeBuffers();

	/**
	Sets the computational complexity level to use. The level can be set separately for each hardware 
	device in use. If separate complexity levels are not available, the method call is ignored. If the 
	level specified is not available, the results are undefined. Typically the device will either 
	ignore the request or use the nearest suitable level.

	The complexity level can be changed at any point during playback.

	@param	"aHwDevice"	"The plug-in to control. The value is returned from SelectDecoderL()or 
						SelectPostProcessorL() when the device is selected."
	@param	"aLevel"	"The computational complexity level to use. Level zero (0) is the most 
						complex one, with the highest quality. Higher level numbers require less 
						processing and may have lower quality."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void SetComplexityLevel(THwDeviceId aHwDevice, TUint aLevel);

	/**
	Gets the number of complexity levels available.
	@param	"aHwDevice"	"The plug-in to query. The value is returned from SelectDecoderL()or 
						SelectPostProcessorL() when the device is selected."
	@return	"The number of complexity control levels available, or zero if the information is not 
			available yet. The information may not be available if the number of levels depends on 
			the input data, and enough input data has not been read yet. In that case, using level 
			zero is safe."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TUint NumComplexityLevels(THwDeviceId aHwDevice);

	/**
	Gets information about a computational complexity level. This method can be called after 
	NumComplexityLevels() has returned a non-zero value - at that point the information is guaranteed 
	to be available. Some hardware device implementations may not be able to provide all values, 
	in that case the values will be approximated.
	@param	"aHwDevice"	"The hw device to query. The value is returned from SelectDecoderL()or 
						SelectPostProcessorL() when the device is selected."
	@param	"aLevel"	"The computational complexity level to query. The level numbers range from 
						zero (the most complex) to NumComplexityLevels()-1."
	@param	"aInfo"		"The information structure to fill."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetComplexityLevelInfo(THwDeviceId aHwDevice, TUint aLevel, TComplexityLevelInfo& aInfo);

	/**
	Retrieves an empty video input buffer from the decoder. After input data has been written to the 
	buffer, it can be written to the decoder using WriteCodedDataL(). The maximum number of buffers 
	the client can retrieve before returning any to the decoder is determined by the 
	TBufferOptions.iMinNumInputBuffers value set with SetBufferOptionsL().

	If a buffer is not immediately available, the client can wait for a MdvpoNewBuffers() callback 
	before trying again, or poll for new buffer availability after a delay. Note that video decoding 
	may be performed in the same thread, in a separate active object, so the client must not block 
	the active scheduler while waiting for buffers to become available, otherwise the system can 
	deadlock. Assuming that the client does not keep too many buffers retrieved and playback is in 
	progress, new buffers will be available after some time.

	The decoder maintains ownership of the buffers even while they have been retrieved by the client, 
	and will take care of deallocating them. The client must not destroy the buffer objects, even if 
	it has retrieved buffers and it is being shut down. The buffers will be destroyed by the decoder, 
	and will not be valid after the decoder has been shut down.

	@param	"aBufferSize"	"Required buffer size, in bytes. The resulting buffer can be larger than 
							this, but not smaller"
	@return	"A new input data buffer. The buffer is at least as large as requested, but it may be 
			larger. If no free buffers are available, the return value is NULL."
	@leave	"The method will leave if an error occurs. Lack of free buffers is not considered an error."
	*/
	IMPORT_C TVideoInputBuffer* GetBufferL(TUint aBufferSize);

	/**
	Writes a piece of coded video data. The data will be passed to the video decoder and 
	post-processor to be used in video playback. The data buffer must be retrieved from the decoder 
	with GetBufferL().
	@param	"aBuffer"	"The coded data unit to write."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void WriteCodedDataL(TVideoInputBuffer* aBuffer);

	/**
	Writes an uncompressed video picture. The picture must remain valid and unmodified until it is 
	returned with the MdvpoReturnPicture() callback.
	@param	"aPicture"	"The picture to write."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void WritePictureL(TVideoPicture* aPicture);

	/**
	Notifies the system that the end of input data has been reached.
	
	The decoder and post-processor can use this signal to ensure that the remaining data gets processed,
	without waiting for new data.  For example when the data type is not EDuCodedPicture, calling this
	method is necessary otherwise a decoder implementation might be waiting for the start code for the
	next picture to ensure it has a complete picture before starting to decode the previous one. 
	
	  
	After the remaining data has been processed (and displayed, if applicable), the client gets 
	notified with the MdvpoStreamEnd() callback.  The client must then call Stop() on the interface.

	DevVideo clients are encouraged to call this method, but its use is not mandatory for synchronized 
	processing.  For synchronized playback, all video pictures are processed or discarded according to
	their timestamps, and so the client can easily infer when processing is complete.  However, it 
	should be noted that the last picture might not be displayed if this method is not called and the
	input data type is not EDuCodedPicture.

	For non-synchronized playback (e.g. file conversion), a client must call this method otherwise it
	will never find out when the hardware device has finished processing the data.

	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void InputEnd();

	/**
	Gets information about new decoded pictures. "New decoded pictures" are pictures that have not 
	been returned to the caller using NextPicture(). This method can only be called if video 
	destination is memory buffers, i.e. Direct Screen Access is not used.
	@param	"aNumPictures"			"Target for the number of new decoded pictures. If no pictures are 
									available, the value written is zero, and the timestamps are not 
									valid."
	@param	"aEarliestTimestamp"	"Timestamp for the earliest available new decoded picture. If the 
									number of new decoded pictures (aNumPictures) is zero, the 
									timestamp is not valid."
	@param	"aLatestTimestamp"		"Timestamp for the latest available new decoded picture. If the 
									number of new decoded pictures (aNumPictures) is zero, the 
									timestamp is not valid."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetNewPictureInfo(TUint& aNumPictures, 
									TTimeIntervalMicroSeconds& aEarliestTimestamp, 
									TTimeIntervalMicroSeconds& aLatestTimestamp);

	/**
	Gets the next (in displaying order) new decoded picture. Only one picture can be retrieved at a 
	time, the current picture must be returned with ReturnPicture() before a new one can be read.
	@return	"The next new decoded picture. The pointer and the related data is valid until the picture 
			is returned using ReturnPicture(), or the API is shut down. If no new picture is available,
			the method returns NULL."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TVideoPicture* NextPictureL();

	/**
	Returns a picture from NextPicture() back to the device. The picture data cannot be used afterwards.
	@param	"aPicture"	"The picture to return. After this method returns, the pointer is no longer
						valid and the picture data cannot be used."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void ReturnPicture(TVideoPicture* aPicture);

	/**
	Gets a copy of the latest picture displayed.
	@param	"aPictureData"	"Target picture. The memory for the picture must be allocated by the 
							caller, and initialized properly. The data formats must match the snapshot 
							format requested."
	@param	"aFormat"		"The picture format to use for the snapshot."
	@return "ETrue if the snapshot was taken, EFalse if a picture is not available. The picture may not 
			be available if decoding has not progressed far enough yet."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				*KErrNotSupported - The requested data format or picture size is not supported."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C TBool GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat);

	/**
	Gets a copy of a specified picture. Timed snapshots are required for implementing the video 
	telephony use-case. Simply using the latest picture may not work, since the latest picture 
	is typically not the one whose supplemental information is being processed.

	The picture is specified using its presentation timestamp. The timestamp must match the timestamp 
	in the picture exactly, otherwise no snapshot will be taken. The timestamp must refer to the 
	currently displayed picture or a future picture. Typically the timestamp is received with 
	supplemental information (with the MdvpoSupplementalInformation() callback) indicating a snapshot 
	picture.

	When the snapshot is available, it will be returned to the client using the 
	MdvpoTimedSnapshotComplete() callback. To cancel a timed snapshot request, use 
	CancelTimedSnapshot(). Only one timed snapshot request can be active at a time.

	@param	"aPictureData"				"Target picture. The memory for the picture must be allocated 
										by the caller, and initialized properly. The data formats must 
										match the snapshot format requested. The picture must remain 
										valid until the snapshot has been taken or until the request 
										has been cancelled with CancelTimedSnapshot()."
	@param	"aFormat"					"The picture format to use for the snapshot."
	@param	"aPresentationTimestamp"	"Presentation timestamp for the picture to copy. The timestamp 
										must match the timestamp in the picture exactly"
	@leave	"The method will leave if an error occurs. Typical error codes used:
				*KErrNotSupported - The requested data format or picture size is not supported."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetTimedSnapshotL(TPictureData* aPictureData, 
									const TUncompressedVideoFormat& aFormat, 
									const TTimeIntervalMicroSeconds& aPresentationTimestamp);

	/**
	Gets a copy of a specified picture. Timed snapshots are required for implementing the video 
	telephony use-case. Simply using the latest picture may not work, since the latest picture 
	is typically not the one whose supplemental information is being processed.

	The picture is specified using either its picture identifier. The id must refer to the currently 
	displayed picture or a future picture. Typically the picture ID is received with supplemental 
	information (with the MdvpoSupplementalInformation() callback) indicating a snapshot picture.

	When the snapshot is available, it will be returned to the client using the 
	MdvpoTimedSnapshotComplete() callback. To cancel a timed snapshot request, use 
	CancelTimedSnapshot(). Only one timed snapshot request can be active at a time.

	@param	"aPictureData"	"Target picture. The memory for the picture must be allocated by the 
							caller, and initialized properly. The data formats must match the snapshot 
							format requested. The picture must remain valid until the snapshot has been
							taken or until the request has been cancelled with CancelTimedSnapshot()."
	@param	"aFormat"		"The picture format to use for the snapshot."
	@param	"aPictureId"	"Picture identifier for the picture to copy."
	@leave	"The method will leave if an error occurs. Typical error codes used:
				*KErrNotSupported - The requested data format or picture size is not supported."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetTimedSnapshotL(TPictureData* aPictureData, 
									const TUncompressedVideoFormat& aFormat, 
									const TPictureId& aPictureId);

	/**
	Cancels a timed snapshot request.
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void CancelTimedSnapshot();

	/**
	Gets a list of the supported snapshot picture formats.
	@param	"aFormats"	"An array for the result format list. The array must be created and destroyed by the caller."
	@leave	"The method will leave if an error occurs."
	@pre	"This method can only be called after the API has been initialized with Initialize()."
	*/
	IMPORT_C void GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& aFormats);

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
	CMMFDevVideoPlay(MMMFDevVideoPlayObserver& aObserver);

	// Methods to check aHwDevice is valid and return the appropriate HwDevice
	CMMFVideoPlayHwDevice& VideoPlayHwDevice(THwDeviceId aHwDevice) const;
	CMMFVideoPlayHwDevice& RenderingHwDevice() const; //returns the last plugin in the chain
	CMMFVideoDecodeHwDevice& VideoDecodeHwDevice(THwDeviceId aHwDevice) const;// Checks that aHwDevice is valid
	CMMFVideoPostProcHwDevice& VideoPostProcHwDevice(THwDeviceId aHwDevice) const;// Checks that aHwDevice is valid
	CMMFVideoDecodeHwDevice& VideoDecodeHwDevice() const;
	CMMFVideoPostProcHwDevice& VideoPostProcHwDevice() const;
	CMMFVideoHwDevice& VideoHwDevice(THwDeviceId aHwDevice) const;

	// Connects the plugins together
	void ConnectPlugins();

	// Check that we are in a valid initialization state
	// Panics if iInitializationState is not one of aExpected
	void CheckInitializationState(TUint aExpected);

	// Methods to handle init complete callbacks from the hw devices
	void HandlePostProcInitializeComplete(TInt aError);
	void HandleDecodeInitializeComplete(TInt aError);
		
	CMMFVideoDecodeHwDevice* CreateDecoderL(TUid aVideoDecoder);


	// From MMMFDevVideoPlayProxy
	virtual void MdvppNewPicture(TVideoPicture* aPicture);
	virtual void MdvppNewBuffers();
	virtual void MdvppReturnPicture(TVideoPicture* aPicture);
	virtual void MdvppSupplementalInformation(const TDesC8& aData, 
											  const TTimeIntervalMicroSeconds& aTimestamp, 
											  const TPictureId& aPictureId);
	virtual void MdvppPictureLoss();
	virtual void MdvppPictureLoss(const TArray<TPictureId>& aPictures);
	virtual void MdvppSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);
	virtual void MdvppReferencePictureSelection(const TDesC8& aSelectionData);
	virtual void MdvppTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId);
	virtual void MdvppFatalError(CMMFVideoHwDevice* aDevice, TInt aError);
	virtual void MdvppInitializeComplete(CMMFVideoHwDevice* aDevice, TInt aError);
	virtual void MdvppStreamEnd();
	
private:
	MMMFDevVideoPlayObserver& iObserver;
	CMMFVideoDecodeHwDevice* iVideoDecodeHwDevice;
	CMMFVideoPostProcHwDevice* iVideoPostProcHwDevice;
	TUint iInitializationState;

	TDblQue<TVideoPicture> iVideoPictureQue;
	TDblQueIter<TVideoPicture> iVideoPictureQueIter;
	TUint iNumberOfVideoPictures;
	// Not to be used, but kept for BC purposes
	TInt iReserved;

	TUint iNumberOfMdvppStreamEndCallbacks;
	TBool iIsDecodeComplete;
	
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	RImplInfoPtrArray iPuImplementations;
	TBool iPuListCreated;
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	};


/**
The MMMFDevVideoPlayObserver mixin class defines a set of callback functions that the client using 
the MSL video API needs to implement. The callbacks are called in the context of the same thread that 
uses the API. DevVideo API methods can be safely called from the callbacks.
@publishedAll
@released
*/
class MMMFDevVideoPlayObserver
	{
public:
	/**
	Notifies the client that one or more new empty input buffers are available. The client can fetch 
	the input buffer with GetBufferL().
	*/
	virtual void MdvpoNewBuffers() = 0;

	/**
	Returns a used input video picture back to the caller. The picture memory can be re-used or freed.
	@param	"aPicture"	"The picture to return."
	*/
	virtual void MdvpoReturnPicture(TVideoPicture* aPicture) = 0;

	/**
	Delivers supplemental information from a coded data unit. The information is codec-dependent.
	@param	"aData"			"The supplemental data. The reference is only valid until the method 
							returns, and thus the data must be processed or copied immediately."
	@param	"aTimestamp"	"The presentation timestamp for the picture that the supplemental data 
							is part of."
	@param	"aPictureId"	"Picture identifier for the picture. If a picture ID is not available, 
							aPictureId.iIdType is set to ENone."
	*/
	virtual void MdvpoSupplementalInformation(const TDesC8& aData, 
											  const TTimeIntervalMicroSeconds& aTimestamp, 
											  const TPictureId& aPictureId) = 0;

	/**
	Back channel information from the decoder, indicating a picture loss without specifying the lost picture.
	*/
	virtual void MdvpoPictureLoss() = 0;

	/**
	Back channel information from the decoder, indicating the pictures that have been lost.

	@param "aPictures"	"Picture identifiers for the lost pictures. The reference is only valid until the 
						method returns, and thus the data must be processed or copied immediately."
	*/
	virtual void MdvpoPictureLoss(const TArray<TPictureId>& aPictures) = 0;

	/**
	Back channel information from the decoder, indicating the loss of consecutive macroblocks in raster 
	scan order.

	@param "aFirstMacroblock"	"The first lost macroblock.  The macroblocks are numbered such 
								that the macroblock in the upper left corner of the picture is considered 
								macroblock number 1 and the number for each macroblock increases from left 
								to right and then from top to bottom in raster-scan order."
	@param "aNumMacroblocks"	"The number of lost macroblocks that are consecutive in raster-scan order."
	@param "aPicture"			"The picture identifier for the picture where the macroblocks were lost.  
								If the picture is not known, aPicture.iIdType is set to ENone.  The reference 
								is only valid until the method returns."
	*/
	virtual void MdvpoSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture)= 0;

	/**
	Back channel information from the decoder, indicating a reference picture selection request. The 
	request is delivered as a coding-standard specific binary message.  Reference picture selection can 
	be used to select a pervious correctly transmitted picture to use as a reference in case later pictures 
	have been lost.
	
	@param "aSelectionData" "The reference picture selection request message. The message format is 
							coding-standard specific, and defined separately. The reference is only 
							valid until the method returns, and thus the data must be processed or 
							copied immediately."
	*/
	virtual void MdvpoReferencePictureSelection(const TDesC8& aSelectionData)= 0;

	/**
	Called when a timed snapshot request has been completed. When this method is called, the snapshot 
	has been taken, and the memory reserved for the picture can be re-used or freed.
	@param	"aError"					"An error code, KErrNone if no errors occurred. If an error 
										occurred, the data in the snapshot may not be valid, but the 
										memory can still be freed."
	@param	"aPictureData"				"The snapshot picture data."
	@param	"aPresentationTimestamp"	"The presentation timestamp for the snapshot picture. "
	@param	"aPictureId"				"Picture identifier for the picture. If a picture ID is not
										available, aPictureId.iIdType is set to ENone."
	*/
	virtual void MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId) = 0;

	/**
	Notifies the client that one or more new output pictures are available. The client can then use 
	GetNewPictureInfo() and NextPicture() to fetch the pictures.
	*/
	virtual void MdvpoNewPictures() = 0;

	/**
	Reports a fatal decoding or playback error to the client. When these errors
	occur, decoding and playback is stopped automatically. The client must
	destroy the CMMFDevVideoPlay object and create a new instance before
	attempting to continue. Note that scenarios have been identified where
	MdvpoFatalError may get referenced at some point during the execution of a
	CMMFDevVideoPlay procedure. Therefore CMMFDevVideoPlay object should be
	deleted outside of MdvpoFatalError context in order to avoid accidental
	access to de-allocated CMMFDevVideoPlay data members.

	Note that running out of processing power or encountering corrupted
	bitstream data should not be classified as errors if the decoder and
	post-processor can recover automatically.
	
	@param	"aError"	"The error code."
	*/
	virtual void MdvpoFatalError(TInt aError) = 0;

	/**
	Reports that DevVideoPlay initialization has completed. The interface can now be used for video 
	playback.
	@param	"aError"	"Initialization error code, KErrNone if no error occurred."
	*/
	virtual void MdvpoInitComplete(TInt aError) = 0;

	/**
	Reports that the input video stream end has been reached and all pictures have been processed. 
	This method is only called after the client has called InputEnd(). No more output pictures will 
	be available.
	*/
	virtual void MdvpoStreamEnd() = 0;
	};
	
	
#include <mmf/devvideo/devvideoplay.inl>

#endif
