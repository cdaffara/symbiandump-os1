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
// @file notifybatteryandsignal.h
// 
// 

#ifndef __NOTIFYBATTERYANDSIGNAL_H__
#define __NOTIFYBATTERYANDSIGNAL_H__

//user include
#include <callbacktimer.h>
#include "atcommandcontrolbase.h"
#include "globalphonemanager.h"

// forward class
class CATGetBattery;
class CATGetSignal;

/**
 * CNotifyBatteryAndSignal inherits from the CATCommandControlBase class. 
 *
 */
class CNotifyBatteryAndSignal : public CATCommandControlBase,
                                public MTimerObserver
{
public:
    /**
     * Two-phased constructor.
     * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
     */
	static CNotifyBatteryAndSignal* NewL(CGlobalPhonemanager& aGloblePhone, 
		                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	/**
	 * Destructor
	 *
	 */
	~CNotifyBatteryAndSignal();

	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	void StartRequest();	
	void StartTimer(TInt aSecond);
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CNotifyBatteryAndSignal(CGlobalPhonemanager& aGloblePhone, 
		                    CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	//From MTimerObserver
    virtual void TimerRun(TInt aError);
    
private:
	RMobilePhone::TMobilePhoneBatteryInfoV1 iBatteryInfo; 
	TTsySignalInfo iSignalInfo; 
	CATGetBattery* iATGetBattery;
	CATGetSignal* iATGetSignal;
	/**
	 *
	 */
	CCallbackTimer* iCallbackTimer;	
	enum 
		{
		EATNone,
		EATGetBattery,
		EATGetSignal
		} iATCommandName;

};
#endif //__NOTIFYBATTERYANDSIGNAL_H__
