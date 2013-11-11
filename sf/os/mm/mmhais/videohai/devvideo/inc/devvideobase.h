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

#ifndef __DEVVIDEOBASE_H__
#define __DEVVIDEOBASE_H__

#include <e32base.h>
#include <w32std.h>
#include <mmf/devvideo/devvideoconstants.h>
#include <ecom/ecom.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/devvideo/devvideohwdeviceadaptersetup.h>
#endif


/**
Panic utility function
@publishedAll
*/
LOCAL_C void DevVideoPanic(TInt aReason)
	{
	User::Panic(KDevVideoPanicCategory, aReason);
	}


/**
A hardware device ID that identifies an instantiated video hardware device. 
Used in the playback and recording APIs to identify the component being controlled.
@publishedAll
@released
*/
typedef TInt THwDeviceId;


/**
Defines a supported scale factor for a scaling pre-processor or post-processor.

The scale factor is defined as iScaleNum/iScaleDenom, 
where the values are positive integers and relatively prime.
@publishedAll
@released
*/
class TScaleFactor
	{
public:
	/**Scale factor numerator. Non-zero.*/
	TUint iScaleNum;
	/**Scale factor denominator. Non-zero.*/
	TUint iScaleDenom;
	};


/**
A custom YUV/RGB conversion matrix. The same matrix structure is used for both conversion directions.
@publishedAll
@released
*/
class TYuvConversionMatrix
	{
public:
	/**
	Post-multiplication offset, a three-item vector.
	*/
	TReal iPostOffset[3];

	/**
	Multiplication matrix, a 3x3 matrix. iMatrix[3*i+j] contains the j:th item on the i:th row of the matrix.
	*/
	TReal iMatrix[9];

	/**
	Pre-multiplication offset, a three-item vector.
	*/
	TReal iPreOffset[3];
	};


/**
YUV (YCbCr) uncompressed image data format.
@publishedAll
@released
*/
class TYuvFormat
	{
public:
	/**
	The YUV/RGB conversion coefficients to use
	*/
	TYuvCoefficients iCoefficients;

	/**
	Luminance/chrominance sampling pattern.
	*/
	TYuvSamplingPattern iPattern;

	/**
	Data layout, specifies whether the data is stored in a planar or interleaved mode.
	*/
	TYuvDataLayout iDataLayout;

	/**
	Custom YUV to RGB conversion matrix to use. 
	Valid only if custom conversion matrix is used (iCoefficients is ECustomYuvMatrix).
	*/
	TYuvConversionMatrix* iYuv2RgbMatrix;

	/**
	Custom RGB to YUV conversion matrix to use. 
	Valid only if custom conversion matrix is used (iCoefficients is ECustomYuvMatrix).
	*/
	TYuvConversionMatrix* iRgb2YuvMatrix;

	/**
	Pixel aspect ratio numerator.
	*/
	TUint iAspectRatioNum;

	/**
	Pixel aspect ratio denominator. 
	The aspect ratio is defined as iAspectRatioNum/iAspectRatioDenom, 
	where the values are positive integers and relatively prime.
	*/
	TUint iAspectRatioDenom;

	/**
	Tests whether this TYuvFormat object is the same as aOther.
	@param  "aOther" "The object to compare."
	@return "ETrue if they are the same, EFalse if not."
	*/
	inline TBool operator==(const TYuvFormat& aOther) const;
	};

/**
Defines an uncompressed video format. 
This structure is mainly just a combination of YUV and RGB formats, defined to simplify the rest of the API.
@publishedAll
@released
*/
class TUncompressedVideoFormat
	{
public:
	/**
	The image data format. The validity of the rest of the fields depends on the data format used.
	*/
	TImageDataFormat iDataFormat;
	union
		{
		/**
		YUV picture format details, valid if iDataFormat is EYuvRawData.
		*/
		TYuvFormat iYuvFormat;

		/**
		RGB picture format details, valid if iDataFormat is ERgbRawData or ERgbFbsBitmap.
		*/
		TRgbFormat iRgbFormat;
		};

	/**
	Tests whether this TUncompressedVideoFormat object is the same as aOther.
	@param  "aOther" "The object to compare."
	@return "ETrue if they are the same, EFalse if not."
	*/
	inline TBool operator==(const TUncompressedVideoFormat& aOther) const;

	/**
	Sets this object equal to aOther.
	@param  "aOther" "The object to clone."
	*/
	inline void operator=(const TUncompressedVideoFormat& aOther);
	};


