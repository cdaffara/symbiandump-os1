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
// Constants and structs specific to H.263.  See the DevVideo specs for more details.
// 
//

#ifndef __DEVVIDEO_H263_H__
#define __DEVVIDEO_H263_H__

#include <e32base.h>


/**
Specifies the H263 picture type.
@publishedAll
@released
*/
enum TH263PictureType
    {
	/**INTRA picture*/
    EH263PictureTypeI   = 0x01,
	/**INTER picture*/
    EH263PictureTypeP   = 0x02,
	/**B picture according to H.263 Annex O*/
    EH263PictureTypeB   = 0x04,
	/**EI picture according to H.263 Annex O*/
    EH263PictureTypeEI  = 0x08,
	/**EP picture according to H.263 Annex O*/
    EH263PictureTypeEP  = 0x10
    };

/**
These enumerations can be used in codec capability indication and/or bitstream property indication.
When used as part of an encoder capability indication, a signalled value indicates the capability
of an encoder to produce such supplemental information to the bitstream and the additional meanings
as specified below. When used as part of a decoder capability indication, a signalled value 
indicates the capability of a decoder to react to such supplemental information in a way as 
specified below. If a decoder does not have such capability, it shall be able to parse and discard
the supplemental information. When used as part of a bitstream property indication, a non-signalled
value indicates the absence of such information in the bitstream and a signalled value indicates 
the possibility of having such information in the bitstream.
@publishedAll
@released
*/
enum TH263SupplementalInformation
    {
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 1
	*/
    EH263SEIDoNothing			 					= 0x00000001,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to freeze displayed picture according to subclause L.4 of H.263capability to release frozen displayed picture as a response to bit 5 of PTYPE
	bitstream property:	existence of FTYPE equal to 2
	*/
    EH263SEIFullPictureFreeze            			= 0x00000002,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to freeze displayed picture partially (subclause L.5 of H.263)
	bitstream property:	existence of FTYPE equal to 3
	*/
    EH263SEIPartialPictureFreeze         			= 0x00000004,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to freeze and resize displayed picture partially (H.263 L.6)
	bitstream property:	existence of FTYPE equal to 4
	*/
    EH263SEIResizingPartialPictureFreeze 			= 0x00000008,

	/**
	encoder capability:	no meaning
	decoder capability:	capability to release partially frozen picture (subclause L.7 of H.263)
	bitstream property:	existence of FTYPE equal to 5
	*/
    EH263SEIPartialPictureFreezeRelease  			= 0x00000010,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 6
	*/
    EH263SEIFullPictureSnapshotTag       			= 0x00000020,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 7
	*/
    EH263SEIPartialPictureSnapshotTag    			= 0x00000040,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 8
	*/
    EH263SEIVideoTimeSegmentStartTag     			= 0x00000080,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 9
	*/
    EH263SEIVideoTimeSegmentEndTag       			= 0x00000100,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 10
	*/
    EH263SEIProgressiveRefinementStartTag   		= 0x00000200,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 11
	*/
    EH263SEIProgressiveRefinementEndTag     		= 0x00000400,
	
	/**
	encoder capability:	no meaning
	decoder capability:	operation according to sublause L.14 of H.263
	bitstream property:	existence of FTYPE equal to 12
	*/
    EH263SEIChromaKeying                 			= 0x00000800,
	
	/**
	encoder capability:	use of the fixed-point IDCT algorithm specified in Annex W of H.263
	decoder capability:	use of the fixed-point IDCT algorithm specified in Annex W of H.263
	bitstream property:	existence of FTYPE equal to 13
	*/
    EH263SEIFixedPointIDCT               			= 0x00001000,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 0
	*/
    EH263SEIArbitraryBinaryData          			= 0x00002000,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 1
	*/
    EH263SEIArbitraryText                   		= 0x00004000,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 2
	*/
    EH263SEICopyrightText                   		= 0x00008000,
	
	/**
	encoder capability:	no meaning
	decoder capability:	capability of display caption text as specified in subclause W.6.3.5 of H.263
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 3
	*/
    EH263SEICaptionText                     		= 0x00010000,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 4
	*/
    EH263SEIVideoDescriptionText            		= 0x00020000,
	
	/**
	encoder capability:	no meaning
	decoder capability:	no meaning
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 5
	*/
    EH263SEIUniformResourceIdentifierText   		= 0x00040000,
	
	/**
	encoder capability:	capability to repeat the current picture header (subclause W.6.3.7 of H.263)
	decoder capability:	capability to recover a corrupted picture header with a repeated one
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 6
	*/
    EH263SEICurrentPictureHeaderRepetition  		= 0x00080000,
	
	/**
	encoder capability:	capability to repeat the previous picture header (subclause W.6.3.8 of H.263)
	decoder capability:	capability to recover a corrupted or lost picture header with a repeated one
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 7
	*/
    EH263SEIPreviousPictureHeaderRepetition 		= 0x00100000,
	
	/**
	encoder capability:	capability to repeat the next picture header (W.6.3.9 of H.263)
	decoder capability:	capability to recover a corrupted or lost picture header with a repeated one
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 8
	*/
    EH263SEINextPictureHeaderRepetitionReliableTR  	= 0x00200000,
	
	/**
	encoder capability:	capability to repeat the next picture header (W.6.3.10 of H.263)
	decoder capability:	capability to recover a corrupted or lost picture header with a repeated one
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 9
	*/
    EH263SEINextPictureHeaderRepetitionUnreliableTR	= 0x00400000,
	
	/**
	encoder capability:	capability to encode top fields of interlaced pictures 
	decoder capability:	capability to decode and display top fields of interlaced pictures (W.6.3.11of H.263)
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 10
	*/
    EH263SEITopInterlacedFieldIndication		   	= 0x00800000,
	
	/**
	encoder capability:	capability to encode bottom fields of interlaced pictures 
	decoder capability:	capability to decode and display bottom fields of interlaced pictures (W.6.3.11of H.263)
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 10
	*/
    EH263SEIBottomInterlacedFieldIndication       	= 0x01000000,
	
	/**
	encoder capability:	capability to transmit picture numbers (subclause W.6.3.12 of H.263)
	decoder capability:	capability to indicate reference picture losses from missing picture numbers
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 12
	*/
    EH263SEIPictureNumber                   		= 0x02000000,
	
	/**
	encoder capability:	capability to generate spare reference picture information (W.6.3.13 of H.263)
	decoder capability:	capability to use a spare reference picture in absence of real reference picture
	bitstream property:	existence of FTYPE equal to 14 and MTYPE equal to 13
	*/
    EH263SEISpareReferencePictures          		= 0x04000000
    };


