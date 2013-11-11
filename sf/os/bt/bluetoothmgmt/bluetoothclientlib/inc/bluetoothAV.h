// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef BLUETOOTHAV_H
#define BLUETOOTHAV_H

#include <bt_sock.h>

class TSEID;

typedef TUint8 TTCID;
typedef TUint8 TTSID;
typedef TUint8 TAvdtpTransactionLabel;
typedef TUint8 TBluetoothMediaCodecType;

// to cover AVDTP, GAVDP, A2DP and VDP enums
// NOTE: 	Changing this from a TUint8 will cause a 'Break'.
//			See in-source documentation for:-
//			TInt SymbianBluetoothAV::ConvertToSymbianError::DoConvertError(TBluetoothAvDistributionError aAVError)
typedef TUint8 TBluetoothAvDistributionError;

/**
TSEID class represents an AVDTP SEID as described in the AVDTP v1.0 specification.
*/
NONSHARABLE_CLASS(TSEID)
	{
private:
static const TUint KIsLocalBit = 1<<31;
public:
	IMPORT_C TSEID();
	IMPORT_C TSEID(TUint aSEID, TBool aIsLocal);
	IMPORT_C explicit TSEID(TUint aValue);
	IMPORT_C TUint SEID() const;
	IMPORT_C TUint PacketValue() const;
	IMPORT_C TBool IsValid() const;
	IMPORT_C void Set(TUint aValue);
	IMPORT_C TBool operator==(TSEID aSEID) const;
	IMPORT_C TBool operator!=(TSEID aSEID) const;
	IMPORT_C static TSEID _Spare(TUint8 aUnused);
	IMPORT_C static TSEID FromPacketL(TUint8 aPacketValue, TBool aSemanticallyLocal);
	IMPORT_C void Reset();
	IMPORT_C TBool IsLocal() const;
	IMPORT_C TUint Value() const; 
	 
private:
	static const TUint  KInvalidSEID;
	TUint	iValue; // large size for interface compatibility
	};

const TInt KAVDTP = 0x0019;	// protocol number, as per assigned number
_LIT(KAVDTPProtocolName, "AVDTP");

/**
Error Code: -18000.  Indicates the start of the error range for AVDTP and the AV profiles 
that sit above it.

The error range this encompasses: -18000 to -18300 (inclusive).
*/
const TInt KErrAvdtpBaseError = -18000;

/**
Error Code: -18001.  Error raised when the AVDTP is started without 
having been bound to a lower layer protocol which provides the 
Service Access Points.
*/
const TInt KErrAvdtpNotBound = KErrAvdtpBaseError -1;

/**
Error Code: -18002.  Error raised when trying to bind the AVDTP
to a lower layer protocol when it is already bound to one.
*/
const TInt KErrAvdtpAlreadyBound = KErrAvdtpBaseError -2;

/**
Error Code: -18003.  Error raised when trying to add a packet
to a full packetpool.
*/
const TInt KErrAvdtpPacketPoolBalk = KErrAvdtpBaseError -3;

/**
Error Code: -18004.  
*/
const TInt KErrAvdtpClientBufferInUse = KErrAvdtpBaseError -4;

/**
Error Code: -18005.  The Acceptor did not respond to the AVDTP 
signalling message within the period of the protocol's timeout.
*/
const TInt KErrAvdtpRequestTimeout = KErrAvdtpBaseError -5;

/**
Error Code: -18006.
*/
const TInt KErrAvdtpRemoteReject = KErrAvdtpBaseError -6;

/**
Error Code: -18007.  Error raised when trying to reconfigure the application 
service parameters of an AV stream if either device is not in an OPEN state 
as defined in the GAVDP specification.  So the error is raised when not in the 
Ready state.
*/
const TInt KErrAvdtpInvalidStateForReconfigure = KErrAvdtpBaseError -7;

// errors from -18000 to -18045 are "locally-originated" errors

/**
Constant: -18045.  Indicates the start of the error range for AV signalling.
Errors from -18000 to -18044 (inclusive) are "locally-originated" errors that are not sent 
over the air and return from the AVDTP layer with an output parameter to the application 
rather than causing an error event.

Errors from -18045 to -18300 (inclusive) are "on-air" errors that are returned from the 
AVDTP or application layers on the peer device, resulting in error events.
*/
const TInt KErrAvdtpSignallingErrorBase = KErrAvdtpBaseError - 45;
// the error space is divided across the AVDTP, GAVDP, A2DP and VDP specs
// it is possible in future to have any of 255 errors on the air
// therefore from KErrAvdtpSignallingErrorBase to -18300 downwards are "air-originated errors"

