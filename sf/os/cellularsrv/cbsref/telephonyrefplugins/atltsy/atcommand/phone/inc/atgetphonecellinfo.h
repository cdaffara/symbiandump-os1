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
// @file atgetphonecellinfo.h
// This contains CATGetPhoneCellInfo which is defines the AT command for Getting Battery status
// 

#ifndef __ATGETPHONECELLINFO_H__
#define __ATGETPHONECELLINFO_H__

//system include
#include <etelmm.h>

//user include
#include "atcommandbase.h"

// forward class

/**
 * class CATGetPhoneCellInfo 
 * brief Determines the Operators present in the GSM Network.
 *
 * CATGetPhoneCellInfo inherits from the CAtCommandBase class. 
 * This class transmits the AT+COPS=? Command in order to get 
 * Battery status in this phone
 */
class CATGetPhoneCellInfo : public CAtCommandBase
{
public:
	static CATGetPhoneCellInfo* NewL(CGlobalPhonemanager& aGloblePhone, 
		                             CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	/**
	 * Destructor
	 *
	 */
	~CATGetPhoneCellInfo();

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
	CATGetPhoneCellInfo(CGlobalPhonemanager& aGloblePhone, 
		                CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:
	/*
	 * The return battery infomation  of AT command executed 
	 */
	RMobilePhone::TMobilePhoneCellInfoV9 iCellInfo;
	
	/**
	 * Store the error code
	 */
	TInt iError;
};
#endif //__ATGETPHONECELLINFO_H__
