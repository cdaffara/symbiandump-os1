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
// @file activatecontextandgetip.h
// 
// 

#ifndef __CACTIVATECONTEXTANDGETIP_H__
#define __CACTIVATECONTEXTANDGETIP_H__

// user include
#include <callbacktimer.h>
#include "atcommandcontrolbase.h"

class CATHelperCommand;
class CATGprsContextActivate;
class CATGPRSGetContextAddr;
class CATGprsContextConnect;

/**
 * CActivatContextAndGetAddr inherits from the CATCommandControlBase class. 
 * This class provide the common functionalities for activatinf context and 
 * get context address.
 *
 */
class CActivatContextAndGetAddr : public CATCommandControlBase
{
public:
	static CActivatContextAndGetAddr* NewL(CGlobalPhonemanager& aGloblePhone, 
		                                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CActivatContextAndGetAddr();
	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	/**
	 * start the request 
	 */
	void StartRequest();
	/*
	 * Set the parameter of AT command 
	 * 
	 * @param aContext The context to be set configuration
	 */
	void SetContext(const TInt aContextId);

protected :
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CActivatContextAndGetAddr(CGlobalPhonemanager& aGloblePhone, 
		                      CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:
	/**
	 * Call back when the at command finished
	 * 
	 */
	void DispatcherCallback(TInt aError);
	/**
	 * Start an AT Command
	 * @param  aAtCommand the AT Command class 
	 */
	void StartATCommand(CAtCommandBase* aAtCommand);
	/**
	 * End an AT Command
	 * @param  aAtCommand  the AT Command class 
	 */
	void EndATCommand(CAtCommandBase* aAtCommand);
	/**
	 * Start an AT Help Command
	 * @param  aATString the AT Command string 
	 * @param  aATType the AT Command Type 
	 */
	void ExecuteATHelpCommand(const TDesC8& aATString,TLtsyATCommandType aATType);
	
private:
	CATHelperCommand* iATHelperCommand;
	CATGprsContextActivate* iATGprsContextActivate;
	CATGPRSGetContextAddr* iATGPRSGetContextAddr;
	CATGprsContextConnect* iATGprsContextConnect;
	/**
	 * Context ID
	 */
	TInt iContextId;
    /**
	* Identify the Type of AT Command
	*/
	TLtsyATCommandType iATType;
	TUint iConnectionSpeed;
	enum 
		{
		EATNone,
		EATActivateContext,
		EATGetContextIp
		} iATCommandName;

};
#endif //__CACTIVATECONTEXTANDGETIP_H__
