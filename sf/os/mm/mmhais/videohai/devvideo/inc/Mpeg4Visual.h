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
// Constants and structs specific to Mpeg4 Visual.  See the DevVideo specs for more details.
// 
//

#ifndef __DEVVIDEO_MPEG4VISUAL_H__
#define __DEVVIDEO_MPEG4VISUAL_H__

#include <e32base.h>
#include <mmf/devvideo/h263.h>

/**
Specifies the picture type.
@publishedAll
@released
*/
enum TMPEG4VisualVOPType
    {
	/**
	An Intra-coded (I) VOP is coded using information only from itself.
	*/
    EMPEG4VisualVOPTypeI = 0x01,

	/**
	A Predictive-coded (P) VOP is a VOP which is coded using motion compensated prediction from a 
	past reference VOP.
	*/
    EMPEG4VisualVOPTypeP = 0x02,

	/**
	A Bidirectionally predictive-coded (B) VOP is a VOP which is coded using motion compensated 
	prediction from a past and/or future reference VOP(s).
	*/
    EMPEG4VisualVOPTypeB = 0x04,

	/**
	A sprite (S) VOP is a VOP for a sprite object or a VOP which is coded using prediction based on
	global motion compensation from a past reference VOP.
	*/
    EMPEG4VisualVOPTypeS = 0x08
    };

/**
Specifies the MPEG-4 Visual header types.
@publishedAll
@released
*/
enum TMPEG4VisualHeaderType
    {
	/**
	Visual Object Sequence Header.
	*/
    EMPEG4VisualHeaderSequence = 0x01,

	/**
	Visual Object Header.
	*/
    EMPEG4VisualHeaderObject   = 0x02,

	/**
	Video Object Layer Header.
	*/
    EMPEG4VisualHeaderVOL      = 0x04,

	/**
	Group of Video Object Plane Header.
	*/
    EMPEG4VisualHeaderGOV      = 0x08,

	/**
	Video Object Plane Header.
	*/
    EMPEG4VisualHeaderVOP      = 0x10
    };


/**
Object sequence header.
@publishedAll
@released
*/
class TMPEG4VisualObjectSequenceHeader
    {
public:
	/**
	Used to signal the profile and level identification according to Table G-1 of MPEG-4 Visual.
	*/
    TUint iProfileLevel;

	/**
	Contains the user_data bytes, if any, that are directly included in the VideoObjectSequence() 
	syntax structure (subclause 6.2.2 of MPEG-4 Visual), in bitstream order. The pointer must remain
	valid as long as the object it belongs to is being processed by the client (for playback) or MSL
	video subsystem (for recording).
	*/
    TPtrC8 iUserData;
    };

/**
Object header.
@publishedAll
@released
*/
class TMPEG4VisualObjectHeader
    {
public:
	/**
	Identifies the version number of the visual object as specified in the semantics of 
	visual_object_verid syntax element of MPEG-4 Visual.
	*/
    TUint iVisualObjectVerid;

	/**
	Specifies the priority of the visual object as specified in the semantics of 
	visual_object_priority syntax element of MPEG-4 Visual. If the visual_object_priority syntax 
	element is not present in the bitstream, iVisualObjectPriority shall be equal to 0.
	*/
    TUint iVisualObjectPriority;

	/**
	Identifies the type of the visual object as specified in the semantics of 
	visual_object_type syntax element of MPEG-4 Visual. HW devices according to this specification 
	are required to set iVisualObjectType to 1 (to indicate "video ID").
	*/
    TUint iVisualObjectType;

	/**
	Uniquely identifies the video object. The value of iVideoObjectId shall be the same as the 
	value of the video_object_id syntax element specified in MPEG-4 Visual.
	*/
    TUint iVideoObjectId;

	/**
	Contains the user_data bytes, if any, that are directly included in the VideoObject() syntax 
	structure (subclause 6.2.2 of MPEG-4 Visual), in bitstream order. The pointer must remain valid
	as long as the object it belongs to is being processed by the client (for playback) or MSL video
	subsystem (for recording).
	*/
    TPtrC8 iUserData;
    };

