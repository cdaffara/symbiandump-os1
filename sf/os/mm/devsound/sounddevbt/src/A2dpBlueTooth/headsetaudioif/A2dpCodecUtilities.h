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

#ifndef __A2DPCODECUTILITIES_H__
#define __A2DPCODECUTILITIES_H__


#include <bluetoothav.h> //TAvdtpSEPInfo, TSEID
#include <mmf/server/sounddevice.h> //TMMFStereoSupport
#include "BtSBCFrameParameters.h"

//[TODO] change this to 44100 when we have hardware that supports higher data rates
const TUint KDefaultSampleRate = 32000;
const TUint KDefaultBitPoolValue = 20;
//[TODO] change this to EMMFStereo & EMMFInterleavedOnly when we have hardware that supports higher data rates
const TUint KDefaultChannels = EMMFMono;
const TMMFStereoSupport KDefaultStereoSupport = EMMFNone;

const TUint KMinBitPoolValue = 2;
const TUint KMaxBitPoolValue = 250;

//A2DP codec specific data masks
const TUint8 KA2dpMPEG12AudioChannelModeMask = 0x0F; //A2DP 4.4.2.3
const TUint8 KA2dpMPEG12LayerMask = 0xE0; //A2DP 4.4.2.1
const TUint8 KA2dpMPEG12LayerMP3Supported = 0x20; //A2DP 4.4.2.1
const TUint8 KA2dpMPEG12SamplingFrequencyMask = 0x3F; //A2DP 4.4.2.5
const TUint8 KA2dpMPEG12SupportedBitRateIndex = 0xFE; //A2DP 4.4.2.7 octet 3 all bit rates <=96kbs-1 except free

//RTP Codec profile specific constants
const TUint KMPEG12RTPTimeStampClock = 90000; //RFC2250 3.3
const TUint KSbcRTPPayloadType = 0; //A2DP 4.3.3.2 isn't too clear on this but 0 seems ok
const TUint KMPEG12RTPAudioPayloadType = 14; //MPA MPEG12 PT RFC3551 6
const TUint KSbcRTPMediaPayloadHeaderLength = 1;//A2DP 4.3.4
const TUint KMPEG12RTPMediaPayloadHeaderLength = 4;//MPF-1 RFC2250 3.5 

//Frame header masks
const TUint8 KSbcFrameHeaderSyncWord = 0x9C;
const TUint8 KSbcFrameHeaderSamplingFrequencyMask = 0xC0;
const TUint8 KSbcFrameHeaderBlocksMask = 0x30;
const TUint8 KSbcFrameHeaderChannelModeMask = 0x0C;
const TUint8 KSbcFrameHeaderSubbandsMask = 0x01;
const TUint8 KMPEGAudioFrameHeaderSyncWord = 0xFF;
const TUint8 KMp3AudioFrameHeaderIdMask = 0xFE;//mask out CRC
const TUint8 KMp3AudioFrameHeaderId = 0xFA;
const TUint8 KMp3FrameHeaderSamplingFrequencyMask = 0x0C;
const TUint8 KMp3FrameHeaderBitRateIndexMask = 0xF0;

using namespace SymbianSBC;
using namespace SymbianBluetoothAV;	


typedef TUint8	TMPEG12SamplingFrequencyBitmask;
	
/**
Mask for configuring mp3 sampling frequencies
*/
enum TMPEG12SamplingFrequency
	{
	EMPEG12_16kHz		= 0x20,
	EMPEG12_22050Hz		= 0x10,
	EMPEG12_24kHz		= 0x8,
	EMPEG12_32kHz		= 0x4,
	EMPEG12_44100Hz		= 0x2,
	EMPEG12_48kHz		= 0x1
	};


/**
mp3 bit rate index table
*/	
enum TMp3BitRateIndex
	{
	EMp3BitRateIndex32000	= 0x01,
	EMp3BitRateIndex40000	= 0x02,
	EMp3BitRateIndex48000	= 0x03,
	EMp3BitRateIndex56000	= 0x04,
	EMp3BitRateIndex64000	= 0x05,
	EMp3BitRateIndex80000	= 0x06,
	EMp3BitRateIndex96000	= 0x07,
	EMp3BitRateIndex112000	= 0x08,
	EMp3BitRateIndex128000	= 0x09,
	EMp3BitRateIndex160000	= 0x0A,
	EMp3BitRateIndex192000	= 0x0B,
	EMp3BitRateIndex224000	= 0x0C,
	EMp3BitRateIndex256000	= 0x0D,
	EMp3BitRateIndex320000	= 0x0E
	};

/**
Utility that parses the codec capabilities to get the supported sample rates and channels

@internalComponent
*/
class TA2dpCodecCapabilityParser
	{
public:
	static TInt GetSupportedSampleRates(const TAvdtpMediaCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedDiscreteRates);
	static TInt GetSupportedChannels(const TAvdtpMediaCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport);
private:
	static TInt GetSupportedSBCSampleRates(const TSBCCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedDiscreteRates);
	static TInt GetSupportedMPEG12SampleRates(const TNonSBCCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedDiscreteRates);
	static TInt GetSupportedSBCChannels(const TSBCCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport);
	static TInt GetSupportedMPEG12Channels(const TNonSBCCodecCapabilities& aCodecCaps, RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport);
	static TInt GetSupportedChannelsCommonCode(TSBCChannelModeBitmask aChannelModesBitMask, RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport);
	};


/**
Utility to get codec specific RTP header parameters

@internalComponent
*/
class TRTPa2dpCodecSpecificUtils
	{
public:
	static TUint8 PayloadType(const TFourCC& aCodecDataType);
	static TUint MediaPayloadHeaderLength(const TFourCC& aCodecDataType);
	};


