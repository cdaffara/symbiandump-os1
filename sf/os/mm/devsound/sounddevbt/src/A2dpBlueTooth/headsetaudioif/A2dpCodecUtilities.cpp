// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "A2dpCodecUtilities.h"


/**
A2dp codec utilities Panics
**/
enum TA2dpCodecUtilitiesPanic
	{
	EA2dpCodecUtilNoRemoteCodecConfig, //0
	EA2dpCodecUtilUnexpectedDataType, //1
	EA2dpCodecUtilUnsupportedDataType, //2
	EA2dpCodecUtilUnexpectedConfiguration, //3
	EA2dpCodecUtilDataMemberNotSet //4
	};


static void Panic(TA2dpCodecUtilitiesPanic aPanic)
// Panic client
	{
	_LIT(KA2dpCodecUtilitiesPanicName, "A2DP Codec Util Panic");
	User::Panic(KA2dpCodecUtilitiesPanicName, aPanic);
	}


TInt TA2dpCodecCapabilityParser::GetSupportedSBCSampleRates(const TSBCCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedDiscreteRates)
	{
	TInt err = KErrNone;
	TSBCSamplingFrequencyBitmask samplingRatesBitMask = aCodecCaps.SamplingFrequencies();
	if (samplingRatesBitMask & E16kHz)
		{
		err = aSupportedDiscreteRates.Append(16000);
		}
	if (!err && (samplingRatesBitMask & E32kHz))
		{
		err = aSupportedDiscreteRates.Append(32000);
		}
	if (!err && (samplingRatesBitMask & E44100Hz))
		{
		err = aSupportedDiscreteRates.Append(44100);
		}
	if (!err && (samplingRatesBitMask & E48kHz))
		{
		err = aSupportedDiscreteRates.Append(48000);
		}
	return err;
	}

	
TInt TA2dpCodecCapabilityParser::GetSupportedMPEG12SampleRates(const TNonSBCCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedDiscreteRates)
	{
	TInt err = KErrNone;
	
	TPtrC8 codecCapsData(aCodecCaps.CodecData());
	
	TMPEG12SamplingFrequencyBitmask samplingRatesBitMask = codecCapsData[1] & KA2dpMPEG12SamplingFrequencyMask;

	if (samplingRatesBitMask & EMPEG12_16kHz)
		{
		err = aSupportedDiscreteRates.Append(16000);		
		}
	if (!err && samplingRatesBitMask & EMPEG12_22050Hz)
		{
		err = aSupportedDiscreteRates.Append(22050);	
		}
	if (!err && samplingRatesBitMask & EMPEG12_24kHz)
		{
		err = aSupportedDiscreteRates.Append(24000);	
		}
	if (!err && samplingRatesBitMask & EMPEG12_32kHz)
		{
		err = aSupportedDiscreteRates.Append(32000);		
		}
	if (!err && samplingRatesBitMask & EMPEG12_44100Hz)
		{
		err = aSupportedDiscreteRates.Append(44100);		
		}
	if (!err && samplingRatesBitMask & EMPEG12_48kHz)
		{
		err = aSupportedDiscreteRates.Append(48000);			
		}
				
	return err;
	}
	
	
TInt TA2dpCodecCapabilityParser::GetSupportedSampleRates(const TAvdtpMediaCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedDiscreteRates)
	{
	TInt err = KErrNotSupported;
	switch(aCodecCaps.MediaCodecType())
		{
		case EAudioCodecSBC:
			err = GetSupportedSBCSampleRates(static_cast<const TSBCCodecCapabilities&>(aCodecCaps), aSupportedDiscreteRates);
			break;
		case EAudioCodecMPEG12Audio:
			err = GetSupportedMPEG12SampleRates(static_cast<const TNonSBCCodecCapabilities&>(aCodecCaps), aSupportedDiscreteRates);
			break;
		case EAudioCodecMPEG24AAC:
			break;
		case EAudioCodecATRAC:
			break;
		default:
			Panic(EA2dpCodecUtilUnexpectedDataType);
			break;
		}
	return err;
	}


TInt TA2dpCodecCapabilityParser::GetSupportedSBCChannels(const TSBCCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport)
	{
	TInt err = KErrNotSupported;

	TSBCChannelModeBitmask channelModesBitMask = aCodecCaps.ChannelModes();
	
	err = GetSupportedChannelsCommonCode(channelModesBitMask, aSupportedChannels, aStereoSupport);
	
	return err;
	}

	
TInt TA2dpCodecCapabilityParser::GetSupportedMPEG12Channels(const TNonSBCCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport)
	{
	TInt err = KErrNotSupported;
	
	TPtrC8 codecCapsData(aCodecCaps.CodecData());
	TSBCChannelModeBitmask channelModesBitMask = codecCapsData[0] & KA2dpMPEG12AudioChannelModeMask;
		
	err = GetSupportedChannelsCommonCode(channelModesBitMask, aSupportedChannels, aStereoSupport);
		
	return err;
	}


/**
MPEG12 uses the same channel mode bit mask as SBC so we'll factor out the common code
and reuse the TSBCChannelModeBitmask for mp3 as well
*/	
TInt TA2dpCodecCapabilityParser::GetSupportedChannelsCommonCode(TSBCChannelModeBitmask aChannelModesBitMask, RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport)
	{
	TInt err = KErrNotSupported;

	if (aChannelModesBitMask & EMono)
		{
		err = aSupportedChannels.Append(EMMFMono);
		}
	if (!err)
		{
		if ((aChannelModesBitMask & EStereo) || (aChannelModesBitMask & EJointStereo))
			{
			err = aSupportedChannels.Append(EMMFStereo);
			aStereoSupport = EMMFInterleavedOnly;
			}
		}
	if (!err && (aChannelModesBitMask & EJointStereo))
		{
		//we have to cast as the TMMFStereoSupport is an enumeration
		//but is really should be a bitmap
		//but don't want to change it in order to preserve BC
		TUint aStereoSupportInt = static_cast<TUint>(aStereoSupport);
		aStereoSupportInt |= static_cast<TUint>(EMMFJoint);
		aStereoSupport = static_cast<TMMFStereoSupport>(aStereoSupportInt);
		}
	return err;
	}


