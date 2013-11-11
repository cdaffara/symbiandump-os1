/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "T_BTUtil.h"

//	EPOC includes
#include <bt_sock.h>

/*@{*/
// TL2CapConfig::TChannelPriority
_LIT(KEHigh,								"EHigh");
_LIT(KEMedium,								"EMedium");
_LIT(KELow,									"ELow");
// TL2CapConfig::TChannelReliability
_LIT(KEReliableChannel,						"EReliableChannel");
_LIT(KEUnreliableChannel,					"EUnreliableChannel");
// TBps
_LIT(KEBps50,								"EBps50");
_LIT(KEBps75,								"EBps75");
_LIT(KEBps110,								"EBps110");
_LIT(KEBps134,								"EBps134");
_LIT(KEBps150,								"EBps150");
_LIT(KEBps300,								"EBps300");
_LIT(KEBps600,								"EBps600");
_LIT(KEBps1200,								"EBps1200");
_LIT(KEBps1800,								"EBps1800");
_LIT(KEBps2000,								"EBps2000");
_LIT(KEBps2400,								"EBps2400");
_LIT(KEBps3600,								"EBps3600");
_LIT(KEBps4800,								"EBps4800");
_LIT(KEBps7200,								"EBps7200");
_LIT(KEBps9600,								"EBps9600");
_LIT(KEBps19200,							"EBps19200");
_LIT(KEBps38400,							"EBps38400");
_LIT(KEBps57600,							"EBps57600");
_LIT(KEBps115200,							"EBps115200");
_LIT(KEBps230400,							"EBps230400");
_LIT(KEBps460800,							"EBps460800");
_LIT(KEBps576000,							"EBps576000");
_LIT(KEBps1152000,							"EBps1152000");
_LIT(KEBps4000000,							"EBps4000000");
_LIT(KEBps921600,							"EBps921600");
_LIT(KEBpsAutobaud,							"EBpsAutobaud");
_LIT(KEBpsSpecial,							"EBpsSpecial");
// TDataBits
_LIT(KEData5,								"EData5");
_LIT(KEData6,								"EData6");
_LIT(KEData7,								"EData7");
_LIT(KEData8,								"EData8");
// TParity
_LIT(KEParityNone,							"EParityNone");
_LIT(KEParityEven,							"EParityEven");
_LIT(KEParityOdd,							"EParityOdd");
_LIT(KEParityMark,							"EParityMark");
_LIT(KEParitySpace,							"EParitySpace");
// TStopBits
_LIT(KEStop1,								"EStop1");
_LIT(KEStop2,								"EStop2");
// TRPNFlowCtrlMask
_LIT(KEFCXOnOffInput,						"EFCXOnOffInput");
_LIT(KEFCXOnOffOutput,						"EFCXOnOffOutput");
_LIT(KEFCRTRInput,							"EFCRTRInput");
_LIT(KEFCRTROutput,							"EFCRTROutput");
_LIT(KEFCRTCInput,							"EFCRTCInput");
_LIT(KEFCRTCOutput,							"EFCRTCOutput");
//TBTSyncPackets
_LIT(KESyncPacketsHV1,						"ESyncPacketsHV1");
_LIT(KESyncPacketsHV2,						"ESyncPacketsHV2");
_LIT(KESyncPacketsHV3,						"ESyncPacketsHV3");
_LIT(KESyncPacketsEV3,						"ESyncPacketsEV3");
_LIT(KESyncPacketsEV4,						"ESyncPacketsEV4");
_LIT(KESyncPacketsEV5,						"ESyncPacketsEV5");
_LIT(KESyncAnySCOPacket,					"ESyncAnySCOPacket");
_LIT(KESyncAnyESCOPacket,					"ESyncAnyESCOPacket");
//TBTeSCORetransmissionTypes
_LIT(KEeSCORetransmitNone,					"EeSCORetransmitNone");
_LIT(KEeSCORetransmitPower,					"EeSCORetransmitPower");
_LIT(KEeSCORetransmitReliability,			"EeSCORetransmitReliability");
_LIT(KEeSCORetransmitDontCare,				"EeSCORetransmitDontCare");
_LIT(KSdpElementTypeNil,					"ETypeNil");
_LIT(KSdpElementTypeUint,					"ETypeUint");
_LIT(KSdpElementTypeInt,					"ETypeInt");
_LIT(KSdpElementTypeUUID,					"ETypeUUID");
_LIT(KSdpElementTypeString,					"ETypeString");
_LIT(KSdpElementTypeBoolean,				"ETypeBoolean");
_LIT(KSdpElementTypeDES,					"ETypeDES");
_LIT(KSdpElementTypeDEA,					"ETypeDEA");
_LIT(KSdpElementTypeURL,					"ETypeURL");
_LIT(KSdpElementTypeEncoded,				"ETypeEncoded");

_LIT(KCSdpAttrValueDES,						"CSdpAttrValueDES");
_LIT(KCSdpAttrValueDEA,						"CSdpAttrValueDEA");
_LIT(KCSdpAttrValueList,					"CSdpAttrValueList");

_LIT(KLowPowerActiveMode,					"EActiveMode");
_LIT(KLowPowerHoldMode,						"EHoldMode");
_LIT(KLowPowerSniffMode,					"ESniffMode");
_LIT(KLowPowerParkMode,						"EParkMode");
_LIT(KLowPowerScatterMode,					"EScatterMode");
_LIT(KLowPowerAnyLowPowerMode,				"EAnyLowPowerMode");

_LIT(KLowPowerMode,							"lowpowermode%d");

_LIT(KShutdownNormal,						"ENormal");
_LIT(KShutdownStopInput,					"EStopInput");
_LIT(KShutdownStopOutput,					"EStopOutput");
_LIT(KShutdownImmediate,					"EImmediate");

_LIT(KShutdownMethod,						"shutdownmethod");

_LIT(KEPacketsDM1,							"EPacketsDM1");
_LIT(KEPacketsDH1,							"EPacketsDH1");
_LIT(KEPacketsDM3,							"EPacketsDM3");
_LIT(KEPacketsDH3,							"EPacketsDH3");
_LIT(KEPacketsDM5,							"EPacketsDM5");
_LIT(KEPacketsDH5,							"EPacketsDH5");
_LIT(KEPackets2_DH1,						"EPackets2_DH1");
_LIT(KEPackets3_DH1,						"EPackets3_DH1");
_LIT(KEPackets2_DH3,						"EPackets2_DH3");
_LIT(KEPackets3_DH3,						"EPackets3_DH3");
_LIT(KEPackets2_DH5,						"EPackets2_DH5");
_LIT(KEPackets3_DH5,						"EPackets3_DH5");
_LIT(KEPacketsHV1,							"EPacketsHV1");
_LIT(KEPacketsHV2,							"EPacketsHV2");
_LIT(KEPacketsHV3,							"EPacketsHV3");
_LIT(KEAnyACLPacket,						"EAnyACLPacket");
_LIT(KEAnySCOPacket,						"EAnySCOPacket");
_LIT(KEAnyPacket,							"EAnyPacket");

_LIT(KPacketTypes,							"packettypes%d");

_LIT(KENotifyMaster,						"ENotifyMaster");
_LIT(KENotifySlave,							"ENotifySlave");
_LIT(KENotifyActiveMode,					"ENotifyActiveMode");
_LIT(KENotifySniffMode,						"ENotifySniffMode");
_LIT(KENotifyParkMode,						"ENotifyParkMode");
_LIT(KENotifyHoldMode,						"ENotifyHoldMode");
_LIT(KENotifyMaxSlots1,						"ENotifyMaxSlots1");
_LIT(KENotifyMaxSlots3,						"ENotifyMaxSlots3");
_LIT(KENotifyMaxSlots5,						"ENotifyMaxSlots5");
_LIT(KENotifyPacketsDM1,					"ENotifyPacketsDM1");
_LIT(KENotifyPacketsDH1,					"ENotifyPacketsDH1");
_LIT(KENotifyPacketsDM3,					"ENotifyPacketsDM3");
_LIT(KENotifyPacketsDH3,					"ENotifyPacketsDH3");
_LIT(KENotifyPacketsDM5,					"ENotifyPacketsDM5");
_LIT(KENotifyPacketsDH5,					"ENotifyPacketsDH5");
_LIT(KENotifyPacketsHV1,					"ENotifyPacketsHV1");
_LIT(KENotifyPacketsHV2,					"ENotifyPacketsHV2");
_LIT(KENotifyPacketsHV3,					"ENotifyPacketsHV3");
_LIT(KENotifyAuthenticationComplete,		"ENotifyAuthenticationComplete");
_LIT(KENotifyEncryptionChangeOn,			"ENotifyEncryptionChangeOn");
_LIT(KENotifyEncryptionChangeOff,			"ENotifyEncryptionChangeOff");
_LIT(KENotifyPhysicalLinkUp,				"ENotifyPhysicalLinkUp");
_LIT(KENotifyPhysicalLinkDown,				"ENotifyPhysicalLinkDown");
_LIT(KENotifyPhysicalLinkError,				"ENotifyPhysicalLinkError");
_LIT(KENotifySynchronousLinkUp,				"ENotifySynchronousLinkUp");
_LIT(KENotifySynchronousLinkDown,			"ENotifySynchronousLinkDown");
_LIT(KENotifySynchronousLinkError,			"ENotifySynchronousLinkError");
_LIT(KENotifySCOLinkUp,						"ENotifySCOLinkUp");
_LIT(KENotifySCOLinkDown,					"ENotifySCOLinkDown");
_LIT(KENotifySCOLinkError,					"ENotifySCOLinkError");
//TBTPhysicalLinkStateNotifierCombinations
_LIT(KENotifyAnyRole,						"ENotifyAnyRole");
_LIT(KENotifyAnyLowPowerMode,				"ENotifyAnyLowPowerMode");
_LIT(KENotifyAnyPowerMode,					"ENotifyAnyPowerMode");
_LIT(KENotifyAnyMaxSlots,					"ENotifyAnyMaxSlots");
_LIT(KENotifyAnyACLPacketType,				"ENotifyAnyACLPacketType");
_LIT(KENotifyAnySCOPacketType,				"ENotifyAnySCOPacketType");
_LIT(KENotifyAnyPacketType,					"ENotifyAnyPacketType");
_LIT(KENotifyAnyPhysicalLinkState,			"ENotifyAnyPhysicalLinkState");

//TShutdown
_LIT(KBaseBandMode,										"basebandmode%d");

//TAvdtpServiceCategory 
_LIT(KAvdtpServiceCategoryNull,							"EServiceCategoryNull");
_LIT(KAvdtpServiceCategoryMediaTransport,				"EServiceCategoryMediaTransport");
_LIT(KAvdtpServiceCategoryReporting,					"EServiceCategoryReporting");
_LIT(KAvdtpServiceCategoryRecovery,						"EServiceCategoryRecovery");
_LIT(KAvdtpServiceCategoryContentProtection,			"EServiceCategoryContentProtection");
_LIT(KAvdtpServiceCategoryHeaderCompression,			"EServiceCategoryHeaderCompression");
_LIT(KAvdtpServiceCategoryMultiplexing,					"EServiceCategoryMultiplexing");
_LIT(KAvdtpServiceCategoryMediaCodec,					"EServiceCategoryMediaCodec");
_LIT(KNumberOfAvdtpServiceCategories,					"ENumberOfServiceCategories");
_LIT(KALLAvdtpServiceCategories,						"EAllServiceCategories");

//TAvdtpRecoveryType 
_LIT(KAvdtpRecoveryTypeEForbidden,						"EForbiddenRecovery");
_LIT(KAvdtpRecoveryTypeERFC2733,						"ERFC2733Recovery");

//TAvdtpSignallingErrorCode
_LIT(KAvdtpBadHeaderFormat,								"EAvdtpBadHeaderFormat");
_LIT(KAvdtpBadLength,									"EAvdtpBadLength");
_LIT(KEAvdtpBadACPSEID,									"EAvdtpBadACPSEID");
_LIT(KAvdtpSEPInUse,									"EAvdtpSEPInUse");
_LIT(KAvdtpSepNotInUse,									"EAvdtpSepNotInUse");
_LIT(KAvdtpBadServCategory,								"EAvdtpBadServCategory");
_LIT(KAvdtpBadPayloadFormat,							"EAvdtpBadPayloadFormat");
_LIT(KAvdtpNotSupportedCommand,							"EAvdtpNotSupportedCommand");
_LIT(KAvdtpInvalidCapabilities,							"EAvdtpInvalidCapabilities");
_LIT(KAvdtpBadRecoveryType,								"EAvdtpBadRecoveryType");
_LIT(KAvdtpBadMediaTransportFormat,						"EAvdtpBadMediaTransportFormat");
_LIT(KAvdtpBadReportingFormat,							"EAvdtpBadReportingFormat");
_LIT(KAvdtpBadRecoveryFormat,							"EAvdtpBadRecoveryFormat");
_LIT(KAvdtpBadRohcFormat,								"EAvdtpBadRohcFormat");
_LIT(KAvdtpBadCpFormat,									"EAvdtpBadCpFormat");
_LIT(KAvdtpBadMultiplexingFormat,						"EAvdtpBadMultiplexingFormat");
_LIT(KAvdtpUnsupportedConfiguration,					"EAvdtpUnsupportedConfiguration");
_LIT(KAvdtpBadState,									"EAvdtpBadState");

//TGavdpSignallingErrorCode
_LIT(KGavdpBadService,									"EGavdpBadService");
_LIT(KGavdpInsufficientResource,						"EGavdpInsufficientResource");

