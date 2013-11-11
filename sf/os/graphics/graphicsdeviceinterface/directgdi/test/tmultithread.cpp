// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent 
 @test
*/

#include "tmultithread.h"
#include <graphics/directgdicontext.h>

const TUint KMinTestThreadHeapSize = 0x00000100;
const TUint KMaxTestThreadheapSize = 0x00100000;

CTMultiThread::CTMultiThread()
	{
	SetTestStepName(KTMultiThreadStep);
	}

CTMultiThread::~CTMultiThread()
	{
	iThread1.Close();
	iThread2.Close();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTMultiThread::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTMultiThread::doTestStepL()
	{
	if (iUseDirectGdi)
		{
		RunTestsL();
		// No framework OOM tests are run for the multithreaded tests as OOM testing only checks the 
		// heap in the current thread and these tests use multiple threads. Some heap checking is
		// performed within the tests themselves.
		}
	else
		{
		INFO_PRINTF1(_L("Test skipped under BitGDI.\n"));	
		}	
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTMultiThread::RunTestsL()
	{
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-MULTITHREAD-0001"));
	TestDirectGdiMultipleThreadIndependenceL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-MULTITHREAD-0002"));
	TestShareEGLImageBetweenSources_MultithreadedL();
	RecordTestResultL();
	}

/**
@SYMTestCaseID  
	GRAPHICS-DIRECTGDI-MULTITHREAD-0001
	
@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestCaseDesc  
	Ensure multi-threaded use of DirectGDI is truly independent.
	
@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions 
	The following sequence should be legal:
		1: (thread 1) initialise DirectGDI by calling ThreadOneStart()
		2: (thread 2) initialise DirectGDI by calling ThreadTwoStart()
		3: (thread 1) close DirectGDI
		4: (thread 2) render using DirectGDI
		
@SYMTestExpectedResults 
	There should be no panics.
*/
void CTMultiThread::TestDirectGdiMultipleThreadIndependenceL()
	{
	INFO_PRINTF1(_L("Multithread_MultipleThreadIndependence"));	
	// Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
   	CleanupClosePushL(sem);	
   	
   	//create threads
	User::LeaveIfError(iThread1.Create(KNameThreadOne, ThreadOneStart, KDefaultStackSize, KMinTestThreadHeapSize, KMaxTestThreadheapSize, NULL));
	User::LeaveIfError(iThread2.Create(KNameThreadTwo, ThreadTwoStart, KDefaultStackSize, KMinTestThreadHeapSize, KMaxTestThreadheapSize, NULL));
	
	//launch thread1
	TRequestStatus thread1Status;
	iThread1.Logon(thread1Status);
	iThread1.SetPriority(EPriorityLess);
	iThread1.Resume();
	sem.Wait();
	iThread1.Suspend();
	
	//launch thread2
	TRequestStatus thread2Status;
	iThread2.Logon(thread2Status);
	iThread2.SetPriority(EPriorityLess);
	iThread2.Resume();
	sem.Wait();
	iThread2.Suspend();
	
	//resume thread1
	iThread1.Resume();
	User::WaitForRequest(thread1Status);
	
	//resume thread2
	iThread2.Resume();
	User::WaitForRequest(thread2Status);	
	
	TESTNOERROR(iThread1.ExitReason());
	TESTNOERROR(iThread2.ExitReason());
			
	iThread1.Close();
	iThread2.Close();
	CleanupStack::PopAndDestroy(&sem);
	}

/**
Function for initializing DirectGdi, used by TestDirectGdiMultipleThreadIndependenceL().
@see TestDirectGdiMultipleThreadIndependenceL() 
@param aInfo Not used
@return KErrNone if successful, one of the system wide error codes otherwise
 */
TInt CTMultiThread::ThreadOneStart(TAny* /*aInfo*/)
	{	
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	__UHEAP_MARK;
	
	RSemaphore sem;
	TInt ret = sem.OpenGlobal(KMultiThreadSemaphore);
	if (ret!=KErrNone)
		{
		return ret;
		}		
	
	//initialize graphics resource driver
	ret = SgDriver::Open();
	if (ret!=KErrNone)
		{
		return ret;
		}		
	
	//initialise DirectGDI
	ret = CDirectGdiDriver::Open();
	if (ret!=KErrNone)
		{
		return ret;
		}		

	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	if(dgdiDriver == NULL)
		{
		return KErrGeneral;	
		}
	
	sem.Signal();

	//close DirectGDI
	dgdiDriver->Close();
	SgDriver::Close();	
	sem.Close();
	__UHEAP_MARKEND;
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		ret = KErrGeneral;  // Thread-owned handles not closed
		}
	return ret;
	}

/**
Function for initializing DirectGdi then activating a target and drawing on it,
used by TestDirectGdiMultipleThreadIndependenceL().
@see TestDirectGdiMultipleThreadIndependenceL()
@param aInfo Not used
@return KErrNone if successful, one of the system wide error codes otherwise 
 */
TInt CTMultiThread::ThreadTwoStart(TAny* /*aInfo*/)
	{
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	if (cleanupStack==NULL)
		{
		return KErrNoMemory;
		}
	
	RSemaphore sem;
	TInt ret = sem.OpenGlobal(KMultiThreadSemaphore);
	if (ret!=KErrNone)
		{
		return ret;
		}		
	
	//initialize graphics resource driver
	ret = SgDriver::Open();
	if (ret!=KErrNone)
		{
		return ret;
		}		
	
	//initialise DirectGDI
	ret = CDirectGdiDriver::Open();
	if (ret!=KErrNone)
		{
		return ret;
		}		

	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	if(dgdiDriver == NULL)
		{
		return KErrGeneral;	
		}
	
	sem.Signal();
	
	//render using DirectGDI
	CDirectGdiContext* gc = NULL;
	TRAPD(err, gc=CDirectGdiContext::NewL(*dgdiDriver));
	if(err != KErrNone)
		{
		return err;
		}
	
	RSgImage rsgImage;	
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = TSize (320, 240);
	imageInfo.iPixelFormat = EUidPixelFormatRGB_565;
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	ret = rsgImage.Create(imageInfo, NULL,0);
	if(ret != KErrNone)
		{
		return ret;
		}
	RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);	
	ret = dgdiImageTarget.Create(rsgImage);
	if(ret != KErrNone)
		{
		return ret;
		}
	gc->Activate(dgdiImageTarget);
	gc->SetPenColor(TRgb(100,100,100));
	gc->DrawRect(TRect(0,0,30,30));
	
	rsgImage.Close();
	dgdiImageTarget.Close();
	delete gc;
	dgdiDriver->Close();
	SgDriver::Close();
	delete cleanupStack;
	__UHEAP_MARKEND;
	sem.Close();	
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		ret = KErrGeneral;  // Thread-owned handles not closed
		}
	return ret;
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-MULTITHREAD-0002

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestStatus
	Implemented

