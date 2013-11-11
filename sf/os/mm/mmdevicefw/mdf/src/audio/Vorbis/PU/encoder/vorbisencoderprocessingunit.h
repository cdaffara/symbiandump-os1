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

#ifndef VORBISENCODERPROCESSINGUNIT_H
#define VORBISENCODERPROCESSINGUNIT_H

#include <e32std.h>
#include <e32base.h>
#include <mdf/mdfprocessingunit.h>
#include <omxprocessingunit.h>

// for the bitrate custom interface
#include <mmf/server/devsoundstandardcustominterfaces.h>

class CVorbisEncoderOutputPort;
class CVorbisEncoderInputPort;

class CVorbisEncoderProcessingUnit 
  : public COmxProcessingUnit, public MMMFDevSoundCustomInterfaceBitRate
	{
public:
	enum TDataType
		{
		EVorbis,
		EPCM16
		};
public:
	static CVorbisEncoderProcessingUnit* NewL();
	TInt Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver);
	TInt CreateCustomInterface(TUid aUid);
	TAny* CustomInterface(TUid aUid);
	~CVorbisEncoderProcessingUnit();
	
    //from MMMFDevSoundCustomInterfaceBitRate
    void GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates);
    TInt BitRateL();
    void SetBitRateL(TInt aBitRate);

private:
	CVorbisEncoderProcessingUnit();
	void ConstructL();
	void CreateL(const MMdfProcessingUnitObserver& aProcessingUnitObserver);
	
private:
	CVorbisEncoderInputPort* 	iVorbisEncoderInputPort;
	CVorbisEncoderOutputPort* 	iVorbisEncoderOutputPort;
	RArray<TInt>				iSupportedBitRates;

	TInt 						iInputBitsPerSample;
	OMX_NUMERICALDATATYPE		iNumInputType;
	};
	
#endif // VORBISENCODERPROCESSINGUNIT_H