TInt TA2dpCodecCapabilityParser::GetSupportedChannels(const TAvdtpMediaCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport)
	{
	TInt err = KErrNotSupported;
	switch(aCodecCaps.MediaCodecType())
		{
		case EAudioCodecSBC:
			err = GetSupportedSBCChannels(static_cast<const TSBCCodecCapabilities&>(aCodecCaps), aSupportedChannels, aStereoSupport);
			break;
		case EAudioCodecMPEG12Audio:
			err = GetSupportedMPEG12Channels(static_cast<const TNonSBCCodecCapabilities&>(aCodecCaps), aSupportedChannels, aStereoSupport);
			break;
		case EAudioCodecMPEG24AAC:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		case EAudioCodecATRAC:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		default:
			Panic(EA2dpCodecUtilUnexpectedDataType);
			break;
		}
	return err;
	}

	
/**
static
*/
TUint8 TRTPa2dpCodecSpecificUtils::PayloadType(const TFourCC& aCodecDataType)
	{
	TUint8 payloadType = 0;
	switch(const_cast<TFourCC&>(aCodecDataType).FourCC())
		{
		case KMMFFourCCCodeSBC:
			payloadType = KSbcRTPPayloadType;
			break;
		case KMMFFourCCCodeMP3:
			payloadType = KMPEG12RTPAudioPayloadType; 
			break;
		case KMMFFourCCCodeAAC:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		case KMMFFourCCCodeATRAC3:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		default:
			Panic(EA2dpCodecUtilUnexpectedDataType);
			break;
		}
	return payloadType;
	}


TUint TRTPa2dpCodecSpecificUtils::MediaPayloadHeaderLength(const TFourCC& aCodecDataType)
	{
	TUint mediaPayloadHeaderLength = 0;
	switch(const_cast<TFourCC&>(aCodecDataType).FourCC())
		{
		case KMMFFourCCCodeSBC:
			mediaPayloadHeaderLength = KSbcRTPMediaPayloadHeaderLength;
			break;
		case KMMFFourCCCodeMP3:
			mediaPayloadHeaderLength = KMPEG12RTPMediaPayloadHeaderLength;
			break;
		case KMMFFourCCCodeAAC:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		case KMMFFourCCCodeATRAC3:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		default:
			Panic(EA2dpCodecUtilUnexpectedDataType);
			break;
		}
	return mediaPayloadHeaderLength;
	}


	
TTimeIntervalMicroSeconds32 TFrameTimingUtils::FrameDuration(TUint aFrameLength, TUint aBitRate)
	{
	TUint frameDurationuS = (aFrameLength*8*1000000)/aBitRate;
	return TTimeIntervalMicroSeconds32(frameDurationuS);
	}
	

TUint TFrameTimingUtils::CalculateSBCTimeStampIncrementPerFrame(TUint aFrameLength, TUint aBitRate, TUint aSampleRate)
	{
	//SBC uses the sampleRate clock as the timestamp clock A2DP spec 4.3.3.1
	TUint frameDurationuS = FrameDuration(aFrameLength, aBitRate).Int();
	
	return (frameDurationuS*aSampleRate)/1000000;
	}
	

TUint TFrameTimingUtils::CalculateMPEG12TimeStampIncrementPerFrame(TUint aFrameLength, TUint aBitRate)
	{
	//MPEG12 uses a 90Khz clock as the timestamp clock RFC3551 section 4.5.13
	TUint frameDurationuS = FrameDuration(aFrameLength, aBitRate).Int();

	//div 1000000 as frame duration is in microseconds
	return (frameDurationuS*KMPEG12RTPTimeStampClock)/1000000;
	}
	
	
TUint TFrameTimingUtils::TimeStampIncrementPerFrame(const TFourCC& aCodecDataType, TUint aFrameLength, TUint aBitRate, TUint aSampleRate)
	{
	TInt timeStampIncrement = 0;
	switch (const_cast<TFourCC&>(aCodecDataType).FourCC())
		{
		case KMMFFourCCCodeSBC:
			timeStampIncrement = CalculateSBCTimeStampIncrementPerFrame(aFrameLength, aBitRate, aSampleRate);
			break;
		case KMMFFourCCCodeMP3:
			timeStampIncrement = CalculateMPEG12TimeStampIncrementPerFrame(aFrameLength, aBitRate);
			break;
		case KMMFFourCCCodeAAC:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		case KMMFFourCCCodeATRAC3:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		default:
			//the datatype is a non A2DP datatype
			//which is not supported so panic
			Panic(EA2dpCodecUtilUnexpectedDataType);
			break;
		}
	return timeStampIncrement;
	}


CA2dpLocalCodecCapabilities* CA2dpLocalCodecCapabilities::NewL()
	{
	return new(ELeave) CA2dpLocalCodecCapabilities();
	}


CA2dpLocalCodecCapabilities::CA2dpLocalCodecCapabilities()	
	{
	
	}

	
CA2dpLocalCodecCapabilities::~CA2dpLocalCodecCapabilities()
	{
	delete iLocalCodecCapabilities;
	}

	
TAvdtpMediaCodecCapabilities*  CA2dpLocalCodecCapabilities::LocalCodecCapabilities(const TFourCC& aCodecDataType)
	{
	switch(const_cast<TFourCC&>(aCodecDataType).FourCC())
		{
		case KMMFFourCCCodeSBC:
			iLocalCodecCapabilities = LocalSBCCodecCapabilities();
			break;
		case KMMFFourCCCodeMP3:
			iLocalCodecCapabilities = LocalMPEG12CodecCapabilities();
			break;
		case KMMFFourCCCodeAAC:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		case KMMFFourCCCodeATRAC3:
			Panic(EA2dpCodecUtilUnsupportedDataType);
			break;
		default:
			Panic(EA2dpCodecUtilUnexpectedDataType);
			break;
		}
	return iLocalCodecCapabilities;
	}
	