/**
Mpeg4 Visual VBV parameters.

If the syntax elements that are used to derive the values of the class member variables are not 
present in the bitstream, the variable values shall be set to default values as specified in 
Annex D of MPEG-4 Visual.
@publishedAll
@released
*/
class TMPEG4VisualVbvParams
    {
public:
	/**
	Specifies the instantaneous video object layer channel bit rate in bits per second. Shall be set
	to ((first_half_bit_rate << 15) + latter_half_bit_rate) * 400, where the values of 
	first_half_bit_rate and latter_half_bit_rate are conveyed in the VideoObjectLayer syntax 
	structure of MPEG-4 Visual.
	*/
    TInt64 iBitRate;

	/**
	Specifies the VBV buffer size in bytes. Shall be set to 
	((first_half_vbv_buffer_size) << 3) + latter_half_vbv_buffer_size) * 2048, 
	where the values of first_half_vbv_buffer_size and latter_half_vbv_buffer_size are conveyed in 
	the VideoObjectLayer syntax structure of MPEG-4 Visual.
	*/
    TUint32 iVbvBufferSize;

	/**
	Specifies VBV occupancy in bytes just before the removal of the first VOP following the VOL 
	header. The purpose for the quantity is to provide the initial condition for VBV buffer fullness. 
	Shall be set to ((first_half_vbv_occupancy) << 15) + latter_half_vbv_occupancy) * 2048, where
	the values of first_half_vbv_occupancy and latter_half_vbv_occupancy are conveyed in the 
	VideoObjectLayer syntax structure of MPEG-4 Visual.
	*/
    TUint32 iVbvOccupancy;
    };