//TA2dpSignallingErrorCode
_LIT(KA2dpInvalidCodec,									"EA2dpInvalidCodec");
_LIT(KA2dpNotSupportedCodec,							"EA2dpNotSupportedCodec");
_LIT(KA2dpInvalidSamplingFrequency,						"EA2dpInvalidSamplingFrequency");
_LIT(KA2dpNotSupportedSamplingFrequency,				"EA2dpNotSupportedSamplingFrequency");
_LIT(KA2dpInvalidChannelMode,							"EA2dpInvalidChannelMode");
_LIT(KA2dpNotSupportedChannelMode,						"EA2dpNotSupportedChannelMode");
_LIT(KA2dpInvalidSubbands,								"EA2dpInvalidSubbands");
_LIT(KA2dpNotSupportedSubbands,							"EA2dpNotSupportedSubbands");
_LIT(KA2dpInvalidAllocationMethod,						"EA2dpInvalidAllocationMethod");
_LIT(KA2dpNotSupportedAllocationMethod,					"EA2dpNotSupportedAllocationMethod");
_LIT(KA2dpInvalidMinimumBitPoolValue,					"EA2dpInvalidMinimumBitPoolValue");
_LIT(KA2dpNotSupportedMinimumBitPoolValue,				"EA2dpNotSupportedMinimumBitPoolValue");
_LIT(KA2dpInvalidMaximumBitPoolValue,					"EA2dpInvalidMaximumBitPoolValue");
_LIT(KA2dpNotSupportedMaximumBitPoolValue,				"EA2dpNotSupportedMaximumBitPoolValue");
_LIT(KA2dpInvalidLayer,									"EA2dpInvalidLayer");
_LIT(KA2dpNotSupportedLayer,							"EA2dpNotSupportedLayer");
_LIT(KA2dpNotSupportedCRC,								"EA2dpNotSupportedCRC");
_LIT(KA2dpNotSupportedMPF,								"EA2dpNotSupportedMPF");
_LIT(KA2dpNotSupportedVBR,								"EA2dpNotSupportedVBR");
_LIT(KA2dpInvalidBitRate,								"EA2dpInvalidBitRate");
_LIT(KA2dpNotSupportedBitRate,							"EA2dpNotSupportedBitRate");
_LIT(KA2dpInvalidObjectType,							"EA2dpInvalidObjectType");
_LIT(KA2dpNotSupportedObjectType,						"EA2dpNotSupportedObjectType");
_LIT(KA2dpInvalidChannels,								"EA2dpInvalidChannels");
_LIT(KA2dpNotSupportedChannels,							"EA2dpNotSupportedChannels");
_LIT(KA2dpInvalidVersion,								"EA2dpInvalidVersion");
_LIT(KA2dpNotSupportedVersion,							"EA2dpNotSupportedVersion");
_LIT(KA2dpNotSupportedSUL,								"EA2dpNotSupportedSUL");
_LIT(KA2dpInvalidBlockLength,							"EA2dpInvalidBlockLength");
_LIT(KA2dpInvalidCPType,								"EA2dpInvalidCPType");
_LIT(KA2dpInvalidCPFormat,								"EA2dpInvalidCPFormat");

//TVdpSignallingErrorCode
_LIT(KVdpInvalidCodecType,								"EVdpInvalidCodecType");
_LIT(KVdpNotSupportedCodecType,							"EVdpNotSupportedCodecType");
_LIT(KVdpInvalidLevel,									"EVdpInvalidLevel");
_LIT(KVdpNotSupportedLevel,								"EVdpNotSupportedLevel");
_LIT(KVdpInvalidCPType,									"EVdpInvalidCPType");
_LIT(KVdpInvalidCPFormat,								"EVdpInvalidCPFormat");

//TBluetoothMediaType
_LIT(KAvdtpMediaTypeAudio,								"EAvdtpMediaTypeAudio");
_LIT(KAvdtpMediaTypeVideo,								"EAvdtpMediaTypeVideo");
_LIT(KAvdtpMediaTypeMultimedia,							"EAvdtpMediaTypeMultimedia");

//TAvdtpTransportSessionType
_LIT(KMedia,											"EMedia");
_LIT(KReporting,										"EReporting");
_LIT(KRecovery,											"ERecovery");
_LIT(KSignalling,										"ESignalling");
_LIT(KNumStreamTypes,									"ENumStreamTypes");
/*@}*/

