// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef EGLTEST_SYNCOBJECT_H
#define EGLTEST_SYNCOBJECT_H

#include "eglteststep.h"
#include <test/egltestcommonsession.h>


_LIT(KSyncObject_Positive_Base, "SyncObject_Base");
NONSHARABLE_CLASS(CEglTest_SyncObject_Base) : public CEglTestStep
	{
public:
	CEglTest_SyncObject_Base();
	~CEglTest_SyncObject_Base();
	TBool GetProcAddress();
	void CreateEglSessionL();
	void CheckSyncAttrib(EGLint attribute, EGLint expectedValue);
	void CleanAll();
	
	/**
	@param aEndTime End time obtained from the user nano kernel tick counter
	@param aStartTime Start time obtained from the user nano kernel tick counter
	@return elapsed time in microsecond
	*/
	inline TInt ElapsedTime(TUint32 aEndTime, TUint32 aStartTime) const
		{
		return ((aEndTime - aStartTime) * iTickPeriodMicroSeconds);
		}

	// from CTestStep
	TVerdict doTestStepPreambleL();
	
protected:
	CTestEglSession* iEglSess;
	EGLSyncKHR iSyncObject;
	EGLSyncKHR iSyncObject1;
	EGLSyncKHR iSyncObject2;
	
	TBool iIsSyncObjectExtensionSupported; //sync object API, defined in EGL Khronos API spec with the name KHR_reusable_sync 
    TBool iIsSyncObjectPrivateExtensionSupported; //internal NOKIA extension which is not the part of the Khronos API spec 

	TInt iThreshold; //level of tolerance
	TInt iDelaySignalling; //delay before the signal occurs 
	TInt iWaitSyncTimeout; //timeout client waits on sync object, must be greater than iDelaySignalling  
	TInt iLongDelaySignalling; //long delay before signalling occurs, must be greater than iWaitSyncTimeout
	TInt iNumStressIterations; //number of iterations before stress test exits
	TInt iTimeOverhead;
	
	TFPtrEglCreateSyncKhr iPfnEglCreateSyncKHR;
	TFPtrEglDestroySyncKhr iPfnEglDestroySyncKHR;
	TFPtrEglClientWaitSyncKhr iPfnEglClientWaitSyncKHR;
	TFPtrEglSignalSyncKhr iPfnEglSignalSyncKHR;
	TFPtrEglGetSyncAttribKhr iPfnEglGetSyncAttribKHR;
	TFPtrEglPrivateSignalSyncNok iPfnEglPrivateSignalSyncNOK;
	
	TInt iTickPeriodMicroSeconds; //nanokernel ticks, in microseconds
	};

_LIT(KSyncObject_Positive_GetProcAddress, "SyncObject_Positive_GetProcAddress");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_GetProcAddress) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
	};

_LIT(KSyncObject_Positive_CreateDestroy, "SyncObject_Positive_CreateDestroy");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_CreateDestroy) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
	};

_LIT(KSyncObject_Positive_WaitSignal, "SyncObject_Positive_WaitSignal");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_WaitSignal) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KSyncObject_Positive_WaitSignal2, "SyncObject_Positive_WaitSignal2");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_WaitSignal2) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KSyncObject_Positive_WaitDelete, "SyncObject_Positive_WaitDelete");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_WaitDelete) : public CEglTest_SyncObject_Base
    {
public: 
    TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doThreadFunctionL(TInt aIdx);
    };

_LIT(KSyncObject_Positive_WaitWithTimeoutExpired, "SyncObject_Positive_WaitWithTimeoutExpired");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_WaitWithTimeoutExpired) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
	};

_LIT(KSyncObject_Positive_SignalBeforeTimeoutExpired, "SyncObject_Positive_SignalBeforeTimeoutExpired");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_SignalBeforeTimeoutExpired) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KSyncObject_Positive_DeleteBeforeTimeoutExpired, "SyncObject_Positive_DeleteBeforeTimeoutExpired");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_DeleteBeforeTimeoutExpired) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KSyncObject_Positive_Multithread_SignalBeforeTimeout, "SyncObject_Positive_Multithread_SignalBeforeTimeout");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_Multithread_SignalBeforeTimeout) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KSyncObject_Positive_Multithread_SignalAfterTimeout, "SyncObject_Positive_Multithread_SignalAfterTimeout");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_Multithread_SignalAfterTimeout) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KSyncObject_Positive_Multithread_WaitOnTwoSyncObject, "SyncObject_Positive_Multithread_WaitOnTwoSyncObject");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_Multithread_WaitOnTwoSyncObject) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KSyncObject_Positive_Wait_TestMode, "SyncObject_Positive_Wait_TestMode");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_Wait_TestMode) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
	};

_LIT(KSyncObject_Positive_Multithread_Deletion, "SyncObject_Positive_Multithread_Deletion");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_Multithread_Deletion) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KSyncObject_Positive_Stress, "SyncObject_Positive_Stress");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_Stress) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
private:
	TBool iStopTest; //to ensure that we are not going to signal on deleted sync objects 
	TInt iLastThreadToSignal; //varies from 0..2, where 2 is the main client thread 
	};

_LIT(KSyncObject_Positive_WaitFlush, "SyncObject_Positive_WaitFlush");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_WaitFlush) : public CEglTest_SyncObject_Base
{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
};

_LIT(KSyncObject_Positive_Terminate, "SyncObject_Positive_Terminate");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_Terminate) : public CEglTest_SyncObject_Base
    {
public: 
    TVerdict doTestStepL();
    };

_LIT(KSyncObject_Positive_TerminateBeforeTimeoutExpired, "SyncObject_Positive_TerminateBeforeTimeoutExpired");
NONSHARABLE_CLASS(CEglTest_SyncObject_Positive_TerminateBeforeTimeoutExpired) : public CEglTest_SyncObject_Base
    {
public: 
    TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doThreadFunctionL(TInt aIdx);
    };

_LIT(KSyncObject_Negative_WrongParameters, "SyncObject_Negative_WrongParameters");
NONSHARABLE_CLASS(CEglTest_SyncObject_Negative_WrongParameters) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
	};

_LIT(KSyncObject_Negative_SignalImpl, "SyncObject_Negative_SignalImpl");
NONSHARABLE_CLASS(CEglTest_SyncObject_Negative_SignalImpl) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
	};

_LIT(KSyncObject_Negative_OOM, "SyncObject_Negative_OOM");
NONSHARABLE_CLASS(CEglTest_SyncObject_Negative_OOM) : public CEglTest_SyncObject_Base
	{
public:	
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

#endif // EGLTEST_SYNCOBJECT_H
