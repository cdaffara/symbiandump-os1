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
// Surface manager multi-processed test code
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>
#include <e32std.h>
#include <e32cmn.h>
#include <e32def.h>
#include "tsurfacemanagermultiprocess.h"

_LIT(KSecondProcess,"tsecondprocess.exe");
_LIT(KThirdProcess,"tthirdprocess.exe");


CTSurfaceManagerMultiProcess::CTSurfaceManagerMultiProcess(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	}

CTSurfaceManagerMultiProcess::~CTSurfaceManagerMultiProcess()
	{
	}

void CTSurfaceManagerMultiProcess::RunTestCaseL(TInt aCurTestCase)
	{
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch(aCurTestCase)
		{
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0083
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test mapping a surface created in the existing shared chunk to a different process
						Process 1: Create a surface in a new shared chunk with valid attributes
						Process 1: Map the surface in the current process to get the chunk handle.
						Process 1: Create another surface in the existing chunk (rotation of the first)
						Process 2: Open and Map the second surface in and get the second chunk handles
						Process 2: Check these two chunks handles are different
						Process 2: Get the pixel start address of the pixel data for the second surface via the second chunk handle and write a number to it
						Process 1: Get the pixel start address of the pixel data for the first surface via the first chunk handle and read the number.
						Process 1: Check the number is identical to what we write to in the second process.						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults Mapping one surface created in the existing shared chunk to the other process will not alter the surface chunk memory
*/
	case 1:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0083"));
		TestMapSurfaceExistinChunkL();
		break;		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0084
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test that the API SynchronizeCache() behaves correctly in the multi-process case. 
						Process 1: Create the surface in a new shared chunk with valid creation attributes. The cache attribute is set to ECached. 
						Process 2: Open the surface and map it in the current process to get the chunk handle
						Process 2: Synchronise the cache before the hardware writes to the buffer. Pass TSyncOperation::ESyncBeforeHandwareWrite, valid buffer number and surfaceID in SynchronizeCache()
						Process 2: Synchronise again the cache after the hardware write the buffer. Pass TSyncOperation::ESyncAfterHandwareWrite, valid buffer number and surfaceID in SynchronizeCache()
						Process 1: Synchronise again the cache before the hardware reads the buffer. Pass TSyncOperation::ESyncBeforeHardwareRead, valid buffer number and surfaceID in SynchronizeCache()
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults All synchronisation operation return KErrNone, the surface memory is synchronised properly with cached contents.
*/
	case 2:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0084"));
		TestSynchronizeCacheMultiProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0046
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Testing the behaviour of closing driver channels in multiple processes when the last channel to the device driver is closed
						Process 1: Open a channel called as SurfaceManager1 to the LDD by calling RSurfaceManager::Open() 
						Process 1: Create a surface SurfaceID1 using SurfaceManager1
						Process 1: Open another channel called as SurfaceManager2 to the LDD 
						Process 1: Create another surface SurfaceID2 using SurfaceManager2
						Process 1: Open SurfaceID1 using both SurfaceManager1 and SurfaceManager2
						Process 1: Open SurfaceID2 using both SurfaceManager1 and SurfaceManager2
						Process 2: Open a channel to the LDD and open the surfaceID1 in the process 
						Process 1: Close both channels to the LDD SurfaceManager1 and SurfaceManager2
						Process 2: Call SurfaceInfo() on surfaceID1 and check the return is KErrNone. Call SurfaceInfo() on surfaceID2 and check it returns KErrArgument.
						Process 1: Reopen the channel to the LDD.  Call SurfaceInfo() on surfaceID1. The return value is KErrNone. Call SurfaceInfo() on surfaceID2. The return value is KErrArgument.
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults If all the channels to the driver are closed in a process, the surfaces which have been opened in the other process will not be closed. They are still usable when the first process connects to the driver again.  
*/
	case 3:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0046"));
		TestCloseChannelsMultiProcess1L();
		break;	
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0047
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Testing the behaviour of closing driver channels and surfaces when the process is killed
						Process 2: Open a channel called as SurfaceManager1 to the LDD by calling RSurfaceManager::Open() 
						Process 2: Create a surface SurfaceID1 using SurfaceManager1
						Process 2: Open another channel called as SurfaceManager2 to the LDD 
						Process 2: Create another surface SurfaceID2 using SurfaceManager2
						Process 2: Open SurfaceID1 using both SurfaceManager1 and SurfaceManager2
						Process 2: Open SurfaceID2 using both SurfaceManager1 and SurfaceManager2
						Process 1: Open a channel called as SurfaceManager3 to the LDD and open the surfaceID1 in the process 
						Process 2: Kill the process
						Process 1: Call OpenSurface() on surfaceID1 using SurfaceManager3 and check it returns KErrNone, as the surfaceID1 is still open
						Process 1: Call OpenSurface() on surfaceID2 using SurfaceManager3 and check it returns KErrArgument, as the surfaceID2 is already deleted
						Process 1: Close surfaceID1 and SurfaceManager3
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults When the last channel to the surface manager by the process exiting, the surface manager will automatically close the surfaces which are open in that process.  Any surfaces which were only open in that process will be deleted.
*/
	case 4:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0047"));
		TestCloseChannelsMultiProcess2L();
		break;		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0048
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Multiprocess API testing for getting information using SurfaceInfo
						Process 1: Create the Surface
						Process 2: Receive the Surface Id 
						Process 2: Receive the attributes expected to be used by the surface
						Process 2: Open the surface using the id	
						Process 2: Map the surface
						Process 2: Call SurfaceInfo to get the attributes of the Surface
						Process 2: Check if these are equal to the ones received.
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults The surface can be successfully transmitted between processes
*/
	case 5:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0048"));
		TestSurfaceInfoUsingSurfaceIdL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0049
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Multiprocess API testing for opening the Surface using SurfaceId
						Priocess 1: Create the surface
						Process 2: Receive the Surface id
						Process 2: Open the Surface using the stored SurfaceId
						Process 2: Check that it returns KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface()
@SYMTestExpectedResults Check that opening the surface using a recieved id returns KErrNone
*/
	case 6:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0049"));
		TestOpeningSurfaceUsingSurfaceIdL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0050
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Multiprocess test for opening the surface (OpenSurface negative test)
						Receive a Surface Id from another process
						Change Surface Id by
							1. adding 500 to the SurfaceId
							2. making the Surface ID negative
							3. converting the type of the Surface ID to EInvalidSurface
						Call OpenSurface using the new SurfaceId					
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface()
@SYMTestExpectedResults Check that opening the surface using a modified id returns KErrArgument
*/
	case 7:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0050"));
		TestOpenSurfaceInvalidParamsL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0051
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Closing an unopened surface in a second process 
						Process 1: Create Surface
						Process 2: Close Surface 		
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface()
@SYMTestExpectedResults Check that CloseSurface() returns KErrAccessDenied		
*/
	case 8:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0051"));
		TestClosingUnopenedSurfaceL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0052
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Accessing an unopened surface in a second process give KErrArgument
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults The surface cant be accessed/closed if it is not opened first in another process.	
*/
	case 9:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0052"));
		TestSurfaceInfoUnopenedSurfaceL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0053
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Create, Open and Close in 3 different processes, 
						leaves surface accessible in first 2 processes 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults The surface cant be accessed/closed if it is not opened first in another process.	
*/
	case 10:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0053"));
		CreateOpenCloseThreeProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0054
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test surface can be accessed when creating process dies
						Process 2: Create Surface
						Process 1: Open Surface
						Process 2: Kill Process
						Process 1: Map Surface 
						Process 1: Surface Info
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults It is expected that the surface can be accessed when creating process dies	
*/
	case 11:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0054"));
		TestSurfaceAccessWhenCreatingProcessDiesL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0055
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test surface can be closed when creating process dies
						Process 2: Create Surface
						Process 1: Open Surface
						Process 2: Kill Process
						Process 1: Close Surface 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults It is expected that the surface can be closed when creating process dies	
*/
	case 12:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0055"));
		TestClosingSurfaceWhenCreatingProcessDiesL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0056
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test surface can be closed from third process when 
						creating process dies and second process closes
						Process 2: Create Surface
						Process 1: Open Surface
						Process 3: Open Surface
						Process 2: Kill Process
						Process 1: Close Surface - KErrNone
						Process 3: Close Surface - KErrNone
						Process 3: Open Surface - KErrArgument
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults It is expected that surface can be closed from third process when 
						creating process dies and second process closes	
*/
	case 13:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0056"));
		TestCloseSurfaceInThirdProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0057
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test surface can't be accessed in a second process when open 
						and closed in the first process.
						Process 2: Create Surface
						Process 1: Open Surface
						Process 1: Close Surface
						Process 2: Kill Process
						Process 1: Open Surface 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults It is expected that test surface can't be accessed when closed (2process)
*/
	case 14:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0057"));
		TestNoAccessWhenSurfaceClosedTwoProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0058
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing on one process doesn't prevent opening on other processes, 
						provided one process still owns surface
						Process 1: Create Surface
						Process 2: Open Surface
						Process 2: Close Surface
						Process 3: Open Surface - KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults It is expected that closing on one process doesn't prevent opening on other processes, 
						provided one process still owns surface
*/
	case 15:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0058"));
		TestOpeningOnProcessAfterClosingOnOtherL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0059
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing on one process doesn't prevent access on other 
						processes, provided one process still owns surface
						Process 1: Create Surface
						Process 2: Open Surface
						Process 2: Close Surface
						Process 1: Map Surface - KErrNone
						Process 1: SurfaceInfo - KErrNone
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface(), MapSurface(), SurfaceInfo()
@SYMTestExpectedResults It is expected that closing on one process doesn't prevent access on other 
*/
	case 16:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0059"));
		TestAccessAfterClosingL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0060
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test closing a surface in the creating process when it 
						has already been closed in a second process 
						Process 1: Create Surface
						Process 2: Open Surface
						Process 2: Close Surface
						Process 1: Close Surface 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults Check Close Surface in process 1 returns KErrNone
*/
	case 17:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0060"));
		TestClosingAfterClosingOnOtherProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0061
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test a surface cannot be accessed in a second process if not opened
						Process 1: Create Surface
						Process 2: Map Surface  
						Process 2: Surface Info 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),CloseSurface(), OpenSurface()
@SYMTestExpectedResults Check Map Surface and SurfaceInfo in the second process return KErrAccessDenied
*/
	case 18:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0061"));
		TestErrSufaceAccessNotOpenL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0062
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test surface buffers written to in one process can be read from in another
						Process 1: Create Surface
						Process 1: Map Surface 
						Process 1: Write to buffer
						Process 2: Open the surface 
						Process 2: Read from buffer 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),MapSurface(), OpenSurface()