// GAVDP errors start at KErrAvdtpSignallingErrorBase - 0x80

// concrete profile (A2DP, VDP) errors start at KErrAvdtpSignallingErrorBase - 0xC0

const TUint8 KMaxTUint6 = (1<<6) - 1;
const TUint8 KMaxTUint5 = (1<<5) - 1;

enum TAvdtpTransportSessionType
	{
	EMedia,
	EReporting,
	ERecovery,
	ESignalling,	// not a true session as per spec
	ENumStreamTypes // always last enumeration
	};

enum TBluetoothAVPanics
	{
	EBadValue,
	};
	
enum TAvdtpServiceCategory
	{
	EServiceCategoryNull				=0x00,
	EServiceCategoryMediaTransport 		=0x01,
	EServiceCategoryReporting			=0x02,
	EServiceCategoryRecovery			=0x03,
	EServiceCategoryContentProtection	=0x04,
	EServiceCategoryHeaderCompression	=0x05,
	EServiceCategoryMultiplexing		=0x06,
	EServiceCategoryMediaCodec			=0x07,
	ENumberOfServiceCategories			=0x08,	// the number of real categories in spec
	EAllServiceCategories				=0xff,
	};
	
enum TAvdtpOptions
	{
	EAvdtpMediaGetMaximumPacketSize,
	EAvdtpMediaGetMaximumReceivePacketSize,
	};

enum TAvdtpClientIoctls
	{
	ENotifyAvdtpMediaPacketDropped
	};

namespace SymbianBluetoothAV
{
/**
As specified by Bluetooth SIG
Depending on result of MediaType this can be used for casting
*/
enum TBluetoothMediaType
	{
	EAvdtpMediaTypeAudio = 0x00,
	EAvdtpMediaTypeVideo = 0x01,
	EAvdtpMediaTypeMultimedia = 0x02,
	};

/**
As specified by Bluetooth SIG
Depending on result of MediaType this can be used for casting
*/
enum TBluetoothAudioCodecType
	{
	EAudioCodecSBC = 0x00,
	EAudioCodecMPEG12Audio = 0x01,
	EAudioCodecMPEG24AAC = 0x02,
	EAudioCodecATRAC = 0x04,
	EAudioCodecNonA2DP = 0xFF,
	};

/**
As specified by Bluetooth SIG
Depending on result of MediaType this can be used for casting
*/
enum TBluetoothVideoCodecType
	{
	EVideoCodecH263Baseline = 0x01,
	EVideoCodecMPEG4VisualSimpleProfile = 0x02,
	EVideoCodecH263Profile3 = 0x03,
	EVideoCodecH263Profile8 = 0x04,
	EVideoCodecNonVDP = 0xFF,
	};

enum TAvdtpRecoveryType
	{
	EForbiddenRecovery =0x00, //for checking purposes
	ERFC2733Recovery = 0x01,
	};

/**
Error codes relating to the Audio/Video Distribution Transport Protocol signalling messages.  Error desciptions taken from the Bluetooth SIG specification.
These error codes are sent over the air from the Acceptor device (the device to whom the original signal was sent) 
to the Initiator device (from which the signal originated) in a signal response message when the Acceptor rejects 
the signalling command from the Initiator.  The rejection can come either from the Application itself or the AVDTP layer 
(when the message cannot be interpreted).
*/
enum TAvdtpSignallingErrorCode
	{
	/**
	Error Code: -18046.  The request packet header format error is not specified above ERROR_CODE.  Related Signalling Command: All messages.
	*/
	EAvdtpBadHeaderFormat			=0x01,
	
	/**
	Error Code: -18062.  The request packet length does not match the assumed length.  Related Signalling Command: All messages.
	*/
	EAvdtpBadLength					=0x11,

	/**
	Error Code: -18063.  The requested command indicates an invalid ACP Stream End Point Identifier (not addressable).  Related Signalling Command: All messages.
	*/	
	EAvdtpBadACPSEID				=0x12,
	
	/**
	Error Code: -18064.  The Stream End Point is in use.  Related Signalling Command: Set Configuration.
	*/	
	EAvdtpSEPInUse					=0x13,
	