void CT_BTUtil::GetChannelPriority(CDataWrapper& aDataWrapper, TL2CapConfig::TChannelPriority& aType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	typeName;		
	TBool ret=aDataWrapper.GetStringFromConfig(aSection, aField, typeName);
	if ( ret )
		{
		if (typeName == KEHigh)
			{
			aType = TL2CapConfig::EHigh;
			}
		else if (typeName == KEMedium)
			{
			aType = TL2CapConfig::EMedium;
			}
		else if (typeName == KELow)
			{
			aType = TL2CapConfig::ELow;
			}	
		else 
			{
			TInt	intType;
			if ( aDataWrapper.GetIntFromConfig(aSection, aField, intType) )
				{
				aType = (TL2CapConfig::TChannelPriority) intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TL2CapConfig::TChannelPriority %S"), &aField);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	}

void CT_BTUtil::GetChannel(CDataWrapper& aDataWrapper, TL2CapConfig::TChannelReliability& aType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	typeName;		
	
	TBool ret=aDataWrapper.GetStringFromConfig(aSection, aField, typeName);
	if ( ret )
		{
		if (typeName == KEReliableChannel)
			{
			aType = TL2CapConfig::EReliableChannel;
			}
		else if (typeName == KEUnreliableChannel)
			{
			aType = TL2CapConfig::EUnreliableChannel;
			}
		else 
			{
			TInt	intType;
			if ( aDataWrapper.GetIntFromConfig(aSection, aField, intType) )
				{
				aType = (TL2CapConfig::TChannelReliability) intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TL2CapConfig::TChannelReliability %S"), &aField);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	}

void CT_BTUtil::GetBitValue(CDataWrapper& aDataWrapper, TBps& aType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	typeName;		
	TBool ret=aDataWrapper.GetStringFromConfig(aSection, aField, typeName);
	if ( ret )
		{
		if (typeName == KEBps50)
			{
			aType = EBps50;
			}
		else if (typeName == KEBps75)
			{
			aType = EBps75;
			}
		else if (typeName == KEBps110)
			{
			aType = EBps110;
			}
		else if (typeName == KEBps134)
			{
			aType = EBps134;
			}
		else if (typeName == KEBps150)
			{
			aType = EBps150;
			}
		else if (typeName == KEBps300)
			{
			aType = EBps300;
			}
		else if (typeName == KEBps600)
			{
			aType = EBps600;
			}
		else if (typeName == KEBps1200)
			{
			aType = EBps1200;
			}
		else if (typeName == KEBps1800)
			{
			aType = EBps1800;
			}
		else if (typeName == KEBps2000)
			{
			aType = EBps2000;
			}
		else if (typeName == KEBps2400)
			{
			aType = EBps2400;
			}
		else if (typeName == KEBps3600)
			{
			aType = EBps3600;
			}
		else if (typeName == KEBps4800)
			{
			aType = EBps4800;
			}
		else if (typeName == KEBps7200)
			{
			aType = EBps7200;
			}
		else if (typeName == KEBps9600)
			{
			aType = EBps9600;
			}
		else if (typeName == KEBps19200)
			{
			aType = EBps19200;
			}
		else if (typeName == KEBps38400)
			{
			aType = EBps38400;
			}
		else if (typeName == KEBps57600)
			{
			aType = EBps57600;
			}
		else if (typeName == KEBps115200)
			{
			aType = EBps115200;
			}
		else if (typeName == KEBps230400)
			{
			aType = EBps230400;
			}
		else if (typeName == KEBps460800)
			{
			aType = EBps460800;
			}
		else if (typeName == KEBps576000)
			{
			aType = EBps576000;
			}
		else if (typeName == KEBps1152000)
			{
			aType = EBps1152000;
			}
		else if (typeName == KEBps4000000)
			{
			aType = EBps4000000;
			}
		else if (typeName == KEBps921600)
			{
			aType = EBps921600;
			}
		else if (typeName == KEBpsAutobaud)
			{
			aType = EBpsAutobaud;
			}
		else if (typeName == KEBpsSpecial)
			{
			aType = EBpsSpecial;
			}
		else 
			{
			TInt	intType;
			if ( aDataWrapper.GetIntFromConfig(aSection, aField, intType) )
				{
				aType = (TBps) intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TBps %S"), &aField);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	}

void CT_BTUtil::GetDataBits(CDataWrapper& aDataWrapper, TDataBits& aType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	typeName;		
	TBool ret=aDataWrapper.GetStringFromConfig(aSection, aField, typeName);
	if ( ret )
		{
		if (typeName == KEData5)
			{
			aType = EData5;
			}
		else if (typeName == KEData6)
			{
			aType = EData6;
			}
		else if (typeName == KEData7)
			{
			aType = EData7;
			}
		else if (typeName == KEData8)
			{
			aType = EData8;
			}
		else 
			{
			TInt	intType;
			ret = aDataWrapper.GetIntFromConfig(aSection, aField, intType); 
			if ( ret )
				{
				aType = (TDataBits) intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TDataBits %S"), &aField);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	}

void CT_BTUtil::GetParity(CDataWrapper& aDataWrapper, TParity& aType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	typeName;		
	TBool ret=aDataWrapper.GetStringFromConfig(aSection, aField, typeName);
	if ( ret )
		{
		if (typeName == KEParityNone)
			{
			aType = EParityNone;
			}
		else if (typeName == KEParityEven)
			{
			aType = EParityEven;
			}
		else if (typeName == KEParityOdd)
			{
			aType = EParityOdd;
			}
		else if (typeName == KEParityMark)
			{
			aType = EParityMark;
			}
		else if (typeName == KEParitySpace)
			{
			aType = EParitySpace;
			}
		else 
			{
			TInt	intType;
			if ( aDataWrapper.GetIntFromConfig(aSection, aField, intType) )
				{
				aType = (TParity) intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TParity %S"), &aField);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	}

void CT_BTUtil::GetStopBits(CDataWrapper& aDataWrapper, TStopBits& aType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	typeName;		
	TBool ret=aDataWrapper.GetStringFromConfig(aSection, aField, typeName);
	if ( ret )
		{
		if (typeName == KEStop1)
			{
			aType = EStop1;
			}
		else if (typeName == KEStop2)
			{
			aType = EStop2;
			}
		else 
			{
			TInt	intType;
			if ( aDataWrapper.GetIntFromConfig(aSection, aField, intType) )
				{
				aType = (TStopBits) intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TStopBits %S"), &aField);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	}

void CT_BTUtil::GetTRPNFlowCtrlMask(CDataWrapper& aDataWrapper, TRPNFlowCtrlMask& aType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	typeName;		
	TBool ret=aDataWrapper.GetStringFromConfig(aSection, aField, typeName);
	if ( ret )
		{
		if (typeName == KEFCXOnOffInput)
			{
			aType = EFCXOnOffInput;
			}
		else if (typeName == KEFCXOnOffOutput)
			{
			aType = EFCXOnOffOutput;
			}
		else if (typeName == KEFCRTRInput)
			{
			aType = EFCRTRInput;
			}
		else if (typeName == KEFCRTROutput)
			{
			aType = EFCRTROutput;
			}
		else if (typeName == KEFCRTCInput)
			{
			aType = EFCRTCInput;
			}
		else if (typeName == KEFCRTCOutput)
			{
			aType = EFCRTCOutput;
			}
		else
			{
			TInt	intType;
			if ( aDataWrapper.GetIntFromConfig(aSection, aField, intType) )
				{
				aType = (TRPNFlowCtrlMask) intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TRPNFlowCtrlMask mask %S"), &aField);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	}


TBool CT_BTUtil::GetPacketTypeIntValue(TPtrC packetTypeName, TInt& aValue)
	{
	TBool	ret	= ETrue;
	//TBTPacketType
	if ( packetTypeName==KEPacketsDM1 )
		{
		aValue=EPacketsDM1;
		}
	else if ( packetTypeName==KEPacketsDH1 )
		{
		aValue=EPacketsDH1;
		}
	else if ( packetTypeName==KEPacketsDM3 )
		{
		aValue=EPacketsDM3;
		}
	else if ( packetTypeName==KEPacketsDH3 )
		{
		aValue=EPacketsDH3;
		}
	else if ( packetTypeName==KEPacketsDM5 )
		{
		aValue=EPacketsDM5;
		}
	else if ( packetTypeName==KEPacketsDH5 )
		{
		aValue=EPacketsDH5;
		}
	else if ( packetTypeName==KEPackets2_DH1 )
		{
		aValue=EPackets2_DH1;
		}
	else if ( packetTypeName==KEPackets3_DH1 )
		{
		aValue=EPackets3_DH1;
		}
	else if ( packetTypeName==KEPackets2_DH3 )
		{
		aValue=EPackets2_DH3;
		}
	else if ( packetTypeName==KEPackets3_DH3 )
		{
		aValue=EPackets3_DH3;
		}
	else if ( packetTypeName==KEPackets2_DH5 )
		{
		aValue=EPackets2_DH5;
		}
	else if ( packetTypeName==KEPackets3_DH5 )
		{
		aValue=EPackets3_DH5;
		}
	else if ( packetTypeName==KEPacketsHV1 )
		{
		aValue=EPacketsHV1;
		}
	else if ( packetTypeName==KEPacketsHV2 )
		{
		aValue=EPacketsHV2;
		}
	else if ( packetTypeName==KEPacketsHV3 )
		{
		aValue=EPacketsHV3;
		}
	//TBTPacketTypeCombinations
	else if ( packetTypeName==KEAnyACLPacket )
		{
		aValue=EAnyACLPacket;
		}
	else if ( packetTypeName==KEAnySCOPacket )
		{
		aValue=EAnySCOPacket;
		}
	else if ( packetTypeName==KEAnyPacket )
		{
		aValue=EAnyPacket;
		}
	//ESCO packet type
	else if ( packetTypeName==KESyncPacketsEV3 )
		{
		aValue=TBTSyncPackets::ESyncPacketsEV3;
		}
	else if ( packetTypeName==KESyncPacketsEV4 )
		{
		aValue=TBTSyncPackets::ESyncPacketsEV4;
		}
	else if ( packetTypeName==KESyncPacketsEV5 )
		{
		aValue=TBTSyncPackets::ESyncPacketsEV5;
		}
	else if ( packetTypeName==KESyncAnyESCOPacket )
		{
		aValue=TBTSyncPackets::ESyncAnyESCOPacket;
		}
	else
		{
		ret=EFalse;
		aValue=-1;
		}
	
	return ret;
	}


TBool CT_BTUtil::ReadTBTSyncPackets(CDataWrapper& aDataWrapper, TBTSyncPackets::TSyncPackets& aSyncPacket, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	syncPacket;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, aField, syncPacket);
	if ( ret )
		{
		if ( syncPacket==KESyncPacketsHV1 )
			{
			aSyncPacket=TBTSyncPackets::ESyncPacketsHV1;
			}
		else if ( syncPacket==KESyncPacketsHV2 )
			{
			aSyncPacket=TBTSyncPackets::ESyncPacketsHV2;
			}
		else if ( syncPacket==KESyncPacketsHV3 )
			{
			aSyncPacket=TBTSyncPackets::ESyncPacketsHV3;
			}
		else if ( syncPacket==KESyncPacketsEV3 )
			{
			aSyncPacket=TBTSyncPackets::ESyncPacketsEV3;
			}
		else if ( syncPacket==KESyncPacketsEV4 )
			{
			aSyncPacket=TBTSyncPackets::ESyncPacketsEV4;
			}
		else if ( syncPacket==KESyncPacketsEV5 )
			{
			aSyncPacket=TBTSyncPackets::ESyncPacketsEV5;
			}
		else if ( syncPacket==KESyncAnySCOPacket )
			{
			aSyncPacket=TBTSyncPackets::ESyncAnySCOPacket;
			}
		else if ( syncPacket==KESyncAnyESCOPacket )
			{
			aSyncPacket=TBTSyncPackets::ESyncAnyESCOPacket;
			}
		else
			{
			TInt	intType;
			ret=aDataWrapper.GetIntFromConfig(aSection, aField, intType);
			if ( ret )
				{
				aSyncPacket=(TBTSyncPackets::TSyncPackets)intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF1(_L("Unknow input for TBTSyncPackets"));
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	return ret;
	}


TBool CT_BTUtil::ReadTBTeSCORetransmissionTypes(CDataWrapper& aDataWrapper, TBTeSCORetransmissionTypes& aRetransmissionType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	retransmissionType;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, aField, retransmissionType);
	if ( ret )
		{
		if ( retransmissionType==KEeSCORetransmitNone )
			{
			aRetransmissionType=EeSCORetransmitNone;
			}
		else if ( retransmissionType==KEeSCORetransmitPower )
			{
			aRetransmissionType=EeSCORetransmitPower;
			}
		else if ( retransmissionType==KEeSCORetransmitReliability )
			{
			aRetransmissionType=EeSCORetransmitReliability;
			}
		else if ( retransmissionType==KEeSCORetransmitDontCare )
			{
			aRetransmissionType=EeSCORetransmitDontCare;
			}
		else
			{
			TInt	intType;
			ret=aDataWrapper.GetIntFromConfig(aSection, aField, intType);
			if ( ret )
				{
				aRetransmissionType=(TBTeSCORetransmissionTypes)intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF1(_L("Unknow input for TBTeSCORetransmissionTypes"));
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	return ret;
	}


TBool CT_BTUtil::GetLowPowerModeIntValue(TPtrC lowPowerModeName, TInt& aValue)
	{
	TBool	ret	= ETrue;
	if (lowPowerModeName == KLowPowerHoldMode)
		{
		aValue = EHoldMode;
		}
	else if (lowPowerModeName == KLowPowerSniffMode)
		{
		aValue = ESniffMode;
		}
	else if (lowPowerModeName == KLowPowerParkMode)
		{
		aValue = EParkMode;
		}
	else if (lowPowerModeName == KLowPowerAnyLowPowerMode)
		{
		aValue = EAnyLowPowerMode;
		}
	else
		{
		ret=EFalse;
		aValue=-1;
		}
	
	return ret;
	}


TBool CT_BTUtil::GetPhysicalLinkStateIntValue(TPtrC physicalLinkStateName, TInt& aValue)
	{
	TBool	ret	= ETrue;
	//TBTPhysicalLinkStateNotifier
	if (physicalLinkStateName == KENotifyMaster)
		{
		aValue = ENotifyMaster;
		}
	else if (physicalLinkStateName == KENotifySlave)
		{
		aValue = ENotifySlave;
		}
	else if (physicalLinkStateName == KENotifyActiveMode)
		{
		aValue = ENotifyActiveMode;
		}
	else if (physicalLinkStateName == KENotifySniffMode)
		{
		aValue = ENotifySniffMode;
		}
	else if (physicalLinkStateName == KENotifyParkMode)
		{
		aValue = ENotifyParkMode;
		}
	else if (physicalLinkStateName == KENotifyHoldMode)
		{
		aValue = ENotifyHoldMode;
		}
	else if (physicalLinkStateName == KENotifyMaxSlots1)
		{
		aValue = ENotifyMaxSlots1;
		}
	else if (physicalLinkStateName == KENotifyMaxSlots3)
		{
		aValue = ENotifyMaxSlots3;
		}
	else if (physicalLinkStateName == KENotifyMaxSlots5)
		{
		aValue = ENotifyMaxSlots5;
		}
	else if (physicalLinkStateName == KENotifyPacketsDM1)
		{
		aValue = ENotifyPacketsDM1;
		}
	else if (physicalLinkStateName == KENotifyPacketsDH1)
		{
		aValue = ENotifyPacketsDH1;
		}
	else if (physicalLinkStateName == KENotifyPacketsDM3)
		{
		aValue = ENotifyPacketsDM3;
		}
	else if (physicalLinkStateName == KENotifyPacketsDH3)
		{
		aValue = ENotifyPacketsDH3;
		}
	else if (physicalLinkStateName == KENotifyPacketsDM5)
		{
		aValue = ENotifyPacketsDM5;
		}
	else if (physicalLinkStateName == KENotifyPacketsDH5)
		{
		aValue = ENotifyPacketsDH5;
		}
	else if (physicalLinkStateName == KENotifyPacketsHV1)
		{
		aValue = ENotifyPacketsHV1;
		}
	else if (physicalLinkStateName == KENotifyPacketsHV2)
		{
		aValue = ENotifyPacketsHV2;
		}
	else if (physicalLinkStateName == KENotifyPacketsHV3)
		{
		aValue = ENotifyPacketsHV3;
		}
	else if (physicalLinkStateName == KENotifyAuthenticationComplete)
		{
		aValue = ENotifyAuthenticationComplete;
		}
	else if (physicalLinkStateName == KENotifyEncryptionChangeOn)
		{
		aValue = ENotifyEncryptionChangeOn;
		}
	else if (physicalLinkStateName == KENotifyEncryptionChangeOff)
		{
		aValue = ENotifyEncryptionChangeOff;
		}
	else if (physicalLinkStateName == KENotifyPhysicalLinkUp)
		{
		aValue = ENotifyPhysicalLinkUp;
		}
	else if (physicalLinkStateName == KENotifyPhysicalLinkDown)
		{
		aValue = ENotifyPhysicalLinkDown;
		}
	else if (physicalLinkStateName == KENotifyPhysicalLinkError)
		{
		aValue = ENotifyPhysicalLinkError;
		}
	else if (physicalLinkStateName == KENotifySynchronousLinkUp)
		{
		aValue = ENotifySynchronousLinkUp;
		}
	else if (physicalLinkStateName == KENotifySynchronousLinkDown)
		{
		aValue = ENotifySynchronousLinkDown;
		}
	else if (physicalLinkStateName == KENotifySynchronousLinkError)
		{
		aValue = ENotifySynchronousLinkError;
		}
	else if (physicalLinkStateName == KENotifySCOLinkUp)
		{
		aValue = ENotifySCOLinkUp;
		}
	else if (physicalLinkStateName == KENotifySCOLinkDown)
		{
		aValue = ENotifySCOLinkDown;
		}
	else if (physicalLinkStateName == KENotifySCOLinkError)
		{
		aValue = ENotifySCOLinkError;
		}
	//TBTPhysicalLinkStateNotifierCombinations
	else if (physicalLinkStateName == KENotifyAnyRole)
		{
		aValue = ENotifyAnyRole;
		}
	else if (physicalLinkStateName == KENotifyAnyLowPowerMode)
		{
		aValue = ENotifyAnyLowPowerMode;
		}
	else if (physicalLinkStateName == KENotifyAnyPowerMode)
		{
		aValue = ENotifyAnyPowerMode;
		}
	else if (physicalLinkStateName == KENotifyAnyMaxSlots)
		{
		aValue = ENotifyAnyMaxSlots;
		}
	else if (physicalLinkStateName == KENotifyAnyACLPacketType)
		{
		aValue = ENotifyAnyACLPacketType;
		}
	else if (physicalLinkStateName == KENotifyAnySCOPacketType)
		{
		aValue = ENotifyAnySCOPacketType;
		}
	else if (physicalLinkStateName == KENotifyAnyPacketType)
		{
		aValue = ENotifyAnyPacketType;
		}
	else if (physicalLinkStateName == KENotifyAnyPhysicalLinkState)
		{
		aValue = ENotifyAnyPhysicalLinkState;
		}
	else
		{
		ret=EFalse;
		aValue=-1;
		}
	
	return ret;
	}

TBool CT_BTUtil::ReadSdpElementType(CDataWrapper& aDataWrapper, TSdpElementType& aType, const TDesC& aSection, const TDesC& aField)
	{
	TPtrC	type;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, aField, type);
	if ( ret )
		{
		if ( type==KSdpElementTypeNil )
			{
			aType=ETypeNil;
			}
		else if ( type==KSdpElementTypeUint )
			{
			aType=ETypeUint;
			}
		else if ( type==KSdpElementTypeInt )
			{
			aType=ETypeInt;
			}
		else if ( type==KSdpElementTypeUUID )
			{
			aType=ETypeUUID;
			}
		else if ( type==KSdpElementTypeString )
			{
			aType=ETypeString;
			}
		else if ( type==KSdpElementTypeBoolean )
			{
			aType=ETypeBoolean;
			}
		else if ( type==KSdpElementTypeDES )
			{
			aType=ETypeDES;
			}
		else if ( type==KSdpElementTypeDEA )
			{
			aType=ETypeDEA;
			}
		else if ( type==KSdpElementTypeURL )
			{
			aType=ETypeURL;
			}
		else if ( type==KSdpElementTypeEncoded )
			{
			aType=ETypeEncoded;
			}
		else
			{
			TInt	intType;
			ret=aDataWrapper.GetIntFromConfig(aSection, aField, intType);
			if ( ret )
				{
				aType=(TSdpElementType)intType;
				}
			else
				{
				aDataWrapper.ERR_PRINTF1(_L("Unknow input for TSdpElementType"));
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}

	return ret;
	}

MSdpElementBuilder* CT_BTUtil::ElementBuilderCastL(CDataWrapper& aDataWrapper, const TDesC& aObjectName, const TDesC& aTypeName)
	{
	MSdpElementBuilder*	ret = NULL;
	TAny*				object=aDataWrapper.GetDataObjectL(aObjectName);

	if( aTypeName == KCSdpAttrValueDES )
		{
		ret = static_cast<CSdpAttrValueDES*>(object);
		}
	else if( aTypeName == KCSdpAttrValueDEA )
		{
		ret = static_cast<CSdpAttrValueDEA*>(object);
		}
	else if( aTypeName == KCSdpAttrValueList )
		{
		ret = static_cast<CSdpAttrValueList*>(object);
		}
	else
		{
		aDataWrapper.ERR_PRINTF2(_L("Unknow Type %S for ElementBuilderCastL call"), &aTypeName);
		aDataWrapper.SetBlockResult(EFail);
		}

	return ret;
	}

CSdpAttrValueList* CT_BTUtil::AttrValueListCastL(CDataWrapper& aDataWrapper, const TDesC& aObjectName, const TDesC& aTypeName)
	{
	CSdpAttrValueList*	ret = NULL;
	TAny*				object=aDataWrapper.GetDataObjectL(aObjectName);

	if( aTypeName == KCSdpAttrValueDES )
		{
		ret = static_cast<CSdpAttrValueDES*>(object);
		}
	else if( aTypeName == KCSdpAttrValueDEA )
		{
		ret = static_cast<CSdpAttrValueDEA*>(object);
		}
	else if( aTypeName == KCSdpAttrValueList )
		{
		ret = static_cast<CSdpAttrValueList*>(object);
		}
	else
		{
		aDataWrapper.ERR_PRINTF2(_L("Unknow Type %S for AttrValueListCastL call"), &aTypeName);
		aDataWrapper.SetBlockResult(EFail);
		}

	return ret;
	}

TUint CT_BTUtil::ConvertUUID32toInt(const TUUID& aUUID)
	{
	TUint	result = aUUID[3] + 256*(aUUID[2] + 256*(aUUID[1] + aUUID[0]*256));

	return result;
	}

void CT_BTUtil::GetLowPowerMode(CDataWrapper& aDataWrapper, const TDesC& aSection, TUint32& aModes)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	aModes=0;
	TBool	moreData=ETrue;
	TInt	index=0;
	while ( moreData )
		{
		TPtrC	modeName;
		tempStore.Format(KLowPowerMode(), ++index);

		moreData=aDataWrapper.GetStringFromConfig(aSection, tempStore, modeName);
		if ( moreData )
			{
			if (modeName == KLowPowerActiveMode)
				{
				aModes |= EActiveMode;
				}
			else if (modeName == KLowPowerHoldMode)
				{
				aModes |= EHoldMode;
				}
			else if (modeName == KLowPowerSniffMode)
				{
				aModes |= ESniffMode;
				}
			else if (modeName == KLowPowerParkMode)
				{
				aModes |= EParkMode;
				}
			else if (modeName == KLowPowerScatterMode)
				{
				aModes |= EScatterMode;
				}
			else if (modeName == KLowPowerAnyLowPowerMode)
				{
				aModes |= EAnyLowPowerMode;
				}
			else
				{
				TInt	intMode;
				if ( aDataWrapper.GetIntFromConfig(aSection, tempStore, intMode) )
					{
					aModes |= intMode;
					}
				else
					{
					aDataWrapper.ERR_PRINTF2(_L("Illegal mode %S"), &tempStore);
					aDataWrapper.SetBlockResult(EFail);
					}
				}
			}
		else if (index==1)
			{
			//Special case as we would want to use default if not found at all
			aModes=EAnyLowPowerMode;
			}
		}
	}

TBool CT_BTUtil::GetShutdown(CDataWrapper& aDataWrapper, const TDesC& aSection, RSocket::TShutdown& aShutdown)
	{

	TPtrC	shutdownMethod;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, KShutdownMethod(), shutdownMethod);
	if ( ret )
		{
		if (shutdownMethod == KShutdownNormal)
			{
			aShutdown = RSocket::ENormal;
			}
		else if (shutdownMethod == KShutdownStopInput)
			{
			aShutdown = RSocket::EStopInput;
			}
		else if (shutdownMethod == KShutdownStopOutput)
			{
			aShutdown = RSocket::EStopOutput;
			}
		else if (shutdownMethod == KShutdownImmediate)
			{
			aShutdown = RSocket::EImmediate;
			}
		else
			{
			TInt	intMode;
			if ( aDataWrapper.GetIntFromConfig(aSection, KShutdownMethod(), intMode) )
				{
				aShutdown = (RSocket::TShutdown)intMode;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal mode %S"), &KShutdownMethod());
				aDataWrapper.SetBlockResult(EFail);
				ret=EFalse;
				}
			}
		}

	return ret;
	}

void CT_BTUtil::GetTBTPacketType(CDataWrapper& aDataWrapper, const TDesC& aSection, TInt& aType)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	aType=0;
	TBool	moreData=ETrue;
	TInt	index=0;
	while ( moreData )
		{
		TPtrC	typeName;
		tempStore.Format(KPacketTypes(), ++index);

		moreData=aDataWrapper.GetStringFromConfig(aSection, tempStore, typeName);
		if ( moreData )
			{
			if (typeName == KEPacketsDM1)
				{
				aType |= EPacketsDM1;
				}
			else if (typeName == KEPacketsDH1)
				{
				aType |= EPacketsDH1;
				}
			else if (typeName == KEPacketsDM3)
				{
				aType |= EPacketsDM3;
				}
			else if (typeName == KEPacketsDH3)
				{
				aType |= EPacketsDH3;
				}
			else if (typeName == KEPacketsDM5)
				{
				aType |= EPacketsDM5;
				}
			else if (typeName == KEPacketsDH5)
				{
				aType |= EPacketsDH5;
				}
			else if (typeName == KEPacketsHV1)
				{
				aType |= EPacketsHV1;
				}
		 	else if (typeName == KEPacketsHV2)
				{
				aType |= EPacketsHV2;
				}
		 	else if (typeName == KEPacketsHV3)
				{
				aType |= EPacketsHV3;
				}
		  	else if (typeName == KEAnyACLPacket)
				{
				aType |= EAnyACLPacket;
				}
		  	else if (typeName == KEAnySCOPacket)
				{
				aType |= EAnySCOPacket;
				}
		  	else if (typeName == KEAnyPacket)
				{
				aType |= EAnyPacket;
				}
			else
				{
				TInt	intType;
				if ( aDataWrapper.GetIntFromConfig(aSection, tempStore, intType) )
					{
					aType |= intType;
					}
				else
					{
					aDataWrapper.ERR_PRINTF2(_L("Illegal type %S"), &tempStore);
					aDataWrapper.SetBlockResult(EFail);
					}
				}
			}
		else if (index==1)
			{
			//Special case as we would want to use default if not found at all
			aType=EAnyPacket;
			}
		}
	}


void CT_BTUtil::GetLinkStateNotifierBits(CDataWrapper& aDataWrapper, const TDesC& aSection, TUint32& aModes)
	{
	// This is from BTTypes.h

	// Bitmask values for notifying/requesting notification of the state of a physical link.
	//
	// These values have a two fold purpose:
	// 1) They can be used along with TBTPhysicalLinkStateNotifierCombinations to specify
	// a set of baseband change events for which notification is desired.
	// 2) They can be used to notify the state of a physical link or a subset of that state.
	// For example if only mode change events are being monitored, then at each mode change event
	// only the bit reperesenting the new mode will be returned, however if the whole link state
	// is required, then a combination of bit values indicating that state will be returned.
	//
	// THESE BIT VALUES MUST ONLY BE USED WHEN REQUESTING NOTIFICATION, OR WHEN PARSING THE
	// RESULTS OF A NOTIFICATION. All other baseband state activities should use one of the
	// appropriate Bluetooth SIG specified enums
	// TBTBasebandRole, TBTLinkMode, TBTLinkModeCombinations, TBTPacketType,
	// TBTPacketTypeCombinations
	//

	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	aModes=0;
	TBool	moreData=ETrue;
	TInt	index=0;
	while ( moreData )
		{
		TPtrC	modeName;
		tempStore.Format(KBaseBandMode(), ++index);

		moreData=aDataWrapper.GetStringFromConfig(aSection, tempStore, modeName);
		if ( moreData )
			{
			//Master
			if (modeName == KENotifyMaster)
				{
				aModes |= ENotifyMaster;
				}
			//Slave
			else if (modeName == KENotifySlave)
				{
				aModes |= ENotifySlave;
				}
			//Active Mode
			else if (modeName == KENotifyActiveMode)
				{
				aModes |= ENotifyActiveMode;
				}
			//Sniff mode
			else if (modeName == KENotifySniffMode)
				{
				aModes |= ENotifySniffMode;
				}
			//Park mode
			else if (modeName == KENotifyParkMode)
				{
				aModes |= ENotifyParkMode;
				}
			//Hold mode
			else if (modeName == KENotifyHoldMode)
				{
				aModes |= ENotifyHoldMode;
				}
			//Max slots 1
			else if (modeName == KENotifyMaxSlots1)
				{
				aModes |= ENotifyMaxSlots1;
				}
			//Max slots 3
			else if (modeName == KENotifyMaxSlots3)
				{
				aModes |= ENotifyMaxSlots3;
				}
			//Max slots 5
			else if (modeName == KENotifyMaxSlots5)
				{
				aModes |= ENotifyMaxSlots5;
				}
			//Packets DM1
			else if (modeName == KENotifyPacketsDM1)
				{
				aModes |= ENotifyPacketsDM1;
				}
			//Packets DH1
			else if (modeName == KENotifyPacketsDH1)
				{
				aModes |= ENotifyPacketsDH1;
				}
			//Packets DM3
			else if (modeName == KENotifyPacketsDM3)
				{
				aModes |= ENotifyPacketsDM3;
				}
			//Packets DH3
			else if (modeName == KENotifyPacketsDH3)
				{
				aModes |= ENotifyPacketsDH3;
				}
			//Packets DM5
			else if (modeName == KENotifyPacketsDM5)
				{
				aModes |= ENotifyPacketsDM5;
				}
			//Packets DH5
			else if (modeName == KENotifyPacketsDH5)
				{
				aModes |= ENotifyPacketsDH5;
				}
			//HV1
			else if (modeName == KENotifyPacketsHV1)
				{
				aModes |= ENotifyPacketsHV1;
				}
			//HV2
			else if (modeName == KENotifyPacketsHV2)
				{
				aModes |= ENotifyPacketsHV2;
				}
			//HV3
			else if (modeName == KENotifyPacketsHV3)
				{
				aModes |= ENotifyPacketsHV3;
				}
			//Authentication Complete
			else if (modeName == KENotifyAuthenticationComplete)
				{
				aModes |= ENotifyAuthenticationComplete;
				}
			//Encryption Change On
			else if (modeName == KENotifyEncryptionChangeOn)
				{
				aModes |= ENotifyEncryptionChangeOn;
				}
			//Encryption Change Off
			else if (modeName == KENotifyEncryptionChangeOff)
				{
				aModes |= ENotifyEncryptionChangeOff;
				}
			//Physical Link Up
			else if (modeName == KENotifyPhysicalLinkUp)
				{
				aModes |= ENotifyPhysicalLinkUp;
				}
			//Physical Link Down
			else if (modeName == KENotifyPhysicalLinkDown)
				{
				aModes |= ENotifyPhysicalLinkDown;
				}
			//Physical Link Error
			else if (modeName == KENotifyPhysicalLinkError)
				{
				aModes |= ENotifyPhysicalLinkError;
				}
			//SCO Link Up
			else if (modeName == KENotifySCOLinkUp)
				{
				aModes |= ENotifySCOLinkUp;
				}
			//SCO Link Down
			else if (modeName == KENotifySCOLinkDown)
				{
				aModes |= ENotifySCOLinkDown;
				}
			//SCO Link Error
			else if (modeName == KENotifySCOLinkError)
				{
				aModes |= ENotifySCOLinkError;
				}
			else
				{
				TInt	intMode;
				if ( aDataWrapper.GetIntFromConfig(aSection, tempStore, intMode) )
					{
					aModes |= intMode;
					}
				else
					{
					aDataWrapper.ERR_PRINTF2(_L("Illegal type %S"), &KShutdownMethod());
					aDataWrapper.SetBlockResult(EFail);
					}
				}
			}
		}
	}

// method splitted in blocks to save stack space
TBool CT_BTUtil::GetIntValue(TPtrC aConstantName, TInt& aValue)
	{
	TBool	ret	= ETrue;
	// ************************************************************************
	// bt_sock.h
	// ************************************************************************
	_LIT(KBTAddrFamilyName,							"KBTAddrFamily");
	_LIT(KBTLinkManagerName,						"KBTLinkManager");
	_LIT(KL2CAPName,								"KL2CAP");
	_LIT(KRFCOMMName,								"KRFCOMM");
	_LIT(KSDPName,									"KSDP");
	_LIT(KAVCTPName,								"KAVCTP");
	_LIT(KTCIL2CAPName,								"KTCIL2CAP");
	_LIT(KBTMajorName,								"KBTMajor");
	_LIT(KBTMinorName,								"KBTMinor");
	_LIT(KBTBuildName,								"KBTBuild");
	_LIT(KSolBtBlogName,							"KSolBtBlog");
	_LIT(KSolBtHCIName,								"KSolBtHCI");
	_LIT(KSolBtLMName,								"KSolBtLM");
	_LIT(KSolBtL2CAPName,							"KSolBtL2CAP");
	_LIT(KSolBtRFCOMMName,							"KSolBtRFCOMM");
	_LIT(KSolBtAVCTPName,							"KSolBtAVCTP");
	_LIT(KSolBtACLName,								"KSolBtACL");
	_LIT(KSolBtAVDTPSignallingName,					"KSolBtAVDTPSignalling");
	_LIT(KSolBtAVDTPMediaName,						"KSolBtAVDTPMedia");
	_LIT(KSolBtAVDTPReportingName,					"KSolBtAVDTPReporting");
	_LIT(KSolBtAVDTPRecoveryName,					"KSolBtAVDTPRecovery");
	_LIT(KSolBtAVDTPInternalName,					"KSolBtAVDTPInternal");
	_LIT(KSolBtLMProxyName,							"KSolBtLMProxy");
	_LIT(KSolBtSAPBaseName,							"KSolBtSAPBase");
	_LIT(KSCOListenQueSizeName,						"KSCOListenQueSize");
	_LIT(KL2MinMTUName,								"KL2MinMTU");
	_LIT(KMaxPSMName,								"KMaxPSM");
	_LIT(KMinUserPSMName,							"KMinUserPSM");
	_LIT(KMinPSMName,								"KMinPSM");
	_LIT(KHostResInquiryName,						"KHostResInquiry");
	_LIT(KHostResNameName,							"KHostResName");
	_LIT(KHostResIgnoreCacheName,					"KHostResIgnoreCache");
	_LIT(KHostResCacheName,							"KHostResCache");
	_LIT(KL2CAPEchoRequestIoctlName,				"KL2CAPEchoRequestIoctl");
	_LIT(KL2CAPIncomingMTUIoctlName,				"KL2CAPIncomingMTUIoctl");
	_LIT(KL2CAPOutgoingMTUIoctlName,				"KL2CAPOutgoingMTUIoctl");
	_LIT(KL2CAPUpdateChannelConfigIoctlName,		"KL2CAPUpdateChannelConfigIoctl");
	_LIT(KDefaultBasebandConnectionTimeoutName,		"KDefaultBasebandConnectionTimeout");
	_LIT(KLMAddSCOConnIoctlName,					"KLMAddSCOConnIoctl");
	_LIT(KHCIRemoveSCOConnIoctlName,				"KHCIRemoveSCOConnIoctl");
	_LIT(KHCIChangePacketTypeIoctlName,				"KHCIChangePacketTypeIoctl");
	_LIT(KHCIAuthRequestIoctlName,					"KHCIAuthRequestIoctl");
	_LIT(KHCIEncryptIoctlName,						"KHCIAuthRequestIoctl");
	_LIT(KHCIChangeLinkKeyIoctlName,				"KHCIChangeLinkKeyIoctl");
	_LIT(KHCIMasterLinkKeyIoctlName,				"KHCIMasterLinkKeyIoctl");
	_LIT(KHCIHoldModeIoctlName,						"KHCIHoldModeIoctl");
	_LIT(KHCISniffModeIoctlName,					"KHCISniffModeIoctl");
	_LIT(KHCIExitSniffModeIoctlName,				"KHCIExitSniffModeIoctl");
	_LIT(KHCIParkModeIoctlName,						"KHCIParkModeIoctl");
	_LIT(KHCIExitParkModeIoctlName,					"KHCIExitParkModeIoctl");
	_LIT(KHCIReadPageTimeoutIoctlName,				"KHCIReadPageTimeoutIoctl");
	_LIT(KHCIWritePageTimeoutIoctlName,				"KHCIWritePageTimeoutIoctl");
	_LIT(KHCIReadScanEnableIoctlName,				"KHCIReadScanEnableIoctl");
	_LIT(KHCIWriteScanEnableIoctlName,				"KHCIWriteScanEnableIoctl");
	_LIT(KHCIReadDeviceClassIoctlName,				"KHCIReadDeviceClassIoctl");
	_LIT(KHCIWriteDeviceClassIoctlName,				"KHCIWriteDeviceClassIoctl");
	_LIT(KHCIReadVoiceSettingIoctlName,				"KHCIReadVoiceSettingIoctl");
	_LIT(KHCIWriteVoiceSettingIoctlName,			"KHCIWriteVoiceSettingIoctl");
	_LIT(KHCIReadHoldModeActivityIoctlName,			"KHCIReadHoldModeActivityIoctl");
	_LIT(KHCIWriteHoldModeActivityIoctlName,		"KHCIWriteHoldModeActivityIoctl");
	_LIT(KHCILocalVersionIoctlName,					"KHCILocalVersionIoctl");
	_LIT(KHCILocalFeaturesIoctlName,				"KHCILocalFeaturesIoctl");
	_LIT(KHCICountryCodeIoctlName,					"KHCICountryCodeIoctl");
	_LIT(KHCILocalAddressIoctlName,					"KHCILocalAddressIoctl");
	_LIT(KHCIWriteDiscoverabilityIoctlName,			"KHCIWriteDiscoverabilityIoctl");
	_LIT(KHCIReadDiscoverabilityIoctlName,			"KHCIReadDiscoverabilityIoctl");
	_LIT(KHCIReadAuthenticationEnableIoctlName,		"KHCIReadAuthenticationEnableIoctl");
	_LIT(KHCIWriteAuthenticationEnableIoctlName,	"KHCIWriteAuthenticationEnableIoctl");
	_LIT(KL2CAPPassiveAutoBindName,					"KL2CAPPassiveAutoBind");
	_LIT(KRFCOMMModemStatusCmdIoctlName,			"KRFCOMMModemStatusCmdIoctl");
	_LIT(KRFCOMMRemoteLineStatusCmdIoctlName,		"KRFCOMMRemoteLineStatusCmdIoctl");
	_LIT(KRFCOMMRemotePortNegCmdIoctlName,			"KRFCOMMRemotePortNegCmdIoctl");
	_LIT(KRFCOMMRemotePortNegRequestIoctlName,		"KRFCOMMRemotePortNegRequestIoctl");
	_LIT(KRFCOMMConfigChangeIndicationIoctlName,	"KRFCOMMConfigChangeIndicationIoctl");
	_LIT(KRFCOMMLocalPortParameterName,				"KRFCOMMLocalPortParameter");
	_LIT(KRFCOMMGetAvailableServerChannelName,		"KRFCOMMGetAvailableServerChannel");
	_LIT(KRFCOMMMaximumSupportedMTUName,			"KRFCOMMMaximumSupportedMTU");
	_LIT(KRFCOMMGetDebug1Name,						"KRFCOMMGetDebug1");
	_LIT(KRFCOMMGetRemoteModemStatusName,			"KRFCOMMGetRemoteModemStatus");
	_LIT(KRFCOMMGetTransmitCreditName,				"KRFCOMMGetTransmitCredit");
	_LIT(KRFCOMMGetReceiveCreditName,				"KRFCOMMGetReceiveCredit");
	_LIT(KRFCOMMGetReUsedCountName,					"KRFCOMMGetReUsedCount");
	_LIT(KRFCOMMFlowTypeCBFCName,					"KRFCOMMFlowTypeCBFC");
	_LIT(KRFCOMMLocalModemStatusName,				"KRFCOMMLocalModemStatus");
	_LIT(KErrBadHandleName,							"KErrBadHandle");
	_LIT(KZero,										"Zero");
	_LIT(KOne,										"One");
	_LIT(ELMOutboundACLSizeName,					"ELMOutboundACLSize");
	_LIT(ELMInboundACLSizeName,						"ELMInboundACLSize");
	_LIT(KLMGetACLHandleName,						"KLMGetACLHandle");
	_LIT(KLMGetACLLinkCountName,					"KLMGetACLLinkCount");
	_LIT(KLMGetACLLinkArrayName,					"KLMGetACLLinkArray");
	_LIT(KLMSetBasebandConnectionPolicyName,		"KLMSetBasebandConnectionPolicy");
	_LIT(KLMGetBasebandHandleName,					"KLMGetBasebandHandle");
	_LIT(EBBSubscribePhysicalLinkName,				"EBBSubscribePhysicalLink");
	_LIT(EBBBeginRawName,							"EBBBeginRaw");
	_LIT(EBBRequestRoleMasterName,					"EBBRequestRoleMaster");
	_LIT(EBBRequestRoleSlaveName,					"EBBRequestRoleSlave");
	_LIT(EBBCancelModeRequestName,					"EBBCancelModeRequest");
	_LIT(EBBRequestSniffName,						"EBBRequestSniff");
	_LIT(EBBRequestParkName,						"EBBRequestPark");
	_LIT(EBBRequestPreventRoleChangeName,			"EBBRequestPreventRoleChange");
	_LIT(EBBRequestAllowRoleChangeName,				"EBBRequestAllowRoleChange");
	_LIT(EBBRequestChangeSupportedPacketTypesName,	"EBBRequestChangeSupportedPacketTypes");
	_LIT(EBBEnumeratePhysicalLinksName,				"EBBEnumeratePhysicalLinks");
	_LIT(EBBGetPhysicalLinkStateName,				"EBBGetPhysicalLinkState");
	_LIT(EBBGetSniffIntervalName,					"EBBGetSniffInterval");
	_LIT(EBBRequestLinkAuthenticationName,			"EBBRequestLinkAuthentication");
	_LIT(EBBRequestPreventSniffName,				"EBBRequestPreventSniff");
	_LIT(EBBRequestPreventHoldName,					"EBBRequestPreventHold");
	_LIT(EBBRequestPreventParkName,					"EBBRequestPreventPark");
	_LIT(EBBRequestPreventAllLowPowerModesName,		"EBBRequestPreventAllLowPowerModes");
	_LIT(EBBRequestAllowSniffName,					"EBBRequestAllowSniff");
	_LIT(EBBRequestAllowHoldName,					"EBBRequestAllowHold");
	_LIT(EBBRequestAllowParkName,					"EBBRequestAllowPark");
	_LIT(EBBRequestAllowAllLowPowerModesName,		"EBBRequestAllowAllLowPowerModes");
	_LIT(KBTRegisterCodServiceName,					"KBTRegisterCodService");
	_LIT(KBTSecurityDeviceOverrideName,				"KBTSecurityDeviceOverride");
	_LIT(KESockDefaultMessageSlotsName,				"KESockDefaultMessageSlots");
	_LIT(KUseEmbeddedUniqueIdName,					"KUseEmbeddedUniqueId");
	_LIT(KConnProgressDefaultName,					"KConnProgressDefault");
	_LIT(KMaxSockAddrSizeName,						"KMaxSockAddrSize");
	_LIT(KAFUnspecName,								"KAFUnspec");
	_LIT(KSockStreamName,							"KSockStream");
	_LIT(KSockDatagramName,							"KSockDatagram");
	_LIT(KSockSeqPacketName,						"KSockSeqPacket");
	_LIT(KSockRawName,								"KSockRaw");
	_LIT(KSOLSocketName,							"KSOLSocket");
	_LIT(KLevelUnspecifiedName,						"KLevelUnspecified");
	_LIT(KSODebugName,								"KSODebug");
	_LIT(KSORecvBufName,							"KSORecvBuf");
	_LIT(KSOSendBufName,							"KSOSendBuf");
	_LIT(KSONonBlockingIOName,						"KSONonBlockingIO");
	_LIT(KSOBlockingIOName,							"KSOBlockingIO");
	_LIT(KSOSelectPollName,							"KSOSelectPoll");
	_LIT(KSOReadBytesPendingName,					"KSOReadBytesPending");
	_LIT(KSOUrgentDataOffsetName,					"KSOUrgentDataOffset");
	_LIT(KSOSelectLastErrorName,					"KSOSelectLastError");
	_LIT(KSOEnableTransferName,						"KSOEnableTransfer");
	_LIT(KSODisableTransferName,					"KSODisableTransfer");
	_LIT(KSocketBufSizeUndefinedName,				"KSocketBufSizeUndefined");
	_LIT(KSocketDefaultBufferSizeName,				"KSocketDefaultBufferSize");
	_LIT(KSocketInternalOptionBitName,				"KSocketInternalOptionBit");
	_LIT(KIOctlSelectName,							"KIOctlSelect");
	_LIT(KSockSelectReadName,						"KSockSelectRead");
	_LIT(KSockSelectWriteName,						"KSockSelectWrite");
	_LIT(KSockSelectExceptName,						"KSockSelectExcept");
	_LIT(KSockSelectReadContinuationName,			"KSockSelectReadContinuation");
	_LIT(KSockWriteUrgentName,						"KSockWriteUrgent");
	_LIT(KSocketInternalWriteBitName,				"KSocketInternalWriteBit");
	_LIT(KSockWriteSystemMaskName,					"KSockWriteSystemMask");
	_LIT(KSockReadPeekName,							"KSockReadPeek");
	_LIT(KSocketInternalReadBitName,				"KSocketInternalReadBit");
	_LIT(KSockReadContinuationName,					"KSockReadContinuation");
	_LIT(KSIConnectionLessName,						"KSIConnectionLess");
	_LIT(KSIReliableName,							"KSIReliable");
	_LIT(KSIInOrderName,							"KSIInOrder");
	_LIT(KSIMessageBasedName,						"KSIMessageBased");
	_LIT(KSIDatagramName,							"KSIDatagram");
	_LIT(KSIStreamBasedName,						"KSIStreamBased");
	_LIT(KSIPseudoStreamName,						"KSIPseudoStream");
	_LIT(KSIUrgentDataName,							"KSIUrgentData");
	_LIT(KSIConnectDataName,						"KSIConnectData");
	_LIT(KSIDisconnectDataName,						"KSIDisconnectData");
	_LIT(KSIBroadcastName,							"KSIBroadcast");
	_LIT(KSIMultiPointName,							"KSIMultiPoint");
	_LIT(KSIQOSName,								"KSIQOS");
	_LIT(KSIWriteOnlyName,							"KSIWriteOnly");
	_LIT(KSIReadOnlyName,							"KSIReadOnly");
	_LIT(KSIGracefulCloseName,						"KSIGracefulClose");
	_LIT(KSICanReconnectName,						"KSICanReconnect");
	_LIT(KSIPeekDataName,							"KSIPeekData");
	_LIT(KSIRequiresOwnerInfoName,					"KSIRequiresOwnerInfo");
	_LIT(KNSNameResolutionName,						"KNSNameResolution");
	_LIT(KNSHeirarchicalNamingName,					"KNSHeirarchicalNaming");
	_LIT(KNSRemoteDiscoveryName,					"KNSRemoteDiscovery");
	_LIT(KNSServiceResolutionName,					"KNSServiceResolution");
	_LIT(KNSNameRegistrationName,					"KNSNameRegistration");
	_LIT(KNSServiceRegistrationName,				"KNSServiceRegistration");
	_LIT(KNSDynamicAddressingName,					"KNSDynamicAddressing");
	_LIT(KNSInfoDatabaseName,						"KNSInfoDatabase");
	_LIT(KNSRequiresConnectionStartupName,			"KNSRequiresConnectionStartup");
	_LIT(KSocketNoSecurityName,						"KSocketNoSecurity");
	_LIT(KSecureSocketsName,						"KSecureSockets");
	_LIT(KSocketMessageSizeIsStreamName,			"KSocketMessageSizeIsStream");
	_LIT(KSocketMessageSizeUndefinedName,			"KSocketMessageSizeUndefined");
	_LIT(KSocketMessageSizeNoLimitName,				"KSocketMessageSizeNoLimit");
	_LIT(KUndefinedSockTypeName,					"KUndefinedSockType");
	_LIT(KUndefinedProtocolName,					"KUndefinedProtocol");
	_LIT(KErrWouldBlockName,						"KErrWouldBlock");
	_LIT(KErrConnectionTerminatedName,				"KErrConnectionTerminated");
	_LIT(KErrCannotFindProtocolName,				"KErrCannotFindProtocol");
	_LIT(KCOLConnectionName,						"KCOLConnection");
	_LIT(KCOLProviderName,							"KCOLProvider");
	_LIT(KConnInternalOptionBitName,				"KConnInternalOptionBit");
	_LIT(KConnWriteUserDataBitName,					"KConnWriteUserDataBit");
	_LIT(KConnReadUserDataBitName,					"KConnReadUserDataBit");
	_LIT(KCoEnumerateConnectionClientsName,			"KCoEnumerateConnectionClients");
	_LIT(KCoGetConnectionClientInfoName,			"KCoGetConnectionClientInfo");
	_LIT(KCoGetConnectionSocketInfoName,			"KCoGetConnectionSocketInfo");
	_LIT(KConnectionTypeDefaultName,				"KConnectionTypeDefault");
	_LIT(KSubConnParamsInterfaceUidName,			"KSubConnParamsInterfaceUid");
	_LIT(KSubConnEventInterfaceUidName,				"KSubConnEventInterfaceUid");
	_LIT(KSubConnGenericParamsImplUidName,			"KSubConnGenericParamsImplUid");
	_LIT(KSubConnGenericEventsImplUidName,			"KSubConnGenericEventsImplUid");
	_LIT(KSubConGlobalFamilyName,					"KSubConGlobalFamily");
	_LIT(KSubConQoSFamilyName,						"KSubConQoSFamily");
	_LIT(KSubConAuthorisationFamilyName,			"KSubConAuthorisationFamily");
	_LIT(KNotificationEventMaxSizeName,				"KNotificationEventMaxSize");

	// BT Address Family
	if (aConstantName==KBTAddrFamilyName)
		{
		aValue=KBTAddrFamily;
		}
	else if (aConstantName==KBTLinkManagerName)
		{
		aValue=KBTLinkManager;
		}
	else if (aConstantName==KL2CAPName)
		{
		aValue=KL2CAP;
		}
	else if (aConstantName==KRFCOMMName)
		{
		aValue=KRFCOMM;
		}
	else if (aConstantName==KSDPName)
		{
		aValue=KSDP;
		}
	else if (aConstantName==KAVCTPName)
		{
		aValue=KAVCTP;
		}
	else if (aConstantName==KTCIL2CAPName)
		{
		aValue=KTCIL2CAP;
		}
	else if (aConstantName==KBTMajorName)
		{
		aValue=KBTMajor;
		}
	else if (aConstantName==KBTMinorName)
		{
		aValue=KBTMinor;
		}
	else if (aConstantName==KBTBuildName)
		{
		aValue=KBTBuild;
		}
	else if (aConstantName==KSolBtBlogName)
		{
		aValue=KSolBtBlog;
		}
	else if (aConstantName==KSolBtHCIName)
		{
		aValue=KSolBtHCI;
		}
	else if (aConstantName==KSolBtLMName)
		{
		aValue=KSolBtLM;
		}
	else if (aConstantName==KSolBtL2CAPName)
		{
		aValue=KSolBtL2CAP;
		}
	else if (aConstantName==KSolBtRFCOMMName)
		{
		aValue=KSolBtRFCOMM;
		}
	else if (aConstantName==KSolBtAVCTPName)
		{
		aValue=KSolBtAVCTP;
		}
	else if (aConstantName==KSolBtACLName)
		{
		aValue=KSolBtACL;
		}
	else if (aConstantName==KSolBtAVDTPSignallingName)
		{
		aValue=KSolBtAVDTPSignalling;
		}
	else if (aConstantName==KSolBtAVDTPMediaName)
		{
		aValue=KSolBtAVDTPMedia;
		}
	else if (aConstantName==KSolBtAVDTPReportingName)
		{
		aValue=KSolBtAVDTPReporting;
		}
	else if (aConstantName==KSolBtAVDTPRecoveryName)
		{
		aValue=KSolBtAVDTPRecovery;
		}
	else if (aConstantName==KSolBtAVDTPInternalName)
		{
		aValue=KSolBtAVDTPInternal;
		}
	else if (aConstantName==KSolBtLMProxyName)
		{
		aValue=KSolBtLMProxy;
		}
	else if (aConstantName==KSolBtSAPBaseName)
		{
		aValue=KSolBtSAPBase;
		}
	else if (aConstantName==KSCOListenQueSizeName)
		{
		aValue=KSCOListenQueSize;
		}
	else if (aConstantName==KL2MinMTUName)
		{
		aValue=KL2MinMTU;
		}
	else if (aConstantName==KMaxPSMName)
		{
		aValue=KMaxPSM;
		}
	else if (aConstantName==KMinUserPSMName)
		{
		aValue=KMinUserPSM;
		}
	else if (aConstantName==KMinPSMName)
		{
		aValue=KMinPSM;
		}
	else if (aConstantName==KHostResInquiryName)
		{
		aValue=KHostResInquiry;
		}
	else if (aConstantName==KHostResNameName)
		{
		aValue=KHostResName;
		}
	else if (aConstantName==KHostResIgnoreCacheName)
		{
		aValue=KHostResIgnoreCache;
		}
	else if (aConstantName==KHostResCacheName)
		{
		aValue=KHostResCache;
		}
	else if (aConstantName==KL2CAPEchoRequestIoctlName)
		{
		aValue=KL2CAPEchoRequestIoctl;
		}
	else if (aConstantName==KL2CAPIncomingMTUIoctlName)
		{
		aValue=KL2CAPIncomingMTUIoctl;
		}
	else if (aConstantName==KL2CAPOutgoingMTUIoctlName)
		{
		aValue=KL2CAPOutgoingMTUIoctl;
		}
	else if (aConstantName==KL2CAPUpdateChannelConfigIoctlName)
		{
		aValue=KL2CAPUpdateChannelConfigIoctl;
		}
	else if (aConstantName==KDefaultBasebandConnectionTimeoutName)
		{
		aValue=KDefaultBasebandConnectionTimeout;
		}
	else if (aConstantName==KLMAddSCOConnIoctlName)
		{
		aValue=KLMAddSCOConnIoctl;
		}
	else if (aConstantName==KHCIRemoveSCOConnIoctlName)
		{
		aValue=KHCIRemoveSCOConnIoctl;
		}
	else if (aConstantName==KHCIChangePacketTypeIoctlName)
		{
		aValue=KHCIChangePacketTypeIoctl;
		}
	else if (aConstantName==KHCIAuthRequestIoctlName)
		{
		aValue=KHCIAuthRequestIoctl;
		}
	else if (aConstantName==KHCIEncryptIoctlName)
		{
		aValue=KHCIEncryptIoctl;
		}
	else if (aConstantName==KHCIChangeLinkKeyIoctlName)
		{
		aValue=KHCIChangeLinkKeyIoctl;
		}
	else if (aConstantName==KHCIMasterLinkKeyIoctlName)
		{
		aValue=KHCIMasterLinkKeyIoctl;
		}
	else if (aConstantName==KHCIHoldModeIoctlName)
		{
		aValue=KHCIHoldModeIoctl;
		}
	else if (aConstantName==KHCISniffModeIoctlName)
		{
		aValue=KHCISniffModeIoctl;
		}
	else if (aConstantName==KHCIExitSniffModeIoctlName)
		{
		aValue=KHCIExitSniffModeIoctl;
		}
	else if (aConstantName==KHCIParkModeIoctlName)
		{
		aValue=KHCIParkModeIoctl;
		}
	else if (aConstantName==KHCIExitParkModeIoctlName)
		{
		aValue=KHCIExitParkModeIoctl;
		}
	else if (aConstantName==KHCIReadPageTimeoutIoctlName)
		{
		aValue=KHCIReadPageTimeoutIoctl;
		}
	else if (aConstantName==KHCIWritePageTimeoutIoctlName)
		{
		aValue=KHCIWritePageTimeoutIoctl;
		}
	else if (aConstantName==KHCIReadScanEnableIoctlName)
		{
		aValue=KHCIReadScanEnableIoctl;
		}
	else if (aConstantName==KHCIWriteScanEnableIoctlName)
		{
		aValue=KHCIWriteScanEnableIoctl;
		}
	else if (aConstantName==KHCIReadDeviceClassIoctlName)
		{
		aValue=KHCIReadDeviceClassIoctl;
		}
	else if (aConstantName==KHCIWriteDeviceClassIoctlName)
		{
		aValue=KHCIWriteDeviceClassIoctl;
		}
	else if (aConstantName==KHCIReadVoiceSettingIoctlName)
		{
		aValue=KHCIReadVoiceSettingIoctl;
		}
	else if (aConstantName==KHCIWriteVoiceSettingIoctlName)
		{
		aValue=KHCIWriteVoiceSettingIoctl;
		}
	else if (aConstantName==KHCIReadHoldModeActivityIoctlName)
		{
		aValue=KHCIReadHoldModeActivityIoctl;
		}
	else if (aConstantName==KHCIWriteHoldModeActivityIoctlName)
		{
		aValue=KHCIWriteHoldModeActivityIoctl;
		}
	else if (aConstantName==KHCILocalVersionIoctlName)
		{
		aValue=KHCILocalVersionIoctl;
		}
	else if (aConstantName==KHCILocalFeaturesIoctlName)
		{
		aValue=KHCILocalFeaturesIoctl;
		}
	else if (aConstantName==KHCICountryCodeIoctlName)
		{
		aValue=KHCICountryCodeIoctl;
		}
	else if (aConstantName==KHCILocalAddressIoctlName)
		{
		aValue=KHCILocalAddressIoctl;
		}
	else if (aConstantName==KHCIWriteDiscoverabilityIoctlName)
		{
		aValue=KHCIWriteDiscoverabilityIoctl;
		}
	else if (aConstantName==KHCIReadDiscoverabilityIoctlName)
		{
		aValue=KHCIReadDiscoverabilityIoctl;
		}
	else if (aConstantName==KHCIReadAuthenticationEnableIoctlName)
		{
		aValue=KHCIReadAuthenticationEnableIoctl;
		}
	else if (aConstantName==KHCIWriteAuthenticationEnableIoctlName)
		{
		aValue=KHCIWriteAuthenticationEnableIoctl;
		}
	else if (aConstantName==KL2CAPPassiveAutoBindName)
		{
		aValue=KL2CAPPassiveAutoBind;
		}
	else if (aConstantName==KRFCOMMModemStatusCmdIoctlName)
		{
		aValue=KRFCOMMModemStatusCmdIoctl;
		}
	else if (aConstantName==KRFCOMMRemoteLineStatusCmdIoctlName)
		{
		aValue=KRFCOMMRemoteLineStatusCmdIoctl;
		}
	else if (aConstantName==KRFCOMMRemotePortNegCmdIoctlName)
		{
		aValue=KRFCOMMRemotePortNegCmdIoctl;
		}
	else if (aConstantName==KRFCOMMRemotePortNegRequestIoctlName)
		{
		aValue=KRFCOMMRemotePortNegRequestIoctl;
		}
	else if (aConstantName==KRFCOMMConfigChangeIndicationIoctlName)
		{
		aValue=KRFCOMMConfigChangeIndicationIoctl;
		}
	else if (aConstantName==KRFCOMMLocalPortParameterName)
		{
		aValue=KRFCOMMLocalPortParameter;
		}
	else if (aConstantName==KRFCOMMGetAvailableServerChannelName)
		{
		aValue=KRFCOMMGetAvailableServerChannel;
		}
	else if (aConstantName==KRFCOMMMaximumSupportedMTUName)
		{
		aValue=KRFCOMMMaximumSupportedMTU;
		}
	else if (aConstantName==KRFCOMMGetDebug1Name)
		{
		aValue=KRFCOMMGetDebug1;
		}
	else if (aConstantName==KRFCOMMGetRemoteModemStatusName)
		{
		aValue=KRFCOMMGetRemoteModemStatus;
		}
	else if (aConstantName==KRFCOMMGetTransmitCreditName)
		{
		aValue=KRFCOMMGetTransmitCredit;
		}
	else if (aConstantName==KRFCOMMGetReceiveCreditName)
		{
		aValue=KRFCOMMGetReceiveCredit;
		}
	else if (aConstantName==KRFCOMMGetReUsedCountName)
		{
		aValue=KRFCOMMGetReUsedCount;
		}
	else if (aConstantName==KRFCOMMFlowTypeCBFCName)
		{
		aValue=KRFCOMMFlowTypeCBFC;
		}
	else if (aConstantName==KRFCOMMLocalModemStatusName)
		{
		aValue=KRFCOMMLocalModemStatus;
		}
	else if (aConstantName==KErrBadHandleName)
		{
		aValue=KErrBadHandle;
		}
	else if (aConstantName==KZero)
		{
		aValue=0;
		}
	else if (aConstantName==KOne)
		{
		aValue=1;
		}
	else if (aConstantName==ELMOutboundACLSizeName)
		{
		aValue=ELMOutboundACLSize;
		}
	else if (aConstantName==ELMInboundACLSizeName)
		{
		aValue=ELMInboundACLSize;
		}
	else if (aConstantName==KLMGetACLHandleName)
		{
		aValue=KLMGetACLHandle;
		}
	else if (aConstantName==KLMGetACLLinkCountName)
		{
		aValue=KLMGetACLLinkCount;
		}
	else if (aConstantName==KLMGetACLLinkArrayName)
		{
		aValue=KLMGetACLLinkArray;
		}
	else if (aConstantName==KLMSetBasebandConnectionPolicyName)
		{
		aValue=KLMSetBasebandConnectionPolicy;
		}
	else if (aConstantName==KLMGetBasebandHandleName)
		{
		aValue=KLMGetBasebandHandle;
		}
	else if (aConstantName==EBBSubscribePhysicalLinkName)
		{
		aValue=EBBSubscribePhysicalLink;
		}
	else if (aConstantName==EBBBeginRawName)
		{
		aValue=EBBBeginRaw;
		}
	else if (aConstantName==EBBRequestRoleMasterName)
		{
		aValue=EBBRequestRoleMaster;
		}
	else if (aConstantName==EBBRequestRoleSlaveName)
		{
		aValue=EBBRequestRoleSlave;
		}
	else if (aConstantName==EBBCancelModeRequestName)
		{
		aValue=EBBCancelModeRequest;
		}
	else if (aConstantName==EBBRequestSniffName)
		{
		aValue=EBBRequestSniff;
		}
	else if (aConstantName==EBBRequestParkName)
		{
		aValue=EBBRequestPark;
		}
	else if (aConstantName==EBBRequestPreventRoleChangeName)
		{
		aValue=EBBRequestPreventRoleChange;
		}
	else if (aConstantName==EBBRequestAllowRoleChangeName)
		{
		aValue=EBBRequestAllowRoleChange;
		}
	else if (aConstantName==EBBRequestChangeSupportedPacketTypesName)
		{
		aValue=EBBRequestChangeSupportedPacketTypes;
		}
	else if (aConstantName==EBBEnumeratePhysicalLinksName)
		{
		aValue=EBBEnumeratePhysicalLinks;
		}
	else if (aConstantName==EBBGetPhysicalLinkStateName)
		{
		aValue=EBBGetPhysicalLinkState;
		}
	else if (aConstantName==EBBGetSniffIntervalName)
		{
		aValue=EBBGetSniffInterval;
		}
	else if (aConstantName==EBBRequestLinkAuthenticationName)
		{
		aValue=EBBRequestLinkAuthentication;
		}
	else if (aConstantName==EBBRequestPreventSniffName)
		{
		aValue=EBBRequestPreventSniff;
		}
	else if (aConstantName==EBBRequestPreventHoldName)
		{
		aValue=EBBRequestPreventHold;
		}
	else if (aConstantName==EBBRequestPreventParkName)
		{
		aValue=EBBRequestPreventPark;
		}
	else if (aConstantName==EBBRequestPreventAllLowPowerModesName)
		{
		aValue=EBBRequestPreventAllLowPowerModes;
		}
	else if (aConstantName==EBBRequestAllowSniffName)
		{
		aValue=EBBRequestAllowSniff;
		}
	else if (aConstantName==EBBRequestAllowHoldName)
		{
		aValue=EBBRequestAllowHold;
		}
	else if (aConstantName==EBBRequestAllowParkName)
		{
		aValue=EBBRequestAllowPark;
		}
	else if (aConstantName==EBBRequestAllowAllLowPowerModesName)
		{
		aValue=EBBRequestAllowAllLowPowerModes;
		}
	else if (aConstantName==KBTRegisterCodServiceName)
		{
		aValue=KBTRegisterCodService;
		}
	else if (aConstantName==KBTSecurityDeviceOverrideName)
		{
		aValue=KBTSecurityDeviceOverride;
		}
	else if (aConstantName==KESockDefaultMessageSlotsName)
		{
		aValue=KESockDefaultMessageSlots;
		}
	else if (aConstantName==KUseEmbeddedUniqueIdName)
		{
		aValue=KUseEmbeddedUniqueId;
		}
	else if (aConstantName==KConnProgressDefaultName)
		{
		aValue=KConnProgressDefault;
		}
	else if (aConstantName==KMaxSockAddrSizeName)
		{
		aValue=KMaxSockAddrSize;
		}
	else if (aConstantName==KAFUnspecName)
		{
		aValue=KAFUnspec;
		}
	else if (aConstantName==KSockStreamName)
		{
		aValue=KSockStream;
		}
	else if (aConstantName==KSockDatagramName)
		{
		aValue=KSockDatagram;
		}
	else if (aConstantName==KSockSeqPacketName)
		{
		aValue=KSockSeqPacket;
		}
	else if (aConstantName==KSockRawName)
		{
		aValue=KSockRaw;
		}
	else if (aConstantName==KSOLSocketName)
		{
		aValue=KSOLSocket;
		}
	else if (aConstantName==KLevelUnspecifiedName)
		{
		aValue=KLevelUnspecified;
		}
	else if (aConstantName==KSODebugName)
		{
		aValue=KSODebug;
		}
	else if (aConstantName==KSORecvBufName)
		{
		aValue=KSORecvBuf;
		}
	else if (aConstantName==KSOSendBufName)
		{
		aValue=KSOSendBuf;
		}
	else if (aConstantName==KSONonBlockingIOName)
		{
		aValue=KSONonBlockingIO;
		}
	else if (aConstantName==KSOBlockingIOName)
		{
		aValue=KSOBlockingIO;
		}
	else if (aConstantName==KSOSelectPollName)
		{
		aValue=KSOSelectPoll;
		}
	else if (aConstantName==KSOReadBytesPendingName)
		{
		aValue=KSOReadBytesPending;
		}
	else if (aConstantName==KSOUrgentDataOffsetName)
		{
		aValue=KSOUrgentDataOffset;
		}
	else if (aConstantName==KSOSelectLastErrorName)
		{
		aValue=KSOSelectLastError;
		}
	else if (aConstantName==KSOEnableTransferName)
		{
		aValue=KSOEnableTransfer;
		}
	else if (aConstantName==KSODisableTransferName)
		{
		aValue=KSODisableTransfer;
		}
	else if (aConstantName==KSocketBufSizeUndefinedName)
		{
		aValue=KSocketBufSizeUndefined;
		}
	else if (aConstantName==KSocketDefaultBufferSizeName)
		{
		aValue=KSocketDefaultBufferSize;
		}
	else if (aConstantName==KSocketInternalOptionBitName)
		{
		aValue=KSocketInternalOptionBit;
		}
	else if (aConstantName==KIOctlSelectName)
		{
		aValue=KIOctlSelect;
		}
	else if (aConstantName==KSockSelectReadName)
		{
		aValue=KSockSelectRead;
		}
	else if (aConstantName==KSockSelectWriteName)
		{
		aValue=KSockSelectWrite;
		}
	else if (aConstantName==KSockSelectExceptName)
		{
		aValue=KSockSelectExcept;
		}
	else if (aConstantName==KSockSelectReadContinuationName)
		{
		aValue=KSockSelectReadContinuation;
		}
	else if (aConstantName==KSockWriteUrgentName)
		{
		aValue=KSockWriteUrgent;
		}
	else if (aConstantName==KSocketInternalWriteBitName)
		{
		aValue=KSocketInternalWriteBit;
		}
	else if (aConstantName==KSockWriteSystemMaskName)
		{
		aValue=KSockWriteSystemMask;
		}
	else if (aConstantName==KSockReadPeekName)
		{
		aValue=KSockReadPeek;
		}
	else if (aConstantName==KSocketInternalReadBitName)
		{
		aValue=KSocketInternalReadBit;
		}
	else if (aConstantName==KSockReadContinuationName)
		{
		aValue=KSockReadContinuation;
		}
	else if (aConstantName==KSIConnectionLessName)
		{
		aValue=KSIConnectionLess;
		}
	else if (aConstantName==KSIReliableName)
		{
		aValue=KSIReliable;
		}
	else if (aConstantName==KSIInOrderName)
		{
		aValue=KSIInOrder;
		}
	else if (aConstantName==KSIMessageBasedName)
		{
		aValue=KSIMessageBased;
		}
	else if (aConstantName==KSIDatagramName)
		{
		aValue=KSIDatagram;
		}
	else if (aConstantName==KSIStreamBasedName)
		{
		aValue=KSIStreamBased;
		}
	else if (aConstantName==KSIPseudoStreamName)
		{
		aValue=KSIPseudoStream;
		}
	else if (aConstantName==KSIUrgentDataName)
		{
		aValue=KSIUrgentData;
		}
	else if (aConstantName==KSIConnectDataName)
		{
		aValue=KSIConnectData;
		}
	else if (aConstantName==KSIDisconnectDataName)
		{
		aValue=KSIDisconnectData;
		}
	else if (aConstantName==KSIBroadcastName)
		{
		aValue=KSIBroadcast;
		}
	else if (aConstantName==KSIMultiPointName)
		{
		aValue=KSIMultiPoint;
		}
	else if (aConstantName==KSIQOSName)
		{
		aValue=KSIQOS;
		}
	else if (aConstantName==KSIWriteOnlyName)
		{
		aValue=KSIWriteOnly;
		}
	else if (aConstantName==KSIReadOnlyName)
		{
		aValue=KSIReadOnly;
		}
	else if (aConstantName==KSIGracefulCloseName)
		{
		aValue=KSIGracefulClose;
		}
	else if (aConstantName==KSICanReconnectName)
		{
		aValue=KSICanReconnect;
		}
	else if (aConstantName==KSIPeekDataName)
		{
		aValue=KSIPeekData;
		}
	else if (aConstantName==KSIRequiresOwnerInfoName)
		{
		aValue=KSIRequiresOwnerInfo;
		}
	else if (aConstantName==KNSNameResolutionName)
		{
		aValue=KNSNameResolution;
		}
	else if (aConstantName==KNSHeirarchicalNamingName)
		{
		aValue=KNSHeirarchicalNaming;
		}
	else if (aConstantName==KNSRemoteDiscoveryName)
		{
		aValue=KNSRemoteDiscovery;
		}
	else if (aConstantName==KNSServiceResolutionName)
		{
		aValue=KNSServiceResolution;
		}
	else if (aConstantName==KNSNameRegistrationName)
		{
		aValue=KNSNameRegistration;
		}
	else if (aConstantName==KNSServiceRegistrationName)
		{
		aValue=KNSServiceRegistration;
		}
	else if (aConstantName==KNSDynamicAddressingName)
		{
		aValue=KNSDynamicAddressing;
		}
	else if (aConstantName==KNSInfoDatabaseName)
		{
		aValue=KNSInfoDatabase;
		}
	else if (aConstantName==KNSRequiresConnectionStartupName)
		{
		aValue=KNSRequiresConnectionStartup;
		}
	else if (aConstantName==KSocketNoSecurityName)
		{
		aValue=KSocketNoSecurity;
		}
	else if (aConstantName==KSecureSocketsName)
		{
		aValue=KSecureSockets;
		}
	else if (aConstantName==KSocketMessageSizeIsStreamName)
		{
		aValue=KSocketMessageSizeIsStream;
		}
	else if (aConstantName==KSocketMessageSizeUndefinedName)
		{
		aValue=KSocketMessageSizeUndefined;
		}
	else if (aConstantName==KSocketMessageSizeNoLimitName)
		{
		aValue=KSocketMessageSizeNoLimit;
		}
	else if (aConstantName==KUndefinedSockTypeName)
		{
		aValue=KUndefinedSockType;
		}
	else if (aConstantName==KUndefinedProtocolName)
		{
		aValue=KUndefinedProtocol;
		}
	else if (aConstantName==KErrWouldBlockName)
		{
		aValue=KErrWouldBlock;
		}
	else if (aConstantName==KErrConnectionTerminatedName)
		{
		aValue=KErrConnectionTerminated;
		}
	else if (aConstantName==KErrCannotFindProtocolName)
		{
		aValue=KErrCannotFindProtocol;
		}
	else if (aConstantName==KCOLConnectionName)
		{
		aValue=KCOLConnection;
		}
	else if (aConstantName==KCOLProviderName)
		{
		aValue=KCOLProvider;
		}
	else if (aConstantName==KConnInternalOptionBitName)
		{
		aValue=KConnInternalOptionBit;
		}
	else if (aConstantName==KConnWriteUserDataBitName)
		{
		aValue=KConnWriteUserDataBit;
		}
	else if (aConstantName==KConnReadUserDataBitName)
		{
		aValue=KConnReadUserDataBit;
		}
	else if (aConstantName==KCoEnumerateConnectionClientsName)
		{
		aValue=KCoEnumerateConnectionClients;
		}
	else if (aConstantName==KCoGetConnectionClientInfoName)
		{
		aValue=KCoGetConnectionClientInfo;
		}
	else if (aConstantName==KCoGetConnectionSocketInfoName)
		{
		aValue=KCoGetConnectionSocketInfo;
		}
	else if (aConstantName==KConnectionTypeDefaultName)
		{
		aValue=KConnectionTypeDefault;
		}
	else if (aConstantName==KSubConnParamsInterfaceUidName)
		{
		aValue=KSubConnParamsInterfaceUid;
		}
	else if (aConstantName==KSubConnEventInterfaceUidName)
		{
		aValue=KSubConnEventInterfaceUid;
		}
	else if (aConstantName==KSubConnGenericParamsImplUidName)
		{
		aValue=KSubConnGenericParamsImplUid;
		}
	else if (aConstantName==KSubConnGenericEventsImplUidName)
		{
		aValue=KSubConnGenericEventsImplUid;
		}
	else if (aConstantName==KSubConGlobalFamilyName)
		{
		aValue=KSubConGlobalFamily;
		}
	else if (aConstantName==KSubConQoSFamilyName)
		{
		aValue=KSubConQoSFamily;
		}
	else if (aConstantName==KSubConAuthorisationFamilyName)
		{
		aValue=KSubConAuthorisationFamily;
		}
	else if (aConstantName==KNotificationEventMaxSizeName)
		{
		aValue=KNotificationEventMaxSize;
		}
	else
		{
		ret=EFalse;
		aValue=-1;
		}

	return ret;
	}


TBool CT_BTUtil::ReadAvdtpServiceCategory(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpServiceCategory& aCategory)
	{
	// Read TAvdtpServiceCategory from ini file
	TPtrC	str;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, str);
	if ( ret )
		{
		if(str == KAvdtpServiceCategoryNull)
			{
			aCategory = EServiceCategoryNull;
			}
		else if(str == KAvdtpServiceCategoryMediaTransport)
			{
			aCategory = EServiceCategoryMediaTransport;
			}
		else if(str == KAvdtpServiceCategoryReporting)
			{
			aCategory = EServiceCategoryReporting;
			}
		else if(str == KAvdtpServiceCategoryRecovery)
			{
			aCategory = EServiceCategoryRecovery;
			}
		else if(str == KAvdtpServiceCategoryContentProtection)
			{
			aCategory = EServiceCategoryContentProtection;
			}
		else if(str == KAvdtpServiceCategoryHeaderCompression)
			{
			aCategory = EServiceCategoryHeaderCompression;
			}
		else if(str == KAvdtpServiceCategoryMultiplexing)
			{
			aCategory = EServiceCategoryMultiplexing;
			}
		else if(str == KAvdtpServiceCategoryMediaCodec)
			{
			aCategory = EServiceCategoryMediaCodec;
			}
		else if(str == KNumberOfAvdtpServiceCategories)
			{
			aCategory = ENumberOfServiceCategories;
			}
		else if(str == KALLAvdtpServiceCategories)
			{
			aCategory = EAllServiceCategories;
			}
		else
			{
    	    TInt	intVal=0;
        	ret=aDataWrapper.GetIntFromConfig(aSectName, aKeyName, intVal);
			if ( ret )
				{
				aCategory =(TAvdtpServiceCategory)intVal;
				}
			}
		}
	return ret;
	}

	TBool CT_BTUtil::ReadAvdtpRecoveryType(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TAvdtpRecoveryType& aRecoveryType)
		{
		// Read TAvdtpRecoveryType from ini file
		TPtrC	str;
		TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, str);
		if ( ret )
			{
			if(str == KAvdtpRecoveryTypeEForbidden)
				{
				aRecoveryType = SymbianBluetoothAV::EForbiddenRecovery;
				}
			else if(str == KAvdtpRecoveryTypeERFC2733)
				{
				aRecoveryType = SymbianBluetoothAV::ERFC2733Recovery;
				}
			else
				{
	    	    TInt	intVal=0;
	        	ret=aDataWrapper.GetIntFromConfig(aSectName, aKeyName, intVal);
				if ( ret )
					{
					aRecoveryType = (SymbianBluetoothAV::TAvdtpRecoveryType)intVal;
					}
				}
			}
		return ret;		
		}
	
	TBool CT_BTUtil::ReadAvdtpSignallingErrorCode(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TAvdtpSignallingErrorCode& aErrorCode)
		{
		// Read TAvdtpSignallingErrorCode from ini file
		TPtrC	str;
		TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, str);
		if ( ret )
			{
			if(str == KAvdtpBadHeaderFormat)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadHeaderFormat;
				}
			else if(str == KAvdtpBadLength)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadLength;
				}
			else if(str == KEAvdtpBadACPSEID)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadACPSEID;
				}
			else if(str == KAvdtpSEPInUse)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpSEPInUse;
				}
			else if(str == KAvdtpSepNotInUse)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpSepNotInUse;
				}
			else if(str == KAvdtpBadServCategory)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadServCategory;
				}
			else if(str == KAvdtpBadPayloadFormat)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadPayloadFormat;
				}
			else if(str == KAvdtpNotSupportedCommand)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpNotSupportedCommand;
				}
			else if(str == KAvdtpInvalidCapabilities)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpInvalidCapabilities;
				}
			else if(str == KAvdtpBadRecoveryType)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadRecoveryType;
				}
			else if(str == KAvdtpBadMediaTransportFormat)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadMediaTransportFormat;
				}
			else if(str == KAvdtpBadReportingFormat)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadReportingFormat;
				}
			else if(str == KAvdtpBadRecoveryFormat)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadRecoveryFormat;
				}
			else if(str == KAvdtpBadRohcFormat)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadRohcFormat;
				}
			else if(str == KAvdtpBadCpFormat)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadCpFormat;
				}
			else if(str == KAvdtpBadMultiplexingFormat)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadMultiplexingFormat;
				}
			else if(str == KAvdtpUnsupportedConfiguration)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpUnsupportedConfiguration;
				}
			else if(str == KAvdtpBadState)
				{
				aErrorCode = SymbianBluetoothAV::EAvdtpBadState;
				}
			else
				{
	    	    TInt	intVal=0;
	        	ret=aDataWrapper.GetIntFromConfig(aSectName, aKeyName, intVal);
				if ( ret )
					{
					aErrorCode = (SymbianBluetoothAV::TAvdtpSignallingErrorCode)intVal;
					}
				}
			}
		return ret;			
		}
	
	TBool CT_BTUtil::ReadGavdpSignallingErrorCode(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TGavdpSignallingErrorCode& aErrorCode)
		{
		// Read TGavdpSignallingErrorCode from ini file
		TPtrC	str;
		TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, str);
		if ( ret )
			{
			if(str == KGavdpBadService)
				{
				aErrorCode = SymbianBluetoothAV::EGavdpBadService;
				}
			else if(str == KGavdpInsufficientResource)
				{
				aErrorCode = SymbianBluetoothAV::EGavdpInsufficientResource;
				}
			else
				{
	    	    TInt	intVal=0;
	        	ret=aDataWrapper.GetIntFromConfig(aSectName, aKeyName, intVal);
				if ( ret )
					{
					aErrorCode = (SymbianBluetoothAV::TGavdpSignallingErrorCode)intVal;
					}
				}
			}
		return ret;		
		}
	
	TBool CT_BTUtil::ReadA2dpSignallingErrorCode(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TA2dpSignallingErrorCode& aErrorCode)
		{
		// Read TA2dpSignallingErrorCode from ini file
		TPtrC	str;
		TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, str);
		if ( ret )
			{
			if(str == KA2dpInvalidCodec)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidCodec;
				}
			else if(str == KA2dpNotSupportedCodec)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedCodec;
				}
			else if(str == KA2dpInvalidSamplingFrequency)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidSamplingFrequency;
				}
			else if(str == KA2dpNotSupportedSamplingFrequency)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedSamplingFrequency;
				}
			else if(str == KA2dpInvalidChannelMode)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidChannelMode;
				}
			else if(str == KA2dpNotSupportedChannelMode)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedChannelMode;
				}
			else if(str == KA2dpInvalidSubbands)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidSubbands;
				}
			else if(str == KA2dpNotSupportedSubbands)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedSubbands;
				}
			else if(str == KA2dpInvalidAllocationMethod)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidAllocationMethod;
				}
			else if(str == KA2dpNotSupportedAllocationMethod)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedAllocationMethod;
				}
			else if(str == KA2dpInvalidMinimumBitPoolValue)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidMinimumBitPoolValue;
				}
			else if(str == KA2dpNotSupportedMinimumBitPoolValue)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedMinimumBitPoolValue;
				}
			else if(str == KA2dpInvalidMaximumBitPoolValue)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidMaximumBitPoolValue;
				}
			else if(str == KA2dpNotSupportedMaximumBitPoolValue)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedMaximumBitPoolValue;
				}
			else if(str == KA2dpInvalidLayer)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidLayer;
				}
			else if(str == KA2dpNotSupportedLayer)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedLayer;
				}
			else if(str == KA2dpNotSupportedCRC)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedCRC;
				}
			else if(str == KA2dpNotSupportedMPF)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedMPF;
				}
			else if(str == KA2dpNotSupportedVBR)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedVBR;
				}
			else if(str == KA2dpInvalidBitRate)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidBitRate;
				}
			else if(str == KA2dpNotSupportedBitRate)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedBitRate;
				}
			else if(str == KA2dpInvalidObjectType)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidObjectType;
				}
			else if(str == KA2dpNotSupportedObjectType)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedObjectType;
				}
			else if(str == KA2dpInvalidChannels)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidChannels;
				}
			else if(str == KA2dpNotSupportedChannels)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedChannels;
				}
			else if(str == KA2dpInvalidVersion)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidVersion;
				}
			else if(str == KA2dpNotSupportedVersion)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedVersion;
				}
			else if(str == KA2dpNotSupportedSUL)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpNotSupportedSUL;
				}
			else if(str == KA2dpInvalidBlockLength)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidBlockLength;
				}
			else if(str == KA2dpInvalidCPType)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidCPType;
				}
			else if(str == KA2dpInvalidCPFormat)
				{
				aErrorCode = SymbianBluetoothAV::EA2dpInvalidCPFormat;
				}
			else
				{
	    	    TInt	intVal=0;
	        	ret=aDataWrapper.GetIntFromConfig(aSectName, aKeyName, intVal);
				if ( ret )
					{
					aErrorCode = (SymbianBluetoothAV::TA2dpSignallingErrorCode)intVal;
					}
				}
			}
		return ret;		
		}
	
	TBool CT_BTUtil::ReadVdpSignallingErrorCode(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TVdpSignallingErrorCode& aErrorCode)
		{
		// Read TVdpSignallingErrorCode from ini file
		TPtrC	str;
		TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, str);
		if ( ret )
			{
			if(str == KVdpInvalidCodecType)
				{
				aErrorCode = SymbianBluetoothAV::EVdpInvalidCodecType;
				}
			else if(str == KVdpNotSupportedCodecType)
				{
				aErrorCode = SymbianBluetoothAV::EVdpNotSupportedCodecType;
				}
			else if(str == KVdpInvalidLevel)
				{
				aErrorCode = SymbianBluetoothAV::EVdpInvalidLevel;
				}
			else if(str == KVdpNotSupportedLevel)
				{
				aErrorCode = SymbianBluetoothAV::EVdpNotSupportedLevel;
				}
			else if(str == KVdpInvalidCPType)
				{
				aErrorCode = SymbianBluetoothAV::EVdpInvalidCPType;
				}
			else if(str == KVdpInvalidCPFormat)
				{
				aErrorCode = SymbianBluetoothAV::EVdpInvalidCPFormat;
				}
			else
				{
	    	    TInt	intVal=0;
	        	ret=aDataWrapper.GetIntFromConfig(aSectName, aKeyName, intVal);
				if ( ret )
					{
					aErrorCode = (SymbianBluetoothAV::TVdpSignallingErrorCode)intVal;
					}
				}
			}
		return ret;		
		}
	
	TBool CT_BTUtil::ReadBluetoothMediaType(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TBluetoothMediaType& aMediaType)
		{
		// Read TBluetoothMediaType from ini file
		TPtrC	str;
		TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, str);
		if ( ret )
			{
			if(str == KAvdtpMediaTypeAudio)
				{
				aMediaType = SymbianBluetoothAV::EAvdtpMediaTypeAudio;
				}
			else if(str == KAvdtpMediaTypeVideo)
				{
				aMediaType = SymbianBluetoothAV::EAvdtpMediaTypeVideo;
				}
			else if(str == KAvdtpMediaTypeMultimedia)
				{
				aMediaType = SymbianBluetoothAV::EAvdtpMediaTypeMultimedia;
				}
			else
				{
	    	    TInt	intVal=0;
	        	ret=aDataWrapper.GetIntFromConfig(aSectName, aKeyName, intVal);
				if ( ret )
					{
					aMediaType = (SymbianBluetoothAV::TBluetoothMediaType)intVal;
					}
				}
			}
		return ret;				
		}

	TBool CT_BTUtil::ReadAvdtpTransportSessionType(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpTransportSessionType& aSessionType)
		{
		// Read TAvdtpTransportSessionType from ini file
		TPtrC	str;
		TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, str);
		if ( ret )
			{
			if(str == KMedia)
				{
				aSessionType = EMedia;
				}
			else if(str == KReporting)
				{
				aSessionType = EReporting;
				}
			else if(str == KRecovery)
				{
				aSessionType = ERecovery;
				}
			else if(str == KSignalling)
				{
				aSessionType = ESignalling;
				}
			else if(str == KNumStreamTypes)
				{
				aSessionType = ENumStreamTypes;
				}
			else
				{
	    	    TInt	intVal=0;
	        	ret=aDataWrapper.GetIntFromConfig(aSectName, aKeyName, intVal);
				if ( ret )
					{
					aSessionType = (TAvdtpTransportSessionType)intVal;
					}
				}
			}
		return ret;		
		}

