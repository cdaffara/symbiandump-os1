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
// @file atphbkstoredelete.h
// This contains CAtPhbkStoreDelete which is used to delete a phonebook Entry
// 

#ifndef ATPHBKSTOREDELETE_H
#define ATPHBKSTOREDELETE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

/**
 * CAtPhbkStoreDelete
 *
 */
class CAtPhbkStoreDelete : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtPhbkStoreDelete();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkStoreDelete* NewL(CGlobalPhonemanager& aGloblePhone, 
                                    CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkStoreDelete* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	
	/**
	* Set Phonebook Index should be deleted
	* 
	* @param  aIndex
	*/
	void SetDeleteIndex(TInt aIndex);
protected:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	CAtPhbkStoreDelete(CGlobalPhonemanager& aGloblePhone, 
                       CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
private:
	/**
	*  Executer AT command for Deleting an Entry
	*/
	void DeleteEntry();
	
	/**
	* Get the maximum number length for each phonebook store type
	*/
	void GetMaxNumberLen();
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
		EDeleteEntry,
		EDeleteEntryComplete,
		EATNotInProgress
		}iState;
		
	/**
	* The return error value of AT command executed 
	*/		
	TInt iError;	
	
	/**
	*  The index phonebook Entry should be deleted
	*/
	TInt iIndex;
	
	/**
	* the maximum number length 
	*/
	TInt iMaxLen;
	
	}; // class AtPhbkStoreDelete
#endif // ATPHBKSTOREDELETE_H
