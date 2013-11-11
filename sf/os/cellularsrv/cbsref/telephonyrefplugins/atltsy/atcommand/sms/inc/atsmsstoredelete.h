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
// @file atsmsstoredelete.h
// This contains CAtSmsStoreDelete and CAtSmsStoreDeleteAll which is used to delete message(s)
// 

#ifndef ATSMSSTOREDELETE_H
#define ATSMSSTOREDELETE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

// CLASS DECLARATION
/**
 * CAtSmsStoreDelete
 *
 */
class CAtSmsStoreDelete : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtSmsStoreDelete();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreDelete* NewL(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC 
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreDelete* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	* Set the message index to be deleted
	* 
	*/
	void SetDelIndex(TInt aIndex);
private:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	CAtSmsStoreDelete(CGlobalPhonemanager& aGloblePhone, 
                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
private:
	/**
	*  message's index 
	* 
	*/	
	TInt iIndex;
	
	/**
	* The return error value of AT command executed 
	*/
	TInt iError;
	
	}; // class CAtSmsStoreDelete


/**
 * CAtSmsStoreDeleteAll
 *
 */
class CAtSmsStoreDeleteAll : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtSmsStoreDeleteAll();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreDeleteAll* NewL(CGlobalPhonemanager& aGloblePhone, 
                                      CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC 
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreDeleteAll* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	CAtSmsStoreDeleteAll(CGlobalPhonemanager& aGloblePhone, 
                         CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
private:
	/**
	* The return error value of AT command executed 
	*/
	TInt iError;	
	
	}; // class CAtSmsStoreDeleteAll	
#endif // ATSMSDELETE_H
