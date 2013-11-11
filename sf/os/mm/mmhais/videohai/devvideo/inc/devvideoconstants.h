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

#ifndef __DEVVIDEOCONSTANTS_H__
#define __DEVVIDEOCONSTANTS_H__

#include <e32base.h>
#include <mmf/devvideo/devvideoplugininterfaceuids.hrh>
#include <mm/conversioncoefficient.h>

/**
DevVideo Panic Category

@publishedAll
@released
*/
_LIT(KDevVideoPanicCategory, "DevVideo");

/**
DevVideo Panic Codes

@publishedAll
@released
*/
enum TDevVideoPanicCodes
	{
	/**
	A pre-condition on a method has been violated.
	*/
	EDevVideoPanicPreConditionViolation = 1,
	/**
	A post-condition on a method has been violated.
	*/
	EDevVideoPanicPostConditionViolation = 2,
	/**
	An invalid hardware device ID has been supplied.
	*/
	EDevVideoPanicInvalidHwDeviceId = 3
	};


// DevVideo Plugin Interface UIDs

/** Video Decoder HW Device Plugin Interface UID 
@publishedAll
@released
*/
const TUid KUidDevVideoDecoderHwDevice = {KUidDevVideoDecoderHwDeviceDefine};

/** Video Post Processor HW Device Plugin Interface UID
@publishedAll
@released
*/
const TUid KUidDevVideoPostProcessorHwDevice = {KUidDevVideoPostProcessorHwDeviceDefine};

/** Video Encoder HW Device Plugin Interface UID 
@publishedAll
@released
*/
const TUid KUidDevVideoEncoderHwDevice = {KUidDevVideoEncoderHwDeviceDefine};

/** Video Pre Processor HW Device Plugin Interface UID
@publishedAll
@released
*/
const TUid KUidDevVideoPreProcessorHwDevice = {KUidDevVideoPreProcessorHwDeviceDefine};

// DevVideo Custom Interface Uids

/** MMMFVideoPlayHwDeviceExtensionScanCopy Custom Interface UID
@publishedAll
@released
*/
const TUid KUidDevVideoPlayHwDeviceExtensionScanCopy = {KUidDevVideoPlayHwDeviceExScanCopyDefine};

/** 
Picture frame rate constants

Using these constants is recommended when the picture rate is known to match 
one of them, to ensure that floating point equality comparisons work as expected.

Note that the MSL video APIs currently only deal with non-interlaced frames.  For interlaced 
video, all references to the term "picture" should be considered to refer to complete frames. 
As such, the term "picture rate" here refers to the frame rate for interlaced video.

@publishedAll
@released
*/
const TReal KPictureRate5 = 5.0;
const TReal KPictureRate75 = 7.5;
const TReal KPictureRate10 = 10.0;
const TReal KPictureRate15 = 15.0;
const TReal KPictureRateNTSC24 = 23.97602397602398; // == 24000/1001
const TReal KPictureRate25 = 25.0;
const TReal KPictureRateNTSC30 = 29.97002997002997; // == 30000/1001
const TReal KPictureRate30 = 30.0;


/**
Specifies the data format used for an uncompressed picture. 
The values are bit patterns that can be combined with other format definition constants.

@publishedAll
@released
*/
enum TImageDataFormat
	{
	/** Raw RGB picture data in a memory area.
	*/
	ERgbRawData		= 0x01000000,
	/** RGB picture data stored in a Symbian OS CFbsBitmap object.
	*/
	ERgbFbsBitmap	  = 0x02000000,
	/** Raw YUV picture data stored in a memory area. The data storage 
	format depends on the YUV sampling pattern and data layout used.
	*/
	EYuvRawData		= 0x04000000,
	
	/** Picture stored in a surface buffer.
	 @See MMmfVideoSurfaceHandleControl::MmvshcSetSurfaceHandle
	*/
	ESurfaceBuffer = 0x08000000
	};


