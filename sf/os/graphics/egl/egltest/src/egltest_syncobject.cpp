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
#include "egltest_syncobject.h"
#include <test/tefunit.h>
#include <hal.h>
#include <test/egltestcommonsgimageinfo.h>

const TUint32  KThreshold = 0.5*1000*1000; // 0.5 sec
const TUint32  KDelaySignalling = 2.5*1000*1000; // 2.5 sec
const TUint32  KWaitSyncTimeout = 5*1000*1000; // 5 sec
const TUint32  KLongDelaySignalling = 7.5*1000*1000; // 7.5 sec
const TInt 	   KNumStressIterations = 1000;

typedef void 	(*TEglDebugHeapMarkStartPtr)		(void);
typedef EGLint	(*TEglDebugHeapMarkEndPtr)			(EGLint count);
typedef void	(*TEglDebugSetBurstAllocFailPtr)	(EGLenum type, EGLint rate, EGLint burst);

struct TSurfaceToDestroy
	{
	EGLSurface iSurface;
	EGLDisplay	iDisplay;
	};

//the function clean up surface resource. Needs to be push into cleanup stack beforehand
void DestroySurface(TAny* aAny)
	{
	TSurfaceToDestroy* surfaceToDestroy = (TSurfaceToDestroy*)aAny;
	eglDestroySurface(surfaceToDestroy->iDisplay, surfaceToDestroy->iSurface);
	}

CEglTest_SyncObject_Base::CEglTest_SyncObject_Base() : 
	iSyncObject(EGL_NO_SYNC_KHR), iSyncObject1(EGL_NO_SYNC_KHR), iSyncObject2(EGL_NO_SYNC_KHR), 
	iThreshold(KThreshold), iDelaySignalling(KDelaySignalling),
	iWaitSyncTimeout(KWaitSyncTimeout), iLongDelaySignalling(KLongDelaySignalling),
	iNumStressIterations(KNumStressIterations)
	{
	}

CEglTest_SyncObject_Base::~CEglTest_SyncObject_Base()
	{
	if(iPfnEglDestroySyncKHR)
	    {
	    iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	    iPfnEglDestroySyncKHR(iDisplay, iSyncObject1);
	    iPfnEglDestroySyncKHR(iDisplay, iSyncObject2);
	    }
	CleanAll();
	}

// from CTestStep
TVerdict CEglTest_SyncObject_Base::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CEglTest_SyncObject_Base::doTestStepPreambleL()"));
	CEglTestStep::doTestStepPreambleL();
	TInt res = HAL::Get(HALData::ENanoTickPeriod, iTickPeriodMicroSeconds);
	if(res != KErrNone)
		{
		ERR_PRINTF2(_L("Can't retrieve tick period, err %d"), res);
		User::Leave(res);
		}
	INFO_PRINTF2(_L("Tick period in micro seconds %d"), iTickPeriodMicroSeconds);
	
	_LIT(KKeyThreshold, "Threshold");
	_LIT(KKeyDelaySignalling, "DelaySignalling");
	_LIT(KKeyWaitSyncTimeout, "WaitSyncTimeout");
	_LIT(KKeyLongDelaySignalling, "LongDelaySignalling");
	_LIT(KKeyNumStressIterations, "NumIterations");

	//retrive all setting from INI file
	GetIntFromConfig(ConfigSection(), KKeyThreshold, iThreshold);
	GetIntFromConfig(ConfigSection(), KKeyDelaySignalling, iDelaySignalling);
	GetIntFromConfig(ConfigSection(), KKeyWaitSyncTimeout, iWaitSyncTimeout);
	GetIntFromConfig(ConfigSection(), KKeyLongDelaySignalling, iLongDelaySignalling);
	GetIntFromConfig(ConfigSection(), KKeyNumStressIterations, iNumStressIterations);
	
	INFO_PRINTF2(_L("Level of tolerance %d"), iThreshold);
	INFO_PRINTF2(_L("Delay before the signal occurs  %d"), iDelaySignalling);
	INFO_PRINTF2(_L("Timeout client waits on sync object %d"), iWaitSyncTimeout);
	INFO_PRINTF2(_L("Long delay before signalling occurs %d"), iLongDelaySignalling);
	INFO_PRINTF2(_L("Number of iterations before stress test exits %d"), iNumStressIterations);

	INFO_PRINTF1(_L("Check if the Sync object Khronos extension is supported"));
	iIsSyncObjectExtensionSupported = CheckForExtensionL(KEGL_KHR_reusable_sync);

    INFO_PRINTF1(_L("Check if the Sync object private extension is supported"));
	iIsSyncObjectPrivateExtensionSupported = CheckForExtensionL(KEGL_NOK__private__signal_sync);
	
	CleanAll();
    TEST(GetProcAddress());
	INFO_PRINTF1(_L("End of CEglTest_SyncObject_Base::doTestStepPreambleL()"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Base::CreateEglSessionL()
	{
	delete iEglSess; //just in case it was called twice
	iEglSess = CTestEglSession::NewL(Logger(), iDisplay, 0);
	}

void CEglTest_SyncObject_Base::CheckSyncAttrib(EGLint aAttribute, EGLint aExpectedValue)
	{
	EGLint value;
	EGLBoolean res = iPfnEglGetSyncAttribKHR(iDisplay,iSyncObject,aAttribute,&value);
	TEST(res == EGL_TRUE);
	TEST(value == aExpectedValue);
	
	switch(aAttribute)
		{
		case EGL_SYNC_TYPE_KHR:
			{
			if (value == EGL_SYNC_REUSABLE_KHR)
				{
				INFO_PRINTF1(_L("  EGL_SYNC_TYPE_KHR: EGL_SYNC_REUSABLE_KHR"));
				}
			else
				{
				ERR_PRINTF1(_L("  EGL_SYNC_TYPE_KHR: Invalid attribute"));
				}
			break;
			}
		case EGL_SYNC_STATUS_KHR:
			{
			if (value == EGL_SIGNALED_KHR)
				{
				INFO_PRINTF1(_L("  EGL_SYNC_STATUS_KHR: EGL_SIGNALED_KHR"));
				}
			else if (value == EGL_UNSIGNALED_KHR)
				{
				INFO_PRINTF1(_L("  EGL_SYNC_STATUS_KHR: EGL_UNSIGNALED_KHR"));
				}
			else
				{
				ERR_PRINTF1(_L("  EGL_SYNC_STATUS_KHR: Invalid attribute"));
				}
			break;
			}
		default:
			{
			ERR_PRINTF1(_L("  Invalid attribute"));
			}
		}
	}

/**
 Obtain extension functions for the sync object 
 */
TBool CEglTest_SyncObject_Base::GetProcAddress()
	{
    if(!iIsSyncObjectExtensionSupported)
        {
        INFO_PRINTF1(_L("Sync object extension is not defined in test INI file.\
The retrieval of the pointers to the extension sync object function will be skipped"));
        return ETrue;
        }

    //retrieve the pointers to the EGL sync object extension functions 
    INFO_PRINTF1(_L("retrieve the pointers to the EGL sync object extension functions"));
    
	iPfnEglCreateSyncKHR = reinterpret_cast <TFPtrEglCreateSyncKhr> (eglGetProcAddress("eglCreateSyncKHR"));
	iPfnEglDestroySyncKHR = reinterpret_cast <TFPtrEglDestroySyncKhr> (eglGetProcAddress("eglDestroySyncKHR"));
	iPfnEglClientWaitSyncKHR = reinterpret_cast <TFPtrEglClientWaitSyncKhr> (eglGetProcAddress("eglClientWaitSyncKHR"));
	iPfnEglSignalSyncKHR = reinterpret_cast <TFPtrEglSignalSyncKhr> (eglGetProcAddress("eglSignalSyncKHR"));
	iPfnEglGetSyncAttribKHR = reinterpret_cast <TFPtrEglGetSyncAttribKhr> (eglGetProcAddress("eglGetSyncAttribKHR"));
    if(iIsSyncObjectPrivateExtensionSupported)
        {
        iPfnEglPrivateSignalSyncNOK = reinterpret_cast <TFPtrEglPrivateSignalSyncNok> (eglGetProcAddress("egl_Private_SignalSyncNOK"));
        if(!iPfnEglPrivateSignalSyncNOK)
            {
            ERR_PRINTF1(_L("   Pointer to function \"egl_Private_SignalSyncNOK\" is not valid"));
            }
        }
	if(!iPfnEglCreateSyncKHR)
		{
		ERR_PRINTF1(_L("   Pointer to function \"eglCreateSyncKHR\" is not valid"));
		}
	if(!iPfnEglDestroySyncKHR)
		{
		ERR_PRINTF1(_L("   Pointer to function \"eglDestroySyncKHR\" is not valid"));
		}
	if(!iPfnEglClientWaitSyncKHR)
		{
		ERR_PRINTF1(_L("   Pointer to function \"eglClientWaitSyncKHR\" is not valid"));
		}
	if(!iPfnEglSignalSyncKHR)
		{
		ERR_PRINTF1(_L("   Pointer to function \"eglSignalSyncKHR\" is not valid"));
		}

	if(!iPfnEglGetSyncAttribKHR)
		{
		ERR_PRINTF1(_L("   Pointer to function \"eglGetSyncAttribKHR\" is not valid"));
		}

	if(!(iPfnEglCreateSyncKHR && iPfnEglDestroySyncKHR && iPfnEglClientWaitSyncKHR 
	        && iPfnEglSignalSyncKHR && iPfnEglGetSyncAttribKHR && 
	        (!iIsSyncObjectPrivateExtensionSupported || (iPfnEglPrivateSignalSyncNOK))))
		{//if the private extension is not declared in the test ini file, we won't signal the failure
		return EFalse;
		}
	return ETrue;
	}

void CEglTest_SyncObject_Base::CleanAll()
	{
	if(iEglSess)
		{
		delete iEglSess;
		iEglSess = NULL;
		}
	TRAPD(res, TerminateDisplayL());
	TEST(res == KErrNone);
	INFO_PRINTF2(_L("Display termination completed with error %d"), res);
	
	INFO_PRINTF1(_L("Main Thread: Calling eglReleaseThread()"));
	ASSERT_EGL_TRUE(eglReleaseThread())
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-001

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that current implementation support EGL sync object extension

@SYMTestActions
1.	Request supporting extension for the existing display by calling eglQueryString(...) and passing EGL_EXTENSIONS as a second parameter.
2.	Obtain an EGL sync object extension functions by supplying the following procname to the function  void (*eglGetProcAddress(const char *procname)):
"eglCreateSyncKHR"
"eglDestroySyncKHR"
"eglClientWaitSyncKHR"
"eglSignalSyncKHR"
"eglGetSyncAttribKHR"

@SYMTestExpectedResults
1. The function must return a string which includes space separated sub-string "EGL_KHR_reusable_sync".
2. The pointer to the functions should be non zero.
*/
TVerdict CEglTest_SyncObject_Positive_GetProcAddress::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-001"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_GetProcAddress::doTestStepL"));

   if(!iIsSyncObjectExtensionSupported)
        {
        INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
        CleanAll();
        RecordTestResultL();
        CloseTMSGraphicsStep();
        return TestStepResult();
        }
	
	// check that extension "EGL_KHR_reusable_sync" is supported
	INFO_PRINTF1(_L("Check that extension \"EGL_KHR_reusable_sync\" is supported"));
	TEST(CheckForExtensionL(KEGL_KHR_reusable_sync));

	INFO_PRINTF1(_L("Obtain EGL sync object extension functions"));
	TEST(GetProcAddress());
	
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("End of CEglTest_SyncObject_Positive_GetProcAddress::doTestStepL"));
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-005

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To validate the creation and deletion of the sync object.