/**
H.263 picture header.
@publishedAll
@released
*/
class TH263PictureHeader
    {
public:
	/**
	Indicates the temporal reference. iTemporalReference is equal to the value of TR or the value
	of (ETR << 8) + TR if ETR is present in the picture header.
	*/
    TUint iTemporalReference;

	/**
	iLongTermPicture equal to ETrue indicates that the picture has been marked as a long-term
	picture according to the memory management control operation (MMCO) commands as specified 
	in Annex U of H.263. Otherwise the picture is not a long-term picture.
	*/
    TBool iLongTermPicture;

	/**
	iSplitScreen is equal to the value of bit 3 of PTYPE.
	*/
    TBool iSplitScreen;

	/**
	iDocumentCamera is equal to the value of bit 4 of PTYPE.
	*/
    TBool iDocumentCamera;

	/**
	iFreezePictureRelease is equal to the value of bit 5 of PTYPE.
	*/
    TBool iFreezePictureRelease;

	/**
	Indicates the picture coding type. iPictureType is set according to the picture header as one 
	of the values defined in TH263PictureType.
	*/
    TH263PictureType iPictureType;

	/**
	Indicates the initial value of the quantization parameter. iPQuant is equal to the value of 
	the PQUANT syntax element.
	*/
    TUint iPQuant;

	/**
	Indicates the presence of the extended PTYPE field in the picture layer. The extended PTYPE 
	field is specified in ITU-T Recommendation H.263 version 2 and later. iPlusPType is equal to 1 
	if bits 6 - 8 of PTYPE are equal to '111'. Otherwise, iPlusPType is equal to 0.
	*/
    TBool iPlusPType;

	/**
	Indicates the presence of the full extended PTYPE field in the picture layer. If iPlusPType is 
	EFalse, the value of of iUpdateFullExtendedPType is not valid. Otherwise, 
	iUpdateFullExtendedPType is equal to the value of the UFEP syntax element.
	*/
    TBool iUpdateFullExtendedPType;

	/**
	Pixel aspect ratio numerator and denominator respectively. The pixel aspect ratio is defined as
	iAspectRatioNum/iAspectRatioDenom, where the values are positive integers and relatively prime.
	If iPlusPType is equal to 0, or iPlusPType is ETrue and iUpdateFullExtendedPType is ETrue and 
	bits 1 - 3 of the OPPTYPE syntax element are not equal to '110', iAspectRatioNum shall be equal
	to 12 and iAspectRatioDenom shall be equal to 11 (as specified in subclause 4.1 of ITU-T 
	Recommendation H.263). If iPlusPType is ETrue and iUpdateFullExtendedPType is ETrue and bits
	1 - 3 of the OPPTYPE syntax element are equal to '110', the values of iAspectRatioNum and 
	iAspectRatioDenom are set as specified in subclauses 5.1.5 and 5.1.6 of ITU-T Recommendation
	H.263. Otherwise (iPlusPType is ETrue and iUpdateFullExtendedPType is EFalse), the values of
	iAspectRatioNum and iAspectRatioDenom are not valid.
	*/
    TUint iAspectRatioNum;

	/**
	Pixel aspect ratio numerator and denominator respectively. The pixel aspect ratio is defined as
	iAspectRatioNum/iAspectRatioDenom, where the values are positive integers and relatively prime.
	If iPlusPType is equal to 0, or iPlusPType is ETrue and iUpdateFullExtendedPType is ETrue and 
	bits 1 - 3 of the OPPTYPE syntax element are not equal to '110', iAspectRatioNum shall be equal
	to 12 and iAspectRatioDenom shall be equal to 11 (as specified in subclause 4.1 of ITU-T 
	Recommendation H.263). If iPlusPType is ETrue and iUpdateFullExtendedPType is ETrue and bits
	1 - 3 of the OPPTYPE syntax element are equal to '110', the values of iAspectRatioNum and 
	iAspectRatioDenom are set as specified in subclauses 5.1.5 and 5.1.6 of ITU-T Recommendation
	H.263. Otherwise (iPlusPType is ETrue and iUpdateFullExtendedPType is EFalse), the values of
	iAspectRatioNum and iAspectRatioDenom are not valid.
	*/
    TUint iAspectRatioDenom;

	/**
	Picture clock frequency numerator and denominator respectively. The picture clock frequency 
	for temporal reference is defined as iPictureClockFrequencyRate / iPictureClockFrequencyScale
	in floating point arithmetic and where the values are positive integers and relatively prime. 
	If iPlusPType is equal to 0, or iPlusPType is equal to 1 and iUpdateFullExtendedPType is equal 
	to 1 and bit 4 of the OPPTYPE syntax element is equal to 0, iPictureClockFrequencyRate shall be 
	equal to 30000 and iPictureClockFrequencyScale shall be equal to 1001 (as specified in subclause
	4.1 of ITU-T Recommendation H.263). If iPlusPType is equal to 1 and iUpdateFullExtendedPType is
	equal to 1 and bit 4 of the OPPTYPE syntax element is equal to 1, the values of 
	iPictureClockFrequencyRate and iPictureClockFrequencyScale are set as specified in subclause
	5.1.7 of ITU-T Recommendation H.263. Otherwise (iPlusPType is equal to 1 and 
	iUpdateFullExtendedPType is equal to 0), the values of iPictureClockFrequencyRate and
	iPictureClockFrequencyScale are not valid.
	*/
    TUint iPictureClockFrequencyRate;

	/**
	Picture clock frequency numerator and denominator respectively. The picture clock frequency 
	for temporal reference is defined as iPictureClockFrequencyRate / iPictureClockFrequencyScale
	in floating point arithmetic and where the values are positive integers and relatively prime. 
	If iPlusPType is equal to 0, or iPlusPType is equal to 1 and iUpdateFullExtendedPType is equal 
	to 1 and bit 4 of the OPPTYPE syntax element is equal to 0, iPictureClockFrequencyRate shall be 
	equal to 30000 and iPictureClockFrequencyScale shall be equal to 1001 (as specified in subclause
	4.1 of ITU-T Recommendation H.263). If iPlusPType is equal to 1 and iUpdateFullExtendedPType is
	equal to 1 and bit 4 of the OPPTYPE syntax element is equal to 1, the values of 
	iPictureClockFrequencyRate and iPictureClockFrequencyScale are set as specified in subclause
	5.1.7 of ITU-T Recommendation H.263. Otherwise (iPlusPType is equal to 1 and 
	iUpdateFullExtendedPType is equal to 0), the values of iPictureClockFrequencyRate and
	iPictureClockFrequencyScale are not valid.
	*/
    TUint iPictureClockFrequencyScale;
    };