@SYMTestExpectedResults Value written to buffer in process 1 is equal to value retrieved from buffer in process 2.
*/
	case 19:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0062"));
		TestReadFromBufferInSecondProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0063
@SYMPREQ				PREQ1007, PREQ 1879
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test that the API GetSurfaceHint() behaves correctly in the multi-process case. 
						Process 1: Create the surface in a new shared chunk with valid creation attributes.
						Process 2: Set a THintPair with a key value set to 0x124580 and value set to 300, iMutable set to ETrue.
						Process 2: Call GetSurfaceHint()and Check it returns KErrAccessDenied
						Process 2: Open the surface and then Call SetSurfaceHint(), Check it returns KErrNone.
						Process 2: Call GetSurfaceHint() to retrieve the THintPair with the key value 0x124578
								   Check the THintPair has the iValue set to 20 and iMutable set to ETrue.
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), GetSurfaceHint()
@SYMTestExpectedResults GetSurfaceHint() behaves properly in the multiprocess. 
*/
	case 20:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0063"));
		TestGetSurfaceHintMultiProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0064
@SYMPREQ				PREQ1007, PREQ 1879
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test that the API SetSurfaceHint() behaves correctly in the multi-process case. 
						Process 1: Create the surface in a new shared chunk with valid creation attributes.
						Process 2: Set a THintPair with a key value set to 0x124580 and value set to 300, iMutable set to ETrue.
						Process 2: Call SetSurfaceHint()and Check it returns KErrAccessDenied
						Process 2: Open the surface and then Call SetSurfaceHint(), Check it returns KErrNone.
						Process 2: Call GetSurfaceHint() to retrieve the THintPair with the key value 0x124578
								   Check the retrieved THintPair has the iValue set to 300.
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), SetSurfaceHint(),GetSurfaceHint()
@SYMTestExpectedResults SetSurfaceHint() behaves properly in the multiprocess. 
*/
	case 21:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0064"));
		TestSetSurfaceHintMultiProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0065