TSBCCodecCapabilities* CA2dpLocalCodecCapabilities::LocalSBCCodecCapabilities()
	{
	delete iLocalCodecCapabilities;
	iLocalCodecCapabilities = NULL;
	TSBCCodecCapabilities* localSbcCodecCapabilities = new TSBCCodecCapabilities();
	if (localSbcCodecCapabilities)
		{
		localSbcCodecCapabilities->SetSamplingFrequencies(E16kHz|E32kHz|E44100Hz|E48kHz);
		localSbcCodecCapabilities->SetChannelModes(EMono|EStereo|EJointStereo);
		localSbcCodecCapabilities->SetBlockLengths(EBlockLenFour|EBlockLenEight|EBlockLenTwelve|EBlockLenSixteen);
		localSbcCodecCapabilities->SetSubbands(EFourSubbands|EEightSubbands);
		localSbcCodecCapabilities->SetAllocationMethods(ELoudness|ESNR);
		localSbcCodecCapabilities->SetMinBitpoolValue(KMinBitPoolValue);
		localSbcCodecCapabilities->SetMaxBitpoolValue(KMaxBitPoolValue);
		}
	return localSbcCodecCapabilities;
	}


TNonSBCCodecCapabilities* CA2dpLocalCodecCapabilities::LocalMPEG12CodecCapabilities()
	{
	delete iLocalCodecCapabilities;
	iLocalCodecCapabilities = NULL;
	TNonSBCCodecCapabilities* localSEPmpeg12CodecCapabilities = new TNonSBCCodecCapabilities(EAvdtpMediaTypeAudio, EAudioCodecMPEG12Audio);
	if (localSEPmpeg12CodecCapabilities)
		{
		TBuf8<4>	mpeg12CodecCapabilitiesData;

		//MPEG12 channel modes are the same as SBC so we'll use SBC structure
		TSBCChannelModeBitmask channelModes = (EMono|EStereo);
		mpeg12CodecCapabilitiesData.Append(KA2dpMPEG12LayerMP3Supported | channelModes);
		
		//no MPF-2 support, will support 16-44100khz sample
		TInt8 samplingFrequencySupport = EMPEG12_16kHz|EMPEG12_22050Hz|EMPEG12_24kHz|EMPEG12_32kHz|EMPEG12_44100Hz;
		mpeg12CodecCapabilitiesData.Append(samplingFrequencySupport);
		
		//no VBR all bitrates <= 96kbs-1 except free format
		mpeg12CodecCapabilitiesData.Append(0);
		mpeg12CodecCapabilitiesData.Append(KA2dpMPEG12SupportedBitRateIndex);//all bit rates <= 96kbs-1
		localSEPmpeg12CodecCapabilities->SetCodecData(mpeg12CodecCapabilitiesData);
		}
	return localSEPmpeg12CodecCapabilities;
	}


	
CA2dpAudioCodecConfiguration* CA2dpAudioCodecConfiguration::NewL()
	{
	return new(ELeave)CA2dpAudioCodecConfiguration();
	}
	

CA2dpAudioCodecConfiguration::CA2dpAudioCodecConfiguration() : iHeadsetCodecDataType(KMMFFourCCCodeSBC), iSampleRate(KDefaultSampleRate), iChannels(KDefaultChannels), iStereoSupport(KDefaultStereoSupport)
	{	
	}
	
CA2dpAudioCodecConfiguration::~CA2dpAudioCodecConfiguration()
	{
	delete iRemoteCodecConfiguration;
	}


void CA2dpAudioCodecConfiguration::Reset()
	{
	delete iRemoteCodecConfiguration;
	iRemoteCodecConfiguration = NULL;
	iHeadsetCodecDataType = KMMFFourCCCodeSBC;
	iSampleRate = KDefaultSampleRate;
	iChannels = KDefaultChannels;
	iStereoSupport = KDefaultStereoSupport;
	iLocalSBCCodecConfiguration.Reset();
	}

/**
Function used to return a TAvdtpMediaCodecCapabilities structure
that can be used to configure the SEP at the remote end ie on the headset
The capabilities are used to determine the configuration
need to return by pointer rather than by ref as TAvdtpMediaCodecCapabilities is abstract
Note that this parameter passed in treats the  TAvdtpMediaCodecCapabilities as 
the capabilities of the code, whereas the returned TAvdtpMediaCodecCapabilities is a
configuration ie a specific set of values that can be used to configure the remote codec

@param aCodecCapabilities  This is the capabilities supported by the remote codec
@return The configuration for the remote codec
*/
//void CA2dpAudioCodecConfiguration::GetRemoteCodecConfiguration(const TAvdtpMediaCodecCapabilities& aRemoteCodecCapabilities, TAvdtpMediaCodecCapabilities& aRemoteCodecConfiguration)
TAvdtpMediaCodecCapabilities* CA2dpAudioCodecConfiguration::UpdateRemoteCodecConfiguration(const TAvdtpMediaCodecCapabilities& aRemoteCodecCapabilities)
	{
	delete iRemoteCodecConfiguration;
	iRemoteCodecConfiguration = NULL;
		{
		//this hasn't been set yet so set it
		//first find out the capabilities of the codec
		switch (aRemoteCodecCapabilities.MediaCodecType())
			{
			case EAudioCodecSBC:
				iRemoteCodecConfiguration = GetRemoteSBCCodecConfiguration(static_cast<const TSBCCodecCapabilities&>(aRemoteCodecCapabilities));
				iHeadsetCodecDataType = KMMFFourCCCodeSBC;
				break;
			case EAudioCodecMPEG12Audio:
				iRemoteCodecConfiguration = GetRemoteMPEG12CodecConfiguration(static_cast<const TNonSBCCodecCapabilities&>(aRemoteCodecCapabilities));
				iHeadsetCodecDataType = KMMFFourCCCodeMP3;
				break;
			case EAudioCodecMPEG24AAC:
				Panic(EA2dpCodecUtilUnsupportedDataType);
				break;
			case EAudioCodecATRAC:
				Panic(EA2dpCodecUtilUnsupportedDataType);
				break;
			default:
				//the datatype is a non A2DP datatype
				//which is not supported so panic
				Panic(EA2dpCodecUtilUnexpectedDataType);
				break;
			}
		}
	return iRemoteCodecConfiguration;
	}
		

