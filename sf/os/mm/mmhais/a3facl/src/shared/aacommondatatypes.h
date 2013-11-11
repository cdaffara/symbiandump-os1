/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef AACOMMONDATATYPES_H
#define AACOMMONDATATYPES_H

//  INCLUDES
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfbase.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

const TInt KDeviceMaxVolume = 10;
const TInt KDevsoundMaxGain = 1;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
enum TAudioInputPreference
	{
	ENoPref             = 0x00,
	EInputDefaultMic    = 0x01,
	EInputFMRadio       = 0x02,
	EInputSpeakerOutput = 0x04,
	EInputVoiceCall     = 0x08,
	EInputLineIn        = 0x10
	};

enum TAudioOutputPreference
	{
	EOutputNone         = 0x00,
	EOutputDefault      = 0x01,
	EOutputPublic       = 0x02,
	EOutputPrivate      = 0x04,
	EOutputAll          = 0x08
	};

struct TAudioFormat
	{
	TFourCC format;
	TUint16 extension;
	};

enum TBitsInSample
	{
	E16BitsInSample,
	E20BitsInSample,
	E24BitsInSample,
	E32BitsInSample,
	ENotDefined = 0xFF
	};

enum TAudioType
	{
	EMedia, // to be removed
	ESpeech, // to be removed
	EDefaultAudio,
	ERingingTone,
	ECsCall,
	EVoip,
	ERadio,
	EKeyTone,
	ESind,
	EPoc,
	EStereoBt,
	EVisualization,
	EVoipNonSignaling,
	ENotification,
	EVideoCall,
	EKodiakPoc
	};

enum TCodecLocation
	{
	ECodecLocationMCU,
	ECodecLocationADSP,
	ECodecLocationCDSP,
	ECodecLocationUndefined
	};

// buffer size parameters
enum TBufferSize
	{
	EBufferSizeSmall,
	EBufferSizeMedium,
	EBufferSizeLarge,
	EBufferSizeCustom
	};

#ifndef _DEVSOUND_DATATYPES_DEFINED_
#define _DEVSOUND_DATATYPES_DEFINED_

enum TMMFSampleRate
	{
	EMMFSampleRate8000Hz  = 0x00000001,	///< 8000 Hz Sampling Rate
	EMMFSampleRate11025Hz = 0x00000002,	///< 11025 Hz Sampling Rate
	EMMFSampleRate16000Hz = 0x00000004,	///< 16000 Hz Sampling Rate
	EMMFSampleRate22050Hz = 0x00000008,	///< 22050 Hz Sampling Rate
	EMMFSampleRate32000Hz = 0x00000010,	///< 32000 Hz Sampling Rate
	EMMFSampleRate44100Hz = 0x00000020,	///< 44100 Hz Sampling Rate
	EMMFSampleRate48000Hz = 0x00000040,	///< 48000 Hz Sampling Rate
	EMMFSampleRate88200Hz = 0x00000080,	///< 88200 Hz Sampling Rate
	EMMFSampleRate96000Hz = 0x00000100,	///< 96000 Hz Sampling Rate
	EMMFSampleRate12000Hz = 0x00000200,	///< 12000 Hz Sampling Rate
	EMMFSampleRate24000Hz = 0x00000400,	///< 24000 Hz Sampling Rate
	EMMFSampleRate64000Hz = 0x00000800
	};

enum TMMFMonoStereo
	{
	EMMFMono	= 0x00000001,	///< Mono mode
	EMMFStereo	= 0x00000002	///< Stereo mode
	};

class TMMFCapabilities
	{
public:
	/** Sampling Rates supported
	*/
	TUint	iRate;
	/** Encodings supported
	*/
	TUint	iEncoding;
	/** Mono/Stereo support
	*/
	TUint	iChannels;
	/** Buffer size supported
	*/
	TInt	iBufferSize;
	};

#endif // _DEVSOUND_DATATYPES_DEFINED_

struct TAudioStreamParams
	{
	// Sample rate used in data pipe
	TMMFSampleRate iSampleRate;

	// Stereo mode used in data pipe
	TMMFMonoStereo iStereoMode;

	// Type of stream audio
	TAudioType iStreamType;

	// Playback / Record / Tone
	TMMFState iStreamState;

	// Stream audio format
	TFourCC iFormat;

	TCodecLocation iCodecLocation;

	TBufferSize iBufferSize;
	};

// allocation info for client
class TAllocationInfo
	{
	public : // Member data
		// Audio type. e.g. ECsCall
		TAudioType 	iAudioType;

		// Unique policy id for each client
		TUint32 	iPolicyId;

		// buffer size information
		TBufferSize iBufferSize;

		// Codec location
		TCodecLocation iCodecLocation;

		// Codec Uid
		TUid iCodecUid;
	};

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// CLASS DECLARATION

#endif /* AACOMMONDATATYPES_H */

// End of File