/**
Video object layer header.
@publishedAll
@released
*/
class TMPEG4VisualVOLHeader
    {
public:
	/**
	Uniquely identifies the video object layer. The value of iVideoObjectLayerId shall be the same
	as the value of the video_object_layer_id syntax element specified in MPEG-4 Visual.
	*/
    TUint iVideoObjectLayerId;

	/**
	iShortVideoHeader equal to ETrue indicates that the associated elementary stream conforms to
	ITU-T Recommendation H.263 (without optional coding modes).  The value of iShortVideoHeader 
	shall be equal to the value of the short_video_header flag of MPEG-4 Visual. 
	*/
    TBool iShortVideoHeader;

	/**
	iRandomAccessibleVOL equal to ETrue indicates that every VOP in this VOL is individually
	decodable. The value of iRandomAccessibleVOL shall be equal to the value of the 
	random_accessible_vol flag of MPEG-4 Visual.
	*/
    TBool iRandomAccessibleVOL;

	/**
	Indicates the object type as specified in Table 6-10 of MPEG-4 Visual and constrains the 
	associated elementary stream to use tools from the indicated object type. HW devices according
	to this specification are required to set iVideoObjectTypeIndication to 1 (to indicate Simple
	Object Type).
	*/
    TUint iVideoObjectTypeIndication;

	/**
	Identifies the version number of the video object layer as specified in the semantics of 
	video_object_layer_verid syntax element of MPEG-4 Visual.
	*/
    TUint iVideoObjectLayerVerid;

	/**
	*/
    TUint iVideoObjectLayerPriority;

	/**
	Specifies the priority of the video object layer as specified in the semantics of 
	video_object_layer_priority syntax element of MPEG-4 Visual. If the video_object_layer_priority
	syntax element is not present in the bitstream, iVideoObjectLayerPriority shall be equal to 0.
	*/
    TUint iAspectRatioNum;

	/**
	Pixel aspect ratio numerator and denominator respectively. The pixel aspect ratio is defined as 
	iAspectRatioNum/iAspectRatioDenom, where the values are positive integers and relatively prime. 
	These values shall be set according to the value of aspect_ratio_info, par_width (if present), 
	and par_height (if present) syntax elements in the VideoObjectLayer() syntax structure of MPEG-4
	Visual.
	*/
    TUint iAspectRatioDenom;

	/**
	Specifies the VBV parameters in use for the VOL. The values in iVbvParams are valid if 
	iShortVideoHeader equals to EFalse. If iShortVideoHeader equals to ETrue, the VBV operation and 
	parameters are specified in Annex D of MPEG-4 Visual.
	*/
    TMPEG4VisualVbvParams iVbvParams;

	/**
	Indicates the number of evenly spaced subintervals, called ticks, within one modulo time. One
	modulo time represents the fixed interval of one second. Shall be set equal to the value of 
	vop_time_increment_resolution of the VideoObjectLayer() syntax structure of MPEG-4 Visual.
	*/
    TUint16 iVOPTimeIncrementResolution;

	/**
	iFixedVOPRate equal to ETrue indicates that all VOPs are coded with a fixed VOP rate. 
	iFixedVOPRate equal to EFalse indicates that some VOPs may not be coded with a fixed VOP rate. 
	Shall be set equal to the value of fixed_vop_rate of the VideoObjectLayer() syntax structure
	of MPEG-4 Visual.
	*/
    TBool iFixedVOPRate;

	/**
	The number of ticks between two successive VOPs in the display order. Valid only if 
	iFixedVOPRate is equal to ETrue. Shall be set equal to the value of fixed_vop_time_increment 
	of the VideoObjectLayer() syntax structure of MPEG-4 Visual.
	*/
    TUint16 iFixedVOPTimeIncrement;

	/**
	iDataPartitioning equal to ETrue indicates that slices are organized in data partitions within 
	the associated elementary bitstream. Shall be set equal to the value of the data_partitioned 
	syntax element of the VideoObjectLayer() syntax structure of MPEG-4 Visual.
	*/
    TBool iDataPartitioning;

	/**
	iReversibleVLC equal to ETrue indicates that the reversible variable length tables of MPEG-4 
	Visual are in use in the associated elementary bistream. Shall be set equal to the value of the
	reversible_vlc syntax element of the VideoObjectLayer() syntax structure of MPEG-4 Visual. If 
	reversible_vlc is not present in the bitstream, the value of iReversibleVLC shall be set to 
	EFalse.
	*/
    TBool iReversibleVLC;

	/**
	Contains the user_data bytes, if any, that are directly included in the VideoObjectLayer()
	syntax structure, in bitstream order. The pointer must remain valid as long as the object it 
	belongs to is being processed by the client (for playback) or MSL video subsystem (for 
	recording).
	*/
    TPtrC8 iUserData;
    };


/**
Mpeg4 visual GOV header.
@publishedAll
@released
*/
class TMPEG4VisualGOVHeader
    {
public:
	/**
	iTimeCodeHours, iTimeCodeMinutes and iTimeCodeSeconds together specify the modulo part (i.e. the 
	full second units) of the time base for the first object plane (in display order) after the GOV 
	header according to the semantics of the time_code syntax element of MPEG-4 Visual.
	*/
    TUint iTimeCodeHours;

	/**
	iTimeCodeHours, iTimeCodeMinutes and iTimeCodeSeconds together specify the modulo part (i.e. the 
	full second units) of the time base for the first object plane (in display order) after the GOV 
	header according to the semantics of the time_code syntax element of MPEG-4 Visual.
	*/
    TUint iTimeCodeMinutes;

	/**
	iTimeCodeHours, iTimeCodeMinutes and iTimeCodeSeconds together specify the modulo part (i.e. the 
	full second units) of the time base for the first object plane (in display order) after the GOV 
	header according to the semantics of the time_code syntax element of MPEG-4 Visual.
	*/
    TUint iTimeCodeSeconds;

	/**
	Indicates the nature of the predictions used in the first consecutive B-VOPs (if any) 
	immediately following the first coded I-VOP after the GOV header. iClosedGOV equal to ETrue
	indicates that there are no such B-VOPs or that these B-VOPs have been encoded using only 
	backward prediction or intra coding. The value of iClosedGOV shall be set equal to the value of
	closed_gov syntax element of MPEG-4 Visual.
	*/
    TBool iClosedGOV;

	/**
	iBrokenLink equal to ETrue indicates that the first consecutive B-VOPs (if any) immediately 
	following the first coded I-VOP following the GOV header may not be correctly decoded because
	the reference frame which is used for prediction is not available (e.g., due to result of 
	editing in compressed domain). A decoder may use this flag to avoid displaying frames that 
	cannot be correctly decoded. The value of iBrokenLink shall be set equal to the value of 
	broken_link syntax element of MPEG-4 Visual.
	*/
    TBool iBrokenLink;
    };


