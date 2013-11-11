// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <mdf/mdfpuconfig.h>
#include <omxoutputport.h>

#include "vorbisdecoderoutputport.h"
#include "vorbisdecoderprocessingunit.h"

#include <e32debug.h>

const TInt KPcm16BitsPerSample = 16;

// #define VORBISDECODEROUTPUTPORT_DEBUG	1
#if defined(VORBISDECODEROUTPUTPORT_DEBUG)
#define DEBUG_PRINT RDebug::Print
#else
#define DEBUG_PRINT
#endif

const TUint KSampleRate 	= 44100;
const TUint KMonoChannel 	= 1;
const TUint KStereoChannel	= 2;

CVorbisDecoderOutputPort* CVorbisDecoderOutputPort::NewL(TInt aIndex, CVorbisDecoderProcessingUnit* aComponent)
	{
	CVorbisDecoderOutputPort* self = new (ELeave) CVorbisDecoderOutputPort;
	CleanupStack::PushL (self);
	self->ConstructL(aIndex, aComponent);
	CleanupStack::Pop();
	return self;
	}

CVorbisDecoderOutputPort::CVorbisDecoderOutputPort() :
	iSampleRate(KSampleRate),
	iChannels(KStereoChannel),
	iInterleaved(EFalse)
	{
	}



TInt CVorbisDecoderOutputPort::MopConfigure(const TPuConfig& aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidTTaskConfig))
		{
		const TTaskConfig* config = TPuTaskConfig::GetStructure(aConfig);	
	
		iSampleRate = config->iRate;
		iChannels = (config->iStereoMode & ETaskMono)? KMonoChannel : KStereoChannel;
		iInterleaved = (config->iStereoMode & ETaskInterleaved)?ETrue : EFalse;
		return KErrNone;
		}
	return KErrNotSupported;	
	}
	
void CVorbisDecoderOutputPort::MopInitialize()
	{
	COmxOutputPort::MopInitialize();
	
	OMX_VERSIONTYPE ver = 
		{
		1,0
		};

	// Set Output Port (PCM16)
	OMX_AUDIO_PARAM_PCMMODETYPE pcm;
	pcm.nVersion = ver;
	pcm.nSize = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
	pcm.nPortIndex = KVorbisDecoderOutputPortIndex;
	pcm.nSamplingRate = iSampleRate;
	pcm.ePCMMode = OMX_AUDIO_PCMModeLinear;
	pcm.eNumData = OMX_NumericalDataSigned; 
	pcm.nBitPerSample = KPcm16BitsPerSample;
	pcm.nChannels = iChannels;
	pcm.bInterleaved = (OMX_BOOL)iInterleaved;
	
	Component()->OmxSetParameter(OMX_IndexParamAudioPcm, &pcm);
	}

// end
