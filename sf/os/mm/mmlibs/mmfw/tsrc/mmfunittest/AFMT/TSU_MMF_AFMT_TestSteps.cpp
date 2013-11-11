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
// TSU_MMInitialiseFileTestsLF_AFMT_TestSteps.CPP
// EPOC includes
// 
//

#include <e32base.h>


// Includes for 
#include <mmf/server/mmffile.h>  // for TMMFFileParams and CMMFFile
#include <mmf/server/mmfdatasourcesink.hrh>
#include <mmf/server/mmfdatasource.h>

#include <mmf/server/mmfformat.h>

#include <mmf/plugin/mmfformatimplementationuids.hrh> // for KMmfUidFormatWAVRead UIDs

// Dummy Sink UIDs/Class declaration files
#include "TSU_MMF_AFMT_TestDataSinkUIDs.hrh"
#include "TSU_MMF_AFMT_TestDataSourceUIDs.hrh"
#include "TSU_MMF_AFMT_testdatasink.h"
#include "TSU_MMF_AFMT_TestDataSource.h"
#include <mmf/server/mmfcodec.h>

// Test system includes
#include "TSU_MMF_AFMT.h"
#include "TSU_MMF_AFMTSuite.h"
#include "TSU_MMF_AFMT_TestSteps.h"

// Some standard defines
const TUint KMono = 1;
const TUint KStereo = 2;

const TUint KGsmTestFileDuration = 5600000; //5.6 secs in us.

// from formatUtils.h
const TUint KMMFFormatMinFrameSize = 0x800; //2K
const TUint KMMFFormatMaxFrameSize = 0x4000; //16K

// Setting these to -1 does as many as possible.
// Otherwise it will do however many runs are specified.
const TInt KNumberOfFillBufferLTests = 5;
//const TInt KNumberOfEmptyBufferLTests = -1; // EABI warning removal


//------------------------------------------------------------------
//			** TEST MMF_AFMT STEPS ***

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0001::CTest_MMF_AFMT_U_0001()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0001-HP");
	}

