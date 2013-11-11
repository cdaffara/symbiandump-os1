// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Surface manager multi-threaded test code
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tsurfacemanagermultithread.h"
#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>
#include <e32std.h>
#include <e32cmn.h>
#include <e32def.h>	  

static TSurfaceId globalSurfaceId;	
static TInt testResult;

typedef CTSurfaceManagerMultiThread::TInfo TInfo;

CTSurfaceManagerMultiThread::CTSurfaceManagerMultiThread(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	
	}

CTSurfaceManagerMultiThread::~CTSurfaceManagerMultiThread()
	{
	iSurfaceManager.Close();
	}

void CTSurfaceManagerMultiThread::RunTestCaseL(TInt aCurTestCase)
	{
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0078
@SYMPREQ				PREQ1879, PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test accessing surface in another thread without opening it
						Thread 1: Create Surface 
						Thread 2: Map Surface - KErrNone (still accessible)
						Thread 2: SurfaceInfo - KErrNone (still accessible)
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),MapSurface(), SurfaceInfo()
@SYMTestExpectedResults The surface can be successfully accessed in other threads without first opening it.
*/
	case 1:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0078"));
		TestAccessSurfaceWithoutOpeningL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0079
@SYMPREQ				PREQ1879, PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test accessing a surface created in the existing shared chunk in another thread without opening it.
						Thread 1: Create a Surface in the new chunk with valid creation attributes
						Thread 1: Map the surface in the current process to get the chunk handle
						Thread 1: Create a new surface in the existing chunk with valid creation attributes
						Thread 2: Map the second Surface – KErrNone (still accessible)
						Thread 2: SurfaceInfo – KErrNone (still accessible)
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),MapSurface(), SurfaceInfo()
@SYMTestExpectedResults MapSurface, SurfaceInfo return KerrNone
*/	
	case 2:
        ((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0079"));
		TestAccessSurfaceInExistingSharedChunkL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0080
@SYMPREQ				PREQ1879, PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing surface in another thread without opening it
						Thread 1: Create Surface 
						Thread 2: Close Surface - KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface()
@SYMTestExpectedResults The surface can be successfully closed in other threads without first opening it.
*/
	case 3:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0080"));
		TestCloseSurfaceWithoutOpeningL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0081
@SYMPREQ				PREQ1879, PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test accessing an surface which is created in another thread but closed in current thread 
						Thread 2: Create Surface
						Thread 1: Close Surface - KerrNone
						Thread 2: Map Surface - KErrArgument
						Thread 2: SurfaceInfo - KErrArgument
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults The surface cant be accessd even after it is closed in other threads.
*/
	case 4:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0081"));
		TestAccessSurfaceClosedThreadL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0067
@SYMPREQ				PREQ1879, PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing surface created in the existing chunk in another thread without opening it 
						Thread 1: Create a Surface in a new shared chunk with valid attributes
						Thread 1: Map the surface in the process to get the shared chunk handle
						Thread 1: Create a new surface in the existing shared chunk
						Thread 2: Close the second surface – KErrNone (the second surface is deleted)
						Thread 2: Call SurfaceInfo to the first surface and it returns KErrNone, but it returns KErrArgument to the second surface.
						Thread 2: Close the first surface and check it returns KErrNone (the first surface is deleted).
						Thread 1: Call SurfaceInfo to both surfaces and return KErrArgument
						Thread 1: Check the chunk is still accessible by calling RChunk::Base() 
						Thread 1: Close the chunk handle  
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface()
@SYMTestExpectedResults CloseSurface returns KErrNone. The chunk handle is not closed even when both surfaces are deleted.
*/		
	case 5:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0067"));
		TestCloseSurfaceExistingSharedChunkL();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0068
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing an surface which is already closed in other thread return KErrArgument 
						Thread 2: Create Surface
						Thread 1: Close Surface - KerrNone
						Thread 2: Close Surface - KErrArgument
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface()
@SYMTestExpectedResults The surface cant be closed again.
*/
	case 6:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0068"));
		TestCloseSurfaceClosedThreadL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0069
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test accessing an surface which is already opened and closed in another thread, given that the process still owns the surface 
						Thread 1: Create Surface
						Thread 2: Open Surface -- KErrNone
						Thread 2: Close Surface -- KErrNone
						Thread 1: Map Surface - KErrNone
						Thread 1: SurfaceInfo - KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), MapSurface(), SurfaceInfo, OpenSurface()
@SYMTestExpectedResults The surface is still accessible in the thread residing the owning process
*/
	case 7:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0069"));
		TestAccessSurfaceOpenedClosedThreadL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0070
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing an surface which is already opened and closed in another thread, given that the process still owns the surface
    					Thread 1: Create Surface
						Thread 2: Open Surface
						Thread 2: Close Surface
						Thread 1: Close Surface - KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults The surface is still closible in the thread residing in the owning process
*/
	case 8:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0070"));
		TestCloseSurfaceOpenedClosedThreadL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0071
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test accessing surface in another thread when the opening thread is killed   
						Thread 1: Create Surface 
						Thread 2: Open Surface
						Thread 2: Kill the thread
						Thread 1: Map Surface - KErrNone
						Thread 1: Surface Info -- KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), MapSurface(), SurfaceInfo(), OpenSurface()
@SYMTestExpectedResults The surface is accessible when the opening thread is killed
*/
	case 9:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0071"));
		TestAccessSurfaceOpenedKilledThreadL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0072
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing surface in another thread when the opening thread is killed   
						Thread 1: Create Surface 
						Thread 2: Open Surface
						Thread 2: Kill the thread
						Thread 1: Close Surface - KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults The surface is closible when the opening thread is killed