@SYMTestCaseDesc
	Create two CDirectGdiImageSource objects from the same RSgImage, but in different threads.

@SYMTestActions	
	Test the use case where we:
	Create an RSgImage
	Create two CDirectGdiImageSource objects, one in the current thread, and one in a new thread.
	The CDirectGdiImageSource objects should share the EGL image created from the RSgImage in the 
	current thread as only one EGL image can be created per RSgImage per process.
	If the EGL image sharing is not working an error will occur when creating the
	second CDirectGdiImageSource object in the new thread.
*/
void CTMultiThread::TestShareEGLImageBetweenSources_MultithreadedL()
	{
	INFO_PRINTF1(_L("Multithread_ShareEGLImageBetweenSources"));
	
	TUidPixelFormat pixelFormat = EUidPixelFormatXRGB_8888;
	SetTargetL(pixelFormat);
	
	// Initialize graphics resource driver
	TInt res = SgDriver::Open();
	TESTNOERRORL(res);
	
	res = CDirectGdiDriver::Open();
	TESTNOERRORL(res);		
	
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);	
	CleanupClosePushL(*dgdiDriver);	
	
	// Create a context
	CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);
	TESTL(gc != NULL);	
	CleanupStack::PushL(gc);	
	
	// Create a CFbsBitmap	
	TSize patternSize(90,50);
	TRect rect(0,0,90,50);
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(pixelFormat, patternSize);
	TESTL(NULL != bitmap);
	CleanupStack::PushL(bitmap);	
	
	// Create an RSgImage from the CFbsBitmap
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = patternSize;
	imageInfo.iPixelFormat = pixelFormat;
	imageInfo.iUsage = ESgUsageDirectGdiSource;
	RSgImage sgImage;	
	res = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());	
	TESTNOERRORL(res);	
	CleanupClosePushL(sgImage);
	
	// Create a RDirectGdiDrawableSource from the RSgImage
	RDirectGdiDrawableSource dgdiImageSource(*dgdiDriver);	
	res = dgdiImageSource.Create(sgImage);		
	TESTNOERRORL(res);
	CleanupClosePushL(dgdiImageSource);	
	
	// Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
   	CleanupClosePushL(sem);	
   	
   	// Create the thread that will create a source from the RSgImage above   	
   	TSgDrawableId sgImageId = sgImage.Id();
	User::LeaveIfError(iThread1.Create(KNameThreadOne, ThreadEGLImageStart, KDefaultStackSize, KMinTestThreadHeapSize, KMaxTestThreadheapSize, &sgImageId));	
	
	// Launch the thread
	TRequestStatus threadStatus;
	iThread1.Logon(threadStatus);
	iThread1.SetPriority(EPriorityLess);
	iThread1.Resume();
	sem.Wait();
	iThread1.Suspend();
		
	// Resume the thread
	iThread1.Resume();
	User::WaitForRequest(threadStatus);
	
	TESTNOERROR(iThread1.ExitReason());	
			
	iThread1.Close();	
	SgDriver::Close();
	CleanupStack::PopAndDestroy(6, dgdiDriver);
	}