/** @xxxx
 * Create a new Wav Format object and check it is set up correctly
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0001::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KMono, 22050, TFourCC(' ', 'P', '1', '6') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0001::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLUid, KWavPcm16BitMono22Khz, KMmfUidFormatWAVRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0001::Close()
	{
	// Cleanup test data 
	Cleanup();
	}



//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0002::CTest_MMF_AFMT_U_0002()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0002-HP");
	}

/** @xxxx
 * Create a new Wav Format object and check that it is set up correctly
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0002::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KMono, 8000, TFourCC(' ', 'P', '1', '6') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0002::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLFilename, KWavPcm16BitMono8Khz, KMmfUidFormatWAVRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0002::Close()
	{
	// Cleanup test data 
	Cleanup();
	}
//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0003::CTest_MMF_AFMT_U_0003()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0003-HP");
	}

/** @xxxx
 * Create a new Wav Format object and check that it is set up correctly
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0003::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KMono, 11025, TFourCC(' ', 'P', 'U', '8') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0003::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseDescriptorTestsL(KNewLDescriptor, KWavPcm8BitMono11Khz, KMmfUidFormatWAVRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0003::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0004::CTest_MMF_AFMT_U_0004()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0004-HP");
	}

/** @xxxx
 * Create a new Wav Format object and check that it is set up correctly
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0004::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KStereo, 8000, TFourCC(' ', 'P', '1', '6') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0004::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLClip, KWavPcm16BitStereo8Khz, KMmfUidFormatWAVRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0004::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0016::CTest_MMF_AFMT_U_0016()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0016-HP");
	}

/** @xxxx
 * Tests the FrameTimeInterval of PCM16 22k mono file.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0016::DoTestStepL()
	{	
	return RunTest(EAudioPCM, 22050, 16, 1); // PCM Decode, 22Khz, 16-bit, Mono
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0016::OpenL()
	{	
	return SetupL(KWavPcm16BitMono22Khz, KMmfUidFormatWAVRead);	
	}

// Postamble
void CTest_MMF_AFMT_U_0016::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0018::CTest_MMF_AFMT_U_0018()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0018-HP");
	}

/** @xxxx
 * Tests the FrameTimeInterval of ADPCM 8k mono file.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0018::DoTestStepL()
	{	
	return RunTest(EAudioADPCM, 8000, 4, 1); //ADPCM Decode, 8Khz, Mono
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0018::OpenL()
	{	
	return SetupL(KWavAdpcm16BitMono8Khz, KMmfUidFormatWAVRead);	
	}

// Postamble
void CTest_MMF_AFMT_U_0018::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0020::CTest_MMF_AFMT_U_0020()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0020-HP");
	}

/** @xxxx
 * Tests the FrameTimeInterval of GSM610 file.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0020::DoTestStepL()
	{	
	return RunTest(EAudioGSM, ETrue); // Decode, 22Khz, 16-bit, Mono
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0020::OpenL()
	{	
	return SetupL(KWavGsmMono8Khz, KMmfUidFormatWAVRead);	
	}

// Postamble
void CTest_MMF_AFMT_U_0020::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0025::CTest_MMF_AFMT_U_0025()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0025-HP");
	}

/** @xxxx
 * Tests the Duration call with a PCM16 file
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0025::DoTestStepL()
	{	
	return RunDurationTest( TInt64(1000000) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0025::OpenL()
	{	
	return SetupL(KWavPcm16BitMono22Khz, KMmfUidFormatWAVRead);		
	}

// Postamble
void CTest_MMF_AFMT_U_0025::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0027::CTest_MMF_AFMT_U_0027()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0027-HP");
	}

/** @xxxx
 * Tests the Duration call with a ADPCM file
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0027::DoTestStepL()
	{	
	return RunDurationTest( TInt64(5600000)); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0027::OpenL()
	{	
	return SetupL(KWavAdpcm16BitMono8Khz, KMmfUidFormatWAVRead);		
	}

// Postamble
void CTest_MMF_AFMT_U_0027::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0028::CTest_MMF_AFMT_U_0028()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0028-HP");
	}

/** @xxxx
 * Tests the Duration call with a IMA ADPCM file
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0028::DoTestStepL()
	{	
	return RunDurationTest(TInt64(280601360) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0028::OpenL()
	{	
	_LIT(KImaAdpcm4BitMono11Khz, "c:\\mm\\mmf\\testfiles\\afmt\\pcm4mono11khz.wav");
	return SetupL(KImaAdpcm4BitMono11Khz, KMmfUidFormatWAVRead);		
	}

// Postamble
void CTest_MMF_AFMT_U_0028::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0029::CTest_MMF_AFMT_U_0029()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0029-HP");
	}

/** @xxxx
 * Tests the Duration call with a GSM610 file
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0029::DoTestStepL()
	{	
	return RunDurationTest( TInt64(KGsmTestFileDuration) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0029::OpenL()
	{		
	return SetupL(KWavGsmMono8Khz, KMmfUidFormatWAVRead);	
	}

// Postamble
void CTest_MMF_AFMT_U_0029::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0030::CTest_MMF_AFMT_U_0030()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0030-HP");
	}

/** @xxxx
 * Tests the CreateSourceBufferL call with a CMMFWavFormat Decode object
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0030::DoTestStepL()
	{	
	TVerdict verdict;
	TBool myBool = ETrue;
	CMMFBuffer* retBuffer = iFormatDec->CreateSourceBufferL(KUidMediaTypeAudio, 
															myBool);

	TUint bufferSize = retBuffer->BufferSize();
	if (bufferSize >= KMMFFormatMinFrameSize && bufferSize <= KMMFFormatMaxFrameSize)
		verdict = EPass;
	else 
		verdict = EFail;
		
	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0030::OpenL()
	{		
	SetupL(KWavPcm16BitStereo8Khz, KMmfUidFormatWAVRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0030::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0031::CTest_MMF_AFMT_U_0031()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0031-HP");
	}

/** @xxxx
 * Tests the CanCreateSourceBufferL call with a CMMFWavFormat Decode object
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0031::DoTestStepL()
	{	
	TVerdict verdict;

	// Check that the DECODE object CAN create a SOURCE buffer...
	if (iFormatDec && iFormatDec->CanCreateSourceBuffer())
		verdict = EPass;
	else
		verdict = EFail;

	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0031::OpenL()
	{		
	SetupL(KWavPcm16BitStereo8Khz, KMmfUidFormatWAVRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0031::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0032::CTest_MMF_AFMT_U_0032()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0032-HP");
	}

/** @xxxx
 * Tests the Number of Channels returned is correct
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0032::DoTestStepL()
	{	
	return TestNumChannelsL(KWavPcm8BitMono11Khz, KWavPcm16BitStereo8Khz);
	}

TVerdict CTest_MMF_AFMT_U_0032::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatWAVRead);
	return EPass;
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0033::CTest_MMF_AFMT_U_0033()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0033-HP");
	}

/** @xxxx
 * Tests the Samplerate returned is correct
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0033::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	if (TestSampleRateL(8000, KWavPcm16BitMono8Khz) &&	//8khz test
		TestSampleRateL(11025,KWavPcm8BitMono11Khz) &&	//11khz test
		TestSampleRateL(22050,KWavPcm16BitMono22Khz) &&	//22khz test
		TestSampleRateL(44100,KWavPcm8BitMono44khz) )	//44khz test
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0033::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatWAVRead);
	return EPass;
	}

void CTest_MMF_AFMT_U_0033::Close()
	{
	}
	

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0036::CTest_MMF_AFMT_U_0036()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0036-HP");
	}

/** @xxxx
 * This tests the call to CMMFWavFormat's FillbufferL call with PCM16 data.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0036::DoTestStepL()
	{
	return RunFillBufferTestL(KNumberOfFillBufferLTests, KWavPcm16BitStereo8KhzCompare);		
	}

TVerdict CTest_MMF_AFMT_U_0036::OpenL()
	{
	return SetupL(KWavPcm16BitStereo8Khz, KMmfUidFormatWAVRead);
	}

void CTest_MMF_AFMT_U_0036::Close()
	{
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0037::CTest_MMF_AFMT_U_0037()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0037-HP");
	}

/** @xxxx
 * This tests the call to CMMFWavFormat's FillbufferL call with ADPCM data.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0037::DoTestStepL()
	{	
	return RunFillBufferTestL(KNumberOfFillBufferLTests, KWavAdpcm16BitMono8KhzCompare);		
	}

TVerdict CTest_MMF_AFMT_U_0037::OpenL()
	{
	return SetupL(KWavAdpcm16BitMono8Khz, KMmfUidFormatWAVRead);	
	}

void CTest_MMF_AFMT_U_0037::Close()
	{
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0038::CTest_MMF_AFMT_U_0038()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0038-HP");
	}

/** @xxxx
 * This tests the call to CMMFWavFormat's FillbufferL call with GSM610 data.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0038::DoTestStepL()
	{
	return RunFillBufferTestL(KNumberOfFillBufferLTests, KWavGsmMono8KhzCompare);		
	}

TVerdict CTest_MMF_AFMT_U_0038::OpenL()
	{
	return SetupL(KWavGsmMono8Khz, KMmfUidFormatWAVRead);
	}

void CTest_MMF_AFMT_U_0038::Close()
	{
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0042::CTest_MMF_AFMT_U_0042()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0042-HP");
	}

/** @xxxx
 * This tests the call to create a new CMmfWavFormatEncode object.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0042::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KMono, 22050, TFourCC(' ', 'P', '1', '6') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0042::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLUid, KWavPcm16BitMono22Khz, KMmfUidFormatWAVWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0042::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0043::CTest_MMF_AFMT_U_0043()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0043-HP");
	}

/** @xxxx
 * This tests the call to create a new CMmfWavFormatEncode object.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0043::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KMono, 8000, TFourCC(' ', 'P', '1', '6') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0043::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLFilename, KWavPcm16BitMono8Khz, KMmfUidFormatWAVWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0043::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0044::CTest_MMF_AFMT_U_0044()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0044-HP");
	}

/** @xxxx
 * This tests the call to create a new CMmfWavFormatEncode object.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0044::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KMono, 11025, TFourCC(' ', 'P', 'U', '8') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0044::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseDescriptorTestsL(KNewLDescriptor, KWavPcm8BitMono11Khz, KMmfUidFormatWAVWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0044::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0045::CTest_MMF_AFMT_U_0045()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0045-HP");
	}

/** @xxxx
 * This tests the call to create a new CMmfWavFormatEncode object.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0045::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KStereo, 8000, TFourCC(' ', 'P', '1', '6') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0045::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLClip, KWavPcm16BitStereo8Khz, KMmfUidFormatWAVWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0045::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0056::CTest_MMF_AFMT_U_0056()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0056-HP");
	}

/** @xxxx
 * Tests the FrameTimeInterval of PCM16 22k mono file.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0056::DoTestStepL()
	{	
	return RunTest(EAudioPCM, 22050, 16, 1); //PCM Encode, 22Khz, 16-bit, Mono
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0056::OpenL()
	{	
	return SetupL(KWavPcm16BitMono22Khz, KMmfUidFormatWAVWrite);	
	}

// Postamble
void CTest_MMF_AFMT_U_0056::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0058::CTest_MMF_AFMT_U_0058()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0058-HP");
	}

/** @xxxx
 * Tests the FrameTimeInterval of ADPCM 8k mono file.
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0058::DoTestStepL()
	{	
	return RunTest(EAudioADPCM, 8000, 4, 1); //ADPCM Encode, 8Khz, Mono
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0058::OpenL()
	{	
	return SetupL(KWavAdpcm16BitMono8Khz, KMmfUidFormatWAVWrite);	
	}

// Postamble
void CTest_MMF_AFMT_U_0058::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0065::CTest_MMF_AFMT_U_0065()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0065-HP");
	}

/** @xxxx
 * Tests the Duration call with a PCM16 file
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0065::DoTestStepL()
	{	
	return RunDurationTest( TInt64(1000000) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0065::OpenL()
	{	
	return SetupL(KWavPcm16BitMono22Khz, KMmfUidFormatWAVWrite);		
	}

// Postamble
void CTest_MMF_AFMT_U_0065::Close()
	{
	// Cleanup test data 
	Cleanup();
	}



//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0067::CTest_MMF_AFMT_U_0067()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0067-HP");
	}

/** @xxxx
 * Tests the Duration call with a ADPCM file
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0067::DoTestStepL()
	{	
	return RunDurationTest( TInt64(5600000)); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0067::OpenL()
	{	
	return SetupL(KWavAdpcm16BitMono8Khz, KMmfUidFormatWAVWrite);		
	}

// Postamble
void CTest_MMF_AFMT_U_0067::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0069::CTest_MMF_AFMT_U_0069()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0069-HP");
	}

/** @xxxx
 * Tests the Duration call with a IMA ADPCM file
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0069::DoTestStepL()
	{	
	return RunDurationTest(TInt64(280601360) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0069::OpenL()
	{	
	_LIT(KImaAdpcm4BitMono11Khz, "c:\\mm\\mmf\\testfiles\\afmt\\pcm4mono11khz.wav");
	return SetupL(KImaAdpcm4BitMono11Khz, KMmfUidFormatWAVWrite);		
	}

// Postamble
void CTest_MMF_AFMT_U_0069::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0074::CTest_MMF_AFMT_U_0074()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0074-HP");
	}

/** @xxxx
 * Tests the CreateSinkBufferL call 
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0074::DoTestStepL()
	{	
	TVerdict verdict;
	TBool myBool = ETrue;
	CMMFBuffer* retBuffer = iFormatEnc->CreateSinkBufferL(KUidMediaTypeAudio, 
															myBool);

	TUint bufferSize = retBuffer->BufferSize();
	if (bufferSize >= KMMFFormatMinFrameSize && bufferSize <= KMMFFormatMaxFrameSize)
		{
		INFO_PRINTF2(_L("BufferSize() returned expected value %d"), retBuffer->BufferSize());
		verdict = EPass;
		}
	else
		{
		INFO_PRINTF2(_L("BufferSize() returned unexpected value %d"), retBuffer->BufferSize());
		verdict = EFail;
		}
		
	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0074::OpenL()
	{		
	SetupL(KWavPcm16BitStereo8Khz, KMmfUidFormatWAVWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0074::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0075::CTest_MMF_AFMT_U_0075()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0075-HP");
	}

/** @xxxx
 * Tests the CanCreateSinkBufferL call 
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0075::DoTestStepL()
	{	
	TVerdict verdict;

	// Check that the DECODE object CAN create a SOURCE buffer...
	if (iFormatEnc && iFormatEnc->CanCreateSinkBuffer())
		verdict = EPass;
	else
		verdict = EFail;

	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0075::OpenL()
	{		
	SetupL(KWavPcm16BitStereo8Khz, KMmfUidFormatWAVWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0075::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0076::CTest_MMF_AFMT_U_0076()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0076-HP");
	}

/** @xxxx
 * Tests the Number of Channels returned is correct
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0076::DoTestStepL()
	{	
	return TestNumChannelsL(KWavPcm8BitMono11Khz, KWavPcm16BitStereo8Khz);
	}

TVerdict CTest_MMF_AFMT_U_0076::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatWAVWrite);
	return EPass;
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0077::CTest_MMF_AFMT_U_0077()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0077-HP");
	}

/** @xxxx
 * Tests the SetNumChannels function works correctly
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0077::DoTestStepL()
	{	
	return TestSetNumChannelsL(KNonExistingWavFile, KNonExistingWavFile);
	}

TVerdict CTest_MMF_AFMT_U_0077::OpenL()
	{
	DeleteFileL(KNonExistingWavFile); // just to make sure...
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatWAVWrite);
	return EPass;
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0078::CTest_MMF_AFMT_U_0078()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0078-HP");
	}

/** @xxxx
 * Tests the Samplerate returned is correct
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0078::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	if (TestSampleRateL(8000, KWavPcm16BitMono8Khz) && //8khz test
		TestSampleRateL(11025,KWavPcm8BitMono11Khz) && //11khz test
		TestSampleRateL(22050,KWavPcm16BitMono22Khz) && //22khz test
		TestSampleRateL(44100,KWavPcm8BitMono44khz) ) //44khz test
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0078::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatWAVWrite);
	return EPass;
	}

void CTest_MMF_AFMT_U_0078::Close()
	{
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0079::CTest_MMF_AFMT_U_0079()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0079-HP");
	}

/** @xxxx
 * Tests the SetSampleRate function works correctly
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0079::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	// NOTE: For the SET-SampleRate tests, you have to specify a
	// file that doesn't exist, as otherwise it won't let you change 
	// the SampleRate...
	if (KErrNone == TestSetSampleRateL(8000, KNonExistingWavFile) && //8khz test
		KErrNone == TestSetSampleRateL(11025,KNonExistingWavFile) && //11khz test
		KErrNone == TestSetSampleRateL(22050,KNonExistingWavFile) && //22khz test
		KErrNone == TestSetSampleRateL(44100,KNonExistingWavFile) && //44khz test

	// These tests if the iFormatEnc object has been setup with a file
	// that already exists, we CAN'T change the sample rate.
		KErrAlreadyExists == TestSetSampleRateL(8000, KWavPcm8BitMono44khz) && 
		KErrAlreadyExists == TestSetSampleRateL(11025,KWavPcm16BitMono8Khz) && 
		KErrAlreadyExists == TestSetSampleRateL(22050,KWavPcm8BitMono11Khz) && 
		KErrAlreadyExists == TestSetSampleRateL(44100,KWavPcm16BitMono22Khz) )
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0079::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatWAVWrite);
	return EPass;
	}

void CTest_MMF_AFMT_U_0079::Close()
	{
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0082::CTest_MMF_AFMT_U_0082()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0082-HP");
	}

/** @xxxx
 * Tests the SetSampleRate function works correctly
 * @test Req. under test 
 */
