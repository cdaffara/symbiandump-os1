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
// @file atgetmodemstatus.h
// This contains CATGetModemStatus which is defines the AT command for Getting modem staus
// 

/**
 * @file
 *
 * AT command definition
 * This file defines the AT command for Getting Modem status
 * This queries the ME (Mobile Equipment) Modem,
 * using a standard (ETSI GSM 07.07 April 1997) AT command : AT
 * 
 */

#ifndef __CMODEMSTATUS_H__
#define __CMODEMSTATUS_H__

//user include
#include "atcommandbase.h"


/**
 * \class CATGetModemStatus 
 * \brief get Modem status.
 *
 * CATGetModemStatus inherits from the CAtCommandBase class. 
 * This class transmits the AT Command in order to get Modem status 
 * in the phone, using a standard AT command : AT+CGSN
 */
class CATGetModemStatus : public CAtCommandBase
{
public:
	static CATGetModemStatus* NewL(CGlobalPhonemanager& aGloblePhone, 
		                           CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGetModemStatus();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
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
	/**
	 * Get error value executed AT Command
	 */
	TInt GetErrorValue();
	/**
	 * Start this request
	 */
	virtual void StartRequest();
	
	/**
	 * Start this request
	 */
	void Complete(TInt aError);
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATGetModemStatus(CGlobalPhonemanager& aGloblePhone, 
		              CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/**
	 * The return error value of AT command executed 
	 */
	TInt iError;

};
#endif
