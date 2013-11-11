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
// vorbisencoderprocessingunit.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef VORBISDECODERPROCESSINGUNIT_H
#define VORBISDECODERPROCESSINGUNIT_H

#include <e32std.h>
#include <e32base.h>
#include <mdf/mdfprocessingunit.h>
#include <omxprocessingunit.h>

class CVorbisDecoderOutputPort;
class CVorbisDecoderInputPort;

class CVorbisDecoderProcessingUnit 
  : public COmxProcessingUnit
	{
public:
	enum TDataType
		{
		EVorbis,
		EPCM16
		};
public:
	static CVorbisDecoderProcessingUnit* NewL();
	TInt Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver);

	~CVorbisDecoderProcessingUnit();
	
private:
	CVorbisDecoderProcessingUnit();
	void ConstructL();
	void CreateL(const MMdfProcessingUnitObserver& aProcessingUnitObserver);	
	
private:
	MMdfProcessingUnitObserver* iVorbisDecoderPuObserver;
	CVorbisDecoderInputPort* 	iVorbisDecoderInputPort;
	CVorbisDecoderOutputPort* 	iVorbisDecoderOutputPort;
	};
	
#endif // VORBISDECODERPROCESSINGUNIT_H



