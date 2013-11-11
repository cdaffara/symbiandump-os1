// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <mmf/plugin/mmfsbccodecimplementationuids.hrh>

#include "TSU_MMF_SbcCodec.h"
#include "TSU_MMF_SbcCodecSuite.h"

/**
 * Size of PCM16 wave file header is 44
 */
const TInt  KWavFileHeaderSize = 44;
/**
 * The default buffer size
 */
const TUint KDefaultBufferSize = 4096;

/**
 * UID of sbc encoder ECom plugin
 */
const TUid  KSbcCodecPluginUid = { KMmfUidCodecPCM16ToSBC };
/**
 * UID of sbc encoder configuration
 */
const TUid	KSbcCodecConfigUid = { KMmfUidSBCConfigure };

#ifdef __WINS__

_LIT(KSourceFileDir, "c:\\mm\\mmf\\testfiles\\sbc\\");
_LIT(KOutputFileDir, "c:\\mm\\mmf\\testfiles\\sbc\\");
_LIT(KReferenceFileDir, "c:\\mm\\mmf\\ref\\sbc\\");

#else

/**
 * armv5 directories
 * All source and comparison files need to be copied manually onto the flash card.
 * This code assumes the flash card is represented by drive E: on the target board.
 */

/**
 * we don't copy all the wave files onto C: as they are too big, leave them on E:
 */
_LIT(KSourceFileDir, "e:\\");
/**
 * This folder keeps all the output files
 */
_LIT(KOutputFileDir, "c:\\sbc\\");
/**
 * This folder keeps all the reference files for comparison
 */
_LIT(KReferenceFileDir, "c:\\sbc\\ref\\");

#endif

// Test source files names
_LIT(KTestWavFile01, "sbc_test_01.wav");
_LIT(KTestWavFile02, "sbc_test_02.wav");
_LIT(KTestWavFile03, "sbc_test_03.wav"); 
_LIT(KTestWavFile04, "sbc_test_04.wav");
_LIT(KTestWavFile05, "sbc_test_05.wav"); 
_LIT(KTestWavFile06, "sbc_test_06.wav");
_LIT(KTestWavFile07, "sbc_test_07.wav"); 
_LIT(KTestWavFile08, "sbc_test_08.wav");
_LIT(KTestWavFile09, "sbc_test_09.wav"); 
_LIT(KTestWavFile10, "sbc_test_10.wav");

_LIT(KTestWavFile11, "sbc_test_11.wav"); 
_LIT(KTestWavFile12, "sbc_test_12.wav");
_LIT(KTestWavFile13, "sbc_test_13.wav"); 
_LIT(KTestWavFile14, "sbc_test_14.wav");
_LIT(KTestWavFile15, "sbc_test_15.wav"); 
_LIT(KTestWavFile16, "sbc_test_16.wav");
_LIT(KTestWavFile17, "sbc_test_17.wav"); 
_LIT(KTestWavFile18, "sbc_test_18.wav");
_LIT(KTestWavFile19, "sbc_test_19.wav"); 
_LIT(KTestWavFile20, "sbc_test_20.wav");

_LIT(KTestWavFile21, "sbc_test_21.wav"); 
_LIT(KTestWavFile22, "sbc_test_22.wav");
_LIT(KTestWavFile23, "sbc_test_23.wav"); 
_LIT(KTestWavFile24, "sbc_test_24.wav");
_LIT(KTestWavFile25, "sbc_test_25.wav"); 
_LIT(KTestWavFile26, "sbc_test_26.wav");
_LIT(KTestWavFile27, "sbc_test_27.wav"); 
_LIT(KTestWavFile28, "sbc_test_28.wav");

// Test reference file name
_LIT(KRefSbcFile01, "sbc_test_01.sbc"); 
_LIT(KRefSbcFile02, "sbc_test_02.sbc");
_LIT(KRefSbcFile03, "sbc_test_03.sbc"); 
_LIT(KRefSbcFile04, "sbc_test_04.sbc");
_LIT(KRefSbcFile05, "sbc_test_05.sbc"); 
_LIT(KRefSbcFile06, "sbc_test_06.sbc");
_LIT(KRefSbcFile07, "sbc_test_07.sbc"); 
_LIT(KRefSbcFile08, "sbc_test_08.sbc");
_LIT(KRefSbcFile09, "sbc_test_09.sbc"); 
_LIT(KRefSbcFile10, "sbc_test_10.sbc");

_LIT(KRefSbcFile11, "sbc_test_11.sbc"); 
_LIT(KRefSbcFile12, "sbc_test_12.sbc");
_LIT(KRefSbcFile13, "sbc_test_13.sbc"); 
_LIT(KRefSbcFile14, "sbc_test_14.sbc");
_LIT(KRefSbcFile15, "sbc_test_15.sbc"); 
_LIT(KRefSbcFile16, "sbc_test_16.sbc");
_LIT(KRefSbcFile17, "sbc_test_17.sbc"); 
_LIT(KRefSbcFile18, "sbc_test_18.sbc");
_LIT(KRefSbcFile19, "sbc_test_19.sbc"); 
_LIT(KRefSbcFile20, "sbc_test_20.sbc");

_LIT(KRefSbcFile21, "sbc_test_21.sbc"); 
_LIT(KRefSbcFile22, "sbc_test_22.sbc");
_LIT(KRefSbcFile23, "sbc_test_23.sbc"); 
_LIT(KRefSbcFile24, "sbc_test_24.sbc");
_LIT(KRefSbcFile25, "sbc_test_25.sbc"); 
_LIT(KRefSbcFile26, "sbc_test_26.sbc");
_LIT(KRefSbcFile27, "sbc_test_27.sbc"); 
_LIT(KRefSbcFile28, "sbc_test_28.sbc");

/**
 * List of channel mode settings
 */
enum TChannelMode
	{
	/**
	channel mode is Mono
	*/
	EMono,
	/**
	channel mode is Dual Channel
	*/
	EDual,
	/**
	channel mode is Stereo
	*/
	EStereo,
	/**
	channel mode is Joint Stereo
	*/
	EJoint
	};

/**
 * List of allocation method settings
 */
enum TAllocMethod
	{
	/**
	allocation method is SNR
	*/
	ESNR,
	/**
	allocation method is Loudness
	*/
	ELoud
	};
	
/**
 * Table of all 28 tests configurations, each row contains the following fields in order
 * subbands, block length, sampling frequency, channel mode, allocation method, bitpool
 */