TVerdict CTest_MMF_AFMT_U_0082::DoTestStepL()
	{	
	TVerdict verdict = RunEmptyBufferTestL();
	return verdict;
	}

/**
*
* OpenL
*/
TVerdict CTest_MMF_AFMT_U_0082::OpenL()
	{
	DeleteFileL(KNonExistingWavFile); // ensure file does not exist
	CopyFileL(KWavPcm16BitStereo8KhzCompare, KNonExistingWavFile  );
	return SetupL(KWavPcm16BitStereo8KhzCompare, KMmfUidFormatWAVRead, KNonExistingWavFile, KMmfUidFormatWAVWrite);
	}

void CTest_MMF_AFMT_U_0082::Close()
	{
	}


//-----------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0083::CTest_MMF_AFMT_U_0083()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0083-HP");
	}

/** @xxxx
 * DoTestStepL
 */
TVerdict CTest_MMF_AFMT_U_0083::DoTestStepL()
	{	
	TVerdict verdict = RunEmptyBufferTestL();
	return verdict;
	}

/** @xxxx
 * OpenL
 */
TVerdict CTest_MMF_AFMT_U_0083::OpenL()
	{
	DeleteFileL(KNonExistingRawFile); // ensure file does not exist
	CopyFileL(KRawAdpcm16BitMono8Khz, KNonExistingRawFile  );
	//[ ensure these parameters reflect the input file]
	iChannels   = KMono;
	iSampleRate = 8000;
	iFourCC     =KMMFFourCCCodeIMAD;
	return SetupL(KRawAdpcm16BitMono8Khz, KMmfUidFormatRAWRead, KNonExistingRawFile, KMmfUidFormatRAWWrite);
	}

/** @xxxx
* Close
*/
void CTest_MMF_AFMT_U_0083::Close()
	{
	
	}

/** @xxxx
* SetupL
*/
TVerdict CTest_MMF_AFMT_U_0083::SetupL( const TDesC& aSrcFilename, const TInt& aSrcUid, const TDesC& aSinkFilename, const TInt& aSinkUid)
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

	iFormatDec->SetNumChannels( iChannels );
	iFormatDec->SetSampleRate( iSampleRate );
	STATIC_CAST( MDataSource*, iFormatDec)->SetSourceDataTypeCode( iFourCC, KUidMediaTypeAudio );

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

	// explicitly set the fourcc, samplerate & bitrate
	// since it cannot be gleaned from the file for raw formats :-(
	//
	iFormatEnc->SetNumChannels( iChannels );
	iFormatEnc->SetSampleRate( iSampleRate );
	STATIC_CAST( MDataSink*, iFormatEnc)->SetSinkDataTypeCode( iFourCC, KUidMediaTypeAudio );

	iFormatEnc->SinkPlayL();

	// Store this for future use... :-)
	iFilename.Set(aSrcFilename);

	return verdict;
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0084::CTest_MMF_AFMT_U_0084()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0084-HP");
	}

/** @xxxx
 * DoTestStepL
 */
TVerdict CTest_MMF_AFMT_U_0084::DoTestStepL()
	{
	return RunEmptyBufferTestL();
	}

