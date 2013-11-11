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
// @file atselectnetworkmode.h
//This file defines the AT command for Selecting Network Mode


#ifndef __CATSELECTNETWORKMODE_H__
#define __CATSELECTNETWORKMODE_H__

//user include
#include "atcommandbase.h"

/**
 * class CATSelectNetworkMode 
 *
 * CATSelectNetworkMode inherits from the CAtCommandBase class. 
 * This class transmits the AT+COPS=x Command in order to  
 * Select Network Mode
 */
class CATSelectNetworkMode : public CAtCommandBase
{
public:
	static CATSelectNetworkMode* NewL(CGlobalPhonemanager& aGloblePhone, 
		                              CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	/**
	 * Destructor
	 *
	 */
	~CATSelectNetworkMode();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	
	virtual void StartRequest();
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
	void SetNetworkSelection(const TInt aNetworkSelection);

protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATSelectNetworkMode(CGlobalPhonemanager& aGloblePhone, 
		                 CCtsyDispatcherCallback& aCtsyDispatcherCallback);	
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	TInt iError;
	TInt iNetworkSelection;
};
#endif