/**

*/
TSBCCodecCapabilities* CA2dpAudioCodecConfiguration::GetRemoteSBCCodecConfiguration(const TSBCCodecCapabilities& aCodecCaps)
	{
	TSBCCodecCapabilities* sbcCodecConfiguration = new TSBCCodecCapabilities();
	
	if (sbcCodecConfiguration)	
		{
		//--sampling frequency--
		TSBCSamplingFrequencyBitmask freqs;
		switch(iSampleRate)
			{
			case 16000:
				freqs = E16kHz;
				break;
			case 32000:
				freqs = E32kHz;
				break;
			case 44100:
				freqs = E44100Hz;
				break;
			case 48000:
				freqs = E48kHz;
				break;
			default:
				freqs = E16kHz;
				break;
			}
		//check we really can support this sampling frequency
		//since we may be using the default which may not be supported
		//by the headset (even if the A2DP spec says it is mandatory)
		TSBCSamplingFrequencyBitmask samplingFreqsSupportedByHeadset = aCodecCaps.SamplingFrequencies();
		if 	(iForcedRemoteSBCCodecConfiguration)
			{
			samplingFreqsSupportedByHeadset = iForcedRemoteSBCCodecConfiguration->SamplingFrequencies();
			}
		if (!(freqs & samplingFreqsSupportedByHeadset))
			{//then the headset doesn't support the sampling frequency
			//this could happen if iSampleRate is a default that the headset doesn't support
			//note that evenif the default is a mandatory sample rate eg 44100
			//we still do not make any assumptions about what is supported 
			if (samplingFreqsSupportedByHeadset & E16kHz)
				{
				iSampleRate = 16000;
				freqs = E16kHz;
				}
			else if (samplingFreqsSupportedByHeadset & E32kHz)
				{
				iSampleRate = 32000;
				freqs = E32kHz;
				}
			else if (samplingFreqsSupportedByHeadset & E44100Hz)
				{
				iSampleRate = 44100;
				freqs = E44100Hz;
				}
			else if (samplingFreqsSupportedByHeadset & E48kHz)
				{
				iSampleRate = 48000;
				freqs = E48kHz;
				}
			//else just keep as is
			}
			sbcCodecConfiguration->SetSamplingFrequencies(freqs);
			
		//--channels--
		TSBCChannelModeBitmask channelMode = EMono;
		TSBCChannelModeBitmask channelModesSupportedByHeadset = EMono;
		channelModesSupportedByHeadset = aCodecCaps.ChannelModes();
		if (iChannels == EMMFMono)
			{
			channelMode = EMono;
			}
		else if ((iChannels == EMMFStereo) && (iStereoSupport == EMMFInterleavedOnly))
			{
			channelMode = EStereo;
			}
		else if ((iChannels == EMMFStereo) && (iStereoSupport == EMMFJoint))
			{
			channelMode = EJointStereo;
			}
		if (!(channelMode & channelModesSupportedByHeadset))
			{
			//then we don't support the selected channel mode
			if (channelModesSupportedByHeadset & EMono)
				{
				iChannels = EMMFMono;
				iStereoSupport = EMMFNone;
				channelMode = EMono;
				}
			else if (channelModesSupportedByHeadset & EJointStereo)
				{
				iChannels = EMMFStereo;
				iStereoSupport = EMMFJoint;
				channelMode = EJointStereo;
				}
			else if (channelModesSupportedByHeadset & EStereo)
				{
				iChannels = EMMFStereo;
				iStereoSupport = EMMFInterleavedOnly;
				channelMode = EStereo;
				}
			}
		sbcCodecConfiguration->SetChannelModes(channelMode);	
	
		//--block mode--
		//we'll choose a preference order of 16,12, 8,4
		//although there may be a more intelligent way of doing this
		//based on the other parameters.
		TSBCBlockLengthBitmask blockLength = EBlockLenSixteen;
		TSBCBlockLengthBitmask blockLengthsSupportedByHeadset = aCodecCaps.BlockLengths();
		if 	(iForcedRemoteSBCCodecConfiguration)
			{
			blockLengthsSupportedByHeadset = iForcedRemoteSBCCodecConfiguration->BlockLengths();
			}
		if (blockLengthsSupportedByHeadset & EBlockLenSixteen)
			{
			blockLength = EBlockLenSixteen;
			}
		else if (blockLengthsSupportedByHeadset & EBlockLenTwelve)
			{
			blockLength = EBlockLenTwelve;
			}
		else if (blockLengthsSupportedByHeadset & EBlockLenEight)
			{
			blockLength = EBlockLenEight;
			}
		else if (blockLengthsSupportedByHeadset & EBlockLenFour)
			{
			blockLength = EBlockLenFour;
			}
		sbcCodecConfiguration->SetBlockLengths(blockLength);
	
		//--subbands--
		//Subbands currently have a preference of 8 over 4
		TUint numberOfSubbands = 8; //used later for max bit pool value calculation
		TSBCSubbandsBitmask subbands = EEightSubbands;
		TSBCSubbandsBitmask subbandsSupportedByHeadset = aCodecCaps.Subbands();
		if 	(iForcedRemoteSBCCodecConfiguration)
			{
			subbandsSupportedByHeadset = iForcedRemoteSBCCodecConfiguration->Subbands();
			}
		if (subbandsSupportedByHeadset & EEightSubbands)
			{
			subbands = EEightSubbands;
			}
		else if (subbandsSupportedByHeadset & EFourSubbands)
			{
			subbands = EFourSubbands;
			numberOfSubbands = 4;
			} 
		sbcCodecConfiguration->SetSubbands(subbands);
	
		//--Allocation--
		//although allocation support of SNR and loudness are mandatory the headset
		//may have reconfigured to use a particular allocation method
		//If both allocation methods are available we have to choose which one
		//so we'll choose a preference order of loudness over SNR
		TSBCAllocationMethodBitmask allocationMethod = ELoudness;
		TSBCAllocationMethodBitmask allocationMethodsSupportedByHeadset = aCodecCaps.AllocationMethods(); 
		if (iForcedRemoteSBCCodecConfiguration)
			{
			allocationMethodsSupportedByHeadset = iForcedRemoteSBCCodecConfiguration->AllocationMethods();
			}
		if (allocationMethodsSupportedByHeadset & ELoudness)
			{
			allocationMethod = ELoudness;
			}
		else if (allocationMethodsSupportedByHeadset & ESNR)
			{
			allocationMethod = ESNR;
			}
		sbcCodecConfiguration->SetAllocationMethods(allocationMethod);

		//--bitpool
		// The bitpool value must be in the range of 2-250 and must not exceed
		// 16*numberOfSubbands*channels
		// note that we don't normally play SBC directly, except for test purposes
		// in order to arbitary play any SBC file extra code would be required
		// to parse the SBC frame header to get the bitpool value
		// since for unit testing the the SBC test file is known to have a
		// bit pool value of 20, this shall be the default value
		// This code could be made more more intelligent and base the 
		// bitpool value around table 4.7 in the A2DP specification
		TUint minBitPoolValue = KMinBitPoolValue;
		TUint maxBitPoolValue = KMaxBitPoolValue; 
		if (KMinBitPoolValue < aCodecCaps.MinBitpoolValue())
			{
			minBitPoolValue = aCodecCaps.MinBitpoolValue();
			}
		if (KMaxBitPoolValue > aCodecCaps.MaxBitpoolValue())
			{
			TUint maxAllowableBitPoolValueForCurrentConfiguration = 16*iChannels*numberOfSubbands;
			maxBitPoolValue = aCodecCaps.MaxBitpoolValue();
			if (maxBitPoolValue > maxAllowableBitPoolValueForCurrentConfiguration)
				{
				maxBitPoolValue = maxAllowableBitPoolValueForCurrentConfiguration;
				}
			}
		sbcCodecConfiguration->SetMinBitpoolValue(minBitPoolValue);
		sbcCodecConfiguration->SetMaxBitpoolValue(maxBitPoolValue);
		}//if (sbcCodecConfiguration)

	return sbcCodecConfiguration;
	}
	