/**
 * Utility method that fetches MGavdpUser pointer by command parameter name from INI-file.
 */
TBool CT_BTUtil::GetGavdpUserL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, MGavdpUser*& aData)
	{	
	// get MGavdpUser data object from parameters
	TPtrC	name;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, name);
	if ( ret )
		{
		// Data object found
		aData=static_cast<MGavdpUser*>(aDataWrapper.GetDataObjectL(name));
		}

	return ret;
	}

/**
 * Utility method that fetches RSocketServ pointer by command parameter name from INI-file.
 */
TBool CT_BTUtil::GetSocketServL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, RSocketServ*& aData)
	{	
	// get RSocketServ data object from parameters
	TPtrC	name;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, name);
	if ( ret )
		{
		// Data object found
		aData=static_cast<RSocketServ*>(aDataWrapper.GetDataObjectL(name));
		}

	return ret;
	}

/**
 * Utility method that fetches TSEID pointer by command parameter name from INI-file.
 */
TBool CT_BTUtil::GetSEIDL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TSEID*& aData)
	{	
	// get TSEID data object from parameters
	TPtrC	name;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, name);
	if ( ret )
		{
		// Data object found
		aData=static_cast<TSEID*>(aDataWrapper.GetDataObjectL(name));
		}

	return ret;
	}

