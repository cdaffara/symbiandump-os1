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
// @file atphbkenreadall.h
// This contains CAtPhbkEnReadAll which is used to read all PhonebookEn Entries
// 

#ifndef ATPHBKENREADALL_H
#define ATPHBKENREADALL_H

//system include 
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

//constant defination
const TInt KEnNumberLength = 6;

/**
 * CAtPhbkEnReadAll
 *
 */
class CAtPhbkEnReadAll : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtPhbkEnReadAll();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkEnReadAll* NewL(CGlobalPhonemanager& aGloblePhone, 
                                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkEnReadAll* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
protected:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	CAtPhbkEnReadAll(CGlobalPhonemanager& aGloblePhone, 
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
	TBuf<KEnNumberLength> iNumber;
	
	/*
	*  indicate whether it is OK or ERROR string 
	*/
    TBool   iEndFlag;
	
	}; // class CAtPhbkEnReadAll
#endif // ATPHBKENREADALL_H
