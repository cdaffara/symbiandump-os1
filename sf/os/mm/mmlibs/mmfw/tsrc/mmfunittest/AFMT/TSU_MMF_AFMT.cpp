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
#include "TSU_MMF_AFMTSuite.h"


/** 
 * This function creates a datasource object based on either a descriptor
 * or a file.
 * @param aFile - If 'ETrue', datasource if file based, else it's descriptor based.
 * @param aFilename - If filebased, this is the file name passed to the DataSource
 *					  If descriptor based, the data is read from the file.
 * @param aOffset - The offset from the start of the file to read (for Descriptor
 *				     based DataSources only)
 */
void CTestStep_MMF_AFMT::CreateDataSource(const TBool& aFile, const TDesC& aFilename, 
										  const TUint& aOffset)
	{
	if (aFile)
		{ // File based DataSource (making a CMMFFile object)		
		TMMFFileParams fileParams ;
		fileParams.iPath = TFileName(aFilename) ;	
		TMMFFileConfig fileConfig( fileParams ) ;	

		// Create the iDataSource object.
		iDataSource = STATIC_CAST(CMMFFile*, MDataSource::NewSourceL( KUidMmfFileSource, 
												*STATIC_CAST(TDesC8*,&fileConfig)));
		}
	else 
		{ // Descriptor based DataSource
		// Reads first 4k buffer from file + puts in iData.
		ReadFileToDescriptorL(aFilename, aOffset, KFormatDefaultFrameSize);

		// Create parameters for the DataSource
		RThread thread;
		TMMFDescriptorParams descParams;
		descParams.iDes  = &iData;			
		descParams.iDesThreadId = thread.Id(); // This thread
		TMMFDescriptorConfig descConfig(descParams);

		// Create the MDataSource object.
		iDataSource = STATIC_CAST(CMMFDescriptor*, MDataSource::NewSourceL( 
											KUidMmfDescriptorSource, 
											*STATIC_CAST(TDesC8*,&descConfig) 
											));
		}
	}




/** 
 * This function creates a data sink object from a CMmfFile object.
 * @param aFile - If 'ETrue', datasource if file based, else it's descriptor based [NOTE 
 * use of CMmfDescriptor based sinks is currently not supported]
 * @param aFilename - If filebased, this is the file name passed to the DataSource
 *					  If descriptor based, the data is read from the file.
 * @param aOffset - The offset from the start of the file to read (for Descriptor
 *				     based DataSources only)
 */
void CTestStep_MMF_AFMT::CreateDataSink(const TBool& aFile, const TDesC& aFilename, 
										  const TUint& /*aOffset*/)
	{
	TInt err = KErrNone;

	if (aFile)
		{ // File based DataSink (making a CMMFFile object)		
		TMMFFileParams fileParams ;
		fileParams.iPath = TFileName(aFilename) ;	
		TMMFFileConfig fileConfig( fileParams ) ;	

		TRAP(err, iDataSink = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL( KUidMmfFileSink, 
												*STATIC_CAST(TDesC8*,&fileConfig))));
		}
	else 
		{ // Descriptor based DataSink		
		// Not sure how to code this bit up yet, so leave with Not Supported.
//		User::Leave(KErrNotSupported);	
			// Reads first 4k buffer from file + puts in iData.
//		ReadFileToDescriptorL(aFilename, aOffset, KFormatDefaultFrameSize);
		TRAP(err,
			iDescHBuf = HBufC8::NewL(0x1000);	
			TPtr8 dataBuf = iDescHBuf->Des();	

			iData.Set(dataBuf);

			// Create parameters for the DataSource
			RThread thread;
			TMMFDescriptorParams descParams;
			descParams.iDes  = &iData;			
			descParams.iDesThreadId = thread.Id(); // This thread
			TMMFDescriptorConfig descConfig(descParams);

			// Create the MDataSource object.
			iDataSink = STATIC_CAST(CMMFDescriptor*, MDataSink::NewSinkL( 
												KUidMmfDescriptorSink, 
												*STATIC_CAST(TDesC8*,&descConfig) 
												));
			);
		}

	// Check for errors
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CreateDataSink failed with error code %d"), err);
		}
	}




/** 
 * This function creates a Format object (type of which is specified by 'aUid') and
 * a data source or data sink to go with the object.  This function should be used
 * as a quick way for setting up objects for testing.
 * @param aIsFile -  ETrue if we are using CMmfFile or EFalse if using 
 * CMmfDescriptor as the DataSource. NOTE: CMmfDescriptor support has not
 * yet been added to this test harness for SINKs.  Generally it's best to just use
 * CMmfFile based data sources rather than descriptors.
 * @param aFilename - The file which acts as the data sink.  For a CMmfFile
 * based object, this is the file that data gets written to.
 * @param aUid - The UID of the format object to be created.
 */
void CTestStep_MMF_AFMT::CreateObjectL(TBool aIsFile, const TDesC& aFilename, TInt aUid)
	{
	// Use the UID to work out whether we want a Source/Read or Sink/Write pair
	switch (aUid)
		{
		case KMmfUidFormatAUWrite:
		case KMmfUidFormatRAWWrite:
		case KMmfUidFormatWAVWrite:
			// Create the CMMFWavFormatWrite object.
			CreateEncodeObjectL(aIsFile, aFilename, aUid); 
			iDecode = EFalse;
			break;

		case KMmfUidFormatAURead:
		case KMmfUidFormatRAWRead:
		case KMmfUidFormatWAVRead:
			// Create the CMMFWavFormatRead object.
			CreateDecodeObjectL(aIsFile, aFilename, aUid); 
			iDecode = ETrue;
			break;
		default:
			INFO_PRINTF1(_L("Leaving from Setup as UID not supported"));
			User::Leave(KErrNotSupported);
		}
	}