/**
 * Utility method that fetches TAvdtpServiceCategories pointer by command parameter name from INI-file.
 */
TBool CT_BTUtil::GetAvdtpServiceCategoriesL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpServiceCategories*& aData)
	{	
	// get TAvdtpServiceCategories data object from parameters
	TPtrC	name;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, name);
	if ( ret )
		{
		// Data object found
		aData=static_cast<TAvdtpServiceCategories*>(aDataWrapper.GetDataObjectL(name));
		}

	return ret;
	}

/**
 * Utility method that fetches TAvdtpSEPInfo pointer by command parameter name from INI-file.
 */
TBool CT_BTUtil::GetAvdtpSEPInfoL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpSEPInfo*& aData)
	{	
	// get TAvdtpSEPInfo data object from parameters
	TPtrC	name;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, name);
	if ( ret )
		{
		// Data object found
		aData=static_cast<TAvdtpSEPInfo*>(aDataWrapper.GetDataObjectL(name));
		}

	return ret;
	}

/**
 * Utility method that fetches TAvdtpServiceCapability pointer by command parameter name from INI-file.
 */
TBool CT_BTUtil::GetAvdtpServiceCapabilityL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpServiceCapability*& aData)
	{	
	// get TAvdtpServiceCapability data object from parameters
	TPtrC	name;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, name);
	if ( ret )
		{
		// Data object found
		aData=static_cast<TAvdtpServiceCapability*>(aDataWrapper.GetDataObjectL(name));
		}

	return ret;
	}

/**
 * Utility method that fetches TBTDevAddr pointer by command parameter name from INI-file.
 */
TBool CT_BTUtil::GetBTDevAddrL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TBTDevAddr*& aData)
	{
	// get TBTDevAddr data object from parameters
	TPtrC	name;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSectName, aKeyName, name);
	if ( ret )
		{
		// Data object found
		aData=static_cast<TBTDevAddr*>(aDataWrapper.GetDataObjectL(name));
		}

	return ret;	
	}