/**
RGB uncompressed image format alternatives.
@publishedAll
@released
*/
enum TRgbFormat 
	{
	/**
	16-bit RGB data format with four pixels per component. 
	The data format is the same as used in Symbian EColor4K bitmaps, 
	with each pixel using two bytes with the bit layout [ggggbbbb xxxxrrrr]
	where "x" indicates unused bits. (This corresponds to "XRGB" 16-bit little-endian halfwords)
	*/
	ERgb16bit444	   = ERgbRawData   | 0x00000001,

	/**
	16-bit RGB data format with five bits per component for red and blue and 
	six bits for green. The data format is the same as used in Symbian EColor64K bitmaps, 
	with each pixel using two bytes with the bit layout [gggbbbbb rrrrrggg]
	(This corresponds to "RGB" 16-bit little-endian halfwords)
	*/
	ERgb16bit565	   = ERgbRawData   | 0x00000002,

	/**
	32-bit RGB data format with eight bits per component. 
	This data format is the same as is used in Symbian EColor16MU bitmaps. The bit layout is
	[bbbbbbbb gggggggg rrrrrrrr xxxxxxxx] where "x" indicates unused bits. 
	(This corresponds to "XRGB" 32-bit little-endian words)
	*/
	ERgb32bit888	   = ERgbRawData   | 0x00000004,

	/**
	CFbsBitmap object with EColor4K data format.
	*/
	EFbsBitmapColor4K  = ERgbFbsBitmap | 0x00000001,

 	/**
	CFbsBitmap object with EColor64K data format.
	*/
	EFbsBitmapColor64K = ERgbFbsBitmap | 0x00000002,

	/**
	CFbsBitmap object with EColor16M data format.
	*/
	EFbsBitmapColor16M = ERgbFbsBitmap | 0x00000004,

	/**
	CFbsBitmap object with EColor16MU data format.
	*/
	EFbsBitmapColor16MU = ERgbFbsBitmap | 0x00000008
	};


/**
YUV (YCbCr) uncompressed image data sampling pattern.
@publishedAll
@released
*/
enum TYuvSamplingPattern 
	{
	/**
	4:2:0 sampling format. 4 luminance sample positions correspond to one chrominance sample position. 
	The four luminance sample positions are on the corners of a square. The chrominance sample position 
	is vertically half-way of the luminance sample positions and horizontally aligned with the left 
	side of the square. This is the MPEG-2 and the MPEG-4 Part 2 sampling pattern.
	*/
	EYuv420Chroma1 = 0x00000001,

	/**
	4:2:0 sampling format. 4 luminance sample positions correspond to one chrominance sample position. 
	The four luminance sample positions are on the corners of a square. The chrominance sample position 
	is vertically and horizontally in the middle of the luminance sample positions. This is the MPEG-1 sampling pattern.
	*/
	EYuv420Chroma2 = 0x00000002,

	/**
	4:2:0 sampling format. 4 luminance sample positions correspond to one chrominance sample position. 
	The four luminance sample positions are on the corners of a square. The chrominance sample position 
	colocates with the top-left corner of the square. This sampling format is one of the options in Annex E of H.264 | MPEG-4 AVC.
	*/
	EYuv420Chroma3 = 0x00000004,

	/**
	4:2:2 sampling format. 2 luminance sample positions correspond to one chrominance sample position. 
	The luminance sample positions reside on the same pixel row. The chrominance sample position is co-located 
	with the left one of the luminance sample positions. This is the MPEG-2 4:2:2 sampling pattern.
	*/
	EYuv422Chroma1 = 0x00000008,

	/**
	4:2:2 sampling format. 2 luminance sample positions correspond to one chrominance sample position. 
	The luminance sample positions reside on the same pixel row. The chrominance sample position is in the 
	middle of the luminance sample positions. This is the MPEG-1 4:2:2 sampling pattern.
	*/
	EYuv422Chroma2 = 0x00000010
	};


