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


#include <e32base.h>
#include "ssmadaptation.h"
#include "ssmdebug.h"
#include "ssmadaptationcmn.h"


#ifndef __SUSSIMADAPTATION_H__
#define __SUSSIMADAPTATION_H__


/**
CSimAdaptationRequests
CSimAdaptationObservers

These classes implement the Sim Adaptation related functionality as part of 
Adaptation server.These classes implement an active object to route the requests
and notifications to Adaptation plugins loaded by CSsmAdaptationServer.
These classes are also responsible for queueing the requests and pooling
the notification requests  when requests are received from multiple clients
Adaptation server handles only one request from each session and uses 
Cancel() methods to cancel any of the session's requests made before.
The queueing mechanism is to handle multiple clients at a time not to handle
multiple requests from one particular clients session.

@internalComponent
*/


class CSimAdaptationRequests : public CActive
{
public:
    static CSimAdaptationRequests* NewL(MSimAdaptation& aAdaptation);
	~CSimAdaptationRequests();


	void Release();
	void DoGetSimOwnedL(const RMessage2& aMessage);
	void DoGetSimCancel(const RMessage2& aMessage);

	MSimAdaptation& Adaptation();
	
protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private:
	CSimAdaptationRequests(MSimAdaptation& aAdaptation);
	void SubmitOrQueueL(const RMessage2 &aMessage);	 		
	void WriteResponseDataToClientMessageL();
	void Submit(CAdaptationMessage*& aMessage);

private:
	CAdaptationMessage *iCurrentMessage;
	RSsmAdaptationRequestQueue iPendingRequestsQueue;	

	MSimAdaptation& iSimAdaptation;

	TBool iSimOwnershipStatus;
	TBool iSimChangedStatus;
	
	TPckgBuf<TBool> iSimOwnedPckg;
};

/**
Internal class for maintaining observers for notifications.Whenever a client 
registers for  SimAdaptation notifications it will be added to the observers
list and it will be notified when CSimAdaptationObservers receives notification
from the Sim Adaptation plugin loaded by CSsmAdaptationServer.Once client receives 
the notification it has to register again for getting further notification.  

@internalComponent
*/

class RSimAdaptationObserversList
	{
public:
	void AddObserverL(CAdaptationMessage *aNotificationMessage);
	void Close();
	TInt Count();
	void NotifyAndRemoveAll(TSsmSimEventType aEventType,TInt aCompleteCode);
	void RemoveObserver(CAdaptationMessage *);
private:
	RPointerArray<CAdaptationMessage > iObservers;
	};


/**
CSimAdaptationRequests
CSimAdaptationObservers

These classes implement the Sim Adaptation related functionality as part of 
Adaptation server.These classes implement an active object to route the requests
and notifications to Adaptation plugins loaded by CSsmAdaptationServer.
These classes are also responsible for queueing the requests and pooling
the notification requests  when requests are received from multiple clients
Adaptation server handles only one request from each session and uses 
Cancel() methods to cancel any of the session's requests made before.
The queueing mechanism is to handle multiple clients at a time not to handle
multiple requests from one particular clients session.

@internalComponent
*/


class CSimAdaptationObservers : public CActive
{
public:
    static CSimAdaptationObservers* NewL(MSimAdaptation& aAdaptation);
	~CSimAdaptationObservers();

	void DoGetLastSimEvent(const RMessage2& aMessage);	
	void DoNotifySimEventL(const RMessage2& aMessage);
	void DoNotifySimEventCancelL(const RMessage2& aMessage);

protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

	
private:
	CSimAdaptationObservers(MSimAdaptation& aAdaptation);
	void WriteResponseDataToClientMessage();
	
	void StartNotification();
		
private:
	MSimAdaptation& iSimAdaptation;

	RSimAdaptationObserversList iObserversList;
	
	//Added for temp purposes
	TPckgBuf<TSsmSimEventType> iEventPckg;	
};




#endif // __SUSSIMADAPTATION_H__
