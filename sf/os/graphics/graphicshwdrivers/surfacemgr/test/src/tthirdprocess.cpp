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

#include <e32test.h>
#include <e32std.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include "tsmgmultprocessshared.h"

LOCAL_D RTest test(_L("TThirdProcess"));


class CTestDriverThirdProcess : public CTestDriver
{
public:
	CTestDriverThirdProcess();
	static CTestDriverThirdProcess* NewL();
public:
	void CloseSurface();
	void SurfaceInfo();
	void OpenWaitCloseOpen();
	void OpenSurface();
};

CTestDriverThirdProcess::CTestDriverThirdProcess():CTestDriver()
	{}


CTestDriverThirdProcess* CTestDriverThirdProcess::NewL()
	{
	CTestDriverThirdProcess * driver = new (ELeave) CTestDriverThirdProcess();
	CleanupStack::PushL(driver);
	driver->CTestDriver::ConstructL();
	CleanupStack::Pop(driver);
	return driver;
	}

void CTestDriverThirdProcess::CloseSurface()
	{
	TSurfaceId id = iChunkWrapper->GetId();
	// Close Surface
	if(KErrAccessDenied == iSurfaceManager.CloseSurface(id))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	iChunkWrapper->SetThirdProcessResults(iTestResult);
	}

void CTestDriverThirdProcess::SurfaceInfo()
	{
	TSurfaceId id = iChunkWrapper->GetId();
	// SurfaceInfo
	// Call Surface Info on the surface in this process
	RSurfaceManager::TInfoBuf infoBuf;
	if(KErrAccessDenied == iSurfaceManager.SurfaceInfo(id,infoBuf))// This is correct?
		{
		iTestResult |= EFirstTestPassed;
		}
	
	iChunkWrapper->SetThirdProcessResults(iTestResult);
	}

/**
Test 25: Test surface can be accessed from third process when creating 
			process dies and second process closes

...			
Process 3: Open Surface
...
Process 3: Close Surface - KErrNone
Process 3: Open Surface - KErrArgument
*/	
void CTestDriverThirdProcess::OpenWaitCloseOpen()
	{
	TSurfaceId id = iChunkWrapper->GetId();
	
	// Open the surface using the surfaceId
	if(KErrNone == iSurfaceManager.OpenSurface(id))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	// Pass control back to the first process
    RSemaphore sem3;
	if(KErrNone == sem3.OpenGlobal(KMultiProcessSemaphore3))
		{
		iTestResult |= ESecondTestPassed;
		}

    User::After(100000);
    sem3.Signal();
    
    RSemaphore sem4;
	if(KErrNone == sem4.OpenGlobal(KMultiProcessSemaphore4))
		{
		iTestResult |= EThirdTestPassed;
		}

    sem4.Wait();
	
	// Close the surface
	if(KErrNone == iSurfaceManager.CloseSurface(id))
		{
		iTestResult |= EFourthTestPassed;
		}
	
	// Open the surface
	if(KErrArgument == iSurfaceManager.CloseSurface(id))
		{
		iTestResult |= EFifthTestPassed;
		}
	
	iChunkWrapper->SetThirdProcessResults(iTestResult);
	
	sem3.Close();
	sem4.Close();
	}

/**
Test 27: Test closing on one process doesn't prevent opening on other processes, 
			provided one process still owns surface

...
Process 3: Open Surface - KErrNone
...
*/	
void CTestDriverThirdProcess::OpenSurface()
	{	
	TSurfaceId id = iChunkWrapper->GetId();
	
	// Open Surface
	if(KErrNone == iSurfaceManager.OpenSurface(id))
		{
		iTestResult |= EFirstTestPassed;
		}
	
	iChunkWrapper->SetThirdProcessResults(iTestResult);
	
	}

// Real main function
void MainL()
	{
	test.Title();

	test.Start(_L("Staring 3rd process"));
	TInt testCase;
	User::GetTIntParameter(EMultiProcessSecondSlot, testCase);
	
	CTestDriverThirdProcess* testDriver = CTestDriverThirdProcess::NewL(); 

	switch(testCase)
		{
	case ECloseSurface:
		testDriver->CloseSurface();
		break;
	case ESurfaceInfo:
		testDriver->SurfaceInfo();
		break;
	case EOpenWaitCloseOpen:
		testDriver->OpenWaitCloseOpen();
		break;
	case EOpenSurface:
		testDriver->OpenSurface();
		break;
	default:
		User::Leave(KErrArgument);
		break;
		}

	delete testDriver;
	test.End();
	test.Close();
	}

// Cleanup stack harness
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	TRAPD(error, MainL());
	_LIT(KTThirdProcessPanic,"tthirdprocessmain");
	__ASSERT_ALWAYS(!error, User::Panic(KTThirdProcessPanic, error));
	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}
