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
// @file atgprsdetach.h
// AT command definition
// This file defines the AT command used for detaching..
// 

#ifndef _ATGPRSDETACH_H
#define _ATGPRSDETACH_H

#include "atcommandbase.h"

/**
 * CATGprsDetach inherits from the CAtCommandBase class. 
 * Sends the AT command to detach from the network.
 * This class transmits the following commands: AT+CGATT=0
 */
class CATGprsDetach : public CAtCommandBase
{
public:
	static CATGprsDetach* NewL(CGlobalPhonemanager& aGloblePhone, 
                               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGprsDetach();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * Start Execute AT Command
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
	virtual void StartRequest();
	
protected:	
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATGprsDetach(CGlobalPhonemanager& aGloblePhone, 
                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/*
	 * The return value of AT command executed 
	 */
	TInt iError;
};

#endif
