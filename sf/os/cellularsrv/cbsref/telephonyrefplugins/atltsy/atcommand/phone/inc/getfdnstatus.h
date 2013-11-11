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
// @file getfdnstatus.h
// 
// 

#ifndef __GETFDNSTATUS_H__
#define __GETFDNSTATUS_H__

//user include
#include "atcommandcontrolbase.h"

// forward class
class CATGetFdnStatus;
class CATGetFdnSupport;

/**
 * CGetFdnStatus inherits from the CATCommandControlBase class. 
 *
 */
class CGetFdnStatus : public CATCommandControlBase
{
public:
    /**
     * Two-phased constructor.
     * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
     */ 
	static CGetFdnStatus* NewL(CGlobalPhonemanager& aGloblePhone, 
		                       CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CGetFdnStatus();
	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	void StartRequest();	

protected: 
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CGetFdnStatus(CGlobalPhonemanager& aGloblePhone, 
		          CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	void StartATCommand(CAtCommandBase* iAtCommand);
	void EndATCommand(CAtCommandBase* aAtCommand);
	void FdnSupportComplete(const TInt aError);
	void FdnStatusComplete(const TInt aError);
private:
	CATGetFdnSupport* iATGetFdnSupport;
	CATGetFdnStatus* iATGetFdnStatus;
	TLtsyATCommandType iATType;
	/**
	 *
	 */
	enum 
		{
		EATNone,
		EATGetFdnStatus,
		EATGetFdnSupport
		} iATCommandName;

};
#endif //__GETFDNSTATUS_H__