@SYMTestActions
1. Create sync object for the default display. NULL are specified in attribute_list. 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR 
2. Retrieve the following attributes of the sync object: EGL_SYNC_TYPE_KHR , 
   EGL_SYNC_STATUS_KHR
3. Delete the sync object 
4. Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR 
5. Retrieve the following attributes of the sync object: EGL_SYNC_TYPE_KHR , 
   EGL_SYNC_STATUS_KHR
6. Delete the sync object 
   
@SYMTestExpectedResults
1. Sync object handle doesn't equal to EGL_NO_SYNC_KHR 
2. Sync object attribute EGL_SYNC_STATUS_KHR should be set to EGL_UNSIGNALED_KHR by default,  
   attribute EGL_SYNC_TYPE_KHR should be set to EGL_SYNC_REUSABLE_KHR
4. Sync object handle doesn't equal to EGL_NO_SYNC_KHR 
5. Sync object attribute EGL_SYNC_STATUS_KHR should be set to EGL_UNSIGNALED_KHR by default,  
   attribute EGL_SYNC_TYPE_KHR should be set to EGL_SYNC_REUSABLE_KHR
 */
TVerdict CEglTest_SyncObject_Positive_CreateDestroy::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-005"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_CreateDestroy::doTestStepL"));
	
	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}
		
	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create sync object - default display, NULL at attributes, type parameter set to EGL_SYNC_REUSABLE_KHR"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, NULL);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);
	
	//check attributes of the sync object.
	INFO_PRINTF1(_L("Check attributes of sync object"));	
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR, EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR, EGL_UNSIGNALED_KHR);
	
	INFO_PRINTF1(_L("Delete sync object"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);
	
	INFO_PRINTF1(_L("Create sync object - default display, no attribute specified, type parameter set to EGL_SYNC_REUSABLE_KHR "));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);
	
	INFO_PRINTF1(_L("Check attributes"));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR, EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_UNSIGNALED_KHR);

	INFO_PRINTF1(_L("Delete sync object"));
	res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);
	
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-010

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that transition from EGL_UNSIGNALED_KHR to EGL_SIGNALED_KHR state will trigger the signal and unblock the client thread  

@SYMTestActions
1. Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR  
2. Launch thread A
3. Wait on sync object by calling eglClientWaitSyncKHR  
4. Signal on the sync object from thread A with the flag EGL_SIGNALED_KHR
5. Retrieve EGL_SYNC_STATUS_KHR attribute
6. Delete the sync object
   
@SYMTestExpectedResults
3. The main thread is blocked
4. After the signal the main thread should be unblocked and the value returned by eglClientWaitSyncKHR() is set to EGL_CONDITION_SATISFIED_KHR
5. Status attribute is set to EGL_SIGNALED_KHR
*/
TVerdict CEglTest_SyncObject_Positive_WaitSignal::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-010"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_WaitSignal::doTestStepL"));
		
	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	//create sync object
	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);
	
	//launch threadA
	Test_MultiThreadL(1, EFalse);
	
	//Stall the main thread
	TUint32 timestampBefore = User::NTickCount();	//microsecond
	EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
	TUint32 timestampAfter = User::NTickCount();
	TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
	
	INFO_PRINTF4(_L("Main thread waited on eglClientWaitSyncKHR for %d usec (threshold was [%d +- %d])"),timeElapsed, iDelaySignalling, iThreshold);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);
			
	INFO_PRINTF1(_L("Check attributes of sync object"));	
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_SIGNALED_KHR);

	INFO_PRINTF1(_L("Delete sync object"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);
	
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_WaitSignal::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_WaitSignal::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0);
	
	User::After(iDelaySignalling);
	
	INFO_PRINTF1(_L("signal sync object from thread A"));
	EGLBoolean res = iPfnEglSignalSyncKHR(iDisplay,iSyncObject,EGL_SIGNALED_KHR);
	TEST(res == EGL_TRUE);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-015

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that the client will not be blocked by waiting on the sync object 
if the latter is in EGL_SIGNALED_KHR state and will stay blocked until 
transition from unsignal to signal state occurs.
To check that eglSignalSyncKHR sets the state of the sync object correctly.

@SYMTestActions
1. Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR
2. Call eglSignalSyncKHR with EGL_SIGNALED_KHR as a parameter.
3. Retrieve an EGL_SYNC_STATUS_KHR attribute.
4. Try to wait on the sync object by calling eglClientWaitSyncKHR.
5. Call eglSignalSyncKHR with EGL_UNSIGNALED_KHR as a parameter.
6. Retrieve an EGL_SYNC_STATUS_KHR attribute. 
7. Launch thread A.
8. Waiting on the sync object by calling eglClientWaitSyncKHR.
9. From thread A call eglSignalSyncKHR with EGL_UNSIGNALED_KHR as a parameter. 
10. From thread A retrieve EGL_SYNC_STATUS_KHR attribute.
11. From thread A call eglSignalSyncKHR with EGL_SIGNALED_KHR as a parameter.
12. From thread A retrieve EGL_SYNC_STATUS_KHR attribute.
13  Delete the sync object.

@SYMTestExpectedResults
3. EGL_SYNC_STATUS_KHR attribute is set to EGL_SIGNALED_KHR
4. The thread is not blocked and an error code returned by eglClientWaitSyncKHR() is set to EGL_CONDITION_SATISFIED_KHR
6. EGL_SYNC_STATUS_KHR attribute is set to EGL_UNSIGNALED_KHR
8. Main thread is blocked
9. Main thread stays blocked
10. EGL_SYNC_STATUS_KHR attribute is set to EGL_UNSIGNALED_KHR
11. Main thread is unblocked
12. EGL_SYNC_STATUS_KHR attribute is set to EGL_SIGNALED_KHR
*/
TVerdict CEglTest_SyncObject_Positive_WaitSignal2::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-015"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_WaitSignal2::doTestStepL"));
		
	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
		
	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	//set to signal state, basically we are not changing anything here 
	EGLBoolean res = iPfnEglSignalSyncKHR(iDisplay,iSyncObject,EGL_SIGNALED_KHR);
	TEST(res = EGL_TRUE);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR, EGL_SIGNALED_KHR);

	INFO_PRINTF1(_L("Try to wait on the sync object by calling eglClientWaitSyncKHR"));
	TUint32 timestampBefore = User::NTickCount();	//microsecond
	EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
	TUint32 timestampAfter = User::NTickCount();
	TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	INFO_PRINTF1(_L(" Check that main thread was not stalled at eglClientWaitSyncKHR()"));
	TEST(timeElapsed <  iThreshold);

	//set to unsignal state 
	res = iPfnEglSignalSyncKHR(iDisplay,iSyncObject,EGL_UNSIGNALED_KHR);
	TEST(res == EGL_TRUE);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR, EGL_UNSIGNALED_KHR);
	
	//launch threadA
	Test_MultiThreadL(1, EFalse);
	
	//Stall the main thread
	timestampBefore = User::NTickCount();
	resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
	timestampAfter = User::NTickCount();
	timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
	
	INFO_PRINTF5(_L("Main thread waited on eglClientWaitSyncKHR for %d usec (threshold was [%d + %d +- %d])"),timeElapsed, iDelaySignalling*2, iTimeOverhead, iThreshold);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST(Abs(timeElapsed - (iDelaySignalling*2 + iTimeOverhead)) < iThreshold);

	INFO_PRINTF1(_L("Delete sync object"));
	res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);
	
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_WaitSignal2::doThreadFunctionL(TInt aIdx)
	{
	TUint32 timeOverheadStart = User::NTickCount();
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_WaitSignal2::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0);
	
	User::After(iDelaySignalling);
	INFO_PRINTF1(_L("unsignal sync object from thread A"));
	EGLBoolean 	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_UNSIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR, EGL_UNSIGNALED_KHR);
	
	User::After(iDelaySignalling);
	INFO_PRINTF1(_L("signal sync object from thread A"));
	TUint32 timeOverheadEnd = User::NTickCount();
	iTimeOverhead = ElapsedTime(timeOverheadEnd, timeOverheadStart) - iDelaySignalling*2;
	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR, EGL_SIGNALED_KHR);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-020

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that the deletion of the sync object triggers the signalling 

