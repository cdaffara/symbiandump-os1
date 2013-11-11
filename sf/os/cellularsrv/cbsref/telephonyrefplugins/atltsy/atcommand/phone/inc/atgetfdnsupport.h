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
// @file atgetfdnsupport.h
// 

#ifndef __ATGETFDNSUPPORT_H__
#define __ATGETFDNSUPPORT_H__

//user include
#include "atcommandbase.h"


/**
 * class CATGetFdnSupport 
 *
 * CATGetFdnSupport inherits from the CAtCommandBase class. 
 * This class transmits the AT+CPBS=? Command in order to get 
 * FDN support
 */
class CATGetFdnSupport : public CAtCommandBase
{
public:
	static CATGetFdnSupport* NewL(CGlobalPhonemanager& aGloblePhone, 
		                          CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	/**
	 * Destructor
	 *
	 */
	~CATGetFdnSupport();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Get 
	 */
	TBool GetFdnSupport();
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
	CATGetFdnSupport(CGlobalPhonemanager& aGloblePhone, 
		             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/*
	 * iFdnSupport is ETrue if FDN was supported
	 */
	TBool iFdnSupport; 
	/*
	 * The return error value of AT command executed 
	 */
	TInt iError;
	
};
#endif //__ATGETFDNSUPPORT_H__
