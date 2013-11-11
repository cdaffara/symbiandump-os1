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
#include "Tsu_mmf_afmt_NumChannels.h"
#include "TSU_MMF_AFMTSuite.h"
#include "TSU_MMF_AFMT_testdatasink.h"

// Literal strings used by all tests
//_LIT(KNoPreferredSupplier, "");	// EABI warning removal


/**
 * This function tests the specified files have the correct number of channels.
 * @param aMono - This file should have only one channel.
 * @param aStereo - This file should have two channels.
 * @return TVerdict - If the files have the right number of channels, this
 * function will return EPass, else EFail.
 */
TVerdict CTestStep_MMF_AFMT_NumChannels::TestNumChannelsL(	const TDesC& aMono,
															const TDesC& aStereo)
												 
	{
	// iUid should have been setup by now - double check here...
	__ASSERT_ALWAYS(iUid!=0, User::Leave(KErrArgument));

	TVerdict verdict = EFail;

	// Create a Stereo test object
	CreateObjectL(ETrue, aStereo, iUid);
	SetupForRaw(2);	// This only sets channels if using RAW format

	// Call Num Channels (should get 2 as response);
	if (CallNumChannelsL() == 2)
		{
		// Tidyup from the Stereo test
		Cleanup();

		// Create a Mono test object
		CreateObjectL(ETrue, aMono, iUid);
		SetupForRaw(1);	// This only sets channels if using RAW format

		// Call NumChannels (should get 1 as response)
		if (CallNumChannelsL() == 1)	
			{// If we get here, we passed both tests!
			verdict = EPass;
			}
		}		
	// NOTE: second Cleanup is called in Close() call.

	return verdict;
	}


/** 
 * This function calls NumChannels() on whichever Format object is instanciated.
 * @return TInt The number of channels.
 */
TInt CTestStep_MMF_AFMT_NumChannels::CallNumChannelsL()
	{	
	TInt retVal = 0;
	if (iFormatDec && iDecode)
		retVal = iFormatDec->NumChannels();
	else if (iFormatEnc)
		retVal = iFormatEnc->NumChannels();
	else
		User::Leave(KErrArgument);	
	return retVal;
	}


/** 
 * This function calls SetNumChannels() on whichever Format object is 
 * instanciated.
 * @param aChannels - The value to pass to the SetNumChannels() call
 */
void CTestStep_MMF_AFMT_NumChannels::CallSetNumChannelsL(const TInt& aChannels)
	{	
	if (iFormatDec && iDecode)
		iFormatDec->SetNumChannels(aChannels);
	else if (iFormatEnc)
		iFormatEnc->SetNumChannels(aChannels);
	else
		User::Leave(KErrArgument);	
	}


/**
 * This function tests whether a call to SetChannels will work by setting the 
 * channels, then getting them again to see if the change has taken place.
 * @param aMono - This file should not exist, else test may fail.
 * @param aStereo - This file should not exist, else test may fail.
 * @return TVerdict - If the files have the right number of channels, this
 * function will return EPass, else EFail.  
 */
TVerdict CTestStep_MMF_AFMT_NumChannels::TestSetNumChannelsL(	const TDesC& aMono,
																const TDesC& aStereo)
												 
	{
	// iUid should have been setup by now - double check here...
	__ASSERT_ALWAYS(iUid!=0, User::Leave(KErrArgument));
	// SET call only valid for Encode/Write objects
	__ASSERT_ALWAYS(iDecode == EFalse, User::Leave(KErrArgument)); 

	TVerdict verdict = EFail;

	// Create a Stereo test object, then set the number of channels to 
	CreateObjectL(ETrue, aStereo, iUid);
	SetupForRaw(2);	// This only sets channels if using RAW format
	CallSetNumChannelsL(1);

	// Call NumChannels (should now return 1)
	if (CallNumChannelsL() == 1)
		{
		// Tidyup from the Stereo test
		Cleanup();

		// Create a Mono test object then set no of channels to 2.
		CreateObjectL(ETrue, aMono, iUid);
		SetupForRaw(1);	// This only sets channels if using RAW format
		CallSetNumChannelsL(2);

		// Call NumChannels (should now return 2)
		if (CallNumChannelsL() == 2)	
			{// If we get here, we passed both tests!
			verdict = EPass;
			}
	
		}	
	// NOTE: second Cleanup is called in Close() call.
	return verdict;
	}


/**
 * If we are testing the RAW Format object, this function sets the fields
 * required in the Format object for testing.  If we are not testing the
 * RAW format, this function does nothing.
 * @param aChannels - The number of channels to set the raw format as having.
 */
void CTestStep_MMF_AFMT_NumChannels::SetupForRaw(const TInt& aChannels)
	{
	if (iUid == KMmfUidFormatRAWRead && iFormatDec)
		{
		iFormatDec->SetNumChannels(aChannels);		
		}
	else if(iUid == KMmfUidFormatRAWWrite && iFormatEnc)
		{	
		iFormatEnc->SetNumChannels(aChannels);		
		}	
	}	

									







