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
#include "Tsu_mmf_afmt_FillBuffer.h"
#include "TSU_MMF_AFMTSuite.h"
#include "TSU_MMF_AFMT_testdatasink.h"
#include "TSU_MMF_AFMT_TestDataSource.h"

#include <mda/common/audio.hrh>
#include <mda/common/gsmaudio.hrh>						// for  KUidMdaGsmWavCodecDefine def

// Constants




/**
 * This function is used to call FillBufferL on the CMmfXXXFormatDecode member 
 * variable.  When this function completes, the buffer will have been filled
 * and passed back to this test class as 'iBufferReturned'
 * @param aDataSink - The object to be notified when the buffer has been 
 * filled.  For testing purposes, this should be our test DataSink, 'iDummySink'
 */
void CTestStep_MMF_AFMT_FillBufferL::CallFillBufferL(MDataSink& aDataSink, 
													 const TInt& iFrame)
	{	
	// Set the frame of data to retrieve...
	iBuffer->SetFrameNumber(iFrame);	

	// Make the FillBufferL call, which will return but not "be processed" until the 
	// active scheduler is started below.
	iFormatDec->FillBufferL(iBuffer, &aDataSink, KUidMediaTypeAudio);
	
	// This will block the Test Harness call stack, but allow
	// the active object (the CMmfClip) to fill the buffer.
	CActiveScheduler::Start();

	// ... If we get here, then the callback has been called
	// and stopped the Scheduler.  The buffer returned is stored 
	// in 'iBufferReturned' member variable.
	}

/**
*
* CallEmptyBufferL
*
*/ 

void CTestStep_MMF_AFMT_FillBufferL::CallEmptyBufferL(MDataSource& aDataSource )
	{
	iFormatEnc->EmptyBufferL(iBuffer, &aDataSource, KUidMediaTypeAudio);
	// This will block the Test Harness call stack, but allow
	// the active object (the CMmfClip) to empty the buffer.
	CActiveScheduler::Start();
	}

/**
 * CopyFileL
 * @param	"const TDesC& aOld"
 *			Source file.
 *
 * @param	"const TDesC& aNew"
 *			Destination file.
 *
 * @xxxx
 *
 */
void CTestStep_MMF_AFMT_FillBufferL::CopyFileL (const TDesC& aOld,const TDesC& aNew) 
	{
	// local file server
	RFs	iFs;
	// connect to file server
	TInt returnCode = iFs.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF1(_L("Error connecting to file server"));	
		User::Leave(returnCode);
		}

	// create a file manager
	CFileMan* fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);

	// parse the filenames
	TParse parseSource;
	returnCode = parseSource.Set(aOld, NULL, NULL);
	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);
		ERR_PRINTF3( _L("Failed to parse %S - %S"), 
			&aOld, 
			&errortxt);
		iFs.Close();
		User::Leave(returnCode);
		}
 
	// parse the filenames
	TParse parseTarget;
	returnCode = parseTarget.Set(aNew, NULL, NULL);
	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);
		ERR_PRINTF3(_L("Failed to parse %S - %S"), 
			&aNew, 
			&errortxt);
		iFs.Close();
		User::Leave(returnCode);
		}

	// do the copy
	returnCode = fileMan->Copy(parseSource.FullName(), 
		parseTarget.FullName(), 
		CFileMan::EOverWrite | CFileMan::ERecurse);

	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);

		ERR_PRINTF4(_L("Failed to copy %S to %S - %S"), 
			&parseSource.FullName(), 
			&parseTarget.FullName(),
			&errortxt);
		iFs.Close();
		User::Leave(returnCode);
		}

	INFO_PRINTF3(_L("Copied file from %S to %S"), 
					&parseSource.FullName(), 
					&parseTarget.FullName());

	iFs.Close();
	CleanupStack::PopAndDestroy(fileMan);
	}
	

