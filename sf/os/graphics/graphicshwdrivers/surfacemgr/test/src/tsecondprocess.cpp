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
#include <e32debug.h>
#include <e32def_private.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include "tsmgmultprocessshared.h"

LOCAL_D RTest test(_L("TReceiveSurface"));


class CTestDriverSecondProcess : public CTestDriver
{
public:
	CTestDriverSecondProcess();
	~CTestDriverSecondProcess();
	void ConstructL();
	static CTestDriverSecondProcess* NewL();
public:
	void TestMultipleChannelsInSecondProcess2();
	void TestMultipleChannelsInSecondProcess1();
	void TestCheckSyncOperation();
	void TestCheckHandleInSecondProcess();
	void TestSurfaceInfoUsingSurfaceId();
	void TestOpeningSurfaceUsingSurfaceId();
	void TestOpeningSurfaceInvalidParams();
	void OpenWaitMap();
	void CreateWaitKill();
	void OpenClose();
	void MapSurfaceInfoCantAccess();
	void TestReadFromBufferInSecondProcess();
	void TestGetSurfaceHint();
	void TestSetSurfaceHint();
	void TestAddSurfaceHint();
	void TestOutofMemory();
private:
	RSurfaceManager iSurfaceManagerTwo;
};

CTestDriverSecondProcess::CTestDriverSecondProcess():CTestDriver()
	{
	}

CTestDriverSecondProcess::~CTestDriverSecondProcess()
	{
	iSurfaceManagerTwo.Close();
	}

void CTestDriverSecondProcess::ConstructL()
	{
	CTestDriver::ConstructL();
	User::LeaveIfError(	iSurfaceManagerTwo.Open());	
	}

CTestDriverSecondProcess* CTestDriverSecondProcess::NewL()
{
	CTestDriverSecondProcess * driver = new (ELeave) CTestDriverSecondProcess();
	CleanupStack::PushL(driver);
	driver->ConstructL();
	CleanupStack::Pop(driver);
	return driver;
}

void CTestDriverSecondProcess::TestMultipleChannelsInSecondProcess2()
	{
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

	// Create the surface
	TSurfaceId surfaceIdOne;
    if(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdOne))
		{
		iTestResult |= EFirstTestPassed;
		}

	// Create the surface
	TSurfaceId surfaceIdTwo;
    if(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdTwo))
		{
		iTestResult |= ESecondTestPassed;
		}
    
    if(KErrNone == iSurfaceManager.OpenSurface(surfaceIdTwo))
		{
		iTestResult |= EThirdTestPassed;
		}
    
    if(KErrNone == iSurfaceManagerTwo.OpenSurface(surfaceIdOne))
		{
		iTestResult |= EFourthTestPassed;
		}
    
    // Set the results so they can be read and tested by the first process
    iChunkWrapper->SetSecondProcessResults(iTestResult);
    // Put the surfaceId onto the shared chunk
    iChunkWrapper->SetId(surfaceIdOne);

 
    // Pass control back to the first process
    RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KMultiProcessSemaphore))
		{
		iTestResult |= EThirdTestPassed;
		}

    sem.Signal();
    
    RSemaphore sem2;
	if(KErrNone == sem2.OpenGlobal(KMultiProcessSemaphore2))
		{
		iTestResult |= EFourthTestPassed;
		}
    sem2.Wait();
	
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
//	  // Put the surfaceId onto the shared chunk
	iChunkWrapper->SetId(surfaceIdTwo);

	sem.Close();
	sem2.Close();
	}