/**
Defines the YUV data layout in a decoded picture.
@publishedAll
@released
*/
enum TYuvDataLayout
	{
	/**
	The data is stored in a plane mode. The memory buffer contains first all Y component 
	data for the whole picture, followed by U and V, making the data format Y00Y01Y02Y03...U0...V0... 
	For YUV 4:2:0 data, this is the same data format as EFormatYUV420Planar in the Onboard Camera API
	*/
	EYuvDataPlanar		= 0x00000001,

	/**
	The data is stored interleaved mode, all components interleaved in a single memory block. 
	Interleaved layout is only supported for YUV 4:2:2 data. The data byte order is Y1VY0U, 
	corresponding to "UY0VY1" little-endian 32-bit words. 
	This is the same data format as EFormatYUV422Reversed in the Onboard Camera API
	*/
	EYuvDataInterleavedLE = 0x00000002,

	/**
	The data is stored interleaved mode, all components interleaved in a single memory block. 
	Interleaved layout is only supported for YUV 4:2:2 data. The data byte order is UY0VY1, 
	corresponding to "UY0VY1" big-endian 32-bit words. 
	This is the same data format as EFormatYUV422 in the Onboard Camera API
	*/
	EYuvDataInterleavedBE = 0x00000004,
	/**
	The data is stored in a semi-planar mode. The memory buffer contains first all Y component 
	data for the whole picture, followed by U and V components, which are interlaced, making the data 
	format Y00Y01Y02Y03...U0V0U1V1... For YUV 4:2:0 data, this is the same data format as 
	FormatYUV420SemiPlanar in the Onboard Camera API
	*/
	EYuvDataSemiPlanar = 0x00000008
	};

/**
Defines the picture effect used for an input picture. Please refer to ITU-T H.264 | ISO/IEC MPEG-4 AVC [] for the definitions of the transition effects.
@publishedAll
@released
*/
enum TPictureEffect
	{
	/**
	No effect.
	*/
	EEffectNone          					= 0x00000001,

	/**
	Fade from black.
	*/
	EEffectFadeFromBlack 					= 0x00000002,

	/**
	Fade to black.
	*/
	EEffectFadeToBlack   					= 0x00000004,

	/**
	Unspecified transition from or to constant colour.
	*/
	EEffectUnspecifiedThroughConstantColor 	= 0x00000008,

	/**
	Dissolve.
	*/
	EEffectDissolve   						= 0x00000010,

	/**
	Wipe.
	*/
	EEffectWipe   							= 0x00000020,

	/**
	Unspecified mixture of two scenes.
	*/
	EEffectUnspecifiedMixOfTwoScenes  		= 0x00000040
	};

/**
Defines the data value range used for RGB data. Used for determining the correct color space conversion factors. 
@publishedAll
@released
*/
enum TRgbRange
	{
	/**
	The RGB data uses the full 8-bit range of [0…255].
	*/
	ERgbRangeFull	= 0x00000001,

	/**
	The RGB data uses the nominal range of [16…235]. Individual samples can still contain 
	values beyond that range, the rest of the 8-bit range is used for headroom and footroom.
	*/
	ERgbRange16to235 = 0x00000002
	};



/**
Defines possible data unit types for encoded video data. The data unit types are used both 
for encoded video input for playback as well as encoded video output from recording.
@publishedAll
@released
*/
enum TVideoDataUnitType
	{
	/**
	Each data unit is a single coded picture.
	*/
	EDuCodedPicture		   = 0x00000001,

	/**
	Each data unit is a coded video segment.  
	A coded video segment is a part of the coded video data that forms an independently 
	decodable part of a coded video frame. For example, a video packet in MPEG-4 Part 2 
	and slice in H.263 are coded video segments.
	*/
	EDuVideoSegment		   = 0x00000002,

	/**
	Each data unit contains an integer number of video segments consecutive in decoding order, 
	possibly more than one. The video segments shall be a subset of one coded picture.
	*/
	EDuSeveralSegments		= 0x00000004,

	/**
	Each data unit contains a piece of raw video bitstream, not necessarily aligned at any headers. 
	The data must be written in decoding order. This data unit type can be used for playback if the client 
	does not have information about the bitstream syntax, and just writes data in random-sized chunks. For 
	recording this data unit type is useful if the client can handle arbitrarily split data units, giving the
	encoder maximum flexibility in buffer allocation. For encoded data output, each data unit must still 
	belong to exactly one output picture.
	*/
	EDuArbitraryStreamSection = 0x00000008
	};

