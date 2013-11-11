// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "vorbisencoderprocessingunit.h"
#include "vorbisencoderinputport.h"
#include "vorbisencoderoutputport.h"

const TInt KVorbisEncoderInputPortIndex = 0;
const TInt KVorbisEncoderOutputPortIndex = 1;

_LIT8(KVorbisEncoderILComponentName,"OMX.SYMBIAN.AUDIO.ENCODER.VORBIS");

// for the derivation of these constants see
// http://en.wikipedia.org/wiki/Vorbis
const TInt KVorbisQualityMinus1 = 4500;
const TInt KVorbisQuality0 = 6000;
const TInt KVorbisQuality1 = 80000;
const TInt KVorbisQuality2 = 96000;
const TInt KVorbisQuality3 = 110000;
const TInt KVorbisQuality4 = 128000;
const TInt KVorbisQuality5 = 160000;
const TInt KVorbisQuality6 = 192000;
const TInt KVorbisQuality7 = 224000;
const TInt KVorbisQuality8 = 256000;
const TInt KVorbisQuality9 = 320000;
const TInt KVorbisQuality10 = 500000;

CVorbisEncoderProcessingUnit::CVorbisEncoderProcessingUnit()
  : iVorbisEncoderInputPort(NULL),
    iVorbisEncoderOutputPort(NULL)
	{
	
	// input defaults (EPCM16)
	iInputBitsPerSample = 16;
	iNumInputType = OMX_NumericalDataSigned;		

	// no output defaults (EVorbis)
	}

CVorbisEncoderProcessingUnit* CVorbisEncoderProcessingUnit::NewL()
	{
	CVorbisEncoderProcessingUnit* self = new (ELeave) CVorbisEncoderProcessingUnit;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CVorbisEncoderProcessingUnit::ConstructL()
	{
	// setup supported bit rates array
	iSupportedBitRates.Reset();
	iSupportedBitRates.AppendL(KVorbisQualityMinus1);
	iSupportedBitRates.AppendL(KVorbisQuality0);
	iSupportedBitRates.AppendL(KVorbisQuality1);
	iSupportedBitRates.AppendL(KVorbisQuality2);
	iSupportedBitRates.AppendL(KVorbisQuality3);
	iSupportedBitRates.AppendL(KVorbisQuality4);
	iSupportedBitRates.AppendL(KVorbisQuality5);
	iSupportedBitRates.AppendL(KVorbisQuality6);
	iSupportedBitRates.AppendL(KVorbisQuality7);
	iSupportedBitRates.AppendL(KVorbisQuality8);
	iSupportedBitRates.AppendL(KVorbisQuality9);
	iSupportedBitRates.AppendL(KVorbisQuality10);
	}

CVorbisEncoderProcessingUnit::~CVorbisEncoderProcessingUnit()
	{
	iSupportedBitRates.Reset();
	iSupportedBitRates.Close();
	delete iVorbisEncoderOutputPort;			
	delete iVorbisEncoderInputPort;	
	}

TInt CVorbisEncoderProcessingUnit::Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver)
	{
	TRAPD(err, CreateL(aProcessingUnitObserver));
	return err;
	}
	
	
void CVorbisEncoderProcessingUnit::CreateL(const MMdfProcessingUnitObserver& aProcessingUnitObserver)
	{
	COmxProcessingUnit::ConstructL(KVorbisEncoderILComponentName, aProcessingUnitObserver);
	iVorbisEncoderInputPort  = CVorbisEncoderInputPort::NewL(KVorbisEncoderInputPortIndex, this);
	iVorbisEncoderOutputPort = CVorbisEncoderOutputPort::NewL(KVorbisEncoderOutputPortIndex, this);
	User::LeaveIfError(AddInputPort(iVorbisEncoderInputPort));
	User::LeaveIfError(AddOutputPort(iVorbisEncoderOutputPort));
	}


	

TInt CVorbisEncoderProcessingUnit::CreateCustomInterface(TUid aInterfaceId)
	{
	if (aInterfaceId == KUidCustomInterfaceDevSoundBitRate)
		{
		// There is no work to create this interface, so just return no error
		return KErrNone;
		}
	else
		{
		return KErrNotSupported;
		}
	}

TAny* CVorbisEncoderProcessingUnit::CustomInterface(TUid aInterfaceId)
	{
	// The vorbis encoder requires the custom interface for bitrates
	if (aInterfaceId == KUidCustomInterfaceDevSoundBitRate)
		{
		return static_cast<MMMFDevSoundCustomInterfaceBitRate*> (this);
		}
	else
		{
		return NULL;
		}
	}
	
/**
Gets the bit rates that are supported by DevSound in its current configuration.

@param  aSupportedBitRates
The supported bit rates, in bits per second, shall be appended to this array. Note that 
the array shall be reset by this method.
*/
void CVorbisEncoderProcessingUnit::GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates)
	{
	aSupportedBitRates.Reset();
	TInt c = iSupportedBitRates.Count();
	for(TInt i = 0; i < c; i++) 
		{
		aSupportedBitRates.AppendL(iSupportedBitRates[i]);
		}
 	}

TInt CVorbisEncoderProcessingUnit::BitRateL()
	{
	return iVorbisEncoderOutputPort->BitRate();
	}

void CVorbisEncoderProcessingUnit::SetBitRateL(TInt aBitRate)
	{
	// We can't strictly set the bit rate outside of calling
	// MopConfigure() on the output port.
	// However if we own the port we can call its SetBitRateL() method
	// which is not part of MMdfOutputPort.

	// set the bitrate to the nearest available, rounding down
	TInt c = iSupportedBitRates.Count();
	for(TInt i = c-1; i >= 0; i--) 
		{
		if(aBitRate >= iSupportedBitRates[i]) 
			{
			iVorbisEncoderOutputPort->SetBitRate(iSupportedBitRates[i]);
			return;
			}
		}
	User::Leave(KErrNotSupported);	
	}

// end

	
	
