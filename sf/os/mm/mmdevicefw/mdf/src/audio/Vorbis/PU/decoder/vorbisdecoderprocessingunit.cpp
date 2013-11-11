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
#include <mmf/server/taskconfig.h>
#include <omxprocessingunit.h>

#include "vorbisdecoderprocessingunit.h"
#include "vorbisdecoderinputport.h"
#include "vorbisdecoderoutputport.h"


_LIT8(KVorbisDecoderILComponentName,"OMX.SYMBIAN.AUDIO.DECODER.VORBIS");

CVorbisDecoderProcessingUnit::CVorbisDecoderProcessingUnit()
  : iVorbisDecoderPuObserver(NULL),
  	iVorbisDecoderInputPort(NULL),
    iVorbisDecoderOutputPort(NULL)
    
	{
	}

CVorbisDecoderProcessingUnit* CVorbisDecoderProcessingUnit::NewL()
	{
	CVorbisDecoderProcessingUnit* self = new (ELeave) CVorbisDecoderProcessingUnit;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CVorbisDecoderProcessingUnit::ConstructL()
	{
	}

CVorbisDecoderProcessingUnit::~CVorbisDecoderProcessingUnit()
	{
	delete iVorbisDecoderOutputPort;			
	delete iVorbisDecoderInputPort;	
	}
	
TInt CVorbisDecoderProcessingUnit::Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver)
	{
	TRAPD(err, CreateL(aProcessingUnitObserver));
	return err;	
	}

void CVorbisDecoderProcessingUnit::CreateL(const MMdfProcessingUnitObserver& aProcessingUnitObserver)
	{
	iVorbisDecoderPuObserver = const_cast<MMdfProcessingUnitObserver*>(&aProcessingUnitObserver);
	COmxProcessingUnit::ConstructL(KVorbisDecoderILComponentName, aProcessingUnitObserver);
	iVorbisDecoderInputPort  = CVorbisDecoderInputPort::NewL(KVorbisDecoderInputPortIndex, this);
	iVorbisDecoderOutputPort = CVorbisDecoderOutputPort::NewL(KVorbisDecoderOutputPortIndex, this);
	User::LeaveIfError(AddInputPort(iVorbisDecoderInputPort));
	User::LeaveIfError(AddOutputPort(iVorbisDecoderOutputPort));
	}



// end

	
	
