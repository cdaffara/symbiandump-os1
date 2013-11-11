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

// EPOC includes
#include <e32base.h>

#include <mmf/server/mmffile.h>						// for TMMFFileParams and CMMFFile
#include <mmf/server/mmfformat.h>						// For CMMFFormatDecode
#include <mmf/server/mmfdes.h>							// for TMMFDescriptorParams and CMMFDescriptor
#include <mmf/server/mmfdatasource.h> 
#include <mmf/plugin/mmfformatimplementationuids.hrh>	// for KMmfUidFormatWAVRead UIDs

#include <e32test.h>

// Test system includes
#include "TSU_MMF_AFMT.h"
#include "Tsu_mmf_afmt_SampleRate.h"
#include "TSU_MMF_AFMTSuite.h"
#include "TSU_MMF_AFMT_testdatasink.h"




/**
 * Tests the SampleRate() call.
 * @param aSampleRate - The sample rate the file should have
 * @param aAudioFile - The file to be tested
 * @return TBool - Returns ETrue if the file has the specified sample rate, 
 * otherwise it returns EFalse.
 */
TBool CTestStep_MMF_AFMT_SampleRate::TestSampleRateL(	const TInt& aSampleRate, 
														const TDesC& aAudioFile)
	{	
	TBool retVal = EFalse; 

	// Create a test object using 'aAudioFile' as the source
	CreateObjectL(ETrue, aAudioFile, iUid);
	SetupForRaw(aSampleRate);	// This only sets sample rate if using RAW format

	// Call SampleRate for an 8Khz source file
	if (CallSampleRateL() == aSampleRate)
		{
		retVal = ETrue;
		}

	// Tidyup from the test
	Cleanup();

	return retVal;
	}



/**
 * Tests the SetSampleRate() function
 * @param aSampleRate - The sample rate to set the file to.
 * @param aAudioFile - A file to try and set the rate of.
 * @return TInt - If the file exists already, the set will fail and this function
 * will return KErrAlreadyExists.  If the file doesn't exist yet, the sample rate
 * can be set, and KErrNone is returned.  If something else goes wrong, it may
 * return KErrGeneral.
 */
TInt CTestStep_MMF_AFMT_SampleRate::TestSetSampleRateL(const TInt& aSampleRate, 
														const TDesC& aAudioFile)
	{	
	TInt retVal = KErrNone; 
	

	// Create a test object using 'aAudioFile' as the source
	CreateObjectL(ETrue, aAudioFile, iUid);
	SetupForRaw(aSampleRate);	// This only sets sample rate if using RAW format

	// This call to set the SampleRate may fail with KErrAlreadyExists if 
	// the SampleRate is 'locked' already.  In this case, just return that error
	// code.  If it was 'Set' ok though, read it back.
	retVal = CallSetSampleRateL(aSampleRate); // Force the samplerate to be something else

	// If could set the SampleRate ok, check it was really set by reading it back.
	if (retVal == KErrNone)
		{
		if (CallSampleRateL()==aSampleRate)
			{// This should be not change, as this means it was set and retrieved ok.
			retVal = KErrNone; 
			} 
		else // This means we were told it was set ok, but it wasn't. 
			{
			retVal = KErrGeneral; // A different failure code.
			}
		}

	// Tidyup from the test
	Cleanup();

	return retVal;
	}



/**
 * This function calls SampleRate() on the correct object.
 * @return TInt - the return from the SampleRate() call.
 */
TInt CTestStep_MMF_AFMT_SampleRate::CallSampleRateL()
	{	
	TInt samplerate = 0;
	if (iFormatDec && iDecode)
		samplerate = iFormatDec->SampleRate();
	else if (iFormatEnc)
		samplerate = iFormatEnc->SampleRate();
	else
		User::Leave(KErrArgument);	
	return samplerate;
	}

/**
 * This function calls SetSampleRate() on the correct object.
 * @param aSampleRate - The sample rate to try and set.
 * @return TInt - the return from the SampleRate() call.
 */
TInt CTestStep_MMF_AFMT_SampleRate::CallSetSampleRateL(const TInt& aSampleRate)
	{	
	TInt ret = KErrNone;
	if (iFormatDec && iDecode)
		ret = iFormatDec->SetSampleRate(aSampleRate);
	else if (iFormatEnc)
		ret = iFormatEnc->SetSampleRate(aSampleRate);
	else
		User::Leave(KErrArgument);	
	return ret; // If successful should return KErrNone
	}


/**
 * If we are testing the RAW Format object, this function sets the fields
 * required in the Format object for testing.  If we are not testing the
 * RAW format, this function does nothing.
 * @param aSampleRate - The sample rate to set the RAW object to be using.
 */
void CTestStep_MMF_AFMT_SampleRate::SetupForRaw(const TInt& aSampleRate)
	{
	if (iUid == KMmfUidFormatRAWRead && iFormatDec)
		{
		iFormatDec->SetSampleRate(aSampleRate);		
		}
	else if(iUid == KMmfUidFormatRAWWrite && iFormatEnc)
		{	
		iFormatEnc->SetSampleRate(aSampleRate);		
		}	
	}	

									








