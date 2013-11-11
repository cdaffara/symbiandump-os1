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
#include "Tsu_mmf_afmt_oom.h"
#include "TSU_MMF_AFMTSuite.h"
#include "TSU_MMF_AFMT_testdatasink.h"


// Literal strings used by all tests
_LIT(KNoPreferredSupplier, "");




/**
 * This function runs the Out-of-memory test for the specified NewL function.
 * @param aNewL - Specifies which NewL function to call.
 * @return TBool Returns ETrue if everything went Ok, EFalse if there was some
 * error.
 */
TBool CTestStep_MMF_AFMT_OOM::RunOomTestL(const ENewLType& aNewL)
	{
	TAny* testAlloc = NULL;
	
	// Create a CMmfFile based DataSource for testing (iUID already setup)
	if (aNewL == KNewLDescriptor)
		SetupNewL(EFalse, aNewL, iFilename, iUID.iUid);
	else 
		SetupNewL(ETrue, aNewL, iFilename, iUID.iUid);

	__MM_HEAP_MARK;

	// Run through as many iterations of NewL as needed.
	for (TInt failRate = 1; ; ++failRate)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failRate);
		TRAPD( error, RunNewL(aNewL) );						

		testAlloc = User::Alloc(1);	// Alloc 1 bytes
		
		// Delete the object now. 
		// It may have been created despite a memory failure
		// and so we may need to carry on with this loop
		delete iFormatDec;
		iFormatDec = NULL;
		delete iFormatEnc;		
		iFormatEnc = NULL;
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		
		if (testAlloc == 0)  // Test if our test Alloc failed.  If so
			{				 // then we are finished testing the function			
			__ASSERT_ALWAYS (error == KErrNone, User::Leave(error));
			break;
			}
		User::Free(testAlloc); testAlloc = NULL;
		__ASSERT_ALWAYS (error == KErrNoMemory || error == KErrNone, 	User::Leave(error));
		}

	__MM_HEAP_MARKEND;


	// Cleanup anything else that was need for the test.
	Cleanup(); 

	// For traceability, print which test has just completed.
	INFO_PRINTF2(_L("Completed OOM tests on %d"), aNewL);

	// Return a pass if we have got this far.
	return ETrue;
	}



/**
 * Setups up member variables before running a test, then runs all four
 * NewL functions for a given CMmfXXXFormat object (as determined by
 * the aUid).
 * @param aUid - Specifies which object to run the tests on.
 * @TVerdict EPass if all tests passed, else EFail.
 */
TVerdict CTestStep_MMF_AFMT_OOM::RunAllOomTestsL(const TUint& aUid)
	{
	TVerdict verdict = EFail;

	iUID = TUid::Uid(aUid);

	// Work out whether we are doing Encode or Decode Format tests.
	// This sets 'iDecode' which is used in later parts of the OOM
	// tests to use the correct member variables.

	// NOTE: For write object tests, use a Non existing file, 
	// as the file seems to get erased if the NewL fails.  This saves
	// us losing test files. :-)
	switch (aUid)
		{
		case KMmfUidFormatAUWrite:
			iFilename.Set(KEmptyAuFile);
			iDecode = EFalse;
			break;
		case KMmfUidFormatRAWWrite:
			iFilename.Set(KEmptyRawFile);
			iDecode = EFalse;
			break;
		case KMmfUidFormatWAVWrite:
			iFilename.Set(KEmptyWavFile);
			iDecode = EFalse;
			break;

		case KMmfUidFormatAURead:
			iFilename.Set(KAuPcm16BitMono22Khz);
			iDecode = ETrue;
			break;
		case KMmfUidFormatRAWRead:
			iFilename.Set(KRawPcm16BitMono22Khz);
			iDecode = ETrue;
			break;
		case KMmfUidFormatWAVRead:
			iFilename.Set(KWavPcm16BitMono22Khz);
			iDecode = ETrue;
			break;
		default:
			INFO_PRINTF1(_L("Leaving from Setup as UID not supported"));
			User::Leave(KErrNotSupported);
		}

	// NOTE: For RAW testing, we ONLY test the UID NewL call.  For the other
	// format types, all four need to be tested.
	if ( (aUid == KMmfUidFormatRAWRead ||	aUid == KMmfUidFormatRAWWrite) &&
		RunOomTestL(KNewLUid) )
		{
		verdict = EPass;
		}

	// Run all of the tests and return the verdict
	else 
		{
		TBool testFail = EFalse;;
		if (!RunOomTestL(KNewLUid)) testFail = ETrue;
		if (!RunOomTestL(KNewLFilename)) testFail = ETrue;
		if (!RunOomTestL(KNewLClip)) testFail = ETrue;
		if (aUid == KMmfUidFormatAUWrite)
			{
			iUID = TUid::Uid(KMmfUidFormatAURead);
			iFilename.Set(KAuPcm16BitMono22Khz);
			}
		else if (aUid == KMmfUidFormatRAWWrite)
			{
			iUID = TUid::Uid(KMmfUidFormatRAWRead);
			iFilename.Set(KRawPcm16BitMono22Khz);
			}
		else if (aUid == KMmfUidFormatWAVWrite)
			{
			iUID = TUid::Uid(KMmfUidFormatWAVRead);
			iFilename.Set(KWavPcm16BitMono22Khz);
			}
		TBool result = EFalse;
		result = RunOomTestL(KNewLDescriptor); // I suppose this should leave as above

		if (!result) testFail = ETrue;
		
		if (!testFail) verdict = EPass;
		}
	return verdict;
	}

