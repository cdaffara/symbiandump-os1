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
// @file atgetnetworkregistrationstatus.h
// This contains CATNetworkRegistration which is defines the AT command for Getting
// network registration status
// 

#ifndef __CNETWORKREGISTRATION_H__
#define __CNETWORKREGISTRATION_H__

//user include
#include "atcommandbase.h"

// forward class
class CATNetworkRegStatusChange;
/**
 * class CATNetworkRegistration 
 *
 * CATNetworkRegistration inherits from the CAtCommandBase class. 
 * This class transmits the AT+CREG=? Command in order to get 
 * network registration status
 */
class CATNetworkRegistration : public CAtCommandBase
{
public:
	static CATNetworkRegistration* NewL(CGlobalPhonemanager& aGloblePhone, 
		                                CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATNetworkRegistration();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Will be called by AT Manager whenever a event was triggered
	 * 
	 * @param aEventSource
	 * @param aStatus
	 */
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * @param aResponseBuf Line buf reading from baseband 
	 */
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	/**
	 * Get error value executed AT Command
	 */
	TInt GetErrorValue();
	
	virtual void StartRequest();
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATNetworkRegistration(CGlobalPhonemanager& aGloblePhone, 
		                   CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:	
	/*
	 * The return network registration status infomation  of AT command executed 
	 */
	RMobilePhone::TMobilePhoneRegistrationStatus iRegistrationStatus;
	/*
	 * The return error value of AT command executed 
	 */
	TInt iError;
};
#endif
