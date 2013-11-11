// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Common\Audio.h
// 
//

#ifndef __MDA_COMMON_AUDIO_H__
#define __MDA_COMMON_AUDIO_H__

// Standard EPOC32 includes
#include <e32base.h>

// Public Media Server includes
#include <mda/common/base.h>
#include <mda/common/resource.h>
//#include <mda/common/controller.h>
#include <mda/common/audio.hrh>
#include <mda/common/audiostream.hrh>

MDA_UID(KUidMdaPrioritySettings)
MDA_UID(KUidMdaMediaTypeAudio) // Audio data type Uid
MDA_UID(KUidMdaLocalSoundDevice) // Local audio stream location
MDA_UID(KUidMdaClipLocationMaxWriteLength)
MDA_UID(KUidMdaTelephonySoundDevice) // Telephony audio stream location
MDA_UID(KUidMdaClientPCMSoundDevice) // Client PCM audio stream location
MDA_UID(KUidMdaToneGeneratorDevice) // Tone Generator Device
MDA_UID(KUidMdaClipFormatWav) // Wav Audio Clip Format Uid

MDA_UID(KUidMdaAudioOutputStreamDevice) // Output Stream Device

MDA_UID(KUidMdaWavCodec) // Wav Codec Type Uid
MDA_UID(KUidMdaWavPcmCodec) // Wav Codec Uids
MDA_UID(KUidMdaWavAlawCodec) 
MDA_UID(KUidMdaWavMulawCodec) 
MDA_UID(KUidMdaWavImaAdpcmCodec) 


MDA_UID(KUidMdaClipFormatAu) // Sun/Next "Au" Audio Clip format Uid
MDA_UID(KUidMdaAuCodec) // Sun/Next "Au" Audio Codec Type Uid
MDA_UID(KUidMdaAuMulawCodec) // Sun/Next "Au" Audio Codec Uids
MDA_UID(KUidMdaAu8PcmCodec)  
MDA_UID(KUidMdaAu16PcmCodec) 
MDA_UID(KUidMdaAuAlawCodec)

MDA_UID(KUidMdaClipFormatRawAudio) // Raw Audio Clip format Uid
MDA_UID(KUidMdaRawAudioCodec) // Raw Audio Codec Type Uid
MDA_UID(KUidMdaRawAudioMulawCodec)
MDA_UID(KUidMdaRawAudioAlawCodec) 
MDA_UID(KUidMdaRawAudioS8PcmCodec) // Signed 8bit
MDA_UID(KUidMdaRawAudioU8PcmCodec) // Unsigned 8bit
MDA_UID(KUidMdaRawAudioSL16PcmCodec) // Signed little-endian 16bit
MDA_UID(KUidMdaRawAudioSB16PcmCodec) // Signed big-endian 16bit
MDA_UID(KUidMdaRawAudioUL16PcmCodec) // Unsigned little-endian 16bit
MDA_UID(KUidMdaRawAudioUB16PcmCodec) // Unsigned big-endian 16bit

MDA_UID(KUidMdaTelephonyStateQuery) // Telephony state query 
MDA_UID(KUidMdaAudioStreamVolumeRamp) // Audio stream volume ramp 

MDA_UID(KUidMdaDataTypeSettings)

MDA_UID(KUidMdaClipFormatRawAmr) // AMR Audio Clip Format Uid
MDA_UID(KUidMdaRawAmrCodec) // AMR Codec Type Uid

/**
@internalComponent
*/
const TInt KMdaClipLocationMaxWriteLengthNone = -1;

// Package classes

/**
@publishedAll
@released

Generic Audio Parameters
*/
class TMdaPrioritySettings : public TMdaPackage
	{
public:
	inline TMdaPrioritySettings();
public:
	/**
	Absolute priority of a client of the MMF Server.

	Used by the policy server to determine which client should gain access to the sound device.

	The priority which should be an integer in the range -100 to +100.
	*/
	TInt iPriority;
	/**
    The priority preference that expresses the nature of the priority that can be none, 
	time (or speed), quality or both time and quality.
	
	 Values are known to the adaptation and will be iterpreted accordingly.

	*/
	TInt iPref;
	};