	/**
	Error Code: -18065.  The Stream End Point is not in use.  Related Signalling Command: Reconfigure.
	*/	
	EAvdtpSepNotInUse				=0x14,
	
	/**
	Error Code: -18068.  The value of Service Category in the request packet is not defined in AVDTP.  Related Signalling Command: Set Configuration, Reconfigure.
	*/	
	EAvdtpBadServCategory			=0x17,
	
	/**
	Error Code: -18069.  The requested command has an incorrect payload format (Format errors not specified in this ERROR_CODE).  Related Signalling Command: All messages.
	*/	
	EAvdtpBadPayloadFormat			=0x18,
	
	/**
	Error Code: -18070.  The requested command is not supported by the device.  Related Signalling Command: All messages.
	*/	
	EAvdtpNotSupportedCommand		=0x19,
	
	/**
	Error Code: -18071.  The reconfigure command is an attempt to reconfigure transport service capabilities of the Stream End Point. Reconfigure is only permitted for application service capabilities.  Related Signalling Command: Reconfigure.
	*/	
	EAvdtpInvalidCapabilities		=0x1a,
	
	/**
	Error Code: -18079.  The requested Recovery Type is not defined in AVDTP.  Related Signalling Command: Set Configuration.
	*/	
	EAvdtpBadRecoveryType			=0x22,
	
	/**
	Error Code: -18080.  The format of Media Transport Capability is not correct.  Related Signalling Command: Set Configuration.
	*/	
	EAvdtpBadMediaTransportFormat	=0x23,
	
	/**
	Error Code: -18081.  Unused.  This is not in the AVDTP specification, but appears that it should be.
	*/	
	EAvdtpBadReportingFormat		=0x24, // Unused - not in spec, but appears it should be!
	
	/**
	Error Code: -18082.  The format of Recovery Service Capability is not correct.  Related Signalling Command: Set Configuration.
	*/	
	EAvdtpBadRecoveryFormat			=0x25,
	
	/**
	Error Code: -18083.  The format of Header Compression Service Capability is not correct.  Related Signalling Command: Set Configuration.
	*/	
	EAvdtpBadRohcFormat				=0x26,
	
	/**
	Error Code: -18084.  The format of Content Protection Service Capability is not correct.  Related Signalling Command: Set Configuration.
	*/	
	EAvdtpBadCpFormat				=0x27,
	
	/**
	Error Code: -18085.  The format of Multiplexing Service Capability is not correct.  Related Signalling Command: Set Configuration.
	*/	
	EAvdtpBadMultiplexingFormat		=0x28,
	
	/**
	Error Code: -18086.  Configuration not supported.  Related Signalling Command: Set Configuration.
	*/	
	EAvdtpUnsupportedConfiguration	=0x29,
	
	/**
	Error Code: -18094.  Indicates that the ACP state machine is in an invalid state in order to process the signal.  Related Signalling Command: All messages.
	*/	
	EAvdtpBadState					=0x31
	};

/**
Error codes relating to the Generic Audio/Video Distribution Profile signalling messages.  Error desciptions taken from the Bluetooth SIG specification.
These error codes are sent over the air from the Acceptor device (the device to whom the original signal was sent) 
to the Initiator device (from which the signal originated) in a signal response message when the Acceptor rejects 
the signalling command from the Initiator.  

The error range -18300 to -18237 (inclusive) is reserved for the profile sitting on top of the GAVDP.
*/	
enum TGavdpSignallingErrorCode
	{
	/**
	Error Code: -18173.  The service category stated is invalid.  Related Signalling Command: Set Configuration.
	*/
	EGavdpBadService			=0x80,
	