/**
Mpeg4 visual VOP header.
@publishedAll
@released
*/
class TMPEG4VisualVOPHeader
    {
public:
	/**
	Indicates the coding type of the VOP.
	*/
    TMPEG4VisualVOPType iVOPCodingType;

	/**
	Indicates the number of seconds elapsed since the previous GOV header or since the previous
	picture in display order, whichever is closer in display order. The value of iModuloTimeBase 
	shall be set equal to the decoded value derived from a series of modulo_time_base fields as 
	specified in MPEG-4 Visual.
	*/
    TUint iModuloTimeBase;

	/**
	VOP display time relative to iModuloTimeBase in clock ticks. The value of iVOPTimeIncrement 
	shall be set equal to the value of the vop_time_increment syntax element of MPEG-4 Visual.
	*/
    TUint16 iVOPTimeIncrement;

	/**
	iVOPCoded equal to EFalse indicates that the VOP is a copy of the previous VOP in display order.
	The value of iVOPCoded shall be set equal to the value of the vop_coded syntax element of MPEG-4
	Visual.
	*/
    TBool iVOPCoded;

	/**
	Indicates the initial value of the quantization parameter. iVOPQuant shall be set equal to the
	value of the vop_quant syntax element.
	*/
    TUint iVOPQuant;

	/**
	Indicates the VOP ID. iVOPId shall be set equal to the value of the vop_id syntax element. Valid
	only if the flag newpred_enable is equal to 1.
	*/
    TUint16 iVOPId;

	/**
	When equal to ETrue indicates the following iVOPIdForPrediction is valid. Valid only if the flag
	newpred_enable is equal to 1.
	*/
    TBool iVOPIdForPredictionValid;

	/**
	Indicates VOP ID of the VOP that is used as the reference VOP for the current VOP. 
	iVOPIdForPrediction shall be set equal to the value of the vop_id_for_prediction if the syntax 
	element is present (i.e. the ). Valid only if iVOPIdForPredictionValid is equal to ETrue.
	*/
    TUint16 iVOPIdForPrediction;
    };


/**
This class is used to convey information of Visual Object Sequence, Visual Object, VOL, GOV, and VOP 
headers that are consecutive in decoding order without any intervening syntax structures.
@publishedAll
@released
*/
class TMPEG4VisualHeader
    {
public:
	/**
	A binary OR of values specified in TMPEG4VisualHeaderType:
		* EMPEG4VisualHeaderSequence - set if the visual object sequence header is present and 
		  iVisualObjectSequenceHeader is set accordingly. Otherwise, iVisualObjectSequenceHeader 
		  shall be set to NULL.
		* EMPEG4VisualHeaderObject - set if the visual object header is present and 
		  iVisualObjectHeader is set accordingly. Otherwise, iVisualObjectHeader shall be set
		  to NULL.
		* EMPEG4VisualHeaderVOL - set if the VOL header is present and iVOLHeader is set accordingly.
		  Otherwise, iVOLHeader shall be set to NULL.
		* EMPEG4VisualHeaderGOV - set if the GOV header is present and iGOVHeader is set accordingly.
		  Otherwise, iGOVHeader shall be set to NULL.
		* EMPEG4VisualHeaderVOP - set if the VOP header is present and iVOPHeader is set accordingly.
		  Otherwise, iVOPHeader shall be set to NULL
	*/
    TUint32 iConsecutiveHeaders;

	/**
	The visual object sequence header.
	*/
    const TMPEG4VisualObjectSequenceHeader* iVisualObjectSequenceHeader;

	/**
	The visual object header.
	*/
    const TMPEG4VisualObjectHeader* iVisualObjectHeader;

	/**
	The VOL header.
	*/
    const TMPEG4VisualVOLHeader* iVOLHeader;

	/**
	The GOV header.
	*/
    const TMPEG4VisualGOVHeader* iGOVHeader;

	/**
	The VOP header.
	*/
    const TMPEG4VisualVOPHeader* iVOPHeader;
    };



