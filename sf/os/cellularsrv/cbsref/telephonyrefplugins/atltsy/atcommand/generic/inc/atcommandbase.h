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
// @file atcommandbase.h
// This contains CAtCommandBase which is the base class for AT command
// 

#ifndef ATCOMMANDBASE_H_
#define ATCOMMANDBASE_H_

// system include
#include <e32base.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>

// user include
#include "atcommandtype.h"
#include "ltsycommondefine.h"
#include "respondbufparser.h"
#include "requestbase.h"
#include "globalphonemanager.h"


// const defination
const TUint KLtsyGenericBufferSize = 200;

// forward class
class CGlobalPhonemanager;
class CAtManager;
class CCommEngine;
class CCtsyDispatcherCallback;
class MAtCommandExecuteObserver;
class CRequestBase;
class CATSwitchOnLineMode;

/**
* This class provide the common functionalities for AT Command
*
*/
class CAtCommandBase : public CRequestBase
	{
public:
	/**
	* Destructor
	*
	*/
	virtual ~CAtCommandBase();
public:// pure virtual functions	

	/**
	* Pure virtual function. Start Execute AT Command
	*/
	virtual void ExecuteCommand()=0;
	
	/**
	* Cancel the AT Command execution
	*/
	virtual void CancelCommand();
	
public:// virtual functions	
	/**
	* Must be called after the AT Command execution get complete
	*/
	virtual void Complete();
	
	/**
	* It can be overrided by inherited class
	* 
	* @param aResponseBuf Line buf reading from baseband which is 
	*  parsered by class CAtCommandBase or CAtCommandBase's subtype
	*/	
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	
	/**
	 * Virtual Function
	 * This function would be called after one of @see TAtEventSource happened.
	 * Subclass should implement this function.
	 * 
	 * @param aEventSource One of them: EReadCompletion, EWriteCompletion and ETimeoutCompletion
	 * @param aStatus KErrNone if the event was complete without any error, or otherwise.
	 */
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
public:
	/**
	* Return the AT Type
	* @return TLtsyATCommandType The AT Type and @see TLtsyATCommandType
	*/
	TLtsyATCommandType AtType();
	
	/**
	* Add expected response string after an AT Command was executed
	*
	* @param aExpectStr
	*/
	void AddExpectStringL(const TDesC8& aExpectStr);
	
	/**
	* Add expected response string after an AT Command was executed
	*
	* @param aExpectStr
	*/
	void RemoveExpectString(const TDesC8& aExpectStr);
	
	/**
	* Reset and clean all the expect string
	*
	*/
	void RemoveAllExpectString();
	
	/**
	* Add an AT Command to ATCommandStore
	*
	*/
	void AddUnsolicitedAtCommand();
	
	/**
	* Remove an AT Command in ATCommandStore
	*
	*/
	void RemoveAtCommand();
	
	/**
	* Issue writing AT Command to baseband
	*
	*/
	void Write();
	
	/**
	 * Set write and read timout
	 */
	void SetTimeOut(TInt aWriteTimeOut, TInt aReadTimeOut);
	
	/**
	* Clean the whole buffer received from baseband
	*/
	void ClearBuffer();
	
	/**
	* Clean the current line in the buffer
	*/
	void ClearCurrentLine();
	
	/**
	* The buffer for storing the all received data from comms engine
	*/
	TPtrC8 Buffer() const;
	
	/**
	* The current line in the buffer
	*/
	TPtrC8 CurrentLine() const;
	/**
	* The last line which had not matched in the buffer
	*/
	TPtrC8 PrecedingLine() const;
	
	/**
	* Check if the passed string was matched with Expected AT String
	*
	* @param aReadLine
	* @return TBool ETrue if matched, otherwise EFalse
	*/
	virtual TBool MatchExpectString(const TDesC8& aReadLine);
	/**
	 * Set AT command observer
	 * If the observer was set, the AT command should return the data from baseband back to it but not Dispatcher Callback handler.
	 * 
	 * @param aAtCommandObserver The AT command observer @see MAtCommandExecuteObserver
	 */
	void SetAtCommandObserver(MAtCommandExecuteObserver* aAtCommandObserver);
	
	/**
	 * Remove the AT command observer
	 * 
	 */
	void RemoveAtCommandObserver();
	/**
	* Will be called by AT Manager whenever a event was triggered
	* 
	* @param aEventSource
	* @param aStatus
	*/
	void GenericEventSignal(TAtEventSource aEventSource, TInt aStatus);
protected:
	/**
	* Constructor
	*
	* @param aAtManager
	* @param aCommEngine
	* @param aAtCommandObserver
	*/
	CAtCommandBase(CGlobalPhonemanager& aGloblePhone, CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	* This function must be called in his subtype class for 
	* creating instance of class CRespondBufParser
	*/
	void ConstructL();
    /**
	* Start timer and would be expired after time periond of aTimeOut seconds
	*/
	void StartTimer(const TInt aTimeOut);
private:
  	/**
	* Stop the timer
	*/
	void StopTimer();

protected:
    /**
	* Identify the Type of AT Command
	*/
	TLtsyATCommandType iAtType;
	
	/**
	* The array of strings which contain the expected Response Message from BaseBand
	*/
	RArray<TPtrC8> iExpectingArray;
	
	/**
	* The buffer contains the "AT" String which should be sent to BaseBand Modem
	*/
	TBuf8<KLtsyGenericBufferSize> iTxBuffer;
	
	/**
	* The expired time in which the Writing of AT command should be complete
	*/
	TInt iWriteTimeOut;
	
	/**
	* The expired time in which the response message should be received
	*/
	TInt iReadTimeOut;
	
	/**
	* The pointer to class CRespondBufParserparser for parsering response 
	* message buffer
	* 
	*/	
	CRespondBufParser* iParser;
	
	/**
	* The dispacher callback handler
	*/
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;
private:	
	/**
	* The AT command observer.
	* In most of cases, when the AT command was complete, it should use dispathcer callback handler to return data back to CTSY.
	* But in some of cases, AT command should return the data back to the AT Command Observer which actually return the composed data
	* (some of them were collected from other AT commands)
	* back to CTSY.
	* 
	* Not own
	*/
	MAtCommandExecuteObserver* iAtCommandObserver;
	
	/**
	* The buffer contains the line which had not matched
	*/
	TBuf8<KLtsyGenericBufferSize> iNoMatchedLastLine;
	
	
	CATSwitchOnLineMode* iATSwitchOnLineMode;

	};

#endif /*ATCOMMANDBASE_H_*/