/**
Defines possible encapsulation types for coded video data units. The encapsulation information is 
used both for encoded video input for playback as well as encoded video output from recording.
@publishedAll
@released
*/
enum TVideoDataUnitEncapsulation
	{
	/**
	The coded data units can be chained in a bitstream that can be decoded. For example, MPEG-4 
	Part 2 elementary streams, H.263 bitstreams, and H.264 | MPEG-4 AVC Annex B bitstreams fall into this category.
	*/
	EDuElementaryStream = 0x00010000,

	/**
	The coded data units are encapsulated in a general-purpose packet payload format whose coded 
	data units can be decoded independently but cannot be generally chained into a bitstream. 
	For example, the Network Abstraction Layer Units of H.264 | MPEG-4 AVC fall into this category. 
	*/
	EDuGenericPayload   = 0x00020000,

	/**
	The coded data units are encapsulated in RTP packet payload format. The RTP payload header 
	may contain codec-specific items, such as a redundant copy of a picture header in the H.263 
	payload specification RFC2429.
	*/
	EDuRtpPayload	   = 0x00040000
	};

/**
Defines the HRD/VBV specification used in a stream.
@publishedAll
@released
*/
enum THrdVbvSpecification
	{
	/** No HRD/VBV specification. */
	EHrdVbvNone		   = 0x00000001,

	/** The HRD/VBV specification in the corresponding coding standard. */
	EHrdVbvCodingStandard = 0x00000002,

	/** Annex G of 3GPP TS 26.234 Release 5. */
	EHrdVbv3GPP		   = 0x00000004
	};

/**
Defines the pre-processor and post-processor types available in the system. 
One pre-processor or post-processor can implement multiple operations simultaneously, and thus the 
types are defined as bit values that can be combined as a bitfield.
@publishedAll
@released
*/
enum TPrePostProcessType
	{
	/**
	Input cropping, used for pan-scan cropping in video playback and digital zoom in video recording. 
	Pan-scan cropping is useful, for example, for displaying arbitrary-sized pictures with codecs that 
	only support image dimensions that are multiples of 16 pixels.
	*/
	EPpInputCrop =		0x00000001,

	/**
	Horizontal mirroring, flips the image data around a vertical line in its center.
	*/
	EPpMirror =		   0x00000002,

	/**
	Picture rotation, supports rotation by 90 or 180 degrees, clockwise and anticlockwise.
	*/
	EPpRotate =		   0x00000004,

	/**
	Picture scaling to a new size, includes both upscaling and downscaling. 
	The supported scaling types and scale factors depend on the pixel processor.
	*/
	EPpScale =			0x00000008,

	/**
	Crops the picture to a final output rectangle.
	*/
	EPpOutputCrop =	   0x00000010,

	/**
	Pads the output picture to a defined size. Used in video recording to pad pictures to 
	suit the encoder input requirements.
	*/
	EPpOutputPad =		0x00000020,

	/**
	YUV to RGB color space conversion. Supported only for video playback.
	*/
	EPpYuvToRgb =		 0x00000040,

	/**
	RGB to YUV color space conversion. Supported only for video recording.
	*/
	EPpRgbToYuv =		 0x00000080,

	/**
	YUV to YUV data format conversion. Supported only for video recording.
	*/
	EPpYuvToYuv =		 0x00000100,

	/**
	Noise filtering. Noise filtering is typically used to enhance the input 
	picture from the camera, and is usually only supported for video recording.
	*/
	EPpNoiseFilter =	  0x00000200,

	/**
	Color enhancement.  Color enhancement is typically used to enhance the input picture 
	from the camera, and is usually only supported for video recording.
	*/
	EPpColorEnhancement = 0x00000400,

	/**
	Frame stabilisation. Supported only for video recording.
	*/
	EPpFrameStabilisation = 0x00000800,
	
	/**
	Deblocking is typically used to remove artefacts from the output picture that result from 
	high compression or a noisy input signal. Only supported for video playback.
	*/
    EPpDeblocking =         0x00001000,

	/**
	Deringing is typically used to remove artefacts from the output picture that result from 
	a noisy input signal corrupting motion estimates. Only supported for video playback.
	*/
    EPpDeringing =          0x00002000,
 
	/**
	Custom hardware device specific processing.
	*/
	EPpCustom =		   0x10000000
	};

