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
// @file atdetectnetwork.h
// This contains CATDetectNetwork which is describes the GSM Network & Operator information.
// 
#ifndef __ATDETECTNETWORK_H__
#define __ATDETECTNETWORK_H__

#include <mmlist.h>
//User include
#include "atcommandbase.h"


/**
 * CATDetectNetwork inherits from the CAtCommandBase class. 
 * brief Determines the Operators present in the GSM Network.
 * This class transmits the AT+COPS=? Command in order to determine all the Operators 
 * present in the GSM Network.
 *
 */
class CATDetectNetwork : public CAtCommandBase
{
public:
	static CATDetectNetwork* NewL(CGlobalPhonemanager& aGloblePhone, 
		                          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATDetectNetwork();

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
    /**
     * Start the request 
     */
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
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATDetectNetwork(CGlobalPhonemanager& aGloblePhone, 
		             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
public:
	/**
	 * Detect Network Command is running? or not
	 */
	TBool iCommandRunning;
	/**
	 * Detect Network Cancel Command is running? or not
	 */
	TBool iCommandCancel;
private:
	/*
	 * Returned all the Operators which present in the GSM Network.
	 */
	CMobilePhoneNetworkListV2* iList;
	/*
	 * The return value of AT command executed 
	 */
	TInt iError;
};

#endif