const TInt gSbcConfig[28][6] = 
	{
		// configuration tests (8)
		{ 4, 16, 48000, EMono,   ESNR,   18 },
		{ 4, 16, 48000, EDual,   ESNR,   16 },
		{ 8,  8, 44100, EMono,   ELoud,  32 },
		{ 8,  8, 44100, EJoint,  ELoud,  56 },
		{ 8,  4, 32000, EMono,   ESNR,   24 },
		{ 8,  4, 32000, EStereo, ESNR,   48 },
		{ 4, 12, 16000, EMono,   ELoud,  20 },
		{ 4, 12, 16000, EJoint,  ELoud,  42 },
		
		// vbr tests (2)
		{ 4, 16, 44100, EMono,   ELoud,  15 },
		{ 8, 12, 48000, EJoint,  ELoud,  51 },
		
		// maximum bit rate tests (8)
		{ 8, 16, 16000, EMono,   ELoud, 128 },
		{ 8, 16, 16000, EJoint,  ESNR,  249 },
		{ 8, 16, 32000, EMono,   ELoud,  76 },
		{ 8, 16, 32000, EJoint,  ESNR,  121 },
		{ 8, 16, 44100, EMono,   ELoud,  54 },
		{ 8, 16, 44100, EJoint,  ESNR,   86 },
		{ 8, 16, 48000, EMono,   ELoud,  49 },
		{ 8, 16, 48000, EJoint,  ESNR,   78 },

		// special tests (2)
		{ 8, 16, 48000, EMono,   ESNR,   29 },
		{ 8, 16, 44100, EJoint,  ESNR,   53 },

		// typical tests (8)
		{ 8, 16, 44100, EMono,   ELoud,  19 },
		{ 8, 16, 48000, EMono,   ELoud,  18 },
		{ 8, 16, 44100, EJoint,  ELoud,  35 },
		{ 8, 16, 48000, EJoint,  ELoud,  33 },
		{ 8, 16, 44100, EMono,   ELoud,  31 },
		{ 8, 16, 48000, EMono,   ELoud,  29 },
		{ 8, 16, 44100, EJoint,  ELoud,  53 },
		{ 8, 16, 48000, EJoint,  ELoud,  51 }
	};

/**
 * This function converts number of subbands into TSBCFrameParameters::TSubbands value
 * @param 	"TInt aSubbands"
 *		  	number of subbands
 * @return	"TSBCFrameParameters::TSubbands"
 *			TSBCFrameParameters::TSubbands value
 */
static inline TSBCFrameParameters::TSubbands GetSubbandsL(TInt aSubbands)
	{
	switch (aSubbands)
		{
		case 4: return TSBCFrameParameters::E4Subbands;
		case 8: return TSBCFrameParameters::E8Subbands;
		}
	User::Leave(KErrArgument);
	return TSBCFrameParameters::E4Subbands; // just make the compiler happy
	}
	
/**
 * This function converts number of blocks into TSBCFrameParameters::TBlockLength value
 * @param 	"TInt aBlocks"
 *		  	number of blocks
 * @return	"TSBCFrameParameters::TBlockLength"
 *			TSBCFrameParameters::TBlockLength value
 */
static inline TSBCFrameParameters::TBlockLength GetBlockLengthL(TInt aBlocks)
	{
	switch (aBlocks)
		{
		case 4: return TSBCFrameParameters::E4Blocks;
		case 8: return TSBCFrameParameters::E8Blocks;
		case 12: return TSBCFrameParameters::E12Blocks;
		case 16: return TSBCFrameParameters::E16Blocks;
		}
	User::Leave(KErrArgument);
	return TSBCFrameParameters::E4Blocks; //just make the compiler happy
	}

/**
 * This function converts sampling frequency value into TSBCFrameParameters::TSamplingFrequency value
 * @param 	"TInt aSampFreq"
 *		  	real sampling frequency value
 * @return	"TSBCFrameParameters::TSamplingFrequency"
 *			TSBCFrameParameters::TSamplingFrequency value
 */
static inline TSBCFrameParameters::TSamplingFrequency GetSampFreqL(TInt aSampFreq)
	{
	switch (aSampFreq)
		{
		case 16000: return TSBCFrameParameters::E16000Hz;
		case 32000: return TSBCFrameParameters::E32000Hz;
		case 44100: return TSBCFrameParameters::E44100Hz;
		case 48000: return TSBCFrameParameters::E48000Hz;
		}
	User::Leave(KErrArgument);
	return TSBCFrameParameters::E16000Hz; // just make the compiler happy
	}

/**
 * This function converts ChannelMode enum value into TSBCFrameParameters::TChannelMode value
 * @param 	"TInt aChnlMode"
 *		  	ChannelMode enum value
 * @return	"TSBCFrameParameters::TChannelMode"
 *			TSBCFrameParameters::TChannelMode value
 */
static inline TSBCFrameParameters::TChannelMode GetChannelModeL(TInt aChnlMode)
	{
	switch (aChnlMode)
		{
		case EMono: return TSBCFrameParameters::EMono;
		case EDual: return TSBCFrameParameters::EDualChannel;
		case EStereo: return TSBCFrameParameters::EStereo;
		case EJoint: return TSBCFrameParameters::EJointStereo;
		}
	User::Leave(KErrArgument);
	return TSBCFrameParameters::EMono; // just make the compiler happy
	}
	
/**
 * This function converts AllocMethod enum value into TSBCFrameParameters::TAllocationMethod value
 * @param 	"TInt aAllocMthd"
 *		  	AllocMethod enum value
 * @return	"TSBCFrameParameters::TAllocationMethod"
 *			TSBCFrameParameters::TAllocationMethod value
 */
static inline TSBCFrameParameters::TAllocationMethod GetAllocMethodL(TInt aAllocMthd)
	{
	switch (aAllocMthd)
		{
		case ESNR: return TSBCFrameParameters::ESNR;
		case ELoud: return TSBCFrameParameters::ELoudness;
		}
	User::Leave(KErrArgument);
	return TSBCFrameParameters::ESNR;
	}
	
/**
 * This function gets the configuration for one test from SbcConfig[28]
 * @param 	"TInt aTestNumber"
 *		  	test number
 * @return	"TSBCFrameParameters"
 *			the sbc frame parameters used to configure sbc codec
 */
static TSBCFrameParameters SbcParametersL(TInt aTestNumber)
	{
	if (aTestNumber < 0 || aTestNumber > 28)
		{
		User::Leave(KErrArgument);
		}
		
	TSBCFrameParameters param;
	param.SetSubbands(GetSubbandsL(gSbcConfig[aTestNumber - 1][0]) );
	param.SetBlockLength(GetBlockLengthL(gSbcConfig[aTestNumber - 1][1]) );
	param.SetSamplingFrequency(GetSampFreqL(gSbcConfig[aTestNumber - 1][2]) );
	param.SetChannelMode(GetChannelModeL(gSbcConfig[aTestNumber - 1][3]) );
	param.SetAllocationMethod(GetAllocMethodL(gSbcConfig[aTestNumber - 1][4]) );
	param.SetBitpool(static_cast<TUint8>(gSbcConfig[aTestNumber - 1][5]) );
	return param;
	}
	