@SYMTestActions
1. Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR 
2. Launch a thread A
3. Stall the main thread by calling eglClientWaitSyncKHR() on the sync object with timeout value EGL_FOREVER_KHR
4. Delete the sync object from the thread A
   
@SYMTestExpectedResults
3. The main thread which waits for the signal must be stalled
4. After the sync object has been deleted the main client thread will be unblocked and an error code returned by eglClientWaitSyncKHR() is set to EGL_CONDITION_SATISFIED_KHR
*/
TVerdict CEglTest_SyncObject_Positive_WaitDelete::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-020"));
    INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_WaitDelete::doTestStepL"));
    
	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
    GetDisplayL();
    CreateEglSessionL();
	iEglSess->InitializeL();
    
    //create sync object
    INFO_PRINTF1(_L("Create sync object"));
    iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
    TESTL(iSyncObject != EGL_NO_SYNC_KHR);

    //launch threadA
    Test_MultiThreadL(1, EFalse);
 
    //Stall the main thread
    TUint32 timestampBefore = User::NTickCount();  //microsecond
    EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
    TUint32 timestampAfter = User::NTickCount();
    TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
    
    INFO_PRINTF4(_L("Main thread waited on eglClientWaitSyncKHR for %d usec (threshold was [%d +- %d])"),timeElapsed, iDelaySignalling, iThreshold);
    TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
    TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);
    
    CleanAll();
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_SyncObject_Positive_WaitDelete::doThreadFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_WaitDelete::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0);
    
    User::After(iDelaySignalling);
    INFO_PRINTF1(_L("Delete sync object from thread A"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
    TEST(res == EGL_TRUE);
    }

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-025

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that client will be unblocked once the timeout has expired even if there is no signalling occurred.

@SYMTestActions
1.	Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
    The type parameter is set to EGL_SYNC_REUSABLE_KHR
2.	Take the start time stamp
3.	Call eglClientWaitSyncKHR() on the sync object with timeout value equals five seconds
4.	Take the end timestamp
5.	Retrieve EGL_SYNC_STATUS attribute of the sync object
6.	Destroy the sync object

@SYMTestExpectedResults
3. The client thread should stay blocked until timeout is expired
4. Check that (timestamp End - timestamp Before)  equals (roughly) timeout and an error code returned by 
   eglClientWaitSyncKHR is EGL_TIMEOUTE_EXPIRED_KHR
5. The sync object attribute EGL_SYNC_STATUS is set to EGL_UNSIGNALED_KHR   
*/
TVerdict CEglTest_SyncObject_Positive_WaitWithTimeoutExpired::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-025"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_WaitWithTimeoutExpired::doTestStepL started...."));

	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	// calling wait sync without signaling the object should hit the predefined timeout if set to anything different to EGL_FOREVER_KHR
	INFO_PRINTF2(_L("Calling eglClientWaitSyncKHR on the object with a defined timeout of %d usec"), iWaitSyncTimeout);
	EGLTimeKHR timeoutNano = (TUint64)iWaitSyncTimeout * 1000; // EGLTimeKHR is in nanoseconds!!
	TUint32 timestampBefore = User::NTickCount();
	EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,timeoutNano);
	TUint32 timestampAfter = User::NTickCount();
	TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

	INFO_PRINTF4(_L("Waited on eglClientWaitSyncKHR for %d usec (threshold was [%d +- %d])"),timeElapsed, iWaitSyncTimeout, iThreshold);
	TEST(resClientWait == EGL_TIMEOUT_EXPIRED_KHR);
	TEST (Abs(timeElapsed - timeoutNano/1000) < iThreshold);

	INFO_PRINTF1(_L("Check attributes..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_UNSIGNALED_KHR);

	INFO_PRINTF1(_L("Delete sync object"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_WaitWithTimeoutExpired::doTestStepL completed!"));
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-030

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that client will be unblocked once the signalling occurs even if the timeout is not expired

@SYMTestActions
1.	Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
    The type parameter is set to EGL_SYNC_REUSABLE_KHR.
2.	Launch thread A
3.	Take the start time stamp
4.	Call eglClientWaitSyncKHR() on the sync object with timeout value equals five seconds
5.	Signal on the sync object from thread A before timeout is expired (after 2.5 seconds).
6.	Takes the end timestamp
7.	Retrieve EGL_SYNC_STATUS attribute of the sync object from thread A
8.	Destroy the sync object

@SYMTestExpectedResults
4. The client thread should be blocked
6. The main thread is unblocked. Check that (timestamp End - timestamp Before)  equals (roughly)
   2.5 seconds and an error code returned by eglClientWaitSyncKHR is EGL_CONDITION_SATISFIED_KHR
7. The sync object attribute EGL_SYNC_STATUS is set to EGL_SIGNALED_KHR 
*/
TVerdict CEglTest_SyncObject_Positive_SignalBeforeTimeoutExpired::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-030"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_SignalBeforeTimeoutExpired::doTestStepL started...."));

	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	INFO_PRINTF1(_L("Launching thread A..."));
	Test_MultiThreadL(1, EFalse);

	INFO_PRINTF2(_L("Main thread waiting for thread A to signal with a defined timeout of %d usec"), iWaitSyncTimeout);
	EGLTimeKHR timeoutNano = (TUint64)iWaitSyncTimeout * 1000; // EGLTimeKHR is in nanoseconds!!
	TUint32 timestampBefore = User::NTickCount();
	EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,timeoutNano);
	TUint32 timestampAfter = User::NTickCount();
	TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

	INFO_PRINTF4(_L("Main thread waited on eglClientWaitSyncKHR for %d usec (threshold was [%d +- %d])"),timeElapsed, iDelaySignalling, iThreshold);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);

	INFO_PRINTF1(_L("Check attributes..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_SIGNALED_KHR);

	INFO_PRINTF1(_L("Delete sync object"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_SignalBeforeTimeoutExpired::doTestStepL completed!"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_SignalBeforeTimeoutExpired::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_SignalBeforeTimeoutExpired::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0);

	INFO_PRINTF2(_L("Wait for a bit before Signal sync object, Thread %d"),aIdx);
	User::After(iDelaySignalling);
	EGLBoolean 	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	INFO_PRINTF2(_L("Signal sync object done, Thread %d"),aIdx);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-035

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that client will be unblocked once the sync object has been deleted  even if the timeout is not expired

@SYMTestActions
1.	Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
    The type parameter is set to EGL_SYNC_REUSABLE_KHR
2.	Launch thread A
3.	Take the start time stamp
4.	Stall the client thread by calling eglClientWaitSyncKHR() on the sync object with timeout value equals five seconds
5.	Destroy the sync object from thread A before timeout is expired. (2.5 seconds)
6.	Takes the end timestamp
7.	Retrieve EGL_SYNC_STATUS attribute of the sync object
8.	Destroy the sync object

@SYMTestExpectedResults
4. The main client thread should be blocked
5. The main thread is unblocked. Check that (timestamp End - timestamp Before)  equals (roughly) 
   2.5 seconds and an error code returned by eglClientWaitSyncKHR is EGL_CONDITION_SATISFIED_KHR
7. eglGetSyncAttribKHR() returns EGL_FALSE and EGL_BAD_PARAMETER is generated
*/
TVerdict CEglTest_SyncObject_Positive_DeleteBeforeTimeoutExpired::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-035"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_DeleteBeforeTimeoutExpired::doTestStepL started...."));

	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create sync object"));
    iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
    TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	INFO_PRINTF1(_L("Launching thread A..."));
	Test_MultiThreadL(1, EFalse);

	INFO_PRINTF2(_L("Main thread waiting for thread A to signal with a defined timeout of %d usec"), iWaitSyncTimeout);
	EGLTimeKHR timeoutNano = (TUint64)iWaitSyncTimeout * 1000; // EGLTimeKHR is in nanoseconds!!
	TUint32 timestampBefore = User::NTickCount();
	EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,timeoutNano);
	TUint32 timestampAfter = User::NTickCount();
	TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

	INFO_PRINTF4(_L("Main thread waited on eglClientWaitSyncKHR for %d usec before sync object was deleted (threshold was [%d +- %d])"),timeElapsed, iDelaySignalling, iThreshold);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);

	INFO_PRINTF1(_L("Check attributes..."));
	EGLint syncTypeValue=-1;
	EGLBoolean ret = iPfnEglGetSyncAttribKHR(iDisplay,iSyncObject,EGL_SYNC_TYPE_KHR,&syncTypeValue);
	TEST(ret == EGL_FALSE);
	EGLint error = eglGetError();
	TEST(error == EGL_BAD_PARAMETER);
	TEST(syncTypeValue == -1); // due to the error, it wasn't properly retrieved

	INFO_PRINTF1(_L("(attempt to) Delete sync object (was already deleted by a worker thread)"));
	ret = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(ret == EGL_FALSE);
	error = eglGetError();
	TEST(error == EGL_BAD_PARAMETER);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_DeleteBeforeTimeoutExpired::doTestStepL completed!"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_DeleteBeforeTimeoutExpired::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_DeleteBeforeTimeoutExpired::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0);

	INFO_PRINTF2(_L("Wait for a bit before destroying sync object, Thread %d"),aIdx);
	User::After(iDelaySignalling);
	TInt res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);
	INFO_PRINTF2(_L("Delete sync object done, Thread %d"),aIdx);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-040

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that all threads waiting for the signal on the same sync object will be unblocked before the timeout has expired.

@SYMTestActions
1.	Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
    The type parameter is set to EGL_SYNC_REUSABLE_KHR
2.	Launch two threads A and B.
3.	Call eglClientWaitSyncKHR() (from threads A and B) on the sync object with timeout value equals five seconds 
    in thread A and EGL_FOREVER_KHR in thread B.
4.	Signal on the sync object with the flag EGL_SIGNALED_KHR from the main thread before timeout expired (after 2.5 seconds)
5.	Retrieve EGL_SYNC_STATUS attribute of the sync object from the main thread
6.	Destroy the sync object

