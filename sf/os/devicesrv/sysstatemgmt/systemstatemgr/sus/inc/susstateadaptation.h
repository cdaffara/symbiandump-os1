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

#include "ssmstate.h"
#include <e32base.h>
#include "ssmadaptation.h"
#include "ssmdebug.h"
#include "ssmadaptationcmn.h"

	
#ifndef __SUSSTATEADAPTATION_H__
#define __SUSSTATEADAPTATION_H__

/**
CStateAdaptationRequests
CStateAdaptationObservers

These classes implement the State Adaptation related functionality as part of 
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

	
class CStateAdaptationRequests : public CActive
{
public:
    static CStateAdaptationRequests* NewL(MStateAdaptation& aAdaptation);
 	~CStateAdaptationRequests();


	void Release();
	void DoRequestCoopSysStateChangeL(const RMessage2& aMessage);
	void DoRequestCoopSysSelfTestL(const RMessage2& aMessage);
	void DoRequestCoopSysPerformRestartActionsL(const RMessage2& aMessage);
	void DoRequestCoopSysPerformShutdownActionsL(const RMessage2& aMessage);
	void DoRequestCoopSysPerformRfsActionsL(const RMessage2& aMessage);
	void DoRequestCancel(const RMessage2& aMessage);

	MStateAdaptation& Adaptation();
	
protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private:
	CStateAdaptationRequests(MStateAdaptation& aAdaptation);
	void SubmitOrQueueL(const RMessage2 &aMessage);	 	
	void Submit(CAdaptationMessage*& aMessage);
	
private:

	CAdaptationMessage *iCurrentMessage;
	RSsmAdaptationRequestQueue iPendingRequestsQueue;	

	MStateAdaptation& iStateAdaptation;
};






/**
Internal class for maintaining observers for notifications.Whenever a client 
registers for  StateAdaptation notifications it will be added to the observers
list and it will be notified when CStateAdaptationObservers receives notification
from the State Adaptation plugin loaded by CSsmAdaptationServer.Once client receives 
the notification it has to register again for getting further notification.  

@internalComponent
*/



class RStateAdaptationObserversList
	{
public:
	void AddObserverL(CAdaptationMessage *aNotificationMessage);
	void Close();
	TInt Count();
	void NotifyAndRemoveAll(TSsmCoopSysEventType aEventType,TInt aCompleteCode);
	void RemoveObserver(CAdaptationMessage *);
private:
	RPointerArray<CAdaptationMessage > iObservers;
	};
	
	
	
	
/**
CStateAdaptationRequests
CStateAdaptationObservers

These classes implement the State Adaptation related functionality as part of 
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




class CStateAdaptationObservers : public CActive
{
public:
    static CStateAdaptationObservers* NewL(MStateAdaptation& aAdaptation);
    //static CStateAdaptationObservers* NewLC();
	~CStateAdaptationObservers();

	//from MStateAdaptation
	void DoGetLastCoopSysEvent(const RMessage2& aMessage);	
	void DoNotifyCoopSysEventL(const RMessage2& aMessage);
	void DoNotifyCoopSysEventCancelL(const RMessage2& aMessage);
	
protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private:
	CStateAdaptationObservers(MStateAdaptation& aAdaptation);
	
	void StartNotification();
private:
	MStateAdaptation& iStateAdaptation;
	RStateAdaptationObserversList iObserversList;
	
	TPckgBuf<TSsmCoopSysEventType> iEventPckg;
};





#endif // __SUSSTATEADAPTATION_H__
