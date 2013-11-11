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
// Contains definitions for classes that implement Call waiting functionality.
// 
//

/**
 @file
*/

#ifndef CSIMCALLWaiting_H__
#define CSIMCALLWaiting_H__

#include <etelmm.h>
#include <et_phone.h>
#include <mmretrieve.h>
#include "CSimSmsStore.h"//ugly but needed for listReadAllAttempt


class CSimPhone;
class CTestConfigSection;
class CRetrieveMobilePhoneCWList;
class CListReadAllAttempt;

class CSimCallWaiting : public CBase
/**
 * Implements the RMobilePhone based functions that constitute the callWaiting orientated
 * functionality provided by the SIM TSY.
 */
	{
public:
	static CSimCallWaiting* NewL(CSimPhone* aPhone);
	CSimCallWaiting(CSimPhone* aPhone);
	~CSimCallWaiting();
	
	TInt ExtFuncL(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg);
	TInt CancelService(const TInt aIpc, const TTsyReqHandle aReqHandle);
	TInt SetCallWaitingStatusL(const TTsyReqHandle aReqHandle, RMobilePhone::TMobileService* aServiceGroup, RMobilePhone::TMobilePhoneServiceAction* aAction);
	TInt SetCallWaitingStatusCancel(const TTsyReqHandle aReqHandle);
	TInt NotifyCallWaitingStatusChange(const TTsyReqHandle aReqHandle,  RMobilePhone::TMobilePhoneCWInfoEntryV1* aCWStatus);
	TInt NotifyCallWaitingStatusChangeCancel(const TTsyReqHandle aReqHandle);
	
	TInt GetCallWaitingStatusPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneCWList::TGetCallWaitingRequest* aReqData,TInt* aBufSize);
	TInt ProcessGetCallWaitingStatusPhase1L(const TTsyReqHandle aTsyReqHandle,CRetrieveMobilePhoneCWList::TGetCallWaitingRequest* aReqData,TInt* aBufSize);
	TInt GetCallWaitingStatusPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetCallWaitingStatusCancel(const TTsyReqHandle aTsyReqHandle);

	void UpdateCWListL(RMobilePhone::TMobileService* aServiceGroup, RMobilePhone::TMobilePhoneServiceAction* aAction);
//	void CBActivateAllOutgingL(TDesC* codeString);
//	void CBDeactivateAllOutgingL(TDesC* codeString);
	void FindAndCreateCWListL();
	
	const CTestConfigSection* CfgFile();

private:
	void ConstructL();
	

private:
	CSimPhone* iPhone;			// < Pointer to the parent phone class.
	
	struct TCWNotification
		{
		TBool iCWChangeInfoNotificationPending;
		TTsyReqHandle iCWChangeInfoReqHandle;
		RMobilePhone::TMobilePhoneCWInfoEntryV1* iCWInfo;
		};
		
	TCWNotification iCWNotification;
	
	
	CArrayPtrFlat<CListReadAllAttempt>* iGetCWStatus;
	CMobilePhoneCWList* iCWList; //The list for call waiting.
	
	};

#endif
