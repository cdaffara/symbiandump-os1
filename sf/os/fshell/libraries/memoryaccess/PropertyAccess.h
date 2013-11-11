// PropertyAccess.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef PROPERTYACCESS_H
#define PROPERTYACCESS_H

#include <kernel.h>
#include <kern_priv.h>

class TNotifyRequest;

NONSHARABLE_CLASS(DPropertyAccess) : public DBase
	{
public:
	DPropertyAccess(DThread* aThread);
	~DPropertyAccess();
	TInt HandleProperty(TInt aFunction, TAny* aParams, TAny* aVal);
	TInt DoHandleProperty(DThread* aClient, TInt aFunction, TAny* aParams, TAny* aVal);

private:
	TInt GetAllPropertiesFrom(DThread* aClient, RPropertyRef& property, TAny* aVal);
	void CancelNotifys();
	static void PropertyChanged(TAny* aPtr, TInt aReason);
	void DoPropertyChanged(TNotifyRequest& aRequest, TInt aReason);
	TInt SetupNotifyAll(DThread* aClient, TAny* aParams, TAny* aVal);
	TInt SetupNotifyAll(TBool aBtrace, const TDesC8& aUserRequestsBuf);

private:
	DThread* iClient;
	SDblQue iRequests;
	TRequestStatus* iNotifyClientStatus;
	TAny* iClientNotifyPtr;
	TInt iMissedChanges;
	};

class TNotifyRequest
	{
public:
	TNotifyRequest(TPropertyCompleteFn aCompleteFn, TUid aCat, TInt aKey, DPropertyAccess* aPropertyAccess, TBool aBtrace)
		: iRequest(aCompleteFn, this), iPropertyAccess(aPropertyAccess), iBtrace(aBtrace), iCat(aCat), iKey(aKey)
		{}
public:
	RPropertyRef iProperty;
	TPropertySubsRequest iRequest;
	DPropertyAccess* iPropertyAccess;
	SDblQueLink iLink;
	TBool iBtrace; // If set, don't use iNotifyClientStatus/iClientNotifyPtr/iMissedChanges, just use BTrace
	// Lame, no way of getting these from the RPropertyRef
	TUid iCat;
	TInt iKey;
	};

#endif