/**
 * Constructor
 */
CTestStep_MMF_SbcCodec::CTestStep_MMF_SbcCodec()
	{
	}

/**
 * Destructor
 */
CTestStep_MMF_SbcCodec::~CTestStep_MMF_SbcCodec()
	{
	}

/**
 * This function gets test source file name for one test
 * @param 	"TInt aTestNumber"
 *		  	test number
 * @return 	"const TDesC&"
 * 			test file name
 */
const TDesC& CTestStep_MMF_SbcCodec::GetTestFileName(TInt aTestNumber)
	{
	switch (aTestNumber)
		{
		case 1 : return KTestWavFile01;
		case 2 : return KTestWavFile02;
		case 3 : return KTestWavFile03;
		case 4 : return KTestWavFile04;
		case 5 : return KTestWavFile05;
		case 6 : return KTestWavFile06;
		case 7 : return KTestWavFile07;
		case 8 : return KTestWavFile08;
		case 9 : return KTestWavFile09;
		case 10: return KTestWavFile10;

		case 11: return KTestWavFile11;
		case 12: return KTestWavFile12;
		case 13: return KTestWavFile13;
		case 14: return KTestWavFile14;
		case 15: return KTestWavFile15;
		case 16: return KTestWavFile16;
		case 17: return KTestWavFile17;
		case 18: return KTestWavFile18;
		case 19: return KTestWavFile19;
		case 20: return KTestWavFile20;

		case 21: return KTestWavFile21;
		case 22: return KTestWavFile22;
		case 23: return KTestWavFile23;
		case 24: return KTestWavFile24;
		case 25: return KTestWavFile25;
		case 26: return KTestWavFile26;
		case 27: return KTestWavFile27;
		case 28: return KTestWavFile28;
		}
	User::Panic(_L("Invalid test number"), KErrArgument);
	return KTestWavFile01;
	}
	
/**
 * This function gets test reference file name for one test
 * @param 	"TInt aTestNumber"
 *		  	test number
 * @return 	"const TDesC&"
 * 			reference file name
 */
const TDesC& CTestStep_MMF_SbcCodec::GetRefFileName(TInt aTestNumber)
	{
	switch (aTestNumber)
		{
		case 1 : return KRefSbcFile01;
		case 2 : return KRefSbcFile02;
		case 3 : return KRefSbcFile03;
		case 4 : return KRefSbcFile04;
		case 5 : return KRefSbcFile05;
		case 6 : return KRefSbcFile06;
		case 7 : return KRefSbcFile07;
		case 8 : return KRefSbcFile08;
		case 9 : return KRefSbcFile09;
		case 10: return KRefSbcFile10;

		case 11: return KRefSbcFile11;
		case 12: return KRefSbcFile12;
		case 13: return KRefSbcFile13;
		case 14: return KRefSbcFile14;
		case 15: return KRefSbcFile15;
		case 16: return KRefSbcFile16;
		case 17: return KRefSbcFile17;
		case 18: return KRefSbcFile18;
		case 19: return KRefSbcFile19;
		case 20: return KRefSbcFile20;

		case 21: return KRefSbcFile21;
		case 22: return KRefSbcFile22;
		case 23: return KRefSbcFile23;
		case 24: return KRefSbcFile24;
		case 25: return KRefSbcFile25;
		case 26: return KRefSbcFile26;
		case 27: return KRefSbcFile27;
		case 28: return KRefSbcFile28;
		}
	User::Panic(_L("Invalid test number"), KErrArgument);
	return KRefSbcFile01;
	}
	
/**
 * This function reads source out from source file from a specific position 
 and fill the srouce into the source buffer.
 * @param 	"const TDesC& aFileName"
 *		  	the source file name
 * @param 	"TDes8& aSrcBuffer"
 *		  	the source buffer
 * @param 	"TInt aFilePos"
 *		  	the file position
 * @leave if reading file failed
 */
void CTestStep_MMF_SbcCodec::ReadSourceL(const TDesC& aFileName, TDes8& aSrcBuffer, TInt aFilePos)
	{
	TFileName srcFileName(KSourceFileDir);
	srcFileName.Append(aFileName);
	
	// connect to file service
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile srcFile;
	User::LeaveIfError(srcFile.Open(fs, srcFileName, EFileRead | EFileShareAny) );
	CleanupClosePushL(srcFile);
	
	aFilePos += KWavFileHeaderSize; // skip wave file header
	User::LeaveIfError(srcFile.Seek(ESeekStart, aFilePos) );
	User::LeaveIfError(srcFile.Read(aSrcBuffer) );
	
	CleanupStack::PopAndDestroy(2);
	}
	
/**
 * This function writes destination into destination file from a specific position.
 * @param 	"const TDesC& aFileName"
 *		  	the destination file name
 * @param 	"TDes8& aDstBuffer"
 *		  	the destination buffer
 * @param 	"TInt aFilePos"
 *		  	the file position
 * @leave if writing file failed
 */
void CTestStep_MMF_SbcCodec::WriteDstToFileL(const TDesC& aFileName, const TDes8& aDstBuffer, TInt aFilePos)
	{
	TFileName dstFileName(KOutputFileDir);
	dstFileName.Append(aFileName);
	
	// connect to file service
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile dstFile;
	TInt err = dstFile.Open(fs, dstFileName, EFileWrite);
	if (err == KErrNotFound) // file does not exist - create it
		{
		err = dstFile.Create(fs, dstFileName, EFileWrite);
		}
	if (err != KErrNone)
		{
		User::Leave(err);
		}
    CleanupClosePushL(dstFile);
	
	User::LeaveIfError(dstFile.Seek(ESeekStart, aFilePos) );
	User::LeaveIfError(dstFile.Write(aDstBuffer) );
		
	CleanupStack::PopAndDestroy(2);
	}

/**
 * This function compares two files to check if they are the same.
 * @param 	"const TDesC& aFileName"
 *		  	the compare and reference files name
 * @return	"TInt"
 * 			0: two files are the same
 *			-1: two files are NOT the same
 * @leave if reading tow files failed
 */
