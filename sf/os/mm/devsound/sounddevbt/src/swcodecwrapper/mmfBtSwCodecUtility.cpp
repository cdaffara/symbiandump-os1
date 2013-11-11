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

#include <e32std.h>
#include "mmfBtSwCodecUtility.h"



/******************************************************************
 * CMMFSwCodecUtility
 ******************************************************************/
CMMFSwCodecUtility::CMMFSwCodecUtility()
	{
	}


CMMFSwCodecUtility::~CMMFSwCodecUtility()
	{
	}

CMMFSwCodecUtility* CMMFSwCodecUtility::NewL()
	{
	CMMFSwCodecUtility* self = new(ELeave) CMMFSwCodecUtility();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CMMFSwCodecUtility::ConstructL()
	{
	}

void CMMFSwCodecUtility::ConfigAudioRamper(TInt64 aRampTime, TInt aSampleRate, TInt aChannels)
	{
	iRampSamples = I64LOW(((TInt64(aSampleRate) * aRampTime) /1000000 )); // Add this
	iRampSamplesLeft = iRampSamples;
	iChannels = aChannels;
	iRampIncr = 0;
	iSkip = ETrue;	                       
	}

TBool CMMFSwCodecUtility::RampAudio(CMMFDataBuffer* aBuffer)
	{
	TInt i=0;
	TInt length = aBuffer->Data().Length()>>1;
	TInt16* sample = REINTERPRET_CAST(TInt16*,&aBuffer->Data()[0]);
	TInt64 theResult(0);
	while ((i < length) && (iRampIncr < iRampSamples))
		{
		theResult = sample[i];
		theResult *= iRampIncr;
		theResult /= iRampSamples;
		sample[i] = STATIC_CAST(TInt16, I64LOW(theResult) );
          
		if ((iChannels == 1) || (!iSkip))
			{
			iRampIncr++;
			}
		iSkip = !iSkip;
		i++;
		}

	if (iRampIncr < iRampSamples)
		return ETrue;
	else
		return EFalse;

	}

