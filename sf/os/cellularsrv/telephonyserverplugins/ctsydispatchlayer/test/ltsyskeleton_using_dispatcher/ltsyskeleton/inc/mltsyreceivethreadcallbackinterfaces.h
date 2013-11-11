// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef MLTSYRECEIVETHREADCALLBACKIF_H
#define MLTSYRECEIVETHREADCALLBACKIF_H

class MLtsyReceiveThreadEventStateHandler
/**
 * Interface to be implmented to provide LTSY specific handling for the management of 
 * the LtsyReceiveThread
 */
	{
public:
	enum
	{
	/** 
	 * When used as a completion value from ETEL thread to LtsyReceiveThread, 
	 * will cause gracefule shutdown of LtsyReceiveThread
	 */
	KShutdownLtsyReceiveThread = 1
	};
public:	
	/**
	 * Function to post a request to receive the next event from baseband.
	 * Request is expected to be synchronous
	 * 
	 * On receiving the event it should be stored for future use, 
	 * after the thread-switching has taken place. 
	 * 
	 * This function will be called in the LtsyReceiveThread thread of execution.
	 */
	virtual void DoRequestNextEvent() = 0;
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
	virtual	TInt HandleReceivedEvent()=0;
	/**
	 *This function is called after an event has been processed and
	 * the ETEL thread has signalled back to the LtsyReceiveThread. 
	 * 
	 * It is an opportunity for the LTSY to free any memory associated with the event.
	 * 
	 * This function will be called in the LtsyReceiveThread thread of execution. 
	 */
	virtual void DoReleaseEvent()=0;
	
	}; // class MLtsyReceiveEventThreadStateHandler

class MLtsyReceiveThreadObserver
/** Interface to be implmented to provide LTSY specific handling if the 
 * LtsyReceiveThread dies.
 */
	{
public:
	/**
	 * This function is invoked (within the context of the ETEL thread) when the LtsyReceiveThread
	 * dies.
	 * 
	 * It purpose is to allow some action to be performed.  An example caction could be 
	 * to panic the current (ETEL) thread, which would cause a reboot, since the thread is
	 * system critical.  The reason for panic'ing could be because if no events are able to
	 * be received from the baseband then phone is inoperable and therefore mustbe reset.
	 * would cause a 
	 * @param aError Cause code for thread death
	 */
	virtual void HandleReceiveThreadLifeWatcherComplete(TInt aError)=0;
	}; //class MLtsyReceiveThreadObserver

#endif // MLTSYRECEIVETHREADCALLBACKIF_H