/** @xxxx
 * OpenL
 */
TVerdict CTest_MMF_AFMT_U_0084::OpenL()
	{	
	// [ note gsm16 cannot be appended to and wee need a codec to produce valid gsm
	// buffers for wrting]
	DeleteFileL(KNonExistingWavFile); // ensure file does not exist
	return SetupL( KWavPcm16BitMono8Khz, KMmfUidFormatWAVRead, KNonExistingWavFile, KMmfUidFormatWAVWrite);
	}

/** @xxxx
 * Close
 *
 */
void CTest_MMF_AFMT_U_0084::Close()
	{
	}


/** @xxxx
 *
 * 	RunEmptyBufferTestL
 *
 */
TVerdict CTest_MMF_AFMT_U_0084::RunEmptyBufferTestL()
	{
	TVerdict verdict = EFail;

	//[  ok lets load a codec to get a valid gsm buffer from a 
	//	 pcm 16 8khz mono buffer, note only 8khz pcm is supported in this conversion ]
	TFourCC sourceFourCCCode = KMMFFourCCCodePCM16;
	TFourCC sinkFourCCCode   = KMMFFourCCCodeGSM610;
	CMMFCodec* codec         = CMMFCodec::NewL(sourceFourCCCode, sinkFourCCCode);
	CleanupStack::PushL(codec);
			
	// Run through the file, testing up to 1
	for (TInt frame=KStartFrame; frame< (KStartFrame+iRuns) ; frame++)
		{
		// This function calls FillBufferL, and waits for the BufferFilledL call to
		// come back with the buffer.  The buffer that has been filled is 
		// stored in iBufferFilled.	
		CallFillBufferL(*iDummySink, frame);

		iFilename.Set(iSinkFilename);
        
		// Create a buffer to be filled.
	    CMMFDataBuffer* dstBuffer = CMMFDataBuffer::NewL(KGSMAvgBytesPerSecond);
		CleanupStack::PushL(dstBuffer);

		// [ convert the pcm16 8khz buffer to gsm10 and
		// check the return code ]
		TCodecProcessResult result = codec->ProcessL( *iBuffer, *dstBuffer); 
		switch (result.iStatus)
		{
		// we do not care too much about these errors
		// since the name of the game is writing a buffer to file
		case TCodecProcessResult::EProcessComplete:
		case TCodecProcessResult::EProcessIncomplete:
		case TCodecProcessResult::EDstNotFilled:
		case TCodecProcessResult::EEndOfData:
		break;
		// if an error occurred or an unknow code is returned
		// we should fail
		case TCodecProcessResult::EProcessError:
		User::Leave(KErrCorrupt); //codec process error
		break;
		default:
		User::Leave(KErrCorrupt); //should never get here
		}
		
		// pretty sure the process is not completed but we do not care since
		// we are interested in writing one good buffer of data to the sink
	    CleanupStack::Pop(dstBuffer);

		// the buffer to be processed
		// delete iBuffer since we do not need the source data any more
		// and replace with the destination buffer
		delete iBuffer ;
		iBuffer = dstBuffer ; 

		//[ ok lets now append the converted data to the sink file]
		CallEmptyBufferL( *iDummySource );
		}

	    iDataSource->SourceStopL();
		iDataSink->SinkStopL();

		//[ ok now lets ensure the data was written to file]
		RFs testFsSession;
		User::LeaveIfError(testFsSession.Connect()) ;
		CleanupClosePushL(testFsSession);

		// Get the size of the test file.
		RFile sinkFile;
		TFileName sinkFileName( iSinkFilename);
		User::LeaveIfError(sinkFile.Open(testFsSession, sinkFileName, EFileRead));
		CleanupClosePushL(sinkFile);

		TInt sinkFileSize;
		User::LeaveIfError(sinkFile.Size(sinkFileSize));
						
		CleanupStack::PopAndDestroy(3); // codec testFsSession, sinkFile
	
		if( 0 < sinkFileSize )
			{
			verdict = EPass;
			}

	return verdict;
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0090::CTest_MMF_AFMT_U_0090()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0090-HP");
	}

