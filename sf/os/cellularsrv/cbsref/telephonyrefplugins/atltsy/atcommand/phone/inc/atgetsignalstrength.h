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
// @file atgetsignalstrength.h
// 

/**
 * @file
 *
 * AT command definition
 * This file defines the AT command for Getting Signal level
 * This queries the ME (Mobile Equipment) Signal level,
 * using a standard (ETSI GSM 07.07 April 1997) AT command : AT+CSQ?
 * 
 */

#ifndef __CSIGNAL_H__
#define __CSIGNAL_H__

//system include
#include <etelmm.h>
//user include
#include "atcommandbase.h"

// forward class
class MCallBackPhoneHander;
/**
 * \class CATGetSignal 
 * \brief get Modem status.
 *
 * CATGetSignal inherits from the CAtCommandBase class. 
 * This class transmits the AT Command in order to get  Getting Signal level
 * in the phone, using a standard AT command : AT+CSQ?
 */	
class CATGetSignal : public CAtCommandBase
{
public:
	static CATGetSignal* NewL(CGlobalPhonemanager& aGloblePhone, 
		                      CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	/**
	 * Destructor
	 *
	 */
	~CATGetSignal();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
    /**
     * Get network signal information of phone
     */ 
	TTsySignalInfo getSignalInfo();
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
	 * The return error value of AT command executed 
	 */
	TInt GetErrorValue();
	/**
	 * Start this request
	 */
	virtual void StartRequest();
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATGetSignal(CGlobalPhonemanager& aGloblePhone, 
		         CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:
	TTsySignalInfo iSignalInfo; 
	TInt iError;
};
#endif