*/
	case 10:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0072"));
		TestCloseSurfaceOpenedKilledThreadL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0073
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test accessing surface in another thread when the creating thread is killed and it is closed in other threads
						Thread 2: Create Surface
						Thread 1: Close Surface
						Thread 2: Kill Thread
						Thread 1: Map Surface - KErrArgument
						Thread 1: Surface Info - KErrArgument
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), MapSurface(), SurfaceInfo(),
@SYMTestExpectedResults The surface is accessible when the creating thread is killed and it is closed in other threads
*/
	case 11:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0073"));
		TestAccessSurfaceCreateKilledThreadL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0074
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test accessing surface in another thread when the creating thread is killed and it is closed in other threads
						Thread 2: Create Surface
						Thread 1: Close Surface
						Thread 2: Kill Thread
						Thread 1: Close Surface - KErrArgument
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface()
@SYMTestExpectedResults The surface is accessible when the creating thread is killed and it is closed in other threads
*/
	case 12:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0074"));
		TestCloseSurfaceCreateKilledThreadL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0075
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing on one thread doesn't prevent opening on other Threads, provided one Thread still owns surface
    					Thread 1: Create Surface
						Thread 2: Open Surface
						Thread 2: Close Surface
						Thread 3: Open Surface - KerrNone
						Thread 3: Map Surface - KerrNone
						Thread 3: SurfaceInfo -- KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), MapSurface(), SurfaceInfo(),
@SYMTestExpectedResults It is expected that closing on one thread doesn't prevent opening on other Threads, provided one Thread still owns surface
*/
	case 13:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0075"));
		TestAccessSurfaceThreeThreadsL();
		break;	
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0076
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test surface can be accessed from third thread when creating thread dies and second thread closes
    					Thread 2: Create Surface
						Thread 1: Open Surface
						Thread 3: Open Surface
						Thread 1: Close Surface
						Thread 2: Kill Thread
 					 	Thread 3: Close Surface
						Thread 3: Map Surface - KerrNone
						Thread 3: SurfaceInfo -- KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), MapSurface(), SurfaceInfo(), OpenSurface()
@SYMTestExpectedResults It is expected that surface can be accessed from third thread when creating thread dies and second thread closes
*/
	case 14:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0076"));
		TestAccessSurfaceDieCloseOtherThreadsL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0077
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test that the surface can be opened from the third Thread after creating thread dies and second and third thread close the surface
   						Thread 2: Create Surface
						Thread 1: Open Surface
						Thread 3: Open Surface
						Thread 1: Close Surface
						Thread 2: Kill Thread
 					    Thread 3: Close Surface
						Thread 3: Open Surface - KerrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults It is expected that the surface can be opened from the third Thread after creating thread dies and second and third thread close the surface
*/
	case 15:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0077"));
		TestOpenSurfaceDieCloseOtherThreadsL();
		break;
	default:
		((CTSurfaceManagerMultiThreadStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTSurfaceManagerMultiThreadStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTSurfaceManagerMultiThreadStep*)iStep)->RecordTestResultL();
	// Handle check
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		User::Leave(KErrGeneral);  // Thread-owned handles not closed
		}
 	}

void CTSurfaceManagerMultiThread::CloseSurfaceWhenLeave(TAny* aInfo) 
	{
	TInt err1 = ((TInfo*)aInfo)->iSurfaceManager.CloseSurface(((TInfo*)aInfo)->iSurfaceId);
	TInt err2 = ((TInfo*)aInfo)->iSurfaceManager.CloseSurface(((TInfo*)aInfo)->iSurfaceIdNew);
	if (err1!=KErrNone || err2!=KErrNone)
	   	RDebug::Print(_L("Error closing surfaces err1= %d, err2= %d\n"),err1,err2);	
	}

