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

#include <mdf/mdfpuconfig.h>
#include "pcmprocessingunit.h"
#include "pcminputport.h"
#include "pcmoutputport.h"
#include <mmf/server/taskconfig.h>

const TInt KPcmInputPortIndex = 0;
const TInt KPcmOutputPortIndex = 1;
_LIT8(KPcmILComponentName, "OMX.SYMBIAN.AUDIO.CODEC.PCM");


CPcmProcessingUnit::CPcmProcessingUnit()
	{
	}

CPcmProcessingUnit* CPcmProcessingUnit::NewP8P16L()
	{
	CPcmProcessingUnit* self = new (ELeave) CPcmProcessingUnit;
	CleanupStack::PushL(self);
	self->ConstructL(EPCM8, EPCM16);
	CleanupStack::Pop(self);
	return self;
	}

CPcmProcessingUnit* CPcmProcessingUnit::NewP16P8L()
	{
	CPcmProcessingUnit* self = new (ELeave) CPcmProcessingUnit;
	CleanupStack::PushL(self);
	self->ConstructL(EPCM16, EPCM8);
	CleanupStack::Pop(self);
	return self;
	}

CPcmProcessingUnit* CPcmProcessingUnit::NewPU8P16L()
	{
	CPcmProcessingUnit* self = new (ELeave) CPcmProcessingUnit;
	CleanupStack::PushL(self);
	self->ConstructL(EPCMU8, EPCM16);
	CleanupStack::Pop(self);
	return self;
	}

CPcmProcessingUnit* CPcmProcessingUnit::NewP16PU8L()
	{
	CPcmProcessingUnit* self = new (ELeave) CPcmProcessingUnit;
	CleanupStack::PushL(self);
	self->ConstructL(EPCM16, EPCMU8);
	CleanupStack::Pop(self);
	return self;
	}

void CPcmProcessingUnit::ConstructL(TPcmDataType aSrcDataType, TPcmDataType aDestDataType)
	{
	iSrcDataType = aSrcDataType;
	iDestDataType = aDestDataType;
	}


CPcmProcessingUnit::~CPcmProcessingUnit()
	{
	// Delete input and output ports	
	delete iInputPort;
	iInputPort = NULL;
	delete iOutputPort;
	iOutputPort = NULL;		
	}


TInt CPcmProcessingUnit::Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver)
	{
	TInt err = KErrNone;
	// Create underlying OMX component
	TRAP(err, COmxProcessingUnit::ConstructL(KPcmILComponentName, aProcessingUnitObserver));
	if (err == KErrNone)
		{
		SetPuState(EProcessingUnitLoaded);
	
		// Create Input and output ports
		TRAP(err, iInputPort  = CPcmInputPort::NewL(KPcmInputPortIndex, this, iSrcDataType));
		if(err != KErrNone)
			{
			return err;
			}
				
		TRAPD(err, iOutputPort = CPcmOutputPort::NewL(KPcmOutputPortIndex, this, iDestDataType));
		if(err != KErrNone)
			{
			return err;
			}		

		// Add them to processing unit
		err = AddInputPort(iInputPort);
		if (err != KErrNone)
			{
			return err;
			}
		err = AddOutputPort(iOutputPort);					
		}
	return err;
	}


TInt CPcmProcessingUnit::Configure(const TPuConfig& /*aConfig*/)
	{
	return KErrNone;
	}



	
	