TInt CTestStep_MMF_SbcCodec::CompareFileL(const TDesC& aFileName)
	{
	TFileName cmpFileName(KOutputFileDir);
	cmpFileName.Append(aFileName);
	
	TFileName refFileName(KReferenceFileDir);
	refFileName.Append(aFileName);
	
	// connect to file service
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile cmpFile;
	User::LeaveIfError(cmpFile.Open(fs, cmpFileName, EFileRead) );
    CleanupClosePushL(cmpFile);
    
	RFile refFile;
	User::LeaveIfError(refFile.Open(fs, refFileName, EFileRead) );
    CleanupClosePushL(refFile);
    
    TInt cmpFileSize = 0;
    TInt refFileSize = 0;
    
    cmpFile.Size(cmpFileSize);
    refFile.Size(refFileSize);
    
    if (cmpFileSize != refFileSize)
    	{
		CleanupStack::PopAndDestroy(3); // refFile, cmpFile, fs
    	return -1;
    	}
    	
	CMMFDataBuffer* cmpFileBuf = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(cmpFileBuf);
	CMMFDataBuffer* refFileBuf = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(refFileBuf);
    
    TInt result = 0;
    for (;;)
    	{
    	User::LeaveIfError(cmpFile.Read(cmpFileBuf->Data() ) );
    	if (cmpFileBuf->Data().Length() == 0) // end of file
    		{
    		break;
    		}
    	
    	User::LeaveIfError(refFile.Read(refFileBuf->Data() ) );
    	if (cmpFileBuf->Data() != refFileBuf->Data() )
    		{
    		result = -1;
    		break;
    		}
    		
    	cmpFileBuf->Data().Zero();
    	refFileBuf->Data().Zero();
    	}
    	
	CleanupStack::PopAndDestroy(5); // refFileBuff, cmpFileBuff, refFile, cmpFile, fs
	return result;
	}
	
/**
 * This function is called by two TestNewL() tests, it calls ProcessL() once 
 * and checks the result.
 * @param 	"CMMFCodec* aCodec"
 * 			the codec for test
 * @return	"TVerdict"
 *			EPass: at least one byte src processed and one byte of dst generated and no error 
 *			EFail: other wise.
 * @leave if out of memory or ProcessL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::ProcessOnceForNewL(CMMFCodec* aCodec)
	{
	TVerdict testResult = EFail;
	
	// create buffers
	CMMFDataBuffer* srcBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer* dstBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(dstBuffer);
	
	TInt testNumber = 28;
	TPckgBuf<TSBCFrameParameters> config(SbcParametersL(testNumber) );
	aCodec->ConfigureL(KSbcCodecConfigUid, config);
	
	ReadSourceL(GetTestFileName(testNumber), srcBuffer->Data(), 0);
		
	if (srcBuffer->Data().Length() > 0) // end of file
		{
		TCodecProcessResult result = aCodec->ProcessL(*srcBuffer, *dstBuffer);
		
		if (result.iDstBytesAdded > 0 && result.iSrcBytesProcessed > 0)
			{
			testResult = EPass;
			}
		}
	else
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::ProcessOnceForNewL no source read") );
		}

	CleanupStack::PopAndDestroy(2); // dstBuffer, srcBuffer

	return testResult;
}

/**
 * This function tests codec instantiation by ECom plugin UID.
 * @param 	"TUid aCodecUID"
 * 			the codec ECom UID
 * @return	"TVerdict"
 *			EPass: ProcessOnceForNewL() passes 
 *			EFail: ProcessOnceForNewL() fails
 * @leave if CMMFCodec::NewL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestNewL(TUid aCodecUID)
	{
	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(aCodecUID) );
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_SbcCodec::TestNewL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	if (!codec)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestNewL NewL returned NULL") );
		return EFail;
		}

	CleanupStack::PushL(codec);
	TVerdict result = ProcessOnceForNewL(codec);
	CleanupStack::PopAndDestroy(); //codec
	return result;
	}

/**
 * This function tests codec instantiation by two FourCC codes.
 * @param 	"const TFourCC& aSrcFourCC"
 * 			the FourCC code of source media type
 * @param 	"const TFourCC& aDstFourCC"
 * 			the FourCC code of destination media type
 * @return	"TVerdict"
 *			EPass: ProcessOnceForNewL() passes
 *			EFail: ProcessOnceForNewL() fails
 * @leave if CMMFCodec::NewL() or ProcessOnceForNewL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestNewL(const TFourCC& aSrcFourCC, const TFourCC& aDstFourCC)
	{
	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(aSrcFourCC, aDstFourCC) );
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_SbcCodec::TestNewL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	if (!codec)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestNewL NewL returned NULL") );
		return EFail;
		}

	CleanupStack::PushL(codec);
	TVerdict result = ProcessOnceForNewL(codec);
	CleanupStack::PopAndDestroy(); //codec
	return result;
	}
	
/**
 * Performance test
 * This function tests ProcessL() function, it checks if ProcessL() returns the 
 * correct result with different buffer size settings
 * @return	"TVerdict"
 * 			EPass: the results are expected ones. 
 * 			EFail: otherwise. 
 * @leave if out of memory or ProcessL(), ReadSourceL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestProcessL()
	{
	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(KSbcCodecPluginUid) );
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_SbcCodec::TestProcessL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	if (!codec)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessL NewL returned NULL") );
		return EFail;
		}

	CleanupStack::PushL(codec);

	const TInt testNumber = 28;
	
	TSBCFrameParameters param = SbcParametersL(testNumber);
	
	const TUint sbcFrameLength = param.CalcFrameLength();
	const TUint pcmFrameSize = sizeof(TInt16) * param.BlockLength() * param.Channels() * param.Subbands();
	
	// create src buffer
	CMMFDataBuffer* srcBuffer = CMMFDataBuffer::NewL(pcmFrameSize);
	CleanupStack::PushL(srcBuffer);
	
	// dummy buffer, smaller than one frame length
	CMMFDataBuffer* dummyBuffer = CMMFDataBuffer::NewL(sbcFrameLength - 1);
	CleanupStack::PushL(dummyBuffer);
	
	// create dst buffer
	CMMFDataBuffer* dstBuffer = CMMFDataBuffer::NewL(sbcFrameLength); 
	CleanupStack::PushL(dstBuffer);
	
	ReadSourceL(GetTestFileName(testNumber), srcBuffer->Data(), 0);
	
	if (static_cast<TUint>(srcBuffer->Data().Length() ) != pcmFrameSize)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessL read source failed") );
		CleanupStack::PopAndDestroy(3); // dstBuffer, srcBuffer, codec
		return EFail;
		}

	TVerdict testResult = EPass;
	TCodecProcessResult procResult;
	
	/** test ProcessL() without ConfigureL() called, should leave with KErrAbort */
	
	TRAP(err, procResult = codec->ProcessL(*srcBuffer, *dstBuffer) );
	
	if (err != KErrAbort || 
		procResult.iSrcBytesProcessed != 0 || 
		procResult.iDstBytesAdded != 0 || 
		procResult.iStatus != TCodecProcessResult::EProcessError)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessL failed, ProcessL didn't leave with KErrAbort when ConfigureL() is not called") );
		testResult = EFail;
		}
	
	/** test ProcessL() with small dst buffer size, should leave with KErrArgument */
	
	TPckgBuf<TSBCFrameParameters> config(param);
	codec->ConfigureL(KSbcCodecConfigUid, config);
	
	codec->ResetL();
	TRAP(err, procResult = codec->ProcessL(*srcBuffer, *dummyBuffer) );
	
	if (err != KErrArgument || 
		procResult.iSrcBytesProcessed != 0 || 
		procResult.iDstBytesAdded != 0 || 
		procResult.iStatus != TCodecProcessResult::EProcessError)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessL failed in ProcessL") );
		testResult = EFail;
		}
		
	/** test ProcessL() with correct settings, should return EProcessComplete */
	
	codec->ResetL();
	TRAP(err, procResult = codec->ProcessL(*srcBuffer, *dstBuffer) );
	
	if (err != KErrNone || 
		procResult.iSrcBytesProcessed != pcmFrameSize || 
		procResult.iDstBytesAdded != sbcFrameLength || 
		procResult.iStatus != TCodecProcessResult::EProcessComplete)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessL failed, ProcessL didn't work propertly with correct settings") );
		testResult = EFail;
		}
	
	/** test ProcessL() with not enough dst buffer remained, should return EProcessIncomplete */

	// now the remaining dst buffer size is one byte less than one frame length
	dstBuffer->SetPosition(1); 
	
	codec->ResetL();
	TRAP(err, procResult = codec->ProcessL(*srcBuffer, *dstBuffer) );
	
	if (err != KErrNone || 
		procResult.iSrcBytesProcessed != 0 || 
		procResult.iDstBytesAdded != 0 || 
		procResult.iStatus != TCodecProcessResult::EProcessIncomplete)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessL failed, ProcessL didn't return EProcessIncomplete when there is not enough dst buffer") );
		testResult = EFail;
		}
	
	/** test ProcessL() with not enough src buffer remained, should return EDstNotFilled */

	// set this position to an even value as only even number of samples will be cached
	const TInt srcPos = 2;
	// the remaining src buffer size is less than one frame, it should be cached, 
	// and return EProcessDstNotFilled 
	srcBuffer->SetPosition(srcPos); 
	// now the remaining dst buffer size is one frame length
	dstBuffer->SetPosition(0);
	
	codec->ResetL();
	TRAP(err, procResult = codec->ProcessL(*srcBuffer, *dstBuffer) );
	
	if (err != KErrNone || 
		procResult.iSrcBytesProcessed != pcmFrameSize - srcPos || 
		procResult.iDstBytesAdded != 0 || 
		procResult.iStatus != TCodecProcessResult::EDstNotFilled)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessL failed, ProcessL didn't return EDstNotFilled when there is not enough src buffer") );
		testResult = EFail;
		}
		
	CleanupStack::PopAndDestroy(4); // dstBuffer, dummyBuffer, srcBuffer, codec
	return testResult;
	}

