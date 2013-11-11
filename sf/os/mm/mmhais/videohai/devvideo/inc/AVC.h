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
// Constants and structs specific to AVC.  See the DevVideo specs for more details.
// 
//

#ifndef __DEVVIDEO_AVC_H__
#define __DEVVIDEO_AVC_H__

#include <e32base.h>

/**
Specifies the picture type.
@publishedAll
@released
*/
enum TAvcPictureType
    {
	/**
	INTRA picture, a picture consisted of at least one I slice and zero or more SI slices
	*/
	EAvcPictureTypeI   = 0x01,

	/**
	INTER picture, a picture consisted of at least one P slice and zero or more I, SI and SP slices
	*/
	EAvcPictureTypeP   = 0x02,

	/**
	Bi-predictive picture, a picture consisted of at least one B slice and zero or more I, P, SI, 
	and SP slices 
	*/
	EAvcPictureTypeB   = 0x04,

	/**
	SI picture or switching I picture, a picture consisted of SI slices
	*/
	EAvcPictureTypeSI  = 0x08,

	/**
	SP picture or switching P picture, a picture consisted of at least one SP slice and zero or more
	SI slices
	*/
	EAvcPictureTypeSP  = 0x10
    };


/**
These enumerations can be used in codec capability indication and/or bitstream property indication.
When used as part of an encoder capability indication, a signaled value indicates the capability 
of an encoder to produce such Supplemental Enhancement Information (SEI) to the bitstream and the 
additional meanings as specified below. When used as part of a decoder capability indication, a 
signalled value indicates the capability of a decoder to react to such information in a way as
specified below. If a decoder does not have such capability, it shall be able to parse and discard
the information. When used as part of a bitstream property indication, a non-signalled value 
indicates the absence of such information in the bitstream and a signalled value indicates the 
possibility of having such information in the bitstream.
@publishedAll
@released
*/
enum TAvcSupplementalInformation
    {
	/**
	encoder capability:	no meaning
	decoder capability:	capability to initialize Hypothetical Reference Decoder (HRD) as specified 
						in AVC Annex C at any buffering period SEI message
	bitstream property:	existence of Network Abstraction Layer (NAL) units with nal_unit_type equal
						to 6 and the SEI payload type (payloadType) equal to 0 
	*/
	EAvcSeiBufferingPeriod			 				= 0x00000001,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to operate HRD and decode/display video streams according to the
						provided timing information
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal 
						to 1
	*/
	EAvcSeiPictureTiming         					= 0x00000002,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to crop after decoding and to display the specified rectangular 
						region 
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal 
						to 2
	*/
	EAvcSeiPanScanRectangle         				= 0x00000004,

	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal
						to 3
	*/
	EAvcSeiFillerPayload 							= 0x00000008,

	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal 
						to 4
	*/
	EAvcSeiUserDataRegisteredByITUTRecT35  			= 0x00000010,

	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal
						to 5
	*/
	EAvcSeiUserDataUnRegistered       				= 0x00000020,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to start decoding from any access unit with a recovery point SEI 
						message
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal
						to 6
	*/
	EAvcSeiRecoveryPoint    						= 0x00000040,

	/**
	encoder capability:	capability to repeat the decoded reference picture marking syntax structure
						as specified in subclauses 7.3.3.3 and 7.4.3.3 of the AVC specification.
	decoder capability:	capability to recover the decoded reference picture marking process due to
						losses of reference pictures with decoded reference picture marking syntax
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal 
						to 7
	*/
	EAvcSeiDecodedReferencePictureMarkingRepetition	= 0x00000080,

	/**
	encoder capability:	capability to analyze video pictures for creation of spare picture information
	decoder capability:	capability to utilize the spare picture information to enhance video decoding
						in case there is loss of reference pictures
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal
						to 8
	*/
	EAvcSeiSparePicture		       					= 0x00000100,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to utilize the scene information to enhance error concealment
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal
						to 9
	*/
	EAvcSeiSceneInformation						   	= 0x00000200,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to utilize the sub-sequence information to achieve temporal
						scalability
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType
						equal to 10
	*/
	EAvcSeiSubsequenceInformation		     		= 0x00000400,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to utilize the sub-sequence information and the sub-sequence
						layer characteristics to achieve temporal scalability
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType 
						equal to 11 (if sub-sequence layer characteristics SEI message is present,
						sub-sequence information SEI message must be present)
	*/
	EAvcSeiSubsequenceLayerCharacteristics       	= 0x00000800,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to utilize the sub-sequence information and the sub-sequence 
						characteristics to achieve temporal scalability
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal
						to 12 (if sub-sequence characteristics SEI message is present, sub-sequence
						information SEI message must be present)
	*/
	EAvcSeiSubsequenceCharacteristics       		= 0x00001000,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to freeze displayed picture according to AVC Annex D.2.14.
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal
						to 13
	*/
	EAvcSeiFullFrameFreeze	               			= 0x00002000,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to release frozen displayed picture according to AVC Annex D.2.15
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType 
						equal to 14
	*/
	EAvcSeiFullFrameFreezeRelease					= 0x00004000,

	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType
						equal to 15
	*/
	EAvcSeiFullFrameSnapshot				    	= 0x00008000,

	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType
						equal to 16
	*/
	EAvcSeiProgressiveRefinementSegmentStrat     	= 0x00010000,

	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType 
						equal to 17
	*/
	EAvcSeiProgressiveRefinementSegmentEnd   		= 0x00020000,

	/**
	encoder capability:	capability to constrain the inter prediction process according to 
						AVC Annex D.2.19
	decoder capability:	capability to decode only part of the pictures covered by the motion-
						constrained slice group set
	bitstream property:	existence of NAL units with nal_unit_type equal to 6 and payloadType equal
						to 18
	*/
	EAvcSeiMotionConstrainedSliceGroupSet        	= 0x00040000
    };


