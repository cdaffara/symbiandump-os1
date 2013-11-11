// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TSURFACEUPDATE_H__
#define __TSURFACEUPDATE_H__

#include <test/testexecutestepbase.h>
#include <test/tgraphicsharness.h>
#include "surfaceupdateserver.h"
#include "tcompositionbackend.h"
#include "surfaceupdateclient.h"


class CTSurfaceUpdate : public CTGraphicsBase
	{
public:
	CTSurfaceUpdate(CTestStep* aStep);
	~CTSurfaceUpdate();
	void ConstructL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	// Test cases
	void TestCase1();
	void TestCase2L();
	void TestCase3L();
	void TestCase4L();
	void TestCase5L();
	void TestCase6L();
	void TestCase7L();
	void TestCase8();
	void TestCase9L();
	void TestCase10();
	void TestCase11();
    void TestCase12L();
	void TestCase13L();
	void TestCase14();
	void TestCase15();
	void TestCaseNotifyWhenAvailableGlobal();
	void TestCaseRegisterIdenticalPrioritiesL();
	void TestCaseTerminateServer();
	void TestCase16L();
	void TestCase17();
	void TestCase18();

	// Utility functions
	void SetupContentUpdateReceiversL();
	void ResetSusL(); //note that surface update server provider will also be updated at this point 
	void GenerateMultipleUpdateWithPanic();
	static TInt PanicClientThread(TAny*);
	static TInt PanicClientThread1(TAny*);
	static TInt TestMultipleNotifyThreadL(TAny*);
	static TInt TestServerStartupTwoThreads(TAny*);
	void SetupTestCaseRegisterPanicL();
	void DoTestCaseRegisterPanicL(TThreadFunction aThreadFunction);
	void WaitUntilAllNotificationsProcessed();
	inline MSurfaceUpdateServerProvider* Provider(); //cache the pointer with care, as restarting the server will invalidate it
private:
	RPointerArray<CTContentUpdateReceiver> iReceivers;
	CRequestOrder* iSurfaceOrder;
	};

class CTSurfaceUpdateStep : public CTGraphicsStep
	{
public:
	CTSurfaceUpdateStep();
	MSurfaceUpdateServerProvider* SurfaceUpdateProvider() 
		{
		return iSurfaceUpdateProvider;
		}
	TInt StartServer();
	void TerminateServer();
public:
	TBool iGce;    //Flags that symbian_graphics_use_gce is defined
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
protected:
	MSurfaceUpdateServerProvider *iSurfaceUpdateProvider;
	};

_LIT(KTSurfaceUpdateStep,"TSurfaceUpdate");

// Class to do improper things on client side and check that server handles them as expected
class RMisbehavingSession : public RSessionBase
	{
public:
	TInt Connect(TInt aMessageSlots = KDefaultMessageSlot, TBool aWrongVersion = EFalse);
	void Close();
	void NotifyWhenAvailableNoChecking(TRequestStatus& aStatus);
	void NotifyWhenDisplayedNoChecking(TRequestStatus& aStatus);
	void NotifyWhenDisplayedXTimesNoChecking(TInt aCount, TRequestStatus& aStatus);
	TInt SubmitUpdateNoChecking(TInt aScreen, const TSurfaceId& aSurfaceId, 
									TInt aBuffer, const TRegion* aDirtyRegion = NULL);
	void NotifyWhenAvailable(TRequestStatus& aStatus);
	void NotifyWhenDisplayed(TRequestStatus& aStatus, TTimeStamp& aTimeStamp);
	void NotifyWhenDisplayedXTimes(TInt aCount, TRequestStatus& aStatus);
	TInt SubmitUpdate(TInt aScreen, const TSurfaceId& aSurfaceId, 
									TInt aBuffer, const TRegion* aDirtyRegion = NULL);
	void CancelAllUpdateNotifications();
	void TestPassingInvalidOpcode(TInt aInvalidOpcode =500);
	void SetFailRate(TInt aFailRate);
private:
	TVersion Version() const;
	TVersion WrongVersion() const;

private:
	TRequestStatus *iStatusAvailable;
	TRequestStatus *iStatusDisplayed;
	TRequestStatus *iStatusDisplayedXTimes;
	TTimeStamp* iTimeStamp;
	TInt iCount;
	};

// Class to pass status members into a thread
class TMultipleNotify
	{
public:
	TMultipleNotify(TRequestStatus& aStatus, TInt& aTestNum) :
		iStatus(aStatus),
		iTestNum(aTestNum)
		{}
	TRequestStatus& iStatus;
	TInt& iTestNum;
	};

inline MSurfaceUpdateServerProvider* CTSurfaceUpdate::Provider()
	{
	return (static_cast <CTSurfaceUpdateStep*> (iStep)) -> SurfaceUpdateProvider();
	}


#endif	// __TSURFACEUPDATE_H__