	/**
	Error Code: -18174.  Lack of resource new Stream Context.  Related Signalling Command: Set Configuration.
	*/
	EGavdpInsufficientResource	=0x81,
	};
	
	
/**
Error codes relating to the Advanced Audio Distribution Profile signalling messages.  Error desciptions taken from the Bluetooth SIG specification.
These error codes are sent over the air from the Acceptor device (the device to whom the original signal was sent) 
to the Initiator device (from which the signal originated) in a signal response message when the Acceptor rejects 
the signalling command from the Initiator.  

These error codes are used by the application if it receives commands containing relevant errors.

This profile sits on top of the GAVDP and so uses the error range -18300 to -18237 (inclusive).  This range is shared with 
other profiles that sit above GAVDP, such as VDP.
*/		
enum TA2dpSignallingErrorCode
	{
	/**
	Error Code: -18238.  Media Codec Type is not valid.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidCodec					=0xc1,
	
	/**
	Error Code: -18239.  Media Codec Type is not supported.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedCodec				=0xc2,
	
	/**
	Error Code: -18240.  Sampling Frequency is not valid or multiple values have been selected.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidSamplingFrequency		=0xc3,
	
	/**
	Error Code: -18241.  Sampling Frequency is not supported.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedSamplingFrequency	=0xc4,
	
	/**
	Error Code: -18242.  Channel Mode is not valid or multiple values have been selected.  Related Codec: SBC, MPEG-1,2 Audio, ATRAC family.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidChannelMode				=0xc5,
	
	/**
	Error Code: -18243.  Channel Mode is not supported.  Related Codec: SBC; MPEG-1,2 Audio; ATRAC family.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedChannelMode		=0xc6,
	
	/**
	Error Code: -18244.  None or multiple values have been selected for Number of Subbands.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure. 
	*/
	EA2dpInvalidSubbands				=0xc7,
	
	/**
	Error Code: -18245.  Number of Subbands is not supported.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedSubbands			=0xc8,
	
	/**
	Error Code: -18246.  None or multiple values have been selected for Allocation Method.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure.  
	*/
	EA2dpInvalidAllocationMethod		=0xc9,
	
	/**
	Error Code: -18247.  Allocation Method is not supported.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedAllocationMethod	=0xca,
	
	/**
	Error Code: -18248.  Minimum Bitpool Value is not valid.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidMinimumBitPoolValue		=0xcb,
	
	/**
	Error Code: -18249.  Minimum Bitpool Value is not supported.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedMinimumBitPoolValue=0xcc,
	
	/**
	Error Code: -18250.  Maximum Bitpool Value is not valid.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidMaximumBitPoolValue		=0xcd,
	
	/**
	Error Code: -18251.  Maximum Bitpool Value is not supported.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedMaximumBitPoolValue=0xce,
	
	/**
	Error Code: -18252.  None or multiple values have been selected for Layer.  Related Codec: MPEG-1,2 Audio.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidLayer					=0xcf,
	
	/**
	Error Code: -18253.  Layer is not supported.  Related Codec: MPEG-1,2 Audio.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedLayer				=0xd0,
	
	/**
	Error Code: -18254.  CRC is not supported.  Related Codec: MPEG-1,2 Audio.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedCRC				=0xd1,
	
	/**
	Error Code: -18255.  MPF-2 is not supported.  Related Codec: MPEG-1,2 Audio.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedMPF				=0xd2,
	
	/**
	Error Code: -18256.  VBR is not supported.  Related Codec: MPEG-1,2 Audio; MPEG-2,4 AAC; ATRAC family.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedVBR				=0xd3,
	
	/**
	Error Code: -18257.  None or multiple values have been selected for Bit Rate.  Related Codec: MPEG-1,2 Audio; ATRAC family.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidBitRate					=0xd4,
	
	/**
	Error Code: -18258.  Bit Rate is not supported.  Related Codec: MPEG-1,2 Audio; MPEG-2,4 AAC; ATRAC family.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedBitRate			=0xd5,
	
	/**
	Error Code: -18259.  Either 1) Object type is not valid (b3-b0) or 2) None or multiple values have been selected for Object Type.  Related Codec: MPEG-2,4 AAC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidObjectType				=0xd6,
	
	/**
	Error Code: -18260.  Object Type is not supported.  Related Codec: MPEG-2,4 AAC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedObjectType			=0xd7,
	
	/**
	Error Code: -18261.  None or multiple values have been selected for Channels.  Related Codec: MPEG-2,4 AAC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidChannels				=0xd8,
	
	/**
	Error Code: -18262.  Channels is not supported.  Related Codec: MPEG-2,4 AAC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedChannels			=0xd9,
	
	/**
	Error Code: -18263.  Version is not valid.  Related Codec: ATRAC family.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidVersion					=0xda,
	
	/**
	Error Code: -18264.  Version is not supported.  Related Codec: ATRAC family.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedVersion			=0xdb,
	
	/**
	Error Code: -18265.  Maximum SUL is not acceptable for the Decoder in the SNK.  Related Codec: ATRAC family.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpNotSupportedSUL				=0xdc,
	
	/**
	Error Code: -18266.  None or multiple values have been selected for Block Length.  Related Codec: SBC.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidBlockLength				=0xdd,
	
	/**
	Error Code: -18269.  The requested CP Type is not supported.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EA2dpInvalidCPType					=0xe0,
	
	/**
	Error Code: -18270.  The format of Content Protection Service Capability/Content Protection Scheme Dependent Data is not correct.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure, Security Control.
	*/
	EA2dpInvalidCPFormat				=0xe1,
	};
	
	
/**
Error codes relating to the Video Distribution Profile signalling messages.  Error desciptions taken from the Bluetooth SIG specification.
These error codes are sent over the air from the Acceptor device (the device to whom the original signal was sent) 
to the Initiator device (from which the signal originated) in a signal response message when the Acceptor rejects 
the signalling command from the Initiator.  

These error codes are used by the application if it receives commands containing relevant errors.

This profile sits on top of the GAVDP and so uses the error range -18300 to -18237 (inclusive).  This range is shared with 
other profiles that sit above GAVDP, such as A2DP.
*/
enum TVdpSignallingErrorCode
	{
	/**
	Error Code: -18238.  Media Codec Type is not valid.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EVdpInvalidCodecType			=0xc1,
	
	/**
	Error Code: -18239.  Media Codec Type is not supported.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EVdpNotSupportedCodecType		=0xc2,
	
	/**
	Error Code: -18240.  Level is not valid or multiple values have been selected.  Related Codec: H.263 baseline; MPEG-4 Visual Simple Profile; H.263 Profile 3; H.263 Profile 8.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EVdpInvalidLevel				=0xc3,
	
	/**
	Error Code: -18241.  Level is not supported.  Related Codec: H.263 baseline; MPEG-4 Visual Simple Profile; H.263 Profile 3; H.263 Profile 8.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EVdpNotSupportedLevel			=0xc4,
	
	/**
	Error Code: -18269.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EVdpInvalidCPType				=0xe0,
	
	/**
	Error Code: -18270.  Related Codec: All.  Related Signalling Command: Set Configuration, Reconfigure.
	*/
	EVdpInvalidCPFormat				=0xe1,
	};

/**
Class used for converting signalling error codes stored in the local namespace 
into standard system-wide error codes.  The values used locally are the same as the 
error codes set out 
in the various Bluetooth Profile specifications.  

Marked NONSHARABLE and therefore 
cannot be derived outside the dll.
*/
NONSHARABLE_CLASS(ConvertToSymbianError)
	{
public:
	IMPORT_C static TInt AvdtpError(TAvdtpSignallingErrorCode aAvdtpError);
	IMPORT_C static TInt GavdpError(TGavdpSignallingErrorCode aGavdpError);
	IMPORT_C static TInt A2dpError(TA2dpSignallingErrorCode aA2dpError);
	IMPORT_C static TInt VdpError(TVdpSignallingErrorCode aVdpError);

private:
	static TInt DoConvertError(TBluetoothAvDistributionError aError);
	};
} // end SymbianBluetoothAV namespace


