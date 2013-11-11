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
// @file atgprsntwkregstatus.h
// AT command definition
// This file defines the AT command used for getting out the phones current network
//registration status..
// 

#ifndef __ATGPRSNTWKREGSTATUS_H
#define __ATGPRSNTWKREGSTATUS_H

//system include
#include <etelpckt.h>

//user include
#include "atcommandbase.h"

class CATGprsNtwkRegStatusChange;
/**
 * CATGprsNtwkRegStatus inherits from the CAtCommandBase class. 
 * Sends the AT command to find out the phones current network
 * registration status.
 * This class transmits the following commands: AT+CGREG?
 */
class CATGprsNtwkRegStatus : public CAtCommandBase
{
public:
	static CATGprsNtwkRegStatus* NewL(CGlobalPhonemanager& aGloblePhone, 
                                      CCtsyDispatcherCallback& aCtsyDispatcherCallback,
                                      CATGprsNtwkRegStatusChange& aNtwkRegStatusChange);
	/**
	 * Destructor
	 *
	 */
	~CATGprsNtwkRegStatus();
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
	 * @param aNtwkRegStatusChange
	 */
	CATGprsNtwkRegStatus(CGlobalPhonemanager& aGloblePhone, 
                         CCtsyDispatcherCallback& aCtsyDispatcherCallback,
                         CATGprsNtwkRegStatusChange& aNtwkRegStatusChange);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/*
	 * The return value of AT command executed 
	 */
	TInt iError;
	RPacketService::TRegistrationStatus iGprsRegistrationStatus; 
	CATGprsNtwkRegStatusChange& iNtwkRegStatusChange;
    };

#endif