/**
AVC Picture Header.
@publishedAll
@released
*/
class TAvcPictureHeader
    {
public:
	/**
	Equal to ETrue indicates that the bitstream obeys all constraints specified in subclause A.2.1
	of the AVC specification, equal to EFalse indicates that the bitstream may or may not obey the 
	constraints. The value is equal to ETrue (EFalse) if the syntax element constraint_set0_flag is
	equal to 1 (0). 
	*/
	TBool iBaselineProfileConstraintSet;

	/**
	Equal to ETrue indicates that the bitstream obeys all constraints specified in subclause A.2.2
	of the AVC specification, equal to EFalse indicates that the bitstream may or may not obey the 
	constraints. The value is equal to ETrue (EFalse) if the syntax element constraint_set1_flag is
	equal to 1 (0). 
	*/
	TBool iMainProfileConstraintSet;

	/**
	Equal to ETrue indicates that the bitstream obeys all constraints specified in subclause A.2.3 
	of the AVC specification, equal to EFalse indicates that the bitstream may or may not obey the
	constraints. The value is equal to ETrue (EFalse) if the syntax element constraint_set2_flag is
	equal to 1 (0). 
	*/
	TBool iExtendedProfileConstraintSet;

	/**
	Indicates the picture type. iPictureType is set as one of the values defined in TAvcPictureType.
	*/
	TAvcPictureType iPictureType;

	/**
	iLongTermPicture equal to ETrue indicates that the picture is a long-term picture, i.e., it has 
	been marked as "used for long-term reference" according to the decoded reference picture marking 
	process as specified in subclause 8.2.5 of the AVC specification. Otherwise the picture is not a 
	long-term picture.
	*/
	TBool iLongTermPicture;

	/**
	Indicates whether the picture is a frame or a field. iFrame is equal to ETrue if the picture is 
	a frame (field_pic_flag is equal to 0), EFalse if the picture is a field (field_pic_flag is equal
	to 1).
	*/
	TBool iFrame;

	/**
	Valid only if iFrame is equal to EFalse. Indicates whether the field picture is a top field or a
	bottom field. iTopField is equal to ETrue if the picture is a top field (bottom_field_flag is 
	equal to 0), EFalse if the picture is a bottom field (bottom_field_flag is equal to 0).
	*/
	TBool iTopField;

	/**
	Indicates the picture order count of the top field of a frame, the top field of a complementary 
	field pair, or a top field. iTopFieldOrderCount is equal to the value of TopFieldOrderCnt as 
	specified in subclause 8.2.1 of the AVC specification.
	*/
	TUint iTopFieldOrderCount;

	/**
	Indicates the picture order count of the bottom field of a frame, the bottom field of a 
	complementary field pair, or a bottom field. iBottomFieldOrderCount is equal to the value of 
	BottomFieldOrderCnt as specified in subclause 8.2.1 of the AVC specification.
	*/
	TUint iBottomFieldOrderCount;

	/**
	Equal to ETrue indicates that the picture is an Instantaneous Decoding Refresh (IDR) picture;
	otherwise the picture is not an IDR picture. The value of nal_unit_type of all the Video Coding
	Layer (VCL) NAL units of the primary coded picture of an IDR picture shall be equal to 5.
	*/
	TBool iIdrPicture;

	/**
	Indicates the initial value of the quantization parameter. If iPictureType is equal to 
	EAvcPictureTypeSI or EavcPictureTypeSP, iPQY is equal to pic_init_qs_minus26 + 26; otherwise
	iPQY is equal to pic_init_qp_minus26 + 26.
	*/
	TUint iQPY;

	/**
	Pixel aspect ratio numerator. The pixel aspect ratio is defined as 
	iAspectRatioNum/iAspectRatioDenom, where the values are positive integers and relatively prime. 
	When aspect_ratio_idc is equal to 0 or sar_width is equal to 0 or sar_height is equal to 0, the 
	sample aspect ratio shall be considered unspecified, and iAspectRatioNum and iAspectRationDenom 
	are set to 0. When aspect_ratio_idc is equal to any of the value from 1 to 13, inclusive, 
	iAspectRatioNum and iAspectRatioDenom are set according to Table E-1 of the AVC specification.
	When aspect_ratio_idc is equal to 255 (Extended_SAR), iAspectRatioNum is set to sar_width and 
	iAspectRatioDenom is set to sar_height.
	*/
	TUint iAspectRatioNum;

	/**
	Pixel aspect ratio denominator. The pixel aspect ratio is defined as 
	iAspectRatioNum/iAspectRatioDenom, where the values are positive integers and relatively prime. 
	When aspect_ratio_idc is equal to 0 or sar_width is equal to 0 or sar_height is equal to 0, the 
	sample aspect ratio shall be considered unspecified, and iAspectRatioNum and iAspectRationDenom
	are set to 0. When aspect_ratio_idc is equal to any of the value from 1 to 13, inclusive, 
	iAspectRatioNum and iAspectRatioDenom are set according to Table E-1 of the AVC specification.
	When aspect_ratio_idc is equal to 255 (Extended_SAR), iAspectRatioNum is set to sar_width and 
	iAspectRatioDenom is set to sar_height.
	*/
	TUint iAspectRatioDenom;

	/**
	Indicates the frame cropping rectangle as specified in subclause 7.4.2.1 of the AVC specification.
	*/
	TRect iFrameCropRect;

	/**
	Equal to 0 indicates that the cropped (according to the frame cropping rectangle) decoded 
	pictures output contain visually important information in the entire region out to the edges of 
	the cropping rectangle of the picture, such that the cropped decoded pictures output should not 
	be displayed using overscan. Instead, they should be displayed using either an exact match 
	between the display area and the cropping rectangle, or using underscan. Equal to 1 indicates
	that the cropped decoded picture output is suitable for display using overscan. Otherwise, it is 
	unspecified whether the cropped decoded picture output is suitable for display using overscan or
	not. 
	*/
	TUint iOverscanInfo;

	/**
	Indicates the representation of the pictures as specified in Table E-2 of the AVC specification, 
	before being coded. 
	*/
	TUint iVideoFormat;

	/**
	Indicates whether a picture should be displayed as a frame or one or more fields, according to
	Table D-1. iPictureStructure is equal to the syntax element pic_struct.
	*/
	TUint iPictureStructure;

	/**
	Each picture has up to NumClockTS sets of clock timestamp information. The value of NumClockTS
	is as specified in Table D-1 of the AVC specification, with a maximum value equal to 3. Each 
	instance of iClockTimestamp[i] indicates a time of origin, capture, or alternative ideal display,
	in units of microseconds. iClockTimestamp[i] is equal to 
	(clockTimestamp[i] / time_scale * 1000000), where clockTimestamp[i] is the i-th set of clock
	timestamp information as specified in subclause D.2.2 of the AVC specification. 
	iClockTimestamp[i] where i is larger than NumClockTS-1 is invalid. When picture timing 
	information is not available, NumClockTS is set to zero.
	*/
	TTimeIntervalMicroSeconds iClockTimestamp[3];
    };