/**
@publishedAll
@deprecated

Datatype parameters.

Base class for specific data type config packages
*/
class TMdaDatatypeSettings : public TMdaPackage
	{
protected:
	inline TMdaDatatypeSettings(TUid aUid,TInt aDerivedSize);
	};

inline TMdaDatatypeSettings::TMdaDatatypeSettings(TUid aUid,TInt aDerivedSize) :
TMdaPackage(KUidMdaDataTypeSettings,aUid,aDerivedSize) {}

/**
@publishedAll
@released

The settings for audio data played through the media server.

These can be the settings for an audio clip, for streamed audio data, or can be the capabilities
of the sound device. The sample rate and number of channels apply to the audio sample, the maximum
volume applies to the output device and the volume setting can apply to either, depending on the
device. An object of this class is a public data member of class CMdaAudioType and also can be
passed to CMdaAudioOutputStream::Open().


@see CMdaAudioType
@see CMdaAudioOutputStream::Open()
*/
class TMdaAudioDataSettings : public TMdaDatatypeSettings
	{
public:
	/**
	Audio capabilities, which include the sample rate and number of channels. The iCaps
	member is a combination of these flags.
	*/
	enum TAudioCaps
		{

		// Sampled audio capabilities

		/** If specified, the sample rate iSampleRate is the only one supported. If not
		specified, multiple sample rates may be supported.
		*/
		ESampleRateFixed =		0x00000001,

		/** If specified, all sample rates between the minimum and the maximum are supported.
		*/
		ESampleRateAnyInRange = 0x00000002,

		/** The audio sample supports a sample rate of 8.000KHz.
		*/
		ESampleRate8000Hz =		0x00000010,

		/** The audio sample supports a sample rate of 11.025KHz.
		*/
		ESampleRate11025Hz =	0x00000040,

		/** The audio sample supports a sample rate of 12.000KHz.
		*/
		ESampleRate12000Hz =	0x00000080,

		/** The audio sample supports a sample rate of 16.000KHz.
		*/
		ESampleRate16000Hz =	0x00000100,

		/** The audio sample supports a sample rate of 22.000KHz.
		*/
		ESampleRate22050Hz =	0x00000400,

		/** The audio sample supports a sample rate of 24.000KHz.
		*/
		ESampleRate24000Hz =	0x00000800,

		/** The audio sample supports a sample rate of 32.000KHz.
		*/
		ESampleRate32000Hz =	0x00001000,

		/** The audio sample supports a sample rate of 44.100KHz.
		*/
		ESampleRate44100Hz =	0x00004000,

		/** The audio sample supports a sample rate of 48.000KHz.
		*/
		ESampleRate48000Hz =	0x00010000,

		/** The audio sample supports a sample rate of 96.000KHz.
		*/
		ESampleRate96000Hz =	0x00020000,

		/** The audio sample supports a sample rate of 64.000KHz.
		*/
		ESampleRate64000Hz =	0x00040000,

		// Channel related - applies to sampled

		/** The audio sample supports mono.
		*/
		EChannelsMono =			0x02000000,

		/** The audio sample supports stereo.
		*/
		EChannelsStereo =		0x04000000,

		/** Routing related
		*/
		ERoutingControl =		0x08000000,

		/** True if data flow is synchronised with real time (e.g. stream)
		*/
		ERealTime =				0x10000000
		};

	/** Mutually exclusive flags that specify whether audio data is sent to the local device (speakers)
	or the network (phone line) or both. The iFlags member uses these flags.
	*/
	enum TAudioFlags
		{

		/** The sound data is only sent to the network.
		*/
		ENoLocalRouting =		0x00000001,

		/** The sound data is only sent to the local device.
		*/
		ENoNetworkRouting =		0x00000002
		};
public:
	inline TMdaAudioDataSettings();
	inline void Query(); // Call this when you just want to have caps and maxvolume filled
public:
	// Fixed values - must be queried

	/**
	Specifies the capabilities of the audio sample. Its value is a combination of the flags
	contained in the TAudioCaps enum.
	*/
	TInt iCaps;

	/**
	The maximum volume of the audio device.
	*/
	TInt iMaxVolume; // One means fixed volume (zero not valid)

	// Variables
	/**
	The sample rate of the audio sample.
	*/
	TInt iSampleRate;

	/**
	The number of channels for the audio sample.
	*/
	TInt iChannels;

	/**
	The volume. Whether this applies to the audio device or to the audio sample is device-dependent.
	*/
	TInt iVolume;

	/**
	The flags. Its value can be either of the flags contained in the TAudioFlags enum.
	*/
	TInt iFlags;
	};