@SYMTestExpectedResults
3. Threads A and B will be stalled
4. Both threads A and B will be unblocked and a value returned by eglClientWaitSyncKHR() is set to EGL_CONDITION_SATISFIED_KHR.  
5. The sync object attribute EGL_SYNC_STATUS is set to EGL_SIGNALED_KHR
*/
TVerdict CEglTest_SyncObject_Positive_Multithread_SignalBeforeTimeout::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-040"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_Multithread_SignalBeforeTimeout::doTestStepL started...."));

	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();

	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	INFO_PRINTF1(_L("Launching threads A and B..."));
	Test_MultiThreadL(2, EFalse);

	INFO_PRINTF1(_L("Main thread, wait for a bit before Signal sync object"));
	User::After(iDelaySignalling);
	EGLBoolean 	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	INFO_PRINTF1(_L("Main thread Signal sync object done"));

	INFO_PRINTF1(_L("Check attributes..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_SIGNALED_KHR);

	INFO_PRINTF1(_L("Delete sync object"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_Multithread_SignalBeforeTimeout::doTestStepL completed!"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_Multithread_SignalBeforeTimeout::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_Multithread_SignalBeforeTimeout::doThreadFunctionL, Thread %d"), aIdx);
	ASSERT_TRUE(aIdx==0 || aIdx==1);

	//Thread 0 waits for the main thread to signal the sync object, and exits
	if(aIdx == 0)	
		{
		INFO_PRINTF2(_L("Thread %d waiting for main thread to signal with a timeout EGL_FOREVER_KHR"), aIdx);
		TUint32 timestampBefore = User::NTickCount();
		EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
		TUint32 timestampAfter = User::NTickCount();
		TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

		INFO_PRINTF5(_L("Thread %d waited on eglClientWaitSyncKHR for %d usec before sync object was signaled (threshold was [%d +- %d])"), aIdx, timeElapsed, iDelaySignalling, iThreshold);
		TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
		TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);
		}
	//Thread 1 waits for the main thread to signal the sync object, and exits
	else if(aIdx == 1)
		{
		INFO_PRINTF3(_L("Thread %d waiting for main thread to signal with a defined timeout of %d"), aIdx, iWaitSyncTimeout);
		EGLTimeKHR timeoutNano = (TUint64)iWaitSyncTimeout * 1000; // EGLTimeKHR is in nanoseconds!!
		TUint32 timestampBefore = User::NTickCount();
		EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,timeoutNano);
		TUint32 timestampAfter = User::NTickCount();
		TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

		INFO_PRINTF5(_L("Thread %d waited on eglClientWaitSyncKHR for %d usec before sync object was signaled (threshold was [%d +- %d])"), aIdx, timeElapsed, iDelaySignalling, iThreshold);
		TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
		TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);
		}
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-045

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that a thread resumed due to timeout will not unblock another thread waiting for the signal.

@SYMTestActions
1. Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR
2. Launch two threads A and B.
3. Call eglClientWaitSyncKHR() (from threads A and B) on the sync object with timeout value equals five seconds in thread A and EGL_FOREVER_KHR in thread B.
4. Wait for approximately 7.5 seconds
5. Retrieve EGL_SYNC_STATUS attribute of the sync object from the main thread
6. Signal on the sync object with the flag EGL_SIGNALED_KHR from the main thread
7. Retrieve EGL_SYNC_STATUS attribute of the sync object from the main thread
8. Destroy the sync object

