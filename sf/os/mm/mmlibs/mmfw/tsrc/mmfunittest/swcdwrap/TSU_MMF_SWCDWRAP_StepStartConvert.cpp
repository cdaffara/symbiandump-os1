// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Test system includes
#include "TSU_MMF_SWCDWRAP_StepStartConvert.h"
#include "TSU_MMF_SWCDWRAP_TestDeviceImplementationUID.hrh"
#include "TSU_MMF_SWCDWRAP_ConvertHwDevObserver.h"
#include "TSU_MMF_SWCDWRAP_TestDevice.h"

/**
 * sets up test step hw device and observer
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTestConvert::InitializeTestStepUsingHwDeviceL(TUid aHwDeviceUid)
	{
	TVerdict verdict = EPass;

	verdict = CTestStep_MMF_SWCDWRAP::DoTestStepPreambleL();

	if (verdict == EPass)
		{

		TRAPD(err, iHwDevice = CMMFHwDevice::NewL(aHwDeviceUid));

		//check hw device has been created
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("could not create hw device plugin error %d" ), err );
			verdict = EInconclusive;
			}
		}
	
	if (verdict == EPass)
		{
		iActiveScheduler = CActiveScheduler::Current();
		if (!iActiveScheduler) iActiveScheduler = new(ELeave)CActiveScheduler;
		CActiveScheduler::Install(iActiveScheduler);

		iHwDeviceObserver = new(ELeave)CTestConvertMMFHwDeviceObserver(iHwDevice);

		THwDeviceInitParams	devInfo;
		devInfo.iHwDeviceObserver = iHwDeviceObserver;
		TInt error = iHwDevice->Init(devInfo);
		
		if (error != KErrNone)
			{
			verdict = EInconclusive;
			}
		}

	return verdict;
	}

/**
 * Pre-amble for sw codec test step using 'null' codec hw device
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTestConvert::DoTestStepPreambleL()
	{
	TVerdict verdict = EPass;

	TUid hwDeviceUid;
	hwDeviceUid.iUid = KMmfUidTSUSWCDWRAPTestDevice;

	verdict = InitializeTestStepUsingHwDeviceL(hwDeviceUid);

	return verdict;
	}

/**
 * Pre-amble for sw codec test step using 2:1 buffer ratio codec
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTest2To1Convert::DoTestStepPreambleL()
	{
	TVerdict verdict = EPass;

	TUid hwDeviceUid;
	hwDeviceUid.iUid = KMmfUidTSUSWCDWRAPTest2To1Device;

	verdict = InitializeTestStepUsingHwDeviceL(hwDeviceUid);

	return verdict;
	}

/**
 * Post-amble for sw codec test step
 */
TVerdict CTestStep_MMF_SWCDWRAP_StartTestConvert::DoTestStepPostambleL()
	{
	delete iHwDevice;
	delete iHwDeviceObserver;
	delete iActiveScheduler;

	return CTestStep_MMF_SWCDWRAP::DoTestStepPostambleL();
	}
 


CTestStep_MMF_SWCDWRAP_U_0200::CTestStep_MMF_SWCDWRAP_U_0200()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0200-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0200::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to convert one buffer
 * buffer size = codec buffer size
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt convertedFileSize = 0;

	TFileName sourceFilename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");
	TFileName sinkFilename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_converted.raw");

	TInt error = iHwDeviceObserver->SetConvertFiles(sourceFilename, sinkFilename, 1);//convert 1 buffer
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevNullFunc, EDevInAndOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during convert %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(convertedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with converted file error %d"), error);
				verdict = EFail;
				}
			if (convertedFileSize != KTestBufferSize)
				{
				INFO_PRINTF1(_L("converted file was not the expected size"));
				verdict = EFail;
				}
			//do a file comparison between source and sink files
			TBool compareResult = EFalse;
			TRAP(error, compareResult = CompareL(iHwDeviceObserver->ReadFile(), iHwDeviceObserver->WriteFile(),KTestBufferSize));
			if (error)
				{
				INFO_PRINTF2(_L("problem comparing files error %d"), error);
				if (verdict == EPass)
					verdict = EInconclusive;//inconclusive because convert probably still ok
				}
			else if (compareResult == EFalse)
				{
				INFO_PRINTF1(_L("converted file not identical to source file"));
				verdict = EFail;
				}
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening conversion files error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 

TBool CTestStep_MMF_SWCDWRAP_U_0200::CompareL(RFile& aFile1, RFile& aFile2, TUint aCompareLength)
	{
	TBool compareResult = ETrue; //ETrue if both files are identical
	HBufC8* file1Buffer = HBufC8::NewLC(4);
	HBufC8* file2Buffer = HBufC8::NewLC(4);

	//can't use HBufC8 directly with file server as 
	//HBufC8 is derived from TDesC8 but file server expects TDes8
	TPtr8 file1Ptr(file1Buffer->Des());
	TPtr8 file2Ptr(file2Buffer->Des());

	//cannot compare file all in one go as we might run
	//out of heap - so compare in 4 byte chunks
	for (TUint pos=0;pos<aCompareLength;pos+=4)
		{
		aFile1.Read(static_cast<TInt>(pos), file1Ptr, 4);
		aFile2.Read(static_cast<TInt>(pos), file2Ptr, 4);
		if (file1Buffer->Compare(*file2Buffer) != 0)
			{
			compareResult = EFalse; //file mismatch
			}
		}
	CleanupStack::PopAndDestroy( 2 ); //file1Buffer & file2Buffer
	return compareResult;
	}


CTestStep_MMF_SWCDWRAP_U_0240::CTestStep_MMF_SWCDWRAP_U_0240()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0240-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0240::DoTestStepL( void )
/**
 * Use case: N/A
 * Use Start() to convert one buffer
 * buffer size = codec buffer size
 * @test Req. under test REQ1931.2
 */
	{
	 
	TVerdict verdict = EPass;
	TInt convertedFileSize = 0;

	TFileName sourceFilename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");
	TFileName sinkFilename = _L("\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAP_converted2To1.raw");

	TInt error = iHwDeviceObserver->SetConvertFiles(sourceFilename, sinkFilename, 1);//convert 1 buffer
 
	if (error == KErrNone)
		{
		TInt error = iHwDevice->Start(EDevNullFunc, EDevInAndOutFlow);
		if (error == KErrNone)
			{
			iActiveScheduler->Start();
			error = iHwDeviceObserver->ErrorStatus();
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("error during convert %d" ), error );
				verdict = EFail;
				}
			if (iHwDeviceObserver->NumberOfBuffersObserved() != 1)
				{
				INFO_PRINTF1(_L("incorrect number of buffers"));
				verdict = EFail;
				}
			error = iHwDeviceObserver->WriteFile().Size(convertedFileSize);
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("problem with converted file error %d"), error);
				verdict = EFail;
				}
			if (convertedFileSize != KTest2To1SinkBufferSize)
				{
				INFO_PRINTF1(_L("converted file was not the expected size"));
				verdict = EFail;
				}
			//not possible to do a file comparison between source and converted file
			//as the conversion goes through a codec.
			}
		else
			{
			INFO_PRINTF2(_L("error on hw device start %d" ), error );
			verdict = EFail;
			}
		} 
	else
		{
		INFO_PRINTF2(_L("problem opening conversion files error %d" ), error );
		verdict = EInconclusive;
		}

	return verdict ;
	} 

