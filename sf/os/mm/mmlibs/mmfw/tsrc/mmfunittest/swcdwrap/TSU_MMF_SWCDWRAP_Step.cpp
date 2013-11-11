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
#include "TSU_MMF_SWCDWRAP_Step.h"
#include "TSU_MMF_SWCDWRAP_TestDeviceImplementationUID.hrh"
#include "TSU_MMF_SWCDWRAP_HwDevObserver.h"

#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER 
#include "TSU_MMF_SWCDWRAP_PlayHwDevObserver.h"
#endif


/**
 * Pre-amble for sw codec test step
 * Sets __MM_HEAP_MARK for memory leak detection
 */
TVerdict CTestStep_MMF_SWCDWRAP::DoTestStepPreambleL()
	{
	 __MM_HEAP_MARK;
	 return EPass;
	}

/**
 * Post-amble for sw codec test step
 * Sets __MM_HEAP_MARKEND for memory leak detection
 */
TVerdict CTestStep_MMF_SWCDWRAP::DoTestStepPostambleL()
	{
	__MM_HEAP_MARKEND;
	return EPass;
	}



CTestStep_MMF_SWCDWRAP_U_0001::CTestStep_MMF_SWCDWRAP_U_0001()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0001-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0001::DoTestStepL( void )
/**
 * Use case: N/A
 * Instantiate a test CMMFSwCodeWrapper CMMFHwDevice
 * via uid.
 * @test Req. under test REQ1931.2
 */
	 {
	 TVerdict verdict = EPass;
	 CMMFHwDevice* hwDevice = NULL;
	 TUid hwDeviceUid;
	 hwDeviceUid.iUid = KMmfUidTSUSWCDWRAPTestDevice;

	 TRAPD(err, hwDevice = CMMFHwDevice::NewL(hwDeviceUid));

	 //check hw device has been created
	 if (err != KErrNone)
		 {
		 INFO_PRINTF2(_L("could not create hw device plugin error %d" ), err );
		 verdict = EFail;
		 }

	 if (!hwDevice)
		 {
		 verdict = EFail;
		 }
	 else
		 {
		 delete hwDevice;
		 }
	
	 return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0002::CTestStep_MMF_SWCDWRAP_U_0002()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0002-HP");
	}


TVerdict CTestStep_MMF_SWCDWRAP_U_0002::DoTestStepL( void )
/**
 * Use case: N/A
 * Instantiate an invalid CMMFHwDevice
 * @test Req. under test REQ1931.2
 */
	 {
	 TVerdict verdict = EPass;
	 CMMFHwDevice* hwDevice = NULL;
	 TUid hwDeviceUid;
	 hwDeviceUid.iUid = 0xFFFFFFFF;//invalid uid

	 TRAPD(err, hwDevice = CMMFHwDevice::NewL(hwDeviceUid));

	 //check hw device has not been created
	 if (err != KErrNotFound)
		 {
		 INFO_PRINTF2(_L("unexpected result %d" ), err );
		 verdict = EFail;
		 }

	 if (hwDevice)
		 {
		 verdict = EFail;
		 delete hwDevice;
		 }
	
	 return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0003::CTestStep_MMF_SWCDWRAP_U_0003()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0003-HP");
	}
 

