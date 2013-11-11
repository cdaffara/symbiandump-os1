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
// @file athelpercommand.h
// 


#ifndef __ATHELPERCOMMAND_H__
#define __ATHELPERCOMMAND_H__

//user include
#include "atcommandbase.h"

/**
 * \class CATHelperCommand 
 * \brief get Modem status.
 *
 * CATHelperCommand inherits from the CAtCommandBase class. 
 * This file defines the AT command for simple AT command 
 * which only return OK or Error
 */
class CATHelperCommand : public CAtCommandBase
{
public:
	static CATHelperCommand* NewL(CGlobalPhonemanager& aGloblePhone, 
		                           CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CATHelperCommand();

	/**
	 * Virtual function. Inherited from CAtCommandBase
	 *  Start Execute AT Command
	 */
	virtual void ExecuteCommand();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * Cancel the AT Command execution
	 */
	virtual void CancelCommand();
	/**
	 * Virtual function. Inherited from CAtCommandBase
	 * @param aResponseBuf Line buf reading from baseband 
	 */
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	/**
	 * Get error value executed AT Command
	 */
	TInt GetErrorValue();
	/**
	 * Set the string which should be executed AT Command
	 */
	void SetAtCommandString(const TDesC8& aATString);
	/**
	 * Start this request
	 */
	virtual void StartRequest();
	/**
	 * Start this request
	 */
	void SetATType(TLtsyATCommandType aATType);
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CATHelperCommand(CGlobalPhonemanager& aGloblePhone, 
		              CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	/**
	 * The return error value of AT command executed 
	 */
	TInt iError;
	/**
	* The buffer contains the "AT" String which should be sent to BaseBand Modem
	*/
	TBuf8<KLtsyGenericBufferSize> iATBuffer;

};
#endif //__ATHELPERCOMMAND_H__
