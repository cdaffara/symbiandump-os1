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
// @file atgetnetworkinfo.h
 /**
 * @file
 * Network Header file.
 * AT command definition
 * This file defines the AT command for Getting GSM Networ info
 * Describes the GSM Network & Operator information.
 * Using a standard (ETSI GSM 07.07 April 1997) AT command : AT
 * 
 */
#ifndef __CNETWORK_H__
#define __CNETWORK_H__

//system include
#include <etelmm.h>
//user include
#include "atcommandbase.h"

/**
 * Determines GSM Network and Operator Information.
 *
 * CATNetworkInfo inherits from the CAtCommandBase class. 
 * This class transmits the Set Network Operator Command (AT+COPS=x,x) in numeric, short 
 * alphanumeric and long alphanumeric formats. It also determines the current operator 
 * using the operator query command (AT+COPS?).
 */
class CATNetworkInfo : public CAtCommandBase
{
public:
	static CATNetworkInfo* NewL(CGlobalPhonemanager& aGloblePhone, 
		                        CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATNetworkInfo();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Get the return value executed AT Command
	 */
	RMobilePhone::TMobilePhoneNetworkInfoV5 GetNetWorkInfo();
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
	
	void SetToGetHomeNwk();
	/**
	 * Start this request
	 */
	virtual void StartRequest();
	
	enum TPortAccess
		{
		EPortAccessAllowed,
		EPortAccessDenied
		};
protected:	
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATNetworkInfo(CGlobalPhonemanager& aGloblePhone, 
		           CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/**
	 * The return phone infomation  of AT command executed 
	 */
	RMobilePhone::TMobilePhoneNetworkInfoV5 iNetworkInfo; 
	/**
	 * The return error value of AT command executed 
	 */
	TInt iError;
	
	TInt iNameFormat;
	
	TBool iGetHomeNwk;
	
};

#endif
