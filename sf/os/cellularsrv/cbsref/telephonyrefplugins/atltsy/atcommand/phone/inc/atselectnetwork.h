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
// @file atselectnetwork.h
//This file defines the AT command for Selecting Network

#ifndef __CATSELECTNETWORK_H__
#define __CATSELECTNETWORK_H__

//user include
#include "atcommandbase.h"

/**
 * class CATSelectNetwork 
 *
 * CATSelectNetwork inherits from the CAtCommandBase class. 
 * This class transmits the AT+COPS=1,2,x Command in order to  
 * Select Network
 */
class CATSelectNetwork : public CAtCommandBase
{
public:
	static CATSelectNetwork* NewL(CGlobalPhonemanager& aGloblePhone, 
		                          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATSelectNetwork();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * Cancel the AT Command execution
	 */
	virtual void CancelCommand();
	
	virtual void StartRequest();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * @param aResponseBuf Line buf reading from baseband 
	 */
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	/**
	 * Will be called by AT Manager whenever a event was triggered
	 * 
	 * @param aEventSource
	 * @param aStatus
	 */
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
	
	void SetNetworkSelection(const RMobilePhone::TMobilePhoneNetworkManualSelection& aNetworkManualSelection);
public:
	TBool iCommandRunning;
	TBool iCommandCancel;
	TBool iCommandSetBack;
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATSelectNetwork(CGlobalPhonemanager& aGloblePhone, 
		             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	/**
	 * 
	 */
	void ExecuteSetBackCommand();

private:
	TInt iError;
	RMobilePhone::TMobilePhoneNetworkManualSelection iNetworkManualSelection;
};
#endif