/**
 * This function tests the whole encoding routine with a specific settings (e.g. 
 * sbc frameparameters, src buffer size, dst buffer size) and compares the result 
 * file with reference file.
 * @param	"TInt aTestNumber"
 * 			the test number.
 * @param	"TInt aSrcBufSize"
 * 			the source buffer size.
 * @param	"TInt aDstBufSize"
 * 			the destination buffer size.
 * @return	"TVerdict"
 * 			EPass: encoded file is the same as the reference file. 
 * 			EFail: encoded file is NOT the same as the reference file. 
 * @leave if out of memory or ProcessL(), ReadSourceL(), WriteDstToFileL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestEncodeL(TInt aTestNumber, TInt aSrcBufSize, TInt aDstBufSize)
	{
	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(KSbcCodecPluginUid) );
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_SbcCodec::TestEncodeL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	if (!codec)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestEncodeL NewL returned NULL") );
		return EFail;
		}

	CleanupStack::PushL(codec);

	// create buffers
	CMMFDataBuffer* srcBuffer = CMMFDataBuffer::NewL(aSrcBufSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer* dstBuffer = CMMFDataBuffer::NewL(aDstBufSize);
	CleanupStack::PushL(dstBuffer);
	
	TPckgBuf<TSBCFrameParameters> config(SbcParametersL(aTestNumber) );
	codec->ConfigureL(KSbcCodecConfigUid, config);
	
	TInt srcFilePos = 0;
	TInt dstFilePos = 0;
	
	for (;;)
		{
		ReadSourceL(GetTestFileName(aTestNumber), srcBuffer->Data(), srcFilePos);
		
		if (srcBuffer->Data().Length() == 0) // end of file
			{
			break;
			}
		
		TCodecProcessResult result = codec->ProcessL(*srcBuffer, *dstBuffer);

		WriteDstToFileL(GetRefFileName(aTestNumber), dstBuffer->Data(), dstFilePos);
		
		srcBuffer->Data().Zero();
		dstBuffer->Data().Zero();
		
		srcFilePos += result.iSrcBytesProcessed;
		dstFilePos += result.iDstBytesAdded;
		}
	
	TVerdict result = EPass;
	if (CompareFileL(GetRefFileName(aTestNumber) ) != 0)
		{
		result = EFail;
		}
		
	CleanupStack::PopAndDestroy(3); // dstBuffer, srcBuffer, codec
	return result;
	}

/**
 * This function tests codec ResetL() function make sure the cach buffer 
 * gets cleared when it gets called. It compares the encoded file with the reference file
 * @return	"TVerdict"
 * 			EPass: encoded file is the same as the reference file. 
 * 			EFail: encoded file is NOT the same as the reference file. 
 * @leave if out of memory or ProcessL(), ReadSourceL(), WriteDstToFileL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestRepositionL()
	{
	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(KSbcCodecPluginUid) );
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_SbcCodec::TestRepositionL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	if (!codec)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestRepositionL NewL returned NULL") );
		return EFail;
		}

	CleanupStack::PushL(codec);

	const TInt testNumber = 28;
	// this size will make sure some samples will be cached, 
	// also make sure it is an EVEN number
	const TInt srcBufSize = 4094; 
	const TInt dstBufSize = 2048;
	
	// create buffers
	CMMFDataBuffer* srcBuffer = CMMFDataBuffer::NewL(srcBufSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer* dstBuffer = CMMFDataBuffer::NewL(dstBufSize);
	CleanupStack::PushL(dstBuffer);
	
	TSBCFrameParameters param = SbcParametersL(testNumber);
	
	TPckgBuf<TSBCFrameParameters> config(param);
	codec->ConfigureL(KSbcCodecConfigUid, config);
	
	const TUint pcmFrameSize = sizeof(TInt16) * param.BlockLength() * param.Channels() * param.Subbands();
	
	TInt srcFilePos = 0;
	TInt dstFilePos = 0;
	
	for (;;)
		{
		ReadSourceL(GetTestFileName(testNumber), srcBuffer->Data(), srcFilePos);
		
		if (srcBuffer->Data().Length() == 0) // end of file
			{
			break;
			}
			
		TCodecProcessResult result = codec->ProcessL(*srcBuffer, *dstBuffer);
		
		WriteDstToFileL(GetRefFileName(testNumber), dstBuffer->Data(), dstFilePos);
		
		// clear up any cached samples
		codec->ResetL();
		// the iSrcBytesProcessed includes the encoded src size plus the cached src size, 
		// here set the iSrcBytesProcessed to the encoded src size only as we cleared up 
		// the cached samples
		result.iSrcBytesProcessed -= srcBuffer->Data().Length() % pcmFrameSize;
		
		srcBuffer->Data().Zero();
		dstBuffer->Data().Zero();
		
		// set new src file position, which is the end of encoded src (no cached src)
		srcFilePos += result.iSrcBytesProcessed;
		// set new dst file position
		dstFilePos += result.iDstBytesAdded;
		}

	TVerdict result = EPass;
	if (CompareFileL(GetRefFileName(testNumber) ) != 0)
		{
		result = EFail;
		}
		
	CleanupStack::PopAndDestroy(3); // dstBuffer, srcBuffer, codec
	return result;
	}

/**
 * This test tests changing configuration for the codec. It calls ConfigureL() once to 
 * encode one sbc bitstream, then changes a different configuration by calling ConfigureL() 
 * and encode another sbc bitstream.
 * @return	"TVerdict"
 *			EPass: if encoded sbc files are the same as the reference files.
 *			EFail: if not.
 * @leave if ReadSourceL(), ProcessL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestChangeConfigL()
	{
	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(KSbcCodecPluginUid) );
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_SbcCodec::TestChangeConfigL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	if (!codec)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestChangeConfigL NewL returned NULL") );
		return EFail;
		}

	CleanupStack::PushL(codec);

	const TInt testNumber[2] = {1, 28};
	// this size will make sure some samples will be cached, 
	// also make sure it is an EVEN number
	const TInt srcBufSize = 4094; 
	const TInt dstBufSize = 2048;
	
	// create buffers
	CMMFDataBuffer* srcBuffer = CMMFDataBuffer::NewL(srcBufSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer* dstBuffer = CMMFDataBuffer::NewL(dstBufSize);
	CleanupStack::PushL(dstBuffer);
	
	TVerdict result = EPass;
	for (TInt test = 0; test < 2; test++)
		{
		TSBCFrameParameters param = SbcParametersL(testNumber[test]);
		
		TPckgBuf<TSBCFrameParameters> config(param);
		codec->ConfigureL(KSbcCodecConfigUid, config);
		codec->ResetL();
		
		TInt srcFilePos = 0;
		TInt dstFilePos = 0;
		
		for (;;)
			{
			ReadSourceL(GetTestFileName(testNumber[test]), srcBuffer->Data(), srcFilePos);
			
			if (srcBuffer->Data().Length() == 0) // end of file
				{
				break;
				}
				
			TCodecProcessResult result = codec->ProcessL(*srcBuffer, *dstBuffer);
			
			WriteDstToFileL(GetRefFileName(testNumber[test]), dstBuffer->Data(), dstFilePos);
			
			srcBuffer->Data().Zero();
			dstBuffer->Data().Zero();
			
			srcFilePos += result.iSrcBytesProcessed;
			dstFilePos += result.iDstBytesAdded;
			}

		if (CompareFileL(GetRefFileName(testNumber[test]) ) != 0)
			{
			result = EFail;
			}
		}
		
	CleanupStack::PopAndDestroy(3); // dstBuffer, srcBuffer, codec
	return result;
	}
	
/**
 * This tests memory scribble for ProcessL(). It creates a buffer size of frame_length + 2, 
 * the first byte and last byte are used to detect memory scribble,
 * the middle frame_length bytes are used for destination buffer. It calls ProcessL() once.
 * and checks if the first or last byte value changed to check memory scribble.
 * @return	"TVerdict"
 *			EPass: if there is no momery scribble.
 *			EFail: if there is momery scribble.
 * @leave if ReadSourceL(), ProcessL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestMemoryScribbleL()
	{
	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(KSbcCodecPluginUid) );
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_SbcCodec::TestMemoryScribbleL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	if (!codec)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestMemoryScribbleL NewL returned NULL") );
		return EFail;
		}

	CleanupStack::PushL(codec);

	const TInt testNumber = 28;
	
	TSBCFrameParameters param = SbcParametersL(testNumber);
	
	TPckgBuf<TSBCFrameParameters> config(param);
	codec->ConfigureL(KSbcCodecConfigUid, config);
	
	const TUint sbcFrameLength = param.CalcFrameLength();
	const TUint pcmFrameSize = sizeof(TInt16) * param.BlockLength() * param.Channels() * param.Subbands();
	
	// create buffers
	CMMFDataBuffer* srcBuffer = CMMFDataBuffer::NewL(pcmFrameSize);
	CleanupStack::PushL(srcBuffer);
	
	// front and end bytes are used to detect memory scribble
	CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(sbcFrameLength + 2); 
	CleanupStack::PushL(buffer);
	
	TUint8* firstByte = const_cast<TUint8*>(buffer->Data().Ptr() );
	TUint8* lastByte = firstByte + sbcFrameLength + 1;
	
	const TUint8 initValue = 0xaa; // 10101010
	
	*firstByte = initValue;
	*lastByte = initValue;
	
	CMMFPtrBuffer* dstBuffer = CMMFPtrBuffer::NewL(TPtr8(firstByte + 1, sbcFrameLength) );
	CleanupStack::PushL(dstBuffer);
	
	ReadSourceL(GetTestFileName(testNumber), srcBuffer->Data(), 0);
	
	TVerdict testResult = EPass;
	if (static_cast<TUint>(srcBuffer->Data().Length() ) == pcmFrameSize)
		{
		TCodecProcessResult result = codec->ProcessL(*srcBuffer, *dstBuffer);
		
		if (result.iSrcBytesProcessed != pcmFrameSize ||
			result.iDstBytesAdded != sbcFrameLength ||
			result.iStatus != TCodecProcessResult::EProcessComplete)
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestMemoryScribbleL failed in ProcessL") );
			testResult = EFail;
			}
			
		if (*firstByte != initValue || *lastByte != initValue ) // memory been scribbled
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestMemoryScribbleL memory scribble occured") );
			testResult = EFail;
			}
		}
	else
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestMemoryScribbleL read source failed") );
		testResult = EFail;
		}
		
	CleanupStack::PopAndDestroy(4); // dstBuffer, buffer, srcBuffer, codec
	return testResult;
	}

/**
 * This tests all the codec APIs (here we are using TestRepositionL() test as it 
 * uses all the APIs) and checks if there is any memory leak, it panics if so.
 * @return	"TVerdict"
 *			EPass: if there is no momery leak.
 *			EFail: if there is momery leak.
 * @leave if TestRepositionL() leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestMemoryLeakL()
	{
	__MM_HEAP_MARK;
	
	TestRepositionL();
	
	__MM_HEAP_MARKEND;
	
	return EPass;
	}

/**
 * This function tests how NewL deals with a memory allocation failure. This test 
 * will pass if NewL leaves with the correct error code on every memory allocation.
 *
 * @return	"TVerdict"
 *			EPass: if the test passes.
 *			EFail: if the test fails.
 */
