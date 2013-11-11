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
// @file atgetsimstatusready.h
// 
/**
 * @file
 *
 * AT command definition
 * This file defines the AT command for Getting SIM card status
 * 
 */

#ifndef __CATSIMCARD_H__
#define __CATSIMCARD_H__

//user include
#include "atcommandbase.h"

// forward class
class MCallBackPhoneHander;
/**
 * \class CATSIMCard 
 *
 * CATSIMCard inherits from the CAtCommandBase class. 
 * This class transmits the AT Command in order to get SIM card status 
 * in the phone, using a standard AT command : AT+CCID=?
 */
class CATSIMCard : public CAtCommandBase
{
public:
	static CATSIMCard* NewL(CGlobalPhonemanager& aGloblePhone, 
		                    CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATSIMCard();
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
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATSIMCard(CGlobalPhonemanager& aGloblePhone, 
		       CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	TInt iError;
};
#endif