/**
 * This function reads in a specified file to a descriptor, and compares this with the contents 
 * of a CMMFDataBuffer.  This is used in verifying that FillBufferL returned the correct data
 * from the source file.
 * @param aBuffer - The CMMF buffer whose contents are to be checked.
 * @param aFilename - The file containing source data to be compared with the buffer. 
 * @param aFrame - The frame number we are testing (from 1..)
 * @return TBool - If the buffer contents are what is expected, returns ETrue, 
 * otherwise it retuns EFalse.
 */
TBool CTestStep_MMF_AFMT_FillBufferL::CompareBufferWithDescriptorL(const CMMFDataBuffer& aBuffer, 
																   const TDesC& aFilename, 
																   const TInt& aFrame)
	{
	TBool same = EFalse;
	TInt offset = 0;
	TInt bufSize = aBuffer.Data().Size();
	TFourCC fourCC = iFormatDec->SourceDataTypeCode(KUidMediaTypeAudio);

	// Need to read iUid to see what type of data we're playing with.  From this, can work
	// out the size of the header of the file for the format, and therefore, the offset 
	// from where to start reading
	switch (iUID.iUid)
		{
		case KMmfUidFormatWAVRead:
			offset = 44;
			break;
		case KMmfUidFormatAURead:
			offset = 28;
			break;
		case KMmfUidFormatRAWRead:
			offset =0;
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	
	// NOTE: Never ANY header for RAW data format, so skip the header addition 
	// test.
	if (iUID.iUid != KMmfUidFormatRAWRead)
		{	
		switch(fourCC.FourCC()) 
			{
			case KMMFFourCCCodeGSM610:
			case KMMFFourCCCodeIMAD:
				offset += 4;	
				break;
			default:		
				offset +=0; 
			}
		}

	// Read file into iDescHBuf pointed to by 'iData'...
	ReadFileToDescriptorL(aFilename, ((aFrame-1)*bufSize)+offset , bufSize);

	// compare the buffer we have received with the expected buffer
	// stored in a file...
	if (aBuffer.Data().Compare(iData) == 0)
		same = ETrue;

	return same;	
	}

/**
 * This function is called by the dummy sink when it had it's buffer filled.
 * The parameter is the buffer that was filled, which is stored in the member
 * variable 'iBufferReturned'.  This allows us to check that the right buffer
 * and data was returned.
 * @param aBuffer The buffer which has been filled.
 */
void CTestStep_MMF_AFMT_FillBufferL::BufferFilledCallback(CMMFBuffer* aBuffer)
	{
	// Store the pointer to the buffer returned by the DataSource.
	iBufferReturned = (CMMFDataBuffer*) aBuffer;

	// Calling this will continue execution of the test harness
	// just after the ::Start() call in DoFillBufferLTest
	CActiveScheduler::Stop();
	}

/**
 * This function is called by the dummy source when it had it's buffer emptied
 * @param aBuffer The buffer which has been emptied.
 */
void CTestStep_MMF_AFMT_FillBufferL::BufferEmptiedCallback(CMMFBuffer* aBuffer )
	{
	// Store the pointer to the buffer returned by the DataSource.
	iBufferReturned = (CMMFDataBuffer*) aBuffer;

	// Calling this will continue execution of the test harness
	// just after the ::Start() call in DoFillBufferLTest
	CActiveScheduler::Stop();

	}

/**
 * This function checks that the correct buffer has been returned and that
 * the buffer contains the right data for the specified 'frame'.
 * @param aFrame - The frame of data to check (numbered from 1 onwards)
 * @return TVerdict Contains EPass if everything checks ok, otherwise EFail 
 * is returned.
 */
TVerdict CTestStep_MMF_AFMT_FillBufferL::Verdict(const TInt& aFrame)
	{
	TVerdict verdict = EFail;

	// Check that the buffer we passed and the one we got back are the same
	// and also that the data retrieved is correct.
	TRAPD(err,
		if ( iBufferReturned == iBuffer &&
			CompareBufferWithDescriptorL(*iBufferReturned, iFilename, aFrame) )
			{
			verdict = EPass;
			}
		else 
			{
			verdict = EFail;
			}
		);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CTestStep_MMF_AFMT_FillBufferL::Verdict() failed, error code %d"), err);
		verdict = EInconclusive;
		}

	return verdict;
	}