/**
This class is used to signal decoder or encoder capabilities or bitstream properties.
@publishedAll
@released
*/
class TAvcVideoCapability
    {
public:
	/**
	When used as part of a codec capability indication, iProfileLevel[ProfileIdc] indicates the 
	maximum processing level that a decoder or an encoder supports for a particular profile set 
	indicated by ProfileIdc (the indication is described below). A negative value indicates that
	the profile set is not supported. When used as part of a stream property indication,
	iProfileLevel[ProfileIdc] indicates the maximum processing level that is present in the stream
	for a particular profile set indicated by ProfileIdc. A negative value indicates that the stream 
	does not contain data coded according to the indicated profile set.

	ProfileIdc equal to 0 indicates that all of constraint_set0_flag, constraint_set1_flag and 
	constraint_set2_flag are equal to 1, and profile_idc may indicate any of the profiles (baseline,
	main or extended). ProfileIdc equal to 1 indicates that constraint_set0_flag and 
	constraint_set2_flag are equal to 1 while constraint_set1_flag is equal to 0, and 
	profile_idc may indicate either the baseline or extended profile. ProfileIdc equal to 2 
	indicates that constraint_set1_flag and constraint_set2_flag are equal to 1 while 
	constraint_set0_flag is equal to 0, and profile_idc may indicate either the main or extended 
	profile. ProfileIdc equal to 3 indicates that constraint_set1_flag is equal to 1 while the other
	two flags are equal to 0, and profile_idc indicates the main profile. ProfileIdc equal to 4 
	indicates that constraint_set2_flag is equal to 1 while the other two flags are equal to 0, and
	profile_idc indicates the extended profile.
	*/
	TInt iProfileLevel[5];

	/**
	iSupplementalInformationCap indicates the supported supplemental enhancement functions. The 
	value is a binary OR of values from TAvcSupplementalInformation.
	*/
	TUint32 iSupplementalInformationCap;
    };


