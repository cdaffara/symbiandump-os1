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
// @file atgprsconfig.h
// AT command definition
// This contains CATGPRSSetConfig which is defines the AT command used 
// for setting GPRS context configuration.
// 

#ifndef _ATGPRSCONFIG_H
#define _ATGPRSCONFIG_H

// user include
#include "atcommandbase.h"
#include "cltsypacketserviceshandler.h"

/**
 * CATGPRSSetConfig inherits from the CAtCommandBase class. 
 * This class provide the common functionalities for setting 
 * GPRS context configuration.
 *
 */
class CATGPRSSetConfig : public CAtCommandBase
{
public:
	static CATGPRSSetConfig* NewL(CGlobalPhonemanager& aGloblePhone, 
                                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGPRSSetConfig();
	/**
	 * This Function creates the at set string for the AT+CGDCONT command.
	 */
	TInt MakeupCGDCONT();
	/*
	 * Set the parameter of AT command 
	 * 
	 * @param aContext The context to be set configuration
	 */
	void SetContext(const TInt aContextId);
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
	CATGPRSSetConfig( CGlobalPhonemanager& aGloblePhone, 
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
	/*
	 * The context to be set configuration
	 */
	TInt iContextId;
	
	TBool iDeleteContext;
	
};
#endif