void CTestDriverSecondProcess::TestMultipleChannelsInSecondProcess1()
	{
	// Open the chunk wrapper and get the surfaceId
	TSurfaceId surfaceIdOne = iChunkWrapper->GetId();
		
	// Open the surface using the surfaceId - check that it returns KErrNone
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceIdOne))
		{
		iTestResult |= EFirstTestPassed;
		}

    // Pass control back to the first process
    RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KMultiProcessSemaphore))
		{
		iTestResult |= ESecondTestPassed;
		}
    
    RSemaphore sem2;
	if(KErrNone == sem2.OpenGlobal(KMultiProcessSemaphore2))
		{
		iTestResult |= EThirdTestPassed;
		}

   	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);

	sem.Signal();
	sem2.Wait();
    
	// Get the surface info
	RSurfaceManager::TInfoBuf infoBuf;
	if(KErrNone == iSurfaceManager.SurfaceInfo(surfaceIdOne, infoBuf))
		{
		iTestResult |= EFourthTestPassed;
		}
	TSurfaceId surfaceIdTwo = iChunkWrapper->GetId();
	
	if(KErrArgument == iSurfaceManager.SurfaceInfo(surfaceIdTwo, infoBuf))
		{
		iTestResult |= EFifthTestPassed;
		}
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);

	sem.Close();
	sem2.Close();
	}

void CTestDriverSecondProcess::TestCheckSyncOperation()
	{
	// Open the chunk wrapper and get the surfaceId
	TSurfaceId surfaceId = iChunkWrapper->GetId();
	
	// Check it returns KErrAccessDenied when the surface is not Open
	TInt bufferNo = 1;

	RSurfaceManager::TSyncOperation syncOperation = RSurfaceManager::ESyncBeforeNonCPURead;
	
	if(KErrAccessDenied == iSurfaceManager.SynchronizeCache(surfaceId, bufferNo,syncOperation))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	// Open the surface using the surfaceId - check that it returns KErrNone
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= ESecondTestPassed;
		}
	
	// Map the surface 
	RChunk handle;
	
	if(KErrNone == iSurfaceManager.MapSurface(surfaceId, handle))
		{
		iTestResult |= EThirdTestPassed;
		}
	
	if(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId, bufferNo,syncOperation))
		{
		iTestResult |= EFourthTestPassed;
		}
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
	// Close the chunkwrapper, handle and the surface manager
 	handle.Close();

	}
void CTestDriverSecondProcess::TestCheckHandleInSecondProcess()
	{
	// Open the chunk wrapper and get the surfaceId
	TSurfaceId surfaceId = iChunkWrapper->GetId();

	// Open the surface using the surfaceId - check that it returns KErrNone
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	// Map the surface 
	RChunk handle;
	
	if(KErrNone == iSurfaceManager.MapSurface(surfaceId, handle))
		{
		iTestResult |= ESecondTestPassed;
		}
	
	// Get the surface info
	RSurfaceManager::TInfoBuf infoBuf;
	if(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf))
		{
		iTestResult |= EThirdTestPassed;
		}
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	// Get the adress of this chunk of memory
	TUint8* surfaceAdd = handle.Base();
	TInt offsetToFirstBuffer;
	if(KErrNone == iSurfaceManager.GetBufferOffset(surfaceId, 0, offsetToFirstBuffer))
		{
		iTestResult |= EFourthTestPassed;
		}
	TUint8* bufferAdd = surfaceAdd + offsetToFirstBuffer;
	
	// Write to the first buffer, and test the value is written
	*bufferAdd = 20;
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
	
	// Close the chunkwrapper, handle and the surface manager
	handle.Close();
	}