/**
internal function to get a remote codec configuration from the capabilities
Note technically speaking MPEG12 refers to MPEG 1 & MPEG 2 audio layers 1,2 and 3
however since mp1 & mp2 is rarely used in practice this 
will be MPEG 1 layer 3 ie mp3 in most case
*/
TNonSBCCodecCapabilities* CA2dpAudioCodecConfiguration::GetRemoteMPEG12CodecConfiguration(const TNonSBCCodecCapabilities& aCodecCaps)
	{
	TNonSBCCodecCapabilities* mpeg12CodecConfiguration = new TNonSBCCodecCapabilities(EAvdtpMediaTypeAudio, EAudioCodecMPEG12Audio);
	TBuf8<4>	mpeg12CodecConfigurationData;//should be 4 make 6 for bug??
	if (mpeg12CodecConfiguration)
		{//codecData should contain data conformant to A2DP profile
		// codec specific information elements section A2DP 4.4.2
		TPtrC8 codecCapsData(aCodecCaps.CodecData());
		TInt8 mpegLayerSupport = codecCapsData[0] & KA2dpMPEG12LayerMask;
		if (!(mpegLayerSupport & KA2dpMPEG12LayerMP3Supported))
			{
			//then mp3 is not supported must be mp1 or mp2 which we don't support
			delete mpeg12CodecConfiguration;
			mpeg12CodecConfiguration = NULL;
			return mpeg12CodecConfiguration;
			}
		mpegLayerSupport = KA2dpMPEG12LayerMP3Supported; //mp3
		//we don't bother with CRC protection so don't check
		
		//the MPEG12 channel support structure is identical to 
		//SBC so we'll use the SBC structure
		//--channels--
		TSBCChannelModeBitmask channelMode = EMono;
		TSBCChannelModeBitmask channelModesSupportedByHeadset = EMono;
		channelModesSupportedByHeadset = codecCapsData[0] & KA2dpMPEG12AudioChannelModeMask;
		if (iChannels == EMMFMono)
			{
			channelMode = EMono;
			}
		else if ((iChannels == EMMFStereo) && (iStereoSupport == EMMFInterleavedOnly))
			{
			channelMode = EStereo;
			}
		else if ((iChannels == EMMFStereo) && (iStereoSupport == EMMFJoint))
			{
			channelMode = EJointStereo;
			}
		if (!(channelMode & channelModesSupportedByHeadset))
			{
			//then we don't support the selected channel mode
			if (channelModesSupportedByHeadset & EMono)
				{
				iChannels = EMMFMono;
				iStereoSupport = EMMFNone;
				channelMode = EMono;
				}
			else if (channelModesSupportedByHeadset & EJointStereo)
				{
				iChannels = EMMFStereo;
				iStereoSupport = EMMFJoint;
				channelMode = EJointStereo;
				}
			else if (channelModesSupportedByHeadset & EStereo)
				{
				iChannels = EMMFStereo;
				iStereoSupport = EMMFInterleavedOnly;
				channelMode = EStereo;
				}
			}
		mpeg12CodecConfigurationData.Append(mpegLayerSupport | channelMode);
	
		//Media Payload Format 
		//this ref implementation shall only support the media payload format
		//defined in RFC2250, RFC3119 is not supported
		//therefore there is no need to check this since MPF-1/RFC2250 is mandatory
		
		//--sampling frequency--
		//the mp3 sampling frequency mask is not the same as SBC so can't use TSBCSamplingFrequencyBitmask
		TMPEG12SamplingFrequencyBitmask freqs;
		switch(iSampleRate)
			{
			case 16000:
				freqs = EMPEG12_16kHz;
				break;
			case 22050:
				freqs = EMPEG12_22050Hz;
				break;
			case 24000:
				freqs = EMPEG12_24kHz;
				break;
			case 32000:
				freqs = EMPEG12_32kHz;
				break;
			case 44100:
				freqs = EMPEG12_44100Hz;
				break;
			case 48000:
				freqs = EMPEG12_48kHz;
				break;
			default:
				freqs = EMPEG12_16kHz;
				break;
			}
		//check we really can support this sampling frequency
		//since we may be using the default which may not be supported
		//by the headset (even if the A2DP spec says it is mandatory)
		TMPEG12SamplingFrequencyBitmask samplingFreqsSupportedByHeadset = codecCapsData[1] & KA2dpMPEG12SamplingFrequencyMask;
		if (!(freqs & samplingFreqsSupportedByHeadset))
			{//then the headset doesn't support the sampling frequency
			//this could happen if iSampleRate is a default that the headset doesn't support
			//note that evenif the default is a mandatory sample rate eg 44100
			//we still do not make any assumptions about what is supported 
			if (samplingFreqsSupportedByHeadset & EMPEG12_16kHz)
				{
				iSampleRate = 16000;
				freqs = EMPEG12_16kHz;
				}
			else if (samplingFreqsSupportedByHeadset & EMPEG12_22050Hz)
				{
				iSampleRate = 22050;
				freqs = EMPEG12_22050Hz;
				}
			else if (samplingFreqsSupportedByHeadset & EMPEG12_24kHz)
				{
				iSampleRate = 24000;
				freqs = EMPEG12_24kHz;
				}
			else if (samplingFreqsSupportedByHeadset & EMPEG12_32kHz)
				{
				iSampleRate = 32000;
				freqs = EMPEG12_32kHz;
				}
			else if (samplingFreqsSupportedByHeadset & EMPEG12_44100Hz)
				{
				iSampleRate = 44100;
				freqs = EMPEG12_44100Hz;
				}
			else if (samplingFreqsSupportedByHeadset & EMPEG12_48kHz)
				{
				iSampleRate = 48000;
				freqs = EMPEG12_48kHz;
				}
			//else just keep as is
			}
		//set frequency, MPF-1
		mpeg12CodecConfigurationData.Append(freqs);
		
		
		//The casira pod only supports bit rates up to 96kbs-1
		//so check the headset supports these as well
		//we'll just mask with all the lower bit rates
		//all the lower bit rate support up to 96kbs-1are in octet 3
		//so just make out octet 2 bit rate support
		//also VBR = 0 as no VBR support
		mpeg12CodecConfigurationData.Append(0);
		
		//all bitrates <= 96kbs-1 except free format
		mpeg12CodecConfigurationData.Append(KA2dpMPEG12SupportedBitRateIndex & codecCapsData[3]);
		mpeg12CodecConfiguration->SetCodecData(mpeg12CodecConfigurationData);
		}
	return mpeg12CodecConfiguration;
	}