// Wav Config Packages

/**
@publishedAll
@deprecated

The format for .wav audio clips.

An object of this type can be passed to CMdaAudioRecorderUtility::OpenL() or CMdaAudioConvertUtility::OpenL().
*/
class TMdaWavClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaWavClipFormat();
	};

/**
@publishedAll
@deprecated

The base class for codecs used by .wav audio files.

An object derived from this class can be used to specify the compression algorithm to use when recording or 
converting a .wav audio clip. When playing back a .wav clip, the codec is not specified: the media server
identifies the correct one to use.
*/
class TMdaWavCodec : public TMdaPackage
	{
public:
	inline TMdaWavCodec(); // Unknown type
protected:
	inline TMdaWavCodec(TUid aUid,TInt aDerivedSize);
	};

/**
@publishedAll
@deprecated

The codec for uncompressed 8 or 16 bit PCM .wav format sound data.

This codec copies the data unchanged from source to destination.
*/
class TMdaPcmWavCodec : public TMdaWavCodec
	{
public:

	/**
	Indicates whether the sound is sampled using 8 or 16 bits.
	*/
	enum TBits
		{
   		/** The sound is sampled using 8 bits.
		*/
		E8BitPcm=0,

   		/** The sound is sampled using 16 bits.
		*/
		E16BitPcm=1
		};
public:
	inline TMdaPcmWavCodec(TBits aBits=E16BitPcm);
public:
	/**
	Indicates whether the sound is sampled using 8 or 16 bits.
	*/
	TBits iBits;
	};

/**
@publishedAll
@deprecated

The codec for .wav audio data compressed using the A-Law algorithm.
*/
class TMdaAlawWavCodec : public TMdaWavCodec
	{
public:
	inline TMdaAlawWavCodec();
	};

/**
@publishedAll
@deprecated

The codec for .wav audio data compressed using the mu-law algorithm.
*/
class TMdaMulawWavCodec : public TMdaWavCodec
	{
public:
	inline TMdaMulawWavCodec();
	};

/**
@publishedAll
@deprecated

The codec for .wav audio data compressed using the IMA-ADPCM algorithm.
*/
class TMdaImaAdpcmWavCodec : public TMdaWavCodec
	{
public:
	inline TMdaImaAdpcmWavCodec();
	};


// Sun/NeXT "Au" Config Packages
/**
@publishedAll
@deprecated

The format for .au audio clips.

An object of this type can be passed to CMdaAudioRecorderUtility::OpenL() or CMdaAudioConvertUtility::OpenL().
*/
class TMdaAuClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaAuClipFormat();
	};

/**
@publishedAll
@deprecated

The base class for codecs used by .au audio files.

An object derived from this class can be used to specify the compression algorithm to use when recording
or converting a .au audio clip (whose format is TMdaAuClipFormat). When playing back a .au clip, the
codec is not specified: the media server identifies the correct one to use.
*/
class TMdaAuCodec : public TMdaPackage
	{
public:
	inline TMdaAuCodec(); // Unknown type
protected:
	inline TMdaAuCodec(TUid aUid,TInt aDerivedSize);
	};

/**
@publishedAll
@deprecated

The codec for 8 bit PCM .au audio data.
*/
class TMdaPcm8BitAuCodec : public TMdaAuCodec
	{
public:
	inline TMdaPcm8BitAuCodec();
	};

/**
@publishedAll
@deprecated

The codec for 16 bit PCM .au audio data.
*/
class TMdaPcm16BitAuCodec : public TMdaAuCodec
	{
public:
	inline TMdaPcm16BitAuCodec();
	};

/**
@publishedAll
@deprecated

The codec for .au audio data compressed using the A-Law algorithm.
*/
class TMdaAlawAuCodec : public TMdaAuCodec
	{
public:
	inline TMdaAlawAuCodec();
	};