/**
Dithering types.
@publishedAll
@released
*/
enum TDitherType
	{
	/** No dithering. */
	EDitherNone		   = 0x00000001,

	/** Ordered dither. */
	EDitherOrdered		= 0x00000002,

	/** Error diffusion dither. */
	EDitherErrorDiffusion = 0x00000004,

	/** Other hardware device specific dithering type. */
	EDitherOther		  = 0x00000008
	};

/**
Rotation types for pre-processors and post-processors.
@publishedAll
@released
*/
enum TRotationType
	{
	/**	No rotation. */
	ERotateNone			   = 0x00000001,

	/** Rotate the picture 90 degrees clockwise. */
	ERotate90Clockwise	 = 0x00000002,

	/** Rotate the picture 90 degrees anticlockwise. */
	ERotate90Anticlockwise = 0x00000004,

	/** Rotate the picture 180 degrees. */
	ERotate180			 = 0x00000008
	};



/**
Defines possible encoding bit-rate control modes.
@publishedAll
@released
*/
enum TBitrateControlType
	{
	/**
	The encoder does not control the bit-rate, but uses specified target picture quality and picture 
	rate as such. The coded data stream must still remain compliant with the standard and buffer settings 
	in use, if any, and thus HRD/VBV settings can limit the possible bit-rate.
	*/
	EBrControlNone	= 0x00000001,

	/**
	The encoder controls the coded bit-rate of the stream. The caller indicates target bit-rate, target 
	picture quality, target frame rate, spatial-temporal trade-off, and latency-quality trade-off.
	*/
	EBrControlStream  = 0x00000002,

	/**
	The encoder controls the coded bit-rate of each picture. The caller gives the target amount of bits per 
	frame. Each given input frame is coded. This type of operation is applicable only in memory-buffer-based 
	input.
	*/
	EBrControlPicture = 0x00000004
	};


/**
Defines the scalability type for a single bit-rate scalability layer.
@publishedAll
@released
*/
enum TScalabilityType
	{
	/**
	The layer uses temporal scalability. Using the layer increases the picture rate.
	*/
	EScalabilityTemporal		= 0x00000001,

	/**
	The layer uses quality scalability. Using the layer improves picture quality.
	*/
	EScalabilityQuality		 = 0x00000002,

	/**
	The layer uses spatial scalability. Using the layer increases picture resolution.
	*/
	EScalabilitySpatial		 = 0x00000004,

	/**
	The layer is a fine-granularity scalability layer. In fine granularity scalability, the output 
	quality increases gradually as a function of decoded bits from the enhancement layer.
	*/
	EScalabilityFineGranularity = 0x10000000,

	/**
	The layer is a fine-granularity quality scalability layer.
	*/
	EScalabilityQualityFG	   = EScalabilityFineGranularity | EScalabilityQuality
	};

/**
Forward error control strength used for an unequal error protection level. Also other values between 
EFecStrengthNone and EFecStrengthHigh can be used, the encoder will round the values to the levels 
it supports.
@publishedAll
@released
*/
enum TErrorControlStrength
	{
	/** No error control. */
	EFecStrengthNone = 0,

	/** Low error control strength. */
	EFecStrengthLow = 256,

	/** Normal error control strength. */
	EFecStrengthNormal = 512,

	/** High error control strength. */
	EFecStrengthHigh = 768
	};

/**
Defines the scalability type for in-layer bit-rate scalability.
@publishedAll
@released
*/
enum TInLayerScalabilityType
	{
	/** Temporal scalability, such as B-pictures. */
	EInLScalabilityTemporal = 1,

	/** Other scalability type. */
	EInLScalabilityOther
	};

/**
Defines what part of a frame is contained within a video buffer.
@publishedAll
@released
*/
enum TFramePortion
	{
	/** The frame portion is unknown. */
	EFramePortionUnknown,

	/** An entire frame. */
	EFramePortionWhole,

	/** A fragment of a frame containing the start but not the end. */
	EFramePortionStartFragment,

	/** An fragment of a frame containing neither the start nor the end. */
	EFramePortionMidFragment,

	/** A fragment of a frame containing the end but not the start. */
	EFramePortionEndFragment
	};

#endif

