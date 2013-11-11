// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements the main methods for the CTestBase class.  Note that this class has
// at least two pure virtual functions which must be implemented by classes which inherit or
// derive from this class.  The code in this file is responsible for starting the thread which
// acts as the Modem Emulator.  This thread then is responsible for starting the processing of
// the proper script.  The StartEmulator method is called by each test, typically in the RunTest
// method of the test code.
// 
//

/**
 @file
*/

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define PDD2_NAME _L("EUART2")
#define LDD_NAME _L("ECOMM")
#endif

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackcrxmess.h"
#include "Te_LoopBackCNoPduRxMess.h"
#include "Te_LoopBackcsca.h"
#include "Te_LoopBackctxmess.h"
#include "Te_LoopBackcdatacall.h"
#include "Te_LoopBackcoddinits.h"
#include "Te_LoopBackcmessstor.h"
#include "Te_Loopbackcfailinits.h"
#include "Te_LoopBackcincall.h"
#include "Te_LoopBackcsmscancel.h"
#include "Te_LoopBackcshutdown.h"
#include "Te_LoopBackCSmsdelete.h"
#include "Te_LoopBackcssfax.h"
#include "Te_LoopBackcdatacallcancel.h"
#include "Te_Loopbackcfaxpremclose.h"
#include "Te_LoopBackctwofaxrx.h"
#include "Te_LoopBackcvoicecall.h"
#include "Te_LoopBackCDataCallCallBack.h"
#include "Te_LoopBackCDataCallRemoteHangupDial.h"
#include "Te_LoopBackCDataCallRemoteTerm.h"
#include "Te_LoopBackcphoneBook.h"
#include "Te_LoopBackCNetwork.h"
#include "Te_LoopBackCPhoneLine.h"
#include "Te_LoopBackCCalls.h"
#include "Te_LoopBackCNoCnmi.h"
#include "Te_LoopBackcnocmgf.h"
#include "Te_Loopbackcoomvoicecall.h"
#include "Te_LoopBackCDataCallDiffParam.h"
#include "Te_LoopBackCCbstParse.h"
#include "Te_LoopBackccgqreq_responses.h"
#include "Te_LoopBackCUnsolicited.h"

#define MODEM_EMULATOR _L("ModemEmulator")

TInt ResponderThread(TAny* aScriptNum);

const TInt KModEmStackSize=0x8000;
const TInt KModEmHeapSize=0x8000;
const TInt KModEmMaxHeapSize=0x80000;


CTestBase::CTestBase()
/**
 * This method is the basic empty constructor for the CTestBase class.
 */
	{
 	//
 	// Change this process and thread to a higher priority so that we can be in control
 	// when performing tests such as monitoring async request status values.
 	//
	iOrgProcessPriority = RProcess().Priority();
 	iOrgThreadPriority  = RThread().Priority();
   
 	RProcess().SetPriority(EPriorityHigh);
 	RThread().SetPriority(EPriorityMuchMore);	
	}


CTestBase::~CTestBase()
	{
	//
	// Restore the process and thread priority...
	//
	RProcess().SetPriority(iOrgProcessPriority);
	RThread().SetPriority(iOrgThreadPriority);
	
	}

