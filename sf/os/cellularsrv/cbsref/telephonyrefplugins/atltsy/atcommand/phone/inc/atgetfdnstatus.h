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
// @file atgetfdnstatus.h
// 

#ifndef __ATGETFDNSTATUS_H__
#define __ATGETFDNSTATUS_H__

//system include
#include <etelmm.h>
#include <cmmutility.h>
//user include
#include "atcommandbase.h"


/**
 * class CATGetFdnStatus 
 *
 * CATGetFdnStatus inherits from the CAtCommandBase class. 
 * This class transmits the AT+CPBS? Command in order to get 
 * FDN status
 */
class CATGetFdnStatus : public CAtCommandBase
{
public:
	static CATGetFdnStatus* NewL(CGlobalPhonemanager& aGloblePhone, 
		                       CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGetFdnStatus();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Get 
	 */
	TBool GetFdnStatus();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * @param aResponseBuf Line buf reading from baseband 
	 */
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	/**
	 * The return error value of AT command executed 
	 */
	TInt GetErrorValue();

	virtual void StartRequest();
protected:	
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATGetFdnStatus(CGlobalPhonemanager& aGloblePhone, 
		          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/*
	 * iFdnStatus is ETrue if FDN was activing
	 */
	TBool iFdnStatus; 
	/*
	 * The return error value of AT command executed 
	 */
	TInt iError;
	
	
};
#endif //__ATGETFDNSTATUS_H__