@SYMPREQ				PREQ1007, PREQ 1879
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Test that the API AddSurfaceHint() behaves correctly in the multi-process case. 
						Process 1: Process 1: Create the surface in a new shared chunk with valid creation attributes.
						Process 2: Set a THintPair with a key value set to 0x124580 and value set to 300, iMutable set to ETrue.
						Process 2: Call AddSurfaceHint()and Check it returns KErrAccessDenied
						Process 2: Open the surface and then Call AddSurfaceHint(), Check it returns KErrNone.
						Process 2: Call GetSurfaceHint() to retrieve the THintPair with the key value 0x124580
								   Check the retrieved THintPair has the iValue set to 300 and iMutable set to ETrue.
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), GetSurfaceHint(), AddSurfaceHint()
@SYMTestExpectedResults AddSurfaceHint() behaves properly in the multiprocess. 
*/
	case 22:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0065"));
		TestAddSurfaceHintMultiProcessL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0066
@SYMPREQ				PREQ1007, PREQ 1879
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Testing how OpenSurface() and AddConnection() behanves under out of memory conditions 
						Process 1: Create a surface and pass it in to the second process
						Process 2: Open the surface manager, inducing a kernel alloc failure on each possible kernel alloc with the use of MACRO(__KHEAP_SETFAIL).
						Check that returns KerrNoMemory
						Process 2: Call OpenSurface() on the new shared chunk, inducing a kernel alloc failure on each possible kernel alloc with the use of MACRO(__KHEAP_SETFAIL).
						Check that returns KerrNoMemory
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(),OpenSurface(), RSurfaceManager::Open()
@SYMTestExpectedResults OpenSurface and AddConnection should return KerrNoMemory. 
*/		
	case 23:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
#ifdef _DEBUG
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0066"));
		TestOutofMemoryCasesL();
#endif
		break;
	default:
		((CTSurfaceManagerMultiProcessStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTSurfaceManagerMultiProcessStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTSurfaceManagerMultiProcessStep*)iStep)->RecordTestResultL();
	// Handle check
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		User::Leave(KErrGeneral);  // Thread-owned handles not closed
		}
 
	}

void CTSurfaceManagerMultiProcess::TestMapSurfaceExistinChunkL()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Test mapping a surface created in the existing shared chunk to a different process\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	TRequestStatus status;
	// Setup attributes 
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(100,100);
	attributes.iBuffers = 1;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;		// 2bpp
	attributes.iStride = 400;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = 2;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned
	attributes.iMappable = ETrue;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);

	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	//store all the attributes
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == surfaceManager.SurfaceInfo(surfaceId, infoBuf));
	
	//Map the surface in the current processs
	RChunk handle;
	TEST(KErrNone == surfaceManager.MapSurface(surfaceId,handle));
	
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
  	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

	attributesNew.iSize = TSize(480,16);
	attributesNew.iBuffers = 2;				// number of buffers in the surface
	attributesNew.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributesNew.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributesNew.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributesNew.iMappable = ETrue;
	
	attributes.iHintCount = 1;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	
	attributesNew.iAlignment = RSurfaceManager::EPageAligned;
	attributesNew.iOffsetBetweenBuffers = 0;
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceIdNew;
	// Test create surface doesn't return an error
	// For the time being KErrArgument will be returned as the core codes are
	// not ready to check the passed in shared chunk handle.
	TEST(KErrNone == surfaceManager.CreateSurface(buff, surfaceIdNew, handle));
	
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;

  	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ECheckHandle));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);

	
	// Get the adress of this chunk of memory
	TUint8* surfaceAdd = handle.Base();
	TUint8* bufferAdd = surfaceAdd + attributes.iOffsetToFirstBuffer;
	
	// Reads from first buffer, and test the value is written
	TInt temp = *bufferAdd;
	TEST(temp == 20);
	
	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();	
	handle.Close();
	CleanupStack::PopAndDestroy(1, &surfaceManager);
	}