TVerdict CTestStep_MMF_SbcCodec::TestNewLOutOfMemoryL()
	{
	TVerdict testResult = EPass;
	TInt failCount = 1;
	
	for (;;)
		{
		__MM_HEAP_MARK;
		__UHEAP_FAILNEXT(failCount);

		CMMFCodec* codec = NULL;
		TRAPD(err, codec = CMMFCodec::NewL(KSbcCodecPluginUid) );
		
		// it should return either KErrNone or KErrNoMemory, 
		// ideally should only return KErrNone when codec is created.
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1);
			
			delete codec;
			if (testAlloc == NULL)
				{
				break;
				}
			User::Free(testAlloc);
			}
		else if (err != KErrNoMemory)
			{
			delete codec;
			testResult = EFail;
			break;
			}
			
		failCount++;
		
		__UHEAP_RESET;
		__MM_HEAP_MARKEND;
		} // while(1)
	
	INFO_PRINTF2(_L("NewL out of memory test finished at round %d"), failCount);

	return testResult;
	}

/**
 * This function tests how the ProcessL() would deal with a memory allocation failure.
 * @return	"TVerdict"
 *			EPass: if the test passes.
 *			EFail: if the test fails.
 * @leave if out of memory or ConfigureL(), ReadSourceL leaves
 */