/**
Function use by TestShareEGLImageBetweenSources_MultithreadedL() when testing
creation of a target in a separate thread.
@see TestShareEGLImageBetweenSources_MultithreadedL()
@param aInfo Not used
@return KErrNone if successful, one of the system wide error codes otherwise
 */
TInt CTMultiThread::ThreadEGLImageStart(TAny* aInfo)
	{	
	TInt procHandles1 = 0;
	TInt threadHandles1 = 0;
	RThread().HandleCount(procHandles1, threadHandles1);
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	if (cleanupStack==NULL)
		{
		return KErrNoMemory;
		}		
	
	RSemaphore sem;
	TInt ret = sem.OpenGlobal(KMultiThreadSemaphore);
	if (ret!=KErrNone)
		return ret;
	
	// Initialize graphics resource driver
	ret = SgDriver::Open();
	if (ret!=KErrNone)
		return ret;
	
	// Initialise DirectGDI
	ret = CDirectGdiDriver::Open();
	if (ret!=KErrNone)
		return ret;

	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	if(dgdiDriver == NULL)
		{
		return KErrGeneral;	
		}
	
	TSgDrawableId* sgImageId = reinterpret_cast<TSgDrawableId*>(aInfo);
	RSgImage sgImageSource;
	ret = sgImageSource.Open(*sgImageId);
	if (ret!=KErrNone)
		return ret;
	
	// Create a RDirectGdiDrawableSource from the RSgImage
	RDirectGdiDrawableSource dgdiImageSource(*dgdiDriver);	
	ret = dgdiImageSource.Create(sgImageSource);		
	if (ret!=KErrNone)
		return ret;
	
	sem.Signal();
	
	// Render using DirectGDI
	CDirectGdiContext* gc = NULL;
	TRAPD(err, gc=CDirectGdiContext::NewL(*dgdiDriver));
	if(err != KErrNone)
		{
		return err;
		}
	
	RSgImage rsgImage;	
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = TSize (320, 240);
	imageInfo.iPixelFormat = EUidPixelFormatRGB_565;
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	ret = rsgImage.Create(imageInfo, NULL,0);
	if(ret != KErrNone)
		{
		return ret;
		}
	RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);	
	ret = dgdiImageTarget.Create(rsgImage);
	if(ret != KErrNone)
		{
		return ret;
		}
	gc->Activate(dgdiImageTarget);
	gc->SetPenColor(TRgb(100,100,100));
	gc->DrawRect(TRect(0,0,30,30));
	
	dgdiImageSource.Close();
	sgImageSource.Close();
	rsgImage.Close();
	dgdiImageTarget.Close();
	delete gc;
	dgdiDriver->Close();
	SgDriver::Close();
	delete cleanupStack;
	__UHEAP_MARKEND;
	sem.Close();	
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		ret = KErrGeneral;  // Thread-owned handles not closed
		}
	return ret;
	}