/**
This class is used to set the encoder operation mode.
@publishedAll
@released
*/
class TAvcVideoMode
    {
public:
	/**
	Specifies the picture types allowed in the bitstream. The value is a binary OR of values from 
	TAvcPictureType. Signaled picture types that are not included in the prevailing coding profile 
	are ignored.
	*/
	TUint32 iAllowedPictureTypes;

	/**
	Specifies whether Flexible Macroblock Order (FMO) is allowed. Equal to ETrue indicates that 
	num_slice_groups_minus1 is in the range of 0 to 7, inclusive, i.e. FMO is allowed. Equal to
	EFalse indicates that num_slice_groups_minus1 shall be equal to 0 only, i.e., FMO is not allowed.
	*/
	TBool iFlexibleMacroblockOrder;

	/**
	Specifies whether redundant pictures may be encoded. Equal to ETrue indicates that 
	redundant_pic_cnt_present_flag can be equal to either 0 or 1 (i.e., redundant pictures may be 
	encoded), while Equal to EFalse indicates that redundant_pic_cnt_present_flag shall be equal to
	0 only (i.e., redundant pictures cannot be encoded). 
	*/
	TBool iRedundantPictures;

	/**
	Specifies whether data partitioning is in use. When equal to ETrue, data partitioning is in use,
	i.e., NAL unit streams can contain nal_unit_type values in the range of 2 to 4, inclusive. When 
	equal to EFalse, data partitioning is not in use, i.e., NAL unit streams shall not contain 
	nal_unit_type values in the range of 2 to 4, inclusive. If data partitioning is in use, the 
	SetErrorProtectionLevelsL method, if used, should set the number of unequal error protection 
	levels to be larger than one. 
	*/
	TBool iDataPartitioning;

	/**
	iFrameMBsOnly equal to ETrue specifies that every coded picture of the coded video sequence is a
	coded frame containing only frame macroblocks. Equal to EFalse specifies that coded pictures of 
	the coded video sequence may either be coded fields or coded frames.
	*/
	TBool iFrameMBsOnly;

	/**
	iMBAFFCoding equal to EFalse specifies no switching between frame and field 
	macroblocks within a picture. Equal to ETrue specifies the possible use of switching between 
	frame and field macroblocks within frames. Valid only when iFrameMBsOnly is equal to EFalse.
	*/
	TBool iMBAFFCoding;

	/**
	Specifies the entropy decoding method to be applied for the syntax elements for which two 
	descriptors appear in the syntax tables. Equal to EFalse indicates that Exp-Golomb codes or 
	CAVLC (Context-based Adaptive Variable Length Coding), as described in subclause 9.1 or 9.2 of
	the AVC specification, is applied. Otherwise, CABAC (Context-based Adaptive Binary Arithmetic 
	Coding) as described in subclause 9.3 of the AVC specification is applied.
	*/
	TBool iEntropyCodingCABAC;

	/**
	Equal to EFalse specifies that weighted prediction shall not be applied to P and SP slices.
	Equal to ETrue specifies that weighted prediction may be applied to P and SP slices. 
	*/
	TBool iWeightedPPrediction;

	/**
	Equal to 0 specifies that the default weighted prediction is applied to B slices. Equal to 1 
	specifies that explicit weighted prediction is applied to B slices. Equal to 2 specifies that 
	implicit weighted prediction is applied to B slices.
	*/
	TUint iWeightedBipredicitonMode;

	/**
	Specifies the method used in the derivation process for luma motion vectors for B_Skip, 
	B_Direct_16x16 and B_Direct_8x8 as specified in subclause 8.4.1.2 of the AVC specification. When
	iFrameMBsOnly is equal to EFalse, iDirect8x8Inference shall be equal to ETrue. 
	*/
	TBool iDirect8x8Inference;
    };