/**
This class is used to signal decoder or encoder capabilities.
@publishedAll
@released
*/
class TMPEG4VisualCapability
    {
public:
	/*
	iProfileLevel[ i ] indicates a supported combination of profile and level, i.e., 
	profile_and_level_indication, according to Table G-1 of MPEG-4 Visual. The values of i from 62 
	to 255 are reserved (the MPEG-4 Visual standard referenced in the present document has 62 
	combinations of profile and level). 
	*/
    TUint8 iProfileLevel[256];
    };


/**
This class is used to set the encoder operation mode.
@publishedAll
@released
*/
class TMPEG4VisualNormalMPEG4Mode
    {
public:
	/**
	Specifies the picture types allowed in the bitstream. The value is a binary OR of values from
	TMPEG4VisualVOPType. Signaled picture types that are not included in the prevailing coding 
	profile are ignored.
	*/
    TMPEG4VisualVOPType iAllowedVOPTypes;
	
	/**
	Specifies the number of consecutive video packet headers within a VOP, starting from the first
	video packet header in decoding order, where the value of header_extension_code shall be set to 1. 
	*/
    TUint iHeaderExtension;

	/**
	Specifies whether data partitioning is in use. When equal to ETrue, data partitioning is in use.
	When equal to EFalse, data partitioning is not in use. If data partitioning is in use, the 
	SetErrorProtectionLevelsL method, if used, should set the number of unequal error protection
	levels to be larger than one. 
	*/
    TBool iDataPartitioning;
	
	/**
	Specifies whether reversible variable length coding is in use. When equal to ETrue, reversible 
	variable length coding is in use. When equal to EFalse, reversible variable length coding is 
	not in use. Valid only if iDataPartitioned is equal to ETrue.
	*/
    TBool iReversibleVLC;
	
	/**
	Specifies which headers are included in the first output buffer of each intra VOP. (Note: Video
	Object Sequence and Video Object Headers can be repeated for error resiliency. VOL Header 
	includes the initial buffer occupancy level. GOV header includes an update on display times.)
	*/
    TMPEG4VisualHeaderType iHeadersBeforeIntraVOP;
    };

/**
This class is used to set the encoder operation mode.
@publishedAll
@released
*/
class TMPEG4VisualMode
    {
public:
	/**
	Indicates whether the short header mode of MPEG-4 Visual is used. If iShortHeaderMode is 
	equal to ETrue, then iH263VideoMode is valid and all other parameter values are invalid. If 
	iShortHeaderMode is equal to EFalse, then iH263VideoMode is invalid and all other parameter 
	values are valid. 
	*/
    TBool iShortHeaderMode;
	
	/**
	Contains the encoding modes to use when iShortHeaderMode is EFalse.
	*/
    TMPEG4VisualNormalMPEG4Mode iMPEG4VisualNormalMPEG4Mode;

	/**
	Contains the encoding modes to use when iShortHeaderMode is ETrue.
	*/
    TH263VideoMode iH263VideoMode;
    };

	

#endif