/**
This class is used to signal decoder or encoder capabilities or bitstream properties.
@publishedAll
@released
*/
class TH263VideoCapability
    {
public:
	/**
	When part of a codec capability indication, iProfileLevel[ProfileNumber] indicates the 
	maximum processing level that a decoder or an encoder supports for a particular profile 
	indicated by ProfileNumber. A negative value indicates that the profile is not supported.
	When part of a stream property indication, iProfileLevel[ProfileNumber] indicates the maximum
	processing level that is present in the stream for a particular profile indicated by 
	ProfileNumber. A negative value indicates that the stream does not contain data coded according 
	to the indicated profile.
	*/
    TInt iProfileLevel[9];

	/**
	When equal to ETrue, indicating that it is allowed to use PLUSPTYPE (specified in the 1998 
	version of H.263) to indicate custom picture sizes or clock frequencies. When equal to EFalse,
	PLUSPTYPE shall not be used.
	*/
    TBool iPLUSPTYPEAllowed;

	/**
	iSupplementalInformationCap indicates the supported supplemental enhancement functions. The
	value is a binary OR of values from TH263SupplementalInformation.
	*/
    TUint32 iSupplementalInformationCap;
    };


/**
This class is used to set the encoder operation mode.
@publishedAll
@released
*/
class TH263VideoMode
    {
public:
	/**
	Specifies the picture types allowed in the bitstream. The value is a binary OR of values from
	TH263PictureType. Signaled picture types that are not included in the prevailing coding profile
	are ignored.
	*/
    TUint32 iAllowedPictureTypes;

	/**
	iForceRoundingTypeToZero equal to EFalse specifies that the value of the RTYPE bit (bit 6 of 
	MPPTYPE) is not constrained. It is recommended to change the value of the RTYPE bit for each 
	reference picture in error-free communication (subclause 5.1.4.3 of ITU-T H.263 Recommendation).
	iForceRoundingTypeToZero equal to ETrue specifies that the value of the RTYPE bit shall be equal 
	to 0 or that the RTYPE field shall not be present in the picture layer. It is recommended to set 
	iForceRoundingTypeToZero equal to ETrue (and consequently RTYPE equal to 0) in error-prone 
	communication as recommended in section 4.2.6 of H.263 Appendix III [R6] and [R7].
	*/
    TBool iForceRoundingTypeToZero;

	/**
	Specifies the frequency of picture header repetition as specified in this paragraph. If 
	iPictureHeaderRepetition is equal to 0, picture headers are not repeated. Otherwise, a picture 
	header is repeated, if the value of the GFID syntax element is not equal to the value of the 
	GFID syntax element of the previous picture in bitstream order. If EDuRtpPayload data unit 
	encapsulation is in use, a picture header is repeated the number of times indicated by the value 
	of iPictureHeaderRepetition in those RTP payloads of the picture where the P bit according to 
	RFC 2429 [R8] is set equal to 1 and which do not contain the original picture header. If 
	EDuElementaryStream data unit encapsulation is in use, the picture header is repeated once in 
	the supplemental enhancement information of the next picture as specified in subclause W.6.3.8
	of ITU-T Recommendation H.263 and section 4.2.6 of H.263 Appendix III [R6].
	*/
    TUint iPictureHeaderRepetition;

	/**
	Specifies the interval of non-empty GOB headers in units of GOBs. The value of 
	iGOBHeaderInterval is valid if no slice structured coding mode (Annex K or Annex V) is in use. 
	If iGOBHeaderInterval is equal to 0 and an unlimited segment size is specified with the 
	SetSegmentTargetSize method, the encoder should not generate GOB headers. If iGOBHeaderInterval 
	is greater than 0 and an unlimited segment size is specified with the SetSegmentTargetSize 
	method, the encoder should generate a non-empty GOB header for every iGOBHeaderInterval-th GOB.
	If SetSegmentTargetSize is used to set a limited segment size, the encoder should generate 
	segments (i.e., insert non-empty GOB headers) so that the length of the segment is no larger
	than the target size in bytes and no larger than the GOB header frequency specified with the 
	value of iGOBHeaderInterval.
	*/
    TUint iGOBHeaderInterval;
    };

