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


#ifndef PCMPROCESSINGUNIT_H
#define PCMPROCESSINGUNIT_H

#include <e32std.h>
#include <e32base.h>
#include <omxprocessingunit.h>

class CPcmOutputPort;
class CPcmInputPort;

enum TPcmDataType
		{
		EPCM8,
		EPCMU8,
		EPCM16
		};

class CPcmProcessingUnit : public COmxProcessingUnit
	{
public:
	// entry points for each codec variant
	static CPcmProcessingUnit* NewP8P16L();
	static CPcmProcessingUnit* NewP16P8L();
	static CPcmProcessingUnit* NewPU8P16L();
	static CPcmProcessingUnit* NewP16PU8L();
	
	virtual TInt Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver);

	virtual TInt Configure(const TPuConfig& aConfigurationSetup);
	virtual ~CPcmProcessingUnit();

private:
	CPcmProcessingUnit();
	void ConstructL(TPcmDataType aSrcDataType, TPcmDataType aDestDataType);
	
private:
	CPcmInputPort* 				iInputPort;
	CPcmOutputPort* 			iOutputPort;

	TPcmDataType				iSrcDataType;
	TPcmDataType				iDestDataType;
	};

#endif // PCMPROCESSINGUNIT_H



