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
// @file atgetbatteryinfo.h
// This contains CATGetBattery which is defines the AT command for Getting Battery status
// 

#ifndef __CBATTERY_H__
#define __CBATTERY_H__

//system include
#include <etelmm.h>
#include <cmmutility.h>
//user include
#include "atcommandbase.h"


/**
 * class CATGetBattery 
 *
 * CATGetBattery inherits from the CAtCommandBase class. 
 * This class transmits the AT+COPS=? Command in order to get 
 * Battery status in this phone
 */
class CATGetBattery : public CAtCommandBase
{
public:
	static CATGetBattery* NewL(CGlobalPhonemanager& aGloblePhone, 
		                       CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGetBattery();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Get the battery infomation returned by executing AT Command
	 */
	RMobilePhone::TMobilePhoneBatteryInfoV1 GetBatteryinfo();
	/**
	 * Get RF state information
	 */
	TRfStateInfo GetRfStateInfo();
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
     * Start the request 
     */
	virtual void StartRequest();
protected:	
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATGetBattery(CGlobalPhonemanager& aGloblePhone, 
		          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/*
	 * The return battery infomation  of AT command executed 
	 */
	RMobilePhone::TMobilePhoneBatteryInfoV1 iBatteryInfo; 
	/*
	 * The return error value of AT command executed 
	 */
	TInt iError;
	/**
	 * The RF state information
	 */
	TRfStateInfo iRfStateInfo;
	
};
#endif
