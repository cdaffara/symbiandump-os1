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


// Test system includes
#include "TSU_MMF_AFMT.h"
#include "Tsu_mmf_afmt_newl.h"
#include "TSU_MMF_AFMTSuite.h"


// Literal strings used by all tests
_LIT(KNoPreferredSupplier, "");


/** 
 * This function sets up the variables required for a FormatDecode NewL call.
 * @param aIsFile - 'ETrue' if DataSource is file based, 'EFalse' if Descriptor based.
 * @param aNewL - Enum inicating which NewL function should be called.
 * @param aFilename - The filename containing the audio data.
 * @param aUid - Only required if aNewL is KNewLUid.
 */
void CTestStep_MMF_AFMT_NewL::SetupNewL(TBool aIsFile, 
										const ENewLType& aNewL, 
										const TDesC& aFilename, 
										const TInt& aUid)
	{
	// We store the NewL to save having to repeat it in every call for the NewL tests...
	iNewL = aNewL;	

	// Set the iDecode member variable depending on whether we are creating a decode
	// or encoding Format object.
	switch (aUid)
		{
		case KMmfUidFormatAUWrite:
		case KMmfUidFormatRAWWrite:
		case KMmfUidFormatWAVWrite:
			// Create the CMMFWavFormatWrite object.
			iDecode = EFalse;
			CreateDataSink(aIsFile, aFilename);
			break;

		case KMmfUidFormatAURead:
		case KMmfUidFormatRAWRead:
		case KMmfUidFormatWAVRead:
			// Create the CMMFWavFormatRead object.			
			iDecode = ETrue;
			CreateDataSource(aIsFile, aFilename);
			break;

		default:
			INFO_PRINTF1(_L("Leaving from Setup as UID not supported"));
			User::Leave(KErrNotSupported);
		}


	
	switch (aNewL)
		{
		case KNewLUid:		// Call NewL with a UID
			{
			iUID = TUid::Uid(aUid);
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
//			ReadFileToDescriptorL(iFilename, 0, KFormatDefaultFrameSize );
			ReadFileToDescriptorL(aFilename, 0, KFormatDefaultFrameSize );
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


/** 
 * This function makes the specified NewL call for a CMmfXXXFormatDecode object
 * @param aNewL - Which NewL function to call
 */
void CTestStep_MMF_AFMT_NewL::RunDecodeNewL(ENewLType aNewL)
	{
	// If not specified explicitly, use the 'iNewL' value which should have
	// been setup in the 'InitialiseXXXXTestL' call.
	if (aNewL == KNewLUndefined) 
		aNewL = iNewL;

	switch (aNewL)
		{
		case KNewLUid:		// Call NewL with a UID
			iFormatDec = CMMFFormatDecode::NewL( iUID, iDataSource);
			break;

		case KNewLFilename:	// Call NewL with a filename
		 	iFormatDec = CMMFFormatDecode::NewL( iFilename, iDataSource, iPreferredSupplier);
			break;

		case KNewLDescriptor:// Call NewL with a Descriptor of data
			iFormatDec = CMMFFormatDecode::NewL( iData, iDataSource, iPreferredSupplier);
			break;

		case KNewLClip:		// Call NewL passing in a CMMFClip
			iFormatDec = CMMFFormatDecode::NewL(iDataSource, iPreferredSupplier);
			break;

		default:
			User::Leave(KErrArgument);
		}
	}


/** 
 * This function makes the specified NewL call for a CMmfXXXFormatEncode object
 * @param aNewL - Which NewL function to call
 */
void CTestStep_MMF_AFMT_NewL::RunEncodeNewL( ENewLType aNewL)
	{
	// If not specified explicitly, use the 'iNewL' value which should have
	// been setup in the 'InitialiseXXXXTestL' call.
	if (aNewL == KNewLUndefined) 
		aNewL = iNewL;

	switch (aNewL)
		{
		case KNewLUid:		// Call NewL with a UID
			iFormatEnc = CMMFFormatEncode::NewL( iUID, iDataSink);
			break;

		case KNewLFilename:	// Call NewL with a filename
		 	iFormatEnc = CMMFFormatEncode::NewL( iFilename, iDataSink, iPreferredSupplier);
			break;

		case KNewLDescriptor:// Call NewL with a Descriptor of data
			iFormatEnc = CMMFFormatEncode::NewL( iData, iDataSink, iPreferredSupplier);
			break;

		case KNewLClip:		// Call NewL passing in a CMMFClip
			iFormatEnc = CMMFFormatEncode::NewL(iDataSink, iPreferredSupplier);
			break;

		default:
			User::Leave(KErrArgument);
		}
	}




/**
 * Initialises the member variables required for the NewL tests using CMmfDescriptors as the 
 * source to pass to the NewL call.
 * @param aNewL - The NewL call to make
 * @param aFilename - The filename to read the data from for the Descriptor
 * @param aUid - The UID of the Format (Wav, Au or Raw)
 */
void CTestStep_MMF_AFMT_NewL::InitialiseDescriptorTestsL(const ENewLType& aNewL, 
														 const TDesC& aFilename, 
														 const TInt& aUid)
	{
	// Create a Descriptor based data source
	SetupNewL(EFalse, aNewL, aFilename, aUid);	
	}


/**
 * Initialises the member variables required for the NewL tests using CMmfFile as the 
 * source to pass to the NewL call.
 * @param aNewL - The NewL call to make
 * @param aFilename - The filename to read the data from for the Descriptor
 * @param aUid - The UID of the Format (Wav, Au or Raw)
 */
void CTestStep_MMF_AFMT_NewL::InitialiseFileTestsL( const ENewLType& aNewL, 
													const TDesC& aFilename, 
													const TInt& aUid )
	{
	// Create a File based data source
	SetupNewL(ETrue, aNewL, aFilename, aUid);	
	}



/** 
 * This function verifies that the iFormat handle is valid, and that the
 * format of the object has been set up correctly.
 * @param aUid - The UID that DataSourceType() should match
 * @param aChannels - The number of channels that NumChannels() should match
 * @param aSampRate - The Sample Rate that SampleRate() should match
 * @param aFourCC - The FourCC code that SourceDataTypeCode() should match.
 * @return TVerdict - EPass if all data is as expected, else EFail.
 */
TVerdict CTestStep_MMF_AFMT_NewL::VerifyDecodeNewL(	const TUid& aUid , 
													const TUint& aChannels, 
													const TUint& aSampRate, 
													const TFourCC& aFourCC)
	{
	TVerdict verdict = EInconclusive;

	// Check the handle we have is valid.  If not, fail straight away.
	if (!iFormatDec)
		{
		verdict = EFail;
		}
	else // Check the object the handle points to...
		{
		//Verify this has been opened correctly and correct format is detected.
		if (iFormatDec->DataSourceType() == aUid   && 
			iFormatDec->NumChannels() == aChannels &&
			iFormatDec->SampleRate() == aSampRate  &&
			iFormatDec->SourceDataTypeCode(KUidMediaTypeAudio) == aFourCC )
			{
			verdict = EPass;
			}
		else
			{
			verdict = EFail;
			}
		}

	return verdict;
	}


/** 
 * This function verifies that the iFormat handle is valid, and that the
 * format of the object has been set up correctly.
 * @param aUid - The UID that DataSinkType() should match
 * @param aChannels - The number of channels that NumChannels() should match
 * @param aSampRate - The Sample Rate that SampleRate() should match
 * @param aFourCC - The FourCC code that SinkDataTypeCode() should match.
 * @return TVerdict - EPass if all data is as expected, else EFail.
 */
TVerdict CTestStep_MMF_AFMT_NewL::VerifyEncodeNewL(const TUid& aUid , 
											  const TUint& aChannels, 
											  const TUint& aSampRate, 
											  const TFourCC& aFourCC)
	{
	TVerdict verdict = EInconclusive;

	// Check the handle we have is valid.  If not, fail straight away.
	if (!iFormatEnc)
		{
		verdict = EFail;
		}
	else // Check the object the handle points to...
		{
		//Verify this has been opened correctly and correct format is detected.
		if (iFormatEnc->DataSourceType() == aUid   && 
			iFormatEnc->NumChannels() == aChannels &&
			iFormatEnc->SampleRate() == aSampRate  &&
			iFormatEnc->SinkDataTypeCode(KUidMediaTypeAudio) == aFourCC )
			{
			verdict = EPass;
			}
		else
			{
			verdict = EFail;
			}
		}

	return verdict;
	}