void CTSurfaceManagerMultiProcess::TestSynchronizeCacheMultiProcessL()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Test that the API SynchronizeCache() behaves correctly in the multi-process case\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	TRequestStatus status;
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
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	iInfo2.iSurfaceManager = surfaceManager;
	iInfo2.iSurfaceId = surfaceId;		
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ESyncOperation));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	// Only four tests were carried out in the second process
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();	
	CleanupStack::PopAndDestroy(1, &surfaceManager);

	}


void CTSurfaceManagerMultiProcess::TestCloseChannelsMultiProcess1L()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Testing the behaviour of closing driver channels in multiple processes when  the last channel to the device driver is closed\r\n"));
	
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	
	// Open the surface manager
	RSurfaceManager surfaceManagerOne;
	// Close the surface manager without opening it first
	INFO_PRINTF1(_L("Close the surface manager without opening it first\r\n"));
	// Is this Ok?
//	surfaceManagerOne.Close();
 
	User::LeaveIfError(surfaceManagerOne.Open());
 	CleanupClosePushL(surfaceManagerOne);
   	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(280,301);
	attributes.iBuffers = 1;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 1;
	attributes.iOffsetToFirstBuffer = 1;
	attributes.iAlignment = 1;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Create a surface
	TSurfaceId surfaceIdOne;
	TEST(KErrNone == surfaceManagerOne.CreateSurface(buf, surfaceIdOne));

	RSurfaceManager surfaceManagerTwo;
	User::LeaveIfError(surfaceManagerTwo.Open());
	CleanupClosePushL(surfaceManagerTwo);
	// Create a surface
	TSurfaceId surfaceIdTwo;
	TEST(KErrNone == surfaceManagerTwo.CreateSurface(buf,surfaceIdTwo));

	TEST(KErrNone == surfaceManagerOne.OpenSurface(surfaceIdTwo));
	TEST(KErrNone == surfaceManagerTwo.OpenSurface(surfaceIdOne));

	iInfo2.iSurfaceId = surfaceIdOne;
	iInfo2.iSurfaceManager = surfaceManagerOne;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceIdOne);
   	CleanupStack::Pop();

	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));
	
	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ETestChannelMultiProcess1));
	process.Resume();
	CleanupClosePushL(process);
	
	// Passes control to the second process
	sem.Wait();
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	
	surfaceManagerOne.Close();
	surfaceManagerTwo.Close();

	// Pass control off to the second process again
	chunkWrapper->SetId(surfaceIdTwo);
	sem2.Signal();

	// Wait for the second process to terminate
	User::WaitForRequest(status);
	

	// Copme back from the second process - find the surfaceId
	// Check that the tests in the second process have passed 
	result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFourthTestPassed);
	TEST(result & EFifthTestPassed);
	//More things to check in here
//	TEST(KErrNone == surfaceManagerOne.Open()));
	
	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	CleanupStack::PopAndDestroy(5, &sem);
	

	}

void CTSurfaceManagerMultiProcess::TestCloseChannelsMultiProcess2L()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Testing the behaviour of closing driver channels in multiple processes when  the last channel to the device driver is closed\r\n"));
	
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	
	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));
	
	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ETestChannelMultiProcess2));
	process.Resume();
	CleanupClosePushL(process);
	
	// Passes control to the second process
	sem.Wait();
		
	// Open the chunk wrapper and get the surfaceId
	CChunkWrapper* chunkWrapper = CChunkWrapper::OpenL(KSharedChunkName, ETrue);
	TSurfaceId surfaceIdOne = chunkWrapper->GetId();
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);

	// Open the surface manager
	RSurfaceManager surfaceManagerThree;
 	User::LeaveIfError(surfaceManagerThree.Open());
	CleanupClosePushL(surfaceManagerThree);
	
	TEST(KErrNone == surfaceManagerThree.OpenSurface(surfaceIdOne));
	// Pass control off to the second process again
	sem2.Signal();

	// Wait for the second process to terminate
	User::WaitForRequest(status);
	
	TSurfaceId surfaceIdTwo = chunkWrapper->GetId();
	
	// Call OpenSurface() on surfaceID1 using SurfaceManager3 and check it returns KErrNone, as the surfaceID1 is still open
	TEST(KErrNone == surfaceManagerThree.OpenSurface(surfaceIdOne));
	// Call OpenSurface() on surfaceID1 using SurfaceManager3 and check it returns KErrNone, as the surfaceID1 is still open
	TEST(KErrArgument == surfaceManagerThree.OpenSurface(surfaceIdTwo));

	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	CleanupStack::PopAndDestroy(4, &sem);
	}


