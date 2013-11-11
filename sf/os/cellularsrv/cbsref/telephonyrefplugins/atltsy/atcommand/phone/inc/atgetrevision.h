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
// @file atgetrevision.h
// This contains CATGprsAttach which is defines the AT command for retrieving the
// protocol stack revision being used.


#ifndef ATGETREVISION_H__
#define ATGETREVISION_H__

//system include
#include <etelmm.h>
//user include
#include "atcommandbase.h"
/**
 * \class CATGetRevision 
 * \brief retrieving the protocol stack revision being used.
 *
 * CATGetRevision inherits from the CAtCommandBase class. 
 * This class transmits the AT+CGMR Command in order to the protocol 
 * stack revision being used 
 * 
 */
class CATGetRevision : public CAtCommandBase
{
public:
	static CATGetRevision* NewL(CGlobalPhonemanager& aGloblePhone, 
		                        CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	/**
	 * Destructor
	 *
	 */
	~CATGetRevision();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Get the return value executed AT Command
	 */
	RMobilePhone::TMobilePhoneIdentityV1 GetPhoneRevision();
	/**
	 * Get error value executed AT Command
	 */
	TInt GetErrorValue();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * @param aResponseBuf Line buf reading from baseband 
	 */
	void ParseResponseL(const TDesC8& aResponseBuf);
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
	CATGetRevision(CGlobalPhonemanager& aGloblePhone, 
		           CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:
	/**
	 * The return infomation is  multi-word?
	 */
	TBool iMoreInfoFlag;
	/**
	 * The return phone infomation  of AT command executed 
	 */
	RMobilePhone::TMobilePhoneIdentityV1 iPhoneId;
	/**
	 * The return error value of AT command executed 
	 */
	TInt iError;
};

#endif // ATGETREVISION_H__
