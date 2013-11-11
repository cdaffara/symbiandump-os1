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
// @file atphbkgetphonestoreinfo.h
// This contains CAtPhbkGetPhoneStoreInfo which is used to get phonebook information
// 

#ifndef ATPHBKGETPHONESTOREINFO_H
#define ATPHBKGETPHONESTOREINFO_H

//system include 
#include <etelmm.h>
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

/**
 * CAtPhbkGetPhoneStoreInfo
 *
 */
class CAtPhbkGetPhoneStoreInfo : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtPhbkGetPhoneStoreInfo();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkGetPhoneStoreInfo* NewL(CGlobalPhonemanager& aGloblePhone, 
                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkGetPhoneStoreInfo* NewLC(CGlobalPhonemanager& aGloblePhone, 
                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback);

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
	* 
	* @param aResponseBuf Line buf reading from baseband 
	*/
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	
	/**
	* Start this request
	* 
	*/
	void StartRequest();
	
	/**
	* Set PhoneStoreParam by handler
	* 
	* @param aStoreName
	*/
	void SetPhoneStoreParam(const TDesC& aStoreName);
private:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	CAtPhbkGetPhoneStoreInfo(CGlobalPhonemanager& aGloblePhone, 
                             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
	
	/**
	*  Executer AT command for getting Phone Store Info
	*/
	void GetPhoneStoreInfo();
private:
	/**
	*   save the iStoreName by SetPhoneStoreParam
	*/
	TBuf<8>  iStoreName;

	/**
	*  phonestoreinfo for callback
	*/
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 iPhoneStoreInfo;
	
	/**
	*  state machine for EventSignal 
	*/
	enum 
		{
		ESetIccSmsStore,
		ESetIccSmsStoreComplete,
		ESetPhoneStore,
		ESetPhoneStoreComplete,
		EGetPhoneStoreInfo,
		EGetPhoneStoreInfoComplete,
		EATNotInProgress
		}iState;
		
	/**
	* The return error value of AT command executed 
	*/
	TInt iError;			

	}; // class CAtPhbkGetPhoneStoreInfo

#endif //  ATPHBKGETPHONESTOREINFO_H