/**
@publishedAll
@deprecated

The codec for .au audio data compressed using the mu-law algorithm.
*/
class TMdaMulawAuCodec : public TMdaAuCodec
	{
public:
	inline TMdaMulawAuCodec();
	};

// Raw Audio Config Packages
/**
@publishedAll
@deprecated

Audio format for raw audio clips.

An object of this type can be passed to CMdaAudioRecorderUtility::OpenL() or CMdaAudioConvertUtility::OpenL().
*/
class TMdaRawAudioClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaRawAudioClipFormat();
	};

/**
@publishedAll
@deprecated

The base class for raw audio codecs.

An object derived from this class can be used to specify the algorithm to use when opening a
raw audio clip (whose format is TMdaRawAudioClipFormat).

Raw audio clips are have no header information, so that when opening one, the user must supply
a codec and a package containing the settings appropriate for the data in the clip.

@see TMdaRawAudioClipFormat
@see CMdaAudioConvertUtility::OpenL()
@see CMdaAudioRecorderUtility::OpenL()
*/
class TMdaRawAudioCodec : public TMdaPackage
	{
public:
	inline TMdaRawAudioCodec(); // Unknown type
protected:
	inline TMdaRawAudioCodec(TUid aUid,TInt aDerivedSize);
	};

/**
@publishedAll
@deprecated

The codec for raw audio data compressed using the mu-law algorithm.
*/
class TMdaMulawRawAudioCodec : public TMdaRawAudioCodec
	{
public:
	inline TMdaMulawRawAudioCodec();
	};

/**
@publishedAll
@deprecated

The codec for raw audio data compressed using the A-Law algorithm.
*/
class TMdaAlawRawAudioCodec : public TMdaRawAudioCodec
	{
public:
	inline TMdaAlawRawAudioCodec();
	};

/**
@publishedAll
@deprecated

The codec for signed 8 bit PCM raw audio data.
*/
class TMdaS8PcmRawAudioCodec : public TMdaRawAudioCodec
	{
public:
	inline TMdaS8PcmRawAudioCodec();
	};

/**
@publishedAll
@deprecated

The codec for unsigned 8 bit PCM raw audio data.
*/
class TMdaU8PcmRawAudioCodec : public TMdaRawAudioCodec
	{
public:
	inline TMdaU8PcmRawAudioCodec();
	};

/**
@publishedAll
@deprecated

The codec for signed little-endian 16 bit PCM raw audio data.
*/
class TMdaSL16RawAudioCodec : public TMdaRawAudioCodec
	{
public:
	inline TMdaSL16RawAudioCodec();
	};

/**
@publishedAll
@deprecated

The codec for signed big-endian 16 bit PCM raw audio data.
*/
class TMdaSB16RawAudioCodec : public TMdaRawAudioCodec
	{
public:
	inline TMdaSB16RawAudioCodec();
	};

/**
@publishedAll
@deprecated

The codec for unsigned little-endian 16 bit PCM raw audio data.
*/
class TMdaUL16RawAudioCodec : public TMdaRawAudioCodec
	{
public:
	inline TMdaUL16RawAudioCodec();
	};

/**
@publishedAll
@deprecated

The codec for unsigned big-endian 16 bit PCM raw audio data.
*/
class TMdaUB16RawAudioCodec : public TMdaRawAudioCodec
	{
public:
	inline TMdaUB16RawAudioCodec();
	};

/**
@publishedAll
@deprecated

Local Audio Stream Device
Control local microphone/speaker audio stream
May not function during telephone call
*/
class TMdaLocalAudioDevice : public TMdaStreamDevice
	{
public:
	inline TMdaLocalAudioDevice();
	};

/**
@publishedAll
@deprecated

Telephony Audio Stream Device
Control telephony microphone+downlink/speaker+uplink audio stream
Only functional during telephone call
*/
class TMdaTelephonyAudioDevice : public TMdaStreamDevice
	{
public:
	inline TMdaTelephonyAudioDevice();
	};

/**
@publishedAll
@deprecated

This class is used in a ResourceConfig on a telephony port
to determine the current state of telephony
*/
class TMdaTelephonyStateQuery : public TMdaPackage
	{
public:
	inline TMdaTelephonyStateQuery();
public:
	TBool iIsTelephonyActive;
	};