/**
Utility function get the codec capabilities used by Gavdp into the SBC codec paramerers
used by the SBC codec
The function uses the TSBCCodecCapabilities to generate a set of TSBCFrameParameters
which can be used to configure the codec
the remote codec configuration must have already been updated before calling
this function

@param aSBCFrameParameters  These are set to the current configuration settings
*/
TSBCFrameParameters& CA2dpAudioCodecConfiguration::UpdateLocalSBCCodecConfiguration()
	{
	__ASSERT_DEBUG(iRemoteCodecConfiguration, Panic(EA2dpCodecUtilNoRemoteCodecConfig));
	__ASSERT_DEBUG((iHeadsetCodecDataType == KMMFFourCCCodeSBC) && (iRemoteCodecConfiguration->MediaCodecType() == EAudioCodecSBC), Panic(EA2dpCodecUtilUnexpectedDataType));
	TSBCCodecCapabilities* remoteSBCCodecConfiguration =  static_cast<TSBCCodecCapabilities*>(iRemoteCodecConfiguration);
		
	//note that for sampling frequency and channels the capabilites
	//have already been checked in SetSampleRate and SetChannels
	//and once set we don't allow these to be reconfigured
	//so no need to check these again with the codecCaps
	//if set the iSampleRate should always be in agreement with 
	//the settings on the remote codec
	TSBCFrameParameters::TSamplingFrequency SBCSamplingFrequency;
	switch(iSampleRate)
		{
		case 32000:
			SBCSamplingFrequency = TSBCFrameParameters::E32000Hz;
			break;
		case 44100:
			SBCSamplingFrequency = TSBCFrameParameters::E44100Hz;
			break;
		case 48000:
			SBCSamplingFrequency = TSBCFrameParameters::E48000Hz;
			break;
		default://[TODO] change default to 44100 when hardware 
		//(ie BT support H2 or higher bandwidth support on Casira) supports it
			SBCSamplingFrequency = TSBCFrameParameters::E16000Hz;
			break;
		}

	iLocalSBCCodecConfiguration.SetSamplingFrequency(SBCSamplingFrequency);
	
	//although a block length of 4,8,12,16 are mandatory the headset
	//may have reconfigured to use a particular block length
	//If all block lengths are available we have to choose which one
	//For now we'll choose a preference order of 12,16,8,4
	//although there may be a more intelligent way of doing this
	//based on the other parameters.
	switch(remoteSBCCodecConfiguration->BlockLengths())
		{
		case EBlockLenTwelve:
			iLocalSBCCodecConfiguration.SetBlockLength(TSBCFrameParameters::E12Blocks);
			break;
		case EBlockLenSixteen:
			iLocalSBCCodecConfiguration.SetBlockLength(TSBCFrameParameters::E16Blocks);
			break;
		case EBlockLenEight:
			iLocalSBCCodecConfiguration.SetBlockLength(TSBCFrameParameters::E8Blocks);
			break;
		case EBlockLenFour:
			iLocalSBCCodecConfiguration.SetBlockLength(TSBCFrameParameters::E4Blocks);
			break;
		default:
			Panic(EA2dpCodecUtilUnexpectedConfiguration);
			break;
		}
	
	
	//channel mode
	//although the number of channels can't be changed on the fly
	//switching between stereo and joint stereo is supported
	//note different namespaces for EStereo & EJointStereo !
	TSBCFrameParameters::TChannelMode channelMode = TSBCFrameParameters::EMono;//default
	if (iChannels == EMMFStereo)
		{
		if (iStereoSupport & EMMFJoint)
			{
			channelMode = TSBCFrameParameters::EJointStereo;
			}
		else if (iStereoSupport & EMMFInterleavedOnly)
			{
			channelMode = TSBCFrameParameters::EStereo;
			}
		else
			{
			Panic(EA2dpCodecUtilUnexpectedConfiguration);
			}
		}

	iLocalSBCCodecConfiguration.SetChannelMode(channelMode);
	
	//Allocation
	//although allocation support of SNR and loudness are mandatory the headset
	//may have reconfigured to use a particular allocation method
	//If both allocation methods are available we have to choose which one
	//For now we'll choose a preference order of loudness followed by SNR

	switch(remoteSBCCodecConfiguration->AllocationMethods())
		{
		case ELoudness:
			iLocalSBCCodecConfiguration.SetAllocationMethod(TSBCFrameParameters::ELoudness);
			break;
		case ESNR:
			iLocalSBCCodecConfiguration.SetAllocationMethod(TSBCFrameParameters::ESNR);
			break;
		default:
			Panic(EA2dpCodecUtilUnexpectedConfiguration);
			break;
		}
	
	
	//Subbands currently have a preference of four over 8, but change to 8 later
	//when hardware is available to support higher bandwidth
	switch(remoteSBCCodecConfiguration->Subbands())
		{
		case EFourSubbands:
			iLocalSBCCodecConfiguration.SetSubbands(TSBCFrameParameters::E4Subbands);
			break;
		case EEightSubbands:
			iLocalSBCCodecConfiguration.SetSubbands(TSBCFrameParameters::E8Subbands);
			break;
		default:
			Panic(EA2dpCodecUtilUnexpectedConfiguration);
			break;
		}

	// note that we don't normally play SBC directly, except for test purposes
	// in order to arbitary play any SBC file extra code would be required
	// to parse the SBC frame header to get the bitpool value
	// since for unit testing the the SBC test file is known to have a
	// bit pool value of 20, this shall be the default value
	// This code could be made more more intelligent and base the 
	// bitpool value around table 4.7 in the A2DP specification
	TUint bitPoolValue = KDefaultBitPoolValue; //default is 20
	if (KDefaultBitPoolValue < remoteSBCCodecConfiguration->MinBitpoolValue())
		{
		bitPoolValue = remoteSBCCodecConfiguration->MinBitpoolValue();
		}
	else if (KDefaultBitPoolValue > remoteSBCCodecConfiguration->MaxBitpoolValue())
		{
		bitPoolValue = remoteSBCCodecConfiguration->MaxBitpoolValue();
		}
	iLocalSBCCodecConfiguration.SetBitpool(bitPoolValue); 
	
	return iLocalSBCCodecConfiguration;
	}	

	
