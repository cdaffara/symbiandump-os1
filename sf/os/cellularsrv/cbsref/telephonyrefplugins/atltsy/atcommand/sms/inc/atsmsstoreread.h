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
// @file atsmsstoreread.h
// This contains CAtSmsStoreRead which is used to Read PDU Message
// via aIndex 
// 

#ifndef ATSMSSTOREREAD_H
#define ATSMSSTOREREAD_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

// CLASS DECLARATION
/**
 * CAtSmsStoreRead
 *
 */
class CAtSmsStoreRead : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtSmsStoreRead();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreRead* NewL(CGlobalPhonemanager& aGloblePhone, 
                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreRead* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	* set message index should be read
	* 
	* @param aIndex
	*/
	void SetMsgIndex(TInt aIndex);
private:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	CAtSmsStoreRead(CGlobalPhonemanager& aGloblePhone, 
                    CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
	
	/**
	*  Executer AT command for reading message via aIndex
	*/
	void ReadMessage();
private:
	/**
	* readmessage callback struct
	*/
	DispatcherPhonebook::TSmsData iSmsData;  
	
	/**
	*  The message index want to be read
	*/
	TInt iIndex;
	
	/**
	* The return error value of AT command executed 
	*/
	TInt iError; 
	
	}; // class CAtSmsStoreRead

#endif // ATSMSSTOREREAD_H
