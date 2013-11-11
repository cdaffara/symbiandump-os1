/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* TelephonyActSupplServices Active Object class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include <etel.h>
#include <etelmm.h>
#include <mmretrieve.h>
#include "Etel3rdParty.h"

class CTelephony;
class CTelephonyFunctions;	// forward reference
class CAsyncRequestBaseAct;

	
class CGetSupplementaryServiceStatusAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
public:		
	static CGetSupplementaryServiceStatusAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetSupplementaryServiceStatusAct();
	TInt GetCFSupplementaryServiceStatus(const CTelephony::TCallForwardingCondition aCondition, TDes8& aId, const CTelephony::TServiceGroup aServiceGroup);
	TInt GetCBSupplementaryServiceStatus(const CTelephony::TCallBarringCondition aCondition, TDes8& aId, const CTelephony::TServiceGroup aServiceGroup);
	TInt GetCWSupplementaryServiceStatus(TDes8& aId, const CTelephony::TServiceGroup aServiceGroup);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CGetSupplementaryServiceStatusAct(CTelephonyFunctions* aTelephonyFunctions);
	RMobilePhone::TMobilePhoneCBCondition GetMobileCBCondition(const CTelephony::TCallBarringCondition aCBCondition) const;
	RMobilePhone::TMobilePhoneCFCondition GetMobileCFCondition(const CTelephony::TCallForwardingCondition aCFCondition) const;
	RMobilePhone::TMobileService GetMobileServiceGroup( const CTelephony::TServiceGroup aServiceGroup);
	void ResolveCBListL();
	void ResolveCFListL();
	void ResolveCWListL();
	CTelephony::TSupplServiceStatus GetCBSupplServiceStatus(RMobilePhone::TMobilePhoneCBStatus aStatus);
	CTelephony::TSupplServiceStatus GetCFSupplServiceStatus(RMobilePhone::TMobilePhoneCFStatus aStatus);
	CTelephony::TSupplServiceStatus GetCWSupplServiceStatus(RMobilePhone::TMobilePhoneCWStatus aStatus);
private:
	CTelephonyFunctions* iTelephonyFunctions;	
	
	CRetrieveMobilePhoneCFList* 				iGetCFList;
	CRetrieveMobilePhoneCBList* 				iGetCBList;
	CRetrieveMobilePhoneCWList* 				iGetCWList;
	
	CTelephonyFunctions::TOperations			iISVSSType;
	RMobilePhone::TMobileService				iServiceGroup;
	RMobilePhone::TMobilePhoneCBCondition   	iMobileCBCondition;
	RMobilePhone::TMobilePhoneCFCondition		iMobileCFCondition;
	
	CTelephony::TCallBarringCondition	 		iCBCondition;
	CTelephony::TCallForwardingCondition	 		iCFCondition;
	
	CTelephony::TCallForwardingSupplServicesV1* 	iISVCFInfo;
	CTelephony::TCallBarringSupplServicesV1* 		iISVCBInfo;
	CTelephony::TCallWaitingSupplServicesV1* 		iISVCWInfo;
	};
	
class CGetIdentityServiceStatusAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetIdentityServiceStatusAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetIdentityServiceStatusAct();
	void GetIdentityServiceStatus(const CTelephony::TIdentityService aService, TDes8& aId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CGetIdentityServiceStatusAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	
	CTelephony::TIdentityServiceV1* 				iISVServiceStatus;
	
	RMobilePhone::TMobilePhoneIdService 		iMMService;
	RMobilePhone::TMobilePhoneIdServiceStatus 	iMMServiceStatus;
	};