namespace SymbianSBC
{
/*
Mask for configuring SBC channel modes
*/
enum TSBCChannelMode
	{
	EMono			= 0x8,
	EDualChannel	= 0x4,
	EStereo			= 0x2,
	EJointStereo	= 0x1
	};

/*
Mask for configuring SBC sampling frequencies
*/
enum TSBCSamplingFrequency
	{
	E16kHz		= 0x8,
	E32kHz		= 0x4,
	E44100Hz	= 0x2,
	E48kHz		= 0x1,
	};
	
/*
Mask for configuring SBC number of subbands
*/
enum TSBCSubbands
	{
	EFourSubbands  = 0x02,
	EEightSubbands = 0x01,
	};
	
/*
Mask for configuring SBC allocation methods
*/
enum TSBCAllocationMethod
	{
	ELoudness	= 0x01,
	ESNR		= 0x02,
	};
	
	
/*
Mask for configuring SBC block lengths
*/
enum TSBCBlockLength
	{
	EBlockLenFour 		= 0x8,
	EBlockLenEight 		= 0x4,
	EBlockLenTwelve		= 0x2,
	EBlockLenSixteen	= 0x1,
	};

}  // end of namespace SymbianSBC




typedef TUint8	TSBCChannelModeBitmask;
typedef TUint8	TSBCAllocationMethodBitmask;
typedef TUint8	TSBCSamplingFrequencyBitmask;
typedef TUint8	TSBCBlockLengthBitmask;
typedef TUint8	TSBCSubbandsBitmask;