void CTSurfaceManagerMultiProcess::TestSurfaceInfoUsingSurfaceIdL()
	{
	INFO_PRINTF1(_L("Receiving a surface and querying SurfaceInfo for surface properties\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	TRequestStatus status;
	
	// Set attributs for creating the surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes=buf();
    attributes.iSize = TSize(100,100);
    attributes.iBuffers = 1;                // number of buffers in the surface
    attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;     // 2bpp
    attributes.iStride = 400;               // Number of bytes between start of one line and start of next
    attributes.iOffsetToFirstBuffer = 0;    // way of reserving space before the surface pixel data
    attributes.iAlignment = 2;          // alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned
    attributes.iContiguous=ETrue; 
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);
	
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;
	attributes.iMappable = ETrue;
	
	// Create the surface
	TSurfaceId surfaceId;
    TEST(KErrNone == surfaceManager.CreateSurface(attributes, surfaceId));

    iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ETestInfoReceivedSurface));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	TEST(result & EFifthTestPassed);
	TEST(result & ESixthTestPassed);
	TEST(result & ESeventhTestPassed);
	TEST(result & EEighthTestPassed);
	TEST(result & ENinthTestPassed);

	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();	
	CleanupStack::PopAndDestroy(1, &surfaceManager);

	}
	
void CTSurfaceManagerMultiProcess::TestOpeningSurfaceUsingSurfaceIdL()
	{
	INFO_PRINTF1(_L("Test Opening a surface using surfaceId (passed from this to another process)\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	TRequestStatus status;
	
	// Set attributs for creating the surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes=buf();
	attributes.iSize = TSize(200,200);
	attributes.iBuffers = 1;				
	attributes.iPixelFormat = EUidPixelFormatARGB_1555;		
	attributes.iStride = 1600;				
	attributes.iOffsetToFirstBuffer = 80;	
	attributes.iAlignment = 8;			
	attributes.iContiguous = ETrue;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);

	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	// Create the surface
	TSurfaceId surfaceId;
    TEST(KErrNone == surfaceManager.CreateSurface(attributes, surfaceId));

	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();
 
    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ETestOpenReceivedSurface));	
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the test results of the second process
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);

	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();
	CleanupStack::PopAndDestroy(1, &surfaceManager);

	}

void CTSurfaceManagerMultiProcess::TestOpenSurfaceInvalidParamsL()
	{
   	INFO_PRINTF1(_L("Opening a surface with invalid parameters\r\n"));
   	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(150,412);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatARGB_1555; // 2bpp
	attributes.iStride = 1000;
	attributes.iOffsetToFirstBuffer = 5;
	attributes.iAlignment = 1;
	attributes.iContiguous=EFalse;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Create a surface - increments reference count by 1
	TSurfaceId surfaceId;
    TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
		
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);

   	CleanupStack::Pop();

    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ETestOpenSurfaceInvalidParams));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);

	// Tidy up
	delete chunkWrapper;
	process.Close();
	CleanupStack::PopAndDestroy(1, &surfaceManager);
	}

void CTSurfaceManagerMultiProcess::TestClosingUnopenedSurfaceL()
	{
	INFO_PRINTF1(_L("Closing an unopened surface in a second process\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(280,301);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 605;
	attributes.iOffsetToFirstBuffer = 4;
	attributes.iAlignment = 4;
	attributes.iContiguous=EFalse;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);

	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	// Create the surface
	TSurfaceId surfaceId;
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));

	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

	// Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KThirdProcess, KNullDesC));

	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ECloseSurface));
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the result
	TInt result = chunkWrapper->GetThirdProcessResults();
	TEST(result & EFirstTestPassed);
	
	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();
	CleanupStack::PopAndDestroy(1, &surfaceManager);
	}


void CTSurfaceManagerMultiProcess::TestSurfaceInfoUnopenedSurfaceL()
	{
	INFO_PRINTF1(_L("Access an unopened surface in a second process\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(280,301);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 605;
	attributes.iOffsetToFirstBuffer = 4;
	attributes.iAlignment = 4;
	attributes.iContiguous=EFalse;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);

	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	// Create the surface
	TSurfaceId surfaceId;
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

	// Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KThirdProcess, KNullDesC));

	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ESurfaceInfo));
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the result
	TInt result = chunkWrapper->GetThirdProcessResults();
	TEST(result & EFirstTestPassed);
	
	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();
	CleanupStack::PopAndDestroy(1, &surfaceManager);

	}
	
void CTSurfaceManagerMultiProcess::CreateOpenCloseThreeProcessL()
	{
	INFO_PRINTF1(_L("Create Open close across 3 processes\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);
	
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(280,230);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 1100;
	attributes.iOffsetToFirstBuffer = 4;
	attributes.iAlignment = 4;
	attributes.iContiguous=EFalse;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);

	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	// Create the surface
	TSurfaceId surfaceId;
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	

	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

	// Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

	// Create a second process
	RProcess process2;
	TEST(KErrNone == process2.Create(KSecondProcess, KNullDesC));

	// Kick off the process and wait for it to complete
	TRequestStatus status2 = KRequestPending;
	process2.Logon(status2);
	process2.SetParameter(EMultiProcessSecondSlot, EOpenWaitMap);
	process2.Resume();
	
	// Passes control to the second process
	sem.Wait();
	
	// Create a third process
	RProcess process3;
	TEST(KErrNone == process3.Create(KThirdProcess, KNullDesC));

	// Kick off the process and wait for it to complete
	TRequestStatus status3 = KRequestPending;
	process3.Logon(status3);
	process3.SetParameter(EMultiProcessSecondSlot, ECloseSurface);
	process3.Resume();
	
	// Let the second process continue its work - calling MapSurface
	sem2.Signal();
	
	// Call Surface Info on the surface in this process
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == surfaceManager.SurfaceInfo(surfaceId, infoBuf));
	
	// Wait for the second process to terminate
	User::WaitForRequest(status2);
	
	// Check that the tests in the second process have passed
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	
	// Wait for the third process to terminate and chaeck the results
	User::WaitForRequest(status3);
	result = chunkWrapper->GetThirdProcessResults();
	TEST(result & EFirstTestPassed);
	
	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	process2.Close();
	process3.Close();

	CleanupStack::PopAndDestroy(3, &surfaceManager);
	}