/**
H.263 HRD parameters.
@publishedAll
@released
*/
class TH263HrdParams
    {
public:
	/**
	When iAnnexXConstraints is equal to EFalse, iBPPmaxKb, iB, and iRmax indicate the values of HRD 
	parameters. When iAnnexXConstraints is equal to ETrue, the values of iBPPmaxKb, iB, and iRmax in 
	this class are ignored but rather their values are specified in Annex X of ITU-T Recommendation
	H.263. 
	*/
    TBool iAnnexXConstraints;

	/**
	Signals the BPPmaxKb parameter of H.263. As specified in ITU-T Recommendation H.263. The number 
	of bits created by coding any single picture shall not exceed a maximum value specified by the 
	parameter BPPmaxKb that is measured in units of 1024 bits. The minimum value of BPPmaxKb is 
	specified in Table 1 of ITU-T Recommendation H.263.
	*/
    TUint iBPPmaxKb;

	/**
	Signals the B parameter of H.263 Annex B. As specified in Annex B of ITU-T Recommendation H.263. 
	The value of B shall be equal to or greater than 4 * Rmax / PCF, where PCF is the picture clock 
	frequency specified in subclause 5.1.7 of ITU-T Recommendation H.263. The HRD receiving buffer 
	size is equal to (B + BPPmaxKb * 1024 bits).
	*/
    TUint iB;

	/**
	Signals the maximum video bit rate in bits per second, i.e., the Rmax parameter of H.263 Annex B. 
	*/
    TUint iRmax;
    };

/**
H.263 redundant picture header.
Points to a buffer to contain the redundant picture header in the orignal bitstream order. The
buffer may be used by the MSL client in playback to pass the decapsulated redundant picture header 
from an RTP payload formatted according to RFC 2429, or used by the MSL client in recording to pass
the picture header to the RTP encapsulator.
@publishedAll
@released
*/
typedef TPtr8* TH263RedundantPictureHeader;

#endif
