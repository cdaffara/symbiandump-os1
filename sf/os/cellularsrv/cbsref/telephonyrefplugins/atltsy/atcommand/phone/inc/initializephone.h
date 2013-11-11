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
// @file initializephone.h
// 

#ifndef __INITIALIZEPHONE_H__
#define __INITIALIZEPHONE_H__

//user include
#include "atcommandcontrolbase.h"
#include "globalphonemanager.h"
#include "atipcrequestcompleteobserver.h"

// forward class
class CATGetModemStatus;
class CATNetworkRegistration;
class CATSIMCard;
class CATNetworkInfo;
class CATGetSignal;
class CATGetBattery;
class CATHelperCommand;
class CATNetworkRegStatusChange;
class CNotifyBatteryAndSignal;
class CGetCurrentNetworkInfo;

/**
 * CInitializePhone inherits from the CATCommandControlBase class. 
 *
 */
class CInitializePhone : public CATCommandControlBase, public MIpcRequestCompleteObserver
{
public:
    /**
     * Two-phased constructor.
     * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 * @param aNtwkRegStatusChange
	 * @param aNotifyBatteryAndSignal
     */
	static CInitializePhone* NewL(CGlobalPhonemanager& aGloblePhone, 
		                           CCtsyDispatcherCallback& aCtsyDispatcherCallback,
		                           CATNetworkRegStatusChange& aNtwkRegStatusChange,
		                           CNotifyBatteryAndSignal& aNotifyBatteryAndSignal);

	/**
	 * Destructor
	 *
	 */
	~CInitializePhone();

	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	
	/**
	 * Start this request 
	 */
	void StartRequest();
    
	/**
     * Execute next request stored in request list
     */
	void ExecutePendingRequest();
	
	/**
	 * Get the notification when one of the IPC request was complete
	 */
	virtual void NotifyIpcRequestComplete(TInt aIpcId);
	
	/**
	 * Append the IPC request to pending request list
	 */
	void AppendIpcRequestToPendingListL(TInt aIpc);
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 * @param aNtwkRegStatusChange
	 * @param aNotifyBatteryAndSignal
	 */
	CInitializePhone(CGlobalPhonemanager& aGloblePhone, 
		              CCtsyDispatcherCallback& aCtsyDispatcherCallback,
		              CATNetworkRegStatusChange& aNtwkRegStatusChange,
		              CNotifyBatteryAndSignal& aNotifyBatteryAndSignal);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:
    /**
     * Execute next AT commane 
     * 
     * @param aATString The context of excuting aommand
     * @param aATType Identify the Type of AT Command
     */
	void ExecuteNextATHelpCommand(const TDesC8& aATString,TLtsyATCommandType aATType);
	/**
	 * Start an AT Command
	 * @param  aAtCommand the AT Command class 
	 */
	void StartATCommand(CAtCommandBase* aAtCommand);
	/**
	 * End an AT Command
	 * @param  aAtCommand  the AT Command class 
	 */
	void CompleteATCommand(CAtCommandBase* aAtCommand);
	/**
	 * Get modem status command finished
	 */
	void ModemStatusComplete(TInt aError);
	/**
	 * Set oper format command finished
	 */
	void SetOperFormatComplete(const TInt aError);
	/**
	 * Get network infomation command finished
	 */
	void NetworkInfoComplete();
	/**
	 * Get phone's signal infomation command finished
	 */
	void GetSignalComplete(TInt aError);
	/**
	 * Get phone's battery infomation command finished
	 */
	void GetBatteryComplete(TInt aError);
	
private:
    /**
     * AT type is used to identify AT command
     */
    TInt iATType;
    
    /**
     * Check if the current executed pending request is still running
     */
    TBool iIsPendingRequestStillRunning;
    
    /**
     * Get the current modem status
     * Own
     */
	CATGetModemStatus* iATGetModemStatus;
	
	/**
	 * Get the current network registration status
	 * Own
	 */
	CATNetworkRegistration* iNetworkRegStatus;
	
	/**
	 * Get the current SIM card status
	 * Own
	 */
	CATSIMCard* iATSIMCard;
	
	/**
	 * Get the current signal level
	 * Own
	 */
	CATGetSignal* iATGetSignal;
	
	/**
	 * Get the current battery level
	 * Own
	 */
	CATGetBattery* iATGetBattery;
	
	/**
	 * Helper command for get a set of phone info
	 * Own
	 */
	CATHelperCommand* iATHelperCommand;
	
	/**
	 * Get the current netwok info
	 * Own
	 */
	CGetCurrentNetworkInfo* iATGetCurNetworkInfo;
	
	/**
	 * Notify the regisration status change
	 * Not Own
	 */
	CATNetworkRegStatusChange& iNtwkRegStatusChange;
	
	/**
	 * Not Own
	 */
	CNotifyBatteryAndSignal& iNotifyBatteryAndSignal;
	
	/**
	 * Network infomation
	 */
	RMobilePhone::TMobilePhoneNetworkInfoV5 iNetworkInfo; 
	
	/**
	 * Battery infomation
	 */
	RMobilePhone::TMobilePhoneBatteryInfoV1 iBatteryInfo; 
	
	/**
	 * Signal infomation
	 */
	TTsySignalInfo iSignalInfo;
	
	/**
     * This store the IPC request which should be handled specially as those IPC requests would be coming together.
     * So we should store one or another before one of those special IPC requests was complete and then execute next request 
     * retrived from this store.
     */
	RArray<TInt> iPendingRequestList;
};
#endif //__INITIALIZEPHONE_H__
