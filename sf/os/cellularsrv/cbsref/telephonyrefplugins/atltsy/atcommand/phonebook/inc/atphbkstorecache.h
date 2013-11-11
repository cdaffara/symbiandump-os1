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
// @file atphbkstorecache.h
// This contains CAtPhbkStoreCache which is used to cache phonebook Entries
// 

#ifndef ATPHBKSTORECACHE_H
#define ATPHBKSTORECACHE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

/**
 * CAtPhbkStoreCache
 *
 */
class CAtPhbkStoreCache : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtPhbkStoreCache();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkStoreCache* NewL(CGlobalPhonemanager& aGloblePhone, 
                                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkStoreCache* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	* Set Phonebook Type 
	* 
	* @param  aPhonebookType
	*/
	void SetPhonebookType(DispatcherPhonebook::TPhonebook aPhonebookType);
protected:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	CAtPhbkStoreCache(CGlobalPhonemanager& aGloblePhone, 
                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
private:
	/**
	*  Executer AT command for reading phonebook entries
	*/	
	void ReadEntry();
	
	/**
	*  Generate TLV Data should be used by callback
	*/
	TInt GeneratePhbkDataL();
private:	
	/**
	*  save PhoneBookType by SetPhonebookType
	*/	
	DispatcherPhonebook::TPhonebook iPhoneBookType;
	
	/**
	*  state machine for EventSignal 
	*/	
	enum 
		{
		ESetStoreName,
		ESetStoreNameComplete,
		EReadEntry,
		EReadEntryComplete,
		EATNotInProgress
		}iState;
	
	/**
	* The return error value of AT command executed 
	*/		
	TInt iError;	

	/**
	*  This is used to generate TLV data by GeneratePhbkData 
	*/
	TPhonebookReadEntry  iReadEntry;
	
	/*
	* Client reserved space for phonebook data
	*/
	TBuf8<800>   iPhbkData; 
	
	/*
	*  indicate whether it is OK or ERROR string 
	*/
    TBool   iEndFlag;
	
	}; // class CAtPhbkStoreCache
#endif // ATPHBKSTORECACHE_H