/** 
 * This function runs the correct NewL on the correct object.
 * @param aNewL - Enum inicating which NewL function should be called.
 */
void CTestStep_MMF_AFMT_OOM::RunNewL(const ENewLType& aNewL)
	{
	switch (aNewL)
		{
		case KNewLUid:		// Call NewL with a UID
			if (iDecode) 
				iFormatDec = CMMFFormatDecode::NewL( iUID, iDataSource);
			else
				iFormatEnc = CMMFFormatEncode::NewL( iUID, iDataSink);
			break;

		case KNewLFilename:	// Call NewL with a filename
		 	if (iDecode) 
				iFormatDec = CMMFFormatDecode::NewL( iFilename, iDataSource, iPreferredSupplier);
			else 
				iFormatEnc = CMMFFormatEncode::NewL( iFilename, iDataSink, iPreferredSupplier);
			break;

		case KNewLDescriptor:// Call NewL with a Descriptor of data
			if (iDecode) 
				iFormatDec = CMMFFormatDecode::NewL( iData, iDataSource, iPreferredSupplier);
			else
				iFormatEnc = CMMFFormatEncode::NewL( iData, iDataSink, iPreferredSupplier);
			break;

		case KNewLClip:		// Call NewL passing in a CMMFClip
			if (iDecode) 
				iFormatDec = CMMFFormatDecode::NewL(iDataSource, iPreferredSupplier);
			else 
				iFormatEnc = CMMFFormatEncode::NewL(iDataSink, iPreferredSupplier);
			break;

		default:
			INFO_PRINTF1(_L("RunDecodeNewL - Bad NewL type (Leaving KErrArgument)"));
			User::Leave(KErrArgument);
		}
	}


/** 
 * This function sets up the variables required for a FormatDecode NewL call.
 * @param aBool - 'ETrue' if DataSource is file based, 'EFalse' if Descriptor based.
 * @param aNewL - Enum inicating which NewL function should be called.
 * @param aFilename - The filename containing the audio data.
 * @param aUid - Only required if aNewL is KNewLUid.
 */
void CTestStep_MMF_AFMT_OOM::SetupNewL(const TBool& aBool, 
									   const ENewLType& aNewL, 
									   const TDesC& aFilename, 
									   const TInt& aUid)
	{
	// 
	__ASSERT_ALWAYS(iUID != TUid::Null(), User::Leave(KErrArgument) );

	// We store the NewL to save having to repeat it in every call for the NewL tests...
	iNewL = aNewL;	

	// Create the DataSource/Sink object.
	if (iDecode)
		CreateDataSource(aBool, iFilename);
	else 
		CreateDataSink(aBool, iFilename);		

	// Setup parameters for NewL.
	switch (aNewL)
		{
		case KNewLUid:		// Call NewL with a UID
			{
			break;
			}

		case KNewLFilename:	// Call NewL with a filename
			{
			// Setup other parameters for the NewL call.
			iFilename.Set(aFilename);
			iPreferredSupplier.Set(KNoPreferredSupplier);
			break;
			}
			

		case KNewLDescriptor:// Call NewL with a Descriptor of data
			{		
//PAC only load des if read
		if ((aUid == KMmfUidFormatAURead) || 
				(aUid == KMmfUidFormatWAVRead)||
				(aUid == KMmfUidFormatRAWRead))
				ReadFileToDescriptorL(iFilename, 0, KFormatDefaultFrameSize );
			iPreferredSupplier.Set(KNoPreferredSupplier);
			break;
			}

		case KNewLClip:		// Call NewL passing in a CMMFClip
			{
			iPreferredSupplier.Set(KNoPreferredSupplier);
			break;
			}

		default:
			User::Leave(KErrGeneral);
		}	
	}


