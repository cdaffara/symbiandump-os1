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
// @file atgprscontextconnect.h
// AT command definition
// This file defines the AT command used for activating a PDP context.
// 

#ifndef __ATGPRSCONTEXTCONNECT_H
#define __ATGPRSCONTEXTCONNECT_H

// user include
#include "atcommandbase.h"
#include "cltsypacketserviceshandler.h"


/**
 * CATGprsContextConnect inherits from the CATCommands class. 
 * Sends the AT command to attach a context to the network.
 */
class CATGprsContextConnect : public CAtCommandBase
{
public:
	static CATGprsContextConnect* NewL(CGlobalPhonemanager& aGloblePhone, 
                                       CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGprsContextConnect();
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
	/*
	 * Set the parameter of AT command 
	 * 
	 * @param aContext The context to be set configuration
	 */
	void SetContext(const TInt aContextId);
	TInt GetErrorValue();
	virtual void StartRequest();
	TUint GetConnectionSpeed();
	
protected:	
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATGprsContextConnect(CGlobalPhonemanager& aGloblePhone, 
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
	TUint iConnectionSpeed;
};


#endif //__ATGPRSCONTEXTCONNECT_H
