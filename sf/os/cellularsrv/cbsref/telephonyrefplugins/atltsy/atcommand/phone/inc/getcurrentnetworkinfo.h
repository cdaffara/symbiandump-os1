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
// @file getcurrentnetworkinfo.h
// 

#ifndef __GETCURRENTNETWORKINFO_H__
#define __GETCURRENTNETWORKINFO_H__

//user include
#include "atcommandcontrolbase.h"
#include "athelpercommand.h"

// forward class
class CATNetworkInfo;
class CATHelperCommand;

/**
 * CGetCurrentNetworkInfo inherits from the CATCommandControlBase class. 
 *
 */
class CGetCurrentNetworkInfo : public CATCommandControlBase
{
public:
    /**
     * Two-phased constructor.
     * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
     */ 
	static CGetCurrentNetworkInfo* NewL(CGlobalPhonemanager& aGloblePhone, 
		                          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CGetCurrentNetworkInfo();
	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	void StartRequest();
	/**
	 * Get the return value executed AT Command
	 */
	RMobilePhone::TMobilePhoneNetworkInfoV5 GetNetWorkInfo();
	/**
	 * Get error value executed AT Command
	 */
	TInt GetErrorValue();
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CGetCurrentNetworkInfo(CGlobalPhonemanager& aGloblePhone, 
		             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:
    /**
     * Start the AT command
     */
	void StartATCommand(CAtCommandBase* aAtCommand);
	
	/**
	 * Complete AT command
	 */
	void CompeteATCommand(CAtCommandBase* aAtCommand);
	
	/**
	 * Set long name format for modem
	 */
	void SetLongNameFormat();
	
	/**
	 * Get the long name from modem
	 */
	void GetNetworkLongName();
	
	/**
	 * Set short name format for modem
	 */
	void SetShortNameFormat();
	
	/**
	 * Get the short name from modem
	 */
	void GetNetworkShortName();
	
	/**
	 * Set numberic format
	 */
	void SetNumbericFormat();
	
	/**
	 * Get the country code and network Id
	 */
	void GetCountryCodeAndNetworkId();
	
	/**
	 * Handle the error
	 */
	void HandleGetCurNetworkError(TInt aError);
	
	/**
	 * Handle the completion of geting current network information
	 */
	void HandleGetCurNetworkComplete();
private:
    /**
     * The AT command to get the network info
     */
	CATNetworkInfo* iATNetworkInfo;
	
	/**
	 * The Helper AT command @see CATHelperCommand
	 */
	CATHelperCommand* iATHelperCommand;
	
	/**
	 * The network info
	 */
	RMobilePhone::TMobilePhoneNetworkInfoV5 iNetworkInfo;
    
	/**
	 * Define the states for retrieving the network information
	 */
	enum TGetCurNetworkState
		{
		/**
		 * The state is unknown
		 */
		EATUnknown,
		
		/**
		 * The state of seting long format
		 */
		EATSetLongFormat,
		
		/**
		 * The state of getting long name 
		 */
		EATGetLongName,
		
		/**
		 * The state of seting short name format 
		 */
		EATSetShortFormat,
		
		/**
		 * The state of getting short name
		 */
		EATGetShortName,
		
		/**
		 * The state of Seting the numberic format
		 */
		EATSetNumbericFormat,
		
		/**
		 * The state of geting country code and network Id
		 */
		EATGetCountryCodeAndNetwrokId
		} iState;

};
#endif //__GETCURRENTNETWORKINFO_H__
