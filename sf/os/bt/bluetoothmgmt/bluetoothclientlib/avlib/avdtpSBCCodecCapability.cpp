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

/**
 @file
 @publishedPartner
*/

#include <bluetoothav.h>
#include "gavdpinterface.h"
#include "avdtpSBCCodecCapability.h"

/**
Default constructor for SBC codec details
*/
EXPORT_C TSBCCodecCapabilities::TSBCCodecCapabilities()
: TAvdtpMediaCodecCapabilities(SymbianBluetoothAV::EAvdtpMediaTypeAudio,
								SymbianBluetoothAV::EAudioCodecSBC,
								KServiceCatLOSCSBCCodec)
	{
	}

/**
@internalTechnology
*/	
EXPORT_C /*virtual*/ TInt TSBCCodecCapabilities::AsProtocol(RBuf8& aBuffer) const
	{
	using namespace SymbianBluetoothAV;

	TInt ret = AddHeader(aBuffer);
	if (ret == KErrNone)
		{		
		aBuffer.Append(static_cast<TChar>(iMediaType));
		aBuffer.Append(static_cast<TChar>(iMediaCodecType));

		TUint8 octet = (iSamplingFrequencies << KSBCIESampFreqOffset) |
					   (iChannelModes & KSBCIEChModeMask);
		
		aBuffer.Append(static_cast<TChar>(octet));
		
		octet = (iBlockLengths << KSBCIEBlockLenOffset) |
			    ((iSubbands & KSBCIESubbandsMask) << KSBCIESubbandsOffset) |
			    (iAllocationMethod & KSBCIEAllocMethMask);
		
		aBuffer.Append(static_cast<TChar>(octet));
		
		aBuffer.Append(static_cast<TChar>(iMinimumBitpoolValue));
		aBuffer.Append(static_cast<TChar>(iMaximumBitpoolValue));
		}
		
	return ret;

	}
	
/**
@internalTechnology
*/	
EXPORT_C /*virtual*/ void TSBCCodecCapabilities::Parse(const TDesC8& aPtr)
	{
	TInt ptrLength = aPtr.Length();
#ifdef _DEBUG
	_LIT(KSBCCodecCapabilitiesPanic, "SBCConfigPanic");
#endif
	__ASSERT_DEBUG(ptrLength>=6, User::Panic(KSBCCodecCapabilitiesPanic, 0));
	
	// parse the media type and codec type via base-class
	TAvdtpMediaCodecCapabilities::Parse(aPtr);
	
	iSamplingFrequencies = 0;
	iChannelModes = 0;
	iBlockLengths = 0;
	iSubbands = 0;
	iAllocationMethod = 0;
	iMinimumBitpoolValue = 0;
	iMaximumBitpoolValue = 0;
	
	//set the values according to the length of the package
	switch(ptrLength) {
		case 6:
			iMaximumBitpoolValue = static_cast<TInt>(aPtr[5]);
		
		case 5:
			iMinimumBitpoolValue = static_cast<TInt>(aPtr[4]);
		
		case 4:
			iBlockLengths = static_cast<TSBCBlockLengthBitmask>(aPtr[3]>>KSBCIEBlockLenOffset);
			iSubbands = static_cast<TSBCSubbandsBitmask>((aPtr[3]>>KSBCIESubbandsOffset)&KSBCIESubbandsMask);
			iAllocationMethod = static_cast<TSBCAllocationMethodBitmask>(aPtr[3]&KSBCIEAllocMethMask);
		
		case 3:
			iSamplingFrequencies = static_cast<TSBCSamplingFrequencyBitmask>(aPtr[2]>>KSBCIESampFreqOffset);
			iChannelModes = static_cast<TSBCChannelModeBitmask>(aPtr[2]&KSBCIEChModeMask);
		
		default:
			break;
		}
	}
	
/**
Get the SBC sampling frequencies
@return bitmask of supported sampling frequencies
*/	
EXPORT_C TSBCSamplingFrequencyBitmask TSBCCodecCapabilities::SamplingFrequencies() const
	{
	return iSamplingFrequencies;
	}

/**
Set the SBC sampling frequencies
@param aSampFreq bitmask of supported sampling frequencies
*/	
EXPORT_C void TSBCCodecCapabilities::SetSamplingFrequencies(TSBCSamplingFrequencyBitmask aSampFreq)
	{
	iSamplingFrequencies = aSampFreq;
	}
	
/**
Get the SBC channel modes
@return bitmask of supported channel modes
*/	
EXPORT_C TSBCChannelModeBitmask TSBCCodecCapabilities::ChannelModes() const
	{
	return iChannelModes;
	}
	
/**
Set the SBC channel modes
@param aChModes bitmask of supported channel modes
*/	
EXPORT_C void TSBCCodecCapabilities::SetChannelModes(TSBCChannelModeBitmask aChModes)
	{
	iChannelModes = aChModes;
	}
	
/**
Get the SBC block lengths
@return bitmask of supported block lengths
*/	
EXPORT_C TSBCBlockLengthBitmask TSBCCodecCapabilities::BlockLengths() const
	{
	return iBlockLengths;
	}
	
/**
Set the SBC block lengths
@param aBlockLens bitmask of supported block lengths
*/	
EXPORT_C void TSBCCodecCapabilities::SetBlockLengths(TSBCBlockLengthBitmask aBlockLens)
	{
	iBlockLengths=aBlockLens;
	}
	
/**
Get the SBC subbands
@return bitmask of supported subbands
*/	
EXPORT_C TSBCSubbandsBitmask TSBCCodecCapabilities::Subbands() const
	{
	return iSubbands;
	}
	
/**
Set the SBC subbands
@param aSubbands bitmask of supported subbands
*/	
EXPORT_C void TSBCCodecCapabilities::SetSubbands(TSBCSubbandsBitmask aSubbands)
	{
	iSubbands = aSubbands;
	}
	
/**
Get the SBC allocation methods
@return bitmask of supported allocation methods
*/	
EXPORT_C TSBCAllocationMethodBitmask TSBCCodecCapabilities::AllocationMethods() const
	{
	return iAllocationMethod;
	}
	
/**
Set the SBC allocation methods
@param aAllocMeths of supported allocation methods
*/	
EXPORT_C void TSBCCodecCapabilities::SetAllocationMethods(TSBCAllocationMethodBitmask aAllocMeths)
	{
	iAllocationMethod = aAllocMeths;
	}
	
/**
Get the SBC minimum bitpool value
@return minimum bitpool value
*/	
EXPORT_C TInt TSBCCodecCapabilities::MinBitpoolValue() const
	{
	return iMinimumBitpoolValue;
	}
	
/**
Set the SBC minimum bitpool value
@param aMinBP minimum bitpool value
*/	
EXPORT_C void TSBCCodecCapabilities::SetMinBitpoolValue(TInt aMinBP)
	{
	iMinimumBitpoolValue = aMinBP;
	}
	
/**
Get the SBC maximum bitpool value
@return maximum bitpool value
*/	
EXPORT_C TInt TSBCCodecCapabilities::MaxBitpoolValue() const
	{
	return iMaximumBitpoolValue;
	}
	
/**
Set the SBC maximum bitpool value
@param aMaxBP maximum bitpool value
*/	
EXPORT_C void TSBCCodecCapabilities::SetMaxBitpoolValue(TInt aMaxBP)
	{
	iMaximumBitpoolValue = aMaxBP;
	}