/**
TAvcHrdParams specifies a set of AVC-specific HRD parameters. All pointers included in the objects
have the same lifetime as the objects themselves, and the memory is allocated and deallocated by
the same instance that creates and owns the object.
@publishedAll
@released
*/
class TAvcHrdParams
    {
public:
    class TCpbParams
        {
    public:
		/**
		Specifies the delay for a CPB between the time of arrival in the CPB of the first bit of the 
		coded data associated with the access unit associated with the buffering period SEI message 
		and the time of removal from the CPB of the coded data associated with the same access unit,
		for the first buffering period after HRD initialisation. It is in units of a 90 kHz clock.
		The value is equal to the corresponding initinal_cpb_removal_delay of the NAL HRD parameters
		if the current parameter is for the NAL HRD or of the VCL HRD parameters if the current
		parameter is for the VCL HRD. 
		*/
		TUint iInitialCpbRemovalDelay;

		/**
		This parameter is used for a CPB in combination with the cpb_removal_delay to specify the 
		initial delivery time of coded access units to the CPB.  It is in units of a 90 kHz clock.
		The value is equal to the corresponding initinal_cpb_removal_delay_offset of the NAL HRD 
		parameters if the current parameter is for the NAL HRD or of the VCL HRD parameters if the
		current parameter is for the VCL HRD.

		Note: Over the entire coded video sequence, the sum of iInitialCpbRemovalDelay and the 
		corresponding iInitialCpbRemovalDelayOffset shall be constant for either the NAL HRD
		parameters or the VCL HRD parameters.
		*/
		TUint iInitialCpbRemovalDelayOffset;

		/**
		Equal to EFalse specifies that to decode this bitstream by the HRD using the CPB 
		specification, the hypothetical stream scheduler (HSS) operates in an intermittent bit 
		rate mode.  Equal to 1 specifies that the HSS operates in a constant bit rate (CBR) mode. 
		*/
		TBool iCbr;

		/**
		Specifies the maximum input bit rate, in bits per second, of a CPB. The value is equal to 
		Bitrate as specified in subclause E.2.2 of the AVC specification.
		*/
		TUint iBitrate;

		/**
		Specifies the size, in bytes, of a CPB. The value is equal to CpbSize/8, where CpbSize is as
		specified in subclause E.2.2 of the AVC specification.
		*/
		TUint iCpbSize;
        };
    
	/**
	Specifies the required size of the decoded picture buffer (DPB) in unit of bytes. The value is
	equal to max_dec_frame_buffering* PicWidthInSamplesL*FrameHeightInMbs*2, where PicWidthInSamplesL
	and FrameHeightInMbs are specified in subclause 7.4.2.1 of the AVC specification amd the syntax
	element max_dec_frame_buffering as specified in subclause E.2.1 of the AVC specification.
	*/
	TUint iDpbSize;
    
	/**
	Specifies the HRD operational mode as specified in Annex C of the AVC specification. The value
	of LowDelayHrd is equal to ETrue if the syntax element low_delay_hrd_flag is equal to 1, and
	EFalse otherwise. This parameter is valid only if at least one of iNalCpbCount and iVclCpbCount
	is larger than 0.
	*/
	TBool iLowDelayHrd;
    
	/**
	Specifies the number of alternative coded picture buffer (CPB) specifications for NAL HRD 
	parameters in the bitstream. The value of iNalCpbCount is equal to cpb_cnt_minus1 + 1 in the 
	NAL HRD parameters, if present. iNalCpbCount equal to 0 means that NAL HRD parameters are not
	present.
	*/
	TUint iNalCpbCount;
    
	/**
	Valid only if iNalCpbCount is larger than zero. iNalCpbParams is a pointer to an object of 
	TCpbParams, which contains the parameters of an alternative CPB.
	*/
	TCpbParams* iNalCpbParams;
    
	/**
	Specifies the number of alternative coded picture buffer (CPB) specifications for VCL HRD 
	parameters in the bitstream. The value of iVclCpbCount is equal to cpb_cnt_minus1 + 1 in the
	VCL HRD parameters, if present. iVclCpbCount equal to 0 means that VCL HRD parameters are not
	present.
	*/
	TUint iVclCpbCount;
    
	/**
	Valid only if iVclCpbCount is larger than zero. iVclCpbParams is a pointer to an object of 
	TCpbParams, which contains the parameters of an alternative CPB.
	*/
	TCpbParams* iVclCpbParams;
    };





#endif
