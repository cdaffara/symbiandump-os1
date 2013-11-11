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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#if !defined(__TSURFACEMANAGERMULTITHREAD_H__)
#define __TSURFACEMANAGERMULTITHREAD_H__

#include <test/tgraphicsharness.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include "tsmgmultprocessshared.h"

/**
Enums for passing between the multi-threads within the testing process, 
used to define which tests should be run on the sides of other threads.
*/
enum TThreadTestCase
	{
	ECreateSurfaceMapInfo,
	ECreateSurfaceClose,
	EOpenCloseSurface,
	EOpenKillSurface,
	ECreateKillSurface,
	EOpenCloseSurfaceMultiThread,
	EOpenMapSurfaceInfoMultiThread,
	EMapSurfaceInfo,
	ECloseSurfaces,
	EOpenCloseMapSurfaceInfoMultiThread,
	EOpenCloseOpenMultiThread,
	ECloseBothSurfaces,
	ENoTestCase
	};
	
/**
Enums for single test results e.g. OpenSurface
*/	
enum TSingleTestResult
	{
	EAllZero = 0x00000000,
	EOpenSurfaceTestPassed = 0x00000001,
	ECloseSurfaceTestPassed = 0x00000010,
	EMapSurfaceTestPassed = 0x00000100,
	ESurfaceInfoTestPassed = 0x00001000,
	ESurfaceInfoTestPassed2 = 0x00004000,
	ECreateSurfaceTestPassed = 0x00010000,
	EOpenSurfaceTestFailed = 0x00000002,
	ECloseSurfaceTestFailed = 0x00000020,
	EMapSurfaceTestFailed = 0x00000200,
	ESurfaceInfoTestFailed = 0x00002000,
	ESurfaceInfoTestFailed2 = 0x00008000,
	ECreateSurfaceTestFailed = 0x00020000,
	};

_LIT(KTSurfaceManagerMultiThreadStep,"TSurfaceManagerMultiThread");


/**
A class to wrap up the execution in the other threads
*/
class CChildThreadWrapper : public CBase
	{
public:
	CChildThreadWrapper(TAny* aInfo);
	CChildThreadWrapper();
	~CChildThreadWrapper();
	void CreateSurfaceThread();
	void MapSurfaceInfo();
	void CloseSurface();
	void OpenSurface();
	void CloseBothSurfaces();
public:
	RSurfaceManager& iSurfaceManager;
	TSurfaceId& iSurfaceId;
	TSurfaceId iSurfaceIdNew;
	TThreadTestCase iThreadTestCase;
	};
class CTSurfaceManagerMultiThread : public CTGraphicsBase
	{
public:
	CTSurfaceManagerMultiThread(CTestStep* aStep);
	~CTSurfaceManagerMultiThread();
	
	friend class CChildThreadWrapper;
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestAccessSurfaceWithoutOpeningL();
	void TestCloseSurfaceWithoutOpeningL();
	void TestAccessSurfaceClosedThreadL();
	void TestCloseSurfaceClosedThreadL();
	void TestAccessSurfaceOpenedClosedThreadL();
	void TestCloseSurfaceOpenedClosedThreadL();
	void TestAccessSurfaceOpenedKilledThreadL();
	void TestCloseSurfaceOpenedKilledThreadL();
	void TestAccessSurfaceCreateKilledThreadL();
	void TestCloseSurfaceCreateKilledThreadL();
	void TestAccessSurfaceThreeThreadsL();
	void TestAccessSurfaceDieCloseOtherThreadsL();
	void TestOpenSurfaceDieCloseOtherThreadsL();
	void TestAccessSurfaceInExistingSharedChunkL();
	void TestCloseSurfaceExistingSharedChunkL();
	
	static void CloseSurfaceWhenLeave(TAny* aSurfaceId) ;
public:
/**
Information to be passed into the rendering threads
*/
struct TInfo
	{
	RSurfaceManager iSurfaceManager;
	TSurfaceId iSurfaceId;
	TSurfaceId iSurfaceIdNew;
	TThreadTestCase iThreadTestCase;
	};
	
	static TInt ThreadSecondStart(TAny* aInfo);
	static TInt ThreadThirdStart(TAny* aInfo);

private:
	RSurfaceManager iSurfaceManager;
	TSurfaceId iSurfaceId;
	TSurfaceId iSurfaceIdNew;
	TInfo iInfo;
	TInfo iInfo2;
	
	};

	
class CTSurfaceManagerMultiThreadStep : public CTGraphicsStep
	{
public:
	CTSurfaceManagerMultiThreadStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};


_LIT(KMultiThreadSemaphore, "Multi Thread Semaphore");
_LIT(KMainThreadSemaphore, "Main Thread Semaphore");

#endif