/**
Uncompressed picture data for one video picture.
@publishedAll
@released
*/
class TPictureData
	{
public:
	/**
	The image data format. The validity of the rest of the fields depends on the data format used.
	*/
	TImageDataFormat iDataFormat;

	/**
	Image data size in pixels. In decoder output pictures the actual active picture area may be smaller, 
	this is indicated using TVideoPicture.iCropRect.
	*/
	TSize iDataSize;

	union
		{
		/**
		Pointer to raw image data. Valid if iDataFormat is ERgbRawData or iYuvRawData. 
		The data layout depends on the format used.
		*/
		TPtr8* iRawData;

		/**
		Pointer to an RGB bitmap. Valid if iDataFormat is ERgbFbsBitmap.
		*/
		CFbsBitmap* iRgbBitmap;
		
		/**
		 Buffer number of current buffer for composition that is used when submitting a surface update. Valid if iDataFormat is set to ESurfaceBuffer
		*/
		TInt iSurfaceBufNum;
		};
	};

/**
Header information for one decoded picture. 
The header information is returned alongside with decoded pictures, 
or it can be read separately when DevVideoPlay is being initialized.

@publishedAll
@released
*/
class TVideoPictureHeader
	{
public:
	/**
	*/
	enum THeaderOptions
		{
		/** Decoding timestamp is valid
		*/
		EDecodingTimestamp		   = 0x00000001,
		/** Presentation timestamp is valid
		*/
		EPresentationTimestamp	   = 0x00000002,
		/** Pre-decoder buffersize is valid
		*/
		EPreDecoderBufferSize		= 0x00000004,
		/** Post-decoder buffersize is valid
		*/
		EPostDecoderBufferSize	   = 0x00000008,
		/** Picture number field is valid
		*/
		EPictureNumber			   = 0x00000010,
		/** Layered coding is used and the layer number field is valid
		*/
		ELayeredCoding			   = 0x00000020,
		/** Supplemental data is available
		*/
		ESupplementalData			= 0x00000040,
		/** Random access buffering period is valid
		*/
		ERandomAccessBufferingPeriod = 0x00000080,
		/** Random access buffer occupancy is valid
		*/
		ERandomAccessBufferOccupancy = 0x00000100
		};

	/**
	Header options.  The value is a bitfield combined from values from THeaderOptions.
	*/
	TUint32 iOptions;

	/**
	Video codec profile used. Use -1 if not applicable or not defined.
	*/
	TInt iProfile;

	/**
	Video codec level. Use -1 if not applicable or not defined.
	*/
	TInt iLevel;

	/**
	Video codec version. Use -1 if not applicable or not defined.
	*/
	TInt iVersion;

	/**
	Pointer to a descriptor that contains optional codec-specific features. Set to NULL if not used. 
	The format of the data is codec-specific. The pointer and descriptor data are valid as long as 
	the header information structure is valid.
	*/
	const TDesC8* iOptional;

	/**
	Image size in memory, in pixels. May be larger than the displayed picture.
	*/
	TSize iSizeInMemory;

	/**
	The portion of the full image to display.
	*/
	TRect iDisplayedRect;

	/**
	Picture presentation timestamp. Valid only if EPresentationTimestamp is set in the options. 
	The clock frequency is stored in the timestamp structure.
	*/
	TTimeIntervalMicroSeconds iPresentationTimestamp;

	/**
	Picture decoding timestamp. Valid only if EDecodingTimestamp is set in the options.
	*/
	TTimeIntervalMicroSeconds iDecodingTimestamp;

	/**
	Expected pre-decoder buffer size in bytes. Valid only if EPreDecoderBufferSize is set in the options.
	*/
	TUint iPreDecoderBufferSize;

	/**
	Expected post-decoder buffer size in bytes. Valid only if EPostDecoderBufferSize is set in the options. 
	It is assumed that a frame buffer to be displayed is returned before the decoding of the next frame 
	is started. If this is not the case, a larger post-decoder buffer may actually be needed.
	*/
	TUint iPostDecoderBufferSize;

	/**
	Picture number, valid only if EPictureNumber is set in the options. 
	This field is used to indicate one of the following: picture number or long-term picture index for H.263, 
	vop_id for MPEG-4 Visual,  picture number or long-term picture number for AVC.
	*/
	TUint iPictureNumber;

	/**
	Picture layer number if layered coding is used, valid only if ELayeredCoding is set in the options. 
	Layers are numbered [0…n-1], where n is the number of layers available. The first layer (layer zero) 
	is the base layer, it can be decoded independently from the other layers, and it has the lowest total bitrate.
	*/
	TUint iPictureLayer;

	/**
	Picture supplemental data, valid only if ESupplementalData is set in the options. 
	The pointer and descriptor data are valid as long as the header information structure is valid.
	*/
	const TDesC8* iSupplementalData;

	/**
	True if the picture is a random-accessible picture.
	*/
	TBool iIsRandomAccessible;

	/**
	The expected initial pre-decoder buffering period before starting the playback from this picture. 
	Valid only if this picture is randomly accessible (iIsRandomAccessible is true) and 
	ERandomAccessBufferingPeriod is set in the options. MPEG-2 and H.264 | MPEG-4 AVC use this value.
	*/
	TTimeIntervalMicroSeconds32 iRandomAccessBufferingPeriod;

	/**
	The expected initial pre-decoder buffer occupancy in bytes before starting the playback 
	from this picture. Valid if this picture is randomly accessible (iIsRandomAccessible is true) and 
	ERandomAccessBufferOccupancy is set in the options. MPEG-4 Visual uses this value.
	*/
	TUint iRandomAccessBufferOccupancy;
	};



