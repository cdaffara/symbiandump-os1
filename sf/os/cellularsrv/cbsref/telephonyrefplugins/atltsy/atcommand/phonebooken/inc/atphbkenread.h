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
// @file atphbkenread.h
// This contains CAtPhbkEnRead which is use to read a PhonebookEn Entry
// 

#ifndef ATPHBKENREAD_H
#define ATPHBKENREAD_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

//constant defination
const TInt KEnNumberLen = 6;

/**
 * CAtPhbkEnRead
 *
 */
class CAtPhbkEnRead : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtPhbkEnRead();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkEnRead* NewL(CGlobalPhonemanager& aGloblePhone, 
                                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkEnRead* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	* Set read PhonebookEn index
	* 
	* @param  aIndex
	* @param  aSlotNum
	*/
	void SetIndex(TInt aIndex);
protected:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	CAtPhbkEnRead(CGlobalPhonemanager& aGloblePhone, 
                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
private:
	/**
	*  Executer AT command for reading a phonebook entry
	*/	
	void ReadEntry();
	
private:
	/**
	*  state machine for EventSignal 
	*/
	enum 
		{
		ESetEnName,
		ESetEnNameComplete,
		EReadEntry,
		EReadEntryComplete,
		EATNotInProgress
		}iState;
		
	/**
	* The return error value of AT command executed 
	*/		
	TInt iError;
	
	/*
	* the first index should be read
	*/
	TInt iIndex;
	
	/*
	* the phonebooken number want to read
	*/
	TBuf<KEnNumberLen> iNumber;
	
	
	/*
	*  indicate whether it is OK or ERROR string 
	*/
    TBool   iEndFlag;
	
	}; // class CAtPhbkEnRead
#endif // ATPHBKENREAD_H
