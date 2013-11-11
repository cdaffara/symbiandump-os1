// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <e32std.h>

class TBTBasebandEventNotification;
class CPhysicalLink;

template<class T>
class TPolymorphicQLink : public TDblQueLink
	{
public:
	TPolymorphicQLink(T* aObserver):iObserver(aObserver){}
	operator T* () {return iObserver;}
	T* Item() {return iObserver;}
private:
	T* iObserver;
	};

class MPhysicalLinkObserver;
typedef TPolymorphicQLink<MPhysicalLinkObserver> TPhysicalLinkObserverQLink;

class TPhysicalLinkSubscriptionInfo
	{
public:
	TPhysicalLinkSubscriptionInfo(MPhysicalLinkObserver& aMPhysicalLinkObserver, TBTDevAddr aAddr):
					iMPhysicalLinkObserver(aMPhysicalLinkObserver), iAddr(aAddr){};
	MPhysicalLinkObserver& 	iMPhysicalLinkObserver;
	TBTDevAddr 				iAddr;
	};

class MPhysicalLinkObserver
/**
	Interface for simple notifications to the presence of physical channel
	or for especially interested parties a change in security.
**/
	{
public:
	virtual void PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& aPhysicalLink) = 0;
	virtual TPhysicalLinkObserverQLink& ObserverQLink() = 0;
	};


class MBluetoothPrefetchNotifier;
typedef TPolymorphicQLink<MBluetoothPrefetchNotifier> TPrefetchNotifierQLink;

/**
Interface for notification of when a prefetched PIN code entry has completed.
*/
NONSHARABLE_CLASS(MBluetoothPrefetchNotifier)
	{
public:
	virtual TPrefetchNotifierQLink& MbpnQueLink() = 0;
	virtual void MbpnPrefetchComplete(TInt aError) = 0;
	};

#endif
