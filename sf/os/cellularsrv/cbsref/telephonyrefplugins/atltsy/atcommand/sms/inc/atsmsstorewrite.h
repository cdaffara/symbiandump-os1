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
// @file atsmsstorewrite.h
// This contains CAtSmsStoreWrite which is used to write sms message
// 

#ifndef ATSMSSTOREWRITE_H
#define ATSMSSTOREWRITE_H

// system include 
#include <e32std.h>
#include <e32base.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>
#include <callbacktimer.h>

// user include
#include "atcommandbase.h"

// CLASS DECLARATION

/**
 * CAtSmsStoreWrite
 *
 */

class CAtSmsStoreWrite : public CAtCommandBase,
                         public MTimerObserver
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtSmsStoreWrite();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CAtSmsStoreWrite* NewL(CGlobalPhonemanager& aGloblePhone, 
                                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC 
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsStoreWrite* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
	*  set GsmSmsEntry which is passed by handler
	*/
	void SeGsmSmsEntry(RMobileSmsStore::TMobileGsmSmsEntryV1 aMobileGsmEntry);
private:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	CAtSmsStoreWrite(CGlobalPhonemanager& aGloblePhone, 
                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
	
    /**
	* start a timer
	*/
    void BeginTimer();	
    
	/**
	* From MTimerObserver 
	*/
	virtual void TimerRun(TInt aError);
	
	/**
	* Issue writing AT Command to baseband
	* not using base class because the buffer length is KMaxAsciiPduLength
	*/
	void StartWritingPduLength();
	
	/**
	* Issue writing AT Command to baseband
	* not using base class because the buffer length is KMaxAsciiPduLength
	*/
	void StartWritingPdu();
	
	/**
	*  Executer AT command for writing a message
	*/
	void WriteMessage();
private:
	/**
	*  state machine for EventSignal 
	*/
	enum 
		{
		ESetPDULengthComplete,
		EWritePDUComplete,
		}iState;
	
	/**
	*  A pointer for Timer
	*/
	CCallbackTimer* iCallbackTimer;	
	
	/**
	* The buffer contains the "AT" String which should be sent to BaseBand Modem
	*/	
	TBuf8<KMaxAsciiPduLength> iTxBuffer;	
	
	/**
	*  writemessage index should be used by callback
	*/
	TInt iLocation;   

	/**
	* PDU in ascii format
	*/
	TBuf8<KMaxAsciiPduLength> iMsgDataAscii;  
	
	/**
	* get the GsmSmsEntry value passed by handler
    */
	RMobileSmsStore::TMobileGsmSmsEntryV1 iMobileGsmEntry;

	/**
	* The return error value of AT command executed 
	*/
	TInt iError;
	
	/**
	* The error value before Execute write()
	*/
	TInt iCallbackVal;
	}; // class CAtSmsStoreWrite
	
#endif // ATSMSSTOREWRITE_H
