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
// @file atgprsqosprofile.h
// AT command definition
// This file defines the AT command used to set GPRS Qos profile
//

#ifndef __ATGPRSQOSPROFILE_H
#define __ATGPRSQOSPROFILE_H

#include "atcommandbase.h"
#include "cltsypacketserviceshandler.h"

/**
 * CATGprsSetProfileMin inherits from the CAtCommandBase class. 
 * AT command to set GPRS Qos profile for require value
 * This class transmits the following commands: AT+CGQREQ=%d,%d,%d,%d,%d,%d
 */
class CATGprsSetProfileReq : public CAtCommandBase
{
public:
	static CATGprsSetProfileReq* NewL(CGlobalPhonemanager& aGloblePhone, 
                                      CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGprsSetProfileReq();
	/**
	 * Format the command 
	 */
	void MakeupCGQREQ();
	/**
	 * Set the context for config
	 * 
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
	CATGprsSetProfileReq( CGlobalPhonemanager& aGloblePhone, 
                          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	TInt iError;
	TInt iContextId;
	RPacketQoS::TQoSGPRSRequested iQoSConfig;
    };

#endif