@SYMTestExpectedResults
3.Threads A and B will be stalled
4. Thread A will be unblocked after approximately 5 seconds and a value returned by eglClientWaitSyncKHR() is set to EGL_TIMEOUT_EXPIRED_KHR.
5. EGL_SYNC_STATUS attribute of the sync object is set to EGL_UNSIGNALED_KHR
6. Threads B will be unblocked and a value returned by eglClientWaitSyncKHR() is set to EGL_CONDITION_SATISFIED_KHR.  
7. The sync object attribute EGL_SYNC_STATUS is set to EGL_SIGNALED_KHR
*/
TVerdict CEglTest_SyncObject_Positive_Multithread_SignalAfterTimeout::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-045"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_Multithread_SignalAfterTimeout::doTestStepL started...."));

	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	INFO_PRINTF1(_L("Launching threads A and B..."));
	Test_MultiThreadL(2, EFalse);
	TUint32 timeOverheadStart = User::NTickCount();
	
	INFO_PRINTF1(_L("Main thread, wait for a long time, no Signal sync object yet"));
	User::After(iLongDelaySignalling);

	INFO_PRINTF1(_L("Check attributes..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_UNSIGNALED_KHR);

	INFO_PRINTF1(_L("Main thread, Signal sync object now"));
	TUint32 timeOverheadEnd = User::NTickCount();
	iTimeOverhead = ElapsedTime(timeOverheadEnd, timeOverheadStart) - iLongDelaySignalling;
	EGLBoolean 	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	INFO_PRINTF1(_L("Main thread, Signal sync object done"));

	INFO_PRINTF1(_L("Check attributes..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_SIGNALED_KHR);

	INFO_PRINTF1(_L("Delete sync object"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_Multithread_SignalAfterTimeout::doTestStepL completed!"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_Multithread_SignalAfterTimeout::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_Multithread_SignalAfterTimeout::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0 || aIdx==1);

	//Thread 0 waits for the main thread to signal the sync object, and exits
	if(aIdx == 0)	
		{
		INFO_PRINTF2(_L("Thread %d waiting for main thread to signal with a timeout EGL_FOREVER_KHR"), aIdx);
		TUint32 timestampBefore = User::NTickCount();
		EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
		TUint32 timestampAfter = User::NTickCount();
		TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

		INFO_PRINTF6(_L("Thread %d waited on eglClientWaitSyncKHR for %d usec before sync object was signaled (threshold was [%d + %d +- %d])"), aIdx, timeElapsed, iLongDelaySignalling, iTimeOverhead, iThreshold);
		TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
		TEST (Abs(timeElapsed - (iLongDelaySignalling + iTimeOverhead)) < iThreshold);
		}
	//Thread 1 waits for the main thread to signal the sync object, and exits
	else if(aIdx == 1)
		{
		INFO_PRINTF3(_L("Thread %d waiting for main thread to signal with a defined timeout of %d"), aIdx, iWaitSyncTimeout);
		EGLTimeKHR timeoutNano = (TUint64)iWaitSyncTimeout * 1000; // EGLTimeKHR is in nanoseconds!!
		TUint32 timestampBefore = User::NTickCount();
		EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,timeoutNano);
		TUint32 timestampAfter = User::NTickCount();
		TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

		INFO_PRINTF5(_L("Thread %d waited on eglClientWaitSyncKHR for %d usec before sync object was timeout'ed (threshold was [%d +- %d])"), aIdx, timeElapsed, iWaitSyncTimeout, iThreshold);
		TEST(resClientWait == EGL_TIMEOUT_EXPIRED_KHR);
		TEST (Abs(timeElapsed - iWaitSyncTimeout) < iThreshold);
		}
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-050

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that signalling on one sync object won't unblock the thread waiting on another sync object.

@SYMTestActions
1.	Create two sync objects (S1 and S2) for the default display with no attributes are specified and the type parameter is set to EGL_SYNC_REUSABLE_KHR
2.	Launch two threads A and B.
3.	Wait on the sync objects S1 and S2 from threads A and B respectively by calling eglClientWaitSyncKHR()  with timeout set to EGL_FOREVER_KHR.
4.	Signal on S1 with the flag EGL_SIGNALED_KHR from main thread
5.	Retrieve EGL_SYNC_STATUS attribute of S1 and S2 from main thread
6.	Signal on S2 with the flag EGL_SIGNALED_KHR from main thread
7.	Retrieve EGL_SYNC_STATUS attribute of S1 and S2 from main thread
8.	Destroy the sync objects

@SYMTestExpectedResults
3. Threads A and B will be stalled
4. Thread A will be unblocked and an error code returned by eglClientWaitSyncKHR() is set to EGL_CONDITION_SATISFIED_KHR.
   Thread B stays blocked.  
5. The S1 attribute EGL_SYNC_STATUS is set to EGL_SIGNALED_KHR, S2 attribute EGL_SYNC_STATUS is set to EGL_UNSIGNALED_KHR
6. Thread B will be unblocked and an error code returned by eglClientWaitSyncKHR() is set to EGL_CONDITION_SATISFIED_KHR.
7. The S1 & S2 attribute EGL_SYNC_STATUS is set to EGL_SIGNALED_KHR
*/
TVerdict CEglTest_SyncObject_Positive_Multithread_WaitOnTwoSyncObject::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-050"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_Multithread_WaitOnTwoSyncObject::doTestStepL started...."));

	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();

	INFO_PRINTF1(_L("Create two sync objects"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);
	iSyncObject2 = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject2 != EGL_NO_SYNC_KHR);

	INFO_PRINTF1(_L("Launching threads A and B..."));
	Test_MultiThreadL(2, EFalse);
	TUint32 timeOverheadStart = User::NTickCount();
	
	INFO_PRINTF1(_L("Main thread, wait for a bit before Signal sync object number 1"));
	User::After(iDelaySignalling);
	EGLBoolean 	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	INFO_PRINTF1(_L("Main thread Signal sync object number 1 done"));

	INFO_PRINTF1(_L("Check attributes for sync object number 1..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_SIGNALED_KHR);

	INFO_PRINTF1(_L("Check attributes for sync object number 2..."));
	// the CheckSyncAttrib uses iSyncObject, so we need to replace it, and restore it later
	EGLSyncKHR oldSyncObject=iSyncObject;
	iSyncObject=iSyncObject2;
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR, EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_UNSIGNALED_KHR);
	iSyncObject=oldSyncObject;

	INFO_PRINTF1(_L("Main thread, wait for a bit before Signal sync object number 2"));
	User::After(iDelaySignalling);
	
	TUint32 timeOverheadEnd = User::NTickCount();
	iTimeOverhead = ElapsedTime(timeOverheadEnd, timeOverheadStart) - iDelaySignalling*2;
	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject2, EGL_SIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	INFO_PRINTF1(_L("Main thread Signal sync object number 2 done"));

	INFO_PRINTF1(_L("Check attributes for sync object number 1..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_SIGNALED_KHR);

	INFO_PRINTF1(_L("Check attributes for sync object number 2..."));
	oldSyncObject=iSyncObject;
	iSyncObject=iSyncObject2;
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_SIGNALED_KHR);
	iSyncObject=oldSyncObject;

	INFO_PRINTF1(_L("Delete both sync objects"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);
	res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject2);
	TEST(res == EGL_TRUE);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_Multithread_WaitOnTwoSyncObject::doTestStepL completed!"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_Multithread_WaitOnTwoSyncObject::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_Multithread_WaitOnTwoSyncObject::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0 || aIdx==1);

	//Thread 0 waits for the main thread to signal the sync object, and exits
	if(aIdx == 0)	
		{
		INFO_PRINTF2(_L("Thread %d waiting for main thread to signal object 1 with a timeout EGL_FOREVER_KHR"), aIdx);
		TUint32 timestampBefore = User::NTickCount();
		EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
		TUint32 timestampAfter = User::NTickCount();
		TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

		INFO_PRINTF5(_L("Thread %d waited on eglClientWaitSyncKHR for %d usec before sync object was signaled (threshold was [%d +- %d])"), aIdx, timeElapsed, iDelaySignalling, iThreshold);
		TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
		TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);
		}
	//Thread 1 waits for the main thread to signal the sync object, and exits
	else if(aIdx == 1)	
		{
		INFO_PRINTF2(_L("Thread %d waiting for main thread to signal object 2 with a timeout EGL_FOREVER_KHR"), aIdx);
		TUint32 timestampBefore = User::NTickCount();
		EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject2,0,EGL_FOREVER_KHR);
		TUint32 timestampAfter = User::NTickCount();
		TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

		INFO_PRINTF6(_L("Thread %d waited on eglClientWaitSyncKHR for %d usec before sync object was signaled (threshold was [%d + %d +- %d])"), aIdx, timeElapsed, 2*iDelaySignalling, iTimeOverhead, iThreshold);
		TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
		TEST (Abs(timeElapsed - (2*iDelaySignalling + iTimeOverhead)) < iThreshold);
		}
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-055

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that if zero timeout is passed to eglClientWaitSyncKHR(), the function just test the current status without attempt to stall the client thread.

@SYMTestActions
1.	Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
    The type parameter is set to EGL_SYNC_REUSABLE_KHR
2.	Take timestamp before
3.	Call eglClientWaitSyncKHR() on the sync object with timeout value equals to zero
4.	Take timestamp after
5.	Call eglSignalSyncKHR on the sync object with the flag EGL_SIGNALED_KHR
6.	Repeat steps 2 to 4
7.	Destroy the sync object

@SYMTestExpectedResults
3.  The client thread is not stalled.  The function eglClientWaitSyncKHR() should return EGL_TIMEOUT_EXPIRED_KHR. 
4.  The difference timestamp (after - before) should be less than a half second
3.1 The client thread is not stalled.  The function eglClientWaitSyncKHR() should return EGL_CONDITION_SATISFIED_KHR.    
4.1 The difference timestamp (after - before) should be less than a half second 
*/
TVerdict CEglTest_SyncObject_Positive_Wait_TestMode::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-055"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_Wait_TestMode::doTestStepL started...."));

	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();

	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	// calling wait sync without signaling the object, but with timeout set to 0 ==> no stalling
	INFO_PRINTF1(_L("Calling eglClientWaitSyncKHR on the object, without signaling the object, but with timeout set to 0"));
	TUint32 timestampBefore = User::NTickCount();
	EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,0);
	TUint32 timestampAfter = User::NTickCount();
	TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

	INFO_PRINTF3(_L("Check it didn't wait on eglClientWaitSyncKHR. Elapsed time was %d usec (threshold was [%d])"),timeElapsed, iThreshold);
	TEST(resClientWait == EGL_TIMEOUT_EXPIRED_KHR);
	TEST (timeElapsed < iThreshold);

	INFO_PRINTF1(_L("Check attributes..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_UNSIGNALED_KHR);
	
	INFO_PRINTF1(_L("Signal sync object"));
	EGLBoolean 	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);

	INFO_PRINTF1(_L("Repeat same step,  calling eglClientWaitSyncKHR with timeout set to 0"));
	timestampBefore = User::NTickCount();
	resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,0);
	timestampAfter = User::NTickCount();
	timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

	INFO_PRINTF3(_L("Check it didn't wait on eglClientWaitSyncKHR. Elapsed time was %d usec (threshold was [%d])"),timeElapsed, iThreshold);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST (timeElapsed < iThreshold);

	INFO_PRINTF1(_L("Check attributes..."));
	CheckSyncAttrib(EGL_SYNC_TYPE_KHR,EGL_SYNC_REUSABLE_KHR);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_SIGNALED_KHR);

	INFO_PRINTF1(_L("Delete sync object"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_Wait_TestMode::doTestStepL completed!"));
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-060

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that deletion of the sync object will unblock all threads waiting for the signal.

@SYMTestActions
1.	Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
    The type parameter is set to EGL_SYNC_REUSABLE_KHR
2.	Launch threads A and B
3.	Call eglClientWaitSyncKHR() on the sync object in main thread and thread A.
4.	Destroy the sync object from thread B

@SYMTestExpectedResults
3. Main thread and thread A will be stalled
4. All client threads which waited for the signal must be unblocked as if eglSignalSyncKHR() has been called. 
   The function eglClientWaitSyncKHR should return EGL_CONDITION_SATISFIED_KHR
*/
TVerdict CEglTest_SyncObject_Positive_Multithread_Deletion::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-060"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_Multithread_Deletion::doTestStepL started...."));

	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	INFO_PRINTF1(_L("Launching threads A and B..."));
	Test_MultiThreadL(2, EFalse);

	INFO_PRINTF1(_L("Main thread waiting for thread B to destroy the sync object, with timeout EGL_FOREVER_KHR"));
	TUint32 timestampBefore = User::NTickCount();
	EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
	TUint32 timestampAfter = User::NTickCount();
	TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

	INFO_PRINTF4(_L("Main thread waited on eglClientWaitSyncKHR for %d usec before sync object was deleted (threshold was [%d +- %d])"),timeElapsed, iDelaySignalling, iThreshold);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);

	INFO_PRINTF1(_L("Check attributes..."));
	EGLint syncTypeValue=-1;
	EGLBoolean ret = iPfnEglGetSyncAttribKHR(iDisplay,iSyncObject,EGL_SYNC_TYPE_KHR,&syncTypeValue);
	TEST(ret == EGL_FALSE);
	EGLint error = eglGetError();
	TEST(error == EGL_BAD_PARAMETER);
	TEST(syncTypeValue == -1); // due to the error, it wasn't properly retrieved

	INFO_PRINTF1(_L("(attempt to) Delete sync object (was already deleted by a worker thread)"));
	ret = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(ret == EGL_FALSE);
	error = eglGetError();
	TEST(error == EGL_BAD_PARAMETER);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_Multithread_Deletion::doTestStepL completed!"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_Multithread_Deletion::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_Multithread_Deletion::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0 || aIdx==1);

	//Thread 0 waits for a the thread B to delete the sync object, and exits
	if(aIdx == 0)	
		{
		INFO_PRINTF2(_L("Thread %d waiting for thread B to destroy the sync object, with timeout EGL_FOREVER_KHR"), aIdx);
		TUint32 timestampBefore = User::NTickCount();
		EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
		TUint32 timestampAfter = User::NTickCount();
		TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

		INFO_PRINTF5(_L("Thread %d waited on eglClientWaitSyncKHR for %d usec before sync object was deleted (threshold was [%d +- %d])"), aIdx, timeElapsed, iDelaySignalling, iThreshold);
		TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
		TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);
		}
	//Thread 1 waits for a certain amount of time, deletes the sync object and exits
	else if(aIdx == 1)	
		{
		INFO_PRINTF2(_L("Wait for a bit before destroying sync object, Thread %d"),aIdx);
		User::After(iDelaySignalling);
		TInt res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
		TEST(res == EGL_TRUE);
		INFO_PRINTF2(_L("Delete sync object done, Thread %d"),aIdx);
		//This Thread now will just exit
		}
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-065

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To exercise sync object functionality in stress conditions.

@SYMTestActions
1. Create three sync objects (S1, S2, S3) for the default display with no attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR.
2. Launch 2 threads (A and B)
3. Make the threads A and B waiting on sync objects S2 and S3 respectively.
4. Make consequential signalling in that order: Client-> A->B->Client->... for some period of time. 
   Each thread signals the next thread in the chain and then stalls itself on the corresponding sync object. 
   (Whenever each thread is unstalled, corresponding sync object should be reset by unsignalling it.)
   Main thread is waiting on S1 
5. When the number of iteration exceeds some predefined value (by default is 1000, but could be reconfigured in ini file) the sequence, defined in step 4 will stop executing
6. Delete the sync objects

