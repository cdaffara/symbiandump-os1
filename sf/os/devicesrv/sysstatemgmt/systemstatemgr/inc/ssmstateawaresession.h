// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMSTATEAWARESESSION_H__
#define __SSMSTATEAWARESESSION_H__

#include <e32base.h>
#include <domaindefs.h>

#include <ssm/ssmstate.h>

class CSsmStateMonitor;

/**
Read-only client interface to receive notifications when the
System State changes.

CSsmStateAwareSession wraps this class in an active object.

@see CSsmStateAwareSession 
@publishedPartner
@released
*/
NONSHARABLE_CLASS(RSsmStateAwareSession)
	{
public:
	IMPORT_C RSsmStateAwareSession();
	IMPORT_C TInt Connect(TDmDomainId aId);
	IMPORT_C void Close();
	IMPORT_C TSsmState State() const;
	IMPORT_C void RequestStateNotification(TRequestStatus& aStatus);
	IMPORT_C void RequestStateNotificationCancel();
	IMPORT_C void AcknowledgeStateNotification(TInt aError);
	IMPORT_C void AcknowledgeAndRequestStateNotification(TInt aError, TRequestStatus& aStatus);
private:
	RSsmStateAwareSession(const RSsmStateAwareSession& aState);
	
private:
	//The main reason for this class is to hide implementation details that might change
	class RPrivateImpl;
	RPrivateImpl* iPimpl;
	};

/**
Interface to implement for clients that want to use the @c CSsmStateAwareSession 
utility class.

@publishedPartner
@released
*/
class MStateChangeNotificationSubscriber
    {
public:
	/** 
	@c StateChanged is called when the System State have changed
	@param aSsmState contains the new System State
	@see TSsmMainSystemStates
	*/
	virtual void StateChanged(TSsmState aSsmState) = 0;
	};	

/**
Utility class for monitoring the System State. Maintains a list of 
subscribers that share a single active object. When the System State
changes the subscribers will be notified in the order
they were added to the list of subscribers.

@see TSsmMainSystemStates
@see RSsmStateAwareSession
@publishedPartner
@released
*/
NONSHARABLE_CLASS(CSsmStateAwareSession) : public CBase
	{
public:
	IMPORT_C static CSsmStateAwareSession* NewL(TDmDomainId aId);
	IMPORT_C static CSsmStateAwareSession* NewLC(TDmDomainId aId);
	IMPORT_C ~CSsmStateAwareSession();
	IMPORT_C TSsmState State() const;
	IMPORT_C void AddSubscriberL(MStateChangeNotificationSubscriber& aSubscriber);	
	IMPORT_C void RemoveSubscriber(const MStateChangeNotificationSubscriber& aSubscriber);
	// for CSsmStateMonitor
	void NotifySubscribers(TSsmState aNewState);
	
private:
	CSsmStateAwareSession();
	void ConstructL(TDmDomainId aId);
	
private:
	RPointerArray<MStateChangeNotificationSubscriber> iSubscribers; // Elements of the array are not owned.
	CSsmStateMonitor* iMonitor;
	TInt iSpare[4];
	};
	
#endif
