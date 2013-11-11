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

#ifndef __SSMSYSTEMWIDEPROPERTY_H__
#define __SSMSYSTEMWIDEPROPERTY_H__

#include <e32base.h>
#include <e32property.h>

#include <ssm/ssmswp.h>

class CSsmPropertyMonitor;

/**
Read-only client interface to receive notifications when a
System Wide Property's value is changed.

The implementation is similar to RProperty so the general pitfalls
of RProperties applies. I.e. if the value changes several times before 
being read, only the last set value will be available. And to 
avoid missing a second notification, subscribtion must be reissued 
the first thing when you get a change notification, not after the 
change have been handled.

CSsmSystemWideProperty wraps this class in an active object.

@see CSsmSystemWideProperty
@publishedPartner
@released
*/
NONSHARABLE_CLASS(RSsmSystemWideProperty)
	{
public:
	IMPORT_C TInt Connect(TUint aSwpKey);
	TInt Connect(TUint aSwpKey, TUid aProcessSid);
	IMPORT_C void Close();
	IMPORT_C TInt GetValue(TInt& aValue) const;
	IMPORT_C void Subscribe(TRequestStatus& aStatus);
	IMPORT_C void SubscribeCancel();	
	
private:
	TUint iKey;
	RProperty iProperty;
	TInt iSpare[4];
	};

/**
Interface to implement for clients that want to use the @c CSsmSystemWideProperty 
utility class.

@publishedPartner
@released
*/
class MSwpChangeNotificationSubscriber
    {
public:
	/** 
	@c SwpChanged is called when the value of the subscribed Swp have changed
	@param aSwp contains the new value for the monitored Swp
	*/
	virtual void SwpChanged(TSsmSwp aSwp) = 0;
	};	

/**
Utility class for monitoring a system Wide Property. Maintains a list of 
subscribers that share a single active object. When the monitored 
Sytem Wide Property  changes the subscribers will be notified in the order
they were added to the list of subscribers.

@see RSsmSystemWideProperty
@publishedPartner
@released
*/
NONSHARABLE_CLASS(CSsmSystemWideProperty) : public CBase
	{
public:
	IMPORT_C static CSsmSystemWideProperty* NewL(TUint aSwpKey);
	IMPORT_C static CSsmSystemWideProperty* NewLC(TUint aSwpKey);
	static CSsmSystemWideProperty* NewLC(TUint aSwpKey, TUid aProcessSid);
	IMPORT_C ~CSsmSystemWideProperty();
	IMPORT_C TInt GetValue(TSsmSwp& aSwp) const;
	IMPORT_C void AddSubscriberL(MSwpChangeNotificationSubscriber& aSubscriber);	
	IMPORT_C void RemoveSubscriber(const MSwpChangeNotificationSubscriber& aSubscriber);
	//for CSsmPropertyMonitor
	void NotifySubscribers(TSsmSwp aSwp);
	
private:
	CSsmSystemWideProperty();
	void ConstructL(TUint aSwpKey, TUid aProcessSid);
	
private:
	RPointerArray<MSwpChangeNotificationSubscriber> iSubscribers; // Elements of the array are not owned.
	CSsmPropertyMonitor* iMonitor;
	TInt iSpare[4];
	};
	
	
#endif
