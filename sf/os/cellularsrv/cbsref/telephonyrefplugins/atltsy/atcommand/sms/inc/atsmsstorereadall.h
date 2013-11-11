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
// @file atsmsstorereadall.h
// This contains CAtSmsStoreReadAll which is used to read all messages
// 

#ifndef ATSMSSTOREREADALL_H
#define ATSMSSTOREREADALL_H

//system include 
#include <e32std.h>
#include <e32base.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>

//user include
#include "atcommandbase.h"

// CLASS DECLARATION
/**
 * CAtSmsStoreReadAll
 *
 */

class CAtSmsStoreReadAll : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/	
	~CAtSmsStoreReadAll();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreReadAll* NewL(CGlobalPhonemanager& aGloblePhone, 
                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreReadAll* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	CAtSmsStoreReadAll(CGlobalPhonemanager& aGloblePhone, 
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
	/*
	*  The index message should be read
	*/	
    TInt iIndex;
    
    /**
	* readmessage callback struct
	*/
	DispatcherPhonebook::TSmsData iSmsData;
	
	/**
	* The return error value of AT command executed 
	*/
	TInt iError; 
	
	}; // class CAtSmsStoreReadAll

#endif // ATSMSSTOREREADALL_H