void CTSurfaceManagerMultiThread::TestAccessSurfaceWithoutOpeningL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iMappable = ETrue;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
    // Logging info   
    INFO_PRINTF1(_L("Test accessing surface in another thread without opening it\r\n"));
    // Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
	CleanupClosePushL(sem);	
	
	// Create a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iSurfaceIdNew = TSurfaceId::CreateNullId();
	iInfo.iThreadTestCase = EMapSurfaceInfo;		
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));
		
    RThread thread1;
    _LIT(KNameThreadSecond1, "Test_SurfaceManager_ThreadSecond1");
	User::LeaveIfError(thread1.Create(KNameThreadSecond1,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
   	CleanupStack::Pop();
    // Launch second thread
	TRequestStatus statusThreadSecond;
	thread1.Logon(statusThreadSecond);
	thread1.SetPriority(EPriorityLess);
	thread1.Resume();	
	testResult = EAllZero;
	// Passes control to the second process
	sem.Wait();
	thread1.Suspend();

	// Test results in second thread
	TEST (testResult = (EMapSurfaceTestPassed | ESurfaceInfoTestPassed));

	// Pass control off to the second process again
	thread1.Resume();

	// Wait for a second for the second thread to terminate
	User::WaitForRequest(statusThreadSecond);

	//Close the handles
    CleanupStack::PopAndDestroy(1, &sem);  //invoke surface manager to close the surface also close the handle to RSemaphore
    thread1.Close();
    iSurfaceManager.Close();
	}

void CTSurfaceManagerMultiThread::TestAccessSurfaceInExistingSharedChunkL()
    {
    // Logging info   
    INFO_PRINTF1(_L("Test accessing a surface created in the existing shared chunk in another thread without opening it\r\n"));

    User::LeaveIfError(iSurfaceManager.Open());
    
    // Setup attributes 
    
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
    RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

    attributes.iSize = TSize(100,100);
    attributes.iBuffers = 1;                // number of buffers in the surface
    attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;     // 2bpp
    attributes.iStride = 400;               // Number of bytes between start of one line and start of next
    attributes.iOffsetToFirstBuffer = 0;    // way of reserving space before the surface pixel data
    attributes.iAlignment = 2;          // alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

    RSurfaceManager::THintPair hints[2];    // two hint pairs specified
    attributes.iHintCount = 2;
    attributes.iSurfaceHints = hints;
    hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
    hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
    
    attributes.iContiguous = ETrue;
    attributes.iCacheAttrib = RSurfaceManager::ECached;
    attributes.iOffsetBetweenBuffers = 0;
    attributes.iMappable = ETrue;

    // Test create surface doesn't return an error
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
    
    //Map the surface in the current processs
    RChunk handle;
    TEST(KErrNone == iSurfaceManager.MapSurface(iSurfaceId,handle));
    
    // Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
    RSurfaceManager::TSurfaceCreationAttributesBuf buff;
    RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

    attributesNew.iSize = TSize(480,16);
    attributesNew.iBuffers = 2;             // number of buffers in the surface
    attributesNew.iPixelFormat = EUidPixelFormatYUV_422Reversed;        // 2bpp
    attributesNew.iStride = 1013;               // Number of bytes between start of one line and start of next
    attributesNew.iOffsetToFirstBuffer = 0; // way of reserving space before the surface pixel data
    
    attributesNew.iHintCount = 1;
    attributesNew.iSurfaceHints = hints;
    hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
    
    attributesNew.iAlignment = 2;
    attributesNew.iOffsetBetweenBuffers = 0;
    attributes.iMappable = ETrue;
    
    // Test create surface doesn't return an error
    // For the time being KErrArgument will be returned as the core codes are
    // not ready to check the passed in shared chunk handle.
    TEST(KErrNone == iSurfaceManager.CreateSurface(buff, iSurfaceIdNew, handle));


    // Create a semaphore
    RSemaphore sem;
    TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
    CleanupClosePushL(sem); 
    
    // Create a second thread in the current process and opens the handle
    iInfo.iSurfaceManager = iSurfaceManager;
    iInfo.iSurfaceId = iSurfaceIdNew;
    iInfo.iSurfaceIdNew = iSurfaceId;
    iInfo.iThreadTestCase = EMapSurfaceInfo;    
    // Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));
    
    RThread thread2;
    _LIT(KNameThreadSecond2, "Test_SurfaceManager_ThreadSecond2");
    User::LeaveIfError(thread2.Create(KNameThreadSecond2,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
 
    CleanupStack::Pop();
    // Launch second thread
    TRequestStatus statusThreadSecond;
    thread2.Logon(statusThreadSecond);
    thread2.SetPriority(EPriorityLess);
    thread2.Resume();   
    testResult = EAllZero;
    // Passes control to the second process
    sem.Wait();
    thread2.Suspend();

    // Test results in second thread
    TEST (testResult = (EMapSurfaceTestPassed | ESurfaceInfoTestPassed));

    // Pass control off to the second process again
    thread2.Resume();

    // Wait for a second for the second thread to terminate
    User::WaitForRequest(statusThreadSecond);
    
    //Close the handles
    CleanupStack::PopAndDestroy(1, &sem);  //invoke surface manager to close the surface also close the handle to RSemaphore
    handle.Close();
    thread2.Close();
    iSurfaceManager.Close();
    
    }

void CTSurfaceManagerMultiThread::TestCloseSurfaceWithoutOpeningL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iMappable = ETrue;
	
	// Test create surface doesn't return an error
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
   
    INFO_PRINTF1(_L("Test closing surface in another thread without opening it\r\n"));
	
	// Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
   	CleanupClosePushL(sem);	

  	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = ECloseSurfaces;	
   	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread3;
    _LIT(KNameThreadSecond3, "Test_SurfaceManager_ThreadSecond3");
	User::LeaveIfError(thread3.Create(KNameThreadSecond3,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
    CleanupStack::Pop();
    // Launch second thread to close the surface
	TRequestStatus statusThreadSecond;
	thread3.Logon(statusThreadSecond);
	thread3.SetPriority(EPriorityLess);
	thread3.Resume();	
	testResult = EAllZero;
	// Passes control to the second thread
	sem.Wait();
	thread3.Suspend();

	// Test the results in the second thread
	TEST(testResult == ECloseSurfaceTestPassed);
	
	thread3.Resume();
	// Wait for a second for the second process to terminate
	User::WaitForRequest(statusThreadSecond);
	
	//Close the handle
    CleanupStack::PopAndDestroy(1, &sem);
    thread3.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManagerMultiThread::TestAccessSurfaceClosedThreadL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    // Logging info
    INFO_PRINTF1(_L("Test accessing an surface which is created in another thread but closed in current thread\r\n"));
    
    // Create a semaphore
	RSemaphore sem;
	RSemaphore semMain;
	
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
    CleanupClosePushL(sem);	
    TEST(KErrNone == semMain.CreateGlobal(KMainThreadSemaphore, 0));
    CleanupClosePushL(semMain);	

	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = ECreateSurfaceMapInfo;	
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread4;
    _LIT(KNameThreadSecond4, "Test_SurfaceManager_ThreadSecond4");
	User::LeaveIfError(thread4.Create(KNameThreadSecond4,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
    CleanupStack::Pop();
    // Launch the second trhead
	TRequestStatus statusThreadSecond;
	thread4.Logon(statusThreadSecond);
	thread4.SetPriority(EPriorityLess);
	thread4.Resume();	
    
    testResult = EAllZero;
  	// Passes control to the second process
    sem.Wait(); 
	// Test creating surface in second thread
   	TEST(testResult == ECreateSurfaceTestPassed);
   	// Reset testResult
    testResult = EAllZero;
    // Test closing surface
    iSurfaceId = globalSurfaceId;
	TEST(KErrNone == iSurfaceManager.CloseSurface(globalSurfaceId));
	semMain.Signal();
    // Wait the second thread to terminate
    User::WaitForRequest(statusThreadSecond);

	// Test mapsurface and surfaceinfo executed in the second thread
	TEST(testResult == (EMapSurfaceTestFailed|ESurfaceInfoTestFailed));
	// Close all the handles
    CleanupStack::PopAndDestroy(2, &sem);
    thread4.Close();
	iSurfaceManager.Close();

	}

void CTSurfaceManagerMultiThread::TestCloseSurfaceExistingSharedChunkL()
    {
    // Logging info   
    INFO_PRINTF1(_L("Test closing surface created in the existing chunk in another thread without opening it\r\n"));
 
    User::LeaveIfError(iSurfaceManager.Open());
    // Setup attributes 
    
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
    RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

    attributes.iSize = TSize(100,100);
    attributes.iBuffers = 1;                // number of buffers in the surface
    attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;     // 2bpp
    attributes.iStride = 400;               // Number of bytes between start of one line and start of next
    attributes.iOffsetToFirstBuffer = 0;    // way of reserving space before the surface pixel data
    attributes.iAlignment = 2;          // alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

    RSurfaceManager::THintPair hints[2];    // two hint pairs specified
    attributes.iHintCount = 2;
    attributes.iSurfaceHints = hints;
    hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
    hints[1].Set(TUid::Uid(0x237755), 50, ETrue);

    attributes.iContiguous = ETrue;
    attributes.iCacheAttrib = RSurfaceManager::ECached;
    attributes.iOffsetBetweenBuffers = 0;
    attributes.iMappable = ETrue;
    
    // Test create surface doesn't return an error
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
    
    //Map the surface in the current processs
    RChunk handle;
    TEST(KErrNone == iSurfaceManager.MapSurface(iSurfaceId,handle));
    
    // Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
    RSurfaceManager::TSurfaceCreationAttributesBuf buff;
    RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

    attributesNew.iSize = TSize(480,16);
    attributesNew.iBuffers = 2;             // number of buffers in the surface
    attributesNew.iPixelFormat = EUidPixelFormatYUV_422Reversed;        // 2bpp
    attributesNew.iStride = 1013;               // Number of bytes between start of one line and start of next
    attributesNew.iOffsetToFirstBuffer = 0; // way of reserving space before the surface pixel data
    
    attributesNew.iHintCount = 1;
    attributesNew.iSurfaceHints = hints;
    hints[0].Set(TUid::Uid(0x124545), 50, EFalse);

    attributesNew.iAlignment = 2;
    attributesNew.iOffsetBetweenBuffers = 0;
    attributesNew.iMappable = ETrue;

    // Test create surface doesn't return an error
    // For the time being KErrArgument will be returned as the core codes are
    // not ready to check the passed in shared chunk handle.
    TEST(KErrNone == iSurfaceManager.CreateSurface(buff, iSurfaceIdNew, handle));

    // Create a semaphore
    RSemaphore sem;
    TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
    CleanupClosePushL(sem); 
    
    // Create a second thread in the current process and opens the handle
    iInfo.iSurfaceManager = iSurfaceManager;
    iInfo.iSurfaceId = iSurfaceId;
    iInfo.iSurfaceIdNew = iSurfaceIdNew;
    iInfo.iThreadTestCase = ECloseBothSurfaces; 
    // Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));
        
    RThread thread5;
    _LIT(KNameThreadSecond5, "Test_SurfaceManager_ThreadSecond5");
    User::LeaveIfError(thread5.Create(KNameThreadSecond5,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
    CleanupStack::Pop();
    // Launch second thread
    TRequestStatus statusThreadSecond;
    thread5.Logon(statusThreadSecond);
    thread5.SetPriority(EPriorityLess);
    thread5.Resume();   
    testResult = EAllZero;
    // Passes control to the second process
    sem.Wait();
    thread5.Suspend();

    // Test results in second thread
    TEST (testResult = (ECloseSurfaceTestPassed | ESurfaceInfoTestPassed | ESurfaceInfoTestPassed2));
    RSurfaceManager::TInfoBuf infoBuf;
    TEST (KErrArgument == iSurfaceManager.SurfaceInfo(iSurfaceId, infoBuf));
    TEST (KErrArgument == iSurfaceManager.SurfaceInfo(iSurfaceIdNew, infoBuf));

    // Pass control off to the second process again
    thread5.Resume();

    // Wait for a second for the second thread to terminate
    User::WaitForRequest(statusThreadSecond);

    CleanupStack::PopAndDestroy(1, &sem);  //invoke surface manager to close the surface also close the handle to RSemaphore
    handle.Close();
    thread5.Close();
    iSurfaceManager.Close();
    }

void CTSurfaceManagerMultiThread::TestCloseSurfaceClosedThreadL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
    // Logging info
    INFO_PRINTF1(_L("Test closing an surface which is already closed in other thread return KErrArgument\r\n"));
    // Create a semaphore
	RSemaphore sem;
	RSemaphore semMain;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
    CleanupClosePushL(sem);	
    TEST(KErrNone == semMain.CreateGlobal(KMainThreadSemaphore, 0));
    CleanupClosePushL(semMain);	

 	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = ECreateSurfaceClose;	
	
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread6;
    _LIT(KNameThreadSecond6, "Test_SurfaceManager_ThreadSecond6");
	User::LeaveIfError(thread6.Create(KNameThreadSecond6,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
    CleanupStack::Pop();
    // Launch second thread
	TRequestStatus statusThreadSecond;
	thread6.Logon(statusThreadSecond);
	thread6.SetPriority(EPriorityLess);
	thread6.Resume();	
	testResult = EAllZero;
  	// Passes control to the second thread
    sem.Wait(); 
    // Test creating surface in second thread
   	TEST(testResult == ECreateSurfaceTestPassed);
   	testResult = EAllZero;
    // Test closing surface
    iSurfaceId = globalSurfaceId;
	TEST(KErrNone == iSurfaceManager.CloseSurface(globalSurfaceId));
    	
	semMain.Signal();
	// Wait for a second for the second process to terminate
    User::WaitForRequest(statusThreadSecond);

    TEST(testResult == ECloseSurfaceTestFailed);
    // Close all the handles
    CleanupStack::PopAndDestroy(2, &sem);
    thread6.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManagerMultiThread::TestAccessSurfaceOpenedClosedThreadL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
    // Logging info
    INFO_PRINTF1(_L("Test accessing an surface which is already opened and closed in another thread, given that the process still owns the surface\r\n"));
 	// Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
    CleanupClosePushL(sem);	

	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = EOpenCloseSurface;	
 	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));
	
    RThread thread7;
    _LIT(KNameThreadSecond7, "Test_SurfaceManager_ThreadSecond7");
	User::LeaveIfError(thread7.Create(KNameThreadSecond7,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
   	CleanupStack::Pop();
   	
    // Launch the second thread
	TRequestStatus statusThreadSecond;
	thread7.Logon(statusThreadSecond);
	thread7.SetPriority(EPriorityLess);
	thread7.Resume();
	testResult = EAllZero;
	sem.Wait();	  
	
	thread7.Suspend();
	// Test open and close surface in the second thread
	TEST(testResult == (EOpenSurfaceTestPassed|ECloseSurfaceTestPassed));
	
	RChunk handle;
	// To prove killing threads doesnt affect the ref count...
	TEST(KErrNone == iSurfaceManager.MapSurface(iSurfaceId, handle));
	handle.Close();
	
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone ==iSurfaceManager.SurfaceInfo(iSurfaceId, infoBuf));
	
	thread7.Resume();
	
	// Wait for a second for the second process to terminate
    User::WaitForRequest(statusThreadSecond);

	// Close all the handles
    CleanupStack::PopAndDestroy(1, &sem);
    thread7.Close();
	iSurfaceManager.Close();
	}	

void CTSurfaceManagerMultiThread::TestCloseSurfaceOpenedClosedThreadL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	// Test create surface doesn't return an error
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
    
    INFO_PRINTF1(_L("Test closing an surface which is already opened and closed in another thread, given that the process still owns the surface\r\n"));
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
   	CleanupClosePushL(sem);

	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = EOpenCloseSurface;	
 
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread8;
    _LIT(KNameThreadSecond8, "Test_SurfaceManager_ThreadSecond8");
	User::LeaveIfError(thread8.Create(KNameThreadSecond8,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
    CleanupStack::Pop();
	TRequestStatus statusThreadSecond;
	thread8.Logon(statusThreadSecond);
	thread8.SetPriority(EPriorityLess);
	thread8.Resume();
	testResult = EAllZero;
	sem.Wait();	
	thread8.Suspend();
	// Test open and close surface in the second thread
	TEST(testResult == (EOpenSurfaceTestPassed|ECloseSurfaceTestPassed));

	thread8.Resume();
	// Wait for a second for the second process to terminate
    User::WaitForRequest(statusThreadSecond);

	// Close all the handles 
    CleanupStack::PopAndDestroy(1, &sem);
    thread8.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManagerMultiThread::TestAccessSurfaceOpenedKilledThreadL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	// Test create surface doesn't return an error
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
    
    INFO_PRINTF1(_L("Test accessing surface in another thread when the opening thread is killed\r\n"));
    // Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
	CleanupClosePushL(sem);

	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = EOpenKillSurface;	
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread9;
    _LIT(KNameThreadSecond9, "Test_SurfaceManager_ThreadSecond9");
	User::LeaveIfError(thread9.Create(KNameThreadSecond9,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
	CleanupStack::Pop();
	// Launch the second thraed
	TRequestStatus statusThreadSecond;
	thread9.Logon(statusThreadSecond);
	thread9.SetPriority(EPriorityLess);
	thread9.Resume();
	// Waiting the second thread to terminate
    User::WaitForRequest(statusThreadSecond);

   	RChunk handle;
	// Test the surface is still accessible
	TEST(KErrNone == iSurfaceManager.MapSurface(iSurfaceId, handle));
	handle.Close();
	
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone ==iSurfaceManager.SurfaceInfo(iSurfaceId, infoBuf));
	
    // Close all the handles
    CleanupStack::PopAndDestroy(1, &sem);
    thread9.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManagerMultiThread::TestCloseSurfaceOpenedKilledThreadL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
	CleanupClosePushL(sem);
	
	// Test create surface doesn't return an error
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
    // Logging info
    INFO_PRINTF1(_L("Test closing surface in another thread when the opening thread is killed\r\n"));
  	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = EOpenKillSurface;	

	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread10;
    _LIT(KNameThreadSecond10, "Test_SurfaceManager_ThreadSecond10");
	User::LeaveIfError(thread10.Create(KNameThreadSecond10,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
   	CleanupStack::Pop();
	// Launch the second thread
	TRequestStatus statusThreadSecond;
	thread10.Logon(statusThreadSecond);
	thread10.SetPriority(EPriorityLess);
	thread10.Resume();
	// Waiting the second thread to terminate
    User::WaitForRequest(statusThreadSecond);

    // Test closing surface
	TEST(KErrNone ==iSurfaceManager.CloseSurface(iSurfaceId));
	
	// Close all the handles
	CleanupStack::PopAndDestroy(1, &sem);
	thread10.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManagerMultiThread::TestAccessSurfaceCreateKilledThreadL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
    // Logging info
    INFO_PRINTF1(_L("Test accessing surface in another thread when the creating thread is killed\r\n"));
    // Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
	CleanupClosePushL(sem);

  	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = ECreateKillSurface;	

	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread11;
    _LIT(KNameThreadSecond11, "Test_SurfaceManager_ThreadSecond11");
	User::LeaveIfError(thread11.Create(KNameThreadSecond11,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
   	CleanupStack::Pop();
    // Launch the second thread
	TRequestStatus statusThreadSecond;
	thread11.Logon(statusThreadSecond);
	thread11.SetPriority(EPriorityLess);
	thread11.Resume();
	testResult = EAllZero;
	sem.Wait();	
	// Test creating surface in the second thread
	TEST(testResult = ECreateSurfaceTestPassed);
    // Test closing surface
   	TEST(KErrNone == iSurfaceManager.CloseSurface(globalSurfaceId));
    // Wait the second thread to terminate
    User::WaitForRequest(statusThreadSecond);

    // Test the surface is not accessible any more
   	RChunk handle;
	TEST(KErrArgument == iSurfaceManager.MapSurface(globalSurfaceId, handle));
	handle.Close();
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrArgument ==iSurfaceManager.SurfaceInfo(globalSurfaceId, infoBuf));
 	// Close all the handles
	CleanupStack::PopAndDestroy(1, &sem);
	thread11.Close();
	iSurfaceManager.Close();
    }
	
/**
	210 Test closing surface in another thread when the creating thread is killed
	Thread 2: Create Surface 
	Thread 1: Close Surface
	Thread 2: Kill the thread
	Thread 1: Close Surface - KErrArgument
*/
void CTSurfaceManagerMultiThread::TestCloseSurfaceCreateKilledThreadL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
		
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

    // Logging info
    INFO_PRINTF1(_L("Test closing surface in another thread when the creating thread is killed\r\n"));
    // Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
	CleanupClosePushL(sem);

  	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = ECreateKillSurface;	

	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread12;
    _LIT(KNameThreadSecond12, "Test_SurfaceManager_ThreadSecond12");
	User::LeaveIfError(thread12.Create(KNameThreadSecond12,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
   	CleanupStack::Pop();
    // Launch the second thread
	TRequestStatus statusThreadSecond;
	thread12.Logon(statusThreadSecond);
	thread12.SetPriority(EPriorityLess);
	thread12.Resume();
	sem.Wait();	
	// Test creating surface in the second thread
	TEST(testResult = ECreateSurfaceTestPassed);
    // Test closing surface
   	TEST(KErrNone == iSurfaceManager.CloseSurface(globalSurfaceId));
    // Wait the second thread to terminate
    User::WaitForRequest(statusThreadSecond);
     
	// Test the surface is removed and not closable   	
	TEST(KErrArgument ==iSurfaceManager.CloseSurface(globalSurfaceId));
    // Close the handles
   	CleanupStack::PopAndDestroy(1, &sem);
   	thread12.Close();
	iSurfaceManager.Close();
	}
	
void CTSurfaceManagerMultiThread::TestAccessSurfaceThreeThreadsL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Test create surface doesn't return an error
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, iSurfaceId));
    // Logging info
    INFO_PRINTF1(_L("Test closing on one thread doesn't prevent opening on other Threads, provided one Thread still owns surface\r\n"));
    // Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore,0));
	CleanupClosePushL(sem);

   	// Creates a second thread in the current process and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = EOpenCloseSurfaceMultiThread;	
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread13;
    _LIT(KNameThreadSecond13, "Test_SurfaceManager_ThreadSecond13");
	User::LeaveIfError(thread13.Create(KNameThreadSecond13,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
   	CleanupStack::Pop();
    // Launch the second thread
	TRequestStatus statusThreadSecond;
	thread13.Logon(statusThreadSecond);
	thread13.SetPriority(EPriorityLess);
	thread13.Resume();
	testResult = EAllZero;
	sem.Wait();	
	TEST(testResult == EOpenSurfaceTestPassed);
	testResult = EAllZero;

	// Create a third thread in the same process and opens the handle
	iInfo2 = iInfo;
	iInfo2.iThreadTestCase = EOpenMapSurfaceInfoMultiThread;	
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    RThread thread13_2;
    _LIT(KNameThreadSecond13_2, "Test_SurfaceManager_ThreadSecond13_2");
	User::LeaveIfError(thread13_2.Create(KNameThreadSecond13_2,ThreadThirdStart, KDefaultStackSize, &User::Heap(), &iInfo2));
	CleanupStack::Pop();
	// Launch the third thread
	TRequestStatus statusThreadThird;
	thread13_2.Logon(statusThreadThird);
	thread13_2.SetPriority(EPriorityLess);
	thread13_2.Resume();
	sem.Wait();
	TEST(testResult == EOpenSurfaceTestPassed);
	testResult = EAllZero;
	thread13.Resume();
    User::WaitForRequest(statusThreadSecond);

	TEST(testResult == ECloseSurfaceTestPassed);
	testResult = EAllZero;

	thread13_2.Resume();
    User::WaitForRequest(statusThreadThird);

	TEST(testResult == (EMapSurfaceTestPassed|ESurfaceInfoTestPassed));

	// Close all the handles
	CleanupStack::PopAndDestroy(1, &sem);

	thread13.Close();
	thread13_2.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManagerMultiThread::TestAccessSurfaceDieCloseOtherThreadsL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
    // Logging info
    INFO_PRINTF1(_L("Test surface can be accessed from third thread when creating thread dies and second thread closes\r\n"));
    // Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
	CleanupClosePushL(sem);

	// Create a second thread and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = ECreateKillSurface;	

	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));

    RThread thread14;
    _LIT(KNameThreadSecond14, "Test_SurfaceManager_ThreadSecond14");
	User::LeaveIfError(thread14.Create(KNameThreadSecond14,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
   	CleanupStack::Pop();
	// Launch the second thread
	TRequestStatus statusThreadSecond;
	thread14.Logon(statusThreadSecond);
	thread14.SetPriority(EPriorityLess);
	thread14.Resume();
	testResult = EAllZero;
	sem.Wait();	
	//suspend the 2nd thread
	thread14.Suspend(); 
	
	TEST(testResult == ECreateSurfaceTestPassed);
	testResult = EAllZero;
	
	TEST(KErrNone == iSurfaceManager.OpenSurface(globalSurfaceId));

    // Create a third thread in the current process and opens the handle 
	iInfo2.iSurfaceManager = iSurfaceManager;
	iInfo2.iSurfaceId = globalSurfaceId;
	iInfo2.iSurfaceIdNew = iSurfaceId;
	iInfo2.iThreadTestCase = EOpenCloseMapSurfaceInfoMultiThread;	
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    RThread thread14_2;
    _LIT(KNameThreadSecond14_2, "Test_SurfaceManager_ThreadSecond14_2");
	User::LeaveIfError(thread14_2.Create(KNameThreadSecond14_2,ThreadThirdStart, KDefaultStackSize, &User::Heap(), &iInfo2));
   	CleanupStack::Pop();
    // Launch the third thread
	TRequestStatus statusThreadThird;
	thread14_2.Logon(statusThreadThird);
	thread14_2.SetPriority(EPriorityLess);
	thread14_2.Resume();
	sem.Wait();
	// Test opening the surface in the second thread
	TEST(testResult == EOpenSurfaceTestPassed);
	testResult = EAllZero;
    // Test closing the surface in the main thread
	TEST(KErrNone == iSurfaceManager.CloseSurface(globalSurfaceId));

	thread14.Resume();
    User::WaitForRequest(statusThreadSecond);

    thread14_2.Resume();
    User::WaitForRequest(statusThreadThird);

	// Test the execution results in the third thread
	TEST(testResult == (ECloseSurfaceTestPassed|EMapSurfaceTestPassed|ESurfaceInfoTestPassed));	

    // Close all the handles
    CleanupStack::PopAndDestroy(1, &sem);
    thread14.Close();
    thread14_2.Close();
	iSurfaceManager.Close();

	}

void CTSurfaceManagerMultiThread::TestOpenSurfaceDieCloseOtherThreadsL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
    
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
		
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

    // Logging info
    INFO_PRINTF1(_L("Test that the surface can be opened from the third Thread after creating thread dies and second and third thread close the surface\r\n"));
    // Create a semaphore
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiThreadSemaphore, 0));
	CleanupClosePushL(sem);

    // Create a second thread and opens the handle
	iInfo.iSurfaceManager = iSurfaceManager;
	iInfo.iSurfaceId = iSurfaceId;
	iInfo.iThreadTestCase = ECreateKillSurface;	

	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo));
    RThread thread15;
    _LIT(KNameThreadSecond15, "Test_SurfaceManager_ThreadSecond15");
	User::LeaveIfError(thread15.Create(KNameThreadSecond15,ThreadSecondStart, KDefaultStackSize, &User::Heap(), &iInfo));
   	CleanupStack::Pop();
   	// Launch the second thread
	TRequestStatus statusThreadSecond;
	thread15.Logon(statusThreadSecond);
	thread15.SetPriority(EPriorityLess);
	thread15.Resume();
	testResult = EAllZero;
	sem.Wait();	

	thread15.Suspend(); //suspend the 2nd thread

	TEST(testResult == ECreateSurfaceTestPassed);
	testResult = EAllZero;
	// Test creating the surface in the second thread	
	TEST(KErrNone == iSurfaceManager.OpenSurface(globalSurfaceId));

	// Create a third thread and opens the handle
	iInfo2.iSurfaceManager = iSurfaceManager;
	iInfo2.iSurfaceId = globalSurfaceId;
	iInfo2.iSurfaceIdNew = iSurfaceId;
	iInfo2.iThreadTestCase = EOpenCloseOpenMultiThread;	

	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));
    RThread thread15_2;
    _LIT(KNameThreadSecond15_2, "Test_SurfaceManager_ThreadSecond15_2");
	User::LeaveIfError(thread15_2.Create(KNameThreadSecond15_2,ThreadThirdStart, KDefaultStackSize, &User::Heap(), &iInfo2));
   	CleanupStack::Pop();
	// Launch the third thread
	TRequestStatus statusThreadThird;
	thread15_2.Logon(statusThreadThird);
	thread15_2.SetPriority(EPriorityLess);
	thread15_2.Resume();
	sem.Wait();
	// Test opening the surface in the third thread
	TEST(testResult == EOpenSurfaceTestPassed);
	// Test closing the surface in the main thread	
	TEST(KErrNone == iSurfaceManager.CloseSurface(globalSurfaceId));
	thread15.Resume();
    User::WaitForRequest(statusThreadSecond);

    thread15_2.Resume();
    User::WaitForRequest(statusThreadThird);

    // Test the surface is still accessible in the third thread
	TEST(testResult == (ECloseSurfaceTestPassed | EOpenSurfaceTestPassed));
    // Close the handles
	CleanupStack::PopAndDestroy(1, &sem);
	thread15.Close();
	thread15_2.Close();
	iSurfaceManager.Close();

	}
	

/**
The second thread entry point
*/
TInt CTSurfaceManagerMultiThread::ThreadSecondStart(TAny* aInfo)
	{
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	__UHEAP_MARK;
		
	CChildThreadWrapper* newThread = new CChildThreadWrapper(aInfo);
	if (newThread==NULL)
		{
		return KErrNoMemory;
		}

    RSemaphore sem;
    RSemaphore semMain;
 	TInt ret = sem.OpenGlobal(KMultiThreadSemaphore);
    if (ret == KErrNone)
    	{
   		TThreadTestCase testCase = newThread->iThreadTestCase;
		switch (testCase)
			{
			case ECreateSurfaceMapInfo:
				User::LeaveIfError(semMain.OpenGlobal(KMainThreadSemaphore));
				newThread->CreateSurfaceThread();
				sem.Signal();
				semMain.Wait();
				newThread->MapSurfaceInfo();
				delete newThread;
				semMain.Close();
				break;
			case ECreateSurfaceClose:
				User::LeaveIfError(semMain.OpenGlobal(KMainThreadSemaphore));
				newThread->CreateSurfaceThread();
				sem.Signal();
				semMain.Wait();
				newThread->CloseSurface();
				delete newThread;
				semMain.Close();
				break;
			case EMapSurfaceInfo:
		 		newThread->MapSurfaceInfo();
		 		sem.Signal();
				delete newThread;
				break;
			case ECloseSurfaces:
				newThread->CloseSurface();
				sem.Signal();
				delete newThread;
				break;
			case EOpenCloseSurface:
				newThread->OpenSurface();
				newThread->CloseSurface();
				sem.Signal();
				delete newThread;
				break;
			case EOpenKillSurface:
				newThread->OpenSurface();
				delete newThread;
    			sem.Close();
    			RThread().Kill(ret);
				break;
			case ECreateKillSurface:
				newThread->CreateSurfaceThread();
				sem.Signal();
				delete newThread;
    			sem.Close();
    			RThread().Kill(ret);
				break;
			case EOpenCloseSurfaceMultiThread:
				newThread->OpenSurface(); 
				sem.Signal();
				RThread().Suspend();
				newThread->CloseSurface();
				delete newThread;
				sem.Close();
				return ret;
			case ECloseBothSurfaces:
				newThread->CloseBothSurfaces();
				sem.Signal();
				delete newThread;
				break;
			default:
				break;
			}

		}

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
	
// Implementation of CChildThreadWrapper class 
CChildThreadWrapper::CChildThreadWrapper(TAny* aInfo) :
	iSurfaceManager    (((TInfo*)aInfo)->iSurfaceManager),
	iSurfaceId(((TInfo*)aInfo)->iSurfaceId),
	iSurfaceIdNew(((TInfo*)aInfo)->iSurfaceIdNew),
	iThreadTestCase(((TInfo*)aInfo)->iThreadTestCase)
	{
	}

CChildThreadWrapper::~CChildThreadWrapper()
	{
	}

void CChildThreadWrapper::CreateSurfaceThread()
	{
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(200,200);
	attributes.iBuffers = 1;				
	attributes.iPixelFormat = EUidPixelFormatARGB_1555;		
	attributes.iStride = 1024;				
	attributes.iOffsetToFirstBuffer = 80;	
	attributes.iAlignment = 8;			
	attributes.iContiguous=ETrue;

	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);

	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	if (KErrNone == iSurfaceManager.CreateSurface(buf,iSurfaceId))
		{
		testResult |= ECreateSurfaceTestPassed;
		}
    else
    	{
    	testResult |= ECreateSurfaceTestFailed;	
	   	}
		
	// record the surfaceId as a global variable
	globalSurfaceId = iSurfaceId;
	}
	
void CChildThreadWrapper::MapSurfaceInfo()
	{
	// Access the surface and pass back the test results to the main thread
	RChunk handle;
	// Call Map Surface
	if (iSurfaceManager.MapSurface(iSurfaceId,handle)== KErrNone)
		{
		testResult |= EMapSurfaceTestPassed;
		}
	else 
		{
		testResult |= EMapSurfaceTestFailed;
		}
	handle.Close();

	RSurfaceManager::TInfoBuf infoBuf;
	// Call Surface Info
	if (iSurfaceManager.SurfaceInfo(iSurfaceId, infoBuf) == KErrNone)
		{
		testResult |= ESurfaceInfoTestPassed;
		}
	else 
		{
		testResult |= ESurfaceInfoTestFailed;
		}
	} 
			   
void CChildThreadWrapper::CloseSurface()
	{
	// Call close surface and pass back the results to the main thread
	if (iSurfaceManager.CloseSurface(iSurfaceId) == KErrNone)
		{
		testResult |= ECloseSurfaceTestPassed;
		}
	else 
		{
		testResult |= ECloseSurfaceTestFailed;
		}
	} 

void CChildThreadWrapper::OpenSurface()
	{
	// Call close surface and pass back the results to the main thread
	if (iSurfaceManager.OpenSurface(iSurfaceId) == KErrNone)
		{
		testResult |= EOpenSurfaceTestPassed;
		}
	else
		{
		testResult |= EOpenSurfaceTestFailed;
		}
	} 
		 
void CChildThreadWrapper::CloseBothSurfaces()
	{
	// Call close surface and pass back the results to the main thread
	if (iSurfaceManager.CloseSurface(iSurfaceIdNew) == KErrNone)
		{
		testResult |= ECloseSurfaceTestPassed;
		}
	else 
		{
		testResult |= ECloseSurfaceTestFailed;
		}
	RSurfaceManager::TInfoBuf infoBuf;
	// Call Surface Info
	if (iSurfaceManager.SurfaceInfo(iSurfaceId, infoBuf) == KErrNone)
		{
		testResult |= ESurfaceInfoTestPassed;
		}
	else 
		{
		testResult |= ESurfaceInfoTestFailed;
		}
	
	if (iSurfaceManager.SurfaceInfo(iSurfaceIdNew, infoBuf) == KErrArgument)
		{
		testResult |= ESurfaceInfoTestPassed2;
		}
	else 
		{
		testResult |= ESurfaceInfoTestFailed2;
		}
	iSurfaceManager.CloseSurface(iSurfaceId);
	}
/**
The third thread entry point
*/
TInt CTSurfaceManagerMultiThread::ThreadThirdStart(TAny* aInfo)
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
		
	CChildThreadWrapper* newThread = new CChildThreadWrapper(aInfo);
	if (newThread==NULL)
		{
		delete cleanupStack;
		return KErrNoMemory;
		}
		
	// Pass control back to the first process
    RSemaphore sem;
	TInt ret = sem.OpenGlobal(KMultiThreadSemaphore);
	if (ret!=KErrNone)
		return ret;
		
	TInt err = KErrNone;
    TThreadTestCase testCase = newThread->iThreadTestCase;
	switch (testCase)
		{
		case EOpenMapSurfaceInfoMultiThread:
			newThread->OpenSurface();
			sem.Signal();
			RThread().Suspend();
			newThread->MapSurfaceInfo();
			break;
		case EOpenCloseMapSurfaceInfoMultiThread:
			newThread->OpenSurface();
			sem.Signal();
			RThread().Suspend();
			newThread->CloseSurface();
			newThread->MapSurfaceInfo();
			break;
		case EOpenCloseOpenMultiThread:
			newThread->OpenSurface();
			sem.Signal();
			RThread().Suspend();
			newThread->CloseSurface();
			newThread->OpenSurface();
			break;
		default:
			break;
		}

   	delete newThread;
	delete cleanupStack;
	__UHEAP_MARKEND;
	
    sem.Close();	
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		err = KErrGeneral;  // Thread-owned handles not closed
		}

	return err;
	}


	
//--------------
__CONSTRUCT_STEP__(SurfaceManagerMultiThread)

void CTSurfaceManagerMultiThreadStep::TestSetupL()
	{
    }

void CTSurfaceManagerMultiThreadStep::TestClose()
	{
	}