/** @xxxx
 * Create a new AU Format reader and check it is set up correctly
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0090::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KMono, 22050, TFourCC('P', '1', '6', 'B') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0090::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLUid, KAuPcm16BitMono22Khz, KMmfUidFormatAURead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0090::Close()
	{
	// Cleanup test data 
	Cleanup();
	}



//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0091::CTest_MMF_AFMT_U_0091()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0091-HP");
	}

/** @xxxx
 * Create a new AU Format reader and check it is set up correctly
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0091::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KMono, 8000, TFourCC('P', '1', '6', 'B') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0091::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLFilename, KAuPcm16BitMono8Khz, KMmfUidFormatAURead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0091::Close()
	{
	// Cleanup test data 
	Cleanup();
	}
//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0092::CTest_MMF_AFMT_U_0092()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0092-HP");
	}

/** @xxxx
 * Create a new AU Format reader and check it is set up correctly
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0092::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KMono, 11025, TFourCC(' ', ' ', 'P',  '8') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0092::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseDescriptorTestsL(KNewLDescriptor, KAuPcm8BitMono11Khz, KMmfUidFormatAURead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0092::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0093::CTest_MMF_AFMT_U_0093()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0093-HP");
	}

/** @xxxx
 * Create a new AU Format reader and check it is set up correctly
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0093::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KStereo, 8000, TFourCC('P', '1', '6', 'B') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0093::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLClip, KAuPcm16BitStereo8Khz, KMmfUidFormatAURead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0093::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0105::CTest_MMF_AFMT_U_0105()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0105-HP");
	}

/** @xxxx
 * Tests the FrameTimeInterval of PCM16 22k mono file.
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0105::DoTestStepL()
	{	
	return RunTest(EAudioPCM, 22050, 16, 1); // PCM, Decode, 22Khz, 16-bit, Mono
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0105::OpenL()
	{	
	return SetupL(KAuPcm16BitMono22Khz, KMmfUidFormatAURead);	
	}

// Postamble
void CTest_MMF_AFMT_U_0105::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0111::CTest_MMF_AFMT_U_0111()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0111-HP");
	}

/** @xxxx
 * Tests the Duration call with a PCM16 file
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0111::DoTestStepL()
	{	
	return RunDurationTest( TInt64(1000000) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0111::OpenL()
	{	
	return SetupL(KAuPcm16BitMono22Khz, KMmfUidFormatAURead);		
	}

// Postamble
void CTest_MMF_AFMT_U_0111::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0113::CTest_MMF_AFMT_U_0113()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0113-HP");
	}

/** @xxxx
 * Tests the CreateSourceBufferL call
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0113::DoTestStepL()
	{	
	TVerdict verdict;
	TBool myBool = ETrue;
	CMMFBuffer* retBuffer = iFormatDec->CreateSourceBufferL(KUidMediaTypeAudio, 
															myBool);

	TUint bufferSize = retBuffer->BufferSize();
	if (bufferSize >= KMMFFormatMinFrameSize && bufferSize <= KMMFFormatMaxFrameSize)
		verdict = EPass;
	else 
		verdict = EFail;
		
	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0113::OpenL()
	{		
	SetupL(KAuPcm16BitStereo8Khz, KMmfUidFormatAURead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0113::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0114::CTest_MMF_AFMT_U_0114()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0114-HP");
	}

/** @xxxx
 * Tests the CanCreateSourceBuffer call
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0114::DoTestStepL()
	{	
	TVerdict verdict;

	// Check that the DECODE object CAN create a SOURCE buffer...
	if (iFormatDec && iFormatDec->CanCreateSourceBuffer())
		verdict = EPass;
	else
		verdict = EFail;

	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0114::OpenL()
	{		
	SetupL(KAuPcm16BitStereo8Khz, KMmfUidFormatAURead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0114::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0115::CTest_MMF_AFMT_U_0115()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0115-HP");
	}

/** @xxxx
 * Tests the Number of Channels returned is correct
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0115::DoTestStepL()
	{	
	return TestNumChannelsL(KAuPcm8BitMono11Khz, KAuPcm16BitStereo8Khz);
	}

TVerdict CTest_MMF_AFMT_U_0115::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatAURead);
	return EPass;
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0116::CTest_MMF_AFMT_U_0116()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0116-HP");
	}

/** @xxxx
 * Tests the Samplerate returned is correct
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0116::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	if (TestSampleRateL(8000, KAuPcm16BitMono8Khz) &&	//8khz test
		TestSampleRateL(11025,KAuPcm8BitMono11Khz) &&	//11khz test
		TestSampleRateL(22050,KAuPcm16BitMono22Khz) &&	//22khz test
		TestSampleRateL(44100,KAuPcm8BitMono44khz) )	//44khz test
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0116::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatAURead);
	return EPass;
	}

void CTest_MMF_AFMT_U_0116::Close()
	{
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0125::CTest_MMF_AFMT_U_0125()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0125-HP");
	}

/** @xxxx
 * This tests the call to create a new CMmfAUFormatEncode object.
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0125::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KMono, 22050, TFourCC('P', '1', '6', 'B') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0125::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLUid, KAuPcm16BitMono22Khz, KMmfUidFormatAUWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0125::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0126::CTest_MMF_AFMT_U_0126()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0126-HP");
	}

/** @xxxx
 * This tests the call to create a new CMmfAuFormatEncode object.
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0126::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KMono, 8000, TFourCC('P', '1', '6', 'B') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0126::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLFilename, KAuPcm16BitMono8Khz, KMmfUidFormatAUWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0126::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0127::CTest_MMF_AFMT_U_0127()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0127-HP");
	}

/** @xxxx
 * This tests the call to create a new CMmfAuFormatEncode object.
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0127::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KMono, 11025, TFourCC(' ', ' ', 'P',  '8') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0127::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseDescriptorTestsL(KNewLDescriptor, KAuPcm8BitMono11Khz, KMmfUidFormatAUWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0127::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0128::CTest_MMF_AFMT_U_0128()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0128-HP");
	}

/** @xxxx
 * This tests the call to create a new CMmfAuFormatEncode object.
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0128::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KStereo, 8000, TFourCC('P', '1', '6', 'B'));
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0128::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLClip, KAuPcm16BitStereo8Khz, KMmfUidFormatAUWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0128::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0139::CTest_MMF_AFMT_U_0139()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0139-HP");
	}

/** @xxxx
 * Tests the FrameTimeInterval of PCM16 22k mono file.
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0139::DoTestStepL()
	{	
	return RunTest(EAudioPCM, 22050, 16, 1); // PCM Encode, 22Khz, 16-bit, Mono
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0139::OpenL()
	{	
	return SetupL(KAuPcm16BitMono22Khz, KMmfUidFormatAUWrite);	
	}

// Postamble
void CTest_MMF_AFMT_U_0139::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0145::CTest_MMF_AFMT_U_0145()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0145-HP");
	}

/** @xxxx
 * Tests the Duration call with a PCM16 file
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0145::DoTestStepL()
	{	
	return RunDurationTest( TInt64(1000000) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0145::OpenL()
	{	
	return SetupL(KAuPcm16BitMono22Khz, KMmfUidFormatAUWrite);		
	}

// Postamble
void CTest_MMF_AFMT_U_0145::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0149::CTest_MMF_AFMT_U_0149()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0149-HP");
	}

/** @xxxx
 * Tests the CreateSinkBuffer call
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0149::DoTestStepL()
	{	
	TVerdict verdict;
	TBool myBool = ETrue;
	CMMFBuffer* retBuffer = iFormatEnc->CreateSinkBufferL(KUidMediaTypeAudio, 
															myBool);

	TUint bufferSize = retBuffer->BufferSize();
	if (bufferSize >= KMMFFormatMinFrameSize && bufferSize <= KMMFFormatMaxFrameSize)
		verdict = EPass;
	else 
		verdict = EFail;
		
	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0149::OpenL()
	{		
	SetupL(KAuPcm16BitStereo8Khz, KMmfUidFormatAUWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0149::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0150::CTest_MMF_AFMT_U_0150()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0150-HP");
	}

/** @xxxx
 * Tests the CanCreateSinkBuffer call
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0150::DoTestStepL()
	{	
	TVerdict verdict;

	// Check that the DECODE object CAN create a SOURCE buffer...
	if (iFormatEnc && iFormatEnc->CanCreateSinkBuffer())
		verdict = EPass;
	else
		verdict = EFail;

	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0150::OpenL()
	{		
	SetupL(KAuPcm16BitStereo8Khz, KMmfUidFormatAUWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0150::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0151::CTest_MMF_AFMT_U_0151()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0151-HP");
	}

/** @xxxx
 * Tests the Number of Channels returned is correct
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0151::DoTestStepL()
	{	
	return TestNumChannelsL(KAuPcm8BitMono11Khz, KAuPcm16BitStereo8Khz);
	}

TVerdict CTest_MMF_AFMT_U_0151::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatAUWrite);
	return EPass;
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0152::CTest_MMF_AFMT_U_0152()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0152-HP");
	}

/** @xxxx
 * Tests the SetNumChannels call works correctly
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0152::DoTestStepL()
	{	
	return TestSetNumChannelsL(KNonExistingAuFile, KNonExistingAuFile);
	}

TVerdict CTest_MMF_AFMT_U_0152::OpenL()
	{
	DeleteFileL(KNonExistingAuFile); // just to make sure...

	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatAUWrite);
	return EPass;
	}



//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0153::CTest_MMF_AFMT_U_0153()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0153-HP");
	}

/** @xxxx
 * Tests the Samplerate returned is correct
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0153::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	if (TestSampleRateL(8000, KAuPcm16BitMono8Khz) && //8khz test
		TestSampleRateL(11025,KAuPcm8BitMono11Khz) && //11khz test
		TestSampleRateL(22050,KAuPcm16BitMono22Khz) && //22khz test
		TestSampleRateL(44100,KAuPcm8BitMono44khz) ) //44khz test
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0153::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatAUWrite);
	return EPass;
	}

void CTest_MMF_AFMT_U_0153::Close()
	{
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0154::CTest_MMF_AFMT_U_0154()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0154-HP");
	}

/** @xxxx
 * Tests the SetSampleRate call works correctly
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0154::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	// NOTE: For the SET-SampleRate tests, you have to specify a
	// file that doesn't exist, as otherwise it won't let you change 
	// the SampleRate...
	if (KErrNone == TestSetSampleRateL(8000, KNonExistingAuFile) && //8khz test
		KErrNone == TestSetSampleRateL(11025,KNonExistingAuFile) && //11khz test
		KErrNone == TestSetSampleRateL(22050,KNonExistingAuFile) && //22khz test
		KErrNone == TestSetSampleRateL(44100,KNonExistingAuFile) && //44khz test

	// These tests if the iFormatEnc object has been setup with a file
	// that already exists, we CAN'T change the sample rate.
		KErrAlreadyExists == TestSetSampleRateL(8000, KAuPcm8BitMono44khz) && 
		KErrAlreadyExists == TestSetSampleRateL(11025,KAuPcm16BitMono8Khz) && 
		KErrAlreadyExists == TestSetSampleRateL(22050,KAuPcm8BitMono11Khz) && 
		KErrAlreadyExists == TestSetSampleRateL(44100,KAuPcm16BitMono22Khz) )
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0154::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatAUWrite);
	return EPass;
	}

void CTest_MMF_AFMT_U_0154::Close()
	{
	}




//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0165::CTest_MMF_AFMT_U_0165()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0165-HP");
	}

/** @xxxx
 * Create a new RAW Format reader and check it is set up correctly
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0165::DoTestStepL()
	{	
	// Run NewL
	RunDecodeNewL();	

	// Tell RAW Format object what format the data is in
	iFormatDec->SetNumChannels(KStereo);
	iFormatDec->SetSampleRate(8000);
	iFormatDec->SetSourceDataTypeCode(TFourCC(' ', 'P', '1', '6'), KUidMediaTypeAudio);
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyDecodeNewL(KUidMmfFormatDecode, KStereo, 8000, TFourCC(' ', 'P', '1', '6') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0165::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLUid, KRawPcm16BitStereo8Khz, KMmfUidFormatRAWRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0165::Close()
	{
	// Cleanup test data 
	Cleanup();
	}
	
//------------------------------------------------------------------

// Removed CTest_MMF_AFMT_U_0180 - no longer required

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0189::CTest_MMF_AFMT_U_0189()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0189-HP");
	}

/** @xxxx
 * Tests the Duration call with a PCM16 file
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0189::DoTestStepL()
	{	
	return RunDurationTest( TInt64(1000000) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0189::OpenL()
	{	
	TVerdict verdict = SetupL(KRawPcm16BitMono22Khz, KMmfUidFormatRAWRead);		
	// If did bulk of setting up ok, tell the RAW object what the format is.
	// Note this will be different depending on the test file used in "SetupL"
	if (iFormatDec && verdict == EPass)
		{	
		iFormatDec->SetSampleRate(22050);
		iFormatDec->SetNumChannels(KMono);
		iFormatDec->SetSourceDataTypeCode(KMMFFourCCCodePCM16, KUidMediaTypeAudio);
		}
	return verdict;
	}

// Postamble
void CTest_MMF_AFMT_U_0189::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0119::CTest_MMF_AFMT_U_0119()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0119-HP");
	}

/** @xxxx
 * This tests the call to FillbufferL with PCM16 data.
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0119::DoTestStepL()
	{
	return RunFillBufferTestL(KNumberOfFillBufferLTests,KAuPcm16BitStereo8KhzCompare);		
	}

TVerdict CTest_MMF_AFMT_U_0119::OpenL()
	{
	return SetupL(KAuPcm16BitStereo8Khz, KMmfUidFormatAURead);
	}

void CTest_MMF_AFMT_U_0119::Close()
	{
	Cleanup();
	}






//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0194::CTest_MMF_AFMT_U_0194()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0194-HP");
	}

/** @xxxx
 * Tests the CreateSourceBufferL call 
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0194::DoTestStepL()
	{	
	TVerdict verdict;
	TBool myBool = ETrue;
	CMMFBuffer* retBuffer = iFormatDec->CreateSourceBufferL(KUidMediaTypeAudio, 
															myBool);
	
	TUint bufferSize = retBuffer->BufferSize();
	if (bufferSize >= KMMFFormatMinFrameSize && bufferSize <= KMMFFormatMaxFrameSize)
		verdict = EPass;
	else 
		verdict = EFail;
		
	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0194::OpenL()
	{		
	SetupL(KRawPcm16BitStereo8Khz, KMmfUidFormatRAWRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0194::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0195::CTest_MMF_AFMT_U_0195()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0195-HP");
	}

/** @xxxx
 * Tests the CanCreateSourceBufferL call 
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0195::DoTestStepL()
	{	
	TVerdict verdict;

	// Check that the DECODE object CAN create a SOURCE buffer...
	if (iFormatDec && iFormatDec->CanCreateSourceBuffer())
		verdict = EPass;
	else
		verdict = EFail;

	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0195::OpenL()
	{		
	SetupL(KRawPcm16BitStereo8Khz, KMmfUidFormatRAWRead);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0195::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0196::CTest_MMF_AFMT_U_0196()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0196-HP");
	}

/** @xxxx
 * Tests the Number of Channels returned is correct
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0196::DoTestStepL()
	{	
	return TestNumChannelsL(KRawPcm8BitMono11Khz, KRawPcm16BitStereo8Khz);
	}

TVerdict CTest_MMF_AFMT_U_0196::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatRAWRead);	
	return EPass;
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0197::CTest_MMF_AFMT_U_0197()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0197-HP");
	}

/** @xxxx
 * Tests the SetNumChannels() call
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0197::DoTestStepL()
	{	
	return TestSetNumChannelsL(KRawPcm8BitMono11Khz, KRawPcm16BitStereo8Khz);
	}

TVerdict CTest_MMF_AFMT_U_0197::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatRAWRead);
	return EPass;
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0198::CTest_MMF_AFMT_U_0198()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0198-HP");
	}

/** @xxxx
 * Tests the Samplerate returned is correct
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0198::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	if (TestSampleRateL(8000, KRawPcm16BitMono8Khz) &&	//8khz test
		TestSampleRateL(11025,KRawPcm8BitMono11Khz) &&	//11khz test
		TestSampleRateL(22050,KRawPcm16BitMono22Khz) &&	//22khz test
		TestSampleRateL(44100,KRawPcm8BitMono44khz) )	//44khz test
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0198::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatRAWRead);
	return EPass;
	}

void CTest_MMF_AFMT_U_0198::Close()
	{
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0199::CTest_MMF_AFMT_U_0199()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0199-HP");
	}

/** @xxxx
 * Tests the SetSampleRate() call
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0199::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	// NOTE: For the SET-SampleRate tests, you have to specify a
	// DIFFERENT sample rate than that used in the file supplied.
	if (KErrNone == TestSetSampleRateL(8000, KRawPcm8BitMono44khz) &&	//8khz test
		KErrNone == TestSetSampleRateL(11025,KRawPcm16BitMono8Khz) &&	//11khz test
		KErrNone == TestSetSampleRateL(22050,KRawPcm8BitMono11Khz) &&	//22khz test
		KErrNone == TestSetSampleRateL(44100,KRawPcm16BitMono22Khz) )	//44khz test
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0199::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatRAWRead);
	return EPass;
	}

void CTest_MMF_AFMT_U_0199::Close()
	{
	}



//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0203::CTest_MMF_AFMT_U_0203()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0203-HP");
	}

/** @xxxx
 * This tests the call to FillbufferL call with PCM16 data.
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0203::DoTestStepL()
	{
	return RunFillBufferTestL(KNumberOfFillBufferLTests, KRawPcm16BitStereo8KhzCompare);		
	}

TVerdict CTest_MMF_AFMT_U_0203::OpenL()
	{
	TVerdict verdict = SetupL(KRawPcm16BitStereo8Khz, KMmfUidFormatRAWRead);

	if (iFormatDec && verdict == EPass)
		{	
		iFormatDec->SetSampleRate(8000);
		iFormatDec->SetNumChannels(KStereo);
		iFormatDec->SetSourceDataTypeCode(KMMFFourCCCodePCM16, KUidMediaTypeAudio);
		}
	
	return verdict;
	}

void CTest_MMF_AFMT_U_0203::Close()
	{
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0204::CTest_MMF_AFMT_U_0204()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0204-HP");
	}

/** @xxxx
 * This tests the call to FillbufferL call with ADPCM data.
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0204::DoTestStepL()
	{
	return RunFillBufferTestL(KNumberOfFillBufferLTests, KRawAdpcm16BitMono8KhzCompare);		
	}

TVerdict CTest_MMF_AFMT_U_0204::OpenL()
	{
	TVerdict verdict = SetupL(KRawAdpcm16BitMono8Khz, KMmfUidFormatRAWRead);

	if (iFormatDec && verdict == EPass)
		{	
		iFormatDec->SetSampleRate(8000);
		iFormatDec->SetNumChannels(KMono);
		iFormatDec->SetSourceDataTypeCode(KMMFFourCCCodePCM16, KUidMediaTypeAudio);
		}
	
	return verdict;
	}

void CTest_MMF_AFMT_U_0204::Close()
	{
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0205::CTest_MMF_AFMT_U_0205()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0205-HP");
	}

/** @xxxx
 * This tests the call to FillbufferL call with GSM610 data.
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0205::DoTestStepL()
	{
	return RunFillBufferTestL(KNumberOfFillBufferLTests, KRawGsmMono8KhzCompare);		
	}

TVerdict CTest_MMF_AFMT_U_0205::OpenL()
	{
	TVerdict verdict = SetupL(KRawGsmMono8Khz, KMmfUidFormatRAWRead);

	if (iFormatDec && verdict == EPass)
		{	
		iFormatDec->SetSampleRate(8000);
		iFormatDec->SetNumChannels(KMono);
		iFormatDec->SetSourceDataTypeCode(KMMFFourCCCodeGSM610, KUidMediaTypeAudio);
		}
	
	return verdict;
	}

void CTest_MMF_AFMT_U_0205::Close()
	{
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0209::CTest_MMF_AFMT_U_0209()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0209-HP");
	}

/** @xxxx
 * Create a new RAW Format writer and check it is set up correctly
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0209::DoTestStepL()
	{	
	// Run NewL
	RunEncodeNewL();	

	// Tell RAW Format object what format the data is in
	iFormatEnc->SetNumChannels(KStereo);
	iFormatEnc->SetSampleRate(8000);
	iFormatEnc->SetSinkDataTypeCode(TFourCC(' ', 'P', '1', '6'), KUidMediaTypeAudio);
		
	//Verify the iFormat object is created and the correct format is detected.
	return VerifyEncodeNewL(KUidMmfFormatEncode, KStereo, 8000, TFourCC(' ', 'P', '1', '6') );
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0209::OpenL()
	{
	// Setup all the member variables required for running the test.
	InitialiseFileTestsL(KNewLUid, KRawPcm16BitStereo8Khz, KMmfUidFormatRAWWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0209::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0223::CTest_MMF_AFMT_U_0223()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0223-HP");
	}

/** @xxxx
 * Tests the FrameTimeInterval of PCM16 22k stereo file.
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0223::DoTestStepL()
	{	
	return RunTest(EAudioPCM, 22050, 16, 1); // PCM Encode, 16-bit 22k mono
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0223::OpenL()
	{	
	TVerdict verdict = SetupL(KRawPcm16BitMono22Khz, KMmfUidFormatRAWWrite);	
		
	// If did bulk of setting up ok, tell the RAW object what the format is.
	// Note this will be different depending on the test file used in "SetupL"
	if (iFormatEnc && verdict == EPass)
		{	
		iFormatEnc->SetSampleRate(22050);
		iFormatEnc->SetNumChannels(KMono);
		iFormatEnc->SetSinkDataTypeCode(KMMFFourCCCodePCM16,KUidMediaTypeAudio);
		iFormatEnc->SinkPrimeL(); // Update Interval...
		}
	return verdict;
	}

// Postamble
void CTest_MMF_AFMT_U_0223::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0232::CTest_MMF_AFMT_U_0232()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0232-HP");
	}

/** @xxxx
 * Tests the Duration call with a PCM16 file
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0232::DoTestStepL()
	{	
	return RunDurationTest( TInt64(1000000) ); // Microseconds to test for
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0232::OpenL()
	{	
	TVerdict verdict = SetupL(KRawPcm16BitMono22Khz, KMmfUidFormatRAWWrite);		
	// If did bulk of setting up ok, tell the RAW object what the format is.
	// Note this will be different depending on the test file used in "SetupL"
	if (iFormatEnc && verdict == EPass)
		{	
		iFormatEnc->SetSampleRate(22050);
		iFormatEnc->SetNumChannels(KMono);
		iFormatEnc->SetSinkDataTypeCode(KMMFFourCCCodePCM16, KUidMediaTypeAudio);
		}
	return verdict;
	}

// Postamble
void CTest_MMF_AFMT_U_0232::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0241::CTest_MMF_AFMT_U_0241()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0241-HP");
	}

/** @xxxx
 * Tests the CreateSinkBuffer call
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0241::DoTestStepL()
	{	
	TVerdict verdict;
	TBool myBool = ETrue;
	CMMFBuffer* retBuffer = iFormatEnc->CreateSinkBufferL(KUidMediaTypeAudio, 
															myBool);

	TUint bufferSize = retBuffer->BufferSize();
	if (bufferSize >= KMMFFormatMinFrameSize && bufferSize <= KMMFFormatMaxFrameSize)
		verdict = EPass;
	else 
		verdict = EFail;
		
	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0241::OpenL()
	{		
	SetupL(KRawPcm16BitStereo8Khz, KMmfUidFormatRAWWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0241::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0242::CTest_MMF_AFMT_U_0242()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0242-HP");
	}

/** @xxxx
 * Tests the CanCreateSinkBuffer call
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0242::DoTestStepL()
	{	
	TVerdict verdict;

	// Check that the DECODE object CAN create a SOURCE buffer...
	if (iFormatEnc && iFormatEnc->CanCreateSinkBuffer())
		verdict = EPass;
	else
		verdict = EFail;

	return verdict;
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0242::OpenL()
	{		
	SetupL(KRawPcm16BitStereo8Khz, KMmfUidFormatRAWWrite);
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0242::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0243::CTest_MMF_AFMT_U_0243()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0243-HP");
	}

/** @xxxx
 * Tests the Number of Channels returned is correct
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0243::DoTestStepL()
	{	
	return TestNumChannelsL(KRawPcm8BitMono11Khz, KRawPcm16BitStereo8Khz);
	}

TVerdict CTest_MMF_AFMT_U_0243::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatRAWWrite);
	return EPass;
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0244::CTest_MMF_AFMT_U_0244()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0244-HP");
	}

/** @xxxx
 * Tests the SetNumChannels call
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0244::DoTestStepL()
	{	
	return TestSetNumChannelsL(KRawPcm8BitMono11Khz, KRawPcm16BitStereo8Khz);
	}

TVerdict CTest_MMF_AFMT_U_0244::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatRAWWrite);
	return EPass;
	}


//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0245::CTest_MMF_AFMT_U_0245()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0245-HP");
	}

/** @xxxx
 * Tests the Samplerate returned is correct
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0245::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	if (TestSampleRateL(8000, KRawPcm16BitMono8Khz) && //8khz test
		TestSampleRateL(11025,KRawPcm8BitMono11Khz) && //11khz test
		TestSampleRateL(22050,KRawPcm16BitMono22Khz) && //22khz test
		TestSampleRateL(44100,KRawPcm8BitMono44khz) ) //44khz test
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0245::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatRAWWrite);
	return EPass;
	}

void CTest_MMF_AFMT_U_0245::Close()
	{
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0246::CTest_MMF_AFMT_U_0246()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0246-HP");
	}

/** @xxxx
 * Tests the SetSampleRate call
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0246::DoTestStepL()
	{	
	TVerdict verdict = EFail;

	// Run all of the sub-tests (must pass them all to pass this test).
	// NOTE: For the SET-SampleRate tests, you have to specify a
	// file that doesn't exist, as otherwise it won't let you change 
	// the SampleRate...
	if (KErrNone == TestSetSampleRateL(8000, KNonExistingRawFile) && //8khz test
		KErrNone == TestSetSampleRateL(11025,KNonExistingRawFile) && //11khz test
		KErrNone == TestSetSampleRateL(22050,KNonExistingRawFile) && //22khz test
		KErrNone == TestSetSampleRateL(44100,KNonExistingRawFile) )  //44khz test
		{
		verdict = EPass;
		}			

	return verdict;
	}

TVerdict CTest_MMF_AFMT_U_0246::OpenL()
	{
	// Tell test the type of Format object we want
	SetupL(KMmfUidFormatRAWWrite);
	return EPass;
	}

void CTest_MMF_AFMT_U_0246::Close()
	{
	}





//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0257::CTest_MMF_AFMT_U_0257()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0257-HP");
	}

/** @xxxx
 * Tests Out-of-memory condition of NewL
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0257::DoTestStepL()
	{	
	return RunAllOomTestsL(KMmfUidFormatWAVRead); // Run all test steps for WavRead
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0257::OpenL()
	{		
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0257::Close()
	{
	// Cleanup test data 
	Cleanup();
	}



//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0266::CTest_MMF_AFMT_U_0266()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0266-HP");
	}

/** @xxxx
 * Tests Out-of-memory condition of NewL
 * @test Req. under test REQ172.11.21
 */
