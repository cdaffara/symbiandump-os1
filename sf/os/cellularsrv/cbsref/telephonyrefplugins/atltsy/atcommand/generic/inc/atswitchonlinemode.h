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
// This contains CATSwitchOnLineMode which switch to online mode
// 

#ifndef __ATSWITCHONLINEMODE_H__
#define __ATSWITCHONLINEMODE_H__

#include <callbacktimer.h>
//user include
#include "atcommandbase.h"

/**
 * \class CATSwitchOnLineMode 
 * \brief get Modem status.
 *
 * CATSwitchOnLineMode inherits from the CAtCommandBase class. 
 * This file defines the AT command for simple AT command 
 * which only return OK or Error
 */
class CATSwitchOnLineMode : public CAtCommandBase,
                            public MTimerObserver
{
public:
	static CATSwitchOnLineMode* NewL(CGlobalPhonemanager& aGloblePhone, 
		                             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATSwitchOnLineMode();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * @param aResponseBuf Line buf reading from baseband 
	 */
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	/**
	 * Will be called by AT Manager whenever a event was triggered
	 * 
	 * @param aEventSource
	 * @param aStatus
	 */
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
	
	/**
	 * Start this request
	 */
	void StartSwitchOnLineMode(CAtCommandBase* aAtCommandBase,MAtCommandExecuteObserver* aAtCommandObserver);
	/**
	 * 
	 * 
	 */
	void Enable();
	/**
	 * 
	 * 
	 */
	void Disable();
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATSwitchOnLineMode(CGlobalPhonemanager& aGloblePhone, 
		                CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	//From MTimerObserver
    virtual void TimerRun(TInt aError);	
    void StartTimer(const TInt aTimeout);
private:
	/**
	 * The return error value of AT command executed 
	 */
	TInt iError;
	CAtCommandBase* iAtCommandBase;
	MAtCommandExecuteObserver* iAtCommandObserver;
	CCallbackTimer* iCallbackTimer;	
	
	enum 
		{
		ETimerNone,
		ETimerDrop,
		ETimerRaise,
		ETimerReset,
		ETimerRead,
		ETimerWait
		} iWhichTimer;

};
#endif //__ATSWITCHONLINEMODE_H__