TVerdict CTestStep_MMF_SWCDWRAP_U_0003::DoTestStepL( void )
/**
 * Use case: N/A
 * Initialize sw codec wrapper HwDevice checking that the
 * device drivers are loaded
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;
	CMMFHwDevice* hwDevice = NULL;
	TUid hwDeviceUid;
	hwDeviceUid.iUid = KMmfUidTSUSWCDWRAPTestDevice;

#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER //Adapter loads the drivers
	CActiveScheduler* activeScheduler;
	activeScheduler = CActiveScheduler::Current();
	if (!activeScheduler)
		{
		activeScheduler = new(ELeave)CActiveScheduler;
		}
	CActiveScheduler::Install(activeScheduler);
#endif

	TRAPD(err, hwDevice = CMMFHwDevice::NewL(hwDeviceUid));

	THwDeviceInitParams hwDeviceInitParams;

	CTestMMFHwDeviceObserver* hwDeviceObserver = new(ELeave)CTestMMFHwDeviceObserver(hwDevice);

	hwDeviceInitParams.iHwDeviceObserver = hwDeviceObserver;


#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER 
	CTestPlayMMFHwDeviceObserver* hwPlayDeviceObserver = new(ELeave)CTestPlayMMFHwDeviceObserver(hwDevice);
	hwDeviceInitParams.iHwDeviceObserver = hwPlayDeviceObserver;
#endif	

	if (!err)
		{
		//check device drivers are not loaded prior to initialization
		User::FreePhysicalDevice(KPddFileName);
		User::FreeLogicalDevice(KLddFileName);
		err = hwDevice->Init(hwDeviceInitParams);
		
#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
		if (!err)
			{
			TUid playCustomInterfaceUid;
			playCustomInterfaceUid.iUid = KMmfPlaySettingsCustomInterface;
			MPlayCustomInterface* playCustomInterface = 
			static_cast<MPlayCustomInterface*>(hwDevice->CustomInterface(playCustomInterfaceUid));
			playCustomInterface->SetVolume(50000);
			
			TTaskConfig taskConfig;
			taskConfig.iUid = KUidRefDevSoundTaskConfig;
			taskConfig.iRate = 8000;
			taskConfig.iStereoMode = ETaskMono;
			
			if (hwDevice->SetConfig(taskConfig) != KErrNone)
				{
				return EFail;
				}
			
			TInt KTestBufferSize = 0x2000;
			 _LIT(filename,"\\mm\\mmf\\testfiles\\swcdwrap\\TSU_MMF_SWCDWRAPpcm16_8m.raw");

			err = hwPlayDeviceObserver->FillBufferFromFileSync(filename, KTestBufferSize/2);
	
			err = hwDevice->Start(EDevDecode, EDevOutFlow);
			
			activeScheduler->Start();
			TInt error = hwPlayDeviceObserver->ErrorStatus();
			if ((error != KErrNone)&&(error != KErrUnderflow))
				{
				INFO_PRINTF2(_L("error during audio playback %d" ), err );
				verdict = EFail;
				}
			}
#endif
		if (!err)
			{
			TInt ret = User::LoadPhysicalDevice(KPddFileName);
			if (ret!=KErrAlreadyExists)
				verdict = EFail;
			ret = User::LoadLogicalDevice(KLddFileName);
			if (ret!=KErrAlreadyExists)
				verdict = EFail;
			}
		else
			{
			verdict = EFail;
			}
		}
	
	User::FreePhysicalDevice(KPddFileName);
	User::FreeLogicalDevice(KLddFileName);
	delete hwDevice;
	delete hwDeviceObserver;
#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER 
	delete activeScheduler;
	delete hwPlayDeviceObserver;
#endif
	return verdict ;
	} 


CTestStep_MMF_SWCDWRAP_U_0004::CTestStep_MMF_SWCDWRAP_U_0004()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0004-HP");
	}

TVerdict CTestStep_MMF_SWCDWRAP_U_0004::DoTestStepL( void )
/**
 * Use case: N/A
 * Initialize sw codec wrapper HwDevice using an invalid hwdevice observer
 * @test Req. under test REQ1931.2
 */
	{
	TVerdict verdict = EPass;
	CMMFHwDevice* hwDevice = NULL;
	TUid hwDeviceUid;
	hwDeviceUid.iUid = KMmfUidTSUSWCDWRAPTestDevice;

	TRAPD(err, hwDevice = CMMFHwDevice::NewL(hwDeviceUid));
	if (err)
		{
		return EInconclusive;
		}

	THwDeviceInitParams hwDeviceInitParams;

	hwDeviceInitParams.iHwDeviceObserver = NULL;

	if (!err)
		{
		err = hwDevice->Init(hwDeviceInitParams);
		if (err != KErrArgument)
			{
			verdict = EFail;
			}
		}
	delete hwDevice;
	return verdict;
	}
 


CTestStep_MMF_SWCDWRAP_U_0300::CTestStep_MMF_SWCDWRAP_U_0300()
/** Constructor
 */
	{
	iTestStepName = _L("MM-MMF-SWCDWRAP-U-0300-HP");
	}

/** 
 * Verify CMMFHwDevice OOM
 * Use case: N/A
 * @test Req. under test REQ1931.2
 */
TVerdict CTestStep_MMF_SWCDWRAP_U_0300::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 CMMFHwDevice* hwDevice = NULL;
	 TUid hwDeviceUid;
	 hwDeviceUid.iUid = KMmfUidTSUSWCDWRAPTestDevice;

	 TInt KMaxFailRate = 100000;
	 TBool complete = EFalse;
	 TBool nullAlloc = EFalse;
	 TInt failCount;

	 for (failCount=1; !complete && failCount<KMaxFailRate; ++failCount)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD(err, hwDevice = CMMFHwDevice::NewL(hwDeviceUid));
		if (err == KErrNone)
			{//need to check if next alloc fails
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				nullAlloc = ETrue;
				}
			else
				{
				User::Free(testAlloc);
				complete = ETrue;
				}
			//check the hw device has been created
			if (!hwDevice) 
				{
				verdict = EFail;
				complete = ETrue;
				}
			else 
				{
				delete hwDevice;
				hwDevice = NULL;
				}
			}
		else if (err != KErrNoMemory)
			{
			verdict = EFail; // bad error code
			complete = ETrue;
			INFO_PRINTF2(_L("OOM test error %d" ), err );
			}
		}

	 if (failCount >= KMaxFailRate-1)
		 {
		 INFO_PRINTF2(_L("fail count too high %d" ), failCount );
		 verdict = EFail;
		 }

	 if (!nullAlloc)
		 {
		 INFO_PRINTF1(_L("never alloced after no OOM "));
		 }


	 __UHEAP_RESET;

	 delete hwDevice;

	 return verdict ;
	 }