TVerdict CTestBase::doTestStepPreambleL( void )
/**
 * This method connects two
 * sessions to the RTelServer.  It also loads the iPhone module (specified by the define
 * MODULE_NAME) and opens two iPhone objects, one from each server.  If any of these operations
 * fail, this method leaves.
 *
 * @leave	This method leaves if it is unable to connect to the RTelServer or if it is unable
 *			to load and open the phones.
 */
{
	__UHEAP_MARK;

	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);

	TInt ret = iServer.Connect();
	ERR_PRINTF2(TRefByValue<const TDesC>(_L("First Server Connect Status = %d\n")), ret);
	//User::LeaveIfError(ret);
	if (ret)
		{
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret=iServer.LoadPhoneModule(MODULE_NAME);
	ERR_PRINTF2(TRefByValue<const TDesC>(_L("LoadPhoneModuleError = %d\n")), ret);
	//User::LeaveIfError(ret);
	if (ret)
		{
		iServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret = iServer2.Connect();
	ERR_PRINTF2(TRefByValue<const TDesC>(_L("Second Server Connect Status = %d\n")), ret);
	//User::LeaveIfError(ret);
	if (ret)
		{
		iServer.UnloadPhoneModule(MODULE_NAME);
		iServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

//
//  Open the "iPhone" object prior to the test
//
	ret = iPhone.Open(iServer, PHONE_NAME);
	ERR_PRINTF2(TRefByValue<const TDesC>(_L("First Phone Error Code = %d\n")), ret);
	if (ret)
		{
		iPhone.Close();
		iServer2.Close();
		iServer.UnloadPhoneModule(MODULE_NAME);
		iServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret = iPhone2.Open(iServer, PHONE_NAME);
	ERR_PRINTF2(TRefByValue<const TDesC>(_L("Second Phone Error Code = %d\n")), ret);
	if (ret)
		{
		iPhone.Close();
		iPhone2.Close();
		iServer2.Close();
		iServer.UnloadPhoneModule(MODULE_NAME);
		iServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}
	return TestStepResult();
}

TVerdict CTestBase::doTestStepPostambleL( void )
/**
 * This method unloads the iPhone module and closes the phones and servers opened by the
 * doTestStepPreambleL method.
 */
{
	iPhone.Close();
	iPhone2.Close();
	iServer2.Close();
	iServer.UnloadPhoneModule(MODULE_NAME);
	iServer.Close();
	delete iTestScheduler;

	__UHEAP_MARKEND;
	return TestStepResult();
}

TVerdict CTestBase::doTestStepL( void ) 
/**
 *This method just wrap over 'old' RunTestL()
 */
{

	if(RunTestL() != KErrNone || TestStepResult() == EFail)
		SetTestStepResult(EFail);
	return TestStepResult();

} 


TInt CTestBase::StartEmulatorL()
//
// Create modem emulator thread, call function to drive ETel API and then wait for thread
// to complete.
//
/**
 * This method is used by specific tests to start the modem emulation.  The method
 * starts a new thread in the current process with the entry point of ResponderThread.  The
 * responder thread is passed a pointer to the current script (iCurrentScript).  If the modem
 * emulator is already running, this code will attempt to wait and retry the thread
 * creation.  If it exceeds it's maximum retry limit, the requested test will not be run
 * and an error code will be returned.  A semaphore is created by StartEmulator for
 * synchronization with the newly created thread.
 *
 * After the thread is created and the synchronization is complete, this method issues a Logon
 * request to the thread to monitor for the death of the thread.  If the Responder thread
 * dies, an event will be received by the current thread.  Then, StartEmulator calls the
 * DriveETelAPI method associated with the specific test.  DriveETelApiL is a pure virtual
 * function within CTestBase.
 *
 * After the called DriveETelApiL this method exits,
 *
 * @return		KErrNone
 * @return		KErrAlreadyExists is returned if the modem emulator exists and the retry limit
 *              expires.
 * @return		Variable depending on the return value from the test's DriveETelApiL method and
 *				thread monitoring.
 */
	{
	//RSemaphore s;
	TInt res=KErrNone;
	TThreadData threadData;

	// setup data to pass to the Responder Thread when creating it.
	// Note: because "threadData" is an automatic, it is assumed when the Responder Thread
	//       receives this data (passed by reference), is copied into thread specific storage
	//       by the Responder Thread before the semaphore is given back to this function to
	//       indicate the Responder Thread was created successfully.
	threadData.iVariableDelay= iVariableDelay;
	threadData.iScriptList = iCurrentScript;
	threadData.iRestartReqStat=&iRestartReqStat;

	res=iScriptThread.Create(MODEM_EMULATOR,ResponderThread,
					KModEmStackSize,KModEmHeapSize,KModEmMaxHeapSize,&threadData);
	if(res==KErrAlreadyExists)
		{
		for(TInt i=0;i<3;i++)
			{
			iScriptThread.Close();
			User::After(1000000L);
			res=iScriptThread.Create(MODEM_EMULATOR,ResponderThread,
							KModEmStackSize,KModEmHeapSize,KModEmMaxHeapSize,&threadData);
			if(res!=KErrAlreadyExists)
				{
				break;
				}		
			}
		 }
			 
	TRequestStatus statusRequest;

	if(res==KErrNone)
		{
		// Create a Rendezvous with the thread. This will be used
		// to ensure that the thread has been started before continuing 
		TRequestStatus stat;
		iScriptThread.Rendezvous(stat);
				
		if (stat!=KRequestPending)
			{
			iScriptThread.Kill(KErrGeneral);
			}
		else
			{
			// Make the thread eligible for execution
			iScriptThread.Logon(statusRequest);
			iScriptThread.Resume();			
			}
			
		// Wait for thread to complete the rendezvous
		User::WaitForRequest(stat);		
			
		if (stat != KErrNone)
			{
			res=stat.Int();
			return res;
			} 	 
		}
	else
		{
		iScriptThread.Logon(statusRequest);
		}

	// Wait for the Restart Signal active object to be set.
	for(;;)
		{
		if(iRestartReqStat)
			{
			break;
			}
		User::After(500000L);
		}

	res=DriveETelApiL();

	if(res!=KErrNone)
		{
		iScriptThread.Close();
		return res;
		}

	User::WaitForRequest(statusRequest);
	iScriptThread.Close();
	res=statusRequest.Int();
	if(res==KErrCancel)
		{
		res=KErrNone;	// Legitimate termination
		}
		
	return res;
	}

void CTestBase::RestartScript()
	{
	__ASSERT_ALWAYS(iRestartReqStat!=NULL,User::Panic(_L("T_TsyLb Panic"),1));
	TRequestStatus* reqStatus=iRestartReqStat;
	iScriptThread.RequestComplete(reqStatus,KErrNone);
	}

void CTestBase::TerminateScript()
	{
	iScriptThread.Terminate(KErrCancel);
	}