/**
Test 18. Receiving a surface and querying SurfaceInfo for surface properties

Process 1: Create the Surface
Process 2: Receive the Surface Id 
Process 2: Receive the attributes used to create the surface
Process 2: Open the surface using the id
Process 2: Map the surface
Process 2: Call SurfaceInfo to get the attributes of the Surface
Check if these are equal to the ones received.

@see TestSurfaceInfoUsingSurfaceIdL() in tsurfacemanager.cpp
*/	
void CTestDriverSecondProcess::TestSurfaceInfoUsingSurfaceId()
	{	
	// Set attributes for the surface - these are expected attributes in the second process
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
	
	TSurfaceId surfaceId = iChunkWrapper->GetId();
	
	// Open the surface using the surfaceId - check that it returns KErrNone
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	// Map the surface 
	RChunk handle;
	
	if(KErrNone == iSurfaceManager.MapSurface(surfaceId, handle))
		{
		iTestResult |= ESecondTestPassed;
		}
	
	// Get the surface info
	RSurfaceManager::TInfoBuf infoBuf;
	if(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf))		
		{
		iTestResult |= EThirdTestPassed;
		}
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	TInt offsetToFirstBuffer;
	if(KErrNone == iSurfaceManager.GetBufferOffset(surfaceId, 0, offsetToFirstBuffer))  	
		{
		iTestResult |= EFourthTestPassed;
		}
		
	if(info.iSize == attributes.iSize)
		{
		iTestResult |= EFifthTestPassed;
		}
	if(info.iBuffers == attributes.iBuffers)
		{
		iTestResult |= ESixthTestPassed;
		}
	if(info.iPixelFormat == attributes.iPixelFormat)
		{
		iTestResult |= ESeventhTestPassed;
		}
	if(info.iStride == attributes.iStride)
		{
		iTestResult |= EEighthTestPassed;
		}
	if(offsetToFirstBuffer >= attributes.iOffsetToFirstBuffer)
		{
		iTestResult |= ENinthTestPassed;
		}
	if(info.iContiguous == attributes.iContiguous)
		{
		iTestResult |= ETenthTestPassed;
		}
	
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
	
	// Close handle 
	handle.Close();
	}

/**
Test 19. Opening a surface using surfaceId

Priocess 1: Create the surface
Process 2: Receive the Surface id
Process 2: Open the Surface using the stored Surface id
Check OpenSurface returns KErrNone

@see TestOpeningSurfaceUsingSurfaceIdL() in tsurfacemanager.cpp
*/
void CTestDriverSecondProcess::TestOpeningSurfaceUsingSurfaceId()
	{
	// Open the chunk wrapper and get the surfaceId
//	CChunkWrapper* chunkWrapper = CChunkWrapper::OpenL(KSharedChunkName, ETrue);
	TSurfaceId surfaceId = iChunkWrapper->GetId();
		
	// Open the surface using the surfaceId - check that it returns KErrNone	
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= EFirstTestPassed;
		}
		
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);

	}
	
/**
Test 20. Opening a surface using invalid surfaceId

Process 1:Create the surface
Process 2: Receive a Surface Id
Change Surface Id by
1. adding 500 to the SurfaceId
2. making the Surface ID negative
3. converting the type of the Surface ID to EInvalidSurface
Process 2: Call OpenSurface using the new SurfaceId
Check that the return value of OpenSurface is KErrArgument

@see TestOpenSurfaceInvalidParams() in tsurfacemanager.cpp
*/
void CTestDriverSecondProcess::TestOpeningSurfaceInvalidParams()
	{
	
	TSurfaceId surfaceId = iChunkWrapper->GetId();
	
	// Open Surface using the right Id
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= EFirstTestPassed;
		}
	// Open the surface using the invalid surfaceId - check that it returns KErrArgument
	TSurfaceId invalidSurfaceId = surfaceId; 
	//Add 500 to the first field of surfaceId
	invalidSurfaceId.iInternal[0] = surfaceId.iInternal[0]+500;
	if(KErrArgument == iSurfaceManager.OpenSurface(invalidSurfaceId))
		{
		iTestResult |= ESecondTestPassed;
		}
	// Change the surfaceId type to EInvalidSurface
	invalidSurfaceId.iInternal[3] = (surfaceId.iInternal[3] & 0x00FFFFFF) | ( TSurfaceId::EInvalidSurface <<24 ) ;
	if(KErrArgument == iSurfaceManager.OpenSurface(invalidSurfaceId))
		{
		iTestResult |= EThirdTestPassed;
		}
		
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
	
	}

