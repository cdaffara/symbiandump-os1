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
// @file atsetnotifycell.h
//


#ifndef __ATSETNOTIFYCELL_H__
#define __ATSETNOTIFYCELL_H__

//system include
#include "atcommandbase.h"

class CATGetPhoneCellInfo;
class CATNotifyCellInfo;
/**
 * class CATSetNotifyCellInfo 
 *
 * CATSetNotifyCellInfo inherits from the CAtCommandBase class. 
 * This class transmits the AT+CCED=1,1 Command in order to set 
 * Notify Cell infomation
 */
class CATSetNotifyCellInfo : public CAtCommandBase
{
public:
	static CATSetNotifyCellInfo* NewL(CGlobalPhonemanager& aGloblePhone, 
		                              CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATSetNotifyCellInfo();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
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
	/**
	 * The return error value of AT command executed 
	 */
	TInt GetErrorValue();
	
	/**
	 * Start the request
	 */
	virtual void StartRequest();
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATSetNotifyCellInfo(CGlobalPhonemanager& aGloblePhone, 
		                 CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:
	TInt iError;
};
#endif //__ATSETNOTIFYCELL_H__