/** 
 * This function is used as a quick way to setup objects for testing.
 * @param aFilename - The file which acts as the data sink.  For a CMmfFile
 * based object, this is the file that data gets read/written.
 * @param aUid - The UID of the format object to be created.
 * @return TVerdict - Will always return EPass unless it leaves.
 */
TVerdict CTestStep_MMF_AFMT::SetupL(const TDesC& aFilename, TInt aUid)
	{
	CreateObjectL(ETrue, aFilename, aUid);
	return EPass;
	}



/**
 * This function deletes all of the pointer member variables which may have been 
 * initialised.
 */
void CTestStep_MMF_AFMT::Cleanup()
	{	
	if (iFormatDec)
		{
		iFormatDec->SourceThreadLogoff();
		}
	if (iFormatEnc)
		{
		iFormatEnc->SourceThreadLogoff();
		}


	// Tidyup
	delete iDataSource;		iDataSource = NULL;
	delete iFormatDec;		iFormatDec = NULL;
	delete iFormatEnc;		iFormatEnc = NULL;
	delete iDataSink;		iDataSink = NULL;
	delete iDescHBuf;		iDescHBuf = NULL;
	}





/**
 * This function reads from a source file into the member variable heap buffer, 'iDescHBuf'.
 * This can be used to compare the contents of a file with a buffer for example.
 * @param aFilename - The file to read from.
 * @param aPosition - Where to start reading from in the file.
 * @param aSize - How much of the file to read (in bytes) though if you pass '-1', it will
 * try and read the whole file from 'aPosition' onwards.
 */
void CTestStep_MMF_AFMT::ReadFileToDescriptorL(const TDesC& aFilename, 
											   TInt aPosition, 
											   TInt aSize)
	{	
	RFs rfs;
	rfs.Connect();
	RFile file;

	// Try to open the file.
	User::LeaveIfError( file.Open(rfs,aFilename,EFileRead|EFileShareAny) );

	// Create the databuffer in which to store the data.
	TInt fileSize = 0;
	file.Size(fileSize);
	if (aSize == -1)  // if -1, use whole file size
		iDescHBuf = HBufC8::NewL(fileSize-aPosition);	
	else 
		iDescHBuf = HBufC8::NewL(aSize);	
	TPtr8 dataBuf = iDescHBuf->Des();	

	// Check we are not trying to read past the end of the file...
	if (fileSize< (aPosition+aSize) )
		User::Leave(KErrEof);	

	// Seek to the place we want to start reading from
	TInt offset = aPosition;
	file.Seek(ESeekStart, offset);

	// Read the data from the file to the data buffer
	
	if(aSize == -1)
		{
		User::LeaveIfError(file.Read(dataBuf,fileSize-aPosition));
		}
	else
		{ 
		User::LeaveIfError(file.Read(dataBuf, aSize));	
		}
	
	iData.Set(dataBuf);
	
	file.Close();
	rfs.Close();	
	}





/**
 * This function can be used to make sure that a specified file is not present
 * before a set of tests are run.
 * @param aFilename - The name of the file to delete/check is not present.
 */
void CTestStep_MMF_AFMT::DeleteFileL(const TDesC& aFilename)
	{
	RFs fs;	
	User::LeaveIfError( fs.Connect() );
	TInt ret = fs.Delete(aFilename);
	if (ret != KErrNone && ret != KErrNotFound)
		User::Leave(ret);
	}




/** 
 * This function creates a datasource object based on either a descriptor
 * or a file.
 * @param aIsFile -  ETrue if we are using CMmfFile or EFalse if using 
 * CMmfDescriptor as the DataSource
 * @param aFilename - The file which acts as the data source.  For a CMmfFile
 * based object, this IS the file.  For a CMmfDescriptor based object, this
 * file contains the data that is read into the descriptor for test purposes.
 * @param aUid - The UID of the format object to be created.
 */
void CTestStep_MMF_AFMT::CreateDecodeObjectL(TBool aIsFile, const TDesC& aFilename, TInt aUid)
	{	
	// Create the datasource (stored in iDataSource)
	CreateDataSource(aIsFile, aFilename);

	iUID = TUid::Uid(aUid);

	// Make the NewL call.
	iFormatDec = CMMFFormatDecode::NewL( iUID, iDataSource);

	User::LeaveIfError(iFormatDec->SourceThreadLogon(*this));
	iFormatDec->SourcePrimeL();
	}

/** 
 * This function creates a datasink object based on either a descriptor
 * or a file.
 * @param aIsFile -  ETrue if we are using CMmfFile or EFalse if using 
 * CMmfDescriptor as the DataSource. NOTE: CMmfDescriptor support has not
 * yet been added to this test harness for SINKs.
 * @param aFilename - The file which acts as the data sink.  For a CMmfFile
 * based object, this IS the file.  
 * @param aUid - The UID of the format object to be created.
 */
void CTestStep_MMF_AFMT::CreateEncodeObjectL(TBool aIsFile, const TDesC& aFilename, TInt aUid)
	{	
	// Create the data sink (stored in iDataSink)
	CreateDataSink(aIsFile, aFilename);

	iUID = TUid::Uid(aUid);

	// Make the NewL call.
	iFormatEnc = CMMFFormatEncode::NewL( iUID, iDataSink);

	User::LeaveIfError(iFormatEnc->SourceThreadLogon(*this));
	}

/**
 *
 * Destructor for this class.
 *
 */
CTestStep_MMF_AFMT::~CTestStep_MMF_AFMT()
	{
	Cleanup();
	}

/**
 *
 * Close
 *
 */
void CTestStep_MMF_AFMT::Close()
	{
	Cleanup(); 
	}

