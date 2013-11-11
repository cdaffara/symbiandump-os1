// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains definitions for classes that implement network orientated functionality.
// 
//

/**
 @file
*/

#ifndef CSIMCALLFORWARDING_H__
#define CSIMCALLFORWARDING_H__

#include <etelmm.h>
#include <et_phone.h>
#include "csimtimer.h"
#include <mmretrieve.h>
#include "CSimSmsStore.h"//ugly but needed for listReadAllAttempt

class CSimPhone;
class CTestConfigSection;
class CRetrieveMobilePhoneCFList;
class CListReadAllAttempt;

/**
 * Implements the RMobilePhone based functions that constitute the callforwarding orientated
 * functionality provided by the SIM TSY.
 */
class CSimCallForwarding : public CBase
	{
public:
	static CSimCallForwarding* NewL(CSimPhone* aPhone);
	CSimCallForwarding(CSimPhone* aPhone);
	~CSimCallForwarding();
	
	TInt ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg);
	TInt CancelService(const TInt aIpc, const TTsyReqHandle aReqHandle);
	TInt SetCallForwardingStatus(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCFCondition* aCF,  RMobilePhone::TMobilePhoneCFChangeV1* aInfo);
	TInt SetCallForwardingStatusCancel(const TTsyReqHandle aReqHandle);
	TInt NotifyCallForwardingStatusChange(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCFCondition* aCF);
	TInt NotifyCallForwardingStatusChangeCancel(const TTsyReqHandle aReqHandle);
	TInt GetIdentityServiceStatus(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneIdService* aService, RMobilePhone::TMobilePhoneIdServiceStatus* aStatus);
	TInt GetIdentityServiceStatusCancel(const TTsyReqHandle aReqHandle);
	void DoNotifyCallForwardingStatusChanged(TDesC* aString);
	void DoNotifyCallForwardingStatusChanged(TDesC* configString,TDesC* numberString);
	TInt GetCallForwardingStatusPhase1(const TTsyReqHandle aTsyReqHandle, 
												 CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, 
												 TInt* aBufSize);
	TInt ProcessGetCallForwardingStatusPhase1L(const TTsyReqHandle aTsyReqHandle, 
														 CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, 
														 TInt* aBufSize);
	TInt GetCallForwardingStatusPhase2(const TTsyReqHandle aTsyReqHandle, 
												 RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetCallForwardingStatusCancel(const TTsyReqHandle aTsyReqHandle);
	void UpdateCFListL(RMobilePhone::TMobilePhoneCFCondition* aCF, RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo );
	void UnregisterAllCFListL();
	void UpdateCFDivertAllL(TBool aActivateDivertAll);
	void UpdateCFDivertAllPhoneNumberL(TDesC* numberString);
	void RegisterL(RMobilePhone::TMobilePhoneCFCondition* aCF, RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo );
	void ActivateL(RMobilePhone::TMobilePhoneCFCondition* aCF, RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo );
	void DeactivateL(RMobilePhone::TMobilePhoneCFCondition* aCF, RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo );
	void EraseL(RMobilePhone::TMobilePhoneCFCondition* aCF, RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo );
	void UpdateCFQuiescentConditionL(TBool aActivate, RMobilePhone::TMobilePhoneCFCondition aCF);
		
	const CTestConfigSection* CfgFile();

private:
	void ConstructL();
	void FindAndCreateCFListL();
	void UnregisterAllCallDiversionsL();

private:
	CSimPhone* iPhone;			// < Pointer to the parent phone class.
	

	struct TIdentityServiceStatus
		{
		RMobilePhone::TMobilePhoneIdService iService;
		RMobilePhone::TMobilePhoneIdServiceStatus iStatus;
		};
	TInt iIdentityServiceStatusIndex;
		
	CArrayFixFlat<TIdentityServiceStatus>* iIdentityServiceStatus;
	
	struct TCFNotification
		{
		TBool iCFChangeInfoNotificationPending;
		TTsyReqHandle iCFChangeInfoReqHandle;
		RMobilePhone::TMobilePhoneCFChangeV1* iCurrentChangeInfo;
		RMobilePhone::TMobilePhoneCFCondition* iCurrentCFCondition;
		};

	TCFNotification iCFNotification;
	CArrayPtrFlat<CListReadAllAttempt>* iGetCFStatus;
	CMobilePhoneCFList* iCFList; //The list for call forwarding.
	};

#endif
