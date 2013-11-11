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
*/

#ifndef EGLTESTCOMMONSTEP_H
#define EGLTESTCOMMONSTEP_H

#include <e32msgqueue.h>
#include <EGL/egl.h>
#include <VG/openvg.h>
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <sgresource/sgresource.h>
#else
#include <graphics/sgresource.h>
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <test/testexecuteserverbase.h>
#include <test/ttmsgraphicsstep.h>
#include <test/egltestcommonutils.h>
#include <test/egltestcommonsession.h>

const TInt KDefaultWindowGroupId = 200;
const TInt KMaxProcessNumber = 32;
const TInt KMaxThreadNumber = 100;
const TInt KNumSemaphore = 2;

class RWindow;

struct TEglStepMessageBuffer
    {
    TInt iBuf[64];
    };

/**
Abstract base class for Egl test steps
*/
class CEglTestStep : public CTTMSGraphicsStep
	{
	friend class EglTestCommonProcess;

public:
	struct TThreadInfo
		{
		CEglTestStep*			 iSelf;
		TInt 					 iIdx; // << index for example used to identify the thread to semaphores
		} iThreadInfos[KMaxThreadNumber];

	struct TThreadStatus
		{
		TThreadStatus();
		enum TStatusId{ERendezvous, ELogin, ESize};
		RThread			iThread;
		TRequestStatus	iStatus[ESize];
		};

	struct TProcessStatus
		{
		RProcess		iProcess;
		TRequestStatus	iStatus;
		} iProcessStatus[KMaxProcessNumber];

	// from CTestStep
	IMPORT_C TVerdict doTestStepPreambleL();
	IMPORT_C TVerdict doTestStepPostambleL();
	IMPORT_C virtual ~CEglTestStep();
    
	IMPORT_C void PartialInitialiseL(const TDesC& aStepName);
	IMPORT_C void CleanAll();
	IMPORT_C void CreateEglSessionL(TInt aIdx=0);
	inline CTestEglSession* GetEglSess() {return iEglSess;};
	
protected:
	IMPORT_C  CEglTestStep();
 
	// Initialisation
	void SetLoggerForProcessWrapperL();
	
	// Utility Methods
	void HandleMark();
	void HandleMarkEnd();
	IMPORT_C void OpenWsSessionL(TInt aGroupId);
	IMPORT_C void CloseWsSession();
	IMPORT_C void ConstructWindowL(RWindow& aWindow, const TRect& aRect);
    IMPORT_C void PrintUsedPixelConfiguration();
    IMPORT_C void PrintPixelFormat(TUidPixelFormat aFormat);
    IMPORT_C void PrintVGImageFormat(VGImageFormat aAttr);

	// Multi process test utils
	IMPORT_C void Test_MultiProcessL(const TDesC& aTestDllName, TInt aProcessCount, const TDesC& aTestStepName);
	IMPORT_C void Test_MultiProcessL(const TDesC& aTestDllName, TInt aProcessCount, const TDesC& aTestStepName, const TSgDrawableId& aSgId);
	IMPORT_C void Test_MultiProcessL(const TDesC& aTestDllName, TInt aProcessCount, const TDesC& aTestStepName, const RArray<TSgDrawableId>& aSgIdList);
	void CheckProcessStatusL(TInt aIndex, const TRequestStatus& aStatus, const RProcess& aProcess);
	inline TInt ImageIndexFromProcessId(TInt aProcess, TInt aImageCount) {return (aProcess % aImageCount);}
	
	// Multi threaded test utils
	IMPORT_C void Test_MultiThreadL(TInt aThreadCount, TBool aWaitForCompletion);
	IMPORT_C void Test_MultiThread_WaitL(TBool aCloseThreads, TThreadStatus::TStatusId aStatusId);
	void CheckThreadStatusL(TInt aIndex, const TRequestStatus& aStatus, const RThread& aThread);
	static TInt ThreadFunction(TAny* aSelf);
	void ThreadFunctionL(TThreadInfo& aInfo);

	IMPORT_C virtual void doThreadFunctionL(TInt aIdx);
	IMPORT_C virtual void doThreadFunctionL(TInt aIdx,const TSgDrawableId& aSgId);
	IMPORT_C void Rendezvous(TInt aIdx);
	IMPORT_C virtual void doProcessFunctionL(TInt aIdx);
	IMPORT_C virtual void doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId);

	// EGL helpers
	IMPORT_C TBool CheckForExtensionL(TInt aExtensions, const TDesC& aExtensionName = KNullDesC);
	IMPORT_C void GetDisplayL();
	IMPORT_C void TerminateDisplayL();	

	virtual void ReceiveMessageFromClient(RMsgQueue<TEglStepMessageBuffer>& /*aMessageQueueClientProcParam*/) {}
protected:
	// Session object as required for most tests
	CTestEglSession* 	iEglSess;

	// Whether to output verbose logging
	TBool iVerboseLogging;
	 
	// Whether the test step needs to wait for thread completion during postamble 
	TInt  iThreadCount;
	TBool iWaitForCompletionOnPostamble;
	
	// The display is shared between threads.
	EGLDisplay	iDisplay;

	RSemaphore	iSemaphore[KNumSemaphore];
	RArray<TThreadStatus> iThreadStatus;
	
	// pixel formats the test is run
	TUidPixelFormat iSourceFormat;
	VGImageFormat iSurfaceFormat;

	TInt iProcHandleMark;
	TInt iThreadHandleMark;
	TInt iProcHandleMarkEnd;
	TInt iThreadHandleMarkEnd;

	RWsSession iWsSession;
	RWindowGroup iWindowGroup;
	};
	
#endif // EGLTESTCOMMONSTEP_H
