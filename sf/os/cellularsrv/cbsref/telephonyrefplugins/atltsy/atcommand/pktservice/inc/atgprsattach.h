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
// @file atgprsattach.h
// This contains CATGprsAttach which is defines the AT command used for attacheing.
// 


#ifndef _ATGPRSATTACH_H
#define _ATGPRSATTACH_H

// user include
#include "atcommandbase.h"

/**
 * CATGprsAttach inherits from the CAtCommandBase class. 
 * This class provide the common functionalities for attacheing.
 *
 */
class CATGprsAttach : public CAtCommandBase
{
public:
	static CATGprsAttach* NewL(CGlobalPhonemanager& aGloblePhone, 
                               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGprsAttach();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * Cancel the AT Command execution
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
	CATGprsAttach(CGlobalPhonemanager& aGloblePhone, 
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
