/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/




/**
 @file
 @publishedPartner 
 @released
*/

#ifndef MAUDIOCONTEXTOBSERVER_H
#define MAUDIOCONTEXTOBSERVER_H

#include <a3f/a3fbase.h>

const TUid KUidA3FContextCommitUpdate		= {0x1028343E};
const TUid KUidA3FContextPreEmptedCommit	= {0x1028343F};
const TUid KUidA3FContextPreEmption			= {0x10283440};
const TUid KUidA3FContextPreEmptionUpdate	= {0x10283441};
const TUid KUidA3FContextUpdateComplete		= {0x10283442};
const TUid KUidA3FContextAbort				= {0x10283443};

/**
 * KUidA3FContextCommitUpdate - Follows a Commit() call. Signals that Commit() has been successfully received by MMRC 
 * and that update callbacks are about to follow.  Note will still be called if MMRC rejects the request for policy reasons 
 * – in which case, the callbacks will show the original value. 
 *if aError is not KErrNone, then the implementation tried to apply the change but this failed 
 * so that the implementation is “rolled-back” to the previous. However, KErrAbort says that this roll-back failed, 
 * and streams have been placed in a Dead state.
 *
 * KUidA3FContextPreEmptedCommit -  Follows a Commit() call, but signals that that MMRC is doing a simultaneous pre-emption. 
 * Update callbacks are about to follow, but these relate to the pre-emption. The original request is lost.
 * An error of KErrAbort says that the change MMRC requested failed, and typically streams are placed in a “Dead” mode.
 *
 * KUidA3FContextPreEmption -  Initial indication to the client that it is to be pre-empted. 
 * The client should treat the Context and its Components as “locked” – see below. 
 *
 * KUidA3FContextPreEmptionUpdate - Will follow a KUidA3FContextPreEmption and indicates that callbacks 
 * are about to start to indicate the update. An error of KErrAbort says that the change MMRC requested failed, 
 * and typically streams are placed in a “Dead” mode.
 * 
 * KUidA3FContextUpdateComplete - Signals that all callbacks are complete – Commit() can be called again, if required.
 *
 * KUidA3FContextAbort - Error occurred due to the commit or pre-emption cycle itself – 
 * that is something to do with the associated message passing mechanism etc. 
 * this terminates the whole cycle, whatever the current state. It should leave the context in a “reasonable state” 
 *– usually the original state.  aError gives the associated reason for this.
 */

/*
 * Following Commit() or ContextEvent(KUidA3FContextPreEmption), the associated Context should be considered “locked” 
 * until the receipt of events KUidA3FContextCommitUpdate, KUidA3FContextPreEmptedCommit or KUidA3FContextPreEmptionUpdate. 
 * By locked it is meant that no changes shall be requested by the Client from the Context nor its associated Components, 
 * nor Commit() called.
 *
 * Following events KUidA3FContextCommitUpdate, KUidA3FContextPreEmptedCommit or KUidA3FContextPreEmptionUpdate, 
 * the Client can be requested changes to the Context and associated Components, but Commit() cannot be called.
 * 
 * Following event KUidA3FContextUpdateComplete Commit() can be called.
 *
 * Following the receipt of event KUidA3FContextPreEmptedCommit or KUidA3FPreEmptionUpdate, 
 * any pending requested changes to the Context or its Components are lost – there is an implied call to MAudioContext::Reset(). 
 */


/**
 * Defines the interface for observing context states.
 */
class MAudioContextObserver
	{
public:
	/**
	 * Callback to context observer to show progression through Commit() and pre-emption cycles.
	 *
	 * @param a Uid giving the specific event.
	 * @param  an error code. KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual void ContextEvent(TUid aEvent, TInt aError)=0;
	};

#endif // MAUDIOCONTEXTOBSERVER_H
