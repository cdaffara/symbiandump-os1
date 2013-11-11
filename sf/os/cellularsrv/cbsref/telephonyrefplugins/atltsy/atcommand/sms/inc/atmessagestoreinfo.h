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
// @file atmessagestoreinfo.h
// This contains CAtMessageStoreInfo which is used to get message store information
// 

#ifndef ATMESSAGESTOREINFO_H
#define ATMESSAGESTOREINFO_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

/**
 * CAtGetMessageStoreInfo
 *
 */

class CAtMessageStoreInfo : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtMessageStoreInfo();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtMessageStoreInfo* NewL(CGlobalPhonemanager& aGloblePhone, 
                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC 
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtMessageStoreInfo* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	CAtMessageStoreInfo(CGlobalPhonemanager& aGloblePhone, 
                        CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
	
	/**
	* indicate how many items message stored
	*/
	TInt iUsed;
	
	/**
	* total items for storeing message
	*/
	TInt iTotal;
	
	/**
	* The return error value of AT command executed 
	*/
    TInt iError;	
    
	}; // class CAtMessageStoreInfo

#endif // ATMESSAGESTOREINFO_H
