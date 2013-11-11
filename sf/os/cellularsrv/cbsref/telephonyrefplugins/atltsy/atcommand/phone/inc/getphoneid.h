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
// @file getphoneid.h
// 

#ifndef __GETPHONEID_H__
#define __GETPHONEID_H__

//user include
#include "atcommandcontrolbase.h"

// forward class
class CATGetImei;
class CATGetManufacturer;
class CATGetModel;
class CATGetRevision;

/**
 * CGetPhoneID inherits from the CATCommandControlBase class. 
 *
 */
class CGetPhoneID : public CATCommandControlBase
{
public:
    /**
     * Two-phased constructor.
     * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
     */ 
	static CGetPhoneID* NewL(CGlobalPhonemanager& aGloblePhone, 
		                     CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CGetPhoneID();

	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	void StartRequest();
protected:
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CGetPhoneID(CGlobalPhonemanager& aGloblePhone, 
		        CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	void StartATCommand(CAtCommandBase* aAtCommand);
	void EndATCommand(CAtCommandBase* aAtCommand);
	void GetImeiComplete(const TInt aError);
	void GetManufacturerComplete(const TInt aError);
	void GetModelComplete(const TInt aError);
private:
	RMobilePhone::TMobilePhoneIdentityV1 iPhoneId;
	CATGetImei* iATGetImei;
	CATGetManufacturer* iATGetManufacturer;
	CATGetModel* iATGetModel;
	CATGetRevision* iATGetRevision;
	TLtsyATCommandType iATType;

};
#endif //__GETPHONEID_H__