/**
 * This function overides the default SetupL of the baseclass with additional
 * functionality required.  This creates all of the objects required for the
 * FillBufferL tests.
 * @param aFilename - The file to act as the data source
 * @param aUid - The UID of the Format object (always a FormatRead UID)
 * @return TVerdict Should return EPass if setup ok.  This function may leave.
 */
TVerdict CTestStep_MMF_AFMT_FillBufferL::SetupL(const TDesC& aFilename, const TInt& aUid)
	{
	// Need an ActiveScheduler for this test
	CActiveScheduler* iScheduler = new(ELeave)CActiveScheduler;    
	CleanupStack::PushL( iScheduler );		
	CActiveScheduler::Install( iScheduler );
	CleanupStack::Pop(iScheduler);  // Pop the element from the stack

	// Create a buffer to be filled.
	iBuffer = CMMFDataBuffer::NewL(KFormatDefaultFrameSize);

	// Create the CMMFWavFormatRead object.
	CreateObjectL(ETrue, aFilename, aUid);

	// Store this for future use... :-)
	iFilename.Set(aFilename);

	// Create a dummy DataSink to get called back when the buffer is filled.
	// This is what receives the data from the FillBufferL call, and then 
	// passes the data onto this test harness via a call to Callback()
	TPtr8 dummy((TUint8*)this, 0,0);
	iDummySink = STATIC_CAST(MMFTestDataSink*, MDataSink::NewSinkL( TUid::Uid(KMmfTestDataSinkUid),  dummy )  );
	
	return EPass;
	}

/**
* This function overrides the SetUpL
*
*/
 TVerdict CTestStep_MMF_AFMT_FillBufferL::SetupL( const TDesC& aSrcFilename, const TInt& aSrcUid, const TDesC& aSinkFilename, const TInt& aSinkUid)
	{
	TVerdict verdict = EPass;
	// Check the size of the file, to check that there is enough data to do aRuns
	// interations of the test.  If not, just do as many as possible and add a 
	// comment to the log.
	iRuns = 1;
	iRuns = CheckMaxIterationsL( iRuns, aSrcFilename);

	iSrcFilename  = aSrcFilename;
	iSinkFilename = aSinkFilename;

	// Need an ActiveScheduler for this test
	CActiveScheduler* iScheduler = new(ELeave)CActiveScheduler;    
	CleanupStack::PushL( iScheduler );		
	CActiveScheduler::Install( iScheduler );
	CleanupStack::Pop(iScheduler);  // Pop the element from the stack

	// Create a buffer to be filled.
	iBuffer = CMMFDataBuffer::NewL(KFormatDefaultFrameSize);

	// Create the CMMFWavFormatRead object.
	CreateObjectL(ETrue, aSrcFilename, aSrcUid);

	// Create a dummy DataSink to get called back when the buffer is filled.
	// This is what receives the data from the FillBufferL call, and then 
	// passes the data onto this test harness via a call to Callback()
	TPtr8 dummy((TUint8*)this, 0,0);
	iDummySink = STATIC_CAST(MMFTestDataSink*, MDataSink::NewSinkL( TUid::Uid(KMmfTestDataSinkUid),  dummy )  );

	TPtr8 dummySrc((TUint8*)this, 0,0);
	iDummySource = STATIC_CAST(MMFTestDataSource*, MDataSource::NewSourceL( TUid::Uid(KMmfTestDataSourceUid),  dummySrc )  );
	
	// Create the data sink (stored in iDataSink)
	CreateDataSink(ETrue, aSinkFilename);

	// Make the NewL call.
	iFormatEnc = CMMFFormatEncode::NewL( TUid::Uid(aSinkUid), iDataSink);

	User::LeaveIfError(iFormatEnc->SinkThreadLogon(*this));

	iFormatEnc->SinkPrimeL();
	iFormatEnc->NegotiateL( *iFormatDec );
	iFormatEnc->SinkPlayL();

	// Store this for future use... :-)
	iFilename.Set(aSrcFilename);

	return verdict;
	}