void CTSurfaceManagerMultiProcess::TestSurfaceAccessWhenCreatingProcessDiesL()
	{
	INFO_PRINTF1(_L("Test surface can be accessed when creating process dies\r\n"));
	// Create two semaphores
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	
	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ECreateWaitKill));
	process.Resume();
	CleanupClosePushL(process);
	
	// Passes control to the second process
	sem.Wait();
		
	// Copme back from the second process - find the surfaceId
	CChunkWrapper* chunkWrapper = CChunkWrapper::OpenL(KSharedChunkName, ETrue);
	TSurfaceId surfaceId = chunkWrapper->GetId();
	
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	// Open the surface
	TEST(KErrNone == surfaceManager.OpenSurface(surfaceId));
	
	// Pass control off to the second process again
	sem2.Signal();
	
	// Wait for the second process to terminate
	User::WaitForRequest(status);
	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	
	// Map Surface
	RChunk handle;
	TEST(KErrNone == surfaceManager.MapSurface(surfaceId, handle));
	handle.Close();
	// Surface Info
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == surfaceManager.SurfaceInfo(surfaceId, infoBuf));
	
	TEST(KErrNone == surfaceManager.CloseSurface(surfaceId));

	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	CleanupStack::PopAndDestroy(4, &sem);
	}

void CTSurfaceManagerMultiProcess::TestClosingSurfaceWhenCreatingProcessDiesL()
	{
	INFO_PRINTF1(_L("Test surface can be closed when creating process dies\r\n"));
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	
	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));
	
	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ECreateWaitKill));
	process.Resume();
	CleanupClosePushL(process);
	
	// Passes control to the second process
	sem.Wait();
		
	// Copme back from the second process - find the surfaceId
	CChunkWrapper* chunkWrapper = CChunkWrapper::OpenL(KSharedChunkName, ETrue);
	TSurfaceId surfaceId = chunkWrapper->GetId();
	
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);
	
	// Open the surface
	TEST(KErrNone == surfaceManager.OpenSurface(surfaceId));
	
	// Pass control off to the second process again
	sem2.Signal();

	// Wait for the second process to terminate
	User::WaitForRequest(status);

	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	
	TEST(KErrNone == surfaceManager.CloseSurface(surfaceId));
	
	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	CleanupStack::PopAndDestroy(4, &sem);
	}
	
void CTSurfaceManagerMultiProcess::TestCloseSurfaceInThirdProcessL()
	{
	INFO_PRINTF1(_L("Test surface access in a third process\r\n"));
	// Create 2 semaphores for signalling between process 1 and process 2
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	
	// Create a second process
	RProcess process2;
	TEST(KErrNone == process2.Create(KSecondProcess, KNullDesC));
	
	// Kick off the process and wait for it to complete
	TRequestStatus status2 = KRequestPending;
	process2.Logon(status2);
	process2.SetParameter(EMultiProcessSecondSlot, ECreateWaitKill);
	process2.Resume();
	CleanupClosePushL(process2);
	
	// Passes control to the second process
	sem.Wait();
		
	// Come back from the second process - find the surfaceId
	CChunkWrapper* chunkWrapper = CChunkWrapper::OpenL(KSharedChunkName, ETrue);
	TSurfaceId surfaceId = chunkWrapper->GetId();
	
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);
	
	// Open the surface
	TEST(KErrNone == surfaceManager.OpenSurface(surfaceId));
	
	// Create another two semaphores for signalling between process 1 and process 3
	RSemaphore sem3;
	TEST(KErrNone == sem3.CreateGlobal(KMultiProcessSemaphore3, initCount));

	RSemaphore sem4;
	TEST(KErrNone == sem4.CreateGlobal(KMultiProcessSemaphore4, initCount));

	// Create a third process
	RProcess process3;
	TEST(KErrNone == process3.Create(KThirdProcess, KNullDesC));
	
	// Kick off the process and wait for it to complete
	TRequestStatus status3 = KRequestPending;
	process3.Logon(status3);
	process3.SetParameter(EMultiProcessSecondSlot, EOpenWaitCloseOpen);
	process3.Resume();

	// Passes control to the third process - opens the surface
	sem3.Wait();
	
	// Pass control to the second process again - kill the process
	sem2.Signal();
	
	// Wait for the third process to terminate
	User::WaitForRequest(status2);
	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	
	// Close the surface
	TEST(KErrNone == surfaceManager.CloseSurface(surfaceId));
	
	// Pass control to the third process - close the surface
	sem4.Signal();	
	
	// Wait for the third process to terminate
	User::WaitForRequest(status3);
	
	// Check that the tests in the third process have passed 
	result = chunkWrapper->GetThirdProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	TEST(result & EFifthTestPassed);
//	TEST(result & ESixthTestPassed);
	
	// Cleanup
	delete chunkWrapper;
	sem3.Close();
	sem4.Close();
	process3.Close();

	CleanupStack::PopAndDestroy(4, &sem);
	}