@SYMTestExpectedResults
4. The functionality is robust, transition from "unsignaled" to "signaled" 
state always unblocks the threads  
*/
TVerdict CEglTest_SyncObject_Positive_Stress::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-065"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_Stress::doTestStepL"));
	
	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create sync objects"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);
	iSyncObject1 = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject1 != EGL_NO_SYNC_KHR);
	iSyncObject2 = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject2 != EGL_NO_SYNC_KHR);
	
	iLastThreadToSignal = 2;
	
	//launch threadA & threadB
	Test_MultiThreadL(2, EFalse);
	User::After(iDelaySignalling); //to allow threads start up before we signal
	
	for(TInt index = 0; index < iNumStressIterations; index++)
		{
		iLastThreadToSignal = 2;
		EGLBoolean 	resSignal = iPfnEglSignalSyncKHR(iDisplay,iSyncObject1,EGL_SIGNALED_KHR );
		TEST(resSignal == EGL_TRUE);
		
		EGLint res = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
		TEST(iLastThreadToSignal == 1);
		TEST(res == EGL_CONDITION_SATISFIED_KHR);
		resSignal = iPfnEglSignalSyncKHR(iDisplay,iSyncObject,EGL_UNSIGNALED_KHR );//to set the status of iSyncObject EGL_UNSIGNALED_KHR
		TEST(resSignal == EGL_TRUE);    	
		User::After(0);//to yield the processor 
		}
	iStopTest = ETrue;
	INFO_PRINTF2(_L("Threads have been signalling for %d times"), iNumStressIterations);
	INFO_PRINTF1(_L("Delete sync objects"));
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);
	res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject1);
	TEST(res == EGL_TRUE);
	res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject2);
	TEST(res == EGL_TRUE);
	
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("End of CEglTest_SyncObject_Positive_Stress::doTestStepL"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_Stress::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_Stress::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0 || aIdx==1);
	
	EGLint res;
	if(aIdx == 0)
		{
		while(!iStopTest)
			{
			res = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject1,0,EGL_FOREVER_KHR);
			TEST(res == EGL_CONDITION_SATISFIED_KHR);
			User::After(0); //to yield the processor 
			if(!iStopTest)
				{
				TEST(iLastThreadToSignal == 2);
				EGLBoolean resSignal = iPfnEglSignalSyncKHR(iDisplay,iSyncObject1,EGL_UNSIGNALED_KHR );//to reset the status of iSyncObject1 EGL_UNSIGNALED_KHR
                TEST(resSignal == EGL_TRUE);
                
				iLastThreadToSignal = 0;
				resSignal = iPfnEglSignalSyncKHR(iDisplay,iSyncObject2,EGL_SIGNALED_KHR );
				TEST(resSignal == EGL_TRUE);
				}
			}
		}
	else if(aIdx == 1)
		{
		while(!iStopTest)
			{
			res = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject2,0,EGL_FOREVER_KHR);
			TEST(res == EGL_CONDITION_SATISFIED_KHR);
			User::After(0); //to yield the processor  
			if(!iStopTest)
				{
				TEST(iLastThreadToSignal == 0);
	            EGLBoolean resSignal = iPfnEglSignalSyncKHR(iDisplay,iSyncObject2,EGL_UNSIGNALED_KHR );//to reset the status of iSyncObject2 EGL_UNSIGNALED_KHR
	            TEST(resSignal == EGL_TRUE); 
				
				iLastThreadToSignal = 1;
				resSignal = iPfnEglSignalSyncKHR(iDisplay,iSyncObject,EGL_SIGNALED_KHR );
				TEST(resSignal == EGL_TRUE);
				}
			}
		}
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-070

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that flushing of the client API (bit EGL_SYNC_FLUSH_COMMANDS_BIT_KHR is set 
for eglClientWaitSyncKHR()) doesn't cause any problems.

@SYMTestActions
1. Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR
2. Bind Open VG API.
3. Create and make the context current for the bound API.
4. Launch the thread A.
5. Wait on sync object by calling eglClientWaitSyncKHR(), pass on EGL_SYNC_FLUSH_COMMANDS_BIT_KHR as a flag.
6. Signal "EGL_UNSIGNALED_KHR" to the sync object.
7. Bind Open GLES API.
8. Repeat steps 3-6.
9. Repeat step 5, but client waits in test mode (timeout is set to zero)
10. Make no context current
11. Repeat steps 3-5.
12. Delete the sync object.

@SYMTestExpectedResults
All functions succeed. Setting the flag on step 5 doesn't cause any problem.
9. The function eglClientWaitSyncKHR()returns  EGL_TIMEOUT_EXPIRED_KHR 
*/
TVerdict CEglTest_SyncObject_Positive_WaitFlush::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-070"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_WaitFlush::doTestStepL"));
	
	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);

	INFO_PRINTF1(_L("bind OpenVG API"));
	ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));
	EGLConfig currentConfig = iEglSess->GetConfigExactMatchL(EPixmapAttribsColor64K);
	INFO_PRINTF1(_L("Create and make the context current for the bound  API"));
	EGLContext context = eglCreateContext(iDisplay, currentConfig, EGL_NO_CONTEXT, NULL);
	ASSERT_EGL_TRUE(context != EGL_NO_CONTEXT);
	
	//Create an OffScreen Pixmap to be used as an OpenVg target
	TSgImageInfoTest imageInfo = TSgImageInfoTest();
	imageInfo.iSizeInPixels = TSize(200, 200);
	imageInfo.iPixelFormat = EUidPixelFormatRGB_565;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iUsage = ESgUsageBitOpenVgSurface;
#else
    imageInfo.iUsage = ESgUsageOpenVgTarget;
#endif	
	RSgImage sgImageTarget;
	CleanupClosePushL(sgImageTarget);
	TInt ret2 = sgImageTarget.Create(imageInfo, NULL, NULL);
	ASSERT_EQUALS(ret2, KErrNone);
	EGLSurface surface = eglCreatePixmapSurface(iDisplay, currentConfig,&sgImageTarget, KPixmapAttribsNone );
	ASSERT_EGL_TRUE(surface != EGL_NO_SURFACE);
	
	TSurfaceToDestroy surfaceToDestroy;
	surfaceToDestroy.iSurface = surface;
	surfaceToDestroy.iDisplay = iDisplay;
	CleanupStack::PushL(TCleanupItem(DestroySurface, &surfaceToDestroy));

	INFO_PRINTF1(_L("Calling eglMakeCurrent"));
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, surface, surface, context));

	//launch threadA
	Test_MultiThreadL(1, EFalse);
	INFO_PRINTF1(_L("Wait on sync object by calling eglClientWaitSyncKHR(), pass on EGL_SYNC_FLUSH_COMMANDS_BIT_KHR as a flag"));
	TUint32 timestampBefore = User::NTickCount();
	EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,EGL_SYNC_FLUSH_COMMANDS_BIT_KHR,EGL_FOREVER_KHR);
	TUint32 timestampAfter = User::NTickCount();
	TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST(Abs(timeElapsed - iDelaySignalling) < iThreshold);
	EGLBoolean resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_UNSIGNALED_KHR);
	TEST(resSignal == EGL_TRUE); 
	
	//make no context current
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
	
	//Wait for the thread to finish before starting the next part of the test.
	Test_MultiThread_WaitL(ETrue, TThreadStatus::ELogin);
	
	//---- the same test but for OpenGLES	
	INFO_PRINTF1(_L("bind OpenGLES API"));
	ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENGL_ES_API));
	EGLContext context1 = eglCreateContext(iDisplay, currentConfig, EGL_NO_CONTEXT, NULL);
	ASSERT_EGL_TRUE(context1 != EGL_NO_CONTEXT);
	
	INFO_PRINTF1(_L("Calling eglMakeCurrent"));
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, surface, surface, context1));
	
	//launch threadA
	Test_MultiThreadL(1, EFalse);
	INFO_PRINTF1(_L("Wait on sync object by calling eglClientWaitSyncKHR(), pass on EGL_SYNC_FLUSH_COMMANDS_BIT_KHR as a flag"));
	timestampBefore = User::NTickCount();
	resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,EGL_SYNC_FLUSH_COMMANDS_BIT_KHR,EGL_FOREVER_KHR);
	timestampAfter = User::NTickCount();
	timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST(Abs(timeElapsed - iDelaySignalling) < iThreshold);
	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_UNSIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	
	//repeat the same, but in test mode (timeout  is set to zero)
	INFO_PRINTF1(_L("Wait on sync object by calling eglClientWaitSyncKHR(), pass on EGL_SYNC_FLUSH_COMMANDS_BIT_KHR as a flag"));
	timestampBefore = User::NTickCount();
	resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,EGL_SYNC_FLUSH_COMMANDS_BIT_KHR,0);
	timestampAfter = User::NTickCount();
	timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
	TEST(resClientWait == EGL_TIMEOUT_EXPIRED_KHR);//the sync object is in "unsignaled" state
	TEST(timeElapsed < iThreshold);
	resSignal = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_UNSIGNALED_KHR);
	TEST(resSignal == EGL_TRUE);
	
	INFO_PRINTF1(_L("Make no context current"));
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
	
	//Wait for the thread to finish before starting the next part of the test.
	Test_MultiThread_WaitL(ETrue, TThreadStatus::ELogin);
	
	//now try to wait with flushing bit on (EGL_SYNC_FLUSH_COMMANDS_BIT_KHR). The flag will be ignored
	//launch threadA
	Test_MultiThreadL(1, EFalse);
	
	INFO_PRINTF1(_L("Wait on sync object by calling eglClientWaitSyncKHR(), pass on EGL_SYNC_FLUSH_COMMANDS_BIT_KHR as a flag, when no context is current"));
	timestampBefore = User::NTickCount();
	resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,EGL_SYNC_FLUSH_COMMANDS_BIT_KHR,EGL_FOREVER_KHR);
	timestampAfter = User::NTickCount();
	timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
	TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
	TEST(Abs(timeElapsed - iDelaySignalling) < iThreshold);
	
	eglDestroyContext(iDisplay, context1);		//Closing eglContext
	eglDestroyContext(iDisplay, context);		//Closing eglContext
	CleanupStack::PopAndDestroy(2, &sgImageTarget);
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);
	
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("End of CEglTest_SyncObject_Positive_WaitFlush::doTestStepL"));
	return TestStepResult();
	}

void CEglTest_SyncObject_Positive_WaitFlush::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_WaitFlush::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0);

	User::After(TTimeIntervalMicroSeconds32(iDelaySignalling));
	EGLBoolean res = iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(res == EGL_TRUE); 
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-075

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that eglTerminate() destroys sync object associated with display.

@SYMTestActions
1. Create sync object for the default display. NULL are specified in attribute_list. 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR.
2. Call eglTerminate() without deleting the sync objects.

@SYMTestExpectedResults
1. Sync object handle doesn't equal to EGL_NO_SYNC_KHR.
2. Test completes without any error and panic. No memory leak.
*/