/**
Internal function to calculate the SBC buffer length required from the SBC frame parameters
using the given pcm16 buffer length
*/
TUint CA2dpAudioCodecConfiguration::CalculateSBCBufferLength(TUint aPCM16BufferLength) const
	{
	//ASSERT data type = SBC
	
	//first calculate the number of PCM16 samples in one SBC frame
	TUint numberOfSamplesPerSBCFrame = iLocalSBCCodecConfiguration.BlockLength()*iLocalSBCCodecConfiguration.Subbands();
	
	//calculate the number of bytes in one sample
	TUint numberOfPCM16BytesPerSample = 2*iLocalSBCCodecConfiguration.Channels();
	
	TUint numberOfPCM16BytesPerSBCFrame = numberOfSamplesPerSBCFrame*numberOfPCM16BytesPerSample;
	
	TUint numberOfFrames = aPCM16BufferLength/numberOfPCM16BytesPerSBCFrame;
	
	TUint lengthOfSBCFrame = iLocalSBCCodecConfiguration.CalcFrameLength();
	
	return numberOfFrames*lengthOfSBCFrame;
	}
	

/**
Test function to force the remote SBC codec configuration to that set in aRemoteCodecConfiguration
This function is just used for test purposes to set the SBC settings for
playing SBC data direct to the a2dpBTHeadsetAudioIf ie when the Data type is 
set to SBC
It is up to the user to ensure that the headset supports the configuration
since this will bypass the normal capability check
*/	
void CA2dpAudioCodecConfiguration::TEST_ForceRemoteSBCCodecConfiguration(const TSBCCodecCapabilities& aRemoteCodecConfiguration)
	{
	iForcedRemoteSBCCodecConfiguration = const_cast<TSBCCodecCapabilities*>(&aRemoteCodecConfiguration);
	}

	


CA2dpCodecFrameHeaderParser* CA2dpCodecFrameHeaderParser::NewL(const TFourCC& aCodecDataType, const TDesC8& aHeader)
	{
	CA2dpCodecFrameHeaderParser* self = new (ELeave) CA2dpCodecFrameHeaderParser();
	CleanupStack::PushL(self);
	self->ConstructL(aCodecDataType, aHeader);
	CleanupStack::Pop(self);
	return self;
	}


