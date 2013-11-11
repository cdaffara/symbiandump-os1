
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

#ifndef __TSU_MMF_AFMT_SAMPLERATE_H__
#define __TSU_MMF_AFMT_SAMPLERATE_H__

#include <testframework.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatasink.h>




class CTestStep_MMF_AFMT_SampleRate : public CTestStep_MMF_AFMT
	{
public: // Constructor/Destructor
	CTestStep_MMF_AFMT_SampleRate(){}
	virtual ~CTestStep_MMF_AFMT_SampleRate(){}


	/**
	 * Tests the SampleRate() call.
	 * @param aSampleRate - The sample rate the file should have
	 * @param aAudioFile - The file to be tested
	 * @return TBool - Returns ETrue if the file has the specified sample rate, 
	 * otherwise it returns EFalse.
	 */
	TBool TestSampleRateL(const TInt& aSampleRate, const TDesC& aAudioFile);


	/**
	 * Tests the SetSampleRate() function
	 * @param aSampleRate - The sample rate to set the file to.
	 * @param aAudioFile - A file to try and set the rate of.
	 * @return TInt - If the file exists already, the set will fail and this function
	 * will return KErrAlreadyExists.  If the file doesn't exist yet, the sample rate
	 * can be set, and KErrNone is returned.  If something else goes wrong, it may
	 * return KErrGeneral.
	 */
	TBool TestSetSampleRateL(const TInt& aSampleRate, const TDesC& aAudioFile);
	
protected:
	/**
	 * Sets up a member variable (Preamble)
	 * @param aUid - The UID to say which type of Format object we are testing.
	 */
	void SetupL(const TInt& aUid)
		{ iUid = aUid; }

private:
	/**
	 * This function calls SampleRate() on the correct object.
	 * @return TInt - the return from the SampleRate() call.
	 */
	TInt CallSampleRateL();


	/**
	 * This function calls SetSampleRate() on the correct object.
	 * @param aSampleRate - The sample rate to try and set.
	 * @return TInt - the return from the SampleRate() call.
	 */
	TInt CallSetSampleRateL(const TInt& aSampleRate);	


	/**
	 * If we are testing the RAW Format object, this function sets the fields
	 * required in the Format object for testing.  If we are not testing the
	 * RAW format, this function does nothing.
	 * @param aSampleRate - The sample rate to set the RAW object to be using.
	 */
	void SetupForRaw(const TInt& aSampleRate);
	
protected:
	TInt iUid;
	};



#endif