/**
This constant has been deprecated in favour of RGavdp::MaxSecurityControlLength().

Note however that you should call this function each time you wish to use this value rather than storing 
copy because the actual value is subject to change.

@deprecated
*/
const TInt KMaxAvdtpSecurityControlInfo = 255; // our defn: spec doesnt give a max.

typedef TBuf8<KMaxAvdtpSecurityControlInfo> TAvdtpSecurityControlInfo;


/**
An AVDTP Socket Address is composed of a Bluetooth device address, 
an AVDTP session type (eg Media, Reporting or Recovery), and a SEID.
*/
NONSHARABLE_CLASS(TAvdtpSockAddr) : public TBTSockAddr
	{
public:
	IMPORT_C TAvdtpSockAddr();
	IMPORT_C explicit TAvdtpSockAddr(const TSockAddr& aSockAddr);
	IMPORT_C static TAvdtpSockAddr& Cast(const TSockAddr& aSockAddr);
	IMPORT_C void SetSession(TAvdtpTransportSessionType aSession);
	IMPORT_C TAvdtpTransportSessionType Session() const;
	IMPORT_C void SetSEID(TSEID aSEID);
	IMPORT_C TSEID SEID() const;
	};

/**
Used for casting the descriptor passed in the MGavdpUser::GAVDP_Error callback
Allows the client (knowing the previously sent GAVDP primitive) to decode extended errors
*/	
struct TAvdtpConfigReject
	{
	TInt iError;
	TAvdtpServiceCategory iFailedCategory;
	};
	
typedef TPckgBuf<TAvdtpConfigReject> TAvdtpConfigRejectPckgBuf;

typedef TUint TAvdtpServiceCatBitMask;

static const TInt KServiceCatLOSCRecovery 			= 3;
static const TInt KServiceCatLOSCHeaderCompression 	= 1;
static const TInt KServiceCatLOSCMediaCodecMinimum	= 2;
static const TInt KServiceCatLOSCMediaCodecMaximum	= KMaxTUint8;
static const TInt KServiceCatLOSCSBCCodec			= 4+KServiceCatLOSCMediaCodecMinimum;


NONSHARABLE_CLASS(TAvdtpSEPInfo)
	{
public:
	IMPORT_C TAvdtpSEPInfo();

	IMPORT_C TSEID SEID() const;
	IMPORT_C TBool InUse() const;
	IMPORT_C SymbianBluetoothAV::TBluetoothMediaType MediaType() const;
	IMPORT_C TBool IsSink() const;

	IMPORT_C void SetSEID(TSEID aSEID);
	IMPORT_C void SetInUse(TBool aIsInUse);
	IMPORT_C void SetMediaType(SymbianBluetoothAV::TBluetoothMediaType aMediaType);
	IMPORT_C void SetIsSink(TBool aIsSink);

private:
	TSEID				iSEID;
	TBool				iInUse;
	SymbianBluetoothAV::TBluetoothMediaType	iMediaType;
	TBool				iIsSink;
	};
		

/**
This is a base class for AVDTP Service Capabilities as described in the Bluetooth AVDTP specification.
This can be derived externally, but no support is provided for Capabilities not described in v1.0 of 
the AVDTP specification.
@see RGavdp
*/	
class TAvdtpServiceCapability
	{
public:
	IMPORT_C TAvdtpServiceCategory Category() const; // not inline to assist BC
	
	/**AsProtocol() is for Symbian use only
	This method is not to be used otherwise.
	@internalTechnology
	*/
	virtual TInt AsProtocol(RBuf8& aBuffer) const =0;	// write into buffer as per protocol
	
	IMPORT_C static TAvdtpServiceCapability* AllocFromPDUL(TAvdtpServiceCategory aCat, const TDesC8& aDes);
	IMPORT_C static TAvdtpServiceCapability* AllocFromPckgL(const TDesC8& aDes);
	
	/**Parse() is for Symbian use only
	This method is not to be used otherwise.
	@internalTechnology
	*/
	IMPORT_C virtual void Parse(const TDesC8& aPtr);

protected:
	IMPORT_C TAvdtpServiceCapability(TAvdtpServiceCategory aCat, TInt aLOSC=KMaxTInt);
	TInt AddHeader(RBuf8& aBuffer) const;

protected:
	mutable TInt iLOSC;	// Length of Service Capability

private:
	TAvdtpServiceCategory	iCategory;
	};
		