/**
 * This function overrides the default Cleanup() function of the baseclass to 
 * tidy up the extra member variables used for the test.
 */
void CTestStep_MMF_AFMT_FillBufferL::Cleanup()
	{
	delete iDataSource;		iDataSource = NULL;
	delete iFormatDec;		iFormatDec = NULL;
	delete iFormatEnc;		iFormatEnc = NULL;
	delete iDataSink;		iDataSink = NULL;	

	delete iBuffer;		iBuffer = NULL;		
	delete iDummySink;	iDummySink = NULL;
	
	// Delete the active scheduler
	delete iScheduler;	iScheduler = NULL;
	}

/**
 * This function runs the fill buffer tests on the specified filename for 
 * the specified number of buffers.
 * @param aRuns - The number of buffers to be tested.  If '-1' is specified, 
 * it will test as many buffers as the source file allows.
 * @param aFilename - The file to COMPARE the data in the buffers with.  This 
 * has to be a different file from the SOURCE file, but containing the same data
 * as during the repeated runs of FillBufferL, the source file is locked.
 * @return TVerdict Returns EPass if all buffers tested contain the correct
 * data.  Otherwise returns EFail.
 */
TVerdict CTestStep_MMF_AFMT_FillBufferL::RunFillBufferTestL(const TInt& aRuns, 
															const TDesC& aFilename)
	{	
	INFO_PRINTF1(_L("In RunFillBufferTestL"));
	
	TVerdict verdict = EPass;

	// This is the file to COMPARE the data retrieved from the CMmfBuffer.
	iFilename.Set(aFilename);	

	// Check the size of the file, to check that there is enough data to do aRuns
	// interations of the test.  If not, just do as many as possible and add a 
	// comment to the log.
	TInt runs = CheckMaxIterationsL(aRuns, aFilename);


	TTimeIntervalMicroSeconds zero(0);
	TRAPD(err, iFormatDec->SetPositionL(zero));
	if(err != KErrNone && err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Failed to set position on Format err= %d"), err);
		verdict = EFail;
		}
	else
		{
		// Run through the file, testing upto 5 frames
		for (TInt frame=KStartFrame; frame< (KStartFrame+runs) ; frame++)
			{
			
			INFO_PRINTF2(_L("In Loop frame = %d "),frame );

			// This function calls FillBufferL, and waits for the BufferFilledL call to
			// come back with the buffer.  The buffer that has been filled is 
			// stored in iBufferFilled.	
			CallFillBufferL(*iDummySink, frame);

			// Check what we have got back is ok (compared to the expected data for
			// this frame
			if (Verdict(frame) != EPass)
				{
				INFO_PRINTF2(_L("Fail RunFillBufferTestL frame = %d"), frame);
				verdict = EFail;
				break;
				}
			}
		}

	INFO_PRINTF2(_L("In RunFillBufferTestL Verdict = %d"), verdict);
	return verdict;
	}


/**
*
* RunEmptyBufferTestL
*
*/