TVerdict CTestStep_MMF_SbcCodec::TestProcessLOutOfMemoryL()
	{
	__MM_HEAP_MARK;
	
	CMMFCodec* codec = CMMFCodec::NewL(KSbcCodecPluginUid);
	if (!codec)
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessLOutOfMemoryL returned NULL") );
		return EFail;
		}

	CleanupStack::PushL(codec);

	const TInt testNumber = 28;
	// this size will make sure some samples will be cached,
	// also make sure it is an EVEN number
	const TInt srcBufSize = 4094; 
	const TInt dstBufSize = 2048;
	
	TPckgBuf<TSBCFrameParameters> config(SbcParametersL(testNumber) );
	codec->ConfigureL(KSbcCodecConfigUid, config);

	// create buffers
	CMMFDataBuffer* srcBuffer = CMMFDataBuffer::NewL(srcBufSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer* dstBuffer = CMMFDataBuffer::NewL(dstBufSize);
	CleanupStack::PushL(dstBuffer);

	ReadSourceL(GetTestFileName(testNumber), srcBuffer->Data(), 0);
	if (srcBuffer->Data().Length() == 0) // end of file
		{
		ERR_PRINTF1(_L(">> CTestStep_MMF_SbcCodec::TestProcessLOutOfMemoryL no source read") );
		return EFail;
		}
		
	TVerdict testResult = EPass;
	TInt failCount = 1;
	
	for (;;)
		{
		__UHEAP_FAILNEXT(failCount);

		TCodecProcessResult dummyResult; // don't care about this result
		TRAPD(err, dummyResult = codec->ProcessL(*srcBuffer, *dstBuffer) );
		
		// it should return either KErrNone or KErrNoMemory, 
		// ideally should only return KErrNone when codec is created.
		if (err == KErrNone)
			{
			// attempt to allocate another cell. If this fails, 
			// we have tested all of the memory allocs carried out by NewL.
			TAny *testAlloc = User::Alloc(1);
			
			if (testAlloc == NULL)
				{
				break;
				}
			User::Free(testAlloc);
			}
		else if (err != KErrNoMemory)
			{
			testResult = EFail;
			break;
			}
			
		failCount++;
		
		__UHEAP_RESET;
		} // while(1)
	
	INFO_PRINTF2(_L("ProcessL out of memory test finished at round %d"), failCount);
	
	CleanupStack::PopAndDestroy(3); // dstBuffer, srcBuffer, codec

	__MM_HEAP_MARKEND;
		
	return testResult;
	}

//------------------------------------------------------------------
//			** TEST MMF_SBCCODEC STEPS ***
//------------------------------------------------------------------

/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_001::CTest_MMF_SbcCodec_U_001()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-001-HP");
	}