/**
Represents the Media Transport Capability of AVDTP.
*/
NONSHARABLE_CLASS(TAvdtpMediaTransportCapabilities) : public TAvdtpServiceCapability
	{
public:
	IMPORT_C TAvdtpMediaTransportCapabilities();
	IMPORT_C virtual TInt AsProtocol(RBuf8& aBuffer) const;
	};

/**
Represents the Reporting Capability of AVDTP.
*/
NONSHARABLE_CLASS(TAvdtpReportingCapabilities) : public TAvdtpServiceCapability
	{
public:
	IMPORT_C TAvdtpReportingCapabilities();
	IMPORT_C virtual TInt AsProtocol(RBuf8& aBuffer) const;
	};


typedef TUint8 TAvdtpRecoveryWindowSize;

/**
Represents the Recovery Capability of AVDTP.
*/
NONSHARABLE_CLASS(TAvdtpRecoveryCapabilities) : public TAvdtpServiceCapability
	{
public:
	IMPORT_C TAvdtpRecoveryCapabilities();
	IMPORT_C virtual TInt AsProtocol(RBuf8& aBuffer) const;

	IMPORT_C SymbianBluetoothAV::TAvdtpRecoveryType RecoveryType() const;
	IMPORT_C void SetRecoveryType(SymbianBluetoothAV::TAvdtpRecoveryType aType);
	
	IMPORT_C TAvdtpRecoveryWindowSize MaxWindowSize() const;
	//This runs client-side - panic bad values, no return error needed
	IMPORT_C void SetMaxWindowSize(TAvdtpRecoveryWindowSize aSize);
	
	IMPORT_C TAvdtpRecoveryWindowSize MinWindowSize() const;
	//This runs client-side - panic bad values
	IMPORT_C void SetMinWindowSize(TAvdtpRecoveryWindowSize aSize);

	virtual void Parse(const TDesC8& aPtr);

private:
	SymbianBluetoothAV::TAvdtpRecoveryType iRecoveryType;
	TAvdtpRecoveryWindowSize		iMaximumRecoveryWindowSize;
	TAvdtpRecoveryWindowSize		iMinimumRecoveryWindowSize;
	};
	
/**
Represents the Media Codec Capability of AVDTP.
*/
NONSHARABLE_CLASS(TAvdtpMediaCodecCapabilities) : public TAvdtpServiceCapability
	{
public:
	IMPORT_C SymbianBluetoothAV::TBluetoothMediaType MediaType() const;
	IMPORT_C TBluetoothMediaCodecType MediaCodecType() const;
	//factory for derived class
	static TAvdtpMediaCodecCapabilities* AllocCodecFromPDUL(const TDesC8& aBuf);
	IMPORT_C virtual void Parse(const TDesC8& aPtr);

protected:
	//remain abstract - derivers must specify their LOSC
	TAvdtpMediaCodecCapabilities(SymbianBluetoothAV::TBluetoothMediaType aMediaType,
									TBluetoothMediaCodecType aCodecType, TInt aLOSC);

protected:
	SymbianBluetoothAV::TBluetoothMediaType iMediaType;
	TBluetoothMediaCodecType iMediaCodecType;
	};
	
	
/**
Represents the Content Protection Capability of AVDTP.
So this capability at present doesn't parse to anything interesting, it just has a 
setter for CP type and trailing data
*/
NONSHARABLE_CLASS(TAvdtpContentProtectionCapabilities) : public TAvdtpServiceCapability
	{
public:
	IMPORT_C TAvdtpContentProtectionCapabilities();
	IMPORT_C TUint16 ContentProtectionType() const;
	IMPORT_C void SetContentProtectionType(TUint16 aCPType);
	IMPORT_C const TDesC8& ContentProtectionData() const;
	IMPORT_C void SetContentProtectionData(const TDesC8& aData);
	IMPORT_C virtual TInt AsProtocol(RBuf8& aBuffer) const;
	IMPORT_C void Parse(const TDesC8& aPtr);
	
private:
	TUint16	iCPType;
	TBuf8<KMaxAvdtpSecurityControlInfo>	iCPData;
	const TInt KMinContentProtectionLOSC;
	};
	
