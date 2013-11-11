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
// @file atphbkstoreinitialise.h
// This contains CAtPhbkStoreInitialise which initialises all kinds of phonebook 
// 

#ifndef ATPHBKSTOREINITIALISE_H
#define ATPHBKSTOREINITIALISE_H

//system include  
#include <etelmm.h>
#include <e32std.h>
#include <e32base.h>

//user include 
#include "atcommandbase.h"

/**
 * CAtPhbkStoreInitialise
 *
 */
class CAtPhbkStoreInitialise : public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtPhbkStoreInitialise();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkStoreInitialise* NewL(CGlobalPhonemanager& aGloblePhone, 
                                        CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtPhbkStoreInitialise* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	CAtPhbkStoreInitialise(CGlobalPhonemanager& aGloblePhone, 
                           CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
	
	/**
	*  Executer AT command for getting Store Info
	*/
	void GetStoreInfo();
	
	/**
	*  assign totalEntries,maxTextLen and maxNumberLen for each 
	*  phonebook store type 
	*/
	void AssignPhbkType();
private:
	/**
	*  the structure for callback
	*/
	DispatcherPhonebook::TPhonebookStoreInfoV1  iStoreInfo;

	/**
	*  state machine for EventSignal 
	*/	
	enum 
		{
		ESetStoreName,
		ESetStoreNameComplete,
		EGetStoreInfo,
		EGetStoreInfoComplete,
		EATNotInProgress
		}iState;
		
	/**
	* The return error value of AT command executed 
	*/		
	TInt iError;
	
	/**
	*  the array stores phonebook type's string 
	*/
	RArray<TPtrC8>  iStoreArr;
	
    /**
	*  phonebook store type's index 
	*/
	TInt  iStoreIndex;
	
	/**
	*  total Entries for each store type
	*/
	TInt iTotalEntries;
	
	/**
	*  maximum number Length
	*/
	TUint16 iValNumberLen;
	
	/**
	*  maximum text Length
	*/	
	TUint16 iValTextLen;
	
	}; // class CAtPhbkStoreInitialise

#endif // ATPHBKSTOREINITIALISE_H
