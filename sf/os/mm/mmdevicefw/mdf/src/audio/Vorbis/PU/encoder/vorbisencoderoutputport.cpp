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

#include <omxoutputport.h>
#include <mdf/mdfpuconfig.h>

#include "vorbisencoderoutputport.h"
#include "vorbisencoderprocessingunit.h"

#include <e32debug.h>

// #define VORBISENCODEROUTPUTPORT_DEBUG	1
#if defined(VORBISENCODEROUTPUTPORT_DEBUG)
#define DEBUG_PRINT RDebug::Print
#else
#define DEBUG_PRINT
#endif

// from PREQ452 CVorbisEncoder
const TInt KVorbisDefaultBitRate = 128000;

CVorbisEncoderOutputPort* CVorbisEncoderOutputPort::NewL(TInt aIndex, CVorbisEncoderProcessingUnit* aParent)
	{
	CVorbisEncoderOutputPort* self = new (ELeave) CVorbisEncoderOutputPort;
	CleanupStack::PushL (self);
	self->ConstructL(aIndex, aParent);
	CleanupStack::Pop();
	return self;
	}

CVorbisEncoderOutputPort::CVorbisEncoderOutputPort() :
	iBitRate(KVorbisDefaultBitRate)
	{
	}


TInt CVorbisEncoderOutputPort::MopConfigure(const TPuConfig&  aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidTTaskConfig))
		{
		// there is nothing here to set. iBitRate must be set
		// through a custom interface

		return KErrNone;
		}
	else 
		{
		return KErrNotSupported;	
		}
	}
	
void CVorbisEncoderOutputPort::MopInitialize()	
 	{
 	COmxOutputPort::MopInitialize();
	OMX_VERSIONTYPE ver = 
		{
		1,0
		};
	// Set Output Port (Vorbis)
	OMX_AUDIO_PARAM_VORBISTYPE vorbis;
	vorbis.nVersion = ver;
	vorbis.nSize = sizeof(OMX_AUDIO_PARAM_VORBISTYPE);
	vorbis.nPortIndex = 1;
	vorbis.nBitRate = BitRate();
	
	Component()->OmxSetParameter(OMX_IndexParamAudioVorbis, &vorbis);
 	}
	

// get and set bit rate
// NB these are available to the owner of the port only, not as part
// of MMdfOutputPort
TInt CVorbisEncoderOutputPort::BitRate()
	{
	return iBitRate;
	}
	
void CVorbisEncoderOutputPort::SetBitRate(TInt aBitRate)
	{
	iBitRate = aBitRate;
	}
	
// end