/**
Represents the Header Compression Capability of AVDTP.
*/
NONSHARABLE_CLASS(TAvdtpHeaderCompressionCapabilities) : public TAvdtpServiceCapability
	{
public:
	IMPORT_C TAvdtpHeaderCompressionCapabilities();
	IMPORT_C virtual TInt AsProtocol(RBuf8& aBuffer) const;
	
	IMPORT_C TBool Media() const;
	IMPORT_C TBool Recovery() const;
	IMPORT_C TBool BackChannel() const;
	
	IMPORT_C void SetCapabilities(TBool aMedia,TBool aRecovery,TBool aBackChannel);

	virtual void Parse(const TDesC8& aPtr);	
private:
	TBool	iMedia;
	TBool	iRecovery;
	TBool	iBackChannel;
	};
	
//Not muxing - stack looks after it

/**
Class to configure GAVDP SEPs supporting the SBC codec
@see RGavdp
*/
NONSHARABLE_CLASS(TSBCCodecCapabilities) : public TAvdtpMediaCodecCapabilities
	{
public:
	IMPORT_C TSBCCodecCapabilities();
	IMPORT_C virtual TInt AsProtocol(RBuf8& aBuffer) const;
	
	IMPORT_C TSBCSamplingFrequencyBitmask SamplingFrequencies() const;
	IMPORT_C void SetSamplingFrequencies(TSBCSamplingFrequencyBitmask aSampFreq);
	
	IMPORT_C TSBCChannelModeBitmask ChannelModes() const;
	IMPORT_C void SetChannelModes(TSBCChannelModeBitmask aChModes);
	
	IMPORT_C TSBCBlockLengthBitmask BlockLengths() const;
	IMPORT_C void SetBlockLengths(TSBCBlockLengthBitmask aBlkLens);
	
	IMPORT_C TSBCSubbandsBitmask Subbands() const;
	IMPORT_C void SetSubbands(TSBCSubbandsBitmask aSubbands);
	
	IMPORT_C TSBCAllocationMethodBitmask AllocationMethods() const;
	IMPORT_C void SetAllocationMethods(TSBCAllocationMethodBitmask aAllocMeths);
	
	IMPORT_C TInt MinBitpoolValue() const;
	IMPORT_C void SetMinBitpoolValue(TInt aMinBP);
	
	IMPORT_C TInt MaxBitpoolValue() const;
	IMPORT_C void SetMaxBitpoolValue(TInt aMaxBP);
	
	IMPORT_C virtual void Parse(const TDesC8& aPtr);
	
private:
	TSBCSamplingFrequencyBitmask	iSamplingFrequencies;
	TSBCChannelModeBitmask 			iChannelModes;
	TSBCBlockLengthBitmask			iBlockLengths;
	TSBCSubbandsBitmask				iSubbands;
	TSBCAllocationMethodBitmask 	iAllocationMethod;
	TInt 							iMinimumBitpoolValue;
	TInt 							iMaximumBitpoolValue;
	};
	
/**
Allows any other codec - but without handy getters and setters
SBC will dominate heavily in SymbianOS A2DP, but this provided for further extension
without need for recompilation (which would be the way to support codecs in a more friendly manner)
@see RGavdp
*/	
class TNonSBCCodecCapabilities : public TAvdtpMediaCodecCapabilities
	{
public:
	IMPORT_C TNonSBCCodecCapabilities(SymbianBluetoothAV::TBluetoothMediaType aMediaType,
										TBluetoothMediaCodecType aCodecType);
	IMPORT_C virtual TInt AsProtocol(RBuf8& aBuffer) const;
	IMPORT_C const TDesC8& CodecData() const;
	IMPORT_C void SetCodecData(const TDesC8& aData);

	IMPORT_C virtual void Parse(const TDesC8& aPtr);	
private:
	TBuf8<KServiceCatLOSCMediaCodecMaximum>	iCodecData;
	};
/*
Allows for specification of >1 category
*/
NONSHARABLE_CLASS(TAvdtpServiceCategories)
	{
public:
	IMPORT_C TAvdtpServiceCategories();
	IMPORT_C TAvdtpServiceCatBitMask operator()() const;
	IMPORT_C void SetCapability(TAvdtpServiceCategory aCategory);
	IMPORT_C void ClearCapability(TAvdtpServiceCategory aCategory);
	IMPORT_C TBool CapabilityPresent(TAvdtpServiceCategory aCategory) const;
private:
	TAvdtpServiceCatBitMask iCategories;
	};

#endif //BLUETOOTHAV