/**
One uncompressed video picture. Used for both decoded picture output as well as uncompressed picture input.

@publishedAll
@released
*/
class TVideoPicture
	{
public:
	/**
	*/
	enum TVideoPictureOptions
		{
		/** The timestamp field is valid.
		*/
		ETimestamp		 = 0x00000001,
		/** The crop rectangle field is valid.
		*/
		ECropRect		  = 0x00000002,
		/** Picture header information is present.
		*/
		EHeader			= 0x00000004,
		/** The layer bit count targets field is valid.
		*/
		EBitTargets		= 0x00000008,
		/** Set in encoder input to request an instantaneous decoder refresh. 
		As a response, the encoder should code an intra frame and no consecutive 
		frame should refer to any frame before the encoded intra frame (in coding order).
		*/
		EReqInstantRefresh = 0x00000010,
		/** Set in encoder input to indicate a scene cut in the picture stream.
		*/
		ESceneCut		  = 0x00000020,
		/** Set if a picture effect is in use and the picture effect field is valid.
		*/
		EPictureEffect	 = 0x00000040,
		/** Set if picture effect parameters are valid.
		*/
		EEffectParameters  = 0x00000080,
		/** Content protected pictures cannot be displayed on unprotected 
		    external displays such as TV-out.
		*/
		EContentProtected = 0x00000100
		};

	/**
	The picture data. The picture data, including all pointers, must remain valid until 
	the picture has been returned to its originator.
	*/
	TPictureData iData;

	/**
	Picture options. The value is a bitfield combined from values from TVideoPictureOptions.
	*/
	TUint32 iOptions;

	/**
	Picture timestamp. Valid if ETimestamp is set in the options. 
	Used for presentation timestamps in video playback and capture timestamps in uncompressed video 
	input for recording. If the timestamp is not specified for decoded video input for playback, 
	the picture is displayed immediately. For decoded video output in playback and uncompressed 
	video input for recording, the timestamp must always be set.
	*/
	TTimeIntervalMicroSeconds iTimestamp;

	/**
	Pan-scan cropping rectangle. Defines the area of the picture used for further processing. 
	Only used for decoded video output.
	*/
	TRect iCropRect;

	/**
	Picture header information. Valid if EHeader is set in the options. 
	Normally this field is only used in decoded pictures returned from the playback API. 
	In that case the header info pointer is valid until the picture is returned to the API.
	*/
	TVideoPictureHeader* iHeader;

	/**
	The target number of bits for each bit-rate scalability layer, valid when EBitTargets is set in the options. 
	Used in video encoding when the caller controls the bit-rate for each picture separately.
	The field points to a table containing the target number of bits to use for each layer when 
	encoding this picture, starting from the lowest layer. The bit count for an enhancement layer 
	includes all lower layers. For example, if the client uses two layers, and reserves 1.5 kilobits 
	for the base layer and three kilobits for the whole picture, this field is set to {1500, 3000}.
	*/
	RArray<TUint>* iLayerBitRates;

	/**
	The picture effect in use when capturing this picture, valid when EPictureEffect is set in the options. 
	This information can be used when encoding the picture. Note that setting a picture effect does not 
	imply that the encoder should modify the picture data based on the effect. Instead, it can be used as 
	an encoding hint. For example, fade to black implies that the global picture brightness has been decreased, 
	and this knowledge can be used to aid motion prediction.
	@see TPictureEffects
	*/
	TPictureEffect iEffect;

	/**
	Picture effect parameter for fade to/from black, valid when EEffectParameters is set in the options 
	and iEffect is EEffectFadeFromBlack or EEffectFadeToBlack. The value range is [0…65536], with zero 
	indicating the picture is black and 65536 indicating that the lightness of the picture is unchanged. 
	If the parameter is not given, the caller is unaware of the proper value or the value fluctuates spatially.
	*/
	TUint iFadeParam;

	/**
	A pointer for free-form user data. The pointer is set by the module that created the buffer, and is 
	usually used for memory management purposes.
	*/
	TAny* iUser;

	/**
	A queue link used internally by the MSL video components.
	*/
	TDblQueLink iLink;
	};