/**
Test 22: Create, Open and Close in 3 different processes, 
			leaves surface accessible in first 2 processes 

...	
Process 2: Open Surface
...
Process 2: MapSurface - KErrNone (still accessible)
...
*/
void CTestDriverSecondProcess::OpenWaitMap()
	{
	
	// Find the surfaceId
	TSurfaceId id = iChunkWrapper->GetId();
	
	// Open Surface
	if(KErrNone == iSurfaceManager.OpenSurface(id))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	// Pass control back to the first process
    RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KMultiProcessSemaphore))
		{
		iTestResult |= ESecondTestPassed;
		}
    sem.Signal();

    RSemaphore sem2;
	if(KErrNone == sem2.OpenGlobal(KMultiProcessSemaphore2))
		{
		iTestResult |= EThirdTestPassed;
		}
    sem2.Wait();
    
    // Map surface
	RChunk handle;
	if(KErrNone == iSurfaceManager.MapSurface(id, handle))
		{
		iTestResult |= EFourthTestPassed;
		}

	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);

	sem.Close();
	sem2.Close();
	handle.Close();

	}


/**
Test 23/24/25/26: 	Test surface can be accessed when creating process dies /
					Test surface can be closed when creating process dies /
					Test surface can be closed from third process when 
					creating process dies and second process closes / 
					Test surface can't be accessed in a second process when open 
					and closed in the first process.

Process 2: Create Surface
...
Process 2: Kill Process
...
*/
void CTestDriverSecondProcess::CreateWaitKill()
	{
	// Setup attributes
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(20,80);    	// w > 0, h > 0
	attributes.iBuffers = 12; 				// > 0
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;	// 2bpp
	attributes.iStride = 250;				// > 0, < width * bpp
	attributes.iOffsetToFirstBuffer = 200;	// > 0, divisible by alignment
	attributes.iAlignment = 4;				// 1 || 2 || 4 || 8
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
    if(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId))
		{
		iTestResult |= EFirstTestPassed;
		}
    // Put the surfaceId onto the shared chunk
    iChunkWrapper->SetId(surfaceId);
 
    // Pass control back to the first process
    RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KMultiProcessSemaphore))
		{
		iTestResult |= ESecondTestPassed;
		}
    sem.Signal();
    
    RSemaphore sem2;
	if(KErrNone == sem2.OpenGlobal(KMultiProcessSemaphore2))
		{
		iTestResult |= EThirdTestPassed;
		}
    sem2.Wait();
    
    // Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
 
//  CleanupStack::PopAndDestroy(2,&sem);
	sem.Close();
	sem2.Close();
	}

/**
Test 27/28/29: 	Test closing doesn't prevent opening on another process
				Test closing doesn't prevent access on another process
				Test closing a surface in the creating process 
				when it has already been closed in a second process returns KErrNone

...
Process 2: Open Surface
Process 2: Close Surface
...
*/
void CTestDriverSecondProcess::OpenClose()
	{

	// Find the surfaceId
	TSurfaceId id = iChunkWrapper->GetId();
	
	// Open Surface
	if(KErrNone == iSurfaceManager.OpenSurface(id))
		{
		iTestResult |= EFirstTestPassed;
		}
		
	// Close Surface
	if(KErrNone == iSurfaceManager.CloseSurface(id))
		{
		iTestResult |= ESecondTestPassed;
		}
		
	// Pass control back to the first process
    RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KMultiProcessSemaphore))
		{
		iTestResult |= EThirdTestPassed;
		}
    sem.Signal();
    
    RSemaphore sem2;
	if(KErrNone == sem2.OpenGlobal(KMultiProcessSemaphore2))
		{
		iTestResult |= EFourthTestPassed;
		}
    sem2.Wait();
    
    // Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
    
	sem.Close();
	sem2.Close();
	}