TVerdict CEglTest_SyncObject_Positive_Terminate::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-075"));
    INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_Terminate::doTestStepL"));
    
    if(!iIsSyncObjectExtensionSupported)
        {
        INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
        CleanAll();
        RecordTestResultL();
        CloseTMSGraphicsStep();
        return TestStepResult();
        }
        
    ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
    GetDisplayL();
    CreateEglSessionL();
	iEglSess->InitializeL();
    
    INFO_PRINTF1(_L("Create sync object - default display, NULL at attributes, type parameter set to EGL_SYNC_REUSABLE_KHR"));
    iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, NULL);
    TESTL(iSyncObject != EGL_NO_SYNC_KHR);
    
    //check attributes of the sync object.
    INFO_PRINTF1(_L("Check attributes of sync object"));    
    CheckSyncAttrib(EGL_SYNC_TYPE_KHR, EGL_SYNC_REUSABLE_KHR);
    CheckSyncAttrib(EGL_SYNC_STATUS_KHR, EGL_UNSIGNALED_KHR);
    
    TRAPD(result, TerminateDisplayL());
    TEST(result == KErrNone);
    
    CleanAll();
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-080

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that eglTerminate() destroys sync object and will unblock thread waiting for the signal.

@SYMTestActions
1.  Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
    The type parameter is set to EGL_SYNC_REUSABLE_KHR
2.  Launch thread A
3.  Call eglClientWaitSyncKHR() on the sync object in thread A.
4.  Call eglTerminate() from main thread without deleting the sync object.

@SYMTestExpectedResults
1. Sync object handle doesn't equal to EGL_NO_SYNC_KHR.
3. Thread A will be stalled
4. Thread A which waited for the signal must be unblocked as if eglSignalSyncKHR() has been called. 
   The function eglClientWaitSyncKHR should return EGL_CONDITION_SATISFIED_KHR.
   Test completes without any error and panic. No memory leak.
*/

TVerdict CEglTest_SyncObject_Positive_TerminateBeforeTimeoutExpired::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-080"));
    INFO_PRINTF1(_L("CEglTest_SyncObject_Positive_TerminateBeforeTimeoutExpired::doTestStepL started...."));

    if(!iIsSyncObjectExtensionSupported)
        {
        INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
        CleanAll();
        RecordTestResultL();
        CloseTMSGraphicsStep();
        return TestStepResult();
        }

    ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
    GetDisplayL();
    CreateEglSessionL();
	iEglSess->InitializeL();
    
    INFO_PRINTF1(_L("Create sync object"));
    iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
    TESTL(iSyncObject != EGL_NO_SYNC_KHR);

    INFO_PRINTF1(_L("Launching threads A"));
    Test_MultiThreadL(1, EFalse);

    INFO_PRINTF1(_L("Wait for a bit before eglTerminate"));
    User::After(iDelaySignalling);

    TRAPD(result, TerminateDisplayL());
    TEST(result == KErrNone);
    
    CleanAll();
    RecordTestResultL();
    CloseTMSGraphicsStep();
    INFO_PRINTF1(_L("....CEglTest_SyncObject_Positive_TerminateBeforeTimeoutExpired::doTestStepL completed!"));
    return TestStepResult();
    }

void CEglTest_SyncObject_Positive_TerminateBeforeTimeoutExpired::doThreadFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_SyncObject_Positive_TerminateBeforeTimeoutExpired::doThreadFunctionL, Thread %d"),aIdx);
    ASSERT_TRUE(aIdx==0);

    INFO_PRINTF2(_L("Thread %d waiting for main thread to call eglTerminate(), with timeout EGL_FOREVER_KHR"), aIdx);
    TUint32 timestampBefore = User::NTickCount();
    EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
    TUint32 timestampAfter = User::NTickCount();
    TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);

    INFO_PRINTF5(_L("Thread %d waited on eglClientWaitSyncKHR for %d usec before sync object was destroyed by eglTerminate() (threshold was [%d +- %d])"), aIdx, timeElapsed, iDelaySignalling, iThreshold);
    TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);
    TEST (Abs(timeElapsed - iDelaySignalling) < iThreshold);
    }

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-100

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To ensure that sync object API generates correct errors and returns correct 
value if wrong parameters supplied.

@SYMTestActions
1. Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR
2. Call all sync API for the display which doesn't exist in the system.
3. Call eglCreateSyncKHR() with unsupported type parameter.
4. Call eglCreateSyncKHR() with specified attributes, type parameter set to EGL_SYNC_REUSABLE_KHR
5. Call eglCreateSyncKHR() with unspecified attributes, type parameter set to EGL_SYNC_REUSABLE_KHR
6. Call all sync API (with the exception to eglCreateSyncKHR(...), providing invalid EGLSyncKHR as a parameter 
7. Call eglClientWaitSyncKHR(), providing unspecified flags.
8. Call eglSignalSyncKHR(), providing unspecified status.
9. Call eglGetSyncAttribSyncKHR(), providing unspecified attributes.
10. Destroy the sync object

@SYMTestExpectedResults
2. eglCreateSyncKHR(...) should return EGL_NO_SYNC_KHR and EGL_BAD_DISPLAY error is generated. 
   eglClientWaitSyncKHR(...), eglSignalSyncKHR(...), eglGetSyncAttribKHR(...) and eglDestroySyncKHR(...) 
   should return EGL_FALSE and EGL_BAD_DISPLAY error is generated.
3. Should return EGL_NO_SYNC_KHR and EGL_BAD_ATTRIBUTE error is generated.
4.5 Should return EGL_NO_SYNC_KHR and EGL_BAD_ATTRIBUTE error is generated.
6, 7, 8. Should return EGL_FALSE and EGL_BAD_PARAMETER error is generated
9. Should return EGL_FALSE and EGL_BAD_ATTRIBUTE error is generated
*/
TVerdict CEglTest_SyncObject_Negative_WrongParameters::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-100"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Negative_WrongParameters::doTestStepL"));
	
	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();

	//create a sync object
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);
	TEST(eglGetError() == EGL_SUCCESS);

	INFO_PRINTF1(_L("Call all sync API for the display which doesn't exist in the system"));
	EGLDisplay	invalidDisplay = iDisplay + 1;
	iSyncObject1 = iPfnEglCreateSyncKHR(invalidDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TEST(iSyncObject1 == EGL_NO_SYNC_KHR);
	TEST(eglGetError() == EGL_BAD_DISPLAY);
	EGLBoolean res = iPfnEglSignalSyncKHR(invalidDisplay,iSyncObject,EGL_UNSIGNALED_KHR );
	TEST(res == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_DISPLAY);
	EGLint res1 = iPfnEglClientWaitSyncKHR(invalidDisplay,iSyncObject,0,EGL_FOREVER_KHR);
	TEST(res1 == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_DISPLAY);
	EGLint value;
	res = iPfnEglGetSyncAttribKHR(invalidDisplay,iSyncObject,EGL_SYNC_STATUS_KHR, &value);
	TEST(res == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_DISPLAY);
	res = iPfnEglDestroySyncKHR(invalidDisplay, iSyncObject);
	TEST(res == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_DISPLAY);
	
	INFO_PRINTF1(_L("Call eglCreateSyncKHR() with unsupported type parameter."));
	const EGLint unsupportedType = 0x3fff;
	iSyncObject1 = iPfnEglCreateSyncKHR(iDisplay, unsupportedType, KEglSyncAttribsNone);
	TEST(iSyncObject1 == EGL_NO_SYNC_KHR);
	TEST(eglGetError() == EGL_BAD_ATTRIBUTE);
	
	const EGLint KSpecifiedAttrib[] = {
								EGL_SYNC_STATUS_KHR, EGL_UNSIGNALED_KHR,
								EGL_NONE
								};
	INFO_PRINTF1(_L("Call eglCreateSyncKHR() with specified condition, type parameter set to EGL_SYNC_REUSABLE_KHR"));
	iSyncObject1 = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KSpecifiedAttrib);
	TEST(iSyncObject1 == EGL_NO_SYNC_KHR);
	TEST(eglGetError() == EGL_BAD_ATTRIBUTE);
	
	INFO_PRINTF1(_L("Call eglCreateSyncKHR() with unspecified attributes, type parameter set to EGL_SYNC_REUSABLE_KHR"));
	const TInt KUndefinedAttrib = 0x4000; 
	const EGLint KUnspecifiedAttrib1[] = {	
								KUndefinedAttrib, EGL_UNSIGNALED_KHR,
								EGL_NONE
								};
	iSyncObject1 = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KUnspecifiedAttrib1);
	TEST(iSyncObject1 == EGL_NO_SYNC_KHR);
	TEST(eglGetError() == EGL_BAD_ATTRIBUTE);
	const EGLint KUnspecifiedAttrib2[] = {	
								EGL_SYNC_STATUS_KHR, KUndefinedAttrib,
								EGL_NONE
								};
	iSyncObject1 = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KUnspecifiedAttrib2);
	TEST(iSyncObject1 == EGL_NO_SYNC_KHR);
	TEST(eglGetError() == EGL_BAD_ATTRIBUTE);
	
	INFO_PRINTF1(_L("Call all sync API (with the exception to eglCreateSyncKHR(...), providing invalid EGLSyncKHR as a parameter"));
	res = iPfnEglSignalSyncKHR(iDisplay,iSyncObject1,EGL_UNSIGNALED_KHR );
	TEST(res == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_PARAMETER);
	res1 = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject1,0,EGL_FOREVER_KHR);
	TEST(res1 == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_PARAMETER);
	res = iPfnEglGetSyncAttribKHR(iDisplay,iSyncObject1,EGL_SYNC_STATUS_KHR, &value);
	TEST(res == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_PARAMETER);
	res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject1);
	TEST(res == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_PARAMETER);
	
	INFO_PRINTF1(_L("Call eglClientWaitSyncKHR(), providing unspecified flags"));
	const TInt KUnspecifiedFlags = 0x002;
	res1 = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,KUnspecifiedFlags,EGL_FOREVER_KHR);
	TEST(res1 == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_PARAMETER);
		
	INFO_PRINTF1(_L("Call eglSignalSyncKHR(), providing unspecified status"));
	const EGLint unspecifiedStatus = 0x3fff;
	res = iPfnEglSignalSyncKHR(iDisplay,iSyncObject,unspecifiedStatus);
	TEST(res == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_PARAMETER);

	INFO_PRINTF1(_L("Call eglGetSyncAttribSyncKHR(), providing unspecified attributes"));
	const EGLint unspecifiedAttribute = 0x3fff;
	res = iPfnEglGetSyncAttribKHR(iDisplay,iSyncObject,unspecifiedAttribute, &value);
	TEST(res == EGL_FALSE);
	TEST(eglGetError() == EGL_BAD_ATTRIBUTE);
	//close sync object
	res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);

	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("End of CEglTest_SyncObject_Negative_WrongParameters::doTestStepL"));
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-105

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that internal version of the signal function, which is accessible 
via extension mechanism, doesn't generate an error.

