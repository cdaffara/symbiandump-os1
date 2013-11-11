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
// @file getoperatorname.h
// 

#ifndef __GETOPERATORNAME_H__
#define __GETOPERATORNAME_H__

//user include
#include "atcommandcontrolbase.h"
#include "athelpercommand.h"

// forward class
class CATNetworkInfo;
class CATHelperCommand;

/**
 * CGetOperatorNmae inherits from the CATCommandControlBase class. 
 *
 */
class CGetOperatorNmae : public CATCommandControlBase
{
public:
    /**
     * Two-phased constructor.
     * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
     */ 
	static CGetOperatorNmae* NewL(CGlobalPhonemanager& aGloblePhone, 
		                          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CGetOperatorNmae();
	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	void StartRequest();
	void SetRequestTypeL(TInt aIpc);
	
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CGetOperatorNmae(CGlobalPhonemanager& aGloblePhone, 
		             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
	
private:
	void StartATCommand(CAtCommandBase* aAtCommand);
	void EndATCommand(CAtCommandBase* aAtCommand);
	void SetOperFormatComplete(const TInt aError);
	void CtsyDispatcherCallBack(const TInt aError);
private:
	CATNetworkInfo* iATNetworkInfo;
	CATHelperCommand* iATHelperCommand;
	RMobilePhone::TMobilePhoneNetworkInfoV5 iNetworkInfo;
	TInt iError;
	TInt iRequestIpc;
	TLtsyATCommandType iATType;
	enum 
		{
		EATNone,
		EATNetworkInfo,
		EATSimpleCommand
		} iATCommandName;

};
#endif //__GETOPERATORNAME_H__