/**
 * Instantiate a codec with plugin UID
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_001::DoTestStepL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = TestNewL(KSbcCodecPluginUid) );

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		}
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_002::CTest_MMF_SbcCodec_U_002()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-002-HP");
	}

/**
 * Instantiate a codec with FourCC Codes
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_002::DoTestStepL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodeSBC) );

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		}
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_010::CTest_MMF_SbcCodec_U_010()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-010-HP");
	}

/**
 * ProcessL() test
 * This test tests the ProcessL() function with different buffer size settings, and
 * check the returned results
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_010::DoTestStepL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = TestProcessL() );

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		}
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_011::CTest_MMF_SbcCodec_U_011()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-011-HP");
	}

/**
 * Configuration tests
 * This test tests if codec supports 8 different settings (see TSS & TP for A2DP spec)
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_011::DoTestStepL()
	{
	TVerdict testResult = EPass;
	TInt srcBufSize = KDefaultBufferSize; 
	TInt dstBufSize = KDefaultBufferSize >> 1;

	for (TInt testNumber = 1; testNumber <= 8; testNumber++)
		{
		TVerdict result = EFail;
		
		TRAPD(err, result = TestEncodeL(testNumber, srcBufSize, dstBufSize) );

		if (err)
			{
			INFO_PRINTF3(_L("Configuration test %d left with status %d"), testNumber, err);
			testResult = EFail;
			}
		if (result == EFail)
			{
			INFO_PRINTF2(_L("Configuration test %d failed"), testNumber);
			testResult = EFail;
			}
			
		// This is simulating different src buffer size by decreasing the 
		// src buffer size number each time, it covers sample caching and 
		// no caching various cases to make sure the codec can handle different
		// src buffer size correctly
		srcBufSize--;
		}
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_012::CTest_MMF_SbcCodec_U_012()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-012-HP");
	}

/**
 * Maximum Bit Rate tests
 * This test tests sbc coded handles 8 different maximum bit rate settings (see TSS & TP for A2DP spec)
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_012::DoTestStepL()
	{
	TVerdict testResult = EPass;
	TInt srcBufSize = KDefaultBufferSize;
	TInt dstBufSize = KDefaultBufferSize >> 1;
	
	for (TInt testNumber = 11; testNumber <= 18; testNumber++)
		{
		TVerdict result = EFail;
		TRAPD(err, result = TestEncodeL(testNumber, srcBufSize, dstBufSize) );

		if (err)
			{
			INFO_PRINTF3(_L("Max Bit Rate test %d left with status %d"), testNumber, err);
			testResult = EFail;
			}
		if (result == EFail)
			{
			INFO_PRINTF2(_L("Max Bit Rate test %d failed"), testNumber);
			testResult = EFail;
			}

		// This is simulating different src buffer size by decreasing the 
		// src buffer size number each time, it covers sample caching and 
		// no caching various cases to make sure the codec can handle different
		// src buffer size correctly
		srcBufSize--;
		}
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_013::CTest_MMF_SbcCodec_U_013()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-013-HP");
	}

/**
 * Special tests
 * This test tests sbc codec handles 2 special cases (see TSS & TP for A2DP spec)
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_013::DoTestStepL()
	{
	TVerdict testResult = EPass;
	TInt srcBufSize = KDefaultBufferSize;
	TInt dstBufSize = KDefaultBufferSize >> 1;

	for (TInt testNumber = 19; testNumber <= 20; testNumber++)
		{
		TVerdict result = EFail;
		TRAPD(err, result = TestEncodeL(testNumber, srcBufSize, dstBufSize) );

		if (err)
			{
			INFO_PRINTF3(_L("Special test %d left with status %d"), testNumber, err);
			testResult = EFail;
			}
		if (result == EFail)
			{
			INFO_PRINTF2(_L("Special test %d failed"), testNumber);
			testResult = EFail;
			}

		// This is simulating different src buffer size by decreasing the 
		// src buffer size number each time, it covers sample caching and 
		// no caching various cases to make sure the codec can handle different
		// src buffer size correctly
		srcBufSize--;
		}
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_014::CTest_MMF_SbcCodec_U_014()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-014-HP");
	}

/**
 * Typical tests
 * This test tests sbc codec handles 8 typical settings (see TSS & TP for A2DP spec) which 
 * are required in REQ2716, our sbc encoder has to support these settings.
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_014::DoTestStepL()
	{
	TVerdict testResult = EPass;
	TInt srcBufSize = KDefaultBufferSize;
	TInt dstBufSize = KDefaultBufferSize >> 1;

	for (TInt testNumber = 21; testNumber <= 28; testNumber++)
		{
		TVerdict result = EFail;
		TRAPD(err, result = TestEncodeL(testNumber, srcBufSize, dstBufSize) );

		if (err)
			{
			INFO_PRINTF3(_L("Typical test %d left with status %d"), testNumber, err);
			testResult = EFail;
			}
		if (result == EFail)
			{
			INFO_PRINTF2(_L("Typical test %d failed"), testNumber);
			testResult = EFail;
			}

		// This is simulating different src buffer size by decreasing the 
		// src buffer size number each time, it covers sample caching and 
		// no caching various cases to make sure the codec can handle different
		// src buffer size correctly
		srcBufSize--;
		}
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_015::CTest_MMF_SbcCodec_U_015()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-015-HP");
	}

/**
 * Reposition tests
 * This test tests ResetL() function and audio sample cach issue
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_015::DoTestStepL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = TestRepositionL() );

	if (err)
		{
		INFO_PRINTF2(_L("Reposition test left with status %d"), err);
		testResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Reposition test passes") );
		}

	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_016::CTest_MMF_SbcCodec_U_016()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-016-HP");
	}

/**
 * Change configuration test
 * This test tests reconfigure sbc codec by calling ConfigureL() function
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_016::DoTestStepL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = TestChangeConfigL() );

	if (err)
		{
		INFO_PRINTF2(_L("Change conguration test left with status %d"), err);
		testResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Change conguration test passes") );
		}

	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_021::CTest_MMF_SbcCodec_U_021()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-021-HP");
	}

/**
 * Memory Scribble test
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_021::DoTestStepL()
	{
	TVerdict testResult = EPass;
	
	TRAPD(err, testResult = TestMemoryScribbleL() );

	if (err)
		{
		INFO_PRINTF2(_L("Memory scribble test left with status %d"), err);
		testResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Memory scribble test passes") );
		}

	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_022::CTest_MMF_SbcCodec_U_022()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-022-HP");
	}

/**
 * Memory Leak test
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_022::DoTestStepL()
	{
	TVerdict testResult = EPass;

	TRAPD(err, testResult = TestMemoryLeakL() );

	if (err)
		{
		INFO_PRINTF2(_L("Memory leak test left with status %d"), err);
		testResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Memory leak test passes") );
		}
	
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_023::CTest_MMF_SbcCodec_U_023()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-023-HP");
	}

/**
 * Out of Memory test for NewL()
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_023::DoTestStepL()
	{
	TVerdict testResult = EPass;
	
	TRAPD(err, testResult = TestNewLOutOfMemoryL() );

	if (err)
		{
		INFO_PRINTF2(_L("NewL out of memory test left with status %d"), err);
		testResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("NewL out of memory test passes") );
		}
	
	return testResult; 
	}
	
/**
 * Constructor
 */
CTest_MMF_SbcCodec_U_024::CTest_MMF_SbcCodec_U_024()
	{
	iTestStepName = _L("MM-MMF-SbcCodec-U-024-HP");
	}

/**
 * Out of Memory test for ProcessL()
 * @test Req. under test REQ2716
 */
TVerdict CTest_MMF_SbcCodec_U_024::DoTestStepL()
	{
	TVerdict testResult = EPass;
	
	TRAPD(err, testResult = TestProcessLOutOfMemoryL() );

	if (err)
		{
		INFO_PRINTF2(_L("ProcessL out of memory test left with status %d"), err);
		testResult = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("ProcessL out of memory test passes") );
		}
		
	return testResult; 
	}