CA2dpCodecFrameHeaderParser::CA2dpCodecFrameHeaderParser()
	{
	
	}


CA2dpCodecFrameHeaderParser::~CA2dpCodecFrameHeaderParser()
	{
	
	}	


/**
Only used for playing SBC test files
*/
void CA2dpCodecFrameHeaderParser::ParseSBCHeaderL(const TDesC8& aHeader)
	{
	if (aHeader[0] != KSbcFrameHeaderSyncWord)
		{
		User::Leave(KErrCorrupt); //not a valid sbc frame
		}
	
	TUint samplingFrequency = (aHeader[1] & KSbcFrameHeaderSamplingFrequencyMask)>>6;
	switch(samplingFrequency)
		{
		case TSBCFrameParameters::E16000Hz:
			iSampleRate = 16000;
			break;
		case TSBCFrameParameters::E32000Hz:
			iSampleRate = 32000;
			break;
		case TSBCFrameParameters::E44100Hz:
			iSampleRate = 44100;
			break;
		case TSBCFrameParameters::E48000Hz:
			iSampleRate = 48000;
			break;
		default:
			User::Leave(KErrCorrupt);//not a valid header
			break;
		}
		
	TUint8 blocks = (aHeader[1] & KSbcFrameHeaderBlocksMask)>>4;
	switch(blocks)
		{
		case TSBCFrameParameters::E4Blocks:
			blocks = 4;
			break;
		case TSBCFrameParameters::E8Blocks:
			blocks = 8;
			break;
		case TSBCFrameParameters::E12Blocks:
			blocks = 12;
			break;
		case TSBCFrameParameters::E16Blocks:
			blocks = 16;
			break;
		default:
			User::Leave(KErrCorrupt);//not a valid header
			break;
		}
	TUint8 channelMode = (aHeader[1] & KSbcFrameHeaderChannelModeMask)>>2;
	TUint8 channels = 1;
	
	if (channelMode != TSBCFrameParameters::EMono)
		{
		channels = 2;
		}
	
	TUint8 subbands = 4;
	if (aHeader[1] & KSbcFrameHeaderSubbandsMask)
		{
		subbands = 8;
		}
		
	TUint8 bitpool = aHeader[2];
	
	TUint temp = 0;
	switch (channelMode)
		{
		case TSBCFrameParameters::EMono:
			temp = blocks * bitpool; // blocks * bitpool
			break;	
		case TSBCFrameParameters::EDualChannel:
			temp = (blocks * bitpool) << 1; // blocks * bitpool * 2
			break;	
		case TSBCFrameParameters::EStereo:
			temp = blocks * bitpool; // blocks * bitpool
			break;	
		case TSBCFrameParameters::EJointStereo:
			temp = subbands + blocks * bitpool; // subbands + blocks * bitpool
			break;
		}
		
	iFrameLength = 4 + ( (subbands * channels) >> 1) + (temp >> 3);
	if (temp & 0x7)
		{
		iFrameLength++;
		}
		
	iBitRate = (8*iFrameLength*iSampleRate)/(subbands*blocks);
	}


void CA2dpCodecFrameHeaderParser::ParseMPEG12HeaderL(const TDesC8& aHeader)
	{
	if (aHeader[0] != KMPEGAudioFrameHeaderSyncWord )
		{
		User::Leave(KErrCorrupt); //not a valid MPEG audio frame
		}
	
	//check it's really mp3 as opposed to mp1 or mp2
	if ((aHeader[1] & KMp3AudioFrameHeaderIdMask) != KMp3AudioFrameHeaderId)
		{
		User::Leave(KErrNotSupported); //either corrupt or mp1/mp2
		}
	
	TUint8 sampleRateIndex = (aHeader[2] & KMp3FrameHeaderSamplingFrequencyMask)>>2;
	switch(sampleRateIndex)
		{
		case 0x00:
			iSampleRate = 44100;
			break;
		case 0x01:
			iSampleRate = 48000;
			break;
		case 0x02:
			iSampleRate = 32000;
			break;
		default:
			User::Leave(KErrCorrupt); //invalid mp3 header?
			break;
		}
	TUint8 bitRateIndex = (aHeader[2] & KMp3FrameHeaderBitRateIndexMask)>>4;
	switch(bitRateIndex)
		{
		case EMp3BitRateIndex32000:
			iBitRate = 32000;
			break;
		case EMp3BitRateIndex40000:
			iBitRate = 40000;
			break;
		case EMp3BitRateIndex48000:
			iBitRate = 48000;
			break;
		case EMp3BitRateIndex56000:
			iBitRate = 56000;
			break;
		case EMp3BitRateIndex64000:
			iBitRate = 64000;
			break;
		case EMp3BitRateIndex80000:
			iBitRate = 80000;
			break;
		case EMp3BitRateIndex96000:
			iBitRate = 96000;
			break;
		default:
			User::Leave(KErrNotSupported);//don't support more than 96kbs-1
			break;
		}
	
	//this code should really be made more clever to take allowance for any padding
	//and use a frame length such that if padding is used then take the lowest
	//common multiple of frames tht gives a common frame length eg if the frames
	//alternate between 100 and 101 bytes due to an extra padding byte everyother
	//frame then set the frame length to 201;	
	TUint bitRateX144 = iBitRate*144;
	iFrameLength = bitRateX144/iSampleRate;
	}

	
/**
static
*/	
void CA2dpCodecFrameHeaderParser::ConstructL(const TFourCC& aCodecDataType, const TDesC8& aHeader)
	{
	switch(const_cast<TFourCC&>(aCodecDataType).FourCC())
		{
		case KMMFFourCCCodeSBC:
			ParseSBCHeaderL(aHeader);
			break;
		case KMMFFourCCCodeMP3:
			ParseMPEG12HeaderL(aHeader);
			break;
		case KMMFFourCCCodeAAC:
			User::Leave(KErrNotSupported);
			break;
		case KMMFFourCCCodeATRAC3:
			User::Leave(KErrNotSupported);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}	

