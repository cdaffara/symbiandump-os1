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

#include "pcmoutputport.h"
#include "pcmprocessingunit.h"
#include <mdf/mdfpuconfig.h>

const TUint KSampleRate 	= 8000;
const TUint KMonoChannel 	= 1;
const TUint KStereoChannel	= 2;


CPcmOutputPort* CPcmOutputPort::NewL(TInt aIndex, COmxProcessingUnit* aParent, TPcmDataType aDataType)
	{
	CPcmOutputPort* self = new (ELeave) CPcmOutputPort(aDataType);
	CleanupStack::PushL (self);
	self->ConstructL(aIndex, aParent);
	CleanupStack::Pop();
	return self;
	}
		
void CPcmOutputPort::ConstructL(TInt aIndex, COmxProcessingUnit* aParent)
	{
	COmxOutputPort::ConstructL(aIndex, aParent);
	}

CPcmOutputPort::CPcmOutputPort(TPcmDataType aDataType) :
	COmxOutputPort(),
	iSampleRate(KSampleRate),
	iChannels(KMonoChannel),
	iInterleaved(EFalse),
	iDataType(aDataType)
	{
	}

TInt CPcmOutputPort::MopConfigure(const TPuConfig&  aConfig)
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

void CPcmOutputPort::MopInitialize()	
 	{
 	COmxOutputPort::MopInitialize();
	OMX_AUDIO_PARAM_PCMMODETYPE pcm;
	OMX_VERSIONTYPE ver = 
		{
		1,0
		};
	pcm.nVersion = ver;
	pcm.nSize = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
	pcm.nPortIndex = MopIndex();
	pcm.nSamplingRate = iSampleRate;
	pcm.ePCMMode = OMX_AUDIO_PCMModeLinear;
	pcm.nChannels = iChannels;
	pcm.bInterleaved = (OMX_BOOL)iInterleaved;
		
	switch (iDataType)
		{
	case EPCM8:
		pcm.nBitPerSample = 8;
		pcm.eNumData = OMX_NumericalDataSigned; 
		break;
	case EPCMU8:
		pcm.nBitPerSample = 8;
		pcm.eNumData = OMX_NumericalDataUnsigned; 
		break;
	case EPCM16:
		pcm.nBitPerSample = 16;
		pcm.eNumData = OMX_NumericalDataSigned; 
		break;
		}

	// Set Input Port
	Component()->OmxSetParameter(OMX_IndexParamAudioPcm, &pcm);
 	}