/**
Test 30: Test a surface cannot be accessed in a second process if not opened

...
Process 2: Map Surface - KErrAccessDenied
Process 2: Surface Info - KErrAccessDenied
*/
void CTestDriverSecondProcess::MapSurfaceInfoCantAccess()
	{
	// Find the surfaceId
	TSurfaceId id = iChunkWrapper->GetId();
	
	// Map surface
	RChunk handle;
	if(KErrAccessDenied == iSurfaceManager.MapSurface(id, handle))
		{
		iTestResult |= EFirstTestPassed;
		}
		
	// Surface Info
	RSurfaceManager::TInfoBuf infoBuf;
	if(KErrAccessDenied == iSurfaceManager.SurfaceInfo(id, infoBuf))
		{
		iTestResult |= ESecondTestPassed;
		}
		
	// Pass control back to the first process
    RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KMultiProcessSemaphore))
		{
		iTestResult |= EThirdTestPassed;
		}
    sem.Signal();
    
    RSemaphore sem2;
	if(KErrNone == sem2.OpenGlobal(KMultiProcessSemaphore2))
		{
		iTestResult |= EFourthTestPassed;
		}
    sem2.Wait();
    
    // Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
    
//	CleanupStack::PopAndDestroy(3,&handle);
	sem.Close();
	sem2.Close();
	handle.Close();
	}

/**
Test 31: Test that a buffer written to in one surface can be read from in another 

Process 1: Create Surface
Process 1: Map Surface 
Process 1: Write to buffer
Process 2: Open the surface 
Process 2: Read from buffer 
*/	
void CTestDriverSecondProcess::TestReadFromBufferInSecondProcess()
	{
	// Find the surfaceId
	TSurfaceId id = iChunkWrapper->GetId();
	
	// Open Surface
	if(KErrNone == iSurfaceManager.OpenSurface(id))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	// Map surface
	RChunk handle;
	if(KErrNone == iSurfaceManager.MapSurface(id, handle))
		{
		iTestResult |= ESecondTestPassed;
		}
		
	// Read from the buffer
	RSurfaceManager::TInfoBuf infoBuf;
	if(KErrNone == iSurfaceManager.SurfaceInfo(id, infoBuf))
		{
		iTestResult |= EThirdTestPassed;
		}
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	TUint8* surfaceAdd = handle.Base();
	TInt offsetToFirstBuffer;
	if(KErrNone == iSurfaceManager.GetBufferOffset(id, 0, offsetToFirstBuffer))
  		{
		iTestResult |= EFourthTestPassed;
		}
	TUint8* bufferAdd = surfaceAdd + offsetToFirstBuffer;
	if(*bufferAdd == 134)
		{
		iTestResult |= EFifthTestPassed;
		}
	
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
    
	handle.Close();
	}
	
void CTestDriverSecondProcess::TestGetSurfaceHint()
	{
	// Open the chunk wrapper and get the surfaceId
	TSurfaceId surfaceId = iChunkWrapper->GetId();
		
	RSurfaceManager::THintPair hintPair;
	hintPair.iKey.iUid = 0x124578;
	if (KErrAccessDenied == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= ESecondTestPassed;
		}
	
	if (KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair))
		{
		iTestResult |= EThirdTestPassed;
		}
	
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
	
	}


void CTestDriverSecondProcess::TestSetSurfaceHint()
	{
	// Open the chunk wrapper and get the surfaceId
	TSurfaceId surfaceId = iChunkWrapper->GetId();
		
	RSurfaceManager::THintPair hintPair;
	hintPair.iKey.iUid = 0x124578;
	hintPair.iValue = 300;

	if (KErrAccessDenied == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= ESecondTestPassed;
		}
	
	if (KErrNone == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair))
		{
		iTestResult |= EThirdTestPassed;
		}
	RSurfaceManager::THintPair hintPairNew;
	hintPairNew.iKey.iUid = 0x124578;
	
	iSurfaceManager.GetSurfaceHint(surfaceId,hintPairNew);
	if (hintPairNew.iValue == hintPair.iValue)
		{
		iTestResult |= EFourthTestPassed;		
		}
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
	
	}


void CTestDriverSecondProcess::TestAddSurfaceHint()
	{
	// Open the chunk wrapper and get the surfaceId
	TSurfaceId surfaceId = iChunkWrapper->GetId();
		
	RSurfaceManager::THintPair hintPair;
	hintPair.iKey.iUid = 0x124580;
	hintPair.iValue = 300;
	hintPair.iMutable = ETrue;
	if (KErrAccessDenied == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	if(KErrNone == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= ESecondTestPassed;
		}
	
	if (KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair))
		{
		iTestResult |= EThirdTestPassed;
		}
	RSurfaceManager::THintPair hintPairNew;
	hintPairNew.iKey.iUid = 0x124580;
	
	iSurfaceManager.GetSurfaceHint(surfaceId,hintPairNew);
	if (hintPairNew.iValue == hintPair.iValue)
		{
		iTestResult |= EFourthTestPassed;		
		}
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
	
	}