void CTSurfaceManagerMultiProcess::TestNoAccessWhenSurfaceClosedTwoProcessL()
	{
	INFO_PRINTF1(_L("Test surface can't be accessed when closed (2process)\r\n"));
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	
	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));
	
	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ECreateWaitKill));
	process.Resume();
	CleanupClosePushL(process);
	
	// Passes control to the second process
	sem.Wait();
		
	// Come back from the second process - find the surfaceId
	CChunkWrapper* chunkWrapper = CChunkWrapper::OpenL(KSharedChunkName, ETrue);
	TSurfaceId surfaceId = chunkWrapper->GetId();
	
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);   
	// Open the surface
	TEST(KErrNone == surfaceManager.OpenSurface(surfaceId));
	
	// Close the surface
	TEST(KErrNone == surfaceManager.CloseSurface(surfaceId));
	
	// Pass control off to the second process again
	sem2.Signal();
	
	// Wait for the third process to terminate
	User::WaitForRequest(status);
	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	
	TEST(KErrArgument == surfaceManager.OpenSurface(surfaceId));
	
	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	CleanupStack::PopAndDestroy(4, &sem);
	}
void CTSurfaceManagerMultiProcess::TestOpeningOnProcessAfterClosingOnOtherL()
	{
	INFO_PRINTF1(_L("Test closing on one process doesn't prevent opening on another\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);   

	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(280,15);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 901;
	attributes.iOffsetToFirstBuffer = 4;
	attributes.iAlignment = 4;
	attributes.iContiguous=EFalse;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);

	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	// Create the surface
	TSurfaceId surfaceId;
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

	// Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();
	
	// Create a second process
	RProcess process2;
	TEST(KErrNone == process2.Create(KSecondProcess, KNullDesC));

	// Kick off the process and wait for it to complete
	TRequestStatus status2 = KRequestPending;
	process2.Logon(status2);
	process2.SetParameter(EMultiProcessSecondSlot, EOpenClose);
	process2.Resume();
	
	// Passes control to the second process
	sem.Wait();
	
	// Create a third process
	RProcess process3;
	TEST(KErrNone == process3.Create(KThirdProcess, KNullDesC));
	
	// Kick off the process and wait for it to complete
	TRequestStatus status3 = KRequestPending;
	process3.Logon(status3);
	process3.SetParameter(EMultiProcessSecondSlot, EOpenSurface);
	process3.Resume();
	
	// Pass control off to the second process again
	sem2.Signal();
	
	// Wait for the second process to terminate
	User::WaitForRequest(status2);
	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	
	// Wait for the third process to terminate
	User::WaitForRequest(status3);
	
	// Test results
	result = chunkWrapper->GetThirdProcessResults();
	TEST(result & EFirstTestPassed);

	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	process2.Close();
	process3.Close();
	CleanupStack::PopAndDestroy(3, &surfaceManager);
	}

void CTSurfaceManagerMultiProcess::TestAccessAfterClosingL()
	{
	INFO_PRINTF1(_L("Test closing doesn't prevent access\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);   

	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(280,199);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 711;
	attributes.iOffsetToFirstBuffer = 4;
	attributes.iAlignment = 4;
	attributes.iContiguous=EFalse;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);
	
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;
	attributes.iMappable = ETrue;

	// Create the surface
	TSurfaceId surfaceId;
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);

	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));
	
	// Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, EOpenClose));
	process.Resume();
	
	// Passes control to the second process
	sem.Wait();
	
	// Map Surface
	RChunk handle;
	TEST(KErrNone == surfaceManager.MapSurface(surfaceId, handle));
	handle.Close();
	// Surface Info
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == surfaceManager.SurfaceInfo(surfaceId, infoBuf));
	
	// Pass control off to the second process again
	sem2.Signal();
	
	// Wait for the second process to terminate
	User::WaitForRequest(status);
	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	
	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	process.Close();
	CleanupStack::PopAndDestroy(3, &surfaceManager);
	}
	
void CTSurfaceManagerMultiProcess::TestClosingAfterClosingOnOtherProcessL()
	{
	INFO_PRINTF1(_L("Test can close in two processes\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);   
	
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(280,301);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 768;
	attributes.iOffsetToFirstBuffer = 4;
	attributes.iAlignment = 4;
	attributes.iContiguous=EFalse;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);

	attributes.iOffsetBetweenBuffers = 0;
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;
	attributes.iMappable = ETrue;

	// Create the surface
	TSurfaceId surfaceId;
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);
	
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

	
	// Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();
	
	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, EOpenClose));
	process.Resume();
	
	// Passes control to the second process
	sem.Wait();
	
	// Test closing the surface manager
	TEST(KErrNone == surfaceManager.CloseSurface(surfaceId));
	
	// Pass control off to the second process again
	sem2.Signal();
	
	// Wait for the second process to terminate
	User::WaitForRequest(status);
	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	
	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	process.Close();
	CleanupStack::PopAndDestroy(3, &surfaceManager);
	}
	
void CTSurfaceManagerMultiProcess::TestErrSufaceAccessNotOpenL()
	{
	INFO_PRINTF1(_L("Test a surface cannot be accessed in a second process if not opened\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(280,301);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 1400;
	attributes.iOffsetToFirstBuffer = 4;
	attributes.iAlignment = 4;
	attributes.iContiguous=EFalse;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);
	
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;

	// Create the surface
	TSurfaceId surfaceId;
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	// Create a semaphore
	const TInt initCount = 0;
	RSemaphore sem;
	TEST(KErrNone == sem.CreateGlobal(KMultiProcessSemaphore, initCount));
	CleanupClosePushL(sem);
	RSemaphore sem2;
	TEST(KErrNone == sem2.CreateGlobal(KMultiProcessSemaphore2, initCount));
	CleanupClosePushL(sem2);

	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));


	// Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));
	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, EMapSurfaceInfoCantAccess));
	process.Resume();
	
	// Passes control to the second process - to test MapSurface and SurfaceInfo
	sem.Wait();
	
	// Pass control off to the second process again - to allow it to terminate
	sem2.Signal();
	
	// Wait for the second process to terminate
	User::WaitForRequest(status);
	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	
	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	process.Close();
	CleanupStack::PopAndDestroy(3, &surfaceManager);
	}
	
