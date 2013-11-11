// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements AO to retrieve lists
//

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cltsyreceivethreadeventstatehandlerTraces.h"
#endif

#include "cltsyreceivethreadeventstatehandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsypaniccodes.h"

CLtsyReceiveThreadEventStateHandler* CLtsyReceiveThreadEventStateHandler::NewL(/*Add any arguments here*/)
	{
	CLtsyReceiveThreadEventStateHandler* self = new(ELeave) CLtsyReceiveThreadEventStateHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLtsyReceiveThreadEventStateHandler::CLtsyReceiveThreadEventStateHandler()
	{
	}

CLtsyReceiveThreadEventStateHandler::~CLtsyReceiveThreadEventStateHandler()
	{
	//Add any cleanup here
	}

void CLtsyReceiveThreadEventStateHandler::ConstructL()
	{
	//Add any initialisation here.
	}

void CLtsyReceiveThreadEventStateHandler::DoRequestNextEvent()
/**
 * Function to post a request to receive the next event from baseband.
 * Request is expected to be synchronous
 * 
 * On receiving the event it should be stored for future use, 
 * after the thread-switching has taken place. 
 * 
 * This function will be called in the LtsyReceiveThread thread of execution.
 */
	{
	//Add LTSY specific handling here.
	
	// Can be removed in real LTSY implementation
	MTEST_WAIT_FOR_NEXT_EVENT
	//
	}

TInt CLtsyReceiveThreadEventStateHandler::HandleReceivedEvent()
/**
 * This function is called after the thread has switched from the LtsyReceiveThread
 * to the ETEL thread. This fucntion kicks-off the LTSY processing of the event,
 * that will eventualy lead to a call back into the CTsyDispatcher.
 * 
 * @return TInt This return code is used in the RequestComplete back to the LtsyReceiveThread.
 * - KErrNone means continue looping for next event.
 * - KShutdownLtsyReceiveThread means the thread should shutdown gracefully. I.e. Temrinate thread with KErrNone
 * - All other errors shall be treated as termination.
 *  
 * * This function will be called in the ETEL thread of execution. 
 */
	{
	TSYLOGENTRYEXIT;
	TInt err = KErrNone;
	//Add LTSY specific handling here.
	
	// Can be removed in real LTSY implementation
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CLTSYRECEIVETHREADEVENTSTATEHANDLER_HANDLERECEIVEEVENT_1,"LtsyReceiveThread simulated event = %d", iCounter);
	MTEST_COUNTER_EXCEEDED_SHUTDOWN_RECEIVETHREAD
	//
	 
	
	return TSYLOGSETEXITERR(err);
	}

void CLtsyReceiveThreadEventStateHandler::DoReleaseEvent()
/**
 *This function is called after an event has been processed and
 * the ETEL thread has signalled back to the LtsyReceiveThread. 
 * 
 * It is an opportunity for the LTSY to free any memory associated with the event.
 * 
 * This function will be called in the LtsyReceiveThread thread of execution. 
 */
	{
	//Add LTSY specific handling here.
	}

void CLtsyReceiveThreadEventStateHandler::HandleReceiveThreadLifeWatcherComplete(TInt aError)
	 {
	 //An LTSY implementation should determine what to do if the 
	 //LtsyReceiveThread dies
	 
	 //Probably not much else to do other than panic.  
	 //Since panic is on system critical thread  (ETEL thread)this will cause
	 //phone to reset. But what else can be done if no events are 
	 //being received from the baseband
	 if(aError != KErrNone)
		 {
		 User::Panic(KLtsyPanicCategory,ELtsyReceiveThreadDied);
		 }
	 }