TVerdict CTestStep_MMF_AFMT_FillBufferL::RunEmptyBufferTestL()
	{
	TVerdict verdict = EFail;

	// Run through the file, testing up to 1
	for (TInt frame=KStartFrame; frame< (KStartFrame+iRuns) ; frame++)
		{
		// This function calls FillBufferL, and waits for the BufferFilledL call to
		// come back with the buffer.  The buffer that has been filled is 
		// stored in iBufferFilled.	
		CallFillBufferL(*iDummySink, frame);

		// ok lets now append the data to the sink file
		CallEmptyBufferL( *iDummySource );
		}

	    iDataSource->SourceStopL();
		iDataSink->SinkStopL();

		// ok now compare the length of the two files
		RFs testFsSession;
		User::LeaveIfError(testFsSession.Connect()) ;
		CleanupClosePushL(testFsSession);

		// Get the size of the test file.
		RFile srcFile;
		TFileName srcFileName(  iSrcFilename );
		User::LeaveIfError(srcFile.Open(testFsSession, srcFileName, EFileRead));
		CleanupClosePushL( srcFile);

		TInt srcFileSize;
		User::LeaveIfError(srcFile.Size(srcFileSize));

		// Get the size of the test file.
		RFile sinkFile;
		TFileName sinkFileName( iSinkFilename);
		User::LeaveIfError(sinkFile.Open(testFsSession, sinkFileName, EFileRead));
		CleanupClosePushL( sinkFile);

		TInt sinkFileSize;
		User::LeaveIfError(sinkFile.Size(sinkFileSize));
						
		CleanupStack::PopAndDestroy(3); // testFsSession srcFile, sinkFile
	
		if( srcFileSize < sinkFileSize )
			{
			verdict = EPass;
			}

	return verdict;
	}

/**
 * This function is used to make sure that for a given file, we don't try 
 * to read more buffers than there is data in the file.  Given the different
 * compression methods, some files can be very small, so this is just 
 * a safeguard on tests failing because of too small a test file.
 * @param aRuns - The number of buffers to fill.  This can be specified as '-1' in
 * which case the maximum number of buffers from the source file will be used.
 * @param aFilename - The file to be used as the datasource.
 * @return TInt - The number of buffers to be tested.
 */ 
TInt CTestStep_MMF_AFMT_FillBufferL::CheckMaxIterationsL(const TInt& aRuns, const TDesC& aFilename)
	{
	TInt size;
	RFs fs;
	RFile file;
	TInt suggestedRuns = 0;

	// Connect to the file server and open the file.
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL(fs);
	User::LeaveIfError( file.Open( fs, aFilename, EFileRead) );
	CleanupClosePushL(file);
	file.Size(size);

	// I say aRuns+1 because if the user is asking for 4k frame sizes, and 2 runs, but the file
	// is exactly 8k, we want to suggest 1 run, as we have to discount the header size (which may 
	// be between 0-80 bytes depending on the file format).  In this example, the 
	// file is split into header, 4k, and 3.95k chunks, which would only give one chunk.
	if ( ((aRuns+1)*((TInt)KFormatDefaultFrameSize)) > (size) || 
		 aRuns == -1)
		{
		suggestedRuns = size/(KFormatDefaultFrameSize+1);
		}
	else
		{
		suggestedRuns = aRuns;
		}

	// If the test file is too small, warn the user and fail the test.
	if (suggestedRuns == 0)
		{
		INFO_PRINTF1(_L("ERROR: Test file is too small for the buffer sizes required.  Try using a bigger file/smaller buffers"));
		User::Leave(KErrArgument);
		}
	else if (aRuns == -1) // doing MAX number of buffer tests.
		{
		INFO_PRINTF3(_L("Info: Doing as many tests as the source file allows - that's %d x %d buffers to be used"), suggestedRuns, KFormatDefaultFrameSize);		
		}
	else if (aRuns != suggestedRuns) // If we are here, then we have changed the number 
		{ // of runs so tell the user what is going on.
		INFO_PRINTF3(_L("Warning: The test file is too small for %d x %d buffers to be used"), aRuns, KFormatDefaultFrameSize);
		INFO_PRINTF4(_L("         Instead, %d x %d buffers will be used (ie %d iterations of the test)"), 
												suggestedRuns, 
												KFormatDefaultFrameSize, 
												suggestedRuns);
		}

	// Tidy up.
	CleanupStack::Pop(2);	// fs, file
	file.Close();
	fs.Close();

	return suggestedRuns;
	}

