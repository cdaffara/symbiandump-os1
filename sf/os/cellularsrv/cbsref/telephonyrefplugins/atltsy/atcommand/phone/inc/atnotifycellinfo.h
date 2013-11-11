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
// @file atnotifycellinfo.h
// AT command definition
// This file defines the AT command used to Notify Cell infomation

#ifndef __ATNODIFYCELLINFO_H__
#define __ATNODIFYCELLINFO_H__

//system include

//user include
#include "atcommandbase.h"

/**
 * CATNotifyCellInfo inherits from the CAtCommandBase class. 
 * AT command to Notify the phones current network
 * registration status.
 * This class transmits the following commands: +CCED:
 */
class CATNotifyCellInfo : public CAtCommandBase
	{
public:
	static CATNotifyCellInfo* NewL(CGlobalPhonemanager& aGloblePhone,
			                    CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CATNotifyCellInfo* NewLC(CGlobalPhonemanager& aGloblePhone,
			                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	virtual ~CATNotifyCellInfo();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * Start Execute AT Command
	 */
	virtual void ExecuteCommand(){};
	/**
	 * Parses the +CGREG unsolicited string from the modem.
	 * An example response would be '+CREG: 2' where the value denotes the
	 * current registration status.
	 * If parsing succeeds then aRegStatus is updated.
	 * 
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
	CATNotifyCellInfo(CGlobalPhonemanager& aGloblePhone,
				   CCtsyDispatcherCallback& aCtsyDispatcherCallback);	
	/**
	 * 2nd Constructor
	 * This function must be called in his subtype class for 
	 * creating instance of class CRespondBufParser
	 */
	void ConstructL();
	
private:
	/**
	 * The return Cell infomation  of AT command executed 
	 */
	RMmCustomAPI::TMmCellInfo::TGSMCellInfo iCellInfo; 
	/*
	 * The return value of AT command executed 
	 */
	TInt iError;
	
	};

#endif //__ATNODIFYCELLINFO_H__