void CTSurfaceManagerMultiProcess::TestReadFromBufferInSecondProcessL()
	{
	INFO_PRINTF1(_L("Writing to a buffer and reading from another process\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);
	
	// Setup attributes, setting iBuffers to 2
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(12,80);    	// w > 0, h > 0
	attributes.iBuffers = 2; 				// > 0, <= 4
	attributes.iPixelFormat = EUidPixelFormatARGB_1555;	// 2bpp
	attributes.iStride = 25;				// > 0, > width * bpp
	attributes.iOffsetToFirstBuffer = 20;	// > 0, divisible by alignment
	attributes.iAlignment = 4;				// 1 || 2 || 4 || 8
	attributes.iContiguous = ETrue;
	
	RSurfaceManager::THintPair hints[2];	// two hint pairs specified
	attributes.iHintCount = 2;
	attributes.iSurfaceHints = hints;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);
	
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;
	attributes.iMappable = ETrue;
	
	// Create the surface
	TSurfaceId surfaceId;
    TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	// Map the surface to a chunk of memory
	RChunk handle;
	TEST(KErrNone == surfaceManager.MapSurface(surfaceId, handle));
	CleanupClosePushL(handle);
	
	// Get the adress of this chunk of memory
	TUint8* surfaceAdd = handle.Base();
	TUint8* bufferAdd = surfaceAdd + attributes.iOffsetToFirstBuffer;
	
	// Write to the buffer
	*bufferAdd = 134;

   	iInfo2.iSurfaceId = surfaceId;
  	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

	
	// Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

	// Create a second process
	RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));
	// Kick off the process and wait for it to complete
	TRequestStatus status = KRequestPending;
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, EReadFromBuffer));
	process.Logon(status);
	process.Resume();
	
	// Wait for the second process to terminate
	User::WaitForRequest(status);
	
	// Check that the tests in the second process have passed 
	TInt result = chunkWrapper->GetSecondProcessResults();
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	
	// Delete the chunkWrapper and the semaphore
	delete chunkWrapper;
	process.Close();
	CleanupStack::PopAndDestroy(2, &surfaceManager);
	}

void CTSurfaceManagerMultiProcess::TestGetSurfaceHintMultiProcessL()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Test that the GetSurfaceHint() behaves properly in the multiprocesses\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	TRequestStatus status;
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
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, EGetSurfaceHint));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	// Only four tests were carried out in the second process
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	
	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();	
	CleanupStack::PopAndDestroy(1, &surfaceManager);

	}

void CTSurfaceManagerMultiProcess::TestSetSurfaceHintMultiProcessL()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Test that the SetSurfaceHint() behaves properly in the multiprocesses\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	TRequestStatus status;
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
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ESetSurfaceHint));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	// Only four tests were carried out in the second process
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();	
	CleanupStack::PopAndDestroy(1, &surfaceManager);

	}
	
void CTSurfaceManagerMultiProcess::TestAddSurfaceHintMultiProcessL()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Test that the AddSurfaceHint() behaves properly in the multiprocesses\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	TRequestStatus status;
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
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, EAddSurfaceHint));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	// Only four tests were carried out in the second process
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();	
	CleanupStack::PopAndDestroy(1, &surfaceManager);
	
	}

void CTSurfaceManagerMultiProcess::TestOutofMemoryCasesL()
	{
	
	RDebug::Printf("test 45 start");
	INFO_PRINTF1(_L("Test OutofMemory conditions in OpenSurface()and AddConnection()\r\n"));
	// Open the surface manager
	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	CleanupClosePushL(surfaceManager);

	TRequestStatus status;
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
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == surfaceManager.CreateSurface(buf, surfaceId));
	
	iInfo2.iSurfaceId = surfaceId;
	iInfo2.iSurfaceManager = surfaceManager;
	// Create a TCleanupItem object
    CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseSurfaceWhenLeave, &iInfo2));

    // Save the surfaceId to the shared chunk
	CChunkWrapper* chunkWrapper = CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
    chunkWrapper->SetId(surfaceId);
   	CleanupStack::Pop();

    // Create a second process
    RProcess process;
	TEST(KErrNone == process.Create(KSecondProcess, KNullDesC));

	// Specify the test for the second process
	TEST(KErrNone == process.SetParameter(EMultiProcessSecondSlot, ECheckOutofMemory));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	// Check the results of the second process tests
	TInt result = chunkWrapper->GetSecondProcessResults();
	// Only four tests were carried out in the second process
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);

	// Delete the chunkWrapper
	delete chunkWrapper;
	process.Close();	
	CleanupStack::PopAndDestroy(1, &surfaceManager);

	}

//--------------
__CONSTRUCT_STEP__(SurfaceManagerMultiProcess)

void CTSurfaceManagerMultiProcessStep::TestSetupL()
	{
    }

void CTSurfaceManagerMultiProcessStep::TestClose()
	{
	}

