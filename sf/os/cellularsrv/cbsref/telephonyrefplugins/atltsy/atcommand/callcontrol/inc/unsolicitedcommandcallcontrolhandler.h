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
// CUnsolicitedCommandCallControlHandler. The handler for handling the unsolicited command related to call control

#ifndef UNSOLICITEDCOMMANDCALLCONTROLHANDLER_H_
#define UNSOLICITEDCOMMANDCALLCONTROLHANDLER_H_

//system include
#include <e32base.h>
//user include
#include "atunsolicitedobserver.h"
#include "callinformationquerycompletenotify.h"

//class forward
class CGlobalPhonemanager;
class CCtsyDispatcherCallback;
class CATClcc;

class CUnsolicitedCommandCallControlHandler : public CBase, 
											  public MLtsyUnsolicitedCommandObserver,
											  public MCallInformationQueryCompleteNotify											  
	{
public:
	/**
	 * Factory function
	 * 
	 */
	static CUnsolicitedCommandCallControlHandler* NewL(CGlobalPhonemanager& aGloblePhone,
			                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Factory function
	 * 
	 */
	static CUnsolicitedCommandCallControlHandler* NewLC(CGlobalPhonemanager& aGloblePhone,
			                                            CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 * 
	 */
	virtual ~CUnsolicitedCommandCallControlHandler();

	/**
	 * From MLtsyUnsolicitedCommandObserver
	 * 
	 */
	virtual void UnsolicitedCommandArriving(TInt aEvent, const TDesC8& aWindPkg);
	
	/**
	 * From MCallInformationQueryCompleteNotify
	 * 
	 */
	virtual void QueryCallInformationComplete(TInt aError);
	
protected:
	/**
	 * Constructor
	 * 
	 */
	CUnsolicitedCommandCallControlHandler(CGlobalPhonemanager& aGloblePhone,
			                              CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd constructor
	 * 
	 */
	void ConstructL();
	
private:
	/**
	 * Handler sim remove event
	 * 
	 */
	void HandleSimRemoveEvent();
	
	/**
	 * Handle product init event
	 * 
	 */
	void HandleProductInitEvent();
	
	/**
	 * Handle network lost event
	 * 
	 */
	void HandleNetworkLostEvent();
	
	/**
	 * Handle call created event
	 * 
	 */
	void HandleCallCreatedEvent(const TDesC8& aWindPkg);
	
	/**
	 * Handle call release event
	 * 
	 */
	void HandleCallReleasedEvent(const TDesC8& aWindPkg);

private:
	/**
	 * Phone manager
	 * 
	 */
	CGlobalPhonemanager& iGlobalPhone;
	
	/**
	 * Call back reference
	 * 
	 */
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;
	
	/**
	 * The at command for querying the call infomation
	 * 
	 */
	CATClcc* iAtClcc;
	};

#endif /*UNSOLICITEDCOMMANDCALLCONTROLHANDLER_H_*/
