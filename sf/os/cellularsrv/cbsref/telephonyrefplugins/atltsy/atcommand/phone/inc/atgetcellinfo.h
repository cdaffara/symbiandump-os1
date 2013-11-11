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
// @file atgetcellinfo.h
//  

#ifndef __ATGETCELLINFO_H__
#define __ATGETCELLINFO_H__

//system include
#include <etelmm.h>
//user include
#include "atcommandbase.h"
// forward class

/**
 * class CATGetCellInfo 
 * brief Determines the Cell infomation present in the GSM Network.
 *
 * CATGetCellInfo inherits from the CAtCommandBase class. 
 * This class transmits the AT+CCED=0,7\r Command in order to get 
 * Cell infomation
 */
class CATGetCellInfo : public CAtCommandBase
{
public:
	static CATGetCellInfo* NewL(CGlobalPhonemanager& aGloblePhone, 
		                        CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGetCellInfo();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Will be called by AT Manager whenever a event was triggered
	 * 
	 * @param aEventSource
	 * @param aStatus
	 */
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * @param aResponseBuf Line buf reading from baseband 
	 */
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	/**
	 * Start this request
	 */
	virtual void StartRequest();
protected:	
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATGetCellInfo(CGlobalPhonemanager& aGloblePhone, 
		           CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/**
	 * The return Cell infomation  of AT command executed 
	 */
	RMmCustomAPI::TMmCellInfo::TGSMCellInfo iCellInfo; 
	/**
	 * The return error value of AT command executed 
	 */
	TInt iError;	
};
#endif //__ATGETCELLINFO_H__
