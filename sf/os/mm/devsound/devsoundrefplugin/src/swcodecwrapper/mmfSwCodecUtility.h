// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFSWCODECUTILITY_H__
#define __MMFSWCODECUTILITY_H__

#include <e32base.h>
#include <mmf/server/mmfdatabuffer.h>

/**
 * @internalTechnology
 */
class CMMFSwCodecUtility : public CBase
	{
public:
	// Destructor
	~CMMFSwCodecUtility();
	static CMMFSwCodecUtility* NewL();

	void ConfigAudioRamper(TInt64 aRampTime, TInt aSampleRate, TInt aChannels);
	TBool RampAudio(CMMFDataBuffer* aBuffer);
private:
	CMMFSwCodecUtility();
	void ConstructL();

private:
	TInt		iRampSamples;
	TInt		iRampSamplesLeft;
	TInt		iChannels;
	TInt		iRampIncr;
	TBool		iSkip;
	} ;

#endif // __MMFSWCODECUTILITY_H__
