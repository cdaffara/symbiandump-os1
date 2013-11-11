// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CUnsolicitedEventMonitor which mornitor the arriving unsocilited event and then notify this to all observers.
// 

#ifndef UNSOLICITEDEVENTMONITOR_H_
#define UNSOLICITEDEVENTMONITOR_H_

//system include

//user include
#include "atcommandcontrolbase.h"
#include "globalphonemanager.h"
#include "atunsolicitedobserver.h"
#include <ctsy/ltsy/cctsydispatchercallback.h>

/**
 * This is a watcher class which mornitor the unsolicited events sent by baseband.
 * Notify all the interesting observers when an unsolicited event has been received
 */
class CUnsolicitedEventMonitor : public CATCommandControlBase
	{
public:
	/**
	 * Factory function
	 */
	static CUnsolicitedEventMonitor* NewL(CGlobalPhonemanager& aPhoneglobals,
						                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Factory function
	 */
	static CUnsolicitedEventMonitor* NewLC(CGlobalPhonemanager& aPhoneglobals,
			                               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * Destructor
	 */
	virtual ~CUnsolicitedEventMonitor();
	
	/**
	 * Add the observer which is interested in geting the notification of incoming unsolicited event  
	 */
	void AddUnsolicitedObserver(const MLtsyUnsolicitedCommandObserver* aObserver);
	
	/**
	 * Remove the unsolicited observer
	 */
	void RemoveUnsolicitedObserver(const MLtsyUnsolicitedCommandObserver* aObserver);
	
	/**
	 * Notify all the observers that the unsoclicited event is received
	 */
	void NotifyEveryObserver();
	
	/**
	 * Start to mornitor
	 */
	void StartUnsolicitedObserver();
	
	/**
	 * Stop to mornitor
	 */
	void StopUnsolicitedObserver();
	
protected:
	/**
	 * The constructor
	 */
	CUnsolicitedEventMonitor(CGlobalPhonemanager& aPhoneglobals,
			                 CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * 2nd constructor
	 */
	void ConstructL();
	
public:
	/**
	 * Inherit from CAtCommandContolBase
	 */
	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	
private:
	/**
	 * Own
	 * Store a list of observers
	 */
	RPointerArray <MLtsyUnsolicitedCommandObserver> iArray;
	};

#endif /*UNSOLICITEDEVENTMONITOR_H_*/
