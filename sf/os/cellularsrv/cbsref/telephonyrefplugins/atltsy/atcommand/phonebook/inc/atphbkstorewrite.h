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
// @file atphbkstorewrite.h
// This contains CAtPhbkStoreWrite which is use to write a phonebook Entry
// 

#ifndef ATPHBKSTOREWRITE_H
#define ATPHBKSTOREWRITE_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

/**
 * TPhonebookEntry
 *
 */
class TPhonebookEntry
    {
public:
	TUint16     iIndex;
    TBuf16<20>  iNumber;
    TBuf16<14>  iText;	
    };

/**
 * CAtPhbkStoreWrite
 *
 */
class CAtPhbkStoreWrite : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtPhbkStoreWrite();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkStoreWrite* NewL(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkStoreWrite* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	*   set PhoneEntry passed by handler
	*/
	void SetPhonebookEntryL(const TDesC8& aEntry);
	
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
	CAtPhbkStoreWrite(CGlobalPhonemanager& aGloblePhone, 
                      CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	/**
	* 2nd Constructor
	*/
	void ConstructL();
private:
	/**
	*  Executer AT command for writing a phonebook entry
	*/	
	void WriteEntry();
	
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
		EWriteEntry,
		EWriteEntryComplete,
		EATNotInProgress
		}iState;
	
	/**
	* The return error value of AT command executed 
	*/		
	TInt iError;	
	
	/**
	*  The structure decoded for phonebook Entry 
	*/
	TPhonebookEntry iPhbkEntry;
	
	/**
	* the maximum number length 
	*/
	TInt iMaxLen;
	
	}; // class CAtPhbkStoreWrite
#endif // ATPHBKSTOREWRITE_H