/**
Identifies a video picture in feedback messages.

@publishedAll
@released
*/
class TPictureId
	{
public:
	enum TPictureIdType
		{
		/** Unidentified picture. */
		ENone,
		/** The picture is identified using its temporal reference. */
		ETemporalReference,
		/** The picture is identified using its picture number. Picture numbers are used in H.263 annex U and H.264 | MPEG-4 AVC, for example.. */
		EPictureNumber
		};

	/** Picture identified type. */
	TPictureIdType iIdType;	
	
	/** The picture identifier. The interpretation of this field depends on the value iIdType */
	TUint32 iId;
	};


/**
Defines a compressed video format. The format is identified by its MIME type, which may include 
parameters that describe the used format profile and level. The MIME type used for H.263
is video/H263-2000, specified in TS 26.234, and the type for MPEG-4 is video/MP4V-ES, specified in RFC 3016.

@publishedAll
@released
*/
class CCompressedVideoFormat : public CBase
	{
public:
	/**
	Static factory function for creating new CCompressedVideoFormat objects.

	@param "aMimeType"		"Video codec MIME type, including optional parameters for profile, 
							level and version information. The CCompressedVideoFormat object creates
							and owns a copy of this buffer and takes care of deallocation."

	@param "aOptionalData"	"Reference to a descriptor that contains optional codec-specific data. 
							Set to KNullDesC8 if not used. [The format of the data is codec-specific, typically
							a package buffer containing a data structure may be used. The pointer lifetime 
							and validity requirements are specified with each method that uses this structure."

	@return "Pointer to a fully constructed CCompressedVideoFormat object."
	@leave	"This method may leave with one of the system-wide error codes."
	*/
	IMPORT_C static CCompressedVideoFormat* NewL(const TDesC8& aMimeType, const TDesC8& aOptionalData = KNullDesC8);

	/**
	Static factory function for creating a copy of an existing CCompressedVideoFormat object.

	@param "aFormat"          "The CCompressedVideoFormat object to copy."

	@return Pointer to a fully constructed CCompressedVideoFormat object.
	@leave	This method may leave with one of the system-wide error codes.
	*/
	IMPORT_C static CCompressedVideoFormat* NewL(const CCompressedVideoFormat& aFormat);

	/**
	Virtual destructor. Destroys iMimeType.
	*/
	IMPORT_C virtual ~CCompressedVideoFormat();

	/**
	Returns the video codec MIME type.

	@return "Reference to a descriptor that contains the video codec MIME type."
	*/
	IMPORT_C const TDesC8& MimeType() const;

	/**
	Returns the optional data.
	
	@return "Reference to a descriptor that contains optional codec-specific data. 
			Zero length if not used. The format of the data is codec-specific, typically a package buffer 
			containing a data structure may be used."
	*/
	IMPORT_C const TDesC8& OptionalData() const;

	/**
	Tests whether this CCompressedVideoFormat is identical to aOther or not.
	@return "ETrue if the two objects are identical, EFalse if not."
	*/
	IMPORT_C TBool operator==(const CCompressedVideoFormat& aOther) const;
protected:
        /**
        @internalTechnology
        */
	CCompressedVideoFormat();

        /**
        @internalTechnology
        */
	void ConstructL(const TDesC8& aMimeType, const TDesC8& aOptionalData);
private:
	HBufC8* iMimeType;
	HBufC8* iOptionalData;
	};


