
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

#ifndef __TSU_MMF_AFMT_FILLBUFFER_H__
#define __TSU_MMF_AFMT_FILLBUFFER_H__

#include <testframework.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatasink.h>

class MMFTestDataSource;

// This class is a helper class for all Audio Format FillBufferL unit tests.
class CTestStep_MMF_AFMT_FillBufferL : public CTestStep_MMF_AFMT
	{
public: // Constructor/Destructor
	CTestStep_MMF_AFMT_FillBufferL(){}
	virtual ~CTestStep_MMF_AFMT_FillBufferL(){}

protected: // New functions
	/**
	 * This function is used to call FillBufferL on the CMmfXXXFormatDecode member 
	 * variable.  When this function completes, the buffer will have been filled
	 * and passed back to this test class as 'iBufferReturned'
	 * @param aDataSink - The object to be notified when the buffer has been 
	 * filled.  For testing purposes, this should be our test DataSink, 'iDummySink'
	 */
	void CallFillBufferL(MDataSink& aDataSink, const TInt& iFrame);


	/**
	* This function is used to call EmptyBufferL on the CMmfXXXFormatEncode member 
	* variable. When this function completes the buffer will have been emptied
	*/
	void CallEmptyBufferL(MDataSource& aDataSource );

	/**
	* 
	* Copy filea to fileb
	*
	*/
	void CopyFileL (const TDesC& aOld,const TDesC& aNew); 


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
	TBool CompareBufferWithDescriptorL(	const CMMFDataBuffer& aBuffer, 
										const TDesC& aFilename, 
										const TInt& aFrame);


	/**
	 * This function is called by the dummy sink when it had it's buffer filled.
	 * The parameter is the buffer that was filled, which is stored in the member
	 * variable 'iBufferReturned'.  This allows us to check that the right buffer
	 * and data was returned.
	 * @param aBuffer The buffer which has been filled.
	 */
	void BufferFilledCallback(CMMFBuffer* aBuffer);

    /**
	 * This function is called by the dummy source when it had it's buffer is emptied
	 * The parameter is the buffer that was emptied, which is stored in the member
	 * variable 'iBufferReturned'.  .
	 * @param aBuffer The buffer which has been emptied.
	 */
	virtual void BufferEmptiedCallback(CMMFBuffer* aBuffer );	



	/**
	 * This function checks that the correct buffer has been returned and that
	 * the buffer contains the right data for the specified 'frame'.
	 * @param aFrame - The frame of data to check (numbered from 1 onwards)
	 * @return TVerdict Contains EPass if everything checks ok, otherwise EFail 
	 * is returned.
	 */
	TVerdict Verdict(const TInt& aFrame);


	/**
	 * This function overides the default SetupL of the baseclass with additional
	 * functionality required.  This creates all of the objects required for the
	 * FillBufferL tests.
	 * @param aFilename - The file to act as the data source
	 * @param aUid - The UID of the Format object (always a FormatRead UID)
	 * @return TVerdict Should return EPass if setup ok.  This function may leave.
	 */
	TVerdict SetupL(const TDesC& aFilename, const TInt& aUid);

	/**
	* This function provides an override for SetUpL of the basclass with additional functionalit
	* for Empty Buffer Tests
	*/
	 virtual TVerdict SetupL( const TDesC& aSrcFilename, const TInt& aSrcUid, const TDesC& aSinkFilename, const TInt& aSinkUid);


	/**
	 * This function overrides the default Cleanup() function of the baseclass to 
	 * tidy up the extra member variables used for the test.
	 */
	void Cleanup();	


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
	TVerdict RunFillBufferTestL(const TInt& aRuns, const TDesC& aFilename);

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
	TInt CheckMaxIterationsL(const TInt& aRuns, const TDesC& aFilename);

	/**
	*
	* RunEmptyBufferTestL
	*
	*/
	virtual TVerdict RunEmptyBufferTestL();

protected:
	CMMFDataBuffer*    iBuffer;			// The buffer passed to FillBufferL
	CMMFDataBuffer*    iBufferReturned;	// The buffer passed back in BufferFilledL
	MMFTestDataSink*   iDummySink;		// The dummy MDataSink based object
	MMFTestDataSource* iDummySource;    // The dummy MDataSource based object
	TInt               iRuns;           // temp store for the number of buffers to read
	TFileName          iSrcFilename ;   // src file name
	TFileName          iSinkFilename;   // sink file name 
private:
	CActiveScheduler* iScheduler;		// An active scheduler is required for the
										// asynchronous calls
	};

#endif