/**
@publishedAll
@deprecated

Client PCM Audio Stream Device
Used to configure a 16bit PCM audio stream to/from client
*/
class TMdaClientPCMAudioDevice : public TMdaStreamDevice
	{
public:
	inline TMdaClientPCMAudioDevice();
	};


/**
@publishedAll
@deprecated

Volume ramping control on audio stream devices
*/
class TMdaAudioStreamVolumeRamp : public TMdaPackage
	{
public:
	inline TMdaAudioStreamVolumeRamp();
public:
	TTimeIntervalMicroSeconds iDuration;
	};


// Inlines

/** 
Default constructor. The member data is not initialised.
*/
inline TMdaAudioDataSettings::TMdaAudioDataSettings() :
TMdaDatatypeSettings(KUidMdaMediaTypeAudio,sizeof(TMdaAudioDataSettings)) 
	{
	Mem::FillZ(&iSampleRate,sizeof(TInt)*4);
	}

/**
Sets the number of channels, the sample rate and the volume to KMdaUnknown (–1). Call this function when you 
only want to have the iCaps and iMaxVolume members filled.
*/
inline void TMdaAudioDataSettings::Query()
	{
	iChannels = KMdaUnknown;
	iSampleRate = KMdaUnknown;
	iVolume = KMdaUnknown;
	}// Call this when you just want to have caps and maxvolume filled