/**
Specifies the HRD/VBV parameters used when 3GPP TS 26.234 annex G HRD/VBV settings are used (EHrdVbv3GPP). 
See TS 26.234 Release 5 for details.
@publishedAll
@released
*/
class T3gppHrdVbvParams
	{
public:
	/** Initial pre-decoder buffering period. */
	TTimeIntervalMicroSeconds iInitialPreDecoderBufferPeriod;

	/** Initial post-decoder buffering period. */
	TTimeIntervalMicroSeconds iInitialPostDecoderBufferPeriod;

	/** Hypothetical pre-decoder buffer size, in bytes. */
	TUint iPreDecoderBufferSize;

	/** Peak decoding byte rate. By default, the peak decoding byte rate is 
	defined according to the video coding profile and level in use. */
	TUint iPeakDecodingByteRate;

	/** Decoding macroblock rate. The default decoding macroblock rate is defined 
	according to the video coding profile and level in use. */
	TUint iDecodingMacroblockRate;
	};


/**
YUV to RGB post-processing options.
@publishedAll
@released
*/
class TYuvToRgbOptions
	{
public:
	/**
	Lightness setting. The value range is [-100 … 100], with -100 corresponding to minimum setting, 
	100 to maximum setting, and 0 to no change in lightness. The actual lightness change formula 
	used is hardware device specific.
	*/
	TInt iLightness;

	/**
	Saturation setting. The value range is [-100 … 100], with -100 corresponding to minimum setting, 
	100 to maximum setting, and 0 to no change in saturation. The actual saturation formula used 
	is hardware device specific.
	*/
	TInt iSaturation;

	/**
	Contrast setting. The value range is [-100 … 100], with -100 corresponding to minimum setting, 
	100 to maximum setting, and 0 to no change in contrast. The actual contrast change formula 
	used is hardware device specific.
	*/
	TInt iContrast;

	/**
	Gamma setting for conversion. Ignored if the converter does not support gamma correction. 
	The gamma correction is defined as x' = x^(1/g), where x' refers to the corrected value, 
	x to the original input value and g to this field. Gamma correction is performed on the RGB values. 
	Set gamma to 1.0 to disable gamma correction.
	*/
	TReal iGamma;

	/**
	The dithering type to use.
	*/
	TDitherType iDitherType;
	};

/**
Pre-processing options for color enhancement. 
The value ranges have been chosen to match those in the Onboard Camera API.
@publishedAll
@released
*/
class TColorEnhancementOptions
	{
public:
	/**
	Lightness setting. The value range is [-100 … 100], with -100 corresponding to minimum setting, 
	100 to maximum setting, and 0 to no change in lightness.
	*/
	TInt iLightness;

	/**
	Saturation setting. The value range is [-100 … 100], with -100 corresponding to minimum setting, 
	100 to maximum setting, and 0 to no change in saturation.
	*/
	TInt iSaturation;

	/**
	Contrast setting. The value range is [-100 … 100], with -100 corresponding to minimum setting, 
	100 to maximum setting, and 0 to no change in contrast.
	*/
	TInt iContrast;
	};

