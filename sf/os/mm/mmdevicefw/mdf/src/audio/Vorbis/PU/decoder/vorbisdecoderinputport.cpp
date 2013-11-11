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

#include "vorbisdecoderinputport.h"
#include "vorbisdecoderprocessingunit.h"

#include <e32debug.h>

// #define VORBISDECODERINPUTPORT_DEBUG	1
#if defined(VORBISDECODERINPUTPORT_DEBUG)
#define DEBUG_PRINT RDebug::Print
#else
#define DEBUG_PRINT
#endif

CVorbisDecoderInputPort* CVorbisDecoderInputPort::NewL(TInt aIndex, CVorbisDecoderProcessingUnit* aComponent)
	{
	CVorbisDecoderInputPort* self = new (ELeave) CVorbisDecoderInputPort;
	CleanupStack::PushL (self);
	self->ConstructL(aIndex, aComponent);
	CleanupStack::Pop();
	return self;
	}
	
CVorbisDecoderInputPort::CVorbisDecoderInputPort()
	{
	}
	
TInt CVorbisDecoderInputPort::MipConfigure(const TPuConfig&  aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidTTaskConfig))
		{
		// no configuration to be done for the decoder input port;
		// the vorbis decoder will detect that the input buffer is vorbis
		// format (or not).
		return KErrNone;
		}
	return KErrNotSupported;	
	}
	
void CVorbisDecoderInputPort::MipInitialize()
	{
	COmxInputPort::MipInitialize();
	OMX_VERSIONTYPE ver = 
		{
		1,0
		};

	// Set Input Port (Vorbis)
	OMX_AUDIO_PARAM_VORBISTYPE vorbis;
	vorbis.nVersion = ver;
	vorbis.nSize = sizeof(OMX_AUDIO_PARAM_VORBISTYPE);
	vorbis.nPortIndex = KVorbisDecoderInputPortIndex;
	
	Component()->OmxSetParameter(OMX_IndexParamAudioVorbis, &vorbis);
	}
