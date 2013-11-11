// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Notification handling for client requests for notificaiton on database changes
// 
//

/**
 @file 
 @internalTechnology
*/

 
#if (!defined COMMSDATNOTIFIER_H)
#define       COMMSDATNOTIFIER_H 

#include <e32property.h>

namespace CommsDatInternal
{

using namespace CommsDat;

NONSHARABLE_CLASS(TCommsDatPubSubNotification)
//@internalComponent
//@released
	{
public:
	TCommsDatPubSubNotification();
	void Set(TUid aUid, TInt aVal);
public:
	TUid iUid;
	TInt iValue;
	};

_LIT(KCommsdatNotifierExecutable,"ShimNotifierSvr");
/** Commsdat Notifier 
* 
* @internalComponent */
class RCommsdatNotifier : public RSessionBase   
	{
public:
	RCommsdatNotifier();
	TVersion Version() const;
	
	TInt Connect();
	void RequestNotification(TRequestStatus& aStatus);
	void CancelNotification(TRequestStatus& aStatus);
	void CancelAllNotifications();
	void Close();
private:	
	TInt StartNotifierProcess();   	
	};

	
NONSHARABLE_CLASS(CCDNotifier) : public CBase
/*
This class records notification requests from clients and completes them when
own TRequestStatus is completed by storage server.

CommsDat doesn't need to act on the info itself. 
It is just acting as a broker for the client to completely hide the db server. 

FUTURE - At the moment CommsDat makes a simple general request for notification on the whole repository.
Because this is all that Commdb does.  It would be good to change this to register for individual areas of change.

This class also publishes notification of changes made to the database by this client process
where pub/sub notification is required
*/
{
public :

    CCDNotifier(CMDBSessionImpl& aOwner);
    
    ~CCDNotifier();
   
    static CCDNotifier* NewL(CMDBSessionImpl& aOwner);

    void ConstructL();
    
    // Functions to inform clients of commsdat events.

    TInt RegisterNotifyL(TRequestStatus& aStatus);
    
    TInt CancelNotify(TRequestStatus& aStatus);

    TInt CancelAllNotifications();

    TInt NotifyClients(TInt aEventType);
    
    void SuppressRollBackEvents()   
        {
        iSuppressRollBackEvents = ETrue;
        }
    
    // Functions for publish and subscribe notification

   	void MaybeNotifyChange(TMDBElementId aElementId);        
	void MaybeNotifyChangeForDelete(TMDBElementId aElementId, TBool aModemBearerDeleted);
	
    void NotifyAllChanges();

    void ClearPubSubNotifications();
    
    TUint32 GetNewNumber(TUid aUid);

  	void ConvertToUid(TMDBElementId aId, TCommsDatPubSubNotification& aNotification);

private :

    TBool                                   iSuppressRollBackEvents;
    RArray<TCommsDatPubSubNotification>     iNotifications;
    
    RCommsdatNotifier iNotifier;

    CMDBSessionImpl& iOwner;
};

//

} // namespace CommsDatInternal

#endif //COMMSDATNOTIFIER_H