TVerdict CTest_MMF_AFMT_U_0266::DoTestStepL()
	{	
	return RunAllOomTestsL(KMmfUidFormatWAVWrite); // Run all test steps 
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0266::OpenL()
	{		
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0266::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0276::CTest_MMF_AFMT_U_0276()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0276-HP");
	}

/** @xxxx
 * Tests Out-of-memory condition of NewL
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0276::DoTestStepL()
	{	
	return RunAllOomTestsL(KMmfUidFormatAURead); // Run all test steps 
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0276::OpenL()
	{		
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0276::Close()
	{
	// Cleanup test data 
	Cleanup();
	}


//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0284::CTest_MMF_AFMT_U_0284()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0284-HP");
	}

/** @xxxx
 * Tests Out-of-memory condition of NewL
 * @test Req. under test REQ172.11.22
 */
TVerdict CTest_MMF_AFMT_U_0284::DoTestStepL()
	{	
	return RunAllOomTestsL(KMmfUidFormatAUWrite); // Run all test steps 
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0284::OpenL()
	{		
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0284::Close()
	{
	// Cleanup test data 
	Cleanup();
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0294::CTest_MMF_AFMT_U_0294()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0294-HP");
	}

/** @xxxx
 * Tests Out-of-memory condition of NewL
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0294::DoTestStepL()
	{	
	return RunAllOomTestsL(KMmfUidFormatRAWRead); // Run all test steps 
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0294::OpenL()
	{		
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0294::Close()
	{
	// Cleanup test data 
	Cleanup();
	}







//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0302::CTest_MMF_AFMT_U_0302()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0302-HP");
	}

/** @xxxx
 * Tests Out-of-memory condition of NewL
 * @test Req. under test REQ172.11.23
 */
TVerdict CTest_MMF_AFMT_U_0302::DoTestStepL()
	{	
	return RunAllOomTestsL(KMmfUidFormatRAWWrite); // Run all test steps 
	}

// Preamble
TVerdict CTest_MMF_AFMT_U_0302::OpenL()
	{		
	return EPass;
	}

// Postamble
void CTest_MMF_AFMT_U_0302::Close()
	{
	// Cleanup test data 
	Cleanup();
	}



//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0315::CTest_MMF_AFMT_U_0315()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0315-HP");
	
	}

/** @xxxx
 *
 * DoTestStepL
 *
 */
TVerdict CTest_MMF_AFMT_U_0315::DoTestStepL()
	{                                       
	TVerdict verdict = RunEmptyBufferTestL();
	DeleteFileL(KNonExistingWavFile); // clean up the temporary file
	return verdict;
	}

/** @xxxx
 *
 * OpenL
 *
 */
TVerdict CTest_MMF_AFMT_U_0315::OpenL()
	{
	DeleteFileL(KNonExistingWavFile); // ensure file does not exist
	CopyFileL(KWavPcm16BitMono8Khz, KNonExistingWavFile );
	return SetupL(KWavPcm16BitMono8Khz, KMmfUidFormatWAVRead, KNonExistingWavFile, KMmfUidFormatWAVWrite);
  	}

/** @xxxx
 *
 * Close
 *
 */
void CTest_MMF_AFMT_U_0315::Close()
		{
		}

//------------------------------------------------------------------

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_AFMT_U_0317::CTest_MMF_AFMT_U_0317()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0317-HP");

	}

/** @xxxx
 *
 * DoTestStepL
 *
 */
TVerdict CTest_MMF_AFMT_U_0317::DoTestStepL()
	{
	TVerdict verdict = RunEmptyBufferTestL();
	DeleteFileL(KNonExistingAuFile); // clean up the temporary file
	return verdict;
	}

/** @xxxx
 *
 * OpenL
 *
 */
TVerdict CTest_MMF_AFMT_U_0317::OpenL()
	{
	DeleteFileL(KNonExistingAuFile); // ensure file does not exist
	CopyFileL(KAuPcm16BitStereo8KhzCompare, KNonExistingAuFile  );
	return SetupL(KAuPcm16BitStereo8Khz, KMmfUidFormatAURead, KNonExistingAuFile, KMmfUidFormatAUWrite);
	}

/** @xxxx
 *
 * Close
 *
 */
void CTest_MMF_AFMT_U_0317::Close()
	{
	}

//------------------------------------------------------------------

/** @xxxx
 *
 * CTest_MMF_AFMT_U_0319
 *
 */
CTest_MMF_AFMT_U_0319::CTest_MMF_AFMT_U_0319()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-AFMT-U-0319-HP");
	}