@SYMTestActions
1.	Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
    The type parameter is set to EGL_SYNC_REUSABLE_KHR.
2.	Call egl_Private_SignalSyncNOK() on sync object supplying invalid status
3.	Call eglGetError()
4.	Call egl_Private_SignalSyncNOK() on sync object with EGL_SIGNALED_KHR flag supplying invalid display
5.	Call eglGetError()
6.	Retrieve status attribute 
7.	Call eglQueryString(..) supplying invalid name parameter (for instance, "OpenGL_Wrong" ).
8.	Call egl_Private_SignalSyncNOK() on sync object with EGL_SIGNALED_KHR flag supplying invalid display
9.	Call eglGetError()
10. Destroy the sync object

@SYMTestExpectedResults
2. The function returns EGL_BAD_ PARAMETERS
3. An error is set to EGL_SUCCESS
4. The function returns EGL_BAD_DISPLAY.
5. An error is set to EGL_SUCCESS
6. The status attributes indicates that object is still in a signaled state (EGL_UNSIGNALED_KHR)
8. The function returns EGL_BAD_DISPLAY.
9. An error is set to EGL_BAD_PARAMETER
*/
TVerdict CEglTest_SyncObject_Negative_SignalImpl::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-105"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Negative_SignalImpl::doTestStepL"));
	
	if(!iIsSyncObjectExtensionSupported || !iIsSyncObjectPrivateExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extensions (whether EGL_KHR_reusable_sync or EGL_NOK__private__signal_sync or both) \
are not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
	
	INFO_PRINTF1(_L("Create the sync object"));
	iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
	TESTL(iSyncObject != EGL_NO_SYNC_KHR);
	
	INFO_PRINTF1(_L("Call egl_Private_SignalSyncNOK() on sync object supplying invalid status"));
	const EGLint unspecifiedMode = 0x3fff;
	EGLint res1 = iPfnEglPrivateSignalSyncNOK(iDisplay, iSyncObject, unspecifiedMode);
	TEST(res1 == EGL_BAD_PARAMETER);
	TEST(eglGetError() == EGL_SUCCESS);
	
	INFO_PRINTF1(_L("Call egl_Private_SignalSyncNOK() on sync object with EGL_SIGNALED_KHR flag supplying invalid display"));
	const EGLDisplay invalidDisplay = iDisplay + 1;
	res1 = iPfnEglPrivateSignalSyncNOK(invalidDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(res1 == EGL_BAD_DISPLAY);
	TEST(eglGetError() == EGL_SUCCESS);
	CheckSyncAttrib(EGL_SYNC_STATUS_KHR,EGL_UNSIGNALED_KHR); //the status is still "unsignaled"
	
	INFO_PRINTF1(_L("Call eglQueryString(..) supplying invalid name parameter ")); //the actual API doesn't matter, we need to generate an error 
	const TInt invalidExtension = 0xffff;
	const char* extensionsString = eglQueryString(iDisplay, invalidExtension); //this should generate EGL_BAD_PARAMETER
	TEST(!extensionsString);
	
	res1 = iPfnEglPrivateSignalSyncNOK(invalidDisplay, iSyncObject, EGL_SIGNALED_KHR);
	TEST(res1 == EGL_BAD_DISPLAY);
	TEST(eglGetError() == EGL_BAD_PARAMETER); //check that error code was not overidden
	
	EGLBoolean res = iPfnEglDestroySyncKHR(iDisplay, iSyncObject);
	TEST(res == EGL_TRUE);	
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("End of CEglTest_SyncObject_Negative_SignalImpl::doTestStepL"));
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-SYNC-OBJECT-110

@SYMTestPriority 1

@SYMPREQ 2400

@SYMREQ 12326

@SYMTestCaseDesc
To check that system correctly processing errors in the environment with shortage of memory.

@SYMTestActions
1. Run a loop (step 2 to 7) with fail rate counting from 1 upward until 
   the following operations succeed
2. Set heap failure with the fail rate specifying in step 1
3. Create sync object for the default display. No attributes are specified (containing only EGL_NONE). 
   The type parameter is set to EGL_SYNC_REUSABLE_KHR. Skip the following test steps if operation doesn't succeed
4. Launch thread A
5. Wait on the sync object
6. Signal from thread A on the sync object the flag EGL_SIGNALED_KHR 
7. Destroy the sync object
8. Reset heap

@SYMTestExpectedResults
3. If memory is not enough to fulfil an operation EGL_NO_SYNC_KHR is returned. Otherwise it should return a handle to a valid sync object
5. The main thread should be stalled
6. The main thread should be unblocked
*/
TVerdict CEglTest_SyncObject_Negative_OOM::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-SYNC-OBJECT-110"));
	INFO_PRINTF1(_L("CEglTest_SyncObject_Negative_OOM::doTestStepL"));
#ifndef _DEBUG
	WARN_PRINTF1(_L("CEglTest_SyncObject_Negative_OOM can only be run in debug mode"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
#else
	
	if(!iIsSyncObjectExtensionSupported)
		{
		INFO_PRINTF1(_L("Sync object extension is not supported, the test will not be executed"));
		CleanAll();
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	ASSERT_TRUE(iDisplay == EGL_NO_DISPLAY);
	GetDisplayL();
	CreateEglSessionL();
	iEglSess->InitializeL();
 	
	INFO_PRINTF1(_L("Retrieving the heapMark functions for memory testing..."));
	TEglDebugHeapMarkStartPtr		heapMarkStart			= reinterpret_cast<TEglDebugHeapMarkStartPtr>(eglGetProcAddress("egliDebugHeapMarkStart"));
	TEglDebugHeapMarkEndPtr			heapMarkEnd				= reinterpret_cast<TEglDebugHeapMarkEndPtr>(eglGetProcAddress("egliDebugHeapMarkEnd"));
	TEglDebugSetBurstAllocFailPtr	setSyncObjectAllocFail	= reinterpret_cast<TEglDebugSetBurstAllocFailPtr>(eglGetProcAddress("egliDebugSetBurstAllocFail"));
	TESTL(heapMarkStart && heapMarkEnd && setSyncObjectAllocFail);
	
	EGLint err = EGL_BAD_ALLOC;
	for(TUint failAfter=1; (err == EGL_BAD_ALLOC); ++failAfter)
		{
		__UHEAP_MARK;
		heapMarkStart();
		
		// Set heap failure
		INFO_PRINTF2(_L("Set EGL heap to fail after %u allocations"), failAfter);
		setSyncObjectAllocFail(static_cast<EGLenum>(RHeap::EBurstFailNext), failAfter, 742);

		INFO_PRINTF1(_L("Create sync object"));
		iSyncObject = iPfnEglCreateSyncKHR(iDisplay, EGL_SYNC_REUSABLE_KHR, KEglSyncAttribsNone);
		err = eglGetError();
		if(err == EGL_SUCCESS)
			{
			TEST(iSyncObject != EGL_NO_SYNC_KHR);
			//launch threadA
			Test_MultiThreadL(1, EFalse);
			TUint32 timestampBefore = User::NTickCount();
			EGLint resClientWait = iPfnEglClientWaitSyncKHR(iDisplay,iSyncObject,0,EGL_FOREVER_KHR);
			TUint32 timestampAfter = User::NTickCount();
			TInt timeElapsed = ElapsedTime(timestampAfter, timestampBefore);
			TEST(resClientWait == EGL_CONDITION_SATISFIED_KHR);//we do not expect failure here, as eglClientWaitSyncKHR function doesn't allocate any memory
			TEST(Abs(timeElapsed - iDelaySignalling) < iThreshold);
			//close sync object
			TEST(iPfnEglDestroySyncKHR(iDisplay, iSyncObject)== EGL_TRUE);
			
			//Wait for the thread to finish before launching the thread with the same index again.
			Test_MultiThread_WaitL(ETrue, TThreadStatus::ELogin);
			}
		else
			{
			TEST(iSyncObject == EGL_NO_SYNC_KHR);
			}
		// Unset heap failure
		setSyncObjectAllocFail(static_cast<EGLenum>(RHeap::ENone), 0, 0);
		(void)heapMarkEnd(0);
		__UHEAP_MARKEND;
		}
	TEST(err == EGL_SUCCESS);
	
	CleanAll();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	INFO_PRINTF1(_L("End of CEglTest_SyncObject_Negative_OOM::doTestStepL"));
	return TestStepResult();
#endif	
	}

void CEglTest_SyncObject_Negative_OOM::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("CEglTest_SyncObject_Negative_OOM::doThreadFunctionL, Thread %d"),aIdx);
	ASSERT_TRUE(aIdx==0);

	User::After(TTimeIntervalMicroSeconds32(iDelaySignalling));
	TEST(iPfnEglSignalSyncKHR(iDisplay, iSyncObject, EGL_SIGNALED_KHR) == EGL_TRUE); //we do not expect failure here, as eglSignalSyncKHR function doesn't allocate a memory
	}
