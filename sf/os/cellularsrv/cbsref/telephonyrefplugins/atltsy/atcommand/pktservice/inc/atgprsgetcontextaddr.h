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
// @file atgprsgetcontextip.h
// AT command definition
// This contains CATGPRSGetContextAddr which is defines the AT command used 
// for getting GPRS context IP address.
// 

#ifndef _ATGPRSGETCONTEXTIP_H
#define _ATGPRSGETCONTEXTIP_H

// user include
#include "atcommandbase.h"
#include "cltsypacketserviceshandler.h"

/**
 * CATGPRSGetContextAddr inherits from the CAtCommandBase class. 
 * This class provide the common functionalities for getting 
 * GPRS context IP address..
 *
 */
class CATGPRSGetContextAddr : public CAtCommandBase
{
public:
	static CATGPRSGetContextAddr* NewL(CGlobalPhonemanager& aGloblePhone, 
                                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGPRSGetContextAddr();
	/*
	 * Set the parameter of AT command 
	 * 
	 * @param aContext The context to be set configuration
	 */
	void SetContext(const TInt aContextId);
	TInt GetErrorValue();
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
	virtual void StartRequest();
	
protected:	
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATGPRSGetContextAddr( CGlobalPhonemanager& aGloblePhone, 
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
	/*
	 * The config to be set configuration
	 */
	RPacketContext::TContextConfigGPRS iContextConfig;
    };

#endif //_ATGPRSGETCONTEXTIP_H