/**
Describes the YUV to RGB color conversion capabilities of a post-processor.
@publishedAll
@released
*/
class TYuvToRgbCapabilities
	{
public:
	/**
	Input YUV sampling patterns supported, a bitfield combination (bitwise OR) of TYuvSamplingPattern.
	*/
	TUint32 iSamplingPatterns;

 	/**
	YUV to RGB conversion coefficients supported, a bitfield combination of TYuvCoefficients.
	*/
	TUint32 iCoefficients;

 	/**
	Output RGB formats supported, a bitfield combination of TRgbFormat.
	*/
	TUint32 iRgbFormats;

 	/**
	True if lightness control is supported.
	*/
	TBool iLightnessControl;

 	/**
	True if saturation control is supported.
	*/
	TBool iSaturationControl;

 	/**
	True if contrast control is supported.
	*/
	TBool iContrastControl;

 	/**
	True if gamma correction is supported.
	*/
	TBool iGammaCorrection;

 	/**
	Dithering types supported, a bitfield combination of TDitherType.
	*/
	TUint32 iDitherTypes;
	};

/**
Specifies the YUV-to-YUV conversion capabilities for a plug-in.
@publishedAll
@released
*/
class TYuvToYuvCapabilities
	{
public:
	/**
	The YUV sampling patterns supported for input data, a combination (binary OR) of values from TYuvSamplingPatterns.
	*/
	TUint32 iInputSamplingPatterns;

	/**
	The YUV data layouts supported for input data, a combination (binary OR) of values from TYuvDataLayout.
	*/
	TUint32 iInputDataLayouts;

	/**
	The YUV sampling patterns supported for output data, a combination (binary OR) of values from TYuvSamplingPatterns.
	*/
	TUint32 iOutputSamplingPatterns;

	/**
	The YUV data layouts supported for output data, a combination (binary OR) of values from TYuvDataLayout.
	*/
	TUint32 iOutputDataLayouts;
	};

/**
Structure to combine a picture rate and size.  Used when defining the maximum rate/size combinations
available.
@publishedAll
@released
*/
class TPictureRateAndSize
	{
public:
	/** The picture rate. */
	TReal iPictureRate;

	/** The picture size. */
	TSize iPictureSize;
	};

/**
CMMFVideoHwDevice is a base class for all video hardware devices.
@publishedAll
@released
*/
class CMMFVideoHwDevice : public CBase
	{
public:
	/**
	Retrieves a custom interface to the device.
	@param  "aInterface"	"Interface UID, defined with the custom interface."
	@return "Pointer to the interface implementation, or NULL if the device does not 
			implement the interface requested. The return value must be cast to the 
			correct type by the user."
	*/
	virtual TAny* CustomInterface(TUid aInterface) = 0;
	};



/**
Defines the interface that video clock sources must to implement.

A clock source can be used to synchronize video processing to some other processing entity, 
for example an audio stream.  The clock source is created and controlled by the DevVideo client, 
and passed to the video hwdevices via the DevVideo interface. This allows the hwdevice to query 
the current stream time, so it can ascertain which frame should be processed at any given moment.

"Stream time" is defined as the current position in the media stream. For example, when playing
a video clip, the stream time will always equal the current position in the clip. So, when the 
clip is repositioned, the stream time will be equal to the new clip position.  When the clip is
paused, the stream time will pause too.

Many hwdevice implementations will use extra threads to perform the video processing, so it is
essential that all implementations of the MMMFClockSource interface can be used from multiple 
threads.  A hwdevice that receives a clock source from the DevVideo client must be able to
assume that it can pass a pointer to the clock source object into another thread and use the 
object directly from there.

All clock source implementations must protect the methods defined in the MMMFClockSource interface
so they can be called from any thread within the current process.  Practically speaking, this means
using mutexes and critical sections to protect member data from being accessed from multiple threads
simultaneously.  Also, any use of thread-specific handles (e.g. a CMMFDevSound object) must be 
avoided from within these methods.

It can be assumed that any methods defined outside the MMMFClockSource interface (for example 
methods used to control the clock source) will only be executed within the DevVideo client's thread,
so do not require protection.

@publishedAll
@released
*/
class MMMFClockSource
	{
public:
	/**
	Retrieves a custom interface for the clock source.

	This method can be called from any thread running inside the process in which the concrete
	clock source was created.  Note that this does not mean that all methods in the custom 
	interface can be called from any thread - that will be specified by the custom interface itself.

	@param  "aInterface"	"Interface UID, defined by the entity specifying the interface."
	@return "Pointer to the interface implementation, or NULL if the interface is not available. 
			The pointer must be cast to the appropriate interface class."
	*/
	virtual TAny* CustomInterface(TUid aInterface) = 0;

	/**
	Retrieves the current stream time.  For example, if a clip is being played, the stream time will
	be equal to the current position in the clip.

	This method can be called from any thread running inside the process in which the concrete clock
	source was created.

	@return "The number of microseconds passed in the clock compared to the reference time."
	*/
	virtual TTimeIntervalMicroSeconds Time() = 0;
	};

