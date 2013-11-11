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

#ifndef CLTYSRECEIVETHREADEVENTSTATEHANDLER_H
#define CLTYSRECEIVETHREADEVENTSTATEHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "ltsymacros.h"

#include "mltsyreceivethreadcallbackinterfaces.h"

/**
 * 
 * The purpose of this class is to handle 
 * 	-posting a request to receive a event from baseband (DoRequestNextEvent)
 * - handling the event under the ETEL thread of execution (HandleReceivedEvent)
 * -(optionally) releasing any store events after event has been processed (DoReleaseEvent)
 * - handling if the receive thread dies
 *
 * This skeleton version of the class provides default implementations for each of the state handlers that should be properly implemented. 
 */

class CLtsyReceiveThreadEventStateHandler : public CBase, public MLtsyReceiveThreadEventStateHandler, public MLtsyReceiveThreadObserver
	{
public:
	static CLtsyReceiveThreadEventStateHandler* NewL(/*Add any arguments here*/);
	~CLtsyReceiveThreadEventStateHandler();

public: //from MLtsyReceiveThreadEventStateHandler
	virtual void DoRequestNextEvent();
	virtual	TInt HandleReceivedEvent();
	virtual void DoReleaseEvent();
	
public: //from MLtsyReceiveThreadObserver
	virtual void HandleReceiveThreadLifeWatcherComplete(TInt aError);
	
protected:
	void ConstructL();
	CLtsyReceiveThreadEventStateHandler();
private:
	// Can be removed in real LTSY implementation.
	MTEST_DECLARE_EVENT_COUNTER
	};


#endif //CLTYSRECEIVETHREADEVENTSTATEHANDLER_H