/**
The default constructor initialises the object so that it can be identified by the media server.
*/
inline TMdaWavClipFormat::TMdaWavClipFormat() :
TMdaClipFormat(KUidMdaClipFormatWav,sizeof(TMdaWavClipFormat)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaWavCodec::TMdaWavCodec() :
TMdaPackage(KUidMdaWavCodec,KNullUid,sizeof(TMdaWavCodec)) {}

inline TMdaWavCodec::TMdaWavCodec(TUid aUid, TInt aDerivedSize) :
TMdaPackage(KUidMdaWavCodec,aUid,aDerivedSize) {}

/**
Constructs the package with a flag that indicates whether the sound is sampled using 8 or 16 bits.

@param  aBits
        Indicates whether the sound is sampled using 8 or 16 bits.
*/
inline TMdaPcmWavCodec::TMdaPcmWavCodec(TBits aBits) :
TMdaWavCodec(KUidMdaWavPcmCodec,sizeof(TMdaPcmWavCodec)), iBits(aBits) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaAlawWavCodec::TMdaAlawWavCodec() :
TMdaWavCodec(KUidMdaWavAlawCodec,sizeof(TMdaAlawWavCodec)) {}

/**
The default constructor initialises the object so that it can be identified by the media server.
*/
inline TMdaMulawWavCodec::TMdaMulawWavCodec() :
TMdaWavCodec(KUidMdaWavMulawCodec,sizeof(TMdaMulawWavCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaImaAdpcmWavCodec::TMdaImaAdpcmWavCodec() :
TMdaWavCodec(KUidMdaWavImaAdpcmCodec,sizeof(TMdaImaAdpcmWavCodec)) {}

/**
The default constructor initialises the object so that it can be identified by the media server.
*/
inline TMdaAuClipFormat::TMdaAuClipFormat() :
TMdaClipFormat(KUidMdaClipFormatAu,sizeof(TMdaAuClipFormat)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaAuCodec::TMdaAuCodec() :
TMdaPackage(KUidMdaAuCodec,KNullUid,sizeof(TMdaAuCodec)) {}

inline TMdaAuCodec::TMdaAuCodec(TUid aUid,TInt aDerivedSize) :
TMdaPackage(KUidMdaAuCodec,aUid,aDerivedSize) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaPcm16BitAuCodec::TMdaPcm16BitAuCodec() :
TMdaAuCodec(KUidMdaAu16PcmCodec,sizeof(TMdaPcm16BitAuCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaPcm8BitAuCodec::TMdaPcm8BitAuCodec() :
TMdaAuCodec(KUidMdaAu8PcmCodec,sizeof(TMdaPcm8BitAuCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaAlawAuCodec::TMdaAlawAuCodec() :
TMdaAuCodec(KUidMdaAuAlawCodec,sizeof(TMdaAlawAuCodec)) {}

/**
The default constructor initialises the object so that it can be identified by the media server.
*/
inline TMdaMulawAuCodec::TMdaMulawAuCodec() :
TMdaAuCodec(KUidMdaAuMulawCodec,sizeof(TMdaMulawAuCodec)) {}

/**
The default constructor initialises the object so that it can be identified by the media server.
*/
inline TMdaRawAudioClipFormat::TMdaRawAudioClipFormat() :
TMdaClipFormat(KUidMdaClipFormatRawAudio,sizeof(TMdaRawAudioClipFormat)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaRawAudioCodec::TMdaRawAudioCodec() :
TMdaPackage(KUidMdaRawAudioCodec,KNullUid,sizeof(TMdaRawAudioCodec)) {}

inline TMdaRawAudioCodec::TMdaRawAudioCodec(TUid aUid,TInt aDerivedSize) :
TMdaPackage(KUidMdaRawAudioCodec,aUid,aDerivedSize) {}

/**
The default constructor initialises the object so that it can be identified by the media server.
*/
inline TMdaMulawRawAudioCodec::TMdaMulawRawAudioCodec() :
TMdaRawAudioCodec(KUidMdaRawAudioMulawCodec,sizeof(TMdaMulawRawAudioCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaAlawRawAudioCodec::TMdaAlawRawAudioCodec() :
TMdaRawAudioCodec(KUidMdaRawAudioAlawCodec,sizeof(TMdaAlawRawAudioCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaS8PcmRawAudioCodec::TMdaS8PcmRawAudioCodec() :
TMdaRawAudioCodec(KUidMdaRawAudioS8PcmCodec,sizeof(TMdaS8PcmRawAudioCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaU8PcmRawAudioCodec::TMdaU8PcmRawAudioCodec() :
TMdaRawAudioCodec(KUidMdaRawAudioU8PcmCodec,sizeof(TMdaU8PcmRawAudioCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaSL16RawAudioCodec::TMdaSL16RawAudioCodec() :
TMdaRawAudioCodec(KUidMdaRawAudioSL16PcmCodec,sizeof(TMdaSL16RawAudioCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaSB16RawAudioCodec::TMdaSB16RawAudioCodec() :
TMdaRawAudioCodec(KUidMdaRawAudioSB16PcmCodec,sizeof(TMdaSB16RawAudioCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaUL16RawAudioCodec::TMdaUL16RawAudioCodec() :
TMdaRawAudioCodec(KUidMdaRawAudioUL16PcmCodec,sizeof(TMdaUL16RawAudioCodec)) {}

/**
The default constructor initialises the package so that it can be identified by the media server.
*/
inline TMdaUB16RawAudioCodec::TMdaUB16RawAudioCodec() :
TMdaRawAudioCodec(KUidMdaRawAudioUB16PcmCodec,sizeof(TMdaUB16RawAudioCodec)) {}

inline TMdaLocalAudioDevice::TMdaLocalAudioDevice() : 
TMdaStreamDevice(KUidMdaLocalSoundDevice,sizeof(TMdaLocalAudioDevice)) {}

inline TMdaTelephonyAudioDevice::TMdaTelephonyAudioDevice() : 
TMdaStreamDevice(KUidMdaTelephonySoundDevice,sizeof(TMdaTelephonyAudioDevice)) {}

inline TMdaClientPCMAudioDevice::TMdaClientPCMAudioDevice() : 
TMdaStreamDevice(KUidMdaClientPCMSoundDevice,sizeof(TMdaClientPCMAudioDevice)) {}

inline TMdaAudioStreamVolumeRamp::TMdaAudioStreamVolumeRamp() :
TMdaPackage(KUidMdaAudioStreamVolumeRamp,KUidMdaAudioStreamVolumeRamp,sizeof(TMdaAudioStreamVolumeRamp)) {}

inline TMdaTelephonyStateQuery::TMdaTelephonyStateQuery() :
TMdaPackage(KUidMdaTelephonySoundDevice,KUidMdaTelephonyStateQuery,sizeof(TMdaTelephonyStateQuery)) {}

inline TMdaPrioritySettings::TMdaPrioritySettings() :
TMdaPackage(KUidMdaPrioritySettings,KUidMdaPrioritySettings,sizeof(TMdaPrioritySettings)) {}

#endif
