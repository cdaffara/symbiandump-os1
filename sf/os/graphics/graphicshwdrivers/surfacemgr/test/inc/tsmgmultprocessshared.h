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
// surface manager API
// 
//

#ifndef __SHARED_H__
#define __SHARED_H__

#include <e32base.h>
#include <e32cmn.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <test/tgraphicsharness.h>

struct TLeaveInfo
	{
	RSurfaceManager iSurfaceManager;
	TSurfaceId iSurfaceId;
	};

static void CloseSurfaceWhenLeave(TAny* aInfo)
	{
	TInt err = ((TLeaveInfo*)aInfo)->iSurfaceManager.CloseSurface(((TLeaveInfo*)aInfo)->iSurfaceId);
	if (err != KErrNone)
		RDebug::Print(_L("Error closing surface = %d\n"),err);	
	};
 

/**
Enum for the slots for inter-process communication
*/
enum TIPCSlot
	{
	EMultiProcessFirstSlot = 0,
	EMultiProcessSecondSlot,
	EMultiProcessThirdSlot,
	EMultiProcessFourthSlot,
	};

/**
Enums for passing between the test process and the second process used in testing,
used to define which test should be run on the side of the second process.
*/
enum TMultiProcessTestCase
	{
	// Second process opcodes
	ETestOpenReceivedSurface,
	ETestInfoReceivedSurface,
	ETestOpenSurfaceInvalidParams,
	ECreateWaitKill,
	EOpenClose,
	EMapSurfaceInfoCantAccess,
	EOpenWaitMap,
	EReadFromBuffer,
	ECheckHandle,
	ESyncOperation,
	ETestChannelMultiProcess1,
	ETestChannelMultiProcess2,
	EGetSurfaceHint,
	ESetSurfaceHint,
	EAddSurfaceHint,
	ECheckOutofMemory,
	// Third process opcodes
	ECloseSurface = 0x00010000,
	EOpenWaitCloseOpen,
	EOpenSurface,
	ESurfaceInfo
	};

/**
Enums for panics between my testing process and my second process
*/
enum TCrossProcessPanics
	{
	EInvalidTestCase,
	};

/**
Enums for the test results
*/
enum TTestResult
	{
	EFirstTestPassed = 0x00000001,
	ESecondTestPassed = 0x00000002,
	EThirdTestPassed = 0x00000004,
	EFourthTestPassed = 0x00000008,
	EFifthTestPassed = 0x00000010,
	ESixthTestPassed = 0x00000020,
	ESeventhTestPassed = 0x00000040,
	EEighthTestPassed = 0x00000080,
	ENinthTestPassed = 0x00000100,
	ETenthTestPassed = 0x00000200,
	};

_LIT(KMultiProcessSemaphore, "Multi Process Semaphore");
_LIT(KMultiProcessSemaphore2, "Multi Process Semaphore 2");
_LIT(KMultiProcessSemaphore3, "Multi Process Semaphore 3");
_LIT(KMultiProcessSemaphore4, "Multi Process Semaphore 4");
_LIT(KSharedChunkName, "Surfaces test shared chunk");


const TInt KSharedChunkSize = ((4 * sizeof(TUint32)) + (2 * sizeof(TInt)));
	
/**
Wraps a shared chunk for communication between the processes
*/
class CChunkWrapper : public CBase
	{
public:
	~CChunkWrapper();
	static CChunkWrapper* CreateL(const TDesC& aName, TInt aSize, TInt aMaxSize);
	void CreateConstructL(const TDesC& aName, TInt aSize, TInt aMaxSize);
	static CChunkWrapper* OpenL(const TDesC& aName, TBool aIsReadOnly);
	void OpenConstructL(const TDesC& aName, TBool aIsReadOnly);
	void SetSecondProcessResults(TInt aResult);
	TInt GetSecondProcessResults();
	void SetThirdProcessResults(TInt aResult);
	TInt GetThirdProcessResults();
	void SetId(const TSurfaceId& aId);
	TSurfaceId GetId();
	void CloseD();
private:	
	CChunkWrapper();
private:
	RChunk iChunk;
	};


class CTestDriver : public CBase
	{
public:
	CTestDriver();
	virtual ~CTestDriver();
	void ConstructL();
protected:
	RSurfaceManager iSurfaceManager;
	CChunkWrapper* iChunkWrapper;
	TInt iTestResult;
	};

#endif
