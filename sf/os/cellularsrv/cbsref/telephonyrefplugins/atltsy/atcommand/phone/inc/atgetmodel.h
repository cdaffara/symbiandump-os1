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
// @file atgetmodel.h
// This contains CATGetModel which is defines the AT command for Getting Model
// 

#ifndef ATGETMODEL_H__
#define ATGETMODEL_H__

//system include
#include <etelmm.h>
//user include
#include "atcommandbase.h"
/**
 * \class CATGetModel 
 * \brief get the phone Model in the GSM Network.
 *
 * CATGetModel inherits from the CAtCommandBase class. 
 * This class transmits the AT+CGMM Command in order to get Serial Numuber 
 * in the phone
 */
class CATGetModel : public CAtCommandBase
{
public:
	static CATGetModel* NewL(CGlobalPhonemanager& aGloblePhone, 
		                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATGetModel();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Get the return value executed AT Command
	 */
	RMobilePhone::TMobilePhoneIdentityV1 GetPhoneModel();
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
	CATGetModel(CGlobalPhonemanager& aGloblePhone, 
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

#endif // ATGETMODEL_H__
