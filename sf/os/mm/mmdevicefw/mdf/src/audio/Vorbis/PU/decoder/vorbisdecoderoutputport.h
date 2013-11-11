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

#ifndef VORBISDECODEROUTPUTPORT_H
#define VORBISDECODEROUTPUTPORT_H

#include <mdf/mdfoutputport.h>
#include <mdf/mdfinputport.h>
#include <omxoutputport.h>

class CVorbisDecoderProcessingUnit;
class TPuConfig;
class MMdfInputPort;
class CMMFBuffer;

const TInt KVorbisDecoderOutputPortIndex = 1;

class CVorbisDecoderOutputPort : public COmxOutputPort
	{
	friend class CVorbisDecoderProcessingUnit;
public:
	static CVorbisDecoderOutputPort* NewL(TInt aIndex, CVorbisDecoderProcessingUnit* aComponent);
	TInt MopConfigure(const TPuConfig&  aConfiguration);
 	void MopInitialize();
		
private:
	CVorbisDecoderOutputPort();

private:
	MMdfInputPort* 				iPortConnectedTo;
	MMdfOutputPortObserver*		iObserver;
	
	// PCM settings
	TInt 					iSampleRate;
	TInt 					iChannels;
	TBool					iInterleaved;

	TBool 					iStopped;	
	};

#endif // VORBISDECODEROUTPUTPORT_H
