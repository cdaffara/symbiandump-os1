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
// @file atgprscontextdelete.h
// AT command definition
// This file defines the AT command used for deleting a context from the phone.
// 

#ifndef __ATGPRSCONTEXTDELETE_H
#define __ATGPRSCONTEXTDELETE_H

// user include
#include "atcommandbase.h"
#include "cltsypacketserviceshandler.h"

/**
 * CATGprsContextDelete inherits from the CATCommands class. 
 * Sends the AT command 'AT+CGDCONT=x' to delete a context from the phone
 */
class CATGprsContextDelete : public CAtCommandBase
{
public:
	static CATGprsContextDelete* NewL(CGlobalPhonemanager& aGloblePhone, 
                                      CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGprsContextDelete();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/*
	 * Set the parameter of AT command 
	 * 
	 * @param aContext The context to be set configuration
	 */
	void SetContext(const TInt aContextId);
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
	CATGprsContextDelete(CGlobalPhonemanager& aGloblePhone, 
                         CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructorr
	 */
	void ConstructL();
private:
	/*
	 * The return value of AT command executed 
	 */
	TInt iError;
	/*
	 * The context to be set configuration
	 */
	TInt iContextId;
};

#endif
