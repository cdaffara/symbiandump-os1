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

#include "ssmpropertyobserver.h"

#include "ssmdebug.h"

/**
 * Creates a new CSsmProperty Observer.
 * 
 * @param aCategory The category of the property to monitor.
 * @param aKey The key of the property to monitor.
 *  
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmPropertyObserver::CSsmPropertyObserver(TUid aCategory, TUint aKey)
: 	CActive(EPriorityNormal),
	iCategory(aCategory),
	iKey(aKey)
	{
	CActiveScheduler::Add(this);
	} //lint !e1746 Suppress parameter 'aCategory' could be made const reference

/**
 * Releases any resources allocated to this object.
 *  
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmPropertyObserver::~CSsmPropertyObserver()
	{
	Deque();
	if(iProperty.Handle() != 0)
		{
		iProperty.Cancel();
		}
	iProperty.Close();
	}

/**
 * Initialises this object but does not start responding to changes to the monitored property.
 *  
 * @publishedPartner
 * @released
 */
EXPORT_C void CSsmPropertyObserver::InitializeL()
	{
	TInt err = iProperty.Attach(iCategory, iKey);
	if(KErrNone != err)
		{
		DEBUGPRINT2(_L("CSsmPropertyObserver failed to attach to property with error: %d"), err);
		User::Leave(err);
		}
	}

/**
 * Starts this observer responding to the monitored property.
 *  
 * @publishedPartner
 * @released
 */
EXPORT_C void CSsmPropertyObserver::StartL()
	{
	// To start we just call doRunL()
	doRunL();
	}

/**
 * Releases this object freeing any resources allocated to it.
 * 
 * Using this object after calling release will have undefined behaviour.
 *  
 * @publishedPartner
 * @released
 */
EXPORT_C void CSsmPropertyObserver::Release()
	{
	delete this;
	}

/**
 * CActive::RunL implementation to call the observer function and to re-request monitoring.
 *  
 * @publishedPartner
 * @released
 */
EXPORT_C void CSsmPropertyObserver::RunL()
	{
	if(KErrNone != iStatus.Int())
		{
		DEBUGPRINT2A("CSsmPropertyObserver RunL received error status: %d", iStatus.Int());
		User::Leave(iStatus.Int());
		}
	doRunL();	
	}

/**
 * Internal RunL helper method
 * 
 * @internalComponent
 */
void CSsmPropertyObserver::doRunL()
	{
	TInt value;
	// subscribe before we get the value, deciding later if we should cancel
	iProperty.Subscribe(iStatus);
	
	TInt err = iProperty.Get(value);
	if(KErrNone != err)
		{
		DEBUGPRINT2A("CSsmPropertyObserver failed to get property with error: %d", err);
		// Cancel the subscription before leaving 
		iProperty.Cancel();
		User::WaitForRequest(iStatus);
		
		User::Leave(err);
		}
	TBool resubscribe = PropertyChangedL(value);
	if(resubscribe)
		{
		// Just set active as we resubscribed before
		SetActive();
		}
	else
		{
		// Cancel the previous subscription
		iProperty.Cancel();
		User::WaitForRequest(iStatus);
		}
	}

/**
 * Cancels monitoring for this property 
 *  
 * @publishedPartner
 * @released
 */
EXPORT_C void CSsmPropertyObserver::DoCancel()
	{
	iProperty.Cancel();
	}

/**
 * CActive::RunError implementation
 *  
 * @publishedPartner
 * @released
 */
EXPORT_C TInt CSsmPropertyObserver::RunError(TInt aError)
	{
	DEBUGPRINT4A("Property observer for UID: %x Key: %d RunError received error: %d", iCategory.iUid, iKey, aError);
	return aError;
	}