/**
Utility to calculate frame duration and time stamp increments

@internalComponent
*/	
class TFrameTimingUtils
	{
public:
	static TTimeIntervalMicroSeconds32 FrameDuration(TUint aFrameLength, TUint aBitRate);
	static TUint TimeStampIncrementPerFrame(const TFourCC& aCodecDataType, TUint aFrameLength, TUint aBitRate, TUint aSampleRate);
private:
	static TUint CalculateSBCTimeStampIncrementPerFrame(TUint aFrameLength, TUint aBitRate, TUint aSampleRate);
	static TUint CalculateMPEG12TimeStampIncrementPerFrame(TUint aFrameLength, TUint aBitRate);
	};


/**
Utility class to store the local SEP codec capabilities

@internalComponent
*/
NONSHARABLE_CLASS(CA2dpLocalCodecCapabilities) : public CBase
	{
public:
	static CA2dpLocalCodecCapabilities* NewL();
	~CA2dpLocalCodecCapabilities();
	TAvdtpMediaCodecCapabilities* LocalCodecCapabilities(const TFourCC& aCodecDataType);
private:
	CA2dpLocalCodecCapabilities();
	TSBCCodecCapabilities* LocalSBCCodecCapabilities();
	TNonSBCCodecCapabilities* LocalMPEG12CodecCapabilities();
private:
	TAvdtpMediaCodecCapabilities* iLocalCodecCapabilities; //this is owned
	};

	
/** 
Utiltiy class to store remote (ie headset) codec configuration 
and the local (ie symbian OS device) SBC codec configuration.
An instance of this class is owned by CA2dpBTHeadsetAudioInterface 
which sets the settings - the configuration is passed to the GAVDP state machine
which needs the configuration to configure the SEPs and RTPStreeamer uses
the configuration to calculate frame sizes and timings.

@internalComponent
*/
NONSHARABLE_CLASS(CA2dpAudioCodecConfiguration) : public CBase 
	{
public:
	static CA2dpAudioCodecConfiguration* NewL();
	~CA2dpAudioCodecConfiguration();
	void Reset();
	inline void SetHeadsetCodecDataType(const TFourCC& aHeadsetCodecDataType);
	inline void SetSampleRate(TUint aSampleRate);
	inline void SetChannels(TUint aChannels);
	inline void SetStereoSupport(TMMFStereoSupport aStereoSupport);
	inline const TFourCC& HeadsetCodecDataType() const;
	inline TUint SampleRate() const;
	inline TUint Channels() const;
	inline TMMFStereoSupport StereoSupport() const;
	TAvdtpMediaCodecCapabilities* UpdateRemoteCodecConfiguration(const TAvdtpMediaCodecCapabilities& aRemoteCodecCapabilities);
	TSBCFrameParameters& UpdateLocalSBCCodecConfiguration();
	inline TAvdtpMediaCodecCapabilities* RemoteCodecConfiguration() const;
	inline const TSBCFrameParameters& LocalSBCCodecConfiguration() const;
	TTimeIntervalMicroSeconds32 FrameDuration(TUint aFrameLength) const;
	TUint TimeStampIncrementPerFrame(TUint aFrameLength) const;
	TUint CalculateSBCBufferLength(TUint aPCM16BufferLength) const;
	void TEST_ForceRemoteSBCCodecConfiguration(const TSBCCodecCapabilities& aRemoteCodecConfiguration);
private:
	CA2dpAudioCodecConfiguration();
	TSBCCodecCapabilities* GetRemoteSBCCodecConfiguration(const TSBCCodecCapabilities& aCodecCaps);
	TNonSBCCodecCapabilities* GetRemoteMPEG12CodecConfiguration(const TNonSBCCodecCapabilities& aCodecCaps);
	TTimeIntervalMicroSeconds32 CalculateSBCFrameDuration() const;
	TTimeIntervalMicroSeconds32 CalculateMPEG12FrameDuration() const;
	TUint CalculateSBCTimeStampIncrementPerFrame() const;
	TUint CalculateMPEG12TimeStampIncrementPerFrame() const;
private:
	TFourCC iHeadsetCodecDataType;
	TUint	iSampleRate;
	TUint	iChannels;
	TMMFStereoSupport	iStereoSupport;
	TUint	iBitRate;
	TAvdtpMediaCodecCapabilities* iRemoteCodecConfiguration;
	TSBCFrameParameters iLocalSBCCodecConfiguration;
	TSBCCodecCapabilities* iForcedRemoteSBCCodecConfiguration;//just used for unit test purposes	
	};


/**
Utiltiy class to parse the header of non pcm16 data in order to obtain
the frame length and bit rate

@internalComponent
*/
NONSHARABLE_CLASS(CA2dpCodecFrameHeaderParser)
	{
public:
	static CA2dpCodecFrameHeaderParser* NewL(const TFourCC& aCodecDataType, const TDesC8& aHeader);
	~CA2dpCodecFrameHeaderParser();
	inline TUint FrameLength() const;
	inline TUint BitRate() const;
private:
	CA2dpCodecFrameHeaderParser();
	void ConstructL(const TFourCC& aCodecDataType, const TDesC8& aHeader);
	void ParseSBCHeaderL(const TDesC8& aHeader);
	void ParseMPEG12HeaderL(const TDesC8& aHeader);
private:
	TUint iFrameLength;
	TUint iSampleRate;
	TUint iBitRate;
	};

#include "A2dpCodecUtilities.inl"
		
#endif
	
