// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_AFMT_H__
#define __TSU_MMF_AFMT_H__

#include <testframework.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatasink.h>

// Forward declaration
class MMFTestDataSink;

class CTestSuite ;
class CTestSuite_MMF_AFMT;

class MDataSource;
class CMMFFormatDecode;
class CMMFFormatEncode;

// Constants
const TUint KFormatDefaultFrameSize = 0x1000;
const TUint KGSMAvgBytesPerSecond = 1625;
const TInt KOneSecondInMicroSeconds(1000000);


// NOTE: With the test files for audio formats, we have to use PCM only, as
// any other compression type appears to require a whole 'chain' of codecs etc
// to initialise correctly.  This would be better done in System testing.
#ifdef __WINS__
	#include "Tsu_mmf_afmt_WINS.h"
#else 
	// ARM build - if flag below set we use C drive for files
	#ifdef __AFMT_USE_C_DRIVE__
		#include "Tsu_mmf_afmt_WINS.h"
	#else
		#include "Tsu_mmf_afmt_ARM4.h"
	#endif // __AFMT_USE_C_DRIVE__
#endif


// Other constants
const TInt KStartFrame	= 1;				// The first frame in a file.



enum ENewLType {KNewLUndefined, // Default value (leave if we get this).
				KNewLUid,		// Call NewL with a UID
				KNewLFilename,	// Call NewL with a filename
				KNewLDescriptor,// Call NewL with a Descriptor of data
				KNewLClip		// Call NewL passing in a CMMFClip
				};

/** @xxxx
 * @class CTestStep_MMF_AFMT TSU_MMF_AFMT.h "TSU_MMF_AFMT.h"
 * @brief Test class that enables MMF Audio Format tests.
 * It's a base class for all test steps.
 */
class CTestStep_MMF_AFMT : public CTestStep, public MAsyncEventHandler
	{
public:	// Constructor/Destructor
	CTestStep_MMF_AFMT() : iData(0,0,0) {};
	virtual ~CTestStep_MMF_AFMT();
	virtual void Close();
	
public: // From CTestStep
	void SetTestSuite(const CTestSuite_MMF_AFMT* aTestSuite )
		{ iAFMTTestSuite = aTestSuite;}  

public: // New functions
	virtual void BufferEmptiedCallback(CMMFBuffer* /*aBuffer*/){}	
	virtual void BufferFilledCallback(CMMFBuffer* /*aBuffer*/){}	

	
protected:	// New functions

	/** 
	 * This function creates a datasource object based on either a descriptor
	 * or a file.
	 * @param aFile - If 'ETrue', datasource if file based, else it's descriptor based.
	 * @param aFilename - If filebased, this is the file name passed to the DataSource
	 *					  If descriptor based, the data is read from the file.
	 * @param aOffset - The offset from the start of the file to read (for Descriptor
	 *				     based DataSources only)
	 */
	void CreateDataSource(const TBool& aFile, const TDesC& aFilename, const TUint& aOffset=0);


	/** 
	 * This function creates a data sink object from a CMmfFile object.
	 * @param aFile - If 'ETrue', datasource if file based, else it's descriptor based [NOTE 
	 * use of CMmfDescriptor based sinks is currently not supported]
	 * @param aFilename - If filebased, this is the file name passed to the DataSource
	 *					  If descriptor based, the data is read from the file.
	 * @param aOffset - The offset from the start of the file to read (for Descriptor
	 *				     based DataSources only)
	 */
	void CreateDataSink(const TBool& aFile, const TDesC& aFilename, const TUint& aOffset=0);


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
	void CreateObjectL(TBool aIsFile, const TDesC& aFilename, TInt aUid);


	/** 
	 * This function is used as a quick way to setup objects for testing.
	 * @param aFilename - The file which acts as the data sink.  For a CMmfFile
	 * based object, this is the file that data gets read/written.
	 * @param aUid - The UID of the format object to be created.
	 * @return TVerdict - Will always return EPass unless it leaves.
	 */
	TVerdict SetupL(const TDesC& aFilename, TInt aUid);


	/**
	 * This function deletes all of the pointer member variables which may have been 
	 * initialised.
	 */
	void Cleanup();


	/**
	 * This function reads from a source file into the member variable heap buffer, 'iDescHBuf'.
	 * This can be used to compare the contents of a file with a buffer for example.
	 * @param aFilename - The file to read from.
	 * @param aPosition - Where to start reading from in the file.
	 * @param aSize - How much of the file to read (in bytes) though if you pass '-1', it will
	 * try and read the whole file from 'aPosition' onwards.
	 */
	void ReadFileToDescriptorL(const TDesC& aFilename, TInt aPosition, TInt aSize);


	/**
	 * This function can be used to make sure that a specified file is not present
	 * before a set of tests are run.
	 * @param aFilename - The name of the file to delete/check is not present.
	 */
	void DeleteFileL(const TDesC& aFilename);
		

private: // New functions
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
	void CreateDecodeObjectL(TBool aBool, const TDesC& aFilename, TInt aUid);


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
	void CreateEncodeObjectL(TBool aIsFile, const TDesC& aFilename, TInt aUid);

	//from MAsyncEventHandler
	virtual TInt SendEventToClient(const TMMFEvent& /*aEvent*/) {return KErrNone;}//XXX - can we do better here?


protected:	
	TBool iDecode;	// ETrue if using CMmfXXXFormatRead/MDataSource
					// EFalse if using CMmfXXXFormatWrite/MDataSink

	const CTestSuite_MMF_AFMT* iAFMTTestSuite; // pointer to suite which owns this test

	MDataSource* iDataSource;		// Ptr to a DataSource
	MDataSink*	 iDataSink;			// Ptr to a DataSource

	CMMFFormatDecode* iFormatDec;	// Ptr to a Format Decode object
	CMMFFormatEncode* iFormatEnc;	// Ptr to a Format Encode object

	TUid iUID;						// UID object
	TPtrC iFilename;				// Filename
	TPtrC iPreferredSupplier;		// Preferred supplier string
//PAC	TPtrC8 iData;					// Used to hold binary data.
	TPtr8 iData;					// Used to hold binary data.
	ENewLType iNewL;				// Which NewL call to use when create Format object

	HBufC8* iDescHBuf;				// HBuf for holding descriptor data	

private:
	};








#endif// TSU_MMF_AFMT_H__