/**
The CSystemClockSource provides a basic clock source implementation based on the system clock. 
It will count microseconds since the object was created or Reset() was last called, and return 
that count from Time(). It does not implement any custom interfaces.
@publishedAll
@released
*/
class CSystemClockSource : public CBase, public MMMFClockSource
	{
public:
	/**
	Creates a new CSystemClockSource object.
	@return "A new clock source object."
	@leave  "This method may leave with one of the system-wide error codes."
	*/
	IMPORT_C static CSystemClockSource* NewL();

	/**
	Destructor.
	*/
	IMPORT_C ~CSystemClockSource();

// Control methods
	/**
	Resets the clock source to zero. Typically called by the DevVideo client at stream start.
	*/
	IMPORT_C void Reset();

	/**
	Resets the clock source to a user-defined offset. Typically called by the DevVideo client 
	when seeking in a file.

	@param "aOffset"	"The clock offset."
	*/
	IMPORT_C void Reset(const TTimeIntervalMicroSeconds& aOffset);

	/**
	Suspends the clock source. The clock time will not increment until the clock has been resumed.
	This method is used when pausing playback.
	*/
	IMPORT_C void Suspend();

	/**
	Resumes the clock source after a Suspend() method call. This method is used when resuming playback.
	*/
	IMPORT_C void Resume();

// Implementation of MMMFClockSource
	/** 
	No custom interfaces are implemented by this clock source, so this method will always return NULL.
	@param "aInterface"	"The interface"
	@return "NULL"
	*/
	virtual TAny* CustomInterface(TUid aInterface);
	/**
	Retrieves the time that has elapsed since Reset() was last called, subtracting any time 
	during which the clock was suspended.

	@return "The number of microseconds that have elapsed in the stream."
	*/
	virtual TTimeIntervalMicroSeconds Time();

private:
	CSystemClockSource();
	void ConstructL();
private:
	TTime iStartTime;
	TTime iCurrentTime;

	TTimeIntervalMicroSeconds iOffset;

	TTime iTimeWhenSuspended;
	TTimeIntervalMicroSeconds iTimeSuspended;
	TBool iSuspended;
	
	RCriticalSection iCriticalSection;
	};


/**
Observer class to be used with class CMMFClockSourcePeriodicUtility.
@publishedAll
@released
*/
class MMMFClockSourcePeriodicUtilityObserver
	{
public:
	/**
	Notifies the observer that the specified period has elapsed.
	@param	"aTime"	"The current time, queried from the clock source."
	*/
	virtual void MmcspuoTick(const TTimeIntervalMicroSeconds& aTime) = 0;
	};

/**
Utility class that can be used by video HW devices to receive periodic callbacks with the current time.
Note that the exact timing of the callbacks cannot be guaranteed due to other things pre-empting 
the execution of the active object or thread.
@publishedAll
@released
*/
class CMMFClockSourcePeriodicUtility : public CBase
	{
public:
	/**
	Creates a new clock source periodic utility object.

	@param	"aClockSource"	"A reference to the clock source to be used to query the current time."
	@param	"aObserver"		"A reference to the observer of the utility that will receive callbacks  
							each time the specified period elapses."
	@return "A new clock source periodic utility object."
	@leave	"The method will leave if an error occurs."
	*/
	IMPORT_C static CMMFClockSourcePeriodicUtility* NewL(MMMFClockSource& aClockSource, MMMFClockSourcePeriodicUtilityObserver& aObserver);

	/**
	Starts the clock source periodic utility.  The utility will call MmcspuoTick on its observer 
	every aPeriod microseconds until Stop() is called.  Note that the utility will not stop 
	automatically when the clock source is stopped.

	@param  "aPeriod" "Defines the period with which the observer will receive callbacks."
	*/
	IMPORT_C void Start(TTimeIntervalMicroSeconds32 aPeriod);

	/**
	Stops the clock source periodic utility.  No more callbacks will be made after this method has 
	been called.
	*/
	IMPORT_C void Stop();
	
	/**
	Destructor.
	*/
	IMPORT_C ~CMMFClockSourcePeriodicUtility();
private:
	CMMFClockSourcePeriodicUtility(MMMFClockSource& aClockSource, MMMFClockSourcePeriodicUtilityObserver& aObserver);
	void ConstructL();
	static TInt Callback(TAny* aAny);
	void DoCallback();
private:
	CPeriodic* iTimer;
	MMMFClockSource& iClockSource;
	MMMFClockSourcePeriodicUtilityObserver& iObserver;
	};


