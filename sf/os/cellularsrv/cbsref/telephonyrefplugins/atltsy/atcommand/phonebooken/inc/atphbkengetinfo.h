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
// @file atphbkengetinfo.h
// This contains CAtPhbkEnGetInfo which is used to get emergency call info
// 

#ifndef ATPHBKENGETINFO_H
#define ATPHBKENGETINFO_H

//system include 
#include <etelmm.h>
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

/**
 * CAtPhbkEnGetInfo
 *
 */
class CAtPhbkEnGetInfo : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtPhbkEnGetInfo();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkEnGetInfo* NewL(CGlobalPhonemanager& aGloblePhone, 
                                          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkEnGetInfo* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
private:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	CAtPhbkEnGetInfo(CGlobalPhonemanager& aGloblePhone, 
                             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
	
	/**
	*  Executer AT command for getting Phone Store Info
	*/
	void GetEnStoreInfo();
private:

	/**
	*  state machine for EventSignal 
	*/
	enum 
		{
		ESetEnStore,
		ESetEnStoreComplete,
		EGetEnStoreInfo,
		EGetEnStoreInfoComplete,
		EATNotInProgress
		}iState;
	
    /**
	* PhonebookEn used entries
	*/		
	TUint16 iUsed;
	
    /**
	* PhonebookEn total entries
	*/		
	TUint16 iTotal;
	
	/**
	* The return error value of AT command executed 
	*/
	TInt iError;			

	}; // class CAtPhbkEnGetInfo

#endif //  ATPHBKENGETINFO_H
