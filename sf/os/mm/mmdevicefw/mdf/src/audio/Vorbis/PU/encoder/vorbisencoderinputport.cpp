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

#include <omxinputport.h>
#include <mdf/mdfpuconfig.h>

#include "vorbisencoderinputport.h"
#include "vorbisencoderprocessingunit.h"

#include <e32debug.h>

// #define VORBISENCODERINPUTPORT_DEBUG	1
#if defined(VORBISENCODERINPUTPORT_DEBUG)
#define DEBUG_PRINT RDebug::Print
#else
#define DEBUG_PRINT
#endif

const TUint KSampleRate 	= 44100;
const TUint KMonoChannel 	= 1;
const TUint KStereoChannel	= 2;

CVorbisEncoderInputPort* CVorbisEncoderInputPort::NewL(TInt aIndex, CVorbisEncoderProcessingUnit* aParent)
	{
	CVorbisEncoderInputPort* self = new (ELeave) CVorbisEncoderInputPort;
	CleanupStack::PushL(self);
	self->ConstructL(aIndex, aParent);
	CleanupStack::Pop();
	return self;
	}
	
CVorbisEncoderInputPort::CVorbisEncoderInputPort() :
	iSampleRate(KSampleRate),
	iChannels(KStereoChannel),
	iInterleaved(EFalse)
	{
	}
	
	
TInt CVorbisEncoderInputPort::MipConfigure(const TPuConfig& aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidTTaskConfig))
		{
		const TTaskConfig* config = TPuTaskConfig::GetStructure(aConfig);	
	
		iSampleRate = config->iRate;
		iChannels = (config->iStereoMode & ETaskMono)? KMonoChannel : KStereoChannel;
		iInterleaved = (config->iStereoMode & ETaskInterleaved)?ETrue : EFalse;

		return KErrNone;
		}
	else 
		{
		return KErrNotSupported;	
		}
	}
	
void CVorbisEncoderInputPort::MipInitialize()
	{
	COmxInputPort::MipInitialize();
	OMX_VERSIONTYPE ver = 
		{
		1,0
		};

	// Set Input Port (PCM16)
	OMX_AUDIO_PARAM_PCMMODETYPE pcm;
	pcm.nVersion = ver;
	pcm.nSize = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
	pcm.nPortIndex = 0;
	pcm.nSamplingRate = iSampleRate;
	pcm.ePCMMode = OMX_AUDIO_PCMModeLinear;
	pcm.eNumData = OMX_NumericalDataSigned; 
	pcm.nBitPerSample = 16;
	pcm.nChannels = iChannels;
	pcm.bInterleaved = (OMX_BOOL)iInterleaved;
	
	Component()->OmxSetParameter(OMX_IndexParamAudioPcm, &pcm);		
	}


// end

