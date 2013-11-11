// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTestMultipleClients.cpp
// This file tests the scenario of Multiple Clients using the Packet API simultaneously.
// It is primarily used to exercise the mutex in the methods RPacketContext::OpenNewContext()
// and RPacketContext::OpenNewSecondaryContext().
// 
//


// Symbian OS includes
#include <e32base.h>
#include <e32std.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTestMultipleClients.h"
#include "Te_etelpckt_defs.h"


// constructor
CEtelPacketTestMultipleClients::CEtelPacketTestMultipleClients()
{
	// store the name of this test case
	SetTestStepName(_L("Multiple_Clients"));
}

// destructor
CEtelPacketTestMultipleClients::~CEtelPacketTestMultipleClients()
{
}


enum TVerdict CEtelPacketTestMultipleClients::doTestStepL( void )
/**
 * Test step Multiple_Clients tests simultaneous use of the API by Multiple clients.
 */
{
// Allocate the size of the stack and heap objects
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;

// Create 2 RThread objects
	_LIT(KClientApp1ThreadName,"ClientApp1");
	_LIT(KClientApp2ThreadName,"ClientApp2");

	RThread thread1;
	RThread thread2;

	TInt res1=thread1.Create(KClientApp1ThreadName,ContextCreationFunction,
					    KStackSize,KHeapSize,KMaxHeapSize,this);

	TInt res2=thread2.Create(KClientApp2ThreadName,ContextCreationFunction,
					    KStackSize,KHeapSize,KMaxHeapSize,this);

	TRequestStatus thread1Status;
	TRequestStatus thread2Status;
	TBool req1Complete=EFalse;
	TBool req2Complete=EFalse;
	
// If both threads have been created successfully, start their execution
	if (res1==KErrNone && res2==KErrNone)
		{
		thread1.Logon(thread1Status);
		thread2.Logon(thread2Status);
		thread1.Resume();
		thread2.Resume();
	 
		while ( req1Complete==EFalse || req2Complete==EFalse)
			{
			User::WaitForAnyRequest();
			if (thread1Status != KRequestPending && req1Complete==EFalse)
				{
				CHECKPOINT(thread1Status.Int(),KErrNone,CHP_CNTXT_CASE("A.MC2"));
				req1Complete=ETrue;
				}
			if (thread2Status != KRequestPending && req2Complete==EFalse)
				{
				CHECKPOINT(thread2Status.Int(),KErrNone,CHP_CNTXT_CASE("A.MC3"));
				req2Complete=ETrue;
				}
			} // while statement
		} // if statement
	
	thread1.Close();
	thread2.Close();

	return TestStepResult();
}


TInt CEtelPacketTestMultipleClients::ContextCreationFunction(TAny* /*aThread*/ )
/**
 * Static thread function. A primary and secondary context are created in this function.
 * The following sequence is carried out in order to create a secondary context:
 * Connect to the Telephony server, Load the Telephony module, open handles to a phone,
 * packet service and primary context object. 
 * Once the secondary context is created, all the handles are closed. Context 
 * functionality is exercised in other test modules and is not repeated here.
 * Note that logging is done explicitly in this function to the generated test result
 * file. This is to enable a tester examine the behaviour of this test without having to 
 * resort to checking the dummy TSY logs.
 *
 * Note The logging code has been temporarily commented out. This is because the logging
 * used by the test harness, scheduletest is not thread safe for multithreaded calls.
 * When the test harness has been modified (either by making it thread safe for multithreaded
 * calls or by using FLOGGER, the logging for this test case should be updated.
 * - 16/05/02
 */
{
	// Create a cleanup stack object
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		return KErrNoMemory;


// Use the 'this' pointer to access functionality available to the test step class.
	//CEtelPacketTestMultipleClients* ptr = (CEtelPacketTestMultipleClients*) aThreadData;
	//ptr->INFO_PRINTF1(_L("Thread %d finished Wait()."), RThread().Id());
	

// Establish a connection to the ETel Server and open handles for this thread.
// An arbitrary value of 20 is assigned for the buffer size for the context names.
// Using a TName object would incur a significant load on the stack, which is 
// unnecessary for this test code.
	RTelServer server;
	RPhone pcktPhone;
	RPacketService wcdmaService;
	RPacketContext primaryContext;
	RPacketContext wcdmaContext;
	TBuf<20>  contextName;
	TBuf<20> secondaryContextName;
	
	TInt ret= KErrNone;
	
	if ( (ret=server.Connect())==KErrNone)
		{
		//ptr->INFO_PRINTF1(_L("Thread %d: Connect to ETel server - Pass."), RThread().Id());
		if ( (ret=server.LoadPhoneModule(DPCKTTSY_MODULE_NAME))==KErrNone)
			{
			//ptr->INFO_PRINTF1(_L("Thread %d: Load phone module - Pass."), RThread().Id());
			if ( (ret=pcktPhone.Open(server,DPCKTTSY_PHONE_NAME))==KErrNone)
				{
				//ptr->INFO_PRINTF1(_L("Thread %d: Open Packet phone - Pass."), RThread().Id());
				if ( (ret=wcdmaService.Open(pcktPhone))==KErrNone)
					{
					//ptr->INFO_PRINTF1(_L("Thread %d: Open Packet service - Pass."), RThread().Id());
					if ( (ret=primaryContext.OpenNewContext(wcdmaService, contextName))==KErrNone)
						{
						//ptr->INFO_PRINTF1(_L("Thread %d: Open Primary context - Pass."), RThread().Id());
						if ( (ret=wcdmaContext.OpenNewSecondaryContext(wcdmaService, contextName, secondaryContextName))==KErrNone)
							{
							//ptr->INFO_PRINTF1(_L("Thread %d: Open Secondary context - Pass."), RThread().Id());
							wcdmaContext.Close();
							//ptr->INFO_PRINTF1(_L("Thread %d: Close Secondary context - Pass."), RThread().Id());
							}
						primaryContext.Close();
						//ptr->INFO_PRINTF1(_L("Thread %d: Close Primary context - Pass."), RThread().Id());
						}
					wcdmaService.Close();
					//ptr->INFO_PRINTF1(_L("Thread %d: Close Packet service - Pass."), RThread().Id());
					}
				pcktPhone.Close();
				//ptr->INFO_PRINTF1(_L("Thread %d: Close Packet phone - Pass."), RThread().Id());
				}
			server.UnloadPhoneModule(DPCKTTSY_MODULE_NAME);
			//ptr->INFO_PRINTF1(_L("Thread %d: Unload phone module - Pass."), RThread().Id());
			}
		server.Close();
		//ptr->INFO_PRINTF1(_L("Thread %d: Close ETel server - Pass."), RThread().Id());
		}

	delete cleanup;

	return ret;	
}
