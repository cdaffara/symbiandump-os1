// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_AFMT_INTERVAL_H__
#define __TSU_MMF_AFMT_INTERVAL_H__

#include <testframework.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatasink.h>



class CTestStep_MMF_AFMT_IntervalL : public CTestStep_MMF_AFMT
	{
public: 
	// This enum represents the type of audio encoding in the file we
	// are testing.
	enum EAudioType {EAudioPCM,		// PCM audio encoding
					 EAudioADPCM,	// ADPCM audio encoding
					 EAudioGSM};	// GSM audio encoding

public: // Constructor/Destructor
	CTestStep_MMF_AFMT_IntervalL(){}
	virtual ~CTestStep_MMF_AFMT_IntervalL(){}

public: //	New functions
	/**
	 * This function works out the FrameTimeInterval from the supplied parameters.
	 * The return value can then be compared to a real FrameTimeInterval, and they 
	 * should roughly match (depending on rounding errors etc).
	 * @param aType - The type of audio encoding (PCM, ADPCM, GSM) 
	 * @param aSampleRate - The sample rate of the audio data
	 * @param aBitsPerSample - The bits per sample of the audio data
	 * @param aChannels - The number of channels of the audio data.
	 * @return TTimeIntervalMicroSeconds - The estimated FrameTimeInterval.
	 */
	TTimeIntervalMicroSeconds FindFrameTimeInterval(const EAudioType& aType,
													const TInt& aSampleRate =0, 
												    const TInt& aBitsPerSample =0, 
												    const TInt& aChannels =0,
													const TUint aSuggestedFrameSize = 0);


	/**
	 * This function runs the FrameTimeIntervalTest, though it requires alot of information
	 * to make it's own calculations of what the time interval SHOULD be.
	 * @param aType - The type of audio encoding (PCM, ADPCM, GSM) 
	 * @param aSampleRate - The sample rate of the audio data
	 * @param aBitsPerSample - The bits per sample of the audio data
	 * @param aChannels - The number of channels of the audio data.
	 * @return TVerdict - EPass if the interval is about right, else EFail.
	 */
	TVerdict RunTest(const EAudioType& aType, 
					 const TInt& aSampleRate =0, 
					 const TInt& aBitsPerSample =0,
					 const TInt& aChannels =0,
					 const TUint aSuggestedFrameSize = 0);

	};



#endif