/** @xxxx
 *
 * DoTestStepL
 *
 */
TVerdict CTest_MMF_AFMT_U_0319::DoTestStepL()
	{
	TVerdict verdict = RunEmptyBufferTestL();
	DeleteFileL(KNonExistingRawFile2); // clean up the temporary file
	return verdict;
	}

/** @xxxx
 *
 * OpenL
 *
 */
TVerdict CTest_MMF_AFMT_U_0319::OpenL()
		{
		DeleteFileL(KNonExistingRawFile2); // ensure file does not exist
		CopyFileL(KRawPcm16BitMono8Khz, KNonExistingRawFile2  );
		//[ensure these parameters reflect the input file]
		iSampleRate = 8000;
		iChannels   = KMono;
		iFourCC     = KMMFFourCCCodePCM16;
	  	return SetupL(KRawPcm16BitMono8Khz, KMmfUidFormatRAWRead, KNonExistingRawFile2, KMmfUidFormatRAWWrite);
		}

/** @xxxx
 *
 * Close
 *
 */
void CTest_MMF_AFMT_U_0319::Close()
		{
		}

/** 
* @xxx
*
*   SetupL
* needs to be a special for raw format !
*
*/
TVerdict CTest_MMF_AFMT_U_0319::SetupL( const TDesC& aSrcFilename, const TInt& aSrcUid, const TDesC& aSinkFilename, const TInt& aSinkUid)
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

	iFormatDec->SetNumChannels( iChannels );
	iFormatDec->SetSampleRate( iSampleRate );
	STATIC_CAST( MDataSource*, iFormatDec)->SetSourceDataTypeCode( iFourCC, KUidMediaTypeAudio );

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

	// explicitly set the fourcc, samplerate & bitrate
	// since it cannot be gleaned from the file for raw formats :-(
	//
	iFormatEnc->SetNumChannels( iChannels );
	iFormatEnc->SetSampleRate( iSampleRate );
	STATIC_CAST( MDataSink*, iFormatEnc)->SetSinkDataTypeCode( iFourCC, KUidMediaTypeAudio );

	iFormatEnc->SinkPlayL();

	// Store this for future use... :-)
	iFilename.Set(aSrcFilename);

	return verdict;
	}