void CTestDriverSecondProcess::TestOutofMemory()
	{
	// Open the chunk wrapper and get the surfaceId
	TSurfaceId surfaceId = iChunkWrapper->GetId();
	// Test OOM in OpenSurface()
	__KHEAP_SETFAIL(RHeap::EDeterministic, 1);
	if (KErrNoMemory == iSurfaceManager.OpenSurface(surfaceId))
		{
		iTestResult |= EFirstTestPassed;
		}
	__KHEAP_RESET;
	// Test OOM in AddConnection()
	RSurfaceManager surfaceManagerTest;
	__KHEAP_SETFAIL(RHeap::EDeterministic, 1);
	if (KErrNoMemory == surfaceManagerTest.Open())
		{
		iTestResult |= ESecondTestPassed;
		}
	__KHEAP_RESET;
	// Set the results so they can be read and tested by the first process
	iChunkWrapper->SetSecondProcessResults(iTestResult);
	}
// Real main function
void MainL()
	{
	
	test.Title();
	RDebug::Print(_L("marker"));

	test.Start(_L("Starting 2nd Process"));
	TInt testCase;
	User::GetTIntParameter(EMultiProcessSecondSlot, testCase);
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	
	CTestDriverSecondProcess* testDriver = CTestDriverSecondProcess::NewL(); 
	CleanupStack::PushL(testDriver);
	
	switch(testCase)
		{
	case ETestInfoReceivedSurface:
		testDriver->TestSurfaceInfoUsingSurfaceId();
		break;
	case ETestOpenReceivedSurface:
		testDriver->TestOpeningSurfaceUsingSurfaceId();
		break;
	case ETestOpenSurfaceInvalidParams:
		testDriver->TestOpeningSurfaceInvalidParams();	
		break;
	case EOpenWaitMap:
		testDriver->OpenWaitMap();
		break;	
	case ECreateWaitKill:
		testDriver->CreateWaitKill();
		break;
	case EOpenClose:
		testDriver->OpenClose();
		break;
	case EMapSurfaceInfoCantAccess:
		testDriver->MapSurfaceInfoCantAccess();
		break;
	case EReadFromBuffer:
		testDriver->TestReadFromBufferInSecondProcess();
		break;
	case ECheckHandle:
		testDriver->TestCheckHandleInSecondProcess();
		break;
	case ESyncOperation:
		testDriver->TestCheckSyncOperation();
		break;
	case ETestChannelMultiProcess1:
		testDriver->TestMultipleChannelsInSecondProcess1();
		break;
	case ETestChannelMultiProcess2:
		testDriver->TestMultipleChannelsInSecondProcess2();
		break;
	case EGetSurfaceHint:
		testDriver->TestGetSurfaceHint();
		break;
	case ESetSurfaceHint:
		testDriver->TestSetSurfaceHint();
		break;
	case EAddSurfaceHint:
		testDriver->TestAddSurfaceHint();
		break;
	case ECheckOutofMemory:
#ifdef _DEBUG
		testDriver->TestOutofMemory();
#endif
		break;
	default:
		User::Leave(KErrArgument);
		break;
		}
	CleanupStack::PopAndDestroy(testDriver);

	// Handle check
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		User::Leave(KErrGeneral);  // Thread-owned handles not closed
		}

		
	test.End();
	test.Close();
	}

// Cleanup stack harness
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	TRAPD(error, MainL());
	_LIT(KTSecondProcessPanic,"tsecondprocessmain");
	__ASSERT_ALWAYS(!error, User::Panic(KTSecondProcessPanic, error));
	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}