/**
Specifies the encoder buffering options.
@publishedAll
@released
*/
class TEncoderBufferOptions
	{
public:
	/** 
	The maximum number of pictures in the pre-encoder buffer. 
	*/
	TUint iMaxPreEncoderBufferPictures;

	/** 
	The HRD/VBV specification that shall be fullfilled. 
	*/
	THrdVbvSpecification iHrdVbvSpec;

	/**
	The HRD/VBV buffering parameters. The data format depends on the parameters chosen. For 
	3GPP TS 26.234 parameters (iHrdVbvSpec=EHrdVbv3GPP), the data in the descriptor is a package of type 
	TPckC<T3gppHrdVbvParams> (see T3gppHrdVbvParams). If no HRD/VBV parameters are used, the 
	descriptor is empty.
	*/
	TPtrC8 iHrdVbvParams;

	/**
	The maximum size of an output buffer, in bytes. Use KMaxTUint for an unlimited size.
	*/
	TUint iMaxOutputBufferSize;

	/**
	The maximum size of a coded picture, in bytes. Use KMaxTUint for an unlimited size.
	*/
	TUint iMaxCodedPictureSize;

	/**
	The maximum size of a coded video segment, in bytes. Use KMaxTUint for an unlimited size.
	*/
	TUint iMaxCodedSegmentSize;

	/**
	The mimimum number of output buffers.
	*/
	TUint iMinNumOutputBuffers;
	};

/**
Specifies the video encoder bit-rate control options.
@publishedAll
@released
*/
class TRateControlOptions
	{
public:
	/**
	Defines the bit-rate control type.
	*/
	TBitrateControlType iControl;

	/**
	The target bit-rate, in bits per second. Used if bit-rate control type is EBrControlStream. 
	If specified for an enhancement layer, the target bit-rate includes all lower layers. For example, 
	if the client uses two layers, with the base layer using 64000 bps and the whole stream 192000 bps, 
	this field is set to 64000 for layer zero and 192000 for layer one.
	*/
	TUint iBitrate;

	/**
	The target picture quality. The value range is [0…100], with 0 corresponding to minimum quality 
	and 100 to lossless coding (or the closest equivalent supported).
	*/
	TUint iPictureQuality;

	/**
	The target picture rate, in pictures per second. If specified for an enhancement layer, the target 
	frame rate includes all lower layers.
	*/
	TReal iPictureRate;
	
	/**
	The quality/temporal tradeoff for bit-rate control. The value range is [0.0…1.0]. Value 0.0 
	specifies that picture quality should be maintained as well as possible, sacrificing picture rate. 
	Value 1.0 specifies that picture rate should be maintained as well as possible, sacrificing 
	picture quality.
	*/
 	TReal iQualityTemporalTradeoff;
 
	/**
        The latency/quality tradeoff for bit-rate control. The value range is [0.0…1.0]. Value 0.0 
        specifies that the transmission delay and the decoder input buffer occupancy level caused by 
        the bit-rate control is minimized, i.e. the actual coded bit-rate follows the target bit-rate 
        as closely as possible. 1.0 specifies that the transmission delay caused by the bit-rate control 
        should be as high as needed to guarantee a constant picture quality and frame rate as long as 
        the coded data conforms to the given HRD/VBV parameters (if any).
        */
	
	TReal iLatencyQualityTradeoff;
	};
	
/**
Custom interface Uid for setting up the DevVideo hw device adapter
*/
const TInt	KUidDevVideoHwDeviceAdapterSetup = 0x102737EF;
	
#include <mmf/devvideo/devvideobase.inl>

#endif

